/* fd208Drv.c - floppy disk driver for Intel iSBC 208 or Zendex 208a */

/* Copyright 1984,1985,1986,1987,1988,1989 Wind River Systems, Inc. */
extern char copyright_wind_river[]; static char *copyright=copyright_wind_river;

/* 
modification history
--------------------
06b,06jun88,dnw  changed rtLib to rt11Lib.
06a,30may88,dnw  changed to v4 names.
05x,04may88,jcf  changed semaphores to be consistent with new semLib.
05w,11feb88,gae  fixed 05t.  Added multiple drive support, untested!
05t,04jan88,gae  made fd208DevCreate() fail if no driver.
05s,19nov87,ecs  documentation.
05r,18nov87,ecs  documentation.
05q,06nov87,ecs  documentation.
05p,01may87,gae  made fdClose() return status.  Changed fd208Drv() to
		   work properly when in EPROM (static initialization).
		 included "i208.h" and disposed of file.
05o,02apr87,gae  made fd208Drv() benign on successive calls.
		 Made cmndAndRslt() local.  Documentation.
05n,25mar87,jlf  documentation.
05m,04feb87,llk  added nFiles field to disk format record (DISK_REC).
		 Used to determine the size of the RT-11 directory.
05l,21dec86,dnw  changed to not get include files from default directories.
05k,21nov86,gae	 housecleaning.
05j,16nov86,gae	 added fdSem for semaphoring board.
05i,16oct86,gae	 added modeChange() using rtModeChange() to indicate read only
		   volumes; made fdIoctl() format check for write protection.
05h,06oct86,dnw  renamed printErr to fdPrintErr, to not conflict w/ system
		   printErr.
		 changed to use printErr instead of printf for error msgs.
05g,15aug86,ecs  renamed fdIoctrl to fdIoctl, changed to call rtIoctl.
		 changed fd208Drv to check for fd208 hardware, return status.
05f,10aug86,dnw  Increased timeouts to 2 secs each.
		 Added tick delay after controller reset command.
		 Changed fd208HrdInit to use variable "zero" instead of "0"
		   to prevent read-modify-write cycle caused by "clr" instr.
05e,07apr86,dnw  Changed fdReset to not call rtReadyChange on recalibrate
		   error.  rtLib now handles this when reset returns error.
05d,08mar86,dnw  Replaced int_level variable with call to intContext ().
		 Moved interrupt semaphore and status from device structure
		   to drive structure.
		 Removed calls to intDrop().
		 Added watchdog timer.
*/

/*
DESCRIPTION
This is the I/O driver for the Intel iSBC 208, or the Zendex ZX-208a,
Multibus floppy disk controller boards.

USER CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly: fd208Drv () to
initialize the driver, and fd208DevCreate () to create devices.

FD208DRV
Before using the driver, it must be initialized by calling the routine:
.CS
    fd208Drv ()
.CE
This routine should be called exactly once, before any reads, writes, or
fd208DevCreates.  Normally, it is called from usrRoot.

CREATING DISK DEVICES
Before a disk device can be used, it must be created.  This is done
with the fd208DevCreate call.
Each drive to be used may have one or more devices associated with it,
by calling this routine.
The way the device is created with this call determines whether the 
device will be single or double density, whether it covers the whole
disk or just part of it, whether it is RT-11 format-compatible, etc.

.CS
 STATUS fd208DevCreate (name, drive, rtFmt, tOffset, nTracks, devType)
     char *name;	/* name of this device *
     int drive;		/* drive number *
     BOOL rtFmt;	/* TRUE if this device is RT-11 formatted *
     int tOffset;	/* number of tracks offset from beginning *
			 * of physical device. *
     int nTracks;	/* length of this device, in tracks; *
			 * if 0, the rest of the disk. *
     int devType;	/* SS_1D_8 or SS_2D_8 *
.CE

For instance, to create the device "/fd0d/", RT-11 compatible, covering
the whole disk, single density, on drive 0, the proper call would be:
.CS
    fd208DevCreate ("/fd0d/", 0, TRUE, 1, 0, SS_1D_8);
.CE
IOCTL
This driver responds to all the same ioctl codes as a normal block
device driver.  To format a disk use FIODISKFORMAT.

RT-11 COMPATIBILITY
In order to maintain true RT-11 compatibility, to the extent that a floppy
may be written under VxWorks and read under RT-11, or vice-versa, the device
must be created single density, with RT-11 formatting TRUE, and with a track
offset of 1.

MULTIPLE DEVICES ON A DRIVE
It is possible, and reasonable, to have more than one logical device for
a drive.  For instance, if the drive will sometimes be accessed in
RT-11 compatible single density format, and sometimes in double
density non-RT-11, and sometimes as a special device covering only
a few tracks, the following three devices might be created:
.CS
    fd208DevCreate ("/rt0/", 0, TRUE, 1, 0, 1);
    fd208DevCreate ("/fd0/", 0, FALSE, 0, 0, 2);
    fd208DevCreate ("/xd0/", 0, FALSE, 10, 10, 2);
.CE
Now, by using devices named "/rt0/", "/fd0/", or "/xd0/", the drive
may be accessed in any of the ways described above.

INTERNAL 
FdReset might need work for multiple drives since it doesn't do anything
about terminating other commands outstanding on other drives when it
re-initializes the controller. 
*/

#include "vxWorks.h"
#include "ioLib.h"
#include "iosLib.h"
#include "semLib.h"
#include "rt11Lib.h"
#include "wdLib.h"
#include "memLib.h"


#define IV_208		((FUNCPTR *) 0x010c)	/* Intel 208 bd. 8259 lvl 3 */

/* IO port addresses */

#define IO_208			((char *) 0xef0080)	/* 0x30 bytes */

#define I208_DMA0_ADR		(IO_208 + 0x00) 
#define I208_DMA0_CNT		(IO_208 + 0x01)
#define I208_DMA2_ADR		(IO_208 + 0x04)
#define I208_DMA2_CNT		(IO_208 + 0x05)
#define I208_DMA3_ADR		(IO_208 + 0x06)
#define I208_DMA3_CNT		(IO_208 + 0x07)
#define I208_DMA_CMND		(IO_208 + 0x08)
#define I208_DMA_REQ		(IO_208 + 0x09)
#define I208_DMA_SR_MASK	(IO_208 + 0x0a)
#define I208_DMA_MODE		(IO_208 + 0x0b)
#define I208_DMA_FL		(IO_208 + 0x0c)
#define I208_DMA_RESET		(IO_208 + 0x0d)
#define I208_DMA_LD_MASK	(IO_208 + 0x0f)
#define I208_FDC_STAT		(IO_208 + 0x10)
#define I208_FDC_DATA		(IO_208 + 0x11)
#define I208_AUX		(IO_208 + 0x12)
#define I208_POLL_STAT		(IO_208 + 0x12)
#define I208_RESET		(IO_208 + 0x13)
#define I208_LSB_SEG_ADR	(IO_208 + 0x14)
#define I208_MSB_SEG_ADR	(IO_208 + 0x15)

/* Bit values to write to DMAC. All other bits must be 0. */

#define I208_DMA_C_ROT_PRI	0x10		/* Rotating priority if 1 */
#define I208_DMA_C_DISABLE	0x04		/* disable DMA if 1 */

/* Bit values for DMA status */

#define I208_DMA_S_REQ3		0x80		/* Channel 3 DMA req */
#define I208_DMA_S_REQ2		0x40		/* Channel 2 req */
#define I208_DMA_S_REQ0		0x10		/* Channel 0 req */
#define I208_DMA_S_TC3		0x08		/* Channel 3 terminal cnt */
#define I208_DMA_S_TC2		0x04		/* Channel 2 terminal cnt */
#define I208_DMA_S_TC0		0x01		/* Channel 0 terminal cnt */

/* Bit values to write to DMA request reg */

#define I208_DMA_R_SET		0x04		/* Set request bit */
#define I208_DMA_R_RST		0x00		/* Reset request bit */
#define I208_DMA_R_C0		0x00		/* Select channel 0 */
#define I208_DMA_R_C1		0x01		/* Select channel 1 */
#define I208_DMA_R_C2		0x02		/* Select channel 2 */
#define I208_DMA_R_C3		0x03		/* Select channel 3 */

/* Bit values to write to DMA Set/Reset mask reg */

#define I208_DMA_MS_SET		0x04		/* Set mask bit */
#define I208_DMA_MS_RST		0x00		/* Reset mask bit */
#define I208_DMA_MS_C0		0x00		/* Select channel 0 */
#define I208_DMA_MS_C1		0x01		/* Select channel 1 */
#define I208_DMA_MS_C2		0x02		/* Select channel 2 */
#define I208_DMA_MS_C3		0x03		/* Select channel 3 */

/* Bit values to write to DMA load mask reg. */

#define I208_DMA_ML_S0		0x01		/* Set channel 0 mask bit */
#define I208_DMA_ML_S1		0x02		/* Set channel 1 mask bit */
#define I208_DMA_ML_S2		0x04		/* Set channel 2 mask bit */
#define I208_DMA_ML_S3		0x08		/* Set channel 3 mask bit */

/* Bit values to write to DMA Mode reg */

#define I208_DMA_M_BLOCK	0x80		/* Block mode */
#define I208_DMA_M_SINGLE	0x40		/* Single mode */
#define I208_DMA_M_DEMAND	0x00		/* Demand mode */
#define I208_DMA_M_INC		0x00		/* Address increment */
#define I208_DMA_M_DEC		0x20		/* Address decrement */
#define I208_DMA_M_AUTO_EN	0x10		/* Autoinitialize enable */
#define I208_DMA_M_VERIFY	0x00		/* Verify transfer */
#define I208_DMA_M_WRITE	0x04		/* Write transfer */
#define I208_DMA_M_READ		0x08		/* Read transfer */
#define I208_DMA_M_C0		0x00		/* Select channel 0 */
#define I208_DMA_M_C1		0x01		/* Select channel 1 */
#define I208_DMA_M_C2		0x02		/* Select channel 2 */
#define I208_DMA_M_C3		0x03		/* Select channel 3 */

/* Bit values read from FDC status reg */

#define I208_FDC_S_RQM		0x80		/* Request for master */
#define I208_FDC_S_DIO		0x40		/* Data IO */
#define I208_FDC_S_NON_DMA	0x20		/* Non-dma mode */
#define I208_FDC_S_BUSY		0x10		/* FDC busy */
#define I208_FDC_S_3BUSY	0x08		/* FDD 3 busy */
#define I208_FDC_S_2BUSY	0x04		/* FDD 2 busy */
#define I208_FDC_S_1BUSY	0x02		/* FDD 1 busy */
#define I208_FDC_S_0BUSY	0x01		/* FDD 0 busy */

/* Bits read from poll status byte */

#define I208_P_1		0x01		/* Controller interrupt */

/* Commands to the FDC chip */

#define I208_FDC_C_FM		0x00
#define I208_FDC_C_MFM		0x40

#define I208_FDC_C_SPECIFY	0x03
#define I208_FDC_C_SEEK		0x0f
#define I208_FDC_C_RD		0x06
#define I208_FDC_C_RD_DEL	0x0c
#define I208_FDC_C_RD_ID	0x0a
#define I208_FDC_C_RD_TRACK	0x02
#define I208_FDC_C_WT		0x05
#define I208_FDC_C_WT_DEL	0x09
#define I208_FDC_C_FORMAT	0x0d
#define I208_FDC_C_RECALIBRATE	0x07
#define I208_FDC_C_SN_DRIVE	0x04
#define I208_FDC_C_SN_INT	0x08

/* status register 0 definitions */

#define ST0			0	/* index in result data of st0 */
#define ST0_DRIVE		0x03	/* drive number mask */

#define ST0_INTCODE		0xc0	/* interrupt reason code */

#define ST0_IC_ERROR		0x40	/* abnormal command termination */
#define ST0_IC_INVALIDCOMMAND	0x80	/* invalid command phase */
#define ST0_IC_READYCHANGE	0xc0	/* drive changed ready state */

/* status register 3 definitions */

#define ST3_FAULT		0x80	/* fault signal from drive */
#define ST3_WRT_PROTECT		0x40	/* write protected */
#define ST3_READY		0x20	/* ready signal from drive */
#define ST3_TRACK_0		0x10	/* track 0 signal from drive */
#define ST3_TWO_SIDED		0x08	/* two sided signal from drive */
#define ST3_HEAD		0x04	/* side signal from drive */
#define ST3_UNIT		0x03	/* drive number */


#define DRIVE_0		0

#define DIREC_READ	0
#define DIREC_WRITE	1

#define TRACKS_PER_DISK		77
#define SECTORS_PER_TRACK	26

typedef struct	/* DISK_REC - disk format record */
    {
    int tracks_per_disk;
    int sectors_per_track;
    int bytes_per_sector;
    int mf;			/* FM or MFM (for double density) */
    int n;			/* bytes per sector code */
    int gaplength;
    int gpl_format;
    int dtl;			/* bytes per sector (only if n==00) */
    int nFiles;			/* number of files */
    } DISK_REC;

LOCAL DISK_REC diskRec [] =
    {
	/* single sided single density constants */
	{ TRACKS_PER_DISK, SECTORS_PER_TRACK, 128,
	  I208_FDC_C_FM,  0x00, 0x07, 0x1B, 0x80, RT_FILES_FOR_2_BLOCK_SEG},

	/* single sided double density constants */
	{ TRACKS_PER_DISK, SECTORS_PER_TRACK, 256,
	  I208_FDC_C_MFM, 0x01, 0x0E, 0x36, 0xFF, RT_FILES_FOR_2_BLOCK_SEG}
    };

typedef struct		/* FORMAT_REC - track format record */
    {
    UTINY track;
    UTINY head;
    UTINY secnum;
    UTINY secsize;
    } FORMAT_REC;

typedef struct fd208Dev /* FD_208_DEV - fd208 device descriptor */
    {
    RT_VOL_DESC rtvol;		/* RT-11 volume descriptor */
    int drive;			/* drive number */
    int trkOffset;		/* Offset at which this device starts. */
    DISK_REC *dr;		/* disk format record info */
    struct fd208Dev *next;	/* For chaining devs */
    } FD_208_DEV;

typedef struct		/* DRIVE - one per drive */
    {
    int drive;			/* drive number */
    FD_208_DEV *devs;		/* list of devs created on this drive */
    FD_208_DEV *activeDev;	/* Last device making a request on
				   this drive */
    short cur_track;		/* current track drive is positioned to */
    SEM_ID intSemId;		/* interupt level to task level semaphore */
    int intStatus;		/* status from interrupt level processing */
    WDOG_ID wdog;		/* watchdog id */
    BOOL timedOut;		/* set TRUE if wdog times out */
    } DRIVE;


#define RD_WRT_TIMEOUT	120	/* 2 sec timeout for sector read/write */
#define SEEK_TIMEOUT	120	/* 2 sec timeout for seek */
#define RECAL_TIMEOUT	120	/* 2 sec timeout for recalibrate */
#define FORMAT_TIMEOUT	120	/* 2 sec timeout for format */
#define RD_TRK_TIMEOUT	120	/* 2 sec timeout for read track */

#define NUM_FD208_DRIVES	2

LOCAL int fd208DrvNum;	/* driver number of fd208 driver */

LOCAL BOOL fd208_debug;	/* TRUE = output debugging info */
LOCAL int lastDriveNum;	/* last drive accessed */

LOCAL DRIVE drives [NUM_FD208_DRIVES];


/* sector number table for s2:
    single density, single sided, 26 sectors, 128 bytes/sector, interleave 2 */

LOCAL UTINY s2_interleave [] =
    { 01, 14, 02, 15, 03, 16, 04, 17, 05, 18, 06, 19, 07, 
      20, 08, 21, 09, 22, 10, 23, 11, 24, 12, 25, 13, 26, };


LOCAL SEM_ID fd208SemId;		/* for semaphoring fd208 board */

/* forward declarations */

LOCAL int fdCreate ();
LOCAL STATUS fdDelete ();
LOCAL int fdOpen ();
LOCAL STATUS fdClose ();
LOCAL int fdRead ();
LOCAL int fdWrite ();
LOCAL STATUS fdReset ();
LOCAL STATUS fdIoctl ();
LOCAL VOID fdIntlvl ();
LOCAL STATUS fdRdSec ();
LOCAL STATUS fdWrtSec ();
LOCAL VOID fdTimeout ();

/*******************************************************************************
*
* fd208Drv - install fd208 driver
*
* RETURNS: OK, or ERROR if board not present
*/

STATUS fd208Drv ()

    {
    int ix;

    if (fd208DrvNum > 0)
	return (OK);	/* driver already installed */

    /* probe for hardware;
     * if it's not there, connect major number to null device routines
     */

    if (sysMemProbe (I208_FDC_STAT) == ERROR)
	{
	fd208DrvNum = iosDrvInstall ((FUNCPTR) NULL, (FUNCPTR) NULL, 
				  (FUNCPTR) NULL, (FUNCPTR) NULL, 
				  (FUNCPTR) NULL, (FUNCPTR) NULL, 
				  (FUNCPTR) NULL);
	return (ERROR);
	}

    intConnect (IV_208, fdIntlvl, 0);

    for (ix = 0; ix < NUM_FD208_DRIVES; ix++)
	{
	drives [ix].drive	= ix;
	drives [ix].activeDev	= NULL;
	drives [ix].devs	= NULL;
	drives [ix].cur_track	= 0;
	drives [ix].wdog	= wdCreate ();
	}

    fd208SemId = semCreate ();
    semGive (fd208SemId);

    fd208HrdInit ();

    fd208DrvNum = iosDrvInstall (fdCreate, fdDelete, fdOpen, fdClose,
				 fdRead, fdWrite, fdIoctl);

    return (fd208DrvNum == ERROR ? ERROR : OK);
    }
/*******************************************************************************
*
* fd208DevCreate - create an fd208 device
*
* Create a disk device.  The device can be single or double density, with
* an offset from the beginning of the physical disk, RT-11 formatted or
* not.
*
* RETURNS: OK | ERROR
*/

STATUS fd208DevCreate (name, drive, rtFmt, trkOffset, nTracks, devType)
    char *name;		/* name of this device */
    int drive;		/* drive number */
    BOOL rtFmt;		/* TRUE if this device is RT-11 formatted */
    int trkOffset;	/* number of tracks offset from beginning
			 * of physical device. */
    int nTracks;	/* length of this device, in tracks;
			 * if 0, the rest of the disk. */
    int devType;	/* SS_1D_8 or SS_2D_8 */

    {
    FAST FD_208_DEV *pFd208Dv;

    if (fd208DrvNum <= 0)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    if ((pFd208Dv = (FD_208_DEV *) malloc (sizeof (FD_208_DEV))) == NULL)
	{
	printErr ("fd208Drv: can't allocate device.\n");
	return (ERROR);
	}

    switch (devType)
	{
	case SS_1D_8:
	    pFd208Dv->dr = &diskRec [0];
	    break;

	case SS_2D_8:
	    pFd208Dv->dr = &diskRec [1];
	    break;

	default:
	    free ((char *) pFd208Dv); 
	    return(ERROR);
	}

    if (nTracks == 0)
	nTracks = pFd208Dv->dr->tracks_per_disk - trkOffset;

    if (rt11DevInit ((RT_VOL_DESC *) pFd208Dv, pFd208Dv->dr->bytes_per_sector,
		   pFd208Dv->dr->sectors_per_track,
		   nTracks * pFd208Dv->dr->sectors_per_track, rtFmt,
		   pFd208Dv->dr->nFiles, fdRdSec, fdWrtSec, fdReset) == ERROR)
	{
	free ((char *) pFd208Dv); 
	printErr ("fd208Drv: can't init device.\n");
	return (ERROR);
	}

    pFd208Dv->drive = drive;
    pFd208Dv->trkOffset = trkOffset;

    if (iosDevAdd ((DEV_HDR *) pFd208Dv, name, fd208DrvNum) == ERROR)
	{
	free ((char *) pFd208Dv); 
	printErr ("fd208Drv: can't add device.\n");
	return (ERROR);
	}

    /* Insert the device in the drive's device chain */

    pFd208Dv->next = drives [drive].devs;
    drives [drive].devs = pFd208Dv;

    return (OK);
    }
/*******************************************************************************
*
* fd208Dbg - turn debugging output on/off
*/

VOID fd208Dbg (new_debug)
    BOOL new_debug;	/* new debug state */

    {
    fd208_debug = new_debug;
    }

/*
routines supplied to ioLib:
	fdCreate,
	fdDelete,
	fdOpen,
	fdClose,
	fdRead,
	fdWrite,
	fdIoctl,
*/

/*******************************************************************************
*
* fdCreate - create an RT-11 file
*
* RETURNS: file descriptor number, or ERROR.
*/

LOCAL int fdCreate (pFd208Dv, name, mode)
    FD_208_DEV *pFd208Dv;	/* pointer to fd208 device descriptor */
    char *name;			/* name of file to open */
    int mode;			/* access mode */

    {
    RT_FILE_DESC *pfd = rt11Create ((RT_VOL_DESC *) pFd208Dv, name, mode);

    if (pfd == NULL)
	return (ERROR);

    return ((int) pfd);
    }
/*******************************************************************************
*
* fdDelete - delete an RT-11 file
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS fdDelete (pFd208Dv, name)
    FD_208_DEV *pFd208Dv;	/* pointer to fd208 device descriptor */
    char *name;			/* name of file to open */

    {
    return (rt11Delete ((RT_VOL_DESC *) pFd208Dv, name));
    }
/*******************************************************************************
*
* fdOpen - open an RT-11 file
*
* RETURNS: file descriptor number, or ERROR.
*/

LOCAL int fdOpen (pFd208Dv, name, mode)
    FD_208_DEV *pFd208Dv;	/* pointer to fd208 device descriptor */
    char *name;			/* name of file to open */
    int mode;			/* access mode */

    {
    RT_FILE_DESC *pfd = rt11Open ((RT_VOL_DESC *) pFd208Dv, name, mode);

    if (pfd == NULL)
	return (ERROR);

    return ((int) pfd);
    }
/*******************************************************************************
*
* fdClose - close an RT-11 file
*/

LOCAL STATUS fdClose (pfd)
    RT_FILE_DESC *pfd;		/* file descriptor of file to close */

    {
    return (rt11Close (pfd));
    }
/*******************************************************************************
*
* fdRead - read from an RT-11 file
*
* RETURNS: number of bytes read, or
*          0 if end of file, or
*          ERROR if read error.
*/

LOCAL int fdRead (pfd, buffer, maxbytes)
    RT_FILE_DESC *pfd;	/* file descriptor of file to close */
    char *buffer;	/* buffer to receive data */
    int maxbytes;	/* max bytes to read in to buffer */

    {
    return (rt11Read (pfd, buffer, maxbytes));
    }
/*******************************************************************************
*
* fdWrite - write to an RT-11 file
*
* RETURNS: number of bytes written, or ERROR.
*/

LOCAL int fdWrite (pfd, buffer, nbytes)
    RT_FILE_DESC *pfd;	/* file descriptor of file to close */
    char *buffer;	/* buffer to be written */
    int nbytes;		/* number of bytes to write from buffer */

    {
    return (rt11Write (pfd, buffer, nbytes));
    }
/*******************************************************************************
*
* fdIoctl - do device specific control function
*/

LOCAL STATUS fdIoctl (pfd, function, arg)
    RT_FILE_DESC *pfd;	/* file descriptor of file to control */
    int function;	/* function code */
    int	arg;		/* some argument */

    {
    char wp;
    STATUS status;
    FD_208_DEV *pFd208Dv = (FD_208_DEV *) pfd->rfd_vdptr;
    DISK_REC *pDr = pFd208Dv->dr;

    switch (function)
	{
	case FIODISKFORMAT:

	    /* format the disk;
	     * fdReadyChange is called after formatting to indicate that
	     * any previous volume information is now invalid;
	     * drive is reset before and after formatting to clear any errors */

	    fdReset (pFd208Dv);		/* to clear any errors */

	    senseDriveStatus (pFd208Dv->drive, &wp);

	    if ((wp & ST3_WRT_PROTECT) != 0)
		{
		errnoSet (S_ioLib_WRITE_PROTECTED);
		return (ERROR);
		}

	    takeDrive (pFd208Dv);
	    status = fdFormat (pFd208Dv, pDr->n, pDr->sectors_per_track,
		 	       pDr->gpl_format, 0xff, 
			       (arg == NULL) ? s2_interleave : (UTINY *) arg);

	    fdReadyChange (&drives [pFd208Dv->drive]);
	    releaseDrive (pFd208Dv);

	    if (status != OK)
		fdReset (pFd208Dv);	/* to clear any errors */
	    
	    return (status);

	default:
	    return (rt11Ioctl (pfd, function, arg));
	}
    }

/*
routines supplied to rtLib:
	fdReset,
	fdRdSec,
	fdWrtSec
*/

/*******************************************************************************
*
* fdReset - reset drive to known state
*
* This routine is supplied to rtLib as the routine to call to reset
* a drive.  It is called when a volume is to be initialized either
* after an error or a ready change.
* 
* NOTE:
* If any errors occur in resetting, fdReadyChange is called to 
* indicate to rtLib that the volumes on this disk are not ready.
*
* Also, because fd208HdrInit is called, this routine would wipe out
* parallel commands (overlapped seeks?) on other drives, but this
* routine doesn't do anything to terminate or notify such commands
* in progress.
*
* RETURNS: OK | ERROR
*/
LOCAL STATUS fdReset (pFd208Dv)
    FAST FD_208_DEV *pFd208Dv;	/* pointer to device desriptor */

    {
    FAST DRIVE *pDrive = &drives [pFd208Dv->drive];
    char wp;

    /* do hard init to clear a hung fdc or dma */

    fd208HrdInit ();

    /* recalibrate drive */

    takeDrive (pFd208Dv);

    if (recalibrate (pDrive) == ERROR)
	{
	releaseDrive (pFd208Dv);
	return (ERROR);
	}

    /* set write protect status */

    senseDriveStatus (pFd208Dv->drive, &wp);
    
    fdModeChange (pDrive, ((wp & ST3_WRT_PROTECT) != 0) ? READ : UPDATE);

    releaseDrive (pFd208Dv);
    return (OK);
    }
/*******************************************************************************
*
* fdRdSec - read a sector from an RT-11 volume
*
* This routine reads the specified physical sector from the specified volume.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS fdRdSec (pFd208Dv, secNum, buffer)
    FAST FD_208_DEV *pFd208Dv;	/* pointer to device desriptor */
    FAST int secNum;		/* number of sector to read */
    char *buffer;		/* buffer to receive block */

    {
    FAST int secPerTrack = pFd208Dv->rtvol.vd_secTrack;
    int track = (secNum / secPerTrack) + pFd208Dv->trkOffset;
    int phys_sector = (secNum % secPerTrack) + 1;

    /* read the sector */

    takeDrive (pFd208Dv);

    if ((seek (pFd208Dv, track) < OK) ||
	(secRead (pFd208Dv, track, phys_sector, buffer) < OK))
	{
	releaseDrive (pFd208Dv);
	return (ERROR);
	}

    releaseDrive (pFd208Dv);
    return (OK);
    }
/*******************************************************************************
*
* fdWrtSec - write a sector to an RT-11 volume
*
* This routine writes the specified sector to the specified volume.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS fdWrtSec (pFd208Dv, secNum, buffer)
    FAST FD_208_DEV *pFd208Dv;	/* pointer to device desriptor */
    FAST int secNum;		/* number of sector to write */
    char *buffer;		/* buffer to write to block */

    {
    FAST int secPerTrack = (pFd208Dv->rtvol).vd_secTrack;
    int phys_sector = (secNum % secPerTrack) + 1;
    int track = (secNum / secPerTrack) + pFd208Dv->trkOffset;

    /* write the sector */

    takeDrive (pFd208Dv);

    if ((seek (pFd208Dv, track) < OK) ||
	(secWrite (pFd208Dv, track, phys_sector, buffer) < OK))
	{
	releaseDrive (pFd208Dv);
	return (ERROR);
	}

    releaseDrive (pFd208Dv);
    return (OK);
    }

/*******************************************************************************
*
* fd208HrdInit - Initialize intel 208 board hardware.
*
* NOTE: When writing a 0 to a register on a controller you have to be careful,
*       because the C statement "*adrs = 0;" gets compiled into "clr.b adrs".
*       The problem with this is that for some reason, known only to the gnomes
*       at Motorola, a "clr" instruction first reads the address before writing
*       a zero there!  In some cases, this read will screw up the sequence of
*       writing registers.  Hence the use of the variable
*       "zero" the assignment of which is compiled into "move.b  dn,adrs".
*/

LOCAL VOID fd208HrdInit ()

    {
    FAST char zero = 0;	/* see explanation above */

    /* Reset the board and DMA controller */

    *I208_RESET = zero;

    vxTdelay (2);		/* wait for reset to finish */

    *I208_DMA_RESET = zero;

    /* Set FDC chip parameters. */

    specify (0x08, 0x0f, 0x8e);
    }
/*******************************************************************************
*
* fdFormat - format a diskette
* 
* RETURNS: OK | ERROR
*/

LOCAL STATUS fdFormat (pFd208Dv, n, sectors_per_track, gap, filler, secnums)
    FD_208_DEV *pFd208Dv;	/* pointer to device desriptor */
    int n;			/* code for bytes per sector on this track */
    int sectors_per_track;	/* number of sectors on this track */
    int gap;			/* length of gap between sectors */
    char filler;		/* byte with which to fill all sector data */
    UTINY secnums [];		/* sector numbers in physical order */

    {
    char cmndBuf [6];
    char rsltBuf [7];
    FORMAT_REC table [SECTORS_PER_TRACK];	/* format table */
    short track;
    short i;

    cmndBuf [0] = I208_FDC_C_FORMAT | pFd208Dv->dr->mf;
    cmndBuf [1] = pFd208Dv->drive;
    cmndBuf [2] = n;
    cmndBuf [3] = sectors_per_track;
    cmndBuf [4] = gap;
    cmndBuf [5] = filler;


    if (recalibrate (&drives [pFd208Dv->drive]) == ERROR)
	return (ERROR);


    /* format each track */

    for (track = 0; track < pFd208Dv->dr->tracks_per_disk; track++)
	{
	/* fill in format table */

	for (i = 0; i < sectors_per_track; i++)
	    {
	    table [i].track	= track;
	    table [i].head	= 0;
	    table [i].secsize	= n;
	    table [i].secnum	= secnums [i];
	    }

	if (seek (pFd208Dv, track) < OK)
	    return (ERROR);

	dmaSetup ((char *) table, sizeof (FORMAT_REC) * sectors_per_track, 
		  DIREC_WRITE);

	if (cmndAndRslt (cmndBuf, sizeof (cmndBuf), rsltBuf, sizeof (rsltBuf),
		     	 FORMAT_TIMEOUT) == ERROR)
	    return (ERROR);

	if ((rsltBuf [ST0] & ST0_INTCODE) != OK)
	    {
	    fdPrintErr (rsltBuf, sizeof (rsltBuf), "format error, result = ");
	    errnoSet (S_ioLib_DEVICE_ERROR);
	    return (ERROR);
	    }
	}

    return (OK);
    }
/*******************************************************************************
*
* recalibrate - recalibrate drive
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS recalibrate (pDrive)
    FAST DRIVE *pDrive;

    {
    char cmndBuf [2];

    cmndBuf [0] = I208_FDC_C_RECALIBRATE;
    cmndBuf [1] = pDrive->drive;

    if (cmndAndRslt (cmndBuf, sizeof(cmndBuf), (char *) NULL, 0, 
		     RECAL_TIMEOUT) == ERROR)
	return (ERROR);

    if (pDrive->intStatus < OK)
	{
	errnoSet (S_ioLib_DEVICE_ERROR);
	return (ERROR);
	}

    pDrive->cur_track = 0;
    return (OK);
    }
/*******************************************************************************
*
* secRead - read a sector
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS secRead (pFd208Dv, track, sector, dataBuffer)
    FAST FD_208_DEV *pFd208Dv;	/* pointer to device desriptor */
    int track;			/* track to read */
    int sector;			/* sector to read */
    char *dataBuffer;		/* buffer into which to read sector */

    {
    char cmndBuf [9];
    char rsltBuf [7];

    cmndBuf [0] = I208_FDC_C_RD | pFd208Dv->dr->mf;
    cmndBuf [1] = pFd208Dv->drive;
    cmndBuf [2] = track;
    cmndBuf [3] = 0;
    cmndBuf [4] = sector;
    cmndBuf [5] = pFd208Dv->dr->n;
    cmndBuf [6] = pFd208Dv->dr->sectors_per_track;
    cmndBuf [7] = pFd208Dv->dr->gaplength;
    cmndBuf [8] = pFd208Dv->dr->dtl;

    dmaSetup (dataBuffer, pFd208Dv->dr->bytes_per_sector, DIREC_READ);

    if (cmndAndRslt (cmndBuf, sizeof (cmndBuf), rsltBuf, sizeof (rsltBuf),
		     RD_WRT_TIMEOUT) == ERROR)
	return (ERROR);

    if ((rsltBuf [ST0] & ST0_INTCODE) != OK)
	{
	fdPrintErr (rsltBuf, sizeof (rsltBuf), "read error, result = ");
	errnoSet (S_ioLib_DEVICE_ERROR);
	return (ERROR);
	}

    return (OK);
    }
/*******************************************************************************
*
* secWrite - write a sector
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS secWrite (pFd208Dv, track, sector, dataBuffer)
    FAST FD_208_DEV *pFd208Dv;	/* pointer to device desriptor */
    int track;			/* track to write */
    int sector;			/* sector to write */
    char *dataBuffer;		/* buffer from which to write sector */

    {
    char cmndBuf [9];
    char rsltBuf [7];

    cmndBuf [0] = I208_FDC_C_WT | pFd208Dv->dr->mf;
    cmndBuf [1] = pFd208Dv->drive;
    cmndBuf [2] = track;
    cmndBuf [3] = 0;
    cmndBuf [4] = sector;
    cmndBuf [5] = pFd208Dv->dr->n;
    cmndBuf [6] = pFd208Dv->dr->sectors_per_track;
    cmndBuf [7] = pFd208Dv->dr->gaplength;
    cmndBuf [8] = pFd208Dv->dr->dtl;

    dmaSetup (dataBuffer, pFd208Dv->dr->bytes_per_sector, DIREC_WRITE);

    if (cmndAndRslt (cmndBuf, sizeof (cmndBuf), rsltBuf, sizeof (rsltBuf),
		     RD_WRT_TIMEOUT) == ERROR)
	return (ERROR);

    if ((rsltBuf [ST0] & ST0_INTCODE) != OK)
	{
	fdPrintErr (rsltBuf, sizeof (rsltBuf), "write error, result = ");
	errnoSet (S_ioLib_DEVICE_ERROR);
	return (ERROR);
	}

    return (OK);
    }
/*******************************************************************************
*
* seek - seek drive to cylinder
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS seek (pFd208Dv, track)
    FAST FD_208_DEV *pFd208Dv;	/* pointer to device desriptor */
    int track;			/* track to which to seek */

    {
    char cmndBuf [3];
    DRIVE *pDrive = &drives [pFd208Dv->drive];

    if (pDrive->cur_track != track)
	{
	cmndBuf [0] = I208_FDC_C_SEEK;
	cmndBuf [1] = pFd208Dv->drive;
	cmndBuf [2] = track;

	if (cmndAndRslt (cmndBuf, sizeof(cmndBuf), (char *) NULL, 0, 
			 SEEK_TIMEOUT) == ERROR)
	    return (ERROR);

	if (pDrive->intStatus < OK)
	    {
	    fdPrintErr ((char *) NULL, 0, "fd208Drv: seek error.");
	    errnoSet (S_ioLib_DEVICE_ERROR);
	    return (ERROR);
	    }

	pDrive->cur_track = track;
	}

    return (OK);
    }
/*******************************************************************************
*
* senseDriveStatus - get drive status
*
* RETURNS: OK | ERROR
*/

LOCAL VOID senseDriveStatus (drive, pStatus)
    int drive;
    char *pStatus;	/* where to return status byte */

    {
    char cmndBuf [2];

    cmndBuf [0] = I208_FDC_C_SN_DRIVE;
    cmndBuf [1] = drive;

    if (cmndPhase (cmndBuf, sizeof (cmndBuf)) != OK ||
	rsltPhase (pStatus, 1) != OK)
	{
	printErr ("fd208Drv: senseDriveStatus failed\n");
	}
    }
/*******************************************************************************
*
* specify - issue 'specify' command: set fdc parameters
*/

LOCAL VOID specify (step_rate, head_unload, head_load)
    int step_rate;	/* Step rate time, in ms */
    int head_unload;	/* Head unload time, in ms */
    int head_load;	/* Head load time, in ms */

    {
    char cmndBuf [3];

    cmndBuf [0] = I208_FDC_C_SPECIFY;
    cmndBuf [1] = (step_rate << 4) + (head_unload & 0x0f);
    cmndBuf [2] = head_load;

    if (cmndPhase (cmndBuf, 3) != OK)
	printErr ("fd208Drv: specify failed\n");
    }

/*******************************************************************************
*
* cmndAndRslt - send a command, wait for execution, and get result
*
* This routine sends the specified command to the controller and waits
* for the execution to complete.  Unless the rsltLen is specified 0,
* the result will be read into rsltBuf.  The command will be timed
* with a watchdog of the specified number of ticks.
*
* NOTE: this routine assumes that the drive number is in cmndBuf[1].
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS cmndAndRslt (cmndBuf, cmndLen, rsltBuf, rsltLen, timeout)
    char *cmndBuf;	/* command buffer to send */
    int cmndLen; 	/* length of cmndBuf */
    char *rsltBuf;	/* where to put result buffer */
    int rsltLen; 	/* length of rsltBuf */
    int timeout;	/* watchdog timeout in ticks */

    {
    FAST DRIVE *pDrive = &drives [cmndBuf[1]];

    pDrive->intSemId = semCreate ();

    if (cmndPhase (cmndBuf, cmndLen) < OK)
	return (ERROR);

    pDrive->timedOut = FALSE;
    wdStart (pDrive->wdog, timeout, fdTimeout, (int) pDrive);

    semTake (pDrive->intSemId);

    if (pDrive->timedOut)
	{
	fdPrintErr (cmndBuf, cmndLen, "time out: cmnd = ");
	errnoSet (S_ioLib_DEVICE_TIMEOUT);
	return (ERROR);
	}

    wdCancel (pDrive->wdog);

    if (rsltLen != 0)
	{
	if (rsltPhase (rsltBuf, rsltLen) < OK)
	    return (ERROR);
	}
    
    return (OK);
    }
/*******************************************************************************
*
* cmndPhase - send a command to the FDC chip
*
* This routine executes the command phase of a floppy disk command,
* by writing cmndBuf to the FDC chip, in the proper fashion.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS cmndPhase (buf, nbytes)
    char *buf;		/* The command buffer to send */
    short nbytes; 	/* The length of buf */

    {
    FAST short i;

    for (i = 0; i < nbytes; i++)
	{
	/* wait for valid flag settings in status register */

	delay ();

	/* Wait for the FDC board to be ready */

	while (!(*I208_FDC_STAT & I208_FDC_S_RQM))
	    ;

	if (*I208_FDC_STAT & I208_FDC_S_DIO)
	    {
	    fdPrintErr (buf, nbytes, "command phase direction error, cmnd = ");
	    errnoSet (S_ioLib_DEVICE_ERROR);
	    return (ERROR);
	    }

	*I208_FDC_DATA = buf [i];
	}

    if (fd208_debug)
	fdPrintErr (buf, nbytes, "command = ");

    return (OK);
    }
/*******************************************************************************
*
* rsltPhase - Read the result phase values from the FDC chip.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS rsltPhase (buf, nbytes)
    char *buf;
    int nbytes;		/* maximum number of bytes to be accepted */

    {
    FAST int i = 0;

    /* when the FDC is no longer busy, the result phase is finished */

    while (delay (), (*I208_FDC_STAT & I208_FDC_S_BUSY))
	{
	/* check for more bytes than expected */

	if (i >= nbytes)
	    {
	    fdPrintErr (buf, nbytes, "too many result bytes, result = ");
	    errnoSet (S_ioLib_DEVICE_ERROR);
	    return (ERROR);
	    }

	/* Wait for the FDC board to be ready and check direction bit */

	while (!(*I208_FDC_STAT & I208_FDC_S_RQM))
	    ;

	if (!(*I208_FDC_STAT & I208_FDC_S_DIO))
	    {
	    fdPrintErr (buf, i, "result phase direction error, result = ");
	    errnoSet (S_ioLib_DEVICE_ERROR);
	    return (ERROR);
	    }


	/* input byte */

	buf [i++] = *I208_FDC_DATA;
	}


    /* check for no bytes input */

    if (i == 0)
	{
	printErr ("fd208Drv: result phase missing\n");
	errnoSet (S_ioLib_DEVICE_ERROR);
	return (ERROR);
	}

    if (fd208_debug)
	fdPrintErr (buf, i, "result = ");

    return (OK);
    }
/*******************************************************************************
*
* delay - pause for fdc chip to get ready
*
* This routine delays to allow the fdc chip to get ready to read/write
* the next command/status byte.  This delay must be >100us.
*/

