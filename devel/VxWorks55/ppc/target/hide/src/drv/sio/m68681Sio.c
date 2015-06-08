/* m68681Sio.c - M68681 serial communications driver */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,18dec96,db   fixed bugs in m68681OptsSet() and m68681Ioctl().(SPR #7641).
		 changed M68681_DEFAULT_OPTIONS to disable hardware handshake.
01e,06nov96,dgp  doc: final formatting
01d,03jun96,dat  added m68681Opr, m68681Opcr, m68681OpcrSetClr, m68681OprSetClr
01c,21may96,dat  m68681Int loops until all sources serviced.
01b,29mar96,dat  more comments, added m68681OptsSet(), m68681AcrSetClr()
01a,14feb96,dds  written.
*/

/*
DESCRIPTION
This is the driver for the M68681 DUART. This device includes two universal
asynchronous receiver/transmitters, a baud rate generator, and a counter/timer
device.  This driver module provides control of the two serial channels and
the baud-rate generator.  The counter timer is controlled by a separate
driver, src/drv/timer/m68681Timer.c.

A M68681_DUART structure is used to describe the chip. This data structure
contains two M68681_CHAN structures which describe the chip's two serial
channels.  The M68681_DUART structure is defined in m68681Sio.h.

Only asynchronous serial operation is supported by this driver.
The default serial settings are 8 data bits, 1 stop bit, no parity, 9600
baud, and software flow control.  These default settings can be overridden
on a channel-by-channel basis by setting the M68681_CHAN options and `baudRate'
fields to the desired values before calling m68681DevInit().  See sioLib.h
for option values.  The defaults for the module can be changed by
redefining the macros M68681_DEFAULT_OPTIONS and M68681_DEFAULT_BAUD and
recompiling this driver.

This driver supports baud rates of 75, 110, 134.5, 150, 300, 600, 1200,
2000, 2400, 4800, 1800, 9600, 19200, and 38400.

USAGE
The BSP's sysHwInit() routine typically calls sysSerialHwInit()
which initializes all the hardware addresses in the M68681_DUART structure
before calling m68681DevInit().  This enables the chip to operate in
polled mode, but not in interrupt mode.  Calling m68681DevInit2() from
the sysSerialHwInit2() routine allows interrupts to be enabled and
interrupt-mode operation to be used.

The following example shows the first part of the initialization thorugh
calling m68681DevInit():
.CS
#include "drv/sio/m68681Sio.h"

M68681_DUART myDuart;	/@ my device structure @/

#define MY_VEC	(71)	/@ use single vector, #71 @/

sysSerialHwInit()
    {
    /@ initialize the register pointers for portA @/
    myDuart.portA.mr	= M68681_MRA;
    myDuart.portA.sr	= M68681_SRA;
    myDuart.portA.csr	= M68681_CSRA;
    myDuart.portA.cr	= M68681_CRA;
    myDuart.portA.rb	= M68681_RHRA;
    myDuart.portA.tb	= M68681_THRA;

    /@ initialize the register pointers for portB @/
    myDuart.portB.mr = M68681_MRB;
    ...

    /@ initialize the register pointers/data for main duart @/
    myDuart.ivr		= MY_VEC;
    myDuart.ipcr	= M68681_IPCR;
    myDuart.acr		= M68681_ACR;
    myDuart.isr		= M68681_ISR;
    myDuart.imr		= M68681_IMR;
    myDuart.ip		= M68681_IP;
    myDuart.opcr	= M68681_OPCR;
    myDuart.sopbc	= M68681_SOPBC;
    myDuart.ropbc	= M68681_ROPBC;
    myDuart.ctroff	= M68681_CTROFF;
    myDuart.ctron	= M68681_CTRON;
    myDuart.ctlr	= M68681_CTLR;
    myDuart.ctur	= M68681_CTUR;


    m68681DevInit (&myDuart);
    }
.CE

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chips interrupts via intConnect() to the single
interrupt handler m68681Int().  After the interrupt service routines are 
connected, the user then calls m68681DevInit2() to allow the driver to 
turn on interrupt enable bits, as shown in the following example:

.CS
sysSerialHwInit2 ()
    {
    /@ connect single vector for 68681 @/
    intConnect (INUM_TO_IVEC(MY_VEC), m68681Int, (int)&myDuart);

    ...

    /@ allow interrupts to be enabled @/
    m68681DevInit2 (&myDuart);
    }
.CE

SPECIAL CONSIDERATIONS:
The CLOCAL hardware option presumes that OP0 and OP1 output bits are wired
to the CTS outputs for channel 0 and channel 1 respectively.  If not wired
correctly, then the user must not select the CLOCAL option.  CLOCAL is
not one of the default options for this reason.

This driver does not manipulate the output port
or its configuration register in any way.  If the user
selects the CLOCAL option, then the output port bit must be wired correctly
or the hardware flow control will not function correctly.

INCLUDE FILES: drv/sio/m68681Sio.h
*/

#include "vxWorks.h"
#include "sioLib.h"
#include "intLib.h"
#include "errno.h"
#include "drv/sio/m68681Sio.h"

/* forward static declarations */

LOCAL void   m68681InitChannel (M68681_CHAN *, M68681_DUART *);
LOCAL void   m68681InitStruct (M68681_CHAN *);
LOCAL STATUS m68681ModeSet (M68681_CHAN *, UINT);
LOCAL STATUS m68681BaudSet (M68681_CHAN *, UINT);
LOCAL STATUS m68681OptsSet (M68681_CHAN *, UINT);

LOCAL int    m68681Ioctl (M68681_CHAN *, int, void *);
LOCAL int    m68681TxStartup (M68681_CHAN *);
LOCAL int    m68681CallbackInstall (M68681_CHAN *, int, STATUS (*)(), void *);
LOCAL int    m68681PollInput (M68681_CHAN *, char *);
LOCAL int    m68681PollOutput (M68681_CHAN*, char);

/* driver functions */

LOCAL SIO_DRV_FUNCS m68681SioDrvFuncs =
    {
    (int (*)(SIO_CHAN *, int, void *))m68681Ioctl,
    (int (*)(SIO_CHAN *))m68681TxStartup,
    (int (*)())m68681CallbackInstall,
    (int (*)(SIO_CHAN *, char*))m68681PollInput,
    (int (*)(SIO_CHAN *, char))m68681PollOutput
    };

/* typedefs */

typedef struct        /* M68681_BAUD */
    {
    int rate;		/* a baud rate */
    UCHAR csrVal;	/* rate to write to the csr reg to get that baud rate */
    UCHAR baudBit;	/* baud rate select bit, in ACR */
    } M68681_BAUD;


/*
 * BaudTable is a table of the available baud rates, and the values to write
 * to the csr reg to get those rates
 */

#define SET1	0
#define SET2	M68681_ACR_BRG_SELECT

LOCAL M68681_BAUD baudTable [] =
    {
    {75,        M68681_CSR_RX_75    | M68681_CSR_TX_75,		SET2},
    {110,       M68681_CSR_RX_110   | M68681_CSR_TX_110,	SET2},
    {134,       M68681_CSR_RX_134_5 | M68681_CSR_TX_134_5,	SET2},
    {150,       M68681_CSR_RX_150   | M68681_CSR_TX_150,	SET2},
    {300,       M68681_CSR_RX_300   | M68681_CSR_TX_300,	SET2},
    {600,       M68681_CSR_RX_600   | M68681_CSR_TX_600,	SET2},
    {1200,      M68681_CSR_RX_1200  | M68681_CSR_TX_1200,	SET2},
    {2000,      M68681_CSR_RX_2000  | M68681_CSR_TX_2000,	SET2},
    {2400,      M68681_CSR_RX_2400  | M68681_CSR_TX_2400,	SET2},
    {4800,      M68681_CSR_RX_4800  | M68681_CSR_TX_4800,	SET2},
    {1800,      M68681_CSR_RX_1800  | M68681_CSR_TX_1800,	SET2},
    {9600,      M68681_CSR_RX_9600  | M68681_CSR_TX_9600,	SET2},
    {19200,     M68681_CSR_RX_19200 | M68681_CSR_TX_19200,	SET2},
    {38400,     M68681_CSR_RX_38400 | M68681_CSR_TX_38400,	SET1}
    };

#undef SET1
#undef SET2

/* defines */

#ifndef M68681_DEFAULT_BAUD
#   define M68681_DEFAULT_BAUD  9600
#endif

#ifndef M68681_DEFAULT_OPTIONS
    /* no handshake, rcvr enabled, 8 data bits, 1 stop bit, no parity */
#   define M68681_DEFAULT_OPTIONS (CLOCAL | CREAD | CS8)
#endif

/* Hardware read/write routines.  Can be redefined to create drivers
 * for boards with special i/o access procedures. The reg pointer
 * arguments are the register pointers from the M68681_CHAN or
 * M68681_DUART structure.
 */

#ifndef M68681_READ
#   define M68681_READ(x)	(*x)	/* argument is register pointer */
#endif

#ifndef M68681_WRITE
#   define M68681_WRITE(x,y)	(*x = y) /* args are reg ptr and data */
#endif

#define MAX_OPTIONS	(0xff)
#define MAX_BAUD	(38400)

/******************************************************************************
*
* m68681DevInit - intialize a M68681_DUART
*
* The BSP must already have initialized all the device addresses and
* register pointers in the M68681_DUART structure as described in
* `m68681Sio'. This routine initializes some transmitter and receiver status
* values to be used in the interrupt mask register and then resets the chip
* to a quiescent state.
*
* RETURNS: N/A
*/

void m68681DevInit
    (
    M68681_DUART * pDuart
    )
    {
    int oldlevel;
    char resetTimer;

    pDuart->intEnable		= FALSE;
    pDuart->tickRtn             = NULL;
    pDuart->tickArg             = 0;

    m68681ImrSetClr (pDuart, 0, -1);
    m68681AcrSetClr (pDuart, 0, -1);
    m68681OpcrSetClr(pDuart, 0, -1);
    m68681OprSetClr (pDuart, 0, -1);

    pDuart->portA.pDuart	= pDuart;
    pDuart->portA.xmitEnb	= M68681_IMR_TX_RDY_A;
    pDuart->portA.rcvrEnb	= M68681_IMR_RX_RDY_A;
    pDuart->portA.channel	= M68681_CHANNEL_A;

    pDuart->portB.pDuart	= pDuart;
    pDuart->portB.xmitEnb	= M68681_IMR_TX_RDY_B;
    pDuart->portB.rcvrEnb	= M68681_IMR_RX_RDY_B;
    pDuart->portB.channel	= M68681_CHANNEL_B;

    m68681InitStruct (&pDuart->portA);
    m68681InitStruct (&pDuart->portB);

    oldlevel =  intLock ();

    /* Clear the interrupt mask register */

    m68681ImrSetClr (pDuart, 0 , M68681_IMR_CLEAR);

    m68681InitChannel (&pDuart->portA, pDuart);
    m68681InitChannel (&pDuart->portB, pDuart);

    resetTimer                   = M68681_READ (pDuart->ctroff);

    intUnlock (oldlevel);
    }

/******************************************************************************
*
* m68681DevInit2 - intialize a M68681_DUART, part 2
*
* This routine is called as part of sysSerialHwInit2().  It tells
* the driver that interrupt vectors are connected and that it is
* safe to allow interrupts to be enabled.
*
* RETURNS: N/A
*/

void m68681DevInit2
    (
    M68681_DUART * pDuart
    )
    {
    /* Allow interrupt mode */

    pDuart->intEnable = TRUE;

    /* call ModeSet, to startup devices if needed */

    m68681ModeSet (&pDuart->portA, pDuart->portA.mode);
    m68681ModeSet (&pDuart->portB, pDuart->portB.mode);
    }

/*******************************************************************************
*
* m68681ImrSetClr - set and clear bits in the DUART interrupt-mask register
*
* This routine sets and clears bits in the DUART interrupt-mask register (IMR).
* It sets and clears bits in a local copy of the IMR, then
* writes that local copy to the DUART.  This means that all changes to
* the IMR must be performed by this routine.  Any direct changes
* to the IMR are lost the next time this routine is called.
*
* Set has priority over clear.  Thus you can use this routine to update
* multiple bit fields by specifying the field mask as the clear bits.
*
* RETURNS: N/A
*/

void m68681ImrSetClr
    (
    M68681_DUART * pDuart,
    UCHAR setBits,       /* which bits to set in the IMR   */
    UCHAR clearBits      /* which bits to clear in the IMR */
    )
    {
    pDuart->imrCopy = ((pDuart->imrCopy & ~clearBits) | setBits);
    M68681_WRITE (pDuart->imr, pDuart->imrCopy);
    }

/*******************************************************************************
*
* m68681Imr - return the current contents of the DUART interrupt-mask register
*
* This routine returns the contents of the interrupt-mask register (IMR).  
* The IMR is not directly readable; a copy of the last value written is kept
* in the DUART data structure.
*
* RETURNS: The contents of the interrupt-mask register.
*/

UCHAR m68681Imr
    (
    M68681_DUART * pDuart
    )
    {
    return pDuart->imrCopy;
    }

/*******************************************************************************
*
* m68681AcrSetClr - set and clear bits in the DUART auxiliary control register
*
* This routine sets and clears bits in the DUART auxiliary control register
* (ACR).  It sets and clears bits in a local copy of the ACR, then
* writes that local copy to the DUART.  This means that all changes to
* the ACR must be performed by this routine.  Any direct changes
* to the ACR are lost the next time this routine is called.
*
* Set has priority over clear.  Thus you can use this routine to update
* multiple bit fields by specifying the field mask as the clear bits.
*
* RETURNS: N/A
*/

void m68681AcrSetClr
    (
    M68681_DUART * pDuart,
    UCHAR setBits,       /* which bits to set in the ACR   */
    UCHAR clearBits      /* which bits to clear in the ACR */
    )
    {
    pDuart->acrCopy = ((pDuart->acrCopy &  ~clearBits) | setBits);
    M68681_WRITE (pDuart->acr, pDuart->acrCopy);
    }

/*******************************************************************************
*
* m68681Acr - return the contents of the DUART auxiliary control register
*
* This routine returns the contents of the auxilliary control register (ACR).
* The ACR is not directly readable; a copy of the last value written is kept
* in the DUART data structure.
*
* RETURNS: The contents of the auxilliary control register.
*/

UCHAR m68681Acr
    (
    M68681_DUART * pDuart
    )
    {
    return pDuart->acrCopy;
    }

/*******************************************************************************
*
* m68681OprSetClr - set and clear bits in the DUART output port register
*
* This routine sets and clears bits in the DUART output port register
* (OPR).  It sets and clears bits in a local copy of the OPR, then writes
* that local copy to the DUART.  This means that all changes to the OPR must
* be performed by this routine.  Any direct changes to the OPR are lost the
* next time this routine is called.
*
* Set has priority over clear.  Thus you can use this routine to update
* multiple bit fields by specifying the field mask as the clear bits.
*
* RETURNS: N/A
*/

void m68681OprSetClr
    (
    M68681_DUART * pDuart,
    UCHAR setBits,       /* which bits to set in the OPR   */
    UCHAR clearBits      /* which bits to clear in the OPR */
    )
    {
    pDuart->oprCopy = ((pDuart->oprCopy &  ~clearBits) | setBits);
    M68681_WRITE (pDuart->ropbc, clearBits);
    M68681_WRITE (pDuart->sopbc, setBits);
    }

/*******************************************************************************
*
* m68681Opr - return the current state of the DUART output port register
*
* This routine returns the current state of the output port register (OPR) 
* from the saved copy in the DUART data structure.  The actual OPR contents
* are not directly readable.
*
* RETURNS: The current state of the output port register.
*/

UCHAR m68681Opr
    (
    M68681_DUART * pDuart
    )
    {
    return pDuart->oprCopy;
    }

/*******************************************************************************
*
* m68681OpcrSetClr - set and clear bits in the DUART output port configuration register
*
* This routine sets and clears bits in the DUART output port configuration
* register (OPCR).  It sets and clears bits in a local copy of the OPCR,
* then writes that local copy to the DUART.  This means that all changes to
* the OPCR must be performed by this routine.  Any direct changes to the
* OPCR are lost the next time this routine is called.
*
* Set has priority over clear.  Thus you can use this routine to update
* multiple bit fields by specifying the field mask as the clear bits.
*
* RETURNS: N/A
*/

void m68681OpcrSetClr
    (
    M68681_DUART * pDuart,
    UCHAR setBits,       /* which bits to set in the OPCR   */
    UCHAR clearBits      /* which bits to clear in the OPCR */
    )
    {
    pDuart->opcrCopy = ((pDuart->opcrCopy &  ~clearBits) | setBits);
    M68681_WRITE (pDuart->opcr, pDuart->opcrCopy);
    }

/*******************************************************************************
*
* m68681Opcr - return the state of the DUART output port configuration register
*
* This routine returns the state of the output port configuration register
* (OPCR) from the saved copy in the DUART data structure.  The actual OPCR
* contents are not directly readable.
*
* RETURNS: The state of the output port configuration register.
*/

UCHAR m68681Opcr
    (
    M68681_DUART * pDuart
    )
    {
    return pDuart->opcrCopy;
    }

/******************************************************************************
*
* m68681DummyCallback - dummy callback routine.
*
* RETURNS:
* Always returns ERROR
*/

LOCAL STATUS m68681DummyCallback (void)
    {
    return (ERROR);
    }

/******************************************************************************
*
* m68681TxStartup - start the interrupt transmitter.
*
* This routine enables the transmitters for the specified DUART channel so that
* the DUART channel will interrupt the CPU when TxRDY bit in the status
* register is set.
*
* RETURNS:
* Returns OK on success, or EIO on hardware error.
*/

LOCAL int m68681TxStartup
    (
    M68681_CHAN * pChan
    )
    {
    char   outChar;
    int    lvl;

    if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)
	{
	lvl = intLock ();
	M68681_WRITE (pChan->cr, M68681_CR_TX_ENABLE);
	M68681_WRITE (pChan->tb, outChar);
	intUnlock (lvl);
	}
    return (OK);
    }

/******************************************************************************
*
* m68681CallbackInstall - install ISR callbacks to get/put chars.
*
* This driver allows interrupt callbacks, for transmitting characters
* and receiving characters. In general, drivers may support other types
* of callbacks too.
*
* RETURNS:
* Returns OK on success, or ENOSYS for an unsupported callback type.
*/

LOCAL int m68681CallbackInstall
    (
    M68681_CHAN *	pChan,
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
* m68681PollOutput - output a character in polled mode.
*
* This routine polls the status register to see if the TxRDY bit has been set.
* This signals that the transmit holding register is empty and that the
* specified DUART channel is ready for transmission.
*
* RETURNS:
* Returns OK if a character sent, EIO on device error, EAGAIN
* if the output buffer is full.
*/

LOCAL int m68681PollOutput
    (
    M68681_CHAN *	pChan,
    char	outChar
    )
    {
    char statusReg;

    statusReg = M68681_READ (pChan->sr);

    /* is the transitter ready to accept a character? */

    if ((statusReg & M68681_SR_TXRDY) == 0x00)
	return (EAGAIN);

    /* write out the character */

    M68681_WRITE (pChan->tb, outChar);

    return (OK);
    }

/******************************************************************************
*
* m68681PollInput - poll the device for input.
*
* This routine polls the status register to see if the RxRDY bit is set.
* This gets set when the DUART receives a character and signals the
* pressence of a character in the channels receive buffer.
*
* RETURNS:
* Returns OK if a character arrived, EIO on device error, EAGAIN
* if the input buffer if empty.
*/

LOCAL int m68681PollInput
    (
    M68681_CHAN *	pChan,
    char *	thisChar
    )
    {
    char statusReg;

    statusReg = M68681_READ (pChan->sr);

    if ((statusReg & M68681_SR_RXRDY) == 0x00)
	return (EAGAIN);

    M68681_WRITE (thisChar, *pChan->rb);

    return (OK);
    }

/******************************************************************************
*
* m68681ModeSet - change channel mode setting
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

LOCAL STATUS m68681ModeSet
    (
    M68681_CHAN * pChan,
    UINT	newMode
    )
    {
    int oldlevel;

    if ((newMode != SIO_MODE_POLL) && (newMode != SIO_MODE_INT))
	return (ERROR);

    oldlevel = intLock ();

    if (newMode == SIO_MODE_INT
     && pChan->pDuart->intEnable)
        {
	/* Enable the interrupts for receiver/transmitter conditions */

	m68681ImrSetClr (pChan->pDuart, (pChan->rcvrEnb | pChan->xmitEnb), 0);
        }
    else
        {
	/* Disable interrupts and enable the transmitter for the channel */

	m68681ImrSetClr (pChan->pDuart, 0, (pChan->rcvrEnb | pChan->xmitEnb));
	M68681_WRITE (pChan->cr, M68681_CR_TX_ENABLE);
        }

    intUnlock (oldlevel);
    pChan->mode = newMode;

    return (OK);
    }

