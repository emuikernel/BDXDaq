/* esf.h - PowerPC exception stack frames */

/* Copyright 1984-1995 Wind River Systems, Inc. */
/*
modification history
--------------------
01i,22aug01,pch  Replace "sizeof(UINT32)" with SIZ_UNIT32 because the
                 assembler barfs on the former.
01h,15aug01,pch  Add PPC440, remove an old "#if 0" block
01g,25oct00,s_m  renamed PPC405 cpu types
01f,31aug00,sm   added cases for PPC405 and PPC405F
01e,04mar96,tam  added support for PPC403. 
01d,20apr95,yao  add padding word that esf stack size are same
		 for all cpu variations.
01c,06oct94,yao  changed to put sp on the top of the stack.
01b,29sep94,yao  fixed register offset on the exception stack in 
		 correct order.
01a,07jul94,yao  written.
*/

#ifndef __INCesfPpch
#define __INCesfPpch

#ifdef __cplusplus
extern "C" {
#endif

#include "regs.h"

#ifndef	_ASMLANGUAGE

typedef	struct 
    {
    _RType	spCopy;		/* unused - redundant copy of regSet.gpr[1] */ 
    UINT32	vecOffset;	/* vector offset (need padding for 64 bits) */
    UINT32	_errno;		/* error number */
# if     ((CPU == PPC403) || (CPU == PPC405) || (CPU == PPC405F))
    _RType      dear;		/* data exception address register */
    UINT32      bear;           /* bus error address register */
    UINT32      besr;           /* bus error syndrome register */
# elif	(CPU == PPC440)
    _RType      dear;		/* data exception address register */
    UINT32      pad3;           /* bus error address register */
    UINT32      pad4;           /* bus error syndrome register */
# else	/* CPU == PPC4xx */
    _RType      dar;		/* data address register */
    UINT32      dsisr;		/* data storage interrupt status register */
    UINT32      fpcsr;		/* floating-point control and status register */
# endif	/* CPU == PPC4xx */
    REG_SET	regSet;		/* register set */
# if 	(CPU == PPC405F)
    UINT32      fpcsr;		/* floating-point control and status register */
# else	/* CPU == PPC405F */
    UINT32	pad0;
# endif	/* CPU == PPC405F */
    UINT32	pad1;
    UINT32	pad2;
    } ESFPPC;

#endif	/* _ASMLANGUAGE */

#define _PPC_ESF_STK_SIZE 192	

/* register offset on exception stack */

/* Assembler barfs on sizeof(), so do it this way instead */
#define	SIZ_UNIT32	4	/* sizeof(UINT32) */

#define _PPC_ESF_SIZE		(_PPC_REG_SIZE + 16 + _PPC_REG_SET_SIZE)
#define _PPC_ESF_VEC_OFF	_PPC_REG_SIZE
#define _PPC_ESF_ERRNO		(_PPC_ESF_VEC_OFF + SIZ_UNIT32)
#if     ((CPU == PPC403) || (CPU == PPC405))
#define _PPC_ESF_DEAR		(_PPC_ESF_ERRNO + SIZ_UNIT32)
#define _PPC_ESF_BEAR		(_PPC_ESF_DEAR + _PPC_REG_SIZE)
#define _PPC_ESF_BESR		(_PPC_ESF_BEAR + SIZ_UNIT32)
#define _PPC_ESF_REG_BASE	(_PPC_ESF_BESR + SIZ_UNIT32)
#elif	(CPU == PPC405F)
#define _PPC_ESF_DEAR		(_PPC_ESF_ERRNO + SIZ_UNIT32)
#define _PPC_ESF_BEAR		(_PPC_ESF_DEAR + _PPC_REG_SIZE)
#define _PPC_ESF_BESR		(_PPC_ESF_BEAR + SIZ_UNIT32)
#define _PPC_ESF_REG_BASE	(_PPC_ESF_BESR + SIZ_UNIT32)
#define _PPC_ESF_FPCSR		(_PPC_ESF_REG_BASE + _PPC_REG_SET_SIZE)
#elif	(CPU == PPC440)
#define _PPC_ESF_DEAR		(_PPC_ESF_ERRNO + SIZ_UNIT32)
#define _PPC_ESF_PAD3		(_PPC_ESF_DEAR + _PPC_REG_SIZE)
#define _PPC_ESF_PAD4		(_PPC_ESF_PAD3 + SIZ_UNIT32)
#define _PPC_ESF_REG_BASE	(_PPC_ESF_PAD4 + SIZ_UNIT32)
#else	/* CPU == PPC4xx */
#define _PPC_ESF_DAR		(_PPC_ESF_ERRNO + SIZ_UNIT32)
#define _PPC_ESF_DSISR		(_PPC_ESF_DAR + _PPC_REG_SIZE)
#define _PPC_ESF_FPCSR		(_PPC_ESF_DSISR + SIZ_UNIT32)
#define _PPC_ESF_REG_BASE	(_PPC_ESF_FPCSR + SIZ_UNIT32)
#endif	/* CPU == PPC4xx */
#define _PPC_ESF_R(n)		(_PPC_ESF_REG_BASE + (n) * _PPC_REG_SIZE)

#define _PPC_ESF_MSR		_PPC_ESF_R(GREG_NUM)
#define _PPC_ESF_LR		_PPC_ESF_R(GREG_NUM + 1)
#define _PPC_ESF_CTR		_PPC_ESF_R(GREG_NUM + 2)
#define _PPC_ESF_PC		_PPC_ESF_R(GREG_NUM + 3)
#define _PPC_ESF_CR		_PPC_ESF_R(GREG_NUM + 4)
#define _PPC_ESF_XER		(_PPC_ESF_CR + 4)

#if	(CPU == PPC601)
#define _PPC_ESF_MQ		(_PPC_ESF_XER + 4)	
#endif	/* (CPU == PPC601) */

#define _PPC_ESF_R0		_PPC_ESF_R(0)
#define _PPC_ESF_R1		_PPC_ESF_R(1)
#define _PPC_ESF_R2		_PPC_ESF_R(2)
#define _PPC_ESF_R3		_PPC_ESF_R(3)
#define _PPC_ESF_R4		_PPC_ESF_R(4)
#define _PPC_ESF_R5		_PPC_ESF_R(5)
#define _PPC_ESF_R6		_PPC_ESF_R(6)
#define _PPC_ESF_R7		_PPC_ESF_R(7)
#define _PPC_ESF_R8		_PPC_ESF_R(8)
#define _PPC_ESF_R9		_PPC_ESF_R(9)
#define _PPC_ESF_R10		_PPC_ESF_R(10)
#define _PPC_ESF_R11		_PPC_ESF_R(11)
#define _PPC_ESF_R12		_PPC_ESF_R(12)
#define _PPC_ESF_R13		_PPC_ESF_R(13)
#define _PPC_ESF_R14		_PPC_ESF_R(14)
#define _PPC_ESF_R15		_PPC_ESF_R(15)
#define _PPC_ESF_R16		_PPC_ESF_R(16)
#define _PPC_ESF_R17		_PPC_ESF_R(17)
#define _PPC_ESF_R18		_PPC_ESF_R(18)
#define _PPC_ESF_R19		_PPC_ESF_R(19)
#define _PPC_ESF_R20		_PPC_ESF_R(20)
#define _PPC_ESF_R21		_PPC_ESF_R(21)
#define _PPC_ESF_R22		_PPC_ESF_R(22)
#define _PPC_ESF_R23		_PPC_ESF_R(23)
#define _PPC_ESF_R24		_PPC_ESF_R(24)
#define _PPC_ESF_R25		_PPC_ESF_R(25)
#define _PPC_ESF_R26		_PPC_ESF_R(26)
#define _PPC_ESF_R27		_PPC_ESF_R(27)
#define _PPC_ESF_R28		_PPC_ESF_R(28)
#define _PPC_ESF_R29		_PPC_ESF_R(29)
#define _PPC_ESF_R30		_PPC_ESF_R(30)
#define _PPC_ESF_R31		_PPC_ESF_R(31)

#define	_PPC_ESF_P0		_PPC_ESF_R3
#define	_PPC_ESF_P1		_PPC_ESF_R4
#define	_PPC_ESF_P2		_PPC_ESF_R5
#define	_PPC_ESF_P3		_PPC_ESF_R6
#define	_PPC_ESF_P4		_PPC_ESF_R7
#define	_PPC_ESF_P5		_PPC_ESF_R8
#define	_PPC_ESF_P6		_PPC_ESF_R9
#define	_PPC_ESF_P7		_PPC_ESF_R10

#define	_PPC_ESF_T0		_PPC_ESF_R14
#define	_PPC_ESF_T1		_PPC_ESF_R15
#define	_PPC_ESF_T2		_PPC_ESF_R16
#define	_PPC_ESF_T3		_PPC_ESF_R17
#define	_PPC_ESF_T4		_PPC_ESF_R18
#define	_PPC_ESF_T5		_PPC_ESF_R19
#define	_PPC_ESF_T6		_PPC_ESF_R20
#define	_PPC_ESF_T7		_PPC_ESF_R21
#define	_PPC_ESF_T8		_PPC_ESF_R22
#define	_PPC_ESF_T9		_PPC_ESF_R23
#define	_PPC_ESF_T10		_PPC_ESF_R24
#define	_PPC_ESF_T11		_PPC_ESF_R25
#define	_PPC_ESF_T12		_PPC_ESF_R26
#define	_PPC_ESF_T13		_PPC_ESF_R27
#define	_PPC_ESF_T14		_PPC_ESF_R28
#define	_PPC_ESF_T15		_PPC_ESF_R29
#define	_PPC_ESF_T16		_PPC_ESF_R30
#define	_PPC_ESF_T17		_PPC_ESF_R31

#define _PPC_ESF_SP		_PPC_ESF_R1	

#ifdef	_AIX_TOOL
#define _PPC_ESF_RTOC		_PPC_ESF_R(2)
#endif	/* _AIX_TOOL */

#ifdef __cplusplus
}
#endif

#endif /* __INCesfPpch */
