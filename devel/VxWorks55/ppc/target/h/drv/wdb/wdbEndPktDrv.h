/* wdbEndPktLib.h - header file for WDB agents END based packet driver */

/* Copyright 1996-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01k,26apr02,dat  Adding cplusplus protection, SPR 75017
01j,30oct01,jhw Copied version TOR3_1-RC-1.
01i,02aug99,dbt  added WDB standalone agent support.
01h,26feb99,dbt  set WDB END driver MTU to 1500.
01g,25sep97,gnn  SENS beta feedback fixes
01f,22aug97,gnn update of polled mode.
01e,19aug97,gnn changes due to new buffering scheme.
01d,21jan97,gnn added a source NET_ADDRESS.
01c,04nov96,gnn change the MTU size
01b,23oct96,gnn name changes to follow coding standards
01a,01jul96,gnn written.
*/

#ifndef __INCwdbEndPktLibh
#define __INCwdbEndPktLibh

/* includes */

#include "end.h"
#include "wdb/wdb.h"
#include "wdb/wdbCommIfLib.h"
#include "wdb/wdbMbufLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define WDB_END_PKT_MTU           1500    /* MTU of this driver */

/* data types */

typedef struct		/* hidden */
    {
    WDB_DRV_IF	wdbDrvIf;		/* a packet dev must have this first */

    u_int	mode;			/* current mode - poll or int */

    /* END Cookie since we bind to a lower layer. */

    void*	pCookie;

    /* input buffer - loaned to the agent when a packet arrives */

    bool_t	inputBusy;
    M_BLK_ID    pInBlk;

    /* Need a place to store outgoing packet for the driver. */

    M_BLK_ID    pOutBlk;

    /* output buffers - a chain of mbufs given to us by the agent */

    bool_t	outputBusy;

    /* device register addresses etc */

    /* Addressing information. 
     * We have to fake the addressing because the last thing we
     * want is to have to try and broadcast an ARP to find the
     * hardware address of the remote side.
     * The solution is to keep track of the last IP address that
     * we communicated with and the associated hardware address
     * for that host.  
     * If we don't have that info then we send the IP packet
     * in a broadcast Ethernet packet (ff:ff:ff:ff:ff:ff) so
     * so that the host will see it.  This limits our ability
     * to do system level debugging with things like Ethernet
     * to the local network.
     */

    M_BLK_ID lastHAddr;
    M_BLK_ID srcAddr;

    /* device IP address */

    struct in_addr ipAddr;
    } WDB_END_PKT_DEV;

/* function prototypes */

#if defined(__STDC__)

extern STATUS wdbEndPktDevInit
    (
    WDB_END_PKT_DEV *pPktDev,	/* device structure to init */
    void	(*stackRcv)(),	/* receive packet callback (udpRcv) */
    char *	pDevice,	/* Device (ln, ie, etc.) that we wish to */
    				/* bind to. */
    int         unit            /* unit number (0, 1, etc.) */
    );

#else   /* __STDC__ */

extern STATUS    wdbEndPktDevInit ();

#endif  /* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif  /* __INCwdbEndPktLibh */

