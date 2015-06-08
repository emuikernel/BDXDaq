/* wdbVisionPktDrv.h - header file for Wind River Vision Driver WDB agent packet driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,12jan02,g_h  Renaming filed name in WDB_VISION_PKT_DEV struct
01g,01dec01,g_h  Cleaning for T2.2
01f,07may01,g_h  add the FIX_IP_VH macro
01e,06may01,g_h  add support for visionPORT
01d,05may01,g_h  renaming moulde name to wdbVisionPktDrv.h and cleaning
01c,07feb01,g_h  renaming module name and cleaning
01b,03may97,est  adapted for EST Background mode emulator 
01a,23aug95,ms   written.
*/

#ifndef __INCwdbVisionPktDrvh
#define __INCwdbVisionPktDrvh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "sioLib.h"
#include "wdb/wdb.h"
#include "wdb/wdbCommIfLib.h"
#include "wdb/wdbMbufLib.h"

/* defines */

/* Patch for TMD/usrWdb.c structure problem (SENS) */

#define  FIX_IP_VH
                           
#define WDB_VISION_PKT_MTU      1488 /* MUST MATCH VISION_PKT_MTU */
                                     /* in wdbVisionDrv.h         */

/* lower WDB_MTU to WDB_VISION_PKT_MTU for Wind River Vision Connections */

#if	(WDB_MTU > WDB_VISION_PKT_MTU)
#undef	WDB_MTU
#define	WDB_MTU	                WDB_VISION_PKT_MTU
#endif	/* (WDB_MTU > WDB_VISION_PKT_MTU) */

#define VISION_DUMMY_PORT       ((u_short)0x1237) /* port number for wdb packet */
#define VISION_DUMMY_IP_ADRS    0x12345678        /* ip address for wdb packet  */

#define IP_HDR_SIZE             20                /* UDP/IP Header Information */
#define UDP_HDR_SIZE            8
#define UDP_IP_HDR_SIZE         (IP_HDR_SIZE + UDP_HDR_SIZE)

/* data types */

typedef struct      
    {
    WDB_DRV_IF  wdbDrvIf;     /* This must be first */
    int         fileFd;       /* Vision Driver file descriptor from open() */
    int         pollTaskId;   /* Polling Task Id */
    int         mode;         /* current mode - poll or int (not used) */
    bool_t      inputBusy;
    bool_t      outputBusy;
    char      * pRxBuffer;    /* Input Buffer, loaned to agent on receipt */
    int         bytesRead;         
    char      * pTxBuffer;    /* Output Buffer, chain of mbufs */
    int         bytesWritten;         
    } WDB_VISION_PKT_DEV;


/* function prototypes */

#if defined(__STDC__)

IMPORT int wdbVisionPktDevInit (WDB_VISION_PKT_DEV *pPktDev, void (*stackRcv)());

#else   /* __STDC__ */

IMPORT int wdbVisionPktDevInit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbVisionPktDrvh */

