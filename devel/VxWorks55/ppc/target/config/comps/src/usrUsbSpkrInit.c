/* usrUsbSpkrInit.c - Initialization of the USB Speaker driver */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
Modification history
--------------------
01d,30jan02,wef fixed DevFind routine.
01c,07dec01,wef fixed some warnings.
01b,02feb01,wef Added ios functionality - allows for open, close, read,
                ioctl, etc access to the printer.
01a,01feb01,wef Created
*/

/*
DESCRIPTION

This configlette initializes the USB speaker driver.  This assumes the
USB host stack has already been initialized and has a host controller
driver attached.   

This configlette demonstrates how a user might integrate a USB class 
driver into the vxWorks file system.  usbSpkrDevCreate () intalls a USB
speaker and its associated driver functions into the driver table allowing
the speaker to be accesed with standard fopen, close, read, write, etc. type
calls.  The decision to keep this functionality out of the driver itself
was made for backwards compatability reasons. 

*/


/* includes */

#include "drv/usb/usbSpeakerLib.h"
#include "usb/tools/wavFormat.h"		/* Microsoft .wav file format */
#include "usb/usbAudio.h"
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

#define USB_SPKR_MUTEX_TAKE(tmout)		\
    semTake (usbSpkrMutex, (int) (tmout))

#define USB_SPKR_MUTEX_GIVE			\
    semGive (usbSpkrMutex)

#define USB_SPKR_LIST_SEM_TAKE(tmout)		\
    semTake (usbSpkrListMutex, (int) (tmout))

#define USB_SPKR_LIST_SEM_GIVE			\
    semGive (usbSpkrListMutex)


#define SPKR_NAME_LEN_MAX        100
#define USB_SPKR_NAME		"/usbSp/"

/* data types */

/* typedefs */
 
typedef struct usb_spkr_node
    {
    NODE		node;
    struct usb_spkr_dev	* pUsbSpkrDev;
    } USB_SPKR_NODE;

typedef struct usb_spkr_dev /* USB_SPKR_DEV */
    {
    DEV_HDR		ioDev;
    SEQ_DEV	*	pSeqDev;
    UINT16              numOpen;
    UINT32              bufSize;
    UCHAR	*       buff;
    SEL_WAKEUP_LIST     selList;
    USB_SPKR_NODE	* pUsbSpkrNode;
    } USB_SPKR_DEV;
 

/* local variables */

LOCAL SEM_ID    usbSpkrMutex;		/* mutex semaphore */
LOCAL SEM_ID    usbSpkrListMutex;	/* mutex semaphore to protect list */


LOCAL UINT32	spkrCount = 0;
LOCAL LIST      usbSpkrList;		/* all USB speakers in the system */

LOCAL int usbSpkrDrvNum = 0;		/* driver number */

/* forward declarations */

LOCAL void	usbSpkrDrvAttachCallback (void * arg, SEQ_DEV *pSeqDev, 
				   UINT16 attachCode);

LOCAL int	usbSpkrRead (USB_SPKR_DEV * pUsbSpkrDev,UCHAR *buffer, 
			    UINT32 nBytes);

LOCAL int	usbSpkrClose (USB_SPKR_DEV * pUsbSpkrDev);

LOCAL int	usbSpkrWrite (USB_SPKR_DEV * pUsbSpkrDev, UCHAR * buffer,
			     UINT32 nBytes);

LOCAL int	usbSpkrOpen (USB_SPKR_DEV * pUsbSpkrDev, char * name,
			    int flags, int mode);

LOCAL int 	usbSpkrIoctl (USB_SPKR_DEV * pUsbSpkrDev, int request, void * arg);

LOCAL STATUS 	usbSpkrDevFind (SEQ_DEV * pSeqDev, USB_SPKR_DEV ** ppUsbSpkrDev);

LOCAL STATUS	usbSpkrDevDelete (USB_SPKR_DEV * pUsbSpkrDev);


/*******************************************************************************
*
* usbSpkrDevCreate - create a VxWorks device for an USB speaker
*
* This routine creates a device on a specified serial channel.  Each channel
* to be used should have exactly one device associated with it by calling
* this routine.
*
* For instance, to create the device "/ /0", the proper call would be:
* .CS
*     usbSpkrDevCreate ("/usbSp/0", pSeqDev);
* .CE
* Where pSeqDev is the address of the underlying SEQ_DEV serial channel
* descriptor (defined in sioLib.h).
* This routine is typically called by the USB speaker driver, when it detects 
* an insertion of a USB speaker.
*
* RETURNS: OK, or ERROR if the driver is not installed, or the
* device already exists, or failed to allocate memory.
*/

STATUS usbSpkrDevCreate
    (
    char	* name,         /* name to use for this device      */
    SEQ_DEV	* pSeqDev	/* pointer to core driver structure */
    )
    {
    USB_SPKR_NODE	* pUsbSpkrNode = NULL;   /* pointer to device node */
    USB_SPKR_DEV		* pUsbSpkrDev = NULL;     /* pointer to USB device */


    /* Create the mutex semaphores */
 
    usbSpkrMutex = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
			      SEM_INVERSION_SAFE);
 
    usbSpkrListMutex = semMCreate (SEM_Q_PRIORITY | SEM_DELETE_SAFE |
				  SEM_INVERSION_SAFE);
 
    /* initialize the linked list */
 
    lstInit (&usbSpkrList);

    usbSpkrDrvNum = iosDrvInstall ((FUNCPTR) NULL, 
				  (FUNCPTR) usbSpkrDevDelete, 
				  (FUNCPTR) usbSpkrOpen, 
				  (FUNCPTR) usbSpkrClose, 
				  (FUNCPTR) usbSpkrRead, 
				  (FUNCPTR) usbSpkrWrite, 
				  (FUNCPTR) usbSpkrIoctl);

    if (usbSpkrDrvNum <= 0)
        {
        errnoSet (S_ioLib_NO_DRIVER);
	printf ("There is no more room in the driver table\n");
	return (ERROR);
        }

    if (pSeqDev == (SEQ_DEV *) ERROR)
	{
	printf ("pSeqDev is ERROR\n");
	return (ERROR);
	}

    /* allocate memory for the device */

    if ((pUsbSpkrDev = (USB_SPKR_DEV *) calloc (1, sizeof (USB_SPKR_DEV))) == NULL)
	{
	printf ("calloc returned NULL - out of memory\n");
	return (ERROR);
	}

    pUsbSpkrDev->pSeqDev = pSeqDev;

    /* allocate memory for this node, and populate it */

    pUsbSpkrNode = (USB_SPKR_NODE *) calloc (1, sizeof (USB_SPKR_NODE));

    /* record useful information */

    pUsbSpkrNode->pUsbSpkrDev = pUsbSpkrDev;
    pUsbSpkrDev->pUsbSpkrNode = pUsbSpkrNode;

    /* add the node to the list */

    USB_SPKR_LIST_SEM_TAKE (WAIT_FOREVER);

    lstAdd (&usbSpkrList, (NODE *) pUsbSpkrNode);

    USB_SPKR_LIST_SEM_GIVE;
 
    /* allow for select support */

    selWakeupListInit (&pUsbSpkrDev->selList);

    /* start the device in the only supported mode */

/*    sioIoctl (pUsbSpkrDev->pSeqDev, SIO_MODE_SET, (void *) SIO_MODE_INT);*/

    /* add the device to the I/O system */

    return (iosDevAdd (&pUsbSpkrDev->ioDev, name, usbSpkrDrvNum));
    }




/*******************************************************************************
*
* usbSpkrDevDelete - delete a VxWorks device for an USB speaker
*
* This routine deletes a device on a specified serial channel.  
*
* This routine is typically called by the USB speaker driver, when it detects 
* a removal of a USB speaker.
*
* RETURNS: OK, or ERROR if the driver is not installed.
*/

LOCAL STATUS usbSpkrDevDelete
    (
    USB_SPKR_DEV	* pUsbSpkrDev		/* speaker device to read from */
    )
    {
    int		status = OK;		/* holder for the return value */

    if (usbSpkrDrvNum <= 0)
        {
        errnoSet (S_ioLib_NO_DRIVER);
        return (ERROR);
        }

    /* remove the device from the I/O system */

    iosDevDelete (&pUsbSpkrDev->ioDev);

    /* remove from list */

    USB_SPKR_LIST_SEM_TAKE (WAIT_FOREVER);
    lstDelete (&usbSpkrList, (NODE *) pUsbSpkrDev->pUsbSpkrNode);
    USB_SPKR_LIST_SEM_GIVE;

    /* free memory for the device */

    free (pUsbSpkrDev->pUsbSpkrNode);
    free (pUsbSpkrDev);

    return (status);
    }


/*************************************************************************
*
* usbSpkrDevFind - find the USB_SPKR_DEV device for an SIO channel
*
* RETURNS: OK, or ERROR
*/
 
LOCAL STATUS usbSpkrDevFind
    (
    SEQ_DEV *		pSeqDev,          /* pointer to affected SEQ_DEV */
    USB_SPKR_DEV **	ppUsbSpkrDev	/* pointer to an USB_SPKR_DEV */
    )
    {
    USB_SPKR_NODE *      pUsbSpkrNode = NULL;     /* pointer to USB device node */
    USB_SPKR_DEV *	pTempDev;		/* pointer to an USB_SPKR_DEV */

    if (pSeqDev == NULL)
        return (ERROR);
 
    /* protect it from other module's routines */
 
    USB_SPKR_LIST_SEM_TAKE (WAIT_FOREVER);
 
    /* loop through all the devices */
 
    for (pUsbSpkrNode = (USB_SPKR_NODE *) lstFirst (&usbSpkrList);
         pUsbSpkrNode != NULL;
         pUsbSpkrNode = (USB_SPKR_NODE *) lstNext ((NODE *) pUsbSpkrNode))
        {
        pTempDev = pUsbSpkrNode->pUsbSpkrDev;

	/* return as soon as you find it */
	if (pTempDev->pSeqDev == pSeqDev)
	    {
	    * (UINT32 *) ppUsbSpkrDev = (UINT32) pTempDev;
	    USB_SPKR_LIST_SEM_GIVE;
	    return (OK);
	    }
        }

    USB_SPKR_LIST_SEM_GIVE;

    return (ERROR);
    }

/*************************************************************************
*
* usbSpkrDrvAttachCallback - receives attach callbacks from speaker SIO driver
*
* RETURNS: N/A
*/
LOCAL void usbSpkrDrvAttachCallback
    (
    void * arg,			/* caller-defined argument */
    SEQ_DEV *pSeqDev,		/* pointer to affected SEQ_DEV */
    UINT16 attachCode		/* defined as USB_SPKR_xxxx */
    )
    {
    USB_SPKR_DEV * pUsbSpkrDev = NULL;		/* speaker device */
    char	spkrName [SPKR_NAME_LEN_MAX];	/* speaker name */

    if (attachCode == USB_SPKR_ATTACH)
	{
	if (usbSpeakerSeqDevLock (pSeqDev) != OK)
	    {
	    printf("usbSpeakerSeqDevLock () returned ERROR\n");
	    }
	else
	    {
	    sprintf (spkrName, "%s%d", USB_SPKR_NAME, spkrCount);
	    if (usbSpkrDevCreate (spkrName, pSeqDev) != OK)
		{
		printf("usbSpkrDevCreate() returned ERROR\n");
		}

	    /* now we can increment the counter */
	    spkrCount++;

	    if (usbSpkrDevFind (pSeqDev, &pUsbSpkrDev) != OK)
		{
		printf("usbSpkrDevFind() returned ERROR\n");
		return;
		}

	    printf("USB Speaker attached as %s\n", spkrName);
	    }
	}
    else if (attachCode == USB_SPKR_REMOVE)
	{
	/* find the related device */

	if (usbSpkrDevFind (pSeqDev, &pUsbSpkrDev) != OK)
	    {
	    printf ("usbSpkrDevFind could not find channel 0x%d", 
				(UINT32) pSeqDev);
	    return;
	    }	

	/* delete the device */

	usbSpkrDevDelete (pUsbSpkrDev);

	if (usbSpeakerSeqDevUnlock (pSeqDev) != OK)
	    {
	    printf("usbSpeakerSeqDevUnlock () returned ERROR\n");
	    return;
	    }
	sprintf (spkrName, "%s%d", USB_SPKR_NAME, spkrCount-1);
	printf ("USB Speaker %s removed\n", spkrName);
	}

    }

/*******************************************************************************
*
* usbSpkrOpen - open a usbSpkrDrv serial device.
*
* Increments a counter that holds the number of open paths to device. 
*/

LOCAL int usbSpkrOpen
    (
    USB_SPKR_DEV	* pUsbSpkrDev,  /* speaker device to read from */
    char     *	name,		/* device name */
    int		flags,		/* flags */
    int        	mode		/* mode selected */
    )
    {

    pUsbSpkrDev->numOpen++;  /* increment number of open paths */

    return ((int) pUsbSpkrDev);
    }

/*******************************************************************************
*
* usbSpkrDrvUnInit - shuts down an I/O USB speaker driver
*
*
* This is supplied to for the user, but it should be noted that iosDrvRemove()
* may cause unpredictable results.
*
*/

STATUS usbSpkrDrvUnInit (void)
    {
    if (!usbSpkrDrvNum)
	return (OK);

    /* remove the driver */

    if (iosDrvRemove (usbSpkrDrvNum, TRUE) != OK)
	{
	printf("iosDrvRemove () returned ERROR\n");
	return (ERROR);
	}

    /* HELP */
    usbSpkrDrvNum = 0;

    /* delete the mutex semaphores */
 
    if (semDelete (usbSpkrMutex) == ERROR)
	{
	printf("semDelete (usbSpkrMutex) returned ERROR\n");
	return (ERROR);
	}

    if (semDelete (usbSpkrListMutex) == ERROR)
	{
	printf("semDelete (usbSpkrListMutex) returned ERROR\n");
	return (ERROR);
	}

    /* unregister */
 
    if (usbSpeakerDynamicAttachUnRegister (usbSpkrDrvAttachCallback, 
					   (pVOID) NULL) 
					!= OK)
	{
	printf("usbSpeakerDynamicAttachUnRegister () returned ERROR\n");
	return (ERROR);
	}
 
    /* shutdown */

    if (usbSpeakerDevShutdown () != OK)
	{
	printf("usbSpeakerDynamicAttachUnRegister () returned ERROR\n");
	return (ERROR);
	}

    return (OK);
    }

/*******************************************************************************
*
* usbSpkrClose - close a usbSpkrDrv serial device.
*
* Decrements the counter of open paths to device.
*
* RETURNS :
*/

LOCAL int usbSpkrClose
    (
    USB_SPKR_DEV	* pUsbSpkrDev          /* speaker device to read from */
    )
    {


    return ((int) --pUsbSpkrDev->numOpen);
    }

/*******************************************************************************
*
* usbSpkrIoctl - issue device control commands
*
* This routine issues device control commands to an USB speaker device.
*
* RETURNS: depends on the function invoked.
*/

LOCAL int usbSpkrIoctl
    (
    USB_SPKR_DEV	* pUsbSpkrDev,	/* speaker device to read from */
    int		request,		/* request code */
    void *	arg			/* some argument */
    )
    {

    return ((*pUsbSpkrDev->pSeqDev->sd_ioctl) (pUsbSpkrDev->pSeqDev, 
					       request, 
					       arg));
    }

/*******************************************************************************
*
* usbSpkrRead - read from the USB speaker
*
* This routines is a no-op for a read from the USB speaker.
*
* RETURNS: OK or ERROR
*/

LOCAL int usbSpkrRead
    (
    USB_SPKR_DEV	* pUsbSpkrDev,           /* speaker device to read from */
    UCHAR *pBuffer, 
    UINT32 nBytes    
    )
    {

    return ((*pUsbSpkrDev->pSeqDev->sd_seqRd) (pUsbSpkrDev->pSeqDev, 
					       nBytes, 
					       pBuffer, 
					       FALSE));

    }

/*******************************************************************************
*
* usbSpkrWrite - write to the USB speaker
*
* 
* USB mice don't like to be written to.  The don't support this feature.
*
* RETURNS: OK or ERROR
*/

int usbSpkrWrite
    (
    USB_SPKR_DEV	* pUsbSpkrDev,	/* speaker device to read from */
    UCHAR	* pBuffer,		/* buffer of data to write  */
    UINT32	  nBytes		/* number of bytes in buffer */
    )
    {

    return ((*pUsbSpkrDev->pSeqDev->sd_seqWrt) (pUsbSpkrDev->pSeqDev, 
						nBytes, 
						pBuffer, 
						FALSE));
    }


/*****************************************************************************
*
* usrUsbSpkrInit - initialize the USB speaker driver
*
* This function initializes the USB speaker driver and registers for attach 
* callbacks.  
*
* RETURNS: Nothing 
*/

void usrUsbSpkrInit (void) 
    {

    /* Check if driver already installed */

    if (usbSpkrDrvNum > 0)
	{
	printf ("Speaker already initilaized.\n");
	}


    if (usbSpeakerDevInit () == OK)
	{

        printf ("usbSpeakerDevInit() returned OK\n");
	
	if (usbSpeakerDynamicAttachRegister (usbSpkrDrvAttachCallback, 
					     (void *) NULL) 
					  != OK)
	    {
	    printf ("usbSpeakerDynamicAttachRegister() returned ERROR\n");
	    }
	}
    else
        printf ("usbSpeakerDevInit() returned ERROR\n");

    }
