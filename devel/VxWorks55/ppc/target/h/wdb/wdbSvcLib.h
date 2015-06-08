/* wdbSvcLib.h - header file for remote debug server */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,25apr02,jhw  Added C++ support (SPR 76304).
01e,11jan99,dbt  added wdbSvcHookAdd() prototype (SPR #24323).
01d,12feb98,dbt  added "dynamic" field in WDB_SVC structure
		 added wdbSvcDsaSvcRemove() declaration.
01c,05apr95,ms   new data types.
01b,06feb95,ms	 added XPORT handle to dispatch routine.
01a,20sep94,ms   written.
*/

#ifndef __INCwdbSvcLibh
#define __INCwdbSvcLibh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "wdb/wdb.h"
#include "wdb/wdbRpcLib.h"

/* data types */

typedef struct			/* hidden */
    {
    u_int	serviceNum;	
    UINT32	(*serviceRtn)();
    xdrproc_t	inProc;
    xdrproc_t	outProc;
    BOOL	dynamic;	/* dynamically loaded service */
    } WDB_SVC;

/* function prototypes */

#if defined(__STDC__)

extern void		wdbSvcLibInit	(WDB_SVC *wdbSvcArray, u_int size);
extern STATUS		wdbSvcAdd	(u_int procNum, UINT32 (*rout)(),
					 BOOL (*xdrIn)(), BOOL (*xdrOut)());
extern void		wdbSvcDispatch	(WDB_XPORT *pXport, uint_t procNum);
extern void		wdbSvcDsaSvcRemove (void);
extern void		wdbSvcHookAdd (FUNCPTR hookRtn, u_int arg);

#else   /* __STDC__ */

extern void		wdbSvcLibInit	();
extern STATUS		wdbSvcAdd	();
extern void		wdbSvcDispatch	();
extern void		wdbSvcDsaSvcRemove ();
extern void		wdbSvcHookAdd ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbSvcLibh */
