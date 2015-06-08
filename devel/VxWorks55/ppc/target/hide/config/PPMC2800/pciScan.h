/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* pciScan.h - PCI0/1 scan and device configuration header
*
* DESCRIPTION:
*       This file contains PCI0/1 related constants and API of the PCI scanning
*		and device configurating.This file also includes MMU macros and
*		function declaration needed for the GalNet PCI scan module.
*
* DEPENDENCIES:
*		WRS vmLib.
*
******************************************************************************/

#ifndef __INCpciScanh
#define __INCpciScanh

/* includes */
#include <vmLib.h>
#include "config.h"
#include "./coreDrv/gt64360r.h"

/* defines  */
#define PCI_BUS0    0  
#define PCI_FUNC0	0  

#define PCI0_STATUS_COMMAND_REG     PCI_STATUS_AND_COMMAND
#define PCI1_STATUS_COMMAND_REG     PCI_STATUS_AND_COMMAND

#define MAX_PCI0_DEVICES 31
#define PCI_NUM_OF_BARS  42     /* Can be up to 42, see gtPci.h */

#ifdef INCLUDE_MMU
	/* mask parameter to vmStateSet */
	#define INITIAL_STATE_MASK               \
		    (UINT) VM_STATE_MASK_VALID     | \
			       VM_STATE_MASK_WRITABLE  | \
			       VM_STATE_MASK_CACHEABLE | \
			       VM_STATE_MASK_GUARDED
	/* state parameter to vmStateSet */
	#define	INITIAL_STATE                   \
	        (UINT) VM_STATE_VALID         | \
			       VM_STATE_WRITABLE      | \
			       VM_STATE_CACHEABLE_NOT | \
			       VM_STATE_GUARDED

#endif /* INCLUDE_MMU */


/* typedefs */
typedef enum _pciNumber {PCI_NUM0, PCI_NUM1} PCI_NUMBER;

#define MAX_BUSES 10
/* external routines declarations */
#ifdef INCLUDE_MMU
IMPORT STATUS sysMmuMapAdd (void *address,  /* MMU set mapping for PCI0/1 */
                                UINT length,
                                UINT initialStateMask,
                                UINT initialState); 

#endif /* INCLUDE_MMU */

IMPORT FUNCPTR frcPciConfigRead;    /* pciConfigLib pointer to PCI Read func  */
IMPORT FUNCPTR frcPciConfigWrite;   /* pciConfigLib pointer to PCI Write func */

/* pciScan.h API list */
void   frcPciSetActive(PCI_NUMBER);
void   frcPciShow();
STATUS pciScan(int);
IMPORT void pciAutoConfig ();
IMPORT void pciAutoConfigNonMonarch();
IMPORT int frcWhoAmI();
IMPORT void dbg_puts1(const char *);
IMPORT void dbg_puts0(const char *);

#include "pci.target.h"

#endif /* __INCpciScanh */
