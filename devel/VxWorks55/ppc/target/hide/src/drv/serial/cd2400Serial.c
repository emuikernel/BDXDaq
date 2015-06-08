/* cd2400Serial.c - CL-CD2400 MPCC tty driver */

/* Copyright 1984-1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,20nov96,dds  SPR 5438: Added interrupt locking to tyCoStartup () routine.
01n,30nov92,jdi  doc tweak; NOMANUAL for tyCoIntRx(), tyCoIntTx(), tyCoInt()
		 - SPR 1808.
01m,28oct92,caf  tweaked tyCoDevCreate() documentation.
01l,25sep92,ccc  added LOCAL forward declarations.
01k,09sep92,gae  doc tweaks.
01j,01sep92,ccc  changed name to cd2400Serial.c.
01i,18jul92,smb  Changed errno.h to errnoLib.h.
01h,16jun92,ccc  created from 01g version of mv167 tyCoDrv, ansified.
01g,26may92,rrr  the tree shuffle
01f,14oct91,ccc  changed VOID to void.
		 changed copyright notice.
01e,18aug91,jdi  documentation cleanup.
01d,15aug91,ccc  documentation changes.
01c,12aug91,ccc  delinted.
01b,30jul91,ccc  fixed IRQ vector for channels, removed tyCoResetChannel().
		 selected channel in tyCoIoctl().
01a,14jun91,ccc	 written, by modifying 01e of the mv147 version.
*/

/*
DESCRIPTION
This is the driver for the Cirus Logic CD2400 MPCC.  It uses the SCC's in
asynchronous mode only.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling tyCoDrv().
This routine should be called exactly once, before any reads, writes, or
calls to tyCoDevCreate().  Normally, it is called from usrRoot() in usrConfig.c.

Before a terminal can be used, it must be created using tyCoDevCreate().
Each port to be used should have exactly one device associated with it by
calling this routine.

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal tty driver; for
more information, see the manual entry for tyLib.  The available baud rates
are:  50, 110, 150, 300, 600, 1200, 2400, 3600, 4800, 7200, 9600,
19200, and 38400.

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
#include "drv/serial/cd2400.h"

#define DEFAULT_BAUD	9600

IMPORT TY_CO_DEV tyCoDv [];	/* device descriptors */

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

typedef struct			/* BAUD */
    {
    int	rate;			/* baud rate */
    char timeConstant;		/* time constant */
    char tcor;			/* transmit clock option register value */
    char rcor;			/* receive clock option register value */
    } BAUD;

LOCAL BAUD baudTable [] =
    {
     {    50, ((BAUD_CLK_FREQ / 2048) /   50) - 1, TCOR_CLK4, RCOR_CLK4 },
     {   110, ((BAUD_CLK_FREQ / 2048) /  110) - 1, TCOR_CLK4, RCOR_CLK4 },
     {   150, ((BAUD_CLK_FREQ / 2048) /  150) - 1, TCOR_CLK4, RCOR_CLK4 },
     {   300, ((BAUD_CLK_FREQ /  512) /  300) - 1, TCOR_CLK3, RCOR_CLK3 },
     {   600, ((BAUD_CLK_FREQ /  512) /  600) - 1, TCOR_CLK3, RCOR_CLK3 },
     {  1200, ((BAUD_CLK_FREQ /  128) / 1200) - 1, TCOR_CLK2, RCOR_CLK2 },
     {  2400, ((BAUD_CLK_FREQ /  128) / 2400) - 1, TCOR_CLK2, RCOR_CLK2 },
     {  3600, ((BAUD_CLK_FREQ /   32) / 3600) - 1, TCOR_CLK1, RCOR_CLK1 },
     {  4800, ((BAUD_CLK_FREQ /   32) / 4800) - 1, TCOR_CLK1, RCOR_CLK1 },
     {  7200, ((BAUD_CLK_FREQ /   32) / 7200) - 1, TCOR_CLK1, RCOR_CLK1 },
     {  9600, ((BAUD_CLK_FREQ /   32) / 9600) - 1, TCOR_CLK1, RCOR_CLK1 },
     { 19200, ((BAUD_CLK_FREQ /    8) /19200) - 1, TCOR_CLK0, RCOR_CLK0 },
     { 38400, ((BAUD_CLK_FREQ /    8) /38400) - 1, TCOR_CLK0, RCOR_CLK0 }
    };

/* forward declarations */

LOCAL void      tyCoHrdInit ();
LOCAL void      tyCoInitChannel ();
LOCAL STATUS    tyCoIoctl ();
LOCAL int	tyCoOpen ();
LOCAL void	tyCoStartup ();

/*******************************************************************************
*
* tyCoDrv - tty driver initialization routine
*
* This routine initializes the serial driver, sets up interrupt vectors, and
* performs hardware initialization of the serial ports.
*
* This routine should be called exactly once, before any reads, writes, or
* calls to tyCoDevCreate().  Normally, it is called by usrRoot() is
* usrConfig.c.
*
* RETURNS: OK, or ERROR if the driver cannot be installed.
*/

STATUS tyCoDrv (void)

    {
    /* check if driver already installed */

    if (tyCoDrvNum > 0)
	return (OK);

    tyCoHrdInit ();	/* init hardware */

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
*     tyCoDevCreate ("/tyCo/0", 0, 512, 512);
* .CE
*
* RETURNS: OK, or ERROR if the driver is not installed or the channel is
* invalid.
*/

STATUS tyCoDevCreate
    (
    char *      name,           /* name to use for this device      */
    FAST int    channel,        /* physical channel for this device */
    int         rdBufSize,      /* read buffer size, in bytes       */
    int         wrtBufSize      /* write buffer size, in bytes      */
    )
    {
    FAST TY_CO_DEV *pTyCoDv;

    if (tyCoDrvNum <= 0)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    /* if this doesn't represent a valid channel, don't do it */

    if (channel < 0 || channel >= tyCoDv [0].numChannels)
	return (ERROR);

    pTyCoDv = &tyCoDv [channel];

    /* if there is a device already on this channel, don't do it */

    if (pTyCoDv->created)
	return (ERROR);


    /* initialize the ty descriptor */

    if (tyDevInit (&pTyCoDv->tyDev, rdBufSize, wrtBufSize,
		   (FUNCPTR) tyCoStartup) != OK)
	{
	return (ERROR);
	}

    /* initialize the channel hardware */

    tyCoInitChannel (channel);	/* init single channel */

    /* mark the device as created, and add the device to the I/O system */

    pTyCoDv->created = TRUE;

    return (iosDevAdd (&pTyCoDv->tyDev.devHdr, name, tyCoDrvNum));
    }
/*******************************************************************************
*
* tyCoHrdInit - initialize the USART
*/

LOCAL void tyCoHrdInit (void)

    {
    FAST int   oldlevel;	/* current interrupt level mask */

    oldlevel = intLock ();		/* disable interrupts during init */

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

    intUnlock (oldlevel);
    }
/*******************************************************************************
*
* tyCoInitChannel - initialize a single channel
*/

LOCAL void tyCoInitChannel
    (
    int channel
    )
    {
    FAST TY_CO_DEV *pTyCoDv = &tyCoDv [channel];
    FAST int        oldlevel;		/* current interrupt level mask */

    oldlevel = intLock ();		/* disable interrupts during init */

    /* initialize registers */

    *MPCC_CAR  = pTyCoDv->chan_num;

    *MPCC_LIVR = pTyCoDv->int_vec;

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

    *MPCC_IER = IER_RXDATA	|
		IER_TXEMPTY;

    *MPCC_MSVRDTR = MSVR_DTR;
    *MPCC_MSVRRTS = MSVR_RTS;

    intUnlock (oldlevel);
    }

/*******************************************************************************
*
* tyCoOpen - open file to USART
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
* This routine handles FIOBAUDRATE requests and passes all others to tyIoctl.
*
* RETURNS: OK or ERROR if invalid baud rate, or whatever tyIoctl returns.
*/

LOCAL STATUS tyCoIoctl
    (
    TY_CO_DEV *pTyCoDv,	/* device to control */
    int        request,	/* request code      */
    int        arg	/* some argument     */
    )
    {
    FAST int     oldlevel;		/* current interrupt level mask */
    int		 ix;
    FAST STATUS  status;

    switch (request)
	{
	case FIOBAUDRATE:

	    status = ERROR;		/* baud rate out of range */

	    /* disable interrupts during chip access */

	    oldlevel = intLock ();

	    *MPCC_CAR = pTyCoDv->chan_num;      /* set up channel */

	    for (ix = 0; ix < NELEMENTS (baudTable); ix++)
		{
		if (baudTable [ix].rate == arg)
		    {
		    *MPCC_TBPR = baudTable [ix].timeConstant;
		    *MPCC_TCOR = baudTable [ix].tcor;
		    *MPCC_RBPR = baudTable [ix].timeConstant;
		    *MPCC_RCOR = baudTable [ix].rcor;
		    status = OK;
		    break;
		    }
		}

	    intUnlock (oldlevel);

	    break;

	default:
	    status = tyIoctl (&pTyCoDv->tyDev, request, arg);
	    break;
	}

    return (status);
    }

/*******************************************************************************
*
* tyCoIntRx - interrupt level processing for Receive Data Interrupt
*
* This routine handles the interrupts for all channels for a Receive
* Data Interrupt.
*
* NOMANUAL
*/

void tyCoIntRx
    (
    FAST TY_CO_DEV  *pTyCoDv
    )

    {
    if (pTyCoDv->created)
	{
	tyIRd (&pTyCoDv->tyDev, *MPCC_RDR);	/* read char */
	*MPCC_REOIR = EOIR_TRANSFER;
				/* write to register to clear interrupt */
	}
    else
	*MPCC_REOIR = EOIR_NO_TRANSFER;	/* just clear interrupt */
    }

/*******************************************************************************
*
* tyCoIntTx - interrupt level processing
*
* This routine handles all interrupts from the MPCC.
* We determine from the paramter which channel interrupted us.
* Then from the status register we know what kind of interrupt.
*
* NOMANUAL
*/

void tyCoIntTx
    (
    FAST TY_CO_DEV  *pTyCoDv
    )
    {
    char 	     outChar;

    /* Tx Buffer Empty */

    if (pTyCoDv->created && (tyITx (&pTyCoDv->tyDev, &outChar) == OK))
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
* tyCoInt - interrupt level processing for special interrupts
*
* NOMANUAL
*/

void tyCoInt
    (
    FAST TY_CO_DEV  *pTyCoDv
    )
    {
    *MPCC_REOIR = EOIR_NO_TRANSFER;	/* just clear interrupt */
    }

/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* Call interrupt level character output routine.
*/

LOCAL void tyCoStartup
    (
    TY_CO_DEV *pTyCoDv		/* ty device to start up */
    )
    {
    int oldLevel;

    oldLevel = intLock ();

    *MPCC_CAR = pTyCoDv->chan_num;	/* set up channel */

    /* only need to enable interrupt */

    *MPCC_IER = IER_TXEMPTY	|
		IER_TXDATA	|
		IER_RXDATA;

    intUnlock (oldLevel);
    }
