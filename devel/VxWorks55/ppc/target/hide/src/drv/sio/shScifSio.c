/* shScifSio.c - Hitachi SH SCIF (Serial Communications Interface) driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,06mar02,h_k  added clear overrun error for SH7615, SH7750 and SH7751 SCIF
                 (SPR #73777).
01g,02nov01,zl   corrected NULL usage and callback declaration.
01f,16nov00,zl   make BRR=0 invalid for baud rate setting.
01e,24aug99,zl   set initial baud rate
01d,29dec98,hk   changed to use smr/scr/fcr size macros for SH7750.
01c,12aug98,jmb  debugged, use FIFOs at max triggering level.
01b,10aug98,jmb  completed.
01a,31may98,jmb  derived from shScifSio.c.
*/

/*
DESCRIPTION
This is the driver for the Hitachi SH series on-chip SCIF (Serial Communication
Interface with FIFO).  It uses the SCIF in asynchronous mode only.

USAGE
A SCIF_CHAN structure is used to describe the chip. 

The BSP's sysHwInit() routine typically calls sysSerialHwInit()
which initializes all the values in the SCIF_CHAN structure (except
the SIO_DRV_FUNCS) before calling shSciDevInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(), which
connects the chips interrupts via intConnect(). 

INCLUDE FILES: drv/sio/shSciSio.h drv/sio/shScifSio.h sioLib.h
*/

#include "vxWorks.h"
#include "sioLib.h"
#include "intLib.h"
#include "errnoLib.h"
#include "drv/sio/shScifSio.h"

/* external data */

IMPORT SCI_BAUD sysBaudTable[];		/* BSP specific baud-rate constants */


/* forward static declarations */

static int shScifTxStartup (SIO_CHAN * pSioChan);
static int shScifCallbackInstall (SIO_CHAN * pSioChan, int callbackType,
				 STATUS (*callback)(void *, ...), 
				 void * callbackArg);
static int shScifPollOutput (SIO_CHAN * pSioChan, char outChar);
static int shScifPollInput (SIO_CHAN * pSioChan, char * thisChar);
static int shScifIoctl (SIO_CHAN * pSioChan, int request, void * arg);
LOCAL STATUS shScifOptsSet (SCIF_CHAN * pChan, UINT options);
static STATUS dummyCallback (void);


/* local variables */

static SIO_DRV_FUNCS shSioDrvFuncs =
    {
    shScifIoctl,
    shScifTxStartup,
    shScifCallbackInstall,
    shScifPollInput,
    shScifPollOutput
    };

#define SSC_DELAY	100000

/******************************************************************************
*
* shScifDevInit - initialize a on-chip serial communication interface
*
* This routine initializes the driver
* function pointers and then resets the chip in a quiescent state.
* The BSP must have already initialized all the device addresses and the
* baudFreq fields in the SCIF_CHAN structure before passing it to
* this routine.
*
* RETURNS: N/A
*/

void shScifDevInit
    (
    SCIF_CHAN * pChan
    )
    {
    /* initialize a channel's driver function pointers */

    pChan->pDrvFuncs = &shSioDrvFuncs;


    /* install dummy driver callbacks */

    pChan->getTxChar  = dummyCallback;
    pChan->putRcvChar = dummyCallback;
    
    /* Clear and disable the FIFOs */

    *(pChan->fcr2) |= (SZ_FCR2) (SCIF_FCR2_TFRST | SCIF_FCR2_RFRST);
 
    /* Set the FIFO depth to maximum value */
    *(pChan->fcr2) |= (SZ_FCR2) (SCIF_FCR2_RTRG | SCIF_FCR2_TTRG);

    /* reset the chip to 8-none-1, no interrupts enabled */

    *(pChan->scr2) &= 0x3;  /* Keep clock bits */
    *(pChan->smr2) &= 0x3;  /* Keep clock bits */
    *(pChan->smr2) |= (SZ_SMR2)(SCI_SMR_ASYNC | SCI_SMR_8_BITS |
			    SCI_SMR_PAR_DIS | SCI_SMR_1_STOP | SCI_SMR_MP_DIS);

    pChan->options = CS8;	/* 8-bit, no parity, one stop bit */

    /* set initial baud rate */

    shScifIoctl ((SIO_CHAN *)pChan, SIO_BAUD_SET, (void *)pChan->baud);

    /* setting polled mode is one way to make the device quiet */

    shScifIoctl ((SIO_CHAN *)pChan, SIO_MODE_SET, (void *)SIO_MODE_POLL);
    }

/******************************************************************************
*
* shScifIntRcv - handle a channel's receive-character interrupt.
*
* RETURNS: N/A
*/ 

void shScifIntRcv
    (
    SCIF_CHAN * pChan			/* channel generating the interrupt */
    )
    {
    int rcvCount;
    char inChar;
    volatile UINT16 ssr = *(pChan->ssr2);

    /* Get number of bytes in FIFO 
     * The low five bits of the FIFO data register contain the
     * receive count.
     */
    
    rcvCount = *(pChan->fdr2) & 0x1f;   
    /*
     * Empty the receive FIFO a character at a time.
     */
    while (rcvCount--)
	{
	inChar = *(pChan->frdr2);			/* grab the character */

	(*(pChan->putRcvChar)) (pChan->putRcvArg, inChar);     /* hand it off */
	}

    /* Acknowledge the interrupt by clearing the receive-data-fifo full bit */

    *(pChan->ssr2) = ssr & (UINT16) ~(SCIF_SSR2_RDF | SCIF_SSR2_DR);	   
    }

/******************************************************************************
*
* shScifIntTx - handle a channels transmitter-ready interrupt.
*
* RETURNS: N/A
*/ 

void shScifIntTx
    (
    SCIF_CHAN * pChan			/* channel generating the interrupt */
    )
    {
    char outChar;
    UINT count;
    volatile UINT16 ssr = *(pChan->ssr2);

    /*
     * If there's a character to transmit then write it out, else reset
     * the transmitter.  Keep writing characters until the ring buffer
     * is empty or the FIFO is full.
     */
    if (ssr & SCIF_SSR2_TDFE)
	{
        count = SCIF_FIFO_LEN - ((*(pChan->fdr2) >> 8) & 0x0ff);
        while (count--)
            {
	    if ((*(pChan->getTxChar)) (pChan->getTxArg, &outChar) != ERROR)
	        {
	        *(pChan->sfdr2) = outChar;
	        }
	    else
	        {
	        /* no more chars to xmit now.  reset the tx int,
	         * so the SCI does not keep interrupting.
	         */
	        *(pChan->scr2) &= (SZ_SCR2)~SCI_SCR_TIE;
                break;
	        }
            }
	    *(pChan->ssr2) = ssr & (UINT16)~SCIF_SSR2_TDFE;
	}
    }

/******************************************************************************
*
* shScifIntErr - handle a channel's error interrupt.
*
* RETURNS: N/A
*/ 

void shScifIntErr
    (
    SCIF_CHAN * pChan		        /* channel generating the interrupt */
    )
    {
    volatile UINT16 ssr;
    char dummy;

    ssr = *(pChan->ssr2);

    if (ssr & SCIF_SSR2_DR)
       dummy = *(pChan->frdr2);		/* Read stray data */

    *(pChan->ssr2) = (UINT16) NULL;		/* reset errors */

    if (pChan->lsr2 != NULL)
	{
	if (*pChan->lsr2 & (UINT16)SCIF_LSR2_ORER)
	    *pChan->lsr2 &= ~(UINT16)SCIF_LSR2_ORER; /* clear overrun error */
	}

    if (pChan->ss2r2 != NULL)
	{
	if (*pChan->ss2r2 & (UINT8)SCIF_SS2R2_ORER)
	    *pChan->ss2r2 &= ~(UINT8)SCIF_SS2R2_ORER; /* clear overrun error */
	}
    }

/******************************************************************************
*
* shScifTxStartup - start the interrupt transmitter.
*
* RETURNS: OK on success, ENOSYS if the device is polled-only, or
*          EIO on hardware error.
*/

static int shScifTxStartup
    (
    SIO_CHAN * pSioChan				/* channel to start */
    )
    {
    SCIF_CHAN * pChan = (SCIF_CHAN *)pSioChan;

    *(pChan->scr2) |= (SZ_SCR2)SCI_SCR_TIE;	/* only need to enable int */

    return (OK);
    }

/******************************************************************************
*
* shScifCallbackInstall - install ISR callbacks to get/put chars.
*
* This driver allows interrupt callbacks for transmitting characters
* and receiving characters. In general, drivers may support other
* types of callbacks too.
*
* RETURNS: OK on success, or ENOSYS for an unsupported callback type.
*/ 

static int shScifCallbackInstall
    (
    SIO_CHAN * pSioChan,			/* channel */
    int        callbackType,			/* type of callback */
    STATUS     (*callback)(void *, ...),	/* callback */
    void *     callbackArg			/* parameter to callback */
    )
    {
    SCIF_CHAN * pChan = (SCIF_CHAN *)pSioChan;

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
* shScifPollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*	   if the output buffer if full. ENOSYS if the device is
*          interrupt-only.
*/

static int shScifPollOutput
    (
    SIO_CHAN * pSioChan,
    char outChar
    )
    {
    SCIF_CHAN * pChan = (SCIF_CHAN *)pSioChan;
    volatile UINT16 ssr = *(pChan->ssr2);

    /* is the transmitter ready to accept a character? */

    if ((ssr & (UINT8)SCIF_SSR2_TDFE) == 0x00)
	return (EAGAIN);

    /* write out the character */

    *(pChan->sfdr2) = outChar;
    *(pChan->ssr2) = ssr & (UINT16)~SCIF_SSR2_TDFE;

    return (OK);
    }

/******************************************************************************
*
* shScifPollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*	   if the input buffer if empty, ENOSYS if the device is
*          interrupt-only.
*/

static int shScifPollInput
    (
    SIO_CHAN * pSioChan,
    char * thisChar		/* where to put the input character */
    )
    {
    SCIF_CHAN * pChan = (SCIF_CHAN *)pSioChan;
    volatile UINT16 ssr = *(pChan->ssr2);

    if ((ssr & SCIF_SSR2_RDF) == 0x00)
        if ((ssr & SCIF_SSR2_DR) == 0x00)
	    return (EAGAIN);

    /* got a character */

    *thisChar = *(pChan->frdr2);  
    *(pChan->ssr2) = ssr & (UINT16) ~(SCIF_SSR2_RDF | SCIF_SSR2_DR);

    return (OK);
    }

/******************************************************************************
*
* shScifModeSet - toggle between interrupt and polled mode.
*
* RETURNS: OK on success, EIO on unsupported mode.
*/

static int shScifModeSet
    (
    SCIF_CHAN * pChan,			/* channel */
    uint_t newMode			/* new mode */
    )
    {
    int oldlevel;

    if ((newMode != SIO_MODE_POLL) && (newMode != SIO_MODE_INT))
	return (EIO);

    oldlevel = intLock ();		/* LOCK INTERRUPTS */

    pChan->mode = newMode;		/* set the new mode */
    
    if (pChan->mode == SIO_MODE_INT)
	*(pChan->scr2) = (SZ_SCR2)(SCI_SCR_RIE | SCI_SCR_TXE | SCI_SCR_RXE);
    else
	*(pChan->scr2) = (SZ_SCR2)(SCI_SCR_TXE | SCI_SCR_RXE);

    intUnlock (oldlevel);		/* UNLOCK INTERRUPTS */

    return (OK);
    }

/*******************************************************************************
*
* shScifIoctl - special device control
*
* RETURNS: OK on success, ENOSYS on unsupported request, EIO on failed
*          request.
*/

static int shScifIoctl
    (
    SIO_CHAN * pSioChan,		/* channel to control */
    int        request,			/* request code */
    void *     someArg			/* some argument */
    )
    {
    SCIF_CHAN * pChan = (SCIF_CHAN *)pSioChan;
    int        oldlevel, delay;
    SZ_SCR2    oldScr;
    UINT8      cks, brr;
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
	    oldScr = *(pChan->scr2);		/* save old scr */

	    *(pChan->scr2) = (SZ_SCR2) 0;
            *(pChan->fcr2) |= (SZ_FCR2) (SCIF_FCR2_TFRST | SCIF_FCR2_RFRST);
	    *(pChan->smr2) = (SZ_SMR2)((*(pChan->smr2) & SCI_SMR_CKS_MASK)|cks);
	    *(pChan->brr2) = (UINT8)brr;
	    pChan->baud   = arg;

	    for (delay = 0; delay < (SSC_DELAY / arg); delay++) 
		delay = delay;			/* spin wheels for a moment */
	    
            *(pChan->fcr2) &= (SZ_FCR2) ~(SCIF_FCR2_TFRST | SCIF_FCR2_RFRST);
	    *(pChan->scr2) = oldScr;		/* restore scr */
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
	    return (shScifModeSet (pChan, arg));

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
            result =  shScifOptsSet (pChan, arg);
            if (pChan->baud)
	        for (delay = 0; delay < (SSC_DELAY / pChan->baud); delay++)
		    delay = delay;		/* spin wheels for a moment */
            shScifModeSet (pChan, pChan->mode);
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
* shScifOptsSet - set the serial options
*
* Set the channel operating mode to that specified.  These sioLib options
* are supported: CLOCAL, CSIZE (CS7 and CS8), PARENB, and PARODD.
*
* One unsupported options is CREAD.  The routine
* to turn on serial I/O is shScifModeSet().
*
* Note, this routine disables all serial interrupts.  To re-enable
* serial communication, the caller should use shScifModeSet().
*
* RETURNS:
* Returns OK to indicate success.  ERROR is returned if a bad arg is
* detected. ENOSYS is returned for an unsupported option.
*/

LOCAL STATUS shScifOptsSet
    (
    SCIF_CHAN * pChan, 		/* ptr to channel */
    UINT options		/* new hardware options */
    )
    {
    /*  Retain last two bits of serial mode register.  These form the
     *  peripheral clock divisor.
     */
    SZ_SMR2 smrValue = *(pChan->smr2) & 0x3;
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

    *(pChan->scr2) &= 0x03;	/* Disable xmit, receiver, and all interrupts */

    *(pChan->smr2) = smrValue;	/* Reset data format */

    if (options & CLOCAL)
        *(pChan->fcr2) &= ~SCIF_FCR2_MCE;  /* Turn off h/w flow control */
    else
        *(pChan->fcr2) |= SCIF_FCR2_MCE;   /* Turn on h/w flow control */

    *(pChan->scr2) |= (SCI_SCR_TXE | SCI_SCR_RXE);  /* Turn on xmit, recv */

    intUnlock (lvl);				/* UNLOCK INTERRUPTS */

    pChan->options = options;

    return (OK);
    }
