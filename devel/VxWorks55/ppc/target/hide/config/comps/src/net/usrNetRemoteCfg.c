/* usrNetRemoteCfg.c - network remote I/O support */

/* Copyright 1992 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,05apr02,hbh  fixed simulator build error
01c,26mar02,vvv  check for errors (SPR #73503)
01b,06sep01,hbh  Prevented ioDefPath to be erased for the simulators (SPR
                 30368).
01a,18aug98,ann  created from usrNetwork.c
*/

/*
DESCRIPTION
This file is used to configure and initialize the VxWorks networking support
for remote I/O access. The file contents are added to the project 
configuration file when FOLDER_NET_REM_IO is included. It contains the 
initialization routine for the INCLUDE_NET_HOST_SETUP and INCLUDE_NET_REM_IO
components.

NOMANUAL
*/

/*******************************************************************************
*
* usrNetHostSetup - final network initialization
*
* This routine performs the setup which follows any device initialization
* and/or verification of the boot line. It adds any necessary host and 
* gateway entries, analogous to /etc/hosts and /etc/gateways in UNIX systems. 
* It is called by the configuration tool as the component initialization 
* routine for INCLUDE_NET_HOST_SETUP. Any network remote I/O device can be 
* added after this routine is complete.
* 
* RETURNS:
* OK, or
* ERROR if there is a problem in the boot string, or initializing network.
*
* NOMANUAL
*/

STATUS usrNetHostSetup (void)
    {
    char	nad [20];			/* host's network inet addr */

    if (sysBootParams.targetName[0] != EOS)
        sethostname (sysBootParams.targetName, 
                     strlen (sysBootParams.targetName));

    /* 
     * If a gateway was specified, extract the network part of the host's
     * address and add a route to this network
     */

    if (sysBootParams.gad[0] != EOS)
        {
	inet_netof_string (sysBootParams.had, nad);
	if (routeAdd (nad, sysBootParams.gad) == ERROR)
	    printf ("Unable to add route to %s; errno = 0x%x.\n", nad, errno);
        }

    /* associate host name with the specified host address */

    if (hostAdd (sysBootParams.hostName, sysBootParams.had) == ERROR)
	printf ("Unable to add host %s to host table; errno = 0x%x.\n", 
		sysBootParams.hostName, errno);

    return (OK);
    }

/*******************************************************************************
*
* usrNetRemoteCreate - enable remote I/O access over the network
*
* This routine creates a device for network remote I/O access if
* INCLUDE_NET_REM_IO. It is called by the configuration tool as the 
* initialization routine for that component.
* 
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetRemoteCreate (void)
    {
    char        devName [MAX_FILENAME_LENGTH];  /* device name */
    int 	protocol;

    remLastResvPort = 1010;     /* pick an unused port number so we don't *
                                 * have to wait for the one used by the *
                                 * by the bootroms to time out */
    _func_ftpLs = ftpLs;               /* init ptr to ftp dir listing routine */
    _func_netLsByName = netLsByName;   /* init ptr to netDrv listing routine */
    _func_remCurIdGet = (FUNCPTR) remCurIdGet;
    _func_remCurIdSet = (FUNCPTR) remCurIdSet;

    if (netDrv () == ERROR)                        /* init remote file driver */
	printf ("Error initializing netDrv; errno = 0x%x\n", errno);
    else
	{
        /* 
         * SPR 30368 : ioDefPath variable has already been set to the current 
         * working directory when initializing PASSFS for the simulators.
         * Do not execute the lines below for simulators, because host device
	 * has already been created during PASSFS initialization and because
	 * ioDefPath will be erased.
         */

#if (!(defined(INCLUDE_PASSFS)) && !(defined(INCLUDE_NTPASSFS)))    
        sprintf (devName, "%s:", sysBootParams.hostName);   /* make dev name */
        protocol = (sysBootParams.passwd[0] == EOS) ? 0 : 1; /* pick protocol */

        /* Create device. */

        if (netDevCreate (devName, sysBootParams.hostName, protocol) == ERROR)
	    printf ("Error creating network device %s - errno = 0x%x\n", devName,
		    errno);
        else
            if (ioDefPathSet (devName) == ERROR) /* set the current directory */
		printf ("Error setting default path to %s - errno = 0x%x\n",
			devName, errno);
#endif
        }
	    
    iam (sysBootParams.usr, sysBootParams.passwd); /* Set the user id. */

    return;
    }
