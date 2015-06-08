/* sysTffs.c - System-dependent TrueFFS library */

/* Copyright 2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,12mar02,kab  Update for T2.2 
01b,31jul00,cak  Removed inclusion of mv2400.h
01a,15apr00,add   Created from ads860/sysTffs.c
*/

/*
DESCRIPTION
This library provides board-specific hardware access routines for TrueFFS.
In effect, these routines comprise the socket component driver (or drivers)
for your flash device hardware.  At socket registration time, TrueFFS stores
pointers to the functions of this socket component driver in an 'FLSocket'
structure.  When TrueFFS needs to access the flash device, it uses these
functions.

Because this file is, for the most part, a device driver that exports its
functionality by registering function pointers with TrueFFS, very few of the
functions defined here are externally callable.  For the record, these
external functions are flFitInSocketWindow() and flDelayLoop().  You should
never have any need to call these functions.

However, one of the most import functions defined in this file is neither
referenced in an 'FLSocket' structure, nor is it externally callable.  This
function is sysTffsInit().  TrueFFS calls this function at initialization
time to register socket component drivers for all the flash devices attached
to your target.  It is this call to sysTffs() that results in assigning
drive numbers to the flash devices on your target hardware.  Drive numbers
are assigned by the order in which the socket component drivers are registered.
The first to be registered is drive 0, the second is drive 1, and so on up to
4.  As shipped, TrueFFS supports up to five flash drives.

After registering socket component drivers for a flash device, you may
format the flash medium even though there is not yet a block device driver
associated with the flash (see the reference entry for the tffsDevCreate()
routine).  To format the flash medium for use with TrueFFS,
call tffsDevFormat() or, for some BSPs, sysTffsFormat().

The sysTffsFormat() routine is an optional but BSP-specific externally
callable helper function.  Internally, it calls tffsDevFormat() with a
pointer to a 'FormatParams' structure initialized to values that leave a
space on the flash device for a boot image. This space is outside the
region managed by TrueFFS.  This special region is necessary for boot
images because the normal translation and wear-leveling services of TrueFFS
are incompatible with the needs of the boot program and the boot image it
relies upon.  To write a boot image (or any other data) into this area,
use tffsBootImagePut().

Finally, this file also contains define statements for symbolic constants
that determine which MTDs, translation layer modules, and other utilities
are ultimately included in TrueFFS.  These defines are as follows:

.IP "INCLUDE_TL_NFTL"
To include the NAND-based translation layer module.
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
.IP "INCLUDE_MTD_CDSN"
For Toshiba, Samsung: NAND CDSN flash devices.
.IP "INCLUDE_MTD_DOC2"
For Toshiba, Samsung: NAND DOC flash devices.
.IP "INCLUDE_MTD_CFISCS"
For CFI/SCS flash devices.
.IP "INCLUDE_MTD_WAMD"
For AMD, Fujitsu 29F0{40,80,16} 16-bit flash devices.
.IP "INCLUDE_TFFS_BOOT_IMAGE"
To include tffsBootImagePut() in TrueFFS for Tornado.
.LP
To exclude any of the modules mentioned above, edit sysTffs.c and undefine
its associated symbolic constant.

The function sysTffsFormat() is provided so one might seasily format the
Flash SIMM to have the TFFS file system in one half of it.

INCLUDE FILES: flsocket.h, tffsDrv.h
*/


/* includes */

#include "vxWorks.h"
#include "taskLib.h"
#include "config.h"
#include "tffs/flsocket.h"
#include "tffs/tffsDrv.h"
#include "arch/ppc/vxPpcLib.h"


/* defines */
#define INCLUDE_MTD_USR                 /* use local identify routine */ 
#define MTD_USR_IDENTIFY  amd29lvMTDIdentify
#define KILL_TIME_FUNC	 ((iz * iz) / (iz + iz)) / ((iy + iz) / (iy * iz))

#define FLASH_SIZE 0x200000

#define	INCLUDE_TL_FTL			/* FTL translation layer */
#undef	INCLUDE_TL_SSFDC		/* SSFDC translation layer */

/* globals */
UINT32 tffsFlashBaseAdrs = FLASH_BASE_ADRS;

/* locals */

LOCAL UINT32 sysTffsMsecLoopCount = 0;


/* forward declarations */

LOCAL FLBoolean		simmCardDetected (FLSocket vol);
LOCAL void		simmVccOn (FLSocket vol);
LOCAL void		simmVccOff (FLSocket vol);
LOCAL FLStatus		simmInitSocket (FLSocket vol);
LOCAL void		simmSetWindow (FLSocket vol);
LOCAL void		simmSetMappingContext (FLSocket vol, unsigned page);
LOCAL FLBoolean		simmGetAndClearCardChangeIndicator (FLSocket vol);
LOCAL FLBoolean		simmWriteProtected (FLSocket vol);
LOCAL void		simmRegister (void);

#ifdef	SOCKET_12_VOLTS
LOCAL FLStatus		simmVppOn (FLSocket vol);
LOCAL void		simmVppOff (FLSocket vol);
#endif	/* SOCKET_12_VOLTS */

#ifndef DOC
#include "tffs/tffsConfig.c"
#endif  /* DOC */

/*******************************************************************************
*
* sysTffsInit - board level initialization for TFFS
*
* This routine calls the socket registration routines for the socket component
* drivers that will be used with this BSP. The order of registration signifies
* the logical drive number given to the drive associated with the socket.
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
    while (oldTick == tickGet())	/* wait for next clock interrupt */
	;

    oldTick = tickGet();
    while (oldTick == tickGet())	/* loop one clock tick */
	{
	iy = KILL_TIME_FUNC;		/* consume time */
	ix++;				/* increment the counter */
	}

    sysTffsMsecLoopCount = ix * sysClkRateGet() / 1000;

    simmRegister ();			/* SIMM socket interface register */

    }

/*******************************************************************************
*
* simmRegister - install routines for the Flash SIMM
*
* This routine installs necessary functions for the Flash SIMM.
*
* RETURNS: N/A
*/

LOCAL void simmRegister (void)
    {
    FLSocket vol = flSocketOf (noOfDrives);

    tffsSocket[noOfDrives] =	"SIMM";
    vol.window.baseAddress =	tffsFlashBaseAdrs >> 12;
    vol.cardDetected =		simmCardDetected;
    vol.VccOn =			simmVccOn;
    vol.VccOff =		simmVccOff;
#ifdef SOCKET_12_VOLTS
    vol.VppOn =			simmVppOn;
    vol.VppOff =		simmVppOff;
#endif
    vol.initSocket =		simmInitSocket;
    vol.setWindow =		simmSetWindow;
    vol.setMappingContext =	simmSetMappingContext;
    vol.getAndClearCardChangeIndicator = simmGetAndClearCardChangeIndicator;
    vol.writeProtected =	simmWriteProtected;
    noOfDrives++;
    }

/*******************************************************************************
*
* simmCardDetected - detect if a card is present (inserted)
*
* This routine detects if a card is present (inserted).
*
* RETURNS: TRUE, or FALSE if the card is not present.
*/

LOCAL FLBoolean simmCardDetected
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return (TRUE);
    }

/*******************************************************************************
*
* simmVccOn - turn on Vcc (3.3/5 Volts)
*
* This routine turns on Vcc (3.3/5 Volts).  Vcc must be known to be good
* on exit.
*
* RETURNS: N/A
*/

LOCAL void simmVccOn
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    }

/*******************************************************************************
*
* simmVccOff - turn off Vcc (3.3/5 Volts)
*
* This routine turns off Vcc (3.3/5 Volts).
*
* RETURNS: N/A
*/

LOCAL void simmVccOff
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    }


/*******************************************************************************
*
* simmInitSocket - perform all necessary initializations of the socket
*
* This routine performs all necessary initializations of the socket.
*
* RETURNS: flOK always.
*/

LOCAL FLStatus simmInitSocket
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return (flOK);
    }

/*******************************************************************************
*
* simmSetWindow - set current window attributes, Base address, size, etc
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

LOCAL void simmSetWindow
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    /* Physical base as a 4K page */
    vol.window.baseAddress = tffsFlashBaseAdrs >> 12;

    flSetWindowSize (&vol, FLASH_SIZE >> 12);
    }

/*******************************************************************************
*
* simmSetMappingContext - sets the window mapping register to a card address
*
* This routine sets the window mapping register to a card address.
* The window should be set to the value of 'vol.window.currentPage',
* which is the card address divided by 4 KB. An address over 128MB,
* (page over 32K) specifies an attribute-space address.
*
* The page to map is guaranteed to be on a full window-size boundary.
*
* RETURNS: N/A
*/

LOCAL void simmSetMappingContext
    (
    FLSocket vol,		/* pointer identifying drive */
    unsigned page		/* page to be mapped */
    )
    {
    }

/*******************************************************************************
*
* simmGetAndClearCardChangeIndicator - return the hardware card-change indicator
*
* This routine returns the hardware card-change indicator and clears it if set.
*
* RETURNS: FALSE, or TRUE if the card has been changed
*/

LOCAL FLBoolean simmGetAndClearCardChangeIndicator
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return (FALSE);
    }

#ifdef SOCKET_12_VOLTS

/*******************************************************************************
*
* simmVppOn - turns on Vpp (12 Volts)
*
* This routine turns on Vpp (12 Volts). Vpp must be known to be good on exit.
*
* RETURNS: flOK always.
*/

LOCAL FLStatus simmVppOn
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return (flOK);
    }

/*******************************************************************************
*
* simmVppOff - turns off Vpp (12 Volts)
*
* This routine turns off Vpp (12 Volts).
*
* RETURNS: N/A
*/

LOCAL void simmVppOff
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    }

#endif	/* SOCKET_12_VOLTS */

/*******************************************************************************
*
* simmWriteProtected - return the write-protect state of the media
*
* This routine returns the write-protect state of the media
*
* RETURNS: FALSE, or TRUE if the card is write-protected
*/

LOCAL FLBoolean simmWriteProtected
    (
    FLSocket vol		/* pointer identifying drive */
    )
    {
    return (FALSE);
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
    long int chipSize,		/* size of single physical chip in bytes */
    int      interleaving,	/* flash chip interleaving (1,2,4 etc) */
    long int windowSize		/* socket window size in bytes */
    )
    {
    if (chipSize*interleaving > windowSize) /* doesn't fit in socket window */
        {
        int  roundedSizeBits;

        /* fit chip in the socket window */
        chipSize = windowSize / interleaving;

        /* round chip size at powers of 2 */
        for (roundedSizeBits = 0; (0x1L << roundedSizeBits) <= chipSize;
             roundedSizeBits++)
	    ;

        chipSize = (0x1L << (roundedSizeBits - 1));
        }

    return (chipSize);
    }

/*******************************************************************************
*
* flDelayMsecs - wait for specified number of milliseconds
*
* This routine waits for specified number of milliseconds.
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

    for (ix = 0; ix < milliseconds; ix++)
        for (ix = 0; ix < sysTffsMsecLoopCount; ix++)
	    {
	    tickGet ();			/* dummy */
	    iy = KILL_TIME_FUNC;	/* consume time */
	    }
    }

/*******************************************************************************
*
* flDelayLoop - consume the specified time
*
* This routine consumes the specified time.
*
* RETURNS: N/A
*/

void flDelayLoop
    (
    int  cycles
    )
    {
    while (--cycles)
	;
    }

/*******************************************************************************
*
* sysTffsFormat - format the flash memory above an offset
*
* This routine formats the flash memory.  Because this function sets
* start = 0x100000 (1M), the lower half of the specified flash memory 
* is left unformatted.  If the lower half of the flash memory was
* previously formated by TrueFFS, and you are trying to format the upper half,
* you need to erase the lower half of the flash memory before you format the
* upper half.  To do this, you could use:
* .CS
* tffsRawio(0, 3, 0, 8)
* .CE
* The first argument in the tffsRawio() command shown above is the TrueFFS
* drive number, 0.  The second argument, 3, is the function number (also
* known as TFFS_PHYSICAL_ERASE).  The third argument, 0, specifies the unit
* number of the first erase unit you want to erase.  The fourth argument, 8,
* specifies how many erase units you want to erase.
*
* RETURNS: OK, or ERROR if it fails.
*/

STATUS sysTffsFormat (void)
    {
    STATUS status;

    const UINT32 start = 0x100000;

    /* Reserve 1 MB for bootrom */

    tffsDevFormatParams params =
        {{start, 99, 1, 0x10000l, NULL, {0,0,0,0}, NULL, 2, 0, NULL},
         FTL_FORMAT_IF_NEEDED};
    /* we assume that the drive number 0 is SIMM */

    status = tffsDevFormat (0, (int)&params);
    return (status);
    }

