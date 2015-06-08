/* pppHookLib.h - PPP authentication secrets library header */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,29nov95,vin  written.
*/

#ifndef __INCpppHookLibh
#define __INCpppHookLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "vwModNum.h"
#include "netinet/ppp/pppd.h"

/* typedefs */

typedef struct ppp_hooks		/* PPP_HOOK_RTNS */
    {
    FUNCPTR	connectHook;		/* hook called before set up of link */
    FUNCPTR	disconnectHook;		/* hook called after close of link */
    } PPP_HOOK_RTNS;

/* defines */

#define	S_pppHookLib_NOT_INITIALIZED	(M_pppHookLib	| 1)
#define	S_pppHookLib_HOOK_DELETED	(M_pppHookLib	| 2)
#define	S_pppHookLib_HOOK_ADDED		(M_pppHookLib	| 3)
#define S_pppHookLib_HOOK_UNKNOWN	(M_pppHookLib	| 4)
#define S_pppHookLib_INVALID_UNIT	(M_pppHookLib	| 5)

#define PPP_HOOK_CONNECT		0
#define PPP_HOOK_DISCONNECT		1

	
/* globals */

extern PPP_HOOK_RTNS *	pppHookRtns [];		

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS	pppHookLibInit (void);
extern STATUS	pppHookAdd (int unit, FUNCPTR hookRtn, int type);
extern STATUS	pppHookDelete (int unit, int type);

#else	/* __STDC__ */

extern STATUS	pppHookLibInit ();
extern STATUS	pppHookAdd ();
extern STATUS	pppHookDelete ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCpppHookLibh */
