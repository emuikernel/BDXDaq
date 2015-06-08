/* usrNetBoot.c - network initialization, phase 2: validate boot parameters */

/* Copyright 1992 - 2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,01apr02,wap  properly dereference pointer when evaluating unitNum
01f,22mar02,wap  fix missing curly brace
01e,13mar02,wap  properly determine unitNum when booted from non-network
                 device
01d,10oct01,sbs  added check for tffs as a boot device (SPR 28561)
01c,16nov00,spm  enabled new DHCP lease for runtime device setup (SPR #20438)
01b,08mar99,ann  added locals and added NULL assignment to pDevName if 
                 "other" field is null (SPR# 25328)
01a,05dec97,spm  added DHCP code review modifications
*/

/*
DESCRIPTION
This file is included by the configuration tool to start a network device
according to the parameters in the boot line. It contains the initialization 
routines for the usrNetworkBoot initialization group and associated
components.

NOMANUAL
*/

/* locals */

LOCAL BOOL netDevBootFlag = FALSE;  /* 
                                     * Set TRUE once a device is started
                                     * or if processing can't continue.
                                     */

LOCAL BOOL netAttachFlag = FALSE;   /* BSD or END device available? */
LOCAL char * 	pAddrString;        /* Selects address field for device. */
LOCAL char * 	pDevName;           /* Selects name for device. */
LOCAL char *    pTgtName;           /* Selects name for the target */
LOCAL int       uNum;               /* Selects the unit number */
LOCAL int       netmask;            /* Device netmask. */
LOCAL BOOL      backplaneBoot = FALSE;

/*******************************************************************************
*
* usrNetBoot - device independent network boot handling
*
* This routine prepares for the network-related boot parameter processing.
* It is the initialization routine for the INCLUDE_NET_INIT component. 
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetBoot (void)
    {
    /* Set to access the bootline params by default. */

    pAddrString = sysBootParams.ead;
    pTgtName    = sysBootParams.targetName;
    uNum        = sysBootParams.unitNum;

    return;

    }

/******************************************************************************
*
* usrNetDevNameGet - use alternate device name field if necessary
*
* This routine sets the network-related boot parameter processing to read
* the network device name from the "other" field instead of the "boot device"
* field when booting with a block device. It is the initialization routine
* for the INCLUDE_NETDEV_NAMEGET component which is always included when the 
* INCLUDE_NET_INIT component is selected.
*
* When using the "other" field to obtain the device name, make sure
* one of the "attach" components has been included.
* i.e INCLUDE_BSD_BOOT or INCLUDE_END_BOOT
* If one of these is not included, the network interface will not be
* attached.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetDevNameGet (void)
    {
    /*
     * Do nothing if another device is already initialized or an 
     * error was detected in the boot parameters.
     */

    if (netDevBootFlag)
        return;

    pDevName = sysBootParams.bootDev;

    /* If booting from disk, configure additional network device, if any. */

    if ( (strncmp (sysBootParams.bootDev, "scsi", 4) == 0) ||
        (strncmp (sysBootParams.bootDev, "ide", 3) == 0) ||
        (strncmp (sysBootParams.bootDev, "ata", 3) == 0) ||
        (strncmp (sysBootParams.bootDev, "fd", 2) == 0) ||
        (strncmp (sysBootParams.bootDev, "tffs", 4) == 0))
        {
        if (sysBootParams.other [0] != EOS)
            {
            char *	pStr;
            int		unit;

            /*
             * Fix up unitNum correctly. If the boot device
             * was specified in the other field, we need to derive
             * the unit number from here, rather than the one
             * derived from bootDev, since bootDev was a disk
             * device of some kind.
             */
 
            pStr = (char *)&sysBootParams.other;
            while (!isdigit (*pStr) && *pStr != EOS)
                pStr++;
 
            if (*pStr != EOS && sscanf (pStr, "%d", &unit) == 1)
                {
                sysBootParams.unitNum = unit;
                *pStr = EOS;
                }

            pDevName = sysBootParams.other;
            }
        else
            pDevName = NULL;
        }
    return;
    }
