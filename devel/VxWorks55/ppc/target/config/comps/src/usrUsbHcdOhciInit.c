/* usrUsbHcdOhciInit.c - Initialization of a OHCI Host Controller Driver */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
Modification history
--------------------
01c,14jan01,wef	 fixed bug that hcd scan would quit if the host controllers
		 were not sequntially configured in pci memory space.
01b,16jan01,wef	 modified to search for multiple host controllers instead of
		 just one.
01a,23aug00,wef	 Created
*/

/*
DESCRIPTION

This configlette initializes a USB Host Controller if present on a system.
The initialization process includes bringing up the chip and also "attaches" 
the chip to the previously initilazed USB stack.  The stack is initialized in 
usrUsbInit.c

*/


/* includes */

#include "usb/usbPciLib.h"
#include "usb/pciConstants.h"
#include "usb/usbdLib.h"
#include "drv/usb/usbOhci.h"		
#include "drv/usb/usbHcdOhciLib.h"  


/* defines */

#define MAX_OHCI_HCDS	2

/* locals */

LOCAL GENERIC_HANDLE ohciAttachToken[MAX_OHCI_HCDS] = {NULL,NULL};

/*****************************************************************************
*
* usrUsbHcdOhciAttach - attaches a Host Controller to the USB Stack
*
* This function searches the pci bus for an OHCI type
* USB Host Controller.  If it finds one, it attaches it to the already
* initialized USB Stack
*
* RETURNS: OK if sucessful or ERROR if failure
*/


LOCAL STATUS usrUsbHcdOhciAttach ()
    {
    UINT16 status;
    UINT8 busNo;
    UINT8 deviceNo;
    UINT8 funcNo;
    PCI_CFG_HEADER pciCfgHdr;
    UINT8 pciClass    = OHCI_CLASS;
    UINT8 pciSubclass = OHCI_SUBCLASS;
    UINT8 pciPgmIf    = OHCI_PGMIF;
    HCD_EXEC_FUNC execFunc = &usbHcdOhciExec; 
    GENERIC_HANDLE *pToken = &ohciAttachToken;
    int idx;


    /* 
     * Loop through the maximum number of PCI devices 
     * that might be connected to a system in search of 
     * OHCI type USB Controllers. 
     *
     */

    for (idx = 0; idx < PCI_MAX_DEV; idx++)
	{

        if (!usbPciClassFind (pciClass, 
			      pciSubclass, 
			      pciPgmIf, 
			      idx, 
			      &busNo, 
			      &deviceNo, 
			      &funcNo))
            {
            continue;
            }

	printf ("OHCI Controller found.\n");
	printf ("Waiting to attach to USBD...");

        usbPciConfigHeaderGet (busNo, deviceNo, funcNo, &pciCfgHdr);

        /* Attach the OHCI HCD to the USBD. */

        status = usbdHcdAttach (execFunc, &pciCfgHdr, pToken);

	*pToken += sizeof (GENERIC_HANDLE);

        if (status == OK)
	    {
	    printf ("Done.\n");
	    }
        else
            {
	    printf ("Error attaching host controller # %d.\n", idx);
	    return ERROR;
            }
	}
    return OK; 
    
    }
