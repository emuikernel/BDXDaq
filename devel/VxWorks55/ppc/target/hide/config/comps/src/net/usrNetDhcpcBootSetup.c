/* usrNetDhcpcBootSetup.c - process lease information in an address string */

/* Copyright 1992 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,09oct01,rae  merge from truestack (fixed call to dhcpcConfigSet)
01b,16nov00,spm  enabled new DHCP lease for runtime device setup (SPR #20438)
01a,05dec97,spm  added DHCP code review modifications
*/

/*
DESCRIPTION
This file is included by the configuration tool to handle DHCP processing
during the system startup. The initialization routine for the 
INCLUDE_DHCPC_LEASE_GET component removes any lease information from the
target address parameter in the boot line. The initialization routine for 
the INCLUDE_DHCPC_LEASE_TEST component renews the associated lease if that
information is available. Otherwise, it attempts to establish a new lease
if the target address is not specified.

NOMANUAL
*/

/* locals */

LOCAL BOOL dhcpBootFlag = FALSE;  /* Set TRUE if a lease timestamp is found. */

/*******************************************************************************
*
* usrDhcpcLeaseGet - extract DHCP timestamp information from address
*
* This routine stores any values for the DHCP lease origin and lease duration
* in the dhcpcBootLease variable and sets the dhcpBoot flag to indicate
* if those values were found. It is the initialization routine for the 
* INCLUDE_DHCPC_LEASE_GET component which is automatically added when both the 
* INCLUDE_DHCPC and INCLUDE_NET_INIT components are used.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrDhcpcLeaseGet (void)
    {
    /* 
     * Remove the timestamp from the address string if automatic configuration
     * (using the DHCP client) is active.
     */

    if (sysBootParams.flags & SYSFLG_AUTOCONFIG)
        if (dhcpcLeaseGet (pAddrString, &dhcpBootFlag) == ERROR)
            {
            printf ("Unable to verify assigned DHCP address.\n");
            netDevBootFlag = TRUE;    /* Prevents further processing. */
            }
    return;
    }

/*******************************************************************************
*
* usrDhcpcLeaseTest - renew DHCP lease stored in boot parameters
*
* This routine attempts to renew any existing DHCP lease created at boot
* time. It is the initialization routine for the INCLUDE_DHCPC_LEASE_TEST 
* component which is automatically added when both the INCLUDE_DHCPC and 
* INCLUDE_NET_INIT components are used. If no lease is available and no
* target address is specified, the routine will attempt to establish a
* new lease.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrDhcpcLeaseTest (void)
    {
    /*
     * Renew or obtain a lease as needed. If successful, the pDhcpcBootCookie
     * global is set to allow user access to the lease and the timing
     * information is stored in the dhcpcBootLease global for later storage
     * in the boot line.
     */

    if (dhcpcConfigSet (&sysBootParams, pAddrString, &netmask, &dhcpBootFlag, 
			netAttachFlag) == ERROR)
        netDevBootFlag = TRUE;

    return;
    }

/*******************************************************************************
*
* usrDhcpcLeaseSave - preserve new DHCP lease in boot parameters
*
* This routine stores the timestamp information for a new DHCP lease
* established during the runtime startup. It is the initialization
* routine for the INCLUDE_DHCPC_LEASE_SAVE component which is
* automatically added when both the INCLUDE_DHCPC and INCLUDE_NET_INIT
* components are used.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrDhcpcLeaseSave (void)
    {
    char numString [30];    /* buffer for numeric string with timestamps */

    /*
     * If the target address is not manually assigned, add timing
     * information so later reboots will detect the DHCP lease.
     */

    if (dhcpBootFlag == TRUE)
        {
        if (netmask == 0)
            sprintf (numString, "::%lx:%lx", dhcpcBootLease.lease_duration,
                                             dhcpcBootLease.lease_origin);
        else
            sprintf (numString, ":%x:%lx:%lx", ntohl (netmask),
                                               dhcpcBootLease.lease_duration,
                                               dhcpcBootLease.lease_origin);
        if (backplaneBoot)
            strcat (sysBootParams.bad, numString);
        else
            strcat (sysBootParams.ead, numString);

        bootStructToString (BOOT_LINE_ADRS, &sysBootParams);
        }

    return;
    }
