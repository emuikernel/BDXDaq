/* wdbVioDrv.h - header file for remote debug packet library */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,28jun95,ms  written.
*/

#ifndef __INCwdbVioDrvh
#define __INCwdbVioDrvh

/* includes */

#include "wdb/wdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* function prototypes */

#if defined(__STDC__)

extern void    wdbVioDrv (char * devName);

#else   /* __STDC__ */

extern void    wdbPktDevInit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbVioDrvh */

