/* dhcpcLib.h - DHCP include file for user interface of runtime client */

/* Copyright 1984 - 2000 Wind River Systems, Inc. */

/*
modification history
____________________
01h,05apr00,spm  removed entries shared with boot-time client
01g,24nov99,spm  upgraded to RFC 2131 and removed direct link-level access
01f,06oct97,spm  added prototypes for user-callable routines
01e,06aug97,spm  added definitions for C++ compilation
01d,30apr97,spm  changed dhcpcOptionGet() prototype to return length
01c,07apr97,spm  added prototypes for new functions
01b,29jan97,spm  added support for little-endian architectures
01a,03oct96,spm  created.
*/

#ifndef __INCdhcpcLibh
#define __INCdhcpcLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "dhcp/dhcpc.h" 	/* DHCP client data structures */

IMPORT void * 	pDhcpcBootCookie; 	/* User access to boot-time lease. */

/* DHCP data structures for option requests and retrieval. */

/* DHCP (runtime) client user interface routines */

IMPORT STATUS dhcpcLibInit (int, int, int, int, int, int, int);
IMPORT void * dhcpcInit (struct ifnet *, BOOL);
IMPORT STATUS dhcpcEventHookAdd (void *, FUNCPTR);
IMPORT STATUS dhcpcEventHookDelete (void *);
IMPORT STATUS dhcpcCacheHookAdd (FUNCPTR);
IMPORT STATUS dhcpcCacheHookDelete (void);
IMPORT STATUS dhcpcOptionSet (void *, int);
IMPORT STATUS dhcpcOptionAdd (void *, UCHAR, int, UCHAR *);
IMPORT STATUS dhcpcBind (void *, BOOL);
IMPORT STATUS dhcpcVerify (void *);
IMPORT STATUS dhcpcOptionGet (void *, int, int *, char *);
IMPORT STATUS dhcpcServerGet (void *, struct in_addr *);
IMPORT STATUS dhcpcTimerGet (void *, int *, int *);
IMPORT STATUS dhcpcParamsGet (void *, struct dhcp_param *);
IMPORT STATUS dhcpcRelease (void *);
IMPORT STATUS dhcpcShutdown (void);

#ifdef __cplusplus
}
#endif

#endif
