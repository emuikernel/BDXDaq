/* sysBusPci.c - Template autoconfig support */

/* Copyright 1997-1999 Wind River Systems, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,08apr99,dat  SPR 26491, fixed PCI macro names
01a,02feb99,tm   written

*/
/*
DESCRIPTION

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

/* TODO - define the interrupt routing for your platform here */

static UCHAR intLine [][4] =
    {
    /*    IntA  IntB  IntC  IntD   */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 0 */
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
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 11 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 12 */
	{ 0xff, 0xff, 0xff, 0xff },  /* device number 13 */
	{ 0x02, 0xff, 0xff, 0xff },  /* device number 14 */
	{ 0xff, 0xff, 0xff, 0xff }   /* device number 15 */
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

	/* TODO - add any excluded devices by device/vendor ID here */

	default:
	    retVal = OK;
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

    /* TODO - add any non-standard interrupt assignments here */

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
* 16- and 32-bit I/O. Global values for the Cache Line Size and Maximum
* Latency are also specified. Finally, the four supplemental routines for 
* device inclusion/exclusion, interrupt assignment, and pre- and
* post-enumeration bridge initialization are specified. 
*
* RETURNS: N/A
*/

void sysPciAutoConfig (void)
    {

    /* TODO - define platform-specific PCI address spaces here */

    /* 32-bit Non-prefetchable Memory Space */
 
    sysParams.pciMemIo32 = PCI_MSTR_MEMIO_BUS;
    sysParams.pciMemIo32Size = PCI_MSTR_MEMIO_SIZE;

    /* 32-bit Prefetchable Memory Space */
 
    sysParams.pciMem32= PCI_MSTR_MEM_BUS;
    sysParams.pciMem32Size = PCI_MSTR_MEM_SIZE;
 
    /* 16-bit ISA I/O Space */

    sysParams.pciIo16 = 0;
    sysParams.pciIo16Size = 0;

    /* 32-bit PCI I/O Space */
 
    sysParams.pciIo32 = PCI_MSTR_IO_BUS;
    sysParams.pciIo32Size = PCI_MSTR_IO_SIZE;
 
    /* Configuration space parameters */
 
    sysParams.cacheSize = (_CACHE_ALIGN_SIZE/4);
    sysParams.maxLatency = PCI_LAT_TIMER;
    sysParams.autoIntRouting = TRUE;
    sysParams.includeRtn = sysPciAutoConfigInclude;
    sysParams.intAssignRtn = sysPciAutoConfigIntAsgn;
    sysParams.bridgePreConfigInit = NULL;
    sysParams.bridgePostConfigInit = NULL;
    sysParams.autoIntRouting = TRUE;
 
    /* Perform AutoConfig */
 
    pciAutoConfig (&sysParams);
 
    return;
    }
 
