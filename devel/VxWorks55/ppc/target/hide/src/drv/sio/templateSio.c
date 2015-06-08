/* templateSio.c - template serial driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
starting with version 01a and growing the history upward with
each revision.

modification history
--------------------
01o,30may02,dat  mismatch argument warning from diab
01n,05apr02,dat  Poll mode cannot make any callbacks
01m,08dec01,dat  compiler warnings
01l,24sep01,dat  Update for new IOCTL codes/callbacks, new adaptor type
		 macros
01k,07mar01,dat  36080, new driver components, renamed TEMPLATE_REG_READ/WRITE
		 to be TEMPLATE_READ/WRITE
01j,11may98,dat  change Open/Hup handling slightly. SPR 21198
01i,23sep97,dat  added TEMPLATE_REG_READ, TEMPLATE_REG_WRITE
01h,10sep97,dat  added opt declarations for SIO_HUP, SIO_OPEN
01g,10aug97,dat  fixed bugs in ver 01f
01f,01jul97,db	 added ioctl commands used in modem control(SPR 7637,1037).
01e,16apr97,dat  fixed baud rate calc for SIO_BAUD_GET, SPR 8405.
01d,06mar97,dat  doc cleanup
01c,16dec96,dat  added templateOptSet() hardware options template
01b,16oct96,dat  added templateDevInit2(), and templateIntrMode variable.
01a,01aug95,ms   written.
*/

/*
.SH TODO

 - Replace the documentation for this template driver with documentation
for the driver being written.  

 - Begin with an overview of the complete device.  Indicate if the new driver
only implements a sub-section of the whole device or not.

 - Describe all of the operating modes of the device, and indicate which
ones this driver implements.

 - Document the device initialization steps to be used in the BSP to create
and initialize the device.  Document all the macros that
can be used to customize the driver to a particular hardware environment.

 - Document anything that will help the user to understand how this device
works and interacts with this driver.

.SH TEMPLATE OVERVIEW
This is a template serial driver. It can be used as a starting point
when writing new drivers for VxWorks version 5.4 or later.

These drivers support new functionality not found in the older style
serial drivers. First, they provide an interface for setting hardware
options; e.g., the number of stop bits, data bits, parity, etc.
Second, they provide an interface for polled communication which
can be used to provided external mode debugging (i.e., ROM monitor
style debugging) over a serial line. Currently only asynchronous mode
drivers are supported.

Throughout this file the word "template" is used in place of a real
device name, which by convention uses the first letter of the
manufacturers name followed by the part number. For example, the
Zilog 8530 serial device would have a data structure called a
Z8530_CHAN, rather than TEMPLATE_CHAN.

.SH CALLBACKS
Servicing a "transmitter ready" interrupt involves making a callback to a
higher level library in order to get a character to transmit.
By default, this driver installs dummy callback routines which do
nothing. A higher layer library that wants to use this driver (e.g., ttyDrv)
will install its own callback routines using the SIO_INSTALL_CALLBACK
ioctl command. (See below).

The prototype for the transmit data callback SIO_CALLBACK_GET_TX_CHAR is:
.CS
    int sioTxCharGet
	(
	void * arg,	     /@ callback argument @/
	char * pChar	     /@ ptr to data location @/
	)
.CE
This callback routine should fetch the next character to send and store it
in the location pointed to by pChar.  It returns OK to indicate that a
character is ready and should be sent.  It returns ERROR to indicate that
no character was available and the transmitter can be placed in an idle state.

Likewise, a receiver interrupt handler makes a callback to pass the
character to the higher layer library.  It will be called by the driver
for each character received.  This routine should capture the data and pass
it along to other layers and eventually to the user.

The prototype for the receive data callback SIO_CALLBACK_PUT_RCV_CHAR is:
.CS
    void sioRxCharPut
	(
	void * arg,	     /@ callback argument @/
	char data	     /@ data byte @/
	)
.CE
A new error handling callback has been added SIO_CALLBACK_ERROR. This driver
should use this callback to inform the higher layer about error conditions.
The prototype for this callback is:
.CS
    void sioErrorRtn
	(
	void * arg,	     /@ callback argument @/
	int code,	     /@ error code @/
	void * pData,	     /@ opt dev specific data @/
	int dataSize         /@ opt size of data in bytes @/
	)
.CE
The available error codes for this callback are:
.CS
    SIO_ERROR_FRAMING	 /@ (1) Framing error @/
    SIO_ERROR_PARITY	 /@ (2) Parity error @/
    SIO_ERROR_OFLOW	 /@ (3) data overflow @/
    SIO_ERROR_UFLOW	 /@ (4) data underflow @/
    SIO_ERROR_CONNECT	 /@ (5) connection made @/
    SIO_ERROR_DISCONNECT /@ (6) connection lost @/
    SIO_ERROR_NO_CLK	 /@ (7) clock lost @/
    SIO_ERROR_UNKNWN	 /@ (8) other errors @/
.CE
For engineering purposes, the driver may return device specific data in
the form of a data buffer. The argument pData is the location of the buffer
and dataSize is its size, in bytes.  The data is not guaranteed to be static
so it should be copied to a static buffer for safekeeping.

.SH MODES
Ideally the driver should support both polled and interrupt modes, and be
capable of switching modes dynamically. However this is not required.
VxWorks will be able to support a tty device on this driver even if
the driver only supports interrupt mode.
Polled mode is provided solely for WDB system mode usage.  Users can use
the polled mode interface directly, but there is no clear reason for doing so.
Normal access to SIO devices through ttyDrv only uses interrupt mode.

For dynamically switchable drivers, be aware that the driver may be
asked to switch modes in the middle of its input ISR. A driver's input ISR
will look something like this:

   inChar = *pDev->dr;          /@ read a char from data register @/
   *pDev->cr = GOT_IT;          /@ acknowledge the interrupt @/
   pDev->putRcvChar (...);      /@ give the character to the higher layer @/

If this channel is used as a communication path to an external mode
debug agent, and if the character received is a special "end of packet"
character, then the agent's callback will lock interrupts, switch
the device to polled mode, and use the device in polled mode for awhile.
Later on the agent will unlock interrupts, switch the device back to
interrupt mode, and return to the ISR.
In particular, the callback can cause two mode switches, first to polled mode
and then back to interrupt mode, before it returns.
This may require careful ordering of the callback within the interrupt
handler. For example, you may need to acknowledge the interrupt before
invoking the callback.

.SH USAGE
The driver is typically called only by the BSP. The directly callable
routines in this module are templateSioCreate(), templateSioDestroy().

.SH BSP
By convention all the BSP-specific serial initialization is performed in
a file called sysSerial.c, which is #include'ed by sysLib.c.

This driver can be customized by redefining the macros SYS_SIO_READ8 and
SYS_SIO_WRITE8.  These two macros are used for all accesses to the
actual chip.  If not defined, the source code will assume a simple memory
mapped device using byte read/write access to all registers.

The macros SYS_SIO_INT_CONNECT, SYS_SIO_INT_DISCONNECT, SYS_SIO_INT_ENABLE,
and SYS_SIO_PROBE can be redefined by the BSP to perform basic low level
routines with the same calling sequence as intConnect(), intConnect(),
intEnable(), and vxMemProbe().

.SH TESTING
The interrupt driven interface can be tested in the usual way; VxWorks
prints to the serial console when it comes up, so seeing the usual VxWorks
output on power-up shows that the driver is basically working.
The VxWorks portkit test can be used to perform a more strenuous test.

The polled interface should be easy enough to verify - you should be able
to call the channels SIO_MODE_SET ioctl to put it in polled mode.  Note
that the usual print tools won't work with a serial channel in polled mode.
Some agent has to perform a polling loop to handle input/output on a
character by character basis.  It is not automatic.  The WDB agent
performs its own polling loop when it switches the WDB serial line into
polled mode. 

The dynamic mode switching can be verified using the tornado tools.
Reconfigure the agent to use the WDB_COMM_UDLP_SLIP communication path (see
the Configuration section in the VxWorks run-time Guide for details).
Start VxWorks, and connect the tgtsvr to the agent using the wdbserial
backend (see the Tornado Users Guide for details).
Start the wtxtcl shell as follows:
	% wtxtcl
From the tcl prompt, attach to the target server:
	wtxtcl.ex> wtxToolAttach <tgtsvr name>
Tell the agent to switch to external mode, and verify the reply is OK (0).
	wtxtcl.ex>wtxAgentModeSet 2
	0
Ask the agent to suspend the system (the request will reach the agent in
interrupt mode, but the reply will be sent in polled mode):
	wtxtcl.ex>wtxContextSuspend 0 0
	0
At this point the target will be suspended. The console should apprear
frozen (if the board has a console device), and you will not be able to
"ping" the target's network interface.
Resume the target:
	wtxtcl.ex>wtxContextResume 0 0
	0
The target should now be running again, so you should be able to type into
the console (if the board has a console device) and ping the targets network
interface from the host.

.SH INCLUDE FILES:
drv/sio/templateSio.h sioLib.h
*/

#include "vxWorks.h"
#include "intLib.h"
#include "errnoLib.h"
#include "iosLib.h" 	/* For S_iosLib_DEVICE_NOT_FOUND */
#include "cacheLib.h"
#include "stdlib.h"	/* malloc/free */
#include "stdio.h"	/* for printf */
#include "vxLib.h"	/* for vxMemProbe */
#include "drv/sio/templateSio.h"

/* device and channel structures */

/* TODO - Define a driver specific extended SIO_CHAN structure */

typedef struct
    {
    /* SIO_CHAN *MUST* be first */

    SIO_CHAN	sio;		/* standard SIO_CHAN element */

    UINT32	ioBase;
    UINT32	vecBase;
    UINT32	intLevel;

    /* callbacks */

    STATUS      (*getTxChar) (void *, char *);
    void        (*putRcvChar) (void *, char);
    void        (*errorRtn) (void *, int, void *, int);
    void *	getTxArg;
    void *	putRcvArg;
    void *	errorArg;

    /* misc */

    int		intConnect;	/* intConnect done flag */
    int		baudFreq;	/* current baud rate */
    int         mode;           /* current mode (interrupt or poll) */
    int         clkFreq;       /* input clock frequency */
    uint_t	options;	/* Hardware options */
    int		scanMode;	/* keyboard mapping mode */
    } TEMPLATE_CHAN;



/* channel control register (write) definitions */

/* TODO - These are just made up bit defines for a mythical device! */

#define TEMPLATE_RESET_CHIP	0x07	/* reset all */
#define TEMPLATE_RESET_TX	0x01	/* reset the transmitter */
#define TEMPLATE_RESET_ERR	0x02	/* reset error condition */
#define TEMPLATE_RESET_INT	0x04	/* acknoledge the interrupt */
#define	TEMPLATE_INT_ENABLE	0x08	/* enable interrupts */
#define	TEMPLATE_TX_ENABLE	0x10	/* enable interrupts */
#define	TEMPLATE_RX_ENABLE	0x20	/* enable interrupts */

/* channel status register (read) definitions */

#define TEMPLATE_CR_OKAY	0x00	/* no error conditions */
#define TEMPLATE_CR_TX_READY	0x01	/* txmitter ready for another char */
#define TEMPLATE_CR_TX_ERROR	0x04	/* txmitter int enable */
#define TEMPLATE_CR_RX_AVAIL	0x10	/* character has arrived */
#define TEMPLATE_CR_RX_ERROR	0x40	/* receiver error */

/* channel modem status register definitions */

#define	TEMPLATE_MSR_RTS	0x1	/* RTS signal asserted */
#define TEMPLATE_MSR_DTR	0x2	/* DTR signal asserted */
#define TEMPLATE_MSR_DSR	0x4	/* DSR signal asserted */
#define TEMPLATE_MSR_CTS	0x8	/* CTS signal asserted */
#define TEMPLATE_MSR_CD		0x10	/* CD signal asserted */

/* input and output signals */

#define TEMPLATE_ISIG_MASK	(SIO_MODEM_CTS|SIO_MODEM_DSR|SIO_MODEM_CD)
#define TEMPLATE_OSIG_MASK	(SIO_MODEM_RTS|SIO_MODEM_DTR)

/* channel modem control register definitions */

#define TEMPLATE_MC_RTS		0x1	/* enable RTS */
#define TEMPLATE_MC_DTR		0x2	/* enable DTR */

