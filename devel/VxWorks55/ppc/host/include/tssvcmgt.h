/* tssvcmgt.h - target server services management library header */

/* Copyright 1994-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01i,28nov01,jhw  Updated for target library re-packaging.
01h,21jan98,c_c  DLLized Target Server Implementation.
		 Added dsaEnableGet to hide a global variable.
01g,01mar95,p_m  added WDB_SERVICE_DESC.
01f,15feb95,p_m  changed SERVICE_DESC to WTX_WDB_SERVICE_DESC.
		 removed non-ansi prototypes.
01e,21dec94,p_m  added tsWdbKnownSvcAdd().
01d,20dec94,p_m  replaced wtxtypes.h by wtx.h.
01c,04nov94,p_m  changed tsSvcAdd() to tsWdbSvcAdd().
01b,04nov94,p_m  changed tsServiceInit() to tgtSvcMgtInit().	
		 changed tsServiceAdd() to tsSvcAdd();
01a,24oct94,p_m  written.
*/

#ifndef __INCtssvcmgth
#define __INCtssvcmgth

#ifdef __cplusplus
extern "C" {
#endif

#include "wtx.h"

/* defines */

/* 
 * Relative path and filename for known WDB services list. This path is 
 * relative to the WPWR base directory, eg: /folk/vw/wpwr
 */

#define TS_WDB_SVC_LIST_FILE_NAME	"/host/resource/wdb/services"

/* 
 * Subset of path for known services object files. This path is 
 * relative to the WPWR base directory, eg: /folk/vw/wpwr
 * A complete pathname for a known WDB service object file can be:
 *
 * /folk/vw/wpwr/target/lib/objMC68060gnu
 */

#define TS_WDB_SVC_OBJ_PATH		"/target/lib"

/* typedefs */

typedef struct wdb_service_desc     /* WDB service descriptor */
    {
    UINT32	rpcNum[WTX_MAX_WDB_SVC_CNT];/* RPC service numbers */
    BOOL32	avail;		/* service available */
    char *	name;		/* service name */
    char *	svcObjFile;	/* module for new service */
    char *	initRtnName;	/* svc initialization routine name */
    } WDB_SERVICE_DESC;

/* function declaration */

extern BOOL	dsaEnableGet	(void);
extern STATUS	tsSvcMgtInit	(void);
extern STATUS	tsWdbSvcAdd	(WDB_SERVICE_DESC * pWtxWdbSvcDesc);
extern STATUS	tsWdbKnownSvcAdd	(u_int serviceNum);

#ifdef __cplusplus
}
#endif

#endif /* __INCtssvcmgth */
