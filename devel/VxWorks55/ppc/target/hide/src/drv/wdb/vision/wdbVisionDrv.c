/* wdbVisionDrv.c - Wind River Vision Driver */

/* Copyright 1988-2002 Wind River Systems Inc. */

/*
modification history
--------------------
01e,29nov01,g_h  Cleaning for T2.2
01d,05may01,g_h  rename to wdbVisionDrv.c and cleaning
01c,09apr01,rnr  Changed to generic driver that calls different IO packages
01b,07feb01,g_h  renaming module name and cleaning
01a,07may97,est  Adapted from memdrv.c
*/

/*
DESCRIPTION
This driver provides a Vision Communications Channel
between the I/O system and a host. The system requires an Wind River
background mode emulator equiped with a TGTCONS Transparent
Mode.

USER-CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly:  visionDriverInit() to 
initialize the driver, and visionDriverCreate() to create devices.

Before using the driver, it must be initialized by calling vDriverInit().
This routine should be called only once, before any reads, writes, or 
visionDriverCreate() calls.  

The use and functions provided by the driver vary depending on the 
target microprocessor: ColdFire, PowerPC, ARM, XSCALE, MIPS and SH. 

IOCTL
The memory driver responds to the ioctl() codes

SEE ALSO:
.pG "I/O System"
*/

/* includes */

#include "vxWorks.h"
#include "ioLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "cacheLib.h"
#include "errnoLib.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "taskLib.h"
#include "configAll.h"
#include "drv/wdb/vision/wdbVisionDrvIF.h"
#include "drv/wdb/vision/wdbVisionDrv.h"

/* defines */

#undef VISION_DEBUG_VERSION     /* undef for released version */     

/* externals */

#if defined (INCLUDE_WDB_COMM_VTMD)
IMPORT VDR_ULONG  tmdLowLevelIOInit (V_DRIVER_INTERFACE *pInterfaceData);
#endif /* (INCLUDE_WDB_COMM_VTMD) */

IMPORT VDR_ULONG visionLowLevelIOInit (V_DRIVER_INTERFACE *pInterfaceData);

/* locals */

LOCAL int vDrivNum  = 0; /* driver number          */
LOCAL int vDrivOpen = 0; /* number of open devices */

#if defined (INCLUDE_WDB_COMM_VTMD)
LOCAL VDR_INIT_PTR pIOEntryRoutine = (VDR_INIT_PTR) tmdLowLevelIOInit; /* visionTMD I/O subsystem entry  */
#else 
LOCAL VDR_INIT_PTR pIOEntryRoutine = NULL;
#endif /* (INCLUDE_WDB_COMM_VTMD) */

/* forward declarations */

int visionDriverOpen (VDRIV_DEV *pFd, char *pName, int mode);
int visionDriverRead (VDRIV_DEV *pFd, char *pBuffer ,int maxbytes);
int visionDriverWrite (VDRIV_DEV *pFd, char *pBuffer, int nbytes);
int visionDriverIoctl (VDRIV_DEV *pFd, int function, int arg);
int visionDriverClose (VDRIV_DEV *pFd);

LOCAL void visionDrvPollTask (VDRIV_DEV *pFd);
LOCAL int  visionPoll (VDRIV_DEV *pFd);
LOCAL int  visionStartPollTask (VDRIV_DEV *pFd, int priority);
LOCAL int  visionFillRxQueue (VDRIV_DEV *pFd);

LOCAL int visionInitQueue (volatile VISION_QUEUE *pQueue, int size);
LOCAL int visionBytesInQueue (volatile VISION_QUEUE *pQueue);
LOCAL int visionAddToQueue (volatile VISION_QUEUE *pQueue, char *pDt, int sz);
LOCAL int visionRemoveFromQueue (volatile VISION_QUEUE *pQueue, char *pDt, int sz);
LOCAL int visionFlushQueue (volatile VISION_QUEUE *pQueue);
LOCAL int visionSpaceInQueue (volatile VISION_QUEUE *pQueue);

/***************************************************************************
*
* visionDriverInit - install Wind River Vision Driver
*
* This routine initializes the driver.  It must be called first,
* before any other routine in the driver.
*
* RETURNS: OK, or ERROR if the I/O system cannot install the driver.
*/

int visionDriverInit
    (
    void 
    )
    {

    /* Exit if driver has already been installed */ 

    if (vDrivNum > 0)
        {
        return (OK);
        }

    vDrivNum = iosDrvInstall(visionDriverOpen,
                             (FUNCPTR) NULL,
                             visionDriverOpen,
                             visionDriverClose,
                             visionDriverRead,
                             visionDriverWrite,
                             visionDriverIoctl);

    vDrivOpen = (vDrivNum == ERROR) ? 0 : 1;

    return ((vDrivNum == ERROR) ? ERROR : OK);
    }

/***************************************************************************
*
* visionDriverCreate - create an instance of Wind River Virtual Mode Driver. 
*
* This routine create an instance of Wind River Virtual Mode Driver
*   
* RETURNS: OK, ERROR/errno
*
* NOTE: currently only a single device is supported
*/

int visionDriverCreate
    (  
    char *pName,         
    int   opMode,
    int   blockMode,
    int   bufferMode,
    int   taskDelay,
    int   taskPriority
    )
    {
    VDRIV_DEV *pFd      = (VDRIV_DEV*)NULL;
    VDR_ULONG  ioStatus = VDR_FAILURE;
    
    /* sanity check on taskDelay and taskPriority */

    if ((taskDelay < 0) || (taskDelay > VISION_MAX_PTASK_DELAY))
	{
        return (ERROR);
        }

    if ((taskPriority < VISION_MIN_PTASK_PRIOR) || 
        (taskPriority > VISION_MAX_PTASK_PRIOR))
        {
	return(ERROR);
        }

    /*
     *   If device has not been installed or more than one device 
     *   has been opened, return an ERROR.
     */

    if ((vDrivNum < 1) || (vDrivOpen > 1))
        {
        return(ERROR);
        }

    /* Create the device descriptor */

    if ((pFd = (VDRIV_DEV*)cacheDmaMalloc (sizeof(VDRIV_DEV))) == NULL)
        {
        return (ERROR);
        }

    /* Initialize descriptor values */

    pFd->state       = VISION_STATE_CLOSED; 
    pFd->pollTaskId  = ERROR;
    pFd->pDeviceName = pName;
    pFd->opMode      = opMode;
    pFd->rdWrMode    = O_RDWR;
    pFd->bufferMode  = bufferMode;
    pFd->blockMode   = blockMode;
    pFd->loopMode    = VISION_NORMAL_MODE;
    pFd->pollDelay   = taskDelay;

    pFd->inter.openFunc        = (VDR_OPEN_PTR) NULL;
    pFd->inter.closeFunc       = (VDR_CLOSE_PTR) NULL;
    pFd->inter.readFunc        = (VDR_READ_PTR) NULL;
    pFd->inter.writeFunc       = (VDR_WRITE_PTR) NULL;
    pFd->inter.readStatusFunc  = (VDR_READ_STATUS_PTR) NULL;
    pFd->inter.writeStatusFunc = (VDR_WRITE_STATUS_PTR) NULL;
    pFd->inter.privateData     = (VDR_PDATA) NULL;

    /* Try and initialize the low-level I/O driver. */

    ioStatus = (*(VDR_INIT_PTR)pIOEntryRoutine)(&pFd->inter);

    if (ioStatus == VDR_FAILURE)
        {
        return (ERROR);
        }

    /* Add device to device table */

    if ((iosDevAdd ((DEV_HDR*)pFd ,pName ,vDrivNum)) == ERROR)
        {
        return (ERROR);
        }

    /* Create a LOOPBACK queueu */

    if (visionInitQueue (&pFd->loopQueue ,LOOP_BUFFER_SIZE) == ERROR)
        {
        return (ERROR);
        }

    /* If buffered operation is selected, set up initial buffer queues */

    if (bufferMode == VISION_BUFFERED_MODE)
        {
        if (visionInitQueue (&pFd->txQueue ,TX_BUFFER_SIZE) == ERROR)
            {
            return (ERROR);
            }

        if (visionInitQueue (&pFd->rxQueue ,RX_BUFFER_SIZE) == ERROR)
            {
            return (ERROR);
            }
        }

    /* Create a Tx and Rx Critical Section mutual exclusion construct */

    CREATE_CRITSECT(pFd->txCrSection);
    CREATE_CRITSECT(pFd->rxCrSection);
    CREATE_CRITSECT(pFd->loopCrSection);

    /* 
     *   If the operation mode specified is pseudo-interrupt, start
     *   a background polling task to simulate Tx and Rx interrupts.
     */

    if (opMode == VISION_PINTR_MODE)
        {
        if (visionStartPollTask(pFd ,taskPriority) == ERROR)
            {
            return(ERROR);
            }
        }

    pFd->state = VISION_STATE_OPEN;

    return (OK);
    }

/***************************************************************************
*
* visionDriverOpen - open a connection 
*
* This routine open a connection between the high level driver and the low
* level driver
*
* RETURNS: The file descriptor number, or ERROR if the name is not a valid 
*          number.
*/

int visionDriverOpen 
    (
    VDRIV_DEV *pDev,       /* pointer to device descriptor           */
    char      *pRemainder, /* stuff after /vtd0 not valid            */
    int        mode        /* access mode (O_RDONLY,O_WRONLY,O_RDWR) */
    )           
    {
    volatile VDRIV_DEV *pFd      = pDev; 
    VDR_ULONG           ioStatus = VDR_SUCCESS;
   
    if (pRemainder[0] != 0)
        {
        return (ERROR);
        }

    pFd->rdWrMode = mode;

    /*
     *   Invoke the lower level driver's open function to perform any
     *   intialization or allocation of resources.
     */

    ioStatus = pFd->inter.openFunc (pFd->inter.privateData);
  
    if (ioStatus != VDR_SUCCESS)
        {
        return (ERROR);
        }

    return ((int)pFd);
    }

/***************************************************************************
*
* visionDriverClose - close a connection 
*
* This routine close the connection between the high level driver and the low
* level driver

* RETURNS: OK, or ERROR 
*/

int visionDriverClose 
    (
    VDRIV_DEV *pDev /* pointer to device descriptor */
    )           
    {
    volatile VDRIV_DEV *pFd = pDev; 
    VDR_ULONG           ioStatus;

    /*
     *   Invoke the lower level driver's close function to cleanup and 
     *   resources they may have allocated.
     */

    ioStatus = pFd->inter.closeFunc (pFd->inter.privateData);
  
    if (ioStatus == VDR_SUCCESS)
        {
        return (OK);
        }
    else
        {
        return(ERROR);
        }
    }

/***************************************************************************
*
* visionDriverRead - read from a memory file
*
* This routine read data from the low level driver.
*
* RETURNS: The number of bytes read, or ERROR if past the end of memory or 
*          is O_WRONLY only.
*/

int visionDriverRead 
    (
    VDRIV_DEV *pDev,    /* file descriptor of file to close */
    char      *pBuffer, /* buffer to receive data           */
    int        maxbytes /* max bytes to read in to buffer   */
    )
    {
    volatile VDRIV_DEV *pFd = pDev;
    int                 bytesRead = 0;
    VDR_ULONG           ioStatus;
    VDR_ULONG           readStatus;
    VDR_ULONG           recvBytes;

    /* Insure proper rwMode */

    if (pFd->rdWrMode == O_WRONLY)
        {
        return (ERROR);
        }

    /* 
     *   Sanity check on maxbytes, zero is a legal value, 
     *   it is used to indicate a status poll is requested.
     */

    if (maxbytes < 0)
        {
        return (ERROR);
        }

    /* 
     *    Polled verses Pseudo-Interrupt, Blocking verses Non-Blocking,
     *    and Buffered verses Non-Buffered are handled slightly different. 
     *    (see Application Note, Mode Matrix for details)
     */

    if (pFd->opMode == VISION_POLL_MODE)
        {
        if (pFd->blockMode == VISION_BLOCKED_MODE)
            {

            /*
             *   (This mode has to be VISION_NONBUFFERED_MODE)
             *
             *   Wait for available data from emulator 
             */

            readStatus = VDR_DATA_NONE;

            while (readStatus == VDR_DATA_NONE)
                {
                ioStatus = pFd->inter.readStatusFunc (pFd->inter.privateData,
                                                      &readStatus);

                if (ioStatus == VDR_FAILURE)
                    {
                    return (ERROR);
                    }
                }

            ioStatus = pFd->inter.readFunc(pFd->inter.privateData,
                                           (VDR_UCHAR*)pBuffer,
                                           (VDR_ULONG)maxbytes,
                                           &recvBytes);

            if (ioStatus == VDR_FAILURE)
                {
                return (ERROR);
                }

            bytesRead = recvBytes;
            }
        else /* POLL_MODE & NONBLOCKED_MODE */
            {
            if (pFd->bufferMode == VISION_NONBUFFERED_MODE)
                {

                ioStatus = pFd->inter.readFunc (pFd->inter.privateData,
                                                (VDR_UCHAR*) pBuffer,
                                                (VDR_ULONG) maxbytes,
                                                &recvBytes);

                if (ioStatus == VDR_FAILURE)
                    {
                    return (ERROR);
                    }

                bytesRead = recvBytes;
                }
            else /* POLL_MODE, NONBLOCKED_MODE, BUFFERED_MODE */
                {

                /* 
                 *   This is a possible mode, but not a practical mode. The 
                 *   buffered mode was intended to be used with a pTask running.
                 */

                return (ERROR);
                }
            }
        }
    else if (pFd->opMode == VISION_PINTR_MODE)
        {
        if (pFd->blockMode == VISION_BLOCKED_MODE)
            {
            /* (This mode has to be VISION_BUFFERED_MODE) */

            /* 
             * In this mode, the visionDriverRead() function will block until
             * data is available. The background pTask is responsible
             * for monitoring the Rx descriptor buffer and filling the
             * Rx Queue.
             */

            /* Wait for data to be available in the Rx queue */

            do  {
                ENTER_CRITSECT (pFd->rxCrSection);

                bytesRead = visionRemoveFromQueue (&pFd->rxQueue ,pBuffer ,maxbytes);

                EXIT_CRITSECT (pFd->rxCrSection);

                if (bytesRead == 0)
                    {
                    DELAY_TASK (pFd->pollDelay);
                    }

                } while(bytesRead == 0);

            return(bytesRead);
            }
        else /* PINTR_MODE, NONBLOCKED_MODE, BUFFERED_MODE */
            {
            /* (This mode has to be VISION_BUFFERED_MODE) */

            ENTER_CRITSECT (pFd->rxCrSection);

            bytesRead = visionRemoveFromQueue (&pFd->rxQueue ,pBuffer ,maxbytes);

            EXIT_CRITSECT (pFd->rxCrSection);

            return (bytesRead);
            }
        }       
    else if (pFd->opMode == VISION_PISR_MODE)
        {
        return (ERROR);
        }
    else
        {
        return(ERROR);
        }

    return (bytesRead);
    }

/***************************************************************************
*
* visionDriverWrite - write to vision driver
*
* This routine write to the low level driver.
*
* RETURNS: The number of bytes written, or ERROR/errno 
*
* NOTE: some redundancy in various mode sections.
*/

int visionDriverWrite 
    (
    VDRIV_DEV *pDev,    /* file descriptor of file to close     */
    char      *pBuffer, /* buffer to be written                 */
    int        nbytes   /* number of bytes to write from buffer */
    )
    {
    volatile VDRIV_DEV *pFd = pDev;
    VDR_ULONG           nToWrite;
    int                 bytesAdded;
    VDR_ULONG           ioStatus;
    VDR_ULONG           writeStatus;
    VDR_ULONG           curLoc;

    /* Insure driver has been previously opened */

    if (pFd->state != VISION_STATE_OPEN)
        {
        return (ERROR);
        }

    /* Insure proper read/write mode */

    if (pFd->rdWrMode == O_RDONLY)
        {
        return (ERROR);
        }

    /* Normal/Loopback Mode ? */

    if (pFd->loopMode == VISION_LPBK_MODE)
        {
        ENTER_CRITSECT (pFd->loopCrSection);

        bytesAdded = visionAddToQueue(&pFd->loopQueue ,pBuffer ,nbytes); 

        EXIT_CRITSECT (pFd->loopCrSection);

        if (bytesAdded != nbytes) 
            {
            return (ERROR); 
            }
        }

    /* 
     * Polled verses Pseudo-Interrupt, Blocking verses Non-Blocking,
     * and Buffered verses Non-Buffered are handled slightly different. 
     * (see Application Note, Mode Matrix for details). 
     */

    if (pFd->opMode == VISION_POLL_MODE)
        {
        if (pFd->blockMode == VISION_BLOCKED_MODE)
            {

            /* This mode has to be VISION_NONBUFFERED_MODE */

            nToWrite = nbytes;
            curLoc   = 0;

            while (nToWrite > 0)
                {

                /* 
                 *   If emulator is not busy, place buffer in tx descriptor
                 *   otherwise wait for emulator to come ready.
                 */
  
                ioStatus = pFd->inter.writeStatusFunc (pFd->inter.privateData,
                                                       &writeStatus);

                if (ioStatus == VDR_SUCCESS)
                    {
                    if (writeStatus == VDR_WRITE_COMPLETE)
                        {
                        if (nToWrite < VISION_PKT_MTU) 
                            {
                            ioStatus = pFd->inter.writeFunc (pFd->inter.privateData,
                                                             pBuffer,
                                                             nToWrite);
    
                            if (ioStatus == VDR_FAILURE)
                                {
                                return (ERROR);
                                }
        
                            nToWrite = 0;
                            }
                        else
                            {
                            ioStatus = pFd->inter.writeFunc (pFd->inter.privateData,
                                                             &pBuffer[curLoc],
                                                             nToWrite);
    
                            if (ioStatus == VDR_FAILURE)
                                {
                                return (ERROR);
                                }
    
                            nToWrite -= VISION_PKT_MTU;
                            curLoc   += VISION_PKT_MTU;
                            }
                        }
                    else /* Emulator busy reading buffer */
                        {
                        DELAY_TASK (pFd->pollDelay);
                        }
                    }
                else
                    {
                    return (ERROR);
                    }
		}
       
            /* Before returning, insure emulator has retrieved the buffer */

            writeStatus = VDR_WRITE_PENDING;

            while (writeStatus == VDR_WRITE_PENDING)
                {
                DELAY_TASK (pFd->pollDelay);

                ioStatus = pFd->inter.writeStatusFunc (pFd->inter.privateData,
                                                       &writeStatus);

                if (ioStatus == VDR_FAILURE)
                    {
                    return (ERROR);
                    }
                }
   
            return (nbytes);
            }
        else  /* VISION_POLL_MODE & VISION_NONBLOCKED_MODE */
            {
            if (pFd->bufferMode == VISION_NONBUFFERED_MODE)
                {
                /* NonBlocking Poll Mode cannot handle > MTU blocks */

                if (nbytes > VISION_PKT_MTU)
                    {
                    return (ERROR);
                    }

                /* 
                 *   Request that the lowlevel I/O subsystem transmit the
                 *   data across the communications media.
                 */
  
                ioStatus = pFd->inter.writeStatusFunc (pFd->inter.privateData,
                                                       &writeStatus);

                if (ioStatus == VDR_SUCCESS)
                    {
                    if (writeStatus == VDR_WRITE_COMPLETE)
                        {
                        ioStatus = pFd->inter.writeFunc (pFd->inter.privateData,
                                                         pBuffer,
                                                         nbytes);
    
                        if (ioStatus == VDR_FAILURE)
                            {
                            return (ERROR);
                            }
      
                        return (nbytes);
                        }
                    else /* Emulator busy reading buffer */
                        {
                        return(ERROR);
                        }
                    }
                else
                    {
                    return (ERROR);
                    }
                }
            else /* POLL_MODE, NONBLOCKING_MODE, BUFFERED  */
                {

                /* 
                 *   This is a possible mode, but not a practical mode. The 
                 *   buffered mode was intended to be used with a pTask running.
                 */

                return (ERROR);
                }
            }
        }
    else if (pFd->opMode == VISION_PINTR_MODE) 
        {
        if (pFd->blockMode == VISION_BLOCKED_MODE)
            {

            /*
             *  Since this mode is Blocking, there is no reason to 
             *  buffer the transmit packet -- it can be sent out
             *  directly in MTU size chunks (identical to POLL,
             *  BLOCKING,NONBUFFERED). In this mode, the term BUFFERED
             *  refers to the receiver.
             *
             *  Note: the pTask will still monitor the tx queue, but 
             *        since this write routine never writes anything
             *        to it, it is affectively disabled.
             */

            nToWrite = nbytes;
            curLoc   = 0;

            while (nToWrite > 0)
                {

                /* 
                 *   If emulator is not busy, place buffer in tx descriptor
                 *   otherwise wait for emulator to come ready.
                 */
  
                ioStatus = pFd->inter.writeStatusFunc (pFd->inter.privateData,
                                                       &writeStatus);

                if (ioStatus == VDR_SUCCESS)
                    {
                    if (writeStatus == VDR_WRITE_COMPLETE)
                        {
                        if (nToWrite < VISION_PKT_MTU) 
                            {
                            ioStatus = pFd->inter.writeFunc (pFd->inter.privateData,
                                                             pBuffer,
                                                             nToWrite);
    
                            if (ioStatus == VDR_FAILURE)
                                {
                                return (ERROR);
                                }
        
                            nToWrite = 0;
                            }
                        else
                            {
                            ioStatus = pFd->inter.writeFunc (pFd->inter.privateData,
                                                             &pBuffer[curLoc],
                                                             nToWrite);
    
                            if (ioStatus == VDR_FAILURE)
                                {
                                return (ERROR);
                                }
    
                            nToWrite -= VISION_PKT_MTU;
                            curLoc   += VISION_PKT_MTU;
                            }
                        }
                    else  /* Emulator busy reading buffer */
                        {
                        DELAY_TASK (pFd->pollDelay);
                        }
                    }
                else
                    {
                    return (ERROR);
                    }
		}
       
            /* Before returning, insure emulator has retrieved the buffer */

            writeStatus = VDR_WRITE_PENDING;

            while (writeStatus == VDR_WRITE_PENDING)
                {
                DELAY_TASK (pFd->pollDelay);

                ioStatus = pFd->inter.writeStatusFunc (pFd->inter.privateData,
                                                       &writeStatus);

                if (ioStatus == VDR_FAILURE)
                    {
                    return (ERROR);
                    }
                }

            return (nbytes);
            }
        else  /* VISION_PINTR_MODE & VISION_NONBLOCKED_MODE */
            {
            /* (This mode has to be VISION_BUFFERED_MODE) */

            /*
             *   Queue up transmit buffer for background pTask to 
             *   send it -- don't block.
             */

            ENTER_CRITSECT (pFd->txCrSection);
            bytesAdded = visionAddToQueue (&pFd->txQueue , pBuffer, nbytes);
            EXIT_CRITSECT (pFd->txCrSection);

            return (bytesAdded);
            }
        }
    else if (pFd->opMode == VISION_PISR_MODE)
        {
        return (ERROR);
        }
    else   
        {
        return (ERROR);
        }

    return (nbytes);
    }

/***************************************************************************
*
* visionDriverIoctl - do device specific control function
*
* This routine do device specific control functions as listed below:
*
*   VISION_POLL_MODE        - switch to Polling Mode
*   VISION_PINTR_MODE       - switch to Pseudo Interrupt Mode
*   VISION_BLOCKED_MODE     - switch to blocking mode
*   VISION_NONBLOCKED_MODE  - switch to non-blocking mode
*   VISION_BUFFERED_MODE    - switch to buffered mode
*   VISION_NONBUFFERED_MODE - switch to non-buffered mode
*   VISION_RX_QUEUE_SIZE    - Set the size of the RX queue.
*                             This will remove any characters in the RX queue.
*   VISION_TX_QUEUE_SIZE    - Set the size of the TX queue.
*                             This will remove any characters in the TX queue.
*
* RETURNS: OK, or ERROR if seeking passed the end of memory.
*/

int visionDriverIoctl
    (
    VDRIV_DEV *pDev,     /* descriptor to control */
    int        function, /* function code         */
    int        arg       /* some argument         */
    )
    {
    int                  status = OK;
    volatile VDRIV_DEV *pFd    = pDev;

    switch (function)
        {
        case VISION_PINTR_MODE:
        case VISION_PISR_MODE:
        case VISION_BLOCKED_MODE:
        case VISION_NONBLOCKED_MODE:
        case VISION_BUFFERED_MODE:
        case VISION_NONBUFFERED_MODE:
            {
 
            /*
             *   For now, do not allow dynamic mode switching 
             *   of these individual items.
             */
  
            status = ERROR;
            break;                                           
            }
 
        case VISION_POLL_MODE | VISION_NONBLOCKED_MODE | VISION_NONBUFFERED_MODE:
            {
 
            /*
             *   This is a special major mode switch. This is used when the TM 
             *   Driver is configured as a WDB Agent Driver. This mode switch is 
             *   called when specifying 'System-Level' debug mode. Since the 
             *   user will probably reissue a 'Task-Level' debug mode' command, 
             *   don't back completely out of the existing settings -- just 
             *   tidy up and make safe (i.e. don't free malloc'ed memory, etc).
             */
    
            /* Suspend tTmdDrv polling task */
     
            if (taskSuspend (pFd->pollTaskId) == ERROR)
                {
                return (ERROR);
                }

            /* flush the Tx/Rx buffer queues */
 
            ENTER_CRITSECT   ( pFd->txCrSection);
            visionFlushQueue (&pFd->txQueue);
            EXIT_CRITSECT    ( pFd->txCrSection);
 
            ENTER_CRITSECT   ( pFd->rxCrSection);
            visionFlushQueue (&pFd->rxQueue);
            EXIT_CRITSECT    ( pFd->rxCrSection);
 
            ENTER_CRITSECT   ( pFd->loopCrSection);
            visionFlushQueue (&pFd->loopQueue);
            EXIT_CRITSECT    ( pFd->loopCrSection);
 
            /* Specify polling, non-buffered, non-blocking mode */
 
            pFd->opMode     = VISION_POLL_MODE;
            pFd->blockMode  = VISION_NONBLOCKED_MODE;
            pFd->bufferMode = VISION_NONBUFFERED_MODE;

            break;                                           
            }

        case VISION_PINTR_MODE | VISION_BLOCKED_MODE | VISION_BUFFERED_MODE:
            {
            /*
             * This is a special major mode switch, it is used when the 
             * vision Driver is configured  as a WDB Agent Driver. This 
             * mode switch  is called when  specifying 'Task-Level 
             * Debugging Mode'.  Since the user may reissue a 
             * 'System-Level Debug Mode' cmnd,  don't back completely  
             * out of  the existing settings -- just tidy up and make 
             * safe  (i.e. don't free malloc'ed memory, remove tasks, etc).
             */
 
            /* Restart the tVisionDrv polling task */

            if ( taskRestart (pFd->pollTaskId) == ERROR)
                {
                return (ERROR);
                }

            /* Specify polling, non-buffered, non-blocking mode */

            pFd->opMode     = VISION_PINTR_MODE;
            pFd->blockMode  = VISION_BLOCKED_MODE;
            pFd->bufferMode = VISION_BUFFERED_MODE;
 
            break;                                           
            }

        case VISION_PTASK_PRIORITY:
	    {
            if ((arg < VISION_MIN_PTASK_PRIOR) || (arg > VISION_MAX_PTASK_PRIOR))
                {
                status = ERROR;
                }
            else
                {
                status = taskPrioritySet (pFd->pollTaskId , arg);
                }

            break;
            }

        case VISION_PTASK_DELAY: 
            {
            if ((arg < 0) || (arg > VISION_MAX_PTASK_DELAY))
                {
                status = ERROR;
                }
            else
                {
                pFd->pollDelay = arg;
                }

            break;
            }

        case VISION_LPBK_MODE:
            {
            pFd->loopMode = VISION_LPBK_MODE;
            break;
            }

        case VISION_NORMAL_MODE:
            {
            pFd->loopMode = VISION_NORMAL_MODE;
            break;
            }

        case VISION_RX_QUEUE_SIZE:
            {
            free (pFd->rxQueue.pData);

            if (visionInitQueue (&pFd->rxQueue, arg) == ERROR)
                {
                status = ERROR;
                }

            break;
            }

        case VISION_TX_QUEUE_SIZE:
            {
            free (pFd->txQueue.pData);

            if (visionInitQueue (&pFd->txQueue, arg) == ERROR)
                {
                status = ERROR;
                }

            break;
            }

        default: /* Unknown Function */
            {
            status = ERROR;
            break;
            }
        }

    return (status);
    }

