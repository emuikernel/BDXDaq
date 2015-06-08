/* unixSio.c - unix serial driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,30oct01,jmp  optimized unixIntRcv() routine.
		 moved from src/drv/sio to solaris bsp.
01d,18oct01,dat  Documentation fixes
01c,16jul97,jmb  Modify unixIntRcv to do multiple reads so that
		 receive buffer can be smaller than pipe.
01b,10jul97,jmb  Modify unixIntRcv to read all data at once, rather than
		 reading one character at a time.
01a,28mar97,kab  written for SIMHPPA.
*/

/*
DESCRIPTION
This is the driver for the UNIX stdin/stdio-base simulated serial port.

USAGE
A UNIX_CHAN structure is used to describe each channel available.

The BSP's sysHwInit() routine typically calls sysSerial.c:sysSerialHwInit(),
which opens unix tty/pty devices for serial lines & initializes the
UNIX_CHAN u_fd & u_pid fields before calling unixDevInit().

The BSP sysSerialHwInit2() calls unixDevInit2() to enable interrupts.

.CS
i.e.

#include "unixSio.h"

UNIX_CHAN myChan [NUM_TTY];
SIO_CHAN * sysSioChans[NUM_TTY];

sysSerialHwInit (void)
    {
    ...
    for (ix = 0; ix < NUM_TTY; ix++)
        {        
        if (ix > 0)     // dev 0 is unix sdtin/out/err //
	    {
	    UNIX_CHAN * pChan = &myChan[ix];
	    sysSioChans[ix] = (SIO_CHAN *) pChan;
	    pChan->u_fd = ptyXtermOpen (ptyName, &pChan->u_pid, 0);
	    }
        ...
        unixDevInit (&myChan);
        }
    }

sysSerialHwInit2 (void)
    {
    ...
    for (i = 0; i < NUM_TTY; i++)
        intConnect (FD_TO_IVEC(myChan[i]->u_fd), unixInt, (int)&myChan[i]);
    ...
    }
.CE


INCLUDE FILES: unixSio.h sioLib.h
*/

#include "vxWorks.h"
#include "sioLib.h"
#include "intLib.h"
#include "errno.h"
#include "selectLib.h"
#include "sys/times.h"
#include "u_Lib.h"
#include "simLib.h"
#include "unixSio.h"

extern void    bzero ();

/* forward static declarations */

static int unixTxStartup (SIO_CHAN * pSioChan);
static int unixCallbackInstall (SIO_CHAN *pSioChan, int callbackType,
				STATUS (*callback)(), void *callbackArg);
static int unixPollOutput (SIO_CHAN *pSioChan, char outChar);
static int unixPollInput (SIO_CHAN *pSioChan, char *thisChar);
static int unixIoctl (SIO_CHAN *pSioChan, int request, void *arg);
static int unixModeSet (SIO_CHAN *pSioChan, uint_t newMode);
static STATUS dummyCallback (void);

/* local variables */

static SIO_DRV_FUNCS unixSioDrvFuncs =
    {
    (int (*)(SIO_CHAN *, int, void *)) unixIoctl,
    (int (*)(SIO_CHAN *)) unixTxStartup,
    (int (*)()) unixCallbackInstall,
    (int (*)(SIO_CHAN *, char *)) unixPollInput,
    (int (*)(SIO_CHAN *, char)) unixPollOutput
    };

#ifndef BUFSIZE
#define BUFSIZE 2048
#endif

/*
 *  Receive entire contents of pipe, since PPL's pipe is only
 *  760 characters.
 */
#define SIO_RCVBUF_SIZE 1024

/******************************************************************************
*
* unixDevInit - initialize a UNIX_DUSART
*
* This routine initializes the driver function pointers and then
* resets to a quiescent state.  The BSP must have already opened all
* the file descriptors in the structure before passing it to this
* routine.
*
* RETURNS: N/A
*/

void unixDevInit
    (
    UNIX_CHAN * pChan
    )
    {
    /* initialize each channel's driver function pointers */
    pChan->sio.pDrvFuncs	= &unixSioDrvFuncs;

    /* install dummy driver callbacks */
    pChan->getTxChar		= dummyCallback;
    pChan->putRcvChar		= dummyCallback;
    
    /* setting polled mode is one way to make the device quiet */
    pChan->mode			= 0;	/* not valid */
    unixModeSet ((SIO_CHAN *) pChan, SIO_MODE_POLL);

    pChan->created		= 1;
    }


/******************************************************************************
*
* unixDevInit2 - enable interrupts
*
* If called at sysHwInit2 time, this routine will enable the device to
* operate in interrupt mode.  If this routine is not called, the device
* will be limited to polled mode only.
*
* RETURNS: N/A
*/

void unixDevInit2
    (
    UNIX_CHAN * pChan
    )
    {
    if ((pChan != NULL) && (pChan->created != 1))
	unixModeSet ((SIO_CHAN *) pChan, SIO_MODE_INT);
    }
    
/******************************************************************************
*
* unixIntRcv - handle a channel's receive-character interrupt
*
* This is the receive interrupt service routine.
*
* RETURNS: N/A
*/ 

void unixIntRcv
    (
    UNIX_CHAN *	pChan		/* channel generating the interrupt */
    )
    {
    char buf[SIO_RCVBUF_SIZE];
    int nread;
    int ix;

    /*
     *  Read characters into buffer, then pass them one at a time
     *  to tyIRd.
     */

    while ((nread = u_read (pChan->u_fd, buf, SIO_RCVBUF_SIZE)) > 0)
	{
        for (ix = 0; ix < nread; ix++)
	    {
	    (*pChan->putRcvChar) (pChan->putRcvArg, buf[ix]);
	    }
        }
    }

/******************************************************************************
*
* unixTxStartup - start the interrupt transmitter
*
* This routine will start the transmitter.  Transmitter interrupts will
* fetch characters until the buffer is empty.
*
* RETURNS: 
* Returns OK on success, ENOSYS if the device is polled-only, or
* EIO on hardware error.
*/

static int unixTxStartup
    (
    SIO_CHAN * pSioChan                 /* channel to start */
    )
    {
    UNIX_CHAN * pChan = (UNIX_CHAN *) pSioChan;
    char buf [BUFSIZE], * pbuf;
    int writeFd;
    int t, tt;
    int ix;

    if (pChan == NULL)
	return (EIO);
    writeFd = pChan->u_fd;
    if (writeFd == 0)
	writeFd = 1;

    do
	{
	for (ix = 0; ix < sizeof (buf); ix++)
	    {
	    if ((*pChan->getTxChar) (pChan->getTxArg, &buf[ix]) != OK)
		{
		break;
		}
	    }
	pbuf = buf;
	tt = ix;

	if (tt != 0)
	    {
	    while ((t = u_write (writeFd, pbuf, tt)) != tt)
		if (t >= 0)
		    {
		    tt -= t;
		    pbuf += t;
		    }
		else
		    return (EIO);
	    }
	}
    while (ix == sizeof (buf));
    return (OK);
    }

/******************************************************************************
*
* unixCallbackInstall - install ISR callbacks to get/put chars
*
* This driver allows interrupt callbacks for transmitting characters
* and receiving characters. In general, drivers may support other
* types of callbacks too.
*
* RETURNS: OK on success, or ENOSYS for an unsupported callback type.
*/ 

static int unixCallbackInstall
    (
    SIO_CHAN *	pSioChan,               /* channel */
    int		callbackType,           /* type of callback */
    STATUS	(*callback)(),          /* callback */
    void *      callbackArg             /* parameter to callback */
    )
    {
    UNIX_CHAN * pChan = (UNIX_CHAN *) pSioChan;

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
* unixPollOutput - output a character in polled mode
*
* RETURNS: 
* Returns OK if a character arrived, EIO on device error, EAGAIN
* if the output buffer if full. ENOSYS if the device is
* interrupt-only.
*/

static int unixPollOutput
    (
    SIO_CHAN *	pSioChan,
    char	outChar
    )
    {
    UNIX_CHAN * pChan = (UNIX_CHAN *) pSioChan;
    int ret = 0;

    ret = u_write (pChan->u_fd, &outChar, 1);
    if (ret == -1)
	return (EIO);
    else if (ret == 0)
	return (EAGAIN);

    return (OK);
    }

/******************************************************************************
*
* unixPollInput - poll the device for input
*
* RETURNS:
* Returns OK if a character arrived, EIO on device error, EAGAIN
* if the input buffer if empty, ENOSYS if the device is
* interrupt-only.
*/

static int unixPollInput
    (
    SIO_CHAN *	pSioChan,
    char *	pChar
    )
    {
    UNIX_CHAN * pChan = (UNIX_CHAN *) pSioChan;
    int ret;

    if ((ret = u_read (pChan->u_fd, pChar, 1)) == -1)
	return (EIO);
    else if (ret == 0)
	return (EAGAIN);

    return (OK);
    }

/******************************************************************************
*
* unixModeSet - toggle between interrupt and polled mode
*
* RETURNS: OK on success, EIO on unsupported mode.
*/

static int unixModeSet
    (
    SIO_CHAN *  pSioChan,	/* channel */
    uint_t	newMode		/* new mode */
    )
    {
    UNIX_CHAN * pChan = (UNIX_CHAN *) pSioChan;

    if ((newMode != SIO_MODE_POLL) && (newMode != SIO_MODE_INT))
	return (EIO);

    pChan->mode = newMode;
    
    return (OK);
    }

/*******************************************************************************
*
* unixIoctl - special device control
*
* RETURNS: 
* Returns OK on success, ENOSYS on unsupported request, EIO on failed
* request.
*/

static int unixIoctl
    (
    SIO_CHAN *	pSioChan,		/* device to control */
    int		request,		/* request code */
    void *	someArg			/* some argument */
    )
    {
    UNIX_CHAN *pChan = (UNIX_CHAN *) pSioChan;
    int arg = (int)someArg;

    switch (request)
	{
	case SIO_BAUD_SET:
	    /*
	     * Set the baud rate. Return EIO for an invalid baud rate, or
	     * OK on success.
	     */
	    if (arg < 50 || arg > 38400)
	        {
		return (EIO);
	        }
	    /* fake it for Unix, keep tests happy */
	    pChan->baudRate = arg;
	    return (OK);
	case SIO_BAUD_GET:
	    /* Get the baud rate and return OK */
	    *(int *)arg = pChan->baudRate;
	    return (OK);
	case SIO_MODE_SET:
	    /*
	     * Set the mode (e.g., to interrupt or polled). Return OK
	     * or EIO for an unknown or unsupported mode.
	     */
	    return (unixModeSet ((SIO_CHAN *)pChan, arg));
	case SIO_MODE_GET:
	    /*
	     * Get the current mode and return OK.
	     */
	    *(int *)arg = pChan->mode;
	    return (OK);
	case SIO_AVAIL_MODES_GET:
	    /*
	     * Get the available modes and return OK.
	     */
	    *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
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
	    return (ENOSYS);
	case SIO_HW_OPTS_GET:
	    /*
	     * Optional command to get the hardware options (as defined
	     * in sioLib.h).
	     * Return OK or ENOSYS if this command is not implemented.
	     * Note: if this command is unimplemented, it will be
	     * assumed that the driver options are CREAD | CS8 (e.g.,
	     * eight data bits, one stop bit, no parity, ints enabled).
	     */
	    return (ENOSYS);
	default:
	    return (ENOSYS);
	}
    }

/*******************************************************************************
*
* dummyCallback - dummy callback routine
*
* RETURNS: Returns ERROR always.
*/

STATUS dummyCallback (void)
    {
    return (ERROR);
    }

