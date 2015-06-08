/* ifIndexLib.h - interface index library header */

/* Copyright 2000 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,29mar01,spm  file creation: copied from version 01c of tor2_0.open_stack
                 branch (wpwr VOB) for unified code base
*/

#ifndef __INCifIndexLibh
#define __INCifIndexLibh

#ifdef __cplusplus
extern "C" {
#endif


/* function declarations */

extern void ifIndexLibInit(void);
extern void ifIndexLibShutdown(void);
extern int  ifIndexAlloc(void);
extern BOOL ifIndexTest(int ifIndex);

#ifdef __cplusplus
}
#endif

#endif /* __INCifIndexLibh */
