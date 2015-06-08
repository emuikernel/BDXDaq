/* wdbUdpSockLib.h - header file for remote debug agents UDP library */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,25apr02,jhw  Added C++ support (SPR 76304).
01b,05apr95,ms  new data types.
01a,08mar95,ms  written.
*/

#ifndef __INCwdbUdpSockLibh
#define __INCwdbUdpSockLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "wdb/wdbCommIfLib.h"

/* function prototypes */

#if defined(__STDC__)

extern STATUS  wdbUdpSockIfInit (WDB_COMM_IF *pCommIf);

#else   /* __STDC__ */

extern STATUS  wdbUdpSockIfInit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbUdpSockLibh */
