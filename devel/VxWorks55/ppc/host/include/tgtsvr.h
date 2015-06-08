/* tgtsvr.h - target server header file */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01q,18jun98,c_c  Licence Management removal.
01p,24mar98,c_c  Get rid of portmapper.
01o,02mar98,pcn  WTX 2: added tgtRpcAlarmThread.
01n,21jan98,c_c  DLLized target server implementation.
		 Moved some functions into tgtmgt.
01m,28feb97,wmd  added define for "Target Server"
01l,24may96,elp	 added tgtSvrShutdown().
01k,16jan96,pad  changed WTX_SVC_INSTALL definition.
01j,09nov95,p_m  removed un-necessary version defines.
01i,31may95,p_m  changed coreFileName coreFileNameParam.
01h,16may95,p_m  added attachTargetServer, removed tgtSvrInit().
01g,12may95,p_m  added targetAttached, tgtSvrAttach() and tgtSvrInit().
01f,10may95,p_m  changed TARGET_SERVER_VERSION to "1.0 Beta".
01e,05may95,p_m  changed serviceRtn type in tgtRpcSvcAdd().
01d,25feb95,jcf  removed tgtSvrAuthOff.  added tgtSvrUser, tgtSvrLocked.
01c,21feb95,pad  added tgtRpcSvcAdd prototype. Now may use DLL to install
		 WTX services.
01b,09feb95,p_m  added tgtSvrAuthOff.  removed SYSFLG_DEBUG.
01a,15jan95,p_m  written.
*/

#ifndef __INCtgtsvrh
#define __INCtgtsvrh	1

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

/* defines */

/* WTX services installation Dynamically Linked Library name */

#define TGT_SVR_NAME		"tgtsvr"	/* target server name */

#define TGT_SVR_LICENSE          "10"           /* Elan license management */
#define TGTSVR_APPNAME           "Target Server"

/* variable declarations */

extern UINT32	funcCallNum;		/* current function call number */
extern BOOL	dsaEnable;		/* dynamically scalable agent enable */
extern int	tgtSvrUser;		/* uid of most recent user of server */
extern int	tgtSvrVisitor;		/* uid of target server prober */
extern int	tgtSvrLockedUser;	/* uid that has locked target server */
extern time_t	tgtSvrStart;		/* target server start time */
extern time_t	tgtSvrAccess;		/* target server last access time */

extern char *	coreFileNameParam;	/* target core file name parameter */
					/* passed with the -c[ore] option */

/* function declarations */

extern STATUS tgtSvrAttach (void);

extern void   tgtSvrShutdown (void);

extern STATUS tgtSvrLicenseGet (void);

extern char * tgtRpcSvcKey 
    (
    BOOL	usePortMapper	/* do we register ourself to the portmapper ? */
    );

extern STATUS tgtRpcSvcRun
    (
    BOOL	lockSvr
    );

extern void   tgtRpcSvcInstall (void);

extern STATUS tgtRpcSvcLock
    (
    UINT32	lockType
    );

extern void tgtRpcAlarmThread
    (
    int         time
    );

extern STATUS tgtRpcSvcUnlock 
    (
    BOOL	force
    );

extern STATUS tgtRpcSvcAdd 
    (
    UINT32	serviceNum,
    FUNCPTR	serviceRtn,
    FUNCPTR	inProc,
    FUNCPTR	outProc,
    void *	dllHandle
    );

#ifdef __cplusplus
}
#endif

#endif	/* __INCtgtsvrh */
