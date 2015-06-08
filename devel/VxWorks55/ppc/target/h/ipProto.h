/* ipProto.h - defines for the bsd protocol pseudo-device driver */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01p,10oct01,rae  merge from truestack (SPRs 29668, 65773)
01o,07nov00,spm  moved nptFlag from END_OBJ for T2 END binary compatibility
01n,17oct00,spm  merged from version 01m of tor3_x branch (base version 01i):
                 added backward compatibility for END devices
01m,02aug99,pul  adding #include if_ether.h
01l,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
01k,02mar99,pul  protoType for ipAttach(), SPR# 24253
01j,08aug98,ann  Included if_ether.h to reference idr
01i,08dec97,gnn  END code review fixes.
01h,25sep97,gnn  SENS beta feedback fixes
01g,19aug97,gnn  changes due to new buffering scheme.
01f,12aug97,gnn  changes necessitated by MUX/END update.
01e,02jun97,gnn  Added added memWidth to structure.
01d,21mar97,map  Added pRefCntHead, removed nLoan from BSD_DRV_CTRL.
01c,22jan97,gnn  Added new private flags that is seperate from if flags.
01b,21jan97,gnn  Removed buffer loaning stuff.
                 Added reference count information.
                 Added minFirstBuf for scatter/gather.
01a,17dec96,gnn	 written.

*/
 
#ifndef __INCipProtoh
#define __INCipProtoh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "netinet/if_ether.h"

/* defints */

/* typedefs */
typedef struct ip_drv_ctrl
    {
    struct arpcom idr;                /* Interface Data Record */
    void* 	pArpCookie;
    void* 	pIpCookie;
    int		attached;
    BOOL 	nptFlag; 	/* Indicates type of interface: END or NPT. */
    M_BLK_ID 	pSrc; 		/* source address and type (for END device). */
    M_BLK_ID    pDst; 		/* dest. address and type (for END device). */
    char * 	pDstAddr; 	/* cluster containing dest. address */
    } IP_DRV_CTRL;

/* The array of driver control structures */

extern IP_DRV_CTRL ipDrvCtrl[];

/* forward declarations */

/* globals */
IMPORT int ipAttach (int unit, char *pDevice);
IMPORT STATUS ipDetach (int unit, char *pDevice);
IMPORT void *ipMuxCookieGet (unsigned short ifIndex);

/* locals */

/* forward declarations */

#ifdef __cplusplus
}
#endif

#endif /* __INCipProtoh */
