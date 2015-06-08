/* usrNetSlipBoot.c - start a serial line IP boot device */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,18aug98,ann  created from usrNetwork.c
*/

/*
DESCRIPTION
This file is included by the configuration tool to initialize a SLIP
boot device according to the parameters in the boot line. It contains the 
initialization routine for the INCLUDE_SLIP_BOOT component.

NOMANUAL
*/

/*******************************************************************************
*
* usrNetSlipStart - start the SLIP device
*
* This routine creates a SLIP attachment between the host and target specified
* in the boot parameters if it is selected as the boot device. It is the 
* initialization routine for the INCLUDE_SLIP_BOOT component which is 
* automatically added when both the INCLUDE_SLIP and INCLUDE_NET_INIT 
* components are used.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetSlipStart (void)
    {
    char * 	pPeerAddrString;

    /*
     * Do nothing if an error was detected in the boot parameters.
     */

    if (netDevBootFlag)
        return;

    if (strncmp (sysBootParams.bootDev, "sl", 2) == 0)
        {
        /* booting via slip */

        if (sysBootParams.gad[0] == EOS)
            pPeerAddrString = sysBootParams.had;
        else
            pPeerAddrString = sysBootParams.gad;

        if (usrSlipInit (sysBootParams.bootDev, sysBootParams.unitNum, 
                        sysBootParams.ead, pPeerAddrString) == ERROR)
            printf ("Unable to initialize SLIP connection.\n");

        netDevBootFlag = TRUE; 	/* Prevents further processing. */
        }

    return;
    }
