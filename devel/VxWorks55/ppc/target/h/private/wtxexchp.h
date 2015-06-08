/* wtxexchp.h - wtx message exchange private header file */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26feb97,bjl renamed 'private' field in struct _wtx_exchange to 
		'private_data' for c++ compatibility
01a,15may95s_w  written. 
*/

#ifndef __INCwtxexchph
#define __INCwtxexchph	1

#ifdef __cplusplus
extern "C" {
#endif

#include "wtxexch.h"

/* defines */

#define WTX_EXCHANGE_RETURN(handle, errCode, retVal) \
do \
    { \
    handle->error = errCode; \
    return retVal; \
    } while (0)

/* typedefs */

/* This type is private to the exchange protocol implementation */
typedef struct _wtx_exchange 
    {
    WTX_XID	self;		/* pointer to self for validation */
    void *	transport;	/* transport connection data */
    void *	private_data;	/* data private to implementation */
    UINT32	timeout;	/* timeout for exchange service requests */
    WTX_ERROR_T	error;		/* last error code recorded */
    FUNCPTR	xCreate;	/* create connection function */
    FUNCPTR	xDelete;	/* delete connection function */
    FUNCPTR	xExchange;	/* exchange function */
    FUNCPTR	xFree;		/* free result function */
    FUNCPTR	xControl;	/* misc. control function */
    } _WTX_EXCHANGE;		/* WTX exchange handle */

#ifdef __cplusplus
}
#endif

#endif /* __INCwtxexchph */
