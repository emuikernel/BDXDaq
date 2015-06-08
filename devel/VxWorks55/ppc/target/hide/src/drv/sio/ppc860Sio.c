/* ppc860Sio.c - Motorola MPC800 SMC UART serial driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,24apr02,pmr  SPR 75161: returning int from ppc860Startup().
01b,06nov96,tpr  removed DEBUG code.
01c,08nov96,dgp  doc: final formatting
01b,28oct96,tam	 fixed typo error to build man pages. 
01a,14apr96,cah	 adapted from m68360.c and updated for SMC part
*/

/*
DESCRIPTION
This is the driver for the SMCs in the internal Communications Processor (CP)
of the Motorola MPC68860/68821.  This driver only supports the SMCs in 
asynchronous UART mode.

USAGE
A PPC800SMC_CHAN structure is used to describe the chip.
The BSP's sysHwInit() routine typically calls sysSerialHwInit(),
which initializes all the values in the PPC860SMC_CHAN structure (except
the SIO_DRV_FUNCS) before calling ppc860DevInit().

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chip's interrupts via intConnect().

INCLUDE FILES: drv/sio/ppc860Sio.h
*/

/* includes */

#include "vxWorks.h"
#include "intLib.h"
#include "errno.h"
#include "sioLib.h"
#include "drv/multi/ppc860Siu.h"
#include "drv/multi/ppc860Cpm.h"
#include "drv/sio/ppc860Sio.h"

/* defines */

#define DEFAULT_BAUD 9600

/* forward declarations */

static STATUS ppc860Ioctl (PPC860SMC_CHAN *pChan,int request,int arg);
static void   ppc860ResetChannel (PPC860SMC_CHAN *pChan);
static int    ppc860PollOutput (SIO_CHAN *,char);
static int    ppc860PollInput (SIO_CHAN *,char *);
static int    ppc860Startup (PPC860SMC_CHAN *);
static int    ppc860CallbackInstall (SIO_CHAN *, int, STATUS (*)(), void *);

/* local driver function table */

static SIO_DRV_FUNCS ppc860SioDrvFuncs =
    {
    (int (*)())			ppc860Ioctl,
    (int (*)())			ppc860Startup,
    (int (*)())			ppc860CallbackInstall,
    (int (*)())			ppc860PollInput,
    (int (*)(SIO_CHAN *,char))	ppc860PollOutput
    };

/*******************************************************************************
*
* ppc860DevInit - initialize the SMC
*
* This routine is called to initialize the chip to a quiescent state.
* Note that the `smcNum' field of PPC860SMC_CHAN must be either 1 or 2.
*/

void ppc860DevInit
    (
    PPC860SMC_CHAN *pChan
    )
    {
    /* masks off this SMC's interrupt. */
	
    * CIMR(pChan->regBase) &= (~(CIMR_SMC1 >> (pChan->uart.smcNum - 1)));

    pChan->baudRate  = DEFAULT_BAUD;
    pChan->pDrvFuncs = &ppc860SioDrvFuncs;
    }

/*******************************************************************************
*
* ppc860ResetChannel - initialize the SMC
*/

static void ppc860ResetChannel 
    (
    PPC860SMC_CHAN *pChan
    )
    {
    int smc;			/* the SMC number being initialized */
    int baud;			/* the baud rate generator being used */
    int frame;

    int oldlevel = intLock ();	/* lock interrupts */ 

    smc  = pChan->uart.smcNum - 1;		/* get SMC number */
    baud = pChan->bgrNum - 1;			/* get BRG number */

    pChan->uart.intMask = CIMR_SMC1 >> smc;

    /* set up SMC as NMSI, select Baud Rate Generator */

    switch( baud ) 
	{
	default:	/* default to BRG1 */
	case 0: 
	    * SIMODE(pChan->regBase) |= (SIMODE_SMC1CS_BRG1 << (16 * smc));
	    break;
	case 1: 
	    * SIMODE(pChan->regBase) |= (SIMODE_SMC1CS_BRG2 << (16 * smc));
	    break;
	case 2: 
	    * SIMODE(pChan->regBase) |= (SIMODE_SMC1CS_BRG3 << (16 * smc));
	    break;
	case 3: 
	    * SIMODE(pChan->regBase) |= (SIMODE_SMC1CS_BRG4 << (16 * smc));
	    break;
        }
 
    /* reset baud rate generator, wait for reset to clear... */
 
    *pChan->pBaud |= BRGC_RST;
    while (*pChan->pBaud & BRGC_RST);

    ppc860Ioctl (pChan, SIO_BAUD_SET, pChan->baudRate);

    /* set up transmit buffer descriptors */

    pChan->uart.txBdBase = (SMC_BUF *) (pChan->regBase +
			 ((UINT32) pChan->uart.txBdBase ));
    pChan->uart.pSmc->param.tbase = (UINT16) ((UINT32) pChan->uart.txBdBase);
    pChan->uart.pSmc->param.tbptr = (UINT16) ((UINT32) pChan->uart.txBdBase);
    pChan->uart.txBdNext = 0;

    /* initialize each transmit buffer descriptor */

    for (frame = 0; frame < pChan->uart.txBdNum; frame++)
        {
        pChan->uart.txBdBase[frame].statusMode = BD_TX_INTERRUPT_BIT;

        pChan->uart.txBdBase[frame].dataPointer = pChan->uart.txBufBase +
                                                (frame * pChan->uart.txBufSize);
        }

    /* set the last BD to wrap to the first */

    pChan->uart.txBdBase[(frame - 1)].statusMode |= BD_TX_WRAP_BIT;

    /* set up receive buffer descriptors */

    pChan->uart.rxBdBase = (SMC_BUF *) (pChan->regBase +
		         ((UINT32) pChan->uart.rxBdBase ));

    pChan->uart.pSmc->param.rbase = (UINT16) ((UINT32) pChan->uart.rxBdBase);
    pChan->uart.pSmc->param.rbptr = (UINT16) ((UINT32) pChan->uart.rxBdBase);
    pChan->uart.rxBdNext = 0;

    /* initialize each receive buffer descriptor */
	
    for (frame = 0; frame < pChan->uart.rxBdNum; frame++)
        {
        pChan->uart.rxBdBase[frame].statusMode = BD_RX_EMPTY_BIT |
						 BD_RX_INTERRUPT_BIT;
        pChan->uart.rxBdBase[frame].dataLength = 1; /* char oriented */
        pChan->uart.rxBdBase[frame].dataPointer = pChan->uart.rxBufBase + frame;
        }

    /* set the last BD to wrap to the first */

    pChan->uart.rxBdBase[(frame - 1)].statusMode |= BD_RX_WRAP_BIT;

    /* set SMC attributes to standard UART mode */

    pChan->uart.pSmcReg->smcmr = SMCMR_STD_MODE;

    /* initialize parameter RAM area for this SMC */

    pChan->uart.pSmc->param.rfcr   = 0x18;	/* supervisor data access */
    pChan->uart.pSmc->param.tfcr   = 0x18;	/* supervisor data access */
    pChan->uart.pSmc->param.mrblr  = 0x1;	/* one character rx buffers */
    pChan->uart.pSmc->param.maxidl = 0x0;	/* no idle features */
    pChan->uart.pSmc->param.brkln  = 0x0;	/* no breaks received yet */
    pChan->uart.pSmc->param.brkec  = 0x0;	/* zero break condition ctr */
    pChan->uart.pSmc->param.brkcr  = 0x1;	/* xmit 1 BRK on stop */

    /* clear all events */

    pChan->uart.pSmcReg->smce = SMCE_ALL_EVENTS;

    /* enables the transmitter and receiver  */

    pChan->uart.pSmcReg->smcmr |= SMCMR_TEN | SMCMR_REN;

    /* unmask interrupt (Tx, Rx only) */

    pChan->uart.pSmcReg->smcm  = SMCM_TX_MSK | SMCM_RX_MSK;
    *CIMR(pChan->regBase) |= pChan->uart.intMask;

    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* ppc860Ioctl - special device control
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*
*/

LOCAL STATUS ppc860Ioctl
    (
    PPC860SMC_CHAN *	pChan,		/* device to control */
    int			request,	/* request code */
    int			arg		/* some argument */
    )
    {
    int		baudRate;
    int 	oldlevel;
    STATUS 	status = OK;

    switch (request)
	{
	case SIO_BAUD_SET:
            if (arg >=  50 && arg <= 38400)	/* could go higher... */
		{
		/* calculate proper counter value, then enable BRG */

		baudRate = (pChan->clockRate + (8 * arg)) / (16 * arg);

		if (--baudRate > 0xfff)
		    *pChan->pBaud = (BRGC_CD_MSK &
		    (((baudRate + 8) / 16) << BRGC_CD_SHIFT)) | BRGC_EN |
		    BRGC_DIV16;
                else
                    *pChan->pBaud = (BRGC_CD_MSK & 
			(baudRate << 1)) | BRGC_EN;

                pChan->baudRate = arg;
		}
            else
	        status = EIO;
	    break;
    
	case SIO_BAUD_GET:
	    * (int *) arg = pChan->baudRate;
	    break;

	case SIO_MODE_SET:
            if (!((int) arg == SIO_MODE_POLL || (int) arg == SIO_MODE_INT))
                {
                status = EIO;
                break;
                }

            /* lock interrupt  */

            oldlevel = intLock();

            /* initialize channel on first MODE_SET */

            if (!pChan->channelMode)
                ppc860ResetChannel(pChan);

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
                           statusMode & BD_TX_READY_BIT);

                }

            if (arg == SIO_MODE_INT)
		{
                * CISR(pChan->regBase) = pChan->uart.intMask;
				/* reset the SMC's interrupt status bit */

		* CIMR(pChan->regBase) |= pChan->uart.intMask;
				/* enable this SMC's interrupt  */

		pChan->uart.pSmcReg->smce = SMCE_RX;	
				/* reset the receiver status bit */ 

                pChan->uart.pSmcReg->smcm = SMCM_RX_MSK | SMCM_TX_MSK;
				/* enables receive and transmit interrupts */
		}
            else
		{
                pChan->uart.pSmcReg->smcm = 0;
				/* mask off the receive and transmit intrs */

		* CIMR(pChan->regBase) &= (~(pChan->uart.intMask));
				/* mask off this SMC's interrupt */ 

                }

            pChan->channelMode = arg;

            intUnlock(oldlevel);

            break;

        case SIO_MODE_GET:
            * (int *) arg = pChan->channelMode;
	    break;

        case SIO_AVAIL_MODES_GET:
            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    break;

	default:
	    status = ENOSYS;
	}

    return (status);
    }

/*******************************************************************************
*
* ppc860Int - handle an SMC interrupt
*
* This routine is called to handle SMC interrupts.
*/

void ppc860Int
    (
    PPC860SMC_CHAN *pChan
    )
    {
    char		outChar;
    FAST UINT16		dataLen = 0;

    /* check for a receive event */

    if (pChan->uart.pSmcReg->smce & SMCE_RX)
	{
        pChan->uart.pSmcReg->smce = SMCE_RX;

	while (!(pChan->uart.rxBdBase [pChan->uart.rxBdNext].statusMode &
		 BD_RX_EMPTY_BIT))
	    {
	    /* process all filled receive buffers */

	    outChar = pChan->uart.rxBdBase[pChan->uart.rxBdNext].dataPointer[0];

            pChan->uart.rxBdBase[pChan->uart.rxBdNext].statusMode |=
                BD_RX_EMPTY_BIT;

            /* incr BD count */

            pChan->uart.rxBdNext = (pChan->uart.rxBdNext + 1) %
                                  pChan->uart.rxBdNum;

            /* acknowledge interrupt ??? multiple events ??? */

            pChan->uart.pSmcReg->smce = SMCE_RX;

	    (*pChan->putRcvChar) (pChan->putRcvArg,outChar);

	    if (pChan->channelMode == SIO_MODE_POLL)
		break;
	    }
	}

    /* check for a transmit event and if a character needs to be output */

    if ((pChan->uart.pSmcReg->smce & SMCE_TX) &&
        (pChan->channelMode != SIO_MODE_POLL))
	{
        pChan->uart.pSmcReg->smce = SMCE_TX;
    
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

	    pChan->uart.txBdBase[pChan->uart.txBdNext].dataLength = dataLen;

            /* acknowledge interrupt */

            pChan->uart.pSmcReg->smce = SMCE_TX;

	    /* send transmit buffer */

	    pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode |=
	        BD_TX_READY_BIT;

	    /* incr BD count */

 	    pChan->uart.txBdNext = (pChan->uart.txBdNext+ 1) %
							pChan->uart.txBdNum;
	    }
	}

    /* acknowledge all other interrupts - ignore events */

    pChan->uart.pSmcReg->smce = (pChan->uart.pSmcReg->smce & 
							~(SMCE_RX | SMCE_TX));

    * CISR(pChan->regBase) = pChan->uart.intMask;
    }

/*******************************************************************************
*
* ppc860Startup - transmitter startup routine
*/

static int ppc860Startup
    (
    PPC860SMC_CHAN *pChan		/* ty device to start up */
    )
    {
    char outChar;
    FAST UINT16 dataLen = 0;

    if (pChan->channelMode == SIO_MODE_POLL)
	return (ENOSYS);

    /* check if buffer is ready and if a character needs to be output */

    if ((!(pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode &
	   BD_TX_READY_BIT)) &&
        ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK))
	{
	do
	    {
	    pChan->uart.txBdBase[pChan->uart.txBdNext].dataPointer[dataLen++] =
		outChar;
	    }
	while ((dataLen < pChan->uart.txBufSize) &&
               ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK));
	       						/* fill buffer */

	/* send transmit buffer */

	pChan->uart.txBdBase[pChan->uart.txBdNext].dataLength  = dataLen;
	pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode |=
	    BD_TX_READY_BIT;

	/* incr BD count */

        pChan->uart.txBdNext = (pChan->uart.txBdNext + 1) % pChan->uart.txBdNum;
	}

    return (OK);
    }

/******************************************************************************
*
* ppc860PollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the input buffer is empty.
*/

static int ppc860PollInput
    (
    SIO_CHAN *	pSioChan,
    char *	thisChar
    )
    {

    PPC860SMC_CHAN * pChan = (PPC860SMC_CHAN *) pSioChan;

    if (!(pChan->uart.pSmcReg->smce & SMCE_RX))
        return (EAGAIN); 

    if (pChan->uart.rxBdBase[pChan->uart.rxBdNext].statusMode & BD_RX_EMPTY_BIT)
        return (EAGAIN);

    /* get a character */

    *thisChar = pChan->uart.rxBdBase[pChan->uart.rxBdNext].dataPointer[0];

    /* set the empty bit */

    pChan->uart.rxBdBase[pChan->uart.rxBdNext].statusMode |= BD_RX_EMPTY_BIT;

    /* incr BD count */

    pChan->uart.rxBdNext = (pChan->uart.rxBdNext + 1) % pChan->uart.rxBdNum;

    /* only clear RX event if no more characters are ready */

    if (pChan->uart.rxBdBase[pChan->uart.rxBdNext].statusMode & BD_RX_EMPTY_BIT)
        pChan->uart.pSmcReg->smce = SMCE_RX;

    return (OK);
    }

/******************************************************************************
*
* ppc860PollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the output buffer if full.
*/

static int ppc860PollOutput
    (
    SIO_CHAN *	pSioChan,
    char	outChar
    )
    {
    PPC860SMC_CHAN *	pChan = (PPC860SMC_CHAN *) pSioChan;
    int			i;

    /* wait a bit for the last character to get out */
    /* because the PPC604 is a very fast processor */
    /* ???  make the 10000 value a #define */

    i = 0;
    while( (i<10000) && (pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode &
	BD_TX_READY_BIT) )
	{
	i = i + 1;    
	}
	
    /* is the transmitter ready to accept a character? */
    /* if still not, we have a problem */

    if (pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode &
	BD_TX_READY_BIT)
	return(EAGAIN);

    /* reset the transmitter status bit */

    pChan->uart.pSmcReg->smce = SMCE_TX;

    /* write out the character */

    pChan->uart.txBdBase[pChan->uart.txBdNext].dataPointer[0] = outChar;

    pChan->uart.txBdBase[pChan->uart.txBdNext].dataLength  = 1;

    /* send transmit buffer */

    pChan->uart.txBdBase[pChan->uart.txBdNext].statusMode |= BD_TX_READY_BIT;

    pChan->uart.txBdNext = (pChan->uart.txBdNext + 1) % pChan->uart.txBdNum;

    return (OK);
    }

/******************************************************************************
*
* ppc860CallbackInstall - install ISR callbacks to get put chars.
*
*/

static int ppc860CallbackInstall
    (
    SIO_CHAN *	pSioChan,
    int		callbackType,
    STATUS	(* callback)(),
    void *	callbackArg
    )
    {
    PPC860SMC_CHAN * pChan = (PPC860SMC_CHAN *) pSioChan;

    switch (callbackType)
        {
        case SIO_CALLBACK_GET_TX_CHAR:
            pChan->getTxChar    = callback;
            pChan->getTxArg     = callbackArg;
            return (OK);
	    break;

        case SIO_CALLBACK_PUT_RCV_CHAR:
            pChan->putRcvChar   = callback;
            pChan->putRcvArg    = callbackArg;
            return (OK);
	    break;

        default:
            return (ENOSYS);
        }
    }
