/* usrUsbHcdUhciInit.c - Initialization of a UHCI Host Controller Driver */

/* Copyright 1999-2000 Wind River Systems, Inc. */

/*
Modification history
--------------------
01a,23aug00,wef	 Created
*/

/*
DESCRIPTION

This configlette initializes a USB Host Controller if present on a system.
It receives a parameter which indicates whether an OHCI or a UHCI controller
is to be intialized.  The initialization process includes bringing up the 
chip and also "attaches" the chip to the previously initilazed USB stack. 
The stack is initialized in usrUsbInit.c

*/


/* includes */

#include "usb/usbPciLib.h"
#include "usb/usbdLib.h"
#include "drv/usb/usbUhci.h"		
#include "drv/usb/usbHcdUhciLib.h"  


LOCAL GENERIC_HANDLE uhciAttachToken = NULL;




/* defines */

/* locals */


/*****************************************************************************
*
* usrUsbHcdUhciAttach - attaches a Host Controller to the USB Stack
*
* This functions searches the pci bus for either a OHCI or UHCI type
* USB Host Controller.  If it finds one, it attaches it to the already
* initialized USB Stack
*
* RETURNS: OK if sucessful or ERROR if failure
*/


LOCAL STATUS usrUsbHcdUhciAttach ()
    {

    UINT16 ret;
    UINT8 busNo;
    UINT8 deviceNo;
    UINT8 funcNo;
    PCI_CFG_HEADER pciCfgHdr;
    UINT8 pciClass    = UHCI_CLASS;
    UINT8 pciSubclass = UHCI_SUBCLASS;
    UINT8 pciPgmIf    = UHCI_PGMIF;
    HCD_EXEC_FUNC execFunc = &usbHcdUhciExec; 
    GENERIC_HANDLE *pToken = &uhciAttachToken;
    
    if (*pToken == NULL)
        {
        if (!usbPciClassFind (pciClass, pciSubclass, pciPgmIf, 0, 
	    &busNo, &deviceNo, &funcNo))
            {
            printf ("No UHCI host controller found.\n");
            return ERROR;
            }

        printf ("UHCI Controller found.\n"); 
	printf ("Waiting to attach...");


        usbPciConfigHeaderGet (busNo, deviceNo, funcNo, &pciCfgHdr);

        /* Attach the UHCI HCD to the USBD. */

        ret = usbdHcdAttach (execFunc, &pciCfgHdr, pToken);


        if (ret == OK)
	   {
           printf ("Done, AttachToken = 0x%x\n", (UINT) *pToken);
	   }
        else
           {
           printf ("USB HCD Attached FAILED!\n");
           return ERROR;
           }
        }
    else 
        {
        printf("UHCI Already Attached\n");
        }

    return OK; 
    
    }
