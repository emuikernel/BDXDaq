/* sysNet.c - template system network initialization */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1999-2000 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01c,16aug00,dmw  Wind River coding standards review.
01b,07jun00,dmw  Update to PrPMC name convention.
01a,13mar00,dmw  written (from version 01a of mcpn765/sysNet.c).
*/

/*
DESCRIPTION

This library contains board-specific routines for network subsystems.
*/


/* includes */

#include "vxWorks.h"
#include "taskLib.h"
#include "config.h"

/******************************************************************************
*
* sysLanIntEnable - enable the LAN interrupt
*
* This routine enables interrupts at a specified level 
*
* RETURNS: OK, or ERROR if network support not included.
*
* SEE ALSO: sysLanIntDisable()
*/

STATUS sysLanIntEnable
    (
    int intLevel 		/* interrupt level to enable */
    )
    {

#if defined (INCLUDE_NETWORK)

    /* enable the IRQ for LAN */

    intEnable (intLevel);
    return (OK);
#else
    return (ERROR);
#endif
    }


/******************************************************************************
*
* sysLanIntDisable - disable the LAN interrupt
*
* This routine has been modified and is temporarily a stub. Changes to the WRS
* 21x40End driver are required to switch from a dedicated interrupt line for the
* Ethernet chip to the bussed PCI interrupt structure used by the PrPMC750. This
* change is also required to support Ethernet chips resident on PMC sites. WRS
* has assigned SPR #27879 to track the 21x40End driver change.
*
* RETURNS: return of intDisable, or ERROR if network support not included.
*
* SEE ALSO: sysLanIntEnable()
*/

STATUS sysLanIntDisable
    (
    int intLevel 		/* interrupt level to enable */
    )
    {
#if defined (INCLUDE_NETWORK)
    return (OK);
#else
    return (ERROR);
#endif
    }
