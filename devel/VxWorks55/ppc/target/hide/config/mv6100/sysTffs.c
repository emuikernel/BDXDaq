/* sysTffs.c - BSP stub for TrueFFS Socket Component Driver */

/* Copyright 1984-2004 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01j,01nov04,cak  Fixed diab compiler error.
01i,17may04,cak  Modified the declarations of a few variables which were
		 causing "checksum" errors. 
01h,11may04,cak  Added support for using the RTC to set the ANSI C time. 
01g,03may04,cak  Added support for mounting the TFFS drives automatically
		 during kernel initialization.
01f,15apr04,cak  Ported to MVME6100.
01e,30jan04,nrv  ported to mv5500 
01d,13nov01,nrv  made use of PRJ_BUILD
01c,09nov01,nrv  merging in T3 version with some cleanup
01b,14dec00,yp   Documentation cleanup
01a,29nov00,yp   derived from ads860 sysTffs.c 01j.
*/

/*
This stub file provides the user with the means to create a TrueFFS compliant 
socket component driver for use with any BSP. This is sample code and it is
expected, even required, to be modified after it is added to a BSP by the 
project facility when the component INCLUDE_TFFS is added to a kernel project.
Look for #error and TODO statements in the sample code.

This stub does not provides code, just some procedural macros that the
generic driver code will use.

DESCRIPTION
This library must provide board-specific hardware access routines for TrueFFS.
In effect, these routines comprise the socket component driver (or drivers)
for your flash device.  At socket registration time, TrueFFS stores
the functions provided by this socket component driver in an 'FLSocket'
structure.  When TrueFFS needs to access the flash device, it uses these
functions. 
 
Because this file is, for the most part, a device driver that exports its
functionality by registering function pointers with TrueFFS, very few of the
functions defined here are externally callable.  For the record, the only
external functions are flFitInSocketWindow() and flDelayLoop(), and you should
never have to call them.
 
However, one of the most important functions defined in this file is neither
referenced in an 'FLSocket' structure, nor is it externally callable.  This
function is sysTffsInit() and it should only be called by TrueFFS.  TrueFFS 
calls this function at initialization time to register socket component 
drivers for all the flash devices attached to your target.  It is this call 
to sysTffs() that results in assigning drive numbers to the flash devices on 
your target hardware.  Drive numbers are assigned by the order in which the 
socket component drivers are registered. The first to be registered is drive 
0, the second is drive 1, and so on up to 4.  As shipped, TrueFFS supports up 
to five flash drives.
 
After registering socket component drivers for a flash device, you may
format the flash medium even though there is not yet a block device driver
associated with the flash (see the reference entry for the tffsDevCreate()
routine).  To format the flash medium for use with TrueFFS,
call tffsDevFormat() or, for some BSPs, sysTffsFormat().
 
The sysTffsFormat() routine is an optional,BSP-specific helper routine that
can be called externally. Internally, sysTffsFormat() calls tffsDevFormat() 
with a pointer to a 'FormatParams' structure that is initialized to values 
that leave a space on the flash device for a boot image. This space is outside 
the region managed by TrueFFS.  This special region is necessary for boot
images because the normal translation and wear-leveling services of TrueFFS
are incompatible with the needs of the boot program and the boot image it
relies upon.  To write a boot image (or any other data) into this area,
use tffsBootImagePut(). 
 
The function sysTffsFormat() is only provided when a Flash SIMM has to have 
the TrueFFS file system in some desired fraction of it. It is provided only 
for the purpose of simplifying the process of formatting a Flash part 
that should be subdivided.

The Flash SIMM might also be referred to as RFA (Resident Flash Array) in the 
following text.

Example implentations of sysTffs.c can be found in the directory

    $(WIND_BASE)/target/src/drv/tffs/sockets

The files sds860-sysTffs.c and pc386-sysTffs.c have support for single and dual 
socketed PCMCIA devices as well if that might be useful to you. They both 
support multiple sockets.


Finally, this file also contains define statements for symbolic constants
that determine which MTDs, translation layer modules, and other utilities
are ultimately included in TrueFFS.  These defines are as follows:

.IP "INCLUDE_TL_FTL"
To include the NOR-based translation layer module.
.IP "INCLUDE_TL_SSFDC"
To include the SSFDC-appropriate translation layer module.
.IP "INCLUDE_MTD_I28F016"
For Intel 28f016 flash devices.
.IP "INCLUDE_MTD_I28F008"
For Intel 28f008 flash devices.
.IP "INCLUDE_MTD_I28F008_BAJA"
For Intel 28f008 flash devices on the Heurikon Baja 4700.
.IP "INCLUDE_MTD_AMD"
For AMD, Fujitsu: 29F0{40,80,16} 8-bit flash devices.
.IP "INCLUDE_MTD_CFISCS"
For CFI/SCS flash devices.
.IP "INCLUDE_MTD_WAMD"
For AMD, Fujitsu 29F0{40,80,16} 16-bit flash devices.
.IP "INCLUDE_TFFS_BOOT_IMAGE"
To include tffsBootImagePut() in TrueFFS for Tornado.
.LP
To exclude any of the modules mentioned above, edit sysTffs.c and undefine
its associated symbolic constant.


INCLUDE FILES: flsocket.h, tffsDrv.h
*/

/* includes */

#include "vxWorks.h"
#include "config.h"
#include "sysFlash.h"
#include "taskLib.h"

#include "tffs/flsocket.h"
#include "tffs/tffsDrv.h"

#undef TFFS_DEBUG

/* defines */

#undef  INCLUDE_MTD_I28F016             /* Intel: 28f016 */
#undef  INCLUDE_MTD_I28F008             /* Intel: 28f008 */
#undef  INCLUDE_MTD_AMD                 /* AMD, Fujitsu: 29f0{40,80,16} 8bit */
#undef INCLUDE_MTD_CFISCS               /* CFI/SCS */
#undef  INCLUDE_MTD_WAMD                /* AMD, Fujitsu: 29f0{40,80,16} 16bit */
#define  INCLUDE_MTD_STRATAFLASH
#define INCLUDE_TL_FTL                  /* FTL translation layer */
#undef  INCLUDE_TL_SSFDC                /* SSFDC translation layer */
#undef  INCLUDE_SOCKET_SIMM             /* SIMM socket interface */
#undef  INCLUDE_SOCKET_PCMCIA           /* PCMCIA socket interface */

/* BSP contains MTD for Strata Flash */

#ifdef INCLUDE_MTD_STRATAFLASH
#   define INCLUDE_MTD_USR
#   define MTD_USR_IDENTIFY  strataFlashIdentify
#endif

#undef INCLUDE_TFFS_BOOT_IMAGE		/* include tffsBootImagePut() */

#define VCC_DELAY_MSEC		100	/* Millisecs to wait for Vcc ramp up */
#define VPP_DELAY_MSEC		100	/* Millisecs to wait for Vpp ramp up */
#define KILL_TIME_FUNC	 ((iz * iz) / (iz + iz)) / ((iy + iz) / (iy * iz))

#define MV6100_FLASH_WRITE_ENABLE 0xdf

/* locals */

LOCAL UINT32 sysTffsMsecLoopCount = 0;
LOCAL UINT32   flashABaseAddr;
LOCAL UINT32   flashBBaseAddr;

/* forward declarations */

LOCAL FLBoolean rfaCardDetected (FLSocket vol);
LOCAL void      rfaVccOn (FLSocket vol);
LOCAL void      rfaVccOff (FLSocket vol);
#ifdef	SOCKET_12_VOLTS
LOCAL FLStatus  rfaVppOn (FLSocket vol);
LOCAL void      rfaVppOff (FLSocket vol);
#endif	/* SOCKET_12_VOLTS */
LOCAL FLStatus  rfaInitSocket (FLSocket vol);
LOCAL void      rfaSetWindow (FLSocket vol);
LOCAL void      rfaSetMappingContext (FLSocket vol, unsigned page);
LOCAL FLBoolean rfaGetAndClearCardChangeIndicator (FLSocket vol);
LOCAL FLBoolean rfaWriteProtected (FLSocket vol);
LOCAL void      rfaRegister (void);

#include "sysFlash.c"
#include "strataFlash.c"

#ifndef DOC
#include "tffs/tffsConfig.c"
#endif /* DOC */

IMPORT UCHAR sysInByte (ULONG addr);
#ifdef INCLUDE_RTC
   IMPORT void setAnsiSysTime (void);
#endif

