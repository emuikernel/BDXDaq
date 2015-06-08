/* usrTcp.c - Configuration parameters for the TCP library */

/* Copyright 1992 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,13jan03,vvv  merged from branch wrn_velocecp, ver01c (SPR #82107)
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the configuration parameters for
the TCP library. It is included by the INCLUDE_TCP component.

NOMANUAL
*/


TCP_CFG_PARAMS tcpCfgParams =   /* tcp configuration parameters */
    {
    TCP_FLAGS_DFLT,             /* include rfc1323 support */
    TCP_SND_SIZE_DFLT,          /* default send buffer size */
    TCP_RCV_SIZE_DFLT,          /* default recv buffer size */
    TCP_CON_TIMEO_DFLT,         /* initial connection time out */
    TCP_REXMT_THLD_DFLT,        /* retransmit threshold */
    TCP_MSS_DFLT,               /* default maximum segment size */
    TCP_RND_TRIP_DFLT,          /* default round trip time */
    TCP_IDLE_TIMEO_DFLT,        /* idle timeouts before first probe */
    TCP_MAX_PROBE_DFLT,         /* max no. probes before dropping */
    TCP_RAND_FUNC,              /* Random Function for TCP */
    TCP_MSL_CFG                 /* maximum segment lifetime */
    };