#define TEMPLATE_BAUD_MIN	75
#define TEMPLATE_BAUD_MAX	38400

/* Hardware abstraction macros */

/* Macros from BSP */

#define SYS_SIO_READ8(addr, pData) \
	(*pData = *(UINT8 *)(addr))
#define SYS_SIO_WRITE8(addr, data) \
	(*(UINT8 *)addr = data)
#define SYS_SIO_INT_CONNECT(vec, rtn, arg) \
	intConnect ((VOIDFUNCPTR *)vec, (VOIDFUNCPTR)rtn, (int)arg)
#define SYS_SIO_INT_DISCONNECT(vec, rtn, arg) \
	intConnect ((VOIDFUNCPTR *)vec, NULL, 0)
#define SYS_SIO_INT_ENABLE(level) \
	intEnable (level)
#define SYS_SIO_PROBE(addr, mode, size, pData) \
	vxMemProbe (addr, mode, size, pData)
/* #define CACHE_PIPE_FLUSH() */


/* Local driver abstractions, following bus/adaptor model */

#define TEMPLATE_SIO_READ8(pChan, reg, result) \
	SYS_SIO_READ8(((pChan->ioBase) + reg),result)

#define TEMPLATE_SIO_WRITE8(pChan, reg, data) \
	SYS_SIO_WRITE8(((pChan->ioBase) + reg),data)

#define TEMPLATE_SIO_INT_CONNECT(pChan, vec, rtn, arg) \
	do { \
	SYS_SIO_INT_CONNECT((pChan->vecBase) + vec, rtn, arg); \
	SYS_SIO_INT_ENABLE(pChan->intLevel); \
	} while (0)

#define TEMPLATE_INT_DISCONNECT(pChan, vec, rtn, arg) \
	SYS_SIO_INT_DISCONNECT(((pChan)->vecBase + vec), rtn, arg)
	/* Do NOT disable interrupt level for disconnect */

#define TEMPLATE_PROBE(pChan, offset, dir, size, ptr) \
	SYS_SIO_PROBE((char *)((pChan)->ioBase + offset), dir, size, ptr)

#define TEMPLATE_PIPE_FLUSH(pChan) \
	CACHE_PIPE_FLUSH()

/* forward static declarations */


LOCAL int	templateTxStartup (SIO_CHAN * pSioChan);
LOCAL int	templateCallbackInstall (SIO_CHAN *pSioChan, int callbackType,
			STATUS (*callback)(void *,...), void *callbackArg);
LOCAL int	templatePollOutput (SIO_CHAN *pSioChan, char	outChar);
LOCAL int	templatePollInput (SIO_CHAN *pSioChan, char *thisChar);
LOCAL int	templateIoctl (SIO_CHAN *pSioChan, int request, void *arg);
LOCAL STATUS	dummyTxCallback (void *, char *);
LOCAL void	dummyRxCallback (void *, char);
LOCAL void	dummyErrCallback (void *, int, void *, int);
LOCAL void	templateSioIntTx (TEMPLATE_CHAN * pChan);
LOCAL void	templateSioIntRcv (TEMPLATE_CHAN * pChan);
LOCAL void	templateSioIntErr (TEMPLATE_CHAN * pChan);
LOCAL STATUS	templateProbe (TEMPLATE_CHAN *);
LOCAL STATUS	templateVerify (TEMPLATE_CHAN *);
LOCAL int	templateMstatGet (TEMPLATE_CHAN *);
LOCAL int	templateMstatSetClr (TEMPLATE_CHAN *, UINT bits, BOOL setFlag);

/* local variables */

LOCAL	SIO_DRV_FUNCS templateSioDrvFuncs =
    {
    templateIoctl,
    templateTxStartup,
    templateCallbackInstall,
    templatePollInput,
    templatePollOutput
    };

/******************************************************************************
*
* templateSioCreate - create a TEMPLATE_CHAN instance
*
* This routine initializes the driver
* function pointers and then resets the chip to a quiescent state.
*
* RETURNS: N/A
*/

SIO_CHAN * templateSioCreate
    (
    UINT32	ioBase,
    UINT32	vecBase,
    UINT32	level,
    UINT32	clkFreq
    )
    {
    TEMPLATE_CHAN * pChan;

    pChan = (TEMPLATE_CHAN *) malloc (sizeof (TEMPLATE_CHAN));

    if (pChan != NULL)
	{
	pChan->sio.pDrvFuncs	= &templateSioDrvFuncs;
	pChan->ioBase		= ioBase;
	pChan->vecBase		= vecBase;
	pChan->intLevel		= level;
	pChan->clkFreq		= clkFreq;

	/* install dummy driver callbacks */

	pChan->getTxChar    = dummyTxCallback;
	pChan->putRcvChar   = dummyRxCallback;
	pChan->errorRtn	    = dummyErrCallback;

	pChan->intConnect    = FALSE; /* int's not connected yet */
	
	if (templateProbe (pChan) == OK)
	    {
	    TEMPLATE_SIO_WRITE8(pChan,
			TEMPLATE_CSR_ID, TEMPLATE_RESET_CHIP);

	    /* setting polled mode is one way to make the device quiet */

	    templateIoctl (&pChan->sio, SIO_MODE_SET, (void *)SIO_MODE_POLL);
	    }
	else
	    {
	    /* Whoops, device is not there! */

	    free ((char *)pChan);
	    errnoSet (S_iosLib_DEVICE_NOT_FOUND);
	    return NULL;
	    }
	}

    return &pChan->sio;
    }

/******************************************************************************
*
* templateSioDestroy - destroy a TEMPLATE_CHAN instance
*
* This function is used to destroy an instance of a TEMPLATE_CHAN object.
*
* RETURNS: OK upon success, or ERROR on failure or if the object is not a
* valid TEMPLATE_CHAN object.
*/

STATUS templateSioDestroy
    (
    SIO_CHAN * pSioChan
    )
    {
    TEMPLATE_CHAN * pChan = (TEMPLATE_CHAN *)pSioChan;

    /*
     * Verify that it is a valid TEMPLATE_CHAN device, before doing
     * anything.
     */

    if (templateVerify(pChan) == OK)
	{
	if (pChan->intConnect == TRUE)
	    {
	    TEMPLATE_INT_DISCONNECT(pChan,TEMPLATE_RXINT_ID,
			    templateSioIntRcv, (void *)pChan);
	    TEMPLATE_INT_DISCONNECT(pChan, TEMPLATE_TXINT_ID,
			    templateSioIntTx, (void *)pChan);
	    TEMPLATE_INT_DISCONNECT(pChan, TEMPLATE_ERRINT_ID,
			    templateSioIntErr, (void *)pChan);
	    }

	free (pChan);

	return OK;
	}

    return ERROR;
    }

/******************************************************************************
*
* templateSioIntRcv - handle a channel's receive-character interrupt
*
* RETURNS: N/A
*/ 

LOCAL void templateSioIntRcv
    (
    TEMPLATE_CHAN *	pChan		/* channel generating the interrupt */
    )
    {
    char   inChar;  /* rcvr data */
    char   crData = 0; /* rcvr status */
    STATUS status = OK;

    /*
     * TODO - 
     *
     * Get status and data from the device. Determine if valid data is ready
     * or not.
     *
     * For PCI devices, do an immediate return if device is not asserting
     * an interrupt.
     */

    TEMPLATE_SIO_READ8(pChan, TEMPLATE_CSR_ID, &crData);

    /* Check for error conditions */

    if (crData & TEMPLATE_CR_OKAY)
	{
	TEMPLATE_SIO_READ8(pChan, TEMPLATE_DATA_ID, &inChar);

#if 0 /* Keyboard emulation code */

	/*
	 * TODO - For a keyboard type device we would map the raw scan code
	 * to ASCII, or other mapping.  Do that here.
	 */

	if (pChan->scanMode == SIO_KYBD_MODE_ASCII)
	    inChar = templateAsciiTbl[(UINT8)inChar];
#endif
	}
    else
	{
	/*
	 * TODO - Determine precise error condition and perform
	 * recovery actions.
	 */

	status = ERROR;
	}

    /*
     * TODO -  Typically, acknowledge the interrupt as soon as possible.
     * Usually before passing data or error conditions upstream.
     */

    TEMPLATE_SIO_WRITE8(pChan,
		TEMPLATE_CSR_ID, TEMPLATE_RESET_INT); /* ack interrupt*/

    if (status == ERROR)
	{
	/* send error notification upstream */

	(*pChan->errorRtn) (pChan->errorArg, SIO_ERROR_UNKNWN, NULL, 0);
	}
    else
	{
	/* send data character upstream */

	(*pChan->putRcvChar) (pChan->putRcvArg, inChar);
	}
    }

/******************************************************************************
*
* templateSioIntTx - handle a channels transmitter-ready interrupt
*
* RETURNS: N/A
*/ 

LOCAL void templateSioIntTx
    (
    TEMPLATE_CHAN *	pChan		/* channel generating the interrupt */
    )
    {
    char	outChar;
    char	crData = 0;
    BOOL	txReady = TRUE;
    int		errorCode = SIO_ERROR_NONE;

    /*
     * TODO - Check the Tx status
     *
     * For PCI devices, do an immediate return if device is not asserting
     * an interrupt.
     */

    TEMPLATE_PIPE_FLUSH(pChan);
    TEMPLATE_SIO_READ8(pChan, TEMPLATE_CSR_ID, &crData);

    /* TODO - Check for error conditions */

    if (crData & TEMPLATE_CR_TX_ERROR)
	{
	/*
	 * TODO - Typically you should determine the precise error condition
	 * and perform all recovery operations here.
	 */

	TEMPLATE_SIO_WRITE8 (pChan, TEMPLATE_CSR_ID, TEMPLATE_RESET_TX);

	errorCode = SIO_ERROR_UNKNWN;

	txReady = TRUE; /* set to false if xmitter is not ready now */
	}

    /*
     * If transmitter is okay and ready to send, lets see if there is a
     * data character ready to be sent.
     *
     * If there's a character to transmit then write it out, else reset (idle)
     * the transmitter. For chips with output FIFO's it is more efficient
     * to fill the entire FIFO here.
     */

    if (txReady)
	{
	if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)
	    {
	    /* TODO - Output data to device.  */

	    TEMPLATE_SIO_WRITE8(pChan, TEMPLATE_DATA_ID, outChar);

	    /*
	     * TODO - If a device error occurs at this point, then
	     * isolate the precise error type and try to recover.
	     */
	    }
	else
	    {
	    /*
	     * TODO - There is no more data to send.
	     *
	     * Put XMTR in an idle state. Higher layer
	     * will call TxStartup entry to resume xmit operations.
	     */

	    TEMPLATE_SIO_WRITE8(pChan, TEMPLATE_CSR_ID, TEMPLATE_RESET_TX);
	    }
	}

    /*
     * TODO - If needed, you should acknowledge or reset the interrupt source
     * as soon as possible, usually before passing any error conditions
     * upstream.
     */

    TEMPLATE_SIO_WRITE8(pChan, TEMPLATE_CSR_ID, TEMPLATE_RESET_INT);

    /* Pass any errorCodes upstream.  */

    if (errorCode != SIO_ERROR_NONE)
	{
	(*pChan->errorRtn) (pChan->errorArg, errorCode, NULL, 0);
	}
    }

/******************************************************************************
*
* templateSioIntErr - handle a channels error interrupt
*
* RETURNS: N/A
*/ 

LOCAL void templateSioIntErr
    (
    TEMPLATE_CHAN *	pChan		/* channel generating the interrupt */
    )
    {
    int errorCode = SIO_ERROR_NONE;
    char controlReg;

    TEMPLATE_PIPE_FLUSH(pChan);
    TEMPLATE_SIO_READ8 (pChan, TEMPLATE_CSR_ID, &controlReg);

    /*
     * TODO - Determine the precise error condition and perform recovery
     * operations.
     *
     * For PCI devices, do an immediate return if device is not asserting
     * an interrupt.
     */


    /*
     * TODO - If needed, you should acknowledge or reset the interrupt source
     * as soon as possible, usually before passing any error conditions
     * upstream.
     */

    TEMPLATE_SIO_WRITE8(pChan, TEMPLATE_CSR_ID, TEMPLATE_RESET_INT);

    if (errorCode != SIO_ERROR_NONE)
	(*pChan->errorRtn) (pChan->errorArg, errorCode, NULL, 0);
    }

/******************************************************************************
*
* templateTxStartup - start the interrupt transmitter
*
* This routine exits to inform the device to start transmitting data again.
* The actual data will be obtained by calling the TxCharGet callback routine.
*
* This routine is usually just the same as the tx interrupt routine.  This
* routine runs at task level context and does not have to be interrupt safe.
*
* RETURNS: OK on success, ENOSYS if the device is polled-only, or
* EIO on hardware error.
*/

LOCAL int templateTxStartup
    (
    SIO_CHAN * pSioChan                 /* channel to start */
    )
    {
    TEMPLATE_CHAN * pChan = (TEMPLATE_CHAN *)pSioChan;

    /* This routine should only be called while in interrupt mode */

    if (pChan->mode == SIO_MODE_INT)
        {
        if (pChan->options & CLOCAL)
            {
            /* TODO - No modem control - start immediately */
            }
        else
            {
	    /*
	     * TODO - Modem controls are active. 
	     *
	     * If CTS is high, we can start immediately so just enable
	     * the TX interrupt.
	     *
	     * If CTS is low, then we cannot send now.  The driver
	     * should be set up to generate a TX interrupt when the CTS
	     * line returns to the active state.
	     */
            }

        /*
	 * TODO - Enable the correct interrupts. A TX interrupt should either
	 * occur immediately, or later when CTS becomes active.  That will start
	 * the flow of data.
	 *
	 * There are two usual methods here.  The first is to just enable
	 * TX interrupts, which should cause an immediate TX interrupt, which
	 * will begin fetching and sending characters.
	 *
	 * The second method is to call the getTxChara callback here, put
	 * the data to the transmitter directly, and then to enable TX
	 * interrupts to fetch and send additional characters.
	 */

	TEMPLATE_SIO_WRITE8(pChan, TEMPLATE_CSR_ID, TEMPLATE_TX_ENABLE);
        }
    else
	return ENOSYS;  	/* Not valid for polled mode operation */

    return (OK);
    }



/******************************************************************************
*
* templateCallbackInstall - install ISR callbacks to get/put chars
*
* This driver allows interrupt callbacks for transmitting characters
* and receiving characters. In general, drivers may support other
* types of callbacks too.
*
* RETURNS: OK on success, or ENOSYS for an unsupported callback type.
*/ 

LOCAL int templateCallbackInstall
    (
    SIO_CHAN *	pSioChan,               /* channel */
    int		callbackType,           /* type of callback */
    STATUS	(*callback)(void *,...),  /* callback */
    void *      callbackArg             /* parameter to callback */
    )
    {
    TEMPLATE_CHAN * pChan = (TEMPLATE_CHAN *)pSioChan;

    switch (callbackType)
	{
	case SIO_CALLBACK_GET_TX_CHAR:
	    pChan->getTxChar	= (STATUS (*)(void *, char *))callback;
	    pChan->getTxArg	= callbackArg;
	    return (OK);

	case SIO_CALLBACK_PUT_RCV_CHAR:
	    pChan->putRcvChar	= (void (*)(void *, char))callback;
	    pChan->putRcvArg	= callbackArg;
	    return (OK);

	case SIO_CALLBACK_ERROR:
	    pChan->errorRtn	= (void (*)(void *, int, void *, int))callback;
	    pChan->errorArg	= callbackArg;
	    return (OK);

	default:
	    return (ENOSYS);
	}
    }

/*******************************************************************************
*
* templatePollOutput - output a character in polled mode
*
* Polled mode operation takes place without any kernel or other OS
* services available.  Use extreme care to insure that this code does not
* call any kernel services.  Polled mode is only for WDB system mode use.
* Kernel services, semaphores, tasks, etc, are not available during WDB
* system mode.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
* if the output buffer if full. ENOSYS if the device is
* interrupt-only.
*/

LOCAL int templatePollOutput
    (
    SIO_CHAN *	pSioChan,
    char	outChar
    )
    {
    TEMPLATE_CHAN * pChan = (TEMPLATE_CHAN *)pSioChan;
    UINT8	status;

    /* is the transmitter ready to accept a character? */

    TEMPLATE_PIPE_FLUSH(pChan);
    TEMPLATE_SIO_READ8(pChan, TEMPLATE_CSR_ID, &status);

    /* TODO - determine if transmitter is ready */

    if ((status & TEMPLATE_CR_TX_READY) == 0x00)
	{
	/* device is busy, try again later */

	return (EAGAIN);
	}

    /* TODO - Check for TX errors */

    if (status & TEMPLATE_CR_TX_ERROR)
	{
	/* TODO - decode specific error condition and handle it */

	/* Do not make error callback, just return EIO */

	return EIO;
	}
    else
	{
	/* TODO - transmit the character */

	TEMPLATE_SIO_WRITE8(pChan, TEMPLATE_DATA_ID, outChar);
	}

    return (OK);
    }

/******************************************************************************
*
* templatePollInput - poll the device for input
*
* Polled mode operation takes place without any kernel or other OS
* services available.  Use extreme care to insure that this code does not
* call any kernel services.  Polled mode is only for WDB system mode use.
* Kernel services, semaphores, tasks, etc, are not available during WDB
* system mode.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
* if the input buffer if empty, ENOSYS if the device is
* interrupt-only.
*/

LOCAL int templatePollInput
    (
    SIO_CHAN *	pSioChan,
    char *	thisChar
    )
    {
    TEMPLATE_CHAN * pChan = (TEMPLATE_CHAN *)pSioChan;
    UINT8	status;

    /* Read RX device status */

    TEMPLATE_PIPE_FLUSH(pChan);
    TEMPLATE_SIO_READ8(pChan, TEMPLATE_CSR_ID, &status);

    /* TODO - Check if receive data is available */

    if ((status & TEMPLATE_CR_RX_AVAIL) == 0x00)
	{
	return EAGAIN;	/* no input available at this time */
	}

    /* TODO - Check for receive error conditions */

    if (status & TEMPLATE_CR_RX_ERROR)
	{
	/* TODO - Decode and handle the specific error condition */

	/* Do NOT call the error callback routine, just return EIO */

	return EIO;
	}

    /* TODO - read character, store it, and return OK  */

    TEMPLATE_SIO_READ8(pChan, TEMPLATE_DATA_ID, thisChar);

    return (OK);
    }

/******************************************************************************
*
* templateModeSet - toggle between interrupt and polled mode
*
* RETURNS: OK on success, EIO on unsupported mode.
*/

LOCAL int templateModeSet
    (
    TEMPLATE_CHAN * pChan,		/* channel */
    uint_t	    newMode          	/* new mode */
    )
    {
    UINT8 temp;

    if ((newMode != SIO_MODE_POLL) && (newMode != SIO_MODE_INT))
	return (EIO);

    TEMPLATE_PIPE_FLUSH(pChan);

    if (pChan->mode == SIO_MODE_INT)
	{
	/* TODO - switch device to interrupt mode */

	if (pChan->intConnect == FALSE)
	    {
	    TEMPLATE_SIO_INT_CONNECT(pChan,TEMPLATE_RXINT_ID,
			    templateSioIntRcv, (void *)pChan);
	    TEMPLATE_SIO_INT_CONNECT(pChan, TEMPLATE_TXINT_ID,
			    templateSioIntTx, (void *)pChan);
	    TEMPLATE_SIO_INT_CONNECT(pChan, TEMPLATE_ERRINT_ID,
			    templateSioIntErr, (void *)pChan);
	    pChan->intConnect = TRUE;
	    }
	TEMPLATE_SIO_READ8(pChan, TEMPLATE_CSR_ID, &temp);
	temp |= TEMPLATE_INT_ENABLE;
	TEMPLATE_SIO_WRITE8(pChan, TEMPLATE_CSR_ID, temp);
	}
    else
	{
	/* TODO - switch device to polled mode */

	TEMPLATE_SIO_READ8(pChan, TEMPLATE_CSR_ID, &temp);
	temp &= ~TEMPLATE_INT_ENABLE;
	TEMPLATE_SIO_WRITE8(pChan, TEMPLATE_CSR_ID, temp);
	}

    /* activate  the new mode */

    pChan->mode = newMode;

    return (OK);
    }


/*******************************************************************************
*
* templateHup - hang up the modem control lines 
*
* Resets the RTS and DTR signals.
*
* RETURNS: OK always.
*/

LOCAL STATUS templateHup
    (
    TEMPLATE_CHAN * pChan 	/* pointer to channel */
    )
    {
    /*
     * TODO - Use global intLock if lockout time will be very short. If not,
     * use a device specific lockout that will not damage overall system
     * latency.
     */

    templateMstatSetClr (pChan,(SIO_MODEM_RTS|SIO_MODEM_DTR), FALSE);

    return (OK);
    }    

/*******************************************************************************
*
* templateOpen - Set the modem control lines 
*
* Set the modem control lines(RTS, DTR) TRUE if not already set.  
*
* RETURNS: OK
*/

LOCAL STATUS templateOpen
    (
    TEMPLATE_CHAN * pChan 	/* pointer to channel */
    )
    {
    /*
     * TODO - Use global intLock if lockout time will be very short. If not,
     * use a device specific lockout that will not damage overall system
     * latency.
     */

    templateMstatSetClr (pChan, (SIO_MODEM_RTS|SIO_MODEM_DTR), TRUE);

    return (OK);
    }

/******************************************************************************
*
* templateOptSet - set hardware options
*
* This routine sets up the hardware according to the specified option
* argument.  If the hardware cannot support a particular option value, then
* it should ignore that portion of the request.
*
* RETURNS: OK upon success, or EIO for invalid arguments.
*/

LOCAL int templateOptSet
    (
    TEMPLATE_CHAN * pChan,		/* channel */
    uint_t	    newOpts          	/* new options */
    )
    {
    int dataBits = 8;
    int stopBits = 1;
    BOOL hdweFlowCtrl=TRUE;
    BOOL rcvrEnable = TRUE;
    int  lvl;

    if (pChan == NULL || newOpts & 0xffffff00)
	return EIO;

    /* do nothing if options already set */

    if (pChan->options == newOpts)
	return OK;

    /* ignore requests for unsupported options */

    /* decode individual request elements */

    switch (newOpts & CSIZE)
	{
	case CS5:
	    dataBits = 5; break;
	case CS6:
	    dataBits = 6; break;
	case CS7:
	    dataBits = 7; break;
	default:
	case CS8:
	    dataBits = 8; break;
	}

    if (newOpts & STOPB)
	stopBits = 2;
    else
	stopBits = 1;

    switch (newOpts & (PARENB|PARODD))
	{
	case PARENB|PARODD:
	    /* enable odd parity */
	    break;

	case PARENB:
	    /* enable even parity */
	    break;

	case PARODD:
	    /* invalid mode, not normally used. */
	    break;

	default:
	case 0:
	    /* no parity */; break;
	}

    if (newOpts & CLOCAL)
	{

	/* clocal disables hardware flow control */

	hdweFlowCtrl = FALSE;
	}

    if ((newOpts & CREAD) == 0)
	rcvrEnable = FALSE;

    lvl = intLock ();

    /*
     * TODO - Reset the device according to dataBits, stopBits, hdweFlowCtrl,
     * rcvrEnable, and parity selections.
     */
    
    intUnlock (lvl);

    /*
     * TODO - Be sure that pChan->options reflects the actual
     * hardware settings.  If 5 data bits were requested, but unsupported,
     * then be sure pChan->options reflects the actual number of data bits
     * currently selected.
     */

    pChan->options = newOpts;

    return (OK);
    }

/*******************************************************************************
*
* templateIoctl - special device control
*
* This routine handles the IOCTL messages from the user. It supports commands 
* to get/set baud rate, mode(INT,POLL), hardware options(parity, number of 
* data bits) and modem control(RTS/CTS and DTR/DSR handshakes).
* The ioctl commands SIO_HUP and SIO_OPEN are used to implement the HUPCL(hang
* up on last close) function.
*
* As on a UNIX system, requesting a baud rate of zero is translated into
* a hangup request.  The DTR and RTS lines are dropped.  This should cause
* a connected modem to drop the connection.  The SIO_HUP command will only
* hangup if the HUPCL option is active.  The SIO_OPEN function will raise
* DTR and RTS lines whenever it is called. Use the BAUD_RATE=0 function
* to hangup when HUPCL is not active.
*
* The CLOCAL option will disable hardware flow control.  When selected,
* hardware flow control is not used.  When not selected hardware flow control
* is based on the RTS/CTS signals.  CTS is the clear to send input
* from the other end.  It must be true for this end to begin sending new
* characters.  In most drivers, the RTS signal will be assumed to be connected
* to the opposite end's CTS signal and can be used to control output from
* the other end.  Raising RTS asserts CTS at the other end and the other end
* can send data.  Lowering RTS de-asserts CTS and the other end will stop
* sending data. (This is non-EIA defined use of RTS).
*
* RETURNS: OK on success, ENOSYS on unsupported request, EIO on failed
* request.
*/

