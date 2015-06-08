/* wtx.h - wtx protocol header file */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
03v,10dec01,sn   include demanglerTypes.h instead of demangler.h (so that safe
                 for gdb to include)
03u,04dec01,sn   added wtxDefaultDemanglerStyle
03t,05dec01,fle  added Object directory retirving to wtxCpuInfoGet ()
03s,04oct01,c_c  Added wtxToolNameGet API.
03r,11sep01,fle  added declaration of wtxEnvironRegistryOptionsGet () routine
03q,06feb01,dtr  Adding check for altivec support.
03p,25may99,fle  added WTX_SVR_TYPE enum  SPR 67367
03o,23feb99,c_c  Implemented an API to get the target IP address (SPR #25044).
03n,22sep98,l_k  Removed wtxFindExecutable.
03m,21sep98,l_k  Implement wtxFindExecutable.
03l,18sep98,pcn  Implement wtxObjModuleInfoAndPathGet.
03k,18aug98,pcn  Use WTX_MSG_EVTPT_LIST_2.
03j,20jul98,pcn  Added evtptNum and toolId in the wtxEventpointListGet return
		 list.
03i,06jul98,pcn  Removed wtxObjModuleUndefSymAdd from API.
03h,19jun98,pcn  Added wtxObjModuleLoadStart, wtxObjModuleLoadProgressReport,
		 wtxObjModuleLoadCancel.
03g,11jun98,pcn  Added an input parameter at wtxEventListGet.
03f,09jun98,jmp  added wtxAsyncResultFree and wtxAsyncEventGet().
03e,03jun98,pcn  Added 2 requests: wtxSymListByModuleNameGet and
		 wtxSymListByModuleIdGet.
03d,25may98,pcn  Changed wtxTsLock in wtxTsTimedLock, wtxEventpointList_2 in
		 wtxEventpointListGet, wtxObjModuleUnload_2 in
		 wtxObjModuleUnloadByName.
03c,30apr98,dbt  added wtxHwBreakpointAdd and wtxEventpointAdd.
03b,28apr98,pcn  Removed wtxCommandSend.
03a,23apr98,fle  added CPU name retrieving to wtxCpuInfoGet
02z,21apr98,fle  added wtxCpuInfoGet()
02y,26mar98,pcn  WTX 2: added a new parameter in wtxLogging.
02x,03mar98,fle  got rid of wtxRegistryPing() function declaration
02w,02mar98,pcn  WTX 2: moved WTX_LD_M_FILE_DESC in wtxMsg.h. Added
		 WTX_EVTPT_LIST_2 definition. Added wtxAsyncNotifyEnable,
		 wtxAsyncNotifyDisable, wtxCacheTextUpdate, wtxCommandSend,
		 wtxEventListGet, wtxEventpointList_2, wtxLogging,
		 wtxMemWidthRead, wtxMemWidthWrite, wtxObjModuleChecksum,
		 wtxObjModuleLoad_2, wtxObjModuleUnloa wtxUnregisterForEvent.
		 Changed wtxTargetRtTypeGet, wtxTargetCpuTypeGet return type.
02v,29jan98,fle  added wtxRegistryPing() declaration
		 + added WTX_REGISTRY_PING_INTERVAL definition
02u,28jan98,c_c  Packed all wtxEvtxxxStringGet routines into one.
02t,29aug97,fle  Adding the WTX_MEM_DISASSEMBLE service
		 + got rid of old C style declaration (only STDC remains)
02s,30sep96,elp  put in share/, added wtxObjModuleUndefSymAdd(),
		 wtxSymAddWithGroup() (SPR# 6775).
02r,27sep96,sjw  correct prototype for wtxTsVersionGet, was wtsTsVersionGet
02q,18sep96,dgp  doc: change spelling of alignement to alignment for API Guide
		 in wtxMemAlign
02p,05sep96,elp	 changed val in wtxMemSet() from UINT8 to UINT32 (SPR# 6984).
02o,02sep96,jmp  added wtxToolIdGet() prototypes,
		 added WTX RPC service number argument to wtxServiceAdd().
02n,30aug96,elp	 changed wpwrVersionGet() into wtxTsVersionGet().
02m,17jun96,p_m	 changed type of flags parameter in wtxFileOpen() (SPR# 4941).
02l,30may96,elp	 added wtxToolNameGet(), wpwrVersionGet(), wtxServiceAdd() and
		 wtxSymTblInfoGet() prototypes (SPR# 6502).
02k,09nov95,p_m  removed un-necessary version defines.
02h,03nov95,jmp  added wtxErrSet() prototype.
02g,25oct95,p_m  added wtxAgentModeGet() (SPR# 5231) and wtxDirectCall().
02f,26sep95,s_w  rename wtxBreakpointDelete() to wtxEventpointDelete() 
		 (SPR 4852)
02e,05sep95,s_w	 add WTX_LD_M_FILE_DESC type and change wtxObjModuleLoad()
		 prototype (for SPR 4598)
02d,08jun95,c_s  added wtxTargetAttach().
02c,02jun95,p_m  added WTX_PROTOCOL_VERSION.
02b,01jun95,p_m  replaced isText by contextType and added returnType in 
		 WTX_CONTEXT_DESC. Changed wtxFuncCall() parameters to 
		 WTX_CONTEXT_DESC.
02a,26may95,s_w  add wtxSymListGet(), wtxSymRemove(), wtxEventpointList(),
		 wtxMemScan(), wtxVioFileList(), wtxContextExitNotifyAdd()
		 and correct the K&R prototype for wtxStrToEventType().
01z,22may95,s_w  add wtxTsRestart(), wtxVioChanGet() and wtxVioChanRelease().
01y,22may95,jcf  name revision.
01x,14may95,s_w  Removed use of WTX messages in API calls and added extra
		 target info routines. Use exchange API to make calls.
01w,15may95,c_s  added wtxEndian().
01v,04may95,s_w  major re-write for cleanup of WTX C library and addition
		 of error handling routines. Also restruction of headers.
01u,03may95,p_m  changed EVT_ to WTX_EVT_ and removed EVT_CALL_GOT_EXC
		 and EVT_BREAKPOINT.
01t,20apr95,p_m  integrated more WIN32 changes. added wtxShutdown().
01s,10apr95,p_m  moved event strings from "tsevt.h" and added EVT_TOOL_MSG.
		 added wtxMsgErrSet() that was removed by mistake.
01r,09apr95,s_w  correct prototype for wtxErrClear and wtxToolAttachHookSet
01q,09apr95,p_m  added missing prototypes and WTXC_EVT_TYPE.
01p,08apr95,p_m  completed C API.
01o,03mar95,p_m  added wtxMsgErrSet().
01n,28feb95,p_m  added WTX_MAX_SYM_CNT.
01m,23feb95,pad  changed WTX_MAX_SERVICE_CNT to 500.
01l,17feb95,p_m  changed delete to dlete in wtxExchangeInstall to please g++.
01k,08feb95,p_m  added MAX_OBJ_SEG_CNT and MAX_LD_M_SECTION_CNT taken from
		 xdrwtx.c. removed unnecessary constants.
		 made all limits prefix be WTX_MAX_.
01j,02feb95,p_m  merged all status in WTX_ERROR_T. 
01i,22jan95,jcf  added new status returns.
01h,20jan95,jcf  changed include to wtxtypes.h.
01g,20dec94,p_m  now use wtxrpc.h generated by rpcgen from wtx.x.
		 added S_wtx_AGENT_SERVICE_NOT_AVAIL.
		 added S_wtx_TARGET_NOT_ATTACHED.
01f,16nov94,p_m  added WTX_EVENT_ADD and wtxSvcInit().
		 removed S_wtx_NOT_ENOUGH_MEMORY. Changed errno numbers.
		 added S_wtx_NO_SCALABLE_AGENT and S_wtx_WIND_BASE_NOT_SET. 
01e,14nov94,p_m  changed WTX_VIO_OPEN to WTX_OPEN and WTX_VIO_CLOSE 
		 to WTX_CLOSE.  added S_wtx_INVALID_FLAGS.
01e,11nov94,p_m  changed WTX_EVAL_GOPHER to WTX_GOPHER_EVAL.
01d,27oct94,p_m  added WTX_CONTEXT_RESUME.
01c,24oct94,p_m  added WTX_FUNC_CALL_PRI.
01b,20oct94,p_m  added error codes.
01a,06oct94,p_m  written.
*/

#ifndef __INCwtxh
#define __INCwtxh	1

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HOST
#include <setjmp.h>
#include "demanglerTypes.h"
#else
#include "vxWorks.h"
#include "setjmp.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "bootLib.h"
#endif /* HOST */

#include "wtxtypes.h"
#include "wtxmsg.h"
#include "wtxerr.h"

/* defines */

#define N_DASM_INST_DEFAULT     10      /* default # of disassembled insts   */

#define WTX_ALL_EVENTS		~0	/* Number of events returned */
#define WTX_LOG_NO_LIMIT	0	/* No max size for the log file */
#define WTX_LOCK_FOREVER	0  /* Lock indefinitely the target server */

#define LOAD_MODULE_INFO_ONLY   0x1000000       /* module synchronization */

#define LOAD_IN_PROGRESS        0       /* load in progress*/
#define LOAD_DONE               1       /* load finished (good or bad) */

#define WTX_TRY(hWtx) \
{\
	WTX_HANDLER_T errHandler;\
	jmp_buf jumpBuf;\
\
	wtxErrClear (hWtx);\
	errHandler = wtxErrHandlerAdd (hWtx, &wtxErrExceptionFunc, &jumpBuf);\
	if (setjmp (jumpBuf) == 0)

#define WTX_CATCH(hWtx, errCode) else if (wtxErrGet (hWtx) == errCode)
#define WTX_CATCH_ALL(hWtx) else

#define WTX_TRY_END(hWtx) \
	wtxErrHandlerRemove (hWtx, errHandler);\
}

#define WTX_THROW(hWtx, errCode) wtxErrDispatch (hWtx, errCode)


/* typedefs */

typedef enum cpu_info		/* available cpu information types */
    {
    CPU_INFO_NONE	= 0,	/* no info needed                  */
    ARCH_DIR		= 1,	/* target architecture directory   */
    LEADING_CHAR	= 2,	/* symbols prepended character     */
    DEMANGLER		= 3,	/* target symbol demangler name    */
    CPU_NAME		= 4,	/* target CPU name                 */
    ARCH_OBJ_DIR	= 5	/* target architecture object dir  */
    } CPU_INFO;

typedef struct cpu_info_type	/* cpu info type               */
    {
    CPU_INFO	param;		/* parameter value             */
    char *	string;		/* parameter associated string */
    char *	section;	/* parameter section           */
    } CPU_INFO_TYPE;

typedef enum wtx_endian_t
    {
    WTX_ENDIAN_BIG	= 0,
    WTX_ENDIAN_LITTLE	= 1,
    WTX_ENDIAN_UNKNOWN	= WTX_ERROR
    } WTX_ENDIAN_T;

typedef struct _wtx * HWTX;		/* WTX Handle */

typedef BOOL32 (*WTX_HANDLER_FUNC) (); 


typedef struct _wtx_handler_t * WTX_HANDLER_T;


/* XXX - expect typedefs below to move into wtxtypes.h or wtxmsg.h */

typedef struct wtx_context_desc		/* context creation desc. */
    {
    WTX_CONTEXT_TYPE	contextType;	/* context type */
    WTX_RETURN_TYPE	returnType;	/* int or double */
    char *		name;		/* name	*/
    UINT32		priority;	/* priority */
    UINT32		options;	/* options */
    TGT_ADDR_T		stackBase;	/* base of stack */
    UINT32		stackSize;	/* stack size */
    TGT_ADDR_T		entry;		/* context entry point */
    INT32		redirIn;	/* redir. in file or NULL */
    INT32		redirOut;	/* redir. out file or NULL */
    TGT_ARG_T		args[WTX_MAX_ARG_CNT];	/* arguments */
    } WTX_CONTEXT_DESC;

typedef struct wtx_evtpt_list  		/* eventpoint list message */
    {
    UINT32		nEvtpt;		/* num. eventpoint in list */
    WTX_EVTPT *		pEvtpt;		/* eventpoint list */
    } WTX_EVTPT_LIST;

typedef struct wtx_evtpt_list_2		/* eventpoint list message */
    {
    UINT32              nEvtpt;         /* num. eventpoint in list */
    WTX_EVTPT_INFO *    pEvtptInfo;     /* eventpoint info list */
    } WTX_EVTPT_LIST_2;
 
typedef struct wtx_mem_info	 	/* memory information */
    {
    UINT32 	curBytesFree;		/* number of free bytes */
    UINT32 	curBytesAllocated;	/* cur. # of bytes alloc. */
    UINT32 	cumBytesAllocated;	/* cum. # of bytes alloc. */
    UINT32 	curBlocksFree;		/* cur. # of free blocks */
    UINT32 	curBlocksAlloc;		/* cur. # of blocks alloc. */
    UINT32 	cumBlocksAlloc;		/* cum. # of blocks alloc. */
    UINT32 	avgFreeBlockSize;	/* average free blocks size */
    UINT32 	avgAllocBlockSize;	/* average alloc. blocks size */
    UINT32 	cumAvgAllocBlockSize;	/* cum. avg alloc. block size */
    UINT32 	biggestBlockSize;	/* size of biggest block */
    void * 	pReserved;		/* reserved */
    } WTX_MEM_INFO;

typedef struct wtx_module_info		/* object module information */
    {
    UINT32	  	moduleId;	/* module Id */
    char *	  	moduleName;	/* module name */
    char *	  	format;		/* object file format */
    UINT32	  	group;		/* group number */
    int		  	loadFlag;	/* flags used to load mod. */
    UINT32	  	nSegments;	/* number of segments */
    OBJ_SEGMENT *	segment;	/* segment description */
    void *	  	pReserved;	/* reserved */
    } WTX_MODULE_INFO;

typedef struct wtx_module_list		/* object module list */
    {
    UINT32		numObjMod; 	/* number of module in list */
    UINT32 *	 	modIdList; 	/* list of object module id */
    } WTX_MODULE_LIST;

typedef enum wtx_svr_type		/* type of server to connect to      */
    {
    WTX_SVR_NONE	= 0,		/* no server to connect to           */
    WTX_SVR_SERVER	= 1,		/* server is a target server         */
    WTX_SVR_REGISTRY	= 2		/* server is the registry            */
    } WTX_SVR_TYPE;

typedef struct wtx_ts_info		/* TS information message */
    {
    WTX_TGT_LINK_DESC	tgtLinkDesc;	/* Target link descriptor */
    WTX_TGT_INFO	tgtInfo;	/* info obtained from Target */
    WTX_TOOL_DESC *	pWtxToolDesc;	/* info about attached Tools */
    char *		version;	/* Target Server version */
    char *		userName;	/* target server user name */
    char *		startTime;	/* target server start time */
    char *		accessTime;	/* target server access time */
    char *		lockMsg;	/* lock/authorization message */
    void *		pReserved;	/* reserved */
    } WTX_TS_INFO;

typedef struct wtx_load_report_info	/* Progress info for async. loads */
    {
    int         loadState;      /*
                                 * two value :
                                 *  LOAD_IN_PROGRESS : 
                                 *    - informations available in
                                 *      PROGRESSINFO structure.
                                 *  LOAD_DONE : 
                                 *    - informations available in
                                 *       MODULEINFO structure.
                                 */
    union       state_info
        {
        struct ld_m_file_desc		/* object module file descriptor     */
	    {
	    char *              filename;       /* object file name          */
	    INT32               loadFlag;       /* behavior control flags    */
	    UINT32              moduleId;       /* module identifier         */
	    UINT32              nSections;      /* number of sections        */
	    LD_M_SECTION *      section;        /* section description       */
	    WTX_SYM_LIST        undefSymList;   /* list of undefined symbols */
	    } WTX_LD_M_FILE_DESC;

        struct progress_info
            {
            int         state;          /* Current State                  */
            int         maxValue;       /* Maximum value for this state   */
            int         currentValue;   /* current value for this state   */
            }   PROGRESS_INFO;
        }       STATE_INFO;
    }   WTX_LOAD_REPORT_INFO;

/* globals */

#ifndef HOST
/* externals */

extern BOOT_PARAMS	sysBootParams;
#endif /* HOST */


/* function declarations */

extern char * wtxCpuInfoGet
    (
    int		cpuNum,			/* cpu number to get info on */
    CPU_INFO	cpuInfoType		/* cpu info type to get      */
    );

extern STATUS wtxEach			/* examine each WTX-registered service*/
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	namePat,	/* reg expression to match svc name */
    const char *	typePat,	/* reg expression to match svc type */
    const char *	keyPat,		/* reg expression to match svc key */
    FUNCPTR		routine,	/* func to call for each svc entry */
    void *		arg		/* arbitrary user-supplied arg */
    );

extern WTX_DESC * wtxInfo		/* return service descriptor */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	name		/* service name to lookup */
    );

extern WTX_DESC_Q * wtxInfoQ		/* return list of registred services */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	namePat,	/* reg expression to match svc name */
    const char *	typePat,	/* reg expression to match svc type */
    const char *	keyPat		/* reg expression to match svc key */
    );

extern STATUS wtxInitialize		/* init to be called by WTX client */
    (
    HWTX *		phWtx		/* handle to use in next API calls */
    );

extern STATUS wtxProbe			/* probe to see if registry is running*/
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern WTX_DESC * wtxRegister		/* register for a service */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	name, 		/* service name, NULL to be named */
    const char *	type,		/* service type, NULL for unspecified */
    const char *	key		/* unique service key */
    );

extern STATUS wtxTerminate		/* terminate use of WTX client handle */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxTimeoutSet		/* set WTX timeout */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32		msec		/* New timeout value in milliseconds */
    );

extern STATUS wtxTimeoutGet		/* get the current timeout */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32 *		pMsec		/* Pointer to timeout value in ms */
    );

