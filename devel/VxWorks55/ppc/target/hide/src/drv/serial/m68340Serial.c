/* m68340Serial.c - Motorola MC68340 tty driver*/

/* Copyright 1984-1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01j,03feb93,caf  modified interrupt mask register during initialization only,
                 rewrote tyCoStartup() (SPR #1757).
01i,28oct92,caf  tweaked tyCoDevCreate() documentation.
01h,01aug92,caf  made board-independent, ansified.
01g,28jul92,caf  fixed ansi warnings.
01f,18jul92,smb  Changed errno.h to errnoLib.h.
01e,26may92,rrr  the tree shuffle
01d,24jan92,jdi  documentation cleanup.
01c,04oct91,rrr  passed through the ansification filter
		  -changed VOID to void
		  -changed copyright notice
01b,26sep91,jdi  documentation cleanup.
01a,08sep91,jcf  derived from mv165/tyCoDrv.c.
*/

/*
DESCRIPTION
This is the driver for the Motorola MC68340 on-chip DUART.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling tyCoDrv().
This routine should be called exactly once, before any reads, writes, or
calls to tyCoDevCreate().  Normally, it is called from usrRoot() in
usrConfig.c.

Before a terminal can be used, it must be created using tyCoDevCreate().
Each port to be used should have exactly one device associated with it
by calling this routine.

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal tty driver; for
more information, see the manual entry for tyLib.  The available baud
rates are 75, 110, 134.5, 150, 300, 600, 1200, 1800, 2000, 2400, 4800,
9600, and 19200.

SEE ALSO: tyLib
*/

#include "vxWorks.h"
#include "errnoLib.h"
#include "iv.h"
#include "ioLib.h"
#include "intLib.h"
#include "iosLib.h"
#include "tyLib.h"
#include "drv/multi/m68340.h"


typedef struct		/* BAUD */
    {
    int rate;		/* a baud rate */
    char csrVal;	/* rate to write to the csr reg to get that baud rate */
    } BAUD;

/*
 * BaudTable is a table of the available baud rates, and the values to write
 * to the csr reg to get those rates
 */

LOCAL BAUD baudTable [] =	/* BRG bit = 1 */
    {
    {75,	DUART_CSR_RX_CLK_75    | DUART_CSR_TX_CLK_75 },
    {110,	DUART_CSR_RX_CLK_110   | DUART_CSR_TX_CLK_110 },
    {134,	DUART_CSR_RX_CLK_134_5 | DUART_CSR_TX_CLK_134_5 },
    {150,	DUART_CSR_RX_CLK_150   | DUART_CSR_TX_CLK_150 },
    {300,	DUART_CSR_RX_CLK_300   | DUART_CSR_TX_CLK_300 },
    {600,	DUART_CSR_RX_CLK_600   | DUART_CSR_TX_CLK_600 },
    {1200,	DUART_CSR_RX_CLK_1200  | DUART_CSR_TX_CLK_1200 },
    {2000,	DUART_CSR_RX_CLK_2000  | DUART_CSR_TX_CLK_2000 },
    {2400,	DUART_CSR_RX_CLK_2400  | DUART_CSR_TX_CLK_2400 },
    {4800,	DUART_CSR_RX_CLK_4800  | DUART_CSR_TX_CLK_4800 },
    {1800,	DUART_CSR_RX_CLK_1800  | DUART_CSR_TX_CLK_1800 },
    {9600,	DUART_CSR_RX_CLK_9600  | DUART_CSR_TX_CLK_9600 },
    {19200,	DUART_CSR_RX_CLK_19200 | DUART_CSR_TX_CLK_19200}
    };

IMPORT TY_CO_DEV tyCoDv []; /* device descriptors */

/* locals */

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

/* forward declarations */

LOCAL void tyCoStartup ();
LOCAL int tyCoOpen ();
LOCAL STATUS tyCoIoctl ();
void tyCoInt ();
LOCAL void tyCoHrdInit ();
LOCAL void tyTxInt ();


/*******************************************************************************
*
* tyIerSet - set and clear bits in the DUART's interrupt mask register
*
* This routine sets and clears bits in the MC68340 DUART's IER.
*
* This routine sets and clears bits in a local copy of the IER, then
* writes that local copy to the DUART.  This means that all changes to
* the IER must go through this routine.  Otherwise, any direct changes
* to the IER would be lost the next time this routine is called.
*
* RETURNS: N/A
*/

LOCAL void tyIerSet
    (
    char setBits,	/* which bits to set in the IER */
    char clearBits	/* which bits to clear in the IER */
    )
    {
    static int ier;	/* current value of DUART IER register */

    ier = (ier | setBits) & (~clearBits);
    *tyCoDv [0].ier = ier;
    }

/*******************************************************************************
*
* tyCoDrv - initialize the tty driver
*
* This routine initializes the serial driver, sets up interrupt vectors, and
* performs hardware initialization of the serial ports.
*
* This routine should be called exactly once, before any reads, writes, or
* calls to tyCoDevCreate().  Normally, it is called from usrRoot() in
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
* the proper call would be :
* .CS
*    tyCoDevCreate ("/tyCo/0", 0, 512, 512);
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

    /* check for channel validity */

    if (channel < 0 || channel >= tyCoDv [0].numChannels)
	return (ERROR);

    /* if there is a device already on this channel, don't do it */

    if (tyCoDv [channel].created)
	return (ERROR);

    /* init the ty descriptor and enable transmit & receive interrupts */

    if (tyDevInit (&tyCoDv [channel].tyDev,
		    rdBufSize, wrtBufSize, (FUNCPTR) tyCoStartup) != OK)
	{
	return (ERROR);
	}

    tyIerSet ((tyCoDv [channel].rem | tyCoDv [channel].tem), 0);

    /* mark the device as created, and add the device to the I/O system */

    tyCoDv [channel].created = TRUE;

    return (iosDevAdd (&tyCoDv [channel].tyDev.devHdr, name, tyCoDrvNum));
    }

/*******************************************************************************
*
* tyCoHrdInit - initialize the DUART
*
* This routine initializes the MC68340 DUART.
*
* RETURNS: N/A
*/

LOCAL void tyCoHrdInit (void)
    {
    int lock;

    while ((*tyCoDv [0].isr) & DUART_ISR_XTAL_UNSTABLE)	/* wait for good xtal */
	;

    lock = intLock ();					/* LOCK INTERRUPTS */

    /* 8 data bits, 1 stop bit, no parity, set for 9600 baud */

    *tyCoDv [0].acr  = DUART_ACR_BRG; 			/* select set 2 bauds */

    *tyCoDv [0].cr   = DUART_CR_RST_BRK_INT;
    *tyCoDv [0].cr   = DUART_CR_RST_ERR_STS;
    *tyCoDv [0].cr   = DUART_CR_RST_TX;
    *tyCoDv [0].cr   = DUART_CR_RST_RX;
    *tyCoDv [0].cr   = DUART_CR_RST_MR_PTR;
    *tyCoDv [0].mr1  = DUART_MR1_PAR_NONE | DUART_MR1_8BITS;
    *tyCoDv [0].mr2  = DUART_MR2_STOP_BITS_1;
    *tyCoDv [0].csr  = DUART_CSR_RX_CLK_9600 | DUART_CSR_TX_CLK_9600;

    *tyCoDv [1].cr   = DUART_CR_RST_BRK_INT;
    *tyCoDv [1].cr   = DUART_CR_RST_ERR_STS;
    *tyCoDv [1].cr   = DUART_CR_RST_TX;
    *tyCoDv [1].cr   = DUART_CR_RST_RX;
    *tyCoDv [1].cr   = DUART_CR_RST_MR_PTR;
    *tyCoDv [1].mr1  = DUART_MR1_PAR_NONE | DUART_MR1_8BITS;
    *tyCoDv [1].mr2  = DUART_MR2_STOP_BITS_1;
    *tyCoDv [1].csr  = DUART_CSR_RX_CLK_9600 | DUART_CSR_TX_CLK_9600;

    *tyCoDv [0].sop  = 3;			/* drive A,B CTS* low */

    /* enable the receivers on all channels */

    *tyCoDv [0].cr   = DUART_CR_RX_ENABLE;
    *tyCoDv [1].cr   = DUART_CR_RX_ENABLE;

    /*
     * All interrupts are masked out: interrupts will be enabled in
     * tyCoDevCreate()
     */

    intUnlock (lock);					/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* tyCoOpen - open a file to the DUART
*
* This routine opens a file to the MC68340 DUART.
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
    int request,	/* request code */
    int arg		/* some argument */
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
* tyCoInt - handle interrupts from the DUART
*
* This routine checks whether it is an MC68340 DUART transmit or receive
* interrupt, then does the appropriate handoff.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoInt (void)
    {
    if (*tyCoDv [0].isr & DUART_ISR_TX_RDY_A)
	tyTxInt (0);				    /* transmitter channel A */

    if (*tyCoDv [0].isr & DUART_ISR_TX_RDY_B)
	tyTxInt (1);				    /* transmitter channel B */

    if (*tyCoDv [0].isr & DUART_ISR_RX_RDY_A)
	tyIRd (&tyCoDv [0].tyDev, *tyCoDv [0].dr);  /* receiver channel A */

    if (*tyCoDv [0].isr & DUART_ISR_RX_RDY_B)
	tyIRd (&tyCoDv [1].tyDev, *tyCoDv [1].dr);  /* receiver channel B */
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
	*tyCoDv [channel].cr  = DUART_CR_TX_DISABLE;	/* xmitr off */
    }

/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* This routine starts the MC68340 DUART transmitter.
*
* RETURNS: N/A
*/

LOCAL void tyCoStartup
    (
    TY_CO_DEV *pTyCoDv		/* ty device to start up */
    )
    {
    * pTyCoDv->cr = DUART_CR_TX_ENABLE;		/* enable the transmitter */
    }
