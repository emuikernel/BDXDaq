/* routeSockCfg.c - initialize the socket library */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,14nov00,rae  removed unused argument from sockLibAdd()
01b,30jul99,pul  modify sockLibAdd to add extra parameter for future
                 scalability
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the initialization routine for the 
INCLUDE_ROUTE_SOCK component. It is responsible for calling the routine 
to initialize the routing sockets.

NOMANUAL
*/


STATUS routeSockInit()
    {
    if (sockLibAdd ((FUNCPTR) bsdSockLibInit, AF_ROUTE, AF_ROUTE) == ERROR)
        return (ERROR);

    routeSockLibInit () ;
    return (OK);
    }
