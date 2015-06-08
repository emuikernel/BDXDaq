/* etherMultiLib.h - definitions for the Ethernet multicast library */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,12aug97,gnn cleaned up dependencies for multicast handling.
01c,23oct96,gnn name changes to follow coding standards.
01b,07may96,gnn added routines to handle non-IP based multicast
		addresses.
		Added list handling library.
01a,15apr96,gnn	written.

*/
 
/*
DESCRIPTION

This header file describes data structures and function prototypes for
the ethernet multicast library.

INCLUDE FILES:
*/

#ifndef __INCetherMultiLibh
#define __INCetherMultiLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */
#include "lstLib.h"
#include "end.h"
#include "netinet/if_ether.h"

/* defints */

/* typedefs */

/* globals */

/* locals */

/* forward declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern int etherMultiAdd(LIST *pList, char* pAddress );
extern int etherMultiDel(LIST* pList, char* pAddress);
extern int etherMultiGet(LIST *pList, MULTI_TABLE* pTable);
#else	/* __STDC__ */

extern int etherMultiAdd();
extern int etherMultiDel();
extern int etherMultiGet();
#endif /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCetherMultiLibh */
