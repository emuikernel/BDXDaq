/* usrDnsCfg.c - Domain Name Service setup */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,04aug98,spm  written
*/

/*
DESCRIPTION
This file is used to configure and initialize the DNS server.
It contains the initialization routine for the INCLUDE_DNS_RESOLVER
component and is included by the configuration tool.

NOMANUAL
*/


IMPORT int dnsDebug (void);

LOCAL STATUS usrDnsInit 
    (
    BOOL debugFlag
    )
    {
    STATUS 	result;

    if (debugFlag)
        result = resolvInit (RESOLVER_DOMAIN_SERVER, RESOLVER_DOMAIN, 
                             dnsDebug);
    else
        result = resolvInit (RESOLVER_DOMAIN_SERVER, RESOLVER_DOMAIN, NULL);

    return (result);
    }
