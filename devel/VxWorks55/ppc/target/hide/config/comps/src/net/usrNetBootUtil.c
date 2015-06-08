/* usrNetBootUtil.c - generic utilities for handling network boot parameters */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,05dec97,spm  added DHCP code review modifications
*/

/*
DESCRIPTION
This file is included by the configuration tool to handle special cases
associated with starting a network device according to the parameters in the 
boot line. It contains the initialization routines for related components
added by the configuration tool.

NOMANUAL
*/

LOCAL STATUS dhcpcBootLineClean (char *);

/******************************************************************************
*
* usrDhcpcLeaseClean - remove DHCP timestamp information from address
*
* This routine deletes any values for the DHCP lease origin and lease duration
* in the selected address string. It is the initialization routine for the 
* INCLUDE_DHCPC_LEASE_CLEAN component which is automatically added when the 
* INCLUDE_NET_INIT component is used, unless the INCLUDE_DHCPC component 
* is also included. If DHCP timestamp values are found, processing can't 
* continue because the corresponding active DHCP lease cannot be maintained.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrDhcpcLeaseClean (void)
    {
    /* Check address for DHCP lease information. */ 

    if (sysBootParams.flags & SYSFLG_AUTOCONFIG)
        if (dhcpcBootLineClean (pAddrString) == OK)
            {
            printf ("Can't use dynamic address %s without DHCP.\n",
                    pAddrString);
            netDevBootFlag = TRUE;    /* Prevents further processing. */
            }

    return;
    }

/******************************************************************************
*
* dhcpcBootLineClean - remove DHCP information from system bootline
*
* This routine removes the DHCP information (lease duration and origin) from 
* the client address string in the bootline. Those values are present
* if the target IP address obtained during system boot was provided by a
* DHCP server. The routine is called if DHCP is not included in a VxWorks 
* image and an automatic configuration protocol was used by the bootstrap
* loader. Under those circumstances, indications of a DHCP lease result are
* treated as an error by the startup code, since the lease cannot be renewed.
*
* RETURNS: OK if DHCP lease data found in bootline, or ERROR otherwise.
*
* NOMANUAL
*/

LOCAL STATUS dhcpcBootLineClean
    (
    char * 	pAddrString    /* client address string from bootline */
    )
    {
    FAST char *pDelim;
    FAST char *offset;
    STATUS result = ERROR;

    /* Check if lease duration field is present. */

    offset = (char *)index (pAddrString, ':');  /* Find netmask field. */
    if (offset != NULL)
        {
        pDelim = offset + 1;
        offset = (char *)index (pDelim, ':');
        if (offset != NULL)
            {
            /* 
             * Lease duration found - for active DHCP leases,
             * the lease origin field is also present.
             */

            pDelim = offset + 1;
            pDelim = (char *)index (pDelim, ':');
            if (pDelim != NULL)
                 result = OK;     /* Active DHCP lease found. */

            *offset = EOS;    /* Remove DHCP lease information. */
            }
        }
    return (result);
    }

/*******************************************************************************
*
* usrNetmaskGet - remove DHCP timestamp information from address
*
* This routine stores any value for the netmask found in the client 
* address string in the bootline, if needed. It is the initialization
* routine for the INCLUDE_NETMASK_GET component which is automatically
* added when the INCLUDE_NET_INIT component is used. The netmask
* value is not stored if further processing was disabled by an earlier 
* routine.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetmaskGet (void)
    {

    /* 
     * Extract the netmask if processing is not complete. The flag will be
     * set to disable further processing if an error is detected in the
     * address string. It will also be set if the PPP or SLIP device have 
     * been created, since those devices handle any netmask value 
     * independently.
     */

    if (!netDevBootFlag)
        {
        netmask = 0;
        bootNetmaskExtract (pAddrString, &netmask);
        }
    }
