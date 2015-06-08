/* sysBusPci.c - autoconfig support */

/* Copyright 1997-1999 Wind River Systems, Inc. All Rights Reserved */
/* Copyright 1999-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,27sep03,simon  updated for exclude 64360 when probe PCI bus.
01a,02apr03,simon  Ported. from ver 01b, mcpm905/sysBusPci.c.
*/
/*
DESCRIPTION

This module contains the "non-generic" or "board specific" PCI-PCI
bridge initialization code.  The module contains code to:

.CS
  1.  Determine if a particular function is to be excluded from the
          automatic configuration process.
  2.  Program the "interrupt line" field of the PCI configuration header.
.CE

*/

/* includes */

#include "vxWorks.h"
#include "logLib.h"
#include "taskLib.h"
#include "config.h"

#include "drv/pci/pciConfigLib.h"
#include "./pci/pciAutoConfigLib.c"

/* defines */

/* typedefs */

/* globals */

PCI_SYSTEM sysParams;

/* locals */

PHB_PCI_SPACE *pPciSpace;

/* forward declarations */

UINT sysPciMaxBus (int maxBusIF);
LOCAL UCHAR sysPciAutoConfigIntAsgn (PCI_SYSTEM * pSys, PCI_LOC * pFunc,
                                     UCHAR intPin);
LOCAL STATUS sysPciAutoConfigInclude (PCI_SYSTEM *pSys, PCI_LOC *pciLoc,
                                      UINT devVend);
void sysPciAutoConfig (int maxBusIF, PHB_PCI_SPACE * pPhbPciSpace);

/* externals */

/* subroutines */

/******************************************************************************
*
* sysPciMaxBus - calculate the highest numbered bus in the system
*
* This routine calculates the highest numbered PCI bus in the system.
*
* RETURNS: Highest bus number found during probing process.
*/

/* for Monarch mode only */
UINT sysPciMaxBus 
    (
    int maxBusIF	/* Number of PCI bus interfaces in the system */
    )
    {
    int busInterface;
    int bus = 0;
    int busses = 0;
    int subBus = 0;
    int maxBus = 0;

    for ( busInterface = 0; busInterface < maxBusIF; busInterface++)
        {
        switch(busInterface)
            {
            case 0:	/* First Bus Interface */
                    bus = (UINT8)0;
                    break;

            default:
                    bus = (UINT8)0;
                    break;
            }
        subBus = (pciMaxBusCalc(bus) - bus);  /* highest bus on bus interface */
        busses += (subBus + 1); /* running total number of busses */
        }
    maxBus = (busses - 1); /* highest numbered bus in system */
    return(maxBus);
    }

/******************************************************************************
*
* sysPciAutoConfigInclude - Determine if function is to be autoConfigured
*
* This function is called with PCI bus, device, function, and vendor 
* information.  It returns an indication of whether or not the particular
* function should be included in the automatic configuration process.
* This capability is useful if it is desired that a particular function
* NOT be automatically configured.  Of course, if the device is not
* included in automatic configuration, it will be unusable unless the
* user's code made provisions to configure the function outside of the
* the automatic process.
*
* RETURNS: TRUE if function is to be included in automatic configuration,
* FALSE otherwise.
*/
 
LOCAL STATUS
sysPciAutoConfigInclude (
    PCI_SYSTEM * pSys ARG_UNUSED,  /* input: AutoConfig system information */
    PCI_LOC *    pciLoc,	   /* input: PCI address of this function */
    UINT         devVend	   /* input: Device/vendor ID number      */
)
{
  BOOL retVal = OK;
  char txt[100];

  /* If it's the host bridge then exclude it */
  if((pciLoc->bus == 0) && (pciLoc->device == 0))
  {
    return(ERROR);
  }

  switch(devVend)
  {
        
#ifndef INCLUDE_DEC2155X
    case PCI_ID_BR_DEC21554:
      retVal = ERROR;
      PCI_AUTO_DEBUG_MSG(
                "sysPciAutoconfigInclude: Excluding DEC 21554 bridge\n",
		 0, 0, 0, 0, 0, 0);
      break;
#endif

	/* INCLUDED Devices */
    default:
      retVal = OK;
      PCI_AUTO_DEBUG_MSG(
                "sysPciAutoconfigInclude: Including unknown device\n",
		 0, 0, 0, 0, 0, 0);
      break;
  }

  return(retVal);
}

/******************************************************************************
*
* sysPciAutoConfigIntAssign - Assign the "interrupt line" value
*
* RETURNS: "interrupt line" value.
*
*/

LOCAL UCHAR
sysPciAutoConfigIntAsgn (
  PCI_SYSTEM *pSys ARG_UNUSED, /* input: AutoConfig system information */
  PCI_LOC *pFunc,		  /* input: function's location in the system */
  UCHAR intPin 	  /* input: interrupt pin number */
)
{
  UCHAR irqValue = 0xff;  /* Calculated value                */

  if(intPin == 0) return(irqValue);

  irqValue = pPciSpace->intLine [(pFunc->device)][(intPin - 1)];

  PCI_AUTO_DEBUG_MSG("intAssign called for device [%d %d %d] IRQ: %d\n",
			pFunc->bus, pFunc->device, pFunc->function,
			irqValue, 0, 0 );

  return(irqValue);
}
  
/*******************************************************************************
*
* sysPciAutoConfig - PCI autoConfig support routine
*
* For each PCI-Host Bridge in the system this routine instantiates the 
* PCI_SYSTEM structure needed to configure the system. This consists of 
* assigning address ranges to each category of PCI system resource: 
* Prefetchable and Non-Prefetchable 32-bit Memory, and 16- and 32-bit I/O. 
* Global values for the Cache Line Size and Maximum Latency are also specified.
* Finally, the four supplemental routines for device inclusion/exclusion, 
* interrupt assignment, and pre- and post-enumeration bridge initialization 
* are specified. Address ranges as well as PCI IDSEL-to-MPIC interrupt
* routing information for each host bridge is stored in a PCI space structure
* defined in sysLib.c, and which is expected as an input parameter to this
* routine. 
*
* RETURNS: N/A
*/

void sysPciAutoConfig (
  int maxBusIF,  /* Maximum number of Bus Interfaces to configure */
  PHB_PCI_SPACE * pPhbPciSpace  /* PCI space structure */
)
{
  int busInterface;

  for(busInterface = 0; busInterface < maxBusIF; busInterface++)
  {
    /* Initialize the sysParams structure */

    sysParams.pciMem32 = 0x0;
    sysParams.pciMem32Size = 0x0;
    sysParams.pciMemIo32 = 0x0;
    sysParams.pciMemIo32Size = 0x0;
    sysParams.pciIo32 = 0x0;
    sysParams.pciIo32Size = 0x0;
    sysParams.pciIo16 = 0x0;
    sysParams.pciIo16Size = 0x0;
    sysParams.maxBus = 0x0;
    sysParams.cacheSize = 0x0;
    sysParams.maxLatency = 0x0;
    sysParams.autoIntRouting = TRUE;
    sysParams.includeRtn = NULL;
    sysParams.intAssignRtn = NULL;
    sysParams.bridgePreConfigInit = NULL;
    sysParams.bridgePostConfigInit = NULL;
    sysParams.pciRollcallRtn = NULL;

    /* Set PCI space pointer to host bridge's PCI space structure */

    pPciSpace = &pPhbPciSpace[busInterface];

    /* 32-bit Non-prefetchable Memory Space */
 
    sysParams.pciMemIo32     = pPciSpace->pciMstrMemioBus;
    sysParams.pciMemIo32Size = pPciSpace->pciMstrMemioSize;

    /*
     * if address range starts at 0, force non-zero to avoid 
     * allocating zero which turns off BAR (per PCI spec).
     */

    if(sysParams.pciMemIo32 == 0)
    {
      sysParams.pciMemIo32     += 1;
      sysParams.pciMemIo32Size -= 1;
    }

    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMemIo32 = 0x%08x(0x%08x)\n", 
                           sysParams.pciMemIo32,
                           TRANSLATE(sysParams.pciMemIo32,
                           pPciSpace->pciMstrMemioBus,
                           pPciSpace->pciMstrMemioLocal), 0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMemIo32Size = 0x%08x\n",
                           sysParams.pciMemIo32Size,0,0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMemIo32End = 0x%08x(0x%08x)\n",
                           sysParams.pciMemIo32+sysParams.pciMemIo32Size,
                           TRANSLATE(sysParams.pciMemIo32+
                           sysParams.pciMemIo32Size, 
                           pPciSpace->pciMstrMemioBus, 
                           pPciSpace->pciMstrMemioLocal), 0,0,0,0);

    /* 32-bit Prefetchable Memory Space */
 
    sysParams.pciMem32     = pPciSpace->pciMstrMemBus;
    sysParams.pciMem32Size = pPciSpace->pciMstrMemSize;
 
    /*
     * if address range starts at 0, force non-zero to avoid 
     * allocating zero which turns off BAR (per PCI spec).
     */

    if(sysParams.pciMem32 == 0)
    {
      sysParams.pciMem32     += 1;
      sysParams.pciMem32Size -= 1;
    }

    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMem32 = 0x%08x (0x%08x)\n", 
                           sysParams.pciMem32,
                           TRANSLATE(sysParams.pciMem32,
                           pPciSpace->pciMstrMemBus,
                           pPciSpace->pciMstrMemLocal), 0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMem32Size   = 0x%08x\n",
                           sysParams.pciMem32Size,0,0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMem32End = 0x%08x (0x%08x)\n",
                           sysParams.pciMem32+sysParams.pciMem32Size,
                           TRANSLATE(sysParams.pciMem32+sysParams.pciMem32Size,
                           pPciSpace->pciMstrMemBus,
                           pPciSpace->pciMstrMemLocal), 0,0,0,0);

        /* 16-bit ISA I/O Space */

        sysParams.pciIo16     = pPciSpace->isaMstrIoBus;
        sysParams.pciIo16Size = pPciSpace->isaMstrIoSize;

        /*
         * if address range starts at 0, force non-zero to avoid 
         * allocating zero which turns off BAR (per PCI spec).
         */

        if (sysParams.pciIo16 == 0)
            {
            sysParams.pciIo16     += 1;
            sysParams.pciIo16Size -= 1;
            }

        PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo16 = 0x%08x (0x%08x)\n", 
                           sysParams.pciIo16,
                           TRANSLATE(sysParams.pciIo16,
                           pPciSpace->isaMstrIoBus,
                           pPciSpace->isaMstrIoLocal), 0,0,0,0);
        PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo16Size    = 0x%08x\n",
                           sysParams.pciIo16Size,0,0,0,0,0);
        PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo16End = 0x%08x (0x%08x)\n",
                           sysParams.pciIo16+sysParams.pciIo16Size,
                           TRANSLATE(sysParams.pciIo16+sysParams.pciIo16Size,
                           pPciSpace->isaMstrIoBus, 
                           pPciSpace->isaMstrIoLocal), 0,0,0,0);

        /* 32-bit PCI I/O Space */
 
        sysParams.pciIo32     = pPciSpace->pciMstrIoBus;
        sysParams.pciIo32Size = pPciSpace->pciMstrIoSize;

        /*
         * if address range starts at 0, force non-zero to avoid 
         * allocating zero which turns off BAR (per PCI spec).
         */

        if (sysParams.pciIo32 == 0)
            {
            sysParams.pciIo32     += 1;
            sysParams.pciIo32Size -= 1;
            }

        PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo32 = 0x%08x (0x%08x)\n", 
                           sysParams.pciIo32, 
                           TRANSLATE(sysParams.pciIo32,
                           pPciSpace->pciMstrIoBus,
                           pPciSpace->pciMstrIoLocal), 0,0,0,0);
        PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo32Size    = 0x%08x\n",
                           sysParams.pciIo32Size,0,0,0,0,0);
        PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo32End = 0x%08x (0x%08x)\n",
                           sysParams.pciIo32+sysParams.pciIo32Size,
                           TRANSLATE(sysParams.pciIo32+sysParams.pciIo32Size,
                           pPciSpace->pciMstrIoBus, pPciSpace->pciMstrIoLocal),
                           0,0,0,0);

        /* Configuration space parameters */
 
        sysParams.cacheSize      = PCI_CLINE_SZ;
        sysParams.maxLatency     = PCI_LAT_TIMER;
        sysParams.autoIntRouting = TRUE;
        sysParams.includeRtn     = sysPciAutoConfigInclude;
        sysParams.intAssignRtn   = sysPciAutoConfigIntAsgn;
        sysParams.bridgePreConfigInit  = NULL;
        sysParams.bridgePostConfigInit = NULL;
 
        /* Perform AutoConfig */

        pciAutoConfig (&sysParams);

        } 
    }
