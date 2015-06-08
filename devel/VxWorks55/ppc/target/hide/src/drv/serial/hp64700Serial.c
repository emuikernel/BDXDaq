/* hp64700Serial.c - Hewlett Packard HP64700 emulator SIM I/O driver */

/* Copyright 1984-1994 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,21jul94,jds  cleanup
01a,26jan94,dzb  written.
*/

/*
DESCRIPTION

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
calling this routine.

INTERNAL
This serial driver does not use tyLib by virtue of its connection to the host.
Select, line feed and flow control are not useful.
*/

/* includes */

#include "vxWorks.h"
#include "iosLib.h"
#include "intLib.h"
#include "errnoLib.h"
#include "config.h"
#include "semLib.h"
#include "stdlib.h"
#include "taskLib.h"

/* defines */

#define	RS_DELAY_V		0
#define	OP_DELAY_V		0
#define	TX_DELAY_V		0
#define	RX_DELAY_V		10

/* globals */

TY_CO_DEV tyCoDv [NUM_TTY];	/* device descriptors */
char hp64783Keyboard[] = HP64700_KEYBOARD;
char hp64783Display[]  = HP64700_DISPLAY;

SIMIO_CA_PROTECT hp64700ca [SIMIO_CA_MAX];

/* locals */

LOCAL int tyCoDrvNum;		/* driver number assigned to this driver */

/* forward declarations */

LOCAL void	tyCoHrdInit ();
LOCAL STATUS	tyCoReset ();
LOCAL STATUS	tyCoOpen ();
LOCAL int	tyCoRead ();
LOCAL int	tyCoWrite ();

/*******************************************************************************
*
* tyCoDrv - initialize the tty driver
*
* This routine initializes the serial driver, sets up interrupt vectors,
* and performs hardware initialization of the serial ports.
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
    LOCAL SEM_ID mutexSem = NULL; /* mutexSem init to NULL the first time */

    /* mutexSem should be created only the first time this routine is called */

    if (mutexSem == NULL)
	{
        if ((mutexSem = semMCreate (SEM_Q_FIFO)) == NULL)
            return (ERROR);
        }

    /* begin mutual exclusion */ 

    if (semTake (mutexSem, WAIT_FOREVER) == ERROR)
	return (ERROR);

    /* check if driver already installed */

    if (tyCoDrvNum > 0) 
	{
        semGive (mutexSem);
	return (OK);
	}

    /* initialize tyCoDv structure */

    tyCoDv [0].numChannels  = NUM_TTY;          /* num of serial channels */
    tyCoDv [0].created      = FALSE;            /* channel not created */
    tyCoDv [0].rxCA.options = O_RDONLY;
    tyCoDv [0].rxCA.pName   = (UINT8 *) hp64783Keyboard;
    tyCoDv [0].rxCA.pBuf    = (UINT8 *) HP64783_SIMIO_CA1;
    tyCoDv [0].txCA.options = O_WRONLY;
    tyCoDv [0].txCA.pName   = (UINT8 *) hp64783Display;
    tyCoDv [0].txCA.pBuf    = (UINT8 *) HP64783_SIMIO_CA2;


    tyCoHrdInit ();

    tyCoDrvNum = iosDrvInstall (tyCoOpen, (FUNCPTR) NULL, tyCoOpen,
			(FUNCPTR) NULL, tyCoRead, tyCoWrite, (FUNCPTR) NULL);

    /* end mutual exclusion */

    semGive (mutexSem);

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
*    tyCoDevCreate ("/tyCo/0", 0, 512, 512);
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
    if (tyCoDrvNum <= 0)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    /* if this doesn't represent a valid channel, don't do it */

    if ((channel < 0) || (channel >= tyCoDv [0].numChannels))
        return (ERROR);

    /* if this device already exists, don't create it */

    if (tyCoDv[channel].created)
	return (ERROR);

    /* mark the device as created, and add the device to the I/O system */

    tyCoDv[channel].created = TRUE;
    return (iosDevAdd (&tyCoDv[channel].devHdr, name, tyCoDrvNum));
    }

/*******************************************************************************
*
* tyCoHrdInit - initialize SIM I/O
*/

LOCAL void tyCoHrdInit (void)
    {
    TY_CO_DEV *        pTyCoDv;
    SIMIO_CA *         pCA;
    int                channel;
    int                ix;
    int                iy;
    SIMIO_CA_PROTECT * pSC = hp64700ca;

    /* initialize CA protection structures */

    for (ix = 0; ix < SIMIO_CA_MAX; ix++, pSC++)
	{
	pSC->active = FALSE;
	pSC->cAdrs = NULL;
	if ((pSC->semMId = semMCreate (SEM_Q_FIFO)) == NULL)
	    return;
	}

    for (channel = 0; channel < NUM_TTY; channel++)
	{
	pTyCoDv = &tyCoDv[channel];

	/* go through loop twice. Once for rxCA and once for txCA */

	iy = 0;
	while (iy < 2)
	    {
	    /* first rxCA then toggle to txCA */

	    pCA = ((iy == 0) ? &pTyCoDv->rxCA : &pTyCoDv->txCA); 

	    pCA->opened = FALSE;

            if (pCA->pName == NULL)
		continue;

            if (pCA->pBuf == (void *) NONE)
	        {
                pCA->pBuf = (UINT8 *) malloc (SIMIO_BUF_MAX);
	        if (pCA->pBuf == NULL)
	            return;
                }

            pSC = hp64700ca;
            for (ix = 0; (ix < SIMIO_CA_MAX) && (pSC->active); ix++)
		{
	        if (pSC->cAdrs == pCA->pBuf)
	    	    break;
		pSC++;
                }

            if ((!pSC->active) && (ix < SIMIO_CA_MAX))
	        {
	        pSC->active = TRUE;
	        pSC->cAdrs = pCA->pBuf;
                if (tyCoReset (pCA->pBuf) != OK)
		    {
	            pSC->active = FALSE;
	            pSC->cAdrs = NULL;
		    }
                }

	    iy++;

            }
	}

    }

/*******************************************************************************
*
* tyCoSendCommand - send a SIM I/O command
*
* RETURNS: 
*/

LOCAL UINT8 tyCoSendCommand
    (
    UINT8 command,
    UINT8 * pBuf,
    int delayVal
    )
    {
    *pBuf = command;

    FOREVER
	{
        if (*pBuf != command)	/* poll for result */
	    break;
	taskDelay (delayVal);
	}

    return (*pBuf);
    }

/*******************************************************************************
*
* tyCoReset - reset SIM I/O
*
* RETURNS:
*/

LOCAL STATUS tyCoReset
    (
    UINT8 * pBuf
    )
    {
    UINT8              errVal;
    int                ix;
    SIMIO_CA_PROTECT * pSC = hp64700ca;
    SEM_ID             pSI = NULL;

    /* find the channel */

    for (ix = 0; (ix < SIMIO_CA_MAX) && (pSC->active); ix++, pSC++)
	if (pSC->cAdrs == pBuf)
	    {
	    pSI = pSC->semMId;
	    break;
	    }

    /* channel found ? */

    if (pSI == NULL)
	return (ERROR);

    /* exclusive access */

    if (semTake (pSI, WAIT_FOREVER) == ERROR)
	return (ERROR);

    /* send the reset command */

    errVal = tyCoSendCommand (S_RESET, pBuf, RS_DELAY_V);

    /* give up exclusive access */

    semGive (pSI);

    /* check status */

    if (errVal != ENO_ERROR)
	{
	errno = errVal;
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* tyCoOpen - open file
*
* RETURNS:
*/

LOCAL int tyCoOpen
    (
    TY_CO_DEV * pTyCoDv,
    char *      name,
    int         mode
    )
    {
    SIMIO_CA *         pCA;
    volatile UINT8 *   pBuf;
    volatile UINT8 *   pData;
    UINT8              errVal;
    int                ix;
    int                iy;
    SIMIO_CA_PROTECT * pSC = hp64700ca;
    SEM_ID             pSI = NULL;

    /* for Tx and Rx */

    /* go through loop twice. Once for rxCA and once for txCA */

    iy = 0;
    while (iy < 2)
        {
        /* first rxCA then toggle to txCA */

        pCA = ((iy == 0) ? &pTyCoDv->rxCA : &pTyCoDv->txCA); 

	if (pCA->pName == NULL || pCA->opened)
	    continue;

        /* find channel */

        for (ix = 0; (ix < SIMIO_CA_MAX) && (pSC->active); ix++, pSC++)
	    if (pSC->cAdrs == pCA->pBuf)
		{
	        pSI = pSC->semMId;
		break;
		}

        /* channel found ? */

        if (pSI == NULL)
	    return (ERROR);

        /* exclusive access */

        if (semTake (pSI , WAIT_FOREVER) == ERROR)
	    return (ERROR);

        /* set up control and buffer */

        pBuf = pCA->pBuf + 1;
	pData = pBuf;
        *pData++ = (char) pCA->options;
        *pData++ = 0xff;

        /* copy over name */

        while (*pData++ = *pCA->pName++)
            ;
 
	/* send open command */

        errVal = tyCoSendCommand (S_OPEN, pCA->pBuf, OP_DELAY_V);

	/* give up exclusive access */

        semGive (pSI);

        /* check status */

        if (errVal != ENO_ERROR)
	    {
	    errno = errVal;
	    return (ERROR);
	    }

        pCA->fd = *pBuf;
        pCA->opened = TRUE;
	iy++;
	}

    return ((int) pTyCoDv);
    }

/*******************************************************************************
*
* tyCoRead - read into buffer from SIM I/O
*
* RETURNS:
*/

LOCAL int tyCoRead
    (
    TY_CO_DEV * pTyCoDv,	/* ptr to device structure */
    char *      buffer,		/* buffer to read into     */
    int         maxbytes	/* maximum length of read  */
    )
    {
    UINT8 errVal;
    int                ix;
    volatile UINT8 *   pData;
    SIMIO_CA_PROTECT * pSC = hp64700ca;
    SEM_ID             pSI = NULL;
    volatile UINT8 *   pBuf = pTyCoDv->rxCA.pBuf + 1;
    int                mBytes = maxbytes;
 
    if (!pTyCoDv->rxCA.opened)
	return (0);

    /* find channel */

    for (ix = 0; (ix < SIMIO_CA_MAX) && (pSC->active); ix++)
	{
	if (pSC->cAdrs == pTyCoDv->rxCA.pBuf)
	    {
	    pSI = pSC->semMId;
	    break;
	    }
        pSC++;
	}

    /* channel found ? */

    if (pSI == NULL)
	return (0);

    /* exclusive access */

    if (semTake (pSI, WAIT_FOREVER) == ERROR)
	return (0);

    /* setup fd */

    *pBuf++ = pTyCoDv->rxCA.fd;

    while (maxbytes)
	{
        pData = pBuf;
        *pData++ = min (maxbytes, 0xff);
 
	/* read data */

        errVal = tyCoSendCommand (S_READ, pTyCoDv->rxCA.pBuf, RX_DELAY_V);

        /* check status */

	if (errVal != ENO_ERROR)
    	    {
	    errno = errVal;
            break;
	    }

        for (ix = 0; ix < *pBuf; ix++)
	    {
	    maxbytes--;
            *buffer++ = *pData;
	    if (*pData++ == '\n')
		break;
	    }
        }

    /* give up exclusive access */

    semGive (pSI);
    return (mBytes - maxbytes);
    } 

/*******************************************************************************
*
* tyCoWrite - write buffer out to SIM I/O
*
* RETURNS:
*/

LOCAL int tyCoWrite
    (
    TY_CO_DEV * pTyCoDv,	/* ptr to device structure */
    char *      buffer,		/* buffer of data to write  */
    int         nbytes		/* number of bytes in buffer */
    )
    {
    UINT8              errVal;
    int                wBytes;
    int                ix;
    volatile UINT8 *   pData;
    SIMIO_CA_PROTECT * pSC = hp64700ca;
    SEM_ID             pSI = NULL;
    volatile UINT8 *   pBuf = pTyCoDv->txCA.pBuf + 1;
    int                mBytes = nbytes;
 
    if (!pTyCoDv->txCA.opened)
	return (0);

    /* find channel */

    for (ix = 0; (ix < SIMIO_CA_MAX) && (pSC->active); ix++)
	{
	if (pSC->cAdrs == pTyCoDv->txCA.pBuf)
	    {
	    pSI = pSC->semMId;
	    break;
            }
        pSC++;
	}

    /* channel found ? */

    if (pSI == NULL)
	return (0);

    /* exclusivw access */

    if (semTake (pSI, WAIT_FOREVER) == ERROR)
	return (0);

    /* setup fd */

    *pBuf++ = pTyCoDv->txCA.fd;

    while (nbytes)
	{
        pData = pBuf;

	wBytes = min (nbytes, 0xff);
        *pData++ = wBytes;

        for (ix = 0; ix < wBytes; ix++)
            *pData++ = *buffer++;
 
	/* write data */

        errVal = tyCoSendCommand (S_WRITE, pTyCoDv->txCA.pBuf, TX_DELAY_V);
	    
	/* check status */

	if (errVal != ENO_ERROR)
    	    {
	    errno = errVal;
            break;
	    }

	nbytes -= *pBuf;
	buffer -= (wBytes - *pBuf);
        }

    /* give up exclusive access */

    semGive (pSI);
    return (mBytes - nbytes);
    } 


