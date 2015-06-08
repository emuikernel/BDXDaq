/* usrUsbCbiUfiDevInit.c - USB Mass Storage CBI driver initialization */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
Modification history
--------------------
01e,20dec01,wef  declare usrFdiskPartRead.
01d,07dec01,wef  Fixed more warnings.
01b,13nov01,wef  Removed warnings, added CBIO layer for dosFs2.
01a,10dec00,wef  Created
*/
 
/*
DESCRIPTION
 
This configlette initializes the USB Mass Storage Control / Bulk / Interrupt
driver and places it in the driver table.  On boot, it can be refered to by 
the name given specified by CBI_DRIVE_NAME.  This assumes the USB host stack has 
already been initialized and has a host controller driver attached.
 
*/

/* includes */

#include "dosFsLib.h"
#include "dcacheCbio.h"
#include "dpartCbio.h"
#include "usb/usbdLib.h"
#include "usb/usbQueueLib.h"
#include "drv/usb/usbCbiUfiDevLib.h"

#ifdef _WRS_VXWORKS_5_X
#include "usrFdiskPartLib.h"
#endif

/* defines */

#define VX_UNBREAKABLE	0x0002			/* No debuging into this task */


/* locals */

LOCAL QUEUE_HANDLE 	cbiCallbackQueue;
LOCAL USB_MESSAGE  	cbiDeviceStatus;

LOCAL USBD_NODE_ID	ufiNodeId;		/* Store for nodeId of the UFI device */
LOCAL BLK_DEV		* pUfiBlkDev = NULL;	/* Store for drive structure */
LOCAL BOOL		ufiConnected = FALSE;	/* True if a UFI device exists        */
LOCAL BOOL		ufiDevInUse  = FALSE;	/* True while IO operations are       */
						/* operations are being performed     */ 

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
* ufiMountDrive - mounts a drive to the DOSFS.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS ufiMountDrive
    (
    USBD_NODE_ID attachCode		/* attach code */
    )

    {
    CBIO_DEV_ID cbio;

    /* Create the block device with in the driver */

    pUfiBlkDev = (BLK_DEV *) usbCbiUfiBlkDevCreate (ufiNodeId);

    if (pUfiBlkDev == NULL)
	{
	printf ("usbCbiUfiBlkDevCreate() returned ERROR\n");
	return ERROR;
	}

    /* optional dcache */

    cbio = dcacheDevCreate ((CBIO_DEV_ID) pUfiBlkDev, 0, 0, "usbCbiCache");

    if( NULL == cbio )
        {
        /* insufficient memory, will avoid the cache */
 
        printf ("WARNING: Failed to create disk cache\n");
 
        }

    /* Mount the drive to DOSFS */

    if (dosFsDevCreate (CBI_DRIVE_NAME, cbio, 0x20, NONE) == ERROR)
	{
	printf ("dosFsDevCreate () returned ERROR\n");
	return ERROR;
	}

    return OK;
    }	



/*************************************************************************
*
* ufiAttachCallback - user attach callback for USB UFI class driver.
*
* RETURNS: Nothing
*/

LOCAL VOID ufiAttachCallback
    (
    pVOID arg,			    /* caller-defined argument */
    USBD_NODE_ID nodeId,	    /* pointer to UFI Device */
    UINT16 attachCode		    /* attach code */
    )

    {

    usbQueuePut (cbiCallbackQueue,
		 0,				/* msg */
		 attachCode,			/* wParam */
		 (UINT32) nodeId,		/* lParam */
		 5000);

    }

/***************************************************************************
*
* ufiClientThread- Handle control of drives being plugged / unplugged
*
* This function controls what happens when a new drive gets plugged in
* or when an existing drive gets removed.
*
* RETURNS: Nothing
*/
 
LOCAL VOID ufiClientThread(void)
    {
#if 0
    DEV_HDR *hdr;
#endif

    while (1)
        {

        usbQueueGet (cbiCallbackQueue, &cbiDeviceStatus, OSS_BLOCK);

	/* If attached. Only one device is supported at a time */

	if ((cbiDeviceStatus.wParam == USB_UFI_ATTACH) && (ufiConnected != TRUE))
	    {
	    ufiConnected = TRUE;		/* UFI device detected*/

	    /* Store nodeId of UFI device */

	    ufiNodeId    = (USBD_NODE_ID) cbiDeviceStatus.lParam;

	    /* Lock the device for protection */

	    if (usbCbiUfiDevLock (ufiNodeId) != OK)
		printf ("usbCbiUfiDevLock() returned ERROR\n");

	    /* Mount the drive to the DOS file system */

	    if (ufiMountDrive(ufiNodeId) != OK)
		printf ("ufiMountDrive () returned ERROR\n");

	    printf ("UFI Device Installed as %s\n", CBI_DRIVE_NAME);

	    }
	else
	    {
	    /* Ufi device removed */

	    if (cbiDeviceStatus.lParam == (UINT32) ufiNodeId)    
		{ 

		ufiConnected = FALSE;   /* Declare that device doesn't exist */

		if ((ufiDevInUse == FALSE) && (pUfiBlkDev != NULL))
		    {
		
		    /* 
		     * Remove the dosFs handles if the device is not
		     * being used 
		     */

#if 0
/* 
 * This is the ideal way to handle this, but 
 * iosDevDelete () is not supported on block 
 * devices.
 */
		    if ( (hdr = iosDevFind (CBI_DRIVE_NAME, NULL)) != NULL )
			iosDevDelete (hdr);
#endif 

		    /* 
		     * Unlock the UFI device structure, so that it 
		     * gets destroyed 
		     */

		    if (usbCbiUfiDevUnlock ((USBD_NODE_ID) cbiDeviceStatus.lParam)
					 != OK)
			printf ("usbCbiUfiDevUnlock() returned ERROR\n");

		    printf ("%s removed and uninstalled from FS\n", CBI_DRIVE_NAME);

		    /* Mark BlkDev structure as dead */

		    pUfiBlkDev = NULL;
		    }
		}
	    }
	}
    }



/***************************************************************************
*
* ufiClntRegister- Mass Storage Drive Registration
*
* This function registers a CBI - UFI drive with the USBD.  In addition, it
* also spawns a task to handle plugging / unplugging activity.
*
* RETURNS: Nothing
*/
 
LOCAL VOID ufiClntRegister (void)
    {
   
    int taskId;

    /*  This queue is used to pass status parameters to the task spawned below */

    if (usbQueueCreate (128, &cbiCallbackQueue)!=OK)
	printf("callback queue creation error\n ");

    /* Spawn a task to manage drive removal and insertion */

    if((taskId = taskSpawn ("tUfiClnt", 
			    5, 
			    VX_UNBREAKABLE,
			    20000, 
			    (FUNCPTR) ufiClientThread, 
			    0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )) ==ERROR)
	printf(" TaskSpawn Error...!\n");
  
    /* Register for dynamic attach callback */
 
    if (usbCbiUfiDynamicAttachRegister (ufiAttachCallback, (pVOID)NULL) != OK)
	printf ("usbCbiUfiDynamicAttachRegister() returned ERROR\n");

    }




/*************************************************************************
*
* usrUsbCbiUfiDevInit - initializes USB UFI Mass storage driver.
*
* RETURNS: Nothing
*/
 
void usrUsbCbiUfiDevInit (void)
 
    {

    /* Initialize the UFI class driver */
 
    if (usbCbiUfiDevInit () == OK)
	printf ("usbCbiUfiDevInit() returned OK\n");
    else
	printf ("usbCbiUfiDevInit() returned ERROR\n");
 
    /* Registration routine */

    ufiClntRegister ();

    }

