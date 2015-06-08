/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************


*******************************************************************************
* mgiEnd.h - Header File for : MV-643xx network interface header 
*
* DESCRIPTION:
*       This header includes definitions, typedefs and structs needed for the 
*		mgi END driver operations.
*
* DEPENDENCIES:
*       WRS endLib library.
*
******************************************************************************/


#ifndef __INCmgiEndh
#define __INCmgiEndh

/* includes */

#include "etherLib.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/* defines */
#define RX_BUFFER_DEFAULT_SIZE      0x600	
#define TX_BUFFER_DEFAULT_SIZE      0x600

#define END_RX_SEM_TAKE(semId,tmout) \
	    (semTake (semId,tmout))

#define END_RX_SEM_GIVE(semId) \
	    (semGive (semId))

 
#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDrvCtrl, ethCause, pFunc, arg, pRet)  	\
{                                                              	\
	*pRet = OK;                                                	\
	*pRet = bspEndIntConnect (pDrvCtrl->ethPortCtrl.portNum,	\
							  ethCause, 						\
							  (pFunc), 							\
							  (int) (arg),						\
							   0);								\
}
	

#endif /* SYS_INT_CONNECT */
 
#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDrvCtrl, pFunc, arg, pRet)          \
{                                                               \
	*pRet = OK;                                                 \
	*pRet = bspEndIntDisconnect (pDrvCtrl->ethPortCtrl.portNum, \
								 (pFunc),						\
								 (int) (arg),					\
								 0);							\
}

#endif /* SYS_INT_DISCONNECT */
 
#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(pDrvCtrl, ethCause)                      \
	bspEndIntEnable(pDrvCtrl->ethPortCtrl.portNum, ethCause)
#endif /*SYS_INT_ENABLE*/
 
#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDrvCtrl, ethCause)                     \
	bspEndIntDisable(pDrvCtrl->ethPortCtrl.portNum, ethCause)
#endif /*SYS_INT_DISABLE*/
  
#ifndef LOCAL_TO_SYS_ADDR
#define LOCAL_TO_SYS_ADDR(unit,localAddr)                       \
    ((int) pDrvCtrl->ethPortCtrl.sysLocalToBus ?                \
    (*pDrvCtrl->ethPortCtrl.sysLocalToBus) (unit, localAddr) : localAddr)
#endif /* LOCAL_TO_SYS_ADDR */
 
#ifndef SYS_TO_LOCAL_ADDR
#define SYS_TO_LOCAL_ADDR(unit,sysAddr)                         \
    ((int) pDrvCtrl->ethPortCtrl.sysBusToLocal ?                \
    (*pDrvCtrl->ethPortCtrl.sysBusToLocal)(unit, sysAddr) : sysAddr)
#endif /* SYS_TO_LOCAL_ADDR */
 

#define DEV_NAME        "mgi"
#define DEV_NAME_LEN    7
#define DEF_NUM_CFDS    32          /* default number of CFDs (Hex) */
#define DEF_NUM_RFDS    32          /* default number of RFDs (Hex) */
#define MGI_100MBS   100000000	/* bits per sec */
#define MGI_10MBS    10000000    /* bits per sec */
#define EADDR_LEN       6           /* Ethernet's address length */
#define MAC_STR_SIZE	18			/* MAC addr string size 00:00:00:00:00:00 */
 

#define EH_SIZE		14  			/* avoid structure padding issues */
#define N_MCAST		12

#define TX_QUEUE	ETH_Q0
#define RX_QUEUE	ETH_Q0


/* board specific infomation */
#ifdef __cplusplus
}
#endif

IMPORT bool frcMACRead(UINT8, UINT8 *);

#endif /* __INCmgiEndh */

