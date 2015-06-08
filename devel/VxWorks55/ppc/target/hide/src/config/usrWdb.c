/* usrWdb.c - configuration file for the WDB agent */

/* Copyright 1994-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
03q,07jun02,g_h  SPR#78283, rename call fron wdbTmdPktDevInit() to
                 wdbVisionPktDevInit() in wdbCommIfInit()
03p,31mar02,jkf  SPR#74251, using bootLib.h macros for array sizes
03o,21mar02,jhw  Remove network initialization calls. (SPR 73517)
03n,13mar02,j_s  return error if end device table is empty (SPR 73604)
03m,21dec01,g_h  Change the name of the files that include for the TMD to
                 follow the WR naming convention
03l,06dec01,kab  SPR 71985: Fixed test of wdbTgtHasAltivec()
03k,93dec01,g_h  Adding command line support for Transparent Mode Driver.
03j,21nov01,jhw  Get WDB END device from sysBootParams (SPR 71595). 
		 Remove diab compiler warnings in wdbConfig.
		 Move wdbInfo back into WIND_TCB.
03i,20oct01,jhw  Moved wdbExitHook out of WIND_TCB.
03h,04oct01,c_c  Added wdbToolName symbol.
03g,06jun01,pch  Fix length of copy to vxBootFile.
03f,22may01,kab  Removed spurious warning message
03e,16may01,pch  Fix "`wdbTgtHasAltivec' defined but not used" message when
                 building non-AltiVec BSP's.
03d,09may01,dtr  Addition of wdbTgtHasCoprocessor.
03c,05feb01,zl   merged NPT
03b,10nov00,zl   fixed code causing warnings.
03a,03mar00,zl   merged SH support into T2.
03o,12apr01,pch  Add comments to various #else & #endif
03n,30mar01,pcs  Putting wdbTgtHasAltivec to replace OK/ERROR with TRUE/FALSE.
03m,26mar01,pcs  Remove the erronous commenting of the following line
                 "pAltivecRegs = wdbAltivecLibInit();"
03l,19mar01,tpw  Rename WDB_REG_SET_ALTIVEC to WDB_REG_SET_AV for consistency
                 with WTX and other REG_SET names.
03k,15mar01,pcs  Change include filename altiVecLib.h to altivecLib.h
03j,07feb01,dtr  Addition of hasAltivec to RtInfoGet.
03i,30jan01,dtr  Adding altivec register support.
03h,19dec00,pai  Included (uncommented) wdbPipePktDrv.h.  This does not
                 conflict with recent NPT modifications.  Moreover, it is
                 needed for T2 WDB_COMM_PIPE configuration.
03g,25aug99,sj  dont include private muxLib header file 
03f,30jul99,pul  modifications to support NPT
03e,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
03d,31mar99,sj  commenting out #include wdbPipePktDrv.h until tor2_0_x merge
03c,18mar99,sj  removed INCLUDE_NPT check
03b,05mar99,sj  eliminated call to muxTkLibInit. Must set muxMaxBinds
03a,15feb99,dbt remove task creation hook only if it has been previously
                installed (SPR #24195)
02z,08nov98,sj  added NPT support 
02y,12aug98,gnn fixed return values from wdbCommIfInit
02x,06aug98,pdn make sure that WDB_MTU is less or equal WDB_PIPE_PKT_MTU.
02w,03jun98,cym specified type int for local variable in taskCreateHookAdd.
02v,02jun98,dbt wdbTgtHasFpp() routine is now LOCAL.
02u,20may98,dbt test if wdbPipePktDevInit() returns an error.
		completed error checking for other communication links.
02t,13may98,dbt reworked HPSIM fix for SLIP communication link.
		check if sysBootLine is empty before parsing it.
02s,24apr98,dbt test pFpContext before reading task floating point registers
                (SPR #9940).
02r,19mar98,dbt added support for user events and context start notification.
		fixed some problems due to various merges.
02q,18mar98,jmb fix ifdef in previous patch 
02p,18mar98,jmb merge HPSIM poll for connect code 
02o,11mar98,pdn added WDB_COMM_PIPE for VxSims
02n,26jan98,gnn remerged from sirocco, was missing wdbSp function.
02m,21jan98,dbt added more error checking (SPR #9422, SPR #5651).
		vxTaskCreate() returns ERROR instead of NULL when taskCreat()
		fails (SPR #9050).
		Removed vxTaskLock() and vxTaskUnlock().
		Added wdbEvtptLibInit() call.
02l,12jan98,dbt modified for new breakpoint scheme.
		Replaced wdbBpRemove() with wdbDbgBpRemoveAll(). 
		Moved wdbBpSet() to usrBreakpoint.c.
02k,03oct97,gnn fixed a warning in the endLoad call
02j,25sep97,gnn SENS beta feedback fixes
02i,19sep97,vin added WDB_NUM_CL_BLKS and the logic for clBlks.
02c,06aug97,cdp gave type to 'initialised' in vxTaskDeleteHookAdd.
02h,17apr97,gnn added support for configNet type support for END.
02g,07apr97,gnn added new routines for MUX functionality.
02f,17mar97,hdn added sysIntLock() and sysIntUnlock() for I80X86.
02e,05mar97,ms  fixed IU_REGS_GET bug introduced by 01y checkin.
02d,04feb97,ms  Added support for NetROM 500 series.
02c,21jan97,gnn added code to load the device if we are booted standalone.
02b,17dec96,ms  WDB now uses FP_CONTEXT instead of FPREG_SET (SPR 7654).
02a,02oct96,elp changed casts due to TGT_ADDR_T type change.
01z,04nov96,gnn added code to handle smaller END MTU.
01y,28aug96,gnn	added WDB_COMM_END stuff.
01w,09aug96,ms  added "wdbSp" for windsh system mode "sp" (SPR 6254).
01x,23jul96,vin added BSD4.4 changes for mbufs. 
01v,22jul96,jmb added kab (ease) sanity check to wdbBpSet; HPUX can have tasks 
		with "unset" registers, and that gets propogated as addr 0x0.
01u,12mar96,ms  redid host pool init. made exteral agent run in kernelState.
01t,05mar96,tpr put the wdbHostPool always static for PPC (24bits limitation PB)
		+ added VX_FP_TASK flag to WDB_TASK_OPTIONS macro.
01s,24oct95,ms	moved wdbBpSet here.
01r,16oct95,ms  host mem pool created via static buffer instead of malloc
01q,11oct95,ms  turned off character echoing in TTY_TEST to fix SPR 5116
01p,21sep95,ms  macro shuffle + fix for SPR 4936.
01o,31aug95,ms  INCLUDE_WDB_TTY_TEST works if started before kernel (SPR #4808)
01n,23aug95,ms  bump intCnt to fake ISR context in extern agents extern hook
01m,28jun95,tpr added NETROM_TASK_POLL_DELAY macro.
01l,23jun95,ms	changed logMsg to _func_logMsg
01k,21jun95,ms	added taskRestart for the agent on exception.
01j,20jun95,tpr added wdbMemCoreLibInit().
01i,20jun95,ms	moved gopher buffer back to gopherLib.o for DSA
01h,19jun95,ms	moved event lib initialization near the begining of wdbConfig
		fixed vxTaskDeleteHookAdd to return OK or ERROR
		added initialization for INCLUDE_WDB_TTY_TEST
01g,15jun95,ms	updated for new serial drivers.
01f,07jun95,ms	exit hook no longer uses the spare TCB field
		memory size based on sysMemTop() instead of LOCAL_MEM_SIZE
		added some scalability
		WDB_STACK_SIZE used for both task and system agent.
		WDB_COMM_TYCO changed to WDB_COMM_TTY
01e,01jun95,ms	added taskLock/unlock.
		pass buffer to wdbGopherLibInit().
		decreace WDB_MTU when going over serial line.
		changed MTU->WDB_MTU, EXC_NOTIFY->WDB_EXC_NOTIFY,
			MAX_SERVICES->WDB_MAX_SERVICES,
			AGENT_POOL_SIZE->WDB_POOL_SIZE.
01d,25may95,ms	added fpp support for the system agent.
01c,23may95,ms	added some include files.
01b,17jan95,ms  cleaned up.
01a,21sep94,ms  written.
*/

/*
DESCRIPTION

This library configures and initializes the WDB agent.
The only user callable routine is wdbConfig(). This routine
initializes the agents OS function pointers, the communication
function pointers, and then the agent itself.
*/

#include "vxWorks.h"
#include "sysLib.h"
#include "stdlib.h"
#include "vxLib.h"
#include "taskLib.h"
#include "taskHookLib.h"
#ifdef INCLUDE_ALTIVEC
#include "altivecLib.h"
#endif /* INCLUDE_ALTIVEC */
#include "fppLib.h"
#include "intLib.h"
#include "rebootLib.h"
#include "bootLib.h"
#include "version.h"
#include "cacheLib.h"
#include "excLib.h"
#include "config.h"
#include "string.h"
#include "bufLib.h"
#include "sioLib.h"
#include "private/taskLibP.h"
#include "private/kernelLibP.h"
#include "private/vmLibP.h"
#include "private/funcBindP.h"
#include "wdb/wdb.h"
#include "wdb/wdbLib.h"
#include "wdb/wdbLibP.h"
#include "wdb/wdbBpLib.h"
#include "wdb/wdbSvcLib.h"
#include "wdb/wdbUdpLib.h"
#include "wdb/wdbUdpSockLib.h"
#include "wdb/wdbTyCoDrv.h"
#include "wdb/wdbRtIfLib.h"
#include "wdb/wdbCommIfLib.h"
#include "wdb/wdbMbufLib.h"
#include "wdb/wdbRpcLib.h"
#include "wdb/wdbRegs.h"
#include "wdb/wdbVioLib.h"
#include "drv/wdb/wdbVioDrv.h"
#include "drv/wdb/wdbSlipPktDrv.h"
#include "drv/wdb/wdbUlipPktDrv.h"
#include "drv/wdb/wdbNetromPktDrv.h"
#include "drv/wdb/wdbEndPktDrv.h"
#include "drv/wdb/wdbPipePktDrv.h"

#if defined (INCLUDE_WDB_COMM_VTMD)
#include "wdb/vision/wdbVisionDrv.c"
#include "drv/wdb/vision/wdbVisionPktDrv.h"
#include "wdb/vision/wdbVisionPktDrv.c"
#include "wdb/vision/visionTmdDrv.c"
#if   ((CPU == PPC603) || (CPU == PPC860) || (CPU == PPC604) || (CPU == PPC405))
#include "wdb/vision/visionTmdAsmPpc.c"
#elif (CPU == MIPS32)
#include "wdb/vision/visionTmdAsmMips.c"
#elif (CPU == ARMARCH4)
#include "wdb/vision/visionTmdAsmArm.c"
#elif (CPU == XSCALE)
#include "wdb/vision/visionTmdAsmXscale.c"
#elif ((CPU == SH7700) || (CPU == SH7750) || (CPU == SH7600))
#include "wdb/vision/visionTmdAsmSh.c"
#elif (CPU == MCF5200)
#include "wdb/vision/visionTmdAsmCfire.c"
#else
#error "This CPU is not supported !!!"
#endif
#endif /* (INCLUDE_WDB_COMM_VTMD) */

#if (CPU==SIMHPPA) && defined(INCLUDE_SLIP) && \
			(WDB_COMM_TYPE == WDB_COMM_NETWORK)
#include "wdLib.h"
#include "drv/sio/unixSio.h"

IMPORT void intCatch ();
extern int s_asyncio ();
LOCAL void usrWdbPollForConnect ();
LOCAL WDOG_ID wdSlipConnect;
#endif	/* CPU==SIMHPPA && INCLUDE_SLIP */

#if (WDB_COMM_TYPE == WDB_COMM_END)
#include "end.h"
#include "muxLib.h"
#include "muxTkLib.h"
#include "configNet.h"
IMPORT END_TBL_ENTRY endDevTbl[];
#endif /* WDB_COMM_TYPE == WDB_COMM_END */

/* defines */

#define	NUM_MBUFS		5
#define WDB_NUM_CL_BLKS		5
#define MILLION			1000000
#define MAX_LEN			BOOT_LINE_SIZE
#define	INCLUDE_VXWORKS_KERNEL		/* don't remove this */
#define	WDB_RESTART_TIME	10
#define	WDB_MAX_RESTARTS	5
#define WDB_BP_MAX		50	/* max # of break points */
#define WDB_MAX_SERVICES        50	/* max # of agent services */
#define WDB_TASK_PRIORITY       3	/* priority of task agent */
/* Currently, DSP and ALTIVEC are mutually exclusive */
#ifdef INCLUDE_ALTIVEC
#define WDB_TASK_OPTIONS        (VX_UNBREAKABLE | VX_FP_TASK | VX_ALTIVEC_TASK)
#else /* INCLUDE_ALTIVEC */
#ifdef INCLUDE_DSP
#define WDB_TASK_OPTIONS        (VX_UNBREAKABLE | VX_FP_TASK | VX_DSP_TASK)
#else
#define WDB_TASK_OPTIONS        VX_UNBREAKABLE | VX_FP_TASK /* agent options */
#endif /* INCLUDE_DSP */
#endif /* INCLUDE_ALTIVEC */
#define WDB_POOL_BASE		((char *)(FREE_RAM_ADRS))

/* lower WDB_MTU to SLMTU bytes for serial connection */

#if	(WDB_COMM_TYPE == WDB_COMM_TYCODRV_5_2) || \
	(WDB_COMM_TYPE == WDB_COMM_SERIAL)
#if	WDB_MTU > SLMTU
#undef	WDB_MTU
#define	WDB_MTU	SLMTU
#endif	/* WDB_MTU > SLMTU */
#endif	/* WDB_COMM_TYPE */

/* lower WDB_MTU to NETROM_MTU for netrom connections */

#if	(WDB_COMM_TYPE == WDB_COMM_NETROM)
#if	WDB_NETROM_TYPE == 400
#include "wdb/wdbNetromPktDrv.c"
#elif	WDB_NETROM_TYPE == 500
#include "wdb/amc500/wdbNetromPktDrv.c"
#else	/* WDB_NETROM_TYPE */
#error	WDB_NETROM_TYPE unknown
#endif	/* WDB_NETROM_TYPE */
#if	WDB_MTU > NETROM_MTU
#undef	WDB_MTU
#define	WDB_MTU	NETROM_MTU
#endif	/* WDB_MTU > NETROM_MTU */
#endif	/* WDB_COMM_TYPE == WDB_COMM_NETROM */

/* lower WDB_MTU to ULIP_MTU for ULIP connections */

#if	(WDB_COMM_TYPE == WDB_COMM_ULIP)
#if	WDB_MTU > ULIP_MTU
#undef	WDB_MTU
#define	WDB_MTU	ULIP_MTU
#endif	/* WDB_MTU > ULIP_MTU */
#endif	/* WDB_COMM_TYPE == WDB_COMM_ULIP */

#if	(WDB_COMM_TYPE == WDB_COMM_END)
#if	WDB_MTU > WDB_END_PKT_MTU
#undef	WDB_MTU
#define	WDB_MTU	WDB_END_PKT_MTU
#endif	/* WDB_MTU > WDB_END_PKT_MTU */

#ifndef MUX_MAX_BINDS
#define MUX_MAX_BINDS 16
#endif /* MUX_MAX_BINDS */

#endif /* (WDB_COMM_TYPE == WDB_COMM_END) */

#if	(WDB_COMM_TYPE == WDB_COMM_PIPE)
#if	WDB_MTU > WDB_PIPE_PKT_MTU
#undef	WDB_MTU
#define	WDB_MTU	WDB_PIPE_PKT_MTU
#endif	/* WDB_MTU > WDB_PIPE_PKT_MTU */
#endif	/* WDB_COMM_TYPE == WDB_COMM_PIPE */

/* change agent mode to task mode for NETWORK or TYCODRV_5_2 connections */

#if	(WDB_COMM_TYPE == WDB_COMM_TYCODRV_5_2) || (WDB_COMM_TYPE == WDB_COMM_NETWORK)
#undef	WDB_MODE
#define	WDB_MODE	WDB_MODE_TASK
#endif	/* WDB_COMM_TYPE */

/* globals */

uint_t		 wdbCommMtu	= WDB_MTU;
int		 wdbNumMemRegions = 0;	/* number of extra memory regions */
WDB_MEM_REGION * pWdbMemRegions = NULL;	/* array of regions */

/* This two macros transform TOOL define into the string "TOOL" */

#define MKSTR(MACRO) MKSTR_FIRST_PASS(MACRO)
#define MKSTR_FIRST_PASS(MACRO) #MACRO

/* 
 * This symbol "wdbToolName" is used by the tgtsvr to retrieve the name of the
 * tool used to build vxWorks run-time. DO NOT REMOVE !!!
 */
 
#ifdef	TOOL
const char wdbToolName[]	= MKSTR(TOOL);
#else	/* TOOL */
const char  wdbToolName[]	= "Unknown";
#endif	/* TOOL */

#if (CPU==SIMHPPA)
extern void sysSerialWDBSetup ();
#endif /* (CPU==SIMHPPA) */

/* locals */

LOCAL BUF_POOL		wdbMbufPool;
LOCAL BUF_POOL		wdbClBlkPool;
LOCAL char		vxBootFile [MAX_LEN];
LOCAL WDB_RT_IF		wdbRtIf;
LOCAL BOOL		prevKernelState;
LOCAL VOIDFUNCPTR	wdbCreateHook = NULL;	/* WDB task create hook */

/*
 * These are private - but configurable size arrays can't be malloc'ed
 * in external mode, so we define them here.
 */

#if     (WDB_MODE & WDB_MODE_EXTERN)
LOCAL uint_t	wdbExternStackArray [WDB_STACK_SIZE/sizeof(uint_t)];
#endif /* (WDB_MODE & WDB_MODE_EXTERN) */

LOCAL WDB_SVC		wdbSvcArray       [WDB_MAX_SERVICES];
LOCAL uint_t		wdbSvcArraySize = WDB_MAX_SERVICES;

#ifdef INCLUDE_WDB_BP
LOCAL struct brkpt wdbBreakPoints [WDB_BP_MAX];
#endif /* INCLUDE_WDB_BP */

/* forward static declarations */

LOCAL void	wdbMbufInit ();
LOCAL void	wdbRtIfInit ();
LOCAL STATUS	wdbCommIfInit ();
LOCAL bool_t	wdbTgtHasFpp (void);
#ifdef INCLUDE_DSP
LOCAL bool_t	wdbTgtHasDsp (void);
#endif /* INCLUDE_DSP */
#ifdef  INCLUDE_ALTIVEC
LOCAL bool_t	wdbTgtHasAltivec (void);
#endif /* INCLUDE_ALTIVEC */

/******************************************************************************
*
* wdbConfig - configure and initialize the WDB agent.
*
* This routine configures and initializes the WDB agent.
*
* RETURNS :
* OK if at least one of the agents (task or system) was correctly
* initialized. ERROR otherwise.
*
* NOMANUAL
*/

STATUS wdbConfig (void)
    {

#if     (WDB_MODE & WDB_MODE_DUAL)
    STATUS	status1 = ERROR;
    STATUS	status2 = ERROR;
#elif     (WDB_MODE & WDB_MODE_TASK)
    STATUS	status1 = ERROR;
#elif     (WDB_MODE & WDB_MODE_EXTERN)
    STATUS	status2 = ERROR;
#endif	/* WDB_MODE_[DUAL||TASK||EXTERN] */
 
#if     (WDB_MODE & WDB_MODE_EXTERN)
    caddr_t	pExternStack;
#endif	/* WDB_MODE & WDB_MODE_EXTERN */

    /* Initialize the agents interface function pointers */

    wdbRtIfInit ();			/* run-time interface functions */

    if (wdbCommIfInit () == ERROR)	/* communication interface functions */
	{
	if (_func_printErr != NULL)
	    _func_printErr ("wdbConfig: error configuring WDB communication interface\n");
	return (ERROR);
	}

    /* Install some agent services */

    wdbSvcLibInit (wdbSvcArray, wdbSvcArraySize);

    wdbConnectLibInit ();	/* required agent service */

    wdbMemCoreLibInit ();	/* required agent service */

#ifdef	INCLUDE_WDB_MEM
    wdbMemLibInit ();		/* extra memory services */
#endif	/* INCLUDE_WDB_MEM */

#ifdef	INCLUDE_WDB_EVENTS
    wdbEventLibInit();
#endif	/* INCLUDE_WDB_EVENTS */

#ifdef	INCLUDE_WDB_EVENTPOINTS
    wdbEvtptLibInit();
#endif	/* INCLUDE_WDB_EVENTPOINTS */

#ifdef  INCLUDE_WDB_DIRECT_CALL
    wdbDirectCallLibInit ();
#endif  /* INCLUDE_WDB_DIRECT_CALL */

#ifdef	INCLUDE_WDB_CTXT
    wdbCtxLibInit ();
#endif	/* INCLUDE_WDB_CTXT */

#ifdef	INCLUDE_WDB_REG
    wdbRegsLibInit ();
#endif	/* INCLUDE_WDB_REG */

#ifdef	INCLUDE_WDB_GOPHER
    wdbGopherLibInit();
#endif	/* INCLUDE_WDB_GOPHER */

#ifdef	INCLUDE_WDB_EXIT_NOTIFY
    wdbCtxExitLibInit();
#endif	/* INCLUDE_WDB_EXIT_NOTIFY */

#ifdef	INCLUDE_WDB_EXC_NOTIFY
    wdbExcLibInit();
#endif	/* INCLUDE_WDB_EXC_NOTIFY */

#ifdef	INCLUDE_WDB_FUNC_CALL
    wdbFuncCallLibInit ();
#endif	/* INCLUDE_WDB_FUNC_CALL */

#ifdef	INCLUDE_WDB_VIO
    wdbVioLibInit();
    wdbVioDrv("/vio");
#endif	/* INCLUDE_WDB_VIO */

#ifdef  INCLUDE_WDB_TSFS
    wdbTsfsDrv ("/tgtsvr");
#endif  /* INCLUDE_WDB_TSFS */

#ifdef  INCLUDE_WDB_BP
    wdbSysBpLibInit (wdbBreakPoints, WDB_BP_MAX);
#if	(WDB_MODE & WDB_MODE_TASK)
    wdbTaskBpLibInit ();
#endif	/* WDB_MODE & WDB_MODE_TASK */
#endif	/* INCLUDE_WDB_BP */

#ifdef	INCLUDE_WDB_START_NOTIFY
    wdbCtxStartLibInit ();
#endif	/* INCLUDE_WDB_START_NOTIFY */

#ifdef	INCLUDE_WDB_USER_EVENT
    wdbUserEvtLibInit ();
#endif	/* INCLUDE_WDB_USER_EVENT */

    /* Initialize the agent(s) */

#if	(WDB_MODE & WDB_MODE_TASK)
    status1 = wdbTaskInit (WDB_TASK_PRIORITY,
			WDB_TASK_OPTIONS, NULL, WDB_STACK_SIZE);
#endif
#if	(WDB_MODE & WDB_MODE_EXTERN)
#if	_STACK_DIR == _STACK_GROWS_DOWN
    pExternStack = (caddr_t)&wdbExternStackArray
				[WDB_STACK_SIZE/sizeof(uint_t)];
    pExternStack = (caddr_t)STACK_ROUND_DOWN (pExternStack);
#else	/* _STACK_DIR == _STACK_GROWS_UP */
    pExternStack = (caddr_t)wdbExternStackArray;
    pExternStack = (caddr_t)STACK_ROUND_UP (pExternStack);
#endif	/* _STACK_DIR == _STACK_GROWS_DOWN */

    status2 = wdbExternInit (pExternStack);

#ifdef	INCLUDE_HW_FP
    if (wdbTgtHasFpp())
	{
	WDB_REG_SET_OBJ * pFpRegs;
	pFpRegs = wdbFpLibInit();
	wdbExternRegSetObjAdd (pFpRegs);
	}
#endif	/* INCLUDE_HW_FP */

#ifdef	INCLUDE_DSP
    if (wdbTgtHasDsp ())
	{
	WDB_REG_SET_OBJ * pDspRegs;
	pDspRegs = wdbDspLibInit();
	wdbExternRegSetObjAdd (pDspRegs);
	}
#endif	/* INCLUDE_DSP */

#ifdef  INCLUDE_ALTIVEC
    if (wdbTgtHasAltivec())
	{
	WDB_REG_SET_OBJ * pAltivecRegs;
	pAltivecRegs = wdbAltivecLibInit();
	wdbExternRegSetObjAdd (pAltivecRegs);
	}
#endif /* INCLUDE_ALTIVEC */
#endif	/* WDB_MODE & WDB_MODE_EXTERN */

    /* activate one agent only */

#if	(WDB_MODE & WDB_MODE_TASK)
    wdbModeSet (WDB_MODE_TASK);
#else
    wdbModeSet (WDB_MODE_EXTERN);
#endif

#if (WDB_MODE & WDB_MODE_DUAL)
    return ((status1 && status2) ? ERROR : OK);
#elif (WDB_MODE & WDB_MODE_TASK)
    return ((status1) ? ERROR : OK);
#elif (WDB_MODE & WDB_MODE_EXTERN)
    return ((status2) ? ERROR : OK);
#else
    return ERROR;
#endif
    }

/******************************************************************************
*
* wdbExternEnterHook - hook to call when external agent is entered.
*/

void wdbExternEnterHook (void)
    {
    intCnt++;			/* always fake an interrupt context */
    prevKernelState = kernelState;
    kernelState = TRUE;		/* always run in kernel state */

#if	(CPU_FAMILY==I80X86)
    sysIntLock ();
#endif	/* CPU_FAMILY==I80X86 */

#ifdef	INCLUDE_WDB_BP
    wdbDbgBpRemoveAll ();
#endif
    }

/******************************************************************************
*
* wdbExternExitHook - hook to call when the external agent resumes the system.
*/

void wdbExternExitHook (void)
    {
    intCnt--;			/* restore original intCnt value */
    kernelState = prevKernelState; /* restore original kernelState value */

#if	(CPU_FAMILY==I80X86)
    intLock ();
    sysIntUnlock ();
#endif	/* CPU_FAMILY==I80X86 */

#ifdef  INCLUDE_WDB_BP
    wdbBpInstall ();
#endif
    }

/******************************************************************************
*
* wdbTgtHasFpp - TRUE if target has floating point support.
*/

LOCAL bool_t wdbTgtHasFpp (void)
    {
#ifdef	INCLUDE_HW_FP
    if (fppProbe() == OK)
	return (TRUE);
    return (FALSE);
#else
    return (FALSE);
#endif
    }

#ifdef	INCLUDE_DSP
/******************************************************************************
*
* wdbTgtHasDsp - TRUE if target has DSP support.
*/

LOCAL bool_t wdbTgtHasDsp (void)
    {
    if (dspProbe() == OK)
	return (TRUE);
    return (FALSE);
    }
#endif /* INCLUDE_DSP */

#ifdef  INCLUDE_ALTIVEC
/******************************************************************************
*
* wdbTgtHasAltivec - TRUE if target has altivec support.
*/

LOCAL bool_t wdbTgtHasAltivec (void)
    {
    if (altivecProbe() == OK)
        return (TRUE);
    return (FALSE);
    }
#endif /* INCLUDE_ALTIVEC */

/******************************************************************************
* 
* wdbTgtHasCoprocessor - this function's result contains bit fields for each 
* 	co-processor. Coprocessors include floating-point,altivec(PPC) and 
*	dsp(SH).
*/ 

LOCAL UINT32 wdbTgtHasCoprocessor (void)
    {
    UINT32 result;
    result = wdbTgtHasFpp();
#ifdef INCLUDE_ALTIVEC
    result |= (wdbTgtHasAltivec() << WDB_CO_PROC_ALTIVEC);
#endif
#ifdef INCLUDE_DSP
    result |= (wdbTgtHasDsp() << WDB_CO_PROC_DSP);
#endif
    return result;
    }

/******************************************************************************
*
* wdbRtInfoGet - get info on the VxWorks run time system.
*/

LOCAL void wdbRtInfoGet
    (
    WDB_RT_INFO *	pRtInfo
    )
    {
    pRtInfo->rtType	= WDB_RT_VXWORKS;
    pRtInfo->rtVersion	= vxWorksVersion;
    pRtInfo->cpuType	= CPU;
    pRtInfo->hasCoprocessor	= wdbTgtHasCoprocessor();
#ifdef	INCLUDE_PROTECT_TEXT
    pRtInfo->hasWriteProtect	= (vmLibInfo.pVmTextProtectRtn != NULL);
#else	/* !INCLUDE_PROTECT_TEXT */
    pRtInfo->hasWriteProtect	= FALSE;
#endif	/* !INCLUDE_PROTECT_TEXT */
    pRtInfo->pageSize   = VM_PAGE_SIZE_GET();
    pRtInfo->endian	= _BYTE_ORDER;
    pRtInfo->bspName	= sysModel();

    pRtInfo->bootline	= vxBootFile;

#ifdef	HITACHI_SH_KERNEL_ON_SDRAM
    pRtInfo->memBase	= (TGT_ADDR_T)(FREE_RAM_ADRS);
    pRtInfo->memSize	= (int)sysMemTop() - (int)FREE_RAM_ADRS;
#else	/* HITACHI_SH_KERNEL_ON_SDRAM */
    pRtInfo->memBase	= (TGT_ADDR_T)(LOCAL_MEM_LOCAL_ADRS);
    pRtInfo->memSize	= (int)sysMemTop() - (int)LOCAL_MEM_LOCAL_ADRS;
#endif	/* HITACHI_SH_KERNEL_ON_SDRAM */
    pRtInfo->numRegions	= wdbNumMemRegions;
    pRtInfo->memRegion	= pWdbMemRegions;

    pRtInfo->hostPoolBase = (TGT_ADDR_T)WDB_POOL_BASE;
    pRtInfo->hostPoolSize = WDB_POOL_SIZE;
    }

/******************************************************************************
*
* vxReboot - reboot the system.
*/

LOCAL void vxReboot (void)
    {
    reboot (0);
    }

/******************************************************************************
*
* vxMemProtect - protect a region of memory.
*/

LOCAL STATUS vxMemProtect
    (
    char * addr,
    u_int  nBytes,
    bool_t protect		/* TRUE = protect, FALSE = unprotect */
    )
    {
    return (VM_STATE_SET (NULL, addr, nBytes, VM_STATE_MASK_WRITABLE, 
                     (protect ? VM_STATE_WRITABLE_NOT : VM_STATE_WRITABLE)));
    }

#ifdef	INCLUDE_VXWORKS_KERNEL
/******************************************************************************
*
* wdbSp - spawn a task with default params
*/ 

#define PRIORITY        100
#define OPTIONS         VX_FP_TASK

void wdbSp
    (
    int (*func)(),
    int arg0,
    int arg1,
    int arg2,
    int arg3,
    int arg4
    )
    {
    taskSpawn (NULL, PRIORITY, OPTIONS, WDB_SPAWN_STACK_SIZE, func, arg0,
        arg1, arg2, arg3, arg4, 0, 0, 0, 0, 0);
    }

/******************************************************************************
*
* vxTaskCreate - WDB callout to create a task (and leave suspended).
*
* RETURNS : Task ID or ERROR if unable to create a task
*
* NOMANUAL
*/

LOCAL int vxTaskCreate
    (
    char *   name,       /* name of new task (stored at pStackBase)   */
    int      priority,   /* priority of new task                      */
    int      options,    /* task option word                          */
    caddr_t  stackBase,  /* base of stack. ignored by VxWorks	      */
    int      stackSize,  /* size (bytes) of stack needed plus name    */
    caddr_t  entryPt,    /* entry point of new task                   */
    int      arg[10],	 /* 1st of 10 req'd task args to pass to func */
    int      fdIn,	 /* fd for input redirection		      */
    int      fdOut,	 /* fd for output redirection		      */
    int      fdErr	 /* fd for error output redirection	      */
    )
    {
    int tid;

    if (stackSize == 0)
	stackSize = WDB_SPAWN_STACK_SIZE;

    tid = taskCreat (name, priority, options, stackSize, (int (*)())entryPt,
		arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6],
		arg[7], arg[8], arg[9]);

    if (tid == (int)NULL)		/* create failed */
	return (ERROR);

#ifdef	INCLUDE_IO_SYSTEM
    if (fdIn != 0)
	ioTaskStdSet (tid, 0, fdIn);
    if (fdOut != 0)
	ioTaskStdSet (tid, 1, fdOut);
    if (fdErr != 0)
	ioTaskStdSet (tid, 2, fdErr);
#endif	/* INCLUDE_IO_SYSTEM */

    return (tid);
    }


/******************************************************************************
*
* vxTaskResume - WDB callout to resume a suspended task.
*/

LOCAL STATUS vxTaskResume
    (
    WDB_CTX *	pContext
    )
    {
    if (pContext->contextType != WDB_CTX_TASK)
	return (ERROR);

    return (taskResume (pContext->contextId));
    }

/******************************************************************************
*
* vxTaskSuspend - WDB callout to suspend a task.
*/

LOCAL STATUS vxTaskSuspend
    (
    WDB_CTX *	pContext
    )
    {
    if (pContext->contextType != WDB_CTX_TASK)
	return (ERROR);

    return (taskSuspend (pContext->contextId));
    }

/******************************************************************************
*
* vxTaskDelete - WDB callout to delete a task.
*/

LOCAL STATUS vxTaskDelete
    (
    WDB_CTX *	pContext
    )
    {
    if (pContext->contextType != WDB_CTX_TASK)
	return (ERROR);

    return (taskDelete (pContext->contextId));
    }

/******************************************************************************
*
* vxTaskRegsSet - WDB callout to get a task register set.
*/

LOCAL STATUS vxTaskRegsSet
    (
    WDB_CTX *	 pContext,
    WDB_REG_SET_TYPE regSetType,
    char *	 pRegSet
    )
    {
    STATUS		status;

    if (pContext->contextType != WDB_CTX_TASK)
	return (ERROR);

    switch (regSetType)
	{
	case WDB_REG_SET_IU:
	    status = taskRegsSet (pContext->contextId, (REG_SET *)pRegSet);
	    break;

#ifdef  INCLUDE_HW_FP
	case WDB_REG_SET_FPU:
	    {
	    WIND_TCB * pTcb = taskTcb (pContext->contextId);

	    if ((pTcb == NULL) || (pTcb->pFpContext == NULL))
		return (ERROR);

	    bcopy (pRegSet, (char *)(pTcb->pFpContext), sizeof (FP_CONTEXT));
	    return (OK);
	    }
#endif	/* INCLUDE_HW_FP */
#ifdef  INCLUDE_ALTIVEC
	case WDB_REG_SET_AV:
	    {
	    WIND_TCB * pTcb = taskTcb (pContext->contextId);

            if ((pTcb == NULL) || (ALTIVEC_CONTEXT_GET(pTcb) == NULL))
		return (ERROR);

            bcopy (pRegSet, (char *)(ALTIVEC_CONTEXT_GET(pTcb)), sizeof (ALTIVEC_CONTEXT));
	    return (OK);
	    }
#endif /* INCLUDE_ALTIVEC */
#ifdef  INCLUDE_DSP
	case WDB_REG_SET_DSP:
	    {
	    WIND_TCB * pTcb = taskTcb (pContext->contextId);

	    if ((pTcb == NULL) || (pTcb->pDspContext == NULL))
		return (ERROR);

	    /*
	     * If modifying last DSP task to run, force it to restore
	     * when it runs next.  There is no need to do a dspSave()
	     * here as the entire DSP_CONTEXT is about to be overwritten.
	     */
	    if (pTcb == pTaskLastDspTcb)
		pTaskLastDspTcb = NULL;

	    bcopy (pRegSet, (char *)(pTcb->pDspContext), sizeof (DSP_CONTEXT));
	    return (OK);
	    }
#endif	/* INCLUDE_DSP */

	default:
	    status = ERROR;
	}

    return (status);
    }

/******************************************************************************
*
* vxTaskRegsGet - WDB callout to get a tasks register set.
*
* This routine is not reentrant, but it it only called by one thread (the
* WDB agent).
*/

LOCAL STATUS vxTaskRegsGet
    (
    WDB_CTX *		pContext,
    WDB_REG_SET_TYPE 	regSetType,
    char **		ppRegSet
    )
    {
    WIND_TCB * pTcb;
    if (pContext->contextType != WDB_CTX_TASK)
	return (ERROR);

    pTcb = taskTcb (pContext->contextId);
    if (pTcb == NULL)
	return (ERROR);

    switch (regSetType)
	{
	case WDB_REG_SET_IU:
	    {
	    REG_SET dummy;
	    taskRegsGet (pContext->contextId, &dummy);
	    *ppRegSet = (char *) &pTcb->regs;
	    return (OK);
	    }

#ifdef  INCLUDE_HW_FP
	case WDB_REG_SET_FPU:
	    if (pTcb->pFpContext == NULL)
		return (ERROR);		/* no coprocessor support */

	    *ppRegSet = (char *) pTcb->pFpContext;
	    return (OK);
#endif	/* INCLUDE_HW_FP */
#ifdef INCLUDE_ALTIVEC
	case WDB_REG_SET_AV:
            if (ALTIVEC_CONTEXT_GET(pTcb) == NULL)
		return (ERROR);		/* no coprocessor support */

	    *ppRegSet = (char *) pTcb->spare4;
	    return (OK);
#endif /* INCLUDE_ALTIVEC */
#ifdef  INCLUDE_DSP
	case WDB_REG_SET_DSP:
	    if (pTcb == pTaskLastDspTcb)
		dspSave (pTaskLastDspTcb->pDspContext);
	    if (pTcb->pDspContext)
		{
		*ppRegSet = (char *) pTcb->pDspContext;
		return (OK);
		}
	    return (ERROR);
#endif	/* INCLUDE_DSP */

	default:
	    return (ERROR);
	}

    }

/******************************************************************************
*
* vxSemCreate - create a SEMAPHORE
*/

LOCAL void * vxSemCreate (void)
    {
    return ((void *)semBCreate (0, 0));
    }

/******************************************************************************
*
* vxSemGive - give a semaphore
*/

LOCAL STATUS vxSemGive
    (
    void * semId
    )
    {
    return (semGive ((SEM_ID)semId));
    }

/******************************************************************************
*
* vxSemTake - take a semaphore
*/

LOCAL STATUS vxSemTake
    (
    void *		semId,
    struct timeval *	tv
    )
    {
    return (semTake ((SEM_ID) semId, 
	(tv == NULL ? WAIT_FOREVER :
	tv->tv_sec * sysClkRateGet() +
	(tv->tv_usec * sysClkRateGet()) / MILLION)));
    }
#endif	/* INCLUDE_VXWORKS_KERNEL */

/******************************************************************************
*
* vxExcHookAdd -
*/

LOCAL void (*vxExcHook)();

LOCAL int vxExcHookWrapper (int vec, char *pESF, WDB_IU_REGS *pRegs)
    {
    WDB_CTX	context;
    static int	restartCnt;
    extern int	wdbTaskId;

    if (INT_CONTEXT() || wdbIsNowExternal() || (taskIdCurrent == 0))
	context.contextType = WDB_CTX_SYSTEM;
    else
	context.contextType = WDB_CTX_TASK;
    context.contextId	= (int)taskIdCurrent;

    (*vxExcHook)(context, vec, pESF, pRegs);

    /*
     * if the exception is in the agent task, restart the agent
     * after a delay.
     */

    if (((int)taskIdCurrent == wdbTaskId) && (restartCnt < WDB_MAX_RESTARTS))
	{
	restartCnt++;
	if (_func_logMsg != NULL)
	    _func_logMsg ("WDB exception. restarting agent in %d seconds...\n",
		WDB_RESTART_TIME, 0,0,0,0,0);
	taskDelay (sysClkRateGet() * WDB_RESTART_TIME);
	taskRestart (0);
	}

    return (FALSE);
    }

LOCAL void vxExcHookAdd
    (
    void	(*hook)()
    )
    {
    vxExcHook = hook;

    _func_excBaseHook = vxExcHookWrapper;
    }

/******************************************************************************
*
* __wdbTaskDeleteHook -
*/ 

LOCAL int __wdbTaskDeleteHook
    (
    WIND_TCB *pTcb
    )
    {
    WDB_CTX	ctx;
    void	(*hook)();

    hook = pTcb->wdbInfo.wdbExitHook;

    if (hook != NULL)
	{
	ctx.contextType	= WDB_CTX_TASK;
	ctx.contextId	= (UINT32)pTcb;
	(*hook) (ctx, pTcb->exitCode, pTcb->errorStatus);
	}

    return (OK);
    }

/******************************************************************************
*
* vxTaskDeleteHookAdd - task-specific delete hook (one per task).
*
* currently only one hook per task.
*/ 

LOCAL STATUS vxTaskDeleteHookAdd
    (
    UINT32	tid,
    void	(*hook)()
    )
    {
    static int initialized = FALSE;

    if (taskIdVerify ((int)tid) == ERROR)
	return (ERROR);

    (taskTcb (tid)->wdbInfo).wdbExitHook = hook;

    if (!initialized)
	{
	taskDeleteHookAdd (__wdbTaskDeleteHook);
	initialized = TRUE;
	}

    return (OK);
    }

/******************************************************************************
*
* __wdbTaskCreateHook - task create hook
*
* This hook is called each time a task is created.
*
* RETURNS : OK always
*/ 

LOCAL int __wdbTaskCreateHook
    (
    WIND_TCB *	pTcb
    )
    {
    WDB_CTX	createdCtx;
    WDB_CTX	creationCtx;

    if (wdbCreateHook != NULL)
	{
	/* fill createdCtx structure */

	createdCtx.contextType	= WDB_CTX_TASK;
	createdCtx.contextId	= (UINT32)pTcb;

	/* fill creationCtx structure */

	creationCtx.contextType	= WDB_CTX_TASK;
	creationCtx.contextId	= (UINT32)taskIdCurrent;

	(*wdbCreateHook) (&createdCtx, &creationCtx);
	}

    return (OK);
    }

/******************************************************************************
*
* vxTaskCreateHookAdd - install WDB task create hook.
*
* This routine installs or remove the WDB task create hook. 
*
* RETURNS : OK always.
*/ 

LOCAL STATUS vxTaskCreateHookAdd
    (
    void	(*hook)()
    )
    {
    static int initialized = FALSE;

    wdbCreateHook = hook;

    if ((hook == NULL) && initialized)	/* remove task creation hook */
	{
	taskCreateHookDelete (__wdbTaskCreateHook);
	initialized = FALSE;
	}
    else if (!initialized)		/* install task creation hook */
	{
	taskCreateHookAdd (__wdbTaskCreateHook);
	initialized = TRUE;
	}

    return (OK);
    }

/******************************************************************************
*
* wdbRtIfInit - Initialize pointers to the VxWorks routines.
*/

LOCAL void wdbRtIfInit ()
    {
    int 	ix = 0;
    WDB_RT_IF *	pRtIf = &wdbRtIf;

    bzero ((char *)pRtIf, sizeof (WDB_RT_IF));

    pRtIf->rtInfoGet	= wdbRtInfoGet;

    pRtIf->reboot	= vxReboot;

    pRtIf->cacheTextUpdate = (void (*)())cacheLib.textUpdateRtn;

    pRtIf->memProtect   = vxMemProtect;
    pRtIf->memProbe	= (STATUS (*)())vxMemProbe;

    pRtIf->excHookAdd	= vxExcHookAdd;

#ifdef	INCLUDE_VXWORKS_KERNEL
    pRtIf->taskCreate	= vxTaskCreate;
    pRtIf->taskResume	= vxTaskResume;
    pRtIf->taskSuspend	= vxTaskSuspend;
    pRtIf->taskDelete	= vxTaskDelete;

    pRtIf->taskLock	= (VOIDFUNCPTR) taskLock;
    pRtIf->taskUnlock	= (VOIDFUNCPTR) taskUnlock;

    pRtIf->taskRegsSet	= vxTaskRegsSet;
    pRtIf->taskRegsGet  = vxTaskRegsGet;

    pRtIf->malloc	= malloc;
    pRtIf->free		= free;

    pRtIf->semCreate	= vxSemCreate;
    pRtIf->semGive	= vxSemGive;
    pRtIf->semTake	= vxSemTake;

    pRtIf->taskDeleteHookAdd	= vxTaskDeleteHookAdd;
    pRtIf->taskSwitchHookAdd	= (STATUS (*)())taskSwitchHookAdd;
    pRtIf->taskCreateHookAdd	= vxTaskCreateHookAdd;
#endif	/* INCLUDE_VXWORKS_KERNEL */

    /* first check if boot line is empty (eg : no network) */

    if (*sysBootLine != EOS)
	{
	for (ix = 0; ix < MAX_LEN; ix ++)
	    {
	    if (*(sysBootLine + ix) == ')')
		{
		ix++;
		break;
		}
	    }

	/* Copy the bootline following the ')' to vxBootFile */
	strncpy (vxBootFile, sysBootLine + ix, sizeof(vxBootFile));

	/* Truncate vxBootFile at the first space */
	for (ix = 0; ix < MAX_LEN - 1; ix ++)
	    {
	    if (*(vxBootFile + ix) == ' ')
		break;
	    }
	}

    *(vxBootFile + ix) = '\0';

    wdbInstallRtIf (pRtIf);
    }


/******************************************************************************
*
* wdbCommIfInit - Initialize the agent's communction interface
*
* RETURNS : OK or error if we can't initialize the communication interface.
*
* NOMANUAL
*/

LOCAL STATUS wdbCommIfInit ()
    {
    static uint_t	wdbInBuf	  [WDB_MTU/4];
    static uint_t	wdbOutBuf	  [WDB_MTU/4];

    static WDB_XPORT	wdbXport;
    static WDB_COMM_IF	wdbCommIf;

    WDB_COMM_IF * pCommIf = &wdbCommIf;

    wdbMbufInit ();

#if	(WDB_COMM_TYPE == WDB_COMM_NETWORK)
    /* UDP sockets - supports a task agent */

    if (wdbUdpSockIfInit (pCommIf) == ERROR)
	return (ERROR);
#endif	/* (WDB_COMM_TYPE == WDB_COMM_NETWORK) */

#if	(WDB_COMM_TYPE == WDB_COMM_TYCODRV_5_2)
    {
    /* SLIP lite built on a VxWorks serial driver - supports a task agent */

    static WDB_TYCO_SIO_CHAN tyCoSioChan;	/* serial I/O device */
    static WDB_SLIP_PKT_DEV  wdbSlipPktDev;	/* SLIP packet device */

    if (wdbTyCoDevInit	(&tyCoSioChan, WDB_TTY_DEV_NAME, WDB_TTY_BAUD))
	return (ERROR);

#ifdef	INCLUDE_WDB_TTY_TEST
    wdbSioTest ((SIO_CHAN *)&tyCoSioChan, SIO_MODE_INT, 0);
#endif	/* INCLUDE_WDB_TTY_TEST */

    wdbSlipPktDevInit	(&wdbSlipPktDev, (SIO_CHAN *)&tyCoSioChan, udpRcv);
    if (udpCommIfInit (pCommIf, &wdbSlipPktDev.wdbDrvIf))
	return (ERROR);
    }
#endif	/* (WDB_COMM_TYPE == WDB_COMM_TYCODRV_5_2) */

#if	(WDB_COMM_TYPE == WDB_COMM_ULIP)
    {
    /* ULIP packet driver (VxSim only) - supports task or external agent */

    static WDB_ULIP_PKT_DEV	wdbUlipPktDev;	/* ULIP packet device */

    wdbUlipPktDevInit (&wdbUlipPktDev, WDB_ULIP_DEV, udpRcv);
    if (udpCommIfInit (pCommIf, &wdbUlipPktDev.wdbDrvIf))
	return (ERROR);
    }
#endif	/* (WDB_COMM_TYPE == WDB_COMM_ULIP) */

#if	(WDB_COMM_TYPE == WDB_COMM_SERIAL)
    {
    /* SLIP-lite over a raw serial channel - supports task or external agent */

    SIO_CHAN *			pSioChan;	/* serial I/O channel */
    static WDB_SLIP_PKT_DEV	wdbSlipPktDev;	/* SLIP packet device */

    if ((pSioChan = sysSerialChanGet (WDB_TTY_CHANNEL)) == (SIO_CHAN *)ERROR)
	return (ERROR);

    sioIoctl (pSioChan, SIO_BAUD_SET, (void *)WDB_TTY_BAUD);

#ifdef	INCLUDE_WDB_TTY_TEST
    /* test in polled mode if the kernel hasn't started */

    if (taskIdCurrent == 0)
	wdbSioTest (pSioChan, SIO_MODE_POLL, 0);
    else
	wdbSioTest (pSioChan, SIO_MODE_INT, 0);
#endif	/* INCLUDE_WDB_TTY_TEST */

    wdbSlipPktDevInit (&wdbSlipPktDev, pSioChan, udpRcv);
    if (udpCommIfInit (pCommIf, &wdbSlipPktDev.wdbDrvIf))
	return (ERROR);
#if (CPU==SIMHPPA)
    sysSerialWDBSetup ();
#endif /* CPU==SIMHPPA */
    }
#endif	/* (WDB_COMM_TYPE == WDB_COMM_SERIAL) */

#if     (WDB_COMM_TYPE == WDB_COMM_NETROM)
    {
    /* netrom packet driver - supports task or external agent */

    int dpOffset;				/* offset of dualport RAM */
    static WDB_NETROM_PKT_DEV	wdbNetromPktDev; /* NETROM packet device */

    dpOffset = (WDB_NETROM_ROMSIZE - DUALPORT_SIZE) * WDB_NETROM_WIDTH;

    wdbNetromPktDevInit (&wdbNetromPktDev, (caddr_t)ROM_BASE_ADRS + dpOffset,
			 WDB_NETROM_WIDTH, WDB_NETROM_INDEX,
			 WDB_NETROM_NUM_ACCESS, udpRcv,
			 WDB_NETROM_POLL_DELAY);

    if (udpCommIfInit (pCommIf, &wdbNetromPktDev.wdbDrvIf))
	return (ERROR);
    }
#endif  /* (WDB_COMM_TYPE == WDB_COMM_NETROM) */

#if     (WDB_COMM_TYPE == WDB_COMM_VTMD)
    {
    /* vision packet driver - supports task or external agent */

    static WDB_VISION_PKT_DEV wdbVisionPktDev; /* custom packet device */

    wdbVisionPktDevInit (&wdbVisionPktDev, udpRcv);

    if (udpCommIfInit (pCommIf, &wdbVisionPktDev.wdbDrvIf) == ERROR)
	return (ERROR);
    }
#endif  /* (WDB_COMM_TYPE == WDB_COMM_VTMD) */

#if     (WDB_COMM_TYPE == WDB_COMM_CUSTOM)
    {
    /* custom packet driver - supports task or external agent */

    static WDB_CUSTOM_PKT_DEV	wdbCustomPktDev; /* custom packet device */

    wdbCustomPktDevInit (&wdbCustomPktDev, udpRcv);
    if (udpCommIfInit (pCommIf, &wdbCustomPktDev.wdbDrvIf) == ERROR)
	return (ERROR);
    }
#endif  /* (WDB_COMM_TYPE == WDB_COMM_CUSTOM) */


#if	(WDB_COMM_TYPE == WDB_COMM_END)
    {
    /* END agent - supports a network MUX/END agent */
    static WDB_END_PKT_DEV wdbEndPktDev; /* END packet device */
    END_TBL_ENTRY * pDevTbl;
    END_OBJ * pEnd = NULL;
    char devName[END_NAME_MAX];

    /* find the END Device Table entry corresponding to the boot device */

    for(pDevTbl = endDevTbl; pDevTbl->endLoadFunc != NULL; pDevTbl++)
	{
	/* get the name of the device by passing argument devName = '\0' */
    	
	bzero (devName, END_NAME_MAX);
	if (pDevTbl->endLoadFunc(devName, NULL) != 0)
	    {
	    if (_func_logMsg != NULL)
		_func_logMsg ("could not get device name!\n",0,0,0,0,0,0);
	    return (ERROR);
	    }

	/* compare the name of the device to the boot device selected */

	if (strncmp (sysBootParams.bootDev, (const char *) devName,
		     strlen((const char *) devName)) == 0)
	    {
	    /* Verify that the device unit number matches */ 

	    if (pDevTbl->unit == sysBootParams.unitNum)
		break;
	    }
	}

    /* if no END Device found, default to first valid table entry */

    if (pDevTbl->endLoadFunc == NULL)
	{
	if (endDevTbl->endLoadFunc == NULL)
	    {
	    if (_func_logMsg != NULL)
		_func_logMsg ("no device in END device table!\n", 0, 0, 0,
			      0, 0, 0);
	    return (ERROR);
	    }
	else
	    pDevTbl = endDevTbl;
	}

    /* Check END device initialization by netLibInit */ 

    if (!pDevTbl->processed)
        {
	if (_func_logMsg != NULL)
	    _func_logMsg ("Network END device not initialized!\n",
	    		  0, 0, 0, 0, 0, 0);
	return (ERROR);
        }

    /* get the END_OBJ by name and unit */

    pEnd = endFindByName (devName, pDevTbl->unit);

    if (pEnd == NULL)
	{
	if (_func_logMsg != NULL)
	    _func_logMsg ("Could not find device %s unit %d!\n",
			  sysBootParams.bootDev, sysBootParams.unitNum,
			  0, 0, 0, 0);
            return (ERROR);
	}

    if (wdbEndPktDevInit(&wdbEndPktDev, udpRcv,
                     (char *)pEnd->devObject.name,
                     pEnd->devObject.unit) == ERROR)
	return (ERROR);

    if (udpCommIfInit(pCommIf, &wdbEndPktDev.wdbDrvIf) == ERROR)
	return (ERROR);
    }

#endif	/* (WDB_COMM_TYPE == WDB_COMM_END) */

#if     (WDB_COMM_TYPE == WDB_COMM_PIPE)
    {
    static WDB_PIPE_PKT_DEV wdbPipePktDev; /* Pipe packet device */

    if (wdbPipePktDevInit(&wdbPipePktDev, udpRcv) == ERROR)
	return (ERROR);
    if (udpCommIfInit(pCommIf, &wdbPipePktDev.wdbDrvIf) == ERROR)
	return (ERROR);
    }
#endif  /* (WDB_COMM_TYPE == WDB_COMM_PIPE) */

    /*
     * Install the agents communication interface and RPC transport handle.
     * Currently only one agent will be active at a time, so both
     * agents can share the same communication interface and XPORT handle.
     */

    wdbRpcXportInit  (&wdbXport, pCommIf, (char *)wdbInBuf,
		      (char *)wdbOutBuf, WDB_MTU);
    wdbInstallCommIf (pCommIf, &wdbXport);

#if (CPU==SIMHPPA) && defined(INCLUDE_SLIP) && \
			(WDB_COMM_TYPE == WDB_COMM_NETWORK)
    {
    UNIX_CHAN *pChan = (UNIX_CHAN *) sysSerialChanGet (SLIP_TTY);

    /*
     * Begin polling for connection request from tgtsvr.  This is necessary
     * because an earlier connection may have left the pipe in a state in
     * which SIGIO will not be generated until pipe is read.
     */

    wdSlipConnect = wdCreate ();
    usrWdbPollForConnect (FD_TO_IVEC (pChan->u_fd));
    }
#endif	/* (CPU==SIMHPPA) && defined(INCLUDE_SLIP) ... */

    return (OK);
    }

/******************************************************************************
*
* wdbMbufInit - initialize the agent's mbuf memory allocator.
*
* wdbMbufLib manages I/O buffers for the agent since the agent
* can't use malloc().
*
* If the agent is ever hooked up to a network driver that uses standard
* MGET/MFREE for mbuf managment, then the routines wdbMBufAlloc()
* and wdbMBufFree() below should be changed accordingly.
*/ 

LOCAL void wdbMbufInit (void)
    {
    static struct mbuf mbufs[NUM_MBUFS];
    static CL_BLK      wdbClBlks [WDB_NUM_CL_BLKS];
    bufPoolInit (&wdbMbufPool, (char *)mbufs, NUM_MBUFS, sizeof (struct mbuf));
    bufPoolInit (&wdbClBlkPool, (char *)wdbClBlks, WDB_NUM_CL_BLKS,
                 sizeof (CL_BLK));
    }

/******************************************************************************
*
* wdbMbufAlloc - allocate an mbuf
*
* RETURNS: a pointer to an mbuf, or NULL on error.
*/ 

struct mbuf *	wdbMbufAlloc (void)
    {
    struct mbuf * pMbuf;
    CL_BLK_ID	  pClBlk;

    pMbuf = (struct mbuf *)bufAlloc (&wdbMbufPool);

    if (pMbuf == NULL)
        return (NULL); 

    pClBlk = (CL_BLK_ID) bufAlloc (&wdbClBlkPool);
    
    if (pClBlk == NULL)
        {
        wdbMbufFree (pMbuf);
	return (NULL);
        }

    pMbuf->m_next	= NULL;
    pMbuf->m_nextpkt	= NULL;
    pMbuf->m_flags	= 0;
    pMbuf->pClBlk 	= pClBlk;
    return (pMbuf);
    }

/******************************************************************************
*
* wdbMbufFree - free an mbuf
*/ 

void wdbMbufFree
    (
    struct mbuf *	pMbuf		/* mbuf chain to free */
    )
    {
    /* if it is a cluster, see if we need to perform a callback */

    if (pMbuf->m_flags & M_EXT)
	{
	if (--(pMbuf->m_extRefCnt) <= 0)
            {
            if (pMbuf->m_extFreeRtn != NULL)
                {
                (*pMbuf->m_extFreeRtn) (pMbuf->m_extArg1, pMbuf->m_extArg2, 
                                        pMbuf->m_extArg3);
                }
            /* free the cluster blk */
            bufFree (&wdbClBlkPool, (char *) pMbuf->pClBlk);
            }
	}

    bufFree (&wdbMbufPool, (char *)pMbuf);
    }

#if (CPU==SIMHPPA) && defined(INCLUDE_SLIP) && \
				(WDB_COMM_TYPE == WDB_COMM_NETWORK)
/******************************************************************************
*
*  usrWdbPollForConnect - poll SLIP input pipe
*
* INTERNAL
* This routine is not normally part of a BSP.
*
* RETURNS: N/A
*
* NOMANUAL
*/
LOCAL void usrWdbPollForConnect
    (
    int slipIntr                                /* SLIP interrupt number */
    )
    {
    intCatch (slipIntr);                        /* fake I/O interrupt */

    if (wdbTargetIsConnected ())
	{
	wdDelete (wdSlipConnect);               /* stop polling */
	s_asyncio (0);
	s_asyncio (1);
	}
    else
	wdStart (wdSlipConnect, sysClkRateGet() * 3,
			(FUNCPTR) usrWdbPollForConnect, slipIntr);
    }
#endif /* (CPU==SIMHPPA) && defined(INCLUDE_SLIP) */
