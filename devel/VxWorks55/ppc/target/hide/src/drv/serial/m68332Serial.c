/* m68332Serial.c - Motorola MC68332 tty driver*/

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01l,12feb93,caf  documented that 110 is minimum baud rate.
01k,12feb93,caf  fixed tyCoIoctl() parameter checking (SPR 1810).
		 updated copyright notice.
01j,01dec92,jdi  NOMANUAL for tyCoInt() - SPR 1808.
01i,28oct92,caf  tweaked tyCoDevCreate() documentation.
01h,13aug92,caf  fixed baud calculation.  ansified.
01g,28jul92,caf  fixed ansi warnings.
01f,18jul92,smb  Changed errno.h to errnoLib.h.
01e,26may92,rrr  the tree shuffle
01d,24jan92,jdi  documentation cleanup.
01c,04oct91,rrr  passed through the ansification filter
		  -changed VOID to void
		  -changed copyright notice
01b,27sep91,jdi  documentation cleanup.
01a,19sep91,jcf  written.
*/

/*
DESCRIPTION
This is the driver for the Motorola MC68332 on-chip UART.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  tyCoDrv() to
initialize the driver, and tyCoDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling the
routine tyCoDrv().  This routine should be called exactly once, before any
reads, writes, or calls to tyCoDevCreate().  Normally, it is called from
usrRoot() in usrConfig.c.

Before a terminal can be used, it must be created using tyCoDevCreate().
Each port to be used should have exactly one device associated with it by
calling this routine.  The MC68332 has only one channel associated with it.

IOCTL FUNCTIONS
This driver responds to the same ioctl() codes as a normal tty driver; for
more information, see the manual entry for tyLib.  The available baud rates
are 110 to 38400.

SEE ALSO
tyLib
*/

/* includes */

#include "vxWorks.h"
#include "iv.h"
#include "errnoLib.h"
#include "ioLib.h"
#include "intLib.h"
#include "iosLib.h"
#include "tyLib.h"
#include "config.h"

/* defines */

#define DEFAULT_BAUD	9600

/* globals */

IMPORT TY_CO_DEV tyCoDv [];	/* device descriptors */

/* locals */

LOCAL int tyCoDrvNum;		/* driver number of this driver */

/* forward declarations */

LOCAL void	tyCoStartup ();
LOCAL int	tyCoOpen ();
LOCAL STATUS	tyCoIoctl ();
LOCAL void	tyCoHrdInit ();


/*******************************************************************************
*
* tyCoDrv - initialize the tty driver
*
* This routine initializes the serial driver, sets up interrupt vectors,
* and performs hardware initialization of the serial port.
*
* This routine should be called exactly once, before any reads, writes, or
* calls to tyCoDevCreate().  Normally, it is called from usrRoot() in
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
* RETURNS:
* OK, or ERROR if the driver is not installed, the channel is invalid, or
* the device already exists.
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
    FAST TY_CO_DEV *pTyCoDv;

    if (tyCoDrvNum <= 0)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    /* if this doesn't represent a valid channel, don't do it */

    if (channel != 0)
        return (ERROR);

    pTyCoDv = &tyCoDv [channel];

    /* if there is a device already on this channel, don't do it */

    if (pTyCoDv->created)
	return (ERROR);

    /* initialize the ty descriptor */

    if (tyDevInit (&pTyCoDv->tyDev, rdBufSize, wrtBufSize,
		   (FUNCPTR) tyCoStartup) != OK)
	return (ERROR);

    /* mark the device as created, and add the device to the I/O system */

    pTyCoDv->created = TRUE;
    *M332_QSM_SCCR1 |= QSM_SCCR1_RIE;		/* enable receiver interrupts */

    return (iosDevAdd (&pTyCoDv->tyDev.devHdr, name, tyCoDrvNum));
    }

/*******************************************************************************
*
* tyCoHrdInit - initialize the USART
*/

LOCAL void tyCoHrdInit (void)
    {
    FAST int oldlevel = intLock ();	/* LOCK INTERRUPTS */

    /* initialize QSM registers associated with SCI */

    *M332_QSM_QILR &= ~QSM_QILR_SCI_MASK;		/* disable sci ints */

    /* set default baud rate */

    *M332_QSM_SCCR0 = tyCoDv [0].baudFreq / (32 * DEFAULT_BAUD);

    *M332_QSM_SCCR1 = QSM_SCCR1_RE | QSM_SCCR1_TE;	/* enable rx/tx */
    *M332_QSM_QILR |= INT_LVL_SCI;			/* enable sci ints */

    intUnlock (oldlevel);		/* UNLOCK INTERRUPTS */
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
*
* INTERNAL
* The range of SCI baud rates depends on the system clock frequency.
* For 16 MHz systems, the practical range is 110 to 38400 baud.
*/

LOCAL STATUS tyCoIoctl
    (
    TY_CO_DEV *	pTyCoDv,	/* device to control */
    int		request,	/* request code      */
    int		arg		/* some argument     */
    )
    {
    FAST STATUS	status = ERROR;
    UINT16	baudScratch;

    switch (request)
	{
	case FIOBAUDRATE:
	    baudScratch = tyCoDv [0].baudFreq / (32 * arg);
	    if ((baudScratch >= 1) && (baudScratch <= 8191) && (arg <= 38400))
		{
		*M332_QSM_SCCR0 = baudScratch;
		status = OK;
		break;
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
* tyCoInt - interrupt level processing
*
* This routine handles interrupts from the SCI port.
*
* NOMANUAL
*/

void tyCoInt (void)
    {
    char outChar;

    if (*M332_QSM_SCSR & QSM_SCSR_TDRE)
	if (tyITx (&tyCoDv [0].tyDev, &outChar) == OK)
	    *M332_QSM_SCDR = outChar;
	else
	    *M332_QSM_SCCR1 &= ~QSM_SCCR1_TIE;

    if (*M332_QSM_SCSR & QSM_SCSR_RDRF)
	tyIRd (&tyCoDv [0].tyDev, *M332_QSM_SCDR);
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
    char outChar;

    if ((*M332_QSM_SCSR & QSM_SCSR_TDRE) &&
        (tyITx (&pTyCoDv->tyDev, &outChar) == OK))
	{
	*M332_QSM_SCDR   = outChar;
	*M332_QSM_SCCR1 |= QSM_SCCR1_TIE;
	}
    }
