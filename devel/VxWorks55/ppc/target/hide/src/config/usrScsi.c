/* usrScsi.c - SCSI initialization */

/* Copyright 1992-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
02c,17nov96,jdi  doc: made sysScsiConfig() NOMANUAL, since it's in generic BSP.
02b,16sep96,dat  new macro SYS_SCSI_CONFIG, for rtn sysScsiConfig
02a,25jul96,jds  major modification of this file. Most of the previous code
                 is commented out.
01f,09may96,jds  fixed modeData warning with SCSI_AUTO_CONFIG definition
01e,31aug94,ism  added a #ifdef to keep the RT11 file system from being
		 included just because SCSI is included. (SPR #3572)
01d,11jan93,ccc  removed 2nd parameter from call to scsiAutoConfig().
01c,24dec92,jdi  removed note about change of 4th param in scsiPhysDevCreate();
		 mentioned where usrScsiConfig() is found.
01b,24sep92,ccc  note about change of 4th parameter in scsiPhysDevCreate().
01a,18sep92,jcf  written.
*/

/*
DESCRIPTION
This file is used to configure and initialize the VxWorks SCSI support.
This file is included by usrConfig.c.

There are many examples that are provided in this file which demonstrate
use of routines to configure various SCSI devices. Such a configuration is
user specific. Therefore, the examples provided in this file should be 
used as a model and customized to the user requirements.

MAKE NO CHANGES TO THIS FILE.

All BSP specific code should be added to
the sysLib.c file or a sysScsi.c file in the BSP directory.  Define the
macro SYS_SCSI_CONFIG to generate the call to sysScsiConfig().
See usrScsiConfig for more info on the SYS_SCSI_CONFIG macro.

SEE ALSO: usrExtra.c

NOMANUAL
*/

#ifndef  __INCusrScsic
#define  __INCusrScsic

/*******************************************************************************
*
* usrScsiConfig - configure SCSI peripherals
*
* This code configures the SCSI disks and other peripherals on a SCSI
* controller chain.
*
* The macro SCSI_AUTO_CONFIG will include code to scan all possible device/lun
* id's and to configure a scsiPhysDev structure for each device found.  Of
* course this doesn't include final configuration for disk partitions,
* floppy configuration parameters, or tape system setup.  All of these actions
* must be performed by user code, either through sysScsiConfig(),
* the startup script, or by the application program.
*
* The user may customize this code on a per BSP basis using the SYS_SCSI_CONFIG
* macro.  If defined, then this routine will call the routine sysScsiConfig().
* That routine is to be provided by the BSP, either in sysLib.c or sysScsi.c.
* If SYS_SCSI_CONFIG is not defined, then sysScsiConfig() will not be called
* as part of this routine.
*
* An example sysScsiConfig() routine can be found in target/src/config/usrScsi.c.
* The example code contains sample configurations for a hard disk, a floppy
* disk and a tape unit.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO:
* .pG "I/O System, Local File Systems"
*/

STATUS usrScsiConfig (void)

    {
#ifdef SCSI_AUTO_CONFIG

    /* Optional auto-config of physical devices on SCSI bus */

    taskDelay (sysClkRateGet () * 1);	/* allow devices to reset */

    printf ("Auto-configuring SCSI bus...\n\n");

    scsiAutoConfig (pSysScsiCtrl);

    scsiShow (pSysScsiCtrl);

    printf ("\n");
#endif	/* !SCSI_AUTO_CONFIG */

#ifdef SYS_SCSI_CONFIG

    /* Execute BSP configuration code, if any */

    sysScsiConfig ();

#endif

    return (OK);
    }

#if FALSE	/* EXAMPLE CODE ONLY */

/* Data for example code */

SCSI_PHYS_DEV *	pSpd20;
SCSI_PHYS_DEV *	pSpd31;         /* SCSI_PHYS_DEV ptrs (suffix == ID, LUN) */
SCSI_PHYS_DEV *	pSpd40;

BLK_DEV *	pSbd0;
BLK_DEV *	pSbd1;
BLK_DEV *	pSbd2;          /* SCSI_BLK_DEV ptrs for Winchester */
BLK_DEV *	pSbdFloppy;     /* ptr to SCSI floppy block device */
SEQ_DEV *	pSd0;
TAPE_CONFIG *	pTapeConfig;

