/* usrNetShow.c - Support for network show routines */

/* Copyright 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,28mar02,vvv  written
*/

/*
DESCRIPTION
This file provides support for the network show routines, specifically the
route display routines routeShow, arpShow and mRouteShow. It sets up the
configuration parameters for correct functioning of these routines.

NOMANUAL
*/

const UINT rtMem = RT_DISPLAY_MEMORY;
const BOOL bufferedRtShow = RT_BUFFERED_DISPLAY;
