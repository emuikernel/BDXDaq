/* dsmLib.h - disassembler library header */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
02d,22oct01,dee  Merge from T2.1.0 ColdFire
02c,01mar00,frf  Add SH4 support for T2.
02b,23apr97,hk   added SH support.
02b,14aug97,cym  added SIMNT support.
02b,28nov96,cdp  added ARM support.
02a,14sep94,caf  added PPC support.
01s,12jul95,ism  added simsolaris support
01r,19mar95,dvs  removed #ifdef TRON - tron no longer supported.
01q,02dec93,pad  added Am29k family support.
01p,11aug93,gae  vxsim hppa.
01o,20jun93,gae  vxsim.
01n,09jun93,hdn  added support for I80X86
01m,22sep92,rrr  added support for c++
01l,04jul92,jcf  cleaned up.
01k,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01j,09jan92,jwt  converted CPU==SPARC to CPU_FAMILY==SPARC.
01i,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed copyright notice
01h,10seo91,wmd  fixed typo in comments.
01g,02aug91,ajm  added MIPS support
01f,19jul91,gae  renamed architecture specific include file to be xx<arch>.h.
01e,29apr91,hdn  added defines and macros for TRON architecture.
01d,25oct90,shl  fixed CPU_FAMILY logic so 68k and sparc won't clash when
		 compiling for sparc.
01c,05oct90,shl  added copyright notice.
                 made #endif ANSI style.
01b,28sep90,del  include i960/dsmLib.h.
01a,07aug89,gae  written.
*/

#ifndef __INCdsmLibh
#define __INCdsmLibh

#ifdef __cplusplus
extern "C" {
#endif

#if 	CPU_FAMILY==I960
#include "arch/i960/dsmI960Lib.h"
#endif	/* CPU_FAMILY==I960 */

#if	CPU_FAMILY==MC680X0
#include "arch/mc68k/dsmMc68kLib.h"
#endif	/* CPU_FAMILY==MC680X0 */

#if	CPU_FAMILY==COLDFIRE
#include "arch/coldfire/dsmColdfireLib.h"
#endif	/* CPU_FAMILY==COLDFIRE */

#if     CPU_FAMILY==MIPS
#include "arch/mips/dsmMipsLib.h"
#endif	/* CPU_FAMILY==MIPS */

#if     CPU_FAMILY==PPC
#include "arch/ppc/dsmPpcLib.h"
#endif  /* CPU_FAMILY==PPC */

#if	CPU_FAMILY==SPARC
#include "arch/sparc/dsmSparcLib.h"
#endif	/* CPU_FAMILY==SPARC */

#if	CPU_FAMILY==SIMSPARCSUNOS
#include "arch/simsparc/dsmSimsparcLib.h"
#endif	/* CPU_FAMILY==SIMSPARCSUNOS */

#if	CPU_FAMILY==SIMSPARCSOLARIS
#include "arch/simsolaris/dsmSimsolarisLib.h"
#endif	/* CPU_FAMILY==SIMSPARCSOLARIS */

#if	CPU_FAMILY==SIMHPPA
#include "arch/simhppa/dsmSimhppaLib.h"
#endif	/* CPU_FAMILY==SIMHPPA */

#if	CPU_FAMILY==SIMNT
#include "arch/simnt/dsmSimntLib.h"
#endif	/* CPU_FAMILY==SIMNT*/

#if     CPU_FAMILY==I80X86
#include "arch/i86/dsmI86Lib.h"
#endif	/* CPU_FAMILY==I80X86 */

#if     CPU_FAMILY==AM29XXX
#include "arch/am29k/dsmAm29kLib.h"
#endif	/* CPU_FAMILY==AM29XXX */

#if	CPU_FAMILY==SH
#include "arch/sh/dsmShLib.h"
#endif	/* CPU_FAMILY==SH */

#if     CPU_FAMILY==ARM
#include "arch/arm/dsmArmLib.h"
#endif  /* CPU_FAMILY==ARM */

#ifdef __cplusplus
}
#endif

#endif /* __INCdsmLibh */
