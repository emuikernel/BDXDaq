/* coldfireSio.c - coldfire Serial Communications driver */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,31aug98,gls  Fixed incorrect usage of CLOCAL
01b,09jul98,gls  Adapted to WRS coding conventions
01a,01may98,mem  written, based on m5204Sio.c.
*/

/*
DESCRIPTION
This is the driver for the UART contained in the ColdFire Microcontroller.

Only asynchronous serial operation is supported by this driver.  The
default serial settings are 8 data bits, 1 stop bit, no parity, 9600
buad, and software flow control.  These default settings can be
overridden by setting the COLDFIRE_CHAN options and baudRate fields to
the desired values before calling coldfireDevInit.  See sioLib.h for
options values.  The defaults for the module can be changed by
redefining the macros COLDFIRE_DEFAULT_OPTIONS and COLDFIRE_DEFAULT_BAUD and
recompiling this driver.

This driver uses the system clock as the input to the baud rate
generator.  The clkRate field must be set to the system clock rate in
HZ for the baud rate calculations to work correctly.  The actual range
of supported baud rates depends on the system clock speed.  For example,
with a 25MHz system clock, the lowest baud rate is 24, and the highest
is over 38400.  Because the baud rate values are calculated on request,
there is no checking that the requested baud rate is standard, you can
set the UART to operate at 4357 baud if you wish.

USAGE
A COLDFIRE_CHAN structure is used to describe the chip.

The BSP's sysHwInit() routine typically calls sysSerialHwInit()
which initializes all the H/W addresses in the COLDFIRE_CHAN structure
before calling coldfireDevInit().  This enables the chip to operate in
polled mode, but not interrupt mode.  Calling coldfireDevInit2() from
the sysSerialHwInit2() routine allows interrupts to be enabled and
interrupt mode operation to be used.

.CS
i.e.

#include "drv/multi/coldfireSio.h"

COLDFIRE_CHAN coldfireUart;			/@ my device structure @/

#define INT_VEC_UART	(24+3)		/@ use single vector, #27 @/

sysSerialHwInit()
    {
    /@ initialize the register pointers/data for uart @/
    coldfireUart.clkRate	= MASTER_CLOCK;
    coldfireUart.intVec	= INT_VEC_UART;
    coldfireUart.mr	= COLDFIRE_UART_MR(SIM_BASE);
    coldfireUart.sr	= COLDFIRE_UART_SR(SIM_BASE);
    coldfireUart.csr	= COLDFIRE_UART_CSR(SIM_BASE);
    coldfireUart.cr	= COLDFIRE_UART_CR(SIM_BASE);
    coldfireUart.rb	= COLDFIRE_UART_RB(SIM_BASE);
    coldfireUart.tb	= COLDFIRE_UART_TB(SIM_BASE);
    coldfireUart.ipcr	= COLDFIRE_UART_IPCR(SIM_BASE);
    coldfireUart.acr	= COLDFIRE_UART_ACR(SIM_BASE);
    coldfireUart.isr	= COLDFIRE_UART_ISR(SIM_BASE);
    coldfireUart.imr	= COLDFIRE_UART_IMR(SIM_BASE);
    coldfireUart.bg1	= COLDFIRE_UART_BG1(SIM_BASE);
    coldfireUart.bg2	= COLDFIRE_UART_BG2(SIM_BASE);
    coldfireUart.ivr	= COLDFIRE_UART_IVR(SIM_BASE);
    coldfireUart.ip	= COLDFIRE_UART_IP(SIM_BASE);
    coldfireUart.op1	= COLDFIRE_UART_OP1(SIM_BASE);
    coldfireUart.op2	= COLDFIRE_UART_OP2(SIM_BASE);

    coldfireDevInit (&coldfireUart);
    }
.CE

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chips interrupts via intConnect() to the single
interrupt handler coldfireInt.  After the interrupt service routines are 
connected, the user then calls coldfireDevInit2() to allow the driver to 
turn on interrupt enable bits.

.CS
i.e.

sysSerialHwInit2 ()
    {
    /@ connect single vector for 5204 @/
    intConnect (INUM_TO_IVEC(MY_VEC), coldfireInt, (int)&coldfireUart);

    ...

    /@ allow interrupts to be enabled @/
    coldfireDevInit2 (&coldfireUart);
    }
.CE

SPECIAL CONSIDERATIONS:

The CLOCAL hardware option presumes that CTS outputs are not wired as
necessary.  CLOCAL is one of the default options for this
reason.

As to the output port, this driver does not manipulate the output port,
or it's configuration register in any way.  As stated above, if the user
does not select the CLOCAL option then the output port bit must be wired
correctly or the hardware flow control will not function as desired.

INCLUDE FILES: drv/sio/coldfireSio.h
*/

#include "vxWorks.h"
#include "sioLib.h"
#include "intLib.h"
#include "errno.h"
#include "drv/sio/coldfireSio.h"

/* forward static declarations */

LOCAL STATUS coldfireModeSet (COLDFIRE_CHAN *, UINT);
LOCAL STATUS coldfireBaudSet (COLDFIRE_CHAN *, UINT);
LOCAL STATUS coldfireOptsSet (COLDFIRE_CHAN *, UINT);

LOCAL int    coldfireIoctl (COLDFIRE_CHAN *, int, void *);
LOCAL int    coldfireTxStartup (COLDFIRE_CHAN *);
LOCAL int    coldfireCallbackInstall (COLDFIRE_CHAN *, int, STATUS (*)(), void *);
LOCAL int    coldfirePollInput (COLDFIRE_CHAN *, char *);
LOCAL int    coldfirePollOutput (COLDFIRE_CHAN*, char);
LOCAL STATUS coldfireDummyCallback (void);
      void coldfireAcrSetClr (COLDFIRE_CHAN * pChan,
			      UCHAR setBits, UCHAR clearBits);
      void coldfireImrSetClr (COLDFIRE_CHAN * pChan,
			      UCHAR setBits, UCHAR clearBits);
      void coldfireOprSetClr (COLDFIRE_CHAN * pChan,
			      UCHAR setBits, UCHAR clearBits);

/* driver functions */

LOCAL SIO_DRV_FUNCS coldfireSioDrvFuncs =
    {
    (int (*)(SIO_CHAN *, int, void *))coldfireIoctl,
    (int (*)(SIO_CHAN *))coldfireTxStartup,
    (int (*)())coldfireCallbackInstall,
    (int (*)(SIO_CHAN *, char*))coldfirePollInput,
    (int (*)(SIO_CHAN *, char))coldfirePollOutput
    };

/* typedefs */

/* defines */

#ifndef COLDFIRE_DEFAULT_BAUD
#   define COLDFIRE_DEFAULT_BAUD  9600
#endif

#ifndef COLDFIRE_DEFAULT_OPTIONS

    /* no handshake, rcvr enabled, 8 data bits, 1 stop bit, no parity */

#   define COLDFIRE_DEFAULT_OPTIONS (CREAD | CS8 | CLOCAL)
#endif

/* Hardware read/write routines.  Can be redefined to create drivers
 * for boards with special i/o access procedures. The reg pointer
 * arguments are the register pointers from the COLDFIRE_CHAN structure.
 */

#ifndef COLDFIRE_READ
#   define COLDFIRE_READ(x)	(*x)	/* argument is register pointer */
#endif

#ifndef COLDFIRE_WRITE
#   define COLDFIRE_WRITE(x,y)	(*x = y) /* args are reg ptr and data */
#endif

#define MAX_OPTIONS	(0xff)

/******************************************************************************
*
* coldfireDevInit - intialize a COLDFIRE_CHAN
*
* The BSP must have already initialized all the device addresses, etc in
* coldfire_CHAN structure. This routine initializes some transmitter &
* receiver status values to be used in the interrupt mask register and then
* resets the chip to a quiescent state.
*
* RETURNS: N/A
*/

void coldfireDevInit
    (
    COLDFIRE_CHAN * pChan
    )
    {
    int baudRate;
    int oldlevel;
    
    /* save away requested baud rate */

    baudRate = pChan->baudRate;
    pChan->baudRate = 0;

    pChan->pDrvFuncs	= &coldfireSioDrvFuncs;
    pChan->getTxChar	= coldfireDummyCallback;
    pChan->putRcvChar	= coldfireDummyCallback;

    coldfireImrSetClr (pChan, 0, ~0);
    coldfireAcrSetClr (pChan, 0, ~0);
    coldfireOprSetClr (pChan, 0, ~0);

    pChan->mode		= 0; /* undefined */
    pChan->intEnable	= FALSE;
    pChan->acrCopy	= 0;
    pChan->imrCopy	= 0;
    pChan->oprCopy	= 0;

    if ((pChan->options == 0) || (pChan->options > MAX_OPTIONS))
	pChan->options = COLDFIRE_DEFAULT_OPTIONS;

    oldlevel = intLock ();

    /* Clear the interrupt mask register */

    coldfireImrSetClr (pChan, 0, ~0);

    /* Reset the transmitters & receivers  */

    COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_RESET_RX);
    COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_RESET_TX);
    COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_RESET_ERR);
    COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_RESET_BRK);
    COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_RESET_ERR);

    /* Use internal timer */

    COLDFIRE_WRITE (pChan->csr,
		    COLDFIRE_UART_CSR_TIMER_RX | COLDFIRE_UART_CSR_TIMER_TX);

    /* TX is disabled */

    coldfireOptsSet (pChan, pChan->options);

    if (coldfireBaudSet(pChan, baudRate) == ERROR)
	coldfireBaudSet (pChan, COLDFIRE_DEFAULT_BAUD);

    intUnlock (oldlevel);
    }

/******************************************************************************
*
* coldfireDevInit2 - intialize a COLDFIRE_CHAN, part 2
*
* This routine is called as part of sysSerialHwInit2() and tells
* the driver that interrupt vectors are connected and that it is
* safe to allow interrupts to be enabled.
*
* RETURNS: N/A
*/

void coldfireDevInit2
    (
    COLDFIRE_CHAN * pChan
    )
    {

    /* Allow interrupt mode */

    pChan->intEnable = TRUE;
    COLDFIRE_WRITE (pChan->ivr, pChan->intVec);
    coldfireModeSet (pChan, pChan->mode);
    }

/******************************************************************************
*
* coldfireImrSetClr - set and clear bits in the UART's interrupt mask register
*
* This routine sets and clears bits in the UART's IMR.
*
* This routine sets and clears bits in a local copy of the IMR, then
* writes that local copy to the UART.  This means that all changes to
* the IMR must go through this routine.  Otherwise, any direct changes
* to the IMR would be lost the next time this routine is called.
*
* Set has priority over clear.  Thus you can use this routine to update
* multiple bit fields by specifying the field mask as the clear bits.
*
* RETURNS: N/A
*/

void coldfireImrSetClr
    (
    COLDFIRE_CHAN * pChan,
    UCHAR setBits,       /* which bits to set in the IMR   */
    UCHAR clearBits      /* which bits to clear in the IMR */
    )
    {
    pChan->imrCopy = ((pChan->imrCopy & ~clearBits) | setBits);
    COLDFIRE_WRITE (pChan->imr, pChan->imrCopy);
    }

/******************************************************************************
*
* coldfireImr - return current interrupt mask register contents
*
* This routine returns the interrupt mask register contents.  The imr
* is not directly readable, a copy of the last value written is kept
* in the UART data structure.
*
* RETURNS: Returns interrupt mask register contents.
*/

UCHAR coldfireImr
    (
    COLDFIRE_CHAN * pChan
    )
    {
    return (pChan->imrCopy);
    }

/******************************************************************************
*
* coldfireAcrSetClr - set and clear bits in the UART's aux control register
*
* This routine sets and clears bits in the UART's ACR.
*
* This routine sets and clears bits in a local copy of the ACR, then
* writes that local copy to the UART.  This means that all changes to
* the ACR must go through this routine.  Otherwise, any direct changes
* to the ACR would be lost the next time this routine is called.
*
* Set has priority over clear.  Thus you can use this routine to update
* multiple bit fields by specifying the field mask as the clear bits.
*
* RETURNS: N/A
*/

void coldfireAcrSetClr
    (
    COLDFIRE_CHAN * pChan,
    UCHAR setBits,       /* which bits to set in the ACR   */
    UCHAR clearBits      /* which bits to clear in the ACR */
    )
    {
    pChan->acrCopy = ((pChan->acrCopy & ~clearBits) | setBits);
    COLDFIRE_WRITE (pChan->acr, pChan->acrCopy);
    }

/******************************************************************************
*
* coldfireAcr - return aux control register contents
*
* This routine returns the auxilliary control register contents.  The acr
* is not directly readable, a copy of the last value written is kept
* in the UART data structure.
*
* RETURNS: Returns auxilliary control register (acr) contents.
*/

UCHAR coldfireAcr
    (
    COLDFIRE_CHAN * pChan
    )
    {
    return (pChan->acrCopy);
    }

/******************************************************************************
*
* coldfireOprSetClr - set and clear bits in the output port register
*
* This routine sets and clears bits in the UART's OPR.
*
* A copy of the current opr contents is kept in the UART data structure.
*
* RETURNS: N/A
*/

void coldfireOprSetClr
    (
    COLDFIRE_CHAN * pChan,
    UCHAR setBits,       /* which bits to set in the OPR   */
    UCHAR clearBits      /* which bits to clear in the OPR */
    )
    {
    pChan->oprCopy = ((pChan->oprCopy & ~clearBits) | setBits);
    COLDFIRE_WRITE (pChan->op2, clearBits);
    COLDFIRE_WRITE (pChan->op1, setBits);
    }

/******************************************************************************
*
* coldfireOpr - return the current state of the output register
*
* This routine returns the current state of the output register from
* the saved copy in the UART data structure.  The actual opr contents
* are not directly readable.
*
* RETURNS: Returns the current output register state.
*/

UCHAR coldfireOpr
    (
    COLDFIRE_CHAN * pChan
    )
    {
    return (pChan->oprCopy);
    }

/******************************************************************************
*
* coldfireDummyCallback - dummy callback routine.
*
* RETURNS:
* Always returns ERROR
*/

LOCAL STATUS coldfireDummyCallback (void)
    {
    return (ERROR);
    }

/******************************************************************************
*
* coldfireTxStartup - start the interrupt transmitter.
*
* This routine enables the transmitters for the specified UART channel so that
* the UART channel will interrupt the CPU when TxRDY bit in the status
* register is set.
*
* Just enable the transmitter, don't output.  The driver may have just
* switched from POLL mode and may still be outputting a character.
*
* RETURNS:
* Returns OK on success, or EIO on hardware error.
*/

LOCAL int coldfireTxStartup
    (
    COLDFIRE_CHAN * pChan
    )
    {
    COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_TX_ENABLE);
    return (OK);
    }

/******************************************************************************
*
* coldfireCallbackInstall - install ISR callbacks to get/put chars.
*
* This driver allows interrupt callbacks, for transmitting characters
* and receiving characters. In general, drivers may support other types
* of callbacks too.
*
* RETURNS:
* Returns OK on success, or ENOSYS for an unsupported callback type.
*/

LOCAL int coldfireCallbackInstall
    (
    COLDFIRE_CHAN *	pChan,
    int		callbackType,
    STATUS	(*callback)(),
    void *      callbackArg
    )
    {
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

/******************************************************************************
*
* coldfirePollOutput - output a character in polled mode.
*
* This routine polls the status register to see if the TxRDY bit has been set.
* This signals that the transmit holding register is empty and that the
* UART is ready for transmission.
*
* RETURNS:
* Returns OK if a character sent, EIO on device error, EAGAIN
* if the output buffer is full.
*/

LOCAL int coldfirePollOutput
    (
    COLDFIRE_CHAN *pChan,
    char	outChar
    )
    {
    char statusReg;

    /* is the transitter ready to accept a character? */

    statusReg = COLDFIRE_READ (pChan->sr);
    if ((statusReg & COLDFIRE_UART_SR_TXRDY) == 0x00)
	return (EAGAIN);

    /* write out the character */

    COLDFIRE_WRITE (pChan->tb, outChar);

    return (OK);
    }

/******************************************************************************
*
* coldfirePollInput - poll the device for input.
*
* This routine polls the status register to see if the RxRDY bit is set.
* This gets set when the UART receives a character and signals the
* pressence of a character in the receive buffer.
*
* RETURNS:
* Returns OK if a character arrived, EIO on device error, EAGAIN
* if the input buffer if empty.
*/

LOCAL int coldfirePollInput
    (
    COLDFIRE_CHAN *pChan,
    char *	thisChar
    )
    {
    char statusReg;

    statusReg = COLDFIRE_READ (pChan->sr);
    if ((statusReg & COLDFIRE_UART_SR_RXRDY) == 0x00)
	return (EAGAIN);
    *thisChar = COLDFIRE_READ (pChan->rb);
    return (OK);
    }

/******************************************************************************
*
* coldfireModeSet - change channel mode setting
*
* This driver supports both polled and interrupt modes and is capable of
* switching between modes dynamically. If interrupt mode is desired this
* routine enables the channels receiver, transmitter interrupt and the
* received break condition interrupt. If polled mode is desired the xmitrs for
* the specified channel is enabled.
*
* RETURNS:
* Returns a status of OK if the mode was set else ERROR.
*/

LOCAL STATUS coldfireModeSet
    (
    COLDFIRE_CHAN * pChan,
    UINT	newMode
    )
    {
    int oldlevel;

    if ((newMode != SIO_MODE_POLL) && (newMode != SIO_MODE_INT))
	return (ERROR);

    oldlevel = intLock ();
    if ((newMode == SIO_MODE_INT) && pChan->intEnable)
        {

	/* Enable the interrupts for receiver/transmitter conditions */

	coldfireImrSetClr (pChan,
			   COLDFIRE_UART_IMR_TXRDY | COLDFIRE_UART_IMR_RXRDY,
			   0);
        }
    else
        {

	/* Disable interrupts and enable the transmitter for the channel */

	coldfireImrSetClr (pChan,
			   0,
			   COLDFIRE_UART_IMR_TXRDY | COLDFIRE_UART_IMR_RXRDY);
	COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_TX_ENABLE);
        }
    pChan->mode = newMode;
    intUnlock (oldlevel);

    return (OK);
    }

/******************************************************************************
*
* coldfireBaudSet - change baud rate for channel
*
* This routine sets the baud rate for the UART. The interrupts are disabled
* during chip access.
*
* RETURNS:
* Returns a status of OK if the baud rate was set else ERROR.
*/

LOCAL STATUS coldfireBaudSet
    (
    COLDFIRE_CHAN *	pChan,
    UINT		baud
    )
    {
    if (!baud)
	return (ERROR);
    if (baud != pChan->baudRate)
	{
	UINT val;
	int oldlevel;

	val = ((pChan->clkRate / baud) + 16) / 32;
	if ((val < 2) || (val > 0xffff))
	    return (ERROR);

	oldlevel = intLock ();
	pChan->baudRate = baud;
	COLDFIRE_WRITE (pChan->bg1, val >> 8);
	COLDFIRE_WRITE (pChan->bg2, val);
	intUnlock (oldlevel);
	}
    return (OK);
    }

/******************************************************************************
*
* coldfireOptsSet - set the serial options
*
* Set the channel operating mode to that specified.  All sioLib options
* are supported: CLOCAL, CREAD, CSIZE, PARENB, and PARODD.
*
* Note, this routine disables the transmitter.  The calling routine
* may have to re-enable it.
*
* RETURNS:
* Returns OK to indicate success, otherwise ERROR is returned
*/

LOCAL STATUS coldfireOptsSet
    (
    COLDFIRE_CHAN * pChan, /* ptr to channel */
    UINT options	/* new hardware options */
    )
    {
    int mr1Value;
    int mr2Value;
    int lvl;

    if ((pChan == NULL) || (options & 0xffffff00))
	return (ERROR);

    /* Reset the transmitters  & receivers  */

    switch (options & CSIZE)
	{
	case CS5:
	    mr1Value = COLDFIRE_UART_MR1_BITS_CHAR_5;
	    break;
	case CS6:
	    mr1Value = COLDFIRE_UART_MR1_BITS_CHAR_6;
	    break;
	case CS7:
	    mr1Value = COLDFIRE_UART_MR1_BITS_CHAR_7;
	    break;
	default:
	case CS8:
	    mr1Value = COLDFIRE_UART_MR1_BITS_CHAR_8;
	    break;
	}

    if (options & STOPB)
	mr2Value = COLDFIRE_UART_MR2_STOP_BITS_2;
    else
	mr2Value = COLDFIRE_UART_MR2_STOP_BITS_1;

    switch (options & (PARENB|PARODD))
	{
	case PARENB|PARODD:
	    mr1Value |= COLDFIRE_UART_MR1_ODD_PARITY;
	    break;
	case PARENB:
	    mr1Value |= COLDFIRE_UART_MR1_EVEN_PARITY;
	    break;
	case PARODD:
	    mr1Value |= COLDFIRE_UART_MR1_PAR_MODE_MULTI;
	    break;
	default:
	case 0:
	    mr1Value |= COLDFIRE_UART_MR1_NO_PARITY;
	    break;
	}

    if (!(options & CLOCAL))
	{

	/* enable hardware flow control */

	mr1Value |= COLDFIRE_UART_MR1_RX_RTS;
	mr2Value |= COLDFIRE_UART_MR2_TX_CTS;
	}

    lvl = intLock ();

    /* now reset the channel mode registers */

    COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_RESET_MODE_PTR
		    | COLDFIRE_UART_CR_RX_DISABLE
		    | COLDFIRE_UART_CR_TX_DISABLE);
    COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_RESET_TX);
    COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_RESET_RX);

    COLDFIRE_WRITE (pChan->mr, mr1Value);  /* mode register 1  */
    COLDFIRE_WRITE (pChan->mr, mr2Value);  /* mode register 2  */

    if (options & CREAD)
	COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_RX_ENABLE);

    pChan->options = options;
    intUnlock (lvl);

    return (OK);
    }

/******************************************************************************
*
* coldfireIoctl - special device control
*
* RETURNS:
* Returns OK on success, EIO on device error, ENOSYS on unsupported
* request.
*/

LOCAL int coldfireIoctl
    (
    COLDFIRE_CHAN *	pChan,		/* device to control */
    int		request,		/* request code */
    void *	someArg			/* some argument */
    )
    {
    STATUS result = OK;
    int     arg = (int)someArg;

    switch (request)
	{
	case SIO_BAUD_SET:
	    if (coldfireBaudSet(pChan, arg) != OK)
		result = EIO;
	    break;

	case SIO_BAUD_GET:
	    *(int *)arg = pChan->baudRate;
	    return (OK);

	case SIO_MODE_SET:
	    if (coldfireModeSet(pChan, arg) != OK)
		result = EIO;
	    break;

	case SIO_MODE_GET:
	    *(int *)arg = pChan->mode;
	    return (OK);

	case SIO_AVAIL_MODES_GET:
	    *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    return (OK);

	case SIO_HW_OPTS_SET:

	    /* change options, then set mode to restart chip correctly */

	    if (coldfireOptsSet(pChan, arg) != OK)
		result = EIO;
	    coldfireModeSet (pChan, pChan->mode);
	    break;

	case SIO_HW_OPTS_GET:
	    return (pChan->options);

	default:
	    return (ENOSYS);
	}
    return (result);
    }


/******************************************************************************
*
* coldfireIntWr - handle a transmitter interrupt
*
* This routine handles write interrupts from the UART. This isr is invoked
* when the TxRDY bit in the interrupt status register has been set. If there
* is no character to transmit the transmitter is disabled.
*
* RETURNS: N/A
*/

LOCAL void coldfireIntWr
    (
    COLDFIRE_CHAN * pChan
    )
    {
    char            outChar;

    if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)
	{

	/* if char available, tx it */

	COLDFIRE_WRITE (pChan->tb, outChar);
	}
    else
        {
	COLDFIRE_WRITE (pChan->cr, COLDFIRE_UART_CR_TX_DISABLE);
        }
    }


/*****************************************************************************
*
* coldfireIntRd - handle a reciever interrupt
*
* This routine handles read interrupts from the UART.
* The UART has been programmed to generate read interrupts when the RXRDY
* status bit has been set in the interrupt status register. When a character
* has been received it is removed from the receive buffer.
*
* RETURNS: N/A
*/

LOCAL void coldfireIntRd
    (
    COLDFIRE_CHAN * pChan
    )
    {
    UCHAR	inchar;

    while (COLDFIRE_READ(pChan->sr) & COLDFIRE_UART_SR_RXRDY)
        {
	inchar = COLDFIRE_READ (pChan->rb);
	(*pChan->putRcvChar) (pChan->putRcvArg, inchar);
	if (pChan->mode != SIO_MODE_INT)
	    break;
        }
    }


/*******************************************************************************
*
* coldfireInt - handle all interrupts in one vector
*
* All interrupts share a single interrupt vector.
* We identify each interrupting source and service it.
* We must service all interrupt sources for those systems with edge-
* sensitive interrupt controllers.
*
* RETURNS: N/A
*/

void coldfireInt
    (
    COLDFIRE_CHAN * pChan
    )
    {
    UCHAR        intStatus;

    /* loop until all sources have been handled */

    while (intStatus = (COLDFIRE_READ(pChan->isr) & pChan->imrCopy))
	{
	if (intStatus & COLDFIRE_UART_ISR_TXRDY)
	    coldfireIntWr (pChan);
	if (intStatus & COLDFIRE_UART_ISR_RXRDY)
	    coldfireIntRd (pChan);
	if (pChan->mode != SIO_MODE_INT)
	    break;
	}
    }
