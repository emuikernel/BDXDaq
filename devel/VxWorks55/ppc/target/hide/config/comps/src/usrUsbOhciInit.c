/* usrUsbOhciInit.c - USB Configlette */

/* Copyright 1999-2000 Wind River Systems, Inc. */

/*
Modification history
--------------------
01a,26jan00,wef  First.
*/

/*
DESCRIPTION
Containes routines for inializing an OHCI controller and attaching it to the 
USB host stack.  
*/


#include        "drv/pci/pciConfigLib.h"

#include        "usb/usbdLib.h"
# include       "usb/usbPlatform.h"
# include       "usb/usbHandleLib.h"
# include       "drv/usb/usbOhci.h"
# include       "usb/pciConstants.h"
# include       "drv/usb/usbHcd.h"
# include       "drv/usb/usbHcdOhciLib.h"

#include        "usb/usbPciLib.h"



/*************************************************************************
*
* usrUsbOhciInit - Attach OHCI controller
*
* RETURNS:  nothing
*/

void usrUsbOhciInit(void)
    {

/*  
    It is up the the user to create a function here that attaches a host 
    controller appropriatly for the context of the application.  Refer to 
    the code for usbTool.  It presents a good example of the host controller
    attachment process
*/

    }