/* m68302Serial.c - Motorola MC68302 tty driver*/

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01m,06aug97,dat  fixed race condition SPR 5578
01l,01dec92,jdi  NOMANUAL for tyCoInt() - SPR 1808.
01k,28oct92,caf  tweaked tyCoDevCreate() documentation and parm checking.
01j,22oct92,jcf  increased transmit buffer size.
01i,10aug92,caf  ansified.
01h,28jul92,caf  fixed ansi warnings.
01g,18jul92,smb  Changed errno.h to errnoLib.h.
01f,26may92,rrr  the tree shuffle
01e,24jan92,jdi  documentation cleanup.
01d,08jan92,caf  reduced size of transmit buffers (SPR 1243).
		 fixed baud rate in tyCoHrdInit() (SPR 1262).
01c,04oct91,rrr  passed through the ansification filter
		  -changed VOID to void
		  -changed copyright notice
01b,26sep91,jdi  documentation cleanup.
01a,19sep91,jcf  written.
*/

/*
DESCRIPTION
This is the driver for the internal Communications Processor (CP)
of the Motorola MC68302.

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
more information, see the manual entry for tyLib.  The available baud
rates are 300, 600, 1200, 2400, 4800, 9600 and 19200.

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
#include "config.h"

/* defines */

#define DEFAULT_BAUD 9600

/* globals */

IMPORT TY_CO_DEV tyCoDv [];	/* device descriptors */

/* locals */

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

typedef struct		/* BAUD */
    {
    int	   clockRate;		/* system clock rate */
    int	   baudRate;		/* a baud rate */
    UINT16 timeConstant;	/* time constant for scc scon register */
    } BAUD;

LOCAL BAUD baudTable [] =	/* three CPU clock rates are supported */
    {
    /* 20.0 MHz CPU clock */

    {20000000,	300,	0x823}, {20000000,	600,	0x40f},
    {20000000,	1200,	0x822}, {20000000,	2400,	0x410},
    {20000000,	4800,	0x206}, {20000000,	9600,	0x102},
    {20000000,	19200,	0x080},

    /* 16.7 MHz CPU clock */

    {16666666,	300,	0x6c7}, {16666666,	600,	0xd8e},
    {16666666,	1200,	0x6c6}, {16666666,	2400,	0x362},
    {16666666,	4800,	0x1b0}, {16666666,	9600,	0xd8},
    {16666666,	19200,	0x6a},

    /* 16 MHz CPU clock */

    {16000000,	300,	0x683}, {16000000,	600,	0xd04},
    {16000000,	1200,	0x682}, {16000000,	2400,	0x340},
    {16000000,	4800,	0x19e}, {16000000,	9600,	0xce},
    {16000000,	19200,	0x66}
    };

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

    /* enable the receiver and transmitter of the channel's scc */

    tyCoDv[channel].pSccReg->scm |= UART_SCM_ENR | UART_SCM_ENT;

    /* mark the device as created, and add the device to the I/O system */

    tyCoDv[channel].created = TRUE;
    return (iosDevAdd (&tyCoDv[channel].tyDev.devHdr, name, tyCoDrvNum));
    }

/*******************************************************************************
*
* tyCoHrdInit - initialize the USART
*/

LOCAL void tyCoHrdInit (void)
    {
    int channel;
    int frame;
    int iy;
    TY_CO_DEV *pDv;
    int oldlevel = intLock ();			/* LOCK INTERRUPTS */

    *IMP_CR = (CR_RST | CR_FLG);		/* RESET the CP */
    while (*IMP_CR != 0) 			/* Wait for reset to finish */
	;

    *IMP_SIMASK = 0xffff;
    *IMP_SIMODE	= 0x0000;			/* NMSI for all channels */

    for (channel = 0; channel < NUM_TTY; channel ++)
	{
	pDv = &tyCoDv[channel];			/* pDv points to device */

	for (iy = 0; iy < NELEMENTS (baudTable); iy++)
	    {
	    if ((baudTable [iy].clockRate == tyCoDv [0].clockRate) &&
		(baudTable [iy].baudRate == DEFAULT_BAUD))
		{				/* set default baud rate */
		pDv->pSccReg->scon = baudTable [iy].timeConstant;
		break;
		}
	    }

	pDv->pSccReg->scm	= UART_SCM_ASYNC | UART_SCM_MANUAL |
				  UART_SCM_8BIT | UART_SCM_RTSM;

	pDv->pSccReg->dsr	= 0x7e7e;	/* no fractional stop bits */
	pDv->pScc->param.rfcr	= 0x50;		/* sup. data access */
	pDv->pScc->param.tfcr	= 0x50;		/* sup. data access */
	pDv->pScc->param.mrblr	= 0x1;		/* one character rx buffers */


 	/* next receive buffer */
	pDv->rxBdNext		= 0;

	for (frame = 0; frame < 8; frame ++)
	    {
	    pDv->pScc->rxBd[frame].statusMode	= UART_RX_BD_EMPTY |
						  UART_RX_BD_INT;
	    pDv->pScc->rxBd[frame].dataLength	= 1;
	    pDv->pScc->rxBd[frame].dataPointer	= (char *)((channel << 3) +
						           frame + 0x180 +
							   (int)IMP_BASE_ADRS);
	    }

	pDv->pScc->rxBd[7].statusMode  |= UART_RX_BD_WRAP;

	pDv->pScc->txBd[0].statusMode	= UART_TX_BD_INT | UART_TX_BD_WRAP;
	pDv->pScc->txBd[0].dataLength	= 0x80;
	pDv->pScc->txBd[0].dataPointer	= (char *)(channel * 0x80 +
						   (int)IMP_BASE_ADRS);

	((PROT_UART *)pDv->pScc->prot)->maxIdl		= 0x0;
	((PROT_UART *)pDv->pScc->prot)->idlc		= 0x0;
	((PROT_UART *)pDv->pScc->prot)->brkcr		= 0x0;
	((PROT_UART *)pDv->pScc->prot)->parec		= 0x0;
	((PROT_UART *)pDv->pScc->prot)->frmec		= 0x0;
	((PROT_UART *)pDv->pScc->prot)->nosec		= 0x0;
	((PROT_UART *)pDv->pScc->prot)->brkec		= 0x0;
	((PROT_UART *)pDv->pScc->prot)->uaddr1		= 0x0;
	((PROT_UART *)pDv->pScc->prot)->uaddr2		= 0x0;
	((PROT_UART *)pDv->pScc->prot)->cntChar1	= 0x8000;

	pDv->pSccReg->scce = 0xff;				/* clr events */
	pDv->pSccReg->sccm = UART_SCCE_RX | UART_SCCE_TX;	/* unmask int */
	}

    *IMP_IMR |= INT_SCC1 | INT_SCC2 | INT_SCC3;			/* unmask int */

    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
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
* This routine handles FIOBAUDRATE requests and passes all others to tyIoctl.
*
* RETURNS: OK or ERROR if invalid baud rate, or whatever tyIoctl returns.
*/

LOCAL STATUS tyCoIoctl
    (
    TY_CO_DEV *pTyCoDv,	/* device to control */
    int request,	/* request code */
    int arg		/* some argument */
    )
    {
    int ix;
    UINT16 scon;
    STATUS status;

    switch (request)
	{
	case FIOBAUDRATE:

	    status = ERROR;
	    for (ix = 0; ix < NELEMENTS (baudTable); ix++)
		{
		if ((baudTable [ix].clockRate == tyCoDv [0].clockRate) &&
		    (baudTable [ix].baudRate == arg))
		    {
		    scon  = pTyCoDv->pSccReg->scon & 0xf000;
		    scon |= baudTable [ix].timeConstant;
		    pTyCoDv->pSccReg->scon = scon;
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
    FAST UINT16   dataLen = 0;
    FAST UINT8    event = pDv->pSccReg->scce;

    /* acknowledge interrupt */

    pDv->pSccReg->scce	= event;		/* clear events */
    *IMP_ISR		= pDv->intAck;		/* ack. interrupt */

    if (!pDv->created)
	return;

    if ((event & UART_SCCE_TX) && (tyITx (&pDv->tyDev, &outChar) == OK))
	{
	do
	    {
	    pDv->pScc->txBd[0].dataPointer[dataLen++] = outChar;
	    }
	while ((dataLen < 0x80) && (tyITx (&pDv->tyDev, &outChar) == OK));

	pDv->pScc->txBd[0].dataLength  = dataLen;
	pDv->pScc->txBd[0].statusMode |= UART_TX_BD_READY;
	}

    if (event & UART_SCCE_RX)
	while (!(pDv->pScc->rxBd[pDv->rxBdNext].statusMode & UART_RX_BD_EMPTY))
	    {
	    tyIRd (&pDv->tyDev, pDv->pScc->rxBd[pDv->rxBdNext].dataPointer[0]);
	    pDv->pScc->rxBd[pDv->rxBdNext].statusMode |= UART_RX_BD_EMPTY;
	    pDv->rxBdNext = (pDv->rxBdNext + 1) & 0x7;
	    }

    /* all other events ignored */
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
    FAST int dataLen = 0;

    if ((!(pDv->pScc->txBd[0].statusMode & UART_TX_BD_READY)) &&
        (tyITx (&pDv->tyDev, &outChar) == OK))
	{
	do
	    {
	    pDv->pScc->txBd[0].dataPointer[dataLen++] = outChar;
	    }
	while ((dataLen < 0x80) && (tyITx (&pDv->tyDev, &outChar) == OK));

	pDv->pScc->txBd[0].dataLength  = dataLen;
	pDv->pScc->txBd[0].statusMode |= UART_TX_BD_READY;
	}
    }
