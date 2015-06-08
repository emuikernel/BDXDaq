/* usrNetDhcprCfg.c - Initialization routine for the DHCP relay agent */

/* Copyright 1992 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,14may02,wap  Initialize structures to eliminate diab error (SPR #76331,
                 SPR #76377)
01c,26mar02,vvv  check for errors (SPR #73503)
01b,09oct01,rae  merge from truestack (added DHCPS_MAX_MSGSIZE to call)
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the initialization routine for the 
INCLUDE_DHCPR component.

NOMANUAL
*/

#include "dhcp/dhcp.h"
#include "dhcp/common.h"
#include "dhcprLib.h"

LOCAL int dhcprTaskPriority  = 56;    /* Priority level of DHCP relay agent */
LOCAL int dhcprTaskOptions   = 0;     /* Option settings for DHCP relay agent */
LOCAL int dhcprTaskStackSize = 2500;  /* Stack size for DHCP relay agent */
IMPORT STATUS dhcprLibInit (int);
IMPORT STATUS dhcprInit (struct ifnet **, int, DHCP_TARGET_DESC *, int);
IMPORT void dhcprStart (void);

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


void usrDhcprStart (void)
    {
    char        pDevName [MAX_FILENAME_LENGTH];  /* device name */
    STATUS      dhcprResult;

    if (dhcprLibInit (DHCPS_MAX_MSGSIZE) == ERROR)
	{
	printf ("Unable to initialize DHCP relay agent!\n");
	return;
	}

    /*
     * For now, set server or relay agent to listen on boot device.
     * Eventually, need to allow for multiple network devices.
     */

    pDevName [0] = EOS;
    sprintf(pDevName, "%s%d", sysBootParams.bootDev, sysBootParams.unitNum);
    devlist[0] = ifunit (pDevName);


    if (devlist[0] == NULL)
      printf("Network interface %s not found. DHCP relay agent not started.\n",
                pDevName);
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

    }
