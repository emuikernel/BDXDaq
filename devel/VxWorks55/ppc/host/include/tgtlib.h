/* tgtlib.h - host-based target agent header */

/* Copyright 1994-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
02r,04oct01,c_c  Added tgtToolNameGet API.
02q,18sep01,c_c  Changed tgtInit prototype.
02p,26jun98,pcn  asynchronous loader implementation.
02o,02jun98,dbt  added tgtContextStatusGet() prototype.
02n,25mar98,dbt  added contextStatusGetRtn in TGT_OPS.
02m,06mar98,c_c  Removed Log ref. in tgtAttach.
02l,02mar98,pcn  WTX 2: changed tgtAttach, tgtEvtptAdd. Added tgtWidthRead,
                 tgtWidthWrite.
02k,21jan98,c_c  DLLized Target Server Implementation.
02j,09aug96,elp	 Added tgtSymTblIdGet(), tgtRestart(), tgtSvrKill() (SPR# 6943),
		 + added symTblSync flag in tgtInit() interface (SPR# 6775).
02i,05jul96,p_m  Added tgtModeGetRtn in tgtOps (SPR# 6200).
02h,21mar96,pad  Removed tgtEvtPendingClear(). This routine is now obsolete
		 (see SPR #6203).
02g,18jan96,p_m	 added tgtCallByCName() and tgtVioDevOpen() prototype.
02f,08jan96,p_m  added tgtConnect() prototype (SPR# 4805).
		 added tgtSvrNameGet()
02e,02jan96,p_m  removed ";" in #define (SPR 5653).
02d,23oct95,jcf  added tgtMemGet().  changed tgtDirectCall() for varargs.
02c,10aug95,p_m  changed tgtMemMove() to tgtMove() (SPR# 4532).
02b,01jun95,tpr  changed tgtAttach () prototype.
02a,01jun95,p_m  changed tgtFuncCall() parameter to WTX_MSG_CONTEXT_DESC.
01z,30may95,p_m  completed WTX_MEM_SCAN and WTX_MEM_MOVE implementation.
01y,26may95,p_m  changed memScanRtn prototype.
01x,19may95,tpr  changed TGT_OPS function prototypes according new WDB protocol
		 added "host.h".  Changed bcknd by bkend.
          + p_m  get rid of rpc types and rpc includes.
01w,02may95,pad  modified to match configurable loader scheme.
01v,09mar95,p_m  added modeSetRtn in tgtOps. added tgtAgentModeSet() prototype.
01u,06mar95,p_m  took care of new agent.
01t,01mar95,tpr  added memScanRtn, callStackGetRtn, memProtectRtn,
		 cacheTextUpdate and memMoveRtn field in the TGT_OPS structure.
		 removed memCtxtSetRtn field in the TGT_OPS structure.
		 added CALL_STACK in TGT_OUT_ARGS.
		 added tgtMemScan (), tgtCallStackGet (), tgtMemMove () and
		 tgtMemProtect () prototype.
		 removed S_tgtLib_NO_CONNECTION and #include "vwModNum.h"
01s,01mar95,p_m  added tgtDisconnect() prototype.
01r,28feb95,pad  modified tgtInit() prototype (added coreNoSyms parameter).
01q,22feb95,tpr  added TGT_OUT_ARGS type definition.
		 added tgtWdbServiceCall () prototype.
		 added serviceCallRtn field in the TGT_OPS structure.
		 added #include "rpc/xdr.h"  #include "rpc/clnt.h" and
		       #include "rpc/auth.h".
01p,07feb95,p_m  added tgtTextProtect(), tgtCacheTextUpdate() and 
		 tgtDirectCallByName() prototypes.  removed non-ansi prototypes.
		 replaced tgtZero() by tgtSet().
		 changed some return type to STATUS instead of RESULT_STATUS.
	    tpr	 removed memReadZRtn and memWriteZRtn field in TGT_OPS.
01o,06feb95,tpr  replaced BCKND_OPS with TGT_OPS.
		 replaced FUNCPTR in TGT_OPS with real function prototype.
01n,23jan95,p_m  replaced #include "wdb/wdbtypes.h" by #include "wdbtypes.h".
		 changed tgtRegsGet() and tgtRegsSet() prototypes.
		 changed tgtSMemZero() to tgtSMemFill ().
		 added tgtDirectCall().
		 replaced memZeroRtn field by memFillRtn in BCKND_OPS.
		 added directCallRtn field in BCKND_OPS.
01m,20jan95,jcf  made more portable.
01l,24nov94,p_m  added tgtNameSet() and tgtNameGet() prototypes. cleanup.
01k,22nov94,tpr  changed tgtAttach() prototype.
01j,14nov94,tpr  added tgtSVioWrite () prototype.
01i,14nov94,tpr  changed tgtVioWrite() and tgtVioRead() prototype.
01h,13nov94,tpr  added tgtVioCreate(), tgtVioWrite(), tgtSAddService,
		 tgtVioRead(), tgtContextResume() and tgtAddService() prototype.
		 re-organized the prototype list.
01g,11nov94,tpr  changed tgtGopherEval() and tgtEvtptDelete() prototype.
01f,10nov94,tpr  added tgtGopherEval() prototype.
01e,09nov94,tpr  added tgtEvtNotifyFdGet() prototype.
01d,08nov94,tpr  changed #include "wdbtypes.h" to #include "wdb/wdbtypes.h".
01c,24oct94,tpr  added tgtEvtptDelete(), tgtEvtptAdd(), tgtEvtGet(),
		 tgtFuncCall(), tgtWriteMany() and tgtWriteManyInts().
		 changed all contextSourceStep to contextStep.
		 added evaluateGopherRtn, memWriteManyIntsRtn ,memWriteManyRtn
		 funcCallRtn, contextResumeRtn, bckndEvtPendingRtn and
		 bckndEvtPendingClearRtn field  in BCKBD_OPS.
		 changed eventPointAddRtn and eventPointDeleteRtn to
		 eventpointAddRtn and eventpointDeleteRtn.
		 removed vioRedirectRtn.
		 changed tgtEvtptDelete input argument type.
01b,19oct94,tpr  added tgtZero(), tgtChecksun(), tgtRead (), tgtWrite(),
		 tgtContextCreate(), tgtSMemZero(), tgtSMemRead() and
		 tgtSMemWrite() prototypes.
01a,22sep94,tpr  written.
*/

#ifndef __INCtgtlibh
#define __INCtgtlibh

#ifdef __cplusplus
extern "C" {
#endif

#include "host.h"
#include "vmutex.h"
#include "tgtmem.h"
#include "symLib.h"
#include "symbol.h"
#include "wtxmsg.h"
#include "wdb.h"
#include "asyncldlib.h"
#ifndef WIN32
#include "win32ThreadLib.h"
#endif

/* status codes */

/* defines */

#define TGT_LINK_UNKNOWN	 0x00		/* unknown link */
#define	TGT_LINK_ETHERNET_RPC	 0x01		/* ethernet link with RPC */
#define	TGT_LINK_SERIAL_RPC	 0x02		/* serial link with RPC */
						/* nb 3 to 0xfe are reserved */
						/* for future link with RPC */
#define	TGT_LINK_UNKNOWN_RPC	 0xff		/* unknown link with RPC */

#define TGT_LINK_ETHERNET	 0x100		/* ethernet link */
#define	TGT_LINK_SERIAL		 0x101		/* serial link */
#define	TGT_LINK_NETROM		 0x102		/* net_rom link */
#define	TGT_LINK_HP64700	 0x104		/* link with HP64700 emulator */
#define TGT_LINK_PARALLEL	 0x105		/* PARALLEL link */
#define TGT_LINK_SCSI_1		 0x106		/* SCSI 1 link */
#define	TGT_LINK_SCSI_2		 0x107		/* SCSI 2 link */

#define TGT_LINK_USER_START	 0x200		/* user link, start number */


#define SPEED_UNKNOWN		 0		/* link speed unknown */
#define SPEED_ETHERNET		 10000000	/* ethernet speed 10M bits/s */
#define SPEED_9600_BAUD		 9600		/* 9600 baud */
#define SPEED_38400_BAUD	 38400		/* 38400 baud */
#define SPEED_50_BAUD		 50		/* 50 baud */
#define SPEED_75_BAUD		 75		/* 75 baud */
#define SPEED_110_BAUD		 110		/* 110 baud */
#define SPEED_134_BAUD		 134		/* 134 baud */
#define SPEED_150_BAUD		 150		/* 150 baud */
#define SPEED_200_BAUD		 200		/* 200 baud */
#define SPEED_300_BAUD		 300		/* 300 baud */
#define SPEED_600_BAUD		 600		/* 600 baud */
#define SPEED_1200_BAUD		 1200		/* 1200 baud */
#define SPEED_1800_BAUD		 1800		/* 1800 baud */
#define SPEED_2400_BAUD		 2400		/* 2400 baud */
#define SPEED_4800_BAUD		 4800		/* 4800 baud */
#define SPEED_9600_BAUD		 9600		/* 9600 baud */
#define SPEED_19200_BAUD	 19200		/* 19200 baud */
#define SPEED_38400_BAUD	 38400		/* 38400 baud */

/* typedefs */

typedef struct
    {
    char *	name;		/* target/host link name */
    UINT32	type;		/* target/host link type */
    UINT32	speed;		/* target/host link speed */
    } TGT_LINK_DESC;

typedef struct tgt_ops	/* TGT_OPS */
    {
    BOOL	    tgtConnected;		    /* target is connected */
    TGT_LINK_DESC   tgtLink;			    /* target/host link desc. */
    UINT32	    tgtSupInfo;			    /* sup. target info */
    int		    tgtEventFd;			    /* file descriptor */	

    UINT32 (*tgtPingRtn)	(void);
    UINT32 (*tgtConnectRtn)	(WDB_TGT_INFO *);
    UINT32 (*tgtDisconnectRtn)	(void);
    UINT32 (*tgtModeSetRtn)	(UINT32 *);
    UINT32 (*tgtModeGetRtn)	(UINT32 *);

    BOOL   (*freeResultArgsRtn) (void);

    UINT32 (*memReadRtn)	(WDB_MEM_REGION *, WDB_MEM_XFER *);
    UINT32 (*memWriteRtn)	(WDB_MEM_XFER * pMemXfer);    
    UINT32 (*memFillRtn)	(WDB_MEM_REGION *);
    UINT32 (*memMoveRtn)	(WDB_MEM_REGION *);
    UINT32 (*memChecksumRtn)	(WDB_MEM_REGION *, UINT32 *);
    UINT32 (*memProtectRtn)	(WDB_MEM_REGION *);
    UINT32 (*memTxtUpdateRtn)	(WDB_MEM_REGION *);
    UINT32 (*memScanRtn)	(WDB_MEM_SCAN_DESC *, TGT_ADDR_T *);

/*    STATUS (*memWriteManyRtn)	(MANY_DATA *); XXX TPR */
/*    STATUS (*memWriteManyIntsRtn) (MANY_INTS *);  */


    UINT32 (*contextCreateRtn)	(WDB_CTX_CREATE_DESC *, UINT32 *);
    UINT32 (*contextKillRtn)	(WDB_CTX *);
    UINT32 (*contextSuspendRtn)	(WDB_CTX *);
    UINT32 (*contextResumeRtn)	(WDB_CTX *);

    UINT32 (*regsGetRtn)	(WDB_REG_READ_DESC *, WDB_MEM_XFER *);
    UINT32 (*regsSetRtn)	(WDB_REG_WRITE_DESC *);

    UINT32 (*vioWriteRtn)	(WDB_MEM_XFER *, UINT32 *);

    UINT32 (*evtptAddRtn)	(WDB_EVTPT_ADD_DESC *, UINT32 *);
    UINT32 (*evtptDeleteRtn)	(WDB_EVTPT_DEL_DESC *);

    UINT32 (*eventGetRtn)	(WDB_EVT_DATA *);

    UINT32 (*contextContRtn)	(WDB_CTX *);
    UINT32 (*contextStepRtn)	(WDB_CTX_STEP_DESC *);

    UINT32 (*funcCallRtn)	(WDB_CTX_CREATE_DESC *, UINT32 *);
    UINT32 (*gopherEvalRtn)	(WDB_STRING_T *, WDB_MEM_XFER *);
    UINT32 (*directCallRtn)	(WDB_CTX_CREATE_DESC *, UINT32 *);
 
    UINT32 (*serviceCallRtn)	(u_long, FUNCPTR, char *, FUNCPTR, char *);
    BOOL   (*bkendEvtPendingRtn) (void);
    void   (*bkendEvtPendingClearRtn) (void);
    UINT32 (*contextStatusGetRtn) (WDB_CTX *, UINT32 *);
    } TGT_OPS;

typedef union tgtReturnArgs	/* union TGT_OUT_ARGS */
    {
    UINT32		status;			/* status */
    WDB_TGT_INFO	wdbTgtInfo;		/* target information */
    WDB_MEM_XFER	wdbMemXfer;		/* memory transfer */
    WDB_EVT_DATA	wdbEventData;		/* event message */
    } TGT_RETURN_ARGS;


/* function declarations */

extern STATUS tgtInit 
    (
    char *		coreFileName, 
    BOOL 		coreAllSyms, 
    BOOL 		coreNoSyms,
    char * 		objectFormat, 
    BOOL 		displayStart,
    char *		displayName, 
    BOOL 		symTblSync,
    UINT32		tgtmemOptions
    );

extern STATUS tgtAttach 
    (
    char * 		tgtName, 
    char * 		bkendName, 
    UINT32 		timeout, 
    UINT32 		recallNum, 
    char * 		ttyDevName,
    UINT32 		baudRate
    );

extern char * tgtSvrNameGet (void);

extern char * tgtNameGet (void);

extern void tgtNameSet 
    (
    char *		name
    );

extern char * tgtToolNameGet (void);

extern SYMTAB_ID tgtSymTblIdGet (void);

extern void tgtRestart(void);

extern void tgtSvrKill(void);

extern WTX_TGT_INFO * tgtInfoGet (void);

extern STATUS tgtConnect (void);

extern int tgtDisconnect (void);

extern int tgtEvtNotifyFdGet (void);

extern STATUS tgtCoreParams 
    (
    char **    		ppCoreFileName
    );

extern STATUS tgtLinkDescGet 
    (
    WTX_TGT_LINK_DESC *	pWTI
    );

extern int tgtChecksum 
    (
    REMPTR		buffer, 
    int			nBytes, 
    UINT32 *		pChecksum
    );

extern int tgtRead 
    (
    REMPTR 		source, 
    void *		destination, 
    int			nBytes
    );

extern int tgtWidthRead 
    (
    REMPTR 		source, 
    void *		destination, 
    int			nBytes,
    int			width
    );

extern int tgtWrite 
    (
    void *		source, 
    REMPTR		destination, 
    int			nBytes
    );

extern int tgtWidthWrite
    (
    void *              source,
    REMPTR              destination,
    int                 nBytes,
    int			width
    );

extern int tgtMemScan 
    (
    WTX_MSG_MEM_SCAN_DESC * pMemScan, 
    TGT_ADDR_T *	    pAdrs
    );

extern int tgtMemProtect 
    (
    char *		startAdrs, 
    int			numBytes, 
    BOOL		protect
    );

extern int tgtMemFill 
    (
    REMPTR 		source, 
    int			nBytes, 
    int			pattern
    );

extern int tgtMove 
    (
    REMPTR		source, 
    REMPTR		destination, 
    UINT32		numBytes
    );

extern int tgtWriteMany 
    (
    char * 		buffer, 
    UINT32 		numBytes
    );

extern int tgtWriteManyInts 
    (
    char * 		buffer, 
    UINT32 		numBytes
    );

extern char * tgtCommandSend 
    (
    char * 		commandString
    );

extern int tgtContextCreate 
    (
    WTX_MSG_CONTEXT_DESC * pMsgCtxDesc, 
    WTX_CONTEXT_ID_T *     pContextId
    );

extern int tgtContextKill 
    (
    WTX_MSG_CONTEXT *	pMsgCtx
    );

extern int tgtContextSuspend 
    (
    WTX_MSG_CONTEXT *	pMsgCtx
    );

extern int tgtContextResume 
    (
    WTX_MSG_CONTEXT *	pMsgCtx
    );

extern int tgtContextCont 
    (
    WTX_MSG_CONTEXT *	pMsgCtx
    );

extern int tgtContextStatusGet
    (
    WTX_MSG_CONTEXT *		pMsgCtx,
    WTX_CONTEXT_STATUS *	pContextStatus
    );

extern int tgtContextStep 
    (
    WTX_MSG_CONTEXT_STEP_DESC * pMsgCtxStep
    );

extern int tgtRegsGet 
    (
    WTX_MSG_REG_READ *	    pMsgRegRead, 
    WTX_MSG_MEM_XFER_DESC * pWtxMemXfer
    );

extern int tgtRegsSet 
    (
    WTX_MSG_REG_WRITE *	pWtxRegWrite
    );

extern int tgtVioWrite 
    (
    WTX_MSG_VIO_COPY_DESC * pMsgVioCopy, 
    UINT32 *		    pNumBytesWritten
    );

extern int tgtEvtptAdd 
    (
    WTX_MSG_EVTPT_DESC_2 * pMsgEvtpt, 
    UINT32 *		   pEvtptNum
    );

extern int tgtEvtptDelete 
    (
    UINT32 *		pEvtptId, 
    UINT32 *		pEvtptType
    );

extern void tgtEvtGet (void);

extern int tgtFuncCall 
    (
    WTX_MSG_CONTEXT_DESC * pWtxCallParams, 
    UINT32 * 		   pContextId
    );

extern int tgtDirectCall 
    (
    UINT32		option, 
    UINT32 *		pRtnVal, 
    TGT_ADDR_T		rtn, 
    ...
    );

extern int tgtMemGet 
    (
    UINT32		nbytes, 
    REMPTR *		pResult
    );

extern int tgtGopherEval 
    (
    WTX_MSG_PARAM *	  pWtxMsg, 
    WTX_MSG_GOPHER_TAPE * pWMGT
    );

extern int tgtCacheTextUpdate 
    (
    char * 		startAdrs, 
    int 		numBytes
    );

extern int tgtAgentModeSet 
    (
    WTX_MSG_PARAM *	pMode
    );

extern int tgtAgentModeGet 
    (
    WTX_AGENT_MODE_TYPE * pAgentMode
    );

extern BOOL tgtEvtPending (void);

extern int tgtReset (void);

extern STATUS tgtCallByCName 
    (
    UINT32 		option, 
    void * 		pRtnVal, 
    char * 		rtnName,
    TGT_INT_T 		arg0, 
    TGT_INT_T 		arg1, 
    TGT_INT_T 		arg2,
    TGT_INT_T 		arg3, 
    TGT_INT_T 		arg4, 
    TGT_INT_T 		arg5,
    TGT_INT_T 		arg6, 
    TGT_INT_T 		arg7, 
    TGT_INT_T 		arg8,
    TGT_INT_T 		arg9
    );

extern int tgtVioDevOpen 
    (
    UINT32 		channelNum, 
    int 		oflag
    );


/* target server flags manipulation routines */
 
extern void targetAttachedSet
    (
    BOOL	value
    );
 
extern BOOL targetAttachedGet(void);
 
extern void restartTargetServerSet
    (
    BOOL	value
    );
 
extern BOOL restartTargetServerGet (void);
 
extern void killTargetServerSet
    (
    BOOL	value
    );
 
extern BOOL killTargetServerGet (void);
 
extern STATUS tgtBkEndMgtInit(void);
 
extern STATUS tgtMgtInit
    (
    char *	svrName
    );
 
extern void tgtMgtThreadsLock(void);

extern void  asyncLoadInitialize 
    (
    DWORD			loaderThread,
    LOAD_PROGRESS_INFO *	pProgressinfo
    );

extern STATUS asyncLoadUpdate
    (
    int		newState,
    int		maxValue,
    int		currentValue
    );

#ifdef __cplusplus
}
#endif

#endif /* __INCtgtlibh */
