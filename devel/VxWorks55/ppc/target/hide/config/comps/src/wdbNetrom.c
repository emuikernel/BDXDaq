/* wdbNetrom.c - WDB netrom communication initialization library */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,22may98,dbt  reworked
01a,11mar98,ms   written
*/

/*
DESCRIPTION
Initializes the netrom connection for the WDB agent.

NOMANUAL
*/

#if	WDB_NETROM_TYPE == 400
#include "wdb/wdbNetromPktDrv.c"
#elif	WDB_NETROM_TYPE == 500
#include "wdb/amc500/wdbNetromPktDrv.c"
#else	/* WDB_NETROM_TYPE != 400 && WDB_NETROM_TYPE != 500 */
#error  WDB_NETROM_TYPE unknown
#endif	/* WDB_NETROM_TYPE == 400 */

#if     WDB_MTU > NETROM_MTU
#undef  WDB_MTU
#define WDB_MTU NETROM_MTU
#endif  /* WDB_MTU > NETROM_MTU */

/******************************************************************************
*
* wdbCommDevInit - initialize the netrom connection
*/

STATUS wdbCommDevInit
    (
    WDB_COMM_IF *	pCommIf,
    char ** 		ppWdbInBuf,
    char **		ppWdbOutBuf
    )
    {
    int				dpOffset;	/* offset of dualport RAM */
    static WDB_NETROM_PKT_DEV	wdbNetromPktDev;/* NETROM packet device */
    static uint_t		wdbInBuf [WDB_MTU/4];
    static uint_t		wdbOutBuf [WDB_MTU/4];

    /* update input & output buffer pointers */

    *ppWdbInBuf = (char *) wdbInBuf;
    *ppWdbOutBuf = (char *) wdbOutBuf;

    /* update communication interface mtu */

    wdbCommMtu = WDB_MTU;

    /* netrom packet driver - supports task or external agent */

    dpOffset = (ROM_SIZE - DUALPORT_SIZE) * WDB_NETROM_WIDTH;

    wdbNetromPktDevInit (&wdbNetromPktDev, (caddr_t)ROM_BASE_ADRS + dpOffset,
			 WDB_NETROM_WIDTH, WDB_NETROM_INDEX,
			 WDB_NETROM_NUM_ACCESS, udpRcv,
			 WDB_NETROM_POLL_DELAY);

    if (udpCommIfInit (pCommIf, &wdbNetromPktDev.wdbDrvIf) == ERROR)
	return (ERROR);

    return (OK);
    }
