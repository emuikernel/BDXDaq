/* pppLib.h - Point-to-Point Protocol library header */

/* Copyright 1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,30nov95,vin  included pppHookLib.h.
01c,16jun95,dzb  defined VJC for overall compile-time VJ compression control.
01b,09may95,dzb  added inclusion of pppSecretLib.h.
01a,07mar95,dzb  written.
*/

#ifndef	__INCpppLibh
#define	__INCpppLibh

#ifdef	__cplusplus
extern "C" {
#endif

#define VJC				/* define for VJ compression support */

#include "netinet/ppp/pppd.h"
#include "netinet/ppp/pppSecretLib.h"
#include "netinet/ppp/pppShow.h"
#include "netinet/ppp/pppHookLib.h"

#if defined(__STDC__) || defined(__cplusplus)

extern int	pppInit (int unit, char *devname, char *local_addr,
		char *remote_addr, int baud, PPP_OPTIONS *pOptions,
		char *fOptions);
extern void	pppDelete (int unit);

#else	/* __STDC__ */

extern int	pppInit ();
extern void	pppDelete ();

#endif	/* __STDC__ */

#ifdef	__cplusplus
}
#endif

#endif	/* __INCpppLibh */