/******************************************************************************
*
* m68681BaudSet - change baud rate for channel
*
* This routine sets the baud rate for the DUART. The interrupts are disabled
* during chip access.
*
* RETURNS:
* Returns a status of OK if the baud rate was set else ERROR.
*/

LOCAL STATUS  m68681BaudSet
    (
    M68681_CHAN * pChan,
    UINT	baud
    )
    {
    int ix, oldlevel = intLock ();
    STATUS status = ERROR;

    for (ix = 0; ix < NELEMENTS (baudTable); ix ++)
        {
	if (baudTable [ix].rate == baud)
	    {
	    m68681AcrSetClr (pChan->pDuart, baudTable[ix].baudBit,
			    M68681_ACR_BRG_SELECT);
	    M68681_WRITE (pChan->csr, baudTable [ix].csrVal);
	    pChan->baudRate = baud;
	    status = OK;
	    break;
	    }
        }
    intUnlock (oldlevel);

    return (status);
    }

/*******************************************************************************
*
* m68681InitStruct - initializes the channel structure
*
* This routine initializes the specified channels driver functions.
*
* RETURNS: N/A.
*/

LOCAL void m68681InitStruct
    (
    M68681_CHAN * pChan
    )
    {
    pChan->sio.pDrvFuncs= &m68681SioDrvFuncs;

    pChan->getTxChar	= m68681DummyCallback;
    pChan->putRcvChar	= m68681DummyCallback;
    pChan->mode		= 0; /* undefined */

    if (pChan->options == 0 || pChan->options > MAX_OPTIONS)
	pChan->options = M68681_DEFAULT_OPTIONS;

    if (pChan->baudRate == 0 || pChan->baudRate > MAX_BAUD)
	pChan->baudRate = M68681_DEFAULT_BAUD;
    }

/*******************************************************************************
*
* m68681InitChannel - initialize a single channel
*
* This routine initializes the specified channel.  It is required
* that the transmitters and receivers have been issued s/w reset commands
* before the mode registers, clock select registers, auxillary clock
* registers & output port configuration registers are changed.
*
* This routine only sets the initial state as part of m68681DevInit.  The
* user can change this state through ioct clommands as desired.
*
* RETURNS: N/A.
*/

LOCAL void m68681InitChannel
    (
    M68681_CHAN * pChan,
    M68681_DUART * pDuart
    )
    {
    /* Reset the transmitters  & receivers  */

    M68681_WRITE (pChan->cr, M68681_CR_RST_BRK_INT_CMD);
    M68681_WRITE (pChan->cr, M68681_CR_RST_ERR_STS_CMD);

    m68681OptsSet (pChan, pChan->options); /* TX is disabled */

    m68681BaudSet (pChan, pChan->baudRate);
    }

/*******************************************************************************
*
* m68681OptsSet - set the serial options
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

LOCAL STATUS m68681OptsSet
    (
    M68681_CHAN * pChan, /* ptr to channel */
    UINT options	/* new hardware options */
    )
    {
    int mr1Value = 0;
    int mr2Value = 0;
    int lvl;

    if (pChan == NULL || options & 0xffffff00)
	return ERROR;

    /* Reset the transmitters  & receivers  */

    switch (options & CSIZE)
	{
	case CS5:
	    mr1Value = M68681_MR1_BITS_CHAR_5; break;
	case CS6:
	    mr1Value = M68681_MR1_BITS_CHAR_6; break;
	case CS7:
	    mr1Value = M68681_MR1_BITS_CHAR_7; break;
	default:
	case CS8:
	    mr1Value = M68681_MR1_BITS_CHAR_8; break;
	}

    if (options & STOPB)
	mr2Value = M68681_MR2_STOP_BITS_2;
    else
	mr2Value = M68681_MR2_STOP_BITS_1;

    switch (options & (PARENB|PARODD))
	{
	case PARENB|PARODD:
	    mr1Value |= M68681_MR1_ODD_PARITY; break;
	case PARENB:
	    mr1Value |= M68681_MR1_EVEN_PARITY; break;
	case PARODD:
	    mr1Value |= M68681_MR1_PAR_MODE_MULTI; break;
	default:
	case 0:
	    mr1Value |= M68681_MR1_NO_PARITY; break;
	}

    if (!(options & CLOCAL))
	{
	/* clocal disables hardware flow control */
	mr1Value |= M68681_MR1_RX_RTS;
	mr2Value |= M68681_MR2_TX_CTS;
	}

    lvl = intLock ();

    /* now reset the channel mode registers */

    M68681_WRITE (pChan->cr, M68681_CR_RST_MR_PTR_CMD | M68681_CR_RX_DISABLE |
                      M68681_CR_TX_DISABLE);
    M68681_WRITE (pChan->cr, M68681_CR_RST_TX_CMD);
    M68681_WRITE (pChan->cr, M68681_CR_RST_RX_CMD);

    M68681_WRITE (pChan->mr, mr1Value);  /* mode register 1  */
    M68681_WRITE (pChan->mr, mr2Value);  /* mode register 2  */

    if (options & CREAD)
	M68681_WRITE (pChan->cr, M68681_CR_RX_ENABLE);

    intUnlock (lvl);

    pChan->options = options;

    return OK;
    }

