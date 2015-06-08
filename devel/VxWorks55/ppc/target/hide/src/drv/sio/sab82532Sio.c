/* sab82532.c - Siemens SAB 82532 UART tty driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01b,24apr02,pmr  SPR 75161: returning int from sab82532TxStartup().
01a,12jan98,mem  written.
*/

/*
DESCRIPTION
This is the device driver for the sab82532 (D)UART.

USAGE
A SAB82532_CHAN data structure is used to describe each channel
on the chip.

The BSP's sysHwInit() routine typically calls sysSerialHwInit(),
which initializes all the values in the SAB82532_CHAN structure (except
the SIO_DRV_FUNCS) before calling sab82532DevInit().
The BSP's sysHwInit2() routine typically calls sysSerialHwInit2(), which
connects the chips interrupts via intConnect().

INCLUDE FILES: drv/sio/ns16552Sio.h

*/

#include "vxWorks.h"
#include "intLib.h"
#include "errnoLib.h"
#include "errno.h"
#include "sioLib.h"
#include "drv/sio/sab82532Sio.h"

/* local defines       */

/* min/max baud rate */

#define SAB82532_MIN_RATE 50
#define SAB82532_MAX_RATE 115200

#define REG_WR(pchan,reg,val) \
 (*(volatile UINT8 *)((pchan)->regs + (reg)) = (val))

#define REG_RD(pchan,reg) \
 (*(volatile UINT8 *)((pchan)->regs + (reg)))

/* function prototypes */

static int sab82532CallbackInstall (SIO_CHAN *,int,STATUS (*)(),void *);
static STATUS sab82532DummyCallback ();
static void sab82532InitChannel (SAB82532_CHAN *);
static STATUS sab82532Ioctl (SAB82532_CHAN *,int,int);
static int sab82532TxStartup (SAB82532_CHAN *);
static int sab82532PollOutput (SAB82532_CHAN *,char);
static int sab82532PollInput (SAB82532_CHAN *,char *);

/* driver functions */

static SIO_DRV_FUNCS sab82532SioDrvFuncs =
    {
    (int (*)())sab82532Ioctl,
    (int (*)())sab82532TxStartup,
    (int (*)())sab82532CallbackInstall,
    (int (*)())sab82532PollInput,
    (int (*)(SIO_CHAN *,char))sab82532PollOutput
    };

/******************************************************************************
*
* sab82532DummyCallback - dummy callback routine.
*/

static STATUS sab82532DummyCallback (void)
    {
    return (ERROR);
    }

/******************************************************************************
*
* sab82532DevInit - intialize an SAB82532 channel
*
* This routine initializes some SIO_CHAN function pointers and then resets
* the chip in a quiescent state.  Before this routine is called, the BSP
* must already have initialized all the device addresses, etc. in the
* SAB82532_CHAN structure.
*
* RETURNS: N/A
*/

void sab82532DevInit
    (
    SAB82532_DUART * pDuart
    )
    {
    int i;
    int oldlevel = intLock();

    /* initialize the driver function pointers in the SIO_CHAN's */

    for (i = 0; i < 2; ++i)
	{
	pDuart->channel[i].pDrvFuncs    = &sab82532SioDrvFuncs;

	/* set the non BSP-specific constants */
	pDuart->channel[i].getTxChar    = sab82532DummyCallback;
	pDuart->channel[i].putRcvChar   = sab82532DummyCallback;
	pDuart->channel[i].channelMode  = 0;    /* undefined */

	/* reset the chip */
	sab82532InitChannel(&pDuart->channel[i]);
	}

    intUnlock(oldlevel);
    }

/******************************************************************************
*
* sab82532InitChannel - initialize UART
*
* XXX - additional description
*/

static void sab82532InitChannel
    (
    SAB82532_CHAN *pChan
    )
    {
    int n;

    n = (pChan->xtal / (16 * 2 * pChan->baudRate)) - 1;
    REG_WR(pChan, SAB82532_CCR0, 0x80 | 0x40 | 0x03);

    /* command reg/reset rcv/xmit fifos */
    REG_WR(pChan, SAB82532_CMDR, 0x40 | 0x01);
    /* config reg 1/clk mode 7b/bit clk rate/psh|pull */
    REG_WR(pChan, SAB82532_CCR1, 0x10 | 0x08 | 0x07);
    /* config reg 4/crystal runs > 10MHz, so set this*/
    REG_WR(pChan, SAB82532_CCR4, 0x80);
    /* mode cntl reg/enable receiver/define RTS */
    REG_WR(pChan, SAB82532_MODE, 0x20 | 0x08 | 0x04);
    /* receive byte count high reg/ 0*/
    REG_WR(pChan, SAB82532_XBCH, 0x00);
    /* imr0/mask all interrupts */
    REG_WR(pChan, SAB82532_IMR0, 0xff);
    /* imr1/mask all interrupts */
    REG_WR(pChan, SAB82532_IMR1, 0xff);
    /* int port config reg/masked interrupts are visible, active low */
    REG_WR(pChan, SAB82532_IPC, 0x81);
    /* data format reg: 8N1 */
    REG_WR(pChan, SAB82532_DAFO, 0x00);
    /* read fifo cntl reg: read fifo threshold = 1char */
    REG_WR(pChan, SAB82532_RFC, 0x40 | 0x00);
    /* command reg:reset the rcv/xmit fifos*/
    REG_WR(pChan, SAB82532_CMDR, 0x40 | 0x01);
	
    /* Try to toggle RTS */
    /* cts RED */
    REG_WR(pChan, SAB82532_MODE, 0x00 | 0x20 | 0x08 | 0x04);	/* RTS low */

    /* Clk mode 7, BCR set, ODS set */
    REG_WR(pChan, SAB82532_CCR1, 0x10 | 0x08 | 0x07);

    /* Set baud rate */
    REG_WR(pChan, SAB82532_BGR,  n & 0xff);
    /* BDF set, SSEL set, TOE set */
    REG_WR(pChan, SAB82532_CCR2, 0x20 | 0x10 | 0x08
	     | (n >> 8) << 6);

    REG_WR(pChan, SAB82532_IVA, 0x08);
    }

/******************************************************************************
*
* sab82532Ioctl - special device control
*
* RETURNS: OK on success, EIO on device error, ENOSYS on unsupported
*          request.
*/

static STATUS sab82532Ioctl
    (
    SAB82532_CHAN *pChan,	/* device to control */
    int        request,		/* request code */
    int        arg		/* some argument */
    )
    {
    int n;		/* temp for baud rate calculations */
    int oldlevel;	/* current interrupt level mask */
    STATUS status;

    status = OK;

    switch (request)
	{
	case SIO_BAUD_SET:
	    if (arg < SAB82532_MIN_RATE || arg > SAB82532_MAX_RATE)
	        {
		status = EIO;		/* baud rate out of range */
		break;
	        }
	    /* disable interrupts during chip access */
	    oldlevel = intLock ();
	    pChan->baudRate = arg;
	    n = (pChan->xtal / (16 * 2 * pChan->baudRate)) - 1;
	    /* Set baud rate */
	    REG_WR(pChan, SAB82532_BGR,  n & 0xff);
	    /* BDF set, SSEL set, TOE set */
	    REG_WR(pChan, SAB82532_CCR2, 0x20 | 0x10 | 0x08
		   | (n >> 8) << 6);
	    intUnlock (oldlevel);
	    break;

        case SIO_BAUD_GET:
            *(int *)arg = pChan->baudRate;
            break; 

        case SIO_MODE_SET:
            if ((arg != SIO_MODE_POLL) && (arg != SIO_MODE_INT))
                {
                status =  EIO;
                break;
                }
            oldlevel = intLock ();
            if (arg == SIO_MODE_INT)
		{
                /* Enable appropriate interrupts */
		/* XXX */
		REG_WR(pChan, SAB82532_IMR0, ~0x43);
		REG_WR(pChan, SAB82532_IMR1, ~0x21);
		/* XXX */
		REG_WR(pChan, SAB82532_IMR0, 0x00);
		REG_WR(pChan, SAB82532_IMR1, 0x00);
		}
            else
		{
                /* Disable the interrupts */ 
		REG_WR(pChan, SAB82532_IMR0, 0xff);
		REG_WR(pChan, SAB82532_IMR1, 0xff);
		}
	    /* enable receiver */
	    REG_WR(pChan, SAB82532_MODE,
		   REG_RD(pChan, SAB82532_MODE) | 0x08);
            pChan->channelMode = arg;
            intUnlock(oldlevel);
            break;          

        case SIO_MODE_GET:
            *(int *)arg = pChan->channelMode;
            break;

        case SIO_AVAIL_MODES_GET:
	    /* XXX - polled mode not supported yet */
            *(int *)arg = SIO_MODE_INT;
            break;

        case SIO_HW_OPTS_SET:
        case SIO_HW_OPTS_GET:
        default:
            status = ENOSYS;
	}
    return (status);
    }

/*****************************************************************************
*
* sab82532Int - interrupt level processing
*
* This routine handles interrupts from the UART.
*
* RETURNS: N/A
*
*/

void sab82532Int 
    (
    SAB82532_DUART *pDuart
    )
    {
    int i;

    for (i = 0; i < 2; ++i)
	{
	char ch;
	int stat0, stat1;
	SAB82532_CHAN *pChan = &pDuart->channel[i];

	stat0 = REG_RD(pChan, SAB82532_ISR0);
	stat1 = REG_RD(pChan, SAB82532_ISR1);

	if (stat0 & 0x43)
	    {
	    /* receive fifo overflow. */
	    /* receive pool full. */
	    /* time out from last char received. */

	    /* Send RFRD */
	    REG_WR(pChan, SAB82532_CMDR, 0x20);
	    if (REG_RD(pChan, SAB82532_STAR) & 0x20)
		{
		/* get char */
		ch = REG_RD(pChan, SAB82532_RFIFO);
		(*pChan->putRcvChar) (pChan->putRcvArg, ch);
		}
	    /* Send RMC */
	    REG_WR(pChan, SAB82532_CMDR, 0x80);
	    }
	if (stat1 & 0x20)
	    {
	    /* all sent interrupt */
            if ((*pChan->getTxChar) (pChan->getTxArg, &ch) != ERROR)
		{
		while (REG_RD(pChan, SAB82532_STAR) & 0x08)
		    ;
		REG_WR(pChan, SAB82532_TIC, ch);
		}
	    }
	}
    }

/******************************************************************************
*
* sab82532TxStartup - transmitter startup routine
*
* Call interrupt level character output routine and enable interrupt!
*/

static int sab82532TxStartup
    (
    SAB82532_CHAN *pChan 
    )
    {
    char outChar;

    if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) != ERROR)
	{
	while (REG_RD(pChan, SAB82532_STAR) & 0x08)
	    ;
	REG_WR(pChan, SAB82532_TIC, outChar);
	}

    return (OK);
    }

/******************************************************************************
*
* sab82532PollOutput - output a character in polled mode.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*          if the output buffer if full.
*/

static int sab82532PollOutput
    (
    SAB82532_CHAN *  pChan,
    char            outChar
    )
    {
    /* is the transmitter ready to accept a character? */
    while (REG_RD(pChan, SAB82532_STAR) & 0x08)
        return (EAGAIN);

    /* write out the character */
    REG_WR(pChan, SAB82532_TIC, outChar);
    return (OK);
    }

/******************************************************************************
*
* sab82532PollInput - poll the device for input.
*
* RETURNS: OK if a character arrived, EIO on device error, EAGAIN
*          if the input buffer if empty.
*/

static int sab82532PollInput
    (
    SAB82532_CHAN *  pChan,
    char *          thisChar
    )
    {
    /* XXX - */

    /* got a character */
    *thisChar = ' ';
    return (OK);
    }

/******************************************************************************
*
* sab82532CallbackInstall - install ISR callbacks to get/put chars.
*/

static int sab82532CallbackInstall
    (
    SIO_CHAN *  pSioChan,
    int         callbackType,
    STATUS      (*callback)(),
    void *      callbackArg
    )
    {
    SAB82532_CHAN * pChan = (SAB82532_CHAN *)pSioChan;

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
