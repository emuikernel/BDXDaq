/* wdbTemplatePktLib.h - header file for WDB agents template packet driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,23aug95,ms  written.
*/

#ifndef __INCwdbTemplatePktLibh
#define __INCwdbTemplatePktLibh

/* includes */

#include "sioLib.h"
#include "wdb/wdb.h"
#include "wdb/wdbCommIfLib.h"
#include "wdb/wdbMbufLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define WDB_TEMPLATE_PKT_MTU           1000    /* MTU of this driver */

/* data types */

typedef struct		/* hidden */
    {
    WDB_DRV_IF	wdbDrvIf;		/* a packet dev must have this first */

    u_int	mode;			/* current mode - poll or int */

    /* input buffer - loaned to the agent when a packet arrives */

    bool_t	inputBusy;
    char	inBuf [WDB_TEMPLATE_PKT_MTU];

    /* output buffers - a chain of mbufs given to us by the agent */

    bool_t	outputBusy;

    /* device register addresses etc */

    } WDB_TEMPLATE_PKT_DEV;

/* function prototypes */

#if defined(__STDC__)

extern void	wdbTemplatePktDevInit (WDB_TEMPLATE_PKT_DEV *pPktDev,
				  /* device specific init params, */
				  void (*stackRcv)());

#else   /* __STDC__ */

extern void    wdbTemplatePktDevInit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbTemplatePktLibh */

