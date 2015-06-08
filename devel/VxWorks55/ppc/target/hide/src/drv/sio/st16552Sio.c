/* st16552Sio.c - ST 16C552 DUART tty driver */

/* Copyright 1984-1999 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01h,30jun99,jpd  improved interrupt efficiency with FIFOs (SPR #27952).
01g,23jul98,cdp  added ARM big-endian support.
01f,04dec97,jpd  updated to latest standards. Fix max/min baud setting.
01e,03oct97,jpd  Expanded comments/documentation.
01d,04apr97,jpd  Added multiplexed interrupt handler.
01c,18feb97,jpd  Tidied comments/documentation.
01b,03dec96,jpd  Added case IIR_TIMEOUT in st16550Int routine as per change
		 to version 01f of ns16550Sio.c
01a,18jul96,jpd  Created from ns1550Sio.c version 01e, by adding setting of
		 INT bit in MCR in InitChannel().
*/

/*
DESCRIPTION
This is the device driver for the Startech ST16C552 DUART, similar, but
not quite identical to the National Semiconductor 16550 UART.

The chip is a dual universal asynchronous receiver/transmitter with 16
byte transmit and receive FIFOs and a programmable baud-rate
generator. Full modem control capability is included and control over
the four interrupts that can be generated: Tx, Rx, Line status, and
modem status.  Only the Rx and Tx interrupts are used by this driver.
The FIFOs are enabled for both Tx and Rx by this driver.

Only asynchronous serial operation is supported by the UART which
supports 5 to 8 bit bit word lengths with or without parity and with
one or two stop bits. The only serial word format supported by the
driver is 8 data bits, 1 stop bit, no parity,  The default baud rate is
determined by the BSP by filling in the ST16552_CHAN structure before
calling st16552DevInit().

The exact baud rates supported by this driver will depend on the
crystal fitted (and consequently the input clock to the baud-rate
generator), but in general, baud rates from about 50 to about 115200
are possible.

.SH DATA STRUCTURES
An ST16552_CHAN data structure is used to describe the two channels of
the chip and, if necessary, an ST16552_MUX structure is used to
describe the multiplexing of the interrupts for the two channels of the
DUART.  These structures are described in h/drv/sio/st16552Sio.h.

.SH CALLBACKS
Servicing a "transmitter ready" interrupt involves making a callback to
a higher level library in order to get a character to transmit.  By
default, this driver installs dummy callback routines which do nothing.
A higher layer library that wants to use this driver (e.g. ttyDrv)
will install its own callback routine using the SIO_INSTALL_CALLBACK
ioctl command.  Likewise, a receiver interrupt handler makes a callback
to pass the character to the higher layer library.

.SH MODES
This driver supports both polled and interrupt modes.

.SH USAGE
The driver is typically only called by the BSP. The directly callable
routines in this module are st16552DevInit(), st16552Int(),
st16552IntRd(), st16552IntWr(), and st16552MuxInt.

The BSP's sysHwInit() routine typically calls sysSerialHwInit(), which
initialises all the hardware-specific values in the ST16552_CHAN
structure before calling st16552DevInit() which resets the device and
installs the driver function pointers. After this the UART will be
enabled and ready to generate interrupts, but those interrupts will be
disabled in the interrupt controller.

The following example shows the first parts of the initialisation:

.CS
#include "drv/sio/st16552Sio.h"

LOCAL ST16552_CHAN st16552Chan[N_16552_CHANNELS];

void sysSerialHwInit (void)
    {
    int i;

    for (i = 0; i < N_16552_CHANNELS; i++)
	{
	st16552Chan[i].regDelta = devParas[i].regSpace;
	st16552Chan[i].regs = devParas[i].baseAdrs;
	st16552Chan[i].baudRate = CONSOLE_BAUD_RATE;
	st16552Chan[i].xtal = UART_XTAL_FREQ;
	st16552Chan[i].level = devParas[i].intLevel;

	/@
	 * Initialise driver functions, getTxChar, putRcvChar and
	 * channelMode and init UART.
	 @/

	st16552DevInit(&st16552Chan[i]);
	}
    }
.CE

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(),
which connects the chips interrupts via intConnect() (either the single
interrupt `st16552Int', the three interrupts `st16552IntWr',
`st16552IntRd', and `st16552IntEx', or the multiplexed interrupt
handler `st16552MuxInt' which will cope with both channels of a DUART
producing the same interrupt). It then enables those interrupts in the
interrupt controller as shown in the following example:

.CS
void sysSerialHwInit2 (void)
    {
    /@ Connect the multiplexed interrupt handler @/

    (void) intConnect (INUM_TO_IVEC(devParas[0].vector),
			st16552MuxInt, (int) &st16552Mux);
    intEnable (devParas[0].intLevel);
    }
.CE

.SH BSP
By convention all the BSP-specific serial initialisation is performed
in a file called sysSerial.c, which is #include'ed by sysLib.c.
sysSerial.c implements at least four functions, sysSerialHwInit()
sysSerialHwInit2(), sysSerialChanGet(), and sysSerialReset(). The first
two have been described above, the others work as follows:

sysSerialChanGet is called by usrRoot to get the serial channel
descriptor associated with a serial channel number. The routine takes a
single parameter which is a channel number ranging between zero and
NUM_TTY. It returns a pointer to the corresponding channel descriptor,
SIO_CHAN *, which is just the address of the ST16552_CHAN strucure.

sysSerialReset is called from sysToMonitor() and should reset the
serial devices to an inactive state (prevent them from generating any
interrupts).

.SH INCLUDE FILES:
drv/sio/st16552Sio.h sioLib.h

SEE ALSO:
.I "Startech ST16C552 Data Sheet"
*/

#include "vxWorks.h"
#include "intLib.h"
#include "errnoLib.h"
#include "errno.h"
#include "sioLib.h"
#include "drv/sio/st16552Sio.h"

/* local defines  */

#ifndef ST16552_REG_READ
#if (CPU_FAMILY == ARM) && (_BYTE_ORDER == _BIG_ENDIAN)
#define ST16552_REG_READ(pChan, reg, result) \
	result = \
	    (*(volatile UINT8 *)(3+(UINT32)pChan->regs + (reg*pChan->regDelta)))
#else
#define ST16552_REG_READ(pChan, reg, result) \
	result = \
	    (*(volatile UINT8 *)((UINT32)pChan->regs + (reg*pChan->regDelta)))
#endif	/* _BYTE_ORDER == _BIG_ENDIAN */
#endif	/* ifndef ST16552_REG_READ */

#ifndef ST16552_REG_WRITE
#define ST16552_REG_WRITE(pChan, reg, data) \
	(*(volatile UINT8 *)((UINT32)pChan->regs + (reg*pChan->regDelta))) = \
		(data)
#endif


/* locals */

LOCAL BOOL multiplexed = FALSE;	/* whether int from mux-ed handler */
LOCAL char intStatus;



/* function prototypes */

LOCAL STATUS st16552DummyCallback ();
LOCAL void st16552InitChannel (ST16552_CHAN *);
LOCAL STATUS st16552Ioctl (SIO_CHAN * pSioChan, int request, int arg);
LOCAL int st16552TxStartup (SIO_CHAN * pSioChan);
LOCAL int st16552CallbackInstall (SIO_CHAN * pSioChan, int callbackType,
			       STATUS (*callback)(), void * callbackArg);
LOCAL int st16552PollInput (SIO_CHAN * pSioChan, char *);
LOCAL int st16552PollOutput (SIO_CHAN * pSioChan, char);

/* driver functions */

LOCAL SIO_DRV_FUNCS st16552SioDrvFuncs =
    {
    (int (*)())st16552Ioctl,
    st16552TxStartup,
    st16552CallbackInstall,
    st16552PollInput,
    st16552PollOutput
    };

/*******************************************************************************
*
* st16552DummyCallback - dummy callback routine.
*
* RETURNS: ERROR, always.
*/

LOCAL STATUS st16552DummyCallback (void)
    {
    return ERROR;
    }

/*******************************************************************************
*
* st16552DevInit - initialise an ST16552 channel
*
* This routine initialises some SIO_CHAN function pointers and then resets
* the chip in a quiescent state.  Before this routine is called, the BSP
* must already have initialised all the device addresses, etc. in the
* ST16552_CHAN structure.
*
* RETURNS: N/A
*/

void st16552DevInit
    (
    ST16552_CHAN *	pChan
    )
    {
    int oldlevel = intLock();


    /* initialise the driver function pointers in the SIO_CHAN */

    pChan->sio.pDrvFuncs = &st16552SioDrvFuncs;


    /* set the non BSP-specific constants */

    pChan->getTxChar = st16552DummyCallback;
    pChan->putRcvChar = st16552DummyCallback;

    pChan->channelMode = 0;    /* undefined */


    /* reset the chip */

    st16552InitChannel(pChan);

    intUnlock(oldlevel);

    }

/*******************************************************************************
*
* st16552InitChannel - initialise UART
*
* This routine performs hardware initialisation of the UART channel.
*
* RETURNS: N/A
*/

LOCAL void st16552InitChannel
    (
    ST16552_CHAN *	pChan	/* ptr to ST16552 struct describing channel */
    )
    {
    /* Configure Port -  Set 8 bits, 1 stop bit, no parity. */

    /* keep soft copy */

    pChan->lcr = (UINT8)(CHAR_LEN_8 | ONE_STOP | PARITY_NONE);

    /* Set Line Control Register */

    ST16552_REG_WRITE(pChan, LCR, pChan->lcr);


    /* Reset/Enable the FIFOs */

    ST16552_REG_WRITE(pChan, FCR, RxCLEAR | TxCLEAR | FIFO_ENABLE);


    /* Enable access to the divisor latches by setting DLAB in LCR. */

    ST16552_REG_WRITE(pChan, LCR, LCR_DLAB | pChan->lcr);


    /* Set divisor latches to set baud rate */

    ST16552_REG_WRITE(pChan, DLL,  pChan->xtal/(16*pChan->baudRate));
    ST16552_REG_WRITE(pChan, DLM, (pChan->xtal/(16*pChan->baudRate)) >> 8);


    /* Restore Line Control Register */

    ST16552_REG_WRITE(pChan, LCR, pChan->lcr);


    /*
     * This appears to be different from the NS 16550, which defines Bit 3 to
     * be a general purpose output. The ST 16552 datasheet defines it to set
     * the INT output pin from tri-state to normal active mode. If this is not
     * done, then no interrupts are generated, at least on the ARM PID7T
     * board.
     */

    /* Set INT output pin to normal/active operating mode and assert DTR */

    ST16552_REG_WRITE(pChan, MCR, MCR_INT | MCR_DTR);


    /* Make a copy of Interrupt Enable Register */

    pChan->ier = (UINT8)(RxFIFO_BIT | TxFIFO_BIT);


    /* Disable interrupts */

    ST16552_REG_WRITE(pChan, IER, 0);

    }

/*******************************************************************************
*
* st16552Ioctl - special device control
*
* This routine handles the IOCTL messages from the user.
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
* request.
*/

LOCAL STATUS st16552Ioctl
    (
    SIO_CHAN *		pSioChan, /* ptr to SIO_CHAN describing this channel */
    int			request,  /* request code */
    int			arg	  /* some argument */
    )
    {
    int 		oldlevel; /* current interrupt level mask */
    STATUS		status;	  /* status to return */
    ST16552_CHAN *	pChan = (ST16552_CHAN *)pSioChan;
    UINT32		brd;	  /* baud rate divisor */


    status = OK;	/* preset to return OK */

    switch (request)
	{
	case SIO_BAUD_SET:
	    /*
	     * Set the baud rate. Return EIO for an invalid baud rate, or
	     * OK on success.
	     *
	     * baudrate divisor must be non-zero and must fit in a 16-bit
	     * register.
	     */

	    brd = pChan->xtal/(16*arg);	/* calculate baudrate divisor */

	    if ((brd < 1) || (brd > 0xFFFF))
	    	{
	    	status = EIO;		/* baud rate out of range */
	    	break;
	    	}

	    /* disable interrupts during chip access */

	    oldlevel = intLock ();


	    /* Enable access to the divisor latches by setting DLAB in LCR. */

	    ST16552_REG_WRITE(pChan, LCR, LCR_DLAB | pChan->lcr);


	    /* Set divisor latches. */

	    ST16552_REG_WRITE(pChan, DLL, brd);
	    ST16552_REG_WRITE(pChan, DLM, brd >> 8);


	    /* Restore Line Control Register */

	    ST16552_REG_WRITE(pChan, LCR, pChan->lcr);

	    pChan->baudRate = arg;

	    intUnlock (oldlevel);

	    break;


	case SIO_BAUD_GET:
	    *(int *)arg = pChan->baudRate;
	    break;


	case SIO_MODE_SET:
	    if ((arg != SIO_MODE_POLL) && (arg != SIO_MODE_INT))
		{
		status = EIO;
		break;
		}

	    oldlevel = intLock ();

	    if (arg == SIO_MODE_INT)
		{
		/* Enable appropriate interrupts */

		ST16552_REG_WRITE(pChan,
				IER, pChan->ier | RxFIFO_BIT | TxFIFO_BIT);
		}
	    else
		{
		/* Disable the interrupts */

		ST16552_REG_WRITE(pChan, IER, 0);
		}

	    pChan->channelMode = arg;

	    intUnlock(oldlevel);
	    break;


	case SIO_MODE_GET:
	    *(int *)arg = pChan->channelMode;
	    break;


	case SIO_AVAIL_MODES_GET:
	    *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    break;


	case SIO_HW_OPTS_SET:
	case SIO_HW_OPTS_GET:
	default:
	    status = ENOSYS;
	}

    return status;

    }

/*******************************************************************************
*
* st16552IntWr - handle a transmitter interrupt
*
* This routine handles write interrupts from the UART.
*
* RETURNS: N/A
*/

void st16552IntWr
    (
    ST16552_CHAN *	pChan	/* ptr to struct describing channel */
    )
    {
    char outChar;
    BOOL gotOne;
    UINT32 status;

    do
    	{
	/* get a character to send, if available */

	gotOne = (*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR;

	if (gotOne)				/* there was one, so send it */
	    ST16552_REG_WRITE(pChan, THR, outChar);	/* write char to THR */

	ST16552_REG_READ(pChan, LSR, status);	/* read status */
	}
    while (gotOne && ((status & LSR_THRE) != 0));

    if (!gotOne)
	{
	/* no more chars to send, disable Tx interrupt */

	pChan->ier &= (~TxFIFO_BIT);			/* update copy */
	ST16552_REG_WRITE(pChan, IER, pChan->ier);	/* disable Tx int */
	}

    return;
    }

/*****************************************************************************
*
* st16552IntRd - handle a receiver interrupt
*
* This routine handles read interrupts from the UART.
*
* RETURNS: N/A
*/

void st16552IntRd
    (
    ST16552_CHAN *	pChan	/* ptr to struct describing channel */
    )
    {
    char inchar;
    UINT32 status;

    do
	{
	/* read character from Receive Holding Reg. */

	ST16552_REG_READ(pChan, RBR, inchar);

	/* send it on to upper level, via installed callback */

	(*pChan->putRcvChar) (pChan->putRcvArg, inchar);

	ST16552_REG_READ(pChan, LSR, status);	/* read status, more chars? */
	}
    while ((status & LSR_DR) != 0);

    return;
    }

/**********************************************************************
*
* st16552IntEx - miscellaneous interrupt processing
*
* This routine handles miscellaneous interrupts on the UART.
*
* RETURNS: N/A
*/

void st16552IntEx
    (
    ST16552_CHAN *	pChan	/* ptr to struct describing channel */
    )
    {

    /* Nothing for now... */
    }

/******************************************************************************
*
* st16552Int - interrupt level processing
*
* This routine handles interrupts from the UART.
*
* RETURNS: N/A
*/

void st16552Int
    (
    ST16552_CHAN *	pChan	/* ptr to struct describing channel */
    )
    {
    /*
     * If this routine has been called from the multiplexed interrupt
     * handler, then we have already read the Interrupt Status Register,
     * and must not read it again, else this routine has been installled
     * directly using intConnect() and we must now read the Interrupt
     * Status Register (or Interrupt Identification Register).
     */

     if (!multiplexed)
	 {
	 ST16552_REG_READ(pChan, IIR, intStatus);
	 intStatus &= 0x0F;
	 }

    /*
     * This UART chip always produces level active interrupts, and the IIR
     * only indicates the highest priority interrupt.
     * In the case that receive and transmit interrupts happened at
     * the same time, we must clear both interrupt pending to prevent
     * edge-triggered interrupt(output from interrupt controller) from locking
     * up. One way doing it is to disable all the interrupts at the beginning
     * of the ISR and enable at the end.
     */

    ST16552_REG_WRITE(pChan, IER, 0);	/* disable interrupts */

    switch (intStatus)
	{
	case IIR_RLS:
	    /*
	     * overrun, parity error and break interrupt:
	     *
	     * read LSR to reset interrupt
	     */

	    ST16552_REG_READ(pChan, LSR, intStatus);
	    break;


	case IIR_RDA:
	    /* received data available: FALL THROUGH to timeout */

	case IIR_TIMEOUT:
	    /*
	     * receiver FIFO interrupt. In some cases, IIR_RDA
	     * will not be indicated in IIR register when there
	     * is more than one char. in FIFO.
	     */

	    st16552IntRd (pChan);	/* at least one RxChar available */
	    break;


	case IIR_THRE:
	    st16552IntWr (pChan);	/* can transmit at least one char */
	    break;


	default:
	    break;
	}

    ST16552_REG_WRITE(pChan, IER, pChan->ier); /* enable ints accordingly */

    }

/******************************************************************************
*
* st16552MuxInt - multiplexed interrupt level processing
*
* This routine handles multiplexed interrupts from the DUART. It assumes that
* channels 0 and 1 are connected so that they produce the same interrupt.
*
* RETURNS: N/A
*/

void st16552MuxInt
    (
    ST16552_MUX *	pMux	/* ptr to struct describing multiplexed chans */
    )
    {
    ST16552_CHAN *	pChan;

    /* get pointer to structure for channel to examine first */

    pChan = &(pMux->pChan[pMux->nextChan]);


    /*
     * Step on the next channel to examine: use round-robin for which to
     * examine first.
     */

    if (pMux->nextChan == 0)
	pMux->nextChan = 1;
    else
	pMux->nextChan = 0;


    /*
     * Let the st16552Int() routine know it is called from here, not direct
     * from intConnect().
     */

    multiplexed = TRUE;


    /* check Interrupt Status Register for this channel */

    ST16552_REG_READ(pChan, IIR, intStatus);
    intStatus &= 0x0F;


    if ((intStatus & 0x01) == 0)
	{
	/* Call int handler if int active */

	st16552Int (pChan);
	}
    else
	{
	/* step on again */

	pChan = &pMux->pChan[pMux->nextChan];

	if (pMux->nextChan == 0)
	    pMux->nextChan = 1;
	else
	    pMux->nextChan = 0;


	/* get interrupt status for next channel */

	ST16552_REG_READ(pChan, IIR, intStatus);
	intStatus &= 0x0F;


	/* and call interrupt handler if active */

	if ((intStatus & 0x01) == 0)
	    st16552Int (pChan);
	}

    multiplexed = FALSE;

    return;
    }

/*******************************************************************************
*
* st16552TxStartup - transmitter startup routine
*
* Call interrupt level character output routine and enable interrupt!
*
* RETURNS: OK on success, ENOSYS if the device is polled-only, or
* EIO on hardware error.
*/

LOCAL int st16552TxStartup
    (
    SIO_CHAN *		pSioChan  /* ptr to SIO_CHAN describing this channel */
    )
    {
    ST16552_CHAN *	pChan = (ST16552_CHAN *)pSioChan;

    if (pChan->channelMode == SIO_MODE_INT)
	{
	pChan->ier |= TxFIFO_BIT;
	ST16552_REG_WRITE(pChan, IER, pChan->ier);
	return OK;
	}
    else
	return ENOSYS;
    }

/******************************************************************************
*
* st16552PollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
* if the output buffer is full, ENOSYS if the device is interrupt-only.
*/

LOCAL int st16552PollOutput
    (
    SIO_CHAN *		pSioChan, /* ptr to SIO_CHAN describing this channel */
    char		outChar	  /* char to be output */
    )
    {
    ST16552_CHAN *	pChan = (ST16552_CHAN *)pSioChan;
    char pollStatus;

    ST16552_REG_READ(pChan, LSR, pollStatus);

    /* is the transmitter ready to accept a character? */

    if ((pollStatus & LSR_THRE) == 0x00)
	return EAGAIN;


    /* write out the character */

    ST16552_REG_WRITE(pChan, THR, outChar);	/* transmit character */

    return OK;
    }

/******************************************************************************
*
* st16552PollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
* if the input buffer is empty, ENOSYS if the device is interrupt-only.
*/

LOCAL int st16552PollInput
    (
    SIO_CHAN *		pSioChan, /* ptr to SIO_CHAN describing this channel */
    char *		thisChar  /* ptr to where to return character */
    )
    {
    ST16552_CHAN *	pChan = (ST16552_CHAN *)pSioChan;
    char pollStatus;

    ST16552_REG_READ(pChan, LSR, pollStatus);

    if ((pollStatus & LSR_DR) == 0x00)
	return EAGAIN;


    /* got a character */

    ST16552_REG_READ(pChan, RBR, *thisChar);

    return OK;

    }

/******************************************************************************
*
* st16552CallbackInstall - install ISR callbacks to get/put chars.
*
* This routine installs interrupt callbacks for transmitting characters
* and receiving characters.
*
* RETURNS: OK on success, or ENOSYS for an unsupported callback type.
*/

LOCAL int st16552CallbackInstall
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    int		callbackType,	/* type of callback */
    STATUS	(*callback)(),	/* callback */
    void *	callbackArg	/* parameter to callback */

    )
    {
    ST16552_CHAN * pChan = (ST16552_CHAN *)pSioChan;

    switch (callbackType)
	{
	case SIO_CALLBACK_GET_TX_CHAR:
	    pChan->getTxChar	= callback;
	    pChan->getTxArg	= callbackArg;
	    return OK;

	case SIO_CALLBACK_PUT_RCV_CHAR:
	    pChan->putRcvChar	= callback;
	    pChan->putRcvArg	= callbackArg;
	    return OK;

	default:
	    return ENOSYS;
	}
    }
