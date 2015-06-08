/* usrNetPPPBoot.c - start a PPP boot device */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26feb99,ann  added the initialization of the libraries
                 SPR# 24624
01a,18aug98,ann  created from usrNetwork.c
*/

/*
DESCRIPTION
This file is included by the configuration tool to initialize a PPP
boot device according to the parameters in the boot line. It contains the 
initialization routine for the INCLUDE_PPP_BOOT component.

NOMANUAL
*/

/*******************************************************************************
*
* usrNetPPPStart - start the PPP device
*
* This routine creates a PPP attachment between the host and target specified
* in the boot parameters if it is selected as the boot device. It is the 
* initialization routine for the INCLUDE_PPP_BOOT component which is 
* automatically added when both the INCLUDE_PPP and INCLUDE_NET_INIT 
* components are used.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetPPPStart (void)
    {
    char * 	pPeerAddrString;

    /*
     * Do nothing if an error was detected in the boot parameters.
     */

    if (netDevBootFlag)
        return;

    /* Initialize the required libraries */

    loginInit ();           /* for possible login option */

    pppSecretLibInit ();    /* initialize the secrets library */

    pppHookLibInit ();      /* initialize the ppp hooks library */

    pppShowInit ();

    /* Check the bootline and start the protocol */

    if (strncmp (sysBootParams.bootDev, "ppp", 3) == 0)
        {
        /* booting via ppp */

        if (sysBootParams.gad[0] == EOS)
            pPeerAddrString = sysBootParams.had;
        else
            pPeerAddrString = sysBootParams.gad;

        if (usrPPPInit (sysBootParams.bootDev, sysBootParams.unitNum, 
                        sysBootParams.ead, pPeerAddrString) == ERROR)
            printf ("Unable to initialize PPP connection.\n");

        netDevBootFlag = TRUE; 	/* Prevents further processing. */
        }

    return;
    }
