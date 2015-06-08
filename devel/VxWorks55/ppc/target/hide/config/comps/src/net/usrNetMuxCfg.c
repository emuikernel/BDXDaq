/* usrNetMuxCfg.c - Configurations for the MUX library */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,30Jul99,puli  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette initializes the muxMaxBinds extern and initializes the MUX
library.

NOMANUAL
*/
void usrMuxLibInit(void)
    {
    muxMaxBinds=MUX_MAX_BINDS;
    muxLibInit();
    }
