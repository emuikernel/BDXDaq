/* shSciSio.c - Hitachi SH SCI (Serial Communications Interface) driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01i,02nov01,zl   corrected NULL usage and callback declaration.
01h,16nov00,zl   make BRR=0 invalid for baud rate setting.
01g,24aug99,zl   set initial baud rate
01f,01apr99,hk   fix for receive overruns in polled mode.
01e,30may98,jmb  Add support for more data formats.
01d,22mar97,hk   changed to import sysBaudTable[].
01c,10mar97,hk   renamed as shSciSio.c from shSio.c. changed function names to
                 shSciAbc(). deleted casting for shSioDrvFuncs' members.
                 changed SH_DUSART to SCI_CHAN and made CPU conditional simple.
                 added SH704X support. fixed shSciIoctl() return value.
                 changed baud-rate save value to arg from brr in shSciIoctl().
                 also saved default baud-rate in shSciDevInit().
01b,07mar97,hk   fixed shDevInit() to set SIO_MODE_POLL for reboot failure.
01a,16dec96,kn   derived from templateSio.c.
*/

/*
DESCRIPTION
This is the driver for the Hitachi SH series on-chip SCI (Serial Communication
Interface).  It uses the SCI in asynchronous mode only.

USAGE
A SCI_CHAN structure is used to describe the chip. 

The BSP's sysHwInit() routine typically calls sysSerialHwInit()
which initializes all the values in the SCI_CHAN structure (except
the SIO_DRV_FUNCS) before calling shSciDevInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(), which
connects the chips interrupts via intConnect(). 

INCLUDE FILES: drv/sio/shSciSio.h sioLib.h
*/

#include "vxWorks.h"
#include "sioLib.h"
#include "intLib.h"
#include "errnoLib.h"
#include "drv/sio/shSciSio.h"


/* external data */

IMPORT SCI_BAUD sysBaudTable[];		/* BSP specific baud-rate constants */


/* forward static declarations */

static int shSciTxStartup (SIO_CHAN * pSioChan);
static int shSciCallbackInstall (SIO_CHAN * pSioChan, int callbackType,
				 STATUS (*callback)(void *, ...), 
				 void * callbackArg);
static int shSciPollOutput (SIO_CHAN * pSioChan, char outChar);
static int shSciPollInput (SIO_CHAN * pSioChan, char * thisChar);
static int shSciIoctl (SIO_CHAN * pSioChan, int request, void * arg);
LOCAL STATUS shSciOptsSet (SCI_CHAN * pChan, UINT options);
static STATUS dummyCallback (void);


/* local variables */

static SIO_DRV_FUNCS shSioDrvFuncs =
    {
    shSciIoctl,
    shSciTxStartup,
    shSciCallbackInstall,
    shSciPollInput,
    shSciPollOutput
    };

#define SSC_DELAY	100000

/******************************************************************************
*
* shSciDevInit - initialize a on-chip serial communication interface
*
* This routine initializes the driver
* function pointers and then resets the chip in a quiescent state.
* The BSP must have already initialized all the device addresses and the
* baudFreq fields in the SCI_CHAN structure before passing it to
* this routine.
*
* RETURNS: N/A
*/

void shSciDevInit
    (
    SCI_CHAN * pChan
    )
    {
    /* initialize a channel's driver function pointers */

    pChan->pDrvFuncs = &shSioDrvFuncs;


    /* install dummy driver callbacks */

    pChan->getTxChar  = dummyCallback;
    pChan->putRcvChar = dummyCallback;
    

    /* reset the chip to 8-none-1, no interrupts enabled */

    *(pChan->scr) = (UINT8) 0;
    *(pChan->smr) = (UINT8)(SCI_SMR_ASYNC | SCI_SMR_8_BITS |
			    SCI_SMR_PAR_DIS | SCI_SMR_1_STOP | SCI_SMR_MP_DIS);

    pChan->options = CS8;	/* 8-bit, no parity, one stop bit */

    /* set initial baud rate */

    shSciIoctl ((SIO_CHAN *)pChan, SIO_BAUD_SET, (void *)pChan->baud);

    /* setting polled mode is one way to make the device quiet */

    shSciIoctl ((SIO_CHAN *)pChan, SIO_MODE_SET, (void *)SIO_MODE_POLL);
    }

/******************************************************************************
*
* shSciIntRcv - handle a channel's receive-character interrupt.
*
* RETURNS: N/A
*/ 

void shSciIntRcv
    (
    SCI_CHAN * pChan			/* channel generating the interrupt */
    )
    {
    volatile UINT8 ssr = *(pChan->ssr);

    /*
     * Grab the input character from the chip and hand it off via a
     * callback. For chips with input FIFO's it is more efficient
     * to empty the entire FIFO here.
     */
    if (ssr & SCI_SSR_RDRF)
	{
	char inChar = *(pChan->rdr);			/* grab the character */

	*(pChan->ssr) = ssr & (UINT8)~SCI_SSR_RDRF;	   /* int acknowledge */
	(*(pChan->putRcvChar)) (pChan->putRcvArg, inChar);     /* hand it off */
	}
    }

/******************************************************************************
*
* shSciIntTx - handle a channels transmitter-ready interrupt.
*
* RETURNS: N/A
*/ 

void shSciIntTx
    (
    SCI_CHAN * pChan			/* channel generating the interrupt */
    )
    {
    volatile UINT8 ssr = *(pChan->ssr);

    /*
     * If there's a character to transmit then write it out, else reset
     * the transmitter. For chips with output FIFO's it is more efficient
     * to fill the entire FIFO here.
     */
    if (ssr & SCI_SSR_TDRE)
	{
	char outChar;

	if ((*(pChan->getTxChar)) (pChan->getTxArg, &outChar) != ERROR)
	    {
	    *(pChan->tdr) = outChar;
	    *(pChan->ssr) = ssr & (UINT8)~SCI_SSR_TDRE;    /* int acknowledge */
	    }
	else
	    {
	    /* no more chars to xmit now.  reset the tx int,
	     * so the SCI does not keep interrupting.
	     */
	    *(pChan->scr) &= (UINT8)~SCI_SCR_TIE;
	    }
	}
    }

/******************************************************************************
*
* shSciIntErr - handle a channel's error interrupt.
*
* RETURNS: N/A
*/ 

void shSciIntErr
    (
    SCI_CHAN * pChan		        /* channel generating the interrupt */
    )
    {
    volatile UINT8 ssr;

    ssr = *(pChan->ssr);

    *(pChan->ssr) = (UINT8) 0;		/* reset errors */
    }

/******************************************************************************
*
* shSciTxStartup - start the interrupt transmitter.
*
* RETURNS: OK on success, ENOSYS if the device is polled-only, or
*          EIO on hardware error.
*/

static int shSciTxStartup
    (
    SIO_CHAN * pSioChan				/* channel to start */
    )
    {
    SCI_CHAN * pChan = (SCI_CHAN *)pSioChan;

    *(pChan->scr) |= (UINT8)SCI_SCR_TIE;	/* only need to enable int */

    return (OK);
    }

/******************************************************************************
*
* shSciCallbackInstall - install ISR callbacks to get/put chars.
*
* This driver allows interrupt callbacks for transmitting characters
* and receiving characters. In general, drivers may support other
* types of callbacks too.
*
* RETURNS: OK on success, or ENOSYS for an unsupported callback type.
*/ 

static int shSciCallbackInstall
    (
    SIO_CHAN * pSioChan,			/* channel */
    int        callbackType,			/* type of callback */
    STATUS     (*callback)(void *, ...),	/* callback */
    void *     callbackArg			/* parameter to callback */
    )
    {
    SCI_CHAN * pChan = (SCI_CHAN *)pSioChan;

    switch (callbackType)
	{
	case SIO_CALLBACK_GET_TX_CHAR:
	    pChan->getTxChar	= (STATUS (*)(void *, char *))callback;
	    pChan->getTxArg	= callbackArg;
	    return (OK);

	case SIO_CALLBACK_PUT_RCV_CHAR:
	    pChan->putRcvChar	= (STATUS (*)(void *, char *))callback;
	    pChan->putRcvArg	= callbackArg;
	    return (OK);

	default:
	    return (ENOSYS);
	}
    }

/******************************************************************************
*
* shSciPollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*	   if the output buffer if full. ENOSYS if the device is
*          interrupt-only.
*/

static int shSciPollOutput
    (
    SIO_CHAN * pSioChan,
    char outChar
    )
    {
    SCI_CHAN * pChan = (SCI_CHAN *)pSioChan;
    volatile UINT8 ssr = *(pChan->ssr );

    /* is the transmitter ready to accept a character? */

    if ((ssr & (UINT8)SCI_SSR_TDRE) == 0x00)
	return (EAGAIN);

    /* write out the character */

    *(pChan->tdr) = outChar;
    *pChan->ssr = ssr & (UINT8)~SCI_SSR_TDRE;

    return (OK);
    }

/******************************************************************************
*
* shSciPollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*	   if the input buffer if empty, ENOSYS if the device is
*          interrupt-only.
*/

static int shSciPollInput
    (
    SIO_CHAN * pSioChan,
    char * thisChar		/* where to put the input character */
    )
    {
    SCI_CHAN * pChan = (SCI_CHAN *)pSioChan;
    volatile UINT8 ssr = *(pChan->ssr);

    /* If a receive overrun condition is set, the SCI does not receive
     * any more characters.  Since it is likely to happen in polling
     * mode, we have to check and clear the overrun status.
     */
    if (ssr & SCI_SSR_ORER)
       *(pChan->ssr) = ssr & (UINT8)~SCI_SSR_ORER;

    if ((ssr & SCI_SSR_RDRF) == 0x00)
	return (EAGAIN);

    /* got a character */

    *thisChar = *(pChan->rdr);
    *pChan->ssr = ssr & (UINT8)~SCI_SSR_RDRF;

    return (OK);
    }

/******************************************************************************
*
* shSciModeSet - toggle between interrupt and polled mode.
*
* RETURNS: OK on success, EIO on unsupported mode.
*/

static int shSciModeSet
    (
    SCI_CHAN * pChan,			/* channel */
    uint_t newMode			/* new mode */
    )
    {
    int oldlevel;

    if ((newMode != SIO_MODE_POLL) && (newMode != SIO_MODE_INT))
	return (EIO);

    oldlevel = intLock ();		/* LOCK INTERRUPTS */

    pChan->mode = newMode;		/* set the new mode */
    
    if (pChan->mode == SIO_MODE_INT)
	*(pChan->scr) = (UINT8)(SCI_SCR_RIE | SCI_SCR_TXE | SCI_SCR_RXE);
    else
	*(pChan->scr) = (UINT8)(SCI_SCR_TXE | SCI_SCR_RXE);

    intUnlock (oldlevel);		/* UNLOCK INTERRUPTS */

    return (OK);
    }

/*******************************************************************************
*
* shSciIoctl - special device control
*
* RETURNS: OK on success, ENOSYS on unsupported request, EIO on failed
*          request.
*/

static int shSciIoctl
    (
    SIO_CHAN * pSioChan,		/* channel to control */
    int        request,			/* request code */
    void *     someArg			/* some argument */
    )
    {
    SCI_CHAN * pChan = (SCI_CHAN *)pSioChan;
    int        oldlevel, delay;
    UINT8      oldScr, cks, brr;
    STATUS     result;
    int        arg = (int)someArg;

    switch (request)
	{
	case SIO_BAUD_SET:
	    /*
	     * Set the baud rate. Return EIO for an invalid baud rate, or
	     * OK on success.
	     */
	    {
	    int ix, status;

	    for (status = EIO, cks = 0, brr = 0,
		 ix = 0; sysBaudTable[ix].rate != 0; ix++)
		{
		if (sysBaudTable[ix].rate == arg)
		    {
		    cks = sysBaudTable[ix].cksVal;	/* clock select bits */
		    brr = sysBaudTable[ix].brrVal;	/* bit rate */
		    status = OK;
		    break;
		    }
		}
	    if (status == EIO) return (status);

	    /* 
	     * Although the zero value for BRR may be aceptable in rare 
	     * occasions, we use it as invalid value - for example when the
	     * calculated value overflows (larger than 255).
	     */

	    if (brr == 0) return (EIO);
	    }

	    oldlevel = intLock ();		/* LOCK INTERRUPTS */
	    oldScr = *(pChan->scr);		/* save old scr */

	    *(pChan->scr) = (UINT8) 0;
	    *(pChan->smr) = (UINT8)((*(pChan->smr) & SCI_SMR_CKS_MASK) | cks);
	    *(pChan->brr) = (UINT8)brr;
	    pChan->baud   = arg;

	    for (delay = 0; delay < (SSC_DELAY / arg); delay++)
		delay = delay;			/* spin wheels for a moment */
	    
	    *(pChan->scr) = oldScr;		/* restore scr */
	    intUnlock (oldlevel);		/* UNLOCK INTERRUPTS */
	    return (OK);

	case SIO_BAUD_GET:
	    /* Get the baud rate and return OK */
	    *(int *)arg = pChan->baud;
	    return (OK);

	case SIO_MODE_SET:
	    /*
	     * Set the mode (e.g., to interrupt or polled). Return OK
	     * or EIO for an unknown or unsupported mode.
	     */
	    return (shSciModeSet (pChan, arg));

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
            /* change options, then set mode to restart chip correctly */
            result =  shSciOptsSet (pChan, arg);
            if (pChan->baud)
	        for (delay = 0; delay < (SSC_DELAY / pChan->baud); delay++)
		    delay = delay;		/* spin wheels for a moment */
            shSciModeSet (pChan, pChan->mode);
            return (result);

        case SIO_HW_OPTS_GET:
            *(int *)arg = pChan->options;
            return (OK);

	default:
	    return (ENOSYS);
	}
    }

/*******************************************************************************
*
* dummyCallback - dummy callback routine.
*
* RETURNS: ERROR.
*/

STATUS dummyCallback (void)
    {
    return (ERROR);
    }

/*******************************************************************************
*
* shSciOptsSet - set the serial options
*
* Set the channel operating mode to that specified.  These sioLib options
* are supported: CSIZE (CS7 and CS8), PARENB, and PARODD.
*
* The unsupported options are CREAD and CLOCAL.  CLOCAL is ignored because
* SCI does not have hardware flow control.  CREAD is also ignored.  The
* routine to turn on serial I/O is shSciModeSet().
*
* Note, this routine disables all serial interrupts.  To re-enable
* serial communication, the caller should use shSciModeSet().
*
* RETURNS:
* Returns OK to indicate success.  ERROR is returned if a bad arg is
* detected. ENOSYS is returned for an unsupported option.
*/

LOCAL STATUS shSciOptsSet
    (
    SCI_CHAN * pChan, 		/* ptr to channel */
    UINT options		/* new hardware options */
    )
    {
    /*  Retain last two bits of serial mode register.  These form the
     *  peripheral clock divisor.
     */
    UINT8 smrValue = *(pChan->smr) & 0x3;
    int lvl;

    if (pChan == NULL)
	return (ERROR);

    /* Set the data format.  SCI supports even/odd parity, 1/2 stop bits,
     * and 7/8 bit characters.
     */

    switch (options & CSIZE)	
	{
	case CS5: 
	case CS6:
            return (ENOSYS); 
            break;
	case CS7:
	    smrValue |= SCI_SMR_7_BITS; 		/* 7-bit data */
            break;
	case CS8:
	default:
	    smrValue &=  ~SCI_SMR_7_BITS; 		/* 8-bit data */
	    break;
	}

    if (options & STOPB)
	smrValue |=   SCI_SMR_2_STOP;   	/* 2 stop bits */
    else
	smrValue &=  ~SCI_SMR_2_STOP;  		/* 1 stop bit */

    switch (options & (PARENB|PARODD))
	{
	case (PARENB|PARODD):
	case PARODD:
	    smrValue |= SCI_SMR_PAR_EN;          /* parity enabled */
	    smrValue |= SCI_SMR_PAR_ODD;         /* parity odd */
            break; 
	case PARENB:
	    smrValue |= SCI_SMR_PAR_EN;          /* parity enabled */
	    smrValue &= ~SCI_SMR_PAR_ODD;        /* parity even */
            break;
	default:
	case 0:
	    smrValue &= ~SCI_SMR_PAR_EN;         /* parity disabled */
            break;
	}

    lvl = intLock ();				/* LOCK INTERRUPTS */

    /* 
     *  Set the hardware registers
     */

    *(pChan->scr) &= 0x03;	/* Disable xmit, receiver, and all interrupts */

    *(pChan->smr) = smrValue;	/* Reset data format */

    *(pChan->scr) |= (SCI_SCR_TXE | SCI_SCR_RXE);  /* Turn on xmit, recv */

    intUnlock (lvl);				/* UNLOCK INTERRUPTS */

    pChan->options = options;

    return (OK);
    }
