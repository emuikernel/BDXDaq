/* ambaSio.c - ARM AMBA UART tty driver */

/* Copyright 1997 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01d,04dec97,jpd  updated doc'n to latest standards. Fix max/min baud setting.
01c,13oct97,jpd  modified comments/documentation.
01b,26sep97,jpd  modified in line with errata, removed debugging code.
01a,18sep97,jpd  created from st1550Sio.c version 01d.
*/

/*
DESCRIPTION
This is the device driver for the Advanced RISC Machines (ARM) AMBA
UART. This is a generic design of UART used within a number of chips
containing (or for use with) ARM CPUs such as in the Digital Semiconductor
21285 chip as used in the EBSA-285 BSP.

This design contains a universal asynchronous receiver/transmitter, a
baud-rate generator, and an InfraRed Data Association (IrDa) Serial
InfraRed (SiR) protocol encoder. The Sir encoder is not supported by
this driver. The UART contains two 16-entry deep FIFOs for receive and
transmit: if a framing, overrun or parity error occurs during
reception, the appropriate error bits are stored in the receive FIFO
along with the received data. The FIFOs can be programmed to be one
byte deep only, like a conventional UART with double buffering, but the
only mode of operation supported is with the FIFOs enabled.

The UART design does not support the modem control output signals: DTR,
RI and RTS. Moreover, the implementation in the 21285 chip does not
support the modem control inputs: DCD, CTS and DSR.

The UART design can generate four interrupts: Rx, Tx, modem status
change and a UART disabled interrupt (which is asserted when a start
bit is detected on the receive line when the UART is disabled). The
implementation in the 21285 chip has only two interrupts: Rx and Tx,
but the Rx interrupt is a combination of the normal Rx interrupt status
and the UART disabled interrupt status.

Only asynchronous serial operation is supported by the UART which
supports 5 to 8 bit bit word lengths with or without parity and with
one or two stop bits. The only serial word format supported by the
driver is 8 data bits, 1 stop bit, no parity,  The default baud rate is
determined by the BSP by filling in the AMBA_CHAN structure before
calling ambaDevInit().

The exact baud rates supported by this driver will depend on the
crystal fitted (and consequently the input clock to the baud-rate
generator), but in general, baud rates from about 300 to about 115200
are possible.

In theory, any number of UART channels could be implemented within a
chip. This driver has been designed to cope with an arbitrary number of
channels, but at the time of writing, has only ever been tested with
one channel.

.SH DATA STRUCTURES
An AMBA_CHAN data structure is used to describe each channel, this
structure is described in h/drv/sio/ambaSio.h.

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
routines in this modules are ambaDevInit(), ambaIntTx() and
ambaIntRx().

The BSP's sysHwInit() routine typically calls sysSerialHwInit(), which
initialises the hardware-specific fields in the AMBA_CHAN structure
(e.g. register I/O addresses etc) before calling ambaDevInit() which
resets the device and installs the driver function pointers.  After
this the UART will be enabled and ready to generate interrupts, but
those interrupts will be disabled in the interrupt controller.

The following example shows the first parts of the initialisation:

.CS
#include "drv/sio/ambaSio.h"

LOCAL AMBA_CHAN ambaChan[N_AMBA_UART_CHANS];

void sysSerialHwInit (void)
    {
    int i;

    for (i = 0; i < N_AMBA_UART_CHANS; i++)
	{
	ambaChan[i].regs = devParas[i].baseAdrs;
	ambaChan[i].baudRate = CONSOLE_BAUD_RATE;
	ambaChan[i].xtal = UART_XTAL_FREQ; 

	ambaChan[i].levelRx = devParas[i].intLevelRx;
	ambaChan[i].levelTx = devParas[i].intLevelTx;

	/@
	 * Initialise driver functions, getTxChar, putRcvChar and
	 * channelMode, then initialise UART
	 @/

	ambaDevInit(&ambaChan[i]);
	}
    }
.CE

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(),
which connects the chips interrupts via intConnect() (the two
interrupts `ambaIntTx' and `ambaIntRx') and enables those interrupts,
as shown in the following example:

.CS

void sysSerialHwInit2 (void)
    {
    /@ connect and enable Rx interrupt @/

    (void) intConnect (INUM_TO_IVEC(devParas[0].vectorRx),
		       ambaIntRx, (int) &ambaChan[0]);
    intEnable (devParas[0].intLevelRx);


    /@ connect Tx interrupt @/

    (void) intConnect (INUM_TO_IVEC(devParas[0].vectorTx),
		       ambaIntTx, (int) &ambaChan[0]);
    /@
     * There is no point in enabling the Tx interrupt, as it will
     * interrupt immediately and then be disabled.
     @/

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
SIO_CHAN *, which is just the address of the AMBA_CHAN structure.
 
sysSerialReset is called from sysToMonitor() and should reset the
serial devices to an inactive state (prevent them from generating any
interrupts).

.SH INCLUDE FILES:
drv/sio/ambaSio.h sioLib.h

.SH SEE ALSO:
.I "Advanced RISC Machines AMBA UART (AP13) Data Sheet,"
.I "Digital Semiconductor 21285 Core Logic for SA-110 Microprocessor Data
Sheet,"
.I "Digital Semiconductor EBSA-285 Evaluation Board Reference Manual."
*/

#include "vxWorks.h"
#include "intLib.h"
#include "errnoLib.h"
#include "errno.h"
#include "sioLib.h"
#include "drv/sio/ambaSio.h"

/* local defines  */

#ifndef AMBA_UART_REG_READ
#define AMBA_UART_REG_READ(pChan, reg, result) \
	result = (*(volatile UINT32 *)((UINT32)pChan->regs + reg))
#endif

#ifndef AMBA_UART_REG_WRITE
#define AMBA_UART_REG_WRITE(pChan, reg, data) \
	(*(volatile UINT32 *)((UINT32)pChan->regs + reg)) = (data)
#endif


/* locals */

/* function prototypes */

LOCAL STATUS ambaDummyCallback (void);
LOCAL void ambaInitChannel (AMBA_CHAN * pChan);
LOCAL STATUS ambaIoctl (SIO_CHAN * pSioChan, int request, int arg);
LOCAL int ambaTxStartup (SIO_CHAN * pSioChan);
LOCAL int ambaCallbackInstall (SIO_CHAN * pSioChan, int callbackType,
			       STATUS (*callback)(), void * callbackArg);
LOCAL int ambaPollInput (SIO_CHAN * pSioChan, char *);
LOCAL int ambaPollOutput (SIO_CHAN * pSioChan, char);

/* driver functions */

LOCAL SIO_DRV_FUNCS ambaSioDrvFuncs =
    {
    (int (*)())ambaIoctl,
    ambaTxStartup,
    ambaCallbackInstall,
    ambaPollInput,
    ambaPollOutput
    };

/*******************************************************************************
*
* ambaDummyCallback - dummy callback routine.
*
* RETURNS: ERROR, always.
*/

LOCAL STATUS ambaDummyCallback (void)
    {
    return ERROR;
    }

/*******************************************************************************
*
* ambaDevInit - initialise an AMBA channel
*
* This routine initialises some SIO_CHAN function pointers and then resets
* the chip to a quiescent state.  Before this routine is called, the BSP
* must already have initialised all the device addresses, etc. in the
* AMBA_CHAN structure.
*
* RETURNS: N/A
*/

void ambaDevInit
    (
    AMBA_CHAN *	pChan	/* ptr to AMBA_CHAN describing this channel */
    )
    {
    int oldlevel = intLock();


    /* initialise the driver function pointers in the SIO_CHAN */

    pChan->sio.pDrvFuncs = &ambaSioDrvFuncs;


    /* set the non BSP-specific constants */

    pChan->getTxChar = ambaDummyCallback;
    pChan->putRcvChar = ambaDummyCallback;

    pChan->channelMode = 0;    /* undefined */


    /* initialise the chip */

    ambaInitChannel (pChan);


    intUnlock (oldlevel);

    }

/*******************************************************************************
*
* ambaInitChannel - initialise UART
*
* This routine performs hardware initialisation of the UART channel.
*
* RETURNS: N/A
*/

LOCAL void ambaInitChannel
    (
    AMBA_CHAN *	pChan	/* ptr to AMBA_CHAN describing this channel */
    )
    {
    int		i;
    UINT32	discard;

    /* Fill Tx FIFO with nulls (data sheet specifies this) */

    for (i = 0; i < 16; i++)
	AMBA_UART_REG_WRITE(pChan, UARTDR, 0);


    /* Program UART control register */

    AMBA_UART_REG_WRITE(pChan, UARTCON, 0);
    AMBA_UART_REG_WRITE(pChan, UARTCON, UART_ENABLE);


    /* Set baud rate divisor */

    AMBA_UART_REG_WRITE(pChan, L_UBRLCR,
		    (pChan->xtal / (16 * pChan->baudRate) - 1) & 0xFF);
    AMBA_UART_REG_WRITE(pChan, M_UBRLCR,
		    ((pChan->xtal / (16 * pChan->baudRate) - 1) >> 8) & 0xF);

    /*
     * Set word format, enable FIFOs: set 8 bits, 1 stop bit, no parity.
     * This also latches the writes to the two (sub)registers above.
     */

    AMBA_UART_REG_WRITE(pChan, H_UBRLCR,
	(UINT8)(WORD_LEN_8 | ONE_STOP | PARITY_NONE | FIFO_ENABLE));
 

    /* Clear Rx FIFO (data sheet specifies this) */

    for (i = 0; i < 16; i++)
	AMBA_UART_REG_READ(pChan, RXSTAT, discard);

    }

/*******************************************************************************
*
* ambaIoctl - special device control
*
* This routine handles the IOCTL messages from the user.
*
* RETURNS: OK on success, ENOSYS on unsupported request, EIO on failed
* request.
*/

LOCAL STATUS ambaIoctl
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    int		request,	/* request code */
    int		arg		/* some argument */
    )
    {
    int		oldlevel;	/* current interrupt level mask */
    STATUS	status;		/* status to return */
    UINT32	brd;		/* baud rate divisor */
    AMBA_CHAN * pChan = (AMBA_CHAN *)pSioChan;

    status = OK;	/* preset to return OK */

    switch (request)
	{
	case SIO_BAUD_SET:
	    /*
	     * Set the baud rate. Return EIO for an invalid baud rate, or
	     * OK on success.
	     */
								
	    /*
	     * baudrate divisor must be non-zero and must fit in a 12-bit
	     * register.
	     */

	    brd = (pChan->xtal/(16*arg)) - 1;	/* calculate baudrate divisor */

	    if ((brd < 1) || (brd > 0xFFF))
		{
		status = EIO;		/* baud rate out of range */
		break;
		}

	    /* disable interrupts during chip access */

	    oldlevel = intLock ();


	    /* Set baud rate divisor in UART */

	    AMBA_UART_REG_WRITE(pChan, L_UBRLCR, brd & 0xFF);
	    AMBA_UART_REG_WRITE(pChan, M_UBRLCR, (brd >> 8) & 0xF);

	    /*
	     * Set word format, enable FIFOs: set 8 bits, 1 stop bit, no parity.
	     * This also latches the writes to the two (sub)registers above.
	     */

	    AMBA_UART_REG_WRITE(pChan, H_UBRLCR,
		(UINT8)(WORD_LEN_8 | ONE_STOP | PARITY_NONE | FIFO_ENABLE));

	    pChan->baudRate = arg;
 
	    intUnlock (oldlevel);

	    break;


	case SIO_BAUD_GET:

	    /* Get the baud rate and return OK */

	    *(int *)arg = pChan->baudRate;
	    break; 


	case SIO_MODE_SET:

	    /*
	     * Set the mode (e.g., to interrupt or polled). Return OK
	     * or EIO for an unknown or unsupported mode.
	     */

	    if ((arg != SIO_MODE_POLL) && (arg != SIO_MODE_INT))
		{
		status = EIO;
		break;
		}
	   
	    oldlevel = intLock ();

	    if (arg == SIO_MODE_INT)
		{
		/* Enable appropriate interrupts */

		intEnable (pChan->levelRx);

		/*
		 * There is no point in enabling the Tx interrupt, as it
		 * will interrupt immediately and be disabled.
		 */  
		}
	    else
		{
		/* Disable the interrupts */ 

		intDisable (pChan->levelRx);
		intDisable (pChan->levelTx);
		}

	    pChan->channelMode = arg;

	    intUnlock (oldlevel);
	    break;	    


	case SIO_MODE_GET:

	    /* Get the current mode and return OK */

	    *(int *)arg = pChan->channelMode;
	    break;


	case SIO_AVAIL_MODES_GET:

	    /* Get the available modes and return OK */

	    *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    break;


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

	case SIO_HW_OPTS_GET:

	    /*
	     * Optional command to get the hardware options (as defined
	     * in sioLib.h). Return OK or ENOSYS if this command is not
	     * implemented.  Note: if this command is unimplemented, it
	     * will be assumed that the driver options are CREAD | CS8
	     * (e.g., eight data bits, one stop bit, no parity, ints enabled).
	     */

	default:
	    status = ENOSYS;
	}

    return status;

    }

/*******************************************************************************
*
* ambaIntTx - handle a transmitter interrupt 
*
* This routine handles write interrupts from the UART.
*
* RETURNS: N/A
*/

void ambaIntTx 
    (
    AMBA_CHAN *	pChan	/* ptr to AMBA_CHAN describing this channel */
    )
    {
    char outChar;

    if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)

	/* write char. to Transmit Holding Reg. */

	AMBA_UART_REG_WRITE(pChan, UARTDR, outChar);
    else
	intDisable (pChan->levelTx);
    }

/*****************************************************************************
*
* ambaIntRx - handle a receiver interrupt 
*
* This routine handles read interrupts from the UART.
*
* RETURNS: N/A
*/

void ambaIntRx
    (
    AMBA_CHAN *	pChan	/* ptr to AMBA_CHAN describing this channel */
    )
    {
    char inchar;

    /* read character from Receive Holding Reg. */

    AMBA_UART_REG_READ(pChan, UARTDR, inchar);

    (*pChan->putRcvChar) (pChan->putRcvArg, inchar);
    }

/*******************************************************************************
*
* ambaTxStartup - transmitter startup routine
*
* Enable interrupt so that interrupt-level char output routine will be called.
*
* RETURNS: OK on success, ENOSYS if the device is polled-only, or
* EIO on hardware error.
*/

LOCAL int ambaTxStartup
    (
    SIO_CHAN *	pSioChan	/* ptr to SIO_CHAN describing this channel */
    )
    {
    AMBA_CHAN * pChan = (AMBA_CHAN *)pSioChan;

    if (pChan->channelMode == SIO_MODE_INT)
	{
	intEnable (pChan->levelTx);
	return OK;
	}
    else
	return ENOSYS;
    }

/******************************************************************************
*
* ambaPollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
* if the output buffer is full, ENOSYS if the device is interrupt-only.
*/

LOCAL int ambaPollOutput
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    char	outChar 	/* char to output */
    )
    {
    AMBA_CHAN * pChan = (AMBA_CHAN *)pSioChan;
    FAST UINT32 pollStatus;

    AMBA_UART_REG_READ(pChan, UARTFLG, pollStatus);

    /* is the transmitter ready to accept a character? */

    if ((pollStatus & FLG_UTXFF) != 0x00)
	return EAGAIN;


    /* write out the character */

    AMBA_UART_REG_WRITE(pChan, UARTDR, outChar);	/* transmit character */

    return OK;
    }

/******************************************************************************
*
* ambaPollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
* if the input buffer is empty, ENOSYS if the device is interrupt-only.
*/

LOCAL int ambaPollInput
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    char *	thisChar	/* pointer to where to return character */
    )
    {
    AMBA_CHAN * pChan = (AMBA_CHAN *)pSioChan;
    FAST UINT32 pollStatus;

    AMBA_UART_REG_READ(pChan, UARTFLG, pollStatus);

    if ((pollStatus & FLG_URXFE) != 0x00)
	return EAGAIN;


    /* got a character */

    AMBA_UART_REG_READ(pChan, UARTDR, *thisChar);

    return OK;

    }

/******************************************************************************
*
* ambaCallbackInstall - install ISR callbacks to get/put chars.
*
* This routine installs interrupt callbacks for transmitting characters
* and receiving characters.
*
* RETURNS: OK on success, or ENOSYS for an unsupported callback type.
*
*/

LOCAL int ambaCallbackInstall
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    int		callbackType,	/* type of callback */
    STATUS	(*callback)(),	/* callback */
    void *	callbackArg	/* parameter to callback */
		 
    )
    {
    AMBA_CHAN * pChan = (AMBA_CHAN *)pSioChan;

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
