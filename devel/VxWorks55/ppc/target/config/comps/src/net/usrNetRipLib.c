/* usrNetRipLib.c - Initialization routine for the RIP component */

/* Copyright 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,19dec02,vvv  written (SPR #83863, SPR #83022)
*/

/*
DESCRIPTION
This file contains the initialization routine for the INCLUDE_RIP 
component.

NOMANUAL
*/

extern BOOL initInterfaces;
extern int  recvBufSize;

void usrRipLibInit ()
    {
    initInterfaces = RIP_IF_INIT;
    recvBufSize = RIP_ROUTING_SOCKET_BUF_SIZE;

    if (ripLibInit (RIP_SUPPLIER, RIP_GATEWAY, RIP_MULTICAST, RIP_VERSION,
		    RIP_TIMER_RATE, RIP_SUPPLY_INTERVAL, RIP_EXPIRE_TIME,
		    RIP_GARBAGE_TIME, RIP_AUTH_TYPE) == ERROR)
        printf ("RIP initialization failed\n");

    return;
    }
