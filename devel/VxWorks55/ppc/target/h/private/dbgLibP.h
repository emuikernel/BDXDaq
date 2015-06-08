/* dbgLibP.h - private debugging facility header */

/* Copyright 1984-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,18jan99,elg  Authorize breakpoints in branch delay slot (SPR 24356).
01g,13jan98,dbt  modified for new breakpoint scheme
01f,20aug93,dvs  removed declaration of dsmNbytes(), added include dsmLib.h 
		 (SPR #2266).
01e,01oct92,yao  added to pass pEsf, pRegSet to _dbgStepAdd().
01d,22sep92,rrr  added support for c++
01c,21jul92,yao  added declaration of trcDefaultArgs.  changed IMPORT
		 to extern.
01b,06jul92,yao  made user uncallable globals started with '_'.
01a,16jun92,yao  written based on mc68k dbgLib.c ver08f.
*/

#ifndef __INCdbgLibPh
#define __INCdbgLibPh

#ifdef __cplusplus
extern "C" {
#endif

#include "wdb/wdbDbgLib.h"
#include "dbgLib.h"
#include "dsmLib.h" 

/* externals */

extern int    	shellTaskId;
extern char * 	_archHelp_msg;		/* architecture specific help message */
extern int    	trcDefaultArgs;		/* default number of args to print */

extern FUNCPTR 	_dbgDsmInstRtn; 		/* disassembler routine */

#if defined(__STDC__) || defined(__cplusplus)

/* architecture interface routines */

extern void	_dbgArchInit (void); 
extern STATUS	_dbgArchAddrCheck (INSTR * addr, int type);
extern INSTR *	_dbgRetAdrsGet (REG_SET * pRegSet);
extern BOOL	_dbgFuncCallCheck (INSTR * addr);
extern int	_dbgInstSizeGet (INSTR * brkInst);
extern INSTR *	_dbgTaskPCGet (int tid);
extern void	_dbgBrkDisplayHard (BRKPT * pBp);
extern void	_dbgTaskPCSet (int tid, INSTR * pc, INSTR * npc);

#else	/* __STDC__ */

/* architecture interface routines */

extern void	_dbgArchInit (); 
extern STATUS	_dbgArchAddrCheck ();
extern INSTR *	_dbgRetAdrsGet ();
extern BOOL	_dbgFuncCallCheck ();
extern int	_dbgInstSizeGet ();
extern INSTR *	_dbgTaskPCGet ();
extern void	_dbgBrkDisplayHard ();
extern void	_dbgTaskPCSet ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCdbgLibPh */
