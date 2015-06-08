/* smsDrv.c - disk driver for the SMS 8007, or Intel iSBC 214 board */

/* Copyright 1984,1985,1986,1987,1988,1989 Wind River Systems, Inc. */
extern char copyright_wind_river[]; static char *copyright=copyright_wind_river;

/*
modification history
--------------------
03l,06jun88,dnw  changed rtLib to rt11Lib.
03k,30may88,dnw  changed to v4 names.
03j,04may88,jcf  changed semaphores to be consistent with new semLib.
03i,11feb88,gae  made smsDevCreate() not add device if initialization failed.
		 split initialization from smsDevCreate() into separate rtn.
03h,19nov87,ecs  documentation.
03g,18nov87,ecs  documentation.
03f,06nov87,ecs  documentation.
03e,19sep87,dpg  made it work with romboot.
	   +gae  fixed resetting bug by un-kludging smsReset(), sms{Rd,Wrt}Sec.
		 fixed transfer status bug.  Added smsRomboot().
03d,10sep87,dpg  cleaned up last of hard disk stuff - moved #define HD_1
		 out to ioLib.h
03c,24jul87,dpg  finished hard disk routines, fixed so that smsDevCreate
		 knows whether to create a hard disk or a floppy disk
		 device based on parameters handed to it.
		 fixed smsDrv so second call returns same result as first,
		 not just an arbitrary OK - this is used to make sure that
		 smsDevCreate doesn't try to go to work on a non-existant
		 device.
03b,13jun87,gae  fixed semaphoring of iopb and board accesses.
03a,01jun87,dpg  got hard disk code & tables nearly working.
           +gae  fixed smsReset() to get/set status correctly.
02i,01may87,gae  made smsClose() return status.  Changed smsDrv() to
		   work properly when in EPROM (static initialization).
02h,01apr87,gae  Documentation.  Made smsReset() not print stderr messages.
		 Made smsDrv() return error if hardware not present.
		 Made smsDevCreate() return OK when disk not in drive.
		 Made smsSendCommand(), smsDebugOut(), and smsRdBytes() local.
		 smsClose() returns status.
02g,25mar87,jlf  documentation
02f,21mar87,gae  booboo in 02e, can't have fioSetErr() in smsReset().
02e,07mar87,gae  cleaned up and tried to optimize.  Made smsDrv() be a NOP
		   on successive calls.
02d,04feb87,llk  added nFiles field to disk format record (DISK_REC).
		 Used to determine the size of the RT-11 directory.
02c,21dec86,dnw  changed to not get include files from default directories.
02b,21nov86,dnw	 reworked 24 bit addressing stuff.
	   +gae	 changed to check for disk not present or write protected
		   when errors occur: these errors suppress normal error msg
		   printing and just set appropriate task status.  Made
		   smsReset() be for real.  Changed interleave to 3:1.
02a,01oct86,dnw  changed to be compatible with i214 controller.
		 improved error reporting.
		 merged secRead/secWrite directly into smsRdSec/smsWrtSec.
01b,13sep86,dnw  changed smsDevCreate() to specify MOD_ENABLE_EXTENSIONS
		   modifier on initialize command.
01a,13jun86 ecs  written
*/

/*
DESCRIPTION
This is the I/O driver for the Intel iSBC 214,
or the Scientific Micro Systems 8007,
Multibus disk controller board.

USER CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly, smsDrv () to
initialize the driver, and smsDevCreate () to create devices.

SMSDRV
Before using the driver, it must be initialized by calling the routine:
.CS
    smsDrv ()
.CE
This routine should be called exactly once, before any reads, writes, or
smsDevCreates.  Normally, it is called from usrRoot.

CREATING DISK DEVICES
Before a disk device can be used, it must be created.
This is done with the smsDevCreate call.
Each drive to be used may have one or more devices associated with it,
by calling this routine.
The way the device is created with this call determines whether the 
device will be single or double density, whether it covers the whole
disk or just part of it, whether it is RT-11 format compatible, etc.

.CS
 STATUS smsDevCreate (name, drive, rtFmt, trkOffset, nTracks, devType)
     char *name;     /* name of this device *
     int drive;      /* drive number 0-3 floppy/tape, 4-7 hard disk *
     BOOL rtFmt;     /* TRUE if this device is RT-11 formatted *
     int trkOffset;  /* number of tracks offset from beginning
                      * of physical device *
     int nTracks;    /* length of this device, in tracks;
                      * if 0, the rest of the disk *
     int devType;    /* SS_1D_8, SS_2D_8, DS_1D_8, DS_2D_8, or HD_1 *
.CE

For instance, to create the device "/fd0d/", RT-11 compatible, covering
the whole disk, single density, on drive 0, the proper call would be:
.CS
    smsDevCreate ("/fd0d/", 0, TRUE, 1, 0, 1);
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
    smsDevCreate ("/rt0/", 0, TRUE, 1, 0, 1);
    smsDevCreate ("/fd0/", 0, FALSE, 0, 0, 2);
    smsDevCreate ("/xd0/", 0, FALSE, 10, 10, 2);
.CE
Now, by using devices named "/rt0/", "/fd0/", or "/xd0/", the drive
may be accessed in any of the ways described above.
*/

#include "vxWorks.h"
#include "ioLib.h"
#include "iosLib.h"
#include "rt11Lib.h"
#include "semLib.h"
#include "wdLib.h"
#include "memLib.h"
#include "sysLib.h"


#define IV_SMS		((FUNCPTR *) 0x0104)	/* interrupt vector, level 1 */

/* IO port address */

#define IO_SMS_8007	((char *) 0xef0040)


/* messages to send to IO port */

#define SMS_PIO_CLEAR	0x00
#define SMS_PIO_START	0x01
#define SMS_PIO_RESET	0x02


/* address of Wake-up Block */

#define WAKE_UP_BLOCK	(0x000400)


typedef struct		/* DISK_REC - disk format record */
    {
    int tracksPerDisk;
    int sectorsPerTrack;
    int bytesPerSector;
    int numHeads;		/* number of winchester heads or floppy sides */
    UTINY encoding;		/* floppy recording density & data encoding */
				/* on winch = number of alt. cylinders */
    UTINY deviceType;		/* SMS_DEV_WINCHESTER or SMS_DEV_FLOPPY */
    int nFiles;			/* number of files */
    } DISK_REC;

typedef struct smsDev 	/* SMS_DEV - SMS device descriptor */
    {
    RT_VOL_DESC rtvol;		/* RT-11 volume descriptor */
    int drive;			/* drive number */
    int trackOffset;		/* offset at which this device starts. */
    DISK_REC *diskRec;		/* disk format record info */
    struct smsDev *next;	/* for chaining devs */
    } SMS_DEV;

typedef struct		/* DRIVE - one per drive */
    {
    int drive;			/* drive number */
    SMS_DEV *devs;		/* list of devs created on this drive */
    SMS_DEV *activeDev;		/* last device making a request on this drive */
    BOOL opReset;		/* TRUE if "reset" operation in progress */
    SEM_ID intSemId;		/* interupt level to task level semaphore */
    int intStatus;		/* status from interrupt level processing */
    WDOG_ID wdog;		/* watchdog id */
    BOOL timedOut;		/* set TRUE if wdog times out */
    } DRIVE;

#define BIT0	0x01
#define BIT1	0x02
#define BIT2	0x04
#define BIT3	0x08
#define BIT4	0x10
#define BIT5	0x20
#define BIT6	0x40
#define BIT7	0x80

#define TIMEOUT		240	/* 4 sec timeout */
#define NUM_SMS_DRIVES	8	/* 0-3 floppies & tape, 4-7 winchester */

#define DRVNUM_WINCHESTER	0x04	/* bit for hard drives */
#define DRVNUM_UNIT_MASK	0x03	/* bits for physical unit number */


#define ENC_SD		0x00	/* code for single-density floppy */
#define ENC_DD		0x01	/* code for double-density floppy */

#define ENC_STEP_6MS	0x00	/* code for 6 ms step rate */
#define ENC_STEP_12MS	0x02	/* code for 12 ms step rate */
#define ENC_STEP_20MS	0x06	/* code for 20 ms step rate */
#define ENC_STEP_30MS	0x0a	/* code for 30 ms step rate */

#define ENC_HD_LD_14MS	0x00	/* code for 14 ms head load */
#define ENC_HD_LD_30MS	0x40	/* code for 30 ms head load */

#define ENC_NEC_1035	(ENC_STEP_6MS | ENC_HD_LD_30MS)	/* SHOULDN'T BE HERE! */
#define SMS_DEV_WINCHESTER	0
#define SMS_DEV_FLOPPY		3
#define SMS_DEV_TAPE		4

LOCAL DISK_REC diskRec [] =
    {
	{ 80, 8, 256, 1, ENC_SD | ENC_NEC_1035, SMS_DEV_FLOPPY,	/* ss/sd */
		RT_FILES_FOR_2_BLOCK_SEG},
	{ 80, 8, 512, 1, ENC_DD | ENC_NEC_1035, SMS_DEV_FLOPPY,	/* ss/dd */
		RT_FILES_FOR_2_BLOCK_SEG},
	{160, 8, 256, 2, ENC_SD | ENC_NEC_1035, SMS_DEV_FLOPPY, /* ds/sd */
		RT_FILES_FOR_2_BLOCK_SEG},
	{160, 8, 512, 2, ENC_DD | ENC_NEC_1035, SMS_DEV_FLOPPY,	/* ds/dd */
		RT_FILES_FOR_2_BLOCK_SEG},
	{615 * 4, 16, 512, 4, 5, SMS_DEV_WINCHESTER,		/* winch */
		RT_FILES_FOR_2_BLOCK_SEG},
    };

typedef struct		/* SEG_ADD - Segmented Address */
    {
    UTINY offsetLsb;		/* least significant byte of offset */
    UTINY offsetMsb;		/* most significant byte of offset */
    UTINY segmentLsb;		/* least significant byte of segment */
    UTINY segmentMsb;		/* most significant byte of segment */
    } SEG_ADD;

typedef struct		/* WUB - Wake-up Block */
    {
    USHORT reserved;
    SEG_ADD ccb;		/* segmented address of channel control block */
    } WUB;

typedef struct		/* CCB - Channel Control Block */
    {
    UTINY ctrlWord;		/* always 0x01 */
    UTINY busy;			/* 0xFF => controller is busy, 0x00 => idle */
    SEG_ADD cib;		/* segmented address of 5th byte of CIB */

    /* remaining bytes are set to zero for iSBC compatibility */

    USHORT reserved;
    UTINY ctrlWord2;
    UTINY busy2;
    USHORT ctrlPtrAdrs;
    USHORT ctrlPtr;
    } CCB;

#define SMS_BUSY	0xff
#define SMS_IDLE	0

typedef struct		/* CIB - Controller Invocation Block */
    {
    UTINY reserved1;
    UTINY operationStatus;	/* contains latest controller status */
    UTINY reserved2;
    UTINY statusSemaphore;	/* interlocks operationStatus */
    UINT reserved3;		/* CCB points to first byte of this */
    SEG_ADD iopb;		/* Input/Output Parameter Block */
    UINT reserved4;
    } CIB;

/* operationStatus bits */

#define OS_STATUS_MASK		0x0f
#define OS_WINCH_OP_COMPLETE	0x01
#define OS_WINCH_SEEK_COMPLETE	0x02
#define OS_WINCH_MEDIA_CHANGE	0x04
#define OS_FLOP_TAPE_OP_COMPLETE 0x09
#define OS_FLOP_SEEK_COMPLETE	0x0a
#define OS_FLOP_MEDIA_CHANGE	0x0c
#define OS_TAPE_MEDIA_CHANGE	0x0e
#define OS_TAPE_OTHER_COMPLETE	0x0f

#define OS_FLOPPY_TAPE		BIT3
#define OS_UNIT_MASK		(BIT4 | BIT5)
#define OS_HARD_ERROR		BIT6
#define OS_ERROR_DETECTED	BIT7

typedef struct		/* IOPB - Input/Output Parameter Block */
    {
    UINT reserved;		/* neither read nor written by SMS8007 */
    UINT actualXferCount;	/* returned by controller at end of operation */
    UTINY deviceType;		/* SMS_DEV_WINCHESTER or SMS_DEV_FLOPPY */
    UTINY zero;			/* always zero */
    UTINY unit;			/* which drive of deviceType, 0 or 1 */
    UTINY function;		/* operation to be performed */
    UTINY modifier;		/* 8 control flags */
    UTINY test;			/* which test to perform (always 0 for us) */
    USHORT cylinder;		/* cylinder number to start with */
    UTINY head;			/* head number to start with */
    UTINY sector;		/* sector number to start with */
    SEG_ADD pDataBuffer;	/* where data is to be stored or fetched */
    UINT reqXferCnt;		/* number of bytes that are to be transfered */
    UINT reserved2;
    } IOPB;

/* modifier bits */

#define MOD_SUPPRESS_CMD_COMPLETION_INTERRUPT	BIT0
#define MOD_INHIBIT_ERROR_RETRIES		BIT1
#define MOD_ENABLE_DELETED_DATA_ADDRESS_MARKS	BIT2
#define MOD_24_BIT_ADDRESS_MODE			BIT4
#define MOD_WORD_WIDE_MULTIBUS_TRANSFER		BIT5
#define MOD_DIRECT_MULTIBUS_TRANSFER		BIT6
#define MOD_ENABLE_EXTENSIONS			BIT7

typedef struct		/* IOPB_EXT_INIT - IOPB Extension, Initialize Command */
    {
    USHORT numCylinders;	/* total number of cylinders */
    UTINY winchHeads;		/* number of heads on winchester drive */
    UTINY floppySides;		/* number of sides on floppy drive */
    UTINY sectorsPerTrack;	/* (unless MOD_ENABLE_EXTENSIONS) */
    UTINY bytesPerSectorLSB;	/* least significant byte of bytes/sector */
    UTINY bytesPerSectorMSB;	/* most significant byte of bytes/sector */
    UTINY encoding;		/* floppy recording density & data encoding */
    } IOPB_EXT_INIT;

typedef struct		/* IOPB_EXT_FORMAT - IOPB Extension, Format Command */
    {
    UTINY trackType;		/* winchester track type: normal, alternate,
				** defective */
    UTINY userPattern1;		/* data written to sector data area of track */
    UTINY userPattern2;		/* data written to sector data area of track */
    UTINY userPattern3;		/* data written to sector data area of track */
    UTINY userPattern4;		/* data written to sector data area of track */
    UTINY interleave;		/* interleave factor */
    } IOPB_EXT_FORMAT;


#define STATUS_BUF_LENGTH	13	/* num bytes in following structure */

typedef struct		/* DRIVE_STATUS - drive status returned by controller */
    {
    UTINY status0;
    UTINY status1;
    UTINY status2;
    UTINY desiredCylLSB;
    UTINY desiredCylMSB;
    UTINY desiredHead;
    UTINY desiredSec;
    UTINY actualCylLSB;
    UTINY actualCylMSB;
    UTINY actualHead;
    UTINY actualSec;
    UTINY numRetries;
    UTINY smsExtension;
    } DRIVE_STATUS;


/* status byte definitions */

#define STS0_INVALID_FUNCTION		0x01
#define STS0_ROM_ERROR			0x10
#define STS0_SEEK_IN_PROGRESS		0x20
#define STS0_ILLEGAL_FORMAT_TYPE	0x40
#define STS0_END_OF_MEDIA		0x80

#define STS1_ILLEGAL_SECTOR_SIZE	0x01
#define STS1_DIAG_FAULT			0x02
#define STS1_NO_INDEX			0x04
#define STS1_INVALID_FUNCTION		0x08
#define STS1_SECTOR_NOT_FOUND		0x10
#define STS1_INVALID_ADDRESS		0x20
#define STS1_UNIT_NOT_READY		0x40
#define STS1_DISK_WRITE_PROTECTED	0x80

#define STS2_DATA_FIELD_ECC_ERROR	0x08
#define STS2_ID_FIELD_ECC_ERROR		0x10
#define STS2_DRIVE_FAULT		0x20
#define STS2_CYL_ADRS_MISCOMPARE	0x40
#define STS2_SEEK_ERROR			0x80

/* command codes */

#define SMS_CMD_INITIALIZE	0
#define SMS_CMD_XFER_STATUS	1
#define SMS_CMD_FORMAT		2
#define SMS_CMD_READ_DATA	4
#define SMS_CMD_WRITE_DATA	6

#define SMS_TRACK_NORMAL	0
#define SMS_TRACK_ALTERNATE	0x40
#define SMS_TRACK_DEFECTIVE	0x80

LOCAL CCB ccb;			/* Channel Control Block */
LOCAL CIB cib;			/* Controller Invocation Block */
LOCAL IOPB iopb;		/* Input/Output Parameter Block */

LOCAL int smsDrvNum;		/* driver number of SMS driver */
LOCAL int maxTicks;		/* most ticks that command has taken */
LOCAL BOOL smsDebug;		/* TRUE = output debugging info */
LOCAL BOOL smsRom;		/* TRUE = in ROMBOOT mode */
LOCAL BOOL sms24bit;		/* TRUE = use 24-bit addressing */
LOCAL int xferDriveNum;		/* drive number for int routine */
LOCAL SEM_ID smsSemId;		/* semaphore for right to address SMS board */
LOCAL DRIVE *pDrives[NUM_SMS_DRIVES];


/* forward declarations */

LOCAL int smsCreate ();
LOCAL STATUS smsDelete ();
LOCAL int smsOpen ();
LOCAL STATUS smsClose ();
LOCAL int smsRead ();
LOCAL int smsWrite ();
LOCAL STATUS smsReset ();
LOCAL STATUS smsIoctl ();
LOCAL VOID smsIntlvl ();
LOCAL STATUS smsRdSec ();
LOCAL STATUS smsWrtSec ();
LOCAL VOID smsTimeout ();
LOCAL char *segAddToPtr ();


/*******************************************************************************
*
* smsDrv - install SMS driver
*
* RETURNS: OK or ERROR if board not present
*/

STATUS smsDrv ()

    {
    char test = 0xff;	/* for board probe */

    if (smsDrvNum > 0)
	return (OK);	/* driver already installed  */

    /* probe for hardware;
     * if it's not there, connect major number to null device routines
     */
    
    if (vxMemProbe (IO_SMS_8007, WRITE, 1, &test) == ERROR)
	{
	smsDrvNum = iosDrvInstall ((FUNCPTR) NULL, (FUNCPTR) NULL, 
				(FUNCPTR) NULL, (FUNCPTR) NULL, 
				(FUNCPTR) NULL, (FUNCPTR) NULL, 
				(FUNCPTR) NULL);
	return (ERROR);
	}

    sms24bit = sysMemTop () > (char *) 0x100000;

    intConnect (IV_SMS, smsIntlvl, 0);

    xferDriveNum = NONE;	/* set to invalid state */

    smsHrdInit ();

    smsSemId = semCreate ();	/* initialize semaphore of access to board */
    semGive (smsSemId);		/* let SMS board be used */

    smsDrvNum = iosDrvInstall (smsCreate, smsDelete, smsOpen, smsClose,
				smsRead, smsWrite, smsIoctl);

    return (OK);
    }
/*******************************************************************************
*
* smsDevCreate - create an SMS device
*
* Create a disk device.  The device can be single or double density, with
* an offset from the beginning of the physical disk, RT-11 formatted or not.
*
* The SMS drives are 0-3 for floppy disks or tape units, 4-7 for hard disks.
*
* RETURNS: OK | ERROR
*/

STATUS smsDevCreate (name, drive, rtFmt, trackOffset, nTracks, devType)
    char *name;		/* name of this device */
    int drive;		/* drive number 0-3 floppy/tape, 4-7 hard disk */
    BOOL rtFmt;		/* TRUE if this device is RT-11 formatted */
    int trackOffset;	/* number of tracks offset from beginning
			 * of physical device */
    int nTracks;	/* length of this device, in tracks;
			 * if 0, the rest of the disk */
    int devType;	/* SS_1D_8, SS_2D_8, DS_1D_8, DS_2D_8, or HD_1 */

    {
    FAST DISK_REC *pDiskRec;
    FAST SMS_DEV *pSmsDv;
    FAST DRIVE *pDrive = pDrives [drive];

    if (smsDrvNum <= 0)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    /* if this is first device on the specified drive,
     * allocate and initialize drive descriptor
     */

    if (pDrive == NULL)
	{
	pDrives [drive] = pDrive = (DRIVE *) malloc (sizeof (DRIVE));

	if (pDrive == NULL)
	    {
	    printErr ("smsDrv: can't allocate drive descriptor.\n");
	    return (ERROR);
	    }

	pDrive->drive     = drive;
	pDrive->activeDev = NULL;
	pDrive->devs      = NULL;
	pDrive->wdog      = wdCreate ();
	}

    /* allocate and initialize device descriptor */

    pSmsDv = (SMS_DEV *) malloc (sizeof (SMS_DEV));

    if (pSmsDv == NULL)
	{
	printErr ("smsDrv: can't allocate device descriptor.\n");
	return (ERROR);
	}

    switch (devType)
	{
	case SS_1D_8:
	    pSmsDv->diskRec = &diskRec [0];
	    break;

	case SS_2D_8:
	    pSmsDv->diskRec = &diskRec [1];
	    break;

	case DS_1D_8:
	    pSmsDv->diskRec = &diskRec [2];
	    break;
	    
	case DS_2D_8:
	    pSmsDv->diskRec = &diskRec [3];
	    break;

	case HD_1:
	    pSmsDv->diskRec = &diskRec [4];
	    break;

	default:
	    free ((char *)pSmsDv);
	    return (ERROR);
	}

    pDiskRec = pSmsDv->diskRec;

    if (nTracks == 0)
	nTracks = pDiskRec->tracksPerDisk - trackOffset;

    if (rt11DevInit ((RT_VOL_DESC *) pSmsDv, pDiskRec->bytesPerSector,
		   pDiskRec->sectorsPerTrack,
		   nTracks * pDiskRec->sectorsPerTrack, rtFmt,
		   pDiskRec->nFiles, smsRdSec, smsWrtSec, smsReset) == ERROR)

	{
	free ((char *) pSmsDv);
	printErr ("smsDrv: can't rt11DevInit device.\n");
	return (ERROR);
	}

    pSmsDv->drive	= drive;
    pSmsDv->trackOffset = trackOffset;

    /* Insert the device at the head of the drive's device chain */

    pSmsDv->next = pDrive->devs;
    pDrive->devs = pSmsDv;

    if (smsInitialize (pSmsDv, devType) == ERROR)
	{
	free ((char *) pSmsDv);
	printErr ("smsDrv: can't initialize device.\n");
	return (ERROR);
	}

    if (iosDevAdd ((DEV_HDR *) pSmsDv, name, smsDrvNum) == ERROR)
	{
	free ((char *) pSmsDv);
	printErr ("smsDrv: can't add device.\n");
	return (ERROR);
	}

    return (OK);
    }
/*******************************************************************************
*
* smsDbg - turn debugging output on/off
*/

VOID smsDbg (new_debug)
    BOOL new_debug;	/* new debug state */

    {
    smsDebug = new_debug;
    }
/*******************************************************************************
*
* smsRomboot - hook for setting ROMBOOT mode for driver
*
* This routines sets a flag in the driver that it is executing in ROMBOOT mode.
* It should only be called when this driver is used in a boot configuration
* where error messages about missing devices, etc. should be ignored.
*/

VOID smsRomboot ()

    {
    smsRom = TRUE;
    }

/*
routines supplied to ioLib:
	smsCreate,
	smsDelete,
	smsOpen,
	smsClose,
	smsRead,
	smsWrite,
	smsIoctl,
*/

/*******************************************************************************
*
* smsCreate - create an RT-11 file
*
* RETURNS: file descriptor number, or ERROR
*/

LOCAL int smsCreate (pSmsDv, name, mode)
    SMS_DEV *pSmsDv;	/* pointer to SMS device descriptor */
    char *name;		/* name of file to open */
    int mode;		/* access mode */

    {
    RT_FILE_DESC *pFd = rt11Create ((RT_VOL_DESC *) pSmsDv, name, mode);

    if (pFd == NULL)
	return (ERROR);

    return ((int) pFd);
    }
/*******************************************************************************
*
* smsDelete - delete an RT-11 file
*
* RETURNS OK | ERROR
*/

LOCAL STATUS smsDelete (pSmsDv, name)
    SMS_DEV *pSmsDv;	/* pointer to SMS device descriptor */
    char *name;		/* name of file to open */

    {
    return (rt11Delete ((RT_VOL_DESC *) pSmsDv, name));
    }
/*******************************************************************************
*
* smsOpen - open an RT-11 file
*
* RETURNS: file descriptor number, or ERROR
*/

LOCAL int smsOpen (pSmsDv, name, mode)
    SMS_DEV *pSmsDv;	/* pointer to SMS device descriptor */
    char *name;		/* name of file to open */
    int mode;		/* access mode */

    {
    RT_FILE_DESC *pFd = rt11Open ((RT_VOL_DESC *) pSmsDv, name, mode);

    if (pFd == NULL)
	return (ERROR);

    return ((int) pFd);
    }
/*******************************************************************************
*
* smsClose - close an RT-11 file
*/

LOCAL STATUS smsClose (pFd)
    RT_FILE_DESC *pFd;		/* file descriptor of file to close */

    {
    return (rt11Close (pFd));
    }
/*******************************************************************************
*
* smsRead - read from an RT-11 file
*
* RETURNS: number of bytes read, or 0 if end of file, or ERROR if read error.
*/

LOCAL int smsRead (pFd, buffer, maxbytes)
    RT_FILE_DESC *pFd;	/* file descriptor of file to close */
    char *buffer;	/* buffer to receive data */
    int maxbytes;	/* max bytes to read in to buffer */

    {
    return (rt11Read (pFd, buffer, maxbytes));
    }
/*******************************************************************************
*
* smsWrite - write to an RT-11 file
*
* RETURNS: number of bytes written, or ERROR
*/

LOCAL int smsWrite (pFd, buffer, nbytes)
    RT_FILE_DESC *pFd;	/* file descriptor of file to close */
    char *buffer;	/* buffer to be written */
    int nbytes;		/* number of bytes to write from buffer */

    {
    return (rt11Write (pFd, buffer, nbytes));
    }
/*******************************************************************************
*
* smsIoctl - do device specific control function
*/

LOCAL STATUS smsIoctl (pFd, function, arg)
    RT_FILE_DESC *pFd;	/* file descriptor of file to control */
    int function;	/* function code */
    int	arg;		/* some argument */

    {
    STATUS status;
    SMS_DEV *pSmsDv = (SMS_DEV *) pFd->rfd_vdptr;

    switch (function)
	{
	case FIODISKFORMAT:

	    /* format the disk;
	     * readyChange is called after formatting to indicate that
	     * any previous volume information is now invalid;
	     * drive is reset before and after formatting to clear any errors */

	    smsReset (pSmsDv);		/* to clear any errors */

	    status = smsFormat (pSmsDv);
	    smsReadyChange (pDrives[pSmsDv->drive]);

	    if (status != OK)
		smsReset (pSmsDv);	/* to clear any errors */

	    return (status);

	default:
	    return (rt11Ioctl (pFd, function, arg));
	}
    }

/*
routines supplied to rtLib:
	smsReset,
	smsRdSec,
	smsWrtSec
*/

/*******************************************************************************
*
* smsReset - reset drive to known state
*
* This routine is supplied to rtLib as the routine to call to reset
* a drive.  It is called when a volume is to be initialized after
* either an error or a ready change.
*
* This routine checks if a disk is present and if not returns an error.
* If a disk is present, then it is checked for write protection.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS smsReset (pSmsDv)
    FAST SMS_DEV *pSmsDv;	/* pointer to device descriptor */

    {
    char buffer [RT_BYTES_PER_BLOCK];	/* or some big amount */
    STATUS status;
    DRIVE_STATUS driveStatus;		/* read status into here */
    DRIVE *pDrive = pDrives[pSmsDv->drive];

    /* Force resetting of error status by doing a bogus smsRdSec.
     * This is done because the board does not indicate when a disk
     * goes offline.  It is done here so that the user code
     * sms{Open,Read..etc.} does not have to deal with error's
     * if the disk is not formatted.  Error messages are turned
     * of by using "reset" operation.
     */

    if (smsRdSecSup (pSmsDv, 0, buffer, TRUE) == OK)
	{
	/* check for write protection */

	smsWrtSecSup (pSmsDv, 0, buffer, TRUE);
	}

    smsTakeDrive (pSmsDv);

    status = smsGetStatus (pSmsDv->drive, &driveStatus);

    smsReleaseDrive (pSmsDv);

    if (status != OK ||
	(driveStatus.status0 != 0 && driveStatus.status2 != 0))
	{
	errnoSet (S_ioLib_DEVICE_ERROR);
	return (ERROR);
	}

    if (smsDebug)
	printErr ("smsDrv: status1 = 0x%x\n", driveStatus.status1);

    switch (driveStatus.status1)
	{
	case 0:
	    /* disk ok & writable */
	    smsModeChange (pDrive, UPDATE);
	    break;

	case STS1_ILLEGAL_SECTOR_SIZE:
	case STS1_DIAG_FAULT:
	case STS1_NO_INDEX:
	case STS1_INVALID_FUNCTION:
	case STS1_SECTOR_NOT_FOUND:
	case STS1_INVALID_ADDRESS:
	    /* probably unformatted, but can't complain if opening raw disk */
	    break;

	case STS1_DISK_WRITE_PROTECTED:
	    /* disk ok & not writable */
	    smsModeChange (pDrive, READ);
	    break;

	case STS1_UNIT_NOT_READY:
	    /* no disk */
	    errnoSet (S_ioLib_DISK_NOT_PRESENT);

	    if (!smsRom)
		{
		/* this always happens in ROMBOOT code with the
		 * hard disk, in which case we ignore the error */
		return (ERROR);
		}
	    break;
	}

    return (OK);
    }
/*******************************************************************************
*
* smsRdSec - read a sector from an RT-11 volume
*
* This routine reads the specified physical sector from the specified volume.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS smsRdSec (pSmsDv, secNum, buffer)
    FAST SMS_DEV *pSmsDv;	/* pointer to device descriptor */
    int secNum;			/* number of logical sector to read */
    char *buffer;		/* buffer to receive block */

    {
    /* call support routine to do actual work */

    return (smsRdSecSup (pSmsDv, secNum, buffer, FALSE));
    }
/*******************************************************************************
*
* smsRdSecSup - support routine to read a sector from an RT-11 volume
*
* This routine does the actual read of the specified physical sector
* from the specified volume.  It allows certain checks to be turned
* off when it is called directly by the reset routine.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS smsRdSecSup (pSmsDv, secNum, buffer, reset)
    FAST SMS_DEV *pSmsDv;	/* pointer to device descriptor */
    int secNum;			/* number of logical sector to read */
    char *buffer;		/* buffer to receive block */
    BOOL reset;			/* TRUE if invoked for "reset" operation */

    {
    FAST DISK_REC *pDiskRec = pSmsDv->diskRec;
    int secPerTrack = pDiskRec->sectorsPerTrack;
    int track;			/* which track sector will be found in */
    int physSector;		/* sector within track */
    STATUS status;

    /* calculate location of sector */

    track	= (secNum / secPerTrack) + pSmsDv->trackOffset;
    physSector	= secNum % secPerTrack;		/* assume winch at first */

    if (pDiskRec->deviceType == SMS_DEV_FLOPPY)
	physSector++;				/* bump secnum for floppies */

    /* make the drive active, possibly for "reset" operation */

    smsTakeDriveSup (pSmsDv, reset);

    /* read the sector */

    smsIopbInit (pSmsDv->drive, SMS_CMD_READ_DATA, pDiskRec->deviceType,
		 buffer, pDiskRec->bytesPerSector,
		 track / pDiskRec->numHeads,
		 track % pDiskRec->numHeads,
		 physSector);

    if ((status = smsSendCommand (pSmsDv->drive)) != OK)
	smsHandleErr (pSmsDv->drive, "read error");

    smsReleaseDrive (pSmsDv);

    return (status);
    }
/*******************************************************************************
*
* smsWrtSec - write a sector to an RT-11 volume
*
* This routine writes the specified sector to the specified volume.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS smsWrtSec (pSmsDv, secNum, buffer)
    SMS_DEV *pSmsDv;	/* pointer to device descriptor */
    int secNum;		/* number of logical sector to write */
    char *buffer;	/* buffer to write to block */

    {
    return (smsWrtSecSup (pSmsDv, secNum, buffer, FALSE));
    }
/*******************************************************************************
*
* smsWrtSecSup - support routine to write a sector to an RT-11 volume
*
* This routine does the actual write of the specified physical sector
* to the specified volume.  It allows certain checks to be turned
* off when it is called directly by the reset routine.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS smsWrtSecSup (pSmsDv, secNum, buffer, reset)
    FAST SMS_DEV *pSmsDv;	/* pointer to device descriptor */
    int secNum;			/* number of logical sector to write */
    char *buffer;		/* buffer to write to block */
    BOOL reset;			/* TRUE if for "reset" operation */

    {
    FAST DISK_REC *pDiskRec = pSmsDv->diskRec;
    FAST int secPerTrack = pDiskRec->sectorsPerTrack;
    int track;			/* which track sector will be found in */
    int physSector;		/* sector within track */
    STATUS status;

    /* calculate location of sector */

    track	= (secNum / secPerTrack) + pSmsDv->trackOffset;
    physSector	= secNum % secPerTrack;		/* assume winch at first */

    if (pDiskRec->deviceType == SMS_DEV_FLOPPY)
	physSector++;				/* bump secnum for floppies */

    /* make the drive active, possibly for "reset" operation */

    smsTakeDriveSup (pSmsDv, reset);

    /* write the sector */

    smsIopbInit (pSmsDv->drive, SMS_CMD_WRITE_DATA, pDiskRec->deviceType,
		 buffer, pDiskRec->bytesPerSector,
		 track / pDiskRec->numHeads,
		 track % pDiskRec->numHeads, physSector);

    if ((status = smsSendCommand (pSmsDv->drive)) != OK)
	smsHandleErr (pSmsDv->drive, "write error");

    smsReleaseDrive (pSmsDv);

    return (status);
    }

/*******************************************************************************
*
* smsInitialize - initialize device
*/

LOCAL smsInitialize  (pSmsDv, devType)
    FAST SMS_DEV *pSmsDv;	/* pointer to device descriptor */
    int devType;

    {
    IOPB_EXT_INIT init;		/* initialize command's iopb extension */
    FAST DISK_REC *pDiskRec = pSmsDv->diskRec;
    STATUS status = OK;

    init.numCylinders      = pDiskRec->tracksPerDisk / pDiskRec->numHeads;
    binvert ((char *) &init.numCylinders,
	     MEMBER_SIZE (IOPB_EXT_INIT, numCylinders));	/* Intel crap */

    if (devType == HD_1)	/* XXX should cover ALL hard disk types */
	{
	init.winchHeads	   = pDiskRec->numHeads;
	init.floppySides   = 0;
	}
    else
	{
	init.winchHeads	   = 0;
	init.floppySides   = pDiskRec->numHeads;
	}

    init.sectorsPerTrack   = pDiskRec->sectorsPerTrack;
    init.bytesPerSectorLSB = pDiskRec->bytesPerSector & 0xff;
    init.bytesPerSectorMSB = (pDiskRec->bytesPerSector & 0xff00) >> 8;
    init.encoding          = pDiskRec->encoding;

    /* make device active, use "reset" operation if in ROMBOOT mode */

    smsTakeDriveSup (pSmsDv, smsRom);

    /* Send init command.
     * Note: controller returns error on initialization if no disk is in!!
     * If this is the cause of the "error", then ignore it, otherwise
     * report error. */

    smsIopbInit (pSmsDv->drive, SMS_CMD_INITIALIZE, pDiskRec->deviceType,
		 (char *) &init, 0, 0, 0, 0);

    if (smsSendCommand (pSmsDv->drive) != OK)
	{
	if (errnoGet () != S_ioLib_DISK_NOT_PRESENT)
	    {
	    /* may set S_ioLib_DISK_NOT_PRESENT */

	    smsHandleErr (pSmsDv->drive, "initialization error");

	    if (errnoGet () != S_ioLib_DISK_NOT_PRESENT)
		status = ERROR;
	    }
	}

    smsReleaseDrive (pSmsDv);

    return (status);
    }
/*******************************************************************************
*
* smsHrdInit - initialize SMS board hardware
*/

LOCAL VOID smsHrdInit ()

    {
    WUB *pWub = (WUB *) WAKE_UP_BLOCK;	/* pointer to Wake-up Block */
    BOOL orig24bit = sms24bit; 		/* keep proper mode */
    UTINY clear = SMS_PIO_CLEAR;	/* for fooling 68000 into not issuing
					** a CLR.B, which reads!!! */

    sms24bit = FALSE;			/* fakeout segAddFromPtr */

    segAddFromPtr ((char *)&ccb, &pWub->ccb);	/* point at CCB */

    /* set up Channel Control Block */

    ccb.ctrlWord = 1;			/* must be 1 */
    ccb.busy = SMS_BUSY;		/* set the busy flag, to be cleared by
					 * board when it's thru downloading
					 * pointers after first SMS_PIO_START */

    segAddFromPtr ((char *)&cib.reserved3, &ccb.cib);/* point at CIB */

    /* set up Controller Invocation Block */

    cib.operationStatus = 0;		/* no status as yet */
    cib.statusSemaphore = 0;		/* 8007 may write status */

    segAddFromPtr ((char *)&iopb, &cib.iopb);	/* point at iopb */

    sms24bit = orig24bit;		/* restore correct mode */

    *IO_SMS_8007 = SMS_PIO_RESET;	/* reset controller hardware */

    vxTdelay (1);			/* wait 15ms minimum (actually 16.6) */

    *IO_SMS_8007 = clear;		/* remove hardware reset, enabling
					** board to recognize SMS_PIO_START */

    vxTdelay (1);			/* wait 15ms minimum (actually 16.6) */

    *IO_SMS_8007 = SMS_PIO_START;	/* 1st START after RESET makes board
					** download pointers from WUB & CCB */

    while (ccb.busy == SMS_BUSY)
	vxTdelay (1);			/* wait for it to finish */
    }
/*******************************************************************************
*
* smsFormat - format a disk
*
* This routine formats the disk on which the device resides, from the
* device's track offset to the end of the disk.
*
* NOTE: all board access is locked-out during formatting.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS smsFormat (pSmsDv)
    FAST SMS_DEV *pSmsDv;		/* pointer to device descriptor */

    {
    STATUS status = OK;
    IOPB_EXT_FORMAT extension;		/* build iopb extension here */
    FAST DISK_REC *pDiskRec = pSmsDv->diskRec;
    FAST int track;

    /* set up iopb and extension */

    extension.trackType    = SMS_TRACK_NORMAL;
    extension.userPattern1 = 'Z';
    extension.userPattern2 = 'Z';
    extension.userPattern3 = 'Z';
    extension.userPattern4 = 'Z';
    extension.interleave   = 3;

    if (pDiskRec->deviceType == SMS_DEV_WINCHESTER && FALSE)
	{
	extension.userPattern1 = 'Z';
	extension.userPattern2 = 0xe5;
	extension.userPattern3 = 0;
	extension.userPattern4 = 0xff;
        extension.interleave   = 10;
	}

    smsTakeDrive (pSmsDv);

    /* format each track */

    for (track = pSmsDv->trackOffset;
	 track < pDiskRec->tracksPerDisk;
	 ++track)
	{
	smsIopbInit (pSmsDv->drive, SMS_CMD_FORMAT, pDiskRec->deviceType,
		     (char *) &extension, 0,
		     track / pDiskRec->numHeads,
		     track % pDiskRec->numHeads,
		     (pDiskRec->deviceType == SMS_DEV_FLOPPY) ? 1 : 0);

	if (smsSendCommand (pSmsDv->drive) != OK)
	    {
	    smsHandleErr (pSmsDv->drive, "format error, track = %d", track);
	    status = ERROR;
	    break;		/* exit loop */
	    }
	}

    smsReleaseDrive (pSmsDv);
    return (status);
    }

/*******************************************************************************
*
* smsSendCommand - issue command to sms8007
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS smsSendCommand (driveNum)
    int driveNum;

    {
    int startTime;		/* when command was sent - for debugging */
    int numTicks;		/* ticks that command took - for debugging */
    FAST DRIVE *pDrive = pDrives[driveNum];

    pDrive->intSemId = semCreate ();

    if (smsDebug)
	{
	smsDebugOut ();
	startTime = tickGet ();
	}

    ccb.busy = SMS_BUSY;		/* set the busy flag */

    *IO_SMS_8007 = SMS_PIO_START;	/* send command */

    pDrive->timedOut = FALSE;
    wdStart (pDrive->wdog, TIMEOUT, smsTimeout, (int) pDrive);

    semTake (pDrive->intSemId);		/* await command completion interrupt */

    if (smsDebug)
	{
	numTicks = tickGet () - startTime;
	printErr ("smsDrv: %d ticks\n", numTicks);
	if ((numTicks > maxTicks) && (numTicks < TIMEOUT))
	    maxTicks = numTicks;
	}

    if (pDrive->timedOut)		/* semaphore released by watchdog? */
	{
	errnoSet (S_ioLib_DEVICE_TIMEOUT);
	return (ERROR);
	}

    if (pDrive->intStatus & OS_HARD_ERROR)		/* anything nasty? */
	{
	errnoSet (S_ioLib_DEVICE_ERROR);
	return (ERROR);
	}
    else if (smsDebug)
	printErr ("smsDrv: status = 0x%x\n", pDrive->intStatus);

    return (OK);
    }
/*******************************************************************************
*
* smsIntlvl - interrupt level routine for disk handler
*/

LOCAL VOID smsIntlvl ()

    {
    UTINY clear = SMS_PIO_CLEAR;	/* for fooling 68000 into not issuing
					** a CLR.B, which reads!!! */
    FAST DRIVE *pDrive;
    FAST int intStatus = cib.operationStatus;
    FAST int drive = (intStatus & OS_UNIT_MASK) >> 4;

    /* figure out which drive caused interrupt */

    if (iopb.function == SMS_CMD_XFER_STATUS && xferDriveNum != NONE)
	drive = xferDriveNum;
    else if ((intStatus & OS_FLOPPY_TAPE) == 0)
	drive |= DRVNUM_WINCHESTER;

    if (smsDebug)
	logMsg ("smsDrv: interrupt: 0x%x  drive=%d\n", intStatus, drive);

    *IO_SMS_8007 = clear;		/* remove hardware interrupt */
    cib.statusSemaphore = 0;		/* clear semaphore */

    if ((pDrive = pDrives [drive]) == NULL)
	return;

    pDrive->intStatus = intStatus;	/* save status in drive descriptor */

    switch (intStatus & OS_STATUS_MASK)
	{
	case OS_WINCH_MEDIA_CHANGE:
	case OS_FLOP_MEDIA_CHANGE:
	case OS_TAPE_MEDIA_CHANGE:
	    smsReadyChange (pDrive);
	    if (smsDebug)
		logMsg ("smsDrv: MEDIA_CHANGE\n");
	    break;

	case OS_WINCH_OP_COMPLETE:
	case OS_FLOP_TAPE_OP_COMPLETE:
	    wdCancel (pDrive->wdog);
	    semGive (pDrive->intSemId);		/* resume task level */
	    if (smsDebug)
		logMsg ("smsDrv: OP_COMPLETE\n");
	    break;

	case OS_WINCH_SEEK_COMPLETE:
	case OS_FLOP_SEEK_COMPLETE:
	case OS_TAPE_OTHER_COMPLETE:
	    if (smsDebug)
		logMsg ("smsDrv: SEEK_COMPLETE\n");

	    /* drop thru */

	default:
	    wdCancel (pDrive->wdog);
	    semGive (pDrive->intSemId);		/* resume task level */
	    break;
	}
    }
/*******************************************************************************
*
* smsTimeout - handle watchdog timeout
*/

LOCAL VOID smsTimeout (pDrive)
    FAST DRIVE *pDrive;

    {
    pDrive->timedOut = TRUE;
    semGive (pDrive->intSemId);
    }
/*******************************************************************************
*
* smsDebugOut - print info about command being sent
*/

LOCAL VOID smsDebugOut ()

    {
    IOPB_EXT_INIT *pInit = (IOPB_EXT_INIT *) segAddToPtr (&iopb.pDataBuffer);

    switch (iopb.function)
	{
	case SMS_CMD_INITIALIZE:
	    printErr ("\nfunction:INIT    numCylinders=%d    winchHeads=%d\n",
		      (pInit->numCylinders >> 8), pInit->winchHeads);
	    printErr ("    floppySides=%d, sectorsPerTrack=%d\n",
		      pInit->floppySides, pInit->sectorsPerTrack);
	    printErr ("    bytesPerSector=%d, encoding=0x%x\n",
		      pInit->bytesPerSectorLSB + (pInit->bytesPerSectorMSB <<8),
		      pInit->encoding);
	    break;

	case SMS_CMD_XFER_STATUS:
	    printErr ("\nfunction: XFER_STATUS\n");
	    break;

	case SMS_CMD_FORMAT:
	    printErr ("\nfunction: FORMAT\n");
	    break;

	case SMS_CMD_READ_DATA:
	    printErr ("\nfunction: READ_DATA\n");
	    break;

	case SMS_CMD_WRITE_DATA:
	    printErr ("\nfunction: WRITE_DATA\n");
	    break;

	default:
	    printErr ("\nfunction: UNKNOWN\n");
	}

    printErr ("    deviceType=%d, unit=%d, modifier=0x%x\n",
	      iopb.deviceType, iopb.unit, iopb.modifier);
    printErr ("    cyl=%d, head=%d, sector=%d, pDataBuffer=0x%08x\n",
	      (iopb.cylinder >> 8), iopb.head, iopb.sector, iopb.pDataBuffer);
    }
/*******************************************************************************
*
* smsTakeDrive - make a device be active on its drive
*/

LOCAL VOID smsTakeDrive (pSmsDv)
    FAST SMS_DEV *pSmsDv;

    {
    smsTakeDriveSup (pSmsDv, FALSE);
    }
/*******************************************************************************
*
* smsTakeDriveSup - support routine to make a device be active on its drive
*/

LOCAL VOID smsTakeDriveSup (pSmsDv, reset)
    FAST SMS_DEV *pSmsDv;
    BOOL reset;			/* TRUE if for "reset" operation */

    {
    semTake (smsSemId);		/* let no one else talk to SMS board */

    pDrives[pSmsDv->drive]->activeDev = pSmsDv;
    pDrives[pSmsDv->drive]->opReset   = reset;
    }
/*******************************************************************************
*
* smsReleaseDrive - make a drive be inactive
*/

LOCAL VOID smsReleaseDrive (pSmsDv)
    FAST SMS_DEV *pSmsDv;

    {
    pDrives[pSmsDv->drive]->opReset   = FALSE;
    pDrives[pSmsDv->drive]->activeDev = NULL;
    semGive (smsSemId);		/* let others use SMS board */
    }
/*******************************************************************************
*
* smsReadyChange - mark previous volume information as invalid
*/

LOCAL VOID smsReadyChange (pDrive)
    FAST DRIVE *pDrive;

    {
    FAST SMS_DEV *pSmsDv;	/* active device on this drive */

    for (pSmsDv = pDrive->devs; pSmsDv != NULL; pSmsDv = pSmsDv->next)
	rt11ReadyChange (&pSmsDv->rtvol);
    }
/*******************************************************************************
*
* smsModeChange - change READ/WRITE/UPDATE mode of device
*/

LOCAL VOID smsModeChange (pDrive, newMode)
    FAST DRIVE *pDrive;
    int newMode;

    {
    FAST SMS_DEV *pSmsDv;	/* active device on this drive */

    for (pSmsDv = pDrive->devs; pSmsDv != NULL; pSmsDv = pSmsDv->next)
	rt11ModeChange (&pSmsDv->rtvol, newMode);
    }

/*******************************************************************************
*
* smsIopbInit - initialize iopb
*
* Note: smsTakeDrive() must already have been called to lock/semaphore
* access to board and iobp structure.
*/

LOCAL VOID smsIopbInit (driveNum, function, deviceType, buffer, nbytes,
			cyl, head, sector)
    int driveNum;
    int function;
    UTINY deviceType;
    char *buffer;
    int nbytes;
    int cyl;
    int head;
    int sector;

    {
    iopb.zero       = 0;
    iopb.test       = 0;
    iopb.modifier   = sms24bit ? MOD_24_BIT_ADDRESS_MODE : 0;
    iopb.deviceType = deviceType;
    iopb.unit       = driveNum & DRVNUM_UNIT_MASK;
    iopb.function   = function;

    iopb.cylinder   = cyl;
    binvert ((char *) &iopb.cylinder, MEMBER_SIZE (IOPB, cylinder));

    iopb.head	    = head;
    iopb.sector     = sector;

    segAddFromPtr (buffer, &iopb.pDataBuffer);

    iopb.reqXferCnt = nbytes;
    binvert ((char *) &iopb.reqXferCnt, MEMBER_SIZE (IOPB, reqXferCnt));
    }
/*******************************************************************************
*
* segAddFromPtr - convert an ordinary pointer into a segment address
*/

LOCAL VOID segAddFromPtr (ptr, pSegAdd)
    char *ptr;		/* pointer to be converted into a segment address */
    FAST SEG_ADD *pSegAdd;	/* put result here */

    {
    if (sms24bit)
	{
	bcopy ((char *) &ptr, (char *) pSegAdd, sizeof (char *));
	binvert ((char *) pSegAdd, sizeof (char *));
	}
    else
	{
	FAST UTINY *saByte = (UTINY *) &ptr;

	pSegAdd->offsetLsb  = saByte[3] & 0x0f;	/* lsb of offset */
	pSegAdd->offsetMsb  = 0;		/* msb of offset */
	pSegAdd->segmentLsb = ((saByte[2] & 0x0f) << 4) +
			      ((saByte[3] & 0xf0) >> 4);
	pSegAdd->segmentMsb = ((saByte[1] & 0x0f) << 4) +
			      ((saByte[2] & 0xf0) >> 4);
	}
    }
/*******************************************************************************
*
* segAddToPtr - convert a segment address into an ordinary pointer
*/

LOCAL char *segAddToPtr (pSegAdd)
    FAST SEG_ADD *pSegAdd;	/* segment address to be converted */

    {
    char *ptr;

    if (sms24bit)
	{
	bcopy ((char *) pSegAdd, (char *) &ptr, sizeof (char *));
	binvert ((char *) &ptr, sizeof (char *));
	}
    else
	ptr = ((char *)(pSegAdd->offsetLsb +
		       (pSegAdd->offsetMsb << 8) +
		       (pSegAdd->segmentLsb << 4) +
		       (pSegAdd->segmentMsb << 12)));
    return (ptr);
    }
/*******************************************************************************
*
* smsGetStatus - get drive status
*
* NOTE: smsTakeDrive() must already have been called to lock/semaphore
*       access to board and iobp structure.
*
* INTERNAL
* When the transfer status command is issued, the resulting
* interrupt doesn't set the operation status byte correctly.
* The interrupt routine uses this to figure out the drive that
* the interrupt pertains to, so this is annoying.  A kludge
* around this is to set `xferDriveNum' to be the drive number
* that the next transfer status interrupt belongs to.
*/

LOCAL STATUS smsGetStatus (driveNum, pDriveStatus)
    int driveNum;
    DRIVE_STATUS *pDriveStatus;	/* where to return status */

    {
    STATUS status;

    smsIopbInit (driveNum, SMS_CMD_XFER_STATUS, 0, (char *) pDriveStatus,
		 0, 0, 0, 0);

    /* smsIntlvl cannot figure out the drive # because cib.operationStatus
     * it not set (ask Intel?).  Although this is a kludge, it should
     * be ok because board access has been interlocked with smsTakeDrive()
     * and any interrupts, other than transfer status, will be handled
     * properly by smsIntlvl.
     */

    xferDriveNum = driveNum;

    status = smsSendCommand (driveNum);

    xferDriveNum = NONE;	/* set to invalid state */

    return (status);
    }
/*******************************************************************************
*
* smsHandleErr - handle disk error
*
* This routine prints a disk error msg and disk status, unless there
* is no disk present in which case the error msg is suppressed and the
* task status is just set instead.
*
* NOTE: smsTakeDrive() must already have been called to lock/semaphore
*       access to board and iobp structure.
*
* VARARGS2
*/

LOCAL VOID smsHandleErr (driveNum, errMsg, errArg1, errArg2, errArg3)
    int driveNum;
    char *errMsg;
    int errArg1;
    int errArg2;
    int errArg3;

    {
    DRIVE_STATUS driveStatus;		/* read status into here */
    FAST int ix;
    DRIVE *pDrive = pDrives[driveNum];
    STATUS status = smsGetStatus (driveNum, &driveStatus);

    if (status == OK &&
	driveStatus.status0 == 0		    &&
	driveStatus.status1 == STS1_UNIT_NOT_READY  &&
	driveStatus.status2 == 0)
	{
	errnoSet (S_ioLib_DISK_NOT_PRESENT);
	}
    else
	{
	if (pDrive->opReset)
	    return;	/* don't do anything if "reset" operation */

	printErr ("smsDrv: ");
	printErr (errMsg, errArg1, errArg2, errArg3);

	if (pDrive->timedOut)
	    printErr (": timeout (");
	else
	    printErr (": status = 0x%02x (", pDrive->intStatus);

	if (status != OK)
	    printErr ("can't read status");
	else
	    {
	    for (ix = 0; ix < STATUS_BUF_LENGTH; ++ix)
		printErr ("%02x ", (((char *) &driveStatus) [ix]) & 0xff);
	    }

	printErr (")\n");
	}
    }
