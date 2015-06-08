/* dbgLib.h - header file for dbgLib.c */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
02b,15aug97,cym  added SIMNT support.
02c,30apr98,dbt  removed unused DBG_TT.
02b,12jan98,dbt  modified for new debugger scheme.
02b,28nov96,cdp  added ARM support.
02a,26may94,yao  added PPC support.
01t,19mar95,dvs  removed #ifdef TRON - tron no longer supported.
01s,15dec93,hdn  added support for I80X86.
01r,02dec93,pad  Added Am29k family support.
01q,11aug93,gae  vxsim hppa.
01p,20jun93,gae  vxsim.
01r,22feb94,smb  changed typedef EVENT_TYPE to event_t (SPR #3064)
01q,14jan94,c_s  changed protoype of e ().
01p,10dec93,smb  added instrumentation for eventpoints
01o,13nov92,dnw  removed DBG_INFO typedef to taskLib.h (SPR #1768)
01n,22sep92,rrr  added support for c++
01m,25aug92,yao  added function prototypes for dbgBreakNotifyInstall(),
		 dbgStepQuiet(), bdTask().
01l,29jul92,wmd  place #pragma aligns around DBG_INFO for the i960.
01k,10jul92,yao  renamed DBG_STATE to DBG_INFO.  removed dbgMode, pDbgSave,
		 BOOLS resumeTask, sstepTask, sstepQuite, pInterruptBreak.
		 added dbgState to DBG_INFO.  added DBG_TASK_RESUME,
		 DBG_TASK_S_STEP, DBG_TASK_QUIET.  added dbgBrkExists ().
		 added BRK_SINGLE_STEP.  renamed BRK_SO to BRK_STEP_OVER.
01j,06jul92,yao  removed dbgLockCnt in DBG_STATE.
01i,04jul92,jcf  cleaned up.
01h,12mar92,yao  moved TRON related stuff to dbgTRONLib.h. added macros
		 LST_FIRST, LST_NEXT, INST_CMP.  added data structure HWBP,
		 BRKENTRY, DBG_STATE.  added ANSI prototype for architecture
		 interface routines.
01j,26may92,rrr  the tree shuffle
01i,23apr92,wmd  moved include of dbg960Lib.h after defines of structures.
01g,09jan92,jwt  converted CPU==SPARC to CPU_FAMILY==SPARC.
01f,16dec91,hdn  changed a type of bp->code, from INSTR to int, for G100.
01e,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed ASMLANGUAGE to _ASMLANGUAGE
		  -changed copyright notice
01d,11sep91,hdn  added things for redesigned dbgLib.c for TRON.
01c,10jun91,del  added pragma for gnu960 alignment.
01b,24mar91,del  added things for redesigned dbgLib.c only available on
		 i960ca.
01a,05oct90,shl created.
*/

#ifndef __INCdbgLibh
#define __INCdbgLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "regs.h"
#include "esf.h"

#if 	CPU_FAMILY == SIMNT
#include "arch/simnt/dbgSimntLib.h"
#endif 	/* CPU_FAMILY == SIMNT*/

#ifndef _ASMLANGUAGE
#include "lstLib.h"
#include "private/eventP.h"
#include "wdb/wdbDbgLib.h"

/* Macro's are used by routines executing at interrupt level (task switch),
 * so that fatal breakpoints are avoided.
 */

#define	INST_CMP(addr,inst,mask)  ((*(addr) & (mask)) == (inst))

#define ALL		0	/* breakpoint applies to all tasks */
#define LAST		0	/* continue applies to task that last broke */
#define MAX_ARGS	8	/* max args to task level call */

#ifndef	DBG_CRET
#define DBG_CRET	TRUE		/* default to support cret */
#endif	/* DBG_CRET */

#ifndef	DBG_INST_ALIGN
#define	DBG_INST_ALIGN	4		/* default to long word alignment */
#endif

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)
extern STATUS 	c (int taskNameOrId, INSTR * addr, INSTR * addr1);
extern STATUS 	s (int taskNameOrId, INSTR * addr, INSTR * addr1);

extern void 	dbgHelp (void);
extern STATUS 	dbgInit (void);
extern STATUS 	b (INSTR * addr, int taskNameOrId, int count, BOOL quiet);
extern STATUS 	e (INSTR * addr, event_t eventId, int taskNameOrId,
			FUNCPTR evtRtn, int arg);
#if	DBG_HARDWARE_BP
extern STATUS	bh (INSTR * addr, int access, int task, int count, BOOL quiet);
#endif	/* DBG_HARDWARE_BP */
extern STATUS 	bd (INSTR * addr, int taskNameOrId);
extern STATUS 	bdall (int taskNameOrId);
extern STATUS 	cret (int taskNameOrId);
extern STATUS 	so (int taskNameOrId);
extern void 	l (INSTR * addr, int count);
extern STATUS 	tt (int taskNameOrId);

extern void 	dbgPrintCall (INSTR * callAdrs, int funcAdrs, int nargs,
			      UINT32 * pArgs);
extern BOOL	dbgBrkExists (INSTR * addr, int task);
extern void	dbgBreakNotifyInstall (FUNCPTR breakpointRtn);
extern STATUS	dbgStepQuiet (int task);
extern STATUS	bdTask (int task);

#else

extern void 	dbgHelp ();
extern STATUS 	dbgInit ();
extern STATUS 	b ();
extern STATUS 	e ();
#if	DBG_HARDWARE_BP
extern STATUS 	bh ();
#endif	/* DBG_HARDWARE_BP */
extern STATUS 	bd ();
extern STATUS 	bdall ();
extern STATUS 	c ();
extern STATUS 	cret ();
extern STATUS 	s ();
extern STATUS 	so ();
extern void 	l ();
extern STATUS 	tt ();

extern void 	dbgPrintCall ();
extern BOOL	dbgBrkExists ();
extern void	dbgBreakNotifyInstall ();
extern STATUS	dbgStepQuiet ();
extern STATUS	bdTask ();

#endif	/* __STDC__ */

#endif	/* _ASMLANGUAGE */
#ifdef __cplusplus
}
#endif

#endif /* __INCdbgLibh */
