/* mb86940Sio.c - MB 86940 UART tty driver */

/* Copyright 1992-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,09nov95,jdi  doc: style cleanup.
01a,31jul95,myz  written(based on mb86940Serial.c) 
*/

/*
DESCRIPTION
This is the driver for the SPARClite MB86930 on-board serial ports.

USAGE
A MB86940_CHAN structure is used to describe the chip. 

The BSP's sysHwInit() routine typically calls sysSerialHwInit(),
which initializes all the values in the MB86940_CHAN structure (except
the SIO_DRV_FUNCS) before calling mb86940DevInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(), which
connects the chips interrupts via intConnect(). 

IOCTL FUNCTIONS
The UARTs use timer 3 output to generate the following baud rates:
110, 150, 300, 600, 1200, 2400, 4800, 9600, and 19200.
Note that the UARTs will operate at the same baud rate.

INCLUDE FILES: drv/sio/mb86940Sio.h 
*/

#include "vxWorks.h"
#include "sioLib.h"
#include "intLib.h"
#include "errno.h"
#include "drv/sio/mb86940Sio.h"

/* forward declarations */

static int    mb86940Startup (MB86940_CHAN *);
static int    mb86940Ioctl (MB86940_CHAN *,int,int);
static int    mb86940CallbackInstall(SIO_CHAN *,int,STATUS (*)(),void *);
static int    mb86940PTxChar (MB86940_CHAN *,char);
static int    mb86940PRxChar (MB86940_CHAN *,char *);


/* local driver function table */

static SIO_DRV_FUNCS mb86940SioDrvFuncs = 
    {
    (int (*)())mb86940Ioctl,
    (int (*)())mb86940Startup,
    mb86940CallbackInstall,
    (int (*)())mb86940PRxChar,
    (int (*)(SIO_CHAN *,char))mb86940PTxChar
    };


/******************************************************************************
*
* mb86940CallbackInstall - install ISR callbacks to get put chars.
*/

static int mb86940CallbackInstall
    (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS      (*callback)(),
    void *      callbackArg
    )
    {
    MB86940_CHAN * pChan = (MB86940_CHAN *)pSioChan;

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
* mb86940DevInit - install the driver function table
*
* This routine installs the driver function table.  It also
* prevents the serial channel from functioning by disabling the interrupt.
*
* RETURNS: N/A
*
*/

void mb86940DevInit
    (
    MB86940_CHAN *pChan
    )
    {
    int delay;
    volatile int bump;  /* for something to do */

    /* disables the serial channel interrupts */ 
                 
    (*pChan->intDisable)(pChan->rxIntLevel);
    (*pChan->intDisable) (pChan->txIntLevel);

    /* install device driver function table */

    pChan->pDrvFuncs = &mb86940SioDrvFuncs;

    /* Initialization sequence to put the SDTR in a known reset state */
    /* From Fujitsu code */

    (*pChan->asiSeth) ((void *) (pChan->cp), NULL);
    (*pChan->asiSeth) ((void *) (pChan->cp), NULL);
    (*pChan->asiSeth) ((void *) (pChan->cp), NULL);

    (*pChan->asiSeth) ((void *) (pChan->cp), FCC_SDTR_RESET);

    for (delay=0; delay < 10000; delay++)
        bump++;

    /* after reset only the first access is the MODE register */
    (*pChan->asiSeth) ((void *) (pChan->cp),   FCC_SDTR_1_BIT    |
                                               FCC_SDTR_ODD      |
                                               FCC_SDTR_NOPARITY |
                                               FCC_SDTR_8BITS    |
                                               FCC_SDTR_1_16XCLK);

    (*pChan->baudSet)(pChan->baudRate);  /* set default baud rate */

    (*pChan->asiSeth) ((void *) (pChan->cp), FCC_TX_START);

    }

/******************************************************************************
*
* mb86940Ioctl - special device control
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*
* NOTE:  Both serial ports use the output of timer 3 for the baud clock.
* Therefore, both ports will be set for the same baud rate.
*/

static STATUS mb86940Ioctl
    (
    MB86940_CHAN *pChan,     /* device to control */
    int request,	     /* request code */
    int arg		     /* some argument */
    )
    {
    int oldlevel;
    STATUS status = EIO;

    switch (request)
	{
	case SIO_BAUD_SET:

            oldlevel = intLock();
	    if ( (*pChan->baudSet) (arg) == OK )
		{
		status = OK;
		pChan->baudRate = arg;
		}
            intUnlock(oldlevel);
	    break;

        case SIO_BAUD_GET:

            *(int *)arg = pChan->baudRate;
            status = OK;

            break;

        case SIO_MODE_SET:
            if (!(arg == SIO_MODE_POLL || arg == SIO_MODE_INT))
                break;

            oldlevel = intLock();

            /* set to either POLL mode or INT mode */

            if (arg == SIO_MODE_POLL)
		{
                (*pChan->intDisable) (pChan->rxIntLevel);
		(*pChan->intDisable) (pChan->txIntLevel);
		}
            else
		{
                (*pChan->intEnable) (pChan->rxIntLevel);
		(*pChan->intEnable) (pChan->txIntLevel);
		}

            pChan->channelMode = arg;

            intUnlock(oldlevel);
            status = OK;

            break;

        case SIO_MODE_GET:
            *(int *)arg = pChan->channelMode;
            status = OK;
            break;

        case SIO_AVAIL_MODES_GET:
            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
            status = OK;
            break; 

        default:
            status = ENOSYS;
            break;
        }

    return (status);
 
    }

/******************************************************************************
*
* mb86940IntRx - handle a receiver interrupt
*
* RETURNS: N/A
*
* NOMANUAL
*/

void mb86940IntRx
    (
    MB86940_CHAN *pChan 
    )
    {
    char ch;

    ch = (char) (*pChan->asiGeth)((void *)(pChan->dp));

    (*pChan->putRcvChar)(pChan->putRcvArg,ch);
    }
/******************************************************************************
*
* mb86940IntTx - handle a transmitter interrupt
*
* If there is another character to be transmitted, it sends it.  If
* not, we just simply exit.        
*
* RETURNS: N/A
*
* NOMANUAL
*/

void mb86940IntTx
    (
    MB86940_CHAN *pChan 
    )
    {
    char outChar;

    if ( ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK) )
	(*pChan->asiSeth)((void *)(pChan->dp),outChar);
    }
/******************************************************************************
*
* mb86940Startup - transmitter startup routine
*
* Call interrupt level character output routine for SPARClite UART.
*/

static int mb86940Startup
    (
    MB86940_CHAN *pChan		/* ty device to start up */
    )
    {
    char outChar;

    /*
     * if a character is available send it, the IRQ
     * routine will send the rest.
     */
    if ((pChan->channelMode == SIO_MODE_INT) &&
	( (*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK) )
	(*pChan->asiSeth) ((void *)(pChan->dp), outChar);
    return (OK);
    }

/******************************************************************************
*
* mb86940PTxChar - output a character in polled mode.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the output buffer if full.
*/

static int mb86940PTxChar 
    (
    MB86940_CHAN * pChan,
    char ch
    )
    {
    /* wait for Tx empty */

    if ( ((*pChan->asiGeth) ((void *)pChan->cp) & FCC_SDTR_STAT_TRDY) == 0 )
        return (EAGAIN);
       
    (*pChan->asiSeth) ((void *)pChan->dp,ch);
    return (OK); 

    }

/******************************************************************************
*
* mb86940PRxChar - poll the device for input.
*
* RETURNS: OK if a character arrived, ERROR on device error, EAGAIN
*          if the input buffer if empty.
*/

static int mb86940PRxChar 
    (
    MB86940_CHAN * pChan,
    char *ch
    )
    {

    /* check for Rx char */

    if ( !( (*pChan->asiGeth)((void *)pChan->cp) & FCC_SDTR_STAT_RRDY) )
        return(EAGAIN);

    *ch = (*pChan->asiGeth)((void *)pChan->dp);
    return(OK);
    }


