/* sa1100Sio.c - Digital Semiconductor SA-1100 UART tty driver */

/* Copyright 1998 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01a,22jan98,jpd  created from ambaSio.c version 01d.
*/

/*
DESCRIPTION
This is the device driver for the Digital Semiconductor SA-1100 UARTs.
This chip contains 5 serial ports, but only ports 1 and 3 are usable as
UARTs, the others support Universal Serial Bus (USB), SDLC, IrDA
Infrared Communications Port (ICP) and Multimedia Communications Port
(MCP)/Synchronous Serial Port (SSP).

The UARTs are identical in design.  They contain a universal
asynchronous receiver/transmitter, and a baud-rate generator, The UARTs
contain an 8-entry, 8-bit FIFO to buffer outgoing data and a 12-entry
11-bit FIFO to buffer incoming data.  If a framing, overrun or parity
error occurs during reception, the appropriate error bits are stored in
the receive FIFO along with the received data.  The only mode of
operation supported is with the FIFOs enabled.

The UART design does not support modem control input or output signals
e.g. DTR, RI, RTS, DCD, CTS and DSR.

An interrupt is generated when a framing, parity or receiver overrun
error is present within the bottom four entries of the receive FIFO,
when the transmit FIFO is half-empty or receive FIFO is one- to
two-thirds full, when a begin and end of break is detected on the
receiver, and when the receive FIFO is partially full and the receiver
is idle for three or more frame periods.

Only asynchronous serial operation is supported by the UARTs which
supports 7 or 8 bit word lengths with or without parity and with one or
two stop bits. The only serial word format supported by the driver is 8
data bits, 1 stop bit, no parity,  The default baud rate is determined
by the BSP by filling in the SA1100_CHAN structure before calling
sa1100DevInit().

The UART supports baud rates from 56.24 to 230.4 kbps.

.SH DATA STRUCTURES
An SA1100_CHAN data structure is used to describe each channel, this
structure is described in h/drv/sio/sa1100Sio.h.

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
routines in this modules are sa1100DevInit(), and sa1100Int().

The BSP's sysHwInit() routine typically calls sysSerialHwInit(), which
initialises the hardware-specific fields in the SA1100_CHAN structure
(e.g. register I/O addresses etc) before calling sa1100DevInit() which
resets the device and installs the driver function pointers.  After
this the UART will be enabled and ready to generate interrupts, but
those interrupts will be disabled in the interrupt controller.

The following example shows the first parts of the initialisation:

.CS
#include "drv/sio/sa1100Sio.h"

LOCAL SA1100_CHAN sa1100Chan[N_SA1100_UART_CHANS];

void sysSerialHwInit (void)
    {
    int i;

    for (i = 0; i < N_SA1100_UART_CHANNELS; i++)
	{
	sa1100Chan[i].regs = devParas[i].baseAdrs;
	sa1100Chan[i].baudRate = CONSOLE_BAUD_RATE;
	sa1100Chan[i].xtal = UART_XTAL_FREQ; 

	sa1100Chan[i].level = devParas[i].intLevel;

	/@ set up GPIO pins and UART pin reassignment @/

	...


	/@
	 * Initialise driver functions, getTxChar, putRcvChar
	 * and channelMode and initialise UART
	 @/

	sa1100DevInit(&sa1100Chan[i]);
	}
    }
.CE

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(),
which connects the chips interrupts via intConnect() and enables those
interrupts, as shown in the following example:

.CS

void sysSerialHwInit2 (void)
    {
    int i;

    for (i = 0; i < N_SA1100_UART_CHANNELS; i++)
	{
	/@ connect and enable interrupts @/

	(void)intConnect (INUM_TO_IVEC(devParas[i].vector),
			sa1100Int, (int) &sa1100Chan[i]);
	intEnable (devParas[i].intLevel);
	}
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
SIO_CHAN *, which is just the address of the SA1100_CHAN structure.
 
sysSerialReset is called from sysToMonitor() and should reset the
serial devices to an inactive state (prevent them from generating any
interrupts).

.SH INCLUDE FILES:
drv/sio/sa1100Sio.h sioLib.h

.SH SEE ALSO:
.I Digital StrongARM SA-1100 Portable Communications Microcontroller, Data
Sheet,
.I Digital Semiconductor StrongARM SA-1100 Microprocessor Evaluation Platform,
User's Guide
*/

#include "vxWorks.h"
#include "intLib.h"
#include "errnoLib.h"
#include "errno.h"
#include "sioLib.h"
#include "drv/sio/sa1100Sio.h"

/* local defines  */

#ifndef SA1100_UART_REG_READ
#define SA1100_UART_REG_READ(pChan, reg, result) \
	((result) = (*(volatile UINT32 *)((UINT32)(pChan)->regs + (reg))))
#endif

#ifndef SA1100_UART_REG_WRITE
#define SA1100_UART_REG_WRITE(pChan, reg, data) \
	((*(volatile UINT32 *)((UINT32)(pChan)->regs + (reg))) = (data))
#endif


/* locals */

/* function prototypes */

LOCAL STATUS sa1100DummyCallback (void);
LOCAL void sa1100InitChannel (SA1100_CHAN * pChan);
LOCAL STATUS sa1100Ioctl (SIO_CHAN * pSioChan, int request, int arg);
LOCAL int sa1100TxStartup (SIO_CHAN * pSioChan);
LOCAL int sa1100CallbackInstall (SIO_CHAN * pSioChan, int callbackType,
			       STATUS (*callback)(), void * callbackArg);
LOCAL int sa1100PollInput (SIO_CHAN * pSioChan, char *);
LOCAL int sa1100PollOutput (SIO_CHAN * pSioChan, char);

/* driver functions */

LOCAL SIO_DRV_FUNCS sa1100SioDrvFuncs =
    {
    (int (*)())sa1100Ioctl,
    sa1100TxStartup,
    sa1100CallbackInstall,
    sa1100PollInput,
    sa1100PollOutput
    };

/*******************************************************************************
*
* sa1100DummyCallback - dummy callback routine.
*
* RETURNS: ERROR, always.
*/

LOCAL STATUS sa1100DummyCallback (void)
    {
    return ERROR;
    }

/*******************************************************************************
*
* sa1100DevInit - initialise an SA1100 channel
*
* This routine initialises some SIO_CHAN function pointers and then resets
* the chip to a quiescent state.  Before this routine is called, the BSP
* must already have initialised all the device addresses, etc. in the
* SA1100_CHAN structure.
*
* RETURNS: N/A
*/

void sa1100DevInit
    (
    SA1100_CHAN *	pChan	/* ptr to SA1100_CHAN describing this channel */
    )
    {
    int oldlevel = intLock();

    /* initialise the driver function pointers in the SIO_CHAN */

    pChan->sio.pDrvFuncs = &sa1100SioDrvFuncs;


    /* set the non BSP-specific constants */

    pChan->getTxChar = sa1100DummyCallback;
    pChan->putRcvChar = sa1100DummyCallback;

    pChan->channelMode = 0;    /* am undefined mode */


    /* initialise the chip */

    sa1100InitChannel (pChan);


    intUnlock (oldlevel);

    return;
    }

/*******************************************************************************
*
* sa1100InitChannel - initialise UART
*
* This routine performs hardware initialisation of the UART channel.
*
* RETURNS: N/A
*/

