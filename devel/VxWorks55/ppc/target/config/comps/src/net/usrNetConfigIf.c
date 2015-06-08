/* usrNetConfigIf.c - Configure the network interface */

/* Copyright 1992 - 2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,19apr01,jmp  fixed SIMNT route setting (SPR# 64900).
01c,06dec00,spm  renamed file to fix project tool build (alphabetical order
                 requires it to follow usrNetBoot.c for linker dependencies)
01b,16nov00,spm  enabled new DHCP lease for runtime device setup (SPR #20438)
01a,18aug98,ann  created from usrNetwork.c
*/

/* 
DESCRIPTION
This file is included by the configuration tool to setup a network device
according to the parameters in the boot line. It contains the initialization
routine for the INCLUDE_NETDEV_CONFIG component.
*/ 


/******************************************************************************
*
* usrNetIfConfig - configure network interface
*
* This routine configures the specified network interface with the specified
* boot parameters:
*
*	- subnetmask is extracted from inetAdrs and, if present,
*	  set for interface
*	- inet address is set for interface
*	- if present, inet name for interface is added to host table.
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/

STATUS usrNetIfConfig 
    (
    char *      devName, 		/* device name */
    int         unitNum, 		/* unit number */
    char *      inetAdrs, 		/* inet address */
    char *      inetName, 		/* host name */
    int         netmask 		/* subnet mask */
    )
    {
    char ifname [20];


#ifdef  INCLUDE_PCMCIA
    if (strncmp (devName, "pcmcia", 6) == 0)
	devName = "pcmcia";
#endif  /* INCLUDE_PCMCIA */

    /* check for empty inet address */

    if (inetAdrs[0] == EOS)
	{
	printf ("No inet address specified for interface %s.\n", devName);
	return (ERROR);
	}

    /* build interface name */

    sprintf (ifname, "%s%d", devName, unitNum);

    /* set subnet mask, if any specified */

    if (netmask != 0)
	ifMaskSet (ifname, netmask);

    /* set inet address */

    if (ifAddrSet (ifname, inetAdrs) != OK)
	{
	printf ("Error setting inet address of %s to %s, errno = %#x\n",
		ifname, inetAdrs, errno);
	return (ERROR);
	}

    /* add host name to host table */

    if ((inetName != NULL) && (*inetName != EOS))
	hostAdd (inetName, inetAdrs);

#if CPU==SIMNT
    routeAdd("0.0.0.0", sysBootParams.ead);
#endif  /* CPU==SIMNT */

    return (OK);
    }

/*******************************************************************************
*
* usrNetConfig - configure the network boot device
*
* This routine is the initialization routine for the INCLUDE_NETDEV_CONFIG
* component. It assigns the IP address and netmask from the boot parameters
* to the network boot device. Those values might be added manually or
* obtained as a result of automatic configuration with DHCP. The netAttachFlag
* variable is FALSE for devices such as PPP or SLIP which are attached and
* configured already, and also do not support automatic configuration.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetConfig
    (
    char * 	pDevName, 	/* device name */
    int 	unitNum, 	/* unit number */
    char * 	pDevHostName, 	/* device host name */
    char * 	pAddrString 	/* device address */
    )
    {
    /*
     * Do nothing if another device is already configured or an
     * error was detected in the boot parameters.
     */

    if (netDevBootFlag)
        return;

    /* Configure the network device using the address/mask information. */

    if (netAttachFlag)     /* Device attached but not configured yet? */
        {
        if (usrNetIfConfig (pDevName, unitNum, pAddrString, pDevHostName,
                            netmask) != OK)
            netDevBootFlag = TRUE;
        }

    return;
    }
