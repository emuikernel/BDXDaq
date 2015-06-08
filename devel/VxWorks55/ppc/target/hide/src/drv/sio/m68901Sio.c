/* m68901Sio.c - MC68901 MFP tty driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,24apr02,pmr  SPR 75161: returning int from m68901TxStartup() as required.
01b,08nov96,dgp  doc: final formatting
01a,25may96,sub  wrote from m68901Serial.c
*/

/*
DESCRIPTION
This is the SIO driver for the Motorola MC68901 Multi-Function Peripheral
(MFP) chip.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  However, one routine must be called directly:  m68901DevInit()
initializes the driver.  Normally, it is called by sysSerialHwInit()
in sysSerial.c

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as other tty drivers;
for more information, see the manual entry for tyLib.

SEE ALSO
tyLib
*/

/* includes */

#include "vxWorks.h"
#include "errnoLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "tyLib.h"
#include "intLib.h"
#include "iv.h"
#include "drv/sio/m68901Sio.h"

/* defines */

#define DEFAULT_BAUD  9600

/* forward declarations */

LOCAL	void	m68901InitChannel (M68901_CHAN *);
LOCAL	int 	m68901Ioctl (SIO_CHAN *, int, void *);
LOCAL	int  	m68901TxStartup (SIO_CHAN *);
LOCAL	int 	m68901CallBackInstall (SIO_CHAN *,int, STATUS (*)(), void *);
LOCAL	int 	m68901PollInput (SIO_CHAN *, char *);
LOCAL	int 	m68901PollOutput (SIO_CHAN *, char);
LOCAL	int	m68901ModeSet(M68901_CHAN *, uint_t); 

/* locals */

LOCAL SIO_DRV_FUNCS m68901SioDrvFuncs =
    {
    (int (*) ()) m68901Ioctl,
    (int (*) ()) m68901TxStartup,
    (int (*) ()) m68901CallBackInstall,
    (int (*) ()) m68901PollInput,
    (int (*) (SIO_CHAN *, char)) m68901PollOutput
    };


/*******************************************************************************
*
*  m68901DummyCallback - dummy callback routine.
* 
*/

LOCAL STATUS m68901DummyCallback (void)
    {
    return(ERROR);
    }


/*******************************************************************************
*
* m68901InitChannel - initialize a single channel
*
*/

LOCAL void m68901InitChannel 
    (
    M68901_CHAN  *pChan
    )
    {
    int lock = intLock ();

    /* 8 data bits, 1 stop bit, no parity */

    *pChan->ucr = (char)(MFP_UCR_16X | MFP_UCR_8BIT |
		        MFP_UCR_1STOP | MFP_UCR_NO_PARITY);

    /* set baud rate values w/ prescale=4 */

    *pChan->baud1 = pChan->baudFreq / (128 * 9600);
    if (pChan->baud2 != NULL)
        *pChan->baud2 = pChan->baudFreq / (128 * 9600);

    /* enable the receivers and transmitters on both channels */

    *pChan->rsr  = MFP_RSR_RX_ENABLE;      /* enable rcvr status */
    *pChan->tsr  = MFP_TSR_TX_ENABLE;      /* enable xmit status */

    intUnlock (lock);
    }


/*******************************************************************************
*
* m68901Ioctl - special device control
*
*
* RETURNS: OK on success, EIO on device error, ENOSYS on 
* unsupported requests.
*/

LOCAL STATUS m68901Ioctl
    (
    SIO_CHAN *pSioChan,	/* device to control */
    int request,	/* request code */
    void * arg		/* some argument */
    )
    {
    M68901_CHAN *pChan = (M68901_CHAN  *)pSioChan;

    switch (request)
	{
        case SIO_BAUD_SET:
            if (((int)arg >= pChan->baudMin) && ((int)arg <= pChan->baudMax))
                {
		/* set baud rate values w/ prescale=4 */
		*pChan->baud1 = pChan->baudFreq / (128 * (int)arg);
		if (pChan->baud2 != NULL)
		    *pChan->baud2 = pChan->baudFreq / (128 * (int)arg);
		pChan->baudRate = (int)arg; 
		return (OK);
                }
	    return (EIO);
	    
	case SIO_BAUD_GET:
	    *(int *)arg = pChan->baudRate;
	    return (OK);

	case SIO_MODE_SET:
	    return (m68901ModeSet (pChan,(uint_t)arg) == OK ? OK: EIO);

	case SIO_MODE_GET:
	    *(int *)arg = pChan->mode;
	    return (OK);

	case SIO_AVAIL_MODES_GET:
	    *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    return (OK);

	default:
	    return (ENOSYS);
	}
    }


/*******************************************************************************
*
* m68901DevInit - initialize a M68901_CHAN structure
*
* This routine initializes the driver
* function pointers and then resets the chip to a quiescent state.
* The BSP must have already initialized all the device addresses and the
* `baudFreq' fields in the M68901_CHAN structure before passing it to
* this routine.
*
* RETURNS: N/A
*/

void m68901DevInit
    (
    M68901_CHAN * pChan
    )
    {
    int oldlevel = intLock ();

    pChan->pDrvFuncs = &m68901SioDrvFuncs;

    pChan->baudRate = DEFAULT_BAUD;
    pChan->getTxChar = m68901DummyCallback;
    pChan->putRcvChar = m68901DummyCallback;
    pChan->mode = 0;		/* undefined */

    /* reset the chip */

    m68901InitChannel (pChan);

    intUnlock (oldlevel);
    }


/*******************************************************************************
*
* m68901IntWr - handle a transmitter interrupt
*
* This routine gets called to handle transmitter interrupts.
*
* NOMANUAL
*/

void m68901IntWr 
    (
    M68901_CHAN * pChan
    )
    {
    char outChar;
    volatile char *dr = pChan->udr; /* USART data register */
    
    if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)
	*dr = outChar;
    else
	*pChan->imra &= ~MFP_A_TX_EMPTY; /* turn off transmitter */
    }


/*******************************************************************************
*
* m68901IntRd - handle a receiver interrupt
*
* NOMANUAL
*/

void m68901IntRd 
    (
    M68901_CHAN *pChan
    )
    {
    volatile char *dr = pChan->udr;
    char inChar;

    inChar = *dr;		
    (*pChan->putRcvChar) (pChan->putRcvArg, inChar);
    }


/*******************************************************************************
*
* m68901TxStartup - transmitter startup routine
*
* Call interrupt level character output routine.
*/

LOCAL int m68901TxStartup
    (
    SIO_CHAN *pSioChan
    )
    {
    M68901_CHAN * pChan = (M68901_CHAN *)pSioChan;
    
    char outchar;

    /* enable the transmitter and it should interrupt to write the next char */

    if ((*pChan->getTxChar) (pChan->getTxArg, &outchar) !=ERROR)
	*pChan->udr = outchar;

    *pChan->imra |= MFP_A_TX_EMPTY;	/* turn on transmitter */

    return (OK);
    }

/*******************************************************************************
*
* m68901CallBackInstall -  install ISR callbacks to get/put chars
*
*/

LOCAL int m68901CallBackInstall 
    (
    SIO_CHAN * pSioChan,
    int callbackType, 
    STATUS (*callback)(), 
    void *  callbackArg
    )
    {
    M68901_CHAN * pChan = (M68901_CHAN *)pSioChan;
    
    switch (callbackType)
	{
	case SIO_CALLBACK_GET_TX_CHAR:
	    pChan->getTxChar = callback;
	    pChan->getTxArg  = callbackArg;
	    return (OK);

	case SIO_CALLBACK_PUT_RCV_CHAR:
	    pChan->putRcvChar= callback;
	    pChan->putRcvArg = callbackArg;
	    return (OK);

	default:
	    return(ENOSYS);
	}
    }


/*******************************************************************************
*
* m68901PollOutput - output a character in polled mode.
*
* RETURNS:  OK if a character arrived, EIO on device error, EAGAIN if
* the output buffer is full.
*/

LOCAL int m68901PollOutput
    (
    SIO_CHAN * pSioChan,
    char       outChar
    )
    {
    M68901_CHAN * pChan = (M68901_CHAN *) pSioChan;

    if (( *(char *)pChan->tsr & MFP_TSR_BUFFER_EMPTY) == 0)
	return (EAGAIN);

    *pChan->udr = outChar;
    return (OK);
    }


/*******************************************************************************
*
* m68901PollInput - poll device for input
*
* RETURNS:  OK if a character arrived, EIO on device error, EAGAIN if
* the input buffer is empty.
*/

LOCAL int m68901PollInput
    (
    SIO_CHAN * pSioChan,
    char     * outChar
    )
    {
    M68901_CHAN * pChan = (M68901_CHAN *) pSioChan;

    if (( *(char *)pChan->rsr & MFP_RSR_BUFFER_FULL) == 0)
	return (EAGAIN);

    *outChar = *(char *)pChan->udr;
    return (OK);
    }


/******************************************************************************
*
* m68901ModeSet - toggle between interrupt and polled mode.
*
* RETURNS: OK on success, EIO on unsupported mode.
*/

LOCAL int m68901ModeSet
    (
    M68901_CHAN * pChan,		/* channel */
    uint_t	    newMode          	/* new mode */
    )
    {
    int oldlevel;
    
    if ((newMode != SIO_MODE_POLL) && (newMode != SIO_MODE_INT))
	return (EIO);

    /* set the new mode */

    oldlevel = intLock();
    
    pChan->mode = newMode;

    if (pChan->mode == SIO_MODE_INT)
	{
	*pChan->iera |= MFP_A_RX_FULL;  /* enable recv int. */
	*pChan->iera |= MFP_A_RX_ERR;   /* enable recv error int. */
	*pChan->iera |= MFP_A_TX_EMPTY; /* enable transmit int. */

	*pChan->imra |= MFP_A_RX_FULL;	/* unmask recv ints */
	*pChan->imra |= MFP_A_RX_ERR;	/* unmask recv error ints */
	}
    else
	{
	*pChan->iera &= ~MFP_A_RX_FULL;  /* disable recv int. */
	*pChan->iera &= ~MFP_A_RX_ERR;   /* disable recv error int. */
	*pChan->iera &= ~MFP_A_TX_EMPTY; /* disable transmit int. */

	*pChan->imra &= ~MFP_A_RX_FULL;	/* mask recv ints */
	*pChan->imra &= ~MFP_A_RX_ERR;	/* mask recv error ints */
	*pChan->imra &= ~MFP_A_TX_EMPTY;/* mask recv ints */
	}
    
    intUnlock(oldlevel);
    return (OK);
    }
