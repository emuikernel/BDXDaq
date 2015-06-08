/* etherLib.h - ethernet hook routines header */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01k,10oct01,rae  update copyright
01j,17dec96,gnn  removed some prototypes to have variable args
01k,22oct96,gnn  added support for the etherTypeGet call.
                 added the ENET_HDR typedef.
01j,22sep92,rrr  added support for c++
01i,04jul92,jcf  cleaned up.
01h,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01g,26feb92,elh  added prototypes for ether{Input,Output}HookDelete.
01f,04oct91,rrr  passed through the ansification filter
		  -changed includes to have absolute path from h/
		  -fixed #else and #endif
		  -changed copyright notice
01e,15jan91,shl  included "if.h" and "if_ether.h" so ifnet and ether_header
		 are defined before the prototypes.
01d,05oct90,shl  added ANSI function prototypes.
                 made #endif ANSI style.
                 added copyright notice.
01c,07aug90,shl  added INCetherLibh to #endif.
01b,01nov87,dnw  added etherOutputHookRtn.
01a,28aug87,dnw  written
*/

#ifndef __INCetherLibh
#define __INCetherLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "net/if.h"
#include "netinet/if_ether.h"

IMPORT FUNCPTR etherInputHookRtn;
IMPORT FUNCPTR etherOutputHookRtn;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/* defints */
typedef struct enet_hdr
    {
    char dst [6];
    char src [6];
    USHORT type;
    } ENET_HDR;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#define ENET_HDR_SIZ        sizeof(ENET_HDR)
#define ENET_HDR_REAL_SIZ   14

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS 	etherAddrResolve (struct ifnet *pIf, char *targetAddr,
				  char *eHdr, int numTries, int numTicks);
extern STATUS 	etherInputHookAdd ();
extern STATUS 	etherOutput (struct ifnet *pIf, struct ether_header
		*pEtherHeader, char *pData, int dataLength);
extern STATUS 	etherOutputHookAdd ();
extern void 	etherInputHookDelete ();
extern void 	etherOutputHookDelete ();

extern USHORT etherTypeGet (char *pPacket);

#else	/* __STDC__ */

extern STATUS 	etherAddrResolve ();
extern STATUS 	etherInputHookAdd ();
extern STATUS 	etherOutput ();
extern STATUS 	etherOutputHookAdd ();
extern void 	etherInputHookDelete ();
extern void 	etherOutputHookDelete ();
extern USHORT etherTypeGet ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCetherLibh */
