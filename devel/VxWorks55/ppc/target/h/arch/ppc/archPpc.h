/* archPpc.h - PowerPC specific header */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01n,13mar02,sn   SPR 73723 - define supported toolchains
01m,25sep01,yvp  SPR62760 fix: Added default text segment alignment.
01l,14aug01,pch  Add PPC440 support
01k,25oct00,s_m  renamed PPC405 cpu types
01j,12jun00,alp  Added PPC405 support
01i,12mar99,zl   added PPC509 and PPC555 support.
01h,18aug98,tpr  added PowerPC EC 603 support.
01g,10feb97,tam  added FPSCR bit definition macros.
01f,08oct96,tam  added MSR bit definition macros.
01e,11apr96,tpr  added PPC505, PPC602 and PPC860.
01d,14feb96,tpr  added PPC604.
01c,21mar95,caf  moved _STACK_ALIGN_SIZE to toolPpc.h.
01b,07nov94,yao  changed PPC403GA to PPC403.
01a,17mar94,yao  written.
*/

#ifndef __INCarchPpch
#define __INCarchPpch

#ifdef __cplusplus
extern "C" {
#endif

#define _ARCH_SUPPORTS_GCC
#define _ARCH_SUPPORTS_DCC
	
/* upper Machine State Register (MSR) mask */

#define _PPC_MSR_SF_U		0x8000	/* sixty-four bit mode (not 
					 * implemented for 32-bit machine) */
#define _PPC_MSR_POW_U		0x0004	/* power managemnet enable */ 
#define _PPC_MSR_ILE_U		0x0001	/* little endian mode */

/* lower Machine State Register (MSR) mask */

#define	_PPC_MSR_EE		0x8000	/* external interrupt enable */
#define _PPC_MSR_PR		0x4000	/* privilege level */
#define _PPC_MSR_FP		0x2000	/* floating-point available */
#define _PPC_MSR_ME		0x1000	/* machine check enable  */
#define _PPC_MSR_FE0		0x0800	/* floating-point exception mode 0 */
#define _PPC_MSR_SE		0x0400	/* single-step trace enable */
#define _PPC_MSR_BE		0x0200	/* branch trace enable */
#define _PPC_MSR_FE1		0x0100	/* floating-point exception mode 1 */
#define _PPC_MSR_IP		0x0040	/* exception prefix */
#define _PPC_MSR_IR		0x0020	/* instruction address translation */
#define _PPC_MSR_DR		0x0010	/* data address translation */
#define _PPC_MSR_RI		0x0002	/* recoverable interrupt */
#define _PPC_MSR_LE		0x0001	/* little-endian mode */

#define _PPC_MSR_POWERUP	0x0040	/* state of MSR at powerup */

/* MSR bit definitions common to all PPC arch. */

#define _PPC_MSR_BIT_EE		16	/* MSR Ext. Intr. Enable bit - EE */
#define _PPC_MSR_BIT_PR		17	/* MSR Privilege Level bit - PR */
#define _PPC_MSR_BIT_ME		19	/* MSR Machine Check Enable bit - ME */
#define _PPC_MSR_BIT_LE		31	/* MSR Little Endian mode bit - LE */

/* FPSCR bit definitions (valid for the PPC60X familly) */

#define _PPC_FPSCR_FX           0x80000000 /* FP exception summary */
#define _PPC_FPSCR_FEX          0x40000000 /* FP enabled exception summary */
#define _PPC_FPSCR_VX           0x20000000 /* FP invalid exception summary */
#define _PPC_FPSCR_OX           0x10000000 /* FP overflow exception */
#define _PPC_FPSCR_UX           0x08000000 /* FP underflow exception */
#define _PPC_FPSCR_ZX           0x04000000 /* FP divide by zero exception */
#define _PPC_FPSCR_XX           0x02000000 /* FP inexact exception */
#define _PPC_FPSCR_VXSNAN       0x01000000 /* FP invalid exception for SNAN */
#define _PPC_FPSCR_VXISI        0x00800000 /* FP invalid exc. for INF-INF */
#define _PPC_FPSCR_VXIDI        0x00400000 /* FP invalid exc. for INF/INF */
#define _PPC_FPSCR_VXZDZ        0x00200000 /* FP invalid exc. for 0/0 */
#define _PPC_FPSCR_VXIMZ        0x00100000 /* FP invalid exc. for INF*0 */
#define _PPC_FPSCR_VXVC         0x00080000 /* FP inval. exc. for invalid comp.*/
#define _PPC_FPSCR_FR           0x00040000 /* FP fraction rounded */
#define _PPC_FPSCR_FI           0x00020000 /* FP fraction inexact */
#define _PPC_FPSCR_FPRF         0x0001F000 /* FP result flags */
#define _PPC_FPSCR_VXSOFT       0x00000400 /* FP inval. exc. for soft. request*/
#define _PPC_FPSCR_VXSQRT       0x00000200 /* FP inval. exc. for sqrt */
#define _PPC_FPSCR_VXCVI        0x00000100 /* FP inval. exc. for int convert */
#define _PPC_FPSCR_VE           0x00000080 /* FP invalid exc. enable */
#define _PPC_FPSCR_OE           0x00000040 /* FP overflow exc. enable */
#define _PPC_FPSCR_UE           0x00000020 /* FP underflow exc. enable */
#define _PPC_FPSCR_ZE           0x00000010 /* FP divide by zero exc. enable */
#define _PPC_FPSCR_XE           0x00000008 /* FP iinexact exc. enable */
#define _PPC_FPSCR_NI           0x00000004 /* FP non-IEEE mode enable */
#define _PPC_FPSCR_RN(n)        (n)        /* FP rounding control value */

#define _PPC_FPSCR_RN_MSK       0x00000003 /* FP rounding control bits mask */
#define _PPC_FPSCR_EXC_MASK     0x1ff80700 /* FP exception status bits mask */
#define _PPC_FPSCR_CTRL_MASK    0x000000ff /* FP exception control bits mask */

/* FPSCR init value for tasks spawned with VX_FP_TASK (PPC60X only) */

#define _PPC_FPSCR_INIT         (_PPC_FPSCR_OE | _PPC_FPSCR_UE | _PPC_FPSCR_ZE \
				 | _PPC_FPSCR_RN(0))


#ifndef _ASMLANGUAGE
typedef	unsigned int	_RType;		/* default register type */
#endif 	/* _ASMLANGUAGE */

#define _ARCH_MULTIPLE_CACHELIB	FALSE

#define	_DYNAMIC_BUS_SIZING	FALSE	/* require alignment for swap */

#define	_PPC_REG_SIZE		4	/* default register size */

#define _PPC_TEXT_SEG_ALIGN     4       /* 4 byte text segment alignment */

#if 	(CPU == PPC403)
#include "ppc403.h"
#endif	/* CPU == PPC403) */

#if 	((CPU == PPC405) || (CPU == PPC405F))
#include "ppc405.h"
#endif	/* CPU == PPC405) || (CPU == PPC405F) */

#if 	(CPU == PPC440)
#include "ppc440.h"
#endif	/* CPU == PPC440 */

#if 	(CPU == PPC509)
#include "ppc509.h"
#endif	/* CPU == PPC509) */

#if 	(CPU == PPC555)
#include "ppc555.h"
#endif	/* CPU == PPC555) */

#if 	(CPU == PPC601)
#include "ppc601.h"
#endif	/* CPU == PPC601) */

#if 	(CPU == PPC602)
#include "ppc602.h"
#endif	/* CPU == PPC602) */

#if 	((CPU == PPC603) || (CPU == PPCEC603))
#include "ppc603.h"
#endif	/* ((CPU == PPC603) || (CPU == PPCEC603)) */

#if 	(CPU == PPC604)
#include "ppc604.h"
#endif	/* CPU == PPC604) */

#if 	(CPU == PPC860)
#include "ppc860.h"
#endif	/* CPU == PPC860) */

#include "toolPpc.h"

#ifdef __cplusplus
}
#endif

#endif /* __INCarchPpch */
