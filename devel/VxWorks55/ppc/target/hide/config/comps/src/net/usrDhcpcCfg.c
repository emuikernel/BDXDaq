/* usrDhcpcCfg.c - DHCP client initialization */

/* Copyright 1992 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,14may02,wap  Initialize structures to eliminate diab error (SPR #76331,
                 SPR #76377)
01a,31jul98,spm  written.
*/

/*
DESCRIPTION
This file is used to configure and initialize the DHCP client and control
the automatic configuration process when booting over the network.

NOMANUAL
*/

#ifdef INCLUDE_DHCPS
#include "dhcp/dhcp.h"
#include "dhcp/common.h"
#include "dhcpsLib.h"
IMPORT long dhcps_dflt_lease;
IMPORT long dhcps_max_lease;
IMPORT STATUS dhcpsLeaseHookAdd (FUNCPTR);
IMPORT STATUS dhcpsAddressHookAdd (FUNCPTR);

/* Include these declarations if hooks are defined. */

/* IMPORT STATUS DHCPS_LEASE_HOOK (int, char *, int); */

/* IMPORT STATUS DHCPS_ADDRESS_HOOK (int, char *, int); */

#endif

#ifdef INCLUDE_DHCPC
#include "dhcp/dhcpcInit.h"
#else
LOCAL STATUS dhcpcBootLineClean (char *);
#endif /* INCLUDE_DHCPC */

#ifdef INCLUDE_DHCPR
#ifndef INCLUDE_DHCPS        /* Server is superset of relay agent. */
#include "dhcprLib.h"
#endif
#endif


#if defined (INCLUDE_DHCPS) || defined (INCLUDE_DHCPR)
/*
 * The following table lists the IP addresses which will receive DHCP
 * messages forwarded across network boundaries by a DHCP server or
 * relay agent.
 */
DHCP_TARGET_DESC dhcpTargetTbl [] =
    {
    /*
     IP address of DHCP target servers
     ---------------------------------
     */
     /* {"90.11.42.2"}, */
     { NULL } /* List terminator, must be last */
     };
LOCAL int dhcpTargetTblSize = (NELEMENTS(dhcpTargetTbl));

struct ifnet *devlist[1];

int dhcpMaxHops = DHCP_MAX_HOPS;
int dhcpSPort = DHCPS_SPORT;
int dhcpCPort = DHCPS_CPORT;
#endif

/* 
 * Example cache routine - should store records of active leases 
 *                         across reboots. 
 */

/*
#ifdef INCLUDE_DHCPS

STATUS dhcpsDummyCache (int, char *, int);

STATUS dhcpsDummyCache (int op, char *name, int length)
    {
    printf("Cache called with op %d on %d bytes.\n", op, length);
    if (op == DHCPS_STORAGE_READ)
        return (ERROR);          /@ Simulated end-of-file. @/
    return (OK);
    }
#endif
*/

#ifdef INCLUDE_DHCPS

DHCPS_LEASE_DESC dhcpsLeaseTbl [] =
    {
    /*
    Name        Start IP      End IP          Parameters
    ----        ----------    ------          -----------
                                     see man pages
                                (timers, bootfile, etc.)
    */

    /*
     * Host requirements defaults needed for RFC compliance - DO NOT REMOVE!!
     */

   {"dflt",    NULL, NULL, DHCPS_DEFAULT_ENTRY},

    /* Sample database entries. */

/* {"ent1", "90.11.42.24", "90.11.42.24", "clid=\"1:0x08003D21FE90\":maxl=90:dfll=60"},   */
/* {"ent2", "90.11.42.25", "90.11.42.26", "snmk=255.255.255.0:maxl=90:dfll=70:file=/vxWorks"}, */
/* {"ent3", "90.11.42.27", "90.11.42.27", "maxl=0xffffffff:file=/vxWorks"}, */
/* {"entry4", "90.11.42.28", "90.11.42.29", "albp=true:file=/vxWorks"}      */
    };
