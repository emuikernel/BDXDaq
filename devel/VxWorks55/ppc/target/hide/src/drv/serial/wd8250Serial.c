/* wd8250Serial.c - Western Digital 8250 serial driver */

/* Copyright 1984-1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01s,05may93,caf  tweaked for ansi.
01r,01dec92,jdi  NOMANUAL for tyCoInt() - SPR 1808.
01q,28oct92,caf  tweaked tyCoDevCreate() documentation and parm checking.
01p,02oct92,caf  derived from version 01o of star/tyCoDrv.c.
01o,15sep92,caf  made board-independent.  removed ARGSUSED and ttyStatus().
                 changed "recieve" to "receive".  added delay after setting
                 baud rate in tyCoIoctl() to satisfy validation suite.
                 ansified.
01n,18jul92,smb  Changed errno.h to errnoLib.h.
01m,09jun92,ajm  5.1 merge
01l,21jan92,gae  disabled transmitter in tyClrInts() instead of uselessly
		   re-reading status register.
01k,14jan92,jdi  documentation cleanup.
01j,06jan92,ajm  updated for FCS.
01i,06nov91,ajm  fixed tyCoInt to handle ints in prio order, and clear error
		  conditions more gracefully.
01i,11oct91,jdi  documentation cleanup.
01h,16jul91,ajm  added routine to merely clear all UART ints per channel
01g,06jun91,ajm  fixed intHandler to read rec/trans buffer on error conditions
01f,28may91,ajm  fixed pointer problem in tyCoInt routine
01e,25may91,gae  WRS-ized.
01d,30aug90,ajm  fixed tyCoDetermine to use MASK thus correctly clearing
		  UART error conditions (DOPE!!)
01c,07aug90,ajm  made tyCoHrdInit global for use by prom exception handler
01b,25apr90,ajm  added tyCoDetermine to intConnects.  This routine determines
		  whether we have a reciever or transmit interrupt.
01a,23apr90,ajm  written.
*/

/*
DESCRIPTION
This is the driver for the Western Digital 8250 serial chip.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling the
routine tyCoDrv().  This routine should be called exactly once, before any
reads, writes, or calls to tyCoDevCreate().  Normally, it is called by
usrRoot() in usrConfig.c.

Before a terminal can be used, it must be created, using tyCoDevCreate().
Each port to be used should have exactly one device associated with it by
calling this routine.

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal tty driver; for more
information, see the manual entry for tyLib.

SEE ALSO
tyLib
*/

#include "vxWorks.h"
#include "errnoLib.h"
#include "iv.h"
#include "ioLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "tyLib.h"
#include "config.h"
#include "drv/serial/wd8250.h"

#define DEFAULT_BAUD    9600            /* default uart baud rate */

typedef struct			/* BAUD */
    {
    int rate;		/* a baud rate */
    char msbyte;	/* most significant byte to write to the lc register */ 
    char lsbyte;	/* least significant byte to write to the ie register */
    } BAUD;

IMPORT TY_CO_DEV tyCoDv [];


/* baudTable is a table of the available baud rates, and the values to write
   to the csr reg to get those rates */

LOCAL BAUD baudTable [] =
    {
    {50,    MSB(WD_BAUD_50),    LSB(WD_BAUD_50) },
    {75,    MSB(WD_BAUD_75),    LSB(WD_BAUD_75) },
    {110,   MSB(WD_BAUD_110),   LSB(WD_BAUD_110)},
    {134,   MSB(WD_BAUD_134_5), LSB(WD_BAUD_134_5)},
    {150,   MSB(WD_BAUD_150),   LSB(WD_BAUD_150)},
    {300,   MSB(WD_BAUD_300),   LSB(WD_BAUD_300)},
    {600,   MSB(WD_BAUD_600),   LSB(WD_BAUD_600)},
    {1200,  MSB(WD_BAUD_1200),  LSB(WD_BAUD_1200)},
    {1800,  MSB(WD_BAUD_1800),  LSB(WD_BAUD_1800)},
    {2000,  MSB(WD_BAUD_2000),  LSB(WD_BAUD_2000)},
    {2400,  MSB(WD_BAUD_2400),  LSB(WD_BAUD_2400)},
    {4800,  MSB(WD_BAUD_4800),  LSB(WD_BAUD_4800)},
    {7200,  MSB(WD_BAUD_7200),  LSB(WD_BAUD_7200)},
    {9600,  MSB(WD_BAUD_9600),  LSB(WD_BAUD_9600)},
    {19200, MSB(WD_BAUD_19200), LSB(WD_BAUD_19200)},
    {38400, MSB(WD_BAUD_38400), LSB(WD_BAUD_38400)},
    {56000, MSB(WD_BAUD_56000), LSB(WD_BAUD_56000)} 
    };

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

/* forward declarations */

LOCAL void 	tyCoStartup ();
LOCAL int 	tyCoOpen ();
LOCAL STATUS 	tyCoIoctl ();
LOCAL void 	tyCoTxInt ();
LOCAL void 	tyCoRxInt ();
void 		tyCoInt ();

/* external forwards */

void 		tyCoHrdInit ();
int  		tyClrInts ();

/*******************************************************************************
*
* tyCoDrv - initialize the tty driver
*
* This routine initializes the driver, sets up interrupt vectors,
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
    /* check if driver already installed */
     
    if (tyCoDrvNum > 0)
	return (OK);

    tyCoHrdInit (0);	/* initialize uart 0 */
    tyCoHrdInit (1);	/* initialize uart 1 */

    tyCoDrvNum = iosDrvInstall (tyCoOpen, (FUNCPTR) NULL, tyCoOpen,
				(FUNCPTR) NULL, tyRead, tyWrite, tyCoIoctl);


    return (tyCoDrvNum == ERROR ? ERROR : OK);
    }

/*******************************************************************************
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
    char *      name,           /* name to use for this device      */
    FAST int    channel,        /* physical channel for this device */
    int         rdBufSize,      /* read buffer size, in bytes       */
    int         wrtBufSize      /* write buffer size, in bytes      */
    )
    {
    FAST TY_CO_DEV *pTyCoDv = &tyCoDv [channel];

    if (tyCoDrvNum <= 0)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    /* if this doesn't represent a valid channel, don't do it */

    if (channel < 0 || channel >= tyCoDv [0].numChannels)
	return (ERROR);

    /* if this device already exists, don't create it */

    if (pTyCoDv->created)
	return (ERROR);

    /* initialize the ty descriptor, and turn on the bit for this
     * receiver in the interrupt mask */

    if (tyDevInit (&pTyCoDv->tyDev, rdBufSize, wrtBufSize,
		   (FUNCPTR) tyCoStartup) != OK)
	{
        return (ERROR);
	}

    if (channel)
	*(volatile UINT8 *) IO_MASK_REG |= UART_B_MSK; /* board level enable */
    else
	*(volatile UINT8 *) IO_MASK_REG |= UART_A_MSK; /* board level enable */

    *pTyCoDv->int_enable = IE_RCV_LS|IE_DATA_RDY;

    /* Mark the device as created, and add the device to the I/O system */

    pTyCoDv->created = TRUE;

    return (iosDevAdd (&pTyCoDv->tyDev.devHdr, name, tyCoDrvNum));
    }

/*******************************************************************************
*
* tyCoHrdInit - initialize the Western Digital 8250 UART.
*
* This routine initializes the Western Digital 8250 UART for the VxWorks 
* environment. 
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoHrdInit
    (
    int channel
    )
    {
    FAST TY_CO_DEV *pTyCoDv = &tyCoDv [channel];
    FAST int       statusReg;           /* previous value of status reg */
    FAST int       ix;                  /* index for baud lookup        */
    unsigned char  staleValue;		/* read value from CSR's	*/

    statusReg = intLock ();

    /* 
    * channel initialiazation 8 data bits, 1 stop bit, no parity, 
    * set for 9600 baud 
    */

    *pTyCoDv->int_enable = 0x00; 	/* disable ints */

#ifdef	SYS_WB_FLUSH
    sysWbFlush();
#endif	/* SYS_WB_FLUSH */

    tyClrInts(channel);	

    /*
    * Set baud rate.  See the 8250 manual for details.
    */

    for (ix = 0; ix < NELEMENTS (baudTable); ix++)
     {
	if (baudTable [ix].rate == DEFAULT_BAUD)
	 {
    	    *pTyCoDv->line_ctl = LC_DLAB_ENABLE|LC_LENGTH_8;	/* baud setup */

#ifdef	SYS_WB_FLUSH
	    sysWbFlush();
#endif	/* SYS_WB_FLUSH */

	    *pTyCoDv->int_enable = baudTable[ix].msbyte;	/* baud msbyte*/

#ifdef	SYS_WB_FLUSH
	    sysWbFlush();
#endif	/* SYS_WB_FLUSH */

	    *pTyCoDv->receive_trans = baudTable[ix].lsbyte;	/* baud lsbyte*/

#ifdef	SYS_WB_FLUSH
	    sysWbFlush();
#endif	/* SYS_WB_FLUSH */
	 }
     }

    /* 8 bits, no parity, spacing, one stop bit */

    *pTyCoDv->line_ctl = LC_LENGTH_8|LC_PARITY_OFF|LC_STOP_1;

    /* flush old data */

    while (*pTyCoDv->line_stat & LS_DATA_RDY) 
        staleValue = *pTyCoDv->receive_trans;

    /*
     * all interrupts are masked out: the receiver interrupt will be enabled
     * in the tyCoDevCreate
     */

    intUnlock (statusReg);
    } 

/*******************************************************************************
*
* tyCoOpen - open file to Western Digital uart
*/

LOCAL int tyCoOpen
    (
    TY_CO_DEV *pTyCoDv,
    char *name,
    int mode
    )
    {
    return ((int) pTyCoDv);
    }

/*******************************************************************************
*
* tyCoIoctl - special device control
*
* This routine handles baud rate requests, and passes all other requests
* to tyIoctl().
*
* RETURNS: OK, or ERROR if invalid baud rate, or whatever tyIoctl() returns.
*/

LOCAL STATUS tyCoIoctl
    (
    TY_CO_DEV *pTyCoDv,	/* device to control */
    int request,	/* request code      */
    int arg		/* some argument     */
    )
    {
    FAST UCHAR 	lineCtl;
    FAST UCHAR 	enableReg;
    FAST STATUS status;
    FAST int 	ix;

    switch (request)
	{
	case FIOBAUDRATE:
	    status = ERROR;
	    for (ix = 0; ix < NELEMENTS (baudTable); ix++)
		{
		if (baudTable [ix].rate == arg)	/* lookup baud rate value */
		    {
		    lineCtl = *pTyCoDv->line_ctl; 	/* save for later */
		    enableReg = *pTyCoDv->int_enable;	/* save for later */

		    /* setup baud rate */

		    *pTyCoDv->line_ctl = LC_DLAB_ENABLE|LC_LENGTH_8; 

#ifdef	SYS_WB_FLUSH
		    sysWbFlush();
#endif	/* SYS_WB_FLUSH */

		    /* setup baud msbyte */

		    *pTyCoDv->int_enable = baudTable[ix].msbyte;

#ifdef	SYS_WB_FLUSH
		    sysWbFlush();
#endif	/* SYS_WB_FLUSH */

		    /* setup baud lsbyte */

		    *pTyCoDv->receive_trans = baudTable[ix].lsbyte;

#ifdef	SYS_WB_FLUSH
		    sysWbFlush();
#endif	/* SYS_WB_FLUSH */

    		    *pTyCoDv->line_ctl = lineCtl;   /* reset line cntl reg */

#ifdef	SYS_WB_FLUSH
		    sysWbFlush();
#endif	/* SYS_WB_FLUSH */

    		    *pTyCoDv->int_enable = enableReg;/* reset int enable reg */

#ifdef	SYS_WB_FLUSH
		    sysWbFlush();
#endif	/* SYS_WB_FLUSH */

		    /* sync up after changing baud rate */

		    taskDelay (sysClkRateGet () / 4 + 1);

		    status = OK;
		    break;
		    }
		}
	    break;

	default:
	    status = tyIoctl (&pTyCoDv->tyDev, request, arg);
	    break;
	}
    return (status);
    }

/*******************************************************************************
*
* tyCoRxInt - handle a receiver interrupt
*
* This routine gets called at interrupt level to handle a receiver interrupt.
*/

LOCAL void tyCoRxInt
    (
    TY_CO_DEV *pTyCoDv
    )
    {
    (void)tyIRd (&pTyCoDv->tyDev, *pTyCoDv->receive_trans);
    }

/*******************************************************************************
*
* tyCoTxInt - handle a transmitter interrupt
*
* This routine gets called from interrupt level to handle a xmitter interrupt.
* If there is another character to be transmitted, it sends it.  If
* not, or if a device has never been created for this channel, we just
* disable the interrupt.
*/

LOCAL void tyCoTxInt
    (
    TY_CO_DEV *pTyCoDv
    )
    {
    char outChar;

    if (pTyCoDv->created &&
	tyITx (&pTyCoDv->tyDev, &outChar) == OK)
	{
	*pTyCoDv->receive_trans = outChar; 
	sysWbFlush();
	*pTyCoDv->int_enable |= IE_THRE_EMPTY; 
	sysWbFlush();
	}
    else
	{
	/* turn off the transmitter */
	*pTyCoDv->int_enable &= ~IE_THRE_EMPTY; 
	sysWbFlush();
	}
    }

/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* Call interrupt level character output routine for the Western Digital UART.
*/

LOCAL void tyCoStartup
    (
    TY_CO_DEV *pTyCoDv		/* ty device to start up */
    )
    {
    char outChar;

    if (pTyCoDv->created && tyITx (&pTyCoDv->tyDev, &outChar) == OK)
	{
	*pTyCoDv->receive_trans = outChar;
	sysWbFlush();
	*pTyCoDv->int_enable |= IE_THRE_EMPTY;
	sysWbFlush();
	}
    }

/*******************************************************************************
*
* tyCoInt - is the interrupt a receive or transmit interrupt
*
* Read the UART interrupt status register to determine what type of
* interrupt we are looking at.
*
* NOMANUAL
*/

void tyCoInt
    (
    TY_CO_DEV *pTyCoDv				/* device to control */
    )
    {
    FAST UCHAR status;
    FAST UCHAR staleValue;

    while ((status = *pTyCoDv->int_id) != IID_NONE_PEND)
	{
        if ((status&IID_MASK) == IID_RCV_LS)
	    {			/* break, serialization error, modem status */
	    staleValue = *pTyCoDv->line_stat;	
	    pTyCoDv->lastBreak = (int) staleValue;	
	    ++pTyCoDv->breakError;	
	    }
        else if ((status&IID_MASK) == IID_DATA_RDY)
            tyCoRxInt(pTyCoDv);	/* receive interrupt */
        else if ((status&IID_MASK) == IID_THRE)
            tyCoTxInt(pTyCoDv);	/* transmit interrupt */
        else if ((status&IID_MASK) == IID_MODEM_STAT)
	    {			/* rs232 state change, or nothing pending */
	    staleValue = *pTyCoDv->modem_stat;
	    pTyCoDv->lastState = (int) staleValue;	
	    ++pTyCoDv->stateChange;
	    }
	}
    }

/*******************************************************************************
*
* tyClrInts - clear pending interrupts on the Western Digital 8250 UART.
*
* This routine clears all pending interrupt conditions on the given
* channel. 
*
* NOMANUAL
*/

int tyClrInts
    (
    int channel
    )
    {
    FAST TY_CO_DEV      *pTyCoDv = &tyCoDv [channel];
    FAST UCHAR 		staleValue;	/* read value from CSR's	*/
    FAST UCHAR 		status;		/* uart interrupt status	*/

    while ((status = *pTyCoDv->int_id) != IID_NONE_PEND)
        {
        if ((status & IID_MASK) == IID_RCV_LS)
	    staleValue = *pTyCoDv->line_stat;
	else if ((status & IID_MASK) == IID_DATA_RDY)
    	    staleValue = *pTyCoDv->receive_trans;
	else if ((status & IID_MASK) == IID_THRE)
	    *pTyCoDv->int_enable &= ~IE_THRE_EMPTY; /* turn off the xmitter */
	else if ((status & IID_MASK) == IID_MODEM_STAT)
	    staleValue = *pTyCoDv->modem_stat;
	}
    } 