LOCAL void sa1100InitChannel
    (
    SA1100_CHAN *	pChan	/* ptr to SA1100_CHAN describing this channel */
    )
    {
    UINT32	brd;

    /* Disable UART so can set line speed */

    SA1100_UART_REG_WRITE (pChan, UTCR3, 0);


    /* Clear Status Register sticky bits */

    SA1100_UART_REG_WRITE (pChan, UTSR0, 0xFF);


    /* Set word format: set 8 bits, 1 stop bit, no parity. */

    SA1100_UART_REG_WRITE (pChan, UTCR0, (WORD_LEN_8 | ONE_STOP | PARITY_NONE));


    /* Set baud rate divisor */

    brd = (pChan->xtal / (16 * pChan->baudRate)) - 1;

    SA1100_UART_REG_WRITE (pChan, UTCR1, (brd >> 8) & 0xF);
    SA1100_UART_REG_WRITE (pChan, UTCR2, brd & 0xFF);


    /* Enable UART and rx and tx interrupts */

    SA1100_UART_REG_WRITE (pChan, UTCR3,
			   UTCR3_RXE | UTCR3_TXE | UTCR3_RIM | UTCR3_TIM);
    }

/*******************************************************************************
*
* sa1100Ioctl - special device control
*
* This routine handles the IOCTL messages from the user.
*
* RETURNS: OK on success, ENOSYS on unsupported request, EIO on failed
* request.
*/

LOCAL STATUS sa1100Ioctl
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    int		request,	/* request code */
    int		arg		/* some argument */
    )
    {
    int		oldlevel;	/* current interrupt level mask */
    STATUS	status;		/* status to return */
    UINT32	brd;		/* baud rate divisor */
    UINT32 	cr3;
    SA1100_CHAN * pChan = (SA1100_CHAN *)pSioChan;

    status = OK;	/* preset to return OK */

    switch (request)
	{
	case SIO_BAUD_SET:
	    /*
	     * Set the baud rate. Return EIO for an invalid baud rate, or
	     * OK on success.
	     *
	     * Calculate the baud rate divisor: it must be non-zero and must
	     * fit in a 12-bit register.
	     */

	    brd = (pChan->xtal / (16 * arg)) - 1;

	    if (brd & ~0xFFF)
		{
		status = EIO;		/* baud rate out of range */
		break;
		}

	    /* disable interrupts during chip access */

	    oldlevel = intLock ();


	    SA1100_UART_REG_READ (pChan, UTCR3, cr3);	/* save current cr3 */
	    SA1100_UART_REG_WRITE (pChan, UTCR3, 0);	/* disable UART */

	    /* Set baud rate divisor in UART */

	    SA1100_UART_REG_WRITE (pChan, UTCR1, (brd >> 8) & 0xF);
	    SA1100_UART_REG_WRITE (pChan, UTCR2, brd & 0xFF);

	    /* reenable UART and any enabled interrupts */

	    SA1100_UART_REG_WRITE (pChan, UTCR3, cr3);
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
		/* Enable appropriate interrupt */

		intEnable (pChan->level);
		}
	    else
		{
		/* Disable the interrupt */ 

		intDisable (pChan->level);
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
* sa1100Int - handle an interrupt
*
* This routine handles interrupts from the UART.
*
* RETURNS: N/A
*/
 
void sa1100Int
    (
    SA1100_CHAN *       pChan   /* ptr to SA1100_CHAN describing this channel */
    )
    {
    UINT32	ch;		/* Possible char to be in/output */
    UINT32	statusReg0;	/* Status from UART */
    UINT32	statusReg1;	/* Status from UART */
    int		gotOne;


    /* read status registers */

    SA1100_UART_REG_READ (pChan, UTSR0, statusReg0);
    SA1100_UART_REG_READ (pChan, UTSR1, statusReg1);


    /* clear sticky interrupts */

    SA1100_UART_REG_WRITE (pChan, UTSR0, UTSR0_RID | UTSR0_RBB | UTSR0_REB);


    /*
     * loop, removing characters from the rx FIFO (other Sio drivers do this
     * so it must be OK)
     */

    while (statusReg1 & UTSR1_RNE)			   /* data? */
	{
	SA1100_UART_REG_READ (pChan, UTDR, ch);		   /* read character */
	(*pChan->putRcvChar) (pChan->putRcvArg, (char)ch); /* pass it on */
	SA1100_UART_REG_READ (pChan, UTSR1, statusReg1);   /* update status */
	}


    /*
     * if tx FIFO is interrupting, loop, writing characters to the FIFO
     * until the FIFO is full or there are no more characters to transmit
     */

    if (statusReg0 & UTSR0_TFS)		/* tx FIFO needs filling? */
	{
	do
	    {
	    gotOne = (*pChan->getTxChar) (pChan->getTxArg, &ch) != ERROR;
	    if (gotOne)
		SA1100_UART_REG_WRITE(pChan, UTDR, ch & 0xFF);  /* tx char */

	    SA1100_UART_REG_READ (pChan, UTSR1, statusReg1);	/* get status */
	    }
	while (gotOne && (statusReg1 & UTSR1_TNF));	/* while not full */

	if (!gotOne)
	    {
	    /* no more chars to send - disable transmitter interrupts */

	    SA1100_UART_REG_READ (pChan, UTCR3, ch);
	    ch &= ~UTCR3_TIM;
	    SA1100_UART_REG_WRITE (pChan, UTCR3, ch);
	    }
	}
    }

/*******************************************************************************
*
* sa1100TxStartup - transmitter startup routine
*
* Call interrupt level char output routine and enable interrupt
*
* RETURNS: OK on success, ENOSYS if the device is polled-only, or
* EIO on hardware error.
*/

LOCAL int sa1100TxStartup
    (
    SIO_CHAN *	pSioChan	/* ptr to SIO_CHAN describing this channel */
    )
    {
    UINT32 cr3;
    SA1100_CHAN * pChan = (SA1100_CHAN *)pSioChan;

    if (pChan->channelMode == SIO_MODE_INT)
	{
	/* Enable Transmitter interrupts */

	SA1100_UART_REG_READ (pChan, UTCR3, cr3);
	cr3 |= UTCR3_TIM;
	SA1100_UART_REG_WRITE (pChan, UTCR3, cr3);
	return OK;
	}
    else
	return ENOSYS;
    }

/******************************************************************************
*
* sa1100PollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
* if the output buffer is full, ENOSYS if the device is interrupt-only.
*/

LOCAL int sa1100PollOutput
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    char	outChar 	/* char to output */
    )
    {
    SA1100_CHAN * pChan = (SA1100_CHAN *)pSioChan;
    UINT32 pollStatus;

    SA1100_UART_REG_READ (pChan, UTSR1, pollStatus);

    /* is the transmitter ready to accept a character? */

    if ((pollStatus & UTSR1_TNF) == 0x00)
	return EAGAIN;


    /* write out the character */

    SA1100_UART_REG_WRITE (pChan, UTDR, outChar);	/* transmit character */

    return OK;
    }

/******************************************************************************
*
* sa1100PollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
* if the input buffer is empty, ENOSYS if the device is interrupt-only.
*/

LOCAL int sa1100PollInput
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    char *	thisChar	/* pointer to where to return character */
    )
    {
    SA1100_CHAN * pChan = (SA1100_CHAN *)pSioChan;
    UINT32 pollStatus;

    SA1100_UART_REG_READ (pChan, UTSR1, pollStatus);

    if ((pollStatus & UTSR1_RNE) == 0x00)
	return EAGAIN;


    /* got a character */

    SA1100_UART_REG_READ (pChan, UTDR, *thisChar);

    return OK;
    }

/******************************************************************************
*
* sa1100CallbackInstall - install ISR callbacks to get/put chars.
*
* This routine installs interrupt callbacks for transmitting characters
* and receiving characters.
*
* RETURNS: OK on success, or ENOSYS for an unsupported callback type.
*
*/

LOCAL int sa1100CallbackInstall
    (
    SIO_CHAN *	pSioChan,	/* ptr to SIO_CHAN describing this channel */
    int		callbackType,	/* type of callback */
    STATUS	(*callback)(),	/* callback */
    void *	callbackArg	/* parameter to callback */
		 
    )
    {
    SA1100_CHAN * pChan = (SA1100_CHAN *)pSioChan;

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
