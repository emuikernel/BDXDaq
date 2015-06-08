/* m68562Serial.c - MC68562 DUSCC tty driver */

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,07jul93,vin  changed *pTyCoDv->omr = DUSCC_OMR_T....
                 to      *pTyCoDv->omr |= DUSCC_OMR_T.... in 
		 tyCoResetChannel ();
01n,01dec92,jdi  NOMANUAL tyCoRxTxErrInt(), tyCoRxInt(), tyCoTxInt() - SPR 1808.
01m,27oct92,caf  don't set the baud rate if it is already correct (SPR 1721).
		 don't write to the general status register in tyCoStartup().
		 tweaked tyCoDevCreate() documentation.
01l,22oct92,caf  improved DUSCC initialization procedures (SPR 1689),
		 fixed tyCoRxTxErrInt().
01k,01sep92,caf  ansified.  fixed parameter check in tyCoDevCreate().
01j,18jul92,smb  Changed errno.h to errnoLib.h.
01i,01jul92,caf  made board-independent (from vers 01h of frc40/tyCoDrv.c).
01h,26may92,rrr  the tree shuffle
01g,01may92,caf  fixed gsr access in tyCoStartup() (SPR #1355).
                 silenced ansi warnings.
01f,08jan92,jdi  documentation cleanup.
01e,04oct91,rrr  passed through the ansification filter
                  -changed VOID to void
                  -changed copyright notice
01d,18aug91,jdi  documentation cleanup.
01c,30jul91,jpb  fixed bug in tyCoTxInt (see frc33 01i,21jul91,caf)
01b,25jul91,jpb  fill transmit FIFO, and fixed documentation.
01a,07jul91,jpb  reformatted according to WRS conventions.
*/

/*
DESCRIPTION
This is the driver for the MC68562 DUSCC serial chip.
It uses the DUSCC in asynchronous mode only.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create devices.

TYCODRV
Before the driver can be used, it must be initialized by calling tyCoDrv().
This routine should be called exactly once, before any reads, writes, or
calls to tyCoDevCreate().  Normally, it is called by usrRoot() in usrConfig.c.

Before a terminal can be used, it must be created using tyCoDevCreate().
Each port to be used should have exactly one device associated with it
by calling this routine.

IOCTL
This driver responds to the same ioctl() codes as a normal tty driver.
See the manual entry for tyLib for more information.

SEE ALSO: tyLib
*/

#include "vxWorks.h"
#include "iv.h"
#include "ioLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "tyLib.h"
#include "intLib.h"
#include "errnoLib.h"
#include "drv/multi/m68562.h"


#define BIT(val,bit)    ((val & bit) == bit)


typedef struct	       /* BAUD */
    {
    int rate;	       /* a baud rate */
    char xmtVal;       /* rate to write to the Transmitter Timing Register */
    char rcvVal;       /* rate to write to the Reciever Timing Register */
    }      BAUD;

IMPORT TY_CO_DEV tyCoDv []; /* device descriptors */

/* 
 * baudTable is a table of the available baud rates, and the values to 
 * write to the csr reg to get those rates 
 */

LOCAL BAUD baudTable [] =
        {
        {50,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_50,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_50 },
        {75,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_75,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_75 },
        {110,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_110,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_110},
        {134,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_134_5,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_134_5},
        {150,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_150,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_150},
        {200,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_200,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_200},
        {300,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_300,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_300},
        {600,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_600,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_600},
        {1050,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_1050,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_1050},
        {1200,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_1200,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_1200},
        {2000,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_2000,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_2000},
        {2400,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_2400,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_2400},
        {4800,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_4800,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_4800},
        {9600,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_9600,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_9600},
        {19200,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_19200,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_19200},
        {38400,
	DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_38400,
	DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_38400} };

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

/* forward declarations */

LOCAL void tyCoStartup ();
LOCAL int tyCoOpen ();
LOCAL STATUS tyCoIoctl ();
void tyCoTxInt ();
void tyCoRxInt ();
void tyCoRxTxErrInt ();
LOCAL void tyCoHrdInit ();
LOCAL void tyCoResetChannel ();


/*******************************************************************************
*
* tyCoDrv - tty driver initialization routine
*
* This routine initializes the driver, sets up interrupt vectors, and
* performs hardware initialization of the serial ports.
*
* This routine should be called exactly once, before any reads, writes, or
* calls to tyCoDevCreate().  Normally, it is called by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the driver cannot be installed.
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
* This routine creates a device on a specified serial port.  Each port
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device "/tyCo/0", with a buffer size of 512 bytes,
* the proper call would be:
*
* .CS
*     tyCoDevCreate ("/tyCo/0", 0, 512, 512);
* .CE
*
* RETURNS: OK, or ERROR if the driver is not installed, the channel is invalid,
* or the device already exists.
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

    /* check for valid channel and if device already exists, don't create it */

    if (channel < 0 || channel >= tyCoDv [0].numChannels)
	return (ERROR);

    if (tyCoDv[channel].created)
	return (ERROR);

    /*
     * initialize the ty descriptor, and turn on the bit for this receiver in
     * the interrupt mask
     */

    if (tyDevInit (&tyCoDv[channel].tyDev, rdBufSize, wrtBufSize,
		   (FUNCPTR) tyCoStartup) != OK)
        {
	return (ERROR);
        }

    *tyCoDv[channel].ier = DUSCC_IER_RXRDY | DUSCC_IER_RSR_5_4;

    /* mark the device as created, and add the device to the I/O system */

    tyCoDv[channel].created = TRUE;

    return (iosDevAdd (&tyCoDv[channel].tyDev.devHdr, name, tyCoDrvNum));
    }

/*******************************************************************************
*
* tyCoHrdInit - initialize the DUSCC
*/

LOCAL void tyCoHrdInit (void)
    {
    int	ix;
    int	lock = intLock ();      /* LOCK INTERRUPTS */

    for (ix = 0; ix < tyCoDv [0].numChannels; ix ++)
        tyCoResetChannel (tyCoDv + ix);

    intUnlock (lock);		/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* tyCoResetChannel - reset a single channel
*/

LOCAL void tyCoResetChannel
    (
    TY_CO_DEV *pTyCoDv
    )
    {
    /* 8 data bits, 1 stop bit, no parity, set for 9600 baud */

    /*
     * DUSCC Initialization Procedures (from Dec 1987 Signetics app note)
     *
     * "Note that whenever a change is made in the channel mode registers,
     *  the transmitter and receiver should first be disabled.  Also, before
     *  any change is made in the transmit parameter register (TPR) or the
     *  transmit timing register (TTR), the transmitter should be disabled.
     *  After a change(s) has been completed, the trasmitter should be reset; 
     *  then enabled.
     *
     * "If changes are made in either the receive parameter register (RPR)
     *  or the receive timing register (RTR), the receiver should be disabled.
     *  After a changes(s) has been completed, the the receiver should be
     *  reset; then enabled."
     */

    /* disable transmitter and receiver */

    *pTyCoDv->ccr = DUSCC_CCR_TX_DISABLE_TX;
    *pTyCoDv->ccr = DUSCC_CCR_RX_DISABLE_RX;

    /* configure transmitter and receiver */

    *pTyCoDv->cmr1 = DUSCC_CMR1_ASYNC;
    *pTyCoDv->cmr2 = DUSCC_CMR2_DTI_POLL_OR_INT;
    *pTyCoDv->tpr  = DUSCC_TPR_ASYNC_1 | DUSCC_TPR_8BITS;
    *pTyCoDv->ttr  = DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_9600;
    *pTyCoDv->rpr  = DUSCC_RPR_8BITS;
    *pTyCoDv->rtr  = DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_9600;
    *pTyCoDv->omr |= DUSCC_OMR_TXRDY_FIFO_EMPTY |
                    DUSCC_OMR_TX_RES_CHAR_LENGTH_TPR;

    /* reset transmitter and receiver */

    *pTyCoDv->ccr = DUSCC_CCR_TX_RESET_TX;
    *pTyCoDv->ccr = DUSCC_CCR_RX_RESET_RX;

    /* enable transmitter and receiver */

    *pTyCoDv->ccr = DUSCC_CCR_TX_ENABLE_TX;
    *pTyCoDv->ccr = DUSCC_CCR_RX_ENABLE_RX;

    /*
     * all interrupts are masked out: the receiver interrupt will be
     * enabled in the tyCoDevCreate
     */
    }

/*******************************************************************************
*
* tyCoOpen - open file to DUSCC
*
* RETURNS: device number.
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
* This routine handles baud rate requests, and passes all others to tyIoctl.
*
* RETURNS: OK or ERROR if invalid baud rate or whatever tyIoctl returns.
*/

LOCAL STATUS tyCoIoctl
    (
    TY_CO_DEV *pTyCoDv,	/* device to control */
    int request,	/* request code      */
    int arg		/* some argument     */
    )
    {
    int ix;
    STATUS status;

    switch (request)
        {
	case FIOBAUDRATE:
	    status = ERROR;
	    for (ix = 0; ix < NELEMENTS (baudTable); ix ++)
		if (baudTable [ix].rate == arg)
		    {
		    status = OK;	/* baud rate is valid */

		    /*
		     * Only set baud rate if the hardware needs it.
		     * Setting the baud rate to 9600 when the baud
		     * rate is already 9600, for example, would
		     * unnecessarily disable the serial channel and
		     * possibly drop characters.
		     */

		    if ((*pTyCoDv->ttr != baudTable [ix].xmtVal) ||
			(*pTyCoDv->rtr != baudTable [ix].rcvVal))
			{
			/* disable transmitter and receiver */

			*pTyCoDv->ccr = DUSCC_CCR_TX_DISABLE_TX;
			*pTyCoDv->ccr = DUSCC_CCR_RX_DISABLE_RX;

			/* configure transmitter and receiver */

			*pTyCoDv->ttr = baudTable [ix].xmtVal;
			*pTyCoDv->rtr = baudTable [ix].rcvVal;

			/* reset transmitter and receiver */

			*pTyCoDv->ccr = DUSCC_CCR_TX_RESET_TX;
			*pTyCoDv->ccr = DUSCC_CCR_RX_RESET_RX;

			/* enable transmitter and receiver */

			*pTyCoDv->ccr = DUSCC_CCR_TX_ENABLE_TX;
			*pTyCoDv->ccr = DUSCC_CCR_RX_ENABLE_RX;

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
* tyCoRxTxErrInt - handle a receiver/transmitter error status interrupt
*
* Only the receive overrun condition is handled.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoRxTxErrInt
    (
    FAST TY_CO_DEV *pTyCoDv
    )
    {
    if (BIT (*pTyCoDv->rsr, DUSCC_RSR_ASYNC_OVERN_ERROR))
	*pTyCoDv->rsr = DUSCC_RSR_ASYNC_OVERN_ERROR;	/* "1" clears bit */
    }

/*******************************************************************************
*
* tyCoRxInt - handle a receiver interrupt
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoRxInt
    (
    FAST TY_CO_DEV *pTyCoDv
    )
    {
    /* drain receive FIFO */

    do
        {
	(void) tyIRd (&pTyCoDv->tyDev, *pTyCoDv->rx_data);
        }
    while (BIT (*pTyCoDv->gsr, pTyCoDv->rx_rdy));
    }

/*******************************************************************************
*
* tyCoTxInt - handle a transmitter interrupt
*
* If there is another character to be transmitted, it sends it.  If
* not, or if a device has never been created for this channel,
* disable the interrupt.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoTxInt
    (
    FAST TY_CO_DEV *pTyCoDv
    )
    {
    char outChar;

    if (pTyCoDv->created)
        {
        do                                          /* fill transmit FIFO */
            {
            if (tyITx (&pTyCoDv->tyDev, &outChar) == OK)
                *pTyCoDv->tx_data = outChar;
            else
                {
                *pTyCoDv->ier &= ~DUSCC_IER_TXRDY;  /* turn off transmitter */
                break;
                }
            }
        while (BIT (*pTyCoDv->gsr, pTyCoDv->tx_rdy));
        }
    else
        {
	*pTyCoDv->ier &= ~DUSCC_IER_TXRDY;	/* turn off the transmitter */
        }
    }

/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* Call interrupt level character output routine for DUSCC.
*/

LOCAL void tyCoStartup
    (
    FAST TY_CO_DEV *pTyCoDv	/* ty device to start up */
    )
    {
    char outChar;

    if (pTyCoDv->created && tyITx (&pTyCoDv->tyDev, &outChar) == OK)
        {
        *pTyCoDv->tx_data  = outChar;
        *pTyCoDv->ier     |= DUSCC_IER_TXRDY;
        }
    }
