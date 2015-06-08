/* m68562Sio.c - MC68562 DUSCC serial driver */

/* Copyright 1984-1995 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01g,14nov95,myz   rework last fix
01f,14nov95,myz   undo last fix and fixed SPR #5437
01e,08sep95,myz   fixed the SPR #4678
01d,03aug95,myz   fixed the warning messages
01c,20jun95,ms    fixed comments for mangen
01b,15jun95,ms    updated for new driver structure
01a,14apr95,myz   written (using m68562Serial.c + the VxMon polled driver).
*/

/*
DESCRIPTION
This is the driver for the MC68562 DUSCC serial chip.
It uses the DUSCC in asynchronous mode only.

USAGE
A M68562_QUSART structure is used to describe the chip. This data structure
contains M68562_CHAN structures which describe the chip's serial channels.
The BSP's sysHwInit() routine typically calls sysSerialHwInit()
which initializes all the values in the M68562_QUSART structure (except
the SIO_DRV_FUNCS) before calling m68562HrdInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chips interrupts (m68562RxTxErrInt, m68562RxInt, and
m68562TxInt) via intConnect().

IOCTL
This driver responds to the same ioctl() codes as a normal serial driver.
See the file sioLib.h for more information.

INCLUDE FILES: drv/sio/m68562Sio.h
*/

#include "vxWorks.h"
#include "iv.h"
#include "intLib.h"
#include "errnoLib.h"
#include "drv/sio/m68562Sio.h"


#define BIT(val,bit)    ((val & bit) == bit)


/* forward declarations */

static int m68562Startup (M68562_CHAN *pChan);
static int m68562Ioctl (M68562_CHAN *pChan,int request,int arg);
static void m68562ResetChannel (M68562_CHAN *pChan);
static int m68562PTxChar (M68562_CHAN *pChan,char ch);
static int m68562PRxChar (M68562_CHAN *pChan,char *ch);
static int m68562CallbackInstall (SIO_CHAN *, int, STATUS (*)(), void *);

typedef struct	       /* BAUD */
    {
    int rate;	       /* a baud rate */
    char xmtVal;       /* rate to write to the Transmitter Timing Register */
    char rcvVal;       /* rate to write to the Reciever Timing Register */
    }      BAUD;


/* 
 * baudTable is a table of the available baud rates, and the values to 
 * write to the csr reg to get those rates 
 */

static BAUD baudTable [] =
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

/* local driver function table */

static SIO_DRV_FUNCS m68562SioDrvFuncs =
    {
    (int (*)())m68562Ioctl,
    (int (*)())m68562Startup,
    m68562CallbackInstall,
    (int (*)())m68562PRxChar,
    (int (*)(SIO_CHAN *,char))m68562PTxChar
    };


/*******************************************************************************
*
* m68562HrdInit - initialize the DUSCC
*
* The BSP must have already initialized all the device addresses, etc in
* M68562_DUSART structure. This routine resets the chip in a quiescent state.
*/

void m68562HrdInit
    (
    M68562_QUSART *pQusart
    )
    {
    register int ix;
    int	lock = intLock ();      /* LOCK INTERRUPTS */

    for (ix = 0; ix < pQusart->numChannels; ix ++)
        pQusart->channel[ix].pDrvFuncs      = &m68562SioDrvFuncs;

    /* initialize general registers of the device */

    *(pQusart->gsr)  = (char) 0xff;
    *(pQusart->ivr)  = pQusart->int_vec;   /* initialize vector register */

    /* enable a, enable b, modify vec w/stat, present vec to cpu */

    *(pQusart->icr)  = DUSCC_ICR_CHAN_B_MAST_INT_EN |
                       DUSCC_ICR_CHAN_A_MAST_INT_EN |
                       DUSCC_ICR_VECT_INCLDS_STATUS | DUSCC_ICR_VECT_MODE_VECT;


    for (ix = 0; ix < pQusart->numChannels; ix ++)
        {

	/* execute reset commands */

	*(pQusart->channel[ix].ccr) = DUSCC_CCR_TX_RESET_TX;
	*(pQusart->channel[ix].ccr) = DUSCC_CCR_RX_RESET_RX;
	*(pQusart->channel[ix].ccr) = (unsigned char)DUSCC_CCR_CT_STOP;
	*(pQusart->channel[ix].ccr) = (unsigned char)DUSCC_CCR_DPLL_DISABLE_DPLL
	; 

	/* initialize registers */

	*(pQusart->channel[ix].cmr1) = (char) 0x0;
	*(pQusart->channel[ix].cmr2) = (char) 0x0;
	*(pQusart->channel[ix].tpr)  = (char) 0x0;
	*(pQusart->channel[ix].rpr)  = (char) 0x0;
	*(pQusart->channel[ix].omr)  = (char) 0x0;
	*(pQusart->channel[ix].pcr)  = (char) 0x0;
	*(pQusart->channel[ix].rsr)  = (char) 0xff;
	*(pQusart->channel[ix].trsr) = (char) 0xff;
	*(pQusart->channel[ix].ictsr) = DUSCC_ICTSR_CT_ZERO_COUNT;
	*(pQusart->channel[ix].ier)  = (char) 0x0;

        m68562ResetChannel(&(pQusart->channel[ix]));	
        }

    intUnlock (lock);		/* UNLOCK INTERRUPTS */
    }