/*******************************************************************************
*
* sysScsiConfig - Example SCSI device setup code
*
* This is an example of a SCSI routine, that could be added to the BSP sysLib.c
* file to perform SCSI disk/tape setup.  Define SYS_SCSI_CONFIG to have
* the kernel call this routine as part of usrScsiConfig().
*
* Most of the code found here is an example of how to declare a SCSI
* peripheral configuration.  You must edit this routine to reflect the
* actual configuration of your SCSI bus.  This example is found in
* src/config/usrScsi.c.
*
* If you are just getting started, you can test your hardware configuration
* by defining SCSI_AUTO_CONFIG in config.h, which will probe the bus and
* display all devices found.  No device should have the same SCSI bus ID as
* your VxWorks SCSI port (default = 7), or the same as any other device.
* Check for proper bus termination.
*
* There are three configuration examples here that 
* demostrate configuration of a SCSI hard disk (any type), an OMTI 3500 floppy 
* disk, and a tape drive (any type).
*
* The hard disk is divided into two 32-Mbyte partitions and a third
* partition with the remainder of the disk.  The first partition is initialized
* as a dosFs device.  The second and third partitions are initialized as
* rt11Fs devices, each with 256 directory entries.
*
* It is recommended that the first partition (BLK_DEV) on a block device be
* a dosFs device, if the intention is eventually to boot VxWorks from the
* device.  This will simplify the task considerably.
*
* The floppy, since it is a removable medium device, is allowed to have only a
* single partition, and dosFs is the file system of choice for this device,
* since it facilitates media compatibility with IBM PC machines.
*
* While the hard disk configuration is fairly straightforward, the floppy
* setup in this example is a bit intricate.  Note that the
* scsiPhysDevCreate() call is issued twice.  The first time is merely to get
* a "handle" to pass to the scsiModeSelect() function, since the default
* media type is sometimes inappropriate (in the case of generic
* SCSI-to-floppy cards).  After the hardware is correctly configured, the
* handle is discarded via the scsiPhysDevDelete() call, after which a
* second scsiPhysDevCreate() call correctly configures the peripheral.
* (Before the scsiModeSelect() call, the configuration information was
* incorrect.) Also note that following the scsiBlkDevCreate() call, the
* correct values for <sectorsPerTrack> and <nHeads> must be set via the
* scsiBlkDevInit() call.  This is necessary for IBM PC compatibility.
* Similarly, the tape configuration is also a little involved because 
* certain device parameters need to turned off within VxWorks and the
* tape fixed block size needs to be defined, assuming that the tape
* supports fixed blocks.
*
* The last parameter to the dosFsDevInit() call is a pointer to a
* DOS_VOL_CONFIG structure.  By specifying NULL, you are asking
* dosFsDevInit() to read this information off the disk in the drive.  This
* may fail if no disk is present or if the disk has no valid dosFs
* directory.  Should this be the case, you can use the dosFsMkfs() command to
* create a new directory on a disk.  This routine uses default parameters
* (see dosFsLib) that may not be suitable for your application, in which
* case you should use dosFsDevInit() with a pointer to a valid DOS_VOL_CONFIG
* structure that you have created and initialized.  If dosFsDevInit() is
* used, a diskInit() call should be made to write a new directory on the
* disk, if the disk is blank or disposable.
*
* NOTE
* The variable <pSbdFloppy> is global to allow the above calls to be
* made from the VxWorks shell, i.e.:
* .CS
*     -> dosFsMkfs "/fd0/", pSbdFloppy
* .CE
* If a disk is new, use diskFormat() to format it.
*
* INTERNAL
* The fourth parameter passed to scsiPhysDevCreate() is now
* <reqSenseLength> (previously <selTimeout>).
*
* NOMANUAL
*/

sysScsiConfig (void)
    {
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

    scsiDebug = TRUE;		/* enable SCSI debugging output */
    scsiIntsDebug = TRUE;	/* enable SCSI interrupt debugging output */

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

    return (OK);
    }
#endif /*FALSE*/

#endif /* __INCusrScsic */
