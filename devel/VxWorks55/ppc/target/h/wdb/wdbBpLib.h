/* wdbBpLib.h - wdb break point library */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,25apr02,jhw  Added C++ support (SPR 76304).
01i,21apr98,dbt  removed useless defines for ARM.
01h,17apr97,cdp added Thumb (ARM7TDMI_T) support.
01g,28nov96,cdp added ARM support.
01h,04feb98,dbt  moved wdbSysBpLibInit() and wdbTaskBpLibInit() to wdbLib.h
01g,12jan98,dbt  modified for new debugger scheme
01f,22jul96,jmb  merged mem's (ease) patch for HPSIM.
01e,20may96,ms   imported prototypes and macros from wdb[Task]BpLib.h.
01d,25feb96,tam  added single step support for PPC403.
01d,04apr96,ism  added simsolaris support
01c,23jan96,tpr  added PowerPC support.
01c,28nov95,mem  added MIPS support, added macros WDB_CTX_{LOAD,SAVE}.
01b,08jun95,ms   change CPU==SPARC to CPU_FAMILY==SPARC
01a,20nov94,rrr  written.
*/

#ifndef __INCwdbBpLibh
#define __INCwdbBpLibh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

/* includes */

#include "vxWorks.h"
#include "wdb/wdbRegs.h"
#include "wdb/wdbDbgLib.h"

/* function declarations */

#ifdef  __STDC__
extern void	wdbBpInstall (void);
extern UINT32	(*_wdbTaskBpAdd) (WDB_EVTPT_ADD_DESC * pEv);
extern UINT32	(*_wdbTaskStep) (UINT32 contextId, TGT_ADDR_T startAddr,
					    TGT_ADDR_T endAddr);
extern UINT32	(*_wdbTaskCont) (UINT32 contextId);
extern void	(*_wdbTaskBpTrap) (int level, INSTR * addr, void * pInfo, 
				    	    WDB_IU_REGS * pRegisters, 
					    void * pDbgRegs, 
				    BOOL hardware);
extern void	(*_wdbTaskBpBreakpoint) (int level, void * pInfo, 
					    WDB_IU_REGS * pRegisters, 
					    void * pDbgRegs, BOOL hardware);
extern void	(*_wdbTaskBpTrace) (int level, void * pInfo, 
					    WDB_IU_REGS *pRegisters);
#else   /* __STDC__ */
extern	void	wdbBpInstall ();
extern	UINT32	(*_wdbTaskBpAdd) ();
extern	UINT32	(*_wdbTaskStep) ();
extern	UINT32	(*_wdbTaskCont) ();
extern	void	(*_wdbTaskBpTrap) ();
extern	void	(*_wdbTaskBpBreakpoint) ();
extern	void	(*_wdbTaskBpTrace) ();
#endif  /* __STDC__ */
#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* __INCwdbBpLibh */
