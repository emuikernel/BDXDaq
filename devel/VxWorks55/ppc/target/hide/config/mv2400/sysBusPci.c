/* sysBusPci.c - mv2400 autoconfig support */

/* Copyright 1997-1999 Wind River Systems, Inc. All Rights Reserved */
/* Copyright 1999 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01h,16jul99,rhv   Adding code to prevent the allocation of PCI memory or I/O
                  address zero, which will disable a Base Address Register
                  (BAR).
01g,08jul99,rhv   Moving pciAutoConfigLib.h and pciConfigLib.h to proper
                  location in WRS tree.
01f,19may99,rhv   Changed DEC21140 to DEC21143.
01e,12may99,rhv   Modified to use new WRS PCI symbols. Added address debug
                  statements to display PCI auto-configuration pool info.
                  Updated address map.
01d,14apr99,rhv   Modified interrupt vector table to use controller-
                  independent symbols instead of MPIC-specific constants.
01c,02mar99,dmw   Fixed size allocation for 32 bit prefetchable memory,
                  base allocation for non-prefetchable memory, and updated 
                  comment memory map.
01b,21jan99,dmw   Fixed allocation for 16-bit I/O, extended address
                  support and turned on device 20 (PMC-Span) by default.
01a,14jan99,dmw   Adapted to mv2400 based on mp750 WRS version

*/
/*
DESCRIPTION

This module contains the "non-generic" or "board specific" PCI-PCI
bridge initialization code.  The module contains code to:

  1.  Determine if a particular function is to be excluded from the
          automatic configuration process.
  2.  Program the "interrupt line" field of the PCI configuration header.

*/

/* includes */

#include "vxWorks.h"
#include "logLib.h"
#include "taskLib.h"
#include "config.h"

#include "drv/pci/pciConfigLib.h"
#include "drv/pci/pciAutoConfigLib.h"

/* defines */

/* typedefs */

/* globals */

PCI_SYSTEM sysParams;

static UCHAR intLine [][4] =
  {
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 0, PCI Host bridge */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 1 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 2 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 3 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 4 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 5 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 6 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 7 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 8 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 9 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 10 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 11, PCI-ISA/IDE/USB brg */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 12 */

	{ UNIV_INT_VEC,              /* device number 13, Universe */
          0xff,
          0xff,
          0xff },  

	{ LN_INT_VEC,                /* device number 14, DEC 21143 Ethernet */
          0xff,
          0xff,
          0xff },

	{ 0xff, 0xff, 0xff, 0xff },  /* device number 15 */

#ifdef INCLUDE_MPIC

	/* device number 16, PMC slot 1 */
	{ PCI_PRI_INTA_VEC,          
          PCI_PRI_INTB_VEC,
          PCI_PRI_INTC_VEC,
          PCI_PRI_INTD_VEC },

	/* device number 17, PMC slot 2 */
	{ PCI_PRI_INTD_VEC,          
          PCI_PRI_INTA_VEC,
          PCI_PRI_INTB_VEC,
          PCI_PRI_INTC_VEC },

#else

	{ PCI_PRI_INTD_VEC,          /* device number 16, PMC slot 1 */
          PCI_PRI_INTD_VEC,
          PCI_PRI_INTD_VEC,
          PCI_PRI_INTD_VEC },

	{ PCI_PRI_INTD_VEC,          /* device number 17, PMC slot 2 */
          PCI_PRI_INTD_VEC,
          PCI_PRI_INTD_VEC,
          PCI_PRI_INTD_VEC },
#endif

	{ 0xff, 0xff, 0xff, 0xff },  /* device number 18 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 19 */

#ifdef INCLUDE_MPIC
	{ PCI_PRI_INTA_VEC,          /* device number 20, PMC-Span */
          PCI_PRI_INTB_VEC,
          PCI_PRI_INTC_VEC,
          PCI_PRI_INTD_VEC },
#else
	{ PCI_PRI_INTD_VEC,          /* device number 20, PMC-Span */
          PCI_PRI_INTD_VEC,
          PCI_PRI_INTD_VEC,
          PCI_PRI_INTD_VEC },
#endif
    { 0xff, 0xff, 0xff, 0xff },  /* device number 21 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 22 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 23 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 24 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 25 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 26 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 27 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 28 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 29 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 30 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 31 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 32 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 33 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 34 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 35 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 36 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 37 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 38 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 39 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 40 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 41 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 42 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 43 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 44 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 45 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 46 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 47 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 48 */
    { 0xff, 0xff, 0xff, 0xff },  /* device number 49 */
    { 0xff, 0xff, 0xff, 0xff }   /* device number 50 */

  };

/* locals */

/* forward declarations */

LOCAL UCHAR sysPciAutoConfigIntAsgn ( PCI_SYSTEM * pSys, PCI_LOC * pFunc,
    UCHAR intPin );
