/* usrNetSmSecBoot.c - Start the SM as a second interface */

/* Copyright 1992 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,24apr02,mas  non-boot 'sm' unit number hard-coded to zero (SPR 74787); now
		 smNetShowInit() is properly dependent upon INCLUDE_SM_NET_SHOW
01e,08oct01,mas  code cleanup, added IP address display (SPR 4547)
01d,07feb01,spm  added merge record for 30jan01 update from version 01d of
                 tor2_0_x branch (base 01b) and fixed modification history
01c,30jan01,ijm  merged SPR #28602 fixes: proxy ARP services are obsolete
01b,23mar99,dat  SPR 25877, removed old bp driver config
01a,18aug98,ann  created from usrNetwork.c
*/

/*
DESCRIPTION
This file is included by the configuration tool to initialize a shared memory
network device according to the parameters in the boot line. It is called by
the INCLUDE_SECOND_SMNET component and attaches the backplane interface to
allow shared memory communication regardless of the selected boot device.

NOMANUAL
*/

/******************************************************************************
*
* usrBpSecondAttach - Attach backplane interface as second interface
*
* This routine attaches the backplane interface if it was not used as the
* boot device.  In such cases the unit number is always zero.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrBpSecondAttach (void)
    {
    char *         bpDev;
    BOOL           proxyOn      = FALSE;     /* Initialize Defaults */
    u_long         startAddr    = 0;         /* Default sequential Addr off */
    int            proxymask    = 0;
    char           netDev [BOOT_DEV_LEN + 1];
    char           bpAddr [BOOT_DEV_LEN];    /* backplane IP address */
    struct in_addr localAddr;
    BOOL           seqAddrOn    = FALSE;
    BOOL           configureBp  = TRUE;
    BOOL           useEtherAddr = FALSE;

    /* attach backplane interface (as second interface) */

#define BP_ADDR_GIVEN           (sysBootParams.bad [0] != EOS)
#define BP_ADDR_NOT_GIVEN       (sysBootParams.bad [0] == EOS)

    if (!backplaneBoot)
        {
        /* Turn switches ON as needed */

#ifdef INCLUDE_SM_SEQ_ADDR
        seqAddrOn = TRUE;
#endif
#ifdef INCLUDE_PROXY_DEFAULT_ADDR
        useEtherAddr = TRUE;
#endif
#ifdef INCLUDE_PROXY_SERVER
        proxyOn = TRUE;
#endif

        bpDev = "sm";
        sprintf (netDev, "%s", "sm0");
        bootNetmaskExtract (sysBootParams.bad, &proxymask);

        /* Using Subnet with PROXY Arp */

        if (proxyOn == TRUE)
            {
            if (seqAddrOn == TRUE)   /* PROXY WITH SEQ ADDR */
                {
                /* Pick address from backplane or ethernet */

                if (BP_ADDR_GIVEN)
                    {
                     startAddr = ntohl (inet_addr (sysBootParams.bad));
                     netmask = proxymask;
                    }
                else if (sysProcNumGet () == 0)
                    {
                    /*
                     * The shared memory master calculates the next available
                     * address, if possible. Slaves will read it directly.
                     */

                    if ( useEtherAddr )
                        {
                        startAddr = ntohl (inet_addr (sysBootParams.ead)) + 1;
                        netmask = 0xffffffff;
                        }
                    else                /* Configuration error */
                       {
                       printf("Error: No address for proxy service.\n");
                       printf("Backplane IP Address must be specified.\n");
                       configureBp = FALSE;
                       }
                    }
                }
            else                     /* PROXY WITHOUT SEQ ADDR */
                {
                if (BP_ADDR_NOT_GIVEN)
                    {
                    if (sysProcNumGet () == 0)
                        configureBp = FALSE;    /* Can't start master. */
                    }
                else                 /* startAddr is left as zero */
                    {
                    netmask = proxymask;
                    }
                }
            }

        /* Using Subnet without PROXY Arp */

        else
            {
            if (BP_ADDR_GIVEN)
                {
                if (seqAddrOn == TRUE)
                    {
                    /* Assign sequential address to backplane */

                    startAddr = ntohl (inet_addr (sysBootParams.bad));
                    }
                netmask = proxymask;
                }
            else if (sysProcNumGet () == SM_MASTER)
                {
                /* Don't start the master if the backplane address is empty. */

                configureBp = FALSE;
                printf ("Backplane IP address required if no Proxy ARP\n");
                }
            }

        if (configureBp == TRUE)
            {
            if (usrBpInit (bpDev, 0, startAddr) == ERROR)
                return;

            if (startAddr != 0)
                {
                localAddr.s_addr = htonl (startAddr);
                inet_ntoa_b (localAddr, bpAddr);
                }
            else
                {
                strcpy (bpAddr, sysBootParams.bad);
                }
            printf ("Backplane IP address = %s\n", bpAddr);

            (void) usrNetIfAttach (bpDev, 0, sysBootParams.bad);

            /* Assigned Back Plane Address if needed */

            if ((BP_ADDR_NOT_GIVEN) &&
                 (smNetInetGet (netDev, sysBootParams.bad, NONE) == OK))
                 printf ("Backplane address: %s\n", sysBootParams.bad);

            (void) usrNetIfConfig (bpDev, 0, sysBootParams.bad,
                                   (char *) NULL, netmask);
            }
        }

#ifdef INCLUDE_SM_NET_SHOW
# ifndef INCLUDE_BP_5_0
    smNetShowInit ();
# endif /* INCLUDE_BP_5_0 */
#endif /* INCLUDE_SM_NET_SHOW */
    }

