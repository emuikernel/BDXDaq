/* excLib.h - exception library header */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
02e,13nov01,yvp  Added excExtendedVectors - support for extended branch
                 vectors. Also moved private functions into the newly created
                 h/privare/excLibP.h.
02d,22oct01,dee  Merge from T2.1.0 ColdFire
02c,01mar00,frf  Add SH4 support for T2.
02b,23apr97,hk   added SH support.
02b,15aug97,cym  added SIMNT support.
02b,28nov96,cdp  added ARM support.
02a,26may94,yao  added PPC support.
01y,12jul95,ism  added simsolaris support
01x,19mar95,dvs  removed #ifdef TRON - tron no longer supported.
01w,29jan94,gae  added ASMLANGUAGE around typedef.
01v,02dec93,pme  added Am29K family support
01u,15oct93,cd   added #ifndef _ASMLANGUAGE.
01t,11aug93,gae  vxsim hppa.
01s,20jun93,gae  vxsim.
01r,09jun93,hdn  added support for I80X86
01q,22sep92,rrr  added support for c++
01p,02aug92,jcf  removed excDeliverHook decl/added excShowInit.
01o,27jul92,rrr  added excDeliverHook decl
01n,04jul92,jcf  cleaned up.
01m,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01l,20jan92,yao  added ANSI function prototype for excInfoShow(), printExc().
		 added EXC_FAULT_TAB.
01k,09jan92,jwt  added ANSI function prototype for excJobAdd().
01j,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01i,02aug91,ajm  added MIPS support
01h,19jul91,gae  renamed architecture specific include file to be xx<arch>.h.
01g,22may91,wmd  made to include "CPU/excLib.h".
01f,29apr91,hdn  added TRON defines and macros.
01e,05oct90,shl  added ANSI function prototypes.
                 made #endif ANSI style.
                 added copyright notice.
01d,29sep90,del  added include i960/exc960Lib.h for I960 CPU_FAMILY
01c,01aug90,jcf  padded EXC_INFO to four byte boundary.
		 changed USHORT to UINT16.
01b,28apr89,mcl  added SPARC.
01a,28may88,dnw  written
*/

#ifndef __INCexcLibh
#define __INCexcLibh

#ifdef __cplusplus
extern "C" {
#endif

#if 	CPU_FAMILY==I960
#include "arch/i960/excI960Lib.h"
#endif	/* CPU_FAMILY==I960 */

#if     CPU_FAMILY==MC680X0
#include "arch/mc68k/excMc68kLib.h"
#endif  /* CPU_FAMILY==MC680X0 */

#if     CPU_FAMILY==COLDFIRE
#include "arch/coldfire/excColdfireLib.h"
#endif  /* CPU_FAMILY==COLDFIRE */

#if     CPU_FAMILY==MIPS
#include "arch/mips/excMipsLib.h"
#endif	/* CPU_FAMILY==MIPS */

#if     CPU_FAMILY==PPC
#include "arch/ppc/excPpcLib.h"
#endif  /* CPU_FAMILY==PPC */

#if	CPU_FAMILY==SPARC
#include "arch/sparc/excSparcLib.h"
#endif	/* CPU_FAMILY==SPARC */

#if	CPU_FAMILY==SIMSPARCSUNOS
#include "arch/simsparc/excSimsparcLib.h"
#endif	/* CPU_FAMILY==SIMSPARCSUNOS */

#if	CPU_FAMILY==SIMSPARCSOLARIS
#include "arch/simsolaris/excSimsolarisLib.h"
#endif	/* CPU_FAMILY==SIMSPARCSOLARIS */

#if	CPU_FAMILY==SIMHPPA
#include "arch/simhppa/excSimhppaLib.h"
#endif	/* CPU_FAMILY==SIMHPPA */

#if	CPU_FAMILY==SIMNT
#include "arch/simnt/excSimntLib.h"
#endif	/* CPU_FAMILY==SIMNT */

#if     CPU_FAMILY==I80X86
#include "arch/i86/excI86Lib.h"
#endif	/* CPU_FAMILY==I80X86 */

#if     CPU_FAMILY==AM29XXX
#include "arch/am29k/excAm29kLib.h"
#endif	/* CPU_FAMILY==AM29XXX */

#if	CPU_FAMILY==SH
#include "arch/sh/excShLib.h"
#endif	/* CPU_FAMILY==SH */

#if     CPU_FAMILY==ARM
#include "arch/arm/excArmLib.h"
#endif  /* CPU_FAMILY==ARM */


#ifndef	_ASMLANGUAGE

typedef struct  excfaultTab
    {
    int faultType;		/* fault type */
    int subtype;		/* fault sub type */
    int signal;			/* signal */
    int code;			/* code */
    } EXC_FAULT_TAB;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)
extern STATUS 	excShowInit (void);
extern void 	excHookAdd (FUNCPTR excepHook);
extern STATUS 	excJobAdd (VOIDFUNCPTR func, int arg1, int arg2, int arg3,
			   int arg4, int arg5, int arg6);
#else
extern STATUS 	excShowInit ();
extern void 	excHookAdd ();
extern STATUS 	excJobAdd ();
extern void 	printExc ();
#endif	/* __STDC__ */

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCexcLibh */
