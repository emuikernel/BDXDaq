/* usrNetDhcpcCfg.c - Initialization routine for the DHCP client component */

/* Copyright 1992 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,13jan03,vvv  merged from branch wrn_velocecp, ver01d (SPR #83246)
01c,16nov00,spm  fixed description 
01b,17oct00,niq  Integrating T3 DHCP
01c,05apr00,spm  removed startup for other components from initialization
01b,24nov99,spm  upgraded to RFC 2131 and removed direct link-level access
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This file contains the initialization routine for the INCLUDE_DHCPC component.
When DHCP is used to get the target's address and boot file, this routine
executes between the startup routines (in the usrNetDhcpcBootSetup.c module)
for the INCLUDE_DHCPC_LEASE_GET and INCLUDE_DHCPC_LEASE_TEST components.

NOMANUAL
*/

void usrDhcpcStart (void)
    {
    if (backplaneBoot)
        {
        if ((sysBootParams.flags & SYSFLG_AUTOCONFIG) && !(sysBootParams.flags & SYSFLG_PROXY))
            {
            printf ("Warning: DHCP over backplane might require proxy arp.\n");
            }
        }

    if (dhcpcLibInit (DHCPC_SPORT, DHCPC_CPORT, DHCPC_MAX_LEASES,
                      DHCPC_MAX_MSGSIZE, DHCPC_OFFER_TIMEOUT,
                      DHCPC_DEFAULT_LEASE, DHCPC_MIN_LEASE) == ERROR)
        printf ("Error in initializing dhcpcLib\n");

#if (DHCPC_DISCOVER_RETRIES)
    dhcpcDiscoverRetries = DHCPC_DISCOVER_RETRIES;
#endif

    }