LOCAL STATUS sysPciAutoConfigInclude ( PCI_SYSTEM *pSys, PCI_LOC *pciLoc,
    UINT devVend );

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
 
LOCAL STATUS sysPciAutoConfigInclude
    (
    PCI_SYSTEM *pSys,		/* input: AutoConfig system information */
    PCI_LOC *pciLoc,		/* input: PCI address of this function */
    UINT     devVend		/* input: Device/vendor ID number      */
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

	case PCI_ID_LN_DEC21143:
	    retVal = OK;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Including DEC 21143\n",
		    0, 0, 0, 0, 0, 0);
	    break;

	case PCI_ID_BR_DEC21150:
	    retVal = OK;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Including DEC 21150 bridge\n",
		    0, 0, 0, 0, 0, 0);
	    break;

	case PCI_ID_UNIVERSE:
	    retVal = OK;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Including Universe\n",
		    0, 0, 0, 0, 0, 0);
	    break;

	default:
	    retVal = OK;
            PCI_AUTO_DEBUG_MSG("sysPciAutoconfigInclude: Including unknown device\n",
		    0, 0, 0, 0, 0, 0);
	    break;
	}

    return retVal;
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
    PCI_LOC * pFunc,
    UCHAR intPin 		/* input: interrupt pin number */
    )
    {
    UCHAR irqValue = 0xff;    /* Calculated value                */


    if (intPin == 0) 
	return irqValue;

    irqValue = intLine [(pFunc->device)][(intPin - 1)];

    PCI_AUTO_DEBUG_MSG("intAssign called for device [%d %d %d] IRQ: %d\n",
		pFunc->bus, pFunc->device, pFunc->function,
		irqValue, 0, 0 );

    return (irqValue);
    }

