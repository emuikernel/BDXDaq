/* esf.h - exception stack frames */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
02d,22oct01,dee  Merge fro T2.1.0 ColdFire
02c,01mar00,frf  Add SH4 support for T2.
02b,23apr97,hk   added SH support.
02b,15aug97,cym  added SIMNT support.
02b,28nov96,cdp  added ARM support.
02a,26may94,yao  added PPC support.
01o,19mar95,dvs  removed #ifdef TRON - tron no longer supported.
01n,02dec93,pme  added Am29K family support 
01m,11aug93,gae  vxsim hppa.
01l,20jun93,gae  vxsim.
01k,09jun93,hdn  added support for I80X86
01j,22sep92,rrr  added support for c++
01i,04jul92,jcf  cleaned up.
01h,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01g,09jan92,jwt  converted CPU==SPARC to CPU_FAMILY==SPARC.
01f,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed copyright notice
01e,02aug91,ajm  added MIPS support
01d,19jul91,gae  renamed architecture specific include file to be xx<arch>.h.
01c,29apr91,hdn  added defines and macros for TRON architecture.
01b,05oct90,shl  added copyright notice.
                 made #endif ANSI style.
01a,07aug89,gae  written.
*/

#ifndef __INCesfh
#define __INCesfh

#ifdef __cplusplus
extern "C" {
#endif

#if	(CPU_FAMILY==MC680X0)
#include "arch/mc68k/esfMc68k.h"
#endif	/* CPU_FAMILY==MC680X0 */

#if	(CPU_FAMILY==COLDFIRE)
#include "arch/coldfire/esfColdfire.h"
#endif	/* CPU_FAMILY==COLDFIRE */

#if	(CPU_FAMILY==SPARC)
#include "arch/sparc/esfSparc.h"
#endif	/* CPU_FAMILY==SPARC */

#if	(CPU_FAMILY==SIMSPARCSUNOS)
#include "arch/simsparc/esfSimsparc.h"
#endif	/* CPU_FAMILY==SIMSPARCSUNOS */

#if	(CPU_FAMILY==SIMSPARCSOLARIS)
#include "arch/simsolaris/esfSimsolaris.h"
#endif	/* CPU_FAMILY==SIMSPARCSOLARIS */

#if	(CPU_FAMILY==SIMHPPA)
#include "arch/simhppa/esfSimhppa.h"
#endif	/* CPU_FAMILY==SIMHPPA */

#if	(CPU_FAMILY==SIMNT)
#include "arch/simnt/esfSimnt.h"
#endif	/* CPU_FAMILY==SIMNT */

#if     (CPU_FAMILY==MIPS)
#include "arch/mips/esfMips.h"
#endif	/* MIPS */

#if     (CPU_FAMILY==PPC)
#include "arch/ppc/esfPpc.h"
#endif  /* CPU_FAMILY==PPC */

#if     (CPU_FAMILY==I80X86)
#include "arch/i86/esfI86.h"
#endif	/* CPU_FAMILY==I80X86 */

#if     (CPU_FAMILY==AM29XXX)
#include "arch/am29k/esfAm29k.h"
#endif	/* CPU_FAMILY==AM29XXX */

#if	(CPU_FAMILY==SH)
#include "arch/sh/esfSh.h"
#endif	/* CPU_FAMILY==SH */

#if     CPU_FAMILY==ARM
#include "arch/arm/esfArm.h"
#endif  /* CPU_FAMILY==ARM */

#ifdef __cplusplus
}
#endif

#endif /* __INCesfh */
