/* ns16550Serial.c - NS 16550 UART tty driver */

/* Copyright 1994-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,19nov96,dat  fixed SPR 5381, args to tyCoInitUart(), coding stds.
01c,22aug95,kkk  undo 01b changes
            kvk  fixed a serial bug which hangs the board on fast serial input.
01,07aug95,kkk  fix warnings for I960 (spr#4177)
01a,27jun94,snc  written.
*/

/*
DESCRIPTION
This is the device driver for the ns16550 UART.  Enjoy!

USER-CALLABLE_ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create logical devices.

Before the driver can be used, it must be initialized by calling tyCoDrv().
This routine should be called exactly once, before any reads, writes, or
calls to tyCoDevCreate().  Normally, it is called by usrRoot() in usrConfig.c.

Before a terminal can be used, it must be created using tyCoDevCreate().
Each port to be used should have exactly one device associated with it by
calling this routine.

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal tty driver; for
more information, see the manual entry for tyLib.  Available baud rates
range from 50 to 38400.

SEE ALSO
tyLib
*/

#include "vxWorks.h"
#include "iv.h"
#include "ioLib.h"
#include "iosLib.h"
#include "tyLib.h"
#include "intLib.h"
#include "errnoLib.h"
#include "drv/serial/ns16552.h"

#define DEFAULT_BAUD	9600

IMPORT TY_CO_DEV tyCoDv [];	/* device descriptors */

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

/* function prototypes */

LOCAL void		tyCoStartup ();
LOCAL int		tyCoOpen ();
LOCAL STATUS		tyCoIoctl ();
LOCAL void		tyCoInitUart ();

#define UART_DELTA          4           /* register address boundaries */
#define XTAL                1843200



/******************************************************************************
*
* InReg - reads a register from the 16550 UART
*
* Note: UART_DELTA must be defined to be the number of bytes between
* adjacent UART registers
*/

UINT8 InReg
    (
    int reg,
    UINT8 *base_addr	/* Base address of UART device */
    )
    {
    UINT8 val;

    val = *(UINT8 *)((UINT32)base_addr + (reg*UART_DELTA));
    return val;
    }

/******************************************************************************
*
* OutReg - reads a register from the 16550 UART
*
* Note: UART_DELTA must be defined to be the number of bytes between
* adjacent UART registers
*/

void OutReg
    (
    int reg,
    UINT8 val,
    UINT8 *base_addr	/* Base address of UART device */
    )
    {
    *(UINT8 *)((UINT32)base_addr + (reg*UART_DELTA)) = val;
    }

/*******************************************************************************
*
* tyCoDrv - initialize the tty driver
*
* This routine initializes the serial driver, sets up interrupt vectors, and
* performs hardware initialization of the serial ports.
*
* This routine should be called exactly once, before any reads, writes, or
* calls to tyCoDevCreate().  Normally, it is called by usrRoot() in
* usrConfig.c.
*
* RETURNS:  OK, or ERROR if the driver cannot be installed.
*
* SEE ALSO:  tyCoDevCreate()
*/

STATUS tyCoDrv (void)
    {
    /* check if driver already installed */

    if (tyCoDrvNum > 0)
	return (OK);

    tyCoDrvNum = iosDrvInstall (tyCoOpen, (FUNCPTR) NULL, tyCoOpen,
			(FUNCPTR) NULL, tyRead, tyWrite, tyCoIoctl);

    return (tyCoDrvNum == ERROR ? ERROR : OK);
    }

/*******************************************************************************
*
* tyCoDevCreate - create a device for an on-board serial port
*
* This routine creates a device on a specified serial port.  Each port
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device "/tyCo/0", with buffer sizes of 512 bytes,
* the proper call would be:
* .CS
*	tyCoDevCreate ("/tyCo/0", 512, 512);
* .CE
*
* RETURNS:  OK, or ERROR if the driver is not installed,
* or the device already exists.
*
* SEE ALSO: tyCoDrv()
*/

STATUS tyCoDevCreate
    (
    char *	name,		/* name to use for this device	    */
    FAST int	channel,	/* physical channel for this device */
    int		rdBufSize,	/* read buffer size, in bytes	    */
    int		wrtBufSize	/* write buffer size, in bytes	    */
    )
    {
    FAST TY_CO_DEV *pTyCoDv;

    if (tyCoDrvNum <= 0)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    pTyCoDv = &tyCoDv[channel];

    /* If there is a device already, don't create one */

    if (pTyCoDv->created)
	return (ERROR);

    /* Initialize the ty descriptor */

    if (tyDevInit (&pTyCoDv->tyDev, rdBufSize, wrtBufSize,
		(FUNCPTR) tyCoStartup) != OK)
	{
	return (ERROR);
	}

    /* Initialize the hardware */

    tyCoInitUart (channel);

    /* Mark the device as created, and add the device to the I/O system */
	
    pTyCoDv->created = TRUE;

    return (iosDevAdd (&pTyCoDv->tyDev.devHdr, name, tyCoDrvNum));
    }

/*******************************************************************************
*
* tyCoInitUart - initialize UART
*
* Reconfigure the UART; 8 data bits, 1 stop bit, no parity.
*/

LOCAL void tyCoInitUart
    (
    int channel
    )
    {
    FAST TY_CO_DEV *pTyCoDv;
    FAST int        oldlevel;		/* current interrupt level mask */

    pTyCoDv = &tyCoDv[channel];

    oldlevel = intLock ();		/* disable interrupts during init */

    /* Configure Port -  Set 8 bits, 1 stop bit, no parity. */
    pTyCoDv->lcr = (UINT8)(CHAR_LEN_8 | ONE_STOP | PARITY_NONE);
    OutReg(LCR, pTyCoDv->lcr, pTyCoDv->regs);

    /* Reset/Enable the FIFOs */
    OutReg(FCR, RxCLEAR | TxCLEAR, pTyCoDv->regs);
 
    /* Enable access to the divisor latches by setting DLAB in LCR. */
    OutReg(LCR, LCR_DLAB | pTyCoDv->lcr, pTyCoDv->regs);

    /* Set divisor latches. */
    OutReg(DLL, XTAL/(16*DEFAULT_BAUD), pTyCoDv->regs);
    OutReg(DLM, (XTAL/(16*DEFAULT_BAUD)) >> 8, pTyCoDv->regs);

    /* Restore line control register */
    OutReg(LCR, pTyCoDv->lcr, pTyCoDv->regs);

    /* Make a copy since register is not readable */
    pTyCoDv->ier = (UINT8)(RxFIFO_BIT | TxFIFO_BIT); 

    /* Enable appropriate interrupts */
    OutReg(IER, pTyCoDv->ier, pTyCoDv->regs);

    intUnlock (oldlevel);
    }

/*******************************************************************************
*
* tyCoOpen - open file to UART
*
* RETURNS: Ptr to device structure.
*/

LOCAL int tyCoOpen
    (
    TY_CO_DEV *pTyCoDv,
    char      *name,
    int        mode
    )
    {
    return ((int) pTyCoDv);
    }

/*******************************************************************************
*
* tyCoIoctl - special device control
*
* This routine handles FIOBAUDRATE requests and passes all others to tyIoctl().
*
* RETURNS: OK, or ERROR if invalid baud rate, or whatever tyIoctl() returns.
*/

LOCAL STATUS tyCoIoctl
    (
    TY_CO_DEV *pTyCoDv,		/* device to control */
    int        request,		/* request code */
    int        arg		/* some argument */
    )
    {
    FAST int     oldlevel;		/* current interrupt level mask */
    FAST STATUS  status;

    status = 0;

    switch (request)
	{
	case FIOBAUDRATE:

	    if (arg < 50 || arg > 38400)
	        {
		status = ERROR;		/* baud rate out of range */
		break;
	        }

	    /* disable interrupts during chip access */

	    oldlevel = intLock ();

    	    /* Enable access to the divisor latches by setting DLAB in LCR. */
    	    OutReg(LCR, LCR_DLAB | pTyCoDv->lcr, pTyCoDv->regs);
 
    	    /* Set divisor latches. */
    	    OutReg(DLL, XTAL/(16*arg), pTyCoDv->regs);
    	    OutReg(DLM, (XTAL/(16*arg)) >> 8, pTyCoDv->regs);
 
    	    /* Restore line control register */
    	    OutReg(LCR, pTyCoDv->lcr, pTyCoDv->regs);
 
	    intUnlock (oldlevel);

	    status = OK;
	    break;

	default:
	    status = tyIoctl (&pTyCoDv->tyDev, request, arg);
	    break;
	}
    return (status);
    }

/*******************************************************************************
*
* tyCoIntWr - interrupt level processing
*
* This routine handles write interrupts from the UART.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoIntWr (int channel)
    {
    char            outChar;
    FAST TY_CO_DEV *pTyCoDv = &tyCoDv[channel];

    if (pTyCoDv->created && (tyITx (&pTyCoDv->tyDev, &outChar) == OK))
	{
	/* write char. to Transmit Holding Reg. */
	OutReg(THR, outChar, pTyCoDv->regs);
	}
    }

/*****************************************************************************
*
* tyCoIntRd - interrupt level input processing
*
* This routine handles read interrupts from the UART
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoIntRd (int channel)
    {
    FAST TY_CO_DEV *pTyCoDv = &tyCoDv[channel];
    char            inchar;

    /* read character from Receive Holding Reg. */
    inchar = InReg(RBR, pTyCoDv->regs);

    if (pTyCoDv->created)
	tyIRd (&pTyCoDv->tyDev, inchar);
    }

/**********************************************************************
*
* tyCoIntEx - miscellaneous interrupt processing
*
* This routine handles miscellaneous interrupts on the UART
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoIntEx (int channel)
    {
    FAST TY_CO_DEV *pTyCoDv;

    pTyCoDv = &tyCoDv[channel];

    /* Nothing for now... */
    }

/********************************************************************************
* tyCoInt - interrupt level processing
*
* This routine handles interrupts from the UART.
*
* NOMANUAL
*/

void tyCoInt
    (
    int channel	/* unused parameter */
    )
    {
    FAST char        intStatus;
    FAST TY_CO_DEV  *pTyCoDv;

    pTyCoDv = &tyCoDv[channel];

    /* read the Interrrupt Status Register (Int. Ident.) */
    intStatus = InReg(IIR, pTyCoDv->regs);

    switch (intStatus & 0x0f)
        {
	case 0x00:			/* Modem Status Reg. */
	    tyCoIntEx(channel);
	    break;

	case 0x01:			/* No Interrupt Pending */
	    break;

        case 0x02:                      /* Tx Holding Reg. Empty */
	    tyCoIntWr(channel);
            break;
 
        case 0x04:                      /* RxChar Avail */
	    tyCoIntRd(channel);
            break;
 
        case 0x06:                      /* Rx. Line Status Change */
	    tyCoIntEx(channel);
            break;

	case 0x0c:			/* Receive Data Timeout */
	    tyCoIntEx(channel);
	    break;
        }
    }

/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* Call interrupt level character output routine and enable interrupt!
*/

LOCAL void tyCoStartup
    (
    TY_CO_DEV *pTyCoDv 		/* ty device to start up */
    )
    {
    char outChar;

    if (tyITx (&pTyCoDv->tyDev, &outChar) == OK)
	{
	OutReg(THR,outChar, pTyCoDv->regs);	/* transmit character */
	}
    }
