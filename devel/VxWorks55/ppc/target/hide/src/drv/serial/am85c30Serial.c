/* am85c30Serial.c - Am85C30 ESCC (Enhanced Serial Communications Controller)
                     tty driver*/

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,27apr95,tpr  inverted RX and TX text in tyCoInt().
01b,11mar94,pme  changed dedines to defines.
01a,02dec93,pad  written, by modifying 01n of the z8530 version.
*/

/*
DESCRIPTION
This is the driver for the Am85C30 ESCC (Enhanced Serial Communications
Controller).
It uses the ESCCs in asynchronous mode only.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly: tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create devices.

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

/* includes */

#include "vxWorks.h"
#include "iv.h"
#include "ioLib.h"
#include "iosLib.h"
#include "tyLib.h"
#include "intLib.h"
#include "errnoLib.h"
#include "drv/serial/am85c30.h"

/* defines */

#define DEFAULT_BAUD	9600

/* externals */

IMPORT TY_CO_DEV tyCoDv [];	/* device descriptors */
IMPORT int delayCount;		/* used in SCC_DELAY */

/* locals */

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

/* forward declarations */

LOCAL void   tyCoStartup (TY_CO_DEV * pTyCoDv);
LOCAL int    tyCoOpen (TY_CO_DEV * pTyCoDv, char * name, int mode);
LOCAL STATUS tyCoIoctl (TY_CO_DEV * pTyCoDv, int request, int arg);
LOCAL void   tyCoHrdInit (void);
LOCAL void   tyCoInitChannel (int channel);
LOCAL void   tyCoResetChannel (int channel);

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
* RETURNS: OK, or ERROR if the driver cannot be installed.
*
* SEE ALSO: tyCoDevCreate()
*/

STATUS tyCoDrv (void)

    {
    /* check if driver already installed */

    if (tyCoDrvNum > 0)
	return (OK);

    tyCoHrdInit ();

    tyCoDrvNum = iosDrvInstall (tyCoOpen, (FUNCPTR) NULL, tyCoOpen,
				(FUNCPTR) NULL, tyRead, tyWrite, tyCoIoctl);

    (void) intConnect (INUM_TO_IVEC (INT_NUM_SCC), tyCoInt, 0);

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
* RETURNS: OK, or ERROR if the driver is not installed, the channel is
* invalid, or the device already exists.
*
* SEE ALSO: tyCoDrv()
*/

STATUS tyCoDevCreate
    (
    char * name,           /* name to use for this device      */
    int    channel,        /* physical channel for this device */
    int    rdBufSize,      /* read buffer size, in bytes       */
    int    wrtBufSize      /* write buffer size, in bytes      */
    )
    {
    TY_CO_DEV * pTyCoDv;

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

    tyCoInitChannel (channel);

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
    int oldlevel;	/* current interrupt level mask */
    int ix;

    oldlevel = intLock ();	/* disable interrupts during init */

    for (ix=0; ix < tyCoDv [0].numChannels; ix++)
	tyCoResetChannel (ix);	/* reset channel */

    intUnlock (oldlevel);
    }

/********************************************************************************
* tyCoResetChannel - reset a single channel
*/

LOCAL void tyCoResetChannel
    (
    int channel
    )
    {
    volatile char *	cr = tyCoDv [channel].cr; /* SCC control reg adr */
    int			zero = 0;

    *cr = zero;		/* SCC_WR0_NULL_CODE sync the state machine */
    SCC_DELAY;
    *cr = zero;		/* SCC_WR0_NULL_CODE sync the state machine */
    SCC_DELAY;
    *cr = SCC_WR0_ERR_RST;
    SCC_DELAY;
    *cr = SCC_WR0_RST_INT;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR9;      /* write register 9 - master int ctrl */
    SCC_DELAY;

    *cr = ((channel % 2) == 0) ?
        SCC_WR9_CH_A_RST :      /* reset channel A */
        SCC_WR9_CH_B_RST;       /* reset channel B */

    SCC_DELAY; SCC_DELAY; SCC_DELAY; SCC_DELAY; SCC_DELAY; SCC_DELAY; 
    SCC_DELAY; SCC_DELAY; SCC_DELAY; SCC_DELAY; SCC_DELAY; SCC_DELAY;
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
    TY_CO_DEV *		pTyCoDv = &tyCoDv [channel];
    volatile char *	cr = pTyCoDv->cr;	/* SCC control reg adr */
    int			baudConstant;
    int			oldlevel;	/* current interrupt level mask */
    int			zero = 0;

    oldlevel = intLock ();		/* disable interrupts during init */

    /* initialize registers */

    *cr = SCC_WR0_RST_TX_UND;		/* reset transmit underrun */
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR4;              /* write reg 4 - misc parms and modes */
    SCC_DELAY;
    *cr = SCC_WR4_1_STOP | SCC_WR4_16_CLOCK;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR1;
    SCC_DELAY;
    *cr = 0;				/* no DMA */
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR2;
    SCC_DELAY;
    *cr = 0;				/* no vector */
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR3;		/* write reg 3 - receive params */
    SCC_DELAY;
    *cr = SCC_WR3_RX_8_BITS;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR5;		/* tx params */
    SCC_DELAY;
    *cr = SCC_WR5_TX_8_BITS;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR6;		/* no sync mode */
    SCC_DELAY;
    *cr = zero;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR7;		/* no sync mode */
    SCC_DELAY;
    *cr = zero;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR9;		/* no interrupt acknowledge */
    SCC_DELAY;
    *cr = SCC_WR9_NV;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR10;		/* misc tx/rx control */
    SCC_DELAY;
    *cr = zero;				/* clear sync, loop, poll */
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR11;		/* clock mode */
    SCC_DELAY;
    *cr = pTyCoDv->clockModeWR11;
    SCC_DELAY;

    /* Calculate the baud rate constant for the default baud rate
     * from the input clock frequency.  This assumes that the
     * divide-by-16 bit is set (done in WR4 above).
     */

    baudConstant = ((pTyCoDv->baudFreq / 32) / DEFAULT_BAUD) - 2;

    *cr = SCC_WR0_SEL_WR12;		/* LSB of baud constant */
    SCC_DELAY;
    *cr = (char) baudConstant;		/* write LSB */
    SCC_DELAY;
    *cr = SCC_WR0_SEL_WR13;		/* MSB of baud constant */
    SCC_DELAY;
    *cr = (char) (baudConstant >> 8);	/* write MSB */
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR14;		/* clock source : BRG */
    SCC_DELAY;
    *cr = pTyCoDv->clockModeWR14;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR3;		/* write reg 3 - receive params */
    SCC_DELAY;
    *cr = SCC_WR3_RX_8_BITS | SCC_WR3_RX_EN;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR5;		/* tx params */
    SCC_DELAY;
    *cr = SCC_WR5_TX_8_BITS | SCC_WR5_TX_EN | SCC_WR5_DTR | SCC_WR5_RTS;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR15;		/* external/status interrupt control */
    SCC_DELAY;
    *cr = zero;
    SCC_DELAY;

    *cr = SCC_WR0_RST_INT;		/* reset external interrupts */
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR1;		/* int and xfer mode */
    SCC_DELAY;
    *cr = SCC_WR1_INT_ALL_RX | SCC_WR1_TX_INT_EN;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR9;		/* master interrupt control */
    SCC_DELAY;
    *cr = SCC_WR9_MIE | pTyCoDv->intType; /* enable interrupts */
    SCC_DELAY;

    *cr = zero;				/* reset SCC register counter */
    SCC_DELAY;

    intUnlock (oldlevel);
    }

/*******************************************************************************
*
* tyCoOpen - open file to USART
*/

LOCAL int tyCoOpen
    (
    TY_CO_DEV * pTyCoDv,
    char      * name,
    int         mode
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
    TY_CO_DEV * pTyCoDv,	/* device to control */
    int         request,	/* request code */
    int         arg		/* some argument */
    )
    {
    int			oldlevel;	/* current interrupt level mask */
    int			baudConstant;
    STATUS		status;
    volatile char *	cr;		/* SCC control reg adr */

    switch (request)
	{
	case FIOBAUDRATE:

	    if (arg < 50 || arg > 38400)
	        {
		status = ERROR;		/* baud rate out of range */
		break;
	        }

	    /* Calculate the baud rate constant for the new baud rate
	     * from the input clock frequency.  This assumes that the
	     * divide-by-16 bit is set in the Am85C30 WR4 register (done
	     * in tyCoInitChannel).
	     */

	    baudConstant = ((pTyCoDv->baudFreq / 32) / arg) - 2;

	    cr = pTyCoDv->cr;

	    /* disable interrupts during chip access */

	    oldlevel = intLock ();

	    *cr = SCC_WR0_SEL_WR12;	/* LSB of baud constant */
	    SCC_DELAY;
	    *cr = (char)baudConstant;	/* write LSB */
	    SCC_DELAY;
	    *cr = SCC_WR0_SEL_WR13;	/* MSB of baud constant */
	    SCC_DELAY;
	    *cr = (char)(baudConstant >> 8); /* write MSB */
	    SCC_DELAY;

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
* This routine handles write interrupts from the SCC.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoIntWr
    (
    int channel			/* interrupting serial channel */
    )
    {
    char		outChar;
    TY_CO_DEV *		pTyCoDv = &tyCoDv [channel];
    volatile char *	cr = pTyCoDv->cr;

    if (pTyCoDv->created && tyITx (&pTyCoDv->tyDev, &outChar) == OK)
	{
	*pTyCoDv->dr = outChar;
	}
    else
	{
	/* no more chars to xmit now.  reset the tx int,
	 * so the SCC does not keep interrupting.
	 */

	*cr = SCC_WR0_RST_TX_INT;
	SCC_DELAY;
	}

    *cr = SCC_WR0_RST_HI_IUS;	/* end the interrupt acknowledge */
    SCC_DELAY;
    }

/*****************************************************************************
*
* tyCoIntRd - interrupt level input processing
*
* This routine handles read interrupts from the SCC
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoIntRd
    (
    int channel			/* interrupting serial channel */
    )
    {
    TY_CO_DEV *		pTyCoDv = &tyCoDv [channel];
    volatile char *	cr = pTyCoDv->cr;
    char		inchar;

    inchar = *pTyCoDv->dr;

    if (pTyCoDv->created)
	tyIRd (&pTyCoDv->tyDev, inchar);

    *cr = SCC_WR0_RST_HI_IUS;	/* reset the interrupt in the Am85C30 */
    SCC_DELAY;
    }

/**********************************************************************
*
* tyCoIntEx - miscellaneous interrupt processing
*
* This routine handles miscellaneous interrupts on the SCC
*
* RETURNS: N/A
*
* NOMANUAL
*/

void tyCoIntEx
    (
    int channel			/* interrupting serial channel */
    )
    {
    TY_CO_DEV *		pTyCoDv = &tyCoDv [channel];
    volatile char *	cr = pTyCoDv->cr;

    *cr = SCC_WR0_ERR_RST;	/* reset errors */
    SCC_DELAY;
    *cr = SCC_WR0_RST_HI_IUS;	/* reset the interrupt in the Am85C30 */
    SCC_DELAY;
    }

/********************************************************************************
* tyCoInt - interrupt level processing
*
* This routine handles interrupts from both of the SCCs.
* We determine from the parameter which SCC interrupted, then look at
* the code to find out which channel and what kind of interrupt.
*
* NOMANUAL
*/

void tyCoInt
    (
    int sccNum
    )
    {
    volatile char *	cr;
    char		intStatus;
    TY_CO_DEV *		pTyCoDv;
    char		outChar;

    /* We need to find out which channel interrupted.  We need to read
     * the B channel of the interrupting SCC to find out which channel
     * really interrupted.  Note that things are set up so that the A
     * channel is channel 0, even though on the chip it is the one with
     * the higher address
     */  

    pTyCoDv = &tyCoDv [(sccNum * 2) + 1];
    cr = pTyCoDv->cr;
    SCC_DELAY;

    *cr = SCC_WR0_SEL_WR2;                      /* read reg 2 */
    SCC_DELAY;
    intStatus = *cr;

    if ((intStatus & 0x08) != 0)
        {                               /* the A channel interrupted */
        --pTyCoDv;
        cr = pTyCoDv->cr;
        }

    switch (intStatus & 0x06)
        {
        case 0x04:                      /* RxChar Avail */
            if (pTyCoDv->created)
                tyIRd (&pTyCoDv->tyDev, *pTyCoDv->dr);
            SCC_DELAY;
            break;
 
        case 0x00:                      /* Tx Buffer Empty */
            if (pTyCoDv->created && (tyITx (&pTyCoDv->tyDev, &outChar) == OK))
                *pTyCoDv->dr = outChar;
            else
                {
                /* no more chars to xmit now.  reset the tx int,
                 * so the SCC doesn't keep interrupting us.
		 */
 
                *cr = SCC_WR0_RST_TX_INT;
                }
            SCC_DELAY;
            break;
 
        case 0x02:                      /* External Status Change */
            *cr = SCC_WR0_ERR_RST;
            SCC_DELAY;
            outChar = *pTyCoDv->dr;          /* throw away char */
            break;
 
        case 0x06:                      /* Special receive condition */
            *cr = SCC_WR0_ERR_RST;
            SCC_DELAY;
            break;                      /* ignore */
 
        }
 
    *cr = SCC_WR0_RST_HI_IUS;   /* Reset the Highest interrupt in the AM85C30 */
    SCC_DELAY;

    }

/*******************************************************************************
*
* tyCoStartup - transmitter startup routine
*
* Call interrupt level character output routine.
*/

LOCAL void tyCoStartup
    (
    TY_CO_DEV * pTyCoDv 		/* ty device to start up */
    )
    {
    char outChar;

    if (tyITx (&pTyCoDv->tyDev, &outChar) == OK)
	*pTyCoDv->dr = outChar;
    }
