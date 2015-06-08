/* usbPciStub.c - System-specific PCI Functions */

/* Copyright 2000 Wind River Systems, Inc. */

/*
Modification history
--------------------
01a,22nov00,wef  First, created from 01h of the mcp750 bsp stub
*/

/*
DESCRIPTION

This file defines a skeleton of functions to be used for accessing 
the PCI bus capabilities.  These functions allow PCI device drivers 
to be written independent of the underlying O/S's PCI access mechanisms.

The name of each function in this group begins with "usb" to represent
"Device Driver Services."
*/

/* Includes */

#include "vxWorks.h"

#include "string.h"

#include "sysLib.h"
#include "cacheLib.h"

#include "iv.h"
#include "intLib.h"

#include "drv/pci/pciConfigLib.h"   /* VxWorks PCI funcs */

#include "usb/usbPlatform.h"		/* Basic definitions */
#include "usb/usbPciLib.h"			/* Our API */



#error "usbPciStub.c: Customize the USB PCI macros before first use"

/*
 * TODO: Read and update this stub file to customize it for this
 * BSP.  Delete the #error statement above afterwards. Look for
 * TODO references in this file.
 */

/*
 * TODO: Adjust the 3 macros below as needed.
 *
 * USB_PCI_IO_OFFSET is the offset between a physical PCI IO address
 * and the virtual address used to access it.  In this example code,
 * PCI IO addresses map one-to-one with virtual IO addresses used
 * by the sysInByte() and sysOutByte() routines.  In a memory
 * mapped system it would be very unlikely for this value to be zero.
 *
 * USB_PCI_MEMIO_OFFSET is the offset between Memory IO (non prefetch)
 * PCI physical addresses and the local virtual address that corresponds
 * to it.
 *
 * USB_PCI_MEM_OFFSET is the offset between Memory (prefetchable) PCI
 * physical addresses and the local virtual address that corresponds
 * to it.
 */

#define USB_PCI_IO_OFFSET	0x0
#define USB_PCI_MEMIO_OFFSET	0x0
#define USB_PCI_MEM_OFFSET	0x0

/*
 * TODO: Define the following macros to describe how to connect
 * and disconnect interrupts on your hardware.  This should
 * normally be mapped to the pciIntLib functions.
 */

/* Interrupt enable/disable mappings */

#define USB_INT_CONNECT(intNo, func, param) \
    pciIntConnect (INUM_TO_IVEC(intNo), (VOIDFUNCPTR) func, (int) param)

#define USB_INT_DISCONNECT(intNo, func, param) \
    pciIntDisconnect2 (INUM_TO_IVEC(intNo), (VOIDFUNCPTR) func, (int) param)

#define USB_INT_ENABLE(i)	intEnable (i)
#define USB_INT_DISABLE(i)	intDisable (i)

/*
 * TODO: Define the following macros to read/write data
 * to/from PCI I/O space as needed.  This may be I/O
 * mapped accesses or memory accesses.  This example
 * code assumes I/O mapped accesses using sysInByte
 * functions provided by the BSP sysLib module.
 */

/* Map I/O functions to underlying system functions. */

#define	USB_PCI_IN_BYTE(a)	sysInByte ((a) + USB_PCI_IO_OFFSET)
#define	USB_PCI_IN_WORD(a)	sysInWord ((a) + USB_PCI_IO_OFFSET)
#define	USB_PCI_IN_DWORD(a)	sysInLong ((a) + USB_PCI_IO_OFFSET)
#define	USB_PCI_OUT_BYTE(a,v)	sysOutByte ((a) + USB_PCI_IO_OFFSET, (v))
#define	USB_PCI_OUT_WORD(a,v)	sysOutWord ((a) + USB_PCI_IO_OFFSET, (v))
#define	USB_PCI_OUT_DWORD(a,v)	sysOutLong ((a) + USB_PCI_IO_OFFSET, (v))

/* code tracks usage count for interrupts 0..USB_MAX_INT_NO-1. */

#define USB_MAX_INT_NO		16




/* TODO: Leave the code below this point alone. */




/* locals */

LOCAL UINT16 intUsage [USB_MAX_INT_NO] = {0};


/***************************************************************************
*
* usbPciClassFind - Locates PCI devices by class.
*
* A caller uses this function to locate a PCI device by its PCI class.
* The caller must specify the <pciClass>, <subClass>, and <pgmIf> for the
* device being sought.	The function returns the first matching device
* for <index> = 0, the second for <index> = 1, and so forth.  The
* bus number, device number, and function number for the matching device 
* are returned in the <pBusNo>, <pDeviceNo>, and <pFuncNo> buffers provided 
* by the caller. 
*
*
* RETURNS:
* Returns TRUE if matching device found, FALSE if device not found.
*/

BOOL usbPciClassFind
    (
    UINT8 pciClass,		/* PCI device class */
    UINT8 subClass,		/* PCI device sub-class */
    UINT8 pgmIf,		/* Programming interface */
    UINT16 index,		/* Caller wants nth matching dev */
    pUINT8 pBusNo,		/* Bus number of matching dev */
    pUINT8 pDeviceNo,		/* Device number of matching dev */
    pUINT8 pFuncNo		/* Function number of matching dev */
    )
    {
    int intBusNo;		/* VxWorks returns "int" values */
    int intDeviceNo;
    int intFuncNo;

    
    /* Use the VxWorks PCI config. library to find a device within the
    specified class. */

    if (pciFindClass ((pciClass << 16) | (subClass << 8) | pgmIf, index,
	&intBusNo, &intDeviceNo, &intFuncNo) != OK)
	{
	return FALSE;
	}
    else
	{
	if (pBusNo)
	     *pBusNo = (UINT8) intBusNo;
	if (pDeviceNo)
	     *pDeviceNo = (UINT8) intDeviceNo;
	if (pFuncNo)
	     *pFuncNo = (UINT8) intFuncNo;
	}

    return TRUE;
    }


/***************************************************************************
*
* usbPciByteGet - Returns a UINT8 configuration value
*
* This function returns the UINT8 value at offset <regOffset> from 
* the PCI configuration space of the device identified by <busNo>, 
* <deviceNo>, and <funcNo>.
*
* RETURNS: UINT8 value read from device configuration space
*/

UINT8 usbPciByteGet 
    (
    UINT8 busNo,		/* Bus number of device */
    UINT8 deviceNo,		/* Device number of device */
    UINT8 funcNo,		/* Function number of device */
    UINT16 regOffset		/* Offset into PCI config space */
    )
    {
    UINT8 value;

    if (pciConfigInByte (busNo, deviceNo, funcNo, regOffset, &value) != OK)
	return 0;

    return value;
    }


/***************************************************************************
*
* usbPciWordGet - Returns a UINT16 configuration value
*
* This function returns the UINT16 value at offset <regOffset> from 
* the PCI configuration space of the device identified by <busNo>, 
* <deviceNo>, and <funcNo>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: UINT16 value read from device configuration space
*/

UINT32 usbPciWordGet
    (
    UINT8 busNo,		/* Bus number of device */
    UINT8 deviceNo,		/* Device number of device */
    UINT8 funcNo,		/* Function number of device */
    UINT16 regOffset		/* Offset into PCI config space */
    )
    {
    UINT16 value;

    if (pciConfigInWord (busNo, deviceNo, funcNo, regOffset, &value) != OK)
	return 0;

    return value;
    }


/***************************************************************************
*
* usbPciDwordGet - Returns a UINT32 configuration value
*
* This function returns the UINT32 value at offset <regOffset> from 
* the PCI configuration space of the device identified by <busNo>, 
* <deviceNo>, and <funcNo>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: UINT32 value read from device configuration space
*/

UINT32 usbPciDwordGet
    (
    UINT8 busNo,		/* Bus number of device */
    UINT8 deviceNo,		/* Device number of device */
    UINT8 funcNo,		/* Function number of device */
    UINT16 regOffset		/* Offset into PCI config space */
    )
    {
    UINT32 value;

    if (pciConfigInLong (busNo, deviceNo, funcNo, regOffset, &value) != OK)
	return 0;

    return value;
    }


/***************************************************************************
*
* usbPciConfigHeaderGet - Reads a device's PCI configuration header
*
* This function reads the PCI configuration header for the device
* identified by <busNo>, <deviceNo>, and <funcNo>.  The configuration
* header is stored in the <pCfgHdr> buffer provided by the caller.
*
* This function initializes the <pCfgHdr> structure to zeros.  Any 
* fields which cannot be read from the device's configuration header 
* will remain zero upon return.  This function does not attempt to read
* fields defined as "reserved" in the PCI configuration header.
*
* RETURNS: N/A
*/

