/* m68681Serial.c - MC68681/SC2681 DUART tty driver */

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01i,08feb93,caf  fixed ansi warning.
01h,03feb93,caf  modified tyCoStartup() to avoid race condition.
01g,26jan93,caf  corrected note about available baud rates.
01f,20jan93,caf  modified interrupt mask register during initialization only.
		 rewrote tyCoStartup(), removed intLock() call from tyImrSet().
01e,09dec92,caf  fixed ansi warning (SPR 1849).
01d,01dec92,jdi  NOMANUAL tyImrSet(), tyCoInt(), tyCoClkConnect() - SPR 1808.
01c,28oct92,caf  tweaked tyCoDevCreate() documentation.
01b,24aug92,caf  changed VOID to void.  changed errno.h to errnoLib.h.
		 ansified.  added tyCoClkConnect().
01a,06jun92,caf  derived from mv165/tyCoDrv.c.
*/

/*
DESCRIPTION
This is the driver for the MC68681/SC2681 DUART.
It uses the DUART in asynchronous mode only.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly: tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling tyCoDrv().
This routine should be called exactly once, before any reads, writes, or
calls to tyCoDevCreate().  Normally, it is called by usrRoot() in usrConfig.c.

Before a terminal can be used, it must be created using tyCoDevCreate().
Each port to be used should have exactly one device associated with it by
calling this routine.

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal tty driver; for
more information, see the manual entry for tyLib.  The available baud rates
range from 75 to 19200.

SEE ALSO
tyLib
*/

#include "vxWorks.h"
#include "iv.h"
#include "ioLib.h"
#include "iosLib.h"
#include "tyLib.h"
#include "config.h"
#include "intLib.h"
#include "errnoLib.h"
#include "drv/multi/m68681.h"

typedef struct		/* BAUD */
    {
    int rate;		/* a baud rate */
    char csrVal;	/* rate to write to the csr reg to get that baud rate */
    } BAUD;

IMPORT TY_CO_DEV tyCoDv [];	/* device descriptors */

/*
 * BaudTable is a table of the available baud rates, and the values to write
 * to the csr reg to get those rates
 */

LOCAL BAUD baudTable [] =	/* BRG bit = 1 */
    {
    {75,	RX_CLK_75    | TX_CLK_75},
    {110,	RX_CLK_110   | TX_CLK_110},
    {134,	RX_CLK_134_5 | TX_CLK_134_5},
    {150,	RX_CLK_150   | TX_CLK_150},
    {300,	RX_CLK_300   | TX_CLK_300},
    {600,	RX_CLK_600   | TX_CLK_600},
    {1200,	RX_CLK_1200  | TX_CLK_1200},
    {2000,	RX_CLK_2000  | TX_CLK_2000},
    {2400,	RX_CLK_2400  | TX_CLK_2400},
    {4800,	RX_CLK_4800  | TX_CLK_4800},
    {1800,	RX_CLK_1800  | TX_CLK_1800},
    {9600,	RX_CLK_9600  | TX_CLK_9600},
    {19200,	RX_CLK_19200 | TX_CLK_19200}
    };

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

/* forward declarations */

LOCAL void tyCoStartup ();
LOCAL int tyCoOpen ();
LOCAL STATUS tyCoIoctl ();
LOCAL void tyCoHrdInit ();
void tyImrSet (TY_CO_DEV *pTyCoDv, char setBits, char clearBits);
LOCAL void tyTxInt ();
void tyCoInt ();


/*******************************************************************************
*
* tyCoDrv - initialize the tty driver
*
* This routine initializes the serial driver, sets up interrupt vectors, and
* performs hardware initialization of the serial ports.
*
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
    if (tyCoDrvNum > 0)
	return (OK);

    tyCoHrdInit ();

    tyCoDrvNum = iosDrvInstall (tyCoOpen, (FUNCPTR) NULL, tyCoOpen,
				(FUNCPTR) NULL, tyRead, tyWrite, tyCoIoctl);

    return ((tyCoDrvNum == ERROR) ? (ERROR) : (OK));
    }

/*******************************************************************************
*
* tyCoDevCreate - create a device for an on-board serial port
*
* This routine creates a device on a specified serial port.  Each port
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device "/tyCo/0", with buffer sizes of 512 bytes,
* the proper call would be:
* .CS
*     tyCoDevCreate ("/tyCo/0", 0, 512, 512);
* .CE
*
* RETURNS: OK, or ERROR if the driver is not installed, the channel is
* invalid, or the device already exists.
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

    /* check for channel validity */

    if ((channel < 0) || (channel >= tyCoDv [0].numChannels))
	return (ERROR);

    /* if there is a device already on this channel, don't do it */

    if (tyCoDv [channel].created)
	return (ERROR);

    /*
     * Initialize the ty descriptor, and turn on the bits for this
     * receiver and transmitter in the interrupt mask
     */

    if (tyDevInit (&tyCoDv [channel].tyDev,
		    rdBufSize, wrtBufSize, (FUNCPTR)tyCoStartup) != OK)
	{
	return (ERROR);
	}

    tyImrSet (&tyCoDv [channel], (tyCoDv [channel].rem | tyCoDv [channel].tem),
	      0);

    /* mark the device as created, and add the device to the I/O system */

    tyCoDv [channel].created = TRUE;

    return (iosDevAdd (&tyCoDv [channel].tyDev.devHdr, name, tyCoDrvNum));
    }

/*******************************************************************************
*
* tyCoHrdInit - initialize the DUART
*
* RETURNS: N/A
*/

LOCAL void tyCoHrdInit (void)
    {
    int ii;			/* scratch */
    int lock = intLock ();	/* LOCK INTERRUPTS */

    /* 8 data bits, 1 stop bit, no parity, set for 9600 baud */

    for (ii = 0; ii < tyCoDv [0].numChannels; ii ++)
        {
        *tyCoDv [ii].cr  = RST_BRK_INT_CMD;
        *tyCoDv [ii].cr  = RST_ERR_STS_CMD;
        *tyCoDv [ii].cr  = RST_TX_CMD;
        *tyCoDv [ii].cr  = RST_RX_CMD;
        *tyCoDv [ii].cr  = RST_MR_PTR_CMD;
        *tyCoDv [ii].mr  = PAR_MODE_NO | BITS_CHAR_8;	/* mode 1         */
        *tyCoDv [ii].mr  = STOP_BITS_1; 		/* mode 2         */
        *tyCoDv [ii].csr = RX_CLK_9600 | TX_CLK_9600;	/* clock          */
        *tyCoDv [ii].sopbc = 3;				/* A & B CTS* low */
	}

    /* enable the receivers on all channels */

    for (ii = 0; ii < tyCoDv [0].numChannels; ii ++)
        *tyCoDv [ii].cr  = RX_ENABLE;

    /*
     * All interrupts are masked out: interrupts will be enabled
     * in the tyCoDevCreate
     */

    intUnlock (lock);				/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* tyCoOpen - open a file to the DUART
*
* This routine opens a file to the DUART.
*
* RETURNS: a file descriptor
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
    int request,	/* request code      */
    int arg		/* some argument     */
    )
    {
    int    ix;
    STATUS status;

    switch (request)
	{
	case FIOBAUDRATE:
	    status = ERROR;
	    for (ix = 0; ix < NELEMENTS (baudTable); ix ++)
		{
		if (baudTable [ix].rate == arg)
		    {
		    *pTyCoDv->csr = baudTable [ix].csrVal;
		    status = OK;
		    break;
		    }
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
* tyImrSet - set and clear bits in the UART's interrupt mask register
*
* This routine sets and clears bits in the DUART's IMR.
*
* This routine sets and clears bits in a local copy of the IMR, then
* writes that local copy to the UART.  This means that all changes to
* the IMR must go through this routine.  Otherwise, any direct changes
* to the IMR would be lost the next time this routine is called.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyImrSet
    (
    TY_CO_DEV *pTyCoDv,	/* which device                   */
    char setBits,	/* which bits to set in the IMR   */
    char clearBits	/* which bits to clear in the IMR */
    )
    {
    *pTyCoDv->kimr = (*pTyCoDv->kimr | setBits) & (~clearBits);
    *pTyCoDv->imr = *pTyCoDv->kimr;
    }

/*******************************************************************************
*
* tyCoInt - handle interrupts
*
* This routine checks whether it is a UART transmit or receive
* interrupt, then does the appropriate handoff.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoInt
    (
    int	dev		/* which DUART to service */
    )
    {
    volatile char	bucket;	/* scratch */

    dev *= 2;		/* two channels per DUART */

    if (*tyCoDv [dev].isr & TX_RDY_A)	/* xmit channel 0 */
	{
	tyTxInt (dev + 0);
	}
    if (*tyCoDv [dev].isr & TX_RDY_B)	/* xmit channel 1 */
	{
	tyTxInt (dev + 1);		
	}
    if (*tyCoDv [dev].isr & RX_RDY_A)	/* recv channel 0 */
	{
	tyIRd (&tyCoDv [dev + 0].tyDev, *tyCoDv [dev + 0].dr);
	}
    if (*tyCoDv [dev].isr & RX_RDY_B)	/* recv channel 1 */
	{
	tyIRd (&tyCoDv [dev + 1].tyDev, *tyCoDv [dev + 1].dr);
	}
    if (*tyCoDv [dev].isr & CTR_RDY_INT)	/* timer */
	{
	if (tyCoDv [dev].tickRtn != NULL)
	    tyCoDv [dev].tickRtn ();
	else
	    bucket = (* tyCoDv [dev].ctroff);	/* stop timer */
	}
    }

/*******************************************************************************
*
* tyTxInt - handle a transmitter interrupt
*
* This routine is called by tyCoInt() to handle a transmitter interrupt.
* If there is another character to be transmitted, it sends it.  If there
* is no character to be transmitted or a device has never been created for
* this channel, this routine disables the transmitter interrupt.
*
* RETURNS: N/A
*/

LOCAL void tyTxInt
    (
    FAST int channel
    )
    {
    char outChar;

    if (tyCoDv [channel].created &&
	tyITx (&tyCoDv [channel].tyDev, &outChar) == OK)
	{
	*(tyCoDv [channel].dr) = outChar;
	}
    else
        *tyCoDv [channel].cr  = TX_DISABLE;		/* xmitr off */
    }

/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* This routine starts the UART transmitter.
*
* RETURNS: N/A
*/

LOCAL void tyCoStartup
    (
    TY_CO_DEV *pTyCoDv		/* ty device to start up */
    )
    {
    * pTyCoDv->cr = TX_ENABLE;		/* enable the transmitter */
    }

/*******************************************************************************
*
* tyCoClkConnect - connect to DUART timer interrupt
*
* This routine connects <routine> to the DUART timer interrupt.
* The DUART of interest is specified by <device>.
*
* This routine does not enable the DUART timer interrupt.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoClkConnect
    (
    int		device,		/* which DUART to connect                  */
    VOIDFUNCPTR	routine		/* routine to be called at timer interrupt */
    )
    {
    tyCoDv [2 * device].tickRtn = routine;
    }

