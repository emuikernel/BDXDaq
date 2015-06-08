/* usrUsbInit.c - Initialization of the USB Host stack */

/* Copyright 1999-2000 Wind River Systems, Inc. */

/*
Modification history
--------------------
01b,27feb00,wef	 removed unecessary printfs, fixed return codes 
01a,23aug00,wef	 Created
*/

/*
DESCRIPTION

This configlette initializes a USB Host Stack.  This process is done at
boot and is usually followed by the attachment of a USB host controller.
The configlette usrUsbHcdInit.c handles the host controller attachement.

*/


/* includes */

#include "usb/usbdLib.h"

/* defines */

/* locals */

LOCAL BOOL usbdInitByKernel = FALSE;


/*****************************************************************************
*
* usbInit - initialize the USB stack
*
* This function initializes the USB Stack 
*
* RETURNS: OK if sucessful or ERROR if failure
*/

STATUS usbInit (void) 
    {
    UINT16 verStatus;
    UINT16 usbdVersion;
    char   usbdMfg [USBD_NAME_LEN+1];


    if (!usbdInitByKernel)
        {
        if (usbdInitialize() != OK)
       	    {
            printf(" Failed to initialize USBD\n");
            return ERROR;
            }
        else 
            {
            usbdInitByKernel = TRUE;
            printf("USBD Initialized.\n");
            }
        }
    else 
        {
        printf("USDB Already Initialized\n");
        }
    
    if ((verStatus = usbdVersionGet (&usbdVersion, usbdMfg)) != OK)
        {
        printf ("usbdVersionGet() failed..returned %d\n", verStatus);
	return ERROR;
        }
    return OK;
    }

