/* m68360Serial.c - Motorola MC68360 SCC UART tty driver */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,28may96,dat  fixed SPR #5526, #3689, #3517
01d,12nov93,dzb  fixed event race condition in tyCoInt() (SPR #2632).
01c,05oct93,dzb  tweaks for abstraction of SCC_DEV.  removed CP init call.
01b,23aug93,dzb  tweaked to accept multiple NMSI settings.
01a,22jul93,dzb  derived from version 01l of serial/m68302Serial.c.
*/

/*
DESCRIPTION
This is the driver for the SCC's in the internal Communications Processor (CP)
of the Motorola MC68360.  This driver only supports the SCC's in asynchronous
UART mode.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling the
routine tyCoDrv().  This routine should be called exactly once, before any
reads, writes, or calls to tyCoDevCreate().  Normally, it is called from
usrRoot() in usrConfig.c.

Before a terminal can be used, it must be created using tyCoDevCreate().
Each port to be used should have exactly one device associated with it by
calling this routine.

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal tty driver; for
more information, see the manual entry for tyLib.  Available baud
rates range from 50 to 38400.

SEE ALSO
tyLib
*/

/* includes */

#include "vxWorks.h"
#include "iosLib.h"
#include "memLib.h"
#include "tyLib.h"
#include "iv.h"
#include "intLib.h"
#include "errnoLib.h"
#include "drv/multi/m68360.h"

/* defines */

#define DEFAULT_BAUD 9600

/* globals */

IMPORT TY_CO_DEV tyCoDv [];	/* device descriptors */

/* locals */

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

/* forward declarations */

LOCAL void	tyCoStartup ();
LOCAL int	tyCoOpen ();
LOCAL STATUS	tyCoIoctl ();
void		tyCoInt ();
LOCAL void	tyCoHrdInit ();

/*******************************************************************************
*
* tyCoDrv - initialize the tty driver
*
* This routine initializes the serial driver, sets up interrupt vectors,
* and performs hardware initialization of the serial ports.
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
* For instance, to create the device "/tyCo/0", with buffer sizes of 512 bytes,
* the proper call would be:
* .CS
*    tyCoDevCreate ("/tyCo/0", 0, 512, 512);
* .CE
*
* RETURNS:
* OK, or ERROR if the driver is not installed, the channel is invalid, or
* the device already exists.
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

    if (tyCoDv[channel].created )
	return (ERROR);

    if (tyDevInit (&tyCoDv[channel].tyDev, rdBufSize, wrtBufSize,
		   (FUNCPTR) tyCoStartup)!= OK)
	return (ERROR);

    /* enable the receiver and transmitter of the channel's SCC */

    tyCoDv[channel].uart.pSccReg->gsmrl |= SCC_GSMRL_ENR | SCC_GSMRL_ENT;

    /* mark the device as created, and add the device to the I/O system */

    tyCoDv[channel].created = TRUE;
    return (iosDevAdd (&tyCoDv[channel].tyDev.devHdr, name, tyCoDrvNum));
    }

/*******************************************************************************
*
* tyCoHrdInit - initialize the SCC
*/

LOCAL void tyCoHrdInit (void)
    {
    int scc;			/* the SCC number being initialized */
    int baud;			/* the baud rate generator being used */
    int channel;
    int frame;
    TY_CO_DEV *pDv;
    int oldlevel = intLock ();			/* LOCK INTERRUPTS */
 
    for (channel = 0; channel < tyCoDv[0].numChannels; channel ++)
	{
	pDv = &tyCoDv[channel];			/* pDv points to device */

        scc = pDv->uart.sccNum - 1;		/* get SCC number */
        baud = pDv->bgrNum - 1;			/* get BRG number */

        /* set up relevant addresses in the serial descriptor */

        pDv->uart.pScc = (SCC *) ((UINT32) M360_DPR_SCC1(pDv->regBase) +
                                  (scc * 0x100));
        pDv->uart.pSccReg = (SCC_REG *) ((UINT32) M360_CPM_GSMR_L1(pDv->regBase)
					 + (scc * 0x20));
        pDv->pBaud = (UINT32 *) (M360_CPM_BRGC1(pDv->regBase) + baud);
        pDv->uart.intMask = CPIC_CIXR_SCC4 << (3 - scc);

        /* set up SCC as NMSI */
 
        *M360_CPM_SICR(pDv->regBase) |= (UINT32) (baud << (scc << 3));
        *M360_CPM_SICR(pDv->regBase) |= (UINT32) ((baud << 3) << (scc << 3));
 
        /* reset baud rate generator */
 
        *pDv->pBaud |= BRG_CR_RST;
        while (*pDv->pBaud & BRG_CR_RST);

        /* set BRG at default baud rate */

        tyCoIoctl (pDv, FIOBAUDRATE, DEFAULT_BAUD);

        /* set up transmit buffer descriptors */

        pDv->uart.txBdBase = (SCC_BUF *) (pDv->regBase +
					 ((UINT32) pDv->uart.txBdBase & 0xfff));

        pDv->uart.pScc->param.tbase = (UINT16) ((UINT32) pDv->uart.txBdBase &
						0xfff);
        pDv->uart.pScc->param.tbptr = (UINT16) ((UINT32) pDv->uart.txBdBase &
						0xfff);
        pDv->uart.txBdNext = 0;

        /* initialize each transmit buffer descriptor */
	
	for (frame = 0; frame < pDv->uart.txBdNum; frame++)
            {
	    pDv->uart.txBdBase[frame].statusMode = SCC_UART_TX_BD_INT;
	    pDv->uart.txBdBase[frame].dataPointer = pDv->uart.txBufBase +
                (frame * pDv->uart.txBufSize);
            }

	/* set the last BD to wrap to the first */

	pDv->uart.txBdBase[(frame - 1)].statusMode |= SCC_UART_TX_BD_WRAP;

        /* set up receive buffer descriptors */

        pDv->uart.rxBdBase = (SCC_BUF *) (pDv->regBase +
				         ((UINT32) pDv->uart.rxBdBase & 0xfff));

        pDv->uart.pScc->param.rbase = (UINT16) ((UINT32) pDv->uart.rxBdBase &
						0xfff);
        pDv->uart.pScc->param.rbptr = (UINT16) ((UINT32) pDv->uart.rxBdBase &
						0xfff);
        pDv->uart.rxBdNext = 0;

        /* initialize each receive buffer descriptor */
	
	for (frame = 0; frame < pDv->uart.rxBdNum; frame++)
	    {
	    pDv->uart.rxBdBase[frame].statusMode = SCC_UART_RX_BD_EMPTY |
                                                   SCC_UART_RX_BD_INT;
	    pDv->uart.rxBdBase[frame].dataLength = 1; /* char oriented */
	    pDv->uart.rxBdBase[frame].dataPointer = pDv->uart.rxBufBase + frame;
	    }

	/* set the last BD to wrap to the first */

	pDv->uart.rxBdBase[(frame - 1)].statusMode |= SCC_UART_TX_BD_WRAP;

	/* set SCC attributes to UART mode */

	pDv->uart.pSccReg->gsmrl = SCC_GSMRL_RDCR_X16 | SCC_GSMRL_TDCR_X16 |
				   SCC_GSMRL_UART;
	pDv->uart.pSccReg->gsmrh = SCC_GSMRH_RFW      | SCC_GSMRH_TFL;
        pDv->uart.pSccReg->psmr  = SCC_UART_PSMR_FLC  | SCC_UART_PSMR_CL_8BIT;
	pDv->uart.pSccReg->dsr	 = 0x7e7e;	/* no fractional stop bits */

	pDv->uart.pScc->param.rfcr  = 0x18;	/* supervisor data access */
	pDv->uart.pScc->param.tfcr  = 0x18;	/* supervisor data access */
	pDv->uart.pScc->param.mrblr = 0x1;	/* one character rx buffers */

	/* initialize parameter the SCC RAM */

        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->maxIdl      = 0x0;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->brkcr       = 0x1;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->parec       = 0x0;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->frmer       = 0x0;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->nosec       = 0x0;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->brkec       = 0x0;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->uaddr1      = 0x0;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->uaddr2      = 0x0;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->toseq       = 0x0;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->character1  = 0x8000;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->character2  = 0x8000;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->character3  = 0x8000;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->character4  = 0x8000;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->character5  = 0x8000;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->character6  = 0x8000;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->character7  = 0x8000;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->character8  = 0x8000;
        ((SCC_UART_PROTO *)pDv->uart.pScc->prot)->rccm        = 0x8000;

	pDv->uart.pSccReg->scce = 0xffff;	/* clr events */

        /* unmask interrupt */

	pDv->uart.pSccReg->sccm = SCC_UART_SCCX_RX | SCC_UART_SCCX_TX;
        *M360_CPM_CIMR(pDv->regBase) |= pDv->uart.intMask;
	}

    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* tyCoOpen - open file to SCC
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
* RETURNS: OK or ERROR if invalid baud rate, or whatever tyIoctl() returns.
*/

LOCAL STATUS tyCoIoctl
    (
    TY_CO_DEV *pTyCoDv,	/* device to control */
    int request,	/* request code */
    int arg		/* some argument */
    )
    {
    int baudRate;
    STATUS status = OK;

    switch (request)
	{
	case FIOBAUDRATE:
            if (arg >=  50 && arg <= 38400)	/* could go higher... */
		{
		/* calculate proper counter value, then enable BRG */

                baudRate = (pTyCoDv->clockRate / (16 * arg)) - 1;

		if (baudRate > 0xfff)
                    *pTyCoDv->pBaud = (BRG_CR_CD & ((baudRate / 16) << 1)) |
				       BRG_CR_EN | BRG_CR_DIV16;
                else
                    *pTyCoDv->pBaud = (BRG_CR_CD & (baudRate << 1)) |
				       BRG_CR_EN;
		}
            else
	        status = ERROR;
	    break;

	/* pass all other requests to tyIoctl() */

	default:
	    status = tyIoctl (&pTyCoDv->tyDev, request, arg);
	    break;
	}

    return (status);
    }

/*******************************************************************************
*
* tyCoInt - handle a SCC interrupt
*
* This routine gets called to handle SCC interrupts.
*
* NOMANUAL
*/

void tyCoInt
    (
    TY_CO_DEV *pDv
    )
    {
    char outChar;
    FAST UINT16 dataLen = 0;
    int event;

    if (!pDv->created)
	{
        pDv->uart.pSccReg->scce = 0xffff;	/* acknowledge interrupt */

        *M360_CPM_CISR(pDv->regBase) = pDv->uart.intMask;
	return;
	}

    /* get event bits and acknowledge them */

    event = pDv->uart.pSccReg->scce;
    pDv->uart.pSccReg->scce = event;

    /* check for a receive event */

    if (event & SCC_UART_SCCX_RX)
	{
	while (!(pDv->uart.rxBdBase[pDv->uart.rxBdNext].statusMode &
		 SCC_UART_RX_BD_EMPTY))
	    {
	    /* process all filled receive buffers */

	    tyIRd (&pDv->tyDev,
                   pDv->uart.rxBdBase[pDv->uart.rxBdNext].dataPointer[0]);
	    pDv->uart.rxBdBase[pDv->uart.rxBdNext].statusMode |=
		SCC_UART_RX_BD_EMPTY;

	    /* incr BD count */

	    pDv->uart.rxBdNext = (pDv->uart.rxBdNext + 1) % pDv->uart.rxBdNum;
	    }
	}

    /* check for a transmit event and if a character needs to be output */

    if (event & SCC_UART_SCCX_TX)
	{
        if (tyITx (&pDv->tyDev, &outChar) == OK)
	    {
	    do
	        {
	        pDv->uart.txBdBase[pDv->uart.txBdNext].dataPointer[dataLen++] =
                    outChar;
	        }
	    while ((dataLen < pDv->uart.txBufSize) &&
                   (tyITx (&pDv->tyDev, &outChar) == OK));

	    pDv->uart.txBdBase[pDv->uart.txBdNext].dataLength  = dataLen;

	    /* send transmit buffer */

	    pDv->uart.txBdBase[pDv->uart.txBdNext].statusMode |=
	        SCC_UART_TX_BD_READY;

	    /* incr BD count */

 	    pDv->uart.txBdNext = (pDv->uart.txBdNext + 1) % pDv->uart.txBdNum;
	    }
	}

    *M360_CPM_CISR(pDv->regBase) = pDv->uart.intMask;
    }

/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*/

LOCAL void tyCoStartup
    (
    TY_CO_DEV *pDv		/* ty device to start up */
    )
    {
    char outChar;
    FAST UINT16 dataLen = 0;

    /* check if buffer is ready and if a character needs to be output */

    if ((!(pDv->uart.txBdBase[pDv->uart.txBdNext].statusMode &
	   SCC_UART_TX_BD_READY)) &&
        (tyITx (&pDv->tyDev, &outChar) == OK))
	{
	do
	    {
	    pDv->uart.txBdBase[pDv->uart.txBdNext].dataPointer[dataLen++] =
		outChar;
	    }
	while ((dataLen < pDv->uart.txBufSize) &&
               (tyITx (&pDv->tyDev, &outChar) == OK));	/* fill buffer */

	/* send transmit buffer */

	pDv->uart.txBdBase[pDv->uart.txBdNext].dataLength  = dataLen;
	pDv->uart.txBdBase[pDv->uart.txBdNext].statusMode |=
	    SCC_UART_TX_BD_READY;

	/* incr BD count */

        pDv->uart.txBdNext = (pDv->uart.txBdNext + 1) % pDv->uart.txBdNum;
	}
    }
