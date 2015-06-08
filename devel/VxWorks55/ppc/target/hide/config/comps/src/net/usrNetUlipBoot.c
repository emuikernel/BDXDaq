/* usrNetUlipBoot.c - start a user level IP boot device */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,18aug98,ann  created from usrNetwork.c
*/

/*
DESCRIPTION
This file is included by the configuration tool to initialize a ULIP
boot device according to the parameters in the boot line. It contains the 
initialization routine for the INCLUDE_ULIP_BOOT component.

NOMANUAL
*/

/******************************************************************************
*
* ulipRouteSetup - set up ULIP routing
*/

void ulipRouteSetup (void)
    {
    extern char *vxsim_ip_addr;
    extern char *vxsim_ip_name;
    int vxsim_num_ulips = 16;
    int ix;
    char host [50];
    char hostip [50];

    /* declare other simulated vxWorks' */

    for (ix = 0; ix < vxsim_num_ulips; ix++)
        {
        sprintf (host, vxsim_ip_name, ix);
        sprintf (hostip, vxsim_ip_addr, ix);
        hostAdd (host, hostip);
        }

    /* Add default route thru host. Loopback for local addresses */
    routeAdd ("0.0.0.0", sysBootParams.had);
    routeAdd (sysBootParams.ead, "localhost");
    }

/*******************************************************************************
*
* usrNetUlipStart - start the ULIP device
*
* This routine creates a ULIP device as specified in the boot parameters
* if it is selected as the boot device. It is the initialization routine 
* for the INCLUDE_ULIP_BOOT component which is automatically added when 
* both the INCLUDE_ULIP and INCLUDE_NET_INIT components are used. The
* device is used by VxWorks simulators.
* 
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetUlipStart (void)
    {
    /* 
     * Do nothing if another device is already initialized or an
     * error was detected in the boot parameters.
     */

    if (netDevBootFlag)
        return;

    if (strncmp (sysBootParams.bootDev, "ul", 2) == 0)
        {
        printf ("Attaching network interface %s%d... ",
                sysBootParams.bootDev, sysBootParams.unitNum);

        /* XXX last octet of 'ead' == procNum */

        if (ulipInit (sysBootParams.unitNum, sysBootParams.ead, 
                      sysBootParams.had, sysBootParams.procNum) == ERROR)
            {
            if (errno == S_if_ul_NO_UNIX_DEVICE)
                printf ("\nulipInit failed, errno = S_if_ul_NO_UNIX_DEV\n");
            else
                printf ("\nulipInit failed, errno = 0x%x\n", errno);

            }
        else
            printf ("done.\n");

        netDevBootFlag = TRUE; 	/* Prevents further processing. */
        }

    ulipRouteSetup ();

    return;
    }

