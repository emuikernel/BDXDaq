/* cd2400Sio.c - CL-CD2400 MPCC serial driver */

/* Copyright 1984-1995 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,20nov96,dds  SPR 5438: Added interrupt locking to cd2400Startup () routine.
01g,01dec95,myz  removed waiting for transmitter done loop in ioctl SIO_MODE_SET
01f,16nov95,myz  reworked SPR #4678 about the baud rate problem with WDB agent
01e,08sep95,myz  fixed the SPR #4678
01d,03aug95,myz  fixed the warning messages
01c,20jun95,ms	 fixed comments for mangen
01b,15jun95,ms   updated for new driver structure
01a,05mar95,myz  written (using cd2400Serial.c + the VxMon polled driver).
*/

/*
DESCRIPTION
This is the driver for the Cirus Logic CD2400 MPCC.  It uses the SCC's in
asynchronous mode.

USAGE
A CD2400_QUSART structure is used to describe the chip. This data structure
contains four CD2400_CHAN structure which describe the chip's four serial
channels.
The BSP's sysHwInit() routine typically calls sysSerialHwInit()
which initializes all the values in the CD2400_QUSART structure (except
the SIO_DRV_FUNCS) before calling cd2400HrdInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chips interrupts (cd2400Int, cd2400IntRx, and
cd2400IntTx) via intConnect().

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal serial driver; for
more information, see the comments in sioLib.h.  The available baud rates
are:  50, 110, 150, 300, 600, 1200, 2400, 3600, 4800, 7200, 9600,
19200, and 38400.

INCLUDE FILES: drv/sio/cd2400Sio.h
*/

#include "vxWorks.h"
#include "sioLib.h"
#include "intLib.h"
#include "errno.h"
#include "drv/sio/cd2400Sio.h"

#define DEFAULT_BAUD	9600

/* forward declarations */

static int      cd2400Ioctl    (SIO_CHAN *, int, void *);
static int      cd2400Startup  (SIO_CHAN * );
static int      cd2400PRxChar  (SIO_CHAN *, char *);
static int      cd2400PTxChar  (SIO_CHAN *, char);
static void     cd2400ChanIntEnDisable (CD2400_CHAN *, int);
static void     cd2400SetMode  (CD2400_CHAN *, int);
static int      cd2400CallbackInstall (SIO_CHAN *, int, STATUS (*)(), void *);
static void     cd2400InitChannel(CD2400_CHAN *);

/* channel descriptors */

typedef struct			/* BAUD */
    {
    USHORT rate;		/* baud rate */
    USHORT timeConstant;	/* time constant */
    char tcor;			/* transmit clock option register value */
    char rcor;			/* receive clock option register value */
    } BAUD;

static BAUD baudTable [] =
    {
     {    50, 2048, (unsigned char)TCOR_CLK4, RCOR_CLK4 },
     {   110, 2048, (unsigned char)TCOR_CLK4, RCOR_CLK4 },
     {   150, 2048, (unsigned char)TCOR_CLK4, RCOR_CLK4 },
     {   300, 512,                 TCOR_CLK3, RCOR_CLK3 },
     {   600, 512,                 TCOR_CLK3, RCOR_CLK3 },
     {  1200, 128,                 TCOR_CLK2, RCOR_CLK2 },
     {  2400, 128,                 TCOR_CLK2, RCOR_CLK2 },
     {  3600, 32,                  TCOR_CLK1, RCOR_CLK1 },
     {  4800, 32,                  TCOR_CLK1, RCOR_CLK1 },
     {  7200, 32,                  TCOR_CLK1, RCOR_CLK1 },
     {  9600, 32,                  TCOR_CLK1, RCOR_CLK1 },
     { 19200, 8,                   TCOR_CLK0, RCOR_CLK0 },
     { 38400, 8,                   TCOR_CLK0, RCOR_CLK0 }
    };

/* local driver function table */

static SIO_DRV_FUNCS cd2400SioDrvFuncs =
    {
    (int (*)())cd2400Ioctl,
    (int (*)())cd2400Startup,
    cd2400CallbackInstall,
    (int (*)())cd2400PRxChar,
    (int (*)(SIO_CHAN *,char))cd2400PTxChar
    };


/*******************************************************************************
*
* cd2400HrdInit - initialize the chip
*
* This routine initializes the chip and the four channels.
*/

void cd2400HrdInit
    (
    CD2400_QUSART * pQusart		/* chip to reset */
    )
    {
    FAST int   oldlevel;	/* current interrupt level mask */
    int	ix;

    oldlevel = intLock ();      /* disable interrupts during init */

    /* make sure the driver function pointers are installed */

    for (ix = 0; ix < N_CHANNELS; ix++)
	pQusart->channel[ix].pDrvFuncs      = &cd2400SioDrvFuncs;

    while (*MPCC_CCR)
	;			/* make sure no outstanding commands */
    *MPCC_CCR = CCR_RESET_ALL;	/* reset chip */
    while (*MPCC_CCR)
	;			/* make sure we are done */

    while (*MPCC_GFRCR == 0)
	;			/* wait for it to be non-zero */

    *MPCC_TPR = 0x0a;		/* Timer Peroid Register */
    *MPCC_PILR1 = 0x00;		/* Priority Interrupt Level Register */
    *MPCC_PILR2 = 0x02;		/* Priority Interrupt Level Register */
    *MPCC_PILR3 = 0x03;		/* Priority Interrupt Level Register */

    for (ix = 0; ix < N_CHANNELS; ix++)
	cd2400InitChannel (&pQusart->channel[ix]);

    intUnlock (oldlevel);
    }

/*******************************************************************************
*
* cd2400InitChannel - initialize a single channel
*/

static void cd2400InitChannel
    (
    CD2400_CHAN *	pChan		/* channel to reset */
    )
    {
    int        oldlevel;		/* current interrupt level mask */
    CD2400_QUSART * pQusart = pChan->pQusart;

    oldlevel = intLock ();		/* disable interrupts during init */

    /* initialize registers */

    *MPCC_CAR  = pChan->chan_num;

    *MPCC_LIVR = pChan->int_vec;

    *MPCC_COR1 = COR1_NO_PARITY	|
		 COR1_8BITS;

    *MPCC_COR2 = 0x00;

    *MPCC_COR3 = COR3_1STOP_BIT;

    *MPCC_COR4 = 0x00;	/* no modem control for now */

    *MPCC_COR5 = 0x00;	/* no modem control for now */

    *MPCC_COR6 = COR6_NORMAL_CR_NL	|
		 COR6_BRK_EXCEPTION	|
		 COR6_PARITY_EXCEPTION;

    *MPCC_COR7 = COR7_NORMAL_CR_NL;

    *MPCC_CMR = CMR_RX_INTERRUPT	|
		CMR_TX_INTERRUPT	|
		CMR_ASYNC_MODE;


    *MPCC_TBPR = BPR_9600;
    *MPCC_TCOR = TCOR_CLK1;
    *MPCC_RBPR = BPR_9600;
    *MPCC_RCOR = RCOR_CLK1;

    *MPCC_CPSR = CPSR_CRC_V41;

    *MPCC_SCHR1 = 0x00;
    *MPCC_SCHR2 = 0x00;

    *MPCC_SCHR3 = 0x00;		/* Special Char Reg 3 */
    *MPCC_SCHR4 = 0x00;		/* Special Char Reg 4 */

    *MPCC_SCRL = 0x00;		/* Special Char Range Low */
    *MPCC_SCRH = 0x00;		/* Special Char Range High */

    *MPCC_RTPRL = 0x01;		/* set timeout high - ONLY DMA mode */
    *MPCC_RTPRH = 0x00;		/* so this should not be used for now */

    while (*MPCC_CCR)
	;			/* make sure no outstanding commands */

    *MPCC_CCR = CCR_INIT_CHANNEL;

    while (*MPCC_CCR)
	;			/* make sure we are done */

    *MPCC_CCR = CCR_ENABLE_TRANS	|
		CCR_ENABLE_REC;	/* enable Tx and Rx */

    while (*MPCC_CCR)
	;

    *MPCC_IER = 0;       /* disable interrupt */

    *MPCC_MSVRDTR = MSVR_DTR;
    *MPCC_MSVRRTS = MSVR_RTS;

    pChan->created = TRUE;

    intUnlock (oldlevel);
    }

/*******************************************************************************
*
* cd2400Ioctl - special device control
*
* This routine handles SIO_BAUD_SET and SIO_RESET requests.
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*/

static int cd2400Ioctl
    (
    SIO_CHAN * 	pSioChan,	/* device to control */
    int		request,	/* request code      */
    void *	arg		/* some argument     */
    )
    {
    int		oldlevel;		/* current interrupt level mask */
    FAST int	ix;
    STATUS	status;
    USHORT        rcvBaud;
    char        rcvFreq;

    CD2400_QUSART * pQusart = ((CD2400_CHAN *)pSioChan)->pQusart;

    switch (request)
	{
	case SIO_BAUD_SET:

	    status = EIO;		/* baud rate out of range */

	    /* disable interrupts during chip access */

	    oldlevel = intLock ();

	    *MPCC_CAR = ((CD2400_CHAN *)pSioChan)->chan_num; /*  channel */

	    for (ix = 0; ix < NELEMENTS (baudTable); ix++)
		{
		if (baudTable [ix].rate == (int)arg)
		    {
		    *MPCC_TBPR = ( (pQusart->baudClkFreq /
				   baudTable [ix].timeConstant) /
				  (int)arg ) - 1;
		    *MPCC_TCOR = baudTable [ix].tcor;
		    *MPCC_RBPR = ( (pQusart->baudClkFreq /
                                   baudTable [ix].timeConstant) /
                                  (int)arg ) - 1;
		    *MPCC_RCOR = baudTable [ix].rcor;
		    status = OK;
		    break;
		    }
		}

	    intUnlock (oldlevel);

	    break;

        case SIO_BAUD_GET:

            status = EIO;               /* baud rate out of range */

            /* disable interrupts during chip access */

            oldlevel = intLock ();

            *MPCC_CAR = ((CD2400_CHAN *)pSioChan)->chan_num; /*  channel */
            rcvBaud = (unsigned char)(*MPCC_RBPR);
	    rcvFreq = (*MPCC_RCOR) & 0x7;  /* masked off unwanted bits */ 

            for (ix = 0; ix < NELEMENTS (baudTable); ix++)
                {
                if ( (rcvBaud == ( (pQusart->baudClkFreq /
                    baudTable [ix].timeConstant) / baudTable [ix].rate) - 1) &&
		    (rcvFreq == baudTable [ix].rcor) )
                    {
                    *(int *)arg = baudTable [ix].rate;
                    status = OK;
                    }
                } 

            intUnlock (oldlevel);

            break;

        case SIO_MODE_SET:
            if (!((int)arg == SIO_MODE_POLL || (int)arg == SIO_MODE_INT))
                {
                status = EIO;
                break;
                }

            /* lock interrupt  */
            oldlevel = intLock();

	    cd2400SetMode ((CD2400_CHAN *)pSioChan, (int)arg);
            ((CD2400_CHAN *)pSioChan)->channelMode = (int)arg;

            intUnlock(oldlevel);

	    status = OK;
	    break;

	case SIO_MODE_GET:
            *(int *)arg = ((CD2400_CHAN *)pSioChan)->channelMode;
            return (OK);

        case SIO_AVAIL_MODES_GET:
            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
            return (OK);


	default:
	    status = ENOSYS;
	    break;
	}

    return (status);
    }

/*******************************************************************************
*
* cd2400IntRx - handle receiver interrupts
*
* This routine handles the interrupts for all channels for a Receive
* Data Interrupt.
*/

void cd2400IntRx
    (
    CD2400_CHAN  *  pChan
    )
    {
    CD2400_QUSART * pQusart = pChan->pQusart;
    char	thisChar;

    if (pChan->created && pChan->putRcvChar )
	{
	thisChar = *MPCC_RDR;		/* grab the character */
	*MPCC_REOIR = EOIR_TRANSFER;	/* clear the interrupt */
	(*pChan->putRcvChar) (pChan->putRcvArg, thisChar);
	}
    else
	*MPCC_REOIR = EOIR_NO_TRANSFER;	/* just clear interrupt */
    }

/*******************************************************************************
*
* cd2400IntTx - handle transmitter interrupts
*
* This routine handles transmitter interrupts from the MPCC.
*/

void cd2400IntTx
    (
    CD2400_CHAN  *  pChan
    )
    {
    CD2400_QUSART * pQusart = pChan->pQusart;
    char 	    outChar;

    /* Tx Buffer Empty */

    if (pChan->created && pChan->getTxChar &&
       ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK))
	{
	*MPCC_TDR = outChar;
	*MPCC_TEOIR = EOIR_TRANSFER;
	}
    else
	{
	/* no more chars to xmit now.  reset the tx int,
	 * so the SCC doesn't keep interrupting us. */

	*MPCC_IER &= ~(IER_TXEMPTY | IER_TXDATA);
	*MPCC_TEOIR = EOIR_NO_TRANSFER;
	}
    }

/*******************************************************************************
*
* cd2400Int - handle special status interrupts
*
* This routine handles special status interrupts from the MPCC.
*/

void cd2400Int
    (
    CD2400_CHAN *   pChan
    )
    {
    CD2400_QUSART * pQusart = pChan->pQusart;

    *MPCC_REOIR = EOIR_NO_TRANSFER;	/* just clear interrupt */
    }

/******************************************************************************
*
* cd2400PTxChar - transmit a character in poll mode.
*
* RETURNS: N/A
*/

static int cd2400PTxChar
    (
    SIO_CHAN *   pSioChan, 
    char 	 ch			/* character to output */
    )
    {
    CD2400_QUSART * pQusart = ((CD2400_CHAN *)pSioChan)->pQusart;

    if (!(*pQusart->txReady)(pQusart))
	return (EAGAIN);

    *MPCC_TDR = ch;                             /* transmit a character    */

    *MPCC_TEOIR = EOIR_TRANSFER;                /* complete the transfer   */

    return(OK);
    }

/******************************************************************************
*
* cd2400PRxChar - wait for and read a character from input.
*
* RETURNS: A character from input.
*
* NOMANUAL
*/

static int cd2400PRxChar
    (
    SIO_CHAN *	pSioChan,
    char *	ch
    )
    {
    CD2400_QUSART * pQusart = ((CD2400_CHAN *)pSioChan)->pQusart;

    if (!(*pQusart->rxReady)(pQusart))
	return (EAGAIN);

    *ch = * MPCC_RDR;                   /* read a character */

    *MPCC_REOIR = EOIR_TRANSFER;        /* complete the transfer */

    return (OK);
    }


/*******************************************************************************
*
* cd2400Startup - transmitter startup routine
*
* Call interrupt level character output routine.
*/

static int cd2400Startup
    (
    SIO_CHAN *  pSioChan		/* device to start up */
    )
    {
    int oldLevel;
    CD2400_QUSART * pQusart;

    oldLevel = intLock ();

    pQusart = ((CD2400_CHAN *)pSioChan)->pQusart;

    if (((CD2400_CHAN *)pSioChan)->channelMode != SIO_MODE_POLL)
	{
        *MPCC_CAR = ((CD2400_CHAN *)pSioChan)->chan_num; /* set up channel */

        /* only need to enable interrupt */

        *MPCC_IER = IER_TXEMPTY	|
                    IER_TXDATA	|
		    IER_RXDATA;
        }

    intUnlock (oldLevel);

    return (OK);
    }

