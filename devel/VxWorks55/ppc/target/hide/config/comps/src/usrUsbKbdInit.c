/* usrUsbKbdInit.c - Initialization of the USB Keyboard driver */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
Modification history
--------------------
01d,30jan02,wef fixed DevFind routine.
01c,08dec01,wef fixed some warnings
01b,02feb01,wef Added ios functionality - allows for open, close, read,
                ioctl, etc access to the printer.
01a,23aug00,wef	Created
*/

/*
DESCRIPTION

This configlette initializes the USB keyboard driver.  This assumes the
USB host stack has already been initialized and has a host controller
driver attached.   

This configlette demonstrates how a user might integrate a USB class 
driver into the vxWorks file system.  usbKbdDevCreate () intalls a USB
keyboard and its associated driver functions into the driver table allowing
the keyboard to be accesed with standard fopen, close, read, write, etc. type
calls.  

There is an included test routine playWithKeyboard () that demonstrates 
using the ios functionality

*/


/* includes */

#include "drv/usb/usbKeyboardLib.h"
#include "usb/usbHid.h"
#include "vxWorks.h"
#include "iv.h"
#include "ioLib.h"
#include "iosLib.h"
#include "tyLib.h"
#include "intLib.h"
#include "errnoLib.h"
#include "sioLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "logLib.h"
#include "selectLib.h"


/* defines */

#define TX_BUF_NUM	0x10000

#define USB_KBD_MUTEX_TAKE(tmout)		\
    semTake (usbKbdMutex, (int) (tmout))

#define USB_KBD_MUTEX_GIVE			\
    semGive (usbKbdMutex)

#define USB_KBD_LIST_SEM_TAKE(tmout)		\
    semTake (usbKbdListMutex, (int) (tmout))

#define USB_KBD_LIST_SEM_GIVE			\
    semGive (usbKbdListMutex)


#define KBD_NAME_LEN_MAX        100
#define USB_KBD_NAME		"/usbKb/"


#define CTRL_Z			    26	/* ASCI code for ^Z */


/* data types */

/* typedefs */
 
typedef struct usb_kbd_node
    {
    NODE		node;
    struct usb_kbd_dev	* pUsbKbdDev;
    } USB_KBD_NODE;

typedef struct usb_kbd_dev /* USB_KBD_DEV */
    {
    DEV_HDR             ioDev;
    SIO_CHAN    *       pSioChan;
    UINT16              numOpen;
    UINT32              bufSize;
    UCHAR	*       buff;
    SEL_WAKEUP_LIST     selList;
    USB_KBD_NODE	* pUsbKbdNode;
    } USB_KBD_DEV;
 

/* local variables */

LOCAL SEM_ID    usbKbdMutex;		/* mutex semaphore */
LOCAL SEM_ID    usbKbdListMutex;	/* mutex semaphore to protect list */
LOCAL UINT32	kbdCount = 0;
LOCAL LIST      usbKbdList;		/* all USB keyboards in the system */

LOCAL int usbKbdDrvNum = 0;		/* driver number */

LOCAL BOOL enterPressed;

/* forward declarations */

LOCAL void	usbKbdDrvAttachCallback (void * arg, SIO_CHAN *pChan, 
				   UINT16 attachCode);

LOCAL int	usbKbdRead (USB_KBD_DEV * pUsbKbdDev,UCHAR *buffer, 
			    UINT32 nBytes);

LOCAL int	usbKbdClose (USB_KBD_DEV * pUsbKbdDev);

LOCAL int	usbKbdWrite (USB_KBD_DEV * pUsbKbdDev, UCHAR * buffer,
			     UINT32 nBytes);

LOCAL int	usbKbdOpen (USB_KBD_DEV * pUsbKbdDev, char * name,
			    int flags, int mode);

LOCAL int 	usbKbdIoctl (USB_KBD_DEV * pUsbKbdDev, int request, void * arg);

LOCAL STATUS 	usbKbdDevFind (SIO_CHAN *pChan, USB_KBD_DEV ** ppUsbKbdDev);

LOCAL STATUS	usbKbdDevDelete (USB_KBD_DEV * pUsbKbdDev);


/*******************************************************************************
*
* usbKbdDevCreate - create a VxWorks device for an USB keyboard
*
* This routine creates a device on a specified serial channel.  Each channel
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device "/ /0", the proper call would be:
* .CS
*     usbKbdDevCreate ("/usbKb/0", pSioChan);
* .CE
* Where pSioChan is the address of the underlying SIO_CHAN serial channel
* descriptor (defined in sioLib.h).
* This routine is typically called by the USB keyboard driver, when it detects 
* an insertion of a USB keyboard.
*
* RETURNS: OK, or ERROR if the driver is not installed, or the
* device already exists, or failed to allocate memory.
*/

STATUS usbKbdDevCreate
    (
    char	* name,         /* name to use for this device      */
    SIO_CHAN	* pSioChan	/* pointer to core driver structure */
    )
    {
    USB_KBD_NODE	* pUsbKbdNode = NULL;   /* pointer to device node */
    USB_KBD_DEV		* pUsbKbdDev = NULL;     /* pointer to USB device */


    /* Create the mutex semaphores */
 
    usbKbdMutex = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
			      SEM_INVERSION_SAFE);
 
    usbKbdListMutex = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
				  SEM_INVERSION_SAFE);
 
    /* initialize the linked list */
 
    lstInit (&usbKbdList);

    /* Install the appropriate functions into the driver table */

    usbKbdDrvNum = iosDrvInstall ((FUNCPTR) NULL, 
				  (FUNCPTR) usbKbdDevDelete, 
				  (FUNCPTR) usbKbdOpen, 
				  (FUNCPTR) usbKbdClose, 
				  (FUNCPTR) usbKbdRead, 
				  (FUNCPTR) usbKbdWrite, 
				  (FUNCPTR) usbKbdIoctl);

    /*  check to see if there was room to install the driver */

    if (usbKbdDrvNum <= 0)
        {
        errnoSet (S_ioLib_NO_DRIVER);
	printf ("There is no more room in the driver table\n");
	return (ERROR);
        }


    if (pSioChan == (SIO_CHAN *) ERROR)
	{
	printf ("pSioChan is ERROR\n");
	return (ERROR);
	}

    /* allocate memory for the device */

    if ((pUsbKbdDev = (USB_KBD_DEV *) calloc (1, sizeof (USB_KBD_DEV))) == NULL)
	{
	printf ("calloc returned NULL - out of memory\n");
	return (ERROR);
	}

    pUsbKbdDev->pSioChan = pSioChan;

    /* allocate memory for this node, and populate it */

    pUsbKbdNode = (USB_KBD_NODE *) calloc (1, sizeof (USB_KBD_NODE));

    /* record useful information */

    pUsbKbdNode->pUsbKbdDev = pUsbKbdDev;
    pUsbKbdDev->pUsbKbdNode = pUsbKbdNode;

    /* add the node to the list */

    USB_KBD_LIST_SEM_TAKE (WAIT_FOREVER);

    lstAdd (&usbKbdList, (NODE *) pUsbKbdNode);

    USB_KBD_LIST_SEM_GIVE;
 
    /* allow for select support */

    selWakeupListInit (&pUsbKbdDev->selList);

    /* start the device in the only supported mode */

    sioIoctl (pUsbKbdDev->pSioChan, SIO_MODE_SET, (void *) SIO_MODE_INT);

    /* add the device to the I/O system */

    return (iosDevAdd (&pUsbKbdDev->ioDev, name, usbKbdDrvNum));
    }




/*******************************************************************************
*
* usbKbdDevDelete - delete a VxWorks device for an USB keyboard
*
* This routine deletes a device on a specified serial channel.  
*
* This routine is typically called by the USB keyboard driver, when it detects 
* a removal of a USB keyboard.
*
* RETURNS: OK, or ERROR if the driver is not installed.
*/

LOCAL STATUS usbKbdDevDelete
    (
    USB_KBD_DEV	* pUsbKbdDev		/* keyboard device to read from */
    )
    {
    int		status = OK;		/* holder for the return value */

    if (usbKbdDrvNum <= 0)
        {
        errnoSet (S_ioLib_NO_DRIVER);
        return (ERROR);
        }

    /* remove the device from the I/O system */

    iosDevDelete (&pUsbKbdDev->ioDev);

    /* remove from list */

    USB_KBD_LIST_SEM_TAKE (WAIT_FOREVER);
    lstDelete (&usbKbdList, (NODE *) pUsbKbdDev->pUsbKbdNode);
    USB_KBD_LIST_SEM_GIVE;

    /* free memory for the device */

    free (pUsbKbdDev->pUsbKbdNode);
    free (pUsbKbdDev);

    return (status);
    }


/*************************************************************************
*
* usbKbdDevFind - find the USB_KBD_DEV device for an SIO channel
*
* RETURNS: OK, or ERROR
*/
 
LOCAL STATUS usbKbdDevFind
    (
    SIO_CHAN *		pChan,          /* pointer to affected SIO_CHAN */
    USB_KBD_DEV **	ppUsbKbdDev	/* pointer to an USB_KBD_DEV */
    )
    {
    USB_KBD_NODE *      pUsbKbdNode = NULL;     /* pointer to USB device node */
    USB_KBD_DEV *	pTempDev;		/* pointer to an USB_KBD_DEV */

    if (pChan == NULL)
        return (ERROR);
 
    /* protect it from other module's routines */
 
    USB_KBD_LIST_SEM_TAKE (WAIT_FOREVER);
 
    /* loop through all the devices */
 
    for (pUsbKbdNode = (USB_KBD_NODE *) lstFirst (&usbKbdList);
         pUsbKbdNode != NULL;
         pUsbKbdNode = (USB_KBD_NODE *) lstNext ((NODE *) pUsbKbdNode))
        {
        pTempDev = pUsbKbdNode->pUsbKbdDev;

	/* return as soon as you find it */
	if (pTempDev->pSioChan == pChan)
	    {
	    * (UINT32 *) ppUsbKbdDev = (UINT32) pTempDev;
	    USB_KBD_LIST_SEM_GIVE;
	    return (OK);
	    }
        }

    USB_KBD_LIST_SEM_GIVE;

    return (ERROR);
    }

/*************************************************************************
*
* usbKbdDrvAttachCallback - receives attach callbacks from keyboard SIO driver
*
* RETURNS: N/A
*/
LOCAL void usbKbdDrvAttachCallback
    (
    void * arg,			/* caller-defined argument */
    SIO_CHAN *pChan,		/* pointer to affected SIO_CHAN */
    UINT16 attachCode		/* defined as USB_KBD_xxxx */
    )
    {
    USB_KBD_DEV * pUsbKbdDev=NULL;		/* keyboard device */
    char	kbdName [KBD_NAME_LEN_MAX];	/* keyboard name */

    /*  A keyboard has been attached. */

    if (attachCode == USB_KBD_ATTACH)
	{

	/*  Lock this particular channel for this particular keyboard */
	
	if (usbKeyboardSioChanLock (pChan) != OK)
	    {
	    printf("usbKeyboardSioChanLock () returned ERROR\n");
	    }
	else
	    {

	    /*  plugs the name /usbKb/x into the kbdName variable */

	    sprintf (kbdName, "%s%d", USB_KBD_NAME, kbdCount);


	    /*  Create the keyboard device, do all the ios stuff (drvInstall, 
	     *  devAdd...) 
	     */

	    if (usbKbdDevCreate (kbdName, pChan) != OK)
		{
		printf("usbKbdDevCreate() returned ERROR\n");
		}

	    /* now we can increment the counter */

	    kbdCount++;

	    if (usbKbdDevFind (pChan, &pUsbKbdDev) != OK)
		{
		printf("usbKbdDevFind() returned ERROR\n");
		return;
		}
	    printf("USB Keyboard attached as %s\n", kbdName);
	    }
	}
    else if (attachCode == USB_KBD_REMOVE)
	{
	/* find the related device */

	if (usbKbdDevFind (pChan, &pUsbKbdDev) != OK)
	    {
	    printf ("usbKbdDevFind could not find channel 0x%d", (UINT32)pChan);
	    return;
	    }	

	/* delete the device */

	usbKbdDevDelete (pUsbKbdDev);

	if (usbKeyboardSioChanUnlock (pChan) != OK)
	    {
	    printf("usbKeyboardSioChanUnlock () returned ERROR\n");
	    return;
	    }
	sprintf (kbdName, "%s%d", USB_KBD_NAME, kbdCount-1);
	printf ("USB Keyboard %s removed\n", kbdName);
	}

    }

/*******************************************************************************
*
* usbKbdOpen - open a usbKbdDrv serial device.
*
* Increments a counter that holds the number of open paths to device. 
*/

LOCAL int usbKbdOpen
    (
    USB_KBD_DEV	* pUsbKbdDev,  /* keyboard device to read from */
    char     *	name,		/* device name */
    int		flags,		/* flags */
    int        	mode		/* mode selected */
    )
    {

    pUsbKbdDev->numOpen++;  /* increment number of open paths */
    
    sioIoctl (pUsbKbdDev->pSioChan, SIO_OPEN, NULL);

    return ((int) pUsbKbdDev);
    }

/*******************************************************************************
*
* usbKbdDrvUnInit - shuts down an I/O USB keyboard driver
*
*
* This is supplied to for the user, but it should be noted that iosDrvRemove()
* may cause unpredictable results.
*
*/

STATUS usbKbdDrvUnInit (void)
    {
    if (!usbKbdDrvNum)
	return (OK);

    /* remove the driver */

    if (iosDrvRemove (usbKbdDrvNum, TRUE) != OK)
	{
	printf("iosDrvRemove () returned ERROR\n");
	return (ERROR);
	}

    usbKbdDrvNum = 0;

    /* delete the mutex semaphores */
 
    if (semDelete (usbKbdMutex) == ERROR)
	{
	printf("semDelete (usbKbdMutex) returned ERROR\n");
	return (ERROR);
	}

    if (semDelete (usbKbdListMutex) == ERROR)
	{
	printf("semDelete (usbKbdListMutex) returned ERROR\n");
	return (ERROR);
	}

    /* unregister */
 
    if (usbKeyboardDynamicAttachUnRegister (usbKbdDrvAttachCallback, 
					 NULL) 
					!= OK)
	{
	printf("usbKeyboardDynamicAttachUnRegister () returned ERROR\n");
	return (ERROR);
	}
 
    /* shutdown */

    if (usbKeyboardDevShutdown () != OK)
	{
	printf("usbKeyboardDynamicAttachUnRegister () returned ERROR\n");
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* usbKbdClose - close a usbKbdDrv serial device.
*
* Decrements the counter of open paths to device and alerts the driver 
* with an ioctl call when the count reaches zero. This scheme is used to
* implement the HUPCL(hang up on last close).      
*/

LOCAL int usbKbdClose
    (
    USB_KBD_DEV	* pUsbKbdDev          /* keyboard device to read from */
    )
    {

    /* if there are no open channels */

    if (!(--pUsbKbdDev->numOpen))
	{
	sioIoctl (pUsbKbdDev->pSioChan, SIO_HUP, NULL);
	}

    return ((int) pUsbKbdDev);
    }

/*******************************************************************************
*
* usbKbdIoctl - issue device control commands
*
* This routine issues device control commands to an USB keyboard device.
*
* RETURNS: depends on the function invoked.
*/

LOCAL int usbKbdIoctl
    (
    USB_KBD_DEV	* pUsbKbdDev,	/* keyboard device to read from */
    int		request,	/* request code */
    void *	arg		/* some argument */
    )
    {

    return (sioIoctl (pUsbKbdDev->pSioChan, request, arg));
    }

/*******************************************************************************
*
* usbKbdRead - read from the USB keyboard
*
* This routines is a no-op for a read from the USB keyboard.
*
* RETURNS: OK, always.
*/

LOCAL int usbKbdRead
    (
    USB_KBD_DEV	* pUsbKbdDev,           /* keyboard device to read from */
    UCHAR *buffer, 
    UINT32 nBytes    
    )
    {

    return (pUsbKbdDev->pSioChan->pDrvFuncs->pollInput (pUsbKbdDev->pSioChan, 								buffer));
    }

/*******************************************************************************
*
* usbKbdWrite - write to the USB keyboard
*
* 
* USB keyboards don't like to be written to.  We don't support this feature.
*
* RETURNS: ENOSYS
*/

int usbKbdWrite
    (
    USB_KBD_DEV	* pUsbKbdDev,		/* keyboard device to read from */
    UCHAR	* buffer,		/* buffer of data to write  */
    UINT32	  nBytes		/* number of bytes in buffer */
    )
    {

    return (ENOSYS);
    }


/*****************************************************************************
*
* usrUsbKbdInit - initialize the USB keyboard driver
*
* This function initializes the USB keyboard driver and registers for attach 
* callbacks.  
*
* RETURNS: Nothing 
*/

void usrUsbKbdInit (void) 
    {

    /* Check if driver already installed */

    if (usbKbdDrvNum > 0)
	{
	printf ("Keyboard already initilaized.\n");
	}


    if (usbKeyboardDevInit () == OK)
	{

        printf ("usbKeyboardDevInit() returned OK\n");
	
	if (usbKeyboardDynamicAttachRegister (usbKbdDrvAttachCallback, 
					      (void *) NULL) 
					  != OK)
	    {
	    printf ("usbKeyboardDynamicAttachRegister() returned ERROR\n");
	    }
	}
    else
        printf ("usbKeyboardDevInit() returned ERROR\n");

    }

/*************************************************************************
*
* enterThread - waits for user to press [enter]
*
* RETURNS:  N/A
*/

LOCAL void enterThread ( void )

    {
    char bfr [256];

    printf ("Press [enter] to terminate polling.\n");
    gets (bfr);
    enterPressed = TRUE;
    }



/*************************************************************************
*
* playWithKeyboard - prints key presses to the terminal
*
* RETURNS: OK or ERROR
*/

UINT16 playWithKeyboard ( void )
    {
    int		taskId;
    char	inChar;
    int		fd = 0;


    /* Poll for input or until user presses CTRL-Z on USB keyboard or
     * [enter] on main keyboard. */


    if ((fd = open ("/usbKb/0", 2,0)) == ERROR)
	return (ERROR);


    /* Create thread to watch for keypress */

    enterPressed = FALSE;

     if((taskId = taskSpawn ("tEnter", 
			     5,		/* priority */
			     0,		/* task options */ 
			     0x4000, 	/* 16k stack space */	
			     (FUNCPTR) enterThread, 
			     0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )) ==ERROR)
	{
	printf(" TaskSpawn Error...!\n");
	return ERROR;	
	}

    printf ("Press CTRL-Z to terminate polling.\n");

    while (!enterPressed)
	{
	    if (read (fd, &inChar, 1) == OK)
		{
		printf ("ASCII %3d", inChar);
		if (inChar >= 32)
		    printf (" '%c'", inChar);
		printf ("\n");

		if (inChar == CTRL_Z)
		    {
		    printf ("Stopped by CTRL-Z\n");
		    break;
		    }
		}

	taskDelay (2);
	}

    taskDelete (taskId);

    return OK;
    }
