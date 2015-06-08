/* ppc555SciSio.c - MPC555 SCI serial driver */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,27apr99,zl   fixed mode set to allow switch between polled and intr mode
01b,19apr99,zl   added support for queued mode, set default baud rate
01a,22mar99,zl   written based on m68332Sio.c
*/

/*
DESCRIPTION
This is the driver for SCIs of the QSMC the the Motorola PPC555.  The SMC has 
two SCI channels. Both channels are compatible with earlier SCI devices from
Motorola (eg. MC68332), with enhancements to allow external baud clock source
and queued operation fro the first SCI channel. 

.SH DATA STRUCTURES
An PPC555SCI_CHAN data structure is used to describe each channel, this
structure is described in h/drv/sio/ppc555SciSio.h. Based on the "options" 
field of this structure, the driver can work in queued or non-queued mode.
Only the first SCI of the QSMC on the PowerPC 555 provides queued mode
operation.

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
The BSP's sysHwInit() routine typically calls sysSerialHwInit(),
which initializes all the values in the PPC555SCI_CHAN structure (except
the SIO_DRV_FUNCS) before calling m68332DevInit().

The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(), which
connects the chips interrupt (m68332Int) via intConnect().

.SH INCLUDE FILES:
drv/sio/ppc555SciSio.h sioLib.h

.SH SEE ALSO:
.I "Section 14 QUEUED SERIAL MULTI-CHANNEL MODULE, MPC555 User’s Manual"
*/

#include "vxWorks.h"
#include "sioLib.h"
#include "intLib.h"
#include "errno.h"
#include "drv/sio/ppc555SciSio.h"

#define PPC555SCI_BAUD_MIN		75
#define PPC555SCI_BAUD_MAX		115200

#ifndef PPC555SCI_DEFAULT_BAUD
#    define PPC555SCI_DEFAULT_BAUD	9600
#endif

/* Hardware abstraction macros */

#ifndef PPC555SCI_REG_READ
#   define PPC555SCI_REG_READ(pChan, reg, result) \
	((result) = *(pChan->reg))
#endif /*PPC555SCI_REG_READ*/

#ifndef PPC555SCI_REG_WRITE
#   define PPC555SCI_REG_WRITE(pChan, reg, data) \
	(*(pChan->reg) = data)
#endif /*PPC555SCI_REG_WRITE*/

#ifndef PPC555SCI_SCTQ
#   define PPC555SCI_SCTQ(pChan) \
	(pChan->sctq)
#endif /*PPC555SCI_SCTQ*/

#ifndef PPC555SCI_SCRQ
#   define PPC555SCI_SCRQ(pChan) \
	(pChan->scrq)
#endif /*PPC555SCI_SCRQ*/

/* forward declarations */
LOCAL STATUS 	dummyCallback (void);
LOCAL int	ppc555SciIoctl (SIO_CHAN *pSioChan, int request, void *arg);
LOCAL int	ppc555SciTxStartup (SIO_CHAN * pSioChan);
LOCAL int	ppc555SciCallbackInstall (SIO_CHAN *pSioChan, int callbackType,
				    STATUS (*callback)(), void *callbackArg);
LOCAL int	ppc555SciPollOutput (SIO_CHAN *pSioChan, char	outChar);
LOCAL int	ppc555SciPollInput (SIO_CHAN *pSioChan, char *thisChar);
LOCAL void 	ppc555SciIntQueued (PPC555SCI_CHAN *	pChan);
LOCAL void 	ppc555SciIntNotQueued (PPC555SCI_CHAN *	pChan);


/* local variables */

LOCAL	SIO_DRV_FUNCS ppc555SciSioDrvFuncs =
    {
    ppc555SciIoctl,
    ppc555SciTxStartup,
    ppc555SciCallbackInstall,
    ppc555SciPollInput,
    ppc555SciPollOutput
    };

LOCAL BOOL ppc555SciIntrMode = FALSE;	/* interrupt mode allowed flag */

/******************************************************************************
*
* ppc555SciDevInit - initialize a PPC555SCI channel
*
* This routine initializes the driver
* function pointers and then resets the chip in a quiescent state.
* The BSP must have already initialized all the device addresses and the
* baudFreq fields in the PPC555SCI_CHAN structure before passing it to
* this routine.
*
* RETURNS: N/A
*/

void ppc555SciDevInit
    (
    PPC555SCI_CHAN * pChan
    )
    {
    /* initialize the channel's driver function pointers */

    pChan->sio.pDrvFuncs	= &ppc555SciSioDrvFuncs;

    /* install dummy driver callbacks */

    pChan->getTxChar    = dummyCallback;
    pChan->putRcvChar	= dummyCallback;

    ppc555SciIoctl ((SIO_CHAN *) pChan, SIO_BAUD_SET, 
                    (void *) PPC555SCI_DEFAULT_BAUD);

    /* set interrupt level */

    PPC555SCI_REG_WRITE (pChan, scilr, (pChan->intLevel) << QSM_QILR_SCI_SHIFT);

    /* clear control registers 0 and 1 */

    PPC555SCI_REG_WRITE(pChan, sccr0, 0);
    PPC555SCI_REG_WRITE(pChan, sccr1, 0);
    }

/******************************************************************************
*
* ppc555SciDevInit2 - initialize a PPC555SCI, part 2
*
* This routine is called by the BSP after interrupts have been connected.
* The driver can now operate in interrupt mode.  Before this routine is
* called only polled mode operations should be allowed.
*
* RETURNS: N/A
*/

void ppc555SciDevInit2
    (
    PPC555SCI_CHAN * pChan	/* device to initialize */
    )
    {

    /* Interrupt mode is allowed */

    ppc555SciIntrMode = TRUE;

    }

/******************************************************************************
*
* ppc555SciInt - handle a channel's interrupt
*
* RETURNS: N/A
*/ 

void ppc555SciInt
    (
    PPC555SCI_CHAN *	pChan		/* channel generating the interrupt */
    )
    {
    if (pChan->options & QUEUED)
	ppc555SciIntQueued(pChan);
    else
	ppc555SciIntNotQueued(pChan);
    }

/******************************************************************************
*
* ppc555SciIntNotQueued - handle a channel's interrupt in not queued mode
*
* RETURNS: N/A
*/ 

LOCAL void ppc555SciIntNotQueued
    (
    PPC555SCI_CHAN *	pChan		/* channel generating the interrupt */
    )
    {
    char	ioChar;
    FAST UINT16	status;
    FAST UINT16 temp;

    /* Check for receive */

    PPC555SCI_REG_READ(pChan, scsr, status);

    if (status & QSM_SCSR_RDRF)
        {
	PPC555SCI_REG_READ(pChan, scdr, ioChar);
        (*pChan->putRcvChar) (pChan->putRcvArg, ioChar);
        }
 
    /* Check for error conditions and clear them with dummy read */

    if (status & (QSM_SCSR_OR | QSM_SCSR_NF | QSM_SCSR_FE | QSM_SCSR_PF))
	{
	PPC555SCI_REG_READ(pChan, scdr, ioChar);
	}

    /* Check for transmit */

    if (status & QSM_SCSR_TDRE)
	{
	
	/* check if there is more to send */

	if ((*pChan->getTxChar) (pChan->getTxArg, &ioChar) == OK)
	    {
	    PPC555SCI_REG_WRITE(pChan, scdr, ioChar);
	    }
	else
	    {
            /* disable transmit interrupts */

	    PPC555SCI_REG_READ(pChan, sccr1, temp);
	    temp &= ~QSM_SCCR1_TIE;
	    PPC555SCI_REG_WRITE(pChan, sccr1, temp);
	    }
        }

    }

/******************************************************************************
*
* ppc555SciIntQueued - handle a channel's interrupt in queued mode
*
* RETURNS: N/A
*/ 

