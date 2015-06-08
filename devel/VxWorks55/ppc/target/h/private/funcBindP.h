/* funcBindP.h - private function binding header */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
02h,20mar03,wap  added m2If64BitCounters (SPR #86776)
02g,13jan03,rae  Merged from velocecp branch
02f,21nov02,pch  SPR 84107: add _func_altivecTaskRegsGet and _func_altivecProbe
02e,26mar02,pai  added _func_sseTaskRegsShow (SPR 74103).
02d,09nov01,jn   added internal API for symLib
02c,29oct01,gls  added pthread support
02b,26oct01,brk  added _func_selPtyAdd & _func_selPtyDelete (SPR 65498)
02a,21sep01,aeg  added _func_selWakeupListTerm.
01z,16mar01,pcs  ADDED _func_altivecTaskRegsShow
01z,28feb00,frf  Add SH support for T2
01u,08aug98,kab  added _func_dspRegsListHook, _func_dspMregsHook.
01t,23jul98,mem  added _func_dspTaskRegsShow
01y,10aug98,pr   added WindView function pointers for i960
01x,15apr98,cth  added definition of _func_evtLogReserveTaskName
01w,08apr98,pr   added _func_evtLogT0_noInt. Set evtAction as UINT32
01v,13dec97,pr   moved some variables from eventP.h
		 removed some windview 1.0 variables
01u,13nov97,cth  removed evtBuf and scrPad references for WV2.0
01t,24jun97,pr   added evtInstMode
01t,09oct97,ms   added _func_ioTaskStdSet
01s,21feb97,tam  added _dbgDsmInstRtn
01r,08jul96,pr   added _func_evtLogT1_noTS
01q,12may95,p_m  added _func_printErr, _func_symFindByValue, _func_spy*
                       _func_taskCreateHookAdd and _func_taskDeleteHookAdd.
01p,24jan94,smb  added function pointers for windview portable kernel.
01o,10dec93,smb  added function pointers for windview.
01n,05sep93,jcf  added _remCurId[SG]et.
01m,20aug93,jmm  added _bdall
01l,22jul93,jmm  added _netLsByName
01k,13feb93,kdl  added _procNumWasSet.
01j,13nov92,jcf  added _func_logMsg.
01i,22sep92,rrr  added support for c++
01h,20sep92,kdl  added _func_ftpLs, ftpErrorSuppress.
01g,31aug92,rrr  added _func_sigprocmask
01f,23aug92,jcf  added _func_sel*, _func_excJobAdd,_func_memalign,_func_valloc
01e,02aug92,jcf  added/changed _exc*.
01d,29jul92,jcf  added _func_fclose
01c,29jul92,rrr  added _func_sigExcKill, _func_sigTimeoutRecalc,
                 _func_excEsfCrack and _func_excSuspend.
01b,19jul92,pme  added _func_smObjObjShow.
01a,04jul92,jcf  written
*/

#ifndef __INCfuncBindPh
#define __INCfuncBindPh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "semLib.h"

/* variable declarations */

extern FUNCPTR     _func_ioTaskStdSet;
extern FUNCPTR     _func_bdall;
extern FUNCPTR     _func_dspTaskRegsShow;
IMPORT VOIDFUNCPTR _func_dspRegsListHook;	/* arch dependent DSP regs list */
IMPORT FUNCPTR	   _func_dspMregsHook;		/* arch dependent mRegs() hook */
extern FUNCPTR     _func_excBaseHook;
extern FUNCPTR     _func_excInfoShow;
extern FUNCPTR     _func_excIntHook;
extern FUNCPTR     _func_excJobAdd;
extern FUNCPTR     _func_excPanicHook;
extern FUNCPTR     _func_fastUdpErrorNotify;
extern FUNCPTR     _func_fastUdpInput;
extern FUNCPTR     _func_fastUdpPortTest;
extern FUNCPTR     _func_fclose;
extern FUNCPTR     _func_fppTaskRegsShow;
extern FUNCPTR     _func_altivecProbe;
extern FUNCPTR     _func_altivecTaskRegsGet;
extern FUNCPTR     _func_altivecTaskRegsShow;
extern FUNCPTR     _func_ftpLs;
extern FUNCPTR     _func_netLsByName;
extern FUNCPTR     _func_printErr;
extern FUNCPTR     _func_logMsg;
extern FUNCPTR     _func_memalign;
extern FUNCPTR     _func_pthread_setcanceltype;
extern FUNCPTR     _func_selPtyAdd;
extern FUNCPTR     _func_selPtyDelete;
extern FUNCPTR     _func_selTyAdd;
extern FUNCPTR     _func_selTyDelete;
extern FUNCPTR     _func_selWakeupAll;
extern FUNCPTR     _func_selWakeupListInit;
extern FUNCPTR     _func_selWakeupListTerm;
extern VOIDFUNCPTR _func_sigExcKill;
extern FUNCPTR     _func_sigprocmask;
extern FUNCPTR     _func_sigTimeoutRecalc;
extern FUNCPTR     _func_smObjObjShow;
extern FUNCPTR     _func_spy;
extern FUNCPTR     _func_spyStop;
extern FUNCPTR     _func_spyClkStart;
extern FUNCPTR     _func_spyClkStop;
extern FUNCPTR     _func_spyReport;
extern FUNCPTR     _func_spyTask;
extern FUNCPTR     _func_sseTaskRegsShow;
extern FUNCPTR     _func_symFindByValueAndType;   /* obsolete - do not use. */
extern FUNCPTR     _func_symFindByValue;          /* obsolete - do not use. */
extern FUNCPTR     _func_symFindSymbol;
extern FUNCPTR     _func_symNameGet;
extern FUNCPTR     _func_symValueGet;
extern FUNCPTR     _func_symTypeGet;
extern FUNCPTR     _func_taskCreateHookAdd;
extern FUNCPTR     _func_taskDeleteHookAdd;
extern FUNCPTR     _func_valloc;
extern FUNCPTR     _func_remCurIdGet;
extern FUNCPTR     _func_remCurIdSet;

extern FUNCPTR	   _dbgDsmInstRtn;

#ifndef VIRTUAL_STACK
extern BOOL        fastUdpInitialized;
#endif

extern BOOL	   ftpErrorSuppress;
extern BOOL	   _procNumWasSet;
extern int	   m2If64BitCounters;

extern VOIDFUNCPTR _func_evtLogO;
extern VOIDFUNCPTR _func_evtLogOIntLock;

extern VOIDFUNCPTR _func_evtLogM0;
extern VOIDFUNCPTR _func_evtLogM1;
extern VOIDFUNCPTR _func_evtLogM2;
extern VOIDFUNCPTR _func_evtLogM3;

extern VOIDFUNCPTR _func_evtLogT0;
extern VOIDFUNCPTR _func_evtLogT0_noInt;
extern VOIDFUNCPTR _func_evtLogT1;
extern VOIDFUNCPTR _func_evtLogT1_noTS;
extern VOIDFUNCPTR _func_evtLogTSched;

extern VOIDFUNCPTR _func_evtLogString;
extern FUNCPTR     _func_evtLogPoint;
extern FUNCPTR	   _func_evtLogReserveTaskName;

#if CPU_FAMILY==I960

extern VOIDFUNCPTR _func_windInst1;
extern VOIDFUNCPTR _func_windInstDispatch;
extern VOIDFUNCPTR _func_windInstIdle;
extern VOIDFUNCPTR _func_windInstIntEnt;
extern VOIDFUNCPTR _func_windInstIntExit;

#endif

extern FUNCPTR     _func_tmrStamp;
extern FUNCPTR     _func_tmrStampLock;
extern FUNCPTR     _func_tmrFreq;
extern FUNCPTR     _func_tmrPeriod;
extern FUNCPTR     _func_tmrConnect;
extern FUNCPTR     _func_tmrEnable;
extern FUNCPTR     _func_tmrDisable;

extern VOIDFUNCPTR _func_trgCheck;
extern UINT32 evtAction;
extern UINT32 wvEvtClass;
extern UINT32 trgEvtClass;

extern BOOL   wvInstIsOn;             /* windview instrumentation ON/OFF */
extern BOOL   wvObjIsEnabled;         /* Level 1 event collection enable */

#ifdef __cplusplus
}
#endif

#endif /* __INCfuncBindPh */
