/* endLib.h - definitions for Enhanced Network Drivers */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01r,22jan03,rcs  Added MBLK_SIZE and CLBLK_SIZE
01q,13jan03,rae  Merged from velocecp branch (SPR 83033)
01p,17may02,rcs  add for MIB-II interface callbacks. SPR# 77478
01o,08nov01,wap  Add missing prototype for txRcvRtnCall()
01n,10oct01,rae  update copyright
01m,27jun01,rcs  Merge Tor2.0.2 to Tornado-Comp-Drv
01l,23oct00,rae  fixed SPR #33687 by removing semicolon from END_OBJ_READY
01k,16oct00,spm  merged version 01k from tor3_0_x branch (base version 01j):
                 adds link-level broadcast support
01j,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
01i,18mar99,sj   fixed typo in TK_RCV_RTN_CALL
01h,08dec97,gnn  END code review fixes.
01g,17oct97,vin  changed prototypes.
01f,25sep97,gnn  SENS beta feedback fixes
01e,19aug97,gnn  changes due to new buffering scheme.
01d,12aug97,gnn  changes necessitated by MUX/END update.
01c,07apr97,map  Added more prototypes and END_OBJ_UNLOAD.
01b,03feb97,gnn  Added speed as an argument to MIB stuff.
01a,26dec96,gnn	 written.

*/
 
/*
DESCRIPTION

This include file contains definitions used by the Enhanced Network Driver
library (endLib.c) and by all of the ENDs.

If the macro END_MACROS is set during compilation then all of the functions in
the library will included as macros instead of functions, otherwise all of the
routines will be functions.  This is a time/space tradeoff that can be made at
compile time.

INCLUDE FILES:
*/

#ifndef __INCendLibh
#define __INCendLibh

#ifdef __cplusplus
extern "C" {
#endif


/* includes */
#include "vxWorks.h"
#include "end.h"
#include "muxLib.h"

/* defines */

#define MBLK_SIZE               64
#define CLBLK_SIZE              64

#define MIB2_IN_ERRS	0
#define MIB2_IN_UCAST	1
#define MIB2_OUT_ERRS	2
#define MIB2_OUT_UCAST	3

#ifdef END_MACROS

#define MIB_ALLOC(m2Id, ifType, enetAddr, addrLen, mtu, speed, name, unit)   \
if ((pMibRtn != NULL) && (pMibRtn->mibAlloc != NULL))			     \
    ((* (pMibRtn->mibAlloc)) (m2Id, ifType, (UINT8 *)enetAddr, addrLen, mtu, \
                              speed, name, unit))                            

#define MIB_FREE(m2Id)							    \
if ((pMibRtn != NULL) && (pMibRtn->mibFree != NULL))			    \
    ((* (pMibRtn->mibFree)) (m2Id))

#define MIB_CNT_UPDATE(m2Id, cntName, pktData, pktLen)			    \
if ((pMibRtn != NULL) && (pMibRtn->mibCntUpdate != NULL))		    \
    ((* (pMibRtn->mibCntUpdate)) (m2Id, cntName, pktLen, pktData))

#define MIB_VAR_UPDATE(m2Id, varName, data)			    \
if ((pMibRtn != NULL) && (pMibRtn->mibVarUpdate != NULL))		    \
    ((* (pMibRtn->mibVarUpdate)) (m2Id, varName, (caddr_t)data))


#define END_RCV_RTN_CALL(pEnd,pMblk) \
	    if ((pEnd)->receiveRtn) \
		{ \
		(pEnd)->receiveRtn ((pEnd), pMblk,NULL,NULL,NULL,NULL); \
		}

#define TK_RCV_RTN_CALL(pEnd,pMblk, netSvcOffset, netSvcType, \
			uniPromiscuous, pSpareData) \
            if ((pEnd)->receiveRtn) \
                { \
                (pEnd)->receiveRtn ((pEnd), pMblk, netSvcOffset, netSvcType, \
				    uniPromiscuous, pSpareData); \
                }

#define END_TX_SEM_TAKE(pEnd,tmout) \
	    (semTake ((pEnd)->txSem,tmout))

#define END_TX_SEM_GIVE(pEnd) \
	    (semGive ((pEnd)->txSem))

#define	END_FLAGS_CLR(pEnd,clrBits) \
            ((pEnd)->flags &= ~(clrBits))

#define	END_FLAGS_SET(pEnd,setBits) \
            ((pEnd)->flags |= (setBits))

#define	END_FLAGS_GET(pEnd) \
	    ((pEnd)->flags)

#define END_MULTI_LST_CNT(pEnd) \
	    (lstCount (&(pEnd)->multiList))

#define END_MULTI_LST_FIRST(pEnd) \
	    (ETHER_MULTI *)(lstFirst (&(pEnd)->multiList))

#define END_MULTI_LST_NEXT(pCurrent) \
	    (ETHER_MULTI *)(lstNext (&pCurrent->node))

#define END_DEV_NAME(end) \
	    (end.devObject.name)
		
#define END_OBJECT_UNLOAD(pEnd) \
	    { \
	    lstFree (&(pEnd)->multiList); \
	    lstFree (&(pEnd)->protocols); \
	    }

#define	END_OBJ_INIT(pEnd,pDev,name,unit,pFuncs, pDesc) \
	    endObjInit ((pEnd),pDev,name,unit,pFuncs, pDesc)

#define END_OBJ_READY(pEnd, flags) \
	    endObjFlagSet ((pEnd),flags)

#define END_ERR_ADD(pEnd,code,value) \
	    (mib2ErrorAdd(&(pEnd)->mib2Tbl, code, value))

#define END_MIB_INIT(pEnd,type,addr,addrLen,mtu,speed) \
	    (mib2Init(&(pEnd)->mib2Tbl, type,addr, addrLen, mtu, speed))
#else

#define END_RCV_RTN_CALL(pEnd,pData) \
                endRcvRtnCall((pEnd), pData)
                
#define TK_RCV_RTN_CALL(pEnd,pData, netSvcOffset, netSvcType, \
			uniPromiscuous, pSpareData) \
            tkRcvRtnCall((pEnd), pData, netSvcOffset, netSvcType, \
			     uniPromiscuous, pSpareData)

#define END_TX_SEM_TAKE(pEnd,tmout) \
                endTxSemTake((pEnd), tmout)

#define END_TX_SEM_GIVE(pEnd) \
                endTxSemGive(pEnd)

#define	END_FLAGS_CLR(pEnd,clrBits) \
                endFlagsClr((pEnd), clrBits)

#define	END_FLAGS_SET(pEnd,setBits) \
                endFlagsSet((pEnd), setBits)

#define	END_FLAGS_GET(pEnd) \
                (endFlagsGet((pEnd)))

#define END_MULTI_LST_CNT(pEnd) \
                (endMultiLstCnt((pEnd)))

#define END_MULTI_LST_FIRST(pEnd) \
                (endMultiLstFirst((pEnd)))

#define END_MULTI_LST_NEXT(pCurrent) \
                (endMultiLstNext((pCurrent)))

#define END_DEV_NAME(pEnd) \
                (endDevName((pEnd)))
		
#define END_OBJECT_UNLOAD(pEnd) \
                endObjectUnload((pEnd))
                
#define END_OBJ_UNLOAD(X)	END_OBJECT_UNLOAD(X)


#define	END_OBJ_INIT(pEnd,pDev,name,unit,pFuncs, pDescription) \
	    endObjInit ((pEnd),pDev,name,unit,pFuncs, pDescription)

#define END_OBJ_READY(pEnd, flags) \
	    endObjFlagSet ((pEnd),flags)

#define END_ERR_ADD(pEnd,code,value) \
	    (mib2ErrorAdd(&(pEnd)->mib2Tbl, code, value))

#define END_MIB_INIT(pEnd,type,addr,len,mtu,speed) \
	    (mib2Init(&(pEnd)->mib2Tbl, type, addr, len, mtu, speed))
#endif /* END_MACROS */
                
/* typedefs */

typedef struct mib_routines
    {
    FUNCPTR mibAlloc; 
    FUNCPTR mibFree; 
    FUNCPTR mibCntUpdate; 
    FUNCPTR mibVarUpdate;
    } MIB_ROUTINES;


/* globals */

IMPORT MIB_ROUTINES * pMibRtn; 

/* locals */

/* forward declarations */

STATUS endMibIfInit (FUNCPTR pMibAllocRtn, FUNCPTR pMibFreeRtn,
                     FUNCPTR pMibCtrUpdate, FUNCPTR pMibVarUpdate); 
STATUS	endObjInit (END_OBJ* pEndObj, DEV_OBJ* pDevice,
                    char* pBaseName ,int unit, NET_FUNCS* pFuncTable,
                    char* pDescription);
STATUS	endObjFlagSet (END_OBJ*,UINT);
STATUS	mib2Init (M2_INTERFACETBL*,long,UCHAR*,int, int, int);
STATUS	mib2ErrorAdd (M2_INTERFACETBL*, int, int);
void	endTxSemTake (END_OBJ*, int);
void	endTxSemGive (END_OBJ*);
void	endRcvRtnCall (END_OBJ*, M_BLK_ID);
void	tkRcvRtnCall (END_OBJ *, M_BLK_ID, long, long, BOOL, void *);
void	endObjectUnload (END_OBJ*);
void	endFlagsSet (END_OBJ *, int);
void	endFlagsClr (END_OBJ *, int);
int	endFlagsGet (END_OBJ *);
ETHER_MULTI*	endMultiLstFirst (END_OBJ *);
ETHER_MULTI*	endMultiLstNext (ETHER_MULTI *);
M_BLK_ID    endEtherAddressForm (M_BLK_ID pNBuff, M_BLK_ID pSrcAddr,
                                 M_BLK_ID pDstAddr, BOOL bcastFlag);
M_BLK_ID    end8023AddressForm (M_BLK_ID pNBuff, M_BLK_ID pSrcAddr,
                                M_BLK_ID pDstAddr, BOOL bcastFlag);
STATUS  endEtherPacketDataGet (M_BLK_ID pMblk, LL_HDR_INFO * pLinkHdrInfo);
STATUS  endEtherPacketAddrGet (M_BLK_ID pMblk,
                               M_BLK_ID pSrc,
                               M_BLK_ID pDst,
                               M_BLK_ID pESrc,
                               M_BLK_ID pEDst);
STATUS endEtherResolve (void* arg1, void* arg2, void* arg3, void* arg4,
                        void* arg5, void* arg6);

STATUS endPollStatsInit (void *, FUNCPTR);

#ifdef __cplusplus
}
#endif

#endif /* __INCendLibh */
