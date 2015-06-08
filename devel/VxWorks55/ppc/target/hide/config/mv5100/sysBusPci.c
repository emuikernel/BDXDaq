/* sysBusPci.c - autoconfig support */

/* Copyright 1997-2001 Wind River Systems, Inc. All Rights Reserved */
/* Copyright 1999-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01h,30may01,dtr  Putting define for the interrupt routing table in here to
                 stop the project facility from stripping it to a uncompilable
                 shadow of it's former self.
01g,10may01,pch  Add definition of PCI_AUTO_DEBUG_MSG macro.
01f,25apr01,srr  Change reference to global pciAutoConfigLib.
01e,08mar01,rhk  Additional Coding Standards changes.
01e,03nov00,rcs  fix path to pciAutoConfigLib.h
01d,16aug00,dmw  Wind River coding standards review.
01c,10apr00,dmw  Added SCSI support.
01b,03apr00,dmw  included "latest" ./pci/pciAutoConfigLib.h.
01a,13mar00,dmw  Created (from rev 01a mcpn765/sysBusPci.c).
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
#include "drv/pci/pciAutoConfigLib.h"

/* defines */

#ifndef PCI_AUTO_DEBUG_MSG

#  ifdef PCI_AUTO_DEBUG

     IMPORT FUNCPTR _func_logMsg;
     BOOL pciAutoDebug = TRUE;

#    define PCI_AUTO_DEBUG_MSG(s, a, b, c, d, e, f) \
	{ \
	if ((pciAutoDebug == TRUE) && (_func_logMsg != NULL)) \
	    { \
	    (*_func_logMsg)(s, a, b, c, d, e, f); \
	    taskDelay(10); \
	    } \
	}

#  else /* PCI_AUTO_DEBUG */

#    define PCI_AUTO_DEBUG_MSG(s, a, b, c, d, e, f)

#  endif /* PCI_AUTO_DEBUG */

#endif /* PCI_AUTO_DEBUG_MSG */

/* typedefs */

/* globals */

PCI_SYSTEM sysParams;


/* This define is just to say that interrupt routing table is available */
#define INTERRUPT_ROUTING_TABLE

/* This table describes the mv5100 PCI IDSEL-to-MPIC interrupt routing. */
static UCHAR intLine [][4] = \
    { \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 0/31, PCI Host bridge */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 1  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 2  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 3  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 4  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 5  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 6  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 7  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 8  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 9  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 10 - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 11 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 12 */ \
        { UNIV_INT_LVL,             /* device number 13, Universe */ \
          UNIV_INT_LVL1,\
          UNIV_INT_LVL2,\
          UNIV_INT_LVL3 },\
        { LN1_INT_VEC,              /* device number 14, I82559 Ethernet */\
          0xff,\
          0xff,\
          0xff },\
        { 0xff, 0xff, 0xff, 0xff }, /* device number 15 */ \
        { PCI_INTA_VEC,	            /* device number 16, PMC slot 1 */\
          PCI_INTB_VEC,\
          PCI_INTC_VEC,\
          PCI_INTD_VEC },\
        { PCI_INTD_VEC,	            /* device number 17, PMC slot 2 */\
          PCI_INTA_VEC,\
          PCI_INTB_VEC,\
          PCI_INTC_VEC },\
        { 0xff, 0xff, 0xff, 0xff }, /* device number 18 */\
        { LN2_INT_VEC,              /* device number 19 DEC 21143 Ethernet */\
          0xff,\
          0xff,\
          0xff },\
        { PCI_INTA_VEC,             /* device number 20 PMCSpan */ \
          PCI_INTB_VEC,\
          PCI_INTC_VEC,\
          PCI_INTD_VEC },\
        { 0xff, 0xff, 0xff, 0xff }, /* device number 21 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 22 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 23 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 24 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 25 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 26 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 27 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 28 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 29 */ \
        { 0xff, 0xff, 0xff, 0xff }  /* device number 30 */ \
    };



/* locals */

/* forward declarations */

LOCAL UCHAR sysPciAutoConfigIntAsgn ( PCI_SYSTEM * pSys, PCI_LOC * pFunc,
			 	      UCHAR intPin );

LOCAL STATUS sysPciAutoConfigInclude ( PCI_SYSTEM *pSys, PCI_LOC *pciLoc,
				       UINT devVend );

/* externals */

IMPORT	BOOL	sysSysconAsserted (void);

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
 
LOCAL STATUS sysPciAutoConfigInclude
    (
    PCI_SYSTEM * pSys,		/* input: AutoConfig system information */
    PCI_LOC *    pciLoc,	/* input: PCI address of this function */
    UINT         devVend	/* input: Device/vendor ID number      */
    )
    {
    BOOL retVal = OK;

    /* If it's the host bridge then exclude it */

    if ((pciLoc->bus == 0) && (pciLoc->device == 0) && (pciLoc->function == 0))
        return ERROR;

    switch(devVend)
        {

        /* EXCLUDED Devices */

        case PCI_ID_HAWK:
            retVal = ERROR;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Excluding Hawk\n",
                               0, 0, 0, 0, 0, 0);
            break;

        case PCI_ID_IBC:
            retVal = ERROR;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Excluding IBC\n",
                               0, 0, 0, 0, 0, 0);
            break;

        case PCI_ID_IDE:
            retVal = ERROR;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Excluding IDE\n",
                               0, 0, 0, 0, 0, 0);
            break;

	/* INCLUDED Devices */

        case PCI_ID_SCSI:
            retVal = OK;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Including SCSI\n",
                               0, 0, 0, 0, 0, 0);
            break;

        case PCI_ID_UNIVERSE:
            retVal = OK;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Including Universe\n",
                               0, 0, 0, 0, 0, 0);
            break;

        case PCI_ID_I82559:
        case PCI_ID_I82559ER:
            retVal = OK;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Including i82559\n",
				0, 0, 0, 0, 0, 0);
            break;

        case PCI_ID_LN_DEC21040:
        case PCI_ID_LN_DEC21140:
        case PCI_ID_LN_DEC21143:
            retVal = OK;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Including DEC 21x4x\n",
                               0, 0, 0, 0, 0, 0);
            break;

        case PCI_ID_BR_DEC21150: /* used on PMCSpan */
            retVal = OK;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Incl DEC 21150 \n",
                               0, 0, 0, 0, 0, 0);
            break;

        default:
            retVal = OK;
            PCI_AUTO_DEBUG_MSG(
                "sysPciAutoconfigInclude: Including unknown device\n",
		 0, 0, 0, 0, 0, 0);
            break;
        }

    return retVal;
    }


/******************************************************************************
*
* sysPciAutoConfigIntAsgn - Assign the "interrupt line" value
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


    if (intPin == 0) 
	return irqValue;

    irqValue = intLine [(pFunc->device)][(intPin - 1)];

    PCI_AUTO_DEBUG_MSG("intAssign called for device [%d %d %d] IRQ: %d\n",
			pFunc->bus, pFunc->device, pFunc->function,
			irqValue, 0, 0 );

    return (irqValue);
    }
  

/*******************************************************************************
*
* sysPciAutoConfig - PCI autoConfig support routine
*
* This routine instantiates the PCI_SYSTEM structure needed to configure
* the system. This consists of assigning address ranges to each category
* of PCI system resource: Prefetchable and Non-Prefetchable 32-bit Memory, and
* 16- and 32-bit I/O. The 'MASK_PCI_xxx_BITS' is dependant on the memory map,
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
  char txt[100];

  /* 32-bit Non-prefetchable Memory Space */
 
  sysParams.pciMemIo32     = PCI_MSTR_MEMIO_BUS;
  sysParams.pciMemIo32Size = PCI_MSTR_MEMIO_SIZE;

  /*
   * if address range starts at 0, force non-zero to avoid allocating zero
   * which turns off BAR (per PCI spec).
   */

  if(sysParams.pciMemIo32 == 0)
  {
    sysParams.pciMemIo32     += 1;
    sysParams.pciMemIo32Size -= 1;
  }

#ifdef MYDEBUG
  sprintf(txt,"sysPciAutoConfig: pciMemIo32     = 0x%08x (0x%08x)\r\n",
			sysParams.pciMemIo32,
			TRANSLATE(sysParams.pciMemIo32,PCI_MSTR_MEMIO_BUS,
				  PCI_MSTR_MEMIO_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
  sprintf(txt,"sysPciAutoConfig: pciMemIo32Size = 0x%08x\r\n",
			sysParams.pciMemIo32Size);
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
 
  /*
   * if address range starts at 0, force non-zero to avoid allocating zero
   * which turns off BAR (per PCI spec).
   */

  if(sysParams.pciMem32 == 0)
  {
    sysParams.pciMem32     += 1;
    sysParams.pciMem32Size -= 1;
  }

#ifdef MYDEBUG
  sprintf(txt,"sysPciAutoConfig: pciMem32       = 0x%08x (0x%08x)\r\n",
                        sysParams.pciMem32,
                        TRANSLATE(sysParams.pciMem32,PCI_MSTR_MEM_BUS,
                                  PCI_MSTR_MEM_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
  sprintf(txt,"sysPciAutoConfig: pciMem32Size   = 0x%08x\r\n",
                        sysParams.pciMem32Size);
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

#ifdef MYDEBUG
  sprintf(txt,"sysPciAutoConfig: pciIo16        = 0x%08x (0x%08x)\r\n",
                        sysParams.pciIo16,
                        TRANSLATE(sysParams.pciIo16,ISA_MSTR_IO_BUS,
                                  ISA_MSTR_IO_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
  sprintf(txt,"sysPciAutoConfig: pciIo16Size    = 0x%08x\r\n",
                        sysParams.pciIo16Size);
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

  /*
   * if address range starts at 0, force non-zero to avoid allocating zero
   * which turns off BAR (per PCI spec).
   */

  if(sysParams.pciIo32 == 0)
  {
    sysParams.pciIo32     += 1;
    sysParams.pciIo32Size -= 1;
  }

#ifdef MYDEBUG
  sprintf(txt,"sysPciAutoConfig: pciIo32        = 0x%08x (0x%08x)\r\n",
                        sysParams.pciIo32, 
                        TRANSLATE(sysParams.pciIo32,PCI_MSTR_IO_BUS,
                                  PCI_MSTR_IO_LOCAL));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
  sprintf(txt,"sysPciAutoConfig: pciIo32Size    = 0x%08x\r\n",
                        sysParams.pciIo32Size);
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

  pciAutoConfig (&sysParams);
 
  return;
}
