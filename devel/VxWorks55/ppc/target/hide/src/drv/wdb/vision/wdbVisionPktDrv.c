/* wdbVisionPktDrv.c - packet driver for lightweight UDP/IP */

/* Copyright 1988-2002 Wind River Systems Inc. */

/*
modification history
--------------------
01d,29nov01,g_h  Cleaning for T2.2
01c,07feb01,g_h  renaming module name and cleaning
01b,03may97,est  adapted for EST Background mode emulator
01a,23aug95,ms   written.
*/

/*
DESCRIPTION

This module is a Wind River Vision driver for interfacing with the 
WDB agent's lightweight UDP/IP interpreter. It was derived from the
wdbTemplatePktDrv.c template.

PACKET READY CALLBACK
---------------------
When the driver detects that a packet has arrived (either in its receiver
ISR or in its poll input routine), it invokes a callback to pass the
data to the debug agent. The driver's wdbVisionPktDevInit() routine 
passes the callback as parameter and places it in the device data structure. 

MODES
-----
Wind River Vision Driver support Poll Mode/Pseudo-Interrupt Mode only. This is done
by spawning a separate polling task to monitor the receive channel.

USAGE
-----
The driver is typically only called only from usrWdb.c. The only directly
callable routine in this module is wdbVisionPktDevInit().

DATA BUFFERING
--------------
The drivers only need to handle one input packet at a time because
the WDB protocol only supports one outstanding host-request at a time.

For output, the agent will pass the driver a chain of mbufs, which
the driver must send as a packet. When it is done with the mbufs,
it calls wdbMbufChainFree() to free them.
*/

#include "vxWorks.h"
#include "string.h"
#include "errno.h"
#include "sioLib.h"
#include "intLib.h"
#include "stdio.h"
#include "ioLib.h"
#include "iosLib.h"
#include "memLib.h"
#include "errnoLib.h"
#include "taskLib.h"        
#include "netinet/in_systm.h"
#include "netinet/in.h"
#include "netinet/ip.h"
#include "netinet/udp.h"
#include "intLib.h"
#include "cacheLib.h"
#include "wdb/wdbMbufLib.h"

#include "drv/wdb/vision/wdbVisionPktDrv.h"
#include "drv/wdb/vision/wdbVisionDrvIF.h"
#include "drv/wdb/vision/wdbVisionDrv.h"
                               
                                
/* pseudo-macros */

#define DRIVER_RESET_INPUT(pDev) {pDev->inputBusy=FALSE; pDev->bytesRead=0;}
#define DRIVER_RESET_OUTPUT(pDev){pDev->outputBusy=FALSE; pDev->bytesWritten=0;}

/* forward declarations */

LOCAL int  wdbVisionPoll    (void *pDev);
LOCAL int  wdbVisionPktTx   (void *pDev, struct mbuf *pMbuf);
LOCAL int  wdbVisionModeSet (void *pDev, uint_t newMode);
LOCAL void wdbVisionFree    (void *pDev);

/* externals */

int wdbVisionPollTask (void*, int);

/***************************************************************************
*
* wdbVisionPktDevInit - initialize the Vision packet driver.
*
* This routine initialize the vision packet driver.
*
* RETURNS: OK or ERROR
*/

int wdbVisionPktDevInit
    (
    WDB_VISION_PKT_DEV *pPktDev,      /* Vision device structure */
    void                (*stackRcv)() /* receive packet callback */
    )
    {

    /* initialize the wdbDrvIf field with driver info */

    pPktDev->wdbDrvIf.mode      = WDB_COMM_MODE_POLL | WDB_COMM_MODE_INT;
    pPktDev->wdbDrvIf.mtu       = WDB_VISION_PKT_MTU;
    pPktDev->wdbDrvIf.stackRcv  = stackRcv;      
    pPktDev->wdbDrvIf.devId     = (WDB_VISION_PKT_DEV*)pPktDev;
    pPktDev->wdbDrvIf.pollRtn   = wdbVisionPoll;
    pPktDev->wdbDrvIf.pktTxRtn  = wdbVisionPktTx;
    pPktDev->wdbDrvIf.modeSetRtn= wdbVisionModeSet;

    /* Reset driver input/output status indicators */

    DRIVER_RESET_INPUT  (pPktDev);
    DRIVER_RESET_OUTPUT (pPktDev);

    /* Install Wind River Vision I/O Driver */

    if (visionDriverInit () == ERROR)
        return (ERROR);                 

    /*
     * The Vision Driver can support a Task-Level Only or combination of 
     * Task-Level/System-Level. 
     */

#ifdef VISION_SPECIAL_TASKMODE_ONLY          

    if ((visionDriverCreate ("/vision/0", 
                             VISION_PINTR_MODE,    
                             VISION_BLOCKED_MODE, 
                             VISION_BUFFERED_MODE, 
                             VISION_DEFAULT_POLL_DELAY,
                             VISION_DEFAULT_POLL_PRI)) == ERROR)
        return (ERROR);

#else
                                          /* ||     OR      || */
    if ((visionDriverCreate ("/vision/0", /* vv   BLOCKED   vv */
                             VISION_POLL_MODE,        
                             VISION_NONBLOCKED_MODE, 
                             VISION_NONBUFFERED_MODE, 
                             VISION_DEFAULT_POLL_DELAY,
                             VISION_DEFAULT_POLL_PRI)) == ERROR)
        return (ERROR);
#endif

    pPktDev->fileFd = open ("/vision/0", O_RDWR, 0644);

    if (pPktDev->fileFd == ERROR)
        { 
        return (ERROR);
        }

    /* Allocate storage for Tx and Rx Buffers */

    pPktDev->pTxBuffer = cacheDmaMalloc (WDB_VISION_PKT_MTU);

    if (pPktDev->pTxBuffer == NULL)
        {
        return (ERROR);
        }

    pPktDev->pRxBuffer = cacheDmaMalloc (WDB_VISION_PKT_MTU + UDP_IP_HDR_SIZE);

    if (pPktDev->pRxBuffer == NULL)
        {
        free (pPktDev->pTxBuffer);

        return (ERROR);
        }

    /* 
     *  Create a receive polling task to simulate Rx interrupts. 
     */

    if (taskIdCurrent)                /* insure we're multitasking */
        {
        pPktDev->pollTaskId = taskSpawn ("tVisionWdb", 
                                         VISION_DEFAULT_POLL_PRI,0,5000,
                                         (int(*)())wdbVisionPollTask,
                                         (int)pPktDev, 
                                         VISION_DEFAULT_POLL_DELAY,
                                         0,0,0,0,0,0,0,0);

        if (pPktDev->pollTaskId == ERROR)
            {
            free (pPktDev->pTxBuffer);
            free (pPktDev->pRxBuffer);
     
            return (ERROR);
            }
        }
    else  /* need to be multitasking */
        {
        free (pPktDev->pTxBuffer);
        free (pPktDev->pRxBuffer);

        return (ERROR);
        }

    /* Standard operating mode is Pseudo-interrupt mode */

    pPktDev->mode = VISION_PINTR_MODE;

    return (OK);
    }

/***************************************************************************
*
* wdbVisionPktTx - transmit a packet.
*
* This routine transmit a packet.
* 
* RETURNS: OK or ERROR
*
* NOTE: The packet is really a chain of mbufs. We may have to just 
*       queue up this packet if we are already transmitting.
*/

LOCAL int wdbVisionPktTx
    (
    void        *pDev,
    struct mbuf *pMbuf
    )
    {
    WDB_VISION_PKT_DEV *pPktDev    = pDev;
    struct mbuf        *pFirstMbuf = pMbuf;
    int                 lockKey;
    int                 len        = 0;
    
    if (pPktDev->outputBusy == TRUE)
        return ERROR;    

    /* 
     *  The first mbuf contains the udp/ip packet header. 
     *  This is not needed in our case so discard it. Buffer up
     *  the remaining mbufs and send to the Wind River visionPROBRE/ICE box.
     */

    pMbuf = pMbuf->m_next;    

    while (pMbuf != NULL)
        {
        if ((len + pMbuf->m_len) > WDB_VISION_PKT_MTU)
            {
            return (ERROR);
            }

        bcopy (mtod(pMbuf, char*), (char*)(&pPktDev->pTxBuffer[len]), pMbuf->m_len);

        len  += pMbuf->m_len;
        pMbuf = pMbuf->m_next;
        }

    /* 
     * Transmit packet to emulator. 
     * If an error occurred during the write(), just return with 
     * existing errno.
     */

    lockKey = intLock (); 
    pPktDev->outputBusy = TRUE;         

    pPktDev->bytesWritten = write (pPktDev->fileFd, pPktDev->pTxBuffer, len);
    
    intUnlock (lockKey);                

    if (pPktDev->bytesWritten != len)
        {
        DRIVER_RESET_OUTPUT (pPktDev);   
        return (ERROR);
        }

    wdbMbufChainFree (pFirstMbuf);
    
    DRIVER_RESET_OUTPUT (pPktDev);      

    return (OK);
    }

/***************************************************************************
*
* wdbVisionFree - free the input buffer
*
* This routine is the callback used to let us know the agent is done with the
* input buffer we loaded it.
*
* RETURNS: N/A
*/

LOCAL void wdbVisionFree
    (
    void *pDev
    )
    {
    WDB_VISION_PKT_DEV *pPktDev = pDev;

    DRIVER_RESET_INPUT (pPktDev);
    }

/***************************************************************************
*
* wdbVisionModeSet - switch driver modes   
*
* This routine switch the driver modes.
*
* RETURNS: OK for a supported mode, else ERROR
*/

LOCAL int wdbVisionModeSet
    (
    void   *pDev,
    uint_t  newMode
    )
    {
    WDB_VISION_PKT_DEV  *pPktDev = pDev;

    DRIVER_RESET_INPUT (pPktDev);
    DRIVER_RESET_OUTPUT (pPktDev);

    /*
     *  Interrupt Mode is achieved via a polling task
     */

    if (newMode == WDB_COMM_MODE_INT)
        {

        pPktDev->mode = WDB_COMM_MODE_INT;
        }
    else if (newMode == WDB_COMM_MODE_POLL)
        {
        pPktDev->mode = WDB_COMM_MODE_POLL;
        }
    else
        return (ERROR);
 
    return (OK);
    }

/***************************************************************************
*
* wdbVisionPoll - poll for a packet
*
* This routine polls for a Rx packet. If a packet has arrived it invokes
* the agents callback.
*
* RETURNS: OK if a packet has arrived, else ERROR.
*/ 

LOCAL int wdbVisionPoll
    (
    void *pDev
    )
    {
    WDB_VISION_PKT_DEV *pPktDev = pDev;
    struct mbuf        *pMbuf;

    struct udphdr *pUdpHdr;
    struct ip     *pIpHdr;

    /* If the agent is processing a previous message, skip this poll */

    if (pPktDev->inputBusy)
        return (ERROR);
    
    /* Read a packet from the emulator  */


    pPktDev->bytesRead = read (pPktDev->fileFd,
                              (char*)(&pPktDev->pRxBuffer[UDP_IP_HDR_SIZE]),
                              WDB_VISION_PKT_MTU);
    /* 
     *  If an error occurred while reading the packet, just
     *  exit with existing errno value. 
     */

    if (pPktDev->bytesRead == 0)
        return (ERROR);

    /*
     * Else pass received packet to agent. Attach a dummy UDP/IP header. 
     * (Note: most UDP/IP fields are ignorred by the agent)
     */

    pPktDev->bytesRead += UDP_IP_HDR_SIZE;
     
    pIpHdr  = (struct ip*)pPktDev->pRxBuffer;
    pUdpHdr = (struct udphdr*)((int)pIpHdr + IP_HDR_SIZE);

#ifdef FIX_IP_VH
    pIpHdr->ip_v  =  0x4; 
    pIpHdr->ip_hl =  0x5; 
#else
    pIpHdr->ip_v_hl =  0x45; 
#endif /* FIX_IP_VH */

    pIpHdr->ip_src.s_addr = VISION_DUMMY_IP_ADRS;
    pIpHdr->ip_dst.s_addr = VISION_DUMMY_IP_ADRS;

    pIpHdr->ip_sum = 0;

    pIpHdr->ip_p = IPPROTO_UDP;

    pUdpHdr->uh_sport = htons(VISION_DUMMY_PORT);
    pUdpHdr->uh_dport = htons(WDBPORT);
    pUdpHdr->uh_ulen  = pPktDev->bytesRead;
    pUdpHdr->uh_sum   = 0;

    /*
     *  Allocate a mbuf, initialize it, and pass it to the agent.
     */
    
    pMbuf = wdbMbufAlloc ();

    if (pMbuf == NULL)
        {
        DRIVER_RESET_INPUT(pPktDev);     

        return (ERROR);
        }

    pPktDev->inputBusy = TRUE;

    wdbMbufClusterInit (pMbuf, pPktDev->pRxBuffer, pPktDev->bytesRead, \
                        (int(*)())wdbVisionFree, (int)pPktDev);

    /* invoke callback */ 

    (*pPktDev->wdbDrvIf.stackRcv)(pMbuf);  

    return (OK);
    }

/***************************************************************************
* wdbVisionPollTask -  continually poll emulator buffer descriptors and
*                      buffer queues.
*
* This routine continually poll emulator buffer descriptors and buffer 
* queues. This simulates Rx and Tx interrupts.
*
* RETURNS: N/A
*/ 

int wdbVisionPollTask
    (
    void *pDev,
    int   pollTaskDelay
    )
    {
    volatile int key;

    for (;;)
        {
        taskDelay (pollTaskDelay);

        key = intLock ();  
        wdbVisionPoll (pDev);
        intUnlock (key);       
        }
    }