VOID usbPciConfigHeaderGet
    (
    UINT8 busNo,		/* Bus number of device */
    UINT8 deviceNo,		/* Device number of device */
    UINT8 funcNo,		/* Function number of device */
    pPCI_CFG_HEADER pCfgHdr	/* Buffer provided by caller */
    )
    {
    int i;

    /* Do nothing if CfgHdr is NULL */

    if (pCfgHdr == NULL)
	return;


    /* Initialize the buffer to zeros. */

    memset (pCfgHdr, 0, sizeof (*pCfgHdr));


    /* Read and store each field in the PCI configuration header. */

    pCfgHdr->vendorId	= usbPciWordGet (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID);
    pCfgHdr->deviceId	= usbPciWordGet (busNo, deviceNo, funcNo, PCI_CFG_DEVICE_ID);
    pCfgHdr->command	= usbPciWordGet (busNo, deviceNo, funcNo, PCI_CFG_COMMAND);
    pCfgHdr->status	= usbPciWordGet (busNo, deviceNo, funcNo, PCI_CFG_STATUS);
    pCfgHdr->revisionId = usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_REVISION);
    pCfgHdr->pgmIf	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_PROGRAMMING_IF);
    pCfgHdr->subClass	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_SUBCLASS);
    pCfgHdr->pciClass	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_CLASS);
    pCfgHdr->cacheLineSize = usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_CACHE_LINE_SIZE);
    pCfgHdr->latencyTimer = usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_LATENCY_TIMER);
    pCfgHdr->headerType = usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE);
    pCfgHdr->bist	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_BIST);

    for (i = 0; i < PCI_CFG_NUM_BASE_REG; i++)
	pCfgHdr->baseReg [i] = usbPciDwordGet (busNo, deviceNo, funcNo, 
	    PCI_CFG_BASE_ADDRESS_0 + i * sizeof (UINT32));

    pCfgHdr->romBase	= usbPciDwordGet (busNo, deviceNo, funcNo, PCI_CFG_EXPANSION_ROM);
    pCfgHdr->intLine	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_DEV_INT_LINE);
    pCfgHdr->intPin	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_DEV_INT_PIN);
    pCfgHdr->minGrant	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_MIN_GRANT);
    pCfgHdr->maxLatency = usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_MAX_LATENCY);
    }


/***************************************************************************
*
* usbPciByteIn - input a byte from PCI I/O space
*
* Inputs a byte from a PCI I/O address <address>.
*
* RETURNS: byte input from i/o address
*/

UINT8 usbPciByteIn
    (
    UINT32 address		/* PCI I/O address */
    )

    {
    return USB_PCI_IN_BYTE (address);
    }


/***************************************************************************
*
* usbPciWordIn - input a word from PCI I/O space
*
* Inputs a word from a PCI I/O address <address>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: word input from i/o address
*/

UINT16 usbPciWordIn
    (
    UINT32 address		/* PCI I/O address */
    )

    {
    UINT16 w = USB_PCI_IN_WORD (address);
    return FROM_LITTLEW (w);
    }


/***************************************************************************
*
* usbPciDwordIn - input a dword from PCI I/O space
*
* Inputs a dword from a PCI I/O address <address>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: dword input from i/o address
*/

UINT32 usbPciDwordIn
    (
    UINT32 address		/* PCI I/O address */
    )

    {
    UINT32 l = USB_PCI_IN_DWORD (address);
    return FROM_LITTLEL (l);
    }


/***************************************************************************
*
* usbPciByteOut - output a byte to PCI I/O space
*
* Outputs <value> to the PCI I/O address <address>.
*
* RETURNS: N/A
*/

VOID usbPciByteOut
    (
    UINT32 address,		/* PCI I/O address */
    UINT8 value 		/* value */
    )

    {
    USB_PCI_OUT_BYTE (address, value);
    CACHE_PIPE_FLUSH ();
    }


/***************************************************************************
*
* usbPciWordOut - outputs a word to PCI I/O space
*
* Outputs <value> to the PCI I/O address <address>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: N/A
*/

VOID usbPciWordOut
    (
    UINT32 address,		/* PCI I/O address */
    UINT16 value		/* value */
    )

    {
    UINT16 w = TO_LITTLEW (value);
    USB_PCI_OUT_WORD (address, w);
    CACHE_PIPE_FLUSH ();
    }


/***************************************************************************
*
* usbPciDwordOut - outputs a dword to PCI I/O space
*
* Outputs <value> to the PCI I/O address <address>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: N/A
*/

VOID usbPciDwordOut
    (
    UINT32 address,		/* PCI I/O address */
    UINT32 value		/* value */
    )

    {
    UINT32 l = TO_LITTLEL (value);
    USB_PCI_OUT_DWORD (address, l);
    CACHE_PIPE_FLUSH ();
    }


/***************************************************************************
*
* usbPciMemioOffset - Return PCI MEMIO to CPU MEMIO offset
*
* For memory-mapped I/O, the CPU's view of a memory address may not be the
* same as that programmed into the base address register of a PCI adapter.
* The CPU should add the value returned by this function to the BAR in order
* to produce the correct CPU-visible memory address.
*
* RETURNS: USB_PCI_MEMIO_OFFSET
*/

UINT32 usbPciMemioOffset (void)
    {
    return USB_PCI_MEMIO_OFFSET;
    }


/***************************************************************************
*
* usbMemToPci - Convert a memory address to a PCI-reachable memory address
*
* Converts <pMem> to an equivalent 32-bit memory address visible from the 
* PCI bus.  This conversion is necessary to allow PCI bus masters to address
* the same memory viewed by the processor.
*
* RETURNS: converted memory address
*/

UINT32 usbMemToPci
    (
    pVOID pMem			/* memory address to convert */
    )

    {
    pVOID pPhys;
    
    /* The conversion is a two-step process.  First, we need to convert the
     * logical processor address (virtual) to a physical address.  Then, we
     * convert the physical address to one which can be seen from the PCI
     * bus.
     */

    pPhys = CACHE_DRV_VIRT_TO_PHYS (&cacheUserFuncs, pMem);

    return ((UINT32) pPhys) + USB_PCI_MEM_OFFSET;

    }


/***************************************************************************
*
* usbPciToMem - Convert a PCI-reachable address to a CPU-reachable pointer
*
* Converts <pciAdrs> to an equivalent CPU memory address.  
*
* RETURNS: pointer to PCI memory
*/

pVOID usbPciToMem
    (
    UINT32 pciAdrs		/* 32-bit PCI address to be converted */
    )

    {

    return CACHE_DRV_PHYS_TO_VIRT (&cacheUserFuncs, 
	(void *) (pciAdrs - USB_PCI_MEM_OFFSET));

    }


/***************************************************************************
*
* usbPciMemInvalidate - Invalidate cache for a region of memory
*
* When another bus master, such as a PCI bus master, writes to memory, the
* cache may need to be invalidated for that region of memory.
*
* NOTE: Returns immediately if size == 0.
*
* RETURNS: N/A
*/

VOID usbPciMemInvalidate
    (
    pVOID pMem, 		/* base of memory region to invalidate */
    UINT32 size 		/* size of region to invalidate */
    )

    {
    if (size != 0)
	CACHE_USER_INVALIDATE (pMem, size);
    }


/***************************************************************************
*
* usbPciMemFlush - Flush a region of memory through the cache
*
* In systems which implement a non-write-thru cache, the processor may have
* written data to memory which has not yet been flushed to the actual system
* memory.  Before other bus masters may interrogate this memory, it may be
* necessary to flush the cache.
*
* NOTE: Returns immediately if size == 0.
*
* RETURNS: N/A
*/

VOID usbPciMemFlush
    (
    pVOID pMem, 		/* base of memory region to invalidate */
    UINT32 size 		/* size of region to invalidate */
    )

    {
    if (size != 0)
	CACHE_USER_FLUSH (pMem, size);
    }


/***************************************************************************
*
* usbPciIntConnect - Connect to a interrupt vector
*
* Connects the <func> to the interrupt number <intNo>.	<param> is an
* application-specific value which will be passed to <func> each time
* the interrupt handler is invoked.  
*
* RETURNS: OK, or ERROR if unable to connect/enable interrupt
*/

STATUS usbPciIntConnect
    (
    INT_HANDLER_PROTOTYPE func,     /* new interrupt handler */
    pVOID param,		    /* parameter for int handler */
    UINT16 intNo		    /* interrupt vector number */
    )

    {
    if (USB_INT_CONNECT (intNo, func, param) != OK)
	return ERROR;

    if (USB_INT_ENABLE (intNo) != OK)
	{
	USB_INT_DISCONNECT (intNo, func, param);
	return ERROR;
	}

    if (intNo < USB_MAX_INT_NO) 
	intUsage [intNo]++;

    return OK;
    }


/***************************************************************************
*
* usbPciIntDisconnect - Removes an interrupt handler
*
* Removes an interrupt handler installed by usbPciIntConnect().  <func>,
* <param>, and <intNo> must match the corresponding parameters from an earlier 
* call to usbPciIntConnect().
*
* RETURNS: N/A
*/

VOID usbPciIntRestore
    (
    INT_HANDLER_PROTOTYPE func,     /* int handler to be removed */
    pVOID param,		    /* parameter for int handler */
    UINT16 intNo		    /* interrupt vector number */
    )

    {
    if (intNo >= USB_MAX_INT_NO || 
	(intUsage [intNo] != 0 && --intUsage [intNo] == 0))
	{
	USB_INT_DISABLE (intNo);
	}

    USB_INT_DISCONNECT (intNo, func, param);
    }


/* End of file. */

