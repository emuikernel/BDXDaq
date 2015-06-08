/* muxLibP.h - private definitions for the MUX library */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01r,10oct01,rae  merge from truestack (update copyright)
01q,25oct00,niq  Merge RFC2233 changes from open_stack-p1-r1 branch
                 01q,18oct00,ann  insterting Siemens bug #25 for 0 mac addresses
01p,05oct99,pul  removing rtRequest and mCastMap
01o,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
01n,24mar99,sj   changed tkFlag field in muxBindEntry to flags; added defines
01m,24mar99,sj   added defines for flags field of NET_PROTOCOL structure
01l,18mar99,sj   cleaned up: completed the cleanup postponed in 01j,05mar99,sj
01k,08mar99,sj   added place holder for stackrcv routine in BIB entry
01j,05mar99,sj   eliminated hooks; not cleanly though. next version will do that
01i,24feb99,sj   added netSvcType and netDrvType fields to the BIB entry
01h,26jan99,sj   moved BIB entry definition from muxTkLib.c to here
01g,05nov98,sj   PCOOKIE_TO_ENDOBJ must check if pCookie is NULL
01f,03nov98,pul  modified IPPROTO_MCAST_MAP_GET 
01e,03nov98,sj   doc update
01d,20oct98,pul	 don't return ERROR if hook is not valid
01c,15oct98,pul	 added ipProto Hooks 
01b,12oct98,sj 	 changed MUX_TK_ADDR_RES_FUNC_UPDATE, added WRS_*_MODULE 
01a,06oct98,sj 	 written.

*/
 
/*
DESCRIPTION
This file includes macros used by muxLib to avail features of the NPT
architecture if it is installed.


INCLUDE FILES:
*/

#ifndef __INCmuxLibPh
#define __INCmuxLibPh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "muxLib.h"

/* we steal these defines from a END driver */

#define END_HADDR(pEnd)                                                 \
                ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd)                                             \
                ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

/* Alternate defines for the above till all drivers have been changed 
 * to follow rfc2233 implementation
 */
#define END_ALT_HADDR(pEnd) \
                ((pEnd)->pMib2Tbl->m2Data.mibIfTbl.ifPhysAddress.phyAddress)
#define END_ALT_HADDR_LEN(pEnd) \
                ((pEnd)->pMib2Tbl->m2Data.mibIfTbl.ifPhysAddress.addrLength)


                    
#define END_MIB2_TYPE(pEnd)                                             \
                ((pEnd)->mib2Tbl.ifType)

#define END_IOCTL(pEnd)                                             \
		(pEnd->pFuncTable->ioctl)

/*
 * we define these flag bits for the flags field in the NET_PROTOCOL structure
 * that is allocated for each protocol bound to an END
 */

#define MUX_TK_DEV  0x1 	/* protocol is bound to a toolkit device */
#define MUX_BIND    0x2		/* END is bound to using muxBind() */
#define MUX_TK_BIND 0x4		/* END is bound to using muxTkBind() */

/*
 * we define these flag bits for the flags field in the muxBindEntry defined
 * below
 */
#define BIB_DRV_ENTRY      0x1	/* driver loaded at this entry */
#define BIB_TK_DRV         0x2	/* driver is a toolkit(NPT) driver */
#define BIB_PROTO_ENTRY    0x4	/* a protocol has this BIB entry */

/*
 * This defines the structure of the entry in the MUX information base(BIB).
 * There is one such entry for every protocol/network device binding instance
 */

typedef struct muxBindEntry
    {
    int unitNo;                 /* unit number of the device */
    char devName[END_NAME_MAX]; /* device name */
    long netSvcType;		/* network service type */
    long netDrvType;		/* network Driver type */
    UINT32 flags;               /* BIB entry flags */
    END_OBJ* pEnd;              /* reference to the END object */
    void* pNetCallbackId;       /* network service callback Id given at bind */
    FUNCPTR addrResFunc;        /* reference to Address resolution function */
    FUNCPTR netStackRcvRtn;     /* reference to network service stackRcvRtn */
    }MUX_BIND_ENTRY, *MUX_ID;

/* convert from a void * cookie(which is a MUX_ID) to a END_OBJ pointer */

#define PCOOKIE_TO_ENDOBJ(pCookie) ((pCookie) ? ((MUX_ID)pCookie)->pEnd : NULL)

/*
 * These macros are used by muxAddrRecFuncXXX functions
 */
 #define TK_ADDR_RES_GET 1
 #define TK_ADDR_RES_ADD 2
 #define TK_ADDR_RES_DEL 3

/* prototypes */

/* 
 * these functions are used by the SENS MUX API implementations to avail
 * NPT features
 */
extern STATUS muxTkBibInit (void);
extern void * muxTkDevLoadUpdate (END_OBJ * pEnd);
extern void * muxTkBindUpdate (END_OBJ * pEnd, NET_PROTOCOL * pProto);
extern void muxTkUnbindUpdate (void * pCookie);
extern STATUS muxTkUnloadUpdate (END_OBJ * pEnd);
extern void * muxTkAddrFuncUpdate (long netDrvType, long netProtoType, 
				   FUNCPTR pAddrResFunc, int update);
#ifdef __cplusplus
}
#endif

#endif /* __INCmuxLibPh */
