/* esfSimhppa.h - simhppa exception stack frame */

/* Copyright 1993-1994 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,25jan94,gae  defined ESF.
01a,11aug93,gae  from rrr.
*/

#ifndef __INCesfSimhppah
#define __INCesfSimhppah

#ifdef __cplusplus
extern "C" {
#endif

#include "regs.h"
#include "excLib.h"

#define	ESF	EXC_INFO

#ifdef __cplusplus
}
#endif

#endif /* __INCesfSimhppah */
