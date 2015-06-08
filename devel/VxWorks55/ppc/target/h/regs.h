/* regs.h - CPU registers */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
03d,22oct01,dee  Merge from T2.1.0 ColdFire
03c,01mar00,frf  Add SH4 support for T2.
03b,23apr97,hk   added SH support.
03b,15aug97,cym  added SIMNT support.
03b,28nov96,cdp  added ARM support.
03a,26may94,yao  added PPC support.
02i,12jul95,ism  added simsolaris support
02h,19mar95,dvs  removed #ifdef TRON - tron no longer supported.
02g,04feb94,cd   added extra field to REG_INDEX for MIPS (probably safe
		 for all architectures really)
02f,09jun93,hdn  added support for I80X86
02e,02dec93,pme  added Am29K support.
02d,11aug93,gae  vxsim hppa.
02c,20jun93,gae  vxsim.
02b,22sep92,rrr  added support for c++
02a,04jul92,jcf  cleaned up.
01k,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01j,20feb92,yao  added REG_INDEX.
01i,09jan92,jwt  converted CPU==SPARC to CPU_FAMILY==SPARC.
01h,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed copyright notice
01g,02aug91,ajm  added MIPS support
01f,19jul91,gae  renamed architecture specific include file to be xx<arch>.h.
01e,29apr91,hdn  added defines and macros for TRON architecture.
01d,25oct90,shl  fixed CPU_FAMILY logic so 68k and sparc won't clash when
		 compiling for sparc.
01c,05oct90,shl  added copyright notice.
                 made #endif ANSI style.
01b,28sep90,del  added include i960/regs.h for I960 CPU_FAMILY.
01a,07aug89,gae  written.
*/

#ifndef __INCregsh
#define __INCregsh

#ifdef __cplusplus
extern "C" {
#endif

#if 	CPU_FAMILY==I960
#include "arch/i960/regsI960.h"
#endif	/* CPU_FAMILY==I960 */

#if 	CPU_FAMILY==MC680X0
#include "arch/mc68k/regsMc68k.h"
#endif	/* CPU_FAMILY==MC680X0 */

#if 	CPU_FAMILY==COLDFIRE
#include "arch/coldfire/regsColdfire.h"
#endif	/* CPU_FAMILY==COLDFIRE */

#if     CPU_FAMILY==MIPS
#include "arch/mips/regsMips.h"
#endif	/* CPU_FAMILY==MIPS */

#if     CPU_FAMILY==PPC
#include "arch/ppc/regsPpc.h"
#endif  /* CPU_FAMILY==PPC */

#if	CPU_FAMILY==SPARC
#include "arch/sparc/regsSparc.h"
#endif	/* CPU_FAMILY==SPARC */

#if	CPU_FAMILY==SIMSPARCSUNOS
#include "arch/simsparc/regsSimsparc.h"
#endif	/* CPU_FAMILY==SIMSPARCSUNOS */

#if	CPU_FAMILY==SIMSPARCSOLARIS
#include "arch/simsolaris/regsSimsolaris.h"
#endif	/* CPU_FAMILY==SIMSPARCSOLARIS */

#if	CPU_FAMILY==SIMHPPA
#include "arch/simhppa/regsSimhppa.h"
#endif	/* CPU_FAMILY==SIMHPPA */

#if	CPU_FAMILY==SIMNT
#include "arch/simnt/regsSimnt.h"
#endif	/* CPU_FAMILY==SIMNT */

#if     CPU_FAMILY==I80X86
#include "arch/i86/regsI86.h"
#endif	/* CPU_FAMILY==I80X86 */

#if     CPU_FAMILY==AM29XXX
#include "arch/am29k/regsAm29k.h"
#endif	/* CPU_FAMILY==AM29XXX */

#if	CPU_FAMILY==SH
#include "arch/sh/regsSh.h"
#endif	/* CPU_FAMILY==SH */

#if     CPU_FAMILY==ARM
#include "arch/arm/regsArm.h"
#endif  /* CPU_FAMILY==ARM */

#ifndef	_ASMLANGUAGE
typedef struct regindex
    {
    char	*regName;	/* pointer to register name */
    int		regOff;		/* offset to entry in REG_SET */
#if CPU_FAMILY==MIPS
    int		regWidth;	/* register width */
#endif
#if (CPU_FAMILY==COLDFIRE)
    int		regWidth;	/* register width */
    int		regStandard;	/* register is a "standard" register */
#endif
    } REG_INDEX;
#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCregsh */
