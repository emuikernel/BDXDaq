/* symSyncLib.h - header file for symSyncLib.c */

/* Copyright 1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,13dec96,elp changed syncLibInit() into symSyncLibInit(), added
		symSyncTimeoutSet().
01b,06nov96,elp Removed useless definitions.
01a,04oct96,elp written.
*/

#ifndef __INCsyncLibh
#define __INCsyncLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "vxWorks.h"
#include "stdlib.h"
#include "wtx.h"
#include "symLib.h"
#include "moduleLib.h"
#include "dllLib.h"

/* externals */

extern UINT32		syncRegistry;		/* registry inet address */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void 		symSyncLibInit ();
extern UINT32		symSyncTimeoutSet (UINT32 timeout);

#else	/* __STDC__ */

extern void 		symSyncLibInit ();
extern UINT32		symSyncTimeoutSet ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCsyncLibh */
