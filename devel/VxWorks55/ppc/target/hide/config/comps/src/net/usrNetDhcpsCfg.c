/* usrNetDhcpsCfg.c - Initialization routine for the DHCP server */

/* Copyright 1992 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,14may02,wap  Initialize structures to eliminate diab error (SPR #76331,
                 SPR #76377)
01e,26oct01,vvv  allow DHCP server to start when target not booted from 
		 network device (SPR #64898)
01d,09oct01,rae  merge from truestack
01c,25jun01,ppp  fixed the problem involving the dhcp server and client to be
                 built together (SPR #64194)
01b,13dec00,spm  updated DHCP startup for latest version (from tor3_x branch)
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the initialization routine for the
INCLUDE_DHCPS component.

NOMANUAL
*/


#include <dhcpsLib.h>


IMPORT STATUS dhcpsLeaseHookAdd (FUNCPTR);
IMPORT STATUS dhcpsAddressHookAdd (FUNCPTR);

/* Include these declarations if hooks are defined. */

/* IMPORT STATUS DHCPS_LEASE_HOOK (int, char *, int); */

/* IMPORT STATUS DHCPS_ADDRESS_HOOK (int, char *, int); */

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

/* 
 * This table contains a list of interfaces that the server
 * will listen to. The boot device is used when the table's	
 * first entry is the null string.
 */

DHCPS_IF_DESC dhcpsIfTbl [] =
    {
    {""}			/* Use primary interface */
/* Sample interface entries. */
/*  {"fei1"}, */
/*  {"ln0"} */
    };

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

/* {"ent1", "90.11.42.24", "90.11.42.24", "clid=\"1:0x08003D21FE90\":maxl=90:dfll=6
0"},   */
/* {"ent2", "90.11.42.25", "90.11.42.26", "snmk=255.255.255.0:maxl=90:dfll=70:file=
/vxWorks"}, */
/* {"ent3", "90.11.42.27", "90.11.42.27", "maxl=0xffffffff:file=/vxWorks"}, */
/* {"entry4", "90.11.42.28", "90.11.42.29", "albp=true:file=/vxWorks"}      */
    };

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

LOCAL int dhcpsTaskPriority  = 56;      /* Priority level of DHCP server */
LOCAL int dhcpsTaskOptions   = 0;       /* Option settings for DHCP server */
LOCAL int dhcpsTaskStackSize = 5000;    /* Stack size for DHCP server task */

#ifndef INCLUDE_VIRTUAL_STACK
IMPORT void dhcpsStart (void);
#else 
IMPORT void dhcpsStart (int);
IMPORT int myStackNum;
#endif /* INCLUDE_VIRTUAL_STACK */

/* Default configurations parameters for dhcps, definitions are in h/dhcpsLib.h */

DHCPS_CFG_PARAMS dhcpsDfltCfgParams =
    {
    DHCP_MAX_HOPS,
    DHCPS_SPORT,
    DHCPS_CPORT,
    DHCPS_MAX_MSGSIZE,
    DHCPS_DEFAULT_LEASE,
    DHCPS_MAX_LEASE,
    DHCPS_LEASE_HOOK,
    DHCPS_ADDRESS_HOOK,
    dhcpsIfTbl,
    NELEMENTS(dhcpsIfTbl),
    dhcpsLeaseTbl,
    NELEMENTS(dhcpsLeaseTbl),
    dhcpTargetTbl,
    NELEMENTS(dhcpTargetTbl),
    dhcpsRelayTbl,
    NELEMENTS(dhcpsRelayTbl),
    };

void usrDhcpsStart 
    (
    DHCPS_CFG_PARAMS *pDhcpsCfg		/* Configuration parameters */
    )
    {
    STATUS      dhcpsResult; 

    if (pDhcpsCfg == (DHCPS_CFG_PARAMS *)NULL)
	{
	errnoSet (S_dhcpsLib_NOT_INITIALIZED);
	return;
	}

    if (pDhcpsCfg->pDhcpsIfTbl[0].ifName[0] == EOS)
        {
	sprintf (pDhcpsCfg->pDhcpsIfTbl[0].ifName, "%s%d",
	 pDevName, sysBootParams.unitNum);
	pDhcpsCfg->numDev = 1;
        }

    dhcpsResult = dhcpsInit (pDhcpsCfg);

    if (dhcpsResult == ERROR)
	{
	errnoSet (S_dhcpsLib_NOT_INITIALIZED);
	return;
	}

    dhcpsResult = taskSpawn ("tDhcpsTask", dhcpsTaskPriority,
                                 dhcpsTaskOptions, dhcpsTaskStackSize,
                                 (FUNCPTR) dhcpsStart,
#ifndef INCLUDE_VIRTUAL_STACK
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#else
		                 myStackNum, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#endif /* INCLUDE_VIRTUAL_STACK */

    if (dhcpsResult == ERROR)
	{
	errnoSet (S_dhcpsLib_NOT_INITIALIZED);
	}

    return;


    }
