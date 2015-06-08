/* m68360Sio.c - Motorola MC68360 SCC UART serial driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01g,24apr02,pmr  SPR 75161: returning int from m68360Startup() as required.
01f,28may96,dat  fixed SPR #5526, baud rate divisor calculation
01e,10oct95,ms   made poll input routine work if len(RX FIFO)>1 (SPR #5130).
01d,08sep95,myz  fixed the SPR #4678
01c,20jun95,ms   fixed comments for mangen
01b,15jun95,ms   updated for new driver structure
01a,22may95,myz  written (using m68360Serial.c).
*/

/*
DESCRIPTION
This is the driver for the SCC's in the internal Communications Processor (CP)
of the Motorola MC68360.  This driver only supports the SCC's in asynchronous
UART mode.

USAGE
A m68360_CHAN structure is used to describe the chip.
The BSP's sysHwInit() routine typically calls sysSerialHwInit()
which initializes all the values in the M68360_CHAN structure (except
the SIO_DRV_FUNCS) before calling m68360DevInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chips interrupt (m68360Int) via intConnect().

INCLUDE FILES: drv/sio/m68360Sio.h
*/

/* includes */

#include "vxWorks.h"
#include "intLib.h"
#include "errno.h"
#include "sioLib.h"
#include "drv/sio/m68360Sio.h"

/* defines */

#define DEFAULT_BAUD 9600

/* forward declarations */

static STATUS m68360Ioctl (M68360_CHAN *pChan,int request,int arg);
static void   m68360ResetChannel (M68360_CHAN *pChan);
static int    m68360PollOutput (SIO_CHAN *,char);
static int    m68360PollInput (SIO_CHAN *,char *);
static int    m68360Startup (M68360_CHAN *);
static int    m68360CallbackInstall (SIO_CHAN *, int, STATUS (*)(), void *);

/* local driver function table */

static SIO_DRV_FUNCS m68360SioDrvFuncs =
    {
    (int (*)())m68360Ioctl,
    (int (*)())m68360Startup,
    (int (*)())m68360CallbackInstall,
    (int (*)())m68360PollInput,
    (int (*)(SIO_CHAN *,char))m68360PollOutput
    };

/*******************************************************************************
*
* m68360DevInit - initialize the SCC
*
* This routine is called to initialize the chip to a quiescent state.
*/

void m68360DevInit
    (
    M68360_CHAN *pChan
    )
    {
    /* masks off this SCC's interrupt. */

    *M360_CPM_CIMR(pChan->regBase) &=
         (~(CPIC_CIXR_SCC4 << (3 - (pChan->uart.sccNum - 1) )));

    pChan->baudRate = DEFAULT_BAUD;
    pChan->pDrvFuncs = &m68360SioDrvFuncs;
    }

/*******************************************************************************
*
* m68360ResetChannel - initialize the SCC
*/

static void m68360ResetChannel 
    (
    M68360_CHAN *pChan
    )
    {
    int scc;			/* the SCC number being initialized */
    int baud;			/* the baud rate generator being used */
    int frame;

    int oldlevel = intLock ();			/* LOCK INTERRUPTS */
 

    scc = pChan->uart.sccNum - 1;		/* get SCC number */
    baud = pChan->bgrNum - 1;			/* get BRG number */

    pChan->uart.intMask = CPIC_CIXR_SCC4 << (3 - scc);

    /* set up SCC as NMSI */
 
    *M360_CPM_SICR(pChan->regBase) |= (UINT32) (baud << (scc << 3));

    *M360_CPM_SICR(pChan->regBase) |= (UINT32) ((baud << 3) << (scc << 3));
 
    /* reset baud rate generator */
 
    *pChan->pBaud |= BRG_CR_RST;
    while (*pChan->pBaud & BRG_CR_RST);

    m68360Ioctl (pChan, SIO_BAUD_SET, pChan->baudRate);

    /* set up transmit buffer descriptors */

    pChan->uart.txBdBase = (SCC_BUF *) (pChan->regBase +
			 ((UINT32) pChan->uart.txBdBase & 0xfff));

    pChan->uart.pScc->param.tbase = (UINT16) ((UINT32) pChan->uart.txBdBase &
						0xfff);
    pChan->uart.pScc->param.tbptr = (UINT16) ((UINT32) pChan->uart.txBdBase &
						0xfff);
    pChan->uart.txBdNext = 0;

    /* initialize each transmit buffer descriptor */
	
    for (frame = 0; frame < pChan->uart.txBdNum; frame++)
        {
        pChan->uart.txBdBase[frame].statusMode = SCC_UART_TX_BD_INT;

        pChan->uart.txBdBase[frame].dataPointer = pChan->uart.txBufBase +
                                                (frame * pChan->uart.txBufSize);
        }

    /* set the last BD to wrap to the first */

    pChan->uart.txBdBase[(frame - 1)].statusMode |= SCC_UART_TX_BD_WRAP;

    /* set up receive buffer descriptors */

    pChan->uart.rxBdBase = (SCC_BUF *) (pChan->regBase +
		         ((UINT32) pChan->uart.rxBdBase & 0xfff));

    pChan->uart.pScc->param.rbase = (UINT16) ((UINT32) pChan->uart.rxBdBase &
						0xfff);
    pChan->uart.pScc->param.rbptr = (UINT16) ((UINT32) pChan->uart.rxBdBase &
						0xfff);
    pChan->uart.rxBdNext = 0;

    /* initialize each receive buffer descriptor */
	
    for (frame = 0; frame < pChan->uart.rxBdNum; frame++)
        {
        pChan->uart.rxBdBase[frame].statusMode = SCC_UART_RX_BD_EMPTY |
                                               SCC_UART_RX_BD_INT;
        pChan->uart.rxBdBase[frame].dataLength = 1; /* char oriented */
        pChan->uart.rxBdBase[frame].dataPointer = pChan->uart.rxBufBase + frame;
        }

    /* set the last BD to wrap to the first */

    pChan->uart.rxBdBase[(frame - 1)].statusMode |= SCC_UART_TX_BD_WRAP;

    /* set SCC attributes to UART mode */

    pChan->uart.pSccReg->gsmrl = SCC_GSMRL_RDCR_X16 | SCC_GSMRL_TDCR_X16 |
				   SCC_GSMRL_UART;

    pChan->uart.pSccReg->gsmrh = SCC_GSMRH_RFW      | SCC_GSMRH_TFL;
    pChan->uart.pSccReg->psmr  = SCC_UART_PSMR_FLC  | SCC_UART_PSMR_CL_8BIT;
    pChan->uart.pSccReg->dsr	 = 0x7e7e;	/* no fractional stop bits */

    pChan->uart.pScc->param.rfcr  = 0x18;	/* supervisor data access */
    pChan->uart.pScc->param.tfcr  = 0x18;	/* supervisor data access */
    pChan->uart.pScc->param.mrblr = 0x1;	/* one character rx buffers */

    /* initialize parameter the SCC RAM */

    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->maxIdl      = 0x0;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->brkcr       = 0x1;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->parec       = 0x0;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->frmer       = 0x0;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->nosec       = 0x0;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->brkec       = 0x0;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->uaddr1      = 0x0;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->uaddr2      = 0x0;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->toseq       = 0x0;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->character1  = 0x8000;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->character2  = 0x8000;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->character3  = 0x8000;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->character4  = 0x8000;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->character5  = 0x8000;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->character6  = 0x8000;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->character7  = 0x8000;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->character8  = 0x8000;
    ((SCC_UART_PROTO *)pChan->uart.pScc->prot)->rccm        = 0x8000;

    pChan->uart.pSccReg->scce = 0xffff;	/* clr events */

    /* enables the transmitter and receiver  */

    pChan->uart.pSccReg->gsmrl |= SCC_GSMRL_ENR | SCC_GSMRL_ENT;

    /* unmask interrupt */

    pChan->uart.pSccReg->sccm = SCC_UART_SCCX_RX | SCC_UART_SCCX_TX;
    *M360_CPM_CIMR(pChan->regBase) |= pChan->uart.intMask;

    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* m68360Ioctl - special device control
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*
*/

LOCAL STATUS m68360Ioctl
    (
    M68360_CHAN *pChan,	/* device to control */
    int request,	/* request code */
    int arg		/* some argument */
    )
    {
    int baudRate;
    int oldlevel;
    STATUS status = OK;

    switch (request)
	{
	case SIO_BAUD_SET:
            if (arg >=  50 && arg <= 38400)	/* could go higher... */
		{
		/* calculate proper counter value, then enable BRG */

                baudRate = (pChan->clockRate / (16 * arg)) - 1;

		if (baudRate > 0xfff)
                    *pChan->pBaud = (BRG_CR_CD & ((baudRate / 16) << 1)) |
				       BRG_CR_EN | BRG_CR_DIV16;
                else
                    *pChan->pBaud = (BRG_CR_CD & (baudRate << 1)) |
				       BRG_CR_EN;

                pChan->baudRate = arg;
		}
            else
	        status = EIO;
	    break;
    
	case SIO_BAUD_GET:
	    *(int *)arg = pChan->baudRate;
	    break;

	case SIO_MODE_SET:

            if (!((int)arg == SIO_MODE_POLL || (int)arg == SIO_MODE_INT))
                {
                status = EIO;
                break;
                }


            /* lock interrupt  */
            oldlevel = intLock();

            /* initialize channel on first MODE_SET */

            if (!pChan->channelMode)
                m68360ResetChannel(pChan);

            /*
             * if switching from POLL to INT mode, wait for all characters to
             * clear the output pins
             */

            if ((pChan->channelMode == SIO_MODE_POLL) && (arg == SIO_MODE_INT))
                {
		int i;

                for (i=0; i < pChan->uart.txBdNum; i++)
                    while (pChan->uart.txBdBase
                           [(pChan->uart.txBdNext + i) % pChan->uart.txBdNum].
                           statusMode & SCC_UART_TX_BD_READY);

                }

            if (arg == SIO_MODE_INT)
		{
                *M360_CPM_CISR(pChan->regBase) = pChan->uart.intMask
                    ;   /* reset the SCC's interrupt status bit */

		*M360_CPM_CIMR(pChan->regBase) |= pChan->uart.intMask
                    ;   /* enable this SCC's interrupt  */

		pChan->uart.pSccReg->scce = SCC_UART_SCCX_RX
                    ;   /* reset the receiver status bit */ 

                pChan->uart.pSccReg->sccm = SCC_UART_SCCX_RX | SCC_UART_SCCX_TX
                    ;   /* enables the receive and transmit interrupts */
		}
            else
		{
                pChan->uart.pSccReg->sccm = 0
                    ;   /* mask off the receive and transmit interrupts */

		*M360_CPM_CIMR(pChan->regBase) &= (~(pChan->uart.intMask));
                    ;   /* mask off this SCC's interrupt */ 

                }

            pChan->channelMode = arg;

            intUnlock(oldlevel);

            break;

        case SIO_MODE_GET:
            *(int *)arg = pChan->channelMode;
            return (OK);

        case SIO_AVAIL_MODES_GET:
            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
            return (OK);

	default:
	    status = ENOSYS;
	}

    return (status);
    }

/*******************************************************************************
*
* m68360Int - handle an SCC interrupt
*
* This routine gets called to handle SCC interrupts.
*/

void m68360Int
    (
    M68360_CHAN *pChan
    )
    {
    char outChar;
    FAST UINT16 dataLen = 0;

    /* check for a receive event */

    if (pChan->uart.pSccReg->scce & SCC_UART_SCCX_RX)
	{
        pChan->uart.pSccReg->scce = SCC_UART_SCCX_RX;

	while (!(pChan->uart.rxBdBase[pChan->uart.rxBdNext].statusMode &
		 SCC_UART_RX_BD_EMPTY))
	    {
	    /* process all filled receive buffers */

	    outChar = pChan->uart.rxBdBase[pChan->uart.rxBdNext].dataPointer[0];

            pChan->uart.rxBdBase[pChan->uart.rxBdNext].statusMode |=
                SCC_UART_RX_BD_EMPTY;

            /* incr BD count */

            pChan->uart.rxBdNext = (pChan->uart.rxBdNext + 1) %
                                  pChan->uart.rxBdNum;

            /* acknowledge interrupt */

            pChan->uart.pSccReg->scce = SCC_UART_SCCX_RX;

	    (*pChan->putRcvChar) (pChan->putRcvArg,outChar);

	    if (pChan->channelMode == SIO_MODE_POLL)
		break;
	    }
	}

    /* check for a transmit event and if a character needs to be output */

    if ( (pChan->uart.pSccReg->scce & SCC_UART_SCCX_TX) &&
         (pChan->channelMode != SIO_MODE_POLL) )
	{
        pChan->uart.pSccReg->scce = SCC_UART_SCCX_TX;
    
        if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK)
	    {
	    do
	        {
	        pChan->uart.txBdBase[pChan->uart.txBdNext].dataPointer[dataLen++]
		    = outChar;

                if (pChan->channelMode == SIO_MODE_POLL)
                    break;
	        }
	    while ((dataLen < pChan->uart.txBufSize) &&
                   ((*pChan->getTxChar) (pChan->getTxArg, &outChar)
		       == OK));

	    pChan->uart.txBdBase[pChan->uart.txBdNext].dataLength  = dataLen;

            /* acknowledge interrupt */

            pChan->uart.pSccReg->scce = SCC_UART_SCCX_TX;

	    /* send transmit buffer */

	    pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode |=
	        SCC_UART_TX_BD_READY;

	    /* incr BD count */

 	    pChan->uart.txBdNext = (pChan->uart.txBdNext+ 1) % pChan->uart.txBdNum;
	    }
	}

    /* acknowledge all other interrupts - ignore events */

    pChan->uart.pSccReg->scce = (pChan->uart.pSccReg->scce & ~(SCC_UART_SCCX_RX |
                               SCC_UART_SCCX_TX));

    *M360_CPM_CISR(pChan->regBase) = pChan->uart.intMask;
    }

/*******************************************************************************
*
* m68360Startup - transmitter startup routine
*/

static int m68360Startup
    (
    M68360_CHAN *pChan		/* ty device to start up */
    )
    {
    char outChar;
    FAST UINT16 dataLen = 0;

    if (pChan->channelMode == SIO_MODE_POLL)
	return (ENOSYS);

    /* check if buffer is ready and if a character needs to be output */

    if ((!(pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode &
	   SCC_UART_TX_BD_READY)) &&
        ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK))
	{
	do
	    {
	    pChan->uart.txBdBase[pChan->uart.txBdNext].dataPointer[dataLen++] =
		outChar;
	    }
	while ((dataLen < pChan->uart.txBufSize) &&
               ((*pChan->getTxChar) (pChan->getTxArg, &outChar)
               == OK));	/* fill buffer */

	/* send transmit buffer */

	pChan->uart.txBdBase[pChan->uart.txBdNext].dataLength  = dataLen;
	pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode |=
	    SCC_UART_TX_BD_READY;

	/* incr BD count */

        pChan->uart.txBdNext = (pChan->uart.txBdNext + 1) % pChan->uart.txBdNum;
	}

    return (OK);
    }

/******************************************************************************
*
* m68360PollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the input buffer is empty.
*/

static int m68360PollInput
    (
    SIO_CHAN *   pSioChan,
    char *      thisChar
    )
    {
    M68360_CHAN * pChan = (M68360_CHAN *)pSioChan;

    if ( !(pChan->uart.pSccReg->scce & SCC_UART_SCCX_RX) )
        return (EAGAIN); 

    if (pChan->uart.rxBdBase[pChan->uart.rxBdNext].statusMode &
	 SCC_UART_RX_BD_EMPTY)
        return (EAGAIN);

    /* get a character */

    *thisChar = pChan->uart.rxBdBase[pChan->uart.rxBdNext].dataPointer[0];

    /* set the empty bit */

    pChan->uart.rxBdBase[pChan->uart.rxBdNext].statusMode |=
                SCC_UART_RX_BD_EMPTY;

    /* incr BD count */

    pChan->uart.rxBdNext = (pChan->uart.rxBdNext + 1) %
                                  pChan->uart.rxBdNum;

    /* only clear RX event if no more characters are ready */

    if (pChan->uart.rxBdBase[pChan->uart.rxBdNext].statusMode &
         SCC_UART_RX_BD_EMPTY)
        pChan->uart.pSccReg->scce = SCC_UART_SCCX_RX;

    return (OK);
    }

/******************************************************************************
*
* m68360PollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the output buffer if full.
*/

static int m68360PollOutput
    (
    SIO_CHAN *   pSioChan,
    char        outChar
    )
    {
    M68360_CHAN * pChan = (M68360_CHAN *)pSioChan;

    /* is the transmitter ready to accept a character? */

    if (pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode &
	SCC_UART_TX_BD_READY)
	return(EAGAIN);

    /* reset the transmitter status bit */

    pChan->uart.pSccReg->scce = SCC_UART_SCCX_TX;

    /* write out the character */

    pChan->uart.txBdBase[pChan->uart.txBdNext].dataPointer[0] = outChar;

    pChan->uart.txBdBase[pChan->uart.txBdNext].dataLength  = 1;

    /* send transmit buffer */

    pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode |=
							SCC_UART_TX_BD_READY;

    pChan->uart.txBdNext = (pChan->uart.txBdNext + 1) % pChan->uart.txBdNum;

    return (OK);
    }
/******************************************************************************
*
* m68360CallbackInstall - install ISR callbacks to get put chars.
*/

static int m68360CallbackInstall
    (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS      (*callback)(),
    void *      callbackArg
    )
    {
    M68360_CHAN * pChan = (M68360_CHAN *)pSioChan;

    switch (callbackType)
        {
        case SIO_CALLBACK_GET_TX_CHAR:
            pChan->getTxChar    = callback;
            pChan->getTxArg     = callbackArg;
            return (OK);
        case SIO_CALLBACK_PUT_RCV_CHAR:
            pChan->putRcvChar   = callback;
            pChan->putRcvArg    = callbackArg;
            return (OK);
        default:
            return (ENOSYS);
        }
    }