extern STATUS wtxUnregister		/* unregister for descriptor */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	name		/* service unregister, NULL for self */
    );

extern STATUS wtxAgentModeSet		/* set the mode of the target agent */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_AGENT_MODE_TYPE	agentMode	/* debug agent mode */
    );

extern WTX_AGENT_MODE_TYPE wtxAgentModeGet	/* get agent mode */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS   wtxAsyncNotifyEnable    /* start async. event notification */
    (
    HWTX                hWtx,           /* WTX API handle */
    FUNCPTR             pFunc           /* user defined hook routine */
    );

extern STATUS   wtxAsyncNotifyDisable   /* stop async. event notification */
    (
    HWTX                hWtx            /* WTX API handle */
    );

extern WTX_EVENT_DESC * wtxAsyncEventGet
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxAsyncResultFree
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_EVENT_DESC *	pEventDesc	/* pointer to struct. to free */
    );

extern UINT32 wtxBreakpointAdd		/* create a new event point */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_CONTEXT_TYPE	contextType,	/* type of context to put bp in */
    WTX_CONTEXT_ID_T	contextId,	/* associated context */
    TGT_ADDR_T		tgtAddr		/* breakpoint address */
    );

extern STATUS wtxCacheTextUpdate        /* Update the target text cache */
    (
    HWTX                hWtx,           /* WTX API handle */
    TGT_ADDR_T          addr,           /* start address  */
    UINT32              nBytes          /* how many ?     */
    );

extern STATUS wtxClientDataGet		/* get the client data */
    (
    HWTX		hWtx,		/* WTX API handle */
    void **		ppClientData	/* pointer to client data pointer */
    );

extern STATUS wtxClientDataSet		/* set client data */
    (
    HWTX		hWtx,		/* WTX API handle */
    void *		pClientData	/* value to associate with handle */
    );

extern INT32 wtxConsoleCreate		/* create a console window (UNIX) */
     (
     HWTX		hWtx,		/* WTX API handle */
     const char *	name, 		/* Name of console window */
     const char *	display,	/* Display name eg: host:0 */
     INT32 *		fdIn,		/* RETURN: input file descriptor */
     INT32 *		fdOut		/* RETURN: output file descriptor */
     );

extern STATUS wtxConsoleKill		/* kill console (UNIX) */
    (
    HWTX		hWtx,		/* WTX API handle */
    INT32		consoleId	/* id of console to destroy */
    );

extern STATUS wtxContextCont		/* continue execution of target ctxt */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_CONTEXT_TYPE	contextType,	/* type of context to continue */
    WTX_CONTEXT_ID_T	contextId	/* id of context to continue */
    );

extern WTX_CONTEXT_ID_T	wtxContextCreate	/* create a ctxt on target */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_CONTEXT_DESC *	pContextDesc	/* context descriptor */
    );

extern UINT32 wtxContextExitNotifyAdd	/* add exit evpt notification */
    (
    HWTX		hWTx, 		/* WTX API handle */
    WTX_CONTEXT_TYPE	contextType,	/* type of context */
    WTX_CONTEXT_ID_T	contextId	/* associated context */
    );

extern STATUS wtxContextKill		/* kill a target context */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_CONTEXT_TYPE	contextType,	/* type of context to kill */
    WTX_CONTEXT_ID_T	contextId	/* id of context to kill */
    );

extern STATUS wtxContextResume		/* resume execution of a target ctxt */
    (
    HWTX hWtx,				/* WTX API handle */
    WTX_CONTEXT_TYPE contextType,	/* type of context to resume */
    WTX_CONTEXT_ID_T contextId		/* id of context to resume */
    );

extern STATUS wtxContextStep		/* single step exec of target ctxt */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_CONTEXT_TYPE	contextType,	/* type of context to resume */
    WTX_CONTEXT_ID_T	contextId, 	/* id of context to step */
    TGT_ADDR_T		stepStart,	/* stepStart pc value */
    TGT_ADDR_T		stepEnd		/* stepEnd PC vale */
    );

extern STATUS wtxContextSuspend		/* suspend a target context */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_CONTEXT_TYPE	contextType,	/* type of context to suspend */
    WTX_CONTEXT_ID_T	contextId	/* id of context being suspended */
    );

extern STATUS wtxErrClear		/* clear any error for the tool */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern WTX_ERROR_T wtxErrGet		/* return the last error for a handle */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxErrSet			/* set the error code for the handle */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32		errCode		/* error value to set */
    );

extern const char * wtxErrMsgGet	/* fetch last WTX API error string */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern const char * wtxErrToMsg		/* convert error code to string */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_ERROR_T		errCode		/* error code to convert */
    );

extern STATUS wtxErrDispatch		/* dispatch error with supplied code */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_ERROR_T		errCode		/* error code to register */
    );

extern BOOL32 wtxErrExceptionFunc	/* a function to handle an error */
    (
    HWTX		hWtx,		/* WTX API handle */
    void *		pClientData,	/* pointer to a jump buffer */
    void *		pCallData	/* error code to return via setjmp() */
    );

extern WTX_HANDLER_T wtxErrHandlerAdd	/* add an error handler */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_HANDLER_FUNC	pFunc,		/* function to call on error */
    void *		pClientData	/* data to pass function */
    );

extern STATUS wtxErrHandlerRemove	/* remove error handler for WTX handle*/
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_HANDLER_T	pHandler	/* error handler */
    );

extern STATUS wtxEventAdd		/* send an event to the target server */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	event, 		/* event string to send */
    UINT32		addlDataLen,	/* length of addl data block in bytes */
    const void *	pAddlData	/* pointer to additional data */
    );

extern WTX_EVENT_DESC * wtxEventGet	/* get an event from the target */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern WTX_EVENT_NODE * wtxEventListGet /* Get all the events in one call */
    (
    HWTX                hWtx,           /* WTX API handle */
    UINT32              nEvents         /* Number of events to return */
    );

extern UINT32 wtxEventpointAdd		/* Create a new event point */
    (
    HWTX                hWtx,		/* WTX API handle */
    WTX_EVTPT_2 *	pEvtpt		/* eventpoint descriptor */
    );

extern UINT32 wtxHwBreakpointAdd	/* Create a new hardware breakpoint  */
    (
    HWTX                hWtx,		/* WTX API handle */
    WTX_CONTEXT_TYPE	contextType,	/* type of context to put bp in */
    WTX_CONTEXT_ID_T	contextId,	/* associated context */
    TGT_ADDR_T		tgtAddr,	/* breakpoint address */
    TGT_INT_T		type		/* access type (arch dependant) */
    );
extern STATUS wtxEventpointDelete	/* delete eventpoint from the target */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32		evtptId		/* ID of eventpoint to delete */
    );

extern WTX_EVTPT_LIST * wtxEventpointList  /* list evpts on target server*/
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern WTX_EVTPT_LIST_2 * wtxEventpointListGet /* list evpts on TS */ 
    (
    HWTX                hWtx            /* WTX API handle */
    );

extern STATUS wtxFileClose		/* close a file on the target server */
    (
    HWTX		hWtx,		/* WTX API handle */
    INT32		fileDescriptor	/* file to close descriptor */
    );

extern INT32 wtxFileOpen		/* open a file on the target server */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	fileName, 	/* file name */
    WTX_OPEN_FLAG	flags,		/* unix style flags */
    INT32		mode,		/* unix style mode */
    INT32		channel		/* channel id for redirection */
    );

extern WTX_CONTEXT_ID_T	wtxFuncCall	/* call a function on the target */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_CONTEXT_DESC *	pContextDesc	/* pointer to call descriptor */
    ); 

extern STATUS wtxDirectCall		/* call func on target within agent */
    (
    HWTX		hWtx,		/* WTX API handle */
    TGT_ADDR_T		entry,		/* function address */
    void *		pRetVal,	/* pointer to return value */
    TGT_ARG_T		arg0,		/* function argument 1 */
    TGT_ARG_T		arg1,		/* function argument 2 */
    TGT_ARG_T		arg2,		/* function argument 3 */
    TGT_ARG_T		arg3,		/* function argument 4 */
    TGT_ARG_T		arg4,		/* function argument 5 */
    TGT_ARG_T		arg5,		/* function argument 6 */
    TGT_ARG_T		arg6,		/* function argument 7 */
    TGT_ARG_T		arg7,		/* function argument 8 */
    TGT_ARG_T		arg8,		/* function argument 9 */
    TGT_ARG_T		arg9		/* function argument 10 */
    ); 

extern WTX_GOPHER_TAPE * wtxGopherEval	/* evaluate Gopher string on target */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	inputString	/* Gopher program to evaluate */
    );

extern STATUS wtxLogging                /* WDB/WTX logging facilities */
    (
    HWTX        hWtx,                   /* WTX API handle     */
    int         type,                   /* WDB or WTX logging */
    int         action,                 /* Logging ON or OFF  */
    char *      fileName,               /* Logging file name  */
    int         maxSize,                /* log file max size  */
    char *	filter			/* Request filter     */
    );

extern WTX_MEM_INFO * wtxMemInfoGet	/* get info about memory pool */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxMemAddToPool		/* add memory to the agent pool */
    (
    HWTX		hWtx,		/* WTX API handle */
    TGT_ADDR_T		address,	/* base of memory block to add */
    UINT32		size		/* size of memory block to add */
    );

extern TGT_ADDR_T wtxMemAlign		/* allocate aligned target memory */
    (
    HWTX		hWtx,		/* WTX API handle */
    TGT_ADDR_T		alignment, 	/* alignment boundary */
    UINT32		numBytes	/* size of block to allocate */
    );

extern TGT_ADDR_T wtxMemAlloc		/* alloc blocks in memory pool */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32		numBytes	/* size to allocate in bytes */
    );

extern UINT32 wtxMemChecksum		/* perform checksum on target memory */
    (
    HWTX		hWtx,		/* WTX API handle */
    TGT_ADDR_T		startAddr, 	/* remote addr to start checksum at */
    UINT32		numBytes	/* number of bytes to checksum */
    );

extern WTX_DASM_INST_LIST * wtxMemDisassemble	/* disassemble memory */
    (
    HWTX		hWtx,		/* WTX API handle */
    TGT_ADDR_T		startAddr,	/* Inst address to match */
    UINT32		nInst,		/* number of instructions to get */
    TGT_ADDR_T		endAddr,	/* Last address to match */
    BOOL32		printAddr,	/* if instruction's address appended */
    BOOL32		printOpcodes,	/* if instruction's opcodes appended */
    BOOL32		hexaAddr	/* for hex  adress representation */
    );

extern STATUS wtxMemFree		/* free a block of target memory */
    (
    HWTX		hWtx,		/* WTX API handle */
    TGT_ADDR_T		address		/* target mem block address to free */
    );

extern STATUS wtxMemMove		/* move a block of target memory */
    (
    HWTX		hWtx,		/* WTX API handle */
    TGT_ADDR_T		srcAddr,	/* remote addr to move from */
    TGT_ADDR_T		destAddr,	/* remote addr to move to */
    UINT32		numBytes	/* number of bytes to move */
    );

extern UINT32 wtxMemRead		/* read memory from the target */
    (
    HWTX		hWtx,		/* WTX API handle */
    TGT_ADDR_T		fromAddr,	/* Target addr to read from */
    void *		toAddr,		/* Local addr to read to */
    UINT32		numBytes	/* Bytes to read */
    );

extern UINT32 wtxMemWidthRead           /* read memory on <width> bytes */
    (
    HWTX                hWtx,           /* WTX API handle */
    TGT_ADDR_T          fromAddr,       /* Target addr to read from */
    void *              toAddr,         /* Local addr to read to */
    UINT32              numBytes,       /* Bytes to read */
    UINT8               width           /* width to read */
    );

extern TGT_ADDR_T wtxMemRealloc		/* reallocate a block of target mem */
    (
    HWTX		hWtx,		/* WTX API handle */
    TGT_ADDR_T		address, 	/* memory block to reallocate */
    UINT32		numBytes	/* new size */
    );

extern UINT32 wtxMemSet			/* set target memory to given value */
    (
    HWTX		hWtx,		/* WTX API handle */
    TGT_ADDR_T		addr,		/* remote addr to write to */
    UINT32		numBytes, 	/* number of bytes to set */
    UINT32		val		/* value to set */
    );

extern STATUS wtxMemScan		/* scan target memory for pattern */
    (
    HWTX		hWtx,		/* WTX API handle */
    BOOL32		match,		/* Match/Not-match pattern boolean */
    TGT_ADDR_T		startAddr, 	/* Target address to start scan */
    TGT_ADDR_T		endAddr,	/* Target address to finish scan */
    UINT32		numBytes, 	/* Number of bytes in pattern */
    void *		pattern,	/* Pointer to pattern to search for */
    TGT_ADDR_T *	pResult		/* Pointer to result address */
    );

extern UINT32 wtxMemWrite		/* write memory on the target */
    (
    HWTX		hWtx,		/* WTX API handle */
    void *		fromAddr,	/* Local addr to write from */
    TGT_ADDR_T		toAddr,		/* Remote addr to write to */
    UINT32		numBytes	/* Bytes to read */
    );

extern UINT32 wtxMemWidthWrite          /* write memory on the target */
    (
    HWTX                hWtx,           /* WTX API handle */
    void *              fromAddr,       /* Local addr to write from */
    TGT_ADDR_T          toAddr,         /* Remote addr to write to */
    UINT32              numBytes,       /* Bytes to read */
    UINT8               width           /* Width to read */
    );

extern STATUS wtxObjModuleChecksum      /* Checks validity of target memory */
    (
    HWTX                hWtx,           /* WTX API handle */
    INT32               moduleId,       /* Module Id      */
    char *              fileName        /* Module name    */
    );

extern char * wtxObjModuleFindName	/* find obj module name given its ID */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32		modId		/* id of module to find obj name of */
    );

extern UINT32 wtxObjModuleFindId	/* find obj module IF from name */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	name		/* object module file name */
    );

extern WTX_MODULE_INFO * wtxObjModuleInfoGet	/* give info on obj module */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32		modId		/* id of module to look for */
    );

extern WTX_MODULE_INFO * wtxObjModuleInfoAndPathGet /* give info on obj module */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32		modId		/* id of module to look for */
    );

extern WTX_MODULE_LIST * wtxObjModuleList	/* list loaded obj modules */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern WTX_LD_M_FILE_DESC * wtxObjModuleLoad	/* load a new object module */
    (
    HWTX			hWtx,		/* WTX API handle */
    WTX_LD_M_FILE_DESC *	pFileDesc	/* module descriptor */
    );

extern STATUS wtxObjModuleLoadStart		/* asynchronous load */
    (
    HWTX                        hWtx,           /* WTX API handle    */
    WTX_LD_M_FILE_DESC *        pFileDesc       /* Module descriptor */
    );

extern WTX_LOAD_REPORT_INFO * wtxObjModuleLoadProgressReport /* load status */
    (
    HWTX                        hWtx    /* WTX API handle    */
    );

extern STATUS wtxObjModuleLoadCancel	/* cancel an async. load */
    (
    HWTX                        hWtx    /* WTX API handle    */
    );

extern STATUS wtxObjModuleUnload	/* unload an obj module from target */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32		modId		/* id of module to unload */
    );

extern STATUS wtxObjModuleByNameUnload  /* Unload an obj. mod. from target */
    (
    HWTX                hWtx,           /* WTX API handle             */
    char *              name            /* Name of module to look for */
    );

extern STATUS wtxRegisterForEvent	/* send events matching expression */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	regExp		/* Regular expr to select events */
    );

extern STATUS wtxRegsGet		/* read register data from the target */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_CONTEXT_TYPE	contextType,	/* context type to get regs of */
    WTX_CONTEXT_ID_T	contextId,	/* context id to get regs of */
    WTX_REG_SET_TYPE	regSet,		/* type of register set */
    UINT32		firstByte,	/* first byte of register set */
    UINT32		numBytes,	/* number of bytes of register set */
    void *		regMemory	/* place holder for reg. values */
    );

extern STATUS wtxRegsSet		/* write to registers on the target */
    (
    HWTX		hWtx,		/* WTX API handle */
    WTX_CONTEXT_TYPE	contextType,	/* context type to set regs of */
    WTX_CONTEXT_ID_T	contextId,	/* context id to set regs of */
    WTX_REG_SET_TYPE	regSet,		/* type of register set */
    UINT32		firstByte,	/* first byte of reg. set */
    UINT32		numBytes,	/* number of bytes in reg. set. */
    void *		regMemory	/* register contents */
    );

extern void wtxSymListFree
    (
    WTX_SYM_LIST *	pSymList	/* symbol list to free */
    );

extern STATUS wtxResultFree		/* free mem allocated by WTX API call */
    (
    HWTX		hWtx,		/* WTX API handle */
    void *		pResult		/* pointer to WTX result structure */
    );

extern STATUS wtxServiceAdd		/* add a new service to the agent */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32 *		wtxSvcNum,	/* where to return WTX RPC serv num */
    UINT32		wdbSvcNum,	/* WDB RPC service number */
    char *		wdbName,	/* WDB service name */
    char *		wdbObjFile,	/* WDB service object module */
    char *		wdbInitRtn,	/* WDB svc init routine name */
    char *		wtxObjFile,	/* WTX xdr/service DLL file */
    char *		wtxName,	/* WTX service routine name */
    char *		wtxInProcName,	/* WTX service input xdr proc name */
    char *		wtxOutProcName	/* WTX service output xdr proc name */
    );

extern TGT_ADDR_T wtxStrToTgtAddr	/* convert str to a TGT_ADDR_T */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	str		/* string to convert */
    );

extern INT32 wtxStrToInt32		/* convert str to an INT32 */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	str		/* string to convert */
    );

extern WTX_CONTEXT_ID_T wtxStrToContextId	/* convert str to ctxt ID */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	str		/* string to convert */
    );

extern WTX_CONTEXT_TYPE wtxStrToContextType	/* convert str ton ctxt type */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	str		/* string to convert */
    );

extern WTX_EVENT_TYPE wtxStrToEventType	/* convert string to event type */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	str		/* string to convert */
    );

const char * wtxEventToStrType		/* convert event type to a string */
    (
    WTX_EVENT_TYPE	event		/* WTX Event */
    );

extern STATUS wtxSymAdd			/* add symbol with given params */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	symName, 	/* name of symbol to add */
    TGT_ADDR_T		symValue,	/* value of symbol to add */
    UINT8		symType		/* type of symbol to add */
    ); 

extern STATUS wtxSymAddWithGroup	/* add symbol with given group */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	symName,	/* name of symbol to add */
    TGT_ADDR_T		symValue,	/* value of symbol to add */
    UINT8		symType,	/* type of symbol to add */
    UINT16		group		/* group of symbol to add */
    ); 

extern WTX_SYMBOL * wtxSymFind		/* find info on symbol */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	symName,	/* name of symbol */
    TGT_ADDR_T		symValue,	/* value of symbol */
    BOOL32		exactName,	/* must match name exactly */
    UINT8		symType,	/* type of symbol */
    UINT8		typeMask	/* mask to select type bits */
    );

extern WTX_SYM_LIST * wtxSymListGet	/* get list of symbols */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	string,		/* string to match */
    const char *	moduleNameOrId,	/* Module name to search in */
    TGT_ADDR_T		value,		/* Symbol value to match */
    BOOL32		listUnknown	/* List unknown symbols only flag */
    );

extern WTX_SYM_LIST * wtxSymListByModuleIdGet /* get list of symbols */
    (
    HWTX                hWtx,		/* WTX API handle */
    const char *        string,		/* string to match */
    UINT32		moduleId,	/* Module Id to search in */
    TGT_ADDR_T          value,		/* Symbol value to match */
    BOOL32              listUnknown	/* List unknown symbols only flag */
    );

extern WTX_SYM_LIST * wtxSymListByModuleNameGet /* get list of symbols */
    (
    HWTX                hWtx,		/* WTX API handle */
    const char *        string,		/* string to match */
    const char *        moduleName,	/* Module name to search in */
    TGT_ADDR_T          value,		/* Symbol value to match */
    BOOL32              listUnknown	/* List unknown symbols only flag */
    );

extern STATUS wtxSymRemove		/* remove a symbol from sym table */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	symName,	/* Name of symbol to remove */
    UINT8		symType		/* Type of symbol to remove */
    );

extern WTX_SYM_TBL_INFO * wtxSymTblInfoGet	/* return sym table info */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern INT32 wtxTargetRtTypeGet		/* get the target-runtime-type info */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern const char * wtxTargetRtVersionGet	/* get target-runtime version */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern INT32 wtxTargetCpuTypeGet	/* get the target CPU type code */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern BOOL32 wtxTargetHasFppGet	/* check for floating point processor */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern BOOL32 wtxTargetHasAltivecGet	/* check for altivec processor */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern WTX_ENDIAN_T wtxTargetEndianGet	/* get edianness of target */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern const char * wtxTargetBootlineGet	/* get target boot line info */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern const char * wtxTargetBspNameGet	/* get the BSP name string */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxTargetReset		/* reset the target */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern UINT32 wtxTargetIpAddressGet	/* get the target IP address */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern char * wtxTargetToolNameGet/* get the target tool name */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxTargetAttach		/* reattach to the target */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxToolAttach		/* connect client to target server */
    (
    HWTX		hWtx,		/* WTX API handle */
    const char *	targetName,	/* Target Server name */
    const char *	toolName	/* tool name */
    );

extern char * wtxToolNameGet		/* return name of current tool */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern BOOL32 wtxToolConnected		/* check tool connection to server */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxToolDetach		/* detach from the target server */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern UINT32 wtxToolIdGet		/* return tool ID of current tool */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern WTX_TS_INFO * wtxTsInfoGet	/* get info about target and server */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxTsKill			/* kill the target server */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxTsLock			/* lock server for exclusive access */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxTsTimedLock		/* lock target server a limited time */
    (
    HWTX		hWtx,		/* WTX API handle */
    UINT32		seconds		/* duration of lock */
    );

extern const char * wtxTsNameGet	/* get target server name */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxTsRestart		/* restart the target server */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxTsUnlock		/* unlock the connected target server */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxUnregisterForEvent     /* Unregister for some events */
    (
    HWTX                hWtx,           /* WTX API handle */
    char *              regExp          /* regexp for the unwanted events */
    );

extern BOOL32 wtxVerify			/* verify WTX handle is valid for use */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern INT32 wtxVioChanGet		/* get a virtual I/O channel number */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern STATUS wtxVioChanRelease		/* release a virtual I/O channel */
    (
    HWTX		hWtx,		/* WTX API handle */
    INT32		channel		/* the channel number to release */
    );

extern STATUS wtxVioCtl			/* poerform control operation on VIO */
    (
    HWTX		hWtx,		/* WTX API handle */
    INT32		channel,	/* channel to do control operation on */
    UINT32		request,	/* control operation to perform */
    UINT32		arg		/* arg for call */
    );

extern WTX_VIO_FILE_DESC ** wtxVioFileList	/* list server files */
    (
    HWTX		hWtx		/* WTX API handle */
    );

extern UINT32 wtxVioWrite		/* write data to a VIO channel */
    (
    HWTX		hWtx,		/* WTX API handle */
    INT32		channel,	/* channel to write to */
    void *		pData,		/* pointer to data to write */
    UINT32		numBytes	/* number of bytes of data to write */
    );

extern char * wtxTsVersionGet		/* return the Tornado version */
    (
    HWTX		hWtx		/* WTX API handle */
    );

#if (defined (WIN32))
long wtxEnvironRegistryOptionsGet
    (
    HKEY	hKey,			/* Win32 registry key handler         */
    char *	dbDir,			/* Tornado registry DB directory      */
    int *	pPingDelay,		/* target server ping delay           */
    int *	pNUnsuccessPing,	/* target server max unsuccess ping   */
    BOOL *	pUsePmap		/* do we use port mapper ?            */
    );
#endif /* WIN32 */

#ifdef HOST
extern DEMANGLER_STYLE wtxDefaultDemanglerStyle  /* appropriate demangling style for target tool */
    (
    HWTX hWtx                           /* WTX API handle */
    );

extern const char * wtxTargetToolFamilyGet /* returns toolchain family (e.g. "gnu" or "diab") */
    (
    HWTX hWtx                           /* WTX API handle */
    );
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCwtxh */

