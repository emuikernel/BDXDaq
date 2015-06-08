/* systm.h - system header file */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * and the VxWorks Software License Agreement specify the terms and
 * conditions for redistribution.
 *
 *	@(#)systm.h	7.1 (Berkeley) 6/4/86
 */

/*
modification history
--------------------
01n,10oct01,rae  merge from truestack (VIRTUAL_STACK)
01m,28feb00,frf  Add SH4 support for T2
01m,04sep98,cdp  make ARM CPUs with ARM_THUMB==TRUE use portable routines.
01l,20sep97,tpr	 added ARM to !PORTABLE list.
01k,22sep92,rrr  added support for c++
                  updated copyright
02k,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed copyright notice
02j,07mar91,ajm  ifdef'd out _remque, and _insque for mips compiler
02i,05oct90,shl  added copyright notice.
02h,20mar90,jcf  changed semTake operations to include timeout of WAIT_FOREVER.
02g,22jun88,dnw  name tweaks to spl macros.
		 added macros for ovbcopy(), copyin(), copyout(), and imin().
02f,04mar88,jcf  changed splSem to splSemId, and changed sem calls accordingly.
02e,27jan88,rdc  included taskLib.h instead of vxLib.h
02d,23jan88,rdc  changed names of processor level macros because of
		 size issues.
02c,08jan88,rdc  added processor level macros.
02b,28aug87,dnw  removed unnecessary stuff.
02a,03apr87,ecs  added header and copyright.
*/

#ifndef __INCsystmh
#define __INCsystmh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "taskLib.h"
#include "semLib.h"

#ifdef VIRTUAL_STACK
#include "netinet/vsLib.h"
#endif

#if ( defined(PORTABLE) || \
      ( \
        (CPU!=MC68020) && (CPU!=MC68040) && (CPU!=MC68060) && \
        (CPU_FAMILY!=I80X86) && (CPU_FAMILY!=AM29XXX) && \
        (CPU_FAMILY!=SH) && (CPU_FAMILY!=ARM) \
      ) || \
     ((CPU_FAMILY == ARM) && ARM_THUMB))
#define unixLib_PORTABLE
#define INCHKSUM_PORTABLE
#endif	/* defined(PORTABLE) || (CPU_FAMILY != MC680X0) */

#ifndef VIRTUAL_STACK
IMPORT SEM_ID splSemId;
IMPORT int splTid;
#endif


/* casts to keep lint happy */

#define insque(q,p)     _insque((caddr_t)q,(caddr_t)p)
#define remque(q)       _remque((caddr_t)q)

/* macros for Unix routines */

#define ovbcopy		bcopy			/* overlapped buffer copy */
#define copyout(from, to, len)	(bcopy (from, to, len), OK)
#define copyin(from, to, len)	(bcopy (from, to, len), OK)
#define imin(a,b)	(((a) < (b)) ? (a) : (b))	/* interger min */
#define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))


/* processor level macro -
 * these are identical to the routines spl...() and can be used where
 * speed is more important than code size.
 */

#define SPLNET()	(((ULONG)taskIdCurrent == splTid) ?	\
		    	   1 : (semTake(splSemId, WAIT_FOREVER),  \
			       splTid = (ULONG)taskIdCurrent, 0))

#define SPLIMP()	SPLNET()

#define SPLX(x)		if ((x) == 0) (splTid = 0, semGive (splSemId))

#ifdef __cplusplus
}
#endif

#endif /* __INCsystmh */
