/* usrUsbPegasusEndInit.c - Initialization of the USB END driver */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
Modification history
--------------------
01f,25apr02,wef  add support for Micro Connector device.
01e,05mar02,wef  cleaned up printf's.
01d,17dec01,wef  merge from veloce
01c,04dec01,wef  Pulled code from src/test/usb/usbPegasusTest.c here and cleaned
		 up.
01b,10jun01,wef  moved end attach functionality to src/test/usb/usbPegasusTest.c
01a,23aug00,wef	 Created
*/

/*
DESCRIPTION

This configlette initializes the USB END pegasus driver.  This assumes the
USB host stack has already been initialized and has a host controller
driver attached.   

*/

/* includes */

#include "ipProto.h"
#include "usb/ossLib.h"
#include "usb/usbQueueLib.h"
#include "drv/usb/usbPegasusEnd.h"


/* defines */

/* externals */

IMPORT END_OBJ *       usbPegasusEndLoad (char * initString);

/* locals */

LOCAL QUEUE_HANDLE 	pegasusCallbackQueue;
LOCAL USB_MESSAGE  	pegasusDeviceStatus;
LOCAL END_OBJ*   	pEnd;


/******************************************************************************
*
* sysUsbEndPegasusLoad - load (create) USB END device
*
* This routine loads the usb end device with initial parameters specified by
* values given in the BSP configuration files (config.h).
*
* RETURNS: pointer to END object or ERROR.
*
*/
 
END_OBJ * sysUsbPegasusEndLoad
    (
    char * pParamStr,   /* ptr to initialization parameter string */
    void * unused       /* unused optional argument */
    )
    {
    /*
     * The usbEnd driver END_LOAD_STRING should be:
     * "<vendorId>:<productId>:<noofRxbuffers>:<noofIRPs>"
     */

    char paramStr [END_INIT_STR_MAX];   /* from end.h */
    static char usbEndParam[] = "%d:%d:%d:%d";			


    if (strlen (pParamStr) == 0)
        {

        /* 
         * muxDevLoad() calls us twice.  If the string is
         * zero length, then this is the first time through
         * this routine, so we just return the device name.
         */

	pEnd = usbPegasusEndLoad(pParamStr);
        }

    else
	{

	/*
	 * On the second pass though here, we actually create 
	 * the initialization parameter string on the fly.   
	 * Note that we will be handed our unit number on the 
	 * second pass through and we need to preserve that information.
	 * So we use the unit number handed from the input string.
	 */

	sprintf (paramStr, "%c:\n", * pParamStr);

	sprintf (usbEndParam,
		 "%d:%d:%d:%d\n", 
		 SOHOWARE_VENDOR_ID, 
		 SOHOWARE_PRODUCT_ID, 
		 PEGASUS_NO_IN_BFRS, 
		 PEGASUS_NO_OUT_IRPS);

	strcat(paramStr,usbEndParam);

	if ((pEnd = usbPegasusEndLoad (paramStr)) == (END_OBJ *)NULL)
	    {
	    printf ("Error: usb network device failed usbEndLoad routine.\n");
	    }
	}

    return (pEnd);
    }



/******************************************************************************
*
* usbLoadPegasus - load (create) USB END device
*
* This function is taken from usrEndLibInit() and modified suitably to load 
* the end driver when USB Ethernet device is dynamically attached or detached. 
*
* RETURNS: OK or ERROR.
*
*/

STATUS usbLoadPegasus 
    ( 
    int unitNum, 
    USB_PEGASUS_DEV * pDev
    )
    {
    
    END_OBJ*      pCookie = NULL;

    /* Add our default address resolution functions. */

#ifdef INCLUDE_NT_ULIP
    muxAddrResFuncAdd (M2_ifType_ethernet_csmacd, 0x800, ntResolv);
#else
    muxAddrResFuncAdd (M2_ifType_ethernet_csmacd, 0x800, arpresolve);
#endif

    pCookie = (END_OBJ *) muxDevLoad(unitNum, 
				     sysUsbPegasusEndLoad,
				     NULL,
				     1,
				     NULL);

    if (pCookie == NULL)
	{
        printf ("muxDevLoad failed for PEGASUS device \n");
        return(ERROR);	 
	}
    else
	{
        if (muxDevStart(pCookie) == ERROR)
	    {
            printf ("muxDevStart failed for Pegasus device \n");
	    return(ERROR);	
	    }
        }

    return (OK);

    }

/******************************************************************************
*
* usbPegasusEndStart -  make the network device operational
*
* After muxDevStart hass been executed, this then attaches an IP address
* to the USB device and then starts the it.
*
* RETURNS: Nothing
*
*/

void usbPegasusEndStart
    ( 
    int       unitNum,		/* unit number */
    char *    pAddrString,		/* enet address */
    int       netmask			/* netmask */
    )
    {
    M2_INTERFACETBL  endM2Tbl;

    pEnd = endFindByName (PEGASUS_NAME, unitNum);

    if (pEnd == NULL)
	{
	printf ("Could not find %s%d\n", PEGASUS_NAME, unitNum);
	return;
	}

    if (muxIoctl(pEnd, EIOCGMIB2, (caddr_t) &endM2Tbl) == ERROR)
	{
	printf ("Cannot perform EIOCMIB2 ioctl.\n");
	return;
	}
 
    if (ipAttach(unitNum, PEGASUS_NAME) != OK)
	{
	printf ("Failed to attach to device %s", PEGASUS_NAME);
	return;
	}
 
    if (usrNetIfConfig (PEGASUS_NAME, 	
			unitNum, 	
			pAddrString, 	
			PEGASUS_TARGET_NAME, 	
			netmask) 	
		    != OK)
	{
	printf ("Failed to configure %s%d for IP.\n", PEGASUS_NAME, unitNum);
	return;
	}

    printf ("Attached TCP/IP interface to %s unit %d\n", PEGASUS_NAME, unitNum);
    }


/***************************************************************************
*
* usbPegasusAttachCallback - configuration level callback
*
* When a Pegasus device is inserted, this will get called.  It passes the 
* attach message to tUsbPgs task.
*
* RETURNS: nothing
*/
 
VOID usbPegasusAttachCallback
    (
    pVOID arg,                      /* caller-defined argument */
    USB_PEGASUS_DEV * pDev,         /* pointer to Pegasus Device */
    UINT16 attachCode               /* attach code */
    )
 
    {

    usbQueuePut (pegasusCallbackQueue,
		 0,			  	/* msg */
		 attachCode,			/* wParam */
		 (UINT32)pDev,			/* lParam */
		 5000);

    }


/***************************************************************************
*
* pegasusClientThread - handles device insertions / removals.
*
* This routine is the configuration level handler that monitors device 
* insertions and removals.  Upon device connection, it starts the USB 
* network device.
*
* RETURNS: nothing
*/
 
void pegasusClientThread(void)
    {
    int unitNum = 0, index;
    USB_PEGASUS_DEV * pDev;	
    int noOfSupportedDevices = (sizeof (pegasusAdapterList) /
                                 (2 * sizeof (UINT16)));

    while (1)
        {

        usbQueueGet (pegasusCallbackQueue, &pegasusDeviceStatus, OSS_BLOCK);

	pDev = (USB_PEGASUS_DEV *)pegasusDeviceStatus.lParam;

 
	for (index = 0; index < noOfSupportedDevices; index++)
            if (pDev->vendorId == pegasusAdapterList[index][0] &&
                pDev->productId == pegasusAdapterList[index][1])
	        {	
		break;
		} 	

	if (index == noOfSupportedDevices)
	    {
	    printf ("Unsupported Device.\n");
	    continue; 
	    }

	/* Device is connected */

	if (pegasusDeviceStatus.wParam == USB_PEGASUS_ATTACH)
	    {

	    printf ("Loading Pegasus Device\n");

	    if (usbPegasusDevLock ((USBD_NODE_ID) pDev->nodeId) != OK)
		printf ("usbPegasusDevLock() returned ERROR\n");
	    else
		{

		if(usbLoadPegasus(unitNum, pDev) == OK)
		    {

		    /* Attach IP address*/

		    usbPegasusEndStart(unitNum, 
				       PEGASUS_IP_ADDRESS, 
				       PEGASUS_NET_MASK);
		    } 	
		else 
		    printf ("usbLoadPegasus returned ERROR\n");
		}
	    }

	/* Device was removed */

        else if (pegasusDeviceStatus.wParam == USB_PEGASUS_REMOVE)
	    {
	   		
   	    printf ("Pegasus Device Unload in progress\n");

 	    if (usbPegasusDevUnlock ((USBD_NODE_ID) pDev->nodeId) != OK)
		{
		printf ("usbPegasusDevUnlock() returned ERROR\n");
		continue;
		}

            if (routeDelete(PEGASUS_DESTINATION_ADDRESS, 
			    PEGASUS_IP_ADDRESS)
			  !=OK)
		{
		printf (" RouteDeletion failed  \n");
		continue;
		}

	    if (muxDevUnload(PEGASUS_NAME, unitNum)!=OK)
		{
	        printf ("  muxDevUnload failed\n");
		continue;
		}

  	    printf (" Pegasus Device Unloaded sucessfully\n");
	    }
	}

    }


/*****************************************************************************
*
* usrUsbPegasusEndInit - initialize the USB END Pegasus driver
*
* This function initializes the USB END Pegasus driver and spawns a task
* to manage device insertion / removals.
*
* RETURNS: nothing 
*/

void usrUsbPegasusEndInit (void)
    {
    int taskId;

    if (usbPegasusEndInit () == OK)
        printf ("usbPegasusEndInit () returned OK\n");
    else
	{
        printf ("usbPegasusEndInit () returned ERROR\n");
	return;
	}
 
    if (usbQueueCreate (128, &pegasusCallbackQueue)!=OK)
	{
	printf ("pegasusCallbackQueue creation error\n ");
	return;
	}

    if((taskId = taskSpawn ("tUsbPgs", 
			    5, 
			    0,
			    20000, 
			    (FUNCPTR) pegasusClientThread, 
			    0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )) ==ERROR)
	{
	printf ("Error spawning tUsbPgs\n");
	return;
	}

    if (usbPegasusDynamicAttachRegister (usbPegasusAttachCallback, NULL) != OK)
	{
	printf ("usbPegasusDynamicAttachRegister() returned ERROR\n");
	return;
	}

    }
