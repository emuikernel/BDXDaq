/* usrNetBsdBoot.c - start a BSD-compatible boot device */

/* Copyright 1992 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,02jan02,vvv  added a message to be printed when device attaches successfully
01b,16nov00,spm  enabled new DHCP lease for runtime device setup (SPR #20438)
01a,18aug98,ann  created from usrNetwork.c
*/

/*
DESCRIPTION
This file is included by the configuration tool to start a network device
according to the parameters in the boot line. It contains the initialization 
routine for the INCLUDE_BSD_BOOT component.

NOMANUAL
*/

extern NETIF usrNetIfTbl [];

/*******************************************************************************
*
* usrNetBsdDevStart - start the network device
*
* This routine creates a network device using the BSD interface according
* to the boot parameters. Unlike other device creation routines, the device 
* name is not tested since no preassigned name exists for these devices. It 
* is the initialization routine for the INCLUDE_BSD_BOOT component.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetBsdDevStart 
    (
    char * 	pDevName, 	/* device name */
    int 	unitNum, 	/* unit number */
    char * 	pAddrString, 	/* device address */
    BOOL 	errFlag 	/* Able to attach? */
    )
    {
    /*
     * Do nothing if another device is already setup or an
     * error was detected in the boot parameters.
     */

    if (errFlag)
        return;

    /* Do nothing if a device with the given name is already attached. */

    if (netAttachFlag)
        return;

    if (pDevName != NULL)
        {
        if (usrNetIfAttach (pDevName, unitNum, pAddrString) != OK)
            return;

        /*
         * Booting via network: protocol attachment is complete and
         * automatic configuration with DHCP or BOOTP is supported
         * for all these network devices.
         */

        netAttachFlag = TRUE;
        }

    printf ("Attached TCP/IP interface to %s unit %d\n", pDevName, unitNum);

    return;
    }

/******************************************************************************
*
* usrNetIfAttach - attach a  network interface
*
* This routine attaches the specified network interface.
*
*	- interface is attached
*	- interface name is constructed as "<devName>0"
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/

STATUS usrNetIfAttach 
    (
    char * 	devName,
    int 	unitNum,
    char * 	inetAdrs
    )
    {
    FAST NETIF *	pNif;
    STATUS 		status;
    char 		buf [BOOT_DEV_LEN + 1]; /* device name + unit number */

#ifdef  INCLUDE_PCMCIA
    int sock;

    if (strncmp (devName, "pcmcia", 6) == 0)
	{
	if (strlen (devName) == 6)
	    return (ERROR);
	else
	    sscanf (devName, "%*6s%*c%d", &sock);
	*(devName + 6) = '\0';
	}
#endif  /* INCLUDE_PCMCIA */

    /* attach interface */

    /* find interface in table */

    sprintf(buf, "%s%d", devName, unitNum);

    for (pNif = usrNetIfTbl; pNif->ifName != 0; pNif++)
	{
	if (strcmp (devName, pNif->ifName) == 0)
	    break;
	}

    /*
     * For backward compatibility, the device name only is acceptable for
     * unit numbers of 0.
     */
    if (pNif->ifName == 0 && unitNum == 0)
        {
        for (pNif = usrNetIfTbl; pNif->ifName != 0; pNif++)
            {
            if (strcmp (devName, pNif->ifName) == 0)
                break;
            }
        }

    if (pNif->ifName == 0)
	{
	printf ("Network interface %s unknown.\n", devName);
	return (ERROR);
	}

    printf ("Attaching network interface %s... ", buf);

#ifdef  INCLUDE_PCMCIA
    if (strncmp (devName, "pcmcia", 6) == 0)
	pNif->arg1 = (char *)sock;
#endif  /* INCLUDE_PCMCIA */

#if defined (TARGET_VIP10)
        /* SGI VIP10 boards are supposed to come with the ethernet address
         * in SGI formated non volatile ram.  We can not be sure where this
         * is so we default the upper 4 bytes of the address to SGI standard
         * and use the bottom two bytes of the internet address for the rest.
         */
        if (strcmp (devName, "lnsgi") == 0)
            {
            IMPORT char lnsgiEnetAddr [];      /* ethernet addr for lance */

            u_long inet = inet_addr (inetAdrs);
            lnsgiEnetAddr [4] = (inet >> 8) & 0xff;
            lnsgiEnetAddr [5] = inet & 0xff;
            }
#endif  /* TARGET_VIP10 */


    status = pNif->attachRtn (unitNum, pNif->arg1, pNif->arg2, pNif->arg3,
			      pNif->arg4, pNif->arg5, pNif->arg6,
			      pNif->arg7, pNif->arg8);
    if (status != OK)
	{
        if (errno == S_iosLib_CONTROLLER_NOT_PRESENT)
            printf ("failed.\nError: S_iosLib_CONTROLLER_NOT_PRESENT.\n");
        else
	    printf ("failed: errno = %#x.\n", errno);
	return (ERROR);
	}

    printf ("done.\n");

    return (OK);
    }
