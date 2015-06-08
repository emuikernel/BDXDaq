/* wdbLibP.h - private header file for remote debug agent */

/* Copyright 1994-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,25apr02,jhw  Added C++ support (SPR 76304).
01f,14sep01,jhw  Added wdbExternRegsGet() and wdbExternRegsSet() function
                 prototypes.
		 Removed __STDC__ precompiler conditional.
01e,11jun98,dbt  removed wdbTgtHasFpp() prototype.
01d,03jun95,ms	 added prototype for wdbTargetIsConnected()
01c,05apr95,ms   new data types.
01b,06feb95,ms	 removes wdbRpcLib specific things.
01a,20sep94,ms   written.
*/

#ifndef __INCwdbLibPh
#define __INCwdbLibPh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "wdb/wdb.h"

/* defines */

#define WDB_STATE_EXTERN_RUNNING        1

/* agent variables */

extern uint_t   wdbCommMtu;
extern uint_t	wdbTgtMemBase;
extern uint_t	wdbTgtMemSize;
extern uint_t	wdbTgtNumMemRegions;
extern WDB_MEM_REGION * pWdbTgtMemRegions;

/* function prototypes */

extern void	wdbInfoGet	  (WDB_AGENT_INFO * pInfo);
extern void	wdbNotifyHost	  (void);
extern BOOL	wdbTargetIsConnected (void);

extern STATUS	wdbModeSet	  (int newMode);
extern BOOL	wdbIsNowExternal  (void);
extern BOOL	wdbRunsExternal   (void);
extern BOOL	wdbIsNowTasking   (void);
extern BOOL	wdbRunsTasking	  (void);

extern void	wdbSuspendSystem     (
				     /*
				      WDB_IU_REGS *pRegs,
				      void (*callBack)(),
				      int arg
				     */
				     );
extern void	wdbSuspendSystemHere (void (*callBack)(), int arg);
extern void	wdbResumeSystem      (void);
extern STATUS	wdbExternInfRegsGet  (WDB_REG_SET_TYPE type, char **ppRegs);
extern void     wdbExternEnterHook   (void);
extern void     wdbExternExitHook    (void);
extern STATUS	wdbExternRegsGet     (WDB_REG_SET_TYPE type, char **ppRegs);
extern STATUS	wdbExternRegsSet     (WDB_REG_SET_TYPE type, char *pRegs);

extern UINT32	wdbCtxCreate	     (WDB_CTX_CREATE_DESC *, UINT32 *);
extern UINT32	wdbCtxResume	     (WDB_CTX *);
extern void	wdbCtxExitNotifyHook (WDB_CTX, UINT32, UINT32);

#ifdef __cplusplus
}
#endif

#endif /* __INCwdbLibPh */