/******************************************************************************
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
* ADDRESS MAPS:
*
*             Non-Extended VME                    
* 0x80000000  ----------------- ISA_MSTR_IO_LOCAL
*            | ISA LEGACY      |     
* 0x80004000  ----------------- ISA_MSTR_IO_LOCAL + ISA_LEGACY_SIZE
*            | PCI 16-bit I/O  |
* 0x80010000  ----------------- PCI_MSTR_IO_LOCAL = ISA Master + ISA Size
*            | PCI 32-bit I/O  |     
* 0x80810000  ----------------- PCI_MSTR_IO_LOCAL + PCI_MSTR_IO_SIZE
*            :                 :
*  
* 0xC0000000  ----------------- PCI_MSTR_MEMIO_LOCAL
*            | NPREF PCI MEM   | 
* 0xC0800000  ----------------- PCI_MSTR_MEM_LOCAL = PCI MemIO + PCI MemIO Sz
*            | PCI 32-bit MEM  |
* 0xC1800000  ----------------- PCI_MSTR_MEM_LOCAL + PCI_MSTR_MEM_SIZE
* 
* 
*             Extended VME
* 0xFD000000  ----------------- PCI_MSTR_MEMIO_LOCAL
*            | NPREF PCI MEM   |
* 0xFD800000  ----------------- PCI_MSTR_MEM_LOCAL = PCI MemIO + PCI MemIO Sz
*            | PCI 32-bit MEM  |
* 0xFE000000  ----------------- ISA_MSTR_IO_LOCAL 
*            | ISA LEGACY      |
* 0xFE004000  ----------------- ISA_MSTR_IO_LOCAL + ISA_LEGACY_SIZE
*            | PCI 16-bit I/O  | 
* 0xFE010000  ----------------- ISA_MSTR_IO_LOCAL + ISA_MSTR_IO_SIZE
*            | PCI 32-bit I/O  |
* 0xFE810000  ----------------- PCI_MSTR_IO_LOCAL + PCI_MSTR_IO_SIZE
* 
*
* RETURNS: N/A
*/
void
sysPciAutoConfig(void)
{
  char txt[100];

  sprintf(txt,"sysPciAutoConfig reached\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);

    /* 32-bit Non-prefetchable Memory Space */
 
    sysParams.pciMemIo32 = PCI_MSTR_MEMIO_BUS;
    sysParams.pciMemIo32Size = PCI_MSTR_MEMIO_SIZE;

    /*
     * if address range starts at 0, force non-zero to avoid allocating zero
     * which turns off BAR (per PCI spec).
     */

    if (sysParams.pciMemIo32 == 0)
        {
        sysParams.pciMemIo32 += 1;
        sysParams.pciMemIo32Size -= 1;
        }

    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMemIo32     = 0x%08x (0x%08x)\n",
                       sysParams.pciMemIo32,
                       TRANSLATE(sysParams.pciMemIo32,PCI_MSTR_MEMIO_BUS,
                                 PCI_MSTR_MEMIO_LOCAL),0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMemIo32Size = 0x%08x\n",
                       sysParams.pciMemIo32Size,0,0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMemIo32End  = 0x%08x (0x%08x)\n",
                       sysParams.pciMemIo32+sysParams.pciMemIo32Size,
                       TRANSLATE(sysParams.pciMemIo32+sysParams.pciMemIo32Size,
                                 PCI_MSTR_MEMIO_BUS, PCI_MSTR_MEMIO_LOCAL),
                                 0,0,0,0);

    /* 32-bit Prefetchable Memory Space */
 
    sysParams.pciMem32 = PCI_MSTR_MEM_BUS;
    sysParams.pciMem32Size = PCI_MSTR_MEM_SIZE;
 
    /*
     * if address range starts at 0, force non-zero to avoid allocating zero
     * which turns off BAR (per PCI spec).
     */

    if (sysParams.pciMem32 == 0)
        {
        sysParams.pciMem32 += 1;
        sysParams.pciMem32Size -= 1;
        }

    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMem32       = 0x%08x (0x%08x)\n",
                       sysParams.pciMem32,
                       TRANSLATE(sysParams.pciMem32,PCI_MSTR_MEM_BUS,
                                 PCI_MSTR_MEM_LOCAL),0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMem32Size   = 0x%08x\n",
                       sysParams.pciMem32Size,0,0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciMem32End    = 0x%08x (0x%08x)\n",
                        sysParams.pciMem32+sysParams.pciMem32Size,
                       TRANSLATE(sysParams.pciMem32+sysParams.pciMem32Size,
                                 PCI_MSTR_MEM_BUS,PCI_MSTR_MEM_LOCAL),
                       0,0,0,0);

    /* 16-bit ISA I/O Space - start after legacy devices, kbd, com1, ...  */

    sysParams.pciIo16 = ISA_MSTR_IO_BUS + ISA_LEGACY_SIZE;
    sysParams.pciIo16Size = ISA_MSTR_IO_SIZE - ISA_LEGACY_SIZE;

    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo16        = 0x%08x (0x%08x)\n",
                       sysParams.pciIo16,
                       TRANSLATE(sysParams.pciIo16,ISA_MSTR_IO_BUS,
                                 ISA_MSTR_IO_LOCAL),
                       0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo16Size    = 0x%08x\n",
                       sysParams.pciIo16Size,0,0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo16End     = 0x%08x (0x%08x)\n",
                       sysParams.pciIo16+sysParams.pciIo16Size,
                       TRANSLATE(sysParams.pciIo16+sysParams.pciIo16Size,
                                 ISA_MSTR_IO_BUS, ISA_MSTR_IO_LOCAL),
                       0,0,0,0);

    /* 32-bit PCI I/O Space */
 
    sysParams.pciIo32 = PCI_MSTR_IO_BUS;
    sysParams.pciIo32Size = PCI_MSTR_IO_SIZE;

    /*
     * if address range starts at 0, force non-zero to avoid allocating zero
     * which turns off BAR (per PCI spec).
     */

    if (sysParams.pciIo32 == 0)
        {
        sysParams.pciIo32 += 1;
        sysParams.pciIo32Size -= 1;
        }

    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo32        = 0x%08x (0x%08x)\n",
                       sysParams.pciIo32, 
                       TRANSLATE(sysParams.pciIo32,PCI_MSTR_IO_BUS,
                                 PCI_MSTR_IO_LOCAL),
                       0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo32Size    = 0x%08x\n",
                       sysParams.pciIo32Size,0,0,0,0,0);
    PCI_AUTO_DEBUG_MSG("sysPciAutoConfig: pciIo32End     = 0x%08x (0x%08x)\n",
                       sysParams.pciIo32+sysParams.pciIo32Size,
                       TRANSLATE(sysParams.pciIo32+sysParams.pciIo32Size,
                                 PCI_MSTR_IO_BUS, PCI_MSTR_IO_LOCAL),
                       0,0,0,0);

  sprintf(txt,"sysPciAutoConfig 111\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  /* Configuration space parameters */
  sysParams.cacheSize = (_CACHE_ALIGN_SIZE/4);
  sysParams.maxLatency = PCI_LAT_TIMER;
  sysParams.autoIntRouting = TRUE;
  sysParams.includeRtn = sysPciAutoConfigInclude;

  sprintf(txt,"sysPciAutoConfig 222\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);



  sysParams.intAssignRtn = sysPciAutoConfigIntAsgn;
  /*sysParams.intAssignRtn   = NULL;*/



  sprintf(txt,"sysPciAutoConfig 333\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  sysParams.bridgePreConfigInit = NULL;
  sysParams.bridgePostConfigInit = NULL;

  /* Perform AutoConfig */
  sprintf(txt,"sysPciAutoConfig 444\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  pciAutoConfig(&sysParams);

  sprintf(txt,"sysPciAutoConfig 555\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
 
  return;
}










