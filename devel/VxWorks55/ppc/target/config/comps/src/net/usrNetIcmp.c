/* usrNetIcmp.c - Configurations for the ICMP library */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the initialization routine for the
INCLUDE_ICMP component.

NOMANUAL
*/


ICMP_CFG_PARAMS icmpCfgParams = /* icmp configuration parameters */
    {
    ICMP_FLAGS_DFLT             /* no icmp mask replies by default */
    };

