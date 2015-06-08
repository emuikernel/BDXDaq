/* usrNetSmnetBoot.c - start a shared memory network boot device */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
04r,05dec97,spm  added DHCP code review modifications
*/

/*
DESCRIPTION
This file is included by the configuration tool to initialize a shared memory
network device according to the parameters in the boot line. It contains the 
initialization routine for the INCLUDE_SMNET_BOOT component.

NOMANUAL
*/

/*******************************************************************************
*
* usrNetSmnetStart - start the shared memory or backplane device
*
* This routine creates a shared memory device as specified in the boot 
* parameters if it is selected as the boot device. It is the 
* initialization routine for the INCLUDE_SMNET_BOOT component which is 
* automatically added when both the INCLUDE_ULIP and INCLUDE_NET_INIT 
* components are used. The device is attached to the protocol stack
* by a later routine unless an error occurs.
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
        result = usrBpInit (sysBootParams.bootDev, sysBootParams.unitNum, 0);
        if (result == ERROR)
            netDevBootFlag = TRUE; 	/* Prevents further processing. */
        }

    return;
    }


/*******************************************************************************
*
* usrSmNetAddrGet - use backplane address field if necessary
*
* This routine resets the network-related boot parameter processing to
* use the field containing the address for the backplane or shared memory
* device. It is the initialization routine for the INCLUDE_SMNET_ADDRGET
* component which is automatically added when both the INCLUDE_SM_NET and
* INCLUDE_NET_INIT components are used.
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
        }

    return;
    }
