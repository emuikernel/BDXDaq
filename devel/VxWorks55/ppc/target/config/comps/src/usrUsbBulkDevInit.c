/* usrUsbBulkDevInit.c - USB Mass Storage CBI driver initialization */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
Modification history
--------------------
01e,20dec01,wef  declare usrFdiskPartRead.
01d,07dec01,wef  Fixed more warnings. 
01c,13nov01,wef  Removed warnings, added CBIO layer for dosFs2.
01b,12apr01,wef  added support for multiple devices, added new param to 
		 usbBulkBlkDevCreate () for read/ write type.
01a,10dec00,wef  Created
*/
 
/*
DESCRIPTION
 
This configlette initializes the USB Mass Storage Control / Bulk / Interrupt
driver and places it in the driver table.  On boot, it can be refered to by 
the name given specified by the BULK_DRIVE_NAME configuration parameter.  
This configlette assumes the USB host stack has already been initialized and 
has a host controller driver attached.
 
*/

/* includes */

#include "dosFsLib.h"
#include "dcacheCbio.h"
#include "dpartCbio.h"
#include "usb/usbdLib.h"
#include "usb/usbQueueLib.h"
#include "drv/usb/usbBulkDevLib.h"

#ifdef _WRS_VXWORKS_5_X
#include "usrFdiskPartLib.h"
#endif

/* defines */

#define VX_UNBREAKABLE	0x0002			/* No debuging into this task */

/* locals */


LOCAL QUEUE_HANDLE 	bulkCallbackQueue;

LOCAL USB_MESSAGE  	bulkDeviceStatus;

LOCAL BLK_DEV		* pBulkBlkDev = NULL;	/* Store for drive structure */

USBD_NODE_ID        bulkNodeId;


/* externals */

#ifndef _WRS_VXWORKS_5_X
IMPORT STATUS usrFdiskPartRead
    (
    CBIO_DEV_ID cDev,            /* device from which to read blocks */
    PART_TABLE_ENTRY *pPartTab,  /* table where to fill results */
    int nPart                    /* # of entries in <pPartTable> */
    );
#endif

/*************************************************************************
*
* bulkMountDrive - mounts a drive to the DOSFS.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS bulkMountDrive
    (
    USBD_NODE_ID attachCode             /* attach code */
    )

    {
    CBIO_DEV_ID cbio, masterCbio;

    /* Create the block device with in the driver */

    pBulkBlkDev = (BLK_DEV *) usbBulkBlkDevCreate (bulkNodeId, 
						 0, 
						 0, 
						 USB_SCSI_FLAG_READ_WRITE10);

    if (pBulkBlkDev == NULL)
	{
	logMsg ("usbBulkBlkDevCreate() returned ERROR\n", 0, 0, 0, 0, 0, 0);
	return ERROR;
	}


    /* optional dcache */

    cbio = dcacheDevCreate ((CBIO_DEV_ID) pBulkBlkDev, 0, 0, "usbBulkCache");

    if( NULL == cbio )
        {
        /* insufficient memory, will avoid the cache */

        printf ("WARNING: Failed to create disk cache\n");

        }

    masterCbio = dpartDevCreate (cbio, 1, usrFdiskPartRead);

    if( NULL == masterCbio )
        {
        printf ("Error creating partition manager\n");
        return ERROR;
        }

    /* Mount the drive to DOSFS */

    if (dosFsDevCreate (BULK_DRIVE_NAME, 
			dpartPartGet(masterCbio, 0), 
			0x20,
			NONE) 
		== ERROR)
            {
            printf ("Error creating dosFs device\n");
            return ERROR;
            }

    return OK;
    }	



/*************************************************************************
*
* bulkAttachCallback - user attach callback for USB BULK class driver.
*
* RETURNS: Nothing
*/

LOCAL VOID bulkAttachCallback
    (
    pVOID arg,			    /* caller-defined argument */
    USBD_NODE_ID nodeId,	    /* pointer to BULK Device */
    UINT16 attachCode		    /* attach code */
    )

    {

    usbQueuePut (bulkCallbackQueue,
		 0,				/* msg */
		 attachCode,			/* wParam */
		 (UINT32) nodeId,		/* lParam */
		 5000);

    }

/***************************************************************************
*
* bulkClientThread- Handle control of drives being plugged / unplugged
*
* This function controls what happens when a new drive gets plugged in
* or when an existing drive gets removed.
*
* RETURNS: Nothing
*/
 
LOCAL VOID bulkClientThread(void)
    {
#if 0
    DEV_HDR *hdr;
#endif

    while (1)
        {

	/* 
	 * the queue parameters will be:
	 * lParam = nodeId
	 * wParam = attach code
	 */

        usbQueueGet (bulkCallbackQueue, &bulkDeviceStatus, OSS_BLOCK);

	/* If attached. Only one device is supported at a time */

	if (bulkDeviceStatus.wParam == USB_BULK_ATTACH)
	    {


	    bulkNodeId = (USBD_NODE_ID) bulkDeviceStatus.lParam;		

	    /* Lock the device for protection */

	    if (usbBulkDevLock (bulkNodeId) != OK)
		printf ("usbBulkDevLock() returned ERROR\n");

	    /* Mount the drive to the DOS file system */

	    if (bulkMountDrive(bulkNodeId) != OK)
		printf ("bulkMountDrive () returned ERROR\n");

            printf ("Bulk Device Installed as %s\n", BULK_DRIVE_NAME);

	    }

	/* Device was removed */

	else if (bulkDeviceStatus.wParam == USB_BULK_REMOVE)

	    {

	    bulkNodeId = (USBD_NODE_ID) bulkDeviceStatus.lParam;		

	    /* Remove the dosFs handles if the device is not being used */
#if 0

/*
 * This is the ideal way to handle this, but
 * iosDevDelete () is not supported on block
 * devices.
 */

            if ( (hdr = iosDevFind (BULK_DRIVE_NAME, NULL)) != NULL )
                iosDevDelete (hdr);
#endif

	    /* Unlock the BULK device structure, so that it gets destroyed */

	    if (usbBulkDevUnlock (bulkNodeId) != OK)
		{
		printf ("usbBulkDevUnlock() returned ERROR\n");
		return;
		}

            printf ("%s removed and uninstalled from FS\n", BULK_DRIVE_NAME);

	    /* Mark bulk node structure as dead */

	    bulkNodeId = NULL;
	    }
	}
    }



/*************************************************************************
*
* usrUsbBulkDevInit - initializes USB BULK Mass storage driver.
*
* This function initializes the BULK driver and registers a CBI - BULK 
* drive with the USBD.  In addition, it also spawns a task to handle 
* plugging / unplugging activity.
*
* RETURNS: Nothing
*/
 
void usrUsbBulkDevInit (void)
 
    {
    int taskId;

    /* Initialize the BULK class driver */
 
    if (usbBulkDevInit () == OK)
	logMsg ("usbBulkDevInit() returned OK\n", 0, 0, 0, 0, 0, 0);
    else
	logMsg ("usbBulkDevInit() returned ERROR\n", 0, 0, 0, 0, 0, 0);
 
    /*  This queue is used to pass status parameters to the task spawned below */

    if (usbQueueCreate (128, &bulkCallbackQueue)!=OK)
        {
	logMsg ("callback queue creation error\n ", 0, 0, 0, 0, 0, 0);
	return;
	}

    /* Spawn a task to manage drive removal and insertion */

    if((taskId = taskSpawn ("tBulkClnt", 
			    5, 
			    0,
			    20000, 
			    (FUNCPTR) bulkClientThread, 
			    0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )) ==ERROR)
	{
	logMsg (" TaskSpawn Error...!\n", 0, 0, 0, 0, 0, 0);
	return;
	}
  
    /* Register for dynamic attach callback */
 
    if (usbBulkDynamicAttachRegister (bulkAttachCallback, (pVOID)NULL) != OK)
	logMsg ("usbBulkDynamicAttachRegister() returned ERROR\n", 0, 0, 0, 0, 0, 0);

    }

