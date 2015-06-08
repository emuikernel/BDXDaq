/* wtxUsrSvc.h - user WTX services DLL interface definition */

/* Copyright 1998  Wind River Systems, Inc. */

/*
01a,09mar98,c_c  Written.
*/

/*
 * This header file represents the interface used by the target server to
 * add user defined WTX services. 
 * If a user wants to add a WTX service to the Target Server, he should build
 * a DLL called "wtxUsrSvc" which exports a 'wtxUserSvcInstall' routine whose
 * prototype is exposed below. This routine is given a pointer to a target
 * Server routine used to add a WTX service. The user will have to supply
 * every parameter to this routine if he wants to see his WTX service added
 * to the target server internal table.
 */

#ifndef __INCwtxUsrSvch
#define __INCwtxUsrSvch

#include "host.h"

/*
 * Prototype of the routine given by the target server to add a WTX service
 * to its database.
 */

typedef STATUS (*TGTSVR_WTX_SVC_ADD_RTN)
    (
    UINT32	serviceNum,	/* RPC service number */
    FUNCPTR	serviceRtn,	/* user supplied routine to be executed */
    FUNCPTR	inProc,		/* XDR deserialization routine */
    FUNCPTR	outProc,	/* XDR serialization routine */
    void *	reserved	/* reserved should be 0 */
    );

/*
 * Prototype of the routine that the user DLL should export.
 */

#ifdef WIN32
__declspec (dllexport) STATUS wtxUserSvcInstall
#else
extern STATUS wtxUserSvcInstall
#endif
    (
    TGTSVR_WTX_SVC_ADD_RTN	tgtsvrWtxSvcAddRtn
    );

#endif	/* __INCwtxUsrSvch */
