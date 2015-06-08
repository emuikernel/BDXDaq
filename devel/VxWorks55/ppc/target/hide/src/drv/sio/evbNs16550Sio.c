/* evbNs16550Sio.c - NS16550 serial driver for the IBM PPC403GA evaluation */
/*  		     board. 						   */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,06nov96,dgp  doc: final formatting
01a,07mar96,tam  written (using i8250Sio.c).
*/

/*
DESCRIPTION
This is the driver for the National NS 16550 UART Chip used on the 
IBM PPC403GA evaluation board.
It uses the SCCs in asynchronous mode only.

USAGE
An EVBNS16550_CHAN structure is used to describe the chip.
The BSP's sysHwInit() routine typically calls sysSerialHwInit()
which initializes all the register values in the EVBNS16550_CHAN structure
(except the SIO_DRV_FUNCS) before calling evbNs16550HrdInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2() which
connects the chip interrupt handler evbNs16550Int() via intConnect().

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as other serial drivers;
for more information, see sioLib.h.

INCLUDE FILES: drv/sio/evbNs16550Sio.h
*/

#include "vxWorks.h"
#include "iv.h"
#include "intLib.h"
#include "errnoLib.h"
#include "drv/sio/evbNs16550Sio.h"


/* locals */

static SIO_DRV_FUNCS evbNs16550SioDrvFuncs;

/* forward declarations */

static void evbNs16550InitChannel (EVBNS16550_CHAN *);
static int  evbNs16550Ioctl(EVBNS16550_CHAN *,int,int);
static int  evbNs16550Startup(SIO_CHAN *pSioChan);
static int  evbNs16550PRxChar (SIO_CHAN *pSioChan,char *ch);
static int  evbNs16550PTxChar(SIO_CHAN *pSioChan,char ch);

/******************************************************************************
*
* evbNs16550CallbackInstall - install ISR callbacks to get put chars.
*/ 

static int evbNs16550CallbackInstall
    (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS      (*callback)(),
    void *      callbackArg
    )
    {
    EVBNS16550_CHAN * pChan = (EVBNS16550_CHAN *)pSioChan;

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


/******************************************************************************
*
* evbNs16550HrdInit - initialize the NS 16550 chip
*
* This routine is called to reset the NS 16550 chip to a quiescent state.
*/

void evbNs16550HrdInit
    (
    EVBNS16550_CHAN *pChan
    )
    {
    (*pChan->outByte) (pChan->ier,0x0);  /* disable interrupts */

    if (evbNs16550SioDrvFuncs.ioctl == NULL)
	{
	evbNs16550SioDrvFuncs.ioctl	= (int (*)()) evbNs16550Ioctl;
	evbNs16550SioDrvFuncs.txStartup	= (int (*)()) evbNs16550Startup;
	evbNs16550SioDrvFuncs.callbackInstall = evbNs16550CallbackInstall;
	evbNs16550SioDrvFuncs.pollInput	= (int (*)()) evbNs16550PRxChar;
	evbNs16550SioDrvFuncs.pollOutput= (int (*)(SIO_CHAN *,char)) 
				     	  evbNs16550PTxChar;
	}

    pChan->pDrvFuncs = &evbNs16550SioDrvFuncs;
    }

/*******************************************************************************
*
* evbNs16550InitChannel  - initialize a single channel
*/

static void evbNs16550InitChannel
    (
    EVBNS16550_CHAN *pChan
    )
    {
    int oldLevel;

    oldLevel = intLock ();

    /* 8 data bits, 1 stop bit, no parity */

    (*pChan->outByte) (pChan->lcr, 0x03);

    /* enable the receiver and transmitter */

    (*pChan->outByte) (pChan->mdc, 0x0b);

    (*pChan->inByte) (pChan->data);	/* clear the port */

    /* enables interrupts  */

    (*pChan->outByte) (pChan->ier,0x1);
    
    intUnlock (oldLevel);
    }

/*******************************************************************************
*
* evbNs16550Ioctl - special device control
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*/

static int evbNs16550Ioctl
    (
    EVBNS16550_CHAN *pChan,	/* device to control */
    int request,		/* request code */
    int arg			/* some argument */
    )
    {
    int status;
    int oldlevel;
    int preset;

    switch (request)
	{
	case SIO_BAUD_SET:
            oldlevel = intLock();
	    preset = (pChan->clkFreq / arg) >> 4;
    	    (*pChan->outByte) (pChan->lcr, (char)0x83);
    	    (*pChan->outByte) (pChan->brdh, MSB (preset));
    	    (*pChan->outByte) (pChan->brdl, LSB (preset));
    	    (*pChan->outByte) (pChan->lcr, 0x03);
            pChan->baudRate = arg;
	    status = OK;
            intUnlock(oldlevel);
	    break;

        case SIO_BAUD_GET:
            *(int *)arg = pChan->baudRate;
            status = OK;
            break;


	case SIO_MODE_SET:
            if (!(arg == SIO_MODE_POLL || arg == SIO_MODE_INT))
                {
                status = EIO;
                break;
                }

            oldlevel = intLock();
           
	    /* only reset the channel if it hasn't done yet */

	    if (!pChan->channelMode)
                evbNs16550InitChannel(pChan);

            if (arg == SIO_MODE_POLL)
                (*pChan->outByte) (pChan->ier, 0x00);
            else
                (*pChan->outByte) (pChan->ier, 0x01);
            
            pChan->channelMode = arg;  
          
            intUnlock(oldlevel);
            status = OK;

	    break;

        case SIO_MODE_GET:
            *(int *)arg = pChan->channelMode;
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
* evbNs16550Int - handle a receiver/transmitter interrupt for the NS 16550 chip
*
* This routine is called to handle interrupts.
* If there is another character to be transmitted, it sends it.  If
* the interrupt handler is called erroneously (for example, if a device
* has never been created for the channel), it
* disables the interrupt.
*/

void evbNs16550Int
    (
    EVBNS16550_CHAN  *pChan 
    )
    {
    char outChar;
    char interruptID;
    char lineStatus;
    int ix = 0;
    
    interruptID = (*pChan->inByte) ( pChan->iid );

    do {

	interruptID &= 0x06;

        if (interruptID == 0x06)
            lineStatus = (*pChan->inByte) (pChan->lst);
        else if (interruptID == 0x04)
	    {
            if (pChan->putRcvChar != NULL)
	        (*pChan->putRcvChar)( pChan->putRcvArg,
                               (*pChan->inByte) (pChan->data) );
	    else
	        (*pChan->inByte) (pChan->data);
	    }
        else if (interruptID == 0x02)
	    {
            if ((pChan->getTxChar != NULL) && 
               (*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK )
	        (*pChan->outByte) (pChan->data, outChar);
            else
	        (*pChan->outByte) (pChan->ier, 0x01);
	    }

        interruptID = (*pChan->inByte) (pChan->iid);

	} while (((interruptID & 0x01) == 0) && (ix++ < 10));
    }

/*******************************************************************************
*
* evbNs16550Startup - transmitter startup routine
*
* Call interrupt level character output routine.
*/

static int evbNs16550Startup
    (
    SIO_CHAN *pSioChan		/* tty device to start up */
    )
    {

    /* enable the transmitter and it should interrupt to write the next char */
    
    if (((EVBNS16550_CHAN *)pSioChan)->channelMode != SIO_MODE_POLL)
        (*((EVBNS16550_CHAN *)pSioChan)->outByte) 
	( ((EVBNS16550_CHAN *)pSioChan)->ier, 0x03);

    return (OK);
    }

/******************************************************************************
*
* evbNs16550PRxChar - poll the device for input.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the input buffer if empty.
*/

static int evbNs16550PRxChar
    (
    SIO_CHAN *pSioChan,
    char *ch
    )
    {
    /* wait for Data */
    if ( ( (*((EVBNS16550_CHAN *)pSioChan)->inByte)
			  (((EVBNS16550_CHAN *)pSioChan)->lst) & 0x01 ) == 0 )
        return(EAGAIN);

    *ch = ( *((EVBNS16550_CHAN *)pSioChan)->inByte) 
	  ( ((EVBNS16550_CHAN *)pSioChan)->data );
    return(OK);
    }

/******************************************************************************
*
* evbNs16550PTxChar - output a character in polled mode.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the output buffer if full.
*/

static int evbNs16550PTxChar
    (
    SIO_CHAN *pSioChan,
    char ch                   /* character to output */
    )
    {

    /* wait for Empty */
    if ( ( (*((EVBNS16550_CHAN *)pSioChan)->inByte)
			  (((EVBNS16550_CHAN *)pSioChan)->lst) & 0x40 ) == 0 )      
        return(EAGAIN);

    (*((EVBNS16550_CHAN *)pSioChan)->outByte) 
    (((EVBNS16550_CHAN *)pSioChan)->data, ch);
    return(OK);
    }

