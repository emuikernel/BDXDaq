/* m68302Sio.c - Motorola MC68302 bimodal tty driver */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01g,11jul97,dgp  doc: correct ttyDrv and tyLib in SEE ALSO
01f,20jun97,map  fixed RX event clear [SPR# 8803]
01e,12dec96,map  handle non-DPRAM buffers [SPR# 7479]
01d,06nov96,dgp  doc: final formatting
01c,30may96,dat  More ISR work, work on ModeSet().
01b,24may96,dat  fixed ISR and modeSet rtn, new baud rate setup
		 added polled mode operation.
01a,01mar96,dat  written (from m68302Serial.c).
*/

/*
DESCRIPTION
This is the driver for the internal communications processor (CP)
of the Motorola MC68302.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Before the driver can be used, it must be initialized by calling the
routines m68302SioInit() and m68302SioInit2().
Normally, they are called by sysSerialHwInit() and sysSerialHwInit2()
in sysSerial.c

This driver uses 408 bytes of buffer space as follows:

  128 bytes for portA tx buffer
  128 bytes for portB tx buffer
  128 bytes for portC tx buffer
    8 bytes for portA rx buffers (8 buffers, 1 byte each)
    8 bytes for portB rx buffers (8 buffers, 1 byte each)
    8 bytes for portC rx buffers (8 buffers, 1 byte each)

The buffer pointer in the `m68302cp' structure points to the buffer area,
which is usually specified as IMP_BASE_ADDR.

INTERNAL
There is only one Tx buffer for each SCC device.  This is probably because
SCC1 and SCC2 can have 8 buffers each, but SCC3 can only have 4.

Hardware abstraction isn't used in this driver.  These SCCs are internal
to the CPU.  It doesn't seem to make sense to abstract it under that
condition - i.e. it can't be used with any other CPU.

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal tty driver; for
more information, see the manual entry for tyLib.  The available baud
rates are 300, 600, 1200, 2400, 4800, 9600 and 19200.

SEE ALSO
ttyDrv, tyLib

INCLUDE FILES: drv/sio/m68302Sio.h sioLib.h
*/

#include "vxWorks.h"
#include "sioLib.h"
#include "intLib.h"
#include "errno.h"
#include "drv/sio/m68302Sio.h"

#define MIN_BRG_DIV 	3
#define MAX_BRG_DIV 	2047
#define RX_BD_NUM 	8	/* number of receiver BDs */
#define DPRAM_START	(char *)((*IMP_BAR & 0x0fff) << 12)
#define DPRAM_END	(DPRAM_START + 0x0fff)

/* forward LOCAL declarations */

LOCAL	int	m68302SioTxStartup (SIO_CHAN * pSioChan);
LOCAL	int	m68302SioCallbackInstall (SIO_CHAN *pSioChan, int callbackType,
				STATUS (*callback)(), void *callbackArg);
LOCAL	int	m68302SioPollOutput (SIO_CHAN *pSioChan, char	outChar);
LOCAL	int	m68302SioPollInput (SIO_CHAN *pSioChan, char *thisChar);
LOCAL	int	m68302SioIoctl (SIO_CHAN *pSioChan, int request, void *arg);
LOCAL	void	m68302SioChanInit (M68302_CHAN *pChan, M68302_CP *pCp);
LOCAL	STATUS	dummyCallback (void);
LOCAL	int	m68302ModeSet (M68302_CHAN *, uint_t);
LOCAL	int	m68302BaudSet (M68302_CHAN *, int);

/* local variables */

#ifndef DEFAULT_BAUD
#   define DEFAULT_BAUD 9600
#endif

LOCAL SIO_DRV_FUNCS m68302SioDrvFuncs =
    {
    m68302SioIoctl,
    m68302SioTxStartup,
    m68302SioCallbackInstall,
    m68302SioPollInput,
    m68302SioPollOutput
    };

/******************************************************************************
*
* m68302SioInit - initialize a M68302_CP
*
* This routine initializes the driver
* function pointers and then resets the chip to a quiescent state.
* The BSP must already have initialized all the device addresses and the
* `baudFreq' fields in the M68302_CP structure before passing it to this
* routine. The routine resets the device and initializes everything to support 
* polled mode (if possible), but does not enable interrupts.
*
* RETURNS: N/A
*/

void m68302SioInit
    (
    M68302_CP * pCp
    )
    {
    int oldlevel = intLock ();			/* LOCK INTERRUPTS */

    /* reset the chip */

    *pCp->pCr =  (char) (CR_RST | CR_FLG);
    while (*pCp->pCr != 0) 		/* Wait for reset to finish */
	/* do nothing */ ;

    *pCp->pImr 	  = 0;
    *pCp->pSiMask = 0xffff;
    *pCp->pSiMode = 0x0000;		/* NMSI for all channels */

    pCp->intEnable= FALSE;		/* disable int mode for now */

    m68302SioChanInit (&pCp->portA, pCp);
    m68302SioChanInit (&pCp->portB, pCp);
    m68302SioChanInit (&pCp->portC, pCp);

    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */
    }

/******************************************************************************
*
* m68302SioInit2 - initialize a M68302_CP (part 2)
*
* Enables interrupt mode of operation.
*
* RETURNS: N/A
*/

void m68302SioInit2
    (
    M68302_CP * pCp
    )
    {
    pCp->intEnable = TRUE;

    /* reset mode, since it may have changed */
    m68302ModeSet (&pCp->portA, pCp->portA.mode);
    m68302ModeSet (&pCp->portB, pCp->portB.mode);
    m68302ModeSet (&pCp->portC, pCp->portC.mode);

    *pCp->pImr |= pCp->imrMask;/* unmask int */
    }


/*********************************************************************
*
* m68302SioChanInit - initialize a single channel
*
* Initialize a single channel of the Communications Processor.  The
* default initialization places the channel in UART protocol.
*
* If the pScc pointer is null, then this channel will not be initialized.
* This is used to prevent initialization of a non-existant SCC channel
* on some CPU variants.
*
* RETURNS: N/A
*/

LOCAL void m68302SioChanInit
    (
    M68302_CHAN *pChan,
    M68302_CP *pCp
    )
    {
    int		frame;
    int		baud;
    int		bdLocFlag=0;

    if (pChan == NULL
     || pChan->pScc == NULL)
	return;

    if ((pCp->buffer < DPRAM_START) || (pCp->buffer > DPRAM_END))
        bdLocFlag = UART_RX_BD_EXT;

    pChan->sio.pDrvFuncs= &m68302SioDrvFuncs;
    pChan->getTxChar    = dummyCallback;
    pChan->putRcvChar	= dummyCallback;
    pChan->pCp		= pCp;

    pChan->pSccReg->scon= 0x0;

    baud = (pChan->baud <= 0 ? DEFAULT_BAUD : pChan->baud);

    m68302SioIoctl (&pChan->sio, SIO_BAUD_SET, (void *)baud);

    pChan->mode = SIO_MODE_POLL;

    /* 8 data bits, 1 stop bit, no parity, Rcvr enabled */
    pChan->options = (CREAD | CS8);

    pChan->pSccReg->scm = UART_SCM_ASYNC | UART_SCM_MANUAL |
			  UART_SCM_8BIT | UART_SCM_RTSM;

    pChan->pSccReg->dsr		= 0x7e7e;	/* no fractional stop bits */
    pChan->pParam->rfcr		= 0x50;		/* sup. data access */
    pChan->pParam->tfcr		= 0x50;		/* sup. data access */
    pChan->pParam->mrblr	= 0x1;		/* one character rx buffers */


    /* next receive buffer */

    pChan->rxBdNext	= 0;

    for (frame = 0; frame < 8; frame ++)
	{
	pChan->pScc->rxBd[frame].statusMode	= bdLocFlag | UART_RX_BD_EMPTY
						    | UART_RX_BD_INT;
	pChan->pScc->rxBd[frame].dataLength	= 1;
	pChan->pScc->rxBd[frame].dataPointer	=
			(char *)((pChan->channel << 3) + frame + 0x180
				+ (int)pCp->buffer);
	}

    pChan->pScc->rxBd[RX_BD_NUM-1].statusMode	|= UART_RX_BD_WRAP;

    pChan->pScc->txBd[0].statusMode	= bdLocFlag | UART_TX_BD_INT
        				  | UART_TX_BD_WRAP;
    pChan->pScc->txBd[0].dataLength	= 0x80;
    pChan->pScc->txBd[0].dataPointer	= (char *)(pChan->channel * 0x80 +
					  (int)pCp->buffer);

    pChan->pProt->maxIdl	= 0x0;
    pChan->pProt->idlc		= 0x0;
    pChan->pProt->brkcr		= 0x0;
    pChan->pProt->parec		= 0x0;
    pChan->pProt->frmec		= 0x0;
    pChan->pProt->nosec		= 0x0;
    pChan->pProt->brkec		= 0x0;
    pChan->pProt->uaddr1	= 0x0;
    pChan->pProt->uaddr2	= 0x0;
    pChan->pProt->cntChar1	= 0x8000;

    pChan->pSccReg->scce = 0xff;		/* clr all events */
    pChan->pSccReg->sccm = 0; 		/* mask all events for now */

    pChan->pSccReg->scm |= UART_SCM_ENR | UART_SCM_ENT; /* enable RX,TX */
    }

/*******************************************************************************
*
* m68302SioInt - handle an SCC interrupt
*
* This routine gets called to handle SCC interrupts.
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void m68302SioInt
    (
    M68302_CHAN *pChan
    )
    {
    char outChar;
    FAST UINT16   dataLen = 0;
    FAST UINT8    event = pChan->pSccReg->scce;

    /* acknowledge interrupt */

    pChan->pSccReg->scce= event;		/* clear events */

    /* restrict ourselves to interrupt events, not polled events */
    event &= pChan->pSccReg->sccm;

    *pChan->pCp->pIsr = pChan->intAck;	/* ack. interrupt */

    /* a negative channel # indicates an inactive channel */
    if (pChan->channel < 0)
	return;

    if (event & UART_SCCE_TX)
	{
	/* fill buffer with data */
	while (dataLen <= 0x80
	    && pChan->mode != SIO_MODE_POLL
	    && (pChan->getTxChar)(pChan->getTxArg, &outChar) == OK)
	    {
	    pChan->pScc->txBd[0].dataPointer[dataLen++] = outChar;
	    }

	/* handoff buffer to device */
	if (dataLen > 0)
	    {
	    pChan->pScc->txBd[0].dataLength  = dataLen;
	    pChan->pScc->txBd[0].statusMode |= UART_TX_BD_READY;
	    }
	}

    /* Rx buffers are 1 chara in length, get all available */
    if (event & UART_SCCE_RX)
	{
	int nextBd = pChan->rxBdNext;

	while (!(pChan->pScc->rxBd[nextBd].statusMode & UART_RX_BD_EMPTY)
	    && pChan->mode != SIO_MODE_POLL )
	    {
	    char data;

	    data = pChan->pScc->rxBd[nextBd].dataPointer[0];

	    pChan->pScc->rxBd[nextBd].statusMode |= UART_RX_BD_EMPTY;

	    nextBd = (nextBd + 1) % RX_BD_NUM;
	    pChan->rxBdNext = nextBd;

	    (*pChan->putRcvChar) (pChan->putRcvArg, data);
	    }
	}

    /* all other events ignored */
    }

/******************************************************************************
*
* m68302SioTxStartup - start the interrupt transmitter
*
* This routine is called from ttyDrv to start the transmitter when data
* is ready to go out.  TtyDrv keeps track of the transmitter status and only
* calls this routine when needed to startup character output.
*
* RETURNS:
* Returns OK on success, ENOSYS if the device is polled-only, or
* EIO on hardware error.
*/

LOCAL int m68302SioTxStartup
    (
    SIO_CHAN * pSioChan		/* channel to start */
    )
    {
    char outChar;
    FAST int dataLen = 0;
    M68302_CHAN *pChan = (M68302_CHAN *)pSioChan;

    if (!(pChan->pScc->txBd[0].statusMode & UART_TX_BD_READY))
	{
	while ((dataLen <= 0x80)
	   && ((*pChan->getTxChar)(pChan->getTxArg, &outChar) == OK))
	    {
	    pChan->pScc->txBd[0].dataPointer[dataLen++] = outChar;
	    }
	if (dataLen > 0)
	    {
	    pChan->pScc->txBd[0].dataLength  = dataLen;
	    pChan->pScc->txBd[0].statusMode |= UART_TX_BD_READY;
	    }
	}

    return (OK);
    }

/**************************************************************************
*
* m68302SioCallbackInstall - install ISR callbacks to get/put chars
*
* This driver allows interrupt callbacks for transmitting characters
* and receiving characters. In general, drivers may support other
* types of callbacks too.
*
* RETURNS:
* Returns OK on success, or ENOSYS for an unsupported callback type.
*/

LOCAL int m68302SioCallbackInstall
    (
    SIO_CHAN   *pSioChan,		/* channel */
    int		callbackType,		/* type of callback */
    FUNCPTR	callback,		/* callback */
    void	*callbackArg		/* parameter to callback */
    )
    {
    M68302_CHAN *pChan = (M68302_CHAN *)pSioChan;

    switch (callbackType)
	{
	case SIO_CALLBACK_GET_TX_CHAR:
	    pChan->getTxChar	= callback;
	    pChan->getTxArg	= callbackArg;
	    return (OK);

	case SIO_CALLBACK_PUT_RCV_CHAR:
	    pChan->putRcvChar	= callback;
	    pChan->putRcvArg	= callbackArg;
	    return (OK);

	default:
	    return (ENOSYS);
	}

    }

/*************************************************************************
*
* m68302SioPollOutput - output a character in polled mode
*
* Send out a single character in polled mode, if the transmitter is not
* busy.  If it is busy, then EAGAIN is returned and the caller routine
* must try again later.
*
* RETURNS:
* Returns OK if a character arrived, EIO on device error, EAGAIN
* if the output buffer if full. ENOSYS if the device is
* interrupt-only.
*/

LOCAL int m68302SioPollOutput
    (
    SIO_CHAN   *pSioChan,
    char	outChar
    )
    {
    M68302_CHAN *pChan = (M68302_CHAN *)pSioChan;

    if (pChan->pScc->txBd[0].statusMode & UART_TX_BD_READY)
        return EAGAIN;
 
    /* reset the transmitter status bit */
    pChan->pSccReg->scce = UART_SCCE_TX;
 
    /* fill buffer with data */
    pChan->pScc->txBd[0].dataPointer[0] = outChar;
    pChan->pScc->txBd[0].dataLength  = 1;
    pChan->pScc->txBd[0].statusMode |= UART_TX_BD_READY;

    return OK;
    }

/******************************************************************************
*
* m68302SioPollInput - poll the device for input
*
* RETURNS:
* Returns OK if a character arrived, EIO on device error, EAGAIN
* if the input buffer if empty, ENOSYS if the device is
* interrupt-only.
*/

LOCAL int m68302SioPollInput
    (
    SIO_CHAN   *pSioChan,
    char *	thisChar
    )
    {
    M68302_CHAN *pChan = (M68302_CHAN *)pSioChan;
    int nextBd;

    nextBd = pChan->rxBdNext;

    if (pChan->pScc->rxBd[nextBd].statusMode & UART_RX_BD_EMPTY)
	return EAGAIN;

    *thisChar = pChan->pScc->rxBd[nextBd].dataPointer[0];

    pChan->pScc->rxBd[nextBd].statusMode |= UART_RX_BD_EMPTY;

    pChan->rxBdNext = (nextBd + 1) % RX_BD_NUM;

    /* reset RX event if no more data */
    if (pChan->pScc->rxBd[pChan->rxBdNext].statusMode & UART_RX_BD_EMPTY)
	pChan->pSccReg->scce = UART_SCCE_RX;

    return OK;
    }

/******************************************************************************
*
* m68302ModeSet - toggle between interrupt and polled mode
*
* Changes the mode of the channel.  Note that only one channel can be
* in polled mode at a time.  Most drivers do not enforce this though.
*
* RETURNS:
* Returns OK on success, EIO on unsupported mode.
*/

LOCAL int m68302ModeSet
    (
    M68302_CHAN *pChan,		/* channel */
    uint_t    newMode	   	/* new mode */
    )
    {
    int level;

    if (pChan == NULL
     || pChan->pScc == NULL
     || (newMode != SIO_MODE_POLL && newMode != SIO_MODE_INT))
	return (EIO);

    if ((pChan->mode == SIO_MODE_POLL) && (newMode == SIO_MODE_INT))
	{
	while (pChan->pScc->txBd[0].statusMode & UART_TX_BD_READY)
	    ;
	}

    level = intLock ();

    if (newMode == SIO_MODE_INT
     && pChan->pCp->intEnable == TRUE)
	{
	/* interrupt mode */
	*pChan->pCp->pIsr = pChan->intAck; /* reset ISR */
	*pChan->pCp->pImr |= pChan->intAck; /* enable int */
	pChan->pSccReg->scce = UART_SCCE_RX; /* reset rcv flag */
	pChan->pSccReg->sccm = UART_SCCE_RX | UART_SCCE_TX; /* enable events */
	}
    else if (newMode == SIO_MODE_POLL)
	{
	/* Polled mode */
	pChan->pSccReg->sccm = 0;		/* mask events */
	*pChan->pCp->pImr &= ~(pChan->intAck);	/* mask interrupts */
	}
    
    pChan->mode = newMode;

    intUnlock (level);

    return (OK);
    }


/******************************************************************************
*
* m68302BaudSet - set baud rate for channel
*
* Change the baud rate for the specified channel.  The new baud rate
* divisor is calculated and written to the scon register.
*
* For very slow baud rates the divide by 4 bit will be turned on.
* Otherwise it will be off.
*
* NOTE: This driver can accept any baud rate as a valid value as long as
* a valid divisor value can be computed.  The exact range of valid baud
* rates will depend on the system clock rate, but rates from 200 to 300,000
* will usually be okay.
*
* RETURNS:
* Returns OK on success, EIO on unsupported mode.
*/


LOCAL int m68302BaudSet
    (
    M68302_CHAN *pChan,
    int baud
    )
    {
    int tempDiv;
    int baudClk;
    int divBy4=0;
    int scon;
    int oldLevel;

    /* Async clock is 1/16 the system clock, Sync clk = system clock */
    baudClk = pChan->pCp->clockRate / 16;

    tempDiv = baudClk / baud;

    /* for slow baud rates, set divBy4 bit and recalc divisor */
    if (tempDiv > (MAX_BRG_DIV + 1))
	{
	tempDiv = (baudClk / 4) / baud;
	divBy4 = 1;
	}

    /* always subtract 1 */
    tempDiv -= 1;

    /* check for valid divisor, return error indication if not valid */
    if (tempDiv < MIN_BRG_DIV
     || tempDiv > MAX_BRG_DIV)
	return EIO;

    /* change current rate */

    oldLevel = intLock ();

    scon  = pChan->pSccReg->scon & 0xf000;
    scon |= (((tempDiv << 1) | divBy4) & 0x0fff);
    pChan->pSccReg->scon = scon;

    intUnlock (oldLevel);

    pChan->baud = baud;

    return OK;
    }


/*******************************************************************************
*
* m68302SioIoctl - special device control
*
* Perform device specific i/o control functions.  This routine is called
* from ttyDrv first, and if this routine returns ENOSYS, then ttyDrv will
* call tyLib with the same arguments.  The tyLib ioctl is not called if
* this routine returns OK, or EIO.
*
* RETURNS:
* Returns OK on success, ENOSYS on unsupported request, EIO on failed
* request.
*/

LOCAL int m68302SioIoctl
    (
    SIO_CHAN   *pSioChan,		/* device to control */
    int		request,		/* request code */
    void *	someArg			/* some argument */
    )
    {
    M68302_CHAN *pChan = (M68302_CHAN *)pSioChan;
    int	arg = (int)someArg;

    switch (request)
	{
	case SIO_BAUD_SET:
	    /* set the baud rate */

	    return m68302BaudSet (pChan, arg);

	case SIO_BAUD_GET:
	    /* Get the baud rate and return OK */

	    *(int *)arg = pChan->baud;
	    return OK;

	case SIO_MODE_SET:
	    /*
	     * Set the mode (e.g., to interrupt or polled). Return OK
	     * or EIO for an unknown or unsupported mode.
	     */

	    return m68302ModeSet (pChan, arg);

	case SIO_MODE_GET:
	    /*
	     * Get the current mode and return OK.
	     */

	    *(int *)arg = pChan->mode;
	    return OK;

	case SIO_AVAIL_MODES_GET:
	    /*
	     * Get the available modes and return OK.
	     */

	    *(int *)arg = (SIO_MODE_INT | SIO_MODE_POLL);
	    return OK;

	case SIO_HW_OPTS_GET:
	    /*
	     * Optional command to get the hardware options (as defined
	     * in sioLib.h).
	     * Return OK or ENOSYS if this command is not implemented.
	     * Note: if this command is unimplemented, it will be
	     * assumed that the driver options are CREAD | CS8 (e.g.,
	     * eight data bits, one stop bit, no parity, ints enabled).
	     */
	    *(int *)arg = pChan->options;
	    return OK;

	case SIO_HW_OPTS_SET:
	    /*
	     * Optional command to set the hardware options (as defined
	     * in sioLib.h).
	     * Return OK, or ENOSYS if this command is not implemented.
	     * Note: several hardware options are specified at once.
	     * This routine should set as many as it can and then return
	     * OK. The SIO_HW_OPTS_GET is used to find out which options
	     * were actually set.
	     */
	    return ENOSYS;


	default:
	    return ENOSYS;
	}
    }

/*******************************************************************************
*
* dummyCallback - dummy callback routine
*
* RETURNS:
* Always returs ERROR.
*
* NOMANUAL
*/

LOCAL STATUS dummyCallback (void)
    {
    return ERROR;
    }
