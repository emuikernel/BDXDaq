/* wtxtypes.h - WTX fundamental type definition */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
02o,04oct01,c_c  Added wtxToolNameGet API.
02n,09may01,dtr  Adding enums for Coprocessors bit shifts and masks.
02m,19mar01,tpw  Resolve SH/altivec conflict; add WTX_REG_SET_LAST so a stable
		 name for the count of WTX_REG_SET's is available.
02l,15jan01,dtr  Adding support for altivec register set.
02l,07mar00,zl   merged SH support from T1
02k,23feb99,c_c  Implemented an API to get the target IP address (SPR #25044).
02j,29jan99,dbt  doc: indicate which elements of WTX_CONTEXT_TYPE are
		 implemented.
02i,12jan99,dbt  indicated that WTX_ACTION_STOP_ALL is not implemented (SPR
                 #22081).
02h,18sep98,pcn  Implement wtxObjModuleInfoAndPathGet.
02g,04sep98,pcn  Added WTX_EVENT_EVTPT_ADDED, WTX_EVENT_EVTPT_DELETED.
02f,18aug98,pcn  Add WTX_SVC_TYPE description.
02e,19jun98,pcn  Added new flags for the asynchronous load.
02d,16jun98,pcn  Changed separator character from "_" to " ".
02c,24apr98,pcn  Changed WTX_ASYNC_HEADER_SIZE.
02b,03apr98,pcn  WTX 2: added WTX_EVENT_TS_KILLED used by the asynchronous
                 notification.
02a,24mar98,dbt  added WTX context status type.
01z,17mar98,pcn  WTX 2: changed WTX_ALL_MODULE_CHECK, MTU. Removed new event
                 types.
01y,12mar98,pcn  WTX 2: added new wtx_event_type.
01x,02mar98,pcn  WTX 2: defined strings for WTX_COMMAND_SEND,
                 WTX_OBJ_MODULE_CHECKSUM, OBJ_MODULE_LOAD_2, WTX_ASYNC_NOTIFY.
                 Changed WTX_EVENT_BREAKPOINT in WTX_EVENT_HW_BP.
01w,29aug97,fle  Adding WTX_MAX_DASM_INST_CNT constant.
01v,31dec96,dgp  doc: more spelling corrections
01u,06dec96,dgp  doc: correct spelling
01t,30sep96,elp  put in share/ (SPR# 6775).
01s,04sep96,dgp  doc: removed WTX_O_RDWR from WTX_OPEN_FLAG since no longer
		 supported per SPR #5959
01r,17jun96,p_m  added flags for WTX_OPEN (SPR# 4941).
01q,26may95,p_m  added WTX_MAX_PATTERN_LEN.
01p,23may95,p_m  changed WTX_CTX... and WTX_REG_SET... values.  Took care
		 of wtx_event_type comments length for manual generation.
01o,19may95,tpr  moved TGT_ADDR_T and TGT_ARG_T in host.h.
          + p_m  removed DETECTION related stuff.
01m,17may95,s_w  extended WTX_EVENT_TYPE to have all target server generated
                 events. Because WTX_EVENT_TYPE is mapped to WDB_EVENT_TYPE
		 in the target server event numbering was made to match
		 for event types 0 through 8.
01l,16may95,p_m  added object kill operations defines.
01k,09may95,p_m  removed WTX_CORE from WTX_DESC and WTX_DESC_Q.
01j,05may95,p_m  changed WTX_STATUS and WTX_REQUEST type to unsigned long.
                 added protocol version number in WTX_CORE.
01i,04may95,s_w  removed include of <rpc/rpc.h> and typedef of WTX_ID. Added
		 WTX_MAX_xxx limit defines that were in wtx.h. Remove HWTX
		 definition that belongs in wtx.h
01h,20apr95,p_m  integrated more WIN32 changes.
01g,08apr95,p_m  added HWTX and _WTX.
01f,04apr95,p_m  added WTX agent mode type.
01e,03apr95,p_m  get rid 0f 0x0 in wtx_action_type.
01d,30mar95,p_m  added new types to completely separate WTX from the 
		 agent protocols. removed #include "comtypes.h".
01c,15mar95,p_m  changed #include "vxWorks.h" to #include "host.h".
01b,10jan95,p_m  added basic types.  added #include "comtypes.h" and 
		 #include "wtxerr.h".  cleanup.
01a,20jan95,p_m  written.
*/

#ifndef __INCwtxtypesh
#define __INCwtxtypesh

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HOST
#include "host.h"		
#else
#include "vxWorks.h"
#include "wdb/wdb.h"

#define WTX_OK			0
#define WTX_ERROR		(-1)

typedef unsigned long   TGT_ARG_T;      /* target function argument */
#endif /* HOST */

#include "wtxerr.h"

/* limits */

#define WTX_MAX_SYM_CNT		256     /* maximum symbols in list */
#define WTX_MAX_DASM_INST_CNT	256     /* maximum instructions in list */
#define WTX_MAX_EVTPT_CNT	256     /* maximum eventpoints in list */
#define WTX_MAX_MODULE_CNT	1000    /* maximum modules in list */
#define WTX_MAX_SERVICE_CNT	500     /* valid service numbers: 0 to 500 */
#define WTX_MAX_ARG_CNT		10      /* maximum arguments for func calls */
#define WTX_MAX_SYM_TBL_CNT	256     /* maximum symbol tables */
#define WTX_MAX_VIO_DATA	512	/* maximum bytes per VIO transaction */
					/* this number should be the same */
					/* as the buffer size used on WDB */
					/* virtual I/O devices */
#define WTX_MAX_PATTERN_LEN	512	/* maximum length of pattern for */
					/* WTX_MEM_SCAN service */
#define WTX_MAX_FILENAME_LEN	80	/* max length of a filename */
#define WTX_MAX_OBJ_SEG_CNT	40	/* maximum num of segment in a module */
#define WTX_MAX_SECTION_CNT	40	/* maximum num of section in an */
					/* object file */

#define WTX_MAX_WDB_SVC_CNT	10	/* maximum num of WDB services in an */
					/* WDB service object module */

/* virtual i/o control operations */

#define WTX_CTL_VIO_REDIRECT	1

/* object kill operations */

#define WTX_OBJ_KILL_DESTROY	1	/* terminate */
#define WTX_OBJ_KILL_RESTART	2	/* start again */

/* Strings passed in WTX_COMMAND_SEND */

#define SEPARATOR                    " "
#define WDB_LOGGING_ON_CMD           "wdbLoggingOn"
#define WDB_LOGGING_OFF_CMD          "wdbLoggingOff"
#define WTX_LOGGING_ON_CMD           "wtxLoggingOn"
#define WTX_LOGGING_OFF_CMD          "wtxLoggingOff"
#define ALL_LOGGING_OFF_CMD          "allLoggingOff"
#define WTX_ASYNC_NOTIFY_ENABLE_CMD  "wtxAsyncNotifyEnable"
#define WTX_ASYNC_NOTIFY_DISABLE_CMD "wtxAsyncNotifyDisable"
#define WTX_OBJ_KILL_CMD             "wtxObjKill"
#define WTX_OBJ_KILL_DESTROY_CMD     "wtxObjKillDestroy"
#define WTX_OBJ_KILL_RESTART_CMD     "wtxObjKillRestart"
#define WTX_TS_LOCK_CMD              "wtxTsLock"
#define WTX_TS_UNLOCK_CMD            "wtxTsUnlock"
#define WTX_TARGET_RESET_CMD         "wtxTargetReset"
#define WTX_LOAD_CANCEL_CMD	     "wtxObjModuleLoadCancel"
#define WTX_TARGET_IP_ADDRESS_CMD    "wtxTargetIpAddressGet"
#define WTX_TARGET_TOOLNAME_GET_CMD  "wtxTargetToolNameGet"	

#define LOG_WDB     0
#define LOG_WTX     1
#define LOG_ALL     2
#define LOG_OFF     0
#define LOG_ON      1

#define WTX_ALL_MODULE_CHECK    "WTX_ALL_MODULE_CHECK"
#define WTX_ALL_MODULE_ID       1       /* For WTX_OBJ_MODULE_CHECKSUM */
#define WTX_FILE_SLICE_SIZE     8192    /* For WTX_OBJ_MODULE_LOAD_2   */

#define WTX_MTU    		1460    /* For WTX_OBJ_MODULE_LOAD_2   */
#define WTX_ASYNC_HEADER_SIZE   36      /* For WTX_ASYNC_NOTIFY        */

/* load behavior flags */

#define WTX_LOAD_FROM_CLIENT	    0x0	/* Open files on the client        */
#define WTX_LOAD_FROM_TARGET_SERVER 0x1	/* Open files on the target server */
#define WTX_LOAD_ASYNCHRONOUSLY	    0x2	/* Send load command and return    */
#define WTX_LOAD_PROGRESS_REPORT    0x4	/* Get load progress status	   */

/* load progress flags */

#define WTX_LOAD_INIT		0x1	/* Initializations are done */
#define WTX_LOAD_RELOC		0x2	/* Relocation phase         */
#define WTX_LOAD_DOWNLOAD	0x4	/* Download to the target   */
#define WTX_LOAD_DONE		0x8	/* Load done                */
#define WTX_LOAD_ERROR		0x10	/* Error during the load    */

/* WTX_OBJ_MODULE_INFO_GET constant */

#define WTX_OBJ_MODULE_PATHNAME_GET	"WTX_OBJ_MODULE_PATHNAME_GET"

/* typedefs */

typedef long		BOOL32;		/* boolean */

typedef unsigned long	WTX_STATUS;	/* wtx status value */
typedef unsigned long 	WTX_REQUEST;	/* WTX request number */

#define WTX_FPP_MASK 		0x0001
#define WTX_ALTIVEC_MASK	0x0002
#define WTX_DSP_MASK		0x0004

typedef enum wtx_coprocessor_bitshift
    {
    WTX_FPP_BITSHIFT	= 0,
    WTX_ALTIVEC_BITSHIFT= 1,
    WTX_DSP_BITSHIFT	= 2,
    WTX_LAST_BITSHIFT   = 16
    }WTX_COPROCESSOR_BITSHIFT;

/*
 * WTX_EVENT_TYPE - The class of events that can be generated by the
 * target server. User defined event types start at 0x100, event types
 * numbered 22 through 255 are reserved for future use.
 */

typedef enum wtx_event_type
    {
    WTX_EVENT_NONE         = 0,	    /* No event                           */
    WTX_EVENT_CTX_START    = 1,	    /* A target context was created       */
    WTX_EVENT_CTX_EXIT     = 2,	    /* A target context was deleted       */
    WTX_EVENT_TEXT_ACCESS  = 3,	    /* A target text access (breakpoint)  */
    WTX_EVENT_DATA_ACCESS  = 4,	    /* A target data access (watchpoint)  */
    WTX_EVENT_EXCEPTION    = 5,	    /* A target exception occurred        */
    WTX_EVENT_VIO_WRITE    = 6,	    /* A target VIO write occurred        */
    WTX_EVENT_HW_BP        = 7,     /* A target breakpoint: obsolete, use */
                                    /* WTX_EVENT_TEXT_ACCESS instead      */
    WTX_EVENT_CALL_RETURN  = 8,     /* A target function call returned    */
    WTX_EVENT_USER         = 9,     /* A target agent user defined event  */ 
    WTX_EVENT_UNKNOWN      = 10,    /* An unknown target event            */
    WTX_EVENT_TGT_RESET    = 11,    /* The target was reset               */
    WTX_EVENT_SYM_ADDED    = 12,    /* A symbol added to the sym. table   */
    WTX_EVENT_SYM_REMOVED  = 13,    /* A symbol removed from the sym tabl */
    WTX_EVENT_OBJ_LOADED   = 14,    /* An object file was loaded          */
    WTX_EVENT_OBJ_UNLOADED = 15,    /* An object file was unloaded        */
    WTX_EVENT_TOOL_ATTACH  = 16,    /* A tool attached to Target Server   */
    WTX_EVENT_TOOL_DETACH  = 17,    /* A tool detached from Target Server */ 
    WTX_EVENT_TOOL_MSG     = 18,    /* A tool to tool message             */
    WTX_EVENT_TS_KILLED    = 19,    /* The target server was reset        */
    WTX_EVENT_EVTPT_ADDED  = 20,    /* A new event point is added 	  */
    WTX_EVENT_EVTPT_DELETED= 21,    /* An event point had been deleted	  */
    WTX_EVENT_OTHER        = 0x100, /* User defined or unrecognized event */
    WTX_EVENT_INVALID = WTX_ERROR   /* Invalid/unidentifiable event       */
    } WTX_EVENT_TYPE;

typedef enum wtx_context_type		/* types of context */
    {
    WTX_CONTEXT_SYSTEM		= 0,	/* system mode */
    WTX_CONTEXT_GROUP		= 1,	/* process group (not implemented) */
    WTX_CONTEXT_ANY		= 2,	/* any context (not implemented) */
    WTX_CONTEXT_TASK		= 3,	/* specific task or processes */
    WTX_CONTEXT_ANY_TASK	= 4,	/* any task */
    WTX_CONTEXT_ISR		= 5,	/* specific ISR (not implemented) */
    WTX_CONTEXT_ISR_ANY		= 6	/* any ISR (not implemented) */
    } WTX_CONTEXT_TYPE;

typedef TGT_ADDR_T WTX_CONTEXT_ID_T;	/* context id type */

/*
 * WTX_ACTION_TYPE - Type of actions to perform when an event occurs. 
 * These action types can be bit-wise or-ed together. If the function called 
 * in an WTX_ACTION_CALL returns TRUE, then any other actions specified are 
 * cancelled. User defined actions use the high order 16 bits of the 
 * WTX_ACTION_TYPE.
 */

typedef enum wtx_action_type		/* type of actions */
    {
    WTX_ACTION_CALL 	= 1,		/* Call a function */
    WTX_ACTION_NOTIFY	= 2,		/* Post the event to the host */
    WTX_ACTION_STOP	= 4,		/* Stop the context */
    WTX_ACTION_STOP_ALL	= 8		/* not implemented */
    } WTX_ACTION_TYPE;

typedef enum wtx_return_type		/* return value type */
    {
    WTX_RETURN_TYPE_INT     = 0,	/* integer return format */
    WTX_RETURN_TYPE_DOUBLE  = 1		/* double return format */
    } WTX_RETURN_TYPE;

typedef enum wtx_svc_type               /* Id for service type & action */
    {
    WDB_LOGGING_ON          = 0,        /* Turn on wdb logging request  */
    WDB_LOGGING_OFF         = 1,        /* Turn off wdb logging request */
    WTX_LOGGING_ON          = 2,        /* Turn on wtx logging request  */
    WTX_LOGGING_OFF         = 3,        /* Turn off wtx logging request */
    ALL_LOGGING_OFF         = 4         /* Turn off wdb & wtx log req.  */
    } WTX_SVC_TYPE;

typedef enum wtx_reg_set_type		/* types of registers */
    {
    WTX_REG_SET_IU	= 0,		/* integer unit reg. set */
    WTX_REG_SET_FPU	= 1,		/* float. point unit reg. set */
    WTX_REG_SET_MMU	= 2,		/* memory managmt unit reg set */
    WTX_REG_SET_CU	= 3,		/* cache unit reg. set */
    WTX_REG_SET_TPU	= 4,		/* time process. unit reg. set */
    WTX_REG_SET_SYS	= 5,		/* system registers */
    WTX_REG_SET_DSP	= 6,		/* DSP registers */
    WTX_REG_SET_AV 	= 7,		/* altivec registers */
    WTX_REG_SET_LAST 	= 8		/* 1 + last used REG_SET number */
    } WTX_REG_SET_TYPE;

typedef enum wtx_agent_mode_type	/* debug agent running mode */
    {
    WTX_AGENT_MODE_TASK 	= 1,	/* run in task mode */
    WTX_AGENT_MODE_EXTERN	= 2,	/* run in system mode */
    WTX_AGENT_MODE_BI		= 3	/* bimodal agent */
    } WTX_AGENT_MODE_TYPE;

typedef enum wtx_context_status		/* context status */
    {
    WTX_CONTEXT_RUNNING		= 0,	/* context is running */		
    WTX_CONTEXT_SUSPENDED 	= 1	/* context is suspended */
    } WTX_CONTEXT_STATUS;

typedef enum wtx_open_flag		/* flag for WTX_OPEN */
    {
    WTX_O_RDONLY 	= 0x1000,	/* Open for reading only */
    WTX_O_WRONLY	= 0x1001,	/* Open for writing only */
    WTX_O_CREAT		= 0x1400	/* Open with file create */
    } WTX_OPEN_FLAG;

typedef struct wtx_core			/* WTX message core */
    {
    UINT32       objId;			/* identifier */
    WTX_ERROR_T  errCode;		/* service error code */
    UINT32       protVersion;		/* WTX protocol version */
    } WTX_CORE;

typedef struct wtx_desc			/* server descriptor */
    {
    char *		wpwrName;	/* name of wpwr service */
    char *		wpwrType;	/* type of service */
    char *		wpwrKey;	/* unique service key */
    } WTX_DESC;

typedef struct wtx_desc_q		/* server descriptor queue */
    {
    char *		wpwrName;	/* name of wpwr service */
    char *		wpwrType;	/* type of service */
    char *		wpwrKey;	/* unique service key */
    struct wtx_desc_q *	pNext;		/* next descriptor */
    } WTX_DESC_Q;

#ifdef __cplusplus
}
#endif

#endif	/* __INCwtxtypesh */
