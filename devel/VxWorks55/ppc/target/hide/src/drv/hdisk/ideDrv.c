/* ideDrv.c - IDE disk device driver */

/* Copyright 1989-1999 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01p,30jul99,jkf  fixed SPR 9729, retryRW condition loses data.
01o,11jan99,aeg  removed redundant semBInit () in ideInit ().
01n,03mar97,dat  fixed SPR 8084, previous edit needed tweeking.
01m,28feb97,dat  fixed SPR 8084, incorrect block number check
01l,17dec96,dat  fixed SPR 3273, incorrect block count
01k,14jun95,hdn  removed function declarations defined in sysLib.h.
01j,24jan95,jdi  doc cleanup.
01i,07dec94,rhp  man page cleanups to ideDrvCreate(), ideRawio() (SPR3734).
01h,25oct94,hdn  cleaned up ideRawio().
		 changed ideSemSec and ideWdSec back to 5.
01g,06oct94,hdn  changed ideSemSec to 10, ideWdSec to 10.
		 added ideRawio() to provide physical IO.
01f,03jun94,hdn  changed DESCRIPTION, 386 to 386/486.
		 added an explanation of parameters for ideDevCreate().
01e,18nov93,hdn  added watchdog to check ideInit() in ideDrv().
01d,11nov93,hdn  added ideWait() in end of each primitive functions.
		 implemented semaphore timeout and watchdog.
01c,10nov93,hdn  separated ideDiagnose() from ideInit().
01b,04nov93,hdn  cleaned up.
01a,18oct93,hdn  written.
*/

/*
DESCRIPTION
This is the driver for the IDE used on the PC 386/486.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  However, two routines must be called directly:  ideDrv() to
initialize the driver, and ideDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling ideDrv().
This routine should be called exactly once, before any reads, writes, or
calls to ideDevCreate().  Normally, it is called from usrRoot() in
usrConfig.c.

The routine ideRawio() provides physical I/O access.  Its first
argument is a drive number, 0 or 1; the second argument is a pointer
to an IDE_RAW structure.

NOTE
Format is not supported, because IDE disks are already formatted, and bad
sectors are mapped.

SEE ALSO:
.pG "I/O System"
*/

#include "vxWorks.h"
#include "taskLib.h"
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
#include "drv/hdisk/ideDrv.h"


/* imports */

IMPORT IDE_TYPE ideTypes[];


/* global */

BOOL ideDebug	 = FALSE;	/* debug flag */
BOOL ideDebugErr = FALSE;	/* debug flag */
int  ideIntCount = 0;		/* interrupt count */
int  ideStatus	 = 0;		/* status register in interrupt level */
int  ideDrives	 = 0;		/* number of IDE drives */
int  ideRetry	 = 3;		/* max retry count */
int  ideSemSec	 = 5;		/* timeout seconds for semaphore */
int  ideWdSec	 = 5;		/* timeout seconds for watchdog */
BOOL ideWaitForever = TRUE;	/* watchdog: wait forever in ideInit() */
WDOG_ID   ideWid;		/* watchdog */
SEMAPHORE ideSyncSem;		/* binary semaphore for syncronization */
SEMAPHORE ideMuteSem;		/* mutex  semaphore for mutual-exclusion */
IDE_PARAM ideParams[IDE_MAX_DRIVES];


/* local */

LOCAL BOOL ideDrvInstalled = FALSE;   /* TRUE = facility installed */


/* function prototypes */

      void   ideShow	(int drive);
LOCAL STATUS ideBlkRd	(IDE_DEV *pIdeDev, int startBlk, int nBlks, char *pBuf);
LOCAL STATUS ideBlkWrt	(IDE_DEV *pIdeDev, int startBlk, int nBlks, char *pBuf);
LOCAL STATUS ideReset	(IDE_DEV *pIdeDev);
LOCAL STATUS ideIoctl	(IDE_DEV *pIdeDev, int function, int arg);
LOCAL STATUS ideBlkRW	(IDE_DEV *pIdeDev, int startBlk, int nBlks, char *pBuf,
			 int direction);
LOCAL void   ideIntr	(int ctrl);
LOCAL void   ideWdog	(int ctrl);
LOCAL void   ideWait	(int request);
LOCAL void   ideInit	(void);
LOCAL STATUS ideDiagnose(void);
LOCAL STATUS idePinit	(int drive);
LOCAL STATUS idePread	(int drive, void *buffer);
LOCAL STATUS ideRecalib	(int drive);
LOCAL STATUS ideSeek	(int drive, int cylinder, int head);
LOCAL STATUS ideRW	(int drive, int cylinder, int head, int sector, 
	 		 void *pBuf, int nSecs, int direction);
LOCAL STATUS ideFormat	(int drive, int cylinder, int head, int interleave);


/*******************************************************************************
*
* ideDrv - initialize the IDE driver
*
* This routine initializes the IDE driver, sets up interrupt vectors,
* and performs hardware initialization of the IDE chip.
*
* This routine should be called exactly once, before any reads, writes,
* or calls to ideDevCreate().  Normally, it is called by usrRoot()
* in usrConfig.c.
*
* The ideDrv() call requires a configuration type, <manualConfig>.  If
* this argument is 1, the driver will initialize drive parameters; if
* the argument is 0, the driver will not initialize drive parameters.
*
* The drive parameters are the number of sectors per track, the number of
* heads, and the number of cylinders.  They are stored in the structure
* table `ideTypes[]' in sysLib.c.  The table has two entries:  the first is
* for drive 0; the second is for drive 1.  The table has two other members
* which are used by the driver: the number of bytes per sector and
* the precompensation cylinder.  These two members should be set properly.
* Definitions of the structure members are:
* .CS
*     int cylinders;              /@ number of cylinders @/
*     int heads;                  /@ number of heads @/
*     int sectorsTrack;           /@ number of sectors per track @/
*     int bytesSector;            /@ number of bytes per sector @/
*     int precomp;                /@ precompensation cylinder @/
* .CE
*
* RETURNS: OK, or ERROR if initialization fails.
*
* SEE ALSO: ideDevCreate()
*/

STATUS ideDrv
    (
    int vector,		/* interrupt vector */
    int level,		/* interrupt level */
    BOOL manualConfig	/* 1 = initialize drive parameters */
    )
    {
    int drive;
    IDE_PARAM *pParam;
    IDE_TYPE *pType;

    if (!ideDrvInstalled)
	{
        semBInit (&ideSyncSem, SEM_Q_FIFO, SEM_EMPTY);

        semMInit (&ideMuteSem, SEM_Q_PRIORITY | SEM_DELETE_SAFE |
		SEM_INVERSION_SAFE);

        (void) intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (vector),
		       (VOIDFUNCPTR)ideIntr, 0);

	ideWid = wdCreate ();

        sysIntEnablePIC (level);	/* unmask the interrupt level */

	wdStart (ideWid, (sysClkRateGet() * ideWdSec), (FUNCPTR)ideWdog, 0);
        ideInit ();
	wdCancel (ideWid);

        if (!ideWaitForever)
	    {
	    ideWaitForever = TRUE;
	    return (ERROR);
	    }

        if (ideDiagnose () != OK)
	    return (ERROR);

        for (drive = 0; drive < IDE_MAX_DRIVES; drive++)
	    {
	    pType = &ideTypes[drive];
	    pParam = &ideParams[drive];
	    if (pType->cylinders == 0)
	        break;
	    ideDrives += 1;
	    if (manualConfig)
	        {
	        (void) idePinit (drive);
                (void) idePread (drive, (char *)pParam);
	        }
	    else
	        {
                (void) idePread (drive, (char *)pParam);
	        pType->cylinders = pParam->cylinders - 1;
	        pType->heads = pParam->heads;
	        pType->sectorsTrack = pParam->sectorsTrack;
	        }
	    (void) ideRecalib (drive);
	    }

	ideDrvInstalled = TRUE;
	}

    return (OK);
    }

/*******************************************************************************
*
* ideDevCreate - create a device for a IDE disk
*
* This routine creates a device for a specified IDE disk.
*
* <drive> is a drive number for the hard drive: it must be 0 or 1.
*
* The <nBlocks> parameter specifies the size of the device, in blocks.
* If <nBlocks> is zero, the whole disk is used.
*
* The <blkOffset> parameter specifies an offset, in blocks, from the start
* of the device to be used when writing or reading the hard disk.  This
* offset is added to the block numbers passed by the file system during
* disk accesses.  (VxWorks file systems always use block numbers beginning
* at zero for the start of a device.)
*
*
* RETURNS:
* A pointer to a block device structure (BLK_DEV), or NULL if memory cannot
* be allocated for the device structure.
*
* SEE ALSO: dosFsMkfs(), dosFsDevInit(), rt11FsDevInit(), rt11FsMkfs(),
* rawFsDevInit()
*/

BLK_DEV *ideDevCreate
    (
    int drive,		/* drive number for hard drive (0 or 1) */
    int nBlocks,	/* device size in blocks (0 = whole disk) */
    int blkOffset	/* offset from start of device */
    )
    {
    IDE_DEV *pIdeDev;
    BLK_DEV *pBlkDev;
    IDE_TYPE *pType;

    if (!ideDrvInstalled)
	return (NULL);

    if ((UINT)drive > ideDrives)
	return (NULL);

    if ((pIdeDev = (IDE_DEV *)calloc(sizeof (IDE_DEV), 1)) == NULL)
	return (NULL);

    pType = &ideTypes[drive];
    pBlkDev = &pIdeDev->blkDev;

    if (nBlocks == 0)
	nBlocks = pType->cylinders * pType->heads * pType->sectorsTrack
		  - blkOffset;

    pBlkDev->bd_nBlocks		= nBlocks;
    pBlkDev->bd_bytesPerBlk	= pType->bytesSector;
    pBlkDev->bd_blksPerTrack	= pType->sectorsTrack;
    pBlkDev->bd_nHeads		= pType->heads;
    pBlkDev->bd_removable	= FALSE;
    pBlkDev->bd_retry		= 1;
    pBlkDev->bd_mode		= O_RDWR;
    pBlkDev->bd_readyChanged	= TRUE;
    pBlkDev->bd_blkRd		= ideBlkRd;
    pBlkDev->bd_blkWrt		= ideBlkWrt;
    pBlkDev->bd_ioctl		= ideIoctl;
    pBlkDev->bd_reset		= ideReset;
    pBlkDev->bd_statusChk	= NULL;

    pIdeDev->drive		= drive;
    pIdeDev->blkOffset		= blkOffset;

    return (&pIdeDev->blkDev);
    }

/*******************************************************************************
*
* ideShow - show IDE disk parameters
*
* Show IDE disk parameters
*
* RETURNS: N/A
*
* NOMANUAL
*/

void ideShow
    (
    int drive
    )
    {
    int ix;
    IDE_PARAM *pParam = &ideParams[drive];
    IDE_TYPE *pType = &ideTypes[drive];

    if ((UINT)drive > ideDrives)
	{
	printErr ("drive should be 0 - %d\n",ideDrives);
	return;
	}

    printf ("ideTypes    cylinders   =%d\n",	pType->cylinders);
    printf ("            heads       =%d\n",	pType->heads);
    printf ("            sectorsTrack=%d\n",	pType->sectorsTrack);
    printf ("            bytesSector =%d\n",	pType->bytesSector);
    printf ("            precomp     =0x%04x\n",pType->precomp);

    printf ("parameters  config      =0x%04x\n",(USHORT)pParam->config);
    printf ("            cylinders   =%d\n",	(USHORT)pParam->cylinders);
    printf ("            removcyl    =0x%04x\n",(USHORT)pParam->removcyl);
    printf ("            heads       =%d\n",	(USHORT)pParam->heads);	
    printf ("            bytesTrack  =0x%04x\n",(USHORT)pParam->bytesTrack);
    printf ("            bytesSec    =0x%04x\n",(USHORT)pParam->bytesSec);
    printf ("            sectorsTrack=%d\n",	(USHORT)pParam->sectorsTrack);
    printf ("            bytesGap    =0x%04x\n",(USHORT)pParam->bytesGap);
    printf ("            bytesSync   =0x%04x\n",(USHORT)pParam->bytesSync);
    printf ("            vendstat    =0x%04x\n",(USHORT)pParam->vendstat);
    printf ("            serial      =");
    for (ix = 0; ix < 10; ix++)
	{
        printf ("%c",			pParam->serial[ix * 2 + 1]);
        printf ("%c",			pParam->serial[ix * 2]);
	}
    printf ("\n");
    printf ("            type        =0x%04x\n",(USHORT)pParam->type);	
    printf ("            size        =0x%04x\n",(USHORT)pParam->size);
    printf ("            bytesEcc    =0x%04x\n",(USHORT)pParam->bytesEcc);
    printf ("            rev         =");
    for (ix = 0; ix < 4; ix++)
	{
        printf ("%c",			pParam->rev[ix * 2 + 1]);
        printf ("%c",			pParam->rev[ix * 2]);
	}
    printf ("\n");
    printf ("            model       =");
    for (ix = 0; ix < 20; ix++)
	{
        printf ("%c",			pParam->model[ix * 2 + 1]);
        printf ("%c",			pParam->model[ix * 2]);
	}
    printf ("\n");
    printf ("            nsecperint  =0x%04x\n",(USHORT)pParam->nsecperint);
    printf ("            usedmovsd   =0x%04x\n",(USHORT)pParam->usedmovsd);
    }

/*******************************************************************************
*
* ideRawio - provide raw I/O access
*
* This routine is called when the raw I/O access is necessary.
*
* <drive> is a drive number for the hard drive: it must be 0 or 1.
*
* The <pIdeRaw> is a pointer to the structure IDE_RAW which is defined in 
* ideDrv.h
*
* RETURNS: OK or ERROR.
*
*/

STATUS ideRawio
    (
    int        drive,	/* drive number for hard drive (0 or 1) */
    IDE_RAW *  pIdeRaw	/* pointer to IDE_RAW structure */
    )
    {
    IDE_DEV ideDev;
    BLK_DEV *pBlkDev	= &ideDev.blkDev;
    IDE_TYPE *pType	= &ideTypes[drive];
    UINT startBlk;

    if (!ideDrvInstalled)
	return (ERROR);

    if ((UINT)drive > ideDrives)
	return (ERROR);

    if ((pIdeRaw->cylinder	>= pType->cylinders)	||
        (pIdeRaw->head		>= pType->heads)	||
        (pIdeRaw->sector	>  pType->sectorsTrack)	||
        (pIdeRaw->sector	== 0))
	return (ERROR);

    pBlkDev->bd_nBlocks		= pType->cylinders * pType->heads * 
				  pType->sectorsTrack;
    pBlkDev->bd_bytesPerBlk	= pType->bytesSector;
    pBlkDev->bd_blksPerTrack	= pType->sectorsTrack;
    pBlkDev->bd_nHeads		= pType->heads;
    pBlkDev->bd_removable	= FALSE;
    pBlkDev->bd_retry		= 1;
    pBlkDev->bd_mode		= O_RDWR;
    pBlkDev->bd_readyChanged	= TRUE;
    pBlkDev->bd_blkRd		= ideBlkRd;
    pBlkDev->bd_blkWrt		= ideBlkWrt;
    pBlkDev->bd_ioctl		= ideIoctl;
    pBlkDev->bd_reset		= ideReset;
    pBlkDev->bd_statusChk	= NULL;

    ideDev.drive		= drive;
    ideDev.blkOffset		= 0;

    startBlk = pIdeRaw->cylinder * (pType->sectorsTrack * pType->heads) +
	       pIdeRaw->head * pType->sectorsTrack +
	       pIdeRaw->sector - 1;

    return (ideBlkRW (&ideDev, startBlk, pIdeRaw->nSecs, pIdeRaw->pBuf,
		     pIdeRaw->direction));
    }

/*******************************************************************************
*
* ideBlkRd - read one or more blocks from a IDE disk
*
* This routine reads one or more blocks from the specified device,
* starting with the specified block number.
*
* If any block offset was specified during ideDevCreate(), it is added
* to <startBlk> before the transfer takes place.
*
* RETURNS: OK, ERROR if the read command didn't succeed.
*/

LOCAL STATUS ideBlkRd
    (
    IDE_DEV *pIdeDev,
    int startBlk,
    int nBlks,
    char *pBuf
    )
    {
    return (ideBlkRW (pIdeDev, startBlk, nBlks, pBuf, O_RDONLY));
    }

/*******************************************************************************
*
* ideBlkWrt - write one or more blocks to a IDE disk
*
* This routine writes one or more blocks to the specified device,
* starting with the specified block number.
*
* If any block offset was specified during ideDevCreate(), it is added
* to <startBlk> before the transfer takes place.
*
* RETURNS: OK, ERROR if the write command didn't succeed.
*/

LOCAL STATUS ideBlkWrt
    (
    IDE_DEV *pIdeDev,
    int startBlk,
    int nBlks,
    char *pBuf
    )
    {
    return (ideBlkRW (pIdeDev, startBlk, nBlks, pBuf, O_WRONLY));
    }

/*******************************************************************************
*
* ideReset - reset a IDE disk controller
*
* This routine resets a IDE disk controller.
*
* RETURNS: OK, always.
*/

LOCAL STATUS ideReset
    (
    IDE_DEV *pIdeDev
    )
    {
    
    semTake (&ideMuteSem, WAIT_FOREVER);

    ideInit ();

    semGive (&ideMuteSem);

    return (OK);
    }

/*******************************************************************************
*
* ideIoctl - do device specific control function
*
* This routine is called when the file system cannot handle an ioctl()
* function.
*
* RETURNS:  OK or ERROR.
*/

LOCAL STATUS ideIoctl
    (
    IDE_DEV *pIdeDev,
    int function,
    int arg
    )
    {
    FAST int status = ERROR;
    FAST int cylinder;
    FAST int head;
    int retryCount;
    IDE_TYPE *pType = &ideTypes[pIdeDev->drive];

    semTake (&ideMuteSem, WAIT_FOREVER);

    switch (function)
	{
	case FIODISKFORMAT:
	    (void) ideRecalib (pIdeDev->drive);

	    for (cylinder = 0; cylinder < pType->cylinders; cylinder++)
	        for (head = 0; head < pType->heads; head++)
		    {
		    retryCount = 0;
		    while (ideSeek(pIdeDev->drive, cylinder, head) != OK)
	    	        if (++retryCount > ideRetry)
			    {
	    		    (void) errnoSet (S_ioLib_DEVICE_ERROR);
			    goto doneIoctl;
			    }
		    retryCount = 0;
		    while (ideFormat(pIdeDev->drive, cylinder, head, arg)
			   != OK)
	    	        if (++retryCount > ideRetry)
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
    semGive (&ideMuteSem);
    return (status);
    }

/*******************************************************************************
*
* ideBlkRW - read or write sectors to a IDE disk.
*
* Read or write sectors to a IDE disk.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ideBlkRW
    (
    IDE_DEV *pIdeDev,
    int startBlk,
    int nBlks,
    char *pBuf,
    int direction
    )
    {
    BLK_DEV *pBlkDev = &pIdeDev->blkDev;
    int cylinder;
    int head;
    int sector;
    int ix;
    int nSecs;
    int retryRW0 = 0;
    int retryRW1 = 0;
    int retrySeek = 0;
    int status = ERROR;
    IDE_TYPE *pType = &ideTypes[pIdeDev->drive];

    /* sanity check */

    nSecs =  pBlkDev->bd_nBlocks;
    if ((startBlk + nBlks) > nSecs)
	{
        if (ideDebugErr)
	    printErr ("startBlk=%d nBlks=%d: 0 - %d\n", startBlk, nBlks, nSecs);
	return (ERROR);
	}

    startBlk += pIdeDev->blkOffset;

    semTake (&ideMuteSem, WAIT_FOREVER);

    for (ix = 0; ix < nBlks; ix += nSecs)
	{
	cylinder = startBlk / (pType->sectorsTrack * pType->heads);
	sector   = startBlk % (pType->sectorsTrack * pType->heads);
	head     = sector / pType->sectorsTrack;
	sector   = sector % pType->sectorsTrack + 1;
	nSecs    = min (nBlks - ix, pType->sectorsTrack - sector + 1);

	retrySeek = 0;
	while (ideSeek(pIdeDev->drive, cylinder, head) != OK)
	    if (++retrySeek > ideRetry)
		goto done;
	
	retryRW1 = 0;
	retryRW0 = 0;
	while (ideRW (pIdeDev->drive, cylinder, head, sector, 
	  pBuf, nSecs, direction) != OK)
	    {
	    if (++retryRW0 > ideRetry)
		{
	        (void) ideRecalib (pIdeDev->drive);
	        if (++retryRW1 > ideRetry)
		    goto done;
	        retrySeek = 0;
	        while (ideSeek(pIdeDev->drive, cylinder, head) != OK)
	            if (++retrySeek > ideRetry)
		        goto done;
	        retryRW0 = 0;
		}
	    }

        startBlk += nSecs;
        pBuf += pBlkDev->bd_bytesPerBlk * nSecs;
	}

    status = OK;

done:
    if (status == ERROR)
        (void)errnoSet (S_ioLib_DEVICE_ERROR);
    semGive (&ideMuteSem);
    return (status);
    }

/*******************************************************************************
*
* ideIntr - IDE controller interrupt handler.
*
* RETURNS: N/A
*/

LOCAL void ideIntr
    (
    int ctrl
    )
    {
    ideIntCount++;	/* XXX */
    ideStatus = sysInByte (IDE_STATUS);
    semGive (&ideSyncSem);
    }

/*******************************************************************************
*
* ideIntr - IDE controller watchdog handler.
*
* RETURNS: N/A
*/

LOCAL void ideWdog
    (
    int ctrl
    )
    {
    ideWaitForever = FALSE;
    }

/*******************************************************************************
*
* ideWait - wait the drive ready
*
* Wait the drive ready
*
* RETURNS: OK, ERROR if the drive didn't become ready in certain period of time.
*/

LOCAL void ideWait
    (
    int request
    )
    {

    switch (request)
	{
	case STAT_READY:
	    wdStart (ideWid, (sysClkRateGet() * ideWdSec), (FUNCPTR)ideInit, 0);
            while (sysInByte (IDE_STATUS) & STAT_BUSY)
	        ;
            while ((sysInByte (IDE_STATUS) & STAT_READY) == 0)
	        ;
	    wdCancel (ideWid);
	    break;

	case STAT_DRQ:
            while ((sysInByte (IDE_STATUS) & STAT_DRQ) == 0)
	        ;
	    break;

	case STAT_SEEKCMPLT:
            while ((sysInByte (IDE_STATUS) & STAT_SEEKCMPLT) == 0)
	        ;
	    break;
	}

    if (ideDebug)
	printErr ("ideWait end: \n");
    }

/*******************************************************************************
*
* ideInit - init a IDE disk controller
*
* This routine initializes a IDE disk controller.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL void ideInit (void)
    {
    int ix;

    sysOutByte (IDE_D_CONTROL, CTL_RST | CTL_IDS);
    for (ix = 0; ix < 100; ix++)
        sysDelay ();

    sysOutByte (IDE_D_CONTROL, CTL_IDS);
    for (ix = 0; ix < 100; ix++)
        sysDelay ();
    while ((sysInByte (IDE_STATUS) & STAT_BUSY) && (ideWaitForever))
	;

    sysOutByte (IDE_D_CONTROL, CTL_4BIT);
    for (ix = 0; ix < 100; ix++)
        sysDelay ();
    while (((sysInByte (IDE_STATUS) & STAT_READY) == 0) && (ideWaitForever))
	;

    if (ideDebug)
	printErr ("ideInit end: \n");
    }

/*******************************************************************************
*
* ideDiagnose - diagnose the drive
*
* Diagnose the drive
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ideDiagnose (void)
    {
    int error;
    int semStatus;

    sysOutByte (IDE_SDH, SDH_IBM);
    ideWait (STAT_READY);

    sysOutByte (IDE_COMMAND, CMD_DIAGNOSE);
    semStatus = semTake (&ideSyncSem, sysClkRateGet() * ideSemSec);

    if (((error = sysInByte(IDE_ERROR)) != DIAG_OK) || (semStatus == ERROR))
	{
	if (ideDebugErr)
	    printErr ("ideDiagnose: status=0x%x error=0x%x\n", 
		      ideStatus, error);
	return (ERROR);
	}

    ideWait (STAT_READY);

    if (ideDebug)
	printErr ("ideDiagnose end: \n");

    return (OK);
    }

/*******************************************************************************
*
* idePinit - Initialize drive parameters
*
* Initialize drive parameters.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS idePinit
    (
    int drive
    )
    {
    int status;
    int error;
    int semStatus;
    int retryCount = 0;
    IDE_TYPE *pType = &ideTypes[drive];

retryPinit:
    sysOutByte (IDE_CYL_LO, pType->cylinders);
    sysOutByte (IDE_CYL_HI, pType->cylinders >> 8);
    sysOutByte (IDE_SECCNT, pType->sectorsTrack);
    sysOutByte (IDE_SDH, SDH_IBM | (drive << 4) | ((pType->heads & 0xf) - 1));
    ideWait (STAT_READY);

    sysOutByte (IDE_COMMAND, CMD_INITP);
    semStatus = semTake (&ideSyncSem, sysClkRateGet() * ideSemSec);

    if ((ideStatus & STAT_ERR) || (semStatus == ERROR))
	{
	error = sysInByte (IDE_ERROR);
	status = sysInByte (IDE_STATUS);
	ideInit ();
	if (ideDebugErr)
            printErr ("idePinit%d err: stat=0x%x 0x%x error=0x%x\n",
	              drive, ideStatus, status, error);
	if (++retryCount < ideRetry)
	    goto retryPinit;
	return (ERROR);
	}

    ideWait (STAT_READY);

    if (ideDebug)
	printErr ("idePinit%d end: \n", drive);

    return (OK);
    }

/*******************************************************************************
*
* idePread - Read drive parameters
*
* Read drive parameters.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS idePread
    (
    int drive,
    void *buffer
    )
    {
    int status;
    int error;
    int semStatus;
    int retryCount = 0;

retryPread:
    sysOutByte (IDE_SDH, SDH_IBM | (drive << 4));
    ideWait (STAT_READY);

    sysOutByte (IDE_COMMAND, CMD_READP);
    semStatus = semTake (&ideSyncSem, sysClkRateGet() * ideSemSec);

    if ((ideStatus & STAT_ERR) || (semStatus == ERROR))
	{
	error = sysInByte (IDE_ERROR);
	status = sysInByte (IDE_STATUS);
	ideInit ();
	if (ideDebugErr)
            printErr ("idePread%d err: stat=0x%x 0x%x error=0x%x\n",
	              drive, ideStatus, status, error);
	if (++retryCount < ideRetry)
	    goto retryPread;
	return (ERROR);
	}

    ideWait (STAT_DRQ);

    sysInWordString (IDE_DATA, (short *)buffer, 256);

    ideWait (STAT_READY);

    if (ideDebug)
	printErr ("idePread%d end: \n", drive);

    return (OK);
    }
/*******************************************************************************
*
* ideRecalib - recalibrate the drive
*
* Recalibrate the drive
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ideRecalib
    (
    int drive
    )
    {
    int status;
    int error;
    int semStatus;
    int retryCount = 0;

retryRecalib:
    sysOutByte (IDE_SDH, SDH_IBM | (drive << 4));
    ideWait (STAT_READY);

    sysOutByte (IDE_COMMAND, CMD_RECALIB);
    semStatus = semTake (&ideSyncSem, sysClkRateGet() * ideSemSec);

    if ((ideStatus & STAT_ERR) || (semStatus == ERROR))
	{
	error = sysInByte (IDE_ERROR);
	status = sysInByte (IDE_STATUS);
	ideInit ();
	if (ideDebugErr)
	    printErr ("ideRecalib%d err: status=0x%x 0x%x error=0x%x\n",
	              drive, ideStatus, status, error);
	if (++retryCount < ideRetry)
	    goto retryRecalib;
	return (ERROR);
	}
    
    ideWait (STAT_READY);

    if (ideDebug)
	printErr ("ideRecalib%d end: \n", drive);

    return (OK);
    }

/*******************************************************************************
*
* ideSeek - seek the drive heads to the specified cylinder
*
* Seek the drive heads to the specified cylinder
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ideSeek
    (
    int drive,
    int cylinder,
    int head
    )
    {
    int status;
    int error;
    int semStatus;
    int retryCount = 0;

retrySeek:
    sysOutByte (IDE_CYL_LO, cylinder);
    sysOutByte (IDE_CYL_HI, cylinder>>8);
    sysOutByte (IDE_SDH, SDH_IBM | (drive << 4) | (head & 0xf));
    ideWait (STAT_READY);

    sysOutByte (IDE_COMMAND, CMD_SEEK);
    semStatus = semTake (&ideSyncSem, sysClkRateGet() * ideSemSec);

    if ((ideStatus & STAT_ERR) || (semStatus == ERROR))
	{
	error = sysInByte (IDE_ERROR);
	status = sysInByte (IDE_STATUS);
	ideInit ();
	if (ideDebugErr)
	    printErr ("ideSeek%d err: c=%d h=%d status=0x%x 0x%x error=0x%x\n",
		      drive, cylinder, head, ideStatus, status, error);
	if (++retryCount < ideRetry)
	    goto retrySeek;
	return (ERROR);
	}

    ideWait (STAT_SEEKCMPLT);

    ideWait (STAT_READY);

    if (ideDebug)
	printErr ("ideSeek%d end: c=%d h=%d\n", drive, cylinder, head);

    return (OK);
    }

/*******************************************************************************
*
* ideRW - read/write a number of sectors on the current track
*
* Read/write a number of sectors on the current track
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ideRW
    (
    int drive,
    int cylinder,
    int head,
    int sector,
    void *buffer,
    int nSecs,
    int direction
    )
    {
    int ix;
    int status;
    int error;
    int semStatus;
    int retryCount = 0;
    short *pBuf; 
    IDE_TYPE *pType = &ideTypes[drive];
    int nWords = pType->bytesSector >> 1;

retryRW:
    pBuf = (short *)buffer;
    sysOutByte (IDE_PRECOMP, pType->precomp);
    sysOutByte (IDE_SECCNT, nSecs);
    sysOutByte (IDE_SECTOR, sector);
    sysOutByte (IDE_CYL_LO, cylinder);
    sysOutByte (IDE_CYL_HI, cylinder>>8);
    sysOutByte (IDE_SDH, SDH_IBM | (drive << 4) | (head & 0xf));
    ideWait (STAT_READY);

    if (direction == O_WRONLY)
	{
	sysOutByte (IDE_COMMAND, CMD_WRITE);
	for (ix = 0; ix < nSecs; ix++)
	    {
            ideWait (STAT_DRQ);
	    sysOutWordString (IDE_DATA, pBuf, nWords);
            semStatus = semTake (&ideSyncSem, sysClkRateGet() * ideSemSec);
    	    if ((ideStatus & STAT_ERR) || (semStatus == ERROR))
	        goto errorRW;
	    pBuf += nWords;
            }
	}
    else
	{
	sysOutByte (IDE_COMMAND, CMD_READ);
	for (ix = 0; ix < nSecs; ix++)
	    {
            semStatus = semTake (&ideSyncSem, sysClkRateGet() * ideSemSec);
    	    if ((ideStatus & STAT_ERR) || (semStatus == ERROR))
	        goto errorRW;
            ideWait (STAT_DRQ);
	    sysInWordString (IDE_DATA, pBuf, nWords);
	    pBuf += nWords;
	    }
	}

    ideWait (STAT_READY);

    if (ideDebug)
	printErr ("ideRW%d end: c=%d h=%d s=%d buf=0x%x n=%d dir=%d\n",
                  drive, cylinder, head, sector, (int)buffer, nSecs, direction);

    return (OK);

errorRW:
    error = sysInByte (IDE_ERROR);
    status = sysInByte (IDE_STATUS);
    ideInit ();
    if (ideDebugErr)
	{
        printErr ("ideRW%d err: c=%d h=%d s=%d buf=0x%x n=%d dir=%d ",
                  drive, cylinder, head, sector, (int)buffer, nSecs, direction);
        printErr ("stat=0x%x 0x%x error=0x%x\n", ideStatus, status, error);
	}
    if (++retryCount < ideRetry)
	goto retryRW;
    return (ERROR);
    }

/*******************************************************************************
*
* ideFormat - format the current track
*
* format the current track; not supported.
*
* RETURNS: ERROR always.
*/

LOCAL STATUS ideFormat
    (
    int drive,
    int cylinder,
    int head,
    int interleave
    )
    {
    return (ERROR);
    }

