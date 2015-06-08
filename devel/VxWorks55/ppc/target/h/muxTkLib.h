/* muxTkLib.h - definitions for muxTkLib.c */

/* Copyright 1984-2003 Wind River Systems, Inc. */

/* 
modification history
--------------------
01k,14nov02,wap  Put back prototype for muxTkReceive(). (SPR 83032)
01j,10oct01,rae  merge from truestack (add muxTkCookieGet)
01i,16oct00,niq  Integrating T3 DHCP
01h,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
01g,30mar99,pul  removed all muxMCastMap and muxSvcFunc function declarations
01f,24feb99,sj   added muxTkPollReceive
01e,23feb99,sj   added spare data last argument for muxTkSend and muxTkPollSend
01d,06nov98,sj   removed prototypes for muxTkReceive and nptHookRtn
01c,27oct98,pul  changed nptHookRtn declaration to return STATUS.
01b,09oct98,sj   fixed bad prototype for muxMCastMapFuncGet
01a,07oct98,pul  written.
*/

/*
DESCRIPTION
This file includes function prototypes for the MUX.

INCLUDE FILES: muxLib.h
*/

#ifndef __INCmuxTkLibh
#define __INCmuxTkLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */
#include "muxLib.h"
/* defines */

/* typedefs */

/* locals */

/* globals */ 
IMPORT int muxMaxBinds;

/* forward declarations */

/* Toolkit functions */
IMPORT STATUS muxTkLibInit (void);
IMPORT int muxTkDrvCheck (char * pDevName);

IMPORT void *  muxTkBind (char * pName, int unit,
		          BOOL (*stackRcvRtn) (void *, long, M_BLK_ID, void *),
		          STATUS (*stackShutdownRtn) (void *),
		          STATUS (*stackTxRestartRtn) (void *),
		          void (*stackErrorRtn) (void *, END_ERR *),
		          long type, char * pProtoName, void * pNetSvcStruct,
		          void * pNetSvcInfo, void * pNetDrvInfo);

IMPORT STATUS muxTkRecieve (void *pCookie, M_BLK_ID pMblk, long netSvcOffset,
                            long netSvcType, BOOL uniPromiscuous,
                            void *pSpare);

IMPORT STATUS muxTkSend (void * pCookie, M_BLK_ID pNBuff, char *,
                         USHORT, void *);
IMPORT STATUS muxTkPollSend (void * pCookie, M_BLK_ID pNBuff, char *,
                             USHORT, void *);
IMPORT STATUS muxTkPollReceive (void * pCookie, M_BLK_ID pNBuff, void *pSpare);

IMPORT STATUS muxTkPollReceive2 (void *, M_BLK_ID  pNBuff);

IMPORT void * muxTkCookieGet (char *, int);
#ifdef __cplusplus
}
#endif

#endif /* __INCmuxTkLibh */

