/* usrNetSlipCfg.c - Main init routine for SLIP */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,18aug98,ann  created from usrNetwork.c
*/

/*
DESCRIPTION
This file contains the init routine for the INCLUDE_SLIP componen that is 
called from usrNetSlipBoot.c.

NOMANUAL
*/


/**************************************************************************
*****
*
* usrSlipInit - initialize the slip device
*
* RETURNS: OK if successful, otherwise ERROR.
*
* NOMANUAL
*/

STATUS usrSlipInit
    (
    char *      pBootDev,               /* boot device */
    int         unitNum,                /* unit number */
    char *      localAddr,              /* local address */
    char *      peerAddr                /* peer address */
    )
    {
    char        slTyDev [20];           /* slip device */
    int         netmask;                /* netmask */

    if (pBootDev [2] == '=')
        {
        /* get the tty device used for SLIP interface e.g. "sl=/tyCo/1" */
        strcpy (slTyDev, &pBootDev [3]);
        pBootDev [2] = '\0';
        }
    else
        {
        /* construct the default SLIP tty device */
        sprintf (slTyDev, "%s%d", "/tyCo/", SLIP_TTY);
        }

    printf ("Attaching network interface sl%d... ", unitNum);

    bootNetmaskExtract (localAddr, &netmask); /* remove and ignore mask */

    if (slipInit (unitNum, slTyDev, localAddr, peerAddr, SLIP_BAUDRATE,
        CSLIP_ENABLE, CSLIP_ALLOW, SLIP_MTU) == ERROR)
        {
        printf ("\nslipInit failed 0x%x\n", errno);
        return (ERROR);
        }

    printf ("done.\n");
    return (OK);

    }


