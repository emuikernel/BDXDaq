/* usrUsbMseInit.c - Initialization of the USB Mouse driver */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
Modification history
--------------------
01e,30jan02,wef fixed DevFind routine.
01d,08dec01,wef  fixing warnings
01c,07dec01,wef  fixed some warnings.
01b,02feb01,wef  Added ios functionality - allows for open, close, read,
                 ioctl, etc access to the printer.
01a,23aug00,wef	 Created
*/

/*
DESCRIPTION

This configlette initializes the USB mouse driver.  This assumes the
USB host stack has already been initialized and has a host controller
driver attached.   

This configlette demonstrates how a user might integrate a USB class 
driver into the vxWorks file system.  usbMseDevCreate () intalls a USB
mouse and its associated driver functions into the driver table allowing
the mouse to be accesed with standard fopen, close, read, write, etc. type
calls.  

*/


/* includes */

#include "errnoLib.h"
#include "drv/usb/usbMouseLib.h"

/* defines */

#define TX_BUF_NUM	0x10000

#define USB_MSE_MUTEX_TAKE(tmout)		\
    semTake (usbMseMutex, (int) (tmout))

#define USB_MSE_MUTEX_GIVE			\
    semGive (usbMseMutex)

#define USB_MSE_LIST_SEM_TAKE(tmout)		\
    semTake (usbMseListMutex, (int) (tmout))

#define USB_MSE_LIST_SEM_GIVE			\
    semGive (usbMseListMutex)


#define MSE_NAME_LEN_MAX        100

#define USB_MSE_NAME		"/usbMo/"

#define USB_MSE_LOGGING 	TRUE

/* data types */

/* typedefs */
 
typedef struct usb_mse_node
    {
    NODE		node;
    struct usb_mse_dev	* pUsbMseDev;
    } USB_MSE_NODE;

typedef struct usb_mse_dev /* USB_MSE_DEV */
    {
    DEV_HDR             ioDev;
    SIO_CHAN    *       pSioChan;
    UINT16              numOpen;
    UINT32              bufSize;
    UCHAR	*       buff;
    SEL_WAKEUP_LIST     selList;
    USB_MSE_NODE	* pUsbMseNode;
    } USB_MSE_DEV;
 

/* local variables */

LOCAL SEM_ID    usbMseMutex;		/* mutex semaphore */
LOCAL SEM_ID    usbMseListMutex;	/* mutex semaphore to protect list */
LOCAL UINT32	mseCount = 0;
LOCAL LIST      usbMseList;		/* all USB mouses in the system */

LOCAL HID_MSE_BOOT_REPORT * pReportlocal;

LOCAL int usbMseDrvNum = 0;		/* driver number */

/* forward declarations */

LOCAL void	usbMseDrvAttachCallback (void * arg, SIO_CHAN *pChan, 
				   UINT16 attachCode);

LOCAL STATUS	usbMseRptCallback (void *putReportArg, 
				   pHID_MSE_BOOT_REPORT pReport);


LOCAL int	usbMseRead (USB_MSE_DEV * pUsbMseDev,UCHAR *buffer, 
			    UINT32 nBytes);

LOCAL int	usbMseClose (USB_MSE_DEV * pUsbMseDev);

LOCAL int	usbMseWrite (USB_MSE_DEV * pUsbMseDev, UCHAR * buffer,
			     UINT32 nBytes);

LOCAL int	usbMseOpen (USB_MSE_DEV * pUsbMseDev, char * name,
			    int flags, int mode);

LOCAL int 	usbMseIoctl (USB_MSE_DEV * pUsbMseDev, int request, void * arg);

LOCAL STATUS 	usbMseDevFind (SIO_CHAN *pChan, USB_MSE_DEV ** ppUsbMseDev);

LOCAL STATUS	usbMseDevDelete (USB_MSE_DEV * pUsbMseDev);


/*******************************************************************************
*
* usbMseDevCreate - create a VxWorks device for an USB mouse
*
* This routine creates a device on a specified serial channel.  Each channel
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device "/ /0", the proper call would be:
* .CS
*     usbMseDevCreate ("/usbMo/0", pSioChan);
* .CE
* Where pSioChan is the address of the underlying SIO_CHAN serial channel
* descriptor (defined in sioLib.h).
* This routine is typically called by the USB mouse driver, when it detects 
* an insertion of a USB mouse.
*
* RETURNS: OK, or ERROR if the driver is not installed, or the
* device already exists, or failed to allocate memory.
*/

STATUS usbMseDevCreate
    (
    char	* name,         /* name to use for this device      */
    SIO_CHAN	* pSioChan	/* pointer to core driver structure */
    )
    {
    USB_MSE_NODE	* pUsbMseNode = NULL;   /* pointer to device node */
    USB_MSE_DEV		* pUsbMseDev = NULL;     /* pointer to USB device */


    /* Create the mutex semaphores */
 
    usbMseMutex = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
			      SEM_INVERSION_SAFE);
 
    usbMseListMutex = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
				  SEM_INVERSION_SAFE);
 
    /* initialize the linked list */
 
    lstInit (&usbMseList);

    usbMseDrvNum = iosDrvInstall ((FUNCPTR) NULL, 
				  (FUNCPTR) usbMseDevDelete, 
				  (FUNCPTR) usbMseOpen, 
				  (FUNCPTR) usbMseClose, 
				  (FUNCPTR) usbMseRead, 
				  (FUNCPTR) usbMseWrite, 
				  (FUNCPTR) usbMseIoctl);


    if (usbMseDrvNum <= 0)
        {
        errnoSet (S_ioLib_NO_DRIVER);
        return (ERROR);
        }

    if (pSioChan == (SIO_CHAN *) ERROR)
	{
        return (ERROR);
	}

    /* allocate memory for the device */

    if ((pUsbMseDev = (USB_MSE_DEV *) calloc (1, sizeof (USB_MSE_DEV))) == NULL)
        return (ERROR);

    pUsbMseDev->pSioChan = pSioChan;

    /* allocate memory for this node, and populate it */

    pUsbMseNode = (USB_MSE_NODE *) calloc (1, sizeof (USB_MSE_NODE));

    /* record useful information */

    pUsbMseNode->pUsbMseDev = pUsbMseDev;
    pUsbMseDev->pUsbMseNode = pUsbMseNode;

    /* add the node to the list */

    USB_MSE_LIST_SEM_TAKE (WAIT_FOREVER);

    lstAdd (&usbMseList, (NODE *) pUsbMseNode);

    USB_MSE_LIST_SEM_GIVE;
 
    /* allow for select support */

    selWakeupListInit (&pUsbMseDev->selList);

    /* start the device in the only supported mode */

    sioIoctl (pUsbMseDev->pSioChan, SIO_MODE_SET, (void *) SIO_MODE_INT);

    /* add the device to the I/O system */

    return (iosDevAdd (&pUsbMseDev->ioDev, name, usbMseDrvNum));
    }




/*******************************************************************************
*
* usbMseDevDelete - delete a VxWorks device for an USB mouse
*
* This routine deletes a device on a specified serial channel.  
*
* This routine is typically called by the USB mouse driver, when it detects 
* a removal of a USB mouse.
*
* RETURNS: OK, or ERROR if the driver is not installed.
*/

LOCAL STATUS usbMseDevDelete
    (
    USB_MSE_DEV	* pUsbMseDev		/* mouse device to read from */
    )
    {
    int		status = OK;		/* holder for the return value */

    if (usbMseDrvNum <= 0)
        {
        errnoSet (S_ioLib_NO_DRIVER);
        return (ERROR);
        }

    /* remove the device from the I/O system */

    iosDevDelete (&pUsbMseDev->ioDev);

    /* remove from list */

    USB_MSE_LIST_SEM_TAKE (WAIT_FOREVER);
    lstDelete (&usbMseList, (NODE *) pUsbMseDev->pUsbMseNode);
    USB_MSE_LIST_SEM_GIVE;

    /* free memory for the device */

    free (pUsbMseDev->pUsbMseNode);
    free (pUsbMseDev);

    return (status);
    }


/*************************************************************************
*
* usbMseDevFind - find the USB_MSE_DEV device for an SIO channel
*
* RETURNS: OK, or ERROR
*/
 
LOCAL STATUS usbMseDevFind
    (
    SIO_CHAN *		pChan,          /* pointer to affected SIO_CHAN */
    USB_MSE_DEV **	ppUsbMseDev	/* pointer to an USB_MSE_DEV */
    )
    {
    USB_MSE_NODE *      pUsbMseNode = NULL;     /* pointer to USB device node */
    USB_MSE_DEV *	pTempDev;		/* pointer to an USB_MSE_DEV */

    if (pChan == NULL)
        return (ERROR);
 
    /* protect it from other module's routines */
 
    USB_MSE_LIST_SEM_TAKE (WAIT_FOREVER);
 
    /* loop through all the devices */
 
    for (pUsbMseNode = (USB_MSE_NODE *) lstFirst (&usbMseList);
         pUsbMseNode != NULL;
         pUsbMseNode = (USB_MSE_NODE *) lstNext ((NODE *) pUsbMseNode))
        {
        pTempDev = pUsbMseNode->pUsbMseDev;

	/* return as soon as you find it */
	if (pTempDev->pSioChan == pChan)
	    {
	    * (UINT32 *) ppUsbMseDev = (UINT32) pTempDev;
	    USB_MSE_LIST_SEM_GIVE;
	    return (OK);
	    }
        }

    USB_MSE_LIST_SEM_GIVE;

    return (ERROR);
    }

