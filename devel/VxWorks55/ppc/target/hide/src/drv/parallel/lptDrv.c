/* lptDrv.c - parallel chip device driver for the IBM-PC LPT */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01m,01apr01,rcs  used LPT_DATA_RES, LPT_STAT_RES, and LPT_CTRL_RES macros
                 exclusively and set the default spacing to 1 SPR# 65937
01l,23mar01,sru  register spacing now provided in LPT_RESOURCE SPR# 65937
01k,15mar01,dat  SPR 28847/65275, disable interrupt after each character
01j,21nov00,jkf  SPR#62266 T3 doc update
01i,26oct00,jkf  SPR 35546, removed unused IMPORT sysIntLevel().
01h,04jan00,dat  SPR 29875, using SEM_ID in structures.
01g,06oct98,jmp  doc: cleanup.
01f,03jun98,hdn  removed spurious interrupt handling from lptIntr().
01e,18oct96,hdn  re-written.
01d,14jun95,hdn  removed function declarations defined in sysLib.h.
01c,24jan95,jdi  doc cleanup.
01b,20nov94,kdl  fixed typo in man page.
01a,13oct94,hdn  written.
*/

/*
DESCRIPTION
This is the basic driver for the LPT used on the IBM-PC.  If the 
component INCLUDE_LPT is enabled, the driver initializes the LPT 
port on the PC.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  However, two routines must be called directly:  lptDrv() to
initialize the driver, and lptDevCreate() to create devices.

There are one other callable routines:  lptShow() to show statistics.
The argument to lptShow() is a channel number, 0 to 2.

Before the driver can be used, it must be initialized by calling lptDrv().
This routine should be called exactly once, before any reads, writes, or
calls to lptDevCreate().  Normally, it is called from usrRoot() in
usrConfig.c.  The first argument to lptDrv() is a number of channels,
0 to 2.  The second argument is a pointer to the resource table.
Definitions of members of the resource table structure are:

.CS
    int  ioBase;         /@ IO base address @/
    int  intVector;      /@ interrupt vector @/
    int  intLevel;       /@ interrupt level @/
    BOOL autofeed;       /@ TRUE if enable autofeed @/
    int  busyWait;       /@ loop count for BUSY wait @/
    int  strobeWait;     /@ loop count for STROBE wait @/
    int  retryCnt;       /@ retry count @/
    int  timeout;        /@ timeout second for syncSem @/
.CE

IOCTL FUNCTIONS
This driver responds to two functions: LPT_SETCONTROL and LPT_GETSTATUS.
The argument for LPT_SETCONTROL is a value of the control register.
The argument for LPT_GETSTATUS is a integer pointer where a value of the
status register is stored.

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
#include "intLib.h"
#include "iv.h"
#include "wdLib.h"
#include "sysLib.h"
#include "sys/fcntlcom.h"
#include "drv/parallel/lptDrv.h"


/* imports */

IMPORT UINT sysStrayIntCount;


/* globals */

LPT_DEV lptDev [N_LPT_CHANNELS];


/* locals */

LOCAL int	lptDrvNum;	/* driver number assigned to this driver */


/* forward declarations */

LOCAL	int	lptOpen		(LPT_DEV *pDev, char *name, int mode);
LOCAL	int	lptRead		(LPT_DEV *pDev, char *pBuf, int size);
LOCAL	int	lptWrite	(LPT_DEV *pDev, char *pBuf, int size);
LOCAL	STATUS	lptIoctl	(LPT_DEV *pDev, int function, int arg);
LOCAL	void	lptIntr		(LPT_DEV *pDev);
LOCAL	void	lptInit		(LPT_DEV *pDev);


/*******************************************************************************
*
* lptDrv - initialize the LPT driver
*
* This routine initializes the LPT driver, sets up interrupt vectors,
* and performs hardware initialization of the LPT ports.
*
* This routine should be called exactly once, before any reads, writes,
* or calls to lptDevCreate().  Normally, it is called by usrRoot()
* in usrConfig.c.
*
* RETURNS: OK, or ERROR if the driver cannot be installed.
*
* SEE ALSO: lptDevCreate()
*/

STATUS lptDrv 
    (
    int channels,		/* LPT channels */
    LPT_RESOURCE *pResource	/* LPT resources */
    )
    {
    int ix;
    LPT_DEV *pDev;

    /* check if driver already installed */

    if (lptDrvNum > 0)
	return (OK);
    
    if (channels > N_LPT_CHANNELS)
	return (ERROR);

    for (ix=0; ix < channels; ix++, pResource++)
	{
	pDev = &lptDev[ix];

	pDev->created		= FALSE;
	pDev->autofeed		= pResource->autofeed;
	pDev->inservice		= FALSE;

        if (pResource->regDelta == 0) 
	   pResource->regDelta = 1; 

        pDev->data		= LPT_DATA_RES (pResource);
        pDev->stat		= LPT_STAT_RES (pResource);
        pDev->ctrl		= LPT_CTRL_RES (pResource);
	pDev->intCnt		= 0;
	pDev->retryCnt		= pResource->retryCnt;
	pDev->busyWait		= pResource->busyWait;
	pDev->strobeWait	= pResource->strobeWait;
	pDev->timeout		= pResource->timeout;
	pDev->intLevel		= pResource->intLevel;

    	pDev->syncSem = semBCreate (SEM_Q_FIFO, SEM_EMPTY);

    	pDev->muteSem = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
				  SEM_INVERSION_SAFE);

    	(void) intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pResource->intVector),
		           (VOIDFUNCPTR)lptIntr, (int)pDev);

    	sysIntEnablePIC (pDev->intLevel);	/* unmask the interrupt */

	lptInit (&lptDev[ix]);
	}

    lptDrvNum = iosDrvInstall (lptOpen, (FUNCPTR) NULL, lptOpen,
			       (FUNCPTR) NULL, lptRead, lptWrite, lptIoctl);

    return (lptDrvNum == ERROR ? ERROR : OK);
    }
/*******************************************************************************
*
* lptDevCreate - create a device for an LPT port
*
* This routine creates a device for a specified LPT port.  Each port
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device `/lpt/0', the proper call would be:
* .CS
*     lptDevCreate ("/lpt/0", 0);
* .CE
*
* RETURNS: OK, or ERROR if the driver is not installed, the channel is
* invalid, or the device already exists.
*
* SEE ALSO: lptDrv()
*/

STATUS lptDevCreate
    (
    char *name,		/* name to use for this device */
    int channel		/* physical channel for this device (0 - 2) */
    )
    {

    if (channel >= N_LPT_CHANNELS)
	return (ERROR);

    if (lptDrvNum <= 0)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    /* if this device already exists, don't create it */

    if (lptDev[channel].created)
	return (ERROR);

    /* mark the device as created, and add the device to the I/O system */

    lptDev[channel].created = TRUE;
    return (iosDevAdd (&lptDev[channel].devHdr, name, lptDrvNum));
    }
/*******************************************************************************
*
* lptOpen - open file to LPT
*
* ARGSUSED1
*/

LOCAL int lptOpen
    (
    LPT_DEV *pDev,
    char    *name,
    int     mode
    )
    {
    return ((int) pDev);
    }
/*******************************************************************************
*
* lptRead - read from the port.
*
* Read from the port.
*
* RETURNS: ERROR.
*/

LOCAL int lptRead
    (
    LPT_DEV *pDev,
    char    *pBuf,
    int     size
    )
    {
    return (ERROR);	/* XXX would be supported in next release */
    }
/*******************************************************************************
*
* lptWrite - write to the port.
*
* Write to the port.
*
* RETURNS: The number of bytes written, or ERROR if the command didn't succeed.
*/

LOCAL int lptWrite
    (
    LPT_DEV *pDev,
    char    *pBuf,
    int     size
    )
    {
    int byteCnt = 0;
    BOOL giveup = FALSE;
    int retry;
    int wait;

    if (size == 0)
	return (size);
    
    semTake (pDev->muteSem, WAIT_FOREVER);

    retry = 0;
    while ((sysInByte (pDev->stat) & S_MASK) != 
	   (S_SELECT | S_NODERR | S_NOBUSY))
	{
	if (retry++ > pDev->retryCnt)
	    {
	    if (giveup)
		{
	        errnoSet (S_ioLib_DEVICE_ERROR);
	        semGive (pDev->muteSem);
	        return (ERROR);
		}
	    else
		{
		lptInit (pDev);
		giveup = TRUE;
		}
	    }
        wait = 0;
	while (wait != pDev->busyWait)
	    wait++;
	}
    
    retry = 0;
    do {
        wait  = 0;
	sysOutByte (pDev->data, *pBuf);
	while (wait != pDev->strobeWait)
	    wait++;
	sysOutByte (pDev->ctrl, sysInByte (pDev->ctrl) | C_STROBE | C_ENABLE);
	while (wait)
	    wait--;
	sysOutByte (pDev->ctrl, sysInByte (pDev->ctrl) & ~C_STROBE);

	if (semTake (pDev->syncSem, pDev->timeout * sysClkRateGet ()) == ERROR)
	    {
	    if (retry++ > pDev->retryCnt)
		{
		errnoSet (S_ioLib_DEVICE_ERROR);
		semGive (pDev->muteSem);
		return (ERROR);
		}
	    }
	else
	    {
	    pBuf++;
	    byteCnt++;
	    }
	} while (byteCnt < size);

    semGive (pDev->muteSem);

    return (size);
    }
/*******************************************************************************
*
* lptIoctl - special device control
*
* This routine handles LPT_SETCONTROL and LPT_GETSTATUS requests.
*
* RETURNS: OK or ERROR if invalid request.
*/

LOCAL STATUS lptIoctl
    (
    LPT_DEV *pDev,	/* device to control */
    int function,	/* request code */
    int arg		/* some argument */
    )
    {
    int status = OK;

    semTake (pDev->muteSem, WAIT_FOREVER);
    switch (function)
	{
	case LPT_SETCONTROL:
    	    sysOutByte (pDev->ctrl, arg);
	    break;

	case LPT_GETSTATUS:
    	    *(int *)arg = sysInByte (pDev->stat);
	    break;

	default:
	    (void) errnoSet (S_ioLib_UNKNOWN_REQUEST);
	    status = ERROR;
	    break;
	}
    semGive (pDev->muteSem);

    return (status);
    }
/*******************************************************************************
*
* lptIntr - handle an interrupt
*
* This routine gets called to handle interrupts.
* If there is another character to be transmitted, it sends it.  If
* not, or if a device has never been created for this channel, just
* return.
*
* Disables interrupts after each character.  This protects the system
* against continuous interrupts when printer is not connected, or interrupt
* controller is not programmed correctly (level versus edge detect).
*/

LOCAL void lptIntr
    (
    LPT_DEV *pDev
    )
    {

    pDev->inservice = TRUE;
    pDev->intCnt++;
    semGive (pDev->syncSem);
    pDev->inservice = FALSE;
    sysOutByte (pDev->ctrl, sysInByte (pDev->ctrl) & ~C_ENABLE);  
    }
/*******************************************************************************
*
* lptInit - initialize the specified LPT port
*
* initialize the specified LPT port.
*/

LOCAL void lptInit
    (
    LPT_DEV *pDev
    )
    {
    sysOutByte (pDev->ctrl, 0);			/* init */
    taskDelay (sysClkRateGet () >> 3);		/* hold min 50 mili sec */
    if (pDev->autofeed)
        sysOutByte (pDev->ctrl, C_NOINIT | C_SELECT | C_AUTOFEED);
    else
        sysOutByte (pDev->ctrl, C_NOINIT | C_SELECT);
    }
/*******************************************************************************
*
* lptShow - show LPT statistics
*
* This routine shows statistics for a specified LPT port.
*
* RETURNS: N/A
*/

void lptShow
    (
    UINT channel	/* channel (0 - 2) */
    )
    {
    LPT_DEV *pDev = &lptDev[channel];

    if (channel > N_LPT_CHANNELS)
	return;

    printf ("controlReg        = 0x%x\n", sysInByte (pDev->ctrl));
    printf ("statusReg         = 0x%x\n", sysInByte (pDev->stat));
    printf ("created           = %s\n",   pDev->created ? "TRUE" : "FALSE");
    printf ("autofeed          = %s\n",   pDev->autofeed ? "TRUE" : "FALSE");
    printf ("inservice         = %s\n",   pDev->inservice ? "TRUE" : "FALSE");
    printf ("normalInt         = %d\n",   pDev->intCnt);
    printf ("defaultInt        = %d\n",   sysStrayIntCount);
    printf ("retryCnt          = %d\n",   pDev->retryCnt);
    printf ("busyWait   (loop) = %d\n",   pDev->busyWait);
    printf ("strobeWait (loop) = %d\n",   pDev->strobeWait);
    printf ("timeout    (sec)  = %d\n",   pDev->timeout);
    printf ("intLevel   (IRQ)  = %d\n",   pDev->intLevel);
    }