/*************************************************************************
* 
* cd2400SetMode - switch between two modes: SIO_MODE_INT and SIO_MODE_POLL
*
* Since only one channel is allowed when device is in polling mode, the 
* other three channels must be disabled when the device is set to POLL 
* mode; and enabled when the device is set back to INT mode from POLL mode.
*
*/
static void cd2400SetMode
    (
    CD2400_CHAN * pChan,
    int		 mode
    )
    {
    int i;
    int pollChannel;
    CD2400_QUSART * pQusart = pChan->pQusart;

    if (mode == SIO_MODE_POLL)
	{
	if (!pQusart->devMode)  /* device is not in poll mode */
	    {
	    /* enables and disables the respective channels */
            for (i=0;i<pQusart->numChannels;i++)
                {
        	if (pQusart->channel[i].created == TRUE &&
		    pChan->chan_num != i)
	            cd2400ChanIntEnDisable(&pQusart->channel[i], FALSE);
                else if (pChan->chan_num == i)
		    cd2400ChanIntEnDisable(&pQusart->channel[i], TRUE);
                }
		    
            pQusart->devMode = pChan->chan_num + 1;
            } 
        } /* end of if (mode == SIO_MODE_POLL) */
     
    if (mode == SIO_MODE_INT)
        {
        if ( (pChan->chan_num+1) == pQusart->devMode)
	    {
	    /* 
	    ** device is in poll mode
	    ** check if other channels need to be in polling mode 
	    */
            for (i=0;i<pQusart->numChannels;i++)
		{
		if (pQusart->channel[i].created == TRUE &&
		    (pChan->chan_num != i) &&
		    pQusart->channel[i].channelMode == SIO_MODE_POLL )
		    break;
                }
          
	    pollChannel = i;
	    /* no other channels in polling mode, enable interrupts */
	    if (pollChannel == pQusart->numChannels)
                {
                for (i=0;i<pQusart->numChannels;i++)
		    {
		    if (pQusart->channel[i].created == TRUE)
		        cd2400ChanIntEnDisable (&pQusart->channel[i], TRUE);
                    }
                pQusart->devMode = 0;
		}
            else /* There is a channel need to be in polling mode */
                {
		for (i=0;i<pQusart->numChannels;i++)
		    {
                    if (pQusart->channel[i].created == TRUE &&
			i != pollChannel)
                        cd2400ChanIntEnDisable (&pQusart->channel[i], FALSE);
                    else if (i == pollChannel)
			cd2400ChanIntEnDisable (&pQusart->channel[i], TRUE);
		    }
                pQusart->devMode = pollChannel + 1;
                }
            }
        }

      /* enable or disable device interrupt */
      mode = pQusart->devMode ? SIO_MODE_POLL: SIO_MODE_INT;

      if (pChan->intEnDisable)
          (*pChan->intEnDisable)(mode);
    }

/********************************************************************
* 
* cd2400ChanIntEnDisable - enable or disable a channel
*
*/

static void cd2400ChanIntEnDisable
    (
    CD2400_CHAN * pChan,
    int		 intEnable
    )
    {
    CD2400_QUSART * pQusart = pChan->pQusart;

    *MPCC_CAR  = pChan->chan_num;     /* channel number */
    
    while (*MPCC_CCR);
    if (intEnable == TRUE)
        *MPCC_IER = IER_RXDATA |IER_TXEMPTY;
    else
	*MPCC_IER = 0;       /* disable interrupt */
    }

/******************************************************************************
*
* cd2400CallbackInstall - install ISR callbacks to get put chars.
*/

static int cd2400CallbackInstall
    (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS      (*callback)(),
    void *      callbackArg
    )
    {
    CD2400_CHAN * pChan = (CD2400_CHAN *)pSioChan;

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

