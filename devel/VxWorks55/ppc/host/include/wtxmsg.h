/* wtxmsg.h - WTX message definition */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
03a,14jun01,pch  Add new WTX_TS_INFO_GET_V2 service to handle WTX_RT_INFO
		 change (from BOOL32 hasFpp to UINT32 hasCoprocessor)
		 without breaking existing clients.
02z,09may01,dtr  Replacing hasFpp and HasAltivec with hasCoprocessor.
02y,06feb01,dtr  Adding check for altivec support.
02x,18aug98,pcn  Moved WTX_SVC_TYPE and renamed WTX_MSG_EVTPT_LIST_GET.
02w,28jul98,lcs  Change TOOL_ID field of WTX_EVTPT_INFO structure to a uint32
02v,20jul98,pcn  Added evtptNum and toolId in the wtxEventpointListGet return
		 list.
02u,19jun98,pcn  Added new fields in WTX_MSG_FILE_LOAD_DESC structure.
02t,26may98,pcn  Changed WTX_MSG_EVTPT_LIST_2 in WTX_MSG_EVTPT_LIST_GET.
02s,08apr98,fle  doc: limit element comment to column 75 for manual generation
		 purpose
02r,07apr98,fle  doc: changed wtx_dasm_desc in wtx_msg_dasm_desc for mangen
02q,24mar98,dbt  added WTX_CONTEXT_STATUS_GET.
02p,03mar98,fle  got rid of WTX_REGISTRY_PING service
02o,02mar98,pcn  WTX 2: added WTX_EVENTPOINT_ADD_2, WTX_EVENTPOINT_LIST_2,
		 WTX_CACHE_TEXT_UPDATE, WTX_MEM_WIDTH_READ,
		 WTX_MEM_WIDTH_WRITE, WTX_OBJ_MODULE_CHECKSUM,
		 WTX_OBJ_MODULE_LOAD_2, WTX_OBJ_MODULE_UNLOAD_2,
		 WTX_UNREGISTER_FOR_EVENT, WTX_EVENT_LIST_GET,
		 WTX_COMMAND_SEND. Removed WTX_UN_REGIS_FOR_EVENT. Added
		 WTX_MSG_MEM_WIDTH_READ_DESC, WTX_MSG_MEM_WIDTH_COPY_DESC,
		 WTX_SVC_TYPE, WTX_MSG_LOG_CTRL, WTX_EVENT_NODE,
		 WTX_MSG_EVENT_LIST, WTX_LD_M_FILE_DESC,
		 WTX_MSG_FILE_LOAD_DESC, WTX_EVENT_2, WTX_EVTPT_2,
		 WTX_MSG_EVTPT_DESC_2, WTX_MSG_EVTPT_LIST_2.
02n,26jan98,fle  added WTX_REGISTRY_PING service number
02m,29aug97,fle  Adding the WTX_MEM_DISASSEMBLE service : creating 
		 WTX_MSG_DASM_INST_LIST, WTX_DASM_INST_LIST and 
		 WTX_MSG_DASM_DESC structures.
02l,06dec96,dgp  doc: correct spelling
02k,30sep96,elp  put in share/, added WTX_OBJ_MODULE_UNDEF_SYM_ADD (SPR# 6775).
02j,17jun96,p_m	 changed flags type in WTX_MSG_OPEN_DESC (SPR# 4941).
02i,10jun96,elp	 re-installed WTX_MSG_SYM_TBL_INFO type to prevent
		 compatibility problems.
02h,20may96,elp	 added WTX_SYM_TBL_INFO type (SPR# 6502).
02g,25oct95,p_m  added WTX_AGENT_MODE_GET service (SPR# 5231) and 
		 WTX_DIRECT_CALL.
02f,17jul95,p_m  cleaned comments for manual generation.
02d,06jun95,p_m  replaced moduleNameOrId in WTX_MSG_SYM_MATCH_DESC by an union.
02c,01jun95,p_m  removed WTX_MSG_CALL_PARAM. added returnType and pReserved
		 and replaced isText by contextType in WTX_MSG_CONTEXT_DESC.
02b,30may95,p_m  completed WTX_MEM_SCAN and WTX_MEM_MOVE implementation.
02a,26may95,p_m  added match field in WTX_MSG_MEM_SCAN_DESC.
01z,23may95,p_m  made missing name changes.
01y,22may95,jcf  name revision.
01x,18may95,p_m  added WTX_VIO_CHAN_GET and WTX_VIO_CHAN_RELEASE.
		 removed DETECTION in enventpoint.
01w,16may95,p_m  added WTX_MSG_KILL_DESC and WTX_TARGET_ATTACH.
01v,10may95,pad  modified WTX_MSG_SYM_LIST and WTX_MSG_SYM_MATCH_DESC to get
		 defined or unknown symbols associated with a module.
01u,09may95,p_m  added wtxregd related messages. added Target Server version
		 in WTX_MSG_TS_INFO.
01t,04may95,s_w  Put in WTX_xxx message numbers previously in private/wtxp.h
01s,02may95,pad  changed WTX_MSG_MODULE_INFO format field from UINT32 to char *
01r,04apr95,p_m  fixed WTX_MSG_CONTEXT_DESC.stackBase comment.
01q,29mar95,p_m  added comments. removed #include "comtypes.h".
		 changed addr type in OBJ_SEGMENT to TGT_ADDR_T.
01p,28mar95,p_m  took care of comments line length for manual generation.
01o,24mar95,p_m  changed addr type in LD_M_SECTION to TGT_ADDR_T.
01n,19mar95,c_s  updated gopher structures.
01m,20mar95,f_v  fix few syntax error to work with mgtools
01l,16mar95,p_m  added WTX_MSG_VIO_FILE_LIST.
01k,15mar95,p_m  changed #include "vxWorks.h" to #include "host.h".
01j,10mar95,p_m	 made endian field a UINT32.
01i,03mar95,p_m	 replaced WRAPSTRING by char *.
		 added gopher structure for WTX.
01h,27feb95,p_m	 cleanup up WTX_MSG_MEM_SCAN_DESC.
		 added symTblId to WTX_MSG_SYM_TBL_INFO.
		 simplified WTX_WDB_SERVICE_DESC.
01g,25feb95,jcf	 extended WTX_TS_INFO.  converted spaces to tabs.
01f,20feb95,pad	 added WTX_WTX_SERVICE_DESC type. Modified type
		 WTX_MSG_SERVICE_DESC.
01e,09feb95,p_m	 renamed structures for more coherency.
		 get rid of structures shared with WDB.
		 added comments and cleanup.
01d,08feb95,p_m	 added loadFlag in WTX_MODULE_INFO.
01c,30jan95,p_m	 added additional routines and filenames in SERVICE_DESC.
		 changed rtnObjFile to svcObjFile in SERVICE_DESC.
		 added WTX_MEM_XFER; added WTX_MEM_SET_DESC;
		 added WTX_REG_WRITE;
01b,22jan95,p_m	 added TOOL_DESC field in WTX_TS_INFO.
		 replaced #include "wdb/wdbtypes.h" by #include "wdbtypes.h".
		 replaced rtnName with initRtnName in SERVICE_DESC.
01a,20jan95,p_m	 written.
*/

#ifndef __INCwtxmsgh
#define __INCwtxmsgh

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HOST

#include "host.h"
#else

#include "vxWorks.h"
#endif /* HOST */

#include "wtxtypes.h"		/* basic wtx types */

/* WTX requests */

#define	WTX_TOOL_ATTACH		1
#define	WTX_TOOL_DETACH		2
#define	WTX_TS_INFO_GET_V1	3   /* info includes BOOL32 hasFpp */
#define	WTX_TARGET_RESET	4
#define	WTX_TS_LOCK		5
#define	WTX_TS_UNLOCK		6
#define	WTX_TARGET_ATTACH	7
#define	WTX_TS_INFO_GET_V2	8   /* info includes UINT32 hasCoprocessor */

/*
 * Prior to the introduction of support for multiple coprocessor types,
 * request #3 was named WTX_TS_INFO_GET.  Older clients may still make
 * this request, and will expect to receive a result containing the BOOL32
 * field hasFpp.  Meanwhile, older target servers will not understand the
 * new WTX_TS_INFO_GET_V2 request.
 *
 * For full compatibility, new servers must support both WTX_TS_INFO_GET_V1
 * and WTX_TS_INFO_GET_V2, returning the appropriate result for each.  New
 * clients should use WTX_TS_INFO_GET_V2 to obtain information concerning
 * all available coprocessors, but if this request is not recognized they
 * should issue WTX_TS_INFO_GET_V1 to determine whether an FPP is present.
 *
 * The following #define is to enable existing clients to be recompiled
 * without breaking.  When compiling an upgraded client, it could be removed
 * to help ensure that all uses of the older message have been changed.
 */
#define	WTX_TS_INFO_GET	WTX_TS_INFO_GET_V1

#define	WTX_CONTEXT_CREATE	10
#define	WTX_CONTEXT_KILL	11
#define	WTX_CONTEXT_SUSPEND	12
#define	WTX_CONTEXT_CONT	13
#define	WTX_CONTEXT_RESUME	14
#define	WTX_CONTEXT_STEP	15
#define	WTX_CONTEXT_STATUS_GET	16

#define	WTX_EVENTPOINT_ADD	20
#define	WTX_EVENTPOINT_DELETE	21
#define	WTX_EVENTPOINT_LIST	22
#define WTX_EVENTPOINT_ADD_2    23
#define WTX_EVENTPOINT_LIST_GET 24

#define	WTX_MEM_CHECKSUM	30
#define	WTX_MEM_READ		31
#define	WTX_MEM_WRITE		32
#define	WTX_MEM_SET		33
#define	WTX_MEM_SCAN		34
#define	WTX_MEM_MOVE		35
#define	WTX_MEM_ALLOC		36
#define	WTX_MEM_FREE		37
#define	WTX_MEM_INFO_GET	38
#define	WTX_MEM_ALIGN		39
#define	WTX_MEM_REALLOC		40
#define	WTX_MEM_ADD_TO_POOL	41
#define WTX_MEM_DISASSEMBLE	42
#define WTX_CACHE_TEXT_UPDATE   43
#define WTX_MEM_WIDTH_READ      44
#define WTX_MEM_WIDTH_WRITE     45

#define	WTX_REGS_GET		50
#define	WTX_REGS_SET		51

#define	WTX_OPEN		60
#define	WTX_VIO_READ		61
#define	WTX_VIO_WRITE		62
#define	WTX_CLOSE		63
#define	WTX_VIO_CTL		64
#define	WTX_VIO_FILE_LIST	65
#define	WTX_VIO_CHAN_GET	66
#define	WTX_VIO_CHAN_RELEASE	67

#define	WTX_OBJ_MODULE_LOAD		70
#define	WTX_OBJ_MODULE_UNLOAD		71
#define	WTX_OBJ_MODULE_LIST		72
#define	WTX_OBJ_MODULE_INFO_GET		73
#define	WTX_OBJ_MODULE_FIND		74
#define WTX_OBJ_MODULE_UNDEF_SYM_ADD	75
#define WTX_OBJ_MODULE_CHECKSUM         76
#define WTX_OBJ_MODULE_LOAD_2           77
#define WTX_OBJ_MODULE_UNLOAD_2         78

#define	WTX_SYM_TBL_CREATE	80
#define	WTX_SYM_TBL_DELETE	81
#define	WTX_SYM_TBL_LIST_GET	82
#define	WTX_SYM_LIST_GET	83
#define	WTX_SYM_TBL_INFO_GET	84
#define	WTX_SYM_FIND		85
#define	WTX_SYM_ADD		86
#define	WTX_SYM_REMOVE		87

#define	WTX_EVENT_GET			100
#define	WTX_REGISTER_FOR_EVENT		101
#define	WTX_UNREGISTER_FOR_EVENT	102
#define	WTX_EVENT_ADD			103
#define WTX_EVENT_LIST_GET          	104

#define	WTX_GOPHER_EVAL		110
#define	WTX_FUNC_CALL		111
#define	WTX_SERVICE_ADD		112
#define	WTX_WTX_SERVICE_LIST	113
#define	WTX_WDB_SERVICE_LIST	114
#define	WTX_CONSOLE_CREATE	115
#define	WTX_CONSOLE_KILL	116
#define	WTX_AGENT_MODE_SET	117
#define	WTX_AGENT_MODE_GET	118
#define	WTX_DIRECT_CALL		119

#define	WTX_OBJ_KILL		120
#define	WTX_OBJ_RESTART		121
#define WTX_COMMAND_SEND        122

#define WTX_REGISTER		201
#define WTX_UNREGISTER		202
#define WTX_INFO_GET		203
#define WTX_INFO_Q_GET		204

/* typedefs */

typedef enum wtx_value_t	/* Simple types definitions */
    {
    V_INT8		= 0,	/* 8  bit signed integer    */
    V_INT16		= 1,	/* 16 bit signed integer    */
    V_INT32		= 2,	/* 32 bit signed integer    */
    V_UINT8		= 3,	/* 8  bit unsigned integer  */
    V_UINT16		= 4,	/* 16 bit unsigned integer  */
    V_UINT32		= 5,	/* 32 bit unsigned integer  */
    V_DOUBLE		= 6,	/* Double floating point    */
    V_BOOL32		= 7,	/* 32 bit boolean           */
    V_PCHAR		= 8,	/* Character pointer        */
    V_PVOID		= 9,	/* Void pointer             */
    V_TGT_ADDR		= 10,	/* Target address           */
    V_TGT_ARG		= 11	/* Target task argument     */
    } WTX_VALUE_T;

typedef struct wtx_value		/* Simple type value      */
    {
    WTX_VALUE_T valueType;		/* Value type             */
    union
	{
	INT8		v_int8;		/* 8  bit signed value    */
	INT16		v_int16;	/* 16 bit signed value    */
	INT32		v_int32;	/* 32 bit signed value    */
	UINT8		v_uint8;	/* 8  bit unsigned value  */
	UINT16		v_uint16;	/* 16 bit unsigned value  */
	UINT32		v_uint32;	/* 32 bit unsigned value  */
	double		v_double;	/* Double value           */
	BOOL32		v_bool32;	/* 32 bit boolean value   */
	char *		v_pchar;	/* String value           */
	void *		v_pvoid;	/* Void pointer value     */
	TGT_ADDR_T	v_tgt_addr;	/* Target address value   */
	TGT_ARG_T	v_tgt_arg;	/* Target task arg. value */
	} value_u;
    } WTX_VALUE;

typedef struct wtx_msg_result		/* Simple request result */
    {
    WTX_CORE		wtxCore;	/* WTX message core      */

    WTX_VALUE		val;		/* Result value of call  */
    } WTX_MSG_RESULT;

typedef struct wtx_msg_param		/* Simple request parameter */
    {
    WTX_CORE		wtxCore;	/* WTX message core         */

    WTX_VALUE		param;		/* Param value of call      */
    } WTX_MSG_PARAM;

typedef struct wtx_mem_region		/* Memory region descriptor */
    {
    TGT_ADDR_T		baseAddr;	/* Mem. region base address */
    UINT32		size;		/* Mem. region size         */
    UINT32		attribute;	/* Mem. region attributes   */
    } WTX_MEM_REGION;

typedef struct wtx_rt_info		/* Target runtime information  */	  {
    UINT32		rtType;		/* Runtime type                */
    char *		rtVersion;	/* Run time version            */
    UINT32		cpuType;	/* Target processor type       */
    UINT32		hasCoprocessor;	/* Coprocessors:  see below    */
    BOOL32		hasWriteProtect;/* Text write protect avail.   */
    UINT32		pageSize;	/* Size of a page              */
    UINT32		endian;		/* Endianness (LITTLE or BIG)  */
    char *		bspName;	/* Board support package name  */
    char *		bootline;	/* Boot file name              */
    TGT_ADDR_T		memBase;	/* Target main mem. base addr. */
    UINT32		memSize;	/* Target main mem. size       */
    UINT32		numRegions;	/* Number of mem. regions      */
    WTX_MEM_REGION *	memRegion;	/* Mem. region descriptor(s)   */
    TGT_ADDR_T		hostPoolBase;	/* Tgtsvr mem. pool            */
    UINT32		hostPoolSize;	/* Tgtsvr mem. pool size       */
    } WTX_RT_INFO;

/*
 * The hasCoprocessor field is bit-significant, see definitions of
 * WTX_FPP_MASK etc. in wtxtypes.h
 */

typedef struct wtx_agent_info		/* Target agent information   */
    {
    char *		agentVersion;	/* WDB agent version          */
    UINT32		mtu;		/* Max. transfer size (bytes) */
    UINT32		mode;		/* Available agent modes      */
    } WTX_AGENT_INFO;

typedef struct wtx_tgt_info		/* Target information          */
    {
    WTX_AGENT_INFO	agentInfo;	/* Info on the agent           */
    WTX_RT_INFO		rtInfo;		/* Info on the run time system */
    } WTX_TGT_INFO;

typedef struct wtx_tgt_link_desc 	/* Link descriptor        */
    {
    char *		name;		/* Target/host link name  */
    UINT32		type;		/* Target/host link type  */
    UINT32		speed;		/* Target/host link speed */
    } WTX_TGT_LINK_DESC;

typedef struct wtx_tool_desc		/* Tool descriptor        */
    {
    UINT32		id;		/* Unique tool identifier */
    char *		toolName;	/* Tool name              */
    char *		toolArgv;	/* Tool argv parameters   */
    char *		toolVersion;	/* Tool version           */
    char *		userName;	/* User name              */
    void *		pReserved;	/* Reserved               */
    struct wtx_tool_desc * next;	/* ptr to next tool desc. */
    } WTX_TOOL_DESC;

typedef WTX_TOOL_DESC * TOOL_ID;

typedef struct wtx_msg_ts_info		/* TS information message     */
    {
    WTX_CORE		wtxCore;	/* WTX message core           */

    WTX_TGT_LINK_DESC	tgtLinkDesc;	/* Target link descriptor     */
    WTX_TGT_INFO	tgtInfo;	/* Info obtained from Target  */
    WTX_TOOL_DESC *	pWtxToolDesc;	/* Info about attached Tools  */
    char *		version;	/* Target Server version      */
    char *		userName;	/* Target server user name    */
    char *		startTime;	/* Target server start time   */
    char *		accessTime;	/* Target server access time  */
    char *		lockMsg;	/* Lock/authorization message */
    void *		pReserved;	/* Reserved                   */
    } WTX_MSG_TS_INFO;

typedef struct wtx_msg_ts_lock		/* Lock message     */
    {
    WTX_CORE		wtxCore;	/* WTX message core */

    UINT32		lockType;	/* Lock type to use */
    } WTX_MSG_TS_LOCK;

typedef struct wtx_msg_ts_unlock	/* Unlock message           */
    {
    WTX_CORE		wtxCore;	/* WTX message core         */

    BOOL32		force;		/* Force unlock type to use */
    } WTX_MSG_TS_UNLOCK;

typedef struct wtx_msg_tool_desc 	/* Tool desc. message */
    {
    WTX_CORE		wtxCore;	/* WTX message core   */

    WTX_TOOL_DESC	wtxToolDesc;	/* Tool descriptor    */
    } WTX_MSG_TOOL_DESC;

typedef struct wtx_msg_tool_id		/* Tool identifier message */
    {
    WTX_CORE		wtxCore;	/* WTX message core        */
    } WTX_MSG_TOOL_ID;

typedef struct wtx_msg_kill_desc	/* Kill descriptor message    */
    {
    WTX_CORE		wtxCore;	/* WTX message core           */

    UINT32		request;	/* Object kill flag           */
    WTX_VALUE		arg;		/* Object kill additional arg */
    } WTX_MSG_KILL_DESC;

typedef struct wtx_symbol		/* Symbol descriptor           */
    {
    UINT32		status;		/* Ret. val. for FIND request  */
    UINT32		symTblId;	/* Symbol table id             */ 
    char *		name;		/* Symbol name                 */	
    BOOL32		exactName;	/* FALSE if _name is OK        */
    TGT_ADDR_T		value;		/* Symbol value                */	
    UINT8		type;		/* Symbol type                 */	
    UINT8		typeMask;	/* Symbol type mask for lookup */	
    UINT16		group;		/* Symbol group                */
    char *		moduleName;	/* Module name                 */
    void *		pReserved;	/* Reserved                    */
    struct wtx_symbol * next;		/* ptr to next symbol in list  */
    } WTX_SYMBOL;

typedef struct wtx_msg_symbol_desc	/* Symbol message   */
    {
    WTX_CORE		wtxCore;	/* WTX message core */

    WTX_SYMBOL		symbol;		/* Symbol           */
    } WTX_MSG_SYMBOL_DESC;

typedef struct wtx_sym_list		/* Symbol list          */
    {
    WTX_SYMBOL *	pSymbol;	/* Start of symbol list */
    } WTX_SYM_LIST;

typedef struct wtx_msg_sym_list		/* Symbol list message           */
    {
    WTX_CORE		wtxCore;	/* WTX message core              */

    WTX_SYM_LIST 	symList;	/* Symbol list                   */
    BOOL32		moreToCome;	/* TRUE if symbols are remaining */
    } WTX_MSG_SYM_LIST;

typedef struct wtx_msg_open_desc	/* File opening descriptor    */
    {
    WTX_CORE		wtxCore;	/* WTX message core           */

    char *		filename;	/* Filename                   */
    WTX_OPEN_FLAG	flags;		/* Unix style open flags      */
    INT32		mode;		/* Unix style mode            */
    INT32		channel;	/* Channel id for redirection */
    } WTX_MSG_OPEN_DESC;

typedef struct wtx_msg_vio_ctl_desc	/* VIO control descriptor */
    {
    WTX_CORE		wtxCore; 	/* WTX message core       */

    INT32		channel;	/* VIO channel number     */
    UINT32		request;	/* Control operation      */
    UINT32		arg;		/* Control argument       */
    } WTX_MSG_VIO_CTL_DESC;

typedef struct wtx_msg_vio_copy_desc	/* Virtual I/O data          */
    {
    WTX_CORE		wtxCore; 	/* WTX message core          */

    UINT32		channel;	/* Virtual I/O channel       */
    UINT32		maxBytes;	/* Maximum bytes to transfer */
    void *		baseAddr;	/* Base address of data      */
    } WTX_MSG_VIO_COPY_DESC;

typedef struct wtx_vio_file_desc	/* Virtual file descriptor      */
    {
    char *	name;			/* File or device name          */
    INT32	fd;			/* File descriptor              */
    INT32	channel;		/* Channel for redirection      */
    INT32	fp;			/* File pointer for streams     */
    INT32	type;			/* File type                    */
    INT32	mode;			/* Open mode (O_RDONLY ...)     */
    INT32	status;			/* Open or closed               */
    INT32	addlInfo;		/* Extra info. (eg: process id) */
    struct wtx_vio_file_desc *	next;	/* ptr to next desc. for list   */
    } WTX_VIO_FILE_DESC;

typedef struct wtx_msg_vio_file_list	/* VIO files list message */
    {
    WTX_CORE		wtxCore; 	/* WTX message core       */

    WTX_VIO_FILE_DESC * pVioFileList;	/* VIO file list          */
    } WTX_MSG_VIO_FILE_LIST;

typedef struct ld_m_section		/* Object module section desc. */
    {
    UINT32		flags;		/* SEC_LOAD, etc.              */
    TGT_ADDR_T		addr;	  	/* Section address             */
    UINT32		length;		/* Section length              */
    } LD_M_SECTION;

typedef struct wtx_msg_ld_m_file_desc	/* Object module file desc.  */ 
    {
    WTX_CORE	   	wtxCore;	/* WTX message core          */

    char *	   	filename;	/* Object file name          */
    INT32	   	loadFlag;	/* Behavior control flags    */
    UINT32	  	moduleId;	/* Module identifier         */
    UINT32	   	nSections;	/* Number of sections        */
    LD_M_SECTION * 	section;	/* Section description       */
    WTX_SYM_LIST   	undefSymList;	/* List of undefined symbols */
    } WTX_MSG_LD_M_FILE_DESC;

typedef struct wtx_msg_module_list	/* Object module list       */
    {
    WTX_CORE		wtxCore;	/* WTX message core         */

    UINT32		numObjMod; 	/* Number of module in list */
    UINT32 *	 	modIdList; 	/* List of object module id */
    } WTX_MSG_MODULE_LIST;

typedef struct obj_segment		/* Object module segment */
    {
    UINT32		flags;		/* Segment flags         */
    TGT_ADDR_T		addr;		/* Segment address       */
    UINT32		length;		/* Segment length        */
    UINT32		reserved1;	/* Reserved              */
    UINT32		reserved2;	/* Reserved              */
    } OBJ_SEGMENT;

typedef struct wtx_msg_module_info	/* Object module information */
    {
    WTX_CORE	  	wtxCore;	/* WTX message core          */

    UINT32	  	moduleId;	/* Module Id                 */
    char *	  	moduleName;	/* Module name               */
    char *	  	format;		/* Object file format        */
    UINT32	  	group;		/* Group number              */
    int		  	loadFlag;	/* Flags used to load mod.   */
    UINT32	  	nSegments;	/* Number of segments        */
    OBJ_SEGMENT *	segment;	/* Segment description       */
    void *	  	pReserved;	/* Reserved                  */
    } WTX_MSG_MODULE_INFO;

typedef struct wtx_msg_mod_name_or_id	/* Module name or identifier */
    {
    WTX_CORE		wtxCore;	/* WTX message core          */

    UINT32		moduleId;	/* Module Id                 */
    char *		filename;	/* Module file name          */
    } WTX_MSG_MOD_NAME_OR_ID;

typedef struct wtx_sym_tbl_info
    {
    UINT32		symTblId;	/* Symbol table identifier */
    UINT32		symNum;		/* Number of symbols       */
    BOOL32		sameNameOk;	/* Name clash policy       */
    } WTX_SYM_TBL_INFO;

typedef struct wtx_msg_sym_tbl_info	/* Symbol table information */
    {
    WTX_CORE		wtxCore;	/* WTX message core */

    UINT32		symTblId;	/* Symbol table identifier  */
    UINT32		symNum;		/* Number of symbols        */
    BOOL32		sameNameOk;	/* Name clash policy        */
    } WTX_MSG_SYM_TBL_INFO;

typedef struct wtx_msg_sym_match_desc	/* Symbol lookup descriptor     */
    {
    WTX_CORE		wtxCore;	/* WTX message core             */

    UINT32		symTblId;	/* Symbol table to look at      */
    BOOL32		matchString;	/* Match string if TRUE, look   */
					/* Around adrs if FALSE         */
    TGT_ADDR_T		adrs;		/* Addr. around which to look   */
    char *		stringToMatch;	/* Substring to match           */

    BOOL32		byModuleName;	/* TRUE if module name is given */
    union
	{
	UINT32		moduleId;	/* Module identifier            */
	char *		moduleName;	/* or module name               */
	} module; 	

    BOOL32		listUnknownSym;	/* Get unknown symbols if TRUE  */
    BOOL32		giveMeNext;	/* TRUE for a follow-up req     */
    } WTX_MSG_SYM_MATCH_DESC;

typedef struct wtx_event_desc		/* WTX event desc.        */
    {
    char *		event;		/* Event descriptor       */
    INT32		addlDataLen;	/* Additional data length */
    char *		addlData;	/* Additional data (VIO)  */
    } WTX_EVENT_DESC;

typedef struct wtx_msg_event_desc  	/* Event message    */
    {
    WTX_CORE		wtxCore;	/* WTX message core */

    WTX_EVENT_DESC	eventDesc;	/* Event descriptor */
    } WTX_MSG_EVENT_DESC;

typedef struct wtx_msg_event_reg_desc	/* WTX event register desc.  */
    {
    WTX_CORE		wtxCore;	/* WTX message core          */

    char *		evtRegExp;	/* Register event reg. expr. */
    } WTX_MSG_EVENT_REG_DESC;

typedef struct wtx_wdb_service_desc	/* WDB service descriptor */
    {
    UINT32		rpcNum;		/* RPC service number     */
    char *		name;		/* Service name           */
    char *		svcObjFile;	/* Module for new service */ 
    char *		initRtnName;	/* Svc init. routine name */
    } WTX_WDB_SERVICE_DESC;

typedef struct wtx_wtx_service_desc	/* WTX service descriptor    */
    {
    char *		svcObjFile;	/* xdr/service routines file */
    char *		svcProcName;	/* Service procedure name    */
    char *		inProcName;	/* Input xdr procedure name  */
    char *		outProcName;	/* Output xdr procedure name */
    } WTX_WTX_SERVICE_DESC;

typedef struct wtx_msg_service_desc	/* protocol service desc. */
    {
    WTX_CORE			wtxCore;	 /* WTX msg core  */

    WTX_WTX_SERVICE_DESC *	pWtxServiceDesc; /* WTX service   */
    WTX_WDB_SERVICE_DESC *	pWdbServiceDesc; /* WDB service   */
    } WTX_MSG_SERVICE_DESC;

typedef struct wtx_msg_service_list	/* Protocol service list    */
    {
    WTX_CORE		wtxCore;	/* WTX message core         */

    UINT32		nServices;	/* Num. of services in list */
    WTX_WDB_SERVICE_DESC *serviceDesc;	/* Services list            */
    } WTX_MSG_SERVICE_LIST;

typedef struct wtx_console_desc 	/* WTX Virtual console desc. */
    {
    INT32		fdIn;		/* Stdin for redirection     */
    INT32		fdOut;		/* Stdout for redirection    */
    INT32		pid;		/* Process identifier        */
    char *		name;		/* Console name              */
    char *		display;	/* Display name eg: host:0   */
    void *		pReserved;	/* Reserved                  */
    } WTX_CONSOLE_DESC;

typedef struct wtx_msg_console_desc	/* WTX Virtual console desc. */
    {
    WTX_CORE		wtxCore;	/* WTX message core          */

    WTX_CONSOLE_DESC	wtxConsDesc;	/* Console descriptor        */
    } WTX_MSG_CONSOLE_DESC;

typedef struct wtx_msg_mem_info	 	/* Memory information         */
    {
    WTX_CORE	wtxCore;		/* WTX message core           */

    UINT32 	curBytesFree;		/* Number of free bytes       */
    UINT32 	curBytesAllocated;	/* Cur. # of bytes alloc.     */
    UINT32 	cumBytesAllocated;	/* Cum. # of bytes alloc.     */
    UINT32 	curBlocksFree;		/* Cur. # of free blocks      */
    UINT32 	curBlocksAlloc;		/* Cur. # of blocks alloc.    */
    UINT32 	cumBlocksAlloc;		/* Cum. # of blocks alloc.    */
    UINT32 	avgFreeBlockSize;	/* Average free blocks size   */
    UINT32 	avgAllocBlockSize;	/* Average alloc. blocks size */
    UINT32 	cumAvgAllocBlockSize;	/* Cum. avg alloc. block size */
    UINT32 	biggestBlockSize;	/* Size of biggest block      */
    void * 	pReserved;		/* Reserved                   */
    } WTX_MSG_MEM_INFO;

typedef struct wtx_mem_xfer		/* Memory transfer           */
    {
    UINT32		numBytes;	/* Num. of bytes to transfer */
    TGT_ADDR_T		destination;	/* Destination address       */
    char *		source;		/* Source address            */
    } WTX_MEM_XFER;

typedef struct wtx_msg_mem_xfer_desc	/* Memory transfer message */
    {
    WTX_CORE		wtxCore;	/* WTX message core        */

    WTX_MEM_XFER	memXfer;	/* Mem. transfer desc.     */
    } WTX_MSG_MEM_XFER_DESC;

typedef struct wtx_event		/* Target event             */
    {
    WTX_EVENT_TYPE 	eventType;	/* Type of event            */
    TGT_ARG_T	   	eventArg;	/* Event type dependent arg */
    } WTX_EVENT;

typedef struct wtx_context		/* Context desc.   */
    {
    WTX_CONTEXT_TYPE	contextType;	/* Type of context */
    WTX_CONTEXT_ID_T	contextId;	/* Context ID      */
    } WTX_CONTEXT;

typedef struct wtx_action		/* Action descriptor         */
    {
    WTX_ACTION_TYPE	actionType;	/* Action type to perform    */
    UINT32		actionArg;	/* Action dependent argument */
    TGT_ADDR_T		callRtn;	/* Function to ACTION_CALL   */
    TGT_ARG_T		callArg;	/* Function argument         */
    } WTX_ACTION;

typedef struct wtx_msg_context		/* Context message  */
    {
    WTX_CORE		wtxCore;	/* WTX message core */

    WTX_CONTEXT_TYPE	contextType;	/* Type of context  */
    WTX_CONTEXT_ID_T	contextId;	/* Context ID       */
    } WTX_MSG_CONTEXT;

typedef struct wtx_evtpt		/* Eventpoint desc.   */
    {
    WTX_EVENT		event;		/* Event to detect    */
    WTX_CONTEXT		context;	/* Context descriptor */
    WTX_ACTION		action;		/* Action to perform  */
    } WTX_EVTPT;

typedef struct wtx_msg_evtpt_desc	/* Eventpoint message */
    {
    WTX_CORE		wtxCore;	/* WTX message core   */

    WTX_EVTPT		wtxEvtpt;	/* Eventpoint         */
    } WTX_MSG_EVTPT_DESC;

typedef struct wtx_msg_evtpt_list  	/* Eventpoint list message */
    {
    WTX_CORE		wtxCore;	/* WTX message core        */

    UINT32		nEvtpt;		/* Num. eventpoint in list */
    WTX_EVTPT *		pEvtpt;		/* Eventpoint list         */
    } WTX_MSG_EVTPT_LIST;

typedef struct wtx_msg_reg_write	/* Register(s) to set       */
    {
    WTX_CORE		wtxCore;	/* WTX message core         */

    WTX_REG_SET_TYPE 	regSetType;	/* Type of register set     */
    WTX_CONTEXT	 	context;	/* Associated context       */
    WTX_MEM_XFER 	memXfer;	/* New value of register(s) */
    } WTX_MSG_REG_WRITE;
		  
typedef struct WTX_MSG_REG_READ		/* Register(s) to get       */
    {
    WTX_CORE	    	wtxCore;	/* WTX message core         */

    WTX_REG_SET_TYPE   	regSetType;	/* Type of register set     */
    WTX_CONTEXT	    	context;	/* Associated context       */
    WTX_MEM_REGION  	memRegion;	/* Register block subregion */
    } WTX_MSG_REG_READ;

typedef struct wtx_msg_mem_block_desc	/* Target memory block      */
    {
    WTX_CORE		wtxCore;	/* WTX message core         */

    TGT_ADDR_T		startAddr;	/* Blocks starts from here  */
    UINT32		numBytes;	/* Number of bytes in block */
    } WTX_MSG_MEM_BLOCK_DESC;

typedef struct wtx_msg_mem_read_desc	/* Target memory read desc. */
    {
    WTX_CORE		wtxCore;	/* WTX message core         */

    TGT_ADDR_T		source;		/* Source mem. address      */
    UINT32		destination;	/* Destination mem. address */
    UINT32		numBytes;	/* Number of bytes to read  */
    } WTX_MSG_MEM_READ_DESC;

typedef struct wtx_msg_mem_copy_desc	/* Target memory copy desc.   */
    {
    WTX_CORE		wtxCore;	/* WTX message core           */

    UINT32		source;		/* Encode from here           */
    TGT_ADDR_T		destination;	/* Decode to here             */
    UINT32		numBytes;	/* Bytes to copy/bytes copied */
    } WTX_MSG_MEM_COPY_DESC;

typedef struct wtx_msg_mem_scan_desc	/* Target memory scan desc.   */
    {
    WTX_CORE		wtxCore;	/* WTX message core           */

    BOOL32		match;		/* TRUE for matching pattern  */
    TGT_ADDR_T		startAddr;	/* Start addr. for scanning   */
    TGT_ADDR_T		endAddr;	/* End addr. for scanning     */
    UINT32		numBytes;	/* Number of bytes in pattern */
    void *		pattern;	/* Byte pattern to look for   */
    } WTX_MSG_MEM_SCAN_DESC;

typedef struct wtx_msg_mem_move_desc	/* Target memory move desc. */
    {
    WTX_CORE		wtxCore;	/* WTX message core         */

    TGT_ADDR_T		source;		/* Copy from here           */
    TGT_ADDR_T		destination;	/* To there                 */
    UINT32		numBytes;	/* Number of bytes to copy  */
    } WTX_MSG_MEM_MOVE_DESC;

typedef struct wtx_msg_mem_set_desc	/* Memory setting desc.        */
    {
    WTX_CORE		wtxCore;	/* WTX message core            */

    UINT32		value;		/* Value to set                */
    UINT8		width;		/* Value width 1, 2 or 4 bytes */
    TGT_ADDR_T		startAddr;	/* Set from here               */
    UINT32		numItems;	/* Number of items to set      */
    } WTX_MSG_MEM_SET_DESC;

typedef struct wtx_msg_context_desc		/* Context creation desc. */
    {
    WTX_CORE		wtxCore;		/* WTX message core       */

    WTX_CONTEXT_TYPE	contextType;		/* Type of context        */
    WTX_RETURN_TYPE	returnType;		/* Int or double          */
    char *		name;			/* Task name              */
    UINT32		priority;		/* Task priority          */
    UINT32		options;		/* Task options           */
    TGT_ADDR_T		stackBase;		/* Base of stack          */
    UINT32		stackSize;		/* Stack size             */
    TGT_ADDR_T		entry;			/* Context entry point    */
    INT32		redirIn;		/* Redir. in file or 0    */
    INT32		redirOut;		/* Redir. out file or 0   */
    TGT_ARG_T		args[WTX_MAX_ARG_CNT];	/* Arguments              */
    void * 		pReserved;		/* Reserved               */
    } WTX_MSG_CONTEXT_DESC;

typedef struct wtx_msg_context_step_desc /* Single step desc.        */
    {
    WTX_CORE		wtxCore;	/* WTX message core          */

    WTX_CONTEXT	 	context;	/* Context to step           */
    TGT_ADDR_T		startAddr;	/* Lower bound of step range */
    TGT_ADDR_T		endAddr;	/* Upper bound of step range */
    } WTX_MSG_CONTEXT_STEP_DESC;

typedef struct wtx_gopher_tape		/* Gopher tape    */
    {
    UINT16	len;			/* Length of tape */
    char *	data;			/* Tape data      */
    } WTX_GOPHER_TAPE;

typedef struct WTX_MSG_GOPHER_TAPE 	/* Gopher tape message */
    {
    WTX_CORE		wtxCore;	/* WTX message core    */

    WTX_GOPHER_TAPE	tape;		/* Gopher tape         */
    } WTX_MSG_GOPHER_TAPE;

typedef struct wtx_msg_wtxregd_pattern	/* wtxregd pattern           */
    {
    WTX_CORE		wtxCore;	/* WTX message core          */

    char *		name;		/* Pattern to match wpwrName */
    char *		type;		/* Pattern to match wpwrType */
    char *		key;		/* Pattern to match wpwrKey  */
    } WTX_MSG_WTXREGD_PATTERN;

typedef struct wtx_svr_desc		/* Server descriptor    */
    {
    char *		wpwrName;	/* Name of wpwr service */
    char *		wpwrType;	/* Type of service      */
    char *		wpwrKey;	/* Unique service key   */
    } WTX_SVR_DESC;

typedef struct wtx_msg_svr_desc		/* Server descriptor message */
    {
    WTX_CORE		wtxCore;	/* WTX message core          */

    WTX_SVR_DESC	wtxSvrDesc;	/* Server Descriptor         */
    } WTX_MSG_SVR_DESC;

typedef struct wtx_svr_desc_q		/* Server descriptor queue */
    {
    WTX_SVR_DESC	    wtxSvrDesc;	/* Server Descriptor       */
    struct wtx_svr_desc_q * pNext;	/* Next descriptor         */
    } WTX_SVR_DESC_Q;

typedef struct wtx_msg_svr_desc_q	/* Server desc. queue message */
    {
    WTX_CORE		  wtxCore;	/* WTX message core           */

    WTX_SVR_DESC_Q	  wtxSvrDescQ;	/* Server Descriptor Queue    */
    } WTX_MSG_SVR_DESC_Q;

typedef struct wtx_msg_dasm_desc	/* Instructions desc. to match   */
    {
    WTX_CORE		wtxCore;	/* WTX message core              */
    
    TGT_ADDR_T		startAddr;	/* Disassembling start address   */
    TGT_ADDR_T		endAddr;	/* Disassembling end address     */
    UINT32		nInst;		/* Number of instructions to get */
    BOOL32		printAddr;	/* Prepends instruction address  */
    BOOL32		printOpcodes;	/* Prepends instruction opcodes  */
    BOOL32		giveMeNext;	/* TRUE for a follow-up req      */
    BOOL32		hexAddr;	/* Turn off "symbolic + offset"  */
    } WTX_MSG_DASM_DESC;

typedef struct wtx_dasm_inst_list	/* Instructions list             */
    {
    char *		pInst;		/* Start of inst list            */
    UINT32		nextInst;	/* Next instruction address      */
    UINT32		listSize;	/* List size                     */
    } WTX_DASM_INST_LIST;

typedef struct wtx_msg_dasm_inst_list	/* Instructions list message     */
    {
    WTX_CORE		wtxCore;	/* WTX message core              */

    WTX_DASM_INST_LIST 	instList;	/* Symbol list                   */
    BOOL32		moreToCome;	/* TRUE if instructions remain   */
    } WTX_MSG_DASM_INST_LIST;

typedef struct wtx_msg_mem_width_read_desc  /* Target memory read desc. */
    {
    WTX_CORE            wtxCore;        /* WTX message core             */

    TGT_ADDR_T          source;         /* Source mem. address          */
    UINT32              destination;    /* Destination mem. address     */
    UINT32              numBytes;       /* Number of bytes to read      */
    UINT8               width;          /* Value width 1, 2 or 4 bytes  */
    } WTX_MSG_MEM_WIDTH_READ_DESC;

typedef struct wtx_msg_mem_width_copy_desc  /* Target memory copy desc. */
    {
    WTX_CORE            wtxCore;        /* WTX message core             */

    UINT32              source;         /* Encode from here             */
    TGT_ADDR_T          destination;    /* Decode to here               */
    UINT32              numBytes;       /* Bytes to copy/bytes copied   */
    UINT8               width;          /* Value width 1, 2 or 4 bytes  */
    } WTX_MSG_MEM_WIDTH_COPY_DESC;

typedef struct wtx_msg_log_ctrl         /* Logging control message */
    {
    WTX_CORE wtxCore;                   /* WTX message core        */

    WTX_SVC_TYPE    svcDesc;            /* Service type and action */
    WTX_VALUE       svcParam;           /* Parameter needed        */
    int             svcValue;           /* Extra parameter         */
    } WTX_MSG_LOG_CTRL;

typedef struct wtx_event_node           /* WTX event node desc.   */
    {
    struct wtx_event_desc    event;     /* Event descriptor       */
    struct wtx_event_node *  pNext;     /* ptr to the next event  */
    } WTX_EVENT_NODE;

typedef struct wtx_msg_event_list       /* Message for all events */
    {
    WTX_CORE            wtxCore;        /* WTX message core       */

    WTX_EVENT_NODE *    pEventList;     /* List of events         */
    } WTX_MSG_EVENT_LIST;

typedef struct wtx_ld_m_file_desc       /* Object module file desc.  */
    {
    char *              filename;       /* Object file name          */
    INT32               loadFlag;       /* Behavior control flags    */
    UINT32              moduleId;       /* Module identifier         */
    UINT32              nSections;      /* Number of sections        */
    LD_M_SECTION *      section;        /* Section description       */
    WTX_SYM_LIST        undefSymList;   /* List of undefined symbols */
    } WTX_LD_M_FILE_DESC;

typedef struct wtx_msg_file_load_desc       /* File descriptor      */
    {
    WTX_CORE                    wtxCore;    /* WTX message core     */

    struct wtx_ld_m_file_desc   fileDesc;   /* Obj mod file desc    */
    char *                      buffer;     /* Slice of file        */
    UINT32			flags;	    /* Load mode flag	    */
    INT32                       numItems;   /* Number of items sent */
    INT32                       fileSize;   /* File size            */
    INT32                       numPacket;  /* Current packet sent  */
    } WTX_MSG_FILE_LOAD_DESC;

typedef struct wtx_event_2              /* Target event version 2 */
    {
    WTX_EVENT_TYPE      eventType;      /* type of event          */
    UINT32              numArgs;        /* Number of arguments    */
    TGT_ARG_T *         args;           /* List of arguments      */
    } WTX_EVENT_2;

typedef struct wtx_evtpt_2              /* Eventpoint desc. version 2 */
    {
    WTX_EVENT_2         event;          /* Event to detect version 2  */
    WTX_CONTEXT         context;        /* Context descriptor         */
    WTX_ACTION          action;         /* Action to perform          */
    } WTX_EVTPT_2;

typedef struct wtx_evtpt_info		/* Eventpoint info */
    {
    WTX_EVTPT_2		wtxEvtpt;	/* Eventpoint descriptor */
    UINT32		toolId;		/* Tool identifier	 */
    UINT32		evtptNum;	/* Eventpoint identifier */
    void *		pReserved1;	/* Reserved		 */
    void *		pReserved2;	/* Reserved		 */
    } WTX_EVTPT_INFO;

typedef struct wtx_msg_evtpt_desc_2     /* Eventpoint message version 2 */
    {
    WTX_CORE            wtxCore;        /* WTX message core             */

    WTX_EVTPT_2         wtxEvtpt;       /* Eventpoint version 2         */
    } WTX_MSG_EVTPT_DESC_2;

typedef struct wtx_msg_evtpt_list_2	/* Eventpoint list message */
    {
    WTX_CORE            wtxCore;        /* WTX message core        */

    UINT32              nEvtpt;         /* Num. eventpoint in list */
    WTX_EVTPT_INFO *    pEvtptInfo;     /* Eventpoint info list    */
    } WTX_MSG_EVTPT_LIST_2;

#ifdef __cplusplus
}
#endif

#endif	/* __INCwtxmsgh */