LOCAL void ppc555SciIntQueued
    (
    PPC555SCI_CHAN *	pChan		/* channel generating the interrupt */
    )
    {
    char	ioChar;
    FAST UINT16	scsrVal;
    FAST UINT16	qscsrVal;
    FAST UINT16 temp;
    FAST UINT16 byteCount;

    /* read SCI status registers */

    PPC555SCI_REG_READ(pChan, qscsr, qscsrVal);
    PPC555SCI_REG_READ(pChan, scsr,  scsrVal);

    /* check for receive top half */

    if (qscsrVal & QSM_QSCSR_QTHF)
        {
	/* read data from queue */

	for (byteCount=0; byteCount < 8; byteCount++)
	    {
	    ioChar = PPC555SCI_SCRQ(pChan)[byteCount];
	    (*pChan->putRcvChar) (pChan->putRcvArg, ioChar);
	    }

	/* clear "top half full" */

	qscsrVal &= ~QSM_QSCSR_QTHF;
	}

    /* check for receive bottom half */

    else if (qscsrVal & QSM_QSCSR_QBHF)
        {
	/* read data from queue */

	for (byteCount=0; byteCount < 8; byteCount++)
	    {
	    ioChar = PPC555SCI_SCRQ(pChan)[byteCount + 8];
	    (*pChan->putRcvChar) (pChan->putRcvArg, ioChar);
	    }

	/* clear "bottom half full" */

	qscsrVal &= ~QSM_QSCSR_QBHF;
	}

    /* Check for idle */

    if (scsrVal & QSM_SCSR_IDLE)
        {
	/* read data from queue */
	
	temp = (qscsrVal & QSM_QSCSR_QRPNT_MSK) >> 4;

	if ((temp == 0) || (temp == 8))
	    {
	    /* need a dummy read to clear IDLE */
	    
	    ioChar = PPC555SCI_SCRQ(pChan)[0];
	    }
	else if (temp < 8)
	    {
	    /* read top half only */

	    for (byteCount=0; byteCount < temp; byteCount++)
		{
		ioChar = PPC555SCI_SCRQ(pChan)[byteCount];
		(*pChan->putRcvChar) (pChan->putRcvArg, ioChar);
		}
	    }
	else 
	    {
	    /* read bottom half only */

	    for (byteCount=8; byteCount < temp; byteCount++)
		{
		ioChar = PPC555SCI_SCRQ(pChan)[byteCount];
		(*pChan->putRcvChar) (pChan->putRcvArg, ioChar);
		}
	    }

	/* temporarily disable receive */

	PPC555SCI_REG_READ(pChan, sccr1, temp);
	temp &= ~QSM_SCCR1_RE;
	PPC555SCI_REG_WRITE(pChan, sccr1, temp);
	}

    /* check for transmit complete */

    if ((scsrVal & QSM_SCSR_TC) && (qscsrVal & QSM_QSCSR_QTHE) && 
        (qscsrVal & QSM_QSCSR_QBHE))
	{
	/* check if there is more to send */

	for (byteCount=0; byteCount < 16; byteCount++)
	    {
	    if ((*pChan->getTxChar) (pChan->getTxArg, &ioChar) == OK)
		PPC555SCI_SCTQ(pChan)[byteCount] = ioChar;
	    else
		break;
	    }
	if (byteCount > 0)
	    {
	    /* Temporarily disable transmission */
    
	    PPC555SCI_REG_READ(pChan, sccr1, temp);
	    temp &= ~QSM_SCCR1_TE;
	    PPC555SCI_REG_WRITE(pChan, sccr1, temp);

	    /* set QTE bit and queue size */
    
	    PPC555SCI_REG_READ(pChan, qsccr, temp);
	    temp &= ~QSM_QSCCR_QTSZ_MSK;
	    temp |= QSM_QSCCR_QTE;
	    temp |= (byteCount-1);
	    PPC555SCI_REG_WRITE(pChan, qsccr, temp);

	    /* clear top half empty and bottom half empty if needed */

	    if (byteCount > 8)
		qscsrVal &= ~(QSM_QSCSR_QTHE | QSM_QSCSR_QBHE);
	    else
	        qscsrVal &= ~QSM_QSCSR_QTHE;
	    }
	else
	    {
	    /* disable transmit interrupts */

	    PPC555SCI_REG_READ(pChan, sccr1, temp);
	    temp &= ~QSM_SCCR1_TCIE;
	    PPC555SCI_REG_WRITE(pChan, sccr1, temp);
	    }
	}

    /* update queued SCI status register. Clear overrun, just in case */

    qscsrVal &= ~QSM_QSCSR_QOR;
    PPC555SCI_REG_WRITE(pChan, qscsr, qscsrVal);

    /* clear error conditions with dummy reads */

    PPC555SCI_REG_READ(pChan, scsr,  scsrVal);
    if (scsrVal & (QSM_SCSR_OR | QSM_SCSR_NF | QSM_SCSR_FE | QSM_SCSR_PF))
	{
	PPC555SCI_REG_READ(pChan,  scdr,  temp);
	ioChar = PPC555SCI_SCRQ(pChan)[0];
	}
	

    /* 
     * Make sure receive is enabled because error conditions and idle line
     * clear these; re-enable transmit.
     */

    PPC555SCI_REG_READ(pChan, qsccr, temp);
    temp |= QSM_QSCCR_QRE;
    PPC555SCI_REG_WRITE(pChan, qsccr, temp);

    PPC555SCI_REG_READ(pChan, sccr1, temp);
    temp |= (QSM_SCCR1_RE | QSM_SCCR1_TE);
    PPC555SCI_REG_WRITE(pChan, sccr1, temp);
    }

/******************************************************************************
*
* ppc555SciTxStartup - start the interrupt transmitter
*
* RETURNS: OK on success, ENOSYS if the device is polled-only.
*/

LOCAL int ppc555SciTxStartup
    (
    SIO_CHAN * pSioChan                 /* channel to start */
    )
    {
    PPC555SCI_CHAN * pChan = (PPC555SCI_CHAN *)pSioChan;
    char	outChar;
    FAST UINT16	status;
    FAST UINT16 temp;

    if (pChan->mode == SIO_MODE_INT)
	{
	if (pChan->options & QUEUED)
	    {
	    PPC555SCI_REG_READ(pChan, sccr1, temp);
	    temp |= QSM_SCCR1_TCIE;
	    PPC555SCI_REG_WRITE(pChan, sccr1, temp);
	    }
	else
	    {
	    PPC555SCI_REG_READ(pChan, scsr, status);

	    /* send if transmit register empty and have character to send */

	    if ((status & QSM_SCSR_TDRE) &&
	        ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK))
		{
	        PPC555SCI_REG_WRITE(pChan, scdr, outChar);

        	/* enable transmit interrupts */

		PPC555SCI_REG_READ(pChan, sccr1, temp);
		temp |= QSM_SCCR1_TIE;
		PPC555SCI_REG_WRITE(pChan, sccr1, temp);
		}
	    }
	return (OK);
	}
    else
	return (ENOSYS);
    }


/******************************************************************************
*
* ppc555SciCallbackInstall - install ISR callbacks to get/put chars
*
* This driver allows interrupt callbacks for transmitting characters
* and receiving characters. In general, drivers may support other
* types of callbacks too.
*
* RETURNS: OK on success, or ENOSYS for an unsupported callback type.
*/ 

LOCAL int ppc555SciCallbackInstall
    (
    SIO_CHAN *	pSioChan,               /* channel */
    int		callbackType,           /* type of callback */
    STATUS	(*callback)(),          /* callback */
    void *      callbackArg             /* parameter to callback */
    )
    {
    PPC555SCI_CHAN * pChan = (PPC555SCI_CHAN *)pSioChan;

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

/*******************************************************************************
*
* ppc555SciPollOutput - output a character in polled mode
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
* if the output buffer if full. ENOSYS if the device is
* interrupt-only.
*/

LOCAL int ppc555SciPollOutput
    (
    SIO_CHAN *	pSioChan,
    char	outChar
    )
    {
    PPC555SCI_CHAN * pChan = (PPC555SCI_CHAN *)pSioChan;
    UINT16	status;

    /* is the transmitter ready to accept a character? */

    PPC555SCI_REG_READ(pChan, scsr, status);
    if ((status & QSM_SCSR_TDRE) == 0x00)
	return (EAGAIN);

    /* write out the character */

    PPC555SCI_REG_WRITE(pChan, scdr, outChar);

    return (OK);
    }

/******************************************************************************
*
* ppc555SciPollInput - poll the device for input
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
* if the input buffer if empty, ENOSYS if the device is
* interrupt-only.
*/

LOCAL int ppc555SciPollInput
    (
    SIO_CHAN *	pSioChan,
    char *	thisChar
    )
    {
    PPC555SCI_CHAN * pChan = (PPC555SCI_CHAN *)pSioChan;
    UINT16	status;

    PPC555SCI_REG_READ(pChan, scsr, status);
    if ((status & QSM_SCSR_RDRF) == 0x00)
	return (EAGAIN);	/* no input available at this time */

    /* got a character */

    PPC555SCI_REG_READ(pChan, scdr, *thisChar);

    return (OK);
    }

/******************************************************************************
*
* ppc555SciModeSet - toggle between interrupt and polled mode
*
* RETURNS: OK on success, EIO on unsupported mode.
*/

LOCAL int ppc555SciModeSet
    (
    PPC555SCI_CHAN * pChan,		/* channel */
    uint_t	    newMode		/* new mode */
    )
    {
    FAST UINT16	temp;
    FAST int	oldlevel;		/* current interrupt level mask */

    if ((newMode != SIO_MODE_POLL) && (newMode != SIO_MODE_INT))
	return (EIO);

    /* Don't enter interrupt mode unless it is allowed. */

    if ((newMode == SIO_MODE_INT) && (!ppc555SciIntrMode))
	return (EIO);

    oldlevel = intLock();

    /* set the new mode */

    pChan->mode = newMode;

    if (pChan->mode == SIO_MODE_INT)
	if (pChan->options & QUEUED)
	    {
	    /* enable SCI in queued mode */

	    temp = (QSM_QSCCR_QTHFI | QSM_QSCCR_QBHFI | 
	            QSM_QSCCR_QTE   | QSM_QSCCR_QRE);
	    PPC555SCI_REG_WRITE(pChan, qsccr, temp);

	    /* enable Rx, Tx and enable idle line detection int */

	    temp = (QSM_SCCR1_RE   | QSM_SCCR1_TE | QSM_SCCR1_TCIE |
	            QSM_SCCR1_ILIE | QSM_SCCR1_ILT);
	    PPC555SCI_REG_WRITE(pChan, sccr1, temp);
	    }
	else
	    {
	    /* enable SCI receive interrupts */
	
	    temp = QSM_SCCR1_RE | QSM_SCCR1_TE | QSM_SCCR1_RIE;
	    PPC555SCI_REG_WRITE(pChan, sccr1, temp);
	    }

    else /* polled mode */
	if (pChan->options & QUEUED)
	    {
	    /* polled mode is not queued */
	
	    PPC555SCI_REG_WRITE(pChan, qsccr, 0);
	    PPC555SCI_REG_WRITE(pChan, sccr1, (QSM_SCCR1_RE | QSM_SCCR1_TE));
	    }
	else
	    {
	    /* enable rx and tx without interrupts */
	
	    PPC555SCI_REG_WRITE(pChan, sccr1, (QSM_SCCR1_RE | QSM_SCCR1_TE));
	    }

    intUnlock(oldlevel);

    return (OK);
    }

/*******************************************************************************
*
* ppc555SciIoctl - special device control
*
* This routine handles the IOCTL messages from the user. It supports commands 
* to get/set baud rate, mode(INT,POLL).
*
* RETURNS: OK on success, ENOSYS on unsupported request, EIO on failed
* request.
*/

LOCAL int ppc555SciIoctl
    (
    SIO_CHAN *	pSioChan,		/* device to control */
    int		request,		/* request code */
    void *	someArg			/* some argument */
    )
    {
    PPC555SCI_CHAN *pChan = (PPC555SCI_CHAN *) pSioChan;
    int	baudScratch;
    int arg = (int)someArg;
    FAST int     oldlevel;		/* current interrupt level mask */

    switch (request)
	{
	case SIO_BAUD_SET:
	    if (arg < PPC555SCI_BAUD_MIN || arg > PPC555SCI_BAUD_MAX)
	        {
		return (EIO);
	        }

	    baudScratch = (pChan->clockRate + 16 * arg) / (32 * arg);
	    if ((baudScratch < 1) && (baudScratch > 8191))
	        {
		return (EIO);
	        }

	    oldlevel = intLock();
	    PPC555SCI_REG_WRITE(pChan, sccr0, baudScratch);
	    intUnlock (oldlevel);

	    pChan->baudRate = arg;
	    
            return (OK);

	case SIO_BAUD_GET:

	    *(int *)arg = pChan->baudRate;

            return (OK);

	case SIO_MODE_SET:

	    /*
	     * Set the mode (e.g., to interrupt or polled). Return OK
	     * or EIO for an unknown or unsupported mode.
	     */

	    return (ppc555SciModeSet (pChan, arg));

	case SIO_MODE_GET:

	    /* Get the current mode and return OK.  */

	    *(int *)arg = pChan->mode;
	    return (OK);

	case SIO_AVAIL_MODES_GET:

	    /* Get the available modes and return OK.  */

	    *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
	    return (OK);

	default:
	    return (ENOSYS);
	}
    }

/*******************************************************************************
*
* dummyCallback - dummy callback routine
*
* RETURNS: ERROR.
*/

LOCAL STATUS dummyCallback (void)
    {
    return (ERROR);
    }