/***************************************************************************
*
* visionFillRxQueue - read from a memory file into receive queue
*
* This routine read from a memory file intp recive queue.
*
* RETURNS: The number of bytes read, or ERORR
*/

LOCAL int visionFillRxQueue
    (
    VDRIV_DEV * pDev /* file descriptor of file to close */
    )
    {
    volatile VDRIV_DEV *pFd   = pDev;
    int                 nRead = 0;
    int                 numBytes;
    char                data;
    VDR_ULONG           ioStatus;
    VDR_ULONG           recvBytes = 0;

    static VDR_UCHAR    buffer[VISION_PKT_MTU];

    /* 
     *   If we are looping back data then extract the data from the 
     *   loopback QUEUE and place it in the Receive QUEUE.
     */

    if (pFd->loopMode == VISION_LPBK_MODE)
        {
        ENTER_CRITSECT (pFd->loopCrSection);

        numBytes = visionBytesInQueue (&pFd->loopQueue); 

        EXIT_CRITSECT (pFd->loopCrSection);

        if (numBytes != 0)
            {
            for ( ;; )
                {
                ENTER_CRITSECT (pFd->loopCrSection);
                numBytes = visionRemoveFromQueue (&pFd->loopQueue, &data, 1);
                EXIT_CRITSECT (pFd->loopCrSection);

                if (numBytes == 0)
                    {
                    break;
                    }
                else
                    { 
                    ENTER_CRITSECT (pFd->rxCrSection);
                    numBytes = visionAddToQueue (&pFd->rxQueue, &data, 1);
                    EXIT_CRITSECT (pFd->rxCrSection);

                    if (numBytes == 1)
                        {
                        nRead ++;
                        }
                    else
                        {
                        break ; /* RNR - need to deal with this better */
                        }
		    }
		}
	    }
	}

    /*
     *   Now that we have dealt with the loopback information lets see
     *   see if there is any data waiting for us.
     */

    ioStatus = pFd->inter.readFunc (pFd->inter.privateData,
                                    (VDR_UCHAR*)buffer,
                                    (VDR_ULONG)VISION_PKT_MTU,
                                    &recvBytes);

    if ((ioStatus == VDR_SUCCESS) && (recvBytes != 0))
        {
        ENTER_CRITSECT (pFd->rxCrSection);
        numBytes = visionAddToQueue (&pFd->rxQueue, buffer, recvBytes);
        EXIT_CRITSECT (pFd->rxCrSection);

        if (recvBytes != numBytes)
            {
            return (0); /* RNR - needs thinking about */
            }

        nRead += numBytes;
        }

    return (nRead);
    }

/***************************************************************************
*
* visionStartPollTask - spawns a polling task to simulate Tx and Rx interrupts
*
* This routine spawns a polling task to simulate Tx and Rx interrupts
*
* RETURNS: OK or ERROR
*/ 

LOCAL int visionStartPollTask
    (
    VDRIV_DEV *pVisionDrv,
    int        taskPriority
    )
    {
    int taskId;

    if (taskIdCurrent) /* insure we're multitasking */
        {

        /*
         *   See if the task is already known. In which ase we only
         *   need to start it up again.
         */

        taskId = taskNameToId ("tVisionDrv");    

        if (taskId == ERROR)
            { 
            pVisionDrv->pollTaskId = taskSpawn ("tVisionDrv",                /* name                  */
                                                taskPriority,                /* runtime priority      */
                                                0,                           /* options               */
                                                5000,                        /* Stack size            */
                                                (int(*)())visionDrvPollTask, /* Entry point of task   */
                                                (int)pVisionDrv,             /* Arg #01 , device info */
                                                0,                           /* Arg #02 , not used    */
                                                0,                           /* Arg #03 , not used    */
                                                0,                           /* Arg #04 , not used    */
                                                0,                           /* Arg #05 , not used    */
                                                0,                           /* Arg #06 , not used    */
                                                0,                           /* Arg #07 , not used    */
                                                0,                           /* Arg #08 , not used    */
                                                0,                           /* Arg #09 , not used    */
                                                0);                          /* Arg #10 , not used    */

            if (pVisionDrv->pollTaskId == ERROR)
                {
                return (ERROR);
                }
            }
        else
            {
            if (taskRestart (taskId) == ERROR)
                {
                return (ERROR);
                }
            }
        }
    else  /* need to be multitasking */
        {
        return (ERROR);
        }

    return (OK);
    }

/***************************************************************************
*
* visionDrvPollTask - background polling task to simulate Rx and Tx interrupts.
*
* This routine is the background polling task to simulate Rx and Tx 
* interrups.
*
* RETURNS: N/A
*/ 

LOCAL void visionDrvPollTask
    (
    VDRIV_DEV *pDev
    )
    {
    volatile VDRIV_DEV *pFd = (VDRIV_DEV*)pDev;

    for ( ;; )
        {
        visionPoll (pDev);
        DELAY_TASK (pFd->pollDelay);
        }
    }

/***************************************************************************
*
* visionPoll - poll and service Tx/Rx emulator descriptor buffers and Tx/Rx
*              queues.
*
* This routine poll and service Tx/Rx emulator descriptor buffers and Tx/Rx 
* queues.
*
* RETURNS: OK or ERROR (return status not used as this point)
*/ 

LOCAL int visionPoll
    (
    VDRIV_DEV *pDev
    )
    {
    volatile VDRIV_DEV *pFd = (VDRIV_DEV*)pDev;
    int                 bytesInQueue;
    int                 bytesToWrite;
    static VDR_UCHAR    localXmitBuf[VISION_PKT_MTU];
           VDR_ULONG    ioStatus;
           VDR_ULONG    writeStatus;
    
    /* Handle any Rx data present in the Rx emulator descriptor */

    ENTER_CRITSECT (pFd->rxCrSection);
    visionFillRxQueue ((VDRIV_DEV*)pFd);
    EXIT_CRITSECT (pFd->rxCrSection);

    /* Send any Tx data present in the Tx queue */

    ENTER_CRITSECT (pFd->txCrSection);

    bytesInQueue = visionBytesInQueue (&pFd->txQueue); 

    if (bytesInQueue != 0)
        {
        bytesToWrite = visionRemoveFromQueue (&pFd->txQueue,
                                              (char*)localXmitBuf,
                                              VISION_PKT_MTU);

        EXIT_CRITSECT (pFd->txCrSection);

        /* 
         * bytesToWrite should be equal to bytesInQueue or queue
         * locking mechanism is not working.
         */

        if (bytesToWrite != bytesInQueue)
            {
            }

        while (bytesToWrite > 0)
            {

            /* 
             *   If emulator is not busy, generate a transmission and
             *   let the emulator pick up the data.
             */
  
            ioStatus = pFd->inter.writeStatusFunc (pFd->inter.privateData,
                                                   &writeStatus);

            if ((ioStatus == VDR_SUCCESS) && (writeStatus == VDR_WRITE_COMPLETE))
                {
                ioStatus = pFd->inter.writeFunc (pFd->inter.privateData,
                                                 localXmitBuf,
                                                 bytesToWrite);

                if (ioStatus == VDR_FAILURE)
                    {
                    return (ERROR);
                    }

                bytesToWrite = 0;
                }
            else  /* Emulator busy reading buffer */
                {
                DELAY_TASK (pFd->pollDelay);
                }
            }
        }
    else
        {
        EXIT_CRITSECT (pFd->txCrSection);
        }

    return (OK);
    }

/***************************************************************************
*
* visionInitQueue - create a VISION_QUEUE.
*
* This routine create a VISION_QUEUE.
*
* RETURNS: OK if queue created or ERROR if faild to create queue
*/ 

LOCAL int visionInitQueue
    (
    volatile VISION_QUEUE *pQueue,
    int                    qSize
    )
    {

    /* Setup the various fields to indicate an empty QUEUE */

    pQueue->nofChars = 0;
    pQueue->size     = qSize;
    pQueue->head     = 0;
    pQueue->tail     = 0;

    /* Actually allocate the QUEUE buffer space */

    if ((pQueue->pData = cacheDmaMalloc (qSize)) == NULL)
        {
        return (ERROR);
        }

    return (OK);
    }

/***************************************************************************
*
* visionAddToQueue - adds a string of characetrs to an VISION_QUEUE.
*
* This routine adds a string of characetrs to an VISION_QUEUE.
*
* RETURNS: number of characters placed into the buffer.
*/ 

LOCAL int visionAddToQueue
    (
    volatile VISION_QUEUE *pQueue, /* Queue to be manipulated */
    char                  *pStr,   /* Data to be inserted     */
    int                    size    /* Amount of data supplied */
    )
    {
    int i;

    /* See of there is any room left in the QUEUE */

    if (pQueue->nofChars >= pQueue->size)
        {
        return (0);
        }

    /*
     *   If the entire string won't fit in buffer then only place the
     *   data that will fit into buffer.
     */

    if (size > (pQueue->size - pQueue->nofChars))
        {
        size = pQueue->size - pQueue->nofChars;
        }

    for (i = 0 ;i < size ; i ++)
        {
        pQueue->pData[pQueue->head++] = *pStr++;
        
        /* Wrap-around the queue's data */

        pQueue->head %= pQueue->size;
        }

    pQueue->nofChars += size;
    
    return (size);
    }

/***************************************************************************
*
* visionRemoveFromQueue - gets a string from an VISION_QUEUE
*
* This routine gets a string from an VISION_QUEUE.
*
* RETURNS: number of characters read from buffer.
*/ 

LOCAL int visionRemoveFromQueue
    (
    volatile VISION_QUEUE *pQueue, /* Queue to be extracted from */
    char                  *str,    /* Retrieved data area        */
    int                    size    /* Amount of data requested   */
    )
    {
    int i;

    /* 
     *   If there is not enough characters in buffer to fill it then only
     *   put in what we have. 
     */

    if (size > pQueue->nofChars)
        {
        size = pQueue->nofChars;
        }

    if (size == 0) /* sanity check */
        {
        return (0);
        }

    /* Extract the requested data from the QUEUE */

    for (i = 0; i < size; i ++)
        {
        *str++ = pQueue->pData[pQueue->tail++];
        
        /* Wrap-around the buffer's data */

        pQueue->tail %= pQueue->size;
        }

    /* Decrement the remaining count by what as extracted */

    pQueue->nofChars -= size;

    return (size);
    }

/***************************************************************************
*
* visionSpaceInQueue - returns space available in queue
*
* This routine returns space available in queue
*
* RETURNS: number of characters available in queue buffer.
*/ 

LOCAL int visionSpaceInQueue
    (
    volatile VISION_QUEUE *pQueue
    )
    {
    return (pQueue->size - pQueue->nofChars); 
    } 

/***************************************************************************
*
* visionBytesInQueue - returns number of bytes in queue
*
* This routine returns number of bytes in queue
*
* RETURNS: number of bytes in the queue
*/ 

LOCAL int visionBytesInQueue
    (
    volatile VISION_QUEUE *pQueue
    )
    {
    return (pQueue->nofChars);
    }

/***************************************************************************
*
* visionFlushQueue - resets queue to empty
*
* This routine resets queue to empty
*
* RETURN: number of characters flushed from queue.
*/ 

LOCAL int visionFlushQueue
    (
    volatile VISION_QUEUE *pQueue
    )
    {
    int charsFlushed;

    charsFlushed     = pQueue->nofChars;
    pQueue->nofChars = 0;
    pQueue->head     = pQueue->tail = 0;
    
    return (charsFlushed);
    }
