/* sysBusPci.c - autoconfig support */

/* Copyright 1997-2001 Wind River Systems, Inc. All Rights Reserved */
/* Copyright 1999-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01c,10oct01,scb  Do not exclude my own harrier in the pciAutoConfig list.
01b,14nov00,dmw  Added Slave Harrier to include list.
01a,08oct00,dmw  Created (from rev 01d mv5100/sysBusPci.c).
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
#include "pci/pciAutoConfigLib.c"

/* defines */

/* typedefs */

/* globals */

PCI_SYSTEM sysParams;

#ifdef INTERRUPT_ROUTING_TABLE
    INTERRUPT_ROUTING_TABLE	/* see config.h for definition */
#endif

/* locals */

/* forward declarations */

LOCAL UCHAR sysPciAutoConfigIntAsgn ( PCI_SYSTEM * pSys, PCI_LOC * pFunc,
			 	      UCHAR intPin );

LOCAL STATUS sysPciAutoConfigInclude ( PCI_SYSTEM *pSys, PCI_LOC *pciLoc,
				       UINT devVend );

/* externals */

IMPORT	BOOL	sysSysconAsserted (void);

/* subroutines */

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
  PCI_SYSTEM * pSys,		/* input: AutoConfig system information */
  PCI_LOC *    pciLoc,	/* input: PCI address of this function */
  UINT         devVend	/* input: Device/vendor ID number      */
)
{
  BOOL retVal = OK;

#ifdef PCI_AUTO_DEBUG
  char txt[100];
  sprintf(txt,"sysPciAutoconfigInclude reached, devVend=0x%08x\r\n",devVend);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  switch(devVend)
  {

    /* EXCLUDED Devices */

#ifndef INCLUDE_DEC2155X
    case PCI_ID_BR_DEC21554:
      retVal = ERROR;
#ifdef PCI_AUTO_DEBUG
      sprintf(txt,"sysPciAutoconfigInclude: Excluding DEC 21554 bridge\r\n");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      break;
#endif

	/* INCLUDED Devices */

    case PCI_ID_HARRIER:
      retVal = OK;
#ifdef PCI_AUTO_DEBUG
      sprintf(txt,"sysPciAutoconfigInclude: Including Slave Harrier\r\n");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      break;

    case PCI_ID_I82559:
    case PCI_ID_I82559ER:
      retVal = OK;
#ifdef PCI_AUTO_DEBUG
      sprintf(txt,"sysPciAutoconfigInclude: Including i82559\r\n");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      break;

    case PCI_ID_LN_DEC21040:
    case PCI_ID_LN_DEC21140:
    case PCI_ID_LN_DEC21143:
      retVal = OK;
#ifdef PCI_AUTO_DEBUG
      sprintf(txt,"sysPciAutoconfigInclude: Including DEC 21x4x\r\n");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      break;

    default:
      retVal = OK;
#ifdef PCI_AUTO_DEBUG
      sprintf(txt,"sysPciAutoconfigInclude: Including unknown device\r\n");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
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

LOCAL UCHAR sysPciAutoConfigIntAsgn
    ( 
    PCI_SYSTEM * pSys,		/* input: AutoConfig system information */
    PCI_LOC *    pFunc,		/* input: function's location in the system */
    UCHAR        intPin 	/* input: interrupt pin number */
    )
{
  UCHAR irqValue = 0xff;  /* Calculated value                */
#ifdef PCI_AUTO_DEBUG
  char txt[100];
#endif


  if(intPin == 0) return(irqValue);

  irqValue = intLine [(pFunc->device)][(intPin - 1)];

#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"intAssign called for device [%d %d %d] IRQ: %d\r\n",
		  pFunc->bus, pFunc->device, pFunc->function,
		  irqValue);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  return(irqValue);
}
  

/*******************************************************************************
*
* sysPciAutoConfig - PCI autoConfig support routine
*
* This routine instantiates the PCI_SYSTEM structure needed to configure
* the system. This consists of assigning address ranges to each category
* of PCI system resource: Prefetchable and Non-Prefetchable 32-bit Memory, and
* 16- and 32-bit I/O. The MASK_PCI_xxx_BITS is dependant on the memory map,
* (i.e. if EXTENDED_VME is defined).  Global values for the Cache Line Size 
* and Maximum Latency are also specified. Finally, the four supplemental 
* routines for device inclusion/exclusion, interrupt assignment, and pre- and
* post-enumeration bridge initialization are specified. 
*
* RETURNS: N/A
*/

void
sysPciAutoConfig (void)
{
#ifdef PCI_AUTO_DEBUG
  char txt[100];
#endif

#ifdef PCI_AUTO_DEBUG
  sysDebugMsg("\n\nsysPciAutoConfig reached\r\n",CONTINUE_EXECUTION);
#endif

  /* 32-bit Non-prefetchable Memory Space */

  sysParams.pciMemIo32     = PCI_MSTR_MEMIO_BUS;
  sysParams.pciMemIo32Size = PCI_MSTR_MEMIO_SIZE;

  /* if address range starts at 0, force non-zero to avoid allocating zero
     which turns off BAR (per PCI spec). */

  if(sysParams.pciMemIo32 == 0)
  {
    sysParams.pciMemIo32     += 1;
    sysParams.pciMemIo32Size -= 1;
  }


#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"sysPciAutoConfig: pciMemIo32     = 0x%08x (0x%08x)\r\n",
			sysParams.pciMemIo32,
			TRANSLATE(sysParams.pciMemIo32,PCI_MSTR_MEMIO_BUS,
				  PCI_MSTR_MEMIO_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  sprintf(txt,"sysPciAutoConfig: pciMemIo32Size = 0x%08x\r\n",sysParams.pciMemIo32Size);
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  sprintf(txt,"sysPciAutoConfig: pciMemIo32End  = 0x%08x (0x%08x)\r\n",
                       sysParams.pciMemIo32+sysParams.pciMemIo32Size,
                       TRANSLATE(sysParams.pciMemIo32+sysParams.pciMemIo32Size,
                                 PCI_MSTR_MEMIO_BUS, PCI_MSTR_MEMIO_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  /* 32-bit Prefetchable Memory Space */
 
  sysParams.pciMem32     = PCI_MSTR_MEM_BUS;
  sysParams.pciMem32Size = PCI_MSTR_MEM_SIZE;
 
  /* if address range starts at 0, force non-zero to avoid allocating zero
     which turns off BAR (per PCI spec). */

  if(sysParams.pciMem32 == 0)
  {
    sysParams.pciMem32     += 1;
    sysParams.pciMem32Size -= 1;
  }

#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"sysPciAutoConfig: pciMem32       = 0x%08x (0x%08x)\r\n",
                        sysParams.pciMem32,
                        TRANSLATE(sysParams.pciMem32,PCI_MSTR_MEM_BUS,
                                  PCI_MSTR_MEM_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  sprintf(txt,"sysPciAutoConfig: pciMem32Size   = 0x%08x\r\n",sysParams.pciMem32Size);
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  sprintf(txt,"sysPciAutoConfig: pciMem32End    = 0x%08x (0x%08x)\r\n",
                        sysParams.pciMem32+sysParams.pciMem32Size,
                        TRANSLATE(sysParams.pciMem32+sysParams.pciMem32Size,
                                  PCI_MSTR_MEM_BUS,PCI_MSTR_MEM_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  /* 16-bit ISA I/O Space - start after legacy devices, kbd, com1, ...  */

  sysParams.pciIo16     = ISA_MSTR_IO_BUS  + ISA_LEGACY_SIZE;
  sysParams.pciIo16Size = ISA_MSTR_IO_SIZE - ISA_LEGACY_SIZE;

#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"sysPciAutoConfig: pciIo16        = 0x%08x (0x%08x)\r\n",
                        sysParams.pciIo16,
                        TRANSLATE(sysParams.pciIo16,ISA_MSTR_IO_BUS,
                                  ISA_MSTR_IO_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  sprintf(txt,"sysPciAutoConfig: pciIo16Size    = 0x%08x\r\n",sysParams.pciIo16Size);
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  sprintf(txt,"sysPciAutoConfig: pciIo16End     = 0x%08x (0x%08x)\r\n",
                        sysParams.pciIo16+sysParams.pciIo16Size,
                        TRANSLATE(sysParams.pciIo16+sysParams.pciIo16Size,
                                  ISA_MSTR_IO_BUS, ISA_MSTR_IO_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  /* 32-bit PCI I/O Space */
 
  sysParams.pciIo32     = PCI_MSTR_IO_BUS;
  sysParams.pciIo32Size = PCI_MSTR_IO_SIZE;

  /* if address range starts at 0, force non-zero to avoid allocating zero
     which turns off BAR (per PCI spec). */

  if(sysParams.pciIo32 == 0)
  {
    sysParams.pciIo32     += 1;
    sysParams.pciIo32Size -= 1;
  }

#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"sysPciAutoConfig: pciIo32        = 0x%08x (0x%08x)\r\n",
                        sysParams.pciIo32, 
                        TRANSLATE(sysParams.pciIo32,PCI_MSTR_IO_BUS,
                                  PCI_MSTR_IO_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  sprintf(txt,"sysPciAutoConfig: pciIo32Size    = 0x%08x\r\n",sysParams.pciIo32Size);
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  sprintf(txt,"sysPciAutoConfig: pciIo32End     = 0x%08x (0x%08x)\r\n",
                        sysParams.pciIo32+sysParams.pciIo32Size,
                        TRANSLATE(sysParams.pciIo32+sysParams.pciIo32Size,
                                  PCI_MSTR_IO_BUS, PCI_MSTR_IO_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif


  /* Configuration space parameters */
 
  sysParams.cacheSize      = (_CACHE_ALIGN_SIZE/4);
  sysParams.maxLatency     = PCI_LAT_TIMER;
  sysParams.autoIntRouting = TRUE;
  sysParams.includeRtn     = sysPciAutoConfigInclude;

#ifdef INTERRUPT_ROUTING_TABLE
  sysParams.intAssignRtn   = sysPciAutoConfigIntAsgn;
#else  /* INTERRUPT_ROUTING_TABLE */
  sysParams.intAssignRtn   = NULL;
#endif  /* INTERRUPT_ROUTING_TABLE */
  sysParams.bridgePreConfigInit  = NULL;
  sysParams.bridgePostConfigInit = NULL;
 

  /* Perform AutoConfig */

#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"pciAutoConfig called.\r\n");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  pciAutoConfig(&sysParams);

#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"pciAutoConfig done.\r\n");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  return;
}
