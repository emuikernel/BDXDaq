/* wdbDbgLib.h -  debugger library */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,25apr02,jhw  Added C++ support (SPR 76304).
01i,22oct01,dee  Merge from T2.1.0 ColdFire
01h,05sep01,pch  Avoid declaring functions which will not be defined
                 according to setting of DBG_NO_SINGLE_STEP
01g,25feb00,frf  Add SH support for T2
01f,14may98,dbt  fixed previous modification.
01e,13may98,dbt  added stddef.h include for offsetof definition.
01d,01may98,cym  added SIMNT support.
01c,29apr98,dbt  code cleanup.
01b,15apr98,kkk  added ARM support.
01a,07oct97,dbt  written
*/

#ifndef __INCwdbDbgLibh
#define __INCwdbDbgLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "vxWorks.h"

#if     CPU_FAMILY == MC680X0
#include "arch/mc68k/dbgMc68kLib.h"
#endif  /* CPU_FAMILY == MC680X0 */

#if     CPU_FAMILY == I960
#include "arch/i960/dbgI960Lib.h"
#endif  /* CPU_FAMILY == I960 */

#if     CPU_FAMILY == MIPS
#include "arch/mips/dbgMipsLib.h"
#endif  /* CPU_FAMILY == MIPS */

#if     CPU_FAMILY == PPC
#include "arch/ppc/dbgPpcLib.h"
#endif  /* CPU_FAMILY == PPC */

#if     CPU_FAMILY == SPARC
#include "arch/sparc/dbgSparcLib.h"
#endif  /* CPU_FAMILY == SPARC */

#if     CPU_FAMILY == SIMSPARCSOLARIS
#include "arch/simsolaris/dbgSimsolarisLib.h"
#endif  /* CPU_FAMILY == SIMSPARCSOLARIS */

#if     CPU_FAMILY == SIMHPPA
#include "arch/simhppa/dbgSimhppaLib.h"
#endif  /* CPU_FAMILY == SIMHPPA */

#if     CPU_FAMILY == SIMNT
#include "arch/simnt/dbgSimntLib.h"
#endif  /* CPU_FAMILY == SIMNT */

#if     CPU_FAMILY==I80X86
#include "arch/i86/dbgI86Lib.h"
#endif  /* CPU_FAMILY==I80X86 */

#if     CPU_FAMILY == ARM
#include "arch/arm/dbgArmLib.h"
#endif  /* CPU_FAMILY == ARM */

#if     CPU_FAMILY == COLDFIRE
#include "arch/coldfire/dbgColdfireLib.h"
#endif  /* CPU_FAMILY == COLDFIRE */

#if     CPU_FAMILY == SH
#include "arch/sh/dbgShLib.h"
#endif  /* CPU_FAMILY == SH */


#ifndef _ASMLANGUAGE
#include "wdb/dll.h"
#include "wdb/wdbRegs.h"
#include "esf.h"
#include "private/vmLibP.h"
#include "stddef.h"

/* defines */

#if     CPU_FAMILY==MIPS
#define WDB_CTX_LOAD(pRegs) _wdbDbgCtxLoad(pRegs)
#define WDB_CTX_SAVE(pRegs) _wdbDbgCtxSave(pRegs)
extern void _wdbDbgCtxLoad();
extern int _wdbDbgCtxSave();
#endif  /* CPU_FAMILY==MIPS */

#if     CPU_FAMILY==SIMHPPA
#define WDB_CTX_LOAD(pRegs) _wdbDbgCtxLoad(pRegs)
extern void _wdbDbgCtxLoad();
#endif  /* CPU_FAMILY==SIMHPPA */

#ifndef WDB_CTX_LOAD
#define WDB_CTX_LOAD(pRegs) _sigCtxLoad(pRegs)
#endif  /* WDB_CTX_LOAD */

#ifndef WDB_CTX_SAVE
#define WDB_CTX_SAVE(pRegs) _sigCtxSave(pRegs)
#endif  /* WDB_CTX_SAVE */

#define WDB_STEP        1
#define WDB_STEP_OVER   2
#define WDB_STEP_RANGE  4
#define WDB_STEPING     (WDB_STEP | WDB_STEP_OVER | WDB_STEP_RANGE)
#define WDB_CLEANME     8
#define WDB_QUEUED      16
#define WDB_TARGET      32
#define WDB_STEP_TARGET (WDB_STEP | WDB_TARGET)

#ifndef	BRK_HARDMASK		/* hardware breakpoint mask */
#define BRK_HARDMASK    0x0
#endif	/* BRK_HARDMASK */

#ifndef	BRK_HARDWARE		/* hardware breakpoint bit */
#define BRK_HARDWARE    0x0
#endif	/*  BRK_HARDWARE*/

#ifndef DBG_HARDWARE_BP		/* support of hardware breakpoint */
#define DBG_HARDWARE_BP 0
#endif	/* DBG_HARDWARE_BP */

#ifndef DBG_NO_SINGLE_STEP	/* trace mode (single step) support */
#define DBG_NO_SINGLE_STEP 0
#endif

#define BP_INSTALLED	0x80000000	/* bp is installed */
#define BP_HOST		0x40000000	/* host breakpoint */

/* defines used only by the target shell debugger */

#define BP_SO		0x08000000	/* step over (or cret) breakpoint */
#define BP_STEP		0x04000000	/* step breakpoint */
#define BP_EVENT	0x02000000	/* windview eventpoint */

#if CPU==SIMNT

extern void simTextLock(char *);
extern void simTextUnlock(char *);

#define TEXT_UNLOCK simTextUnlock
#define TEXT_LOCK simTextLock

#else /* CPU==SIMNT */
#define TEXT_UNLOCK(addr) \
	{\
	int pageSize = VM_PAGE_SIZE_GET(); \
	void * pageAddr = (void *) ((UINT) addr & ~(pageSize - 1)); \
	VM_TEXT_PAGE_PROTECT(pageAddr, FALSE); \
	}

#define TEXT_LOCK(addr) \
	{\
	int pageSize = VM_PAGE_SIZE_GET(); \
	void * pageAddr = (void *) ((UINT) addr & ~(pageSize - 1)); \
	VM_TEXT_PAGE_PROTECT(pageAddr, TRUE); \
	}

#endif /* CPU==SIMNT */

#define BP_SYS		-1
#define BP_ANY_TASK	0

/* data types */

typedef struct brkpt		/* breakpoint structure */
    {
    dll_t	bp_chain;	/* breakpoint chain */
    INSTR *	bp_addr;	/* breakpoint address */
    INSTR	bp_instr;	/* instruction at breakpoint address */
    int		bp_task;	/* breakpoint task */
    unsigned	bp_flags;	/* breakpoint flags */
    unsigned	bp_count;	/* breakpoint count */
    unsigned	bp_action;	/* action associated with breakpoint */
    void	(*bp_callRtn)();/* routine to call (if any) */
    int		bp_callArg;	/* called routine argument */
    } BRKPT;

#define STRUCT_BASE(s,m,p)	((s *)(void *)((char *)(p) - offsetof(s,m)))
#define BP_BASE(p)		(STRUCT_BASE(struct brkpt, bp_chain, (p)))

/* global variable */

extern dll_t bpList;
extern dll_t bpFreeList;

/* function declarations */

#ifdef  __STDC__
extern void	wdbDbgArchInit (void);
extern void	wdbDbgBpListInit (void);
extern STATUS	wdbDbgBpFind (INSTR * pc, int context);
extern void	wdbDbgBpRemoveAll (void);
extern STATUS	wdbDbgBpRemove (BRKPT *pBp);
extern BRKPT *	wdbDbgBpAdd (INSTR * addr, int task, unsigned flags, 
				unsigned action, unsigned count, 
				FUNCPTR callRtn, int callArg);
extern STATUS	wdbDbgBpGet (INSTR * pc, int context, int type, BRKPT * pBp);
extern int	wdbDbgTraceModeSet (REG_SET * pRegs);
extern void	wdbDbgTraceModeClear (REG_SET * pRegs, int traceData);
#if	DBG_NO_SINGLE_STEP
extern INSTR *	wdbDbgGetNpc (REG_SET * pRegs);
extern void	wdbDbgTrap (INSTR * addr, REG_SET * pRegisters, void * pInfo, 
					void * pDbgRegSet, BOOL hardware);
#else	/* DBG_NO_SINGLE_STEP */
extern void	wdbDbgBreakpoint (void * pInfo, REG_SET * pRegisters, 
					void * pDbgRegSet, BOOL hardware);
extern void	wdbDbgTrace (void * pInfo, REG_SET * pRegisters);
#endif	/* DBG_NO_SINGLE_STEP */
#if 	DBG_HARDWARE_BP
extern void	wdbDbgRegsSet (DBG_REGS * pDbgReg);
extern void	wdbDbgRegsClear (void);
extern STATUS	wdbDbgHwBpSet (DBG_REGS * pDbgRegs, UINT32 type, UINT32 addr);
extern STATUS	wdbDbgHwBpFind (DBG_REGS *  pDbgRegs, UINT32 * pType, 
					UINT32 * pAddr);
extern STATUS	wdbDbgHwAddrCheck (UINT32 addr, UINT32 type, 
					FUNCPTR memProbeRtn);
#endif 	/* DBG_HARDWARE_BP */
extern void	dbgTaskBpTrap (int level, INSTR * addr, void * pInfo, 
					REG_SET * pRegisters,
					void * pDbgRegs, BOOL hardware);
extern void	dbgTaskBpTrace (int level, void * pInfo,
					REG_SET * pRegisters);
extern void	dbgTaskBpBreakpoint (int level, void * pInfo,
					REG_SET * pRegisters, 
					void * pDbgRegs, BOOL hardware);
extern STATUS	dbgTaskCont (UINT32 contextId);
extern STATUS	dbgTaskStep (UINT32 contextId, UINT32 startAddr, 
					UINT32 endAddr);
extern void	dbgTaskBpHooksInstall (void);
extern void	usrBreakpointSet (INSTR * addr, INSTR value);
#else   /* __STDC__ */
extern void	wdbDbgArchInit();
extern void	wdbDbgBpListInit ();
extern STATUS	wdbDbgBpFind ();
extern void	wdbDbgBpRemove ();
extern STATUS	wdbDbgBpRemove ();
extern void	wdbDbgBpAdd ();
extern STATUS	wdbDbgBpGet ();
extern int	wdbDbgTraceModeSet ();
extern void	wdbDbgTraceModeClear ();
#if	DBG_NO_SINGLE_STEP
extern INSTR *	wdbDbgGetNpc ();
extern void	wdbDbgTrap ();
#else	/* DBG_NO_SINGLE_STEP */
extern void	wdbDbgBreakpoint ();
extern void	wdbDbgTrace ();
#endif	/* DBG_NO_SINGLE_STEP */
#if 	DBG_HARDWARE_BP
extern void	wdbDbgRegsSet ();
extern void	wdbDbgRegsClear ();
exern STATUS	wdbDbgBpSet ();
extern STATUS	wdbDbgHwBpFind ();
extern STATUS	wdbDbgHwAddrCheck ();
#endif 	/* DBG_HARDWARE_BP */
extern void	dbgTaskBpTrap ();
extern void	dbgTaskBpTrace ();
extern void	dbgTaskBpBreakpoint ();
extern UINT32	dbgTaskCont ();
extern UINT32	dbgTaskStep ();
extern void	dbgTaskBpHooksInstall ();
extern void	usrBreakpointSet ();
#endif  /* __STDC__ */

#endif 	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* __INCwdbDbgLibh */
