/* usrUdp.c - Configuration parameters for UDP */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the configuration parameters for
the UDP library. It is included by the INCLUDE_UDP component.

NOMANUAL
*/



UDP_CFG_PARAMS udpCfgParams =   /* udp configuration parameters */
    {
    UDP_FLAGS_DFLT,
    UDP_SND_SIZE_DFLT,          /* send buffer size */
    UDP_RCV_SIZE_DFLT           /* recv buffer size */
    };

