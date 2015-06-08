/* m68332Sio.c - Motorola MC68332 tty driver*/

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,24apr02,pmr  SPR 75161: returning int from m68332Startup() as required.
01b,09nov95,jdi  doc: style cleanup.
01a,22aug95,myz  written(based on m68332Serial.c).
*/

/*
DESCRIPTION
This is the driver for the Motorola MC68332 on-chip UART.  It has only one
serial channel.

USAGE
A M68332_CHAN structure is used to describe the chip.
The BSP's sysHwInit() routine typically calls sysSerialHwInit(),
which initializes all the values in the M68332_CHAN structure (except
the SIO_DRV_FUNCS) before calling m68332DevInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(), which
connects the chips interrupt (m68332Int) via intConnect().

INCLUDE FILES: drv/sio/m68332Sio.h
*/

/* includes */

#include "vxWorks.h"
#include "intLib.h"
#include "errno.h"
#include "sioLib.h"
#include "drv/sio/m68332Sio.h"

/* locals */

static SIO_DRV_FUNCS m68332SioDrvFuncs;

/* forward declarations */

static STATUS m68332Ioctl (M68332_CHAN *pChan,int request,int arg);
static void   m68332ResetChannel (M68332_CHAN *pChan);
static int    m68332PollOutput (M68332_CHAN *,char);
static int    m68332PollInput (M68332_CHAN *,char *);
static int    m68332Startup (M68332_CHAN *);
static int    m68332CallbackInstall (SIO_CHAN *, int, STATUS (*)(), void *);


/*******************************************************************************
*
* m68332DevInit - initialize the SCC
*
* This initializes the chip to a quiescent state.
*
* RETURNS: N/A
*/

void m68332DevInit
    (
    M68332_CHAN *pChan
    )
    {

    *pChan->qilr &= ~QSM_QILR_SCI_MASK;         /* disable sci ints */

    /* make sure the driver function pointers are installed */

    if (m68332SioDrvFuncs.ioctl == NULL)
        {
        m68332SioDrvFuncs.txStartup   = (int (*)(SIO_CHAN *)) m68332Startup;
        m68332SioDrvFuncs.pollInput   = (int (*)(SIO_CHAN *,char *))
                                        m68332PollInput;
        m68332SioDrvFuncs.pollOutput  = (int (*)(SIO_CHAN *,char ))
                                        m68332PollOutput;
        m68332SioDrvFuncs.ioctl  = (int (*)(SIO_CHAN *,int,void *))m68332Ioctl;
        m68332SioDrvFuncs.callbackInstall = (int (*)()) m68332CallbackInstall;
        }

    pChan->pDrvFuncs = &m68332SioDrvFuncs;

    }

/*******************************************************************************
*
* m68332ResetChannel - initialize the SCC
*/

static void m68332ResetChannel
    (
    M68332_CHAN *pChan
    )
    {
    FAST int oldlevel = intLock ();	/* LOCK INTERRUPTS */

    /* initialize QSM registers associated with SCI */

    *pChan->qilr &= ~QSM_QILR_SCI_MASK;		/* disable sci ints */

    /* set default baud rate */
/*
    *pChan->sccr0 = pChan->clockRate / (32 * pChan->baudRate);
*/
    *pChan->sccr1 = QSM_SCCR1_RE | QSM_SCCR1_TE | QSM_SCCR1_RIE
        ;	/* enable rx/tx  enable receiver interrupts */
    *pChan->qilr |= pChan->intVec;			/* enable sci ints */

    intUnlock (oldlevel);		/* UNLOCK INTERRUPTS */
    }


/*******************************************************************************
*
* m68332Ioctl - special device control
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*
*/

LOCAL STATUS m68332Ioctl
    (
    M68332_CHAN *pChan, /* device to control */
    int request,        /* request code */
    int arg             /* some argument */
    )
    {
    FAST STATUS	status = EIO;
    UINT16	baudScratch;
    int oldlevel;

    switch (request)
	{
	case SIO_BAUD_SET:
	    baudScratch = pChan->clockRate / (32 * arg);
	    if ((baudScratch >= 1) && (baudScratch <= 8191) && (arg <= 38400))
		{
		*pChan->sccr0 = baudScratch;
		status = OK;
		}
	    break;

        case SIO_BAUD_GET:
            *(int *)arg = pChan->baudRate;
            status = OK;
            break;

        case SIO_MODE_SET:

            if (!((int)arg == SIO_MODE_POLL || (int)arg == SIO_MODE_INT))
                break;

            /* lock interrupt  */
            oldlevel = intLock();

            /* don't reset channel if switching between polling and int mode */
            if (!pChan->channelMode)
                m68332ResetChannel(pChan);

            if (arg == SIO_MODE_INT)
                *pChan->qilr |= pChan->intVec;        /* enable sci ints */
            else
                *pChan->qilr &= ~QSM_QILR_SCI_MASK; /* disable sci ints */

            pChan->channelMode = arg;
       
            status = OK;

            intUnlock(oldlevel);

            break;
                
        case SIO_MODE_GET:
            *(int *)arg = pChan->channelMode;
            return (OK);

        case SIO_AVAIL_MODES_GET:
            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
            return (OK);

        default:
            status = ENOSYS;
        }

    return (status);
    }

/*******************************************************************************
*
* m68332Int - handle an SCC interrupt
*
* This routine handles SCC interrupts.
*
* RETURNS: N/A
*/

void m68332Int
    (
    M68332_CHAN *pChan
    )
    {
    char outChar;

    if (*pChan->scsr & QSM_SCSR_TDRE)
	{
	if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK)
	    *pChan->scdr = outChar;
	else
	    *pChan->sccr1 &= ~QSM_SCCR1_TIE;
        }

    if (*pChan->scsr & QSM_SCSR_RDRF)
        {
        outChar = *pChan->scdr;
        (*pChan->putRcvChar) (pChan->putRcvArg,outChar);
        }
    }

/*******************************************************************************
*
* m68332Startup - transmitter startup routine
*/

static int m68332Startup
    (
    M68332_CHAN *pChan          /* ty device to start up */
    )
    {
    char outChar;

    if ((*pChan->scsr & QSM_SCSR_TDRE) &&
        ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK))
	{
	*pChan->scdr   = outChar;
	*pChan->sccr1 |= QSM_SCCR1_TIE;
	}

    return (OK);
    }

/******************************************************************************
*
* m68332PollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the input buffer is empty.
*/

static int m68332PollInput
    (
    M68332_CHAN * pChan,
    char *      thisChar
    )
    {
   
    if ( !(*pChan->scsr & QSM_SCSR_RDRF) )
        return (EAGAIN);

    *thisChar = *pChan->scdr;

    return (OK);
    }

/******************************************************************************
*
* m68332PollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the output buffer if full.
*/

static int m68332PollOutput
    (
    M68332_CHAN * pChan,
    char        outChar
    )
    {
    
    if ( !(*pChan->scsr & QSM_SCSR_TDRE) )
        return(EAGAIN);

    *pChan->scdr = outChar;

    return(OK);
    }

/******************************************************************************
*
* m68332CallbackInstall - install ISR callbacks to get put chars.
*/

static int m68332CallbackInstall
    (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS      (*callback)(),
    void *      callbackArg
    )
    {
    M68332_CHAN * pChan = (M68332_CHAN *)pSioChan;

    switch (callbackType)
        {
        case SIO_CALLBACK_GET_TX_CHAR:
            pChan->getTxChar    = callback;
            pChan->getTxArg     = callbackArg;
            return (OK);

        case SIO_CALLBACK_PUT_RCV_CHAR:
            pChan->putRcvChar   = callback;
            pChan->putRcvArg    = callbackArg;
            return (OK);

        default:
            return (ENOSYS);
        }
    }