/*************************************************************************
*
* usbMseDrvAttachCallback - receives attach callbacks from mouse SIO driver
*
* RETURNS: N/A
*/
LOCAL void usbMseDrvAttachCallback
    (
    void * arg,			/* caller-defined argument */
    SIO_CHAN *pChan,		/* pointer to affected SIO_CHAN */
    UINT16 attachCode		/* defined as USB_KBD_xxxx */
    )
    {
    USB_MSE_DEV * pUsbMseDev = NULL;		/* mouse device */
    char	mseName [MSE_NAME_LEN_MAX];	/* mouse name */

    if (attachCode == USB_MSE_ATTACH)
	{
	if (usbMouseSioChanLock (pChan) != OK)
	    {
	    printf("usbMouseSioChanLock () returned ERROR\n");
	    }
	else
	    {
	    sprintf (mseName, "%s%d", USB_MSE_NAME, mseCount);
	    if (usbMseDevCreate (mseName, pChan) != OK)
		{
		printf("usbMseDevCreate() returned ERROR\n");
		}

	    /* now we can increment the counter */
	    mseCount++;

	    if (usbMseDevFind (pChan, &pUsbMseDev) != OK)
		{
		printf("usbMseDevFind() returned ERROR\n");
		return;
		}

	    if ((*pChan->pDrvFuncs->callbackInstall) 				
				(pChan, 
				SIO_CALLBACK_PUT_MOUSE_REPORT,
				(STATUS  (*) (void *, ...)) usbMseRptCallback, 
				NULL) 
				!= OK)
		{
		printf("usbMseRptCallback () failed to install\n");
		}
	    printf("USB Mouse attached as %s\n", mseName);
	    }
	}
    else if (attachCode == USB_MSE_REMOVE)
	{
	/* find the related device */

	if (usbMseDevFind (pChan, &pUsbMseDev) != OK)
	    {
	    printf ("usbMseDevFind could not find channel 0x%d", (UINT) pChan);
	    return;
	    }	

	/* delete the device */

	usbMseDevDelete (pUsbMseDev);

	if (usbMouseSioChanUnlock (pChan) != OK)
	    {
	    printf("usbMouseSioChanUnlock () returned ERROR\n");
	    return;
	    }
        printf ("USB Mouse %s removed\n", mseName);

	}
    }

/*******************************************************************************
*
* usbMseOpen - open a usbMseDrv serial device.
*
* Increments a counter that holds the number of open paths to device. 
*/

LOCAL int usbMseOpen
    (
    USB_MSE_DEV	* pUsbMseDev,  /* mouse device to read from */
    char     *	name,		/* device name */
    int		flags,		/* flags */
    int        	mode		/* mode selected */
    )
    {

    pUsbMseDev->numOpen++;  /* increment number of open paths */
    
    sioIoctl (pUsbMseDev->pSioChan, SIO_OPEN, NULL);

    return ((int) pUsbMseDev);
    }

/*******************************************************************************
*
* usbMseDrvUnInit - shuts down an I/O USB mouse driver
*
*
* This is supplied to for the user, but it should be noted that iosDrvRemove()
* may cause unpredictable results.
*
*/

STATUS usbMseDrvUnInit (void)
    {
    if (!usbMseDrvNum)
	return (OK);

    /* remove the driver */

    if (iosDrvRemove (usbMseDrvNum, TRUE) != OK)
	{
	printf("iosDrvRemove () returned ERROR\n");
	return (ERROR);
	}

    /* HELP */
    usbMseDrvNum = 0;

    /* delete the mutex semaphores */
 
    if (semDelete (usbMseMutex) == ERROR)
	{
	printf("semDelete (usbMseMutex) returned ERROR\n");
	return (ERROR);
	}

    if (semDelete (usbMseListMutex) == ERROR)
	{
	printf("semDelete (usbMseListMutex) returned ERROR\n");
	return (ERROR);
	}

    /* unregister */
 
    if (usbMouseDynamicAttachUnRegister (usbMseDrvAttachCallback, 
					 NULL) 
					!= OK)
	{
	printf("usbMouseDynamicAttachUnRegister () returned ERROR\n");
	return (ERROR);
	}
 
    /* shutdown */

    if (usbMouseDevShutdown () != OK)
	{
	printf("usbMouseDynamicAttachUnRegister () returned ERROR\n");
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* usbMseClose - close a usbMseDrv serial device.
*
* Decrements the counter of open paths to device and alerts the driver 
* with an ioctl call when the count reaches zero. This scheme is used to
* implement the HUPCL(hang up on last close).      
*/

LOCAL int usbMseClose
    (
    USB_MSE_DEV	* pUsbMseDev          /* mouse device to read from */
    )
    {

    /* if there are no open channels */

    if (!(--pUsbMseDev->numOpen))
	{
	sioIoctl (pUsbMseDev->pSioChan, SIO_HUP, NULL);
	}

    return ((int) pUsbMseDev);
    }

/*******************************************************************************
*
* usbMseIoctl - issue device control commands
*
* This routine issues device control commands to an USB mouse device.
*
* RETURNS: depends on the function invoked.
*/

LOCAL int usbMseIoctl
    (
    USB_MSE_DEV	* pUsbMseDev,	/* mouse device to read from */
    int		request,	/* request code */
    void *	arg		/* some argument */
    )
    {

    return (sioIoctl (pUsbMseDev->pSioChan, request, arg));
    }

/*******************************************************************************
*
* usbMseRead - read from the USB mouse
*
* This routines is a no-op for a read from the USB mouse.
*
* RETURNS: OK, always.
*/

LOCAL int usbMseRead
    (
    USB_MSE_DEV	* pUsbMseDev,           /* mouse device to read from */
    UCHAR *buffer, 
    UINT32 nBytes    
    )
    {
    memcpy(buffer, pReportlocal, nBytes);

/*  This shows the mouse coordinate */

#if USB_MSE_LOGGING

    logMsg("\rx:%d  y:%d  button:%d\n", 
	   pReportlocal->xDisplacement, 
	   pReportlocal->xDisplacement, 
	   pReportlocal->buttonState ,
	   0,0,0);

#endif

    return (sizeof(HID_MSE_BOOT_REPORT));
    }

/*******************************************************************************
*
* usbMseWrite - write to the USB mouse
*
* 
* USB mice don't like to be written to.  The don't support this feature.
*
* RETURNS: ENOSYS
*/

int usbMseWrite
    (
    USB_MSE_DEV	* pUsbMseDev,		/* mouse device to read from */
    UCHAR	* buffer,		/* buffer of data to write  */
    UINT32	  nBytes		/* number of bytes in buffer */
    )
    {

    return (ENOSYS);
    }

/*************************************************************************
*
* usbMseRptCallback - invoked when reports received from mouse
*
* The mouse returns 3 bytes.  The first two describe its x, y displacement 
* from its previous position.  The third describes the button state 
* of its 3 buttons.
*
* In the third byte the first 3 bits represent the state of the buttons as 
* follows:  bit 1 = the middle button, bit 2 = the left button and bit 3 =
* the right button.
* 
* These three bytes are containied in a HID_MSE_BOOT_REPORT structure.  
* this fucntion copys these 3 bytes into a local copy that can be accessed
* with the mouse read routine.  
*
*
* RETURNS: OK
*/

LOCAL STATUS usbMseRptCallback
    (
    void *putReportArg,
    pHID_MSE_BOOT_REPORT pReport
    )

    {

    pReportlocal = pReport;

    return OK;
    }

/*****************************************************************************
*
* usrUsbMseInit - initialize the USB mouse driver
*
* This function initializes the USB mouse driver and registers for attach 
* callbacks.  
*
* RETURNS: Nothing 
*/

void usrUsbMseInit (void) 
    {

    /* Check if driver already installed */

    if (usbMseDrvNum > 0)
	{
	printf ("Mouse already initilaized.\n");
	}


    if (usbMouseDevInit () == OK)
	{

	pReportlocal = OSS_MALLOC (sizeof (HID_MSE_BOOT_REPORT));

	if (pReportlocal == NULL)
	    printf ("usrUsbMseInit () could not allocate memory\n");

        printf ("usbMouseDevInit() returned OK\n");
	
	if (usbMouseDynamicAttachRegister (usbMseDrvAttachCallback, 
					   (void *) NULL) 
					  != OK)
	    {
	    printf ("usbMouseDynamicAttachRegister() returned ERROR\n");
	    }
	}
    else
        printf ("usbMouseDevInit() returned ERROR\n");

    }
