/* m68901Serial.c - MC68901 MFP tty driver */

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,30jan93,dzb  separated baud rate clocks from particular timer(s).
		 made second baud rate clock conditional on initialization.
		 removed BAUD structure.
01b,11jan93,caf  calculated baud rate from input clock, no more baudTable[].
01a,18dec92,caf  derived from version 01s of mv133/tyCoDrv.c, ansified.
*/

/*
DESCRIPTION
This is the serial driver for the Motorola MC68901 Multi-Function Peripheral
(MFP) chip.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling the
routine tyCoDrv().  This routine should be called exactly once, before any
reads, writes, or calls to tyCoDevCreate().  Normally, it is called by
usrRoot() in usrConfig.c.

Before a terminal can be used, it must be created, using tyCoDevCreate().
Each port to be used should have exactly one device associated with it by
calling this routine.

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal tty driver;
for more information, see the manual entry for tyLib.

SEE ALSO
tyLib
*/

#include "vxWorks.h"
#include "errnoLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "tyLib.h"
#include "intLib.h"
#include "iv.h"
#include "drv/multi/m68901.h"

IMPORT TY_CO_DEV tyCoDv;	/* device descriptor */

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

/* forward declarations */

LOCAL void tyCoStartup ();
LOCAL int tyCoOpen ();
LOCAL STATUS tyCoIoctl ();
LOCAL void tyCoHrdInit ();


/*******************************************************************************
*
* tyCoDrv - initialize the tty driver
*
* This routine initializes the serial driver, sets up interrupt vectors,
* and performs hardware initialization of the serial ports.
* This routine should be called exactly once, before any reads, writes, or
* calls to tyCoDevCreate().  Normally, it is called by usrRoot() in
* usrConfig.c.
*
* RETURNS: OK, or ERROR if the driver cannot be installed.
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
				(FUNCPTR) NULL, tyRead, tyWrite, tyCoIoctl);

    return (tyCoDrvNum == ERROR ? ERROR : OK);
    }

/*******************************************************************************
*
* tyCoDevCreate - create a device for an on-board serial port
*
* This routine creates a device for a specified serial port.  Each port
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device "/tyCo/0", with buffer sizes of 512 bytes,
* the proper call would be:
* .CS
*     tyCoDevCreate ("/tyCo/0", 0, 512, 512);
* .CE
*
* RETURNS: OK, or ERROR if the driver is not installed, the channel is invalid,
* or the device already exists.
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

    /* if this device already exists, don't create it */

    if (tyCoDv.created || channel != 0)
	return (ERROR);

    if (tyDevInit (&tyCoDv.tyDev, rdBufSize, wrtBufSize,
		   (FUNCPTR) tyCoStartup) != OK)
	{
	return (ERROR);
	}

    /* enable the receiver and receiver error */

    *tyCoDv.imra |= MFP_A_RX_FULL;	/* unmask recv ints */
    *tyCoDv.imra |= MFP_A_RX_ERR;	/* unmask recv error ints */

    /* mark the device as created, and add the device to the I/O system */

    tyCoDv.created = TRUE;
    return (iosDevAdd (&tyCoDv.tyDev.devHdr, name, tyCoDrvNum));
    }

/*******************************************************************************
*
* tyCoHrdInit - initialize the USART
*/

LOCAL void tyCoHrdInit (void)
    {
    int lock = intLock ();		/* LOCK INTERRUPTS */

    /* 8 data bits, 1 stop bit, no parity */

    *tyCoDv.ucr = MFP_UCR_16X | MFP_UCR_8BIT |
                              MFP_UCR_1STOP | MFP_UCR_NO_PARITY;

    /* set baud rate values w/ prescale=4 */

    *tyCoDv.baud1 = tyCoDv.baudFreq / (128 * 9600);
    if (tyCoDv.baud2 != NULL)
        *tyCoDv.baud2 = tyCoDv.baudFreq / (128 * 9600);

    /* enable the receivers and transmitters on both channels */

    *tyCoDv.rsr  = MFP_RSR_RX_ENABLE;      /* enable rcvr status */
    *tyCoDv.tsr  = MFP_TSR_TX_ENABLE;      /* enable xmit status */

    *tyCoDv.iera |= MFP_A_RX_FULL;  /* enable recv int. */
    *tyCoDv.iera |= MFP_A_RX_ERR;   /* enable recv error int. */
    *tyCoDv.iera |= MFP_A_TX_EMPTY; /* enable transmit int. */

    /* all interrupts are still masked out via the interrupt mask registers:
     * turn the receiver on and off by twiddling the interrupt mask reg (IMRA),
     * the receiver interrupt will be enabled in tyCoDevCreate,
     * the transmitter interrupt will be enabled in tyCoStartup
     */

    intUnlock (lock);			/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* tyCoOpen - open file to USART
*/

LOCAL int tyCoOpen
    (
    TY_CO_DEV *pTyCoDv,
    char *name,
    int mode
    )
    {
    return ((int) pTyCoDv);
    }

/*******************************************************************************
*
* tyCoIoctl - special device control
*
* This routine handles FIOBAUDRATE requests and passes all others to tyIoctl().
*
* RETURNS: OK, or ERROR if the baud rate is invalid, or whatever tyIoctl()
* returns.
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
            status = ERROR;
            if ((arg >= tyCoDv.baudMin) && (arg <= tyCoDv.baudMax))
                {
		/* set baud rate values w/ prescale=4 */

		*tyCoDv.baud1 = tyCoDv.baudFreq / (128 * arg);
		if (tyCoDv.baud2 != NULL)
		    *tyCoDv.baud2 = tyCoDv.baudFreq / (128 * arg);
                status = OK;
                }

            break;

	default:
	    status = tyIoctl (&pTyCoDv->tyDev, request, arg);
	    break;
	}

    return (status);
    }

/*******************************************************************************
*
* tyCoTxInt - handle a transmitter interrupt
*
* This routine gets called to handle transmitter interrupts.
* If there is another character to be transmitted, it sends it.  If
* not, or if a device has never been created for this channel, just
* disable the interrupt.
*
* NOMANUAL
*/

void tyCoTxInt (void)
    {
    char outChar;

    if (tyCoDv.created && tyITx (&tyCoDv.tyDev, &outChar) == OK)
	*tyCoDv.udr = outChar;
    else
	*tyCoDv.imra &= ~MFP_A_TX_EMPTY; /* turn off transmitter */
    }

/*******************************************************************************
*
* tyCoRxInt - handle a receiver interrupt
*
* NOMANUAL
*/

void tyCoRxInt (void)
    {
    tyIRd (&tyCoDv.tyDev, *tyCoDv.udr);
    }

/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* Call interrupt level character output routine.
*/

LOCAL void tyCoStartup
    (
    TY_CO_DEV *pTyCoDv		/* ty device to start up */
    )
    {
    char outchar;

    /* enable the transmitter and it should interrupt to write the next char */

    if (tyITx (&pTyCoDv->tyDev, &outchar) == OK)
	*pTyCoDv->udr = outchar;

    *tyCoDv.imra |= MFP_A_TX_EMPTY;	/* turn on transmitter */
    }