/*******************************************************************************
*
* sysTffsInit - board level initialization for TFFS
*
* This routine calls the socket registration routines for the socket component
* drivers that will be used with this BSP. The order of registration signifies
* the logical drive number assigned to the drive associated with the socket.
*
* RETURNS: N/A
*/

LOCAL void sysTffsInit (void)
    {
    UINT32 ix = 0;
    UINT32 iy = 1;
    UINT32 iz = 2;
    int oldTick;

    /* we assume followings:
     *   - no interrupts except timer is happening.
     *   - the loop count that consumes 1 msec is in 32 bit.
     * it is done in the early stage of usrRoot() in tffsDrv().  */

    oldTick = tickGet();
    while ( oldTick == tickGet() )    /* wait for next clock interrupt */
        ;

    oldTick = tickGet();
    while ( oldTick == tickGet() )    /* loop one clock tick */
        {
        iy = KILL_TIME_FUNC;          /* consume time */
        ix++;                         /* increment the counter */
        }

    sysTffsMsecLoopCount = ix * sysClkRateGet() / 1000;

#ifdef INCLUDE_TFFS
    rfaRegister ();                   /* RFA socket interface register */
#endif

    }

/*******************************************************************************
*
* rfaRegister - install routines for the Flash RFA
*
* This routine installs necessary functions for the Resident Flash Array(RFA).
*
* RETURNS: N/A
*/

LOCAL void rfaRegister 
    (
    void
    )
    {
    UINT8    statReg;
    BOOL     bootedFromBankA;
    FLSocket * vol = 0;

    /*
     * Determine base addresses for FLASH bank A and bank B
     *
     * Since we can boot from either FLASH bank A or FLASH bank B
     * the flash banks can have different base addresses.
     */

    statReg = sysInByte(SYSTEM_STATUS_REG_1);
    statReg &= SYSTEM_STATUS_REG_1_BANK_SELECT_MASK;

    bootedFromBankA = (statReg == 0);

    if (bootedFromBankA)
	{
	flashABaseAddr = MV64360_BOOTCS_BASE_ADDR;
	flashBBaseAddr = MV64360_DEVCS0_BASE_ADDR;
	}
    else
	{
	flashABaseAddr = MV64360_DEVCS0_BASE_ADDR;
	flashBBaseAddr = MV64360_BOOTCS_BASE_ADDR;
	}

#ifdef INCLUDE_TFFS_FLASHA	
    vol = flSocketOf (noOfDrives);
    tffsSocket[noOfDrives] = "SIMM";
#ifdef FLASH_DEBUG
    printf("simRegister: SIMM\n");
#endif
    vol->window.baseAddress = (unsigned int)(flashABaseAddr >> 12);
    vol->cardDetected =      rfaCardDetected;
    vol->VccOn =             rfaVccOn;
    vol->VccOff =            rfaVccOff;
#ifdef SOCKET_12_VOLTS
    vol->VppOn =             rfaVppOn;
    vol->VppOff =            rfaVppOff;
#endif
    vol->initSocket =        rfaInitSocket;
    vol->setWindow =         rfaSetWindow;
    vol->setMappingContext = rfaSetMappingContext;
    vol->getAndClearCardChangeIndicator = rfaGetAndClearCardChangeIndicator;
    vol->writeProtected =    rfaWriteProtected;
    noOfDrives++;
#endif /* INCLUDE_TFFS_FLASHA */

#ifdef INCLUDE_TFFS_FLASHB
    vol = flSocketOf (noOfDrives);
    tffsSocket[noOfDrives] = "SIMM";
#ifdef FLASH_DEBUG
    printf("simRegister: SIMM\n");
#endif
    vol->window.baseAddress = (unsigned int)(flashBBaseAddr >> 12);
    vol->cardDetected =      rfaCardDetected;
    vol->VccOn =             rfaVccOn;
    vol->VccOff =            rfaVccOff;
#ifdef SOCKET_12_VOLTS
    vol->VppOn =             rfaVppOn;
    vol->VppOff =            rfaVppOff;
#endif
    vol->initSocket =        rfaInitSocket;
    vol->setWindow =         rfaSetWindow;
    vol->setMappingContext = rfaSetMappingContext;
    vol->getAndClearCardChangeIndicator = rfaGetAndClearCardChangeIndicator;
    vol->writeProtected =    rfaWriteProtected;
    noOfDrives++;
#endif /* INCLUDE_TFFS_FLASHB */
    }

/*******************************************************************************
*
* rfaCardDetected - detect if a card is present (inserted)
*
* This routine detects if a card is present (inserted).
* Always return TRUE in RFA environments since device is not removable.
*
* RETURNS: TRUE, or FALSE if the card is not present.
*/

LOCAL FLBoolean rfaCardDetected
    (
    FLSocket vol        /* pointer identifying drive */
    )
    {
    return(TRUE);
    }

/*******************************************************************************
*
* rfaVccOn - turn on Vcc (3.3/5 Volts)
*
* This routine turns on Vcc (3.3/5 Volts).  Vcc must be known to be good
* on exit. Assumed to be ON constantly in RFA environment.
*
* RETURNS: N/A
*/

LOCAL void rfaVccOn
    (
    FLSocket vol        /* pointer identifying drive */
    )
    {
    }

/*******************************************************************************
*
* rfaVccOff - turn off Vcc (3.3/5 Volts)
*
* This routine turns off Vcc (3.3/5 Volts) (PCMCIA). Assumed to be ON 
* constantly in RFA environment.
*
* RETURNS: N/A
*/

LOCAL void rfaVccOff
    (
    FLSocket vol        /* pointer identifying drive */
    )
    {
    }

#ifdef SOCKET_12_VOLTS

/*******************************************************************************
*
* rfaVppOn - turns on Vpp (12 Volts)
*
* This routine turns on Vpp (12 Volts). Vpp must be known to be good on exit.
* Assumed to be ON constantly in RFA environment. This is not optional and 
* must always be implemented. 
*
* RETURNS: flOK always.
*/

LOCAL FLStatus rfaVppOn
    (
    FLSocket vol        /* pointer identifying drive */
    )
    {
    return(flOK);
    }

/*******************************************************************************
*
* rfaVppOff - turns off Vpp (12 Volts)
*
* This routine turns off Vpp (12 Volts). Assumed to be ON constantly 
* in RFA environment.This is not optional and must always be implemented.
*
* RETURNS: N/A
*/

LOCAL void rfaVppOff
    (
    FLSocket vol        /* pointer identifying drive */
    )
    {
    }

#endif	/* SOCKET_12_VOLTS */

/*******************************************************************************
*
* rfaInitSocket - perform all necessary initializations of the socket
*
* This routine performs all necessary initializations of the socket.
*
* RETURNS: flOK always.
*/

/* TODO: 
 * This function is always board specific.
 * Please set this to your specific needs.
 */

LOCAL FLStatus rfaInitSocket
    (
    FLSocket vol        /* pointer identifying drive */
    )
    {
    return(flOK);
    }

/*******************************************************************************
*
* rfaSetWindow - set current window attributes, base address, size, etc
*
* This routine sets current window hardware attributes: Base address, size,
* speed and bus width.  The requested settings are given in the 'vol.window' 
* structure.  If it is not possible to set the window size requested in
* 'vol.window.size', the window size should be set to a larger value, 
* if possible. In any case, 'vol.window.size' should contain the 
* actual window size (in 4 KB units) on exit.
*
* RETURNS: N/A
*/

LOCAL void rfaSetWindow
    (
    FLSocket vol        /* pointer identifying drive */
    )
    {
    /* Physical base as a 4K page */

    if (vol.volNo == 0)
        vol.window.baseAddress = (unsigned int)(flashABaseAddr >> 12);
    else
        vol.window.baseAddress = (unsigned int)(flashBBaseAddr >> 12);

    flSetWindowSize (&vol, FLASH_SIZE >> 12);
    }

/*******************************************************************************
*
* rfaSetMappingContext - sets the window mapping register to a card address
*
* This routine sets the window mapping register to a card address.
* The window should be set to the value of 'vol.window.currentPage',
* which is the card address divided by 4 KB. An address over 128MB,
* (page over 32K) specifies an attribute-space address. On entry to this 
* routine vol.window.currentPage is the page already mapped into the window.
* (In otherwords the page that was mapped by the last call to this routine.)
* The page to map is guaranteed to be on a full window-size boundary.
* This is meaningful only in environments that use sliding window mechanism
* to view flash memory, like in PCMCIA. Not common in RFA environments. 
*
* RETURNS: N/A
*/

