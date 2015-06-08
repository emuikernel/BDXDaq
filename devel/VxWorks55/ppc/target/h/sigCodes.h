/* sigCodes.h - defines for the code passed to signal handlers */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,01mar00,frf  Add SH4 support for T2.
01f,23apr97,hk   added support for SH.
01e,07jun95,yao  added support for PPC.
01d,19mar95,dvs  removed #ifdef TRON - tron no longer supported.
01c,09jun93,hdn  added support for I80X86
01b,22sep92,rrr  added support for c++
01a,20aug92,rrr  written.
*/

#ifndef __INCsigCodesh
#define __INCsigCodesh

#ifdef __cplusplus
extern "C" {
#endif

#if 	CPU_FAMILY==I960
#include "arch/i960/sigI960Codes.h"
#endif	/* CPU_FAMILY==I960 */

#if 	CPU_FAMILY==MC680X0
#include "arch/mc68k/sigMc68kCodes.h"
#endif	/* CPU_FAMILY==MC680X0 */

#if     CPU_FAMILY==MIPS
#include "arch/mips/sigMipsCodes.h"
#endif	/* CPU_FAMILY==MIPS */

#if	CPU_FAMILY==SPARC
#include "arch/sparc/sigSparcCodes.h"
#endif	/* CPU_FAMILY==SPARC */

#if     CPU_FAMILY==I80X86
#include "arch/i86/sigI86Codes.h"
#endif	/* CPU_FAMILY==I80X86 */

#if     CPU_FAMILY==PPC
#include "arch/ppc/sigPpcCodes.h"
#endif	/* CPU_FAMILY==PPC */

#if	CPU_FAMILY==SH
#include "arch/sh/sigShCodes.h"
#endif	/* CPU_FAMILY==SH */

#ifdef __cplusplus
}
#endif

#endif /* __INCsigCodesh */
