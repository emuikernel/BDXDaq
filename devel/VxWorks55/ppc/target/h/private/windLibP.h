/* windLibP.h - VxWorks kernel header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,22may02,jgn  move vxTicks & vxAbsTicks here - SPR #70255
01c,22sep92,rrr  added support for c++
01b,19jul92,pme	 added windReadyQPut() and windReadyQRemove().
		 windPendQPut(), windDelete(), windPendQRemove() return STATUS.
01a,04jul92,jcf	 extracted from wind.h.
*/

#ifndef __INCwindLibPh
#define __INCwindLibPh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "vwModNum.h"
#include "qLib.h"
#include "taskLib.h"
#include "semLib.h"
#include "private/wdLibP.h"

/* variable declarations */

extern BOOL   kernelState;		/* mutex to enter kernel state */
extern BOOL   kernelIsIdle;		/* boolean reflecting idle state */

extern ULONG    vxTicks;                /* relative time counter */
extern UINT64	vxAbsTicks;		/* absolute time counter */

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void	windSpawn (WIND_TCB *pTcb);
extern STATUS	windDelete (WIND_TCB *pTcb);
extern void	windSuspend (WIND_TCB *pTcb);
extern void	windResume (WIND_TCB *pTcb);
extern void	windPriNormalSet (WIND_TCB *pTcb, UINT priNormal);
extern void	windPrioritySet (WIND_TCB *pTcb, UINT priority);
extern void	windSemDelete (SEM_ID semId);
extern void	windTickAnnounce (void);
extern STATUS	windDelay (int timeout);
extern STATUS	windUndelay (WIND_TCB *pTcb);
extern STATUS	windWdStart (WDOG *wdId, int timeout);
extern void	windWdCancel (WDOG *wdId);
extern void	windPendQGet (Q_HEAD *pQHead);
extern void	windReadyQPut (WIND_TCB * pTcb);
extern void	windReadyQRemove (Q_HEAD *pQHead, int timeout);
extern void	windPendQFlush (Q_HEAD *pQHead);
extern STATUS	windPendQPut (Q_HEAD *pQHead, int timeout);
extern STATUS	windPendQRemove (WIND_TCB *pTcb);
extern void	windPendQTerminate (Q_HEAD *pQHead);
extern STATUS	windExit ();
extern void	windIntStackSet (char *pBotStack);
extern void	vxTaskEntry ();
extern void	intEnt ();
extern void	intExit ();
extern UINT64	tick64Get ();
extern void	tick64Set (UINT64 ticks);

#else

extern void	windSpawn ();
extern STATUS	windDelete ();
extern void	windSuspend ();
extern void	windResume ();
extern void	windPriNormalSet ();
extern void	windPrioritySet ();
extern void	windSemDelete ();
extern void	windTickAnnounce ();
extern STATUS	windDelay ();
extern STATUS	windUndelay ();
extern STATUS	windWdStart ();
extern void	windWdCancel ();
extern void	windPendQGet ();
extern void	windReadyQPut ();
extern void	windReadyQRemove ();
extern void	windPendQFlush ();
extern STATUS	windPendQPut ();
extern STATUS	windPendQRemove ();
extern void	windPendQTerminate ();
extern STATUS	windExit ();
extern void	windIntStackSet ();
extern void	vxTaskEntry ();
extern void	intEnt ();
extern void	intExit ();
extern UINT64	tick64Get ();
extern void	tick64Set ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwindLibPh */
