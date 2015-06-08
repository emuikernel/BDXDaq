/* tyC16Drv.c - The microbar COM16 serial i/o tty handler */

/* Copyright 1984,1985,1986,1987,1988,1989 Wind River Systems, Inc. */
extern char copyright_wind_river[]; static char *copyright=copyright_wind_river;

/*
modification history
--------------------
01h,06jun88,dnw  changed taskSpawn/taskCreate args.
01g,30may88,dnw  changed to v4 names.
01f,04may88,jcf  changed semaphores to be consistent with new semLib.
01e,28jan88,jcf  made kernel independent.
01d,19nov87,ecs  documentation.
01c,06nov87,ecs  documentation & delinting.
01b,23jun87,ecs  minor documentation
01a,22jan87,ecs  written, by severely modifying tyCoDrv.c,v04e.
*/

/*
DESCRIPTION
This is the driver for the Microbar COM16 serial controller board.

USER CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly: tyC16Drv () to
initialize the driver, and tyC16DevCreate () to create devices.

TYC16DRV
Before using the driver, it must be initialized by calling the routine:
.CS
 STATUS tyC16Drv (ioBase, intVector, intLevel)
     UTINY *ioBase;		/* base i/o address *
     FUNCPTR *intVector;	/* base interrupt vector *
     int intLevel;		/* interrupt level *
.CE
The first time tyC16DRV is called, it will install board number 0,
the second time board number 1, and so on up to N_COM16_BOARDS.
The status returned is OK, or ERROR if non-existent hardware or it
ran out of memory.

This routine should be called exactly once, before any reads, writes, or
tyC16DevCreates.  Normally, it is called from usrRoot (2) in usrConfig.c.

CREATING TERMINAL DEVICES
Before a terminal can be used, it must be created.  This is done
with the tyC16DevCreate call.
Each port to be used should have exactly one device associated with it,
by calling this routine.

.CS
 STATUS tyC16DevCreate (name, channel, rdBufSize, wBufSize, baudRate)
     char *name;     /* Name to use for this device *
     int channel;    /* encoded COM16 port number of device;
                      * upper word is board number,
                      * lower word is line number *
     int rdBufSize;  /* Read buffer size, in bytes *
     int wBufSize;   /* Write buffer size, in bytes *
     int baudRate;   /* Baud rate to create device with *
.CE

For instance, to create the device "/ty/0", with buffer sizes of 512 bytes,
at 9600 baud, the proper call would be:
.CS
   tyC16DevCreate ("/ty/0", 0, 512, 512, 9600);
.CE
IOCTL
This driver responds to all the same ioctl codes as a normal ty driver.
All baud rates between 50 and 56000 are available.
*/

#include "vxWorks.h"
#include "ioLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "semLib.h"
#include "tyLib.h"
#include "taskLib.h"


#define N_COM16_CHANNELS	8	/* number of serial channels */
#define C16_BUF_SIZE	64
#define WAIT_LIMIT	2000
#define CHAN_MASK	0x07	/* 8 channels */
#define	N_COM16_BOARDS	8	/* up to 8 COM16 boards */

typedef struct		/* C16_PORT - com16 i/o ports */
    {
    UTINY cmdRslt;	/* 0: send commands and data, read results */
    UTINY ieStat;	/* 1: send interrupt enable, read interrupt status */
    UTINY txdTln;	/* 2: transmit data buffer (xmit mode 00 only) */
    UTINY fill1;	/* 3: not used */
    UTINY rbf;		/* 4: read data buffer (receive mode 0 only) */
    UTINY fill2;	/* 5: not used */
    UTINY rbst;		/* 6: read buffer status (receive mode 0 only) */
    } C16_PORT;

/* function codes sent to cmdRslt */

#define COMFC_READ_ATTN_REG	0x00	/* read attention register */
#define COMFC_INIT_INT_MODE	0x10	/* initialize interrupt mode */
#define COMFC_INIT_CNTRL_TABLE	0x20	/* initialize control table */
#define COMFC_LINE_PARAMS	0x30	/* read/set line parameters */
#define COMFC_MODEM_CONTROLS	0x50	/* read/set modem controls */
#define COMFC_INT_ENABLES	0x60	/* read/set interrupt enables */
#define COMFC_BEGIN_TRANSMIT	0x70	/* begin transmission */
#define COMFC_HALT_TRANSMIT	0x80	/* halt/abort transmission */
#define COMFC_PAUSE_TRANSMIT	0x90	/* pause transmission */
#define COMFC_RESUME_TRANSMIT	0xa0	/* resume transmission */
#define COMFC_ENABLE_RECEPTION	0xb0	/* halt/restore reception */
#define COMFC_SEND_BREAK	0xc0	/* send break */
#define COMFC_iSBX_PERIPH	0xd0	/* read/write iSBX peripheral */
#define COMFC_INIT_DMA_XFER	0xe0	/* initialize iSBX DMA transfer */
#define COMFC_MISC_COMMAND	0xf0	/* miscellaneous command */

/* bits written to, read from ieStat */

#define COMST_IE		0x80	/* interrupt enable (W & R) */
#define COMST_INIT		0x40	/* initialize COM16 (W) */
#define COMST_RDY		0x40	/* ready flag (R) */
#define COMST_NXM		0x20	/* non-existent memory (W & R) */
#define COMST_DTRQ		0x10	/* data request flag (R) */
#define COMST_ATTN		0x08	/* attention flag (R) */
#define COMST_TBE		0x04	/* transmit buffer empty flag (R) */
#define COMST_RBF		0x02	/* read buffer full flag (R) */

/* possible values in attention register after COMFC_READ_ATTN_REG */

#define ATTN_NOTHING		0x00	/* COM16 has no message for us */
#define ATTN_INIT_COMPLETE	0x01	/* initialization complete */
#define ATTN_MINTR0		0x02	/* iSBX periph. module MINTR0 */
#define ATTN_MINTR1		0x03	/* iSBX periph. module MINTR1 */
#define ATTN_DMA_COMPLETE	0x04	/* iSBX DMA transfer complete */
#define ATTN_MODEM		0x10	/* modem signal change */
#define ATTN_10_XMIT_COMPLETE	0x20	/* transmission complete (mode 10) */
#define ATTN_PACKET_RECEIVED	0x28	/* received packet (mode 1) */
#define ATTN_XMIT_UNDERRUN	0x30	/* synchronous transmission underrun */
#define ATTN_RECEIVER_ERROR	0x40	/* receiver error (mode 1) */
#define ATTN_BREAK		0x50	/* BREAK condition received */
#define ATTN_RECEIVE_OVERRUN	0x60	/* receive overrun error (mode 1) */
#define ATTN_XMIT_COMPLETE	0x70	/* transmission complete (mode 01) */

typedef struct		/* C16_DEV - COM16 device descriptor */
    {
    TY_DEV tyDev;		/* tty device descriptor */
    int board;			/* COM16 board number of device */
    UTINY line;			/* COM16 line number of device */
    TBOOL xbusy;		/* transmitter busy */
    char cbuf[C16_BUF_SIZE];	/* character buffer */
    } C16_DEV;

typedef struct 		/* C16_CTRL - COM16 controller structure */
    {
    C16_PORT *port;		/* COM16 i/o port base address */
    SEM_ID boardSemId;		/* semaphore id for exclusive access to COM16 */
    SEM_ID intSemId;		/* interrupt semaphore id */
    int driverNum;		/* number assigned to this driver */
    C16_DEV *c16Dev[N_COM16_CHANNELS];	/* device descriptors */
    } C16_CTRL;

LOCAL C16_CTRL *c16Ctrl[N_COM16_BOARDS];	/* COM16 board controllers */
LOCAL int numBoards = 0;	/* number of COM16 boards installed */

/* forward declarations */

LOCAL VOID tyC16Startup ();
LOCAL int tyC16Open ();
LOCAL STATUS tyC16Close ();
LOCAL int tyC16Read ();
LOCAL int tyC16Write ();
LOCAL int tyC16Ioctl ();
LOCAL VOID tyC16Int ();
LOCAL VOID tyC16d ();
LOCAL UTINY sendCommand ();
LOCAL UTINY readData ();

/*******************************************************************************
*
* tyC16Drv - install COM16 serial i/o tty driver
*
* The first time called will install board number 0,
* second time board number 1, and so on up to N_COM16_BOARDS.
*
* RETURNS: OK, or ERROR if non-existent hardware or ran out of memory.
*/

STATUS tyC16Drv (ioBase, intVector, intLevel)
    UTINY *ioBase;	/* base i/o address */
    FUNCPTR *intVector;	/* base interrupt vector */
    int intLevel;	/* interrupt level */

    {
    char test;
    C16_CTRL *pCtrl;	/* com 16 controller structure */

    if ((pCtrl = (C16_CTRL *) malloc (sizeof (C16_CTRL))) == NULL)
	return (ERROR);

    if (numBoards >= N_COM16_BOARDS)
	return (ERROR);			/* no more room */

    c16Ctrl[numBoards++] = pCtrl;	/* save a pointer to controller */

    /* probe for com16 hardware;  if it's not there, connect major number
     *   to null device routines and quit */

    if (vxMemProbe ((char *)ioBase, READ, 1, &test) < OK)
	{
	pCtrl->driverNum = iosDrvInstall ((FUNCPTR) NULL, (FUNCPTR) NULL, 
					  (FUNCPTR) NULL, (FUNCPTR) NULL, 
					  (FUNCPTR) NULL, (FUNCPTR) NULL, 
					  (FUNCPTR) NULL);
	return (ERROR);
	}

    pCtrl->port = (C16_PORT *) ioBase;	/* point to i/o port */

    pCtrl->intSemId = semCreate ();	/* initialize interrupt semaphore */
    pCtrl->boardSemId = semCreate ();	/* initialize COM16 mutex semaphore */
    semGive (pCtrl->boardSemId);	/* free to first taker */

    /* set up interrupt vector */

    intConnect (intVector, tyC16Int, (int) pCtrl);
    sysIntEnable (intLevel);		/* enable the interrupt */

    tyC16HrdInit (pCtrl);		/* initialize hardware */

    taskSpawn ("tyC16Task", 1, VX_SUPERVISOR_MODE, 1000, tyC16d,
	       (int) pCtrl, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    pCtrl->driverNum = iosDrvInstall (tyC16Open, tyC16Close, tyC16Open,
				      tyC16Close, tyC16Read, tyC16Write,
				      tyC16Ioctl);

    return ((pCtrl->driverNum == ERROR) ? ERROR : OK);
    }
/******************************************************************
*
* tyC16DevCreate - create a device for a COM16 port
*/

STATUS tyC16DevCreate (name, channel, rbSize, wbSize, baudRate)
    char *name;		/* name of device */
    int channel;	/* encoded COM16 port number of device;
			 * upper word is board number,
			 * lower word is line number */
    int rbSize;		/* size of read buffer */
    int wbSize;		/* size of write buffer */
    int baudRate;	/* baud rate of device */

    {
    C16_DEV *pC16Dev;
    int line = channel & CHAN_MASK;
    int board = (channel >> 16) & 0xff;

    if (board >= numBoards)
	return (ERROR);		/* board not installed */

    /* allocate device descriptor */

    if ((pC16Dev = (C16_DEV *) malloc (sizeof (C16_DEV))) == NULL)
	return (ERROR);

    /* initialize device descriptor */

    if (tyDevInit ((TY_DEV_ID) pC16Dev, rbSize, wbSize, tyC16Startup) != OK)
	return (ERROR);
    pC16Dev->board  = board;
    pC16Dev->line   = line;
    pC16Dev->xbusy  = FALSE;

    c16Ctrl[board]->c16Dev[line] = pC16Dev;

    tyC16Ioctl (pC16Dev, FIOBAUDRATE, baudRate);	/* init baud rate */

    return (iosDevAdd ((DEV_HDR *) pC16Dev, name, c16Ctrl[board]->driverNum));
    }
/******************************************************************
*
* tyC16HrdInit - initialize the COM16 board.
*/

LOCAL VOID tyC16HrdInit (pCtrl)
    FAST C16_CTRL *pCtrl;	/* com 16 controller structure */

    {
    int oldlevel;	/* current interrupt level mask */
    FAST UTINY lineNum;
    UTINY attnReg;	/* contents of attention register */

    oldlevel = intLock ();

    pCtrl->port->ieStat = COMST_INIT;		/* software reset */
    while (!(pCtrl->port->ieStat & COMST_ATTN))
	;					/* wait for attention flag */
    if ((attnReg = sendCommand (pCtrl, COMFC_READ_ATTN_REG))
	!= ATTN_INIT_COMPLETE)
	logMsg ("ATTN after software reset = %x\n", attnReg);


    sendCommand (pCtrl, COMFC_INIT_INT_MODE);	/* initialize interrupt mode */
    sendData (pCtrl, 0x01);			/* non-vectored */

    /* set the interrupt enable flags: non-existent memory, attention,
       and read buffer full */

    sendCommand (pCtrl, COMFC_INT_ENABLES);
    sendData (pCtrl, 0xaa);

    pCtrl->port->ieStat = COMST_IE;		/* enable interrupts */


    /* set line parameters for each line */

    for (lineNum = 0; lineNum < N_COM16_CHANNELS; ++lineNum)
	{
	sendCommand (pCtrl, COMFC_LINE_PARAMS | lineNum);

	sendData (pCtrl, 0x05);	/* receive in FIFO mode, transmit in
				 * pseudo-DMA mode */
	sendData (pCtrl, 0);	/* transmit & receive at 9600 baud */
	sendData (pCtrl, 0xc1);	/* receive 8-bit characters */
	sendData (pCtrl, 0x04);	/* no parity, 1 stop bit */
	sendData (pCtrl, 0x68);	/* transmit 8-bit characters */
	sendData (pCtrl, 0);	/* don't care */
	sendData (pCtrl, 0);	/* don't care */

	sendCommand (pCtrl, COMFC_ENABLE_RECEPTION | lineNum);
	sendData (pCtrl, 0);	/* disable reception on this line */

	sendCommand (pCtrl, COMFC_MODEM_CONTROLS | lineNum);
	sendData (pCtrl, 0x20);	/* ignore modem signal changes */
	}

    intUnlock (oldlevel);
    }

/*******************************************************************************
*
* tyC16Open - open file to microbar COM16 port
*
* ARGSUSED
*/

LOCAL int tyC16Open (pC16Dev, name, mode)
    C16_DEV *pC16Dev;
    char *name;
    int mode;

    {
    C16_CTRL *pCtrl = c16Ctrl[pC16Dev->board];

    semTake (pCtrl->boardSemId);		/* get mutex for COM16 board */
    sendCommand (pCtrl, COMFC_ENABLE_RECEPTION | pC16Dev->line);
    sendData (pCtrl, 0x01);		/* enable reception on this line */
    semGive (pCtrl->boardSemId);		/* relinquish COM16 mutex */

    return ((int) pC16Dev);
    }
/*******************************************************************************
*
* tyC16Close - close COM16 port
*
* This routine disables reception on the line associated with the given device.
*
* ARGSUSED
*/

LOCAL STATUS tyC16Close (pC16Dev)
    C16_DEV *pC16Dev;		/* device to be closed */

    {
    C16_CTRL *pCtrl = c16Ctrl[pC16Dev->board];

    semTake (pCtrl->boardSemId);		/* get mutex for COM16 board */
    sendCommand (pCtrl, COMFC_ENABLE_RECEPTION | pC16Dev->line);
    sendData (pCtrl, 0);		/* disable reception on this line */
    semGive (pCtrl->boardSemId);		/* relinquish COM16 mutex */

    return (OK);
    }
/**********************************************************************
*
* tyC16Read - Task level read routine for microbar COM16 port.
*/

LOCAL int tyC16Read (pC16Dev, buffer, maxbytes)
    C16_DEV *pC16Dev;
    char *buffer;
    int maxbytes;

    {
    return (tyRead ((TY_DEV_ID) pC16Dev, buffer, maxbytes));
    }
/**********************************************************************
*
* tyC16Write - Task level write routine for microbar COM16 port.
*/

LOCAL int tyC16Write (pC16Dev, buffer, nbytes)
    C16_DEV *pC16Dev;
    char *buffer;
    int nbytes;

    {
    return (tyWrite ((TY_DEV_ID) pC16Dev, buffer, nbytes));
    }
/***********************************************************************
*
* tyC16Ioctl - special device control
*
* This routine handles baud rate changes. All other i/o control commands
* get passed on to tyIoctl.
*/

LOCAL int tyC16Ioctl (pC16Dev, request, arg)
    C16_DEV *pC16Dev;		/* device to control */
    int request;		/* request code */
    int arg;			/* some argument */

    {
    int oldLevel;
    FAST int ix;
    UTINY lineParams[7];	/* read COM16 line parameters into here */
    C16_CTRL *pCtrl = c16Ctrl[pC16Dev->board];

    switch (request)
	{
	case FIOBAUDRATE:
	    oldLevel = intLock ();	/* lock out interrupts */

	    /* read the existing line parameters */

	    sendCommand (pCtrl, COMFC_LINE_PARAMS | pC16Dev->line);
	    sendData (pCtrl, 0);		/* read line parameters */
	    for (ix = 0; ix < 7; ++ix)
		lineParams[ix] = readData (pCtrl);

	    lineParams[0] |= 0x01;	/* enable updating of line parameters */

	    /* substitute new baud rate into line parameters */

	    ix = baudRateCode (arg);		/* get COM16 baud rate code */
	    lineParams[1] = ix | (ix << 4);	/* same xmit & receive speeds */

	    /* write out new line parameters */

	    sendCommand (pCtrl, COMFC_LINE_PARAMS | pC16Dev->line);
	    for (ix = 0; ix < 7; ++ix)
		sendData (pCtrl, lineParams[ix]);

	    intUnlock (oldLevel);	/* re-enable interrupts */

	    return (0);

	default:
	    return (tyIoctl ((TY_DEV_ID) pC16Dev, request, arg));
	}
    }

LOCAL BOOL c16intlog;		/* XXX */
LOCAL int c16rdcnt;		/* XXX */
/*******************************************************************************
*
* tyC16Int - interrupt level processing
*
* This routine handles interrupts from the COM16 board. Read-buffer-full
* interrupts are serviced at interrupt level. For other interrupts, the
* interrupt semaphore is given, allowing tyC16d to execute another loop.
*/

LOCAL VOID tyC16Int (pCtrl)
    C16_CTRL *pCtrl;		/* com 16 controller structure */

    {
    /* service interrupts at task level */

    if (c16intlog)
	logMsg ("boom\n");

    semGive (pCtrl->intSemId);	/* nudge tyC16d */
    }
/*******************************************************************************
*
* tyC16d - a daemon to handle COM16 interrupts at task level
*/

LOCAL VOID tyC16d (pCtrl)
    C16_CTRL *pCtrl;		/* com 16 controller structure */

    {
    FAST UTINY zero = 0;

    FOREVER
	{
	pCtrl->port->ieStat = COMST_IE;	/* enable interrupts */
	semTake (pCtrl->intSemId);	/* wait for an interrupt */
	pCtrl->port->ieStat = zero;	/* disable interrupts */

	semTake (pCtrl->boardSemId);	/* get mutex for COM16 board */
	handleInt (pCtrl);
	semGive (pCtrl->boardSemId);	/* relinquish COM16 mutex */
	}
    }
/************************************************************************
*
* handleInt - deal with com16 interrupt
*/

VOID handleInt (pCtrl)
    C16_CTRL *pCtrl;		/* com 16 controller structure */

    {
    FAST UTINY rch;		/* character received from read buffer */
    FAST UTINY rbst;		/* byte read from read buffer status register */
    FAST C16_DEV *pC16Dev;	/* ty device to give received character to */

    if (pCtrl->port->ieStat & COMST_NXM)	/* non-existent memory */
	{
	pCtrl->port->ieStat = COMST_NXM;	/* clear it */
	logMsg ("NXM interrupt\n");		/* log it */
	}

    while (pCtrl->port->ieStat & COMST_RBF)	/* read-buffer-full interrupt */
	{
	rch = pCtrl->port->rbf;		/* get character from read buffer */
	rbst = pCtrl->port->rbst;	/* get status from status register */

	pC16Dev = pCtrl->c16Dev[rbst & CHAN_MASK];	/* find out which dev */
	if (pC16Dev == NULL)
	    {
	    logMsg ("RBF on unknown device.  Rbf: %x,  Rbst: %x\n",
		    rch, rbst);
	    continue;			/* don't process this one further */
	    }

	if (rbst & 0x70)
	    logMsg ("Read buffer status: %x\n", rbst);	/* XXX error */

	tyIRd ((TY_DEV_ID) pC16Dev, (char) rch);	/* give char to io sys*/
	++c16rdcnt;
	}

    while (pCtrl->port->ieStat & COMST_ATTN)
	attention (pCtrl);		/* handle attention interrupt */
    }
/*******************************************************************************
*
* attention - handle COM16 ATTN interrupt
*/

LOCAL VOID attention (pCtrl)
    C16_CTRL *pCtrl;

    {
    FAST UTINY attnReg;		/* contents of attention register put here */
    FAST int nChars;		/* number of characters to transmit */
    FAST C16_DEV *pC16Dev;	/* ty device to give attention to */
    int channel;		/* channel number that COM16 is excited about */

    attnReg = sendCommand (pCtrl, COMFC_READ_ATTN_REG);

    switch (attnReg)
	{
	case ATTN_NOTHING:		/* COM16 has nothing to say to us */
	case ATTN_INIT_COMPLETE:	/* initialization complete */
	case ATTN_MINTR0:		/* XXX shouldn't get this */
	case ATTN_MINTR1:		/* XXX shouldn't get this */
	case ATTN_DMA_COMPLETE:		/* XXX shouldn't get this */
	    return;
	}

    channel = attnReg & CHAN_MASK;	/* last 3 bits are channel # (if any) */
    pC16Dev = pCtrl->c16Dev[channel];	/* pointer to device (if any) */

    switch (attnReg & 0xf8)		/* XXX assumes <= 8 channels */
	{
	case ATTN_10_XMIT_COMPLETE:	/* XXX shouldn't get this */
	case ATTN_XMIT_UNDERRUN:	/* XXX shouldn't get this */
	case ATTN_MODEM:		/* XXX shouldn't get this */

	case ATTN_PACKET_RECEIVED:
	case ATTN_RECEIVER_ERROR:
	case ATTN_BREAK:
	case ATTN_RECEIVE_OVERRUN:
	    logMsg ("Attention says: %x\n", attnReg);
	    break;

	case ATTN_XMIT_COMPLETE:
	    /* a line has finished transmitting, see if there's more to do */

	    if (pC16Dev == NULL)
		break;				/* uh-oh, no such device */

	    pC16Dev->xbusy = FALSE;

	    if ((nChars = getCharsToSend (pC16Dev)) > 0)
		transmit (pC16Dev, nChars);	/* start transmitting */
	    break;
	}
    }
/**********************************************************************
*
* tyC16Startup - Transmitter startup routine
*
* Transmission initiation, for tasks other than tyC16d.
*/

LOCAL VOID tyC16Startup (pC16Dev)
    C16_DEV *pC16Dev;		/* ty device to start up */

    {
    FAST int nChars;		/* # of characters put into device buffer */
    C16_CTRL *pCtrl = c16Ctrl[pC16Dev->board];

    if (pC16Dev->xbusy)
	return;				/* don't bother transmitter now */

    if ((nChars = getCharsToSend (pC16Dev)) == 0)
	return;				/* nothing to output */

    semTake (pCtrl->boardSemId);		/* get mutex for COM16 board */
    transmit (pC16Dev, nChars);		/* start transmitting */
    semGive (pCtrl->boardSemId);		/* relinquish COM16 mutex */
    }
/*******************************************************************************
*
* getCharsToSend - fill device buffer with characters to send to transmitter
*
* This routine puts up to C16_BUF_SIZE characters into the given C16_DEV's
* buffer.
*
* RETURNS: number of characters put into device buffer
*/

LOCAL int getCharsToSend (pC16Dev)
    FAST C16_DEV *pC16Dev;	/* ty device with buffer to fill up */

    {
    FAST int ix;		/* counts characters put into device buffer */

    for (ix = 0; ix < C16_BUF_SIZE; ++ix)
	if (tyITx ((TY_DEV_ID) pC16Dev, &pC16Dev->cbuf[ix]) != OK)
	    break;			/* that's all that's available */

    return (ix);
    }
/*******************************************************************************
*
* transmit - initiate transmission to COM16 board
*
* Caller must possess pCtrl->boardSemId semaphore before calling this routine.
*/

LOCAL VOID transmit (pC16Dev, nChars)
    C16_DEV *pC16Dev;		/* ty device to transmit on */
    FAST int nChars;		/* # of characters to transmit */

    {
    FAST int bufAddr = (int)(pC16Dev->cbuf);
    UTINY result;
    FAST C16_CTRL *pCtrl = c16Ctrl[pC16Dev->board];

    pC16Dev->xbusy = TRUE;

    result = sendCommand (pCtrl, COMFC_BEGIN_TRANSMIT | pC16Dev->line);
    if (result & 0x80)
	{
	logMsg ("transmit error: %x\n", result & 0x7f);
	return;
	}

    sendData (pCtrl, (UTINY) (nChars & 0xff));		/* l.b. of byte count */
    sendData (pCtrl, (UTINY) ((nChars & 0xff00) >> 8));	/* high byte */
    sendData (pCtrl, (UTINY) (bufAddr & 0xff));		/* l.b. of buf addr */
    sendData (pCtrl, (UTINY) ((bufAddr >> 8) & 0xff));	/* middle byte */
    sendData (pCtrl, (UTINY) ((bufAddr >> 16) & 0xff));	/* high byte */
    }
/*******************************************************************************
*
* waitForDataRequest - wait until com16 DTRQ status bit goes high
*
* RETURNS: TRUE if we never got a data request
*/

LOCAL BOOL waitForDataRequest (pCtrl)
    C16_CTRL *pCtrl;	/* com 16 controller structure */

    {
    FAST int cnt;	/* low counts before reading data */

    for (cnt = 0;
	 cnt < WAIT_LIMIT && !(pCtrl->port->ieStat & COMST_DTRQ);
	 ++cnt)
	;

    return (cnt >= WAIT_LIMIT);
    }
/*******************************************************************************
*
* sendCommand - send message to COM16 command port
*
* RETURNS: contents of COMRSLT, the encoded result of the command.
*
* Note that if the command is one that causes data to be placed in the result
* register and DTRQ to be set, such as many of the Miscellaneous commands,
* that data is returned by sendCommand - a separate readData call is not needed.
*/

LOCAL UTINY sendCommand (pCtrl, cmd)
    FAST C16_CTRL *pCtrl;	/* com 16 controller structure */
    UTINY cmd;			/* command to be sent */

    {
    FAST UTINY rslt;	/* result of command */
    FAST int cnt1;	/* low counts before sending command */
    FAST int cnt2;	/* high counts after sending command */
    FAST int cnt3;	/* low counts after sending command */

    /* wait for RDY bit in COMSTAT to be high */

    for (cnt1 = 0;
	 cnt1 < WAIT_LIMIT && !(pCtrl->port->ieStat & COMST_RDY);
	 ++cnt1)
	;

    if (cnt1 >= WAIT_LIMIT)
	{
	logMsg ("sendCommand (%x) not ready: COMSTAT = %x\n",
		cmd, pCtrl->port->ieStat);
	return (0);
	}

    cnt2 = cnt3 = 0;

    pCtrl->port->cmdRslt = cmd;		/* send the command */

    /* the RDY bit in COMSTAT will go low then high; after that the result
       register should be valid */

    while (pCtrl->port->ieStat & COMST_RDY)	/* wait for !RDY */
	if (++cnt2 > WAIT_LIMIT)
	    break;				/* don't wait too long */

    /* wait for RDY | DTRQ */

    while (!(pCtrl->port->ieStat & (COMST_RDY | COMST_DTRQ)))
	if (++cnt3 > WAIT_LIMIT)
	    break;				/* don't wait too long */

    rslt = pCtrl->port->cmdRslt;		/* read the result */

/*    logMsg ("Command: %x  Count1: %d  Count2: %d  Count3: %d  Result: %x\n",
/*	    cmd, cnt1, cnt2, cnt3, rslt);
*/

    return (rslt);
    }
/*******************************************************************************
*
* sendData - send message to COM16 command parameter port
*/

LOCAL VOID sendData (pCtrl, data)
    FAST C16_CTRL *pCtrl;	/* com 16 controller structure */
    UTINY data;			/* data to be sent */

    {
    if (waitForDataRequest (pCtrl))
	{
	logMsg ("sendData (%x) not ready: COMSTAT = %x\n",
		data, pCtrl->port->ieStat);
	return;
	}

    pCtrl->port->cmdRslt = data;
    }
/*******************************************************************************
*
* readData - get data from COM16 command parameter port
*
* RETURNS: contents of command parameter port
*/

LOCAL UTINY readData (pCtrl)
    C16_CTRL *pCtrl;	/* com 16 controller structure */

    {
    if (waitForDataRequest (pCtrl))
	{
	logMsg ("readData not ready: COMSTAT = %x\n", pCtrl->port->ieStat);
	return (0);
	}

    return (pCtrl->port->cmdRslt);
    }
/*******************************************************************************
*
* baudRateCode - return COM16 baud rate code corresponding to baud rate
*
* RETURNS: COM16 baud rate code, default 0 (9600 baud) if unknown baud rate
*/

LOCAL int baudRateCode (baudrate)
    int baudrate;	/* baud rate for which to find code */

    {
    switch (baudrate)
	{
	case 50:    return (0x01);
	case 75:    return (0x02);
	case 110:   return (0x03);
	case 134:   return (0x04);
	case 150:   return (0x05);
	case 200:   return (0x06);
	case 300:   return (0x07);
	case 600:   return (0x08);
	case 1200:  return (0x09);
	case 1800:  return (0x0a);
	case 2400:  return (0x0b);
	case 4800:  return (0x0c);
	case 9600:  return (0x00);
	case 19200: return (0x0d);
	case 38400: return (0x0e);
	case 56000: return (0x0f);
	default:    return (0x00);	/* 9600 - since it's so popular */
	}
    /* NOTREACHED */
    }
