/* usrProxyClientCfg.c - Initialization routine for the proxy ARP client */

/* Copyright 1992 - 1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,01sep99,spm  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the init routine for the INCLUDE_PROXY_CLIENT
component. It initializes the proxy ARP client.

NOMANUAL
*/


STATUS usrProxyClientInit ()
    {
    char devName [MAX_FILENAME_LENGTH];  /* device name */

    if (sysFlags & SYSFLG_PROXY)
        {
        printf ("registering proxy client %s...", PROXYC_ADDRESS);

        if (proxyReg (PROXYC_DEVICE, PROXYC_ADDRESS) == ERROR)
            {
            printf ("failed: error %x\n", errno);
            return (ERROR);
            }
        printf ("done.\n");
        }
    return (OK);
    }
