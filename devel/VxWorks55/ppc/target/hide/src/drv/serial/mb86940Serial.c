/* mb86940Serial.c - MB 86940 UART tty driver */

/* Copyright 1992-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,21jun93,ccc  modified for mb931.
01g,28oct92,caf  tweaked tyCoDevCreate() documentation and parm checking.
01f,18aug92,ccc  made board-independent (from ver 01e of mb930/tyCoDrv.c).
01e,18jul92,smb  Changed errno.h to errnoLib.h.
01d,16jun92,jwt  cleaned up ANSI compiler warnings.
01c,26may92,rrr  the tree shuffle
01b,26apr92,jwt  passed through the ansification filter
		  -changed VOID to void
		  -changed copyright notice
01a,07jan92,ccc  written.
*/

/*
DESCRIPTION
This is the driver for the SPARClite MB86930 on-board serial ports.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly: tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling tyCoDrv().
This routine should be called exactly once, before any reads, writes, or
calls to tyCoDevCreate().  Normally, it is called by usrRoot() in usrConfig.c.

Before a terminal can be used, it must be created using tyCoDevCreate().
Each port to be used should have exactly one device associated with it
by calling this routine.

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal tty driver;
for more information, see the manual entry for tyLib.  The UARTs use timer 
3 output to generate the following baud rates:  110, 150, 300, 600, 1200,
2400, 4800, 9600, and 19200.  Note that the UARTs will operate at the same
baud rate.
*/

#include "vxWorks.h"
#include "ioLib.h"
#include "tyLib.h"
#include "intLib.h"
#include "errnoLib.h"
#include "arch/sparc/mb86940.h"

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

IMPORT TY_CO_DEV tyCoDv []; /* device descriptors */

/* forward declarations */

LOCAL void   tyCoStartup ();
LOCAL int    tyCoOpen ();
LOCAL int    tyCoRead ();
LOCAL int    tyCoWrite ();
LOCAL STATUS tyCoIoctl ();
LOCAL void   tyCoHrdInit ();

/******************************************************************************
*
* tyCoDrv - initialize the tty driver
*
* This routine initializes the serial driver, sets up interrupt vectors,
* and performs hardware initialization of the serial ports.
*
* This routine must be called in supervisor state, since it does
* physical I/O directly.  
*
* This routine should be called exactly once, before any reads, writes, or 
* calls to tyCoDevCreate().  Normally, it is called by usrRoot() in 
* usrConfig.c.
*
* RETURNS
* OK, or ERROR if the driver cannot be installed.
*
* SEE ALSO: tyCoDevCreate()
*/

STATUS tyCoDrv (void)

    {
    /* check if driver already installed */

    if (tyCoDrvNum > 0)
	return (OK);

    tyCoHrdInit ();

    tyCoDrvNum = iosDrvInstall (tyCoOpen, (FUNCPTR) NULL, tyCoOpen,
				(FUNCPTR) NULL, tyCoRead, tyCoWrite, tyCoIoctl);

    return (tyCoDrvNum == ERROR ? ERROR : OK);
    }

/******************************************************************************
*
* tyCoDevCreate - create a device for an on-board serial ports
*
* This routine creates a device on a specified serial port.  Each port
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device "/tyCo/0", with buffer sizes of 512
* bytes, the proper call would be:
* .CS
*     tyCoDevCreate ("/tyCo/0", 0, 512, 512);
* .CE
*
* RETURNS
* OK, or ERROR if the driver is not installed, the channel is invalid, or the
* device already exists.
*
* SEE ALSO: tyCoDrv()
*/

STATUS tyCoDevCreate
    (
    char *      name,           /* name to use for this device      */
    FAST int    channel,        /* physical channel for this device */
    int         rdBufSize,      /* read buffer size, in bytes       */
    int         wrtBufSize      /* write buffer size, in bytes      */
    )
    {
    if (tyCoDrvNum <= 0)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    /* if this doesn't represent a valid channel, don't do it */

    if (channel < 0 || channel >= tyCoDv [0].numChannels)
        return (ERROR);

    /* if this device already exists, don't create it */

    if (tyCoDv [channel].created)
	return (ERROR);

    /* initialize the ty descriptor, and turn on the bit for this
     * receiver and transmitter in the interrupt mask */

    if (tyDevInit ((TY_DEV_ID) &tyCoDv [channel],
		    rdBufSize, wrtBufSize, (FUNCPTR) tyCoStartup) != OK)
	{
	return (ERROR);
	}

    /* Mark the device as having been created, and add the device to
     * the I/O system */

    tyCoDv [channel].created = TRUE;
    return (iosDevAdd ((DEV_HDR *) &tyCoDv [channel], name, tyCoDrvNum));
    }

/******************************************************************************
*
* tyCoHrdInit - initialize the UARTs.
*
* This routine initializes the on-board UARTs for the VxWorks environment.
*
* This routine must be called in supervisor mode, since it accesses I/O
* space. (This depends on how the page lookup table is setup!)
*
*/

LOCAL void tyCoHrdInit (void)

    {
    int ix;
    int delay;		/* delay for reset */
    volatile int bump;	/* for something to do */
    int oldlevel;	/* current interrupt level mask */

    oldlevel = intLock ();	/* disable interrupts during init */

    /* set up baud rate at 9600 */

    sysBaudSet (9600);

    /* Initialization sequence to put the SDTR in a known reset state */
    /* From Fujitsu code */
    sys940AsiSeth ((void *) (tyCoDv [0].cp), NULL);
    sys940AsiSeth ((void *) (tyCoDv [1].cp), NULL);
    sys940AsiSeth ((void *) (tyCoDv [0].cp), NULL);
    sys940AsiSeth ((void *) (tyCoDv [1].cp), NULL);
    sys940AsiSeth ((void *) (tyCoDv [0].cp), NULL);
    sys940AsiSeth ((void *) (tyCoDv [1].cp), NULL);

    for (ix = 0; ix < tyCoDv [0].numChannels; ix++) /* should go to N_UARTS */
	{
	sys940AsiSeth ((void *) (tyCoDv [ix].cp), FCC_SDTR_RESET);

	for(delay=0; delay < 10000; delay++)
	    bump++;

	/* after reset only the first access is the MODE register */
	sys940AsiSeth ((void *) (tyCoDv [ix].cp), FCC_SDTR_1_BIT    |
					       FCC_SDTR_ODD      |
					       FCC_SDTR_NOPARITY |
					       FCC_SDTR_8BITS    |
					       FCC_SDTR_1_16XCLK);

	sys940AsiSeth ((void *) (tyCoDv [ix].cp), FCC_TX_START);
	}

    intUnlock (oldlevel);
    }

/******************************************************************************
*
* tyCoOpen - open file to SPARClite UART
*/

LOCAL int tyCoOpen
    (
    TY_CO_DEV *pTyCoDv,
    char *name,
    int flags,
    int mode
    )
    {
    return ((int) pTyCoDv);
    }
/******************************************************************************
*
* tyCoRead - task level read routine for SPARClite UART
*
* This routine fields all read calls to the SPARClite UART.
*/

LOCAL int tyCoRead
    (
    TY_CO_DEV *pTyCoDv,
    char *buffer,
    int maxbytes
    )
    {
    return (tyRead ((TY_DEV_ID) pTyCoDv, buffer, maxbytes));
    }
/******************************************************************************
*
* tyCoWrite - task level write routine for SPARClite UART
*
* This routine fields all write calls to the SPARClite UART.
*/

LOCAL int tyCoWrite
    (
    TY_CO_DEV *pTyCoDv,
    char *buffer,
    int nbytes
    )
    {
    return (tyWrite ((TY_DEV_ID) pTyCoDv, buffer, nbytes));
    }
/******************************************************************************
*
* tyCoIoctl - special device control
*
* This routine handles baud rate requests, and passes all other requests
* to tyIoctl().
*
* NOTE:  Both serial ports use the output of timer 3 for the baud clock.
* Therefore, both ports will be set for the same baud rate.
*/

LOCAL STATUS tyCoIoctl
    (
    TY_CO_DEV *pTyCoDv,	/* device to control */
    int request,	/* request code */
    int arg		/* some argument */
    )
    {
    STATUS status;

    switch (request)
	{
	case FIOBAUDRATE:
	    status = sysBaudSet (arg);
	    break;

	default:
	    status = tyIoctl ((TY_DEV_ID) pTyCoDv, request, arg);
	    break;
	}

    return (status);
    }

/******************************************************************************
*
* tyCoIntRx - handle a receiver interrupt
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoIntRx
    (
    FAST int channel
    )
    {
    (void) tyIRd ((TY_DEV_ID) &tyCoDv [channel],
		  sys940AsiGeth ((void *) tyCoDv [channel].dp));
    }
/******************************************************************************
*
* tyCoIntTx - handle a transmitter interrupt
*
* If there is another character to be transmitted, it sends it.  If
* not, or if a device has never been created for this channel, we just
* disable the interrupt.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoIntTx
    (
    FAST int channel
    )
    {
    char outChar;

    if ((tyCoDv [channel].created) &&
	(tyITx ((TY_DEV_ID) &tyCoDv [channel], &outChar) == OK))
	{
	sys940AsiSeth ((void *) (tyCoDv [channel].dp), outChar);
	}
    }
/******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* Call interrupt level character output routine for SPARClite UART.
*/

LOCAL void tyCoStartup
    (
    TY_CO_DEV *pTyCoDv		/* ty device to start up */
    )
    {
    char outChar;

    /*
     * if created, and a character is available send it, the the IRQ
     * routine will send the rest.
     */

    if ((pTyCoDv->created) && (tyITx (&pTyCoDv->tyDev, &outChar) == OK))
	sys940AsiSeth ((void *) (pTyCoDv->dp), outChar);
    }
