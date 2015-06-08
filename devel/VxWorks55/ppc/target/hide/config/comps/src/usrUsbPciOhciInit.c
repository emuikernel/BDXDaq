/* usrUsbPciOhciInit.c - BSP-specific USB Functions */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
Modification history
--------------------
01g,08may02,wef  rewrite description to indicate this is now sample code.
01f,13dec01,wef  merge from veloce view
01g,08dec01,wef  fixed warnings
01f,02oct01,wef  merge from USB 1.1 to veloce
01e,10jun00,wef  changed some defines for ebsa - added pci mem configuration for
		 mtx604.
01d,10jan00,wef  added support for new lucent chip ohci controller.  also this
		 now scans for all supported device types instead of looking for
		 what ever vid and pid were hard coded.  BSP specific parts are
		 now limited to only the configuration of the HC instead of 
		 the finding AND the configuration.  Support for this card is 
		 only on the pcPentium BSP.  Added ARM support.  
01c,10feb00,wef  moved generic pci utility functions to usrUsbPciLib.c
01b,09feb00,wef  changed to be a configlette instead of separate files
		 contained in BSP directorys
01a,08feb00,wef  adapted from version 01e of usrUsbPciLib.c.
*/

/*
DESCRIPTION

This files demonstrates how the a USB OHCI conrtoller can be manually
configured into a vxWorks system.  The examples given are the steps necessary
for the, pcPentium, mtx604 and ebsa285 BSP's, as each BSP has different 
requirements on how a pci device should be configured. 

*/

/* includes */

#include "drv/pci/pciConfigLib.h"	/* VxWorks PCI funcs */
#include "drv/pci/pciIntLib.h"
#include "usb/pciConstants.h"

/* Device specific defines */

#define MTX604_PCI_INTERRUPT_LINE	0xb
#define NUM_SLOTS			4

/*
 * Constants related to OHCI PCI Cards
 *
 * If a new OHCI card comes to market, add its vendor and device ID's
 * here and increment NUM_OHCI_CONTROLLER_TYPES
 *
 */

#define PCI_VENDOR_ID_OPTI		0x1045
#define PCI_DEVICE_ID_OPTI		0xc861
#define PCI_VENDOR_ID_LUCENT		0x11c1
#define PCI_DEVICE_ID_LUCENT		0x5803
#define PCI_VENDOR_ID_CMD		0x1095
#define PCI_DEVICE_ID_CMD		0x0670
#define NUM_OHCI_CONTROLLER_TYPES	3

/*
 * OHCI_PCI_RESOURCES
 *
 * OHCI_PCI_RESOURCES - establishes a structure to maintain a 
 * list of OHCI card types that are supported by the Wind River
 * USB stack.
 *
 */

typedef struct ohci_pci_resources
    {
    UINT32	vendorId;
    UINT32	deviceId;
    } OHCI_PCI_RESOURCES;

/* Locals */

LOCAL OHCI_PCI_RESOURCES ohciPciResources [NUM_OHCI_CONTROLLER_TYPES] =
    {
    {PCI_VENDOR_ID_LUCENT, PCI_DEVICE_ID_LUCENT},	/* Lucent USS344 */
    {PCI_VENDOR_ID_OPTI, PCI_DEVICE_ID_OPTI},		/* Opti		*/
    {PCI_VENDOR_ID_CMD, PCI_DEVICE_ID_CMD}		/* CMD 670	*/
    };


#if CPU == STRONGARM
LOCAL UINT32 pciDeviceMemBaseAddr = 0x80000000;

LOCAL char ohciPciIrqAssignments [NUM_SLOTS] = /* four slots on ebsa285 */
    {
    (char) PCI_INT_LVL0,
    (char) PCI_INT_LVL1,
    (char) PCI_INT_LVL2,
    (char) PCI_INT_LVL3
    };
#endif

#ifdef  MTX
LOCAL UINT32 pciDeviceMemBaseAddr = CPU2PCI_ADDR2_START_VAL;
LOCAL UINT32 * pciDeviceMemSize = NULL; 
#endif

/* imports */

#if CPU == PENTIUM
IMPORT STATUS           sysMmuMapAdd (void * address, UINT len,
                                      UINT initialStateMask,
                                      UINT initialState);
#endif


/*****************************************************************************
*
* sysUsbPciOhciInit - initialize BSP specific USB hardware
*
* This function initialize BSP specific USB hardware
*
* RETURNS: N/A
*/

void sysUsbPciOhciInit (void) 
    {

    int pciBus;
    int pciDevice;
    int pciFunc;
    int idx = 0, idy = 0;
    int hostControllerUnits;
    UINT32 membaseCsr;


    /* Search for all supported typs of OHCI cards that may be plugged in */

    for (idx = 0; idx < NUM_OHCI_CONTROLLER_TYPES; idx++)
	{

	if (ohciPciResources[idx].vendorId == PCI_VENDOR_ID_LUCENT && 
	    ohciPciResources[idx].deviceId == PCI_DEVICE_ID_LUCENT)
	    {

	    /*  
	     * the Lucent card contains 4 ohci controllers, loop through 
	     * and find all of them.
	     */

	    hostControllerUnits = 4;

	    }
	else 
	    {
	    /* this case is all other single host controller cards */
	
	    hostControllerUnits = 1;
	    }

	for (idy = 0; idy < hostControllerUnits; idy++)
	    {
	    if (pciFindDevice (ohciPciResources[idx].vendorId,
			       ohciPciResources[idx].deviceId,
			       idy,
			       &pciBus,
			       &pciDevice,
			       &pciFunc)		!= OK)
		{
		continue;
		}

/*  BSP specific PCI configuration section */
#if CPU == PENTIUM
	    /* get memory base address and IO base address */

	    pciConfigInLong (pciBus, 
			     pciDevice, 
			     pciFunc, 
			     PCI_CFG_BASE_ADDRESS_0, 
			     &membaseCsr);

	    membaseCsr &= PCI_MEMBASE_MASK;

	    /* add the entry to sysPhysMemDesc[] */

#if (defined(INCLUDE_MMU_FULL) || defined(INCLUDE_MMU_BASIC))
	    if (sysMmuMapAdd((void *)membaseCsr, 0x1000, 0x3f, 0x5) == ERROR)
		break; 
#endif

#endif	/* endif PC_PENTIUM */

#ifdef MTX
	/*  
	 * Attempt to determine how much memory the controller needs.  The 
	 * PCI spec says to write all 1's to the BAR, then read it back.  It
	 * will return, in inverse 1's complement form  how much it uses on 
	 * this read.
	 */
			
	pciConfigOutLong (pciBus,
		          pciDevice, 
		          pciFunc, 
		          PCI_CFG_BASE_ADDRESS_0,
			  0xffff); 

	pciConfigInLong (pciBus,
		         pciDevice, 
		         pciFunc, 
		         PCI_CFG_BASE_ADDRESS_0,
			 pciDeviceMemSize); 

	pciDevConfig (pciBus, 
		      pciDevice, 
		      pciFunc, 
		      NULL, 
		      pciDeviceMemBaseAddr,
		      PCI_CMD_MASTER_ENABLE | PCI_CMD_MEM_ENABLE);

	/* convert value read from inverse 1's compelemnt to a hex value */

	*pciDeviceMemSize = (~((UINT32)*pciDeviceMemSize -1)) & 0x0000ffff;

  	pciDeviceMemBaseAddr += *pciDeviceMemSize;

	/* Set up the controller's interrupt assignment. */

/* Hack for MTX604.  BSP should define this for us */

#define PCI_CFG_TYPE PCI_CFG_FORCE  

	if (PCI_CFG_TYPE == PCI_CFG_FORCE)
	     pciConfigOutByte (pciBus, 
			       pciDevice, 
			       pciFunc, 
			       PCI_CFG_DEV_INT_LINE, 
			       MTX604_PCI_INTERRUPT_LINE);
#endif	/* endif MTX */

#if	(CPU == STRONGARM)
	pciConfigOutByte (pciBus, 
  		          pciDevice, 
			  pciFunc,
			  PCI_CFG_DEV_INT_LINE,
			  ohciPciIrqAssignments[pciDevice]);

	pciDevConfig (pciBus, 
		      pciDevice, 
		      pciFunc, 
		      NULL, 
		      pciDeviceMemBaseAddr,
		      PCI_CMD_MASTER_ENABLE | PCI_CMD_MEM_ENABLE);
#endif	/* STRONGARM */


	    }
	}
    }

