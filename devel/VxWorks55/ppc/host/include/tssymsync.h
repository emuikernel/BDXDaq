/* tssymsync.h - host/target symbol tables synchronization library header */

/* Copyright 1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,29oct96,elp  added tsSymTblSyncLog() prototype.
01a,23aug96,elp  written
*/

#ifndef __INCtssymsynch
#define __INCtssymsynch

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "wtxmsg.h"

/* defines */

/* typedefs */

typedef struct 
    {
    REMPTR 	tgtSvrName;	/* target server name string on target */
    REMPTR	pCredBase;	/* authentication credentials */
    } TGT_SYNC_INFO;

/* function declaration */

extern STATUS	tsSymTblSyncInit	(void);
extern void	tsSymTblSyncLog		(WTX_MSG_EVENT_DESC * pWtxEvent);
extern STATUS	tsSymTblSyncExit	(void);

#ifdef __cplusplus
}
#endif

#endif /* __INCtssymsynch */
