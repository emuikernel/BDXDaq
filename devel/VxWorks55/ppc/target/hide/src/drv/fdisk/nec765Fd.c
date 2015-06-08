/* nec765Fd.c - NEC 765 floppy disk device driver */

/* Copyright 1989-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01i,01aug01,jkf  doubled sync semaphore timeout per T3 SPR#63543.
01o,11jun01,jyo  SPR#65990, write protected media handling.
01i,24feb98,hdn  added support for disk-change in fdStatusChk().
01h,30aug95,hdn  added support for write-protected, unformatted, no-disk.
01g,14jun95,hdn  removed function declarations defined in sysLib.h.
01f,24jan95,jdi  doc cleanup.
01e,25oct94,hdn  added fdRawio() to provide physical IO.
		 swapped 1st and 2nd parameter of fdDevCreate().
01d,03jun94,hdn  changed DESCRIPTION, 386 to 386/486.
		 added an explanation of parameters for fdDevCreate().
01c,22apr94,hdn  made two imported globals sysFdBuf, sysFdBufSize.
01b,04nov93,hdn  cleaned up.
01a,21sep93,hdn  written.
*/

/*
DESCRIPTION
This is the driver for the NEC 765 Floppy Chip used on the PC 386/486.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  However, two routines must be called directly:  fdDrv() to
initialize the driver, and fdDevCreate() to create devices.
Before the driver can be used, it must be initialized by calling fdDrv().
This routine should be called exactly once, before any reads, writes, or
calls to fdDevCreate().  Normally, it is called from usrRoot() in
usrConfig.c.

The routine fdRawio() allows physical I/O access.  Its first argument is a
drive number, 0 to 3; the second argument is a type of diskette; the third
argument is a pointer to the FD_RAW structure, which is defined in nec765Fd.h.

Interleaving is not supported when the driver formats.

Two types of diskettes are currently supported:
3.5" 2HD 1.44MB and 5.25" 2HD 1.2MB.  You can add additional diskette
types to the `fdTypes[]' table in sysLib.c.

The BLK_DEV bd_mode field will reflect the disk's write protect tab.

INTERNAL
The driver uses memory area at sysFdBuf(FD_DMA_BUF) size of sysFdBufSize
(FD_DMA_BUF_SIZE) for DMA. Reasons are First of all, the DMA chip 
understand only 24 bits address. Secondly, a buffer should be in one page,
the other word a buffer can not cross the 64k byte boundary. 

SEE ALSO:
.pG "I/O System"
*/

#include "vxWorks.h"
#include "taskLib.h"
#include "blkIo.h"
#include "ioLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "stdlib.h"
#include "errnoLib.h"
#include "stdio.h"
#include "string.h"
#include "private/semLibP.h"
#include "intLib.h"
#include "iv.h"
#include "wdLib.h"
#include "sysLib.h"
#include "sys/fcntlcom.h"
#include "drv/fdisk/nec765Fd.h"


IMPORT int dmaSetup (int direction, void *pAddr, UINT nbytes, UINT chan);
IMPORT FD_TYPE fdTypes[];
IMPORT UINT sysFdBuf;
IMPORT UINT sysFdBufSize;


/* global */

int  fdIntCount	= 0;		/* interrupt count */
int  fdRetry	= 2;		/* max retry count */
int  fdTimeout	= 10000;	/* max timeout count */
int  fdSemSec	= 2;		/* semaphore timeout seconds */
int  fdWdSec	= 4;		/* watchdog timeout seconds */
SEMAPHORE  fdSyncSem;		/* binary semaphore for syncronization */
SEMAPHORE  fdMuteSem;		/* mutex  semaphore for mutual-exclusion */
WDOG_ID  fdWid;			/* watchdog to turn motor off */


/* local */

LOCAL char fdDORvalues[]	= {0x1c, 0x2d, 0x4e, 0x8f};
LOCAL char fdAccess[]		= {0,0,0,0};	/* flag indicate first access */
LOCAL BOOL fdDrvInstalled	= FALSE;	/* TRUE = facility installed */
LOCAL int  fdCylinder		= 1;		/* last cylinder read/written */


/* function prototypes */

LOCAL STATUS fdBlkRd	(FD_DEV *pFdDev, int startBlk, int nBlks, char *pBuf);
LOCAL STATUS fdBlkWrt	(FD_DEV *pFdDev, int startBlk, int nBlks, char *pBuf);
LOCAL STATUS fdIoctl	(FD_DEV *pFdDev, int function, int arg);
LOCAL STATUS fdReset	(FD_DEV *pFdDev);
LOCAL STATUS fdStatusChk(FD_DEV *pFdDev);
LOCAL STATUS fdBlkRW    (FD_DEV *pFdDev, int startBlk, int nBlks, char *pBuf,
			 int direction);
LOCAL void   fdIntr         (int ctrl);
LOCAL STATUS fdCmdSend      (UCHAR *pCommand, int nBytes);
LOCAL STATUS fdResultPhase  (UCHAR *pResults, BOOL immediate, int nBytes);
LOCAL STATUS fdIntSense     (int seekEnd);
LOCAL void   fdInit	    (void);
LOCAL void   fdDriveRelease (void);
LOCAL void   fdDriveSelect  (int fdType, int drive);
LOCAL STATUS fdRecalib      (int drive);
LOCAL STATUS fdSeek         (int drive, int cylinder, int head);
LOCAL STATUS fdRW           (int fdType, int drive, int cylinder, int head,
			     int sector, void *pBuf, int nSecs, int direction);
LOCAL STATUS fdFormat       (int fdType, int drive, int cylinder, int head,
    			     int interleave);
LOCAL int    fdDriveIsWP    (int drive);



/*******************************************************************************
*
* fdDrv - initialize the floppy disk driver
*
* This routine initializes the floppy driver, sets up interrupt vectors,
* and performs hardware initialization of the floppy chip.
*
* This routine should be called exactly once, before any reads, writes,
* or calls to fdDevCreate().  Normally, it is called by usrRoot()
* in usrConfig.c.
*
* RETURNS: OK.
*
* SEE ALSO: fdDevCreate(), fdRawio()
*/

STATUS fdDrv
    (
    int vector,		/* interrupt vector */
    int level		/* interrupt level */
    )
    {
    if (!fdDrvInstalled)
	{
        semBInit (&fdSyncSem, SEM_Q_FIFO, SEM_EMPTY);

        semMInit (&fdMuteSem, SEM_Q_PRIORITY | SEM_DELETE_SAFE |
		  SEM_INVERSION_SAFE);

        (void) intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (vector),
		           (VOIDFUNCPTR)fdIntr, 0);

        sysIntEnablePIC (level);	/* unmask the interrupt level */

        fdWid = wdCreate ();

        fdInit ();

	fdDrvInstalled = TRUE;
	}

    return (OK);
    }

/*******************************************************************************
*
* fdDevCreate - create a device for a floppy disk
*
* This routine creates a device for a specified floppy disk.
*
* The <drive> parameter is the drive number of the floppy disk;
* valid values are 0 to 3.
*
* The <fdType> parameter specifies the type of diskette, which is described
* in the structure table `fdTypes[]' in sysLib.c.  <fdType> is an index to
* the table.  Currently the table contains two diskette types:
* .iP "" 4
* An <fdType> of 0 indicates the first entry in the table (3.5" 2HD, 1.44MB);
* .iP
* An <fdType> of 1 indicates the second entry in the table (5.25" 2HD, 1.2MB).
* .LP
* Members of the `fdTypes[]' structure are:
* .CS
*     int  sectors;       /@ no of sectors @/
*     int  sectorsTrack;  /@ sectors per track @/
*     int  heads;         /@ no of heads @/
*     int  cylinders;     /@ no of cylinders @/
*     int  secSize;       /@ bytes per sector, 128 << secSize @/
*     char gap1;          /@ gap1 size for read, write @/
*     char gap2;          /@ gap2 size for format @/
*     char dataRate;      /@ data transfer rate @/
*     char stepRate;      /@ stepping rate @/
*     char headUnload;    /@ head unload time @/
*     char headLoad;      /@ head load time @/
*     char mfm;           /@ MFM bit for read, write, format @/
*     char sk;            /@ SK bit for read @/
*     char *name;         /@ name @/
* .CE
*
* The <nBlocks> parameter specifies the size of the device, in blocks.
* If <nBlocks> is zero, the whole disk is used.
*
* The <blkOffset> parameter specifies an offset, in blocks, from the start
* of the device to be used when writing or reading the floppy disk.  This
* offset is added to the block numbers passed by the file system during
* disk accesses.  (VxWorks file systems always use block numbers beginning
* at zero for the start of a device.)  Normally, <blkOffset> is 0.
*
*
* RETURNS:
* A pointer to a block device structure (BLK_DEV) or NULL if memory cannot
* be allocated for the device structure.
*
* SEE ALSO: fdDrv(), fdRawio(), dosFsMkfs(), dosFsDevInit(), rt11FsDevInit(),
* rt11FsMkfs(), rawFsDevInit()
*/

BLK_DEV *fdDevCreate
    (
    int drive,		/* driver number of floppy disk (0 - 3) */
    int fdType,		/* type of floppy disk */
    int nBlocks,	/* device size in blocks (0 = whole disk) */
    int blkOffset	/* offset from start of device */
    )
    {
    FD_DEV *pFdDev;
    BLK_DEV *pBlkDev;
    FD_TYPE *pType = &fdTypes[fdType];

    if (!fdDrvInstalled)
	return (NULL);

    if (nBlocks == 0)
	nBlocks = pType->sectors;

    if ((UINT)drive >= FD_MAX_DRIVES)
	return (NULL);

    if ((pFdDev = (FD_DEV *)calloc(sizeof (FD_DEV), 1)) == NULL)
	return (NULL);

    pBlkDev = &pFdDev->blkDev;

    pBlkDev->bd_nBlocks		= nBlocks;
    pBlkDev->bd_bytesPerBlk	= 128 << pType->secSize;
    pBlkDev->bd_blksPerTrack	= pType->sectorsTrack;
    pBlkDev->bd_nHeads		= pType->heads;
    pBlkDev->bd_removable	= TRUE;
    pBlkDev->bd_retry		= 1;
    pBlkDev->bd_readyChanged	= TRUE;
    pBlkDev->bd_blkRd		= fdBlkRd;
    pBlkDev->bd_blkWrt		= fdBlkWrt;
    pBlkDev->bd_ioctl		= fdIoctl;
    pBlkDev->bd_reset		= fdReset;
    pBlkDev->bd_statusChk	= fdStatusChk;

    pFdDev->fdType		= fdType;
    pFdDev->drive		= drive;
    pFdDev->blkOffset		= blkOffset;

    /* SPR#65990, now check for the WP setting */

    /* power up the drive */

    sysOutByte (FD_REG_OUTPUT, fdDORvalues[drive]); 
    sysDelay ();

    /* set the bd_mode per the WP tab */

    pBlkDev->bd_mode		= (fdDriveIsWP(drive)) ? O_RDONLY : O_RDWR;

    /* power down the device */

    fdDriveRelease (); 
    sysDelay();

    return (&pFdDev->blkDev);
    }

/*******************************************************************************
*
* fdRawio - provide raw I/O access
*
* This routine is called when the raw I/O access is necessary.
*
* The <drive> parameter is the drive number of the floppy disk;
* valid values are 0 to 3.
*
* The <fdType> parameter specifies the type of diskette, which is described
* in the structure table `fdTypes[]' in sysLib.c.  <fdType> is an index to
* the table.  Currently the table contains two diskette types:
* .iP "" 4
* An <fdType> of 0 indicates the first entry in the table (3.5" 2HD, 1.44MB);
* .iP
* An <fdType> of 1 indicates the second entry in the table (5.25" 2HD, 1.2MB).
* .LP
*
* The <pFdRaw> is a pointer to the structure FD_RAW, defined in nec765Fd.h
*
* RETURNS:  OK or ERROR.
*
* SEE ALSO: fdDrv(), fdDevCreate()
*/

STATUS fdRawio
    (
    int drive,		/* drive number of floppy disk (0 - 3) */
    int fdType,		/* type of floppy disk */
    FD_RAW *pFdRaw	/* pointer to FD_RAW structure */
    )
    {
    FD_DEV fdDev;
    BLK_DEV *pBlkDev	= &fdDev.blkDev;
    FD_TYPE *pType	= &fdTypes[fdType];
    UINT startBlk;

    if (!fdDrvInstalled)
	return (ERROR);

    if ((UINT)drive >= FD_MAX_DRIVES)
	return (ERROR);

    if ((pFdRaw->cylinder	>= pType->cylinders)	||
        (pFdRaw->head		>= pType->heads)	||
        (pFdRaw->sector		>  pType->sectorsTrack)	||
        (pFdRaw->sector		== 0))
	return (ERROR);

    pBlkDev->bd_nBlocks		= pType->sectors;
    pBlkDev->bd_bytesPerBlk	= 128 << pType->secSize;
    pBlkDev->bd_blksPerTrack	= pType->sectorsTrack;
    pBlkDev->bd_nHeads		= pType->heads;
    pBlkDev->bd_removable	= TRUE;
    pBlkDev->bd_retry		= 1;
    pBlkDev->bd_readyChanged	= TRUE;
    pBlkDev->bd_blkRd		= fdBlkRd;
    pBlkDev->bd_blkWrt		= fdBlkWrt;
    pBlkDev->bd_ioctl		= fdIoctl;
    pBlkDev->bd_reset		= fdReset;
    pBlkDev->bd_statusChk	= fdStatusChk;

    fdDev.drive			= drive;
    fdDev.fdType		= fdType;
    fdDev.blkOffset		= 0;

    /* SPR#65990, now check for the WP setting */

    /* power up the drive */

    sysOutByte (FD_REG_OUTPUT, fdDORvalues[drive]); 
    sysDelay ();

    /* set the bd_mode per the WP tab */

    pBlkDev->bd_mode		= (fdDriveIsWP(drive)) ? O_RDONLY : O_RDWR;

    startBlk = pFdRaw->cylinder * (pType->sectorsTrack * pType->heads) +
	       pFdRaw->head * pType->sectorsTrack +
	       pFdRaw->sector - 1;

    return (fdBlkRW (&fdDev, startBlk, pFdRaw->nSecs, pFdRaw->pBuf,
		     pFdRaw->direction));
    }
/*******************************************************************************
*
* fdBlkRd - read one or more blocks from a floppy disk
*
* This routine reads one or more blocks from the specified device,
* starting with the specified block number.
*
* If any block offset was specified during fdDevCreate(), it is added
* to <startBlk> before the transfer takes place.
*
* RETURNS: OK, ERROR if the read command didn't succeed.
*/

LOCAL STATUS fdBlkRd
    (
    FD_DEV *pFdDev,
    int startBlk,
    int nBlks,
    char *pBuf
    )
    {
    return (fdBlkRW (pFdDev, startBlk, nBlks, pBuf, O_RDONLY));
    }

/*******************************************************************************
*
* fdBlkWrt - write one or more blocks to a floppy disk
*
* This routine writes one or more blocks to the specified device,
* starting with the specified block number.
*
* If any block offset was specified during fdDevCreate(), it is added
* to <startBlk> before the transfer takes place.
*
* RETURNS: OK, ERROR if the write command didn't succeed.
*/

LOCAL STATUS fdBlkWrt
    (
    FD_DEV *pFdDev,
    int startBlk,
    int nBlks,
    char *pBuf
    )
    {
    return (fdBlkRW (pFdDev, startBlk, nBlks, pBuf, O_WRONLY));
    }

/*******************************************************************************
*
* fdReset - reset a floppy disk controller
*
* This routine resets a floppy disk controller.
*
* RETURNS: OK, always.
*/

LOCAL STATUS fdReset
    (
    FD_DEV *pFdDev
    )
    {
    
    fdInit ();

    fdDriveSelect (pFdDev->fdType, pFdDev->drive);
    (void) fdRecalib (pFdDev->drive);
    fdDriveRelease ();

    return (OK);
    }

/*******************************************************************************
*
* fdIoctl - do device specific control function
*
* This routine is called when the file system cannot handle an ioctl()
* function.
*
* RETURNS:  OK or ERROR.
*/

LOCAL STATUS fdIoctl
    (
    FD_DEV *pFdDev,
    int function,
    int arg
    )
    {
    FD_TYPE *pType = &fdTypes[pFdDev->fdType];
    FAST int status = ERROR;
    FAST int cylinder;
    FAST int head;
    int retryCount;

    wdCancel (fdWid);
    semTake (&fdMuteSem, WAIT_FOREVER);

    switch (function)
	{
	case FIODISKFORMAT:
            fdDriveSelect (pFdDev->fdType, pFdDev->drive);
	    (void) fdRecalib (pFdDev->drive);

	    for (cylinder = 0; cylinder < pType->cylinders; cylinder++)
	        for (head = 0; head < pType->heads; head++)
		    {
		    retryCount = 0;
		    while (fdSeek(pFdDev->drive, cylinder, head) != OK)
	    	        if (++retryCount > fdRetry)
			    {
	    		    (void) errnoSet (S_ioLib_DEVICE_ERROR);
			    goto doneIoctl;
			    }
		    retryCount = 0;
		    while (fdFormat(pFdDev->fdType, pFdDev->drive, 
			   cylinder, head, arg) != OK)
	    	        if (++retryCount > fdRetry)
			    {
	    		    (void) errnoSet (S_ioLib_DEVICE_ERROR);
			    goto doneIoctl;
			    }
		    }
	    status = OK;
	    break;

	default:
	    (void) errnoSet (S_ioLib_UNKNOWN_REQUEST);
	}

doneIoctl:
    semGive (&fdMuteSem);
    wdStart (fdWid, (sysClkRateGet() * fdWdSec), (FUNCPTR)fdDriveRelease, 0);
    return (status);
    }

/*******************************************************************************
*
* fdStatusChk - check a status of a floppy disk
*
* This routine checks for a disk change on devices.
*
* RETURNS: OK.
*/

LOCAL STATUS fdStatusChk
    (
    FD_DEV *pFdDev
    )
    {
    FD_TYPE *pType = &fdTypes[pFdDev->fdType];

    wdCancel (fdWid);
    semTake (&fdMuteSem, WAIT_FOREVER);

    sysOutByte (FD_REG_OUTPUT, fdDORvalues[pFdDev->drive]); 
    sysDelay ();

    /* 
     * Bit 7 is set when a diskette is changed.
     * To clear the bit, we need to perform a seek.  
     */

    if (sysInByte (FD_REG_INPUT) & 0x80)
	{
	pFdDev->blkDev.bd_readyChanged	= TRUE;

	/* do seek to clear DCHG bit in FD_REG_INPUT */

	if (++fdCylinder >= pType->cylinders)
	    fdCylinder = 1;

        fdDriveSelect (pFdDev->fdType, pFdDev->drive);
	(void) fdSeek(pFdDev->drive, fdCylinder, 0);

        /* set the bd_mode per the WP tab */

        pFdDev->blkDev.bd_mode =
			(fdDriveIsWP(pFdDev->drive))? O_RDONLY : O_RDWR;
	}
    
    semGive (&fdMuteSem);
    wdStart (fdWid, (sysClkRateGet() * fdWdSec), 
		(FUNCPTR)fdDriveRelease, 0);
    return (OK);
    }

/*******************************************************************************
*
* fdBlkRW - read or write sectors to a floppy disk.
*
* Read or write sectors to a floppy disk.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS fdBlkRW
    (
    FD_DEV *pFdDev,
    int startBlk,
    int nBlks,
    char *pBuf,
    int direction
    )
    {
    BLK_DEV *pBlkDev	= &pFdDev->blkDev;
    FD_TYPE *pType	= &fdTypes[pFdDev->fdType];
    int status		= ERROR;
    int rwStatus;
    int head;
    int cylinder;
    int sector;
    int ix;
    int nSecs;
    int retryRW0;
    int retryRW1;
    int retrySeek;

    wdCancel (fdWid);
    semTake (&fdMuteSem, WAIT_FOREVER);

    startBlk += pFdDev->blkOffset;

    /* recalibrate is necessary before the first access */

    fdDriveSelect (pFdDev->fdType, pFdDev->drive);

    if (fdAccess[pFdDev->drive] == 0)
	{
	(void) fdRecalib (pFdDev->drive);
        fdAccess[pFdDev->drive] = 1;
	}

    for (ix = 0; ix < nBlks; ix += nSecs)
	{
	cylinder = startBlk / (pType->sectorsTrack * pType->heads);
	sector   = startBlk % (pType->sectorsTrack * pType->heads);
	head     = sector / pType->sectorsTrack;
	sector   = sector % pType->sectorsTrack + 1;

	fdCylinder = cylinder;		/* remember it for fdStatusChk() */

	retrySeek = 0;
	while (fdSeek(pFdDev->drive, cylinder, head) != OK)
	    if (++retrySeek > fdRetry)
		goto doneRW;
	
	nSecs = min (nBlks - ix, pType->sectorsTrack - sector + 1);
        while ((pBlkDev->bd_bytesPerBlk * nSecs) > sysFdBufSize)
	    nSecs -= 1;

	retryRW1 = 0;
	retryRW0 = 0;
	while ((rwStatus = fdRW (pFdDev->fdType, pFdDev->drive, cylinder, head, 
				 sector, pBuf, nSecs, direction)) != OK)
	    {
	    if ((rwStatus == FD_UNFORMATED) ||
	    	(rwStatus == FD_WRITE_PROTECTED) ||
	    	(rwStatus == FD_DISK_NOT_PRESENT))
		retryRW0 = fdRetry;

	    if (++retryRW0 > fdRetry)
		{
	        (void) fdRecalib (pFdDev->drive);
	        if (++retryRW1 > fdRetry)
		    goto doneRW;
		retrySeek = 0;
		while (fdSeek(pFdDev->drive, cylinder, head) != OK)
	    	    if (++retrySeek > fdRetry)
			goto doneRW;
		retryRW0 = 0;
		}
	    }

        startBlk += nSecs;
        pBuf	 += pBlkDev->bd_bytesPerBlk * nSecs;
	}

    status = OK;

doneRW:
    if (rwStatus == FD_UNFORMATED)
        (void)errnoSet (S_ioLib_UNFORMATED);
    else if (rwStatus == FD_WRITE_PROTECTED)
	{
        pBlkDev->bd_mode = O_RDONLY;
        (void)errnoSet (S_ioLib_WRITE_PROTECTED);
	}
    else if (rwStatus == FD_DISK_NOT_PRESENT)
        (void)errnoSet (S_ioLib_DISK_NOT_PRESENT);
    else if (rwStatus == ERROR)
        (void)errnoSet (S_ioLib_DEVICE_ERROR);

    semGive (&fdMuteSem);
    wdStart (fdWid, (sysClkRateGet() * fdWdSec), (FUNCPTR)fdDriveRelease, 0);
    return (status);
    }

/*******************************************************************************
*
* fdInit - init a floppy disk controller
*
* This routine initializes a floppy disk controller.
*
* RETURNS: N/A
*/

LOCAL void fdInit (void)
    {
    int ix;

    /* reset the chip */

    sysOutByte (FD_REG_OUTPUT, (FD_DOR_RESET | FD_DOR_DMA_DISABLE));
    taskDelay (sysClkRateGet() >> 1);

    sysOutByte (FD_REG_OUTPUT, (FD_DOR_CLEAR_RESET | FD_DOR_DMA_ENABLE));
    taskDelay (sysClkRateGet() >> 1);
    
    sysOutByte (FD_REG_CONFIG, 0);

    if (semTake (&fdSyncSem, sysClkRateGet() * fdSemSec) != OK)
	return;

    for (ix = 0; ix < FD_MAX_DRIVES; ix++)
	(void) fdIntSense (0);

#ifdef	FD_DEBUG
    printErr ("fdInit\n");
#endif	/* FD_DEBUG */
    }

/*******************************************************************************
*
* fdIntr - Floppy controller interrupt handler.
*
* RETURNS: N/A
*/

LOCAL void fdIntr
    (
    int ctrl
    )
    {
    fdIntCount++;	/* XXX */
    semGive (&fdSyncSem);
    }

/*******************************************************************************
*
* fdDriveSelect - select and turn on the specified drive.
*
* Select and turn on the specified drive.
*
* RETURNS: N/A
*/

LOCAL void fdDriveSelect
    (
    int fdType,
    int drive
    )
    {
    FD_TYPE *pType = &fdTypes[fdType];
    UCHAR command[12];

    /* turn on the motor */

    sysOutByte (FD_REG_OUTPUT, fdDORvalues[drive]);
    sysDelay ();

    /* set data rate */

    sysOutByte (FD_REG_CONFIG, pType->dataRate);
    sysDelay ();

    command[0] = FD_CMD_SPECIFY;
    command[1] = (pType->stepRate << 4) | pType->headUnload;
    command[2] = pType->headLoad << 1;

    fdCmdSend (command, FD_CMD_LEN_SPECIFY);
    sysDelay ();

#ifdef	FD_DEBUG
    printErr ("fdDriveSelect\n");
#endif	/* FD_DEBUG */
    }

/*******************************************************************************
*
* fdDriveRelease - release and turn off the specified drive.
*
* Release and turn off the specified drive.
*
* RETURNS: N/A
*/

LOCAL void fdDriveRelease (void)
    {

    /* turn off the motor */

    sysOutByte (FD_REG_OUTPUT, (FD_DOR_CLEAR_RESET | FD_DOR_DMA_ENABLE));
    sysDelay ();
    }

/*******************************************************************************
*
* fdIntSense - get information concerning the last drive interrupt
*
* Get information concerning the last drive interrupt
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS fdIntSense
    (
    int seekEnd
    )
    {
    UCHAR rValue;
    UCHAR command[12];
    UCHAR results[12];

    command[0] = FD_CMD_SENSEINT;

#ifdef	FD_DEBUG
    {
    int ix;

    printErr ("fdIntSense: ");
    for (ix = 0; ix < FD_CMD_LEN_SENSEINT; ix++)
        printErr ("0x%x ", command[ix]);
    printErr (" intCnt=%d\n", fdIntCount);
    }
#endif	/* FD_DEBUG */

    fdCmdSend (command, FD_CMD_LEN_SENSEINT);

    rValue = fdResultPhase (results, TRUE, 2);

    if ((rValue == 0) && (seekEnd == 0) &&
	(((results[0] & 0xc0) == 0x00) || ((results[0] & 0xc0) == 0xc0)))
	return (OK);
    else if ((rValue == 0) && (seekEnd == 1) && ((results[0] & 0xe0) == 0x20))
	return (OK);

#ifdef	FD_DEBUG
    printErr ("fdIntSense: rValue=0x%x r0=0x%x 0x%x\n",
              rValue, results[0], results[1]);
#endif	/* FD_DEBUG */

    return (ERROR);
    }

/*******************************************************************************
*
* fdRecalib - recalibrate the drive
*
* Recalibrate the drive
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS fdRecalib
    (
    int drive
    )
    {
    int ix;
    UCHAR rValue[2];
    UCHAR command[12];

    command[0] = FD_CMD_RECALIBRATE;
    command[1] = drive & 0x03;

#ifdef	FD_DEBUG
    {
    printErr ("fdRecalib: ");
    for (ix = 0; ix < FD_CMD_LEN_RECALIBRATE; ix++)
        printErr ("0x%x ", command[ix]);
    printErr (" intCnt=%d\n", fdIntCount);
    }
#endif	/* FD_DEBUG */

    for (ix = 0; ix < 2; ix++)
	{
        fdCmdSend (command, FD_CMD_LEN_RECALIBRATE);

        if (semTake (&fdSyncSem, sysClkRateGet() * fdSemSec) != OK)
	    return (ERROR);

        rValue[ix] = fdIntSense (1);
	}

    if ((rValue[0] == OK) && (rValue[1] == OK))
	return (OK);

#ifdef	FD_DEBUG
    printErr ("fdRecalib: rValue=0x%x 0x%x\n", rValue[0], rValue[1]);
#endif	/* FD_DEBUG */

    return (ERROR);
    }

/*******************************************************************************
*
* fdSeek - seek the drive heads to the specified cylinder
*
* Seek the drive heads to the specified cylinder
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS fdSeek
    (
    int drive,
    int cylinder,
    int head
    )
    {
    UCHAR rValue;
    UCHAR command[12];

    command[0] = FD_CMD_SEEK;
    command[1] = (head << 2) | (drive & 0x03);
    command[2] = cylinder;

#ifdef	FD_DEBUG
    {
    int ix;

    printErr ("fdSeek: ");
    for (ix = 0; ix < FD_CMD_LEN_SEEK; ix++)
        printErr ("0x%x ", command[ix]);
    printErr (" intCnt=%d\n", fdIntCount);
    }
#endif	/* FD_DEBUG */

    fdCmdSend (command, FD_CMD_LEN_SEEK);

    if (semTake (&fdSyncSem, sysClkRateGet() * fdSemSec) != OK)
	return (ERROR);

    rValue = fdIntSense (1);

    if (rValue == OK)
	return (OK);

#ifdef	FD_DEBUG
    printErr ("fdSeek: rValue=0x%x\n", rValue);
#endif	/* FD_DEBUG */

    return (ERROR);
    }

/*******************************************************************************
*
* fdRW - read/write a number of sectors on the current track
*
* Read/write a number of sectors on the current track
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS fdRW
    (
    int fdType,
    int drive,
    int cylinder,
    int head,
    int sector,
    void *pBuf,
    int nSecs,
    int direction
    )
    {
    FD_TYPE *pType = &fdTypes[fdType];
    UCHAR rValue;
    char fd_mt = 0;
    unsigned nBytes = nSecs * (128 << pType->secSize);
    UCHAR command[12];
    UCHAR results[12];

    if (pType->heads > 1)
	fd_mt = 1;

    /* setup DMA controller */

    if (direction == O_RDONLY)
	{
        if (dmaSetup (O_RDONLY, (void *)sysFdBuf, nBytes, FD_DMA_CHAN) != OK)
	    return (ERROR);
        command[0] = FD_CMD_READ | (fd_mt << 7) | (pType->mfm << 6) |
			 (pType->sk << 5);
	}
    else
	{
        if (dmaSetup (O_WRONLY, (void *)sysFdBuf, nBytes, FD_DMA_CHAN) != OK)
	    return (ERROR);
        command[0] = FD_CMD_WRITE | (fd_mt << 7) | (pType->mfm << 6);
	bcopy ((char *)pBuf, (char *)sysFdBuf, nBytes);
	}

    command[1] = (head << 2) | (drive & 0x03);
    command[2] = cylinder;
    command[3] = head;
    command[4] = sector;
    command[5] = pType->secSize;
    command[6] = pType->sectorsTrack;
    command[7] = pType->gap1;
    command[8] = 0xff;

#ifdef	FD_DEBUG
    {
    int ix;

    printErr ("fdRW %c : ", (direction == O_RDONLY) ? 'R' : 'W');
    for (ix = 0; ix < FD_CMD_LEN_RW; ix++)
        printErr ("0x%x ", command[ix]);
    printErr (" intCnt=%d\n", fdIntCount);
    }
#endif	/* FD_DEBUG */

    fdCmdSend (command, FD_CMD_LEN_RW);

    if (semTake (&fdSyncSem, sysClkRateGet() * fdSemSec) != OK)
	return (FD_DISK_NOT_PRESENT);

    rValue = fdResultPhase (results, FALSE, 7);

    if (rValue == 0)
	{
	if ((results[0] & 0xc0) == 0x00)
	    {
            if (direction == O_RDONLY)
	        bcopy ((char *)sysFdBuf, (char *)pBuf, nBytes);
	    return (OK);
	    }
	else
	    {
	    if ((results[1] & 0x04) == 0x04)
		return (FD_UNFORMATED);
	    else if ((results[1] & 0x02) == 0x02)
		return (FD_WRITE_PROTECTED);
	    else
		return (ERROR);
	    }
	}

#ifdef	FD_DEBUG
    printErr ("fdRW %c : ", (direction == O_RDONLY) ? 'R' : 'W');
    printErr ("rValue=0x%x r0=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
               rValue, results[0], results[1], results[2],
               results[3], results[4], results[5], results[6]);
#endif	/* FD_DEBUG */

    return (ERROR);
    }

/*******************************************************************************
*
* fdFormat - format the current track
*
* format the current track
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS fdFormat
    (
    int fdType,
    int drive,
    int cylinder,
    int head,
    int interleave
    )
    {
    FD_TYPE *pType = &fdTypes[fdType];
    int ix;
    int sector;
    UCHAR rValue;
    UCHAR command[12];
    UCHAR results[12];
    char *pBuf = (char *)sysFdBuf;

    sector = 1;
    for (ix = 0; ix < pType->sectorsTrack; ix++)
	{
	*pBuf++ = (char)cylinder;
	*pBuf++ = (char)head;
	*pBuf++ = (char)sector;
	*pBuf++ = (char)pType->secSize;
	sector += 1;			/* XXX no interleave yet */
	}

    /* setup DMA controller */

    if (dmaSetup (O_WRONLY, (void *)sysFdBuf, pType->sectorsTrack * 4,
		  FD_DMA_CHAN) != OK)
	return (ERROR);

    command[0] = FD_CMD_FORMAT | (pType->mfm << 6);
    command[1] = (head << 2) | (drive & 0x03);
    command[2] = pType->secSize;
    command[3] = pType->sectorsTrack;
    command[4] = pType->gap2;
    command[5] = 0xff;

#ifdef	FD_DEBUG
    {
    printErr ("fdFormat: ");
    for (ix = 0; ix < FD_CMD_LEN_FORMAT; ix++)
        printErr ("0x%x ", command[ix]);
    printErr (" intCnt=%d\n", fdIntCount);
    }
#endif	/* FD_DEBUG */

    fdCmdSend (command, FD_CMD_LEN_FORMAT);

    if (semTake (&fdSyncSem, sysClkRateGet() * fdSemSec) != OK)
	return (ERROR);

    rValue = fdResultPhase (results, FALSE, 7);

    if ((rValue == 0) && ((results[0] & 0xc0) == 0x00))
	return (OK);

#ifdef	FD_DEBUG
    printErr ("fdFmt: rValue=0x%x r0=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
              rValue, results[0], results[1], results[2],
              results[3], results[4], results[5], results[6]);
#endif	/* FD_DEBUG */

    return (ERROR);
    }

/*******************************************************************************
*
* fdCmdSend - send commands to the chip
*
* Send commands to the chip
*
* RETURNS: 0, -1 or -2 if the command didn't succeed.
*/

LOCAL int fdCmdSend
    (
    UCHAR *pCommand,
    int nBytes
    )
    {
    int timeout;
    int ix;

    /* send the command to the FDC */
    for (ix = 0; ix < nBytes; ix++)
	{

	/* wait for the RQM flag */

	timeout = 0;
	while ((sysInByte (FD_REG_STATUS) & FD_MSR_RQM) != FD_MSR_RQM)
	    {
	    if (++timeout > fdTimeout)
		return (-1);
	    }
	
	/* check that the FDC is ready for input */

	if ((sysInByte (FD_REG_STATUS) & FD_MSR_DIRECTION) != 0)
	    return (-2);
	
	sysOutByte (FD_REG_DATA, (*pCommand++));
	sysDelay ();
	}
    
    return (0);
    }

/*******************************************************************************
*
* fdResultPhase - get results from the chip
*
* Get results from the chip
*
* RETURNS: 0, -1 -2 -3 -4 if the command didn't succeed.
*/

LOCAL int fdResultPhase
    (
    UCHAR *pResults,
    BOOL immediate,
    int nBytes
    )
    {
    int ix;
    int timeout;

    /* input results from the FDC */

    if (immediate || (sysInByte (FD_REG_STATUS) & FD_MSR_FD_BUSY) != 0)
	{

	/* operation is completed, get FDC result bytes */

	for (ix = 0; ix < nBytes; ix++)
	    {

	    /* wait for the RQM flag */

	    timeout = 0;
	    while ((sysInByte (FD_REG_STATUS) & FD_MSR_RQM) != FD_MSR_RQM)
	        {
	        if (++timeout > fdTimeout)
		    return (-1);
	        }
	
	    /* check that the FDC is ready for output */

	    if ((sysInByte (FD_REG_STATUS) & FD_MSR_DIRECTION) == 0)
	        return (-2);
	    
	    pResults[ix] = sysInByte (FD_REG_DATA);
	    sysDelay ();
	    }
	
	/* make sure that FDC has completed sending data */

	timeout = 0;
	while ((sysInByte (FD_REG_STATUS) & FD_MSR_FD_BUSY) != 0)
	    {
	    if (++timeout > fdTimeout)
	        return (-3);
	    }

#ifdef	FD_DEBUG
	{
	printErr ("fdResultPhase: ");
	for (ix = 0; ix < nBytes; ix++)
	    printErr ("0x%x ", pResults[ix]);
	printErr ("\n");
	}
#endif	/* FD_DEBUG */

	}
    else
	{

	/* catch all hidden interrupts */

	ix = 0;
	while (fdIntSense(0) != OK)
	    {
	    if (++ix > fdRetry)
		return (-4);
	    }
	}

    return (0);
    }

/*******************************************************************************
*
* fdDriveIsWP - determine if the diskette has write protect tab set.
*
* This function will use the mode sense command to check the status
* of the write protect line.  The drive should be powered on before
* calling this routine, else the data is not valid.
*
* RETURNS: TRUE if WP tab is enabled / FALSE if it isn't;
*/

LOCAL int fdDriveIsWP 
    (
    int drive		/* drive to check WP tab on */
    )
    {
    UCHAR command[2];   /* two byte array for the command phase */
    UCHAR result;	/* one byte for the result phase (st3) */

    /* setup sense drive command data */

    command[0] = FD_CMD_SENSEDRIVE;
    command[1] = drive & 0x03;  /* don't care about the head in this command */

    /* send sense drive status command, which is a two byte command */

    fdCmdSend (command, FD_CMD_LEN_SENSEDRIVE);

    /* get result of command (status reg three) and check WP bit is set */

    fdResultPhase(&result, FALSE, 1);

    /* Is bit 6 set in the result phase data? aka st3 */

    if ((result & 0x40) == 0x40)
	{
#ifdef	FD_DEBUG
        printErr ("fdDriveIsWP: write protect detected on.\n");
#endif	/* FD_DEBUG */
	return (TRUE); /* WP Tab is set */
	}
    else
	{
#ifdef	FD_DEBUG
        printErr ("fdDriveIsWP: write protect detected off.\n");
#endif	/* FD_DEBUG */
	return (FALSE); /* WP Tab is not set */
	}
    }