/*******************************************************************************
*
* m68681Ioctl - special device control
*
* RETURNS:
* Returns OK on success, EIO on device error, ENOSYS on unsupported
* request.
*/

LOCAL int m68681Ioctl
    (
    M68681_CHAN *	pChan,		/* device to control */
    int		request,		/* request code */
    void *	someArg			/* some argument */
    )
    {
    STATUS result;
    int     arg = (int)someArg;

    switch (request)
	{
	case SIO_BAUD_SET:
	    return (m68681BaudSet (pChan, arg) == OK ? OK : EIO);

	case SIO_BAUD_GET:
	    *(int *)arg = pChan->baudRate;
	    return (OK);

	case SIO_MODE_SET:
	    return (m68681ModeSet (pChan, arg) == OK ? OK : EIO);

	case SIO_MODE_GET:
	    *(int *)arg = pChan->mode;
	    return (OK);

	case SIO_AVAIL_MODES_GET:
	    *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    return (OK);

	case SIO_HW_OPTS_SET:
	    /* change options, then set mode to restart chip correctly */
	    result = m68681OptsSet (pChan, arg);
	    m68681ModeSet (pChan, pChan->mode);
	    return result;

	case SIO_HW_OPTS_GET:
	    *(int *)arg = pChan->options;
	    return (OK);

	default:
	    return (ENOSYS);
	}
    }


/*******************************************************************************
*
* m68681IntWr - handle a transmitter interrupt
*
* This routine handles write interrupts from the DUART. This isr is invoked
* when the TxRDY bit in the interrupt status register has been set. If there
* is no character to transmit the transmitter for the channel is disabled.
*
* RETURNS: N/A
*/

LOCAL void m68681IntWr
    (
    M68681_CHAN * pChan
    )
    {
    char            outChar;
    volatile UCHAR  *tb = pChan->tb;

    if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)
	{
	M68681_WRITE (tb, outChar);	   /* if char available, tx it */
	}
    else
        {
	M68681_WRITE (pChan->cr, M68681_CR_TX_DISABLE);
        }
    }

/*****************************************************************************
*
* m68681IntRd - handle a reciever interrupt
*
* This routine handles read interrupts from the DUART.
* The DUART has been programmed to generate read interrupts when the RXRDY
* status bit has been set in the interrupt status register. When a character
* has been  received it is removed from the channels receive buffer.
*
* RETURNS: N/A
*/

LOCAL void m68681IntRd
    (
    M68681_CHAN * pChan
    )
    {
    volatile UCHAR  *rb = pChan->rb;
    UCHAR            inchar;
    char             statusReg;

    statusReg = M68681_READ (pChan->sr);

    while ((statusReg & M68681_SR_RXRDY) != 0x00)
        {
	inchar = M68681_READ (rb);
	(*pChan->putRcvChar) (pChan->putRcvArg, inchar);
	if (pChan->mode != SIO_MODE_INT)
	    break;
	statusReg = M68681_READ (pChan->sr);
        }
    }


/*******************************************************************************
*
* m68681Int - handle all DUART interrupts in one vector
*
* This routine handles all interrupts in a single interrupt vector.
* It identifies and services each interrupting source in turn, using
* edge-sensitive interrupt controllers.
*
* RETURNS: N/A
*/

void m68681Int
    (
    M68681_DUART * pDuart
    )
    {
    UCHAR        	intStatus;
    volatile UCHAR      resetTimer;

    /* loop until all sources have been handled */

    while ((intStatus = (M68681_READ(pDuart->isr) & pDuart->imrCopy)) != 0)
	{
	if ((intStatus & M68681_ISR_TX_RDY_A_INT) != 0)
	    m68681IntWr (&pDuart->portA);

	if ((intStatus & M68681_ISR_RX_RDY_A_INT) != 0)
	    m68681IntRd (&pDuart->portA);

	if ((intStatus & M68681_ISR_TX_RDY_B_INT) != 0)
	    m68681IntWr (&pDuart->portB);
     
	if ((intStatus & M68681_ISR_RX_RDY_B_INT) != 0)
	    m68681IntRd (&pDuart->portB);

	if (intStatus & M68681_ISR_CTR_RDY_INT)
	    {
	    if (pDuart->tickRtn != NULL)
		(*pDuart->tickRtn) (pDuart->tickArg);
	    else
		/* Clear the timer int */
		resetTimer = M68681_READ (pDuart->ctroff);
	    }
	}
    }
