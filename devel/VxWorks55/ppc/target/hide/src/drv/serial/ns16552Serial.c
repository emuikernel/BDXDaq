/* ns16552Serial.c - ns16552 serial tty handler */

 /* Copyright 1984-1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02d,17aug95,kkk  undo some of 02c changes
02c,09aug95,kkk  fix warnings for I960 (spr# 4177)
02b,01dec92,jdi  NOMANUAL for tyCoInt() - SPR 1808.
02a,21oct92,ccc  renamed to ns16552Serial.c, ansified.
01e,28jul92,wmd  fix ansi warnings.
01d,18jul92,smb  Changed errno.h to errnoLib.h.
01c,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01b,27apr92,wmd  updated with documentation cleanup (FCS), fixed ansi
                 warnings.
01a,04feb91,swb  written, by modifying (massively) hkv960 version
*/

/*
DESCRIPTION
This is the driver for the Intel EV80960SX board on-board ns16552 serial ports.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly: tyCoDrv() to
initialize the driver and tyCoDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling the
routine tyCoDrv().  This routine should be called exactly once, before any
reads, writes, or calls to tyCoDevCreate().  Normally, it is called by
usrRoot() in usrConfig.c.

Before a terminal can be used, it must be created, using tyCoDevCreate().
Each port to be used should have exactly one device associated with it by
calling this routine.

IOCTL FUNCTIONS
This driver responds to all the same ioctl() codes as a normal tty driver;
for more information, see the manual entry for tyLib.  Available baud
rates range from 50 to 38400.

SEE ALSO:
tyLib
*/

#include "vxWorks.h"
#include "errnoLib.h"
#include "intLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "logLib.h"
#include "tyLib.h"
#include "vxLib.h"
#include "config.h"
#include "drv/serial/ns16552.h"
#ifdef EVSX_USE_8259
#include "i8259a.h"
#endif

#define SERIAL_REG_ALIGN 2
#define UART(base, register) ((volatile UINT8*)(base+register*SERIAL_REG_ALIGN))
#define eat_time(n) {register volatile int delay=n*5000; while (--delay > 0);}

IMPORT TY_CO_DEV tyCoDv [];    /* device descriptors */

IMPORT int tyRead ();
IMPORT int tyWrite ();
int tyCoRead ();
int tyCoWrite ();

LOCAL int tyCoDrvNum;           /* driver number assigned to this driver */

/* forward declarations */

void tyCoHrdInit ();
LOCAL void tyCoStartup ();
LOCAL int tyCoOpen ();
LOCAL STATUS tyCoIoctl ();
void tyCoInt ();
LOCAL void tyCoTxInt ();
LOCAL void tyCoRxInt ();
LOCAL void tyImrSet (volatile unsigned char * imr_addr,
		     volatile unsigned char * imr,
		     char setBits, char clearBits);

/******************************************************************************
*
* tyCoDrv - initialize the tty driver
*
* This routine initializes the serial driver, sets up interrupt vectors,
* and performs hardware initialization of the serial ports.
*
* This routine should be called exactly once, before any reads, writes, or
* calls to tyCoDevCreate().  Normally, it is called by usrRoot() in
* usrConfig.c.
*
* RETURNS: OK, or ERROR if the driver cannot be installed.
*
* SEE ALSO: tyCoDevCreate()
*/

STATUS tyCoDrv (void)
    {
    tyCoHrdInit ();

    tyCoDrvNum = iosDrvInstall (tyCoOpen, (FUNCPTR) NULL, tyCoOpen,
                                (FUNCPTR) NULL, tyRead, tyWrite, tyCoIoctl);

    return (tyCoDrvNum == ERROR ? ERROR : OK);
    }

/******************************************************************************
*
* tyCoDevCreate - create a device for an on-board serial port
*
* This routine creates a device for a specified serial port.  Each port
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device "/tyCo/0", with buffer sizes of 512 bytes,
* the proper call would be:
* .CS
*     tyCoDevCreate ("/tyCo/0", 0, 512, 512);
* .CE
*
* RETURNS: OK, or ERROR if the driver is not installed, the channel is invalid,
* or the device already exists.
*
* SEE ALSO: tyCoDrv()
*/

STATUS tyCoDevCreate
    (
    char * name,         /* Name to use for this device               */
    int  channel,        /* Physical channel for this device (0 or 1) */
    int  rdBufSize,      /* Read buffer size, in bytes                */
    int  wrtBufSize      /* Write buffer size, in bytes               */
    )
    {
    FAST TY_CO_DEV *pTyCoDv;

    if (tyCoDrvNum <= 0)
        {
        errnoSet (S_ioLib_NO_DRIVER);
        return (ERROR);
        }

    pTyCoDv = &tyCoDv [channel];

    /* if there is a device already on this channel, don't do it */

    if (pTyCoDv->created)
        return (ERROR);

    /* initialize the ty descriptor, and turn on the bit for this
     * receiver and transmitter in the interrupt mask */

    if (tyDevInit (&pTyCoDv->tyDev,
		   rdBufSize, wrtBufSize, (FUNCPTR)tyCoStartup) != OK)
        return (ERROR);

    tyImrSet (UART(pTyCoDv->regs, IER), &pTyCoDv->ier, Rx_BIT | RxFIFO_BIT, 0);

    /* Mark the device created, add the device to the I/O system */

    pTyCoDv->created = TRUE;

    return (iosDevAdd (&pTyCoDv->tyDev.devHdr, name, tyCoDrvNum));
    }

/******************************************************************************
*
* tyCoHrdInit - initialize the USART
*
* This routine initializes the NS16552 for the Vx960 environment.
*
* NOMANUAL
*/

#define eat_time(n) {register volatile int delay=n*5000; while (--delay > 0);}

void tyCoHrdInit (void)
    {
    int ix;
    UINT8 *regs;
    int oldlevel;          		/* current interrupt level mask */

    oldlevel = intLock ();      	/* disable interrupts during init */

    for (ix= 0; ix < NUM_TTY; ix++)
        {
        regs = tyCoDv[ix].regs;

	*UART(regs, IER) = NULL;	/* disable UART interrupts for now */
        tyCoDv [ix].ier = NULL;
	*UART(regs, LCR) = CHAR_LEN_8 | PARITY_NONE | ONE_STOP;
	eat_time(1);
	*UART(regs, MCR) = DTR;		/* enable DTR */
	eat_time(1);
	*UART(regs, FCR) = FIFO_ENABLE | RxCLEAR | TxCLEAR; /* reset fifo */
	eat_time(1);

	*UART(regs, LCR) |= DLAB;	/* let BAL/BAH show up at data/ier */
	*UART(regs, RBR)  = BAUD_LO(9600);	/* 9600 baud low */
	*UART(regs, IER)  = BAUD_HI(9600);	/* 9600 baud high */
	*UART(regs, LCR) &= ~DLAB;	/* hide BAL/BAH, revert to data/ier */
	eat_time(1);
	}

        /* all interrupts are masked out: the receiver interrupt will be
           enabled in tyCoDevCreate */

    intUnlock (oldlevel);
    }

/******************************************************************************
*
* tyCoOpen - open file to USART
*/

LOCAL int tyCoOpen
    (
    TY_CO_DEV * pTyCoDv,
    char * name,
    volatile int mode
    )
    {
    return ((int) pTyCoDv);
    }

/******************************************************************************
*
* tyCoIoctl - special device control
*
* This routine handles baud rate requests, and passes all other requests
* to tyIoctl.
*
* RETURNS:
*    ERROR if unsupported baud rate, or
*    whatever tyIoctl returns for other requests.
*/

LOCAL STATUS tyCoIoctl
    (
    TY_CO_DEV * pTyCoDv, /* device to control */
    int request,         /* request code */
    int arg              /* some argument */
    )
    {
    STATUS status;

    switch (request)
        {
        case FIOBAUDRATE:
            if (arg <= 38400 && arg >= 50)               /* Nothing strange */
                {
		*UART(pTyCoDv->regs, LCR) |= DLAB;	/* BAL/BAH at data/ier*/
		*UART(pTyCoDv->regs, RBR)  = BAUD_LO(arg);
		*UART(pTyCoDv->regs, IER)  = BAUD_HI(arg);
		*UART(pTyCoDv->regs, LCR) &= ~DLAB;	/* revert to data/ier */

                status = OK;
                break;
                }
	    else
                status = ERROR;
            break;

        default:
            status = tyIoctl (&pTyCoDv->tyDev, request, arg);
            break;
        }
    return (status);
    }

/******************************************************************************
*
* tyCoInt - handles all interrupts, calls appropriate subroutines
*
* NOMANUAL
*/

void tyCoInt
    (
    int channel
    )
    {
    UINT8 vector;
    volatile UINT8 status;

    /* service all pending interrupts on this port.  IP active LOW. */
    while (((vector = *UART(tyCoDv[channel].regs, IIR)) & IIR_IP) == 0)
	{
	/* service the highest priority interrupt, */
	/* the vector of which was read from IIR ... */
	switch (vector & IIR_ID)
	    {
	    case IIR_TIMEOUT:	/* character timeout indication */
	    case IIR_RDA:	/* read data available */
		tyCoRxInt (channel);
		break;

	    case IIR_THRE:	/* transmitter holding registers empty */
		tyCoTxInt (channel);
		break;

	    case IIR_RLS:	/* receive line status, brk/prty/frmng/ovrrn */
		status = *UART(tyCoDv[channel].regs, LSR); /* clears interrupt*/
		logMsg("unexpected 16552 error interrupt, channel %d, vec %d\n",
			channel, vector, 0, 0, 0, 0);
		break;

	    case IIR_MSTAT:
		status = *UART(tyCoDv[channel].regs, MSR); /* clr modem status*/
		logMsg("unexpected 16552 error interrupt, channel %d, vec %d\n",
			channel, vector, 0, 0, 0, 0);
		break;

	    default:
		/* UNKNOWN? */
		logMsg("unknown 16552 interrupt, channel %d, vec %d\n",
			channel, vector, 0, 0, 0, 0);
		break;
	    }
	}

#ifdef EVSX_USE_8259
    if (tyCoDv[channel].level)
	sys8259IntLevelAck(tyCoDv[channel].level); /* acknowledge interrupt */
#endif
    }

/******************************************************************************
*
* tyCoTxInt - handle a transmitter interrupt
*
* This routine gets called to handle transmitter interrupts.
* If there is another character to be transmitted, it sends it.  If
* not, or if a device has never been created for this channel, we just
* disable the interrupt.
*/

LOCAL void tyCoTxInt
    (
    int channel
    )
    {
    char outChar;
    FAST TY_CO_DEV *pTyCoDv = &tyCoDv[channel];

    if (pTyCoDv->created && tyITx (&pTyCoDv->tyDev, &outChar) == OK)
        *UART(pTyCoDv->regs, THR) = outChar;
    else /* turn off transmitter interrupt */
        tyImrSet (UART(pTyCoDv->regs, IER), &pTyCoDv->ier, 0, TxFIFO_BIT);
    }

/******************************************************************************
*
* tyCoRxInt - handle a receiver interrupt
*
* This routine gets called to handle receiver interrupts.
*/

LOCAL void tyCoRxInt
    (
    int channel
    )
    {
    tyIRd (&tyCoDv [channel].tyDev, *UART(tyCoDv [channel].regs, RBR));
    }

/******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* Call interrupt level character output routine.
*/

LOCAL void tyCoStartup
    (
    TY_CO_DEV * pTyCoDv		/* ty device to start up */
    )
    {
    char outChar;

    /* write the next char out */

    if (tyITx ((TY_DEV_ID) pTyCoDv, &outChar) == OK)
	{
        *UART(pTyCoDv->regs, THR) = outChar;
        tyImrSet (UART(pTyCoDv->regs, IER), &pTyCoDv->ier,
              TxFIFO_BIT, 0); /* enable transmitter interrupt */
	}
    }

/******************************************************************************
*
* tyImrSet - sets and clears bits in the USART's int mask reg
*
* This routine sets and clears bits in a local copy of the imr, then
* writes that local copy to the USART.  So all changes to the register
* must go through this routine.  Otherwise, any direct changes to the
* imr would be lost the next time this routine is called.
*/

LOCAL void tyImrSet
    (
    volatile unsigned char * imr_addr,	/* physical address of imr on USART */
    volatile unsigned char * imr,	/* pointer to the current imr state */
    char setBits,       		/* which bits to set in the imr */
    char clearBits	     		/* which bits to clear in the imr */
    )
    {
    * imr = (* imr | setBits) & (~ clearBits);
    * imr_addr = * imr;
    }