LOCAL VOID delay ()

    {
    int i;

    for (i = 0; i < 20; i++)
	;
    }
/*******************************************************************************
*
* dmaSetup - Set up the dma controller chip on the 208 board for a transfer.
*
* NOTE: When writing a 0 to a register on a controller you have to be careful,
*       because the C statement "*adrs = 0;" gets compiled into "clr.b adrs".
*       The problem with this is that for some reason, known only to the gnomes
*       at Motorola, a "clr" instruction first reads the address before writing
*       a zero there!  In some cases, this read will screw up the sequence of
*       writing registers, as in the case of writing the LSB and MSB of the
*       address register (I208_DMA0_ADR) below.  Hence the use of the variable
*       "zero" the assignment of which is compiled into "move.b  dn,adrs".
*/

LOCAL VOID dmaSetup (buf, nchars, direc)
    char *buf;
    int nchars;
    TINY direc;

    {
    FAST long addr = (long) buf;
    FAST char zero = 0;	/* see explanation above */
    
    /* Load the count. The reg actually gets count - 1. */

    *I208_DMA0_CNT = (-- nchars) & 0xff;
    *I208_DMA0_CNT = (nchars >> 8) & 0xff;

    /* Load the start address */

    *I208_DMA_FL = zero;
    *I208_DMA0_ADR = addr & 0x0f;
    *I208_DMA0_ADR = zero;
    *I208_LSB_SEG_ADR = addr >> 4;
    *I208_MSB_SEG_ADR = addr >> 12;
    *I208_POLL_STAT = (addr >> 16) & 0xf0;

    /* Direction */

    *I208_DMA_MODE = (direc == DIREC_READ) ?
			I208_DMA_M_SINGLE + I208_DMA_M_WRITE :
			I208_DMA_M_SINGLE + I208_DMA_M_READ ;
		
    /* Mask all but channel 0 */

    *I208_DMA_LD_MASK = I208_DMA_ML_S1 + I208_DMA_ML_S2 + I208_DMA_ML_S3;
    }

/*******************************************************************************
*
* fdIntlvl - interrupt level routine for fd208 handler
*
* Does not support multiple drives.
*/

LOCAL VOID fdIntlvl ()

    {
    static char cmndBuf [1] = { I208_FDC_C_SN_INT };
    char rsltBuf [2];
    TBOOL invalid_command = FALSE;
    int driveNum = lastDriveNum;
    DRIVE *pDrive = &drives [driveNum];

    if (*I208_FDC_STAT & I208_FDC_S_BUSY)
	{
	/* a serial command (not seek or recalibrate) has finished;
	 * resume task level */

	semGive (pDrive->intSemId);
	}
    else
	{
	/* a seek or recalibrate command finished, or ready-change interrupt,
	 * or unexpected interrupt */

	while (!invalid_command &&
	       (cmndPhase (cmndBuf, sizeof (cmndBuf)) == OK) &&
	       (rsltPhase (rsltBuf, sizeof (rsltBuf)) == OK))
	    {
	    driveNum = rsltBuf [ST0] & ST0_DRIVE;

	    switch (rsltBuf [ST0] & ST0_INTCODE)
		{
		case OK:
		    pDrive->intStatus = OK;
		    semGive (pDrive->intSemId);
		    break;

		case ST0_IC_ERROR:
		    pDrive->intStatus = ERROR;
		    semGive (pDrive->intSemId);
		    break;

		case ST0_IC_READYCHANGE:
		    fdReadyChange (pDrive);
		    break;

		case ST0_IC_INVALIDCOMMAND:
		    invalid_command = TRUE;
		    break;
		}
	    }
	}
    }
/*******************************************************************************
*
* fdTimeout - handle watchdog timeout
*/

LOCAL VOID fdTimeout (pDrive)
    DRIVE *pDrive;

    {
    pDrive->timedOut = TRUE;
    semGive (pDrive->intSemId);
    }
/*******************************************************************************
*
* takeDrive - make a device be active on its drive
*
*/

LOCAL VOID takeDrive (pFd208Dv)
    FD_208_DEV *pFd208Dv;

    {
    semTake (fd208SemId);
    drives [pFd208Dv->drive].activeDev = pFd208Dv;
    lastDriveNum = pFd208Dv->drive;
    }
/*******************************************************************************
*
* releaseDrive - make a drive be inactive
*/

LOCAL VOID releaseDrive (pFd208Dv)
    FD_208_DEV *pFd208Dv;

    {
    drives [pFd208Dv->drive].activeDev = NULL;
    semGive (fd208SemId);
    }
/*******************************************************************************
*
* fdPrintErr - report error
*/

LOCAL VOID fdPrintErr (buf, bufLen, msg)
    char *buf;
    int bufLen;
    char *msg;

    {
    /* don't print error if at interrupt level */

    if (!intContext ())
	{
	printErr ("fd208Drv: ");
	printErr (msg);		/* this way the message will be printed
				   even from romboot */

	while (bufLen-- > 0)
	    printErr ("%02x ", (*(buf++) & 0xff));

	printErr ("\n");
	}
    }
/*******************************************************************************
*
* fdReadyChange - notify of media change
*/

LOCAL VOID fdReadyChange (pDrive)
    DRIVE *pDrive;

    {
    FAST FD_208_DEV *pFd208Dv;	/* active device on this drive */

    for (pFd208Dv = pDrive->devs; pFd208Dv != NULL; pFd208Dv = pFd208Dv->next)
	rt11ReadyChange (&pFd208Dv->rtvol);
    }
/*******************************************************************************
*
* fdModeChange - change READ/WRITE/UPDATE mode of device
*/

LOCAL VOID fdModeChange (pDrive, newMode)
    DRIVE *pDrive;
    BOOL newMode;

    {
    FAST FD_208_DEV *pFd208Dv;	/* active device on this drive */

    for (pFd208Dv = pDrive->devs; pFd208Dv != NULL; pFd208Dv = pFd208Dv->next)
	rt11ModeChange (&pFd208Dv->rtvol, newMode);
    }