LOCAL int templateIoctl
    (
    SIO_CHAN *	pSioChan,		/* device to control */
    int		request,		/* request code */
    void *	someArg			/* some argument */
    )
    {
    TEMPLATE_CHAN *pChan = (TEMPLATE_CHAN *) pSioChan;
    int     oldLevel;		/* current interrupt level mask */
    int     baudConstant;
    int     arg = (int)someArg;

    TEMPLATE_PIPE_FLUSH(pChan);
    switch (request)
	{
	case SIO_BAUD_SET:

	    /*
	     * like unix, a baud request for 0 is really a request to
	     * hangup.
	     */

	    if (arg == 0)
		return (templateHup (pChan));

	    /*
	     * Set the baud rate. Return EIO for an invalid baud rate, or
	     * OK on success.
	     */

	    if (arg < TEMPLATE_BAUD_MIN || arg > TEMPLATE_BAUD_MAX)
	        {
		return (EIO);
	        }

	    /* TODO - Calculate the baud rate constant for the new baud rate */

	    baudConstant = pChan->clkFreq / arg / 16; /* DUMMY CODE */

	    /* disable interrupts during chip access */

	    oldLevel = intLock ();
	    TEMPLATE_SIO_WRITE8(pChan, TEMPLATE_BAUD_ID, baudConstant);
	    intUnlock (oldLevel);

	    pChan->baudFreq = arg;

	    return (OK);

	case SIO_BAUD_GET:

	    *(int *)someArg = pChan->baudFreq;
	    return (OK);

	case SIO_MODE_SET:

	    /*
	     * Set the mode (e.g., to interrupt or polled). Return OK
	     * or EIO for an unknown or unsupported mode.
	     */

	    return (templateModeSet (pChan, arg));

	case SIO_MODE_GET:

	    /* Get the current mode and return OK.  */

	    *(int *)someArg = pChan->mode;
	    return (OK);

	case SIO_AVAIL_MODES_GET:

	    /* TODO - set the available modes and return OK.  */

	    *(int *)someArg = SIO_MODE_INT | SIO_MODE_POLL;
	    return (OK);

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

	    return (templateOptSet (pChan, arg));

	case SIO_HW_OPTS_GET:

	    /*
	     * Optional command to get the hardware options (as defined
	     * in sioLib.h). Return OK or ENOSYS if this command is not
	     * implemented.  Note: if this command is unimplemented, it
	     * will be assumed that the driver options are CREAD | CS8
	     * (e.g., eight data bits, one stop bit, no parity, ints enabled).
	     */

	    *(int *)someArg = pChan->options;
	    return (OK);

        case SIO_HUP:

            /* check if hupcl option is enabled */

            if (pChan->options & HUPCL) 
                return (templateHup (pChan));
            return (OK);

        case SIO_OPEN:
	    return (templateOpen (pChan)); /* always open */

#if 1 /* TODO - optional modem control line support */

	/*
	 * These new ioctl commands are for monitoring and setting the
	 * state of the modem control lines under user control. The
	 * return values from these calls inform the user about which
	 * control lines are inputs and which are outputs. Basically
	 * this lets the user know if the device is wired for DTE or
	 * DCE configuration.  It also informs the user if any signals
	 * are missing altogether.
	 */

	case SIO_MSTAT_GET:
	    return templateMstatGet(pChan);

	case SIO_MCTRL_BITS_SET:
	    return templateMstatSetClr (pChan, arg, TRUE);

	case SIO_MCTRL_BITS_CLR:
	    return templateMstatSetClr (pChan, arg, FALSE);

	/*
	 * The ISIG and OSIG masks tell the user which signals are actually
	 * outputs and which aren't. In our case here, we assume the device
	 * is DTE mapped with DTR and RTS as outputs. DSR and CTS as inputs.
	 * This template driver doesn't support RI.
	 */
	case SIO_MCTRL_OSIG_MASK:
	    *(int *)someArg = TEMPLATE_OSIG_MASK;
	    break;

	case SIO_MCTRL_ISIG_MASK:
	    *(int *)someArg = TEMPLATE_ISIG_MASK;
	    break;

#endif /* optional Modem control line support */

#if 1 /* TODO - optional keyboard scan code support */

	/*
	 * The following new ioctl commands are meant only for keyboard
	 * input devices to use.  These allow the user to specify and
	 * examine the type of keyboard character mapping to use.  The
	 * possible types are NONE (raw scan codes), ASCII (default ascii
	 * mappings, and UNICODE for standard 16 bit unicode mappings.
	 *
	 * Our template driver supports only raw and ascii modes.
	 */

	case SIO_KYBD_MODE_SET:
	    switch (arg)
		{
		case SIO_KYBD_MODE_RAW:
		case SIO_KYBD_MODE_ASCII:
		    break;

		case SIO_KYBD_MODE_UNICODE:
		    return ENOSYS; /* template doesn't support unicode */
		}
	    pChan->scanMode = arg;
	    return OK;

	case SIO_KYBD_MODE_GET:
	    *(int *)someArg = pChan->scanMode;
	    return OK;

#endif /* optional keyboard scan code support */

	default:
	    return ENOSYS;
	}
    return OK;
    }

/*******************************************************************************
*
* dummyTxCallback - dummy Tx callback routine
*
* RETURNS: ERROR.
*/

LOCAL STATUS dummyTxCallback
    (
    void * callbackArg,	/* argument registered with callback */
    char * pChara 	/* ptr to data location */
    )
    {
    static BOOL doit = TRUE;

    /*
     * TODO - Until an upstream module connects to this device, there
     * is no data available to send.  We should only be concerned
     * if this callback is being called repeatedly and often. That
     * could indicate an interrupt servicing problem of some kind.
     */

    if (doit)
	{
	printf ("Dummy txCallback: 0x%x 0x%x\n",
		(int)callbackArg, (int)pChara);
	doit = FALSE;
	}

    return (ERROR);
    }

/*******************************************************************************
*
* dummyRxCallback - dummy Rx callback routine
*
* RETURNS: N/A.
* ARGSUSED
*/

LOCAL void dummyRxCallback
    (
    void * callbackArg,	/* argument registered with callback */
    char data 		/* receive data */
    )
    {
    static BOOL doit = TRUE;

    /*
     * TODO - Device is transferring data, but there is no
     * upstream module connected to receive it.  Lets log
     * a message about incoming data being lost.  Buts lets
     * do this only once we don't want a 'flood' of logged
     * messages.
     */

    if (doit)
	{
	printf ("Dummy rxCallback: 0x%x 0x%x\n",
		(int)callbackArg, (int)data);
	doit = FALSE;
	}

    return;
    }

/*******************************************************************************
*
* dummyErrCallback - dummy Error callback routine
*
* There should be an sioLib module to provide dummyCallbacks for all SIO
* drivers to use.
*
* RETURNS: N/A.
* ARGSUSED
*/

LOCAL void dummyErrCallback
    (
    void * callbackArg,	/* argument registered with callback */
    int errorCode, 	/* error code */
    void * pData,	/* ptr to device specific data */
    int size		/* size of device specific data */
    )
    {
    static BOOL doit = TRUE;

    /* TODO -  We could log the reported error (once). */

    if (doit)
	{
	printf ("Dummy errorCallback: 0x%x 0x%x 0x%x %d\n",
		(int)callbackArg, errorCode, (int)pData, size);
	doit = FALSE;
	}

    return;
    }

/*******************************************************************************
*
* templateProbe - probe for device 
*
* Try to determine if device is present.  Do not reconfigure device if it
* is there.  This should be a passive probe that does not interfere with
* the device.
*
* RETURNS:
* Returns OK if device adaptor is there, ERROR if not there.
*/

LOCAL STATUS templateProbe
    (
    TEMPLATE_CHAN * pChan /* channel to probe */
    )
    {
    char testData;

    /*
     * TODO - Probe device registers to see if they are there and if they
     * are the proper type of device.  Look for specific bits that are
     * always 0 or 1.  Don't attempt any operations with side effects
     * that might clear interrupt status or flags.  For High Availability
     * situations, some other CPU may be controlling this device. We do
     * not want to interfere with those operations.
     */

    TEMPLATE_PROBE (pChan, TEMPLATE_CSR_ID, VX_READ, 1, &testData);

    /* TODO - Is this really the right type device ? */

    return OK;
    }


/*******************************************************************************
*
* templateVerify - verify template chan structure
*
* Given a pointer to what should be a TEMPLATE_CHAN, verify that it really
* is a TEMPLATE_CHAN structure.
*
* This routine should not be called at every level with every routine.  It is
* primarily provided for use with the xxxDestroy routine.  Performance will be
* a problem if every pointer is checked for validity with every use.
*
* RETURNS:
* Returns OK if pointer is valid, ERROR if not.
*/

LOCAL STATUS templateVerify
    (
    TEMPLATE_CHAN * pChan /* pointer to be verified */
    )
    {
    /*
     * TODO - Examine the structure. Look for magic cookies or flag bits.
     * Anything that would confirm this pointer as being a valid
     * TEMPLATE_CHAN pointer.
     */

    if (pChan == NULL)
	return ERROR;

    return OK;
    }


#if 1 /* TODO - Optional modem control line support */

/*******************************************************************************
*
* templateMstatGet - read device modem control line status
*
* Read the device modem control lines and map them to the standard
* modem signal bits.
*
* RETURNS:
* Returns the modem control line status bits.
*/

LOCAL int templateMstatGet
    (
    TEMPLATE_CHAN *pChan
    )
    {
    UINT8 rawStatus;
    int result = 0;

    TEMPLATE_SIO_READ8 (pChan, TEMPLATE_MSR_ID, &rawStatus);

    /* Now map device status bits, to standard status bits */

    if (rawStatus & TEMPLATE_MSR_CD)
	result |= SIO_MODEM_CD;

    if (rawStatus & TEMPLATE_MSR_DTR)
	result |= SIO_MODEM_DTR;

    if (rawStatus & TEMPLATE_MSR_DSR)
	result |= SIO_MODEM_DSR;

    if (rawStatus & TEMPLATE_MSR_RTS)
	result |= SIO_MODEM_RTS;

    if (rawStatus & TEMPLATE_MSR_CTS)
	result |= SIO_MODEM_CTS;

    return result;
    }

/*******************************************************************************
*
* templateMstatSetClear - set/clear modem control lines
*
* This routine allows the user to set or clear individual modem control
* lines.  Of course, only the output lines can actually be changed.
*
* RETURNS:
* OK, or EIO upon detecting a hardware fault.
*/

LOCAL int templateMstatSetClr
    (
    TEMPLATE_CHAN *pChan,
    UINT bits,		/* bits to change */
    BOOL setFlag	/* TRUE = set, FALSE = clear */
    )
    {
    UINT8 rawStatus;
    UINT8 rawMask = 0;

    /* Read current modem status */
    TEMPLATE_SIO_READ8 (pChan, TEMPLATE_MSR_ID, &rawStatus);

    /* ignore input only bits */
    bits &= TEMPLATE_OSIG_MASK;

    /* Now map standard bits to device specific bits */

    if (bits & SIO_MODEM_DTR)
	rawMask |= TEMPLATE_MSR_DTR;

    if (bits & SIO_MODEM_RTS)
	rawMask |= TEMPLATE_MSR_RTS;

    /* Update device with new output signals */

    if (setFlag)
	rawStatus |= rawMask; /* set new bits */
    else
	rawStatus &= ~rawMask; /* clear bits */

    TEMPLATE_SIO_WRITE8 (pChan, TEMPLATE_MSR_ID, rawStatus);

    return OK;
    }
#endif /* optional modem control line support */