LOCAL void rfaSetMappingContext
    (
    FLSocket vol,       /* pointer identifying drive */
    unsigned page       /* page to be mapped */
    )
    {
    }

/*******************************************************************************
*
* rfaGetAndClearCardChangeIndicator - return the hardware card-change indicator
*
* This routine returns TRUE if the card has been changed and FALSE if not. It
* also clears the "card-changed" indicator if it has been set.
* Always return FALSE in RFA environments since device is not removable.
*
* RETURNS: FALSE, or TRUE if the card has been changed
*/

LOCAL FLBoolean rfaGetAndClearCardChangeIndicator
    (
    FLSocket vol        /* pointer identifying drive */
    )
    {
    return(FALSE);
    }

/*******************************************************************************
*
* rfaWriteProtected - return the write-protect state of the media
*
* This routine returns the write-protect state of the media
*
* RETURNS: FALSE, or TRUE if the card is write-protected
*/

LOCAL FLBoolean rfaWriteProtected
    (
    FLSocket vol        /* pointer identifying drive */
    )
    {
    return(FALSE);
    }

/*******************************************************************************
*
* flFitInSocketWindow - check whether the flash array fits in the socket window
*
* This routine checks whether the flash array fits in the socket window.
*
* RETURNS: A chip size guaranteed to fit in the socket window.
*/

long int flFitInSocketWindow 
    (
    long int chipSize,      /* size of single physical chip in bytes */
    int      interleaving,  /* flash chip interleaving (1,2,4 etc) */
    long int windowSize     /* socket window size in bytes */
    )
    {
    if ( chipSize*interleaving > windowSize ) /* doesn't fit in socket window */
        {
        int  roundedSizeBits;

        /* fit chip in the socket window */

        chipSize = windowSize / interleaving;

        /* round chip size at powers of 2 */

        for ( roundedSizeBits = 0; (0x1L << roundedSizeBits) <= chipSize;
            roundedSizeBits++ )
            ;

        chipSize = (0x1L << (roundedSizeBits - 1));
        }

    return(chipSize);
    }

/*******************************************************************************
*
* flDelayMsecs - wait for specified number of milliseconds
*
* This routine waits for the specified number of milliseconds.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void flDelayMsecs
    (
    unsigned milliseconds       /* milliseconds to wait */
    )
    {
    UINT32 ix;
    UINT32 iy = 1;
    UINT32 iz = 2;

    /* it doesn't count time consumed in interrupt level */

    for ( ix = 0; ix < milliseconds; ix++ )
        for ( ix = 0; ix < sysTffsMsecLoopCount; ix++ )
            {
            tickGet ();         /* dummy */
            iy = KILL_TIME_FUNC;    /* consume time */
            }
    }

/*******************************************************************************
*
* flDelayLoop - consume the specified time
*
* This routine delays for the specified time.
*
* RETURNS: N/A
*/

void flDelayLoop 
    (
    int  cycles
    )
    {
    while ( --cycles )
        ;
    }

/*******************************************************************************
*
* sysTffsFormat - format the flash memory above an offset
*
* This routine formats the flash memory.  If the lower half of the flash 
* memory was previously formated by TrueFFS, and you are trying to format 
* the upper half, the lower half of the flash memory must be erased before 
* you format the upper half.  To do this, tffsRawio is called: 
* .CS
* tffsRawio(0, 3, 0, 64)  
* .CE
* The first argument in the tffsRawio() command shown above is the TrueFFS 
* drive number, 0.  The second argument, 3, is the function number (also 
* known as TFFS_PHYSICAL_ERASE).  The third argument, 0, specifies the unit 
* number of the first erase unit you want to erase.  The fourth argument, 64,
* specifies how many erase units you want to erase.  
*
* RETURNS: OK, or ERROR if it fails.
*/

STATUS sysTffsFormat
    (
    int driveNo	/* TFFS drive number */ 
    )
    {
    STATUS status;
    char devName[5] = "RFAn";
    tffsDevFormatParams params =
	{
	{0x0000000, 99, 1, 0x10000l, NULL, {0,0,0,0}, NULL, 2, 0, NULL},
	FTL_FORMAT_IF_NEEDED
	};

    devName[3] = '0' + driveNo;
    params.formatParams.volumeLabel = devName;
    tffsRawio(driveNo, TFFS_PHYSICAL_ERASE, 0, 64, 0);

    status = tffsDevFormat (driveNo, (int)&params);
    return (status);
    }

