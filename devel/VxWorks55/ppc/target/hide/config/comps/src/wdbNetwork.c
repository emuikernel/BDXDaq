/* wdbNetwork.c - WDB network communication initialization library */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,22may98,dbt	 reworked
01a,11mar98,ms	 written
*/

/*
DESCRIPTION
Initializes the network connection for the WDB agent.

NOMANUAL
*/

/******************************************************************************
*
* wdbCommDevInit - initialize the network connection
*
* This routine initializes the network connection used by the WDB agent.
*
* RETURNS :
* OK or ERROR if the network connection can not be initialized.
*
* NOMANUAL
*/

STATUS wdbCommDevInit
    (
    WDB_COMM_IF *	pCommIf,
    char ** 		ppWdbInBuf,
    char **		ppWdbOutBuf
    )
    {
    static uint_t	wdbInBuf [WDB_MTU/4];
    static uint_t	wdbOutBuf [WDB_MTU/4];

    /* update input & output buffer pointers */

    *ppWdbInBuf = (char *) wdbInBuf;
    *ppWdbOutBuf = (char *) wdbOutBuf;

    /* update communication interface mtu */

    wdbCommMtu = WDB_MTU;

    /* UDP sockets - supports a task agent */

    if (wdbUdpSockIfInit (pCommIf) == ERROR)
	return (ERROR);
    else
	return (OK);
    }
