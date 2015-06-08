/* usrProxyServerCfg.c - Initialization routine for the proxy ARP server */

/* Copyright 1992 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26mar02,vvv  check for errors (SPR #73503)
01a,01sep99,spm  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the init routine for the INCLUDE_PROXY_SERVER
component. It initializes the proxy ARP server. The PROXYD_MAIN_ADDRESS
and PROXYD_PROXY_ADDRESS parameters provide the IP addresses of active
interfaces connected to each network. The default values will extract
those addresses from the boot line.

NOMANUAL
*/


STATUS usrProxyServerInit ()
    {
    int proxymask;

    /* Remove netmask from address if present - not needed by proxy service. */

    bootNetmaskExtract (PROXYD_PROXY_ADDRESS, &proxymask);

    hashLibInit ();         /* make sure hash functions are installed */

    if (proxyArpLibInit (8, 8) == ERROR)
	printf ("Unable to initialize proxy server.\n");
    else
	{
        printf ("Creating proxy network: %s\n", PROXYD_PROXY_ADDRESS);
        if (proxyNetCreate (PROXYD_PROXY_ADDRESS, PROXYD_MAIN_ADDRESS) == ERROR)
            {
            printf ("proxyNetCreate failed:%x\n", errno);
            return (ERROR);
            }
        }
    }
