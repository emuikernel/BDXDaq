/* usrNetSmBoot.c - start a shared memory network boot device */

/* Copyright 1992 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,26apr02,vvv  fixed incorrect dev name comparison in usrBpInit (SPR #74788)
01f,02oct01,mas  added support for all 8 NETIF arguments, #7 = SM_MASTER,
		 #8 = SM_MAX_WAIT (SPR 4547)
01e,16nov00,spm  enabled new DHCP lease for runtime device setup (SPR #20438)
01d,19mar99,dat  SPR 25877, fixed old driver references
01c,24feb99,ann  added code to extract netmask (SPR# 24197)
01b,20jan99,scb  modified to use "sm=" before SM_ANCHOR_ADRS (23035)
01a,05dec97,spm  added DHCP code review modifications
*/

/*
DESCRIPTION
This file is included by the configuration tool to initialize a shared memory
network device according to the parameters in the boot line. It contains the 
initialization routine for the INCLUDE_SM_NET component.

NOMANUAL
*/

extern NETIF usrNetIfTbl [];


/*******************************************************************************
*
* usrNetSmnetStart - start the shared memory or backplane device
*
* This routine creates a shared memory device as specified in the boot 
* parameters if it is selected as the boot device. It is the initialization
* routine for the INCLUDE_SM_NET component. The device is attached to the
* protocol stack by a later routine unless an error occurs.
* 
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetSmnetStart (void)
    {
    STATUS result;

    /* 
     * Do nothing if another device is already initialized or an
     * error was detected in the boot parameters.
     */

    if (netDevBootFlag)
        return;

    if ((strncmp (sysBootParams.bootDev, "bp", 2) == 0) ||
        (strncmp (sysBootParams.bootDev, "sm", 2) == 0))
        {
        if (usrBpInit (sysBootParams.bootDev, sysBootParams.unitNum, 0) == ERROR)
            result = ERROR;

        if (result == ERROR)
            {
            printf ("Error occurred while attaching bp\n");
            netDevBootFlag = TRUE;
            }

        backplaneBoot = TRUE;
        }

    return;
    }


/*******************************************************************************
*
* usrSmNetAddrGet - use backplane address field if necessary
*
* This routine resets the network-related boot parameter processing to
* use the field containing the address for the backplane or shared memory
* device. It is the initialization routine for the INCLUDE_SM_NET_ADDRGET
* component which is automatically added when the INCLUDE_SM_NET component
* is used.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrSmNetAddrGet (void)
    {
    /* change the address string if a related boot device is used */

    if ((strncmp (sysBootParams.bootDev, "bp", 2) == 0) ||
            (strncmp (sysBootParams.bootDev, "sm", 2) == 0))
        {
        /* booting via backplane */

        pAddrString = sysBootParams.bad;
	bootNetmaskExtract (pAddrString, &netmask);
        }

    return;
    }




/******************************************************************************
*
* usrBpInit - initailize backplane driver
*
* usrBpInit initializes the backplane driver shared memory region
* and sets up the backplane parameters to attach.
*
* RETURNS: OK if successful otherwise ERROR
*
* NOMANUAL
*/

STATUS usrBpInit
    (
    char *  devName,        /* device name */
    int     unitNum,        /* unit number */
    u_long  startAddr       /* inet address */
    )
    {
#ifdef  INCLUDE_SM_NET
    char *  bpAnchor;       /* anchor address */
    NETIF * pNif;           /* netif struct */
    STATUS  status;         /* status */
    int     procNum;        /* proc num */
    char    buf [BOOT_DEV_LEN];  /* network device + unit number */

    /*
     * Pick off optional "=<anchorAdrs>" from backplane
     * device.  Also truncates devName to just "bp" or "sm"
     */

    if ((strncmp (devName, "bp=", 3) == 0) ||
        (strncmp (devName, "sm=", 3) == 0))
        {
        if (bootBpAnchorExtract (devName, &bpAnchor) < 0)
            {
            printf ("Invalid anchor address specified: \"%s\"\n", devName);
            return (ERROR);
            }
        }
    else
        {
	bpAnchor = SM_ANCHOR_ADRS;          /* default anchor */
        }

     procNum = sysProcNumGet ();

    /* if we are master, initialize backplane net */

    if (procNum == 0)
        {
        printf ("Initializing backplane net with anchor at %#x... ",
                (int) bpAnchor);

        status = smNetInit ((SM_ANCHOR *) bpAnchor, (char *) SM_MEM_ADRS,
                            (int) SM_MEM_SIZE, SM_TAS_TYPE, SM_CPUS_MAX,
                            SM_PKTS_SIZE, startAddr);

        if (status == ERROR)
            {
            printf ("Error: backplane device %s not initialized\n", devName);
            return (ERROR);
            }

        printf ("done.\n");
        }

    /* Locate NETIF structure for backplane */

    sprintf (buf, "%s%d", devName, unitNum);

    for (pNif = usrNetIfTbl; pNif->ifName != 0; pNif++)
        {
        if (strcmp (buf, pNif->ifName) == 0)
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
        if (strncmp (devName, "bp", 2) == 0)
            printf ("INCLUDE_BP_5_0 not supported - use 'sm' interface\n");
        return (ERROR);
        }

    printf ("Backplane anchor at %#x... ", (int) bpAnchor);

    /* configure all backplane parameters in NETIF struct */

    pNif->arg1 = bpAnchor;      /* anchor address */
    pNif->arg2 = SM_MAX_PKTS;
    pNif->arg3 = SM_INT_TYPE;
    pNif->arg4 = SM_INT_ARG1;
    pNif->arg5 = SM_INT_ARG2;
    pNif->arg6 = SM_INT_ARG3;
    pNif->arg7 = SM_MASTER;
    pNif->arg8 = SM_MAX_WAIT;

    return (OK);

#else   /* INCLUDE_SM_NET */
    printf ("\nError: backplane driver referenced but not included.\n");
    return (ERROR);
#endif  /* INCLUDE_SM_NET */
    }

