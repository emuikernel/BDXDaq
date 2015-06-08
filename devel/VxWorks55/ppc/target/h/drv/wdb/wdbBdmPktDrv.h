/* wdbBdmPktDrv.h - header file for WDB agents bdm packet driver */

/* Copyright 1998-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,16jun98,vrd written.
*/

/*
This is an unsupported release of the Background Debug Mode (BDM) Back
End. It is provided AS IS with no warranties of any kind.
*/

#ifndef __INCwdbBdmPktDrvh
#define __INCwdbBdmPktDrvh

/* includes */

#include "sioLib.h"
#include "wdb/wdb.h"
#include "wdb/wdbCommIfLib.h"
#include "wdb/wdbMbufLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define WDB_BDM_PKT_MTU           1500    /* MTU of this driver */

/* data types */

typedef struct		/* hidden */
    {
    WDB_DRV_IF	wdbDrvIf;		/* a packet dev must have this first */
    u_int	mode;			/* current mode - poll or int */
    } WDB_BDM_PKT_DEV;

/* function prototypes */

#if defined(__STDC__)

extern STATUS wdbBdmPktDevInit (WDB_BDM_PKT_DEV *pPktDev,
				  void (*stackRcv)());

#else   /* __STDC__ */

extern STATUS wdbBdmPktDevInit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbBdmPktDrvh */

