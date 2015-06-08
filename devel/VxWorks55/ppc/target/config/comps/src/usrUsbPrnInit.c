/* usrUsbPrnInit.c - Initialization of the USB Printer driver */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
Modification history
--------------------
01d,08dec01,wef  fixing warnings
01c,07dec01,wef	fixed some warnings.
01b,02feb01,wef	Added ios functionality - allows for open, close, read,
		ioctl, etc access to the printer.
01a,23aug00,wef	Created
*/

/*
DESCRIPTION

This configlette initializes the USB printer driver.  This assumes the
USB host stack has already been initialized and has a host controller
driver attached.   

This configlette demonstrates how a user might integrate a USB class 
driver into the vxWorks file system.  usbPrnDevCreate () intalls a USB
printer and its associated driver functions into the driver table allowing
the printer to be accesed with standard fopen, close, read, write, etc. type
calls.  The decision to keep this functionality out of the driver itself
was made for backwards compatability reasons. 

*/


/* includes */

#include "drv/usb/usbPrinterLib.h"
#include "usb/usbPrinter.h"
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

#define USB_PRN_MUTEX_TAKE(tmout)		\
    semTake (usbPrnMutex, (int) (tmout))

#define USB_PRN_MUTEX_GIVE			\
    semGive (usbPrnMutex)

#define USB_PRN_LIST_SEM_TAKE(tmout)		\
    semTake (usbPrnListMutex, (int) (tmout))

#define USB_PRN_LIST_SEM_GIVE			\
    semGive (usbPrnListMutex)


#define PRN_NAME_LEN_MAX        100
#define USB_PRN_NAME		"/usbPr/"

/* data types */

/* typedefs */
 
typedef struct usb_prn_node
    {
    NODE		node;
    struct usb_prn_dev	* pUsbPrnDev;
    } USB_PRN_NODE;

typedef struct usb_prn_dev /* USB_PRN_DEV */
    {
    DEV_HDR             ioDev;
    SIO_CHAN    *       pSioChan;
    UINT16              numOpen;
    UINT32              bufSize;
    UCHAR	*       buff;
    SEL_WAKEUP_LIST     selList;
    USB_PRN_NODE	* pUsbPrnNode;
    } USB_PRN_DEV;
 

/* local variables */

LOCAL SEM_ID    usbPrnMutex;		/* mutex semaphore */
LOCAL SEM_ID    usbPrnListMutex;	/* mutex semaphore to protect list */
LOCAL UCHAR	* txCallbackBfr = NULL;
LOCAL UINT32	txBytesNum = 0;
LOCAL UINT32	prnCount = 0;
LOCAL LIST      usbPrnList;		/* all USB printers in the system */

LOCAL int usbPrnDrvNum = 0;		/* driver number */

/* forward declarations */

LOCAL void	usbPrnDrvAttachCallback (void * arg, SIO_CHAN *pChan, 
				   UINT16 attachCode);

LOCAL void	usbPrnDrvAttachCallback (void * arg, SIO_CHAN *pChan, 
					 UINT16 attachCode);

LOCAL int	usbPrnRead (USB_PRN_DEV * pUsbPrnDev);

LOCAL int	usbPrnClose (USB_PRN_DEV * pUsbPrnDev);

LOCAL int	usbPrnWrite (USB_PRN_DEV * pUsbPrnDev, UCHAR * buffer,
			     UINT32 nBytes);

LOCAL int	usbPrnOpen (USB_PRN_DEV * pUsbPrnDev, char * name,
			    int flags, int mode);

LOCAL STATUS	usbPrnTxCallback (void *, char *);

LOCAL int 	usbPrnIoctl (USB_PRN_DEV * pUsbPrnDev, int request, void * arg);

LOCAL STATUS 	usbPrnDevFind (SIO_CHAN *pChan, USB_PRN_DEV * pUsbPrnDev);

LOCAL STATUS	usbPrnDevDelete (USB_PRN_DEV * pUsbPrnDev);


/*******************************************************************************
*
* usbPrnDevCreate - create a VxWorks device for an USB printer
*
* This routine creates a device on a specified serial channel.  Each channel
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device "/ /0", the proper call would be:
* .CS
*     usbPrnDevCreate ("/usbPr/0", pSioChan);
* .CE
* Where pSioChan is the address of the underlying SIO_CHAN serial channel
* descriptor (defined in sioLib.h).
* This routine is typically called by the USB printer driver, when it detects 
* an insertion of a USB printer.
*
* RETURNS: OK, or ERROR if the driver is not installed, or the
* device already exists, or failed to allocate memory.
*/

STATUS usbPrnDevCreate
    (
    char	* name,         /* name to use for this device      */
    SIO_CHAN	* pSioChan	/* pointer to core driver structure */
    )
    {
    USB_PRN_NODE	* pUsbPrnNode = NULL;   /* pointer to device node */
    USB_PRN_DEV		* pUsbPrnDev = NULL;     /* pointer to USB device */


    /* Create the mutex semaphores */
 
    usbPrnMutex = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
			      SEM_INVERSION_SAFE);
 
    usbPrnListMutex = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
				  SEM_INVERSION_SAFE);
 
    /* initialize the linked list */
 
    lstInit (&usbPrnList);

    usbPrnDrvNum = iosDrvInstall ((FUNCPTR) NULL, 
				  (FUNCPTR) usbPrnDevDelete, 
				  (FUNCPTR) usbPrnOpen, 
				  (FUNCPTR) usbPrnClose, 
				  (FUNCPTR) usbPrnRead, 
				  (FUNCPTR) usbPrnWrite, 
				  (FUNCPTR) usbPrnIoctl);


    if (usbPrnDrvNum <= 0)
        {
        errnoSet (S_ioLib_NO_DRIVER);
        return (ERROR);
        }

    if (pSioChan == (SIO_CHAN *) ERROR)
	{
        return (ERROR);
	}

    /* allocate memory for the device */

    if ((pUsbPrnDev = (USB_PRN_DEV *) calloc (1, sizeof (USB_PRN_DEV))) == NULL)
        return (ERROR);

    pUsbPrnDev->pSioChan = pSioChan;

    /* allocate memory for this node, and populate it */

    pUsbPrnNode = (USB_PRN_NODE *) calloc (1, sizeof (USB_PRN_NODE));

    /* record useful information */

    pUsbPrnNode->pUsbPrnDev = pUsbPrnDev;
    pUsbPrnDev->pUsbPrnNode = pUsbPrnNode;

    /* add the node to the list */

    USB_PRN_LIST_SEM_TAKE (WAIT_FOREVER);

    lstAdd (&usbPrnList, (NODE *) pUsbPrnNode);

    USB_PRN_LIST_SEM_GIVE;
 
    /* allow for select support */

    selWakeupListInit (&pUsbPrnDev->selList);

    /* start the device in the only supported mode */

    sioIoctl (pUsbPrnDev->pSioChan, SIO_MODE_SET, (void *) SIO_MODE_INT);

    /* add the device to the I/O system */

    return (iosDevAdd (&pUsbPrnDev->ioDev, name, usbPrnDrvNum));
    }




/*******************************************************************************
*
* usbPrnDevDelete - delete a VxWorks device for an USB printer
*
* This routine deletes a device on a specified serial channel.  
*
* This routine is typically called by the USB printer driver, when it detects 
* a removal of a USB printer.
*
* RETURNS: OK, or ERROR if the driver is not installed.
*/

LOCAL STATUS usbPrnDevDelete
    (
    USB_PRN_DEV	* pUsbPrnDev		/* printer device to read from */
    )
    {
    int		status = OK;		/* holder for the return value */

    if (usbPrnDrvNum <= 0)
        {
        errnoSet (S_ioLib_NO_DRIVER);
        return (ERROR);
        }

    /* remove the device from the I/O system */

    iosDevDelete (&pUsbPrnDev->ioDev);

    /* remove from list */

    USB_PRN_LIST_SEM_TAKE (WAIT_FOREVER);
    lstDelete (&usbPrnList, (NODE *) pUsbPrnDev->pUsbPrnNode);
    USB_PRN_LIST_SEM_GIVE;

    /* free memory for the device */

    free (pUsbPrnDev->pUsbPrnNode);
    free (pUsbPrnDev);

    return (status);
    }


/*************************************************************************
*
* usbPrnDevFind - find the USB_PRN_DEV device for an SIO channel
*
* RETURNS: OK, or ERROR
*/
 
LOCAL STATUS usbPrnDevFind
    (
    SIO_CHAN *		pChan,          /* pointer to affected SIO_CHAN */
    USB_PRN_DEV *	pUsbPrnDev	/* pointer to an USB_PRN_DEV */
    )
    {
    USB_PRN_NODE *      pUsbPrnNode = NULL;     /* pointer to USB device node */
    USB_PRN_DEV *	pTempDev;		/* pointer to an USB_PRN_DEV */

    if (pChan == NULL)
        return (ERROR);
 
    /* protect it from other module's routines */
 
    USB_PRN_LIST_SEM_TAKE (WAIT_FOREVER);
 
    /* loop through all the devices */
 
    for (pUsbPrnNode = (USB_PRN_NODE *) lstFirst (&usbPrnList);
         pUsbPrnNode != NULL;
         pUsbPrnNode = (USB_PRN_NODE *) lstNext ((NODE *) pUsbPrnNode))
        {
        pTempDev = pUsbPrnNode->pUsbPrnDev;

	/* return as soon as you find it */
	if (pTempDev->pSioChan == pChan)
	    {
	    * (UINT32 *) pUsbPrnDev = (UINT32) pTempDev;
	    USB_PRN_LIST_SEM_GIVE;
	    return (OK);
	    }
        }

    USB_PRN_LIST_SEM_GIVE;

    return (ERROR);
    }

/*************************************************************************
*
* usbPrnDrvAttachCallback - receives attach callbacks from printer SIO driver
*
* RETURNS: N/A
*/
LOCAL void usbPrnDrvAttachCallback
    (
    void * arg,			/* caller-defined argument */
    SIO_CHAN *pChan,		/* pointer to affected SIO_CHAN */
    UINT16 attachCode		/* defined as USB_KBD_xxxx */
    )
    {
    UINT32	usbPrnDev;		/* printer device */
    char	prnName [PRN_NAME_LEN_MAX];	/* printer name */

 
    /*  A printer has been attached. */

    if (attachCode == USB_PRN_ATTACH)
	{

	/*  Lock this particular channel for this particular printer */

	if (usbPrinterSioChanLock (pChan) != OK)
	    {
	    printf("usbPrinterSioChanLock () returned ERROR\n");
	    }
	else
	    {
	    /*  plugs the name /usbPr/x into the prnName variable */

	    sprintf (prnName, "%s%d", USB_PRN_NAME, prnCount);

	    /*  Create the printer device, do all the ios stuff (drvInstall, 
	     *  devAdd...) 
	     */

	    if (usbPrnDevCreate (prnName, pChan) != OK)
		{
		printf("usbPrnDevCreate() returned ERROR\n");
		}

	    /* now we can increment the counter */

	    prnCount++;

	    if (usbPrnDevFind (pChan, (USB_PRN_DEV *) &usbPrnDev) != OK)
		{
		printf("usbPrnDevFind() returned ERROR\n");
		return;
		}

	    /*  Install a callback to spew characters to the printer 
	     *  usbPrnTxCallback () handles sending chunks when the printer
	     *  is ready for them
	     */

	    if ((*pChan->pDrvFuncs->callbackInstall) (pChan, 
				SIO_CALLBACK_GET_TX_CHAR, 
				(STATUS  (*) (void *, ...)) usbPrnTxCallback, 
				(void *) usbPrnDev) 
			!= OK)
		{
		printf("usbPrnTxCallback () failed to install\n");
		}
	    printf("USB Printer attached as %s\n", prnName);
	    }
	}

    /* A printer has been detached */

    else if (attachCode == USB_PRN_REMOVE)
	{
	/* find the related device */

	if (usbPrnDevFind (pChan, (USB_PRN_DEV *) &usbPrnDev) != OK)
	    {
	    printf ("usbPrnDevFind could not find channel 0x%d", (UINT32)pChan);
	    return;
	    }	

	/* delete the device */

	usbPrnDevDelete ((USB_PRN_DEV *) usbPrnDev);

	/* Unlock this channel since the device that was plugged into it
	 * was removed
	 */
	if (usbPrinterSioChanUnlock (pChan) != OK)
	    {
	    printf("usbPrinterSioChanUnlock () returned ERROR\n");
	    return;
	    }
	}
    }

/*******************************************************************************
*
* usbPrnOpen - open a usbPrnDrv serial device.
*
* Increments a counter that holds the number of open paths to device. 
*/

LOCAL int usbPrnOpen
    (
    USB_PRN_DEV	* pUsbPrnDev,  /* printer device to read from */
    char     *	name,		/* device name */
    int		flags,		/* flags */
    int        	mode		/* mode selected */
    )
    {

    pUsbPrnDev->numOpen++;  /* increment number of open paths */
    
    sioIoctl (pUsbPrnDev->pSioChan, SIO_OPEN, NULL);

    return ((int) pUsbPrnDev);
    }

/*******************************************************************************
*
* usbPrnDrvUnInit - shuts down an I/O USB printer driver
*
*
* This is supplied to for the user, but it should be noted that iosDrvRemove()
* may cause unpredictable results.
*
*/

STATUS usbPrnDrvUnInit (void)
    {
    if (!usbPrnDrvNum)
	return (OK);

    /* remove the driver */

    if (iosDrvRemove (usbPrnDrvNum, TRUE) != OK)
	{
	printf("iosDrvRemove () returned ERROR\n");
	return (ERROR);
	}

    /* HELP */
    usbPrnDrvNum = 0;

    /* delete the mutex semaphores */
 
    if (semDelete (usbPrnMutex) == ERROR)
	{
	printf("semDelete (usbPrnMutex) returned ERROR\n");
	return (ERROR);
	}

    if (semDelete (usbPrnListMutex) == ERROR)
	{
	printf("semDelete (usbPrnListMutex) returned ERROR\n");
	return (ERROR);
	}

    /* unregister */
 
    if (usbPrinterDynamicAttachUnRegister (usbPrnDrvAttachCallback, 
					 NULL) 
					!= OK)
	{
	printf("usbPrinterDynamicAttachUnRegister () returned ERROR\n");
	return (ERROR);
	}
 
    /* shutdown */

    if (usbPrinterDevShutdown () != OK)
	{
	printf("usbPrinterDynamicAttachUnRegister () returned ERROR\n");
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* usbPrnClose - close a usbPrnDrv serial device.
*
* Decrements the counter of open paths to device and alerts the driver 
* with an ioctl call when the count reaches zero. This scheme is used to
* implement the HUPCL(hang up on last close).      
*/

LOCAL int usbPrnClose
    (
    USB_PRN_DEV	* pUsbPrnDev          /* printer device to read from */
    )
    {

    /* if there are no open channels */

    if (!(--pUsbPrnDev->numOpen))
	{
	sioIoctl (pUsbPrnDev->pSioChan, SIO_HUP, NULL);
	}

    return ((int) pUsbPrnDev);
    }

/*******************************************************************************
*
* usbPrnIoctl - issue device control commands
*
* This routine issues device control commands to an USB printer device.
*
* RETURNS: depends on the function invoked.
*/

LOCAL int usbPrnIoctl
    (
    USB_PRN_DEV	* pUsbPrnDev,	/* printer device to read from */
    int		request,	/* request code */
    void *	arg		/* some argument */
    )
    {

    return (sioIoctl (pUsbPrnDev->pSioChan, request, arg));
    }

/*******************************************************************************
*
* usbPrnRead - read from the USB printer
*
* This routines is a no-op for a read from the USB printer.
*
* RETURNS: OK, always.
*/

LOCAL int usbPrnRead
    (
    USB_PRN_DEV	* pUsbPrnDev          /* printer device to read from */
    )
    {
    int		status = OK;		/* holder for the return value */

    return (status);
    }


/*******************************************************************************
*
* usbPrnWrite - write to the USB printer
*
* This routine writes <nBytes> bytes of data from the buffer pointed to by
* <buff> to the USB printer described by <pUsbPrnDev>.
*
* RETURNS: number of bytes written or ERROR
*/

int usbPrnWrite
    (
    USB_PRN_DEV	* pUsbPrnDev,         /* printer device to read from */
    UCHAR	* buffer,             /* buffer of data to write  */
    UINT32	  nBytes              /* number of bytes in buffer */
    )
    {
    int		status = OK;		/* holder for the return value */


    /* protect the critical region */

    USB_PRN_MUTEX_TAKE (WAIT_FOREVER);

    /* store buffer information */

    txCallbackBfr = buffer;
    txBytesNum = nBytes;
    pUsbPrnDev->buff = buffer;
    pUsbPrnDev->bufSize = nBytes;

    if ((*(pUsbPrnDev->pSioChan->pDrvFuncs->txStartup)) (pUsbPrnDev->pSioChan) 
	!= OK)
	{
	status = ERROR;
	}

    /* end of the critical region */

    USB_PRN_MUTEX_GIVE; 

    /* wake up any select-blocked readers */
	 
    selWakeupAll (&pUsbPrnDev->selList, SELWRITE);

    status = (int) (nBytes - txBytesNum);
    return (status);
    }


/*************************************************************************
*
* usbPrnTxCallback - feeds characters to USB printer SIO driver
*
* RETURNS: OK
*/

LOCAL STATUS usbPrnTxCallback
    (
    void *callbackParam,
    char *txChar
    )
    {
    USB_PRN_DEV	* pDev = (USB_PRN_DEV *) callbackParam; /* printer device */

    if (!(pDev->bufSize))
	{
	return (ERROR);
	}

    pDev->bufSize--;
    *txChar = *pDev->buff++;

    return OK;
    }


/*****************************************************************************
*
* usrUsbPrnInit - initialize the USB printer driver
*
* This function initializes the USB printer driver and registers for attach 
* callbacks.  
*
* RETURNS: Nothing 
*/

void usrUsbPrnInit (void) 
    {

    /* Check if driver already installed */

    if (usbPrnDrvNum > 0)
	{
	printf ("Printer already initilaized.\n");
	}

    /* Initialize USB printer SIO driver */

    if (usbPrinterDevInit () == OK)
	{

        printf ("usbPrinterDevInit() returned OK\n");

	/* Register our device for attach callbacks.  */

	if (usbPrinterDynamicAttachRegister (usbPrnDrvAttachCallback, 
					   (void *) NULL) 
					  != OK)
	    {
	    printf ("usbPrinterDynamicAttachRegister() returned ERROR\n");
	    }
	}
    else
        printf ("usbPrinterDevInit() returned ERROR\n");

    }


/*************************************************************************
*
* usbPrnWrTest - test the USB printer write function
*
* This is given as a sample application to print a file using the i/o 
* system calls to access the printer device.  It has been assumed that the 
* file that it sent to the printer has been "canned" for the particular
* printer being used.  Refer to the USB Developer's Kit manual for further
* details.
*
* RETURNS: OK, or ERROR.
*/

STATUS usbPrnWrTest
    (
    char	* fileName		/* file to print */
    )
    {
    int		testFd = 0;
    UCHAR	* tempBuff = NULL;
    FILE	* testFile;
    UINT32	fsize = 0;



    if ((testFile = fopen (fileName, "rb")) == NULL)
	{
	printf ("fopen() failed open the input file\n");
	return (ERROR);
	}

    fseek (testFile, 0, SEEK_END);
    fsize = ftell (testFile);
    fseek (testFile, 0, SEEK_SET);

   printf ("usbPrnTxCallback() file size =0x%x \n",fsize);

    if ((tempBuff = calloc (1, fsize)) == NULL)
	return (ERROR);

    if (fread (tempBuff, 1, fsize, testFile) == 0)
	return (ERROR);

    if ((testFd = open ("/usbPr/0", 2,0)) == ERROR)
	return (ERROR);

    if (write (testFd, tempBuff, fsize) == ERROR)
	return (ERROR);

    return (OK);
    }