/*******************************************************************************
*
* sysTffsErase - erase the specified TFFS volume.
*
*
* RETURNS: OK, ERROR
*/

STATUS sysTffsErase
    (
    int driveNo	/* TFFS drive number */ 
    )
    {
    UINT8 * ptr ;
    UINT32 temp ;

    if (driveNo == 0)
	{    
        temp = flashABaseAddr;
        while ( temp < (flashABaseAddr + FLASH_SIZE) )
            {
            ptr = (UINT8*)temp ;
#ifdef TFFS_DEBUG
            printf("Erasing sector @ 0x%x\n",temp);
#endif
            if ( sysFlashEraseStrataflash(ptr) != OK )
                {
                printf("Error erasing sector @ 0x%x\n", (unsigned)ptr);
                return ERROR ;
                }
            temp+=INTEL28F256_SECTOR_SZ;
            } 
        printf("Volume erased.\n");
        }
    else if (driveNo == 1)
	{
        temp = flashBBaseAddr;
        while ( temp < (flashBBaseAddr + FLASH_SIZE) )
            {
            ptr = (UINT8*)temp ;
#ifdef TFFS_DEBUG
            printf("Erasing sector @ 0x%x\n",temp);
#endif
            if ( sysFlashEraseStrataflash(ptr) != OK )
                {
                printf("Error erasing sector @ 0x%x\n", (unsigned)ptr);
                return ERROR ;
                }
            temp+=INTEL28F256_SECTOR_SZ;
            } 
        printf("Volume erased.\n");
	}
    else
	{
	printf("No such drive number\n");
	return ERROR;
	}      
    return OK ;
    }

/*******************************************************************************
*
* sysTffsMount - Mount tffs filesystem if it exists
*
* Mount tffs filesystem if it exists.
*
* RETURNS: 1 if FLASH FS is usable, 0 otherwise.
*/

int sysTffsMount
    (
    int driveNo /* TFFS drive number */ 
    )
    {
    int status, fd;
    char devName[6] = "/RFAn";
    char devNameOpen[8] = "/RFAn/.";

    devName[4] = '0'+driveNo;
    devNameOpen[4] = '0'+driveNo;

    if (usrTffsConfig(driveNo, 0, devName) == OK)
	status = 1;
    else
	status = 0;

    if (status) 
	{
#ifndef INCLUDE_RTC 
	struct timespec ts;

	/* Set time later than 1/99 toa void start-up warnings */

	ts.tv_sec = 32 * 365 * 24 * 60 * 60; /* ~2002 */
	ts.tv_nsec = 0;
	clock_settime(CLOCK_REALTIME,&ts);
#endif

	/* 
	 * Check filesystem consistency at start-up -- unfortunately
	 * it does print that the fs is clean and really isn't silent.
	 */

	fd = open(devNameOpen, O_RDONLY, 0);
	if (fd >= 0) 
	    {
	    ioctl(fd, FIOCHKDSK, DOS_CHK_REPAIR|DOS_CHK_VERB_SILENT);
	    close(fd);
	    }
	}
    return status;
    }

/*******************************************************************************
*
* sysTffsMountDevs - mount all available TFFS devices
*
* This routine mounts all available TFFS devices.
*
*/

void sysTffsMountDevs ()
    {
    int flshDevNum;

#ifdef INCLUDE_RTC
    setAnsiSysTime();
#endif

    for (flshDevNum = 0; flshDevNum < NUM_TFFS_VOLS; flshDevNum++)
	{	
	if (!sysTffsMount(flshDevNum))
	    {
	    printf("TFFS Volume %d is unusable. Formatting...",flshDevNum);
	    if ( sysTffsFormat(flshDevNum) == ERROR)
		{
		printf("Failed\n");
		}
	    else
		{
		printf("Done\nMounting Drive...\n");
		if (!sysTffsMount(flshDevNum))
		    printf("Mount failed again. Giving up\n");
		else
		    printf("Mount successfuly\n");
		}
	    }
	}	
    }
