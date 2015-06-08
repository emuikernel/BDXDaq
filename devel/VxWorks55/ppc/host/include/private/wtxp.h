/* wtxp.h - wtx protocol private header file */

/*
modification history
--------------------
01s,02oct01,c_c  Added license check.
01r,07jun01,pch  moved host-only part to end of _WTX struct
01q,22jun99,fle  made wtx compile on target side
01p,27may99,fle  added async notification on wtx handle basis  SPR 67367
01o,07jul98,pcn  Added wtxObjModuleUndefSymAdd prototype.
01n,17may95,s_w  update for use of exchange api.
01m,15may95,c_s  added endian field to handle.
01l,04may95,s_w  put in private parts of wtx.h and remove message/rpc code
		 which is now in wtxmsg.h and wtxrpc.h
01k,24mar95,p_m  added WTX_MEM_ALIGN, WTX_MEM_REALLOC and WTX_MEM_ADD_TO_POOL.
01j,16mar95,p_m  added WTX_VIO_FILE_LIST.
01i,10mar95,p_m  added WTX_AGENT_MODE_SET. changed WTX_MEM_INFO_GET to 38.
01h,02mar95,p_m  changed WTX_TS_MEM_MOVE rpc number to 35.
01g,27feb95,p_m  added WTX_TS_MEM_MOVE.
01f,26feb95,jcf  added WTX_TS_[UN]LOCK.  removed WTX_OBJ_[UN]LOCK.
01e,23feb95,pad  changed WTX_FIRST_ADDL_PROC_NUM to 301.
01d,21feb95,pad  instated WTX_FIRST_ADDL_PROC_NUM.
01c,07feb95,p_m  get rid of WTX_MEM_ZERO.
01b,26jan95,p_m  added WTX_OBJ_LOCK, WTX_OBJ_UNLOCK and WTX_MEM_SET, 
	 	 changed WTX_TS_KILL to WTX_OBJ_KILL.	
01a,15jan95,p_m  written.
*/

#ifndef __INCwtxph
#define __INCwtxph	1

#ifdef __cplusplus
extern "C" {
#endif

#include "wtx.h"
#include "wtxexch.h"
#ifdef HOST
    #include "wtxAsync.h"
#endif

#include "sllLib.h"		/* singly linked lists library */

/* defines */

/* This macro registers an error and does a return with the supplied value */
#define WTX_ERROR_RETURN(handle, errCode, retVal) \
do \
    { \
    wtxErrDispatch (handle, errCode); \
    return retVal; \
    } while (0)

/*
 * This macro checks that a WTX API handle is valid and registers a
 * WTX_ERR_API_INVALID_HANDLE error if it is not.
 */
#define WTX_CHECK_HANDLE(handle, retVal) \
do \
    { \
    if (handle == 0 || (handle)->self != handle) \
	WTX_ERROR_RETURN (handle, WTX_ERR_API_INVALID_HANDLE, retVal); \
    } while (0)

/*
 * This macro checks that a WTX API handle is both valid and 
 * connected to a server. WTX_ERR_API_INVALID_HANDLE and
 * WTX_ERR_API_NOT_CONNECTED errors can be raised
 */
#define WTX_CHECK_CONNECTED(handle, retVal) \
do \
    { \
    WTX_CHECK_HANDLE (handle, retVal); \
    if ((handle)->server == NULL) \
	WTX_ERROR_RETURN (handle, WTX_ERR_API_NOT_CONNECTED, retVal); \
    } while (0)


/* typedefs */

typedef struct _wtx
    {
    HWTX		self;		/* pointer to self for validation */
    WTX_XID		server;		/* exchange for WTX server calls  */
    WTX_XID		registry;	/* exchange for WTX registry calls */
    WTX_ERROR_T		errCode;	/* last error code */
    const char *	errMsg;		/* last error message (if available) */
    WTX_HANDLER_T	errHandler;	/* handler for errors */
    WTX_MSG_TOOL_ID	msgToolId;	/* tool id message for convenience */
    WTX_TOOL_DESC *	pToolDesc;	/* tool descriptor for this tool */
    WTX_TS_INFO *	pTsInfo;	/* most recent target server info */
    WTX_DESC *		pServerDesc;	/* message descriptor for server */
    WTX_DESC *		pSelfDesc;	/* message descriptor for us */
    void *		pClientData;	/* pointer to client data */
    SL_LIST *		pWtxFreeList;	/* list of WTX pointers allocated */
#ifdef HOST
    WTX_ASYNC_HANDLE 	asyncHandle;	/* async. context handle */
    BOOL		isLicensed;	/* did this tool got a license ? */
#endif
    char *	targetToolName;		/* tool used to build target runtime */
    } _WTX;

typedef struct _wtx_handler_t
    {
    WTX_HANDLER_FUNC pFunc;
    void *	     pClientData;
    WTX_HANDLER_T    prev;
    } _WTX_HANDLER_T;

/* Functions declarations */

extern STATUS wtxObjModuleUndefSymAdd	/* add undef sym to undef sym list */
    (
    HWTX		hWtx,		/* WTX API handle */
    char *		symName,	/* undefined symbol name */
    UINT32		symGroup	/* undefined symbol group */
    );

#ifdef __cplusplus
}
#endif

#endif /* __INCwtxph */