/*******************************************************************************
*
* m68562ResetChannel - reset a single channel
*/

static void m68562ResetChannel
    (
    M68562_CHAN *pChan
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

    *pChan->ccr = DUSCC_CCR_TX_DISABLE_TX;
    *pChan->ccr = DUSCC_CCR_RX_DISABLE_RX;

    /* configure transmitter and receiver */

    *pChan->cmr1 = DUSCC_CMR1_ASYNC;
    *pChan->cmr2 = DUSCC_CMR2_DTI_POLL_OR_INT;
    *pChan->tpr  = DUSCC_TPR_ASYNC_1 | DUSCC_TPR_8BITS;

    *pChan->ttr  = DUSCC_TTR_CLK_BRG | DUSCC_TTR_BAUD_9600;
    *pChan->rtr  = DUSCC_RTR_CLK_BRG | DUSCC_RTR_BAUD_9600;

    *pChan->rpr  = DUSCC_RPR_8BITS;
    *pChan->omr |= DUSCC_OMR_TXRDY_FIFO_EMPTY |
                    DUSCC_OMR_TX_RES_CHAR_LENGTH_TPR;

    /* reset transmitter and receiver */

    *pChan->ccr = DUSCC_CCR_TX_RESET_TX;
    *pChan->ccr = DUSCC_CCR_RX_RESET_RX;

    /* enable transmitter and receiver */

    *pChan->ccr = DUSCC_CCR_TX_ENABLE_TX;
    *pChan->ccr = DUSCC_CCR_RX_ENABLE_RX;

    }

/*******************************************************************************
*
* m68562Ioctl - special device control
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*/

static STATUS m68562Ioctl
    (
    M68562_CHAN *pChan,	/* device to control */
    int request,	/* request code      */
    int arg		/* some argument     */
    )
    {
    register int ix;
    STATUS status;
    int oldlevel;

    switch (request)
        {
	case SIO_BAUD_SET:

	    status = EIO;

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

		    if ((*pChan->ttr != baudTable [ix].xmtVal) ||
			(*pChan->rtr != baudTable [ix].rcvVal))
			{
                        oldlevel = intLock();

			/* disable transmitter and receiver */

			*pChan->ccr = DUSCC_CCR_TX_DISABLE_TX;
			*pChan->ccr = DUSCC_CCR_RX_DISABLE_RX;

			/* configure transmitter and receiver */

			*pChan->ttr = baudTable [ix].xmtVal;
			*pChan->rtr = baudTable [ix].rcvVal;

			/* reset transmitter and receiver */

			*pChan->ccr = DUSCC_CCR_TX_RESET_TX;
			*pChan->ccr = DUSCC_CCR_RX_RESET_RX;

			/* enable transmitter and receiver */

			*pChan->ccr = DUSCC_CCR_TX_ENABLE_TX;
			*pChan->ccr = DUSCC_CCR_RX_ENABLE_RX;

			intUnlock(oldlevel);

			break;
			}
		    }
	    break;

        case SIO_BAUD_GET:

            status = EIO;               /* baud rate out of range */

            /* disable interrupts during chip access */

            oldlevel = intLock ();

            for (ix = 0; ix < NELEMENTS (baudTable); ix ++)
                if (baudTable [ix].rcvVal == *pChan->rtr)
                    {
                    *(int *)arg = baudTable [ix].rate;
                    status = OK;        /* baud rate is valid */

                    }

            intUnlock (oldlevel);

            break;

        case SIO_MODE_SET:
            if (!((int)arg == SIO_MODE_POLL || (int)arg == SIO_MODE_INT))
                {
                status = EIO;
                break;
                }

            /* lock interrupt  */
            oldlevel = intLock();

            if (arg == SIO_MODE_INT)
                *pChan->ier = DUSCC_IER_RXRDY | DUSCC_IER_RSR_5_4; 
            else
                *pChan->ier = 0;

            pChan->channelMode = arg;

            intUnlock(oldlevel);
            status = OK;

            break;

        case SIO_MODE_GET:
            *(int *)arg = pChan->channelMode;
            return (OK);

        case SIO_AVAIL_MODES_GET:
            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
            return (OK);
            
	default:
	    status = ENOSYS;
	    break;
        }
    return (status);
    }

/*******************************************************************************
*
* m68562RxTxErrInt - handle a receiver/transmitter error interrupt
*
* Only the receive overrun condition is handled.
*
* RETURNS: N/A
*/

void m68562RxTxErrInt
    (
    M68562_CHAN *pChan
    )
    {
    if (BIT (*pChan->rsr, DUSCC_RSR_ASYNC_OVERN_ERROR))
	*pChan->rsr = DUSCC_RSR_ASYNC_OVERN_ERROR;	/* "1" clears bit */

    }

/*******************************************************************************
*
* m68562RxInt - handle a receiver interrupt
*
* RETURNS: N/A
*/

void m68562RxInt
    (
    M68562_CHAN *pChan
    )
    {
    /* drain receive FIFO */

    do
        {
	(*pChan->putRcvChar) (pChan->putRcvArg, *pChan->rx_data);
        }
    while ( ( (*pChan->pQusart->gsr) &  
	   (DUSCC_GSR_A_RXRDY << ((pChan->chan_num)*4) ) ) && 
	   pChan->channelMode != SIO_MODE_POLL ); 
    }

/*******************************************************************************
*
* m68562TxInt - handle a transmitter interrupt
*
* If there is another character to be transmitted, it sends it.  If
* not, or if a device has never been created for this channel,
* disable the interrupt.
*
* RETURNS: N/A
*/

void m68562TxInt
    (
    M68562_CHAN *pChan
    )
    {
    char outChar;

    do                                          /* fill transmit FIFO */
        {
        if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK)
            *pChan->tx_data = outChar;
        else
            {
            *pChan->ier &= ~DUSCC_IER_TXRDY;  /* turn off transmitter */
            break;
            }
        }
        while ( (*pChan->pQusart->gsr) & 
		 (DUSCC_GSR_A_TXRDY << ((pChan->chan_num)*4) ) );
    }

/******************************************************************************
*
* m68562PRxChar - poll the device for input.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the input buffer if empty.
*/

static int m68562PRxChar
    (
    M68562_CHAN *pChan,
    char *ch
    )
    {
    /* wait for Data */
    if (! ( (*pChan->pQusart->gsr) &
              (DUSCC_GSR_A_RXRDY << ((pChan->chan_num)*4) ) ) )
        return(EAGAIN);

    *ch = *pChan->rx_data;
    return(OK);
    }

/******************************************************************************
*
* m68562PTxChar - output a character in polled mode.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the output buffer if full.
*/

static int m68562PTxChar
    (
    M68562_CHAN *pChan,
    char ch                   /* character to output */
    )
    {

    /* check for Empty */
    if (! ( (*pChan->pQusart->gsr) &
               (DUSCC_GSR_A_TXRDY << ((pChan->chan_num)*4) ) ) ) 
        return(EAGAIN);

    *pChan->tx_data = ch;
    return(OK);
    }


/*******************************************************************************
*
* m68562Startup - transmitter startup routine
*
* Call interrupt level character output routine for DUSCC.
*/

static int m68562Startup
    (
    M68562_CHAN * pChan  	/* device to start up */ 
    )
    {
    char outChar;

    if ( pChan->channelMode == SIO_MODE_INT )
        {
	if ( (*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK)
            *pChan->tx_data  = outChar;
	
        *pChan->ier |= DUSCC_IER_TXRDY;

        return (OK);
        }
    else
        return (ERROR);
    }


/******************************************************************************
*
* m68562CallbackInstall - install ISR callbacks to get put chars.
*/

static int m68562CallbackInstall
    (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS      (*callback)(),
    void *      callbackArg
    )
    {
    M68562_CHAN * pChan = (M68562_CHAN *)pSioChan;

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


