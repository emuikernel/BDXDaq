/* wdbLib.h - header file for remote debug server */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,25apr02,jhw  Added C++ support (SPR 76304).
01i,25mar98,dbt  added wdbSystemSuspend() prototype.
01h,04mar98,dbt  added wdbCtxStartLibInit(), wdbUserEvtLibInit() and
                 wdbUserEvtPost() prototypes.
		 added WDB_MAX_USER_EVT_SIZE define.
01g,04feb98,dbt  added wdbEvtptLibInit() prototype. Undo 01f change
01f,12jan98,dbt  moved wdbSysBpLibInit() and wdbTaskBpLibInit() to wdbBpLib.h
01e,20jun95,tpr  added wdbMemCoreLibInit() prototype.
01d,19jun95,ms	 added prototypes for wdbCtxExitLibInit and wdbSioTest
01c,01jun95,ms	 changed prototype for wdbGopherLibInit.
01b,05apr95,ms   new data types.
01a,20sep94,ms   written.
*/

#ifndef __INCwdbLibh
#define __INCwdbLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "wdb/wdb.h"
#include "wdb/wdbBpLib.h"
#include "wdb/wdbRtIfLib.h"
#include "wdb/wdbCommIfLib.h"
#include "wdb/wdbRpcLib.h"
#include "sioLib.h"

/* defines */

#define WDB_MAX_USER_EVT_SIZE	250	/* max. WDB user event size (bytes) */

/* function prototypes */

#ifdef	__STDC__

extern STATUS	wdbTaskInit		(int pri, int opts, caddr_t stackBase,
					 int stackSize);
extern STATUS	wdbExternInit		(void * stackBase);
extern void	wdbInstallRtIf		(WDB_RT_IF *);
extern void	wdbInstallCommIf	(WDB_COMM_IF *, WDB_XPORT *);
extern void	wdbConnectLibInit	(void);
extern void	wdbMemLibInit		(void);
extern void	wdbMemCoreLibInit	(void);
extern void	wdbCtxLibInit		(void);
extern void	wdbCtxExitLibInit	(void);
extern void	wdbRegsLibInit		(void);
extern void	wdbEventLibInit		(void);
extern void	wdbEvtptLibInit		(void);
extern void	wdbFuncCallLibInit	(void);
extern void	wdbDirectCallLibInit	(void);
extern void	wdbGopherLibInit	(void);
extern void	wdbVioLibInit		(void);
extern void	wdbExcLibInit		(void);
extern void     wdbSysBpLibInit		(struct brkpt * pBps, int bpCnt);
extern void     wdbTaskBpLibInit	(void);
extern void	wdbSioTest		(SIO_CHAN *pChan, int mode, char eof);
extern void	wdbUserEvtLibInit	(void);
extern STATUS	wdbUserEvtPost		(char * event);
extern void	wdbCtxStartLibInit	(void);
extern STATUS	wdbSystemSuspend	(void);

#else	/* __STDC__ */

extern STATUS	wdbTaskInit		();
extern STATUS	wdbExternInit		();
extern void	wdbInstallRtIf		();
extern void	wdbInstallCommIf	();
extern void	wdbMemLibInit		();
extern void	wdbMemCoreLibInit	();
extern void	wdbCtxLibInit		();
extern void	wdbCtxExitLibInit	();
extern void	wdbRegsLibInit		();
extern void	wdbAddServiceLibInit	();
extern void	wdbDirectCallLibInit	();
extern void	wdbEventLibInit		();
extern void	wdbEvtptLibInit		();
extern void	wdbFuncCallLibInit	();
extern void	wdbGopherLibInit	();
extern void	wdbVioLibInit		();
extern void	wdbExcLibInit		();
extern void     wdbSysBpLibInit		();
extern void     wdbTaskBpLibInit	();
extern void	wdbSioTest		();
extern void	wdbUserEvtLibInit	();
extern STATUS	wdbUserEvtPost		();
extern void	wdbCtxStartLibInit	();
extern STATUS	wdbSystemSuspend	();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwdbLibh */

