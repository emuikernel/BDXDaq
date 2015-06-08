/* windll.h - header for windows dll */

/* Copyright 1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,01oct96,elp	 added tsWtxObjModUndefSymAddRtn (SPR# 6775)
		 + added tgtNameGetRtn, tgtSvrNameGetRtn and tgtSymTblIdGetRtn,
		 tgtSvrKillRtn, tgtSvrRestartRtn (SPR# 6943).
01d,12jul96,pad  fixed SPR #6880: lack of closing brace for C++.
01c,13may96,pad  added more functions.
01b,26feb96,pad	 conditionally included rpc/rpc.h.
01a,29jan96,pad  written.
	   +elp
*/

#ifndef __INCwindllh
#define __INCwindllh

#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

/* includes */

#ifndef PARISC_HPUX9
#include <rpc/rpc.h>
#endif

#include "loadlib.h"
#include "tgtmem.h"
#include "wtxmsg.h"
#include "dynlklib.h"
#include "wtxxdr.h"

/*
 * These macros allow for calling functions located in the main program (tgtsvr)
 * from inside of a DLL (Windows NT/95).
 * XXX PAD: Note that this should be a temporary solution before the target
 * server is restructured to be more "DLL usage" compliant.
 */

#ifdef WIN32
#define LOAD_SEGMENTS_ALLOCATE          EXT_FUNC.load.loadSegmentsAllocateRtn
#define LOAD_CPU_FAMILY_GET             EXT_FUNC.load.loadCpuFamilyGetRtn
#define LOAD_CORE_FILE_CHECK            EXT_FUNC.load.loadCoreFileCheckRtn
#define LOAD_CORE_BUILDER_SET           EXT_FUNC.load.loadCoreBuilderSetRtn
#define LOAD_COMMON_MANAGE              EXT_FUNC.load.loadCommonManageRtn
#define LOAD_UNDEF_SYM_ADD              EXT_FUNC.load.loadUndefSymAddRtn
#define LOAD_OUTPUT_TO_FILE             EXT_FUNC.load.loadOutputToFileRtn
#define LOAD_CPU_FAMILY_NAME_GET        EXT_FUNC.load.loadCpuFamilyNameGetRtn
#define LOAD_ALIGN_GET			EXT_FUNC.load.loadAlignGetRtn
#define LOAD_FLAGS_CHECK		EXT_FUNC.load.loadFlagsCheckRtn
#define LOAD_BUFFER_FREE		EXT_FUNC.load.loadBufferFreeRtn
#define TGT_MEM_CACHE_SET               EXT_FUNC.tgt.tgtMemCacheSetRtn
#define TGT_MEM_WRITE                   EXT_FUNC.tgt.tgtMemWriteRtn
#define TGT_MEM_SET                     EXT_FUNC.tgt.tgtMemSetRtn
#define TGT_MEM_PROTECT                 EXT_FUNC.tgt.tgtMemProtectRtn
#define TGT_MEM_READ                    EXT_FUNC.tgt.tgtMemReadRtn
#define TGT_MEM_WRITE_MANY_INTS         EXT_FUNC.tgt.tgtMemWriteManyIntsRtn
#define TGT_MEM_WRITE_INT               EXT_FUNC.tgt.tgtMemWriteIntRtn
#define TGT_MEM_WRITE_SHORT             EXT_FUNC.tgt.tgtMemWriteShortRtn
#define TGT_MEM_WRITE_BYTE              EXT_FUNC.tgt.tgtMemWriteByteRtn
#define TGT_MEM_ALIGN_GET		EXT_FUNC.tgt.tgtMemAlignGetRtn
#define TGT_MEM_PART_CREATE		EXT_FUNC.tgt.tgtMemPartCreateRtn
#define TGT_MEM_PART_ALLOC		EXT_FUNC.tgt.tgtMemPartAllocRtn
#define TGT_CACHE_TEXT_UPDATE		EXT_FUNC.tgt.tgtCacheTextUpdateRtn
#define TGT_DIRECT_CALL                 EXT_FUNC.tgt.tgtDirectCallRtn
#define TGT_INFO_GET                    EXT_FUNC.tgt.tgtInfoGetRtn
#define TGT_RPC_SVC_ADD			EXT_FUNC.tgt.tgtRpcSvcAddRtn
#define TGT_SVR_NAME_GET		EXT_FUNC.tgt.tgtSvrNameGetRtn
#define TGT_NAME_GET			EXT_FUNC.tgt.tgtNameGetRtn
#define TGT_SYMTBL_ID_GET		EXT_FUNC.tgt.tgtSymTblIdGetRtn
#define TGT_RESTART			EXT_FUNC.tgt.tgtRestartRtn
#define TGT_KILL			EXT_FUNC.tgt.tgtSvrKillRtn
#define WPWR_LOG_WARN                   EXT_FUNC.wpwr.wpwrLogWarnRtn
#define WPWR_LOG_ERR                    EXT_FUNC.wpwr.wpwrLogErrRtn
#define WPWR_LOG_MSG			EXT_FUNC.wpwr.wpwrLogMsgRtn
#define WPWR_LOG_RAW			EXT_FUNC.wpwr.wpwrLogRawRtn
#define WPWR_DEBUG                      EXT_FUNC.wpwr.wpwrDebugRtn
#define WPWR_GET_ENV			EXT_FUNC.wpwr.wpwrGetEnvRtn
#define WPWR_GET_USER_NAME		EXT_FUNC.wpwr.wpwrGetUserNameRtn
#define SYM_ADD                         EXT_FUNC.sym.symAddRtn
#define SYM_FIND_BY_NAME_AND_TYPE       EXT_FUNC.sym.symFindByNameAndTypeRtn
#define SYM_FIND_BY_C_NAME              EXT_FUNC.sym.symFindByCNameRtn
#define SYM_FIND_BY_NAME		EXT_FUNC.sym.symFindByNameRtn
#define SYM_FIND_BY_VALUE		EXT_FUNC.sym.symFindByValueRtn
#define SYM_EACH                        EXT_FUNC.sym.symEachRtn
#define DYNLK_FV_BIND			EXT_FUNC.dynlk.dynlkFvBindRtn
#define DYNLK_FV_TERMINATE		EXT_FUNC.dynlk.dynlkFvTerminateRtn
#define TS_WDB_KNOWN_SVC_ADD		EXT_FUNC.tswdb.tsWdbKnownSvcAddRtn
#define TS_WTX_TOOL_ATTACH		EXT_FUNC.tswtx.tsWtxToolAttachRtn
#define TS_WTX_TOOL_DETACH		EXT_FUNC.tswtx.tsWtxToolDetachRtn
#define TS_WTX_TS_INFO_GET		EXT_FUNC.tswtx.tsWtxTsInfoGetRtn
#define TS_WTX_TS_LOCK			EXT_FUNC.tswtx.tsWtxTsLockRtn
#define TS_WTX_TS_UNLOCK		EXT_FUNC.tswtx.tsWtxTsUnlockRtn
#define TS_WTX_TARGET_RESET		EXT_FUNC.tswtx.tsWtxTargetResetRtn
#define TS_WTX_CTX_CREATE		EXT_FUNC.tswtx.tsWtxCtxCreateRtn
#define TS_WTX_CTX_KILL			EXT_FUNC.tswtx.tsWtxCtxKillRtn
#define TS_WTX_CTX_SUSPEND		EXT_FUNC.tswtx.tsWtxCtxSuspendRtn
#define TS_WTX_CTX_CONT			EXT_FUNC.tswtx.tsWtxCtxContRtn
#define TS_WTX_CTX_RESUME		EXT_FUNC.tswtx.tsWtxCtxResumeRtn
#define TS_WTX_CTX_STEP			EXT_FUNC.tswtx.tsWtxCtxStepRtn
#define TS_WTX_EVTPT_ADD		EXT_FUNC.tswtx.tsWtxEvtptAddRtn
#define TS_WTX_EVTPT_DELETE		EXT_FUNC.tswtx.tsWtxEvtptDeleteRtn
#define TS_WTX_EVTPT_LIST		EXT_FUNC.tswtx.tsWtxEvtptListRtn
#define TS_WTX_MEM_CHECKSUM		EXT_FUNC.tswtx.tsWtxMemChecksumRtn
#define TS_WTX_MEM_READ			EXT_FUNC.tswtx.tsWtxMemReadRtn
#define TS_WTX_MEM_WRITE		EXT_FUNC.tswtx.tsWtxMemWriteRtn
#define TS_WTX_MEM_SET			EXT_FUNC.tswtx.tsWtxMemSetRtn
#define TS_WTX_MEM_SCAN			EXT_FUNC.tswtx.tsWtxMemScanRtn
#define TS_WTX_MEM_MOVE			EXT_FUNC.tswtx.tsWtxMemMoveRtn
#define TS_WTX_MEM_ALLOC		EXT_FUNC.tswtx.tsWtxMemAllocRtn
#define TS_WTX_MEM_REALLOC		EXT_FUNC.tswtx.tsWtxMemReallocRtn
#define TS_WTX_MEM_ALIGN		EXT_FUNC.tswtx.tsWtxMemAlignRtn
#define TS_WTX_MEM_ADD_TO_POOL		EXT_FUNC.tswtx.tsWtxMemAddToPoolRtn
#define TS_WTX_MEM_FREE			EXT_FUNC.tswtx.tsWtxMemFreeRtn
#define TS_WTX_REGS_GET			EXT_FUNC.tswtx.tsWtxRegsGetRtn
#define TS_WTX_REGS_SET			EXT_FUNC.tswtx.tsWtxRegsSetRtn
#define TS_WTX_OPEN			EXT_FUNC.tswtx.tsWtxOpenRtn
#define TS_WTX_VIO_READ			EXT_FUNC.tswtx.tsWtxVioReadRtn
#define TS_WTX_VIO_WRITE		EXT_FUNC.tswtx.tsWtxVioWriteRtn
#define TS_WTX_VIO_FILE_LIST		EXT_FUNC.tswtx.tsWtxVioFileListRtn
#define TS_WTX_VIO_CHAN_GET		EXT_FUNC.tswtx.tsWtxVioChanGetRtn
#define TS_WTX_VIO_CHAN_RELEASE		EXT_FUNC.tswtx.tsWtxVioChanReleaseRtn
#define TS_WTX_CLOSE			EXT_FUNC.tswtx.tsWtxCloseRtn
#define TS_WTX_VIO_CTL			EXT_FUNC.tswtx.tsWtxVioCtlRtn
#define TS_WTX_OBJ_MOD_LOAD		EXT_FUNC.tswtx.tsWtxObjModLoadRtn
#define TS_WTX_OBJ_MOD_UNLOAD		EXT_FUNC.tswtx.tsWtxObjModUnLoadRtn
#define TS_WTX_OBJ_MOD_LIST		EXT_FUNC.tswtx.tsWtxObjModListRtn
#define TS_WTX_OBJ_MOD_INFO_GET		EXT_FUNC.tswtx.tsWtxObjModInfoGetRtn
#define TS_WTX_OBJ_MOD_FIND		EXT_FUNC.tswtx.tsWtxObjModFindRtn
#define TS_WTX_OBJ_MOD_UNDEF_SYM_ADD	EXT_FUNC.tswtx.tsWtxObjModUndefSymAddRtn
#define TS_WTX_SYM_TBL_INFO_GET		EXT_FUNC.tswtx.tsWtxSymTblInfoGetRtn
#define TS_WTX_SYM_LIST_GET		EXT_FUNC.tswtx.tsWtxSymListGetRtn
#define TS_WTX_SYM_FIND			EXT_FUNC.tswtx.tsWtxSymFindRtn
#define TS_WTX_SYM_ADD			EXT_FUNC.tswtx.tsWtxSymAddRtn
#define TS_WTX_SYM_REMOVE		EXT_FUNC.tswtx.tsWtxSymRemoveRtn
#define TS_WTX_EVENT_GET		EXT_FUNC.tswtx.tsWtxEventGetRtn
#define TS_WTX_REG_FOR_EVENT		EXT_FUNC.tswtx.tsWtxRegForEventRtn
#define TS_WTX_UNREG_FOR_EVENT		EXT_FUNC.tswtx.tsWtxUnRegForEventRtn
#define TS_WTX_EVENT_ADD		EXT_FUNC.tswtx.tsWtxEventAddRtn
#define TS_WTX_GOPHER_EVAL		EXT_FUNC.tswtx.tsWtxGopherEvalRtn
#define TS_WTX_DIRECT_CALL		EXT_FUNC.tswtx.tsWtxDirectCallRtn
#define TS_WTX_FUNC_CALL		EXT_FUNC.tswtx.tsWtxFuncCallRtn
#define TS_WTX_SERVICE_ADD		EXT_FUNC.tswtx.tsWtxServiceAddRtn
#define TS_WTX_CONSOLE_CREATE		EXT_FUNC.tswtx.tsWtxConsoleCreateRtn
#define TS_WTX_CONSOLE_KILL		EXT_FUNC.tswtx.tsWtxConsoleKillRtn
#define TS_WTX_MEM_INFO_GET		EXT_FUNC.tswtx.tsWtxMemInfoGetRtn
#define TS_WTX_AGENT_MODE_SET		EXT_FUNC.tswtx.tsWtxAgentModeSetRtn
#define TS_WTX_AGENT_MODE_GET		EXT_FUNC.tswtx.tsWtxAgentModeGetRtn
#define TS_WTX_OBJ_KILL			EXT_FUNC.tswtx.tsWtxObjKillRtn
#define TS_WTX_TARGET_ATTACH		EXT_FUNC.tswtx.tsWtxTargetAttachRtn
#define XDR_TGT_ADDR_T			EXT_FUNC.xdr.xdr_TGT_ADDR_T_Rtn
#define XDR_WTX_MSG_TOOL_DESC		EXT_FUNC.xdr.xdr_WTX_MSG_TOOL_DESC_Rtn
#define XDR_WTX_MSG_TOOL_ID		EXT_FUNC.xdr.xdr_WTX_MSG_TOOL_ID_Rtn
#define XDR_WTX_MSG_RESULT		EXT_FUNC.xdr.xdr_WTX_MSG_RESULT_Rtn
#define XDR_WTX_MSG_TS_INFO		EXT_FUNC.xdr.xdr_WTX_MSG_TS_INFO_Rtn
#define XDR_WTX_MSG_TS_LOCK		EXT_FUNC.xdr.xdr_WTX_MSG_TS_LOCK_Rtn
#define XDR_WTX_MSG_TS_UNLOCK		EXT_FUNC.xdr.xdr_WTX_MSG_TS_UNLOCK_Rtn
#define XDR_WTX_MSG_CONTEXT_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_CONTEXT_DESC_Rtn
#define XDR_WTX_MSG_CONTEXT		EXT_FUNC.xdr.xdr_WTX_MSG_CONTEXT_Rtn
#define XDR_WTX_MSG_CONTEXT_STEP_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_CONTEXT_STEP_DESC_Rtn
#define XDR_WTX_MSG_EVTPT_DESC		EXT_FUNC.xdr.xdr_WTX_MSG_EVTPT_DESC_Rtn
#define XDR_WTX_MSG_PARAM		EXT_FUNC.xdr.xdr_WTX_MSG_PARAM_Rtn
#define XDR_WTX_MSG_EVTPT_LIST		EXT_FUNC.xdr.xdr_WTX_MSG_EVTPT_LIST_Rtn
#define XDR_WTX_MSG_MEM_SET_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_MEM_SET_DESC_Rtn
#define XDR_WTX_MSG_MEM_READ_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_MEM_READ_DESC_Rtn
#define XDR_WTX_MSG_MEM_COPY_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_MEM_COPY_DESC_Rtn
#define XDR_MEM_COPY_ALLOC		EXT_FUNC.xdr.xdr_MEM_COPY_ALLOC_Rtn
#define XDR_WTX_MSG_MEM_SCAN_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_MEM_SCAN_DESC_Rtn
#define XDR_WTX_MSG_MEM_MOVE_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_MEM_MOVE_DESC_Rtn
#define XDR_WTX_MSG_MEM_BLOCK_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_MEM_BLOCK_DESC_Rtn
#define XDR_WTX_MSG_MEM_INFO		EXT_FUNC.xdr.xdr_WTX_MSG_MEM_INFO_Rtn
#define XDR_WTX_MSG_REG_READ		EXT_FUNC.xdr.xdr_WTX_MSG_REG_READ_Rtn
#define XDR_WTX_MSG_MEM_XFER_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_MEM_XFER_DESC_Rtn
#define XDR_WTX_MSG_REG_WRITE		EXT_FUNC.xdr.xdr_WTX_MSG_REG_WRITE_Rtn
#define XDR_WTX_MSG_OPEN_DESC		EXT_FUNC.xdr.xdr_WTX_MSG_OPEN_DESC_Rtn
#define XDR_WTX_MSG_VIO_COPY_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_VIO_COPY_DESC_Rtn
#define XDR_WTX_MSG_VIO_CTL_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_VIO_CTL_DESC_Rtn
#define XDR_WTX_MSG_VIO_FILE_LIST	EXT_FUNC.xdr.xdr_WTX_MSG_VIO_FILE_LIST_Rtn
#define XDR_WTX_MSG_LD_M_FILE_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_LD_M_FILE_DESC_Rtn
#define XDR_WTX_MSG_MODULE_LIST		EXT_FUNC.xdr.xdr_WTX_MSG_MODULE_LIST_Rtn
#define XDR_WTX_MSG_MOD_NAME_OR_ID	EXT_FUNC.xdr.xdr_WTX_MSG_MOD_NAME_OR_ID_Rtn
#define XDR_WTX_MSG_MODULE_INFO		EXT_FUNC.xdr.xdr_WTX_MSG_MODULE_INFO_Rtn
#define XDR_WTX_MSG_SYM_TBL_INFO	EXT_FUNC.xdr.xdr_WTX_MSG_SYM_TBL_INFO_Rtn
#define XDR_WTX_MSG_SYM_MATCH_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_SYM_MATCH_DESC_Rtn
#define XDR_WTX_MSG_SYM_LIST		EXT_FUNC.xdr.xdr_WTX_MSG_SYM_LIST_Rtn
#define XDR_WTX_MSG_SYMBOL_DESC		EXT_FUNC.xdr.xdr_WTX_MSG_SYMBOL_DESC_Rtn
#define XDR_WTX_MSG_EVENT_REG_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_EVENT_REG_DESC_Rtn
#define XDR_WTX_MSG_EVENT_DESC		EXT_FUNC.xdr.xdr_WTX_MSG_EVENT_DESC_Rtn
#define XDR_WTX_MSG_GOPHER_TAPE		EXT_FUNC.xdr.xdr_WTX_MSG_GOPHER_TAPE_Rtn
#define XDR_WTX_MSG_SERVICE_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_SERVICE_DESC_Rtn
#define XDR_WTX_MSG_CONSOLE_DESC	EXT_FUNC.xdr.xdr_WTX_MSG_CONSOLE_DESC_Rtn
#define XDR_WTX_MSG_KILL_DESC		EXT_FUNC.xdr.xdr_WTX_MSG_KILL_DESC_Rtn
#else
#define LOAD_SEGMENTS_ALLOCATE          loadSegmentsAllocate
#define LOAD_CPU_FAMILY_GET             loadCpuFamilyGet
#define LOAD_CORE_FILE_CHECK            loadCoreFileCheck
#define LOAD_CORE_BUILDER_SET           loadCoreBuilderSet
#define LOAD_COMMON_MANAGE              loadCommonManage
#define LOAD_UNDEF_SYM_ADD              loadUndefSymAdd
#define LOAD_OUTPUT_TO_FILE             loadOutputToFile
#define LOAD_CPU_FAMILY_NAME_GET        loadCpuFamilyNameGet
#define LOAD_ALIGN_GET			loadAlignGet
#define LOAD_FLAGS_CHECK		loadFlagsCheck
#define LOAD_BUFFER_FREE		loadBufferFree
#define TGT_MEM_CACHE_SET               tgtMemCacheSet
#define TGT_MEM_WRITE                   tgtMemWrite
#define TGT_MEM_SET                     tgtMemSet
#define TGT_MEM_PROTECT                 tgtMemProtect
#define TGT_MEM_READ                    tgtMemRead
#define TGT_MEM_WRITE_MANY_INTS         tgtMemWriteManyInts
#define TGT_MEM_WRITE_INT               tgtMemWriteInt
#define TGT_MEM_WRITE_SHORT             tgtMemWriteShort
#define TGT_MEM_WRITE_BYTE              tgtMemWriteByte
#define TGT_MEM_ALIGN_GET		tgtMemAlignGet
#define TGT_MEM_PART_CREATE		tgtMemPartCreate
#define TGT_MEM_PART_ALLOC		tgtMemPartAlloc
#define TGT_CACHE_TEXT_UPDATE		tgtCacheTextUpdate
#define TGT_DIRECT_CALL                 tgtDirectCall
#define TGT_INFO_GET                    tgtInfoGet
#define TGT_RPC_SVC_ADD			tgtRpcSvcAdd
#define TGT_SVR_NAME_GET		tgtSvrNameGet
#define TGT_NAME_GET			tgtNameGet
#define TGT_SYMTBL_ID_GET		tgtSymTblIdGet
#define TGT_RESTART			tgtRestart
#define TGT_KILL			tgtSvrKill
#define WPWR_LOG_WARN                   wpwrLogWarn
#define WPWR_LOG_ERR                    wpwrLogErr
#define WPWR_LOG_MSG			wpwrLogMsg
#define WPWR_LOG_RAW			wpwrLogRaw
#define WPWR_DEBUG                      wpwrDebug
#define WPWR_GET_ENV			wpwrGetEnv
#define WPWR_GET_USER_NAME		wpwrGetUserName
#define SYM_ADD                         symAdd
#define SYM_FIND_BY_NAME_AND_TYPE       symFindByNameAndType
#define SYM_FIND_BY_C_NAME              symFindByCName
#define SYM_FIND_BY_NAME		symFindByName
#define SYM_FIND_BY_VALUE		symFindByValue
#define SYM_EACH                        symEach
#define DYNLK_FV_BIND			dynlkFvBind
#define DYNLK_FV_TERMINATE		dynlkFvTerminate
#define TS_WDB_KNOWN_SVC_ADD		tsWdbKnownSvcAdd
#define TS_WTX_TOOL_ATTACH		tsWtxToolAttach
#define TS_WTX_TOOL_DETACH		tsWtxToolDetach
#define TS_WTX_TS_INFO_GET		tsWtxTsInfoGet
#define TS_WTX_TS_LOCK			tsWtxTsLock
#define TS_WTX_TS_UNLOCK		tsWtxTsUnlock
#define TS_WTX_TARGET_RESET		tsWtxTargetReset
#define TS_WTX_CTX_CREATE		tsWtxCtxCreate
#define TS_WTX_CTX_KILL			tsWtxCtxKill
#define TS_WTX_CTX_SUSPEND		tsWtxCtxSuspend
#define TS_WTX_CTX_CONT			tsWtxCtxCont
#define TS_WTX_CTX_RESUME		tsWtxCtxResume
#define TS_WTX_CTX_STEP			tsWtxCtxStep
#define TS_WTX_EVTPT_ADD		tsWtxEvtptAdd
#define TS_WTX_EVTPT_DELETE		tsWtxEvtptDelete
#define TS_WTX_EVTPT_LIST		tsWtxEvtptList
#define TS_WTX_MEM_CHECKSUM		tsWtxMemChecksum
#define TS_WTX_MEM_READ			tsWtxMemRead
#define TS_WTX_MEM_WRITE		tsWtxMemWrite
#define TS_WTX_MEM_SET			tsWtxMemSet
#define TS_WTX_MEM_SCAN			tsWtxMemScan
#define TS_WTX_MEM_MOVE			tsWtxMemMove
#define TS_WTX_MEM_ALLOC		tsWtxMemAlloc
#define TS_WTX_MEM_REALLOC		tsWtxMemRealloc
#define TS_WTX_MEM_ALIGN		tsWtxMemAlign
#define TS_WTX_MEM_ADD_TO_POOL		tsWtxMemAddToPool
#define TS_WTX_MEM_FREE			tsWtxMemFree
#define TS_WTX_REGS_GET			tsWtxRegsGet
#define TS_WTX_REGS_SET			tsWtxRegsSet
#define TS_WTX_OPEN			tsWtxOpen
#define TS_WTX_VIO_READ			tsWtxVioRead
#define TS_WTX_VIO_WRITE		tsWtxVioWrite
#define TS_WTX_VIO_FILE_LIST		tsWtxVioFileList
#define TS_WTX_VIO_CHAN_GET		tsWtxVioChanGet
#define TS_WTX_VIO_CHAN_RELEASE		tsWtxVioChanRelease
#define TS_WTX_CLOSE			tsWtxClose
#define TS_WTX_VIO_CTL			tsWtxVioCtl
#define TS_WTX_OBJ_MOD_LOAD		tsWtxObjModLoad
#define TS_WTX_OBJ_MOD_UNLOAD		tsWtxObjModUnLoad
#define TS_WTX_OBJ_MOD_LIST		tsWtxObjModList
#define TS_WTX_OBJ_MOD_INFO_GET		tsWtxObjModInfoGet
#define TS_WTX_OBJ_MOD_FIND		tsWtxObjModFind
#define TS_WTX_OBJ_MOD_UNDEF_SYM_ADD	tsWtxObjModUndefSymAdd
#define TS_WTX_SYM_TBL_INFO_GET		tsWtxSymTblInfoGet
#define TS_WTX_SYM_LIST_GET		tsWtxSymListGet
#define TS_WTX_SYM_FIND			tsWtxSymFind
#define TS_WTX_SYM_ADD			tsWtxSymAdd
#define TS_WTX_SYM_REMOVE		tsWtxSymRemove
#define TS_WTX_EVENT_GET		tsWtxEventGet
#define TS_WTX_REG_FOR_EVENT		tsWtxRegForEvent
#define TS_WTX_UNREG_FOR_EVENT		tsWtxUnRegForEvent
#define TS_WTX_EVENT_ADD		tsWtxEventAdd
#define TS_WTX_GOPHER_EVAL		tsWtxGopherEval
#define TS_WTX_DIRECT_CALL		tsWtxDirectCall
#define TS_WTX_FUNC_CALL		tsWtxFuncCall
#define TS_WTX_SERVICE_ADD		tsWtxServiceAdd
#define TS_WTX_CONSOLE_CREATE		tsWtxConsoleCreate
#define TS_WTX_CONSOLE_KILL		tsWtxConsoleKill
#define TS_WTX_MEM_INFO_GET		tsWtxMemInfoGet
#define TS_WTX_AGENT_MODE_SET		tsWtxAgentModeSet
#define TS_WTX_AGENT_MODE_GET		tsWtxAgentModeGet
#define TS_WTX_OBJ_KILL			tsWtxObjKill
#define TS_WTX_TARGET_ATTACH		tsWtxTargetAttach
#define XDR_TGT_ADDR_T			xdr_TGT_ADDR_T
#define XDR_WTX_MSG_TOOL_DESC		xdr_WTX_MSG_TOOL_DESC
#define XDR_WTX_MSG_TOOL_ID		xdr_WTX_MSG_TOOL_ID
#define XDR_WTX_MSG_RESULT		xdr_WTX_MSG_RESULT
#define XDR_WTX_MSG_TS_INFO		xdr_WTX_MSG_TS_INFO
#define XDR_WTX_MSG_TS_LOCK		xdr_WTX_MSG_TS_LOCK
#define XDR_WTX_MSG_TS_UNLOCK		xdr_WTX_MSG_TS_UNLOCK
#define XDR_WTX_MSG_CONTEXT_DESC	xdr_WTX_MSG_CONTEXT_DESC
#define XDR_WTX_MSG_CONTEXT		xdr_WTX_MSG_CONTEXT
#define XDR_WTX_MSG_CONTEXT_STEP_DESC	xdr_WTX_MSG_CONTEXT_STEP_DESC
#define XDR_WTX_MSG_EVTPT_DESC		xdr_WTX_MSG_EVTPT_DESC
#define XDR_WTX_MSG_PARAM		xdr_WTX_MSG_PARAM
#define XDR_WTX_MSG_EVTPT_LIST		xdr_WTX_MSG_EVTPT_LIST
#define XDR_WTX_MSG_MEM_SET_DESC	xdr_WTX_MSG_MEM_SET_DESC
#define XDR_WTX_MSG_MEM_READ_DESC	xdr_WTX_MSG_MEM_READ_DESC
#define XDR_WTX_MSG_MEM_COPY_DESC	xdr_WTX_MSG_MEM_COPY_DESC
#define XDR_MEM_COPY_ALLOC		xdr_MEM_COPY_ALLOC
#define XDR_WTX_MSG_MEM_SCAN_DESC	xdr_WTX_MSG_MEM_SCAN_DESC
#define XDR_WTX_MSG_MEM_MOVE_DESC	xdr_WTX_MSG_MEM_MOVE_DESC
#define XDR_WTX_MSG_MEM_BLOCK_DESC	xdr_WTX_MSG_MEM_BLOCK_DESC
#define XDR_WTX_MSG_MEM_INFO		xdr_WTX_MSG_MEM_INFO
#define XDR_WTX_MSG_REG_READ		xdr_WTX_MSG_REG_READ
#define XDR_WTX_MSG_MEM_XFER_DESC	xdr_WTX_MSG_MEM_XFER_DESC
#define XDR_WTX_MSG_REG_WRITE		xdr_WTX_MSG_REG_WRITE
#define XDR_WTX_MSG_OPEN_DESC		xdr_WTX_MSG_OPEN_DESC
#define XDR_WTX_MSG_VIO_COPY_DESC	xdr_WTX_MSG_VIO_COPY_DESC
#define XDR_WTX_MSG_VIO_CTL_DESC	xdr_WTX_MSG_VIO_CTL_DESC
#define XDR_WTX_MSG_VIO_FILE_LIST	xdr_WTX_MSG_VIO_FILE_LIST
#define XDR_WTX_MSG_LD_M_FILE_DESC	xdr_WTX_MSG_LD_M_FILE_DESC
#define XDR_WTX_MSG_MODULE_LIST		xdr_WTX_MSG_MODULE_LIST
#define XDR_WTX_MSG_MOD_NAME_OR_ID	xdr_WTX_MSG_MOD_NAME_OR_ID
#define XDR_WTX_MSG_MODULE_INFO		xdr_WTX_MSG_MODULE_INFO
#define XDR_WTX_MSG_SYM_TBL_INFO	xdr_WTX_MSG_SYM_TBL_INFO
#define XDR_WTX_MSG_SYM_MATCH_DESC	xdr_WTX_MSG_SYM_MATCH_DESC
#define XDR_WTX_MSG_SYM_LIST		xdr_WTX_MSG_SYM_LIST
#define XDR_WTX_MSG_SYMBOL_DESC		xdr_WTX_MSG_SYMBOL_DESC
#define XDR_WTX_MSG_EVENT_REG_DESC	xdr_WTX_MSG_EVENT_REG_DESC
#define XDR_WTX_MSG_EVENT_DESC		xdr_WTX_MSG_EVENT_DESC
#define XDR_WTX_MSG_GOPHER_TAPE		xdr_WTX_MSG_GOPHER_TAPE
#define XDR_WTX_MSG_SERVICE_DESC	xdr_WTX_MSG_SERVICE_DESC
#define XDR_WTX_MSG_CONSOLE_DESC	xdr_WTX_MSG_CONSOLE_DESC
#define XDR_WTX_MSG_KILL_DESC		xdr_WTX_MSG_KILL_DESC
#endif /* WIN32 */

/*
 * This structure type is used by the loader DLLs on Windows NT/95 to call
 * functions inside the target server code.
 * XXX PAD: Note that this should be a temporary solution before the target
 * server is restructured to be more "DLL usage" compliant.
 */

typedef struct
    {
    struct
	{
	STATUS	(*loadSegmentsAllocateRtn) (SEG_INFO *);
	int	(*loadCpuFamilyGetRtn) (void);
	STATUS	(*loadCoreFileCheckRtn) (REMPTR, void *, UINT32);
	void	(*loadCoreBuilderSetRtn) (char *);
	STATUS	(*loadCommonManageRtn) (int, char *, SYMTAB_ID, SYM_ADRS *,
					SYM_TYPE *, int, SEG_INFO *, int);
	STATUS	(*loadUndefSymAddRtn) (MODULE_ID, char *);
	STATUS	(*loadOutputToFileRtn) (char *, SEG_INFO *);
	char *	(*loadCpuFamilyNameGetRtn) (void);
	UINT32	(*loadAlignGetRtn) (UINT32 alignment, void * pAddrOrSize);
	STATUS	(*loadFlagsCheckRtn) (int loadFlags, void ** ppText,
				      void ** ppData, void ** ppBss);
	void	(*loadBufferFreeRtn) (void ** ppBuf);
	} load;

    struct
	{
	STATUS		(*tgtMemCacheSetRtn) (REMPTR, int, MEM_ATTRIB, BOOL);
	STATUS		(*tgtMemWriteRtn) (void *, REMPTR, int);
	STATUS		(*tgtMemSetRtn) (REMPTR, int, int);
	UINT		(*tgtMemAlignGetRtn) (void);
	int		(*tgtCacheTextUpdateRtn) (char *, int);
	int		(*tgtMemProtectRtn) (char *, int, BOOL);
	STATUS		(*tgtMemReadRtn) (REMPTR, void *, int);
	STATUS		(*tgtMemWriteManyIntsRtn) (REMPTR, int);
	STATUS		(*tgtMemWriteIntRtn) (REMPTR, int);
	STATUS		(*tgtMemWriteShortRtn) (REMPTR, UINT16);
	STATUS		(*tgtMemWriteByteRtn) (REMPTR, char);
	PART_ID		(*tgtMemPartCreateRtn)(REMPTR pPool, unsigned poolSize);
	REMPTR		(*tgtMemPartAllocRtn) (PART_ID partId, unsigned nBytes);
	int		(*tgtDirectCallRtn) (UINT32, UINT32 *, TGT_ADDR_T, ...);
	WTX_TGT_INFO *	(*tgtInfoGetRtn) (void);
	STATUS		(*tgtRpcSvcAddRtn) (UINT32, FUNCPTR, FUNCPTR, FUNCPTR,
					    void *);
	char *		(*tgtNameGetRtn) (void);
	char *		(*tgtSvrNameGetRtn) (void);
	SYMTAB_ID	(*tgtSymTblIdGetRtn) (void);
	void		(*tgtRestartRtn) (void);
	void		(*tgtSvrKillRtn) (void);
	} tgt;

    struct
	{
	void	(*wpwrLogWarnRtn) (char *, ...);
	void	(*wpwrLogErrRtn) (char *, ...);
	void	(*wpwrLogMsgRtn) (char *, ...);
	void	(*wpwrLogRawRtn) (char *, ...);
	void	(*wpwrDebugRtn) (char *, ...);
	char *	(*wpwrGetEnvRtn) (char *);
	char *	(*wpwrGetUserNameRtn) (int);
	} wpwr;

    struct
	{
	STATUS	 (*symFindByValueRtn) (SYMTAB_ID, UINT, char *, int *,
				       SYM_TYPE *);
	STATUS	 (*symFindByNameRtn) (SYMTAB_ID, char *, char **, SYM_TYPE *);
	STATUS	 (*symFindByCNameRtn) (SYMTAB_ID, char *, char **, SYM_TYPE *);
	SYMBOL * (*symEachRtn) (SYMTAB_ID, FUNCPTR, int);
	STATUS	 (*symAddRtn) (SYMTAB_ID, char *, char *, SYM_TYPE, UINT16);
	STATUS	 (*symFindByNameAndTypeRtn) (SYMTAB_ID, char *, char **,
					     SYM_TYPE *, SYM_TYPE, SYM_TYPE);
	} sym;

    struct 
	{
	int	(*dynlkFvBindRtn) (char *, DYNLK_FUNC *, int, void **);
	STATUS	(*dynlkFvTerminateRtn) (void *, DYNLK_FUNC *, int);
	} dynlk;

    struct 
	{
	STATUS	(*tsWdbKnownSvcAddRtn) (u_int);
	} tswdb;

    struct
	{
	WTX_MSG_TOOL_DESC *	(*tsWtxToolAttachRtn) (WTX_MSG_TOOL_DESC *);
	WTX_MSG_RESULT *	(*tsWtxToolDetachRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_TS_INFO *	(*tsWtxTsInfoGetRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_RESULT *	(*tsWtxTsLockRtn) (WTX_MSG_TS_LOCK *);
	WTX_MSG_RESULT *	(*tsWtxTsUnlockRtn) (WTX_MSG_TS_UNLOCK *);
	WTX_MSG_RESULT *	(*tsWtxTargetResetRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_RESULT *	(*tsWtxTargetAttachRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_CONTEXT *	(*tsWtxCtxCreateRtn) (WTX_MSG_CONTEXT_DESC *);
	WTX_MSG_RESULT *	(*tsWtxCtxKillRtn) (WTX_MSG_CONTEXT *);
	WTX_MSG_RESULT *	(*tsWtxCtxSuspendRtn) (WTX_MSG_CONTEXT *);
	WTX_MSG_RESULT *	(*tsWtxCtxContRtn) (WTX_MSG_CONTEXT *);
	WTX_MSG_RESULT *	(*tsWtxCtxResumeRtn) (WTX_MSG_CONTEXT *);
	WTX_MSG_RESULT *	(*tsWtxCtxStepRtn) (WTX_MSG_CONTEXT_STEP_DESC*);
	WTX_MSG_RESULT *	(*tsWtxEvtptAddRtn) (WTX_MSG_EVTPT_DESC *);
	WTX_MSG_RESULT *	(*tsWtxEvtptDeleteRtn) (WTX_MSG_PARAM *);
	WTX_MSG_EVTPT_LIST *	(*tsWtxEvtptListRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_RESULT *	(*tsWtxMemChecksumRtn) (WTX_MSG_MEM_SET_DESC *);
	WTX_MSG_MEM_COPY_DESC *	(*tsWtxMemReadRtn) (WTX_MSG_MEM_READ_DESC *);
	WTX_MSG_RESULT *	(*tsWtxMemWriteRtn) (WTX_MSG_MEM_COPY_DESC *);
	WTX_MSG_RESULT *	(*tsWtxMemSetRtn) (WTX_MSG_MEM_SET_DESC *);
	WTX_MSG_RESULT *	(*tsWtxMemScanRtn) (WTX_MSG_MEM_SCAN_DESC *);
	WTX_MSG_RESULT *	(*tsWtxMemMoveRtn) (WTX_MSG_MEM_MOVE_DESC *);
	WTX_MSG_RESULT *	(*tsWtxMemAllocRtn) (WTX_MSG_PARAM *);
	WTX_MSG_RESULT *	(*tsWtxMemReallocRtn) (WTX_MSG_MEM_BLOCK_DESC*);
	WTX_MSG_RESULT *	(*tsWtxMemAlignRtn) (WTX_MSG_MEM_BLOCK_DESC *);
	WTX_MSG_RESULT *	(*tsWtxMemAddToPoolRtn) (WTX_MSG_MEM_BLOCK_DESC *);
	WTX_MSG_RESULT *	(*tsWtxMemFreeRtn) (WTX_MSG_PARAM *);
	WTX_MSG_MEM_INFO *	(*tsWtxMemInfoGetRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_MEM_XFER_DESC *	(*tsWtxRegsGetRtn) (WTX_MSG_REG_READ *);
	WTX_MSG_RESULT *	(*tsWtxRegsSetRtn) (WTX_MSG_REG_WRITE *);
	WTX_MSG_RESULT *	(*tsWtxOpenRtn) (WTX_MSG_OPEN_DESC *);
	WTX_MSG_VIO_COPY_DESC *	(*tsWtxVioReadRtn) (WTX_MSG_VIO_COPY_DESC *);
	WTX_MSG_RESULT *	(*tsWtxVioWriteRtn) (WTX_MSG_VIO_COPY_DESC *);
	WTX_MSG_RESULT *	(*tsWtxCloseRtn) (WTX_MSG_PARAM *);
	WTX_MSG_RESULT *	(*tsWtxVioCtlRtn) (WTX_MSG_VIO_CTL_DESC *);
	WTX_MSG_RESULT *	(*tsWtxVioChanGetRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_RESULT *	(*tsWtxVioChanReleaseRtn) (WTX_MSG_PARAM *);
	WTX_MSG_VIO_FILE_LIST *	(*tsWtxVioFileListRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_LD_M_FILE_DESC *(*tsWtxObjModLoadRtn) (WTX_MSG_LD_M_FILE_DESC*);
	WTX_MSG_RESULT *	(*tsWtxObjModUnLoadRtn) (WTX_MSG_PARAM *);
	WTX_MSG_MODULE_LIST *	(*tsWtxObjModListRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_MODULE_INFO *	(*tsWtxObjModInfoGetRtn) (WTX_MSG_MOD_NAME_OR_ID*);
	WTX_MSG_MOD_NAME_OR_ID *(*tsWtxObjModFindRtn) (WTX_MSG_MOD_NAME_OR_ID*);
	WTX_MSG_RESULT *	(*tsWtxObjModUndefSymAddRtn) (WTX_MSG_SYMBOL_DESC *);
	WTX_MSG_SYM_TBL_INFO *	(*tsWtxSymTblInfoGetRtn) (WTX_MSG_PARAM *);
	WTX_MSG_SYM_LIST *	(*tsWtxSymListGetRtn) (WTX_MSG_SYM_MATCH_DESC*);
	WTX_MSG_SYMBOL_DESC *	(*tsWtxSymFindRtn) (WTX_MSG_SYMBOL_DESC *);
	WTX_MSG_RESULT *	(*tsWtxSymAddRtn) (WTX_MSG_SYMBOL_DESC *);
	WTX_MSG_RESULT *	(*tsWtxSymRemoveRtn) (WTX_MSG_SYMBOL_DESC *);
	WTX_MSG_EVENT_DESC *	(*tsWtxEventGetRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_RESULT *	(*tsWtxRegForEventRtn) (WTX_MSG_EVENT_REG_DESC *);
	WTX_MSG_RESULT *	(*tsWtxUnRegForEventRtn) (WTX_MSG_EVENT_REG_DESC*);
	WTX_MSG_RESULT *	(*tsWtxEventAddRtn) (WTX_MSG_EVENT_DESC *);
	WTX_MSG_GOPHER_TAPE *	(*tsWtxGopherEvalRtn) (WTX_MSG_PARAM *);
	WTX_MSG_RESULT *	(*tsWtxFuncCallRtn) (WTX_MSG_CONTEXT_DESC *);
	WTX_MSG_RESULT *	(*tsWtxDirectCallRtn) (WTX_MSG_CONTEXT_DESC *);
	WTX_MSG_RESULT *	(*tsWtxServiceAddRtn) (WTX_MSG_SERVICE_DESC *);
	WTX_MSG_RESULT *	(*tsWtxAgentModeSetRtn) (WTX_MSG_PARAM *);
	WTX_MSG_RESULT *	(*tsWtxAgentModeGetRtn) (WTX_MSG_TOOL_ID *);
	WTX_MSG_CONSOLE_DESC *	(*tsWtxConsoleCreateRtn) (WTX_MSG_CONSOLE_DESC *);
	WTX_MSG_RESULT *	(*tsWtxConsoleKillRtn) (WTX_MSG_PARAM *);
	WTX_MSG_RESULT *	(*tsWtxObjKillRtn) (WTX_MSG_KILL_DESC *);
	} tswtx;

    struct
	{
	bool_t  (*xdr_TGT_ADDR_T_Rtn) (XDR *, TGT_ADDR_T *);
	bool_t	(*xdr_WTX_MSG_TOOL_DESC_Rtn) (XDR *, WTX_MSG_TOOL_DESC *);
	bool_t	(*xdr_WTX_MSG_TOOL_ID_Rtn) (XDR *, WTX_MSG_TOOL_ID *);
	bool_t	(*xdr_WTX_MSG_RESULT_Rtn) (XDR *, WTX_MSG_RESULT *);
	bool_t	(*xdr_WTX_MSG_TS_INFO_Rtn) (XDR *, WTX_MSG_TS_INFO *);
	bool_t	(*xdr_WTX_MSG_TS_LOCK_Rtn) (XDR *, WTX_MSG_TS_LOCK *);
	bool_t	(*xdr_WTX_MSG_TS_UNLOCK_Rtn) (XDR *, WTX_MSG_TS_UNLOCK *);
	bool_t	(*xdr_WTX_MSG_CONTEXT_DESC_Rtn) (XDR *, WTX_MSG_CONTEXT_DESC *);
	bool_t	(*xdr_WTX_MSG_CONTEXT_Rtn) (XDR *, WTX_MSG_CONTEXT *);
	bool_t	(*xdr_WTX_MSG_CONTEXT_STEP_DESC_Rtn) (XDR *, WTX_MSG_CONTEXT_STEP_DESC *);
	bool_t	(*xdr_WTX_MSG_EVTPT_DESC_Rtn) (XDR *, WTX_MSG_EVTPT_DESC *);
	bool_t	(*xdr_WTX_MSG_PARAM_Rtn) (XDR *, WTX_MSG_PARAM *);
	bool_t	(*xdr_WTX_MSG_EVTPT_LIST_Rtn) (XDR *, WTX_MSG_EVTPT_LIST *);
	bool_t	(*xdr_WTX_MSG_MEM_SET_DESC_Rtn) (XDR *, WTX_MSG_MEM_SET_DESC *);
	bool_t	(*xdr_WTX_MSG_MEM_READ_DESC_Rtn) (XDR *, WTX_MSG_MEM_READ_DESC *);
	bool_t	(*xdr_WTX_MSG_MEM_COPY_DESC_Rtn) (XDR *, WTX_MSG_MEM_COPY_DESC *);
	bool_t	(*xdr_MEM_COPY_ALLOC_Rtn) (XDR *, WTX_MSG_MEM_COPY_DESC *);
	bool_t	(*xdr_WTX_MSG_MEM_SCAN_DESC_Rtn) (XDR *, WTX_MSG_MEM_SCAN_DESC *);
	bool_t	(*xdr_WTX_MSG_MEM_MOVE_DESC_Rtn) (XDR *, WTX_MSG_MEM_MOVE_DESC *);
	bool_t	(*xdr_WTX_MSG_MEM_BLOCK_DESC_Rtn) (XDR *, WTX_MSG_MEM_BLOCK_DESC *);
	bool_t	(*xdr_WTX_MSG_MEM_INFO_Rtn) (XDR *, WTX_MSG_MEM_INFO *);
	bool_t	(*xdr_WTX_MSG_REG_READ_Rtn) (XDR *, WTX_MSG_REG_READ *);
	bool_t	(*xdr_WTX_MSG_MEM_XFER_DESC_Rtn) (XDR *, WTX_MSG_MEM_XFER_DESC *);
	bool_t	(*xdr_WTX_MSG_REG_WRITE_Rtn) (XDR *, WTX_MSG_REG_WRITE *);
	bool_t	(*xdr_WTX_MSG_OPEN_DESC_Rtn) (XDR *, WTX_MSG_OPEN_DESC *);
	bool_t	(*xdr_WTX_MSG_VIO_COPY_DESC_Rtn) (XDR *, WTX_MSG_VIO_COPY_DESC *);
	bool_t	(*xdr_WTX_MSG_VIO_CTL_DESC_Rtn) (XDR *, WTX_MSG_VIO_CTL_DESC *);
	bool_t	(*xdr_WTX_MSG_VIO_FILE_LIST_Rtn) (XDR *, WTX_MSG_VIO_FILE_LIST *);
	bool_t	(*xdr_WTX_MSG_LD_M_FILE_DESC_Rtn) (XDR *, WTX_MSG_LD_M_FILE_DESC *);
	bool_t	(*xdr_WTX_MSG_MODULE_LIST_Rtn) (XDR *, WTX_MSG_MODULE_LIST *);
	bool_t	(*xdr_WTX_MSG_MOD_NAME_OR_ID_Rtn) (XDR *, WTX_MSG_MOD_NAME_OR_ID *);
	bool_t	(*xdr_WTX_MSG_MODULE_INFO_Rtn) (XDR *, WTX_MSG_MODULE_INFO *);
	bool_t	(*xdr_WTX_MSG_SYM_TBL_INFO_Rtn) (XDR *, WTX_MSG_SYM_TBL_INFO *);
	bool_t	(*xdr_WTX_MSG_SYM_MATCH_DESC_Rtn) (XDR *, WTX_MSG_SYM_MATCH_DESC *);
	bool_t	(*xdr_WTX_MSG_SYM_LIST_Rtn) (XDR *, WTX_MSG_SYM_LIST *);
	bool_t	(*xdr_WTX_MSG_SYMBOL_DESC_Rtn) (XDR *, WTX_MSG_SYMBOL_DESC *);
	bool_t	(*xdr_WTX_MSG_EVENT_REG_DESC_Rtn) (XDR *, WTX_MSG_EVENT_REG_DESC *);
	bool_t	(*xdr_WTX_MSG_EVENT_DESC_Rtn) (XDR *, WTX_MSG_EVENT_DESC *);
	bool_t	(*xdr_WTX_MSG_GOPHER_TAPE_Rtn) (XDR *, WTX_MSG_GOPHER_TAPE *);
	bool_t	(*xdr_WTX_MSG_SERVICE_DESC_Rtn) (XDR *, WTX_MSG_SERVICE_DESC *);
	bool_t	(*xdr_WTX_MSG_CONSOLE_DESC_Rtn) (XDR *, WTX_MSG_CONSOLE_DESC *);
	bool_t	(*xdr_WTX_MSG_KILL_DESC_Rtn) (XDR *, WTX_MSG_KILL_DESC *);
	} xdr;
    } EXT_FUNCS;

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif /* __INCwindllh */
