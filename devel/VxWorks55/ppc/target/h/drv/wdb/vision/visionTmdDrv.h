/* visionTmdDrv.h - header file for Wind River Transparent Mode Driver */

/* Copyright 1986-2002 Wind River Systems Inc. */

/*
modification history
--------------------
01f,21dec01,g_h  Rename to visionTmdDrv.h
01e,01dec01,g_h  Cleaning for T2.2
01d,05may01,g_h  renaming module name to visionTMDrv.h and cleaning
01c,09apr01,rnr  Changed to be a small I/O subsystem
01b,07feb01,g_h  renaming module name and cleaning
01a,15may97,est  written.
*/

#ifndef __INCvisionTmdDrvh
#define __INCvisionTmdDrvh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define TX_BUF_EMPTY            0x0000
#define TX_READY                TX_BUF_EMPTY
#define TX_BUF_NOT_EMPTY        0x8000
#define TX_NOT_READY            TX_BUF_NOT_EMPTY
#define RX_BUF_EMPTY            0x0000     
#define RX_READY                RX_BUF_NOT_EMPTY
#define RX_BUF_NOT_EMPTY        0x8000
#define RX_NOT_READY            RX_BUF_EMPTY

/* typedefs */

typedef struct 
    {
    u_short    status;
    u_short    count;
    u_char     buf[VISION_PKT_MTU];
    } TMD_DESCRIPTOR;

#ifdef __cplusplus
}
#endif

#endif  /* __INCvisionTmdDrvh */





