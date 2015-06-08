/* wdbPipePktLib.h - header file for WDB agents pipe packet driver */

/* Copyright 1998-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,26apr02,dat  Adding cplusplus protection, SPR 75017
01c,21may98,dbt  modified wdbPipePktDevInit prototype.
01b,09apr98,pdn  fixed the WDB_PIPE_PKT_MTU to match w/ tgtsvr.
01a,11mar98,pdn  written.
*/

#ifndef __INCwdbPipePktLibh
#define __INCwdbPipePktLibh

/* includes */

#include "sioLib.h"
#include "wdb/wdb.h"
#include "wdb/wdbCommIfLib.h"
#include "wdb/wdbMbufLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define WDB_PIPE_PKT_MTU           1500    /* MTU of this driver */

/* data types */

typedef struct		/* hidden */
    {
    WDB_DRV_IF	wdbDrvIf;		/* a packet dev must have this first */

    u_int	mode;			/* current mode - poll or int */

    /* input buffer - loaned to the agent when a packet arrives */

    bool_t	inputBusy;
    char	inBuf [WDB_PIPE_PKT_MTU + sizeof(short)];

    /* output buffers - a chain of mbufs given to us by the agent */

    bool_t	outputBusy;

    int		h2t_fd;
    int		t2h_fd;

    /* device register addresses etc */

    } WDB_PIPE_PKT_DEV;

/* function prototypes */

#if defined(__STDC__)

extern STATUS	wdbPipePktDevInit (WDB_PIPE_PKT_DEV *pPktDev,
				  /* device specific init params, */
				  void (*stackRcv)());

#else   /* __STDC__ */

extern STATUS	wdbPipePktDevInit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbPipePktLibh */

