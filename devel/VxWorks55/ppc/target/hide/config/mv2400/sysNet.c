/* sysNet.c - template system network initialization */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,21jun99,tm   Adapted (abridged) for mv2400
01a,01apr99,jkf  written.
*/

/*

DESCRIPTION

This library contains board-specific routines for network subsystems.

*/


/* includes */

#include "vxWorks.h"
#include "taskLib.h"
#include "config.h"

#if defined (INCLUDE_NETWORK)

/* forward declarations */

#if defined(INCLUDE_NETWORK) && defined(INCLUDE_END)
IMPORT STATUS sysDec21x40PciInit (void);
#endif /* defined(INCLUDE_NETWORK) && defined(INCLUDE_END) */

/******************************************************************************
*
* sysNetHwInit - initialize the network interface
*
* This routine initializes the network hardware to a quiescent state.  It
* does not connect interrupts.
*
* Only polled mode operation is possible after calling this routine.
* Interrupt mode operation is possible after the memory system has been
* initialized and sysNetHwInit2() has been called.
*/

void sysNetHwInit (void)
    {
    }


/******************************************************************************
*
* sysNetHwInit2 - initialize additional features of the network interface
*
* This routine completes initialization needed for interrupt mode operation
* of the network device drivers.  Interrupt handlers can be connected.
* Interrupt or DMA operations can begin.
*/

void sysNetHwInit2 (void)
    {
    sysDec21x40PciInit();		/* DEC 21x40 device */
    }

#endif /* INCLUDE_NETWORK */


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
    /* 
     *  enable the ISA IRQ for LAN
     */
#if defined (INCLUDE_NETWORK)
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
* This routine disables interrupts for the on-board LAN chip. 
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
    return (intDisable (intLevel));
#else
    return (ERROR);
#endif
    }