LOCAL int dhcpsLeaseTblSize = (NELEMENTS(dhcpsLeaseTbl));

/*
 * If the DHCP server will receive messages from relay agents,
 * the following table must be filled in by the user.
 */

DHCPS_RELAY_DESC dhcpsRelayTbl [] =
    {
    /*
     IP address of agent              Subnet Number
     --------------------             -------------
     */
    /* {"90.11.42.254",               "90.11.42.0"}, */
    { NULL,                           NULL } /* List terminator, must be last */
    };
LOCAL int dhcpsRelayTblSize = (NELEMENTS(dhcpsRelayTbl));

LOCAL int dhcpsTaskPriority  = 56;      /* Priority level of DHCP server */
LOCAL int dhcpsTaskOptions   = 0;       /* Option settings for DHCP server */
LOCAL int dhcpsTaskStackSize = 5000;    /* Stack size for DHCP server task */
IMPORT void dhcpsStart (void);
#endif

#ifdef INCLUDE_DHCPR                   /* Relay agent settings. */
#ifndef INCLUDE_DHCPS                  /* Server is superset of relay agent. */

LOCAL int dhcprTaskPriority  = 56;    /* Priority level of DHCP relay agent */
LOCAL int dhcprTaskOptions   = 0;     /* Option settings for DHCP relay agent */
LOCAL int dhcprTaskStackSize = 2500;  /* Stack size for DHCP relay agent */
IMPORT void dhcprLibInit (void);
IMPORT STATUS dhcprInit (struct ifnet **, int, DHCP_TARGET_DESC *, int);
IMPORT void dhcprStart (void);
#endif
#endif

/*******************************************************************************
*
* usrNetInit - system-dependent network initialization
*
* This routine initializes the network.  The ethernet and backplane drivers
* and the TCP/IP software are configured.  It also adds hosts (analogous to
* the /etc/hosts file in UNIX), gateways (analogous to /etc/gateways), sets
* up our access rights on the host system (with iam()), optionally
* initializes telnet, rlogin, RPC, and NFS support.
*
* The boot string parameter is normally left behind by the boot ROMs,
* at address BOOT_LINE_ADRS.
*
* Unless the STANDALONE option is selected in usrConfig.c, this routine
* will automatically be called by the root task usrRoot().
*
* RETURNS:
* OK, or
* ERROR if there is a problem in the boot string, or initializing network.
*
* SEE ALSO:  "Network" chapter of the VxWorks Programmer's Guide
*
* NOMANUAL
*/

STATUS usrNetInit 
    (
    char *bootString		/* boot parameter string */
    )
    {
    char	nad [20];			/* host's network inet addr */
    char *	pBootString;
    int		netmask;
    char	devName [MAX_FILENAME_LENGTH];	/* device name */

#ifdef INCLUDE_DHCPC
    BOOL 	dhcpBoot = FALSE;       /* Lease obtained during startup? */
#endif  /* INCLUDE_DHCPC */

#ifdef INCLUDE_DHCPS
    STATUS dhcpsResult;
#endif

#ifdef INCLUDE_DHCPR
#ifndef INCLUDE_DHCPS              /* Server is superset of relay agent. */
    STATUS dhcprResult;
#endif
#endif

#ifdef INCLUDE_DHCPR
#ifndef INCLUDE_DHCPS
    dhcprLibInit ();
#endif
#endif

    if ((strncmp (sysBootParams.bootDev, "bp", 2) == 0) ||
            (strncmp (sysBootParams.bootDev, "sm", 2) == 0))
	{
	/* booting via backplane */

        pBootString = sysBootParams.bad;
        }
    else
        pBootString = sysBootParams.ead;

#ifdef INCLUDE_DHCPC
        /* 
         * Remove any values for DHCP lease origin and lease duration from
         * address string and store in dhcpcBootLease global. Set dhcpBoot
         * flag to indicate if DHCP lease values were found.
         */

        if (sysFlags & SYSFLG_AUTOCONFIG)
            if (dhcpcLeaseGet (pBootString, &dhcpBoot) == ERROR)
                {
                printf ("Unable to verify assigned DHCP address.\n");
                return (ERROR);
                }
#else
        /*
         * If network configuration protocols are used, check address 
         * for DHCP lease information. If found (and removed), exit
         * with an error, since the active DHCP lease cannot be maintained.
         */ 

        if (sysFlags & SYSFLG_AUTOCONFIG)
            if (dhcpcBootLineClean (pBootString) == OK)
                {
                printf ("Can't use dynamic address %s without DHCP.\n",
                            pBootString);
                return (ERROR);
                }
#endif    /* INCLUDE_DHCPC */

    /* attach and configure interfaces */

    /*   
     * Boot Device             Routines
     * -----------             --------
     * "ppp"                   usrPPPInit
     * "sl"                    usrSlipInit
     *   
     *      For the following devices, first remove the netmask value with
     *      bootNetmaskExtract and store it in the "netmask" variable.
     *      The target address field is selected based on the device type.
     *
     * Boot Device   Address Field           Routines
     * -----------   -------------           --------
     * "bp" or "sm"  sysBootParams.bad       usrBpInit, usrNetIfAttach, 
     *                                       usrNetIfConfig
     *           (Side effect: set backplaneBoot to TRUE)
     *
     * All others    sysBootParams.ead       None if "other" field is empty
     *                                       else usrNetIfAttach/usrNetIfConfig
     *                                       for the named device
     *

    /* attach backplane interface (as second interface) */

     /* 
      * Further backplane configuration occurred here if INCLUDE_SM_NET was
      * defined. It handled the case where an address was specified in the 
      * params.bad field and some other boot device besides bp or sm was used. 
      * The shared memory interface was created with usrNetIfAttach and 
      * usrNetIfConfig using the given address at this point.
      */

   /* 
    * This segment of DHCP client processing is only required when
    * INCLUDE_SM_NET is defined.
    */

#ifdef INCLUDE_DHCPC
    if (backplaneBoot)
        {
         if ((sysFlags & SYSFLG_AUTOCONFIG) && !(sysFlags & SYSFLG_PROXY))
             {
              printf("DHCP over backplane only with proxy arp.\n");
              return (ERROR);
             }
        }
#endif /* INCLUDE_DHCPC */


    if (sysBootParams.targetName[0] != EOS)
        sethostname (sysBootParams.targetName, strlen (sysBootParams.targetName));

#ifdef INCLUDE_DHCPC
    if (dhcpcLibInit (DHCPC_SPORT, DHCPC_CPORT, DHCPC_MAX_LEASES, 
                      DHCPC_OFFER_TIMEOUT, DHCPC_DEFAULT_LEASE, 
                      DHCPC_MIN_LEASE) == ERROR)
        return (ERROR);
#ifdef INCLUDE_NET_SHOW
    dhcpcShowInit ();
#endif

    /* 
     * Renew lease and store values in boot line. If successful, the
     * pDhcpcBootCookie global is set to allow user access to the lease.
     */

    if (dhcpcConfigSet (&sysBootParams, &netmask, dhcpBoot) == ERROR)
        return (ERROR);

    if (dhcpBoot == TRUE)
        bootStructToString (BOOT_LINE_ADRS, &sysBootParams);
#endif   /* INCLUDE_DHCPC */

    /* if a gateway was specified, extract the network part of the host's
     * address and add a route to this network
     */

    if (sysBootParams.gad[0] != EOS)
        {
	inet_netof_string (sysBootParams.had, nad);
	routeAdd (nad, sysBootParams.gad);
        }

    /* associate host name with the specified host address */

    hostAdd (sysBootParams.hostName, sysBootParams.had);

/* 
 * INCLUDE_PASSFS/INCLUDE_NET_REM_IO would create a remote access device 
 * at this point to get at the host file system. 
 */

#if defined (INCLUDE_DHCPS) || defined (INCLUDE_DHCPR)
    /*
     * For now, set server or relay agent to listen on boot device.
     * Eventually, need to allow for multiple network devices.
     */

    devName [0] = EOS;
    sprintf(devName, "%s%d", sysBootParams.bootDev, sysBootParams.unitNum);
    devlist[0] = ifunit (devName);

#ifdef INCLUDE_DHCPS
#ifdef INCLUDE_DHCPR
    printf ("DHCP Server present. Relay agent not permitted.\n");
#endif

    if (devlist[0] == NULL)
        printf("Network interface %s not found. DHCP server not started.\n",
                devName);
    else
        {
        dhcpsResult = dhcpsLeaseHookAdd (DHCPS_LEASE_HOOK);
        if (dhcpsResult == ERROR)
            {
            printf ("Warning: Required storage hook not installed.\n");
            printf ("Lease records will not be saved.\n");
            }
        dhcpsResult = dhcpsAddressHookAdd (DHCPS_ADDRESS_HOOK);
        if (dhcpsResult == ERROR)
            {
            printf ("Warning: No DHCP server address cache!");
            printf (" Later entries will not be saved.\n");
            }
        dhcps_dflt_lease = DHCPS_DEFAULT_LEASE;
        dhcps_max_lease = DHCPS_MAX_LEASE;

        dhcpsResult = dhcpsInit (devlist, 1,
                                 dhcpsLeaseTbl, dhcpsLeaseTblSize,
                                 dhcpsRelayTbl, dhcpsRelayTblSize,
                                 dhcpTargetTbl, dhcpTargetTblSize);
        if (dhcpsResult == ERROR)
            printf ("Error initializing DHCP server. Not started.\n");
        else
            {
            dhcpsResult = taskSpawn ("tDhcpsTask", dhcpsTaskPriority,
                                     dhcpsTaskOptions, dhcpsTaskStackSize,
                                     (FUNCPTR) dhcpsStart,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            if (dhcpsResult == ERROR)
                printf ("Unable to start DHCP server task.\n");
            printf ("DHCP server started.\n");
            }
        }
#else
    if (devlist[0] == NULL)
      printf("Network interface %s not found. DHCP relay agent not started.\n",
                devName);
    else
        {
        dhcprResult = dhcprInit (devlist, 1, dhcpTargetTbl, dhcpTargetTblSize);
        if (dhcprResult == ERROR)
            printf ("Error initializing DHCP relay agent. Not started.\n");
        else
            {
            dhcprResult = taskSpawn ("tDhcprTask", dhcprTaskPriority,
                                      dhcprTaskOptions, dhcprTaskStackSize,
                                      (FUNCPTR) dhcprStart,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            if (dhcprResult == ERROR)
                printf ("Unable to start DHCP relay agent task.\n");
            printf ("DHCP relay agent started.\n");
            }
        }
#endif
#endif    /* INCLUDE_DHCPS or INCLUDE_DHCPR */

    return (OK);
    }

#ifndef INCLUDE_DHCPC
/*******************************************************************************
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

    offset = index (pAddrString, ':');     /* Find netmask field. */
    if (offset != NULL)
        {
        pDelim = offset + 1;
        offset = index (pDelim, ':');
        if (offset != NULL)
            {
            /* 
             * Lease duration found - for active DHCP leases,
             * the lease origin field is also present.
             */

            pDelim = offset + 1;
            pDelim = index (pDelim, ':');
            if (pDelim != NULL)
                 result = OK;     /* Active DHCP lease found. */

            *offset = EOS;    /* Remove DHCP lease information. */
            }
        }
    return (result);
    }
#endif
