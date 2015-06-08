/* wdbVisionDrv.h - header file for Wind River Generic Vision Driver */

/* Copyright 1986-2002 Wind River Systems Inc. */

/*
modification history
--------------------
01b,05may01,g_h  renaming to wdbVisionDrv.h and cleaning
01a,09apr01,rnr  written.
*/

#ifndef __INCwdbVisionDrvh
#define __INCwdbVisionDrvh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include   "vxWorks.h"
#include   "semLib.h"

/* defines */

#define VISION_DEFAULT_POLL_PRI        200  /* Rx polling task priority     */
#define VISION_DEFAULT_POLL_DELAY      2    /* Polling Task delay tick      */

#define VISION_PKT_MTU                 1488 /* Maximum Transmission Unit    */

#define VISION_MAX_PTASK_DELAY         50   /* Maximum poll loop delay      */
#define VISION_MIN_PTASK_PRIOR         5    /* Maximum polltask priority    */
#define VISION_MAX_PTASK_PRIOR         255  /* Minimum polltask priority    */
                                            /* (MIN/MAX reversed,0 is high) */

#define TX_BUFFER_SIZE                 (VISION_PKT_MTU * 2)  /* default queue sizes  */
#define RX_BUFFER_SIZE                 (VISION_PKT_MTU * 2)  /* changed with ioctl() */
#define LOOP_BUFFER_SIZE               (VISION_PKT_MTU * 10) /* Loopback space       */

/* Tx/Rx Modes: used in ioctl() case statements */

#define VISION_POLL_MODE               0x0001 /* Polled / Pseudo Interrupt */
#define VISION_PINTR_MODE              0x0002
#define VISION_PISR_MODE               0x0004
#define VISION_BLOCKED_MODE            0x0008 /* Blocking / Non-Blocking   */
#define VISION_NONBLOCKED_MODE         0x0010
#define VISION_BUFFERED_MODE           0x0020 /* Buffered / Non-Buffered   */
#define VISION_NONBUFFERED_MODE        0x0040
#define VISION_LPBK_MODE               0x0080 /* Normal / Loopback Mode    */
#define VISION_NORMAL_MODE             0x0100
#define VISION_TX_QUEUE_SIZE           0x0200 /* Tx/Rx  Queue Sizes        */
#define VISION_RX_QUEUE_SIZE           0x0400
#define VISION_PTASK_PRIORITY          0x0800 /* Change pTask Priority     */
#define VISION_PTASK_DELAY             0x1000 /* Change pTask Loop Delay   */

/* Driver States */

#define VISION_STATE_CLOSED            0
#define VISION_STATE_OPEN              1

/* Tx/Rx Buffering Queues */ 

typedef struct 
    {
    int   nofChars; /* Number of bytes currently stored      */
    int   size;     /* Maximum size of the queue             */
    int   head;     /* Current start of data in queue buffer */
    int   tail;     /* Current end   of data in queue buffer */
    char *pData;    /* Buffer where data is stored           */
    } VISION_QUEUE;

/* MACRO definitions for operating system specific calls */

#define MEMCOPY(d, s, c)               memcpy(d, s, c)

typedef SEM_ID CRITSECT;

#define CREATE_CRITSECT(s)             semMCreate(SEM_Q_FIFO) 
#define REMOVE_CRITSECT(s)             semDelete(s)
#define ENTER_CRITSECT(s)              semTake(s,WAIT_FOREVER)
#define EXIT_CRITSECT(s)               semGive(s)

/* 
 *   DELAY_TASK - places the current task or thread to sleep for c msecs/ticks
 */

#define DELAY_TASK(c)                  taskDelay(c)

/* VDRIV_DEV - Wind River Vision Device descriptor */ 

typedef struct      
    {                      
    DEV_HDR             devHdr;        /* DEV_HDR must com first     */
    char                state;         /* State of the driver        */
    char               *pDeviceName;   /* device name: etd/0         */
    int                 opMode;        /* Poll/Pseudo Interrupt Mode */
    int                 rdWrMode;      /* read/write mode            */
    int                 bufferMode;    /* Buffering mode             */
    int                 blockMode;     /* Blocking mode              */
    int                 loopMode;      /* Diagnostic loopback        */
    int                 pollDelay;     /* Polling Task Delay         */
    int                 pollTaskId;    /* Polling Task Id            */
    VISION_QUEUE        txQueue;       /* Transmit buffer            */
    VISION_QUEUE        rxQueue;       /* Receive buffer             */
    VISION_QUEUE        loopQueue;     /* Loopback queue             */
    CRITSECT            txCrSection;   /* Xmit     Queue protection  */
    CRITSECT            rxCrSection;   /* Recv     Queue protection  */
    CRITSECT            loopCrSection; /* Loopback Queue protection  */
    V_DRIVER_INTERFACE  inter;         /* Interface structure        */
    } VDRIV_DEV ;

/* function prototypes */

#if defined(__STDC__)

IMPORT int visionDriverInit (void);
IMPORT int visionDriverCreate (char *, int, int, int, int, int);

#else   /* __STDC__ */

IMPORT int visionDriverInit ();
IMPORT int visionDriverCreate ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbVisionDrvh */
