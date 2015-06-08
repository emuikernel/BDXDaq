/* usrNetEndBoot.c - start a MUX/END boot device */

/* Copyright 1992 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,02jan02,vvv  changed message that is printed when ipAttach fails
01f,09oct01,rae  merge from truestack (moved ipMaxUnits SPR #70476)
01e,04sep01,hbh  removed routeAdd call to usrNetIfConfig() for SIMNT (SPPR
                 29693).
01d,16nov00,spm  enabled new DHCP lease for runtime device setup (SPR #20438)
01c,18aug99,pul  deleted unwanted endFindByName() and muxIoctl
01b,08mar99,ann  added arguments to usrNetEndDevStart() (SPR# 25354)
                 added check for a NULL in pDevName (SPR# 25328)
01a,05dec97,spm  added DHCP code review modifications
*/

/*
DESCRIPTION
This file is included by the configuration tool to start a network device
according to the parameters in the boot line. It contains the initialization 
routines for the INCLUDE_ENDDEV_START component.

NOMANUAL
*/

extern int ipAttach ();

/*******************************************************************************
*
* usrNetEndDevStart - start the network device
*
* This routine creates a network device using the END interface according
* to the boot parameters. Unlike other device creation routines, the boot
* device field is not tested since no preassigned name exists for these
* devices. It is the initialization routine for the INCLUDE_ENDDEV_START 
* component which is included automatically when INCLUDE_END and 
* INCLUDE_NET_INIT are used.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetEndDevStart 
    (
    char *    pDevName,        /* Device name */
    int       unitNum         /* unit number */
    )
    {

    /*
     * Do nothing if another device is already setup or an
     * error was detected in the boot parameters.
     */

    if (pDevName == NULL)
        return;

#ifdef INCLUDE_END 

    if (netDevBootFlag)
        return;

    if (ipAttach(unitNum, pDevName) != OK)
        {
        printf ("Failed to attach to END device %s\n", pDevName);
        return;
        }

    /*
     * Booting via network: protocol attachment is complete and
     * automatic configuration with DHCP or BOOTP is supported
     * for all these network devices.
     */

    netAttachFlag = TRUE;

    printf ("Attached TCP/IP interface to %s unit %d\n", pDevName, unitNum);

#endif  /* INCLUDE_END */

    return;
    }
