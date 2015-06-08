/* vxLib.h - header file for vxLib.c */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
02l,14nov01,ahm  added vxI86Lib.h for I80x86 family
02k,11oct01,cjj  removed Am29K support
02j,17apr00,hk   added #include for sh/vxShLib.h.
02i,04jun97,dat  added _func_vxMemProbeHook and vxMemArchProbe, SPR 8658
		 removed sysMemProbe.
02h,22oct96,spm  added comment specifying location of checksum() definition
02g,28feb96,tam	 added #include for ppc/vxPpcLib.h.
02f,04jul94,tpr	 added #include for mc68k/vx68kLib.h.
02e,02dec93,pme  added Am29K family support.
02d,26jul94,jwt  added vxMemProbeAsi() prototype for SPARC; copyright '94.
02c,24sep92,yao  added missing arg in vxTas() declaration.
02b,22sep92,rrr  added support for c++
02a,04jul92,jcf  cleaned up.
01f,30jun92,jmm  moved checksum() declarations to here from icmpLib.h
01e,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01d,21apr92,ccc  added vxTas.
01c,27feb92,wmd  added #include for i960/vx960Lib.h.
01b,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed copyright notice
01a,05oct90,shl created.
*/

#ifndef __INCvxLibh
#define __INCvxLibh

#ifdef __cplusplus
extern "C" {
#endif

#if	(CPU_FAMILY == MC680X0)
#include "arch/mc68k/vx68kLib.h"
#endif	/* (CPU_FAMILY == MC680X0) */

#if	CPU_FAMILY==I960
#include "arch/i960/vxI960Lib.h"
#endif  /* CPU_FAMILY==I960 */

#if     CPU_FAMILY==PPC
#include "arch/ppc/vxPpcLib.h"
#endif  /* CPU_FAMILY==PPC */

#if     CPU_FAMILY==SH
#include "arch/sh/vxShLib.h"
#endif  /* CPU_FAMILY==SH */


#if     CPU_FAMILY==I80X86
#include "arch/i86/vxI86Lib.h"
#endif  /* CPU_FAMILY==I80X86 */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	vxMemProbe (char * adrs, int mode, int length, char * pVal);
extern BOOL 	vxTas (void * address);
extern STATUS 	vxMemArchProbe (void * adrs, int mode, int length, void * pVal);
extern STATUS 	(* _func_vxMemProbeHook)(void *, int, int, void *);

/* checksum is defined in ./target/src/util/cksumLib.c */
extern u_short	checksum (u_short * pAddr, int len);

#if	(CPU_FAMILY == SPARC)
extern STATUS 	vxMemProbeAsi (char * adrs, int mode, int length, char * pVal,
			       int asi);
#endif	/* CPU_FAMILY == SPARC */

#else	/* __STDC__ */

extern STATUS 	vxMemProbe ();
extern STATUS 	vxMemArchProbe ();
extern FUNCPTR 	_func_vxMemProbeHook;
extern BOOL 	vxTas ();

/* checksum is defined in ./target/src/util/cksumLib.c */
extern u_short	checksum ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCvxLibh */
