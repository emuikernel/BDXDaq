/* wdbArchIfLib.h - header file for arch-specific routines needed by wdb */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,25apr02,jhw  Added C++ support (SPR 76304).
01c,15jun95,ms	removed _sigCtxIntLock prototype
01b,05apr95,ms	new data types.
01a,21dec94,ms  written.
*/

#ifndef __INCwdbArchIfLibh
#define __INCwdbArchIfLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "wdb/wdb.h"
#include "wdb/wdbRegs.h"
#include "intLib.h"

/* function prototypes */

#if defined(__STDC__)

extern int   	_sigCtxSave	(WDB_IU_REGS *pContext);
extern void   	_sigCtxLoad	(WDB_IU_REGS *pContext);
extern void   	_sigCtxSetup	(WDB_IU_REGS *pContext, char *pStackBase,
				 void (*entry)(), int *pArgs);
extern void	_sigCtxRtnValSet (WDB_IU_REGS *pContext, int val);

#else   /* __STDC__ */

extern int   	_sigCtxSave	();
extern void   	_sigCtxLoad	();
extern void   	_sigCtxSetup	();
extern void	_sigCtxRtnValSet ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbArchIfLibh */

