/* wtxexch.h - wtx message exchange header file */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,28sep01,fle  SPR#28684 : added usePMap parameter to wtxExchangeCreate ()
01b,11may99,fle  changed wtxExchangeInstall declaration since wtxRpcExchange
		 changes  SPR 67367
01a,30sep96,elp  put in share/src/wtx (SPR# 6775).
01a,15may95s_w  written. Based on original design by John Fogelin
*/

#ifndef __INCwtxexchh
#define __INCwtxexchh	1

#ifdef __cplusplus
extern "C" {
#endif

#include "wtxtypes.h"
#include "wtxerr.h"

/* defines */

#define WTX_EXCHANGE_CTL_TIMEOUT_GET		1
#define WTX_EXCHANGE_CTL_TIMEOUT_SET		2
#define WTX_EXCHANGE_CTL_TRANSPORT_ERR_GET	3

/* typedefs */

typedef struct _wtx_exchange * WTX_XID;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus) || defined(WIN32_COMPILER)

STATUS	wtxExchangeInitialize (WTX_XID * pXid);

STATUS	wtxExchangeInstall	/* install exchange marshall funcs for handle */
    (
    WTX_XID xid, 
    STATUS (*xCreate)		/* connect the exchange id to a WTX server    */
	(
	WTX_XID,		/* WTX exchange ID to create                  */
	const char *,		/* server key string                          */
	BOOL			/* do we use port mapper to connect ?         */
	),
    STATUS (*xDelete)		/* disconnect exchange id from a WTX server   */
	(
	WTX_XID			/* WTX exchange ID                            */
	), 
    STATUS (*xExchange)		/* perform an exchange request                */
	(
	WTX_XID,		/* WTX exchange ID                            */
	WTX_REQUEST,		/* exchange request number                    */
	void *,			/* pointer to input arg message               */
	void *			/* pointer to output arg (result) message     */
	),
    WTX_ERROR_T (*xFree)	/* free result of exchange request            */
	(
	WTX_REQUEST,		/* WTX request number                         */
	void *			/* pointer to result message                  */
	),
    STATUS (*xControl)		/* perform misccontrol requests on exchange   */
	(
	WTX_XID,		/* WTX exchange ID                            */
	UINT32,			/* control request number                     */
	void *			/* pointer to control request arg data        */
	)
    );

WTX_ERROR_T	wtxExchangeErrGet (WTX_XID xid);
STATUS	wtxExchangeErrClear (WTX_XID xid);
STATUS	wtxExchangeTerminate (WTX_XID xid);

STATUS	wtxExchange (WTX_XID xid, WTX_REQUEST request, void * pIn, void * pOut);
STATUS	wtxExchangeFree (WTX_XID xid, WTX_REQUEST request, void * pData);
STATUS	wtxExchangeControl (WTX_XID xid, UINT32 ctlRequest, void * pArg);
STATUS	wtxExchangeCreate (WTX_XID xid, const char * key, BOOL usePMap);
STATUS	wtxExchangeDelete (WTX_XID xid);

#else	/* __STDC__ */

STATUS	wtxExchangeInitialize ();
STATUS	wtxExchangeInstall ();
WTX_ERROR_T	wtxExchangeErrGet ();
STATUS	wtxExchangeErrClear ();
STATUS	wtxExchangeTerminate ();

WTX_ERROR_T wtxExchange ();
WTX_ERROR_T wtxExchangeFree ();
WTX_ERROR_T wtxExchangeControl ();
WTX_ERROR_T wtxExchangeCreate ();
WTX_ERROR_T wtxExchangeDelete ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCwtxexchh */
