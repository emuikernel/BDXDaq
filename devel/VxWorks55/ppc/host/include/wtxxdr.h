/* wtxxdr.h - wtx xdr header file */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01y,15jun01,pch  Add new WTX_TS_INFO_GET_V2 service to handle WTX_RT_INFO
                 change (from BOOL32 hasFpp to UINT32 hasCoprocessor) without
                 breaking existing clients.
01x,12feb99,dbt  include wtxmsg.h header file since we use types declared in
                 this file.
01w,18aug98,pcn  Re-use WTX_MSG_EVTPT_LIST_2.
01v,20jul98,pcn  Added evtptNum and toolId in the wtxEventpointListGet return
                 list.
01u,26may98,pcn  Changed WTX_MSG_EVTPT_LIST_2 in WTX_MSG_EVTPT_LIST_GET.
01t,02mar98,pcn  WTX 2: added xdr_WTX_EVENT_NODE, xdr_WTX_MSG_EVENT_LIST,
                 xdr_WTX_LD_M_FILE_DESC, xdr_WTX_MSG_FILE_LOAD_DESC,
                 xdr_MEM_WIDTH_COPY_ALLOC, xdr_WTX_MSG_MEM_WIDTH_READ_DESC,
                 xdr_WTX_MSG_MEM_WIDTH_COPY_DESC, xdr_WTX_EVENT_2,
                 xdr_WTX_EVTPT_2, xdr_WTX_MSG_EVTPT_DESC_2,
                 xdr_WTX_MSG_EVTPT_LIST_2.
01s,29aug97,fle  Adding the WTX_MEM_DISASSEMBLE service. i.e. adding
                   xdr_WTX_MSG_DASM_DESC and xdr_WTX_MSG_DASM_INST_LIST routines
01s,22aug97,c_c  Try to remove warnings during compilation.
01r,30sep96,elp  put in share/src/wtx (SPR# 6775).
01q,02feb96,elp  added declaration of xdr_TGT_ADDR_T().
01p,01jun95,p_m  removed xdr_WTX_MSG_CALL_PARAM.
01o,30may95,pad  don't include rpc/rpc.h for PARISC_HPUX9.
01n,30may95,p_m  completed WTX_MEM_SCAN and WTX_MEM_MOVE implementation.
01m,23may95,p_m  made missing name changes.
01l,22may95,jcf  name revision.
01k,16may95,p_m  added xdr_WTX_MSG_KILL_DESC().
01j,09may95,p_m  re-implemented wtxregd related XDR procedures.
		 removed unneeded prototypes.
01i,04may95,s_w  change include of rpc/rpc.h to wtxrpc.h
01h,24mar95,p_m  added xdr_WTX_MSG_MEM_BLOCK_DESC.
01g,16mar95,p_m  added xdr_WTX_MSG_VIO_FILE_LIST.
01f,03mar95,p_m  added xdr_WTX_MSG_GOPHER_TAPE.
01e,27feb95,p_m  added xdr_WTX_MSG_MEM_SCAN_DESC.
01d,25feb95,jcf  added xdr_WTX_MSG_TS_[UN]LOCK.
01c,15feb95,p_m  changed names to xdr_WTX_MSG and added xdr routines
		 to suppress coupling with WDB xdr routines.
		 added #include <rpc/rpc.h>.
01b,31jan95,p_m  added xdr_WTX_MEM_XFER(), xdr_WTX_REG_WRITE() and
		 xdr_WTX_MEM_SET_DESC().
01a,15jan95,p_m  written.
*/

#ifndef __INCwtxxdrh
#define __INCwtxxdrh	1

#ifdef __cplusplus
extern "C" {
#endif

#include <rpc/rpc.h>
#include "wtxmsg.h"

/* function declarations */

extern bool_t xdr_TGT_ADDR_T		/* transfer a TGT_ADDR_T data */
    (
    XDR *		xdrs,		/* XDR information */
    TGT_ADDR_T *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_TOOL_ID	/* transfer a WTX_MSG_TOOL_ID data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_TOOL_ID *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_TOOL_DESC	/* tranfer a WTX_MSG_TOOL_DESC data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_TOOL_DESC *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_RESULT	/* transfer a WTX_MSG_RESULT data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_RESULT *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_PARAM		/* transfer a WTX_MSG_PARAM data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_PARAM *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_TS_INFO_V1	/* tranfer a vn 1 WTX_MSG_TS_INFO msg */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_TS_INFO *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_TS_INFO_V2	/* tranfer a vn 2 WTX_MSG_TS_INFO msg */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_TS_INFO *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_TS_LOCK	/* transfer a WTX_MSG_TS_LOCK data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_TS_LOCK *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_TS_UNLOCK	/* transfer a WTX_MSG_TS_UNLOCK data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_TS_UNLOCK *	objp		/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_EVTPT_LIST	/* transfer a WTX_MSG_EVTPT_LIST data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_EVTPT_LIST *	objp	/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_SYM_TBL_INFO	/* transfer WTX_MSG_SYM_TBL_INFO data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_SYM_TBL_INFO *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_SYMBOL_DESC	/* transfer WTX_MSG_SYMBOL_DESC data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_SYMBOL_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_SYM_LIST	/* transfer a WTX_MSG_SYM_LIST data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_SYM_LIST *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_SYM_MATCH_DESC	/* WTX_MSG_SYM_MATCH_DESC xdr */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_SYM_MATCH_DESC *	objp	/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_VIO_CTL_DESC	/* tranfer WTX_MSG_VIO_CTL_DESC data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_VIO_CTL_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_OPEN_DESC	/* tranfer WTX_MSG_OPEN_DESC */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_OPEN_DESC *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_VIO_COPY_DESC	/* transfer WTX_MSG_VIO_COPY_DESC data*/
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_VIO_COPY_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_VIO_FILE_LIST	/* transfer WTX_MSG_VIO_FILE_LIST data*/
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_VIO_FILE_LIST *	objp	/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_LD_M_FILE_DESC	/* WTX_MSG_LD_M_FILE_DESC xdr */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_LD_M_FILE_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MODULE_LIST	/* transfer WTX_MSG_MODULE_LIST data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MODULE_LIST *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MODULE_INFO	/* transfer WTX_MSG_MODULE_INFO data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MODULE_INFO *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MOD_NAME_OR_ID	/* WTX_MSG_MOD_NAME_OR_ID xdr */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MOD_NAME_OR_ID *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_CONTEXT_DESC	/* transfer WTX_MSG_CONTEXT_DESC data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_CONTEXT_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_CONTEXT	/* transfer WTX_MSG_CONTEXT data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_CONTEXT *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_CONTEXT_STEP_DESC	/* WTX_MSG_CONTEXT_STEP_DESC */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_CONTEXT_STEP_DESC *	objp	/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_EVENT_DESC	/* transfer WTX_MSG_EVENT_DESC data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_EVENT_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_EVENT_NODE        /* transfer WTX_EVENT_NODE data */
    (
    XDR *                       xdrs,   /* XDR information */
    WTX_EVENT_NODE *            objp    /* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_EVENT_LIST    /* transfer WTX_MSG_EVENT_LIST data */
    (
    XDR *                       xdrs,   /* XDR information */
    WTX_MSG_EVENT_LIST *        objp    /* pointer to object to transfert */
    );

extern bool_t xdr_WTX_MSG_EVENT_REG_DESC	/* WTX_MSG_EVENT_REG_DESC xdr */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_EVENT_REG_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_EVTPT_DESC	/* transfer WTX_MSG_EVTPT_DESC data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_EVTPT_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_LD_M_FILE_DESC    /* transfer WTX_LD_M_FILE_DESC data */
    (
    XDR *       xdrs,                   /* XDR information */
    WTX_LD_M_FILE_DESC * objp           /* pointer to the object to transfer */
    );

extern bool_t xdr_WTX_MSG_FILE_LOAD_DESC /* transfer WTX_MSG_FILE_LOAD_DESC */
    (
    XDR *       xdrs,                    /* XDR information */
    WTX_MSG_FILE_LOAD_DESC * objp        /* pointer to the object to transfer */
    );

extern bool_t xdr_WTX_MSG_KILL_DESC	/* transfer WTX_MSG_KILL_DESC data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_KILL_DESC *	objp		/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_SERVICE_DESC	/* transfer WTX_MSG_SERVICE_DESC data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_SERVICE_DESC *	objp	/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_CONSOLE_DESC	/* transfer WTX_MSG_CONSOLE_DESC data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_CONSOLE_DESC *	objp	/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_MEM_INFO	/* transfer WTX_MSG_MEM_INFO data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_MEM_INFO *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_MEM_COPY_ALLOC	/* transfer WTX_MSG_MEM_COPY_DESC data*/
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MEM_COPY_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_MEM_WIDTH_COPY_ALLOC  /* WTX_MSG_MEM_WIDTH_COPY_DESC data*/ 
    ( 
    XDR *                         xdrs,   /* XDR information */
    WTX_MSG_MEM_WIDTH_COPY_DESC * objp    /* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MEM_XFER_DESC	/* transfer WTX_MSG_MEM_XFER_DESC data*/
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MEM_XFER_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MEM_SCAN_DESC	/* transfer WTX_MSG_MEM_SCAN_DESC data*/
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MEM_SCAN_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MEM_MOVE_DESC	/* transfer WTX_MSG_MEM_MOVE_DESC data*/
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MEM_MOVE_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MEM_SET_DESC	/* transfer WTX_MSG_MEM_SET_DESC data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MEM_SET_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MEM_READ_DESC	/* transfer WTX_MSG_MEM_READ_DESC data*/
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MEM_READ_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MEM_WIDTH_READ_DESC /* WTX_MSG_MEM_WIDTH_READ_DESC */
    (
    XDR *                       xdrs,   /* XDR information */
    WTX_MSG_MEM_WIDTH_READ_DESC * objp  /* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MEM_COPY_DESC	/* transfer WTX_MSG_MEM_COPY_DESC data*/
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MEM_COPY_DESC *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MEM_WIDTH_COPY_DESC /* WTX_MSG_MEM_WIDTH_COPY_DESC */
    (
    XDR *                       xdrs,   /* XDR information */
    WTX_MSG_MEM_WIDTH_COPY_DESC * objp  /* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_MEM_BLOCK_DESC	/* WTX_MSG_MEM_BLOCK_DESC xdr */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_MEM_BLOCK_DESC *	objp	/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_DASM_DESC	/* transfer WTX_MSG_DASM_DESC data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_DASM_DESC *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_DASM_INST_LIST	/* WTX_MSG_DASM_INST_LIST xdr */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_DASM_INST_LIST *	objp	/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_REG_WRITE	/* transfer WTX_MSG_REG_WRITE data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_REG_WRITE *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_REG_READ	/* transfer WTX_MSG_REG_READ data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_REG_READ *	objp		/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_GOPHER_TAPE	/* transfer WTX_MSG_GOPHER_TAPE data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_GOPHER_TAPE *	objp	/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_CORE		/* transfer WTX_CORE data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_CORE *		objp		/* pointer to object to transfer */
    );


extern bool_t xdr_WTX_MSG_SVR_DESC	/* transfer WTX_MSG_SVR_DESC data */
    (
    XDR *		xdrs,		/* XDR information */
    WTX_MSG_SVR_DESC *	objp		/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_SVR_DESC_Q	/* transfer WTX_MSG_SVR_DESC_Q data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_SVR_DESC_Q *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_MSG_WTXREGD_PATTERN	/* WTX_MSG_WTXREGD_PATTERN xdr*/
    (
    XDR *			xdrs,	/* XDR information */
    WTX_MSG_WTXREGD_PATTERN *	objp	/* pointer to object to transfer */
    );

extern bool_t xdr_WTX_EVENT_2           /* transfer WTX_EVENT_2 data */
    (
    XDR *                       xdrs,   /* XDR information */
    WTX_EVENT_2 *               objp    /* pointer to the object to transfer */
    );

extern bool_t xdr_WTX_EVTPT_2           /* transfer WTX_EVTPT_2 data */
    (
    XDR *                       xdrs,   /* XDR information */
    WTX_EVTPT_2 *               objp    /* pointer to the object to transfer */
    );

extern bool_t xdr_WTX_MSG_EVTPT_DESC_2  /* transfer WTX_MSG_EVTPT_DESC_2 data */
    (
    XDR *                       xdrs,   /* XDR information */
    WTX_MSG_EVTPT_DESC_2 *      objp    /* pointer to the object to transfer */
    );

extern bool_t xdr_WTX_EVTPT_INFO	/* transfer WTX_EVTPT_INFO data */
    (
    XDR *			xdrs,	/* XDR information */
    WTX_EVTPT_INFO *		objp	/* Pointer to the object to transfer */
    );

extern bool_t xdr_WTX_MSG_EVTPT_LIST_2	/* transfer WTX_MSG_EVTPT_LIST_2 */
    (
    XDR *                       xdrs,   /* XDR information */
    WTX_MSG_EVTPT_LIST_2 *	objp    /* pointer to the object to transfer */
    );

#ifdef __cplusplus
}
#endif

#endif /* __INCwtxxdrh */
