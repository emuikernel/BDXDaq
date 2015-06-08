/* sysScsi.c - Template SCSI-2 initialization for sysLib.c */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,17jul02,dat  remove obsolete information
01b,08apr99,dat  spr 26491, changed PCI macro names
01a,02feb99,tm   dervied from ncr810 sysScsi.c / AutoConfig support (SPR 24733)
*/

/* 
Description

This file contains the sysScsiInit() and related routines necessary for
initializing the SCSI subsystem. 

It also contains BSP-specific SCSI I/O routines required by the ncr810 driver.
*/

#ifdef	INCLUDE_SCSI

/* includes */

#ifdef	INCLUDE_SCSI2
#include "drv/scsi/ncr810.h"
#else
#error INCLUDE_SCSI2 must be defined for use with this driver.
#endif /* INCLUDE_SCSI2 */

#include "tapeFsLib.h"

/* external declarations */

extern int   ncr810PciMemOffset;
extern int   ncr810DelayCount;
extern ULONG NCR810_INSTRUCTIONS;
extern ULONG ncr810Wait[];
extern BOOL  scsiDebug;
extern UCHAR sysInByte (ULONG);
extern void  sysOutByte (ULONG,UCHAR);

/* function declarations */

LOCAL void swapScript (UINT32 * start, UINT32 * end);

/*******************************************************************************
*
* sysScsiInit - initialize an on-board SCSI port
*
* This routine creates and initializes an NCR 53C8\f2xx\f1 SCSI I/O
* processor (SIOP) structure, enabling use of the on-board SCSI port.  It
* connects the proper interrupt service routine to the desired vector, and
* enables the interrupt at the desired level.
*
* RETURNS: OK, or ERROR if the SIOP structure cannot be created, the
* controller cannot be initialized, valid values cannot be set up in the
* SIOP registers, or the interrupt service routine cannot be connected.
*/
 
STATUS sysScsiInit ()
    {
    int                   pciBusNo;
    int                   pciDevNo;
    int                   pciFuncNo;
    UINT16                devType;
    static BOOL 	  firstTime = TRUE;
    UINT32 iobaseCsr;     /* base address 0 */
    char irq;	          /* IRQ level */	
 
    /* Local structure(s) with a prefill for ncr825SetHwRegister */

    static NCR810_HW_REGS hwRegs  = TEMPLATE_SIOP_HW_REGS;

    /* Set virtual <-> PCI address offset */

    ncr810PciMemOffset = LOCAL2PCI_MEM(0);

    /* 
     * Given the PCI bus is little endian, if the CPU architecture is big
     * endian, then the ncr8xx scripts need to be byte swapped in memory.
     * However, they should only be swapped the first time that 
     * sysScsiInit() is invoked.
     */

    if (firstTime)
	{

	if (_BYTE_ORDER == _BIG_ENDIAN)
	    {
	    swapScript ((UINT32 *) &ncr810Wait, (UINT32 *) 
			((UINT32) &ncr810Wait + 
			 (UINT32)(NCR810_INSTRUCTIONS * 8)));
	    }

	firstTime = FALSE;
	}

    /* Try to automatically configure the correct type of NCR8XX controller. */

    if (pciFindDevice ((PCI_ID_SCSI & 0xFFFF), ((PCI_ID_SCSI >> 16) & 0xFFFF),
                       0, &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR)

        {
	/* Get Device ID */

	pciConfigInWord (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_DEVICE_ID, &devType);

	SCSI_DEBUG_MSG ("Found Primary SCSI Controller - Dev/Ven ID = 0x%x\n", PCI_ID_SCSI,0,0,0,0,0);
	}
    else 
	{
	logMsg ("SCSI controller not found\n", 0, 0, 0, 0, 0, 0);
        return (ERROR);
	}

#ifndef SCSI_WIDE_ENABLE
    devType = NCR810_DEVICE_ID;
#endif

    ncr810DelayCount = NCR810_DELAY_MULT;

    if (PCI_CFG_TYPE == PCI_CFG_FORCE)
        {
        pciConfigOutLong(pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_BASE_ADDRESS_0,
                         SCSI_BASE_ADRS);
        pciConfigOutByte(pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_DEV_INT_LINE, 
                         (SCSI_INT_LVL - EXT_INTERRUPT_BASE));
        }

    /* read the configuration parameters */

    pciConfigInLong(pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_BASE_ADDRESS_0, 
                    &iobaseCsr);    
    pciConfigInByte(pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_DEV_INT_LINE, 
                    &irq);

    /* TODO - Perform any necessary PCI to CPU address translations here */

    /* TODO - Perform any necessary interrupt number calculations here */

    /* Create the SCSI controller */

    SCSI_DEBUG_MSG("sysScsiInit: ncr810CtrlCreate w/ io[0x%08x] irq[0x%02X]\n",
	    iobaseCsr, irq, 0, 0, 0, 0 );

    if ((pSysScsiCtrl = (SCSI_CTRL *) ncr810CtrlCreate 
					  (
					  (UINT8 *) iobaseCsr,
					  (UINT)    NCR810_40MHZ,
					            devType      
					  )) == NULL)
        {
        return (ERROR);
        }
 
    /* connect the SCSI controller's interrupt service routine */
 
    if (intConnect (INUM_TO_IVEC (irq),
                    ncr810Intr, (int) pSysScsiCtrl) == ERROR)
        {
        return (ERROR);
        }
 
    /* Enable SCSI interrupts */

    intEnable (irq);

    /* initialise SCSI controller with default parameters (user tuneable) */
 
    if (ncr810CtrlInit ((NCR_810_SCSI_CTRL *)pSysScsiCtrl, 
			SCSI_DEF_CTRL_BUS_ID) == ERROR)
        return (ERROR);

#if (USER_D_CACHE_MODE &  CACHE_SNOOP_ENABLE)

    scsiCacheSnoopEnable ((SCSI_CTRL *) pSysScsiCtrl);

#else 

    scsiCacheSnoopDisable ((SCSI_CTRL *) pSysScsiCtrl);

#endif

    /*
     * Set the good value in the registers of the SIOP coupled
     * with the hardware implementation
     */

    if (ncr810SetHwRegister ((NCR_810_SCSI_CTRL *)pSysScsiCtrl, &hwRegs) 
	== ERROR)
        return(ERROR);


    ncr810DelayCount = (sysGetBusSpd () * NCR810_DELAY_MULT);

    /* Include tape support if configured in config.h */
 
#ifdef  INCLUDE_TAPEFS
    tapeFsInit ();                     /* initialise tapeFs */
#endif  /* INCLUDE_TAPEFS */
 
    return (OK);

    }


/*******************************************************************************
*
* swapScript - byte swaps the 32 bit NCR8XX scripts
*
* This routine byte swaps 4 byte (32 bit) long scripts instructions due to
* the fact that the PPC processor is big endian and the PCI SCSI device is
* little endian. 
*
* The two parameters supplied <start> and <end> denote the start of scripts
* in memory and the end of the scripts in memory. Note that both these
* values are assumed to be on 4 byte boundaries. It is further assumed that
* the instructions are located in a writable area of memory.
*
* RETURNS: N/A
*/

LOCAL void swapScript 
    (
    UINT32 * start,
    UINT32 * end
    )
    {
    int size;
    UINT32 i;
    UINT32 val;

    /* determine the size (length) of the scripts */

    size = (int) end - (int) start;
    if (size % 4)
	printf ("Incorrect length (start - end) mod 4 != 0 size: %d\n", size);

    /* swap each 4 byte word in memory */

    for (i=0; i < (size/4); i ++)
	{
	val = BYTE_SWAP_32_BIT ( (UINT32) *((UINT32 *) (start + i)) );
        *((UINT32 *) start + i) = val;
	}
    }

/* Data for example code in sysScsiConfig, modify as needed */

SCSI_PHYS_DEV *	pSpd20;
SCSI_PHYS_DEV *	pSpd31;         /* SCSI_PHYS_DEV ptrs (suffix == ID, LUN) */
SCSI_PHYS_DEV *	pSpd40;

BLK_DEV *	pSbd0;
BLK_DEV *	pSbd1;
BLK_DEV *	pSbd2;          /* SCSI_BLK_DEV ptrs for Winchester */
BLK_DEV *	pSbdFloppy;     /* ptr to SCSI floppy block device */

#ifdef INCLUDE_SCSI2
SEQ_DEV *	pSd0;
TAPE_CONFIG *	pTapeConfig;
#endif /* INCLUDE_SCSI2 */

/*******************************************************************************
*
* sysScsiConfig - system SCSI configuration
*
* This routine is an example SCSI configuration routine.
*
* Most of the code for this routine shows how to declare a SCSI peripheral
* configuration.  This routine must be edited to reflect the actual
* configuration of the user's SCSI bus.  This example can also be found in
* src/config/usrScsi.c.
*
* For users just getting started, hardware configurations can be tested
* by defining SCSI_AUTO_CONFIG in config.h, which probes the bus and
* displays all devices found.  No device should have the same SCSI bus ID as
* the VxWorks SCSI port (default = 7), or the same ID as any other device.
* Check for proper bus termination.
*
* This routine includes three configuration examples that demonstrate
* configuration of a SCSI hard disk (any type), of an OMTI 3500 floppy disk,
* and of a tape drive (any type).
*
* The hard disk is divided into two 32-megabyte partitions and a third
* partition with the remainder of the disk.  The first partition is
* initialized as a dosFs device.  The second and third partitions are
* initialized as rt11Fs devices, each with 256 directory entries.
*
* It is recommended that the first partition on a block device (BLK_DEV) be
* a dosFs device, if the intention is eventually to boot VxWorks from the
* device.  This will simplify the task considerably.
*
* The floppy, since it is a removable medium device, is allowed to have only
* a single partition, and dosFs is the file system of choice because it
* facilitates media compatibility with IBM PC machines.
*
* While the hard disk configuration is fairly straightforward, the floppy
* setup in this example is more intricate.  Note that the
* scsiPhysDevCreate() call is issued twice.  The first time is merely to get
* a "handle" to pass to scsiModeSelect(); the default media type is
* sometimes inappropriate (in the case of generic SCSI-to-floppy cards).
* After the hardware is correctly configured, the handle is discarded using
* scsiPhysDevDelete(), after which a second call to scsiPhysDevCreate()
* correctly configures the peripheral.  (Before the scsiModeSelect() call,
* the configuration information was incorrect.)  Also note that following
* the scsiBlkDevCreate() call, correct values for <sectorsPerTrack> and
* <nHeads> must be set using scsiBlkDevInit().  This is necessary for IBM PC
* compatibility.
*
* Similarly, the tape configuration is more complex because certain device
* parameters must be turned off within VxWorks and the tape fixed block size
* must be defined, assuming that the tape supports fixed blocks.
*
* The last parameter to the dosFsDevInit() call is a pointer to a
* DOS_VOL_CONFIG structure.  If NULL is specified, dosFsDevInit() reads this
* information off the disk in the drive.  The read may fail if no disk is
* present or if the disk has no valid dosFs directory.  Should that happen,
* use dosFsMkfs() to create a new directory on a disk.  This routine uses
* default parameters (see dosFsLib) that may not be suitable an application,
* in which case, use dosFsDevInit() with a pointer to a valid DOS_VOL_CONFIG
* structure that has been created and initialized by the user.  If
* dosFsDevInit() is used, a call to diskInit() should be made to write a new
* directory on the disk, if the disk is blank or disposable.
*
* NOTE: The variable <pSbdFloppy> is global to allow the above calls to be
* made from the VxWorks shell, for example:
* .CS
*     -> dosFsMkfs "/fd0/", pSbdFloppy
* .CE
* If a disk is new, use diskFormat() to format it.
*
* INTERNAL
* The fourth parameter passed to scsiPhysDevCreate() is now
* <reqSenseLength> (previously <selTimeout>).
*/

STATUS sysScsiConfig (void)
    {

#if FALSE  /* EXAMPLE CODE IS NOT COMPILED */

    UINT which;
    int  scsiId;
    char modeData [4];		/* array for floppy MODE SELECT data */
    SCSI_OPTIONS options;

    /*
     * NOTE: Either of the following global variables may be set or reset
     * from the VxWorks shell. Under 5.0, they should NOT both be set at the
     * same time, or output will be interleaved and hard to read!! They are
     * intended as an aid to trouble-shooting SCSI problems.
     */

    scsiDebug = FALSE;		/* enable SCSI debugging output */
    scsiIntsDebug = FALSE;	/* enable SCSI interrupt debugging output */

    /*
     * The following section of code provides sample configurations within
     * VxWorks of SCSI peripheral devices and VxWorks file systems. It 
     * should however be noted that the actual parameters provided to
     * scsiPhysDevCreate(), scsiBlkDevCreate(), dosFsDevInit() etc., are
     * highly dependent upon the user environment and should therefore be 
     * modified accordingly.
     */

    /*
     * HARD DRIVE CONFIGURATION
     *
     * In order to configure a hard disk and initialise both dosFs and rt11Fs
     * file systems, the following initialisation code will serve as an
     * example.
     */

    /* configure a SCSI hard disk at busId = 2, LUN = 0 */

    if ((pSpd20 = scsiPhysDevCreate (pSysScsiCtrl, 2, 0, 0, NONE, 0, 0, 0))
        == (SCSI_PHYS_DEV *) NULL)
	{
        printErr ("usrScsiConfig: scsiPhysDevCreate failed.\n",
			0, 0, 0, 0, 0, 0);
	}
    else
	{
	/* create block devices */

        if (((pSbd0 = scsiBlkDevCreate (pSpd20, 0x10000, 0)) == NULL)       ||
            ((pSbd1 = scsiBlkDevCreate (pSpd20, 0x10000, 0x10000)) == NULL) ||
            ((pSbd2 = scsiBlkDevCreate (pSpd20, 0, 0x20000)) == NULL))
	    {
    	    return (ERROR);
	    }

        if ((dosFsDevInit  ("/sd0/", pSbd0, NULL) == NULL) )
	    {
	    return (ERROR);
	    }

#ifdef INCLUDE_RT11FS
	    if ((rt11FsDevInit ("/sd1/", pSbd1, 0, 256, TRUE) == NULL) ||
	    (rt11FsDevInit ("/sd2/", pSbd2, 0, 256, TRUE) == NULL))
	    {
	    return (ERROR);
	    }
#endif
	}


    /* 
     * FLOPPY DRIVE CONFIGURATION
     * 
     * In order to configure a removable media floppy drive with a
     * dosFs file system, the following device specific code will serve
     * as an example. Note that some arguments like mode parameters are
     * highly devcie and vendor specific. Thus, the appropriate peripheral
     * hardware manual should be consulted.
     */

    /* configure floppy at busId = 3, LUN = 1 */

    if ((pSpd31 = scsiPhysDevCreate (pSysScsiCtrl, 3, 1, 0, NONE, 0, 0, 0))
	== (SCSI_PHYS_DEV *) NULL)
	{
        printErr ("usrScsiConfig: scsiPhysDevCreate failed.\n");
	return (ERROR);
	}

    /* 
     * Zero modeData array, then set byte 1 to "medium code" (0x1b). NOTE:
     * MODE SELECT data is highly device-specific. If your device requires
     * configuration via MODE SELECT, please consult the device's Programmer's
     * Reference for the relevant data format.
     */

    bzero (modeData, sizeof (modeData));
    modeData [1] = 0x1b;

    /* issue the MODE SELECT command to correctly configure floppy controller */

    scsiModeSelect (pSpd31, 1, 0, modeData, sizeof (modeData));

    /*
     * delete and re-create the SCSI_PHYS_DEV so that INQUIRY will return the
     * new device parameters, i.e., correct number of blocks
     */

    scsiPhysDevDelete (pSpd31);

    if ((pSpd31 = scsiPhysDevCreate (pSysScsiCtrl, 3, 1, 0, NONE, 0, 0, 0))
	== (SCSI_PHYS_DEV *) NULL)
	{
        printErr ("usrScsiConfig: scsiPhysDevCreate failed.\n");
	return (ERROR);
	}

    if ((pSbdFloppy = scsiBlkDevCreate (pSpd31, 0, 0)) == NULL)
	{
        printErr ("usrScsiConfig: scsiBlkDevCreate failed.\n");
	return (ERROR);
	}

    /*
     * Fill in the <blksPerTrack> (blocks (or sectors) per track) and <nHeads>
     * (number of heads) BLK_DEV fields, since it is impossible to ascertain
     * this information from the SCSI adapter card. This is important for
     * PC compatibility, primarily.
     */

    scsiBlkDevInit ((SCSI_BLK_DEV *) pSbdFloppy, 15, 2);

    /* Initialize as a dosFs device */

    /*
     * NOTE: pSbdFloppy is declared globally in case the following call
     * fails, in which case dosFsMkfs or dosFsDevInit can be
     * called (from the shell) with pSbdFloppy as an argument
     * (assuming pSbdFloppy != NULL)
     */

    if (dosFsDevInit ("/fd0/", pSbdFloppy, NULL) == NULL)
	{
        printErr ("usrScsiConfig: dosFsDevInit failed.\n");
	return (ERROR);
	}


    /* 
     * TAPE DRIVE CONFIGURATION
     * 
     * In order to configure a sequential access tape device and a tapeFs
     * file system, the following code will serve as an example. Note that
     * sequential access and tapeFs support are only available via SCSI-2.
     * To make sure that SCSI-2 is being used, check for the INCLUDE_SCSI2
     * macro definition in the BSP.
     *
     * The tape device does not support synchronous data transfers
     * or wide data transfers. Therefore, turn off the automatic configuration
     * of these features within VxWorks.
     */

    scsiId = 4;
    which = SCSI_SET_OPT_XFER_PARAMS | SCSI_SET_OPT_WIDE_PARAMS;

    options.maxOffset = SCSI_SYNC_XFER_ASYNC_OFFSET;
    options.minPeriod = SCSI_SYNC_XFER_ASYNC_PERIOD;
    options.xferWidth = SCSI_WIDE_XFER_SIZE_NARROW;

    if (scsiTargetOptionsSet (pSysScsiCtrl, scsiId, &options, which) == ERROR)
        {
        printf ("Could not set target option parameters\n");
        return (ERROR);
        }

    /* create SCSI physical device and sequential device */

    if ((pSpd40 = scsiPhysDevCreate (pSysScsiCtrl, scsiId, 0,0,NONE,0,0,0)) 
            == NULL)
        {
        printErr ("scsiPhysDevCreate failed.\n");
        return (ERROR);
        }

    if ((pSd0 = scsiSeqDevCreate (pSpd40)) == NULL)
        {
        printErr ("scsiSeqDevCreate failed.\n");
        return (ERROR);
        }

    /* configure a fixed block and rewind, tape file system */

    pTapeConfig = (TAPE_CONFIG *) calloc (sizeof(TAPE_CONFIG),1);
    pTapeConfig->rewind = TRUE;		/* rewind device */
    pTapeConfig->blkSize = 512;		/* fixed 512 byte block */ 

    if (tapeFsDevInit ("/tape1", pSd0, pTapeConfig) == NULL)
	{
	printErr ("tapeFsDevInit failed.\n");
	return (ERROR);
	}

#endif /*FALSE, END OF EXAMPLE CODE */

    return (OK);
    }


/*******************************************************************************
*
* sysScsiInByte - BSP-specific byte input routine
*
* This routine reads one byte at the specified address in a BSP-specific
* manner.  It is invoked by the SCSI-2 driver, and is a wrapper to a PowerPC
* assembler routine.
*
* RETURNS: unsigned byte
*/

UINT8 sysScsiInByte
    (
    UINT32 adr		/* address of where to read byte */
    )
    {
    return (sysInByte (adr));
    }


/*******************************************************************************
*
* sysScsiOutByte - BSP-specific byte output routine
*
* This routine writes one byte at the specified address in a BSP-specific
* manner.  It is invoked by the SCSI-2 driver, and is a wrapper to a PowerPC
* assembler routine.
*
* RETURNS: N/A.
*/

void sysScsiOutByte
    (
    UINT32 adr,		/* address of where to write byte */
    UINT8  val		/* value of byte to write */
    )
    {
    sysOutByte (adr, val);
    }

#endif /* INCLUDE_SCSI */
