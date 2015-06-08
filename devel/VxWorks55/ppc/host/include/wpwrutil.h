/* wpwrutil.h - wpwr utility library */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01v,17may02,fle  added wpwrTmpDirGet (), wpwrAppDataDirGet () and wpwrDirCheck
                 () routines
01u,18jan99,c_c  Added wpwrExpandVars and wpwrExpandTilde routines'
                 prototypes.
01t,12oct98,jmp  removed shell.h inclusion.
01s,03jun98,c_c  Added a hook mechanism to wpwrRestart and wpwrShutdown.
01r,25may98,fle  put shell related procedures into shellib.c
01q,06may98,pcn  Added tgtsvr args in WDB & WTX log file.
01p,30mar98,c_c  Now signal handler can handle a list of hook routines.
01o,23mar98,c_c  Moved log routines to wpwrlog.h.
01n,04feb98,c_c  Added wpwrDebugModeGet.
01m,28jan98,c_c  Added libwpwr internal flags manipulation routines.
01l,20jan98,c_c  Target Server multi threading and DLLisation integration.
		 Removed globals refs.
01k,17dec97,fle  added wpwrBackslashToSlash()
		 + got rid of old C style declarations
01j,11sep95,p_m  added uid parameter to wpwrGetUserName().
01i,16may95,p_m  made wpwrVerboseMode global.
01h,15may95,s_w  added wpwrGetUserName(), wpwrGetUidFromName(), wpwrGetUid()
01g,20apr95,p_m  added wpwrGetEnv() prototype.
01f,15mar95,pad  changed wpwrInit()'s prototype (added verbose mode).
01e,06mar95,pad  changed wpwrInit()'s prototype. Added prototypes for
		 wpwrLogRaw() and wpwrLogHeaderSet().
01d,01mar95,p_m  added wpwrSigHookAdd().
01c,25jan95,jcf  added wpwrAuthorize* ().
01b,22jan95,jcf  added wpwrVersion().
01a,24sep95,jcf  written.
*/

#ifndef __INCwpwrutilh
#define __INCwpwrutilh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */
 
#include "host.h"
#include "wtx.h"
#include "wpwrlog.h"

/* defines */

#define		WPWR_UID_DEFAULT 0xfffffff0	/* default user id parameter */
						/* for wpwrGetUserName() */
/* typedefs */

typedef void *	HOOK_ID;		/* hook Id definition */

/* globals */

/* function declarations */

extern STATUS       wpwrInit
    (
    char * argv[],
    char * name,
    int rebootDelay,
    BOOL verbose
    );
 
extern void     wpwrVersion (void);
 
extern STATUS	wpwrAuthorize
    (
    char * filename
    );

extern char **	wpwrCmdLineGet (void);
 
extern STATUS	wpwrUserIsAuthorized
    (
    int uid
    );
 
extern void     wpwrShutdown (void);
 
extern void     wpwrRestart (void);
 
extern void     wpwrSigInit (void);
 
extern HOOK_ID	wpwrShutdownHookAdd	/* install a shutdown hook */
    (
    FUNCPTR	sigHookRtn,		/* hook routine */
    void *	sigHookParam		/* hook routine arg */
    );

extern void	wpwrShutdownHookRemove	/* remove a shutdown hook */
    (
    HOOK_ID	hook			/* Hook to be removed */
    );
 
extern HOOK_ID	wpwrRestartHookAdd	/* install a restart hook */
    (
    FUNCPTR	sigHookRtn,		/* hook routine */
    void *	sigHookParam		/* hook routine arg */
    );

extern void	wpwrRestartHookRemove	/* remove a restart hook */
    (
    HOOK_ID	hook			/* Hook to be removed */
    );
 
extern HOOK_ID	wpwrSigHookAdd		/* install a signal hook */
    (
    FUNCPTR	sigHookRtn,		/* hook routine */
    void *	sigHookParam		/* hook routine arg */
    );

extern void	wpwrSigHookRemove	/* remove a signal hook */
    (
    HOOK_ID	hook			/* Hook to be removed */
    );
 
extern char *   wpwrGetEnv
    (
    char * envVar
    );

extern BOOL wpwrExpandVars	/* expand environment var in pathname */
    (
    char *		initialFileName,	/* original PathName */
    char *		expandedFileName,	/* expanded pathname */
    unsigned int	bufferSize		/* size of expandedFileName */
    );

extern BOOL wpwrExpandTilde 	/* expand `~' to home directory in pathname */
    (
    char *	stringToMatch,	/* trailing user name for matching or NULL */
    char *	homeDirectory,	/* Where to return the home directory      */
    int		size		/* size of this buffer                     */
    );
 
extern char *   wpwrGetUserName
    (
    int uid
    );
 
extern int      wpwrGetUidFromName
    (
    char * userName
    );
 
extern int      wpwrGetUid (void);
 
#ifdef WIN32
extern void     wpwrSigEnableSet
    (
    BOOL value
    );
 
extern BOOL     wpwrSigEnableGet(void);
#endif

extern void     wpwrRestartAllowedSet
    (
    BOOL value/* TRUE when the Target Server can be restarted by the sig hdlr */
    );
 
extern char *   wpwrEnvVersionGet(void);
 
extern char *   wpwrSvcKeySet
    (
    char * value
    );
 
extern char *   wpwrSvcKeyGet(void);

extern void	wpwrBackslashToSlash 
    (
    char * string
    );

extern const char * wpwrTmpDirGet (void);

extern const char * wpwrAppDataDirGet (void);

extern STATUS wpwrDirCheck
    (
    const char *	path,
    BOOL		createIt
    );

#ifdef __cplusplus
}
#endif

#endif /* __INCwpwrutilh */

