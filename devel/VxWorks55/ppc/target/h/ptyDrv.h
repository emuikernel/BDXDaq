/* ptyDrv.h - header file for ptyDrv.c */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,02nov01,brk  SPR65498 add SELECT functionality to master
01f,14feb01,spm  merged from version 01f of tor2_0_x branch (base 01e):
                 added removal of pty device (SPR #28675)
01e,22sep92,rrr  added support for c++
01d,04jul92,jcf  cleaned up.
01c,26may92,rrr  the tree shuffle
01b,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed copyright notice
01a,05oct90,shl created.
*/

#ifndef __INCptyDrvh
#define __INCptyDrvh

#include "tyLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	ptyDevCreate (char *name, int rdBufSize, int wrtBufSize);
extern STATUS 	ptyDevRemove (char *pName);
extern STATUS 	ptyDrv (void);

#else	/* __STDC__ */

extern STATUS 	ptyDevCreate ();
extern STATUS 	ptyDevRemove ();
extern STATUS 	ptyDrv ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

typedef struct		/* PSEUDO_DEV */
    {
    TY_DEV	tyDev;
    DEV_HDR	slaveDev;
    SEMAPHORE	masterReadSyncSem;
    BOOL	ateof;
    SEL_WAKEUP_LIST masterSelWakeupList;
    } PSEUDO_DEV;

#endif /* __INCptyDrvh */
