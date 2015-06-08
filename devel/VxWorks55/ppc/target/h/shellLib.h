/* shellLib.h - header for shell facilities */

/* Copyright 1984 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
02e,30apr02,elr  changed API for shellParserControl() and added shellLogin()
           +fmk  added shellIsRemoteConnectedSet() and
                 shellIsRemoteConnectedGet()
02d,14feb01,spm  merged from version 02d of tor2_0x branch (base 02c):
                 added support for alternate command interpreter (SPR #5948)  
02c,22sep92,rrr  added support for c++
02b,16jul92,jcf  added variable declarations.
02a,04jul92,jcf  cleaned up.
01e,26may92,rrr  the tree shuffle
01d,04oct91,rrr  passed through the ansification filter
		  -changed VOID to void
		  -changed copyright notice
01c,05oct90,dnw  deleted private functions
01b,05oct90,shl  added ANSI function prototypes.
                 made #endif ANSI style.
                 added copyright notice.
01a,10aug90,dnw  written
*/

#ifndef __INCshellLibh
#define __INCshellLibh

#ifdef __cplusplus
extern "C" {
#endif

/* variable declarations */

extern char *	shellTaskName;
extern int	shellTaskId;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern BOOL 	shellLock (BOOL request);
extern STATUS 	execute (char *line);
extern STATUS 	shellInit (int stackSize, int arg);
extern void 	shell (BOOL interactive);
extern STATUS 	shellParserControl (UINT32, UINT32, UINT32);
extern void 	shellHistory (int size);
extern STATUS   shellLogin (int fd);
extern void 	shellLoginInstall (FUNCPTR logRtn, int logVar);
extern void 	shellLogout (void);
extern void 	shellLogoutInstall (FUNCPTR logRtn, int logVar);
extern void 	shellOrigStdSet (int which, int fd);
extern void 	shellPromptSet (char *newPrompt);
extern void 	shellRestart (BOOL staySecure);
extern void 	shellScriptAbort (void);
extern void     shellIsRemoteConnectedSet (BOOL remoteConnection);
extern BOOL     shellIsRemoteConnectedGet (void);


#else	/* __STDC__ */

extern BOOL 	shellLock ();
extern STATUS 	execute ();
extern STATUS 	shellInit ();
extern void 	shell ();
extern STATUS 	shellParserControl ();
extern void 	shellHistory ();
extern STATUS   shellLogin ();
extern void 	shellLoginInstall ();
extern void 	shellLogout ();
extern void 	shellLogoutInstall ();
extern void 	shellOrigStdSet ();
extern void 	shellPromptSet ();
extern void 	shellRestart ();
extern void 	shellScriptAbort ();
extern void     shellIsRemoteConnectedSet ();
extern BOOL     shellIsRemoteConnectedGet ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCshellLibh */
