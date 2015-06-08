/* pciConfigShow.c - Show routines of PCI bus(IO mapped) library */

/* Copyright 1984-2000 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,13nov01,tor  Assorted compiler warnings.
01n,25oct01,tor  Add pciConfigTopoShow() and support functions
01m,26feb01,rcs  merge from tornado 3 version main/tor3_x/3
01l,05dec00,dat  merge from sustaining branch to tor2_0_x
01k,16nov00,dat  SPR 36081 driver documentation
01j,17aug00,dat  SPR 33788 DeviceShow does not scan all possible devices
01i,19may00,pai  removed INCLUDE_SHOW_ROUTINES build condition (SPR 27759).
01h,17mar98,tm   documentation cleanup; added drv/pci/.h and config.h includes
01g,11mar98,tm   renamed to pciConfigShow.c from pciIomapShow.c
                 added return to null init routine
01f,04mar98,tm   added include of pciHeaderDefs.h, pciIomapShow.h
01e,28mar97,mas  added IMPORT of pciConfigMech, include of dllLib.h; fixed
		 class display in pciDeviceShow() (SPR 8226).
01d,12jan97,hdn  changed member/variable name "vender" to "vendor".
01c,12jan97,hdn  changed member/variable name "class" to "classCode".
01b,14mar96,hdn  re-written.  changed parameters of the functions.
		 removed BIOS dependent codes.
01a,25feb95,bcs  written
*/


/*
DESCRIPTION

This module contains show routines to see all devices and bridges on the PCI bus.
This module works in conjunction with pciConfigLib.o.
There are two ways to find out an empty device.

.IP " - "
check Master Abort bit after the access.
.IP " - "
check whether the read value is 0xffff.
.LP

It uses the second method, since I didn't see the Master Abort bit of
the host/PCI bridge changing.

*/



#include "vxWorks.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "dllLib.h"
#include "config.h"
#include "pciClass.h"
#include "drv/pci/pciConfigLib.h"
#include "drv/pci/pciConfigShow.h"
#include "drv/pci/pciHeaderDefs.h"


/* defines */


/* externs */

IMPORT int pciLibInitDone;
IMPORT int pciLibInitStatus;
IMPORT int pciConfigMech;


/* globals */


/* locals */


/* forward declarations */

LOCAL void pciDheaderPrint	(PCI_HEADER_DEVICE * pD);
LOCAL void pciBheaderPrint	(PCI_HEADER_BRIDGE * pB);


/*******************************************************************************
*
* pciConfigShowInit - initialize the show routines.
*
* This routine is used to pull in all routines in this library.
*
* NOMANUAL
* 
* RETURNS: N/A
*/

void pciConfigShowInit (void)
    {
    return;
    }

/*******************************************************************************
*
* pciDeviceShow - print information about PCI devices
*
* This routine prints information about PCI devices
* There are two ways to find out an empty device.
*
* .IP " - "
* check Master Abort bit after the access.
* .IP " - "
* check whether the read value is 0xffff.
* .LP
*
* It uses the second method, since I didn't see the Master Abort bit of
* the host/PCI bridge changing.
*
* RETURNS: OK, or ERROR if the library is not initialized.
*/

STATUS pciDeviceShow
    (
    int	busNo		/* bus number */
    )
    {
    int deviceNo;
    UINT16 vendorId;
    UINT16 deviceId;
    union {
	UINT32 classCode;
	UINT8 array[4];
	} u;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    if (pciConfigMech == PCI_MECHANISM_2
     && PCI_MAX_DEV > 16)
        {
        printf ("Invalid configuration. PCI_MAX_DEV > 16, PCI mechanism #2\n");
        return ERROR;
        }

    printf ("Scanning function 0 of each PCI device on bus %d\n", busNo);
    printf ("Using configuration mechanism %d\n", pciConfigMech);
    printf ("bus       device    function  vendorID  deviceID  class\n");

     for (deviceNo=0; deviceNo < PCI_MAX_DEV; deviceNo++)
	{
	pciConfigInWord (busNo, deviceNo, 0, PCI_CFG_VENDOR_ID, 
			(UINT16 *)&vendorId);
	pciConfigInWord (busNo, deviceNo, 0, PCI_CFG_DEVICE_ID, 
			(UINT16 *)&deviceId);
	pciConfigInByte (busNo, deviceNo, 0, PCI_CFG_PROGRAMMING_IF, 
			&u.array[3]);
	pciConfigInByte (busNo, deviceNo, 0, PCI_CFG_SUBCLASS,
			&u.array[2]);
	pciConfigInByte (busNo, deviceNo, 0, PCI_CFG_CLASS,
			&u.array[1]);
	u.array[0] = 0;

        /*
	 * There are two ways to find out an empty device.
	 * 1. check Master Abort bit after the access.
	 * 2. check whether the read value is 0xffff.
	 * Since I didn't see the Master Abort bit of the host/PCI bridge
	 * changing, I use the second method.
	 */

	if (vendorId != 0xffff)
	    printf ("%.8x  %.8x  %.8x  %.8x  %.8x  %.8x\n",
		    busNo, deviceNo, 0, vendorId, deviceId, u.classCode);
	}

    return (OK);
    }

/*******************************************************************************
*
* pciHeaderShow - print a header of the specified PCI device
*
* This routine prints a header of the PCI device specified by busNo, deviceNo,
* and funcNo.
*
* RETURNS: OK, or ERROR if this library is not initialized.
*
*/

STATUS pciHeaderShow
    (
    int	busNo,		/* bus number */
    int	deviceNo,	/* device number */
    int	funcNo		/* function number */
    )
    {
    PCI_HEADER_DEVICE headerDevice;
    PCI_HEADER_BRIDGE headerBridge;
    PCI_HEADER_DEVICE * pD = &headerDevice;
    PCI_HEADER_BRIDGE * pB = &headerBridge;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, 
		     (UINT8 *)&pD->headerType);

    if (pD->headerType & 0x01)		/* PCI-to-PCI bridge */
	{
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID, 
			 (UINT16 *)&pB->vendorId);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_DEVICE_ID, 
			 (UINT16 *)&pB->deviceId);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_COMMAND, 
			 (UINT16 *)&pB->command);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_STATUS, 
			 (UINT16 *)&pB->status);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_REVISION, 
			 (UINT8 *)&pB->revisionId);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_PROGRAMMING_IF, 
			 (UINT8 *)&pB->progIf);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_SUBCLASS, 
			 (UINT8 *)&pB->subClass);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_CLASS, 
			 (UINT8 *)&pB->classCode);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_CACHE_LINE_SIZE, 
			 (UINT8 *)&pB->cacheLine);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_LATENCY_TIMER, 
			 (UINT8 *)&pB->latency);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, 
			 (UINT8 *)&pB->headerType);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_BIST, 
			 (UINT8 *)&pB->bist);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_0, 
			 (UINT32 *)&pB->base0);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_1, 
			 (UINT32 *)&pB->base1);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_PRIMARY_BUS, 
			 (UINT8 *)&pB->priBus);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_SECONDARY_BUS, 
			 (UINT8 *)&pB->secBus);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_SUBORDINATE_BUS, 
			 (UINT8 *)&pB->subBus);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_SEC_LATENCY, 
			 (UINT8 *)&pB->secLatency);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_IO_BASE, 
			 (UINT8 *)&pB->ioBase);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_IO_LIMIT, 
			 (UINT8 *)&pB->ioLimit);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_SEC_STATUS, 
			 (UINT16 *)&pB->secStatus);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_MEM_BASE, 
			 (UINT16 *)&pB->memBase);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_MEM_LIMIT, 
			 (UINT16 *)&pB->memLimit);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_PRE_MEM_BASE, 
			 (UINT16 *)&pB->preBase);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_PRE_MEM_LIMIT, 
			 (UINT16 *)&pB->preLimit);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_PRE_MEM_BASE_U, 
			 (UINT32 *)&pB->preBaseUpper);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_PRE_MEM_LIMIT_U, 
			 (UINT32 *)&pB->preLimitUpper);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_IO_BASE_U, 
			 (UINT16 *)&pB->ioBaseUpper);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_IO_LIMIT_U, 
			 (UINT16 *)&pB->ioLimitUpper);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_ROM_BASE, 
			 (UINT32 *)&pB->romBase);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_BRG_INT_LINE, 
			 (UINT8 *)&pB->intLine);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_BRG_INT_PIN, 
			 (UINT8 *)&pB->intPin);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_BRIDGE_CONTROL, 
			 (UINT16 *)&pB->control);
        pciBheaderPrint (pB);
	}
    else					/* PCI device */
	{
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID, 
			 (UINT16 *)&pD->vendorId);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_DEVICE_ID, 
			 (UINT16 *)&pD->deviceId);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_COMMAND, 
			 (UINT16 *)&pD->command);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_STATUS, 
			 (UINT16 *)&pD->status);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_REVISION, 
			 (UINT8 *)&pD->revisionId);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_PROGRAMMING_IF, 
			 (UINT8 *)&pD->progIf);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_SUBCLASS, 
			 (UINT8 *)&pD->subClass);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_CLASS, 
			 (UINT8 *)&pD->classCode);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_CACHE_LINE_SIZE, 
			 (UINT8 *)&pD->cacheLine);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_LATENCY_TIMER, 
			 (UINT8 *)&pD->latency);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, 
			 (UINT8 *)&pD->headerType);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_BIST, 
			 (UINT8 *)&pD->bist);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_0, 
			 (UINT32 *)&pD->base0);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_1, 
			 (UINT32 *)&pD->base1);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_2, 
			 (UINT32 *)&pD->base2);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_3, 
			 (UINT32 *)&pD->base3);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_4, 
			 (UINT32 *)&pD->base4);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_5, 
			 (UINT32 *)&pD->base5);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_CIS, 
			 (UINT32 *)&pD->cis);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_SUB_VENDER_ID, 
			 (UINT16 *)&pD->subVendorId);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_SUB_SYSTEM_ID, 
			 (UINT16 *)&pD->subSystemId);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_EXPANSION_ROM, 
			 (UINT32 *)&pD->romBase);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_DEV_INT_LINE, 
			 (UINT8 *)&pD->intLine);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_DEV_INT_PIN, 
			 (UINT8 *)&pD->intPin);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_MIN_GRANT, 
			 (UINT8 *)&pD->minGrant);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_MAX_LATENCY, 
			 (UINT8 *)&pD->maxLatency);
        pciDheaderPrint (pD);
	}

    return (OK);
    }

/*******************************************************************************
*
* pciFindDeviceShow - find a device by deviceId, then print an information.
*
* This routine finds a device by deviceId, then print an information.
*
* RETURNS: OK, or ERROR if this library is not initialized.
*
*/

STATUS pciFindDeviceShow
    (
    int	vendorId,	/* vendor ID */
    int	deviceId,	/* device ID */
    int	index		/* desired instance of device */
    )
    {
    int busNo;
    int deviceNo;
    int funcNo;

    if (pciFindDevice (vendorId, deviceId, index, &busNo, &deviceNo, &funcNo)
	== OK)
	{
	printf ("deviceId = 0x%.8x\n", deviceId);
	printf ("vendorId = 0x%.8x\n", vendorId);
	printf ("index =    0x%.8x\n", index);
	printf ("busNo =    0x%.8x\n", busNo);
	printf ("deviceNo = 0x%.8x\n", deviceNo);
	printf ("funcNo =   0x%.8x\n", funcNo);
	return (OK);
	}
    return (ERROR);
    }

/*******************************************************************************
*
* pciFindClassShow - find a device by 24-bit class code
*
* This routine finds a device by its 24-bit PCI class code, then prints its
* information.
*
* RETURNS: OK, or ERROR if this library is not initialized.
*
*/

STATUS pciFindClassShow
    (
    int	classCode,	/* 24-bit class code */
    int	index		/* desired instance of device */
    )
    {
    int busNo;
    int deviceNo;
    int funcNo;

    if (pciFindClass (classCode, index, &busNo, &deviceNo, &funcNo) == OK)
	{
	printf ("class code = 0x%.8x\n", classCode);
	printf ("index =      0x%.8x\n", index);
	printf ("busNo =      0x%.8x\n", busNo);
	printf ("deviceNo =   0x%.8x\n", deviceNo);
	printf ("funcNo =     0x%.8x\n", funcNo);
	return (OK);
	}
    return (ERROR);
    }

/*******************************************************************************
*
* pciDheaderPrint - print a PCI device header
*
* This routine prints a PCI device header.
*
* RETURNS: N/A
*
*/

LOCAL void pciDheaderPrint
    (
    PCI_HEADER_DEVICE * pD
    )
    {
    printf ("vendor ID =                   0x%.4x\n", (ushort_t)pD->vendorId);
    printf ("device ID =                   0x%.4x\n", (ushort_t)pD->deviceId);
    printf ("command register =            0x%.4x\n", (ushort_t)pD->command);
    printf ("status register =             0x%.4x\n", (ushort_t)pD->status);	
    printf ("revision ID =                 0x%.2x\n", (uchar_t)pD->revisionId);
    printf ("class code =                  0x%.2x\n", (uchar_t)pD->classCode);	
    printf ("sub class code =              0x%.2x\n", (uchar_t)pD->subClass);
    printf ("programming interface =       0x%.2x\n", (uchar_t)pD->progIf);	
    printf ("cache line =                  0x%.2x\n", (uchar_t)pD->cacheLine);
    printf ("latency time =                0x%.2x\n", (uchar_t)pD->latency);
    printf ("header type =                 0x%.2x\n", (uchar_t)pD->headerType);
    printf ("BIST =                        0x%.2x\n", (uchar_t)pD->bist);	
    printf ("base address 0 =              0x%.8x\n", pD->base0);	
    printf ("base address 1 =              0x%.8x\n", pD->base1);	
    printf ("base address 2 =              0x%.8x\n", pD->base2);	
    printf ("base address 3 =              0x%.8x\n", pD->base3);	
    printf ("base address 4 =              0x%.8x\n", pD->base4);	
    printf ("base address 5 =              0x%.8x\n", pD->base5);	
    printf ("cardBus CIS pointer =         0x%.8x\n", pD->cis);	
    printf ("sub system vendor ID =        0x%.4x\n", (ushort_t)pD->subVendorId);
    printf ("sub system ID =               0x%.4x\n", (ushort_t)pD->subSystemId);
    printf ("expansion ROM base address =  0x%.8x\n", pD->romBase);
    printf ("interrupt line =              0x%.2x\n", (uchar_t)pD->intLine);
    printf ("interrupt pin =               0x%.2x\n", (uchar_t)pD->intPin);	
    printf ("min Grant =                   0x%.2x\n", (uchar_t)pD->minGrant);
    printf ("max Latency =                 0x%.2x\n", (uchar_t)pD->maxLatency);
    }

/*******************************************************************************
*
* pciBheaderPrint - print a PCI-to-PCI bridge header
*
* This routine prints a PCI-to-PCI bridge header.
*
* RETURNS: N/A
*
*/

LOCAL void pciBheaderPrint
    (
    PCI_HEADER_BRIDGE * pB
    )
    {
    printf ("vendor ID =                   0x%.4x\n", (ushort_t)pB->vendorId);
    printf ("device ID =                   0x%.4x\n", (ushort_t)pB->deviceId);
    printf ("command register =            0x%.4x\n", (ushort_t)pB->command);
    printf ("status register =             0x%.4x\n", (ushort_t)pB->status);	
    printf ("revision ID =                 0x%.2x\n", (uchar_t)pB->revisionId);
    printf ("class code =                  0x%.2x\n", (uchar_t)pB->classCode);	
    printf ("sub class code =              0x%.2x\n", (uchar_t)pB->subClass);
    printf ("programming interface =       0x%.2x\n", (uchar_t)pB->progIf);	
    printf ("cache line =                  0x%.2x\n", (uchar_t)pB->cacheLine);
    printf ("latency time =                0x%.2x\n", (uchar_t)pB->latency);
    printf ("header type =                 0x%.2x\n", (uchar_t)pB->headerType);
    printf ("BIST =                        0x%.2x\n", (uchar_t)pB->bist);
    printf ("base address 0 =              0x%.8x\n", pB->base0);	
    printf ("base address 1 =              0x%.8x\n", pB->base1);	
    printf ("primary bus number =          0x%.2x\n", (uchar_t)pB->priBus);	
    printf ("secondary bus number =        0x%.2x\n", (uchar_t)pB->secBus);	
    printf ("subordinate bus number =      0x%.2x\n", (uchar_t)pB->subBus);	
    printf ("secondary latency timer =     0x%.2x\n", (uchar_t)pB->secLatency);
    printf ("IO base =                     0x%.2x\n", (uchar_t)pB->ioBase);
    printf ("IO limit =                    0x%.2x\n", (uchar_t)pB->ioLimit);
    printf ("secondary status =            0x%.4x\n", (ushort_t)pB->secStatus);
    printf ("memory base =                 0x%.4x\n", (ushort_t)pB->memBase);
    printf ("memory limit =                0x%.4x\n", (ushort_t)pB->memLimit);	
    printf ("prefetch memory base =        0x%.4x\n", (ushort_t)pB->preBase);
    printf ("prefetch memory limit =       0x%.4x\n", (ushort_t)pB->preLimit);
    printf ("prefetch memory base upper =  0x%.8x\n", pB->preBaseUpper);
    printf ("prefetch memory limit upper = 0x%.8x\n", pB->preLimitUpper);
    printf ("IO base upper 16 bits =       0x%.4x\n", (ushort_t)pB->ioBaseUpper);
    printf ("IO limit upper 16 bits =      0x%.4x\n", (ushort_t)pB->ioLimitUpper);
    printf ("expansion ROM base address =  0x%.8x\n", pB->romBase);
    printf ("interrupt line =              0x%.2x\n", (uchar_t)pB->intLine);
    printf ("interrupt pin =               0x%.2x\n", (uchar_t)pB->intPin);	
    printf ("bridge control =              0x%.4x\n", (ushort_t)pB->control);
    }

/*********************************************************************
*
* pciFuncBarShow - show the value contained in a BAR
*
* This function decodes the value of a single BAR on a single
* bus,device,function, and prints the information to the terminal.
*
* RETURNS: N/A.
*/

LOCAL void pciFuncBarShow
    (
    int bus,			/* bus */
    int device,			/* device */
    int function,		/* function */
    int barNo			/* BAR index */
    )
    {
    UINT32 barVal;
    int space;
    int prefetch;
    int offset;

    offset = PCI_CFG_BASE_ADDRESS_0 + (barNo * sizeof(UINT32));
    pciConfigInLong(bus, device, function,
	    	offset, &barVal);

    /* check if BAR implemented */
    if ( barVal == 0 || barVal == 0xffffffff )
	return;

    if ( ( barVal & 0x01 ) == 0x01 )
	printf("\tbar%d in I/O space @ 0x%08x\n",
			barNo, (barVal & (~0x00000001)));
    else
	{
	prefetch = ( barVal >> 3 ) & 0x01;
	space = (barVal >> 1 ) & 0x03;
	barVal = barVal & ~ 0x0f;
	printf("\tbar%d in %s%s mem space @ 0x%08x\n", barNo,
	    prefetch ? "prefetchable " : "",
	    ( space == 0x00 ) ? "32-bit" :
	    ( space == 0x02) ? "64-bit" :
	    "reserved",
	    barVal);
	}
    }

/*********************************************************************
*
* pciStatusWordShow - show the decoded value of the status word.
*
* This routine reads the status word from the specified
* bus,device,function, and prints it in a human-readable format.
*
* RETURNS: OK, always.
*/

LOCAL STATUS pciStatusWordShow
    (
    int bus,		/* bus */
    int device,		/* device */
    int function	/* function */
    )
    {
    UINT16 status;

    pciConfigInWord (bus, device, function, PCI_CFG_STATUS, &status);

    printf("\tstatus=0x%04x (", status);

    if ( status & PCI_STATUS_NEW_CAP )
	printf(" CAP");

    if ( status & PCI_STATUS_66_MHZ )
	printf(" 66MHZ");

    if ( status & PCI_STATUS_UDF )
	printf(" UDF");

    if ( status & PCI_STATUS_FAST_BB )
	printf(" FBTB");

    if ( status & PCI_STATUS_DATA_PARITY_ERR )
	printf(" DATA_PARITY_ERR");

    printf(" DEVSEL=%x", ((status & 0x0600) >> 9 ));

    if ( status & PCI_STATUS_TARGET_ABORT_GEN )
	printf(" TGT_ABORT_GEN");

    if ( status & PCI_STATUS_TARGET_ABORT_RCV )
	printf(" TGT_ABORT_RCV");

    if ( status & PCI_STATUS_MASTER_ABORT_RCV )
	printf(" MSTR_ABORT_RCV");

    if ( status & PCI_STATUS_ASSERT_SERR )
	printf(" ASSERT_SERR");

    if ( status & PCI_STATUS_PARITY_ERROR )
	printf(" PARITY_ERR");

    printf(" )\n");

    return(OK);
    }

/*********************************************************************
*
* pciCmdWordShow - show the decoded value of the command word
*
* This routine reads the command word from the specified
* bus,device,function, and prints the value in a human readable
* format.
*
* RETURNS: OK, always.
*/

LOCAL STATUS pciCmdWordShow
    (
    int bus,		/* bus */
    int device,		/* device */
    int function	/* function */
    )
    {
    UINT16 command;

    pciConfigInWord (bus,device,function,
		PCI_CFG_COMMAND, &command);

    printf("\tcommand=0x%04x (", command);

    if ( command & PCI_CMD_IO_ENABLE )
	printf(" IO_ENABLE");

    if ( command & PCI_CMD_MEM_ENABLE )
	printf(" MEM_ENABLE");

    if ( command & PCI_CMD_MASTER_ENABLE )
	printf(" MASTER_ENABLE");

    if ( command & PCI_CMD_MON_ENABLE )
	printf(" MON_ENABLE");

    if ( command & PCI_CMD_WI_ENABLE )
	printf(" WI_ENABLE");

    if ( command & PCI_CMD_SNOOP_ENABLE )
	printf(" SNOOP_ENABLE");

    if ( command & PCI_CMD_PERR_ENABLE )
	printf(" PERR_ENABLE");

    if ( command & PCI_CMD_WC_ENABLE )
	printf(" WC_ENABLE");

    if ( command & PCI_CMD_SERR_ENABLE )
	printf(" SERR_ENABLE");

    if ( command & PCI_CMD_FBTB_ENABLE )
	printf(" FBTB_ENABLE");

    printf(" )\n");
    return(OK);
    }

/*********************************************************************
*
* pciConfigStatusWordShow - show the decoded value of the status word
*
* This routine reads the value of the status word for the specified
* bus,device,function and prints the value in a human-readable format.
*
* RETURNS: OK, always.
*/

STATUS pciConfigStatusWordShow
    (
    int bus,		/* bus */
    int device,		/* device */
    int function,	/* function */
    void *pArg		/* ignored */
    )
    {
    printf("[%d,%d,%d] ",bus,device,function);
    pciStatusWordShow(bus,device,function);
    return(OK);
    }

/*********************************************************************
*
* pciConfigCmdWordShow - show the decoded value of the command word
*
* This routine reads the value of the command word for the specified
* bus,device,function and prints the value in a human-readable format.
*
* RETURNS: OK, always.
*/

STATUS pciConfigCmdWordShow
    (
    int bus,		/* bus */
    int device,		/* device */
    int function,	/* function */
    void *pArg		/* ignored */
    )
    {
    printf("[%d,%d,%d] ",bus,device,function);
    pciCmdWordShow(bus,device,function);
    return(OK);
    }

/*********************************************************************
*
* pciConfigFuncShow - show configuration details about a function
*
* This routine reads various information from the specified
* bus,device,function, and prints the information in a human-readable
* format.
*
* RETURNS: OK, always.
*/

STATUS pciConfigFuncShow
    (
    int bus,		/* bus */
    int device,		/* device */
    int function,	/* function */
    void *pArg		/* ignored */
    )
    {
    UINT8  clsCode;
    UINT8  subClass;
    UINT8  secBus;
    int    numBars = 6;	/* most devices have 6, but not bridges */
    UINT16 memBase;
    UINT16 memLimit;
    UINT32 memBaseU;
    UINT32 memLimitU;
    UINT8  ioBase;
    UINT8  ioLimit;
    UINT16 ioBaseU;
    UINT16 ioLimitU;
    UINT8  headerType;
    UINT16 cmdReg;

    printf("[%d,%d,%d] type=",bus,device,function);
    pciConfigInByte (bus, device, function,
			     PCI_CFG_CLASS, &clsCode);

    pciConfigInByte(bus,device,function,
			     PCI_CFG_HEADER_TYPE, &headerType);

    if ( ( headerType & PCI_HEADER_TYPE_MASK ) == 1 )
	{
	/* type 1 header has only 2 BARs */
	numBars = 2;
	}

    switch (clsCode)
        {
	case PCI_CLASS_PRE_PCI20: printf("BEFORE_STD\n"); break;
	case PCI_CLASS_MASS_STORAGE: printf("MASS STORAGE\n"); break;
	case PCI_CLASS_NETWORK_CTLR: printf("NET_CNTLR\n"); break;
	case PCI_CLASS_DISPLAY_CTLR: printf("DISP_CNTLR\n"); break;
	case PCI_CLASS_MMEDIA_DEVICE: printf("MULTI_MEDIA\n"); break;
	case PCI_CLASS_MEM_CTLR: printf("MEM_CNTLR\n"); break;
	case PCI_CLASS_COMM_CTLR: printf("COMMUNICATION\n"); break;
	case PCI_CLASS_BASE_PERIPH: printf("PERIPHERAL\n"); break;
	case PCI_CLASS_INPUT_DEVICE: printf("INPUT\n"); break;
	case PCI_CLASS_DOCK_DEVICE: printf("DOCKING STATION\n"); break;
	case PCI_CLASS_PROCESSOR: printf("PROCESSOR\n"); break;
	case PCI_CLASS_SERIAL_BUS: printf("SERIAL BUS\n"); break;
	case PCI_CLASS_WIRELESS: printf("WIRELESS\n"); break;
	case PCI_CLASS_INTLGNT_IO: printf("INTELLIGENT_IO\n"); break;
	case PCI_CLASS_SAT_COMM: printf("SATELLITE\n"); break;
	case PCI_CLASS_EN_DECRYPTION: printf("ENCRYPTION DEV\n"); break;
	case PCI_CLASS_DAQ_DSP: printf("DATA ACQUISITION DEV\n"); break;
	case PCI_CLASS_UNDEFINED: printf("OTHER DEVICE\n"); break;

	case PCI_CLASS_BRIDGE_CTLR:
	    secBus = 0;
	    pciConfigInByte (bus, device, function,
				 PCI_CFG_SUBCLASS, &subClass);

	    switch (subClass)
		{
		case PCI_SUBCLASS_HOST_PCI_BRIDGE: 
		    printf("HOST"); 
		    break;

		case PCI_SUBCLASS_ISA_BRIDGE:
		    printf("ISA");
		    break;

		case PCI_SUBCLASS_BRDG_EISA:
		    printf("EISA");
		    break;

		case PCI_SUBCLASS_BRDG_MCA:
		    printf("MC");
		    break;

		case PCI_SUBCLASS_P2P_BRIDGE:
		    printf("P2P");
		    pciConfigInByte (bus, device, function,
				PCI_CFG_SECONDARY_BUS, &secBus);
		    break;

		case PCI_SUBCLASS_PCMCIA_BRIDGE:
		    printf("PCMCIA");
		    break;

		case PCI_SUBCLASS_BRDG_CARDBUS:
		    printf("CARDBUS");
		    break;

		case PCI_SUBCLASS_BRDG_RACEWAY:
		    printf("RACEWAY");
		    break;

		default:
		    printf("UNKNOWN (0x%02x)", subClass);
		    break;
		}

	    printf(" BRIDGE");
	    if ( secBus != 0 )
		{
		printf(" to [%d,0,0]", secBus); 
		printf("\n");

		pciConfigInWord(bus,device,function,
				PCI_CFG_COMMAND, &cmdReg);

		if ( cmdReg & PCI_CMD_MEM_ENABLE )
		    {
		    pciConfigInWord(bus,device,function,
				PCI_CFG_MEM_BASE, &memBase);
		    pciConfigInWord(bus,device,function,
				PCI_CFG_MEM_LIMIT, &memLimit);
		    printf("\tbase/limit:\n");
		    printf("\t  mem=   0x%04x0000/0x%04xffff\n",
			    memBase & 0xfff0, memLimit | 0x000f);

		    pciConfigInWord(bus,device,function,
				PCI_CFG_PRE_MEM_BASE, &memBase);
		    pciConfigInWord(bus,device,function,
				PCI_CFG_PRE_MEM_LIMIT, &memLimit);
		    if ( ( memBase & 0x000f ) == 0x0001 )
			{
			/* 64-bit memory */
			pciConfigInLong(bus,device,function,
					PCI_CFG_PRE_MEM_BASE_U,
					&memBaseU);
			pciConfigInLong(bus,device,function,
					PCI_CFG_PRE_MEM_LIMIT_U,
					&memLimitU);
			printf("\t  preMem=0x%08x%04x0000/"
			       "0x%08x%04xffff\n",
			    		memBaseU, memBase & 0xfff0, 
			    		memLimitU, memLimit | 0x000f);
			}
		    else
			printf("\t  preMem=0x%04x0000/0x%04xffff\n",
			    memBase & 0xfff0, memLimit | 0x000f);
		    }

		if ( cmdReg & PCI_CMD_IO_ENABLE )
		    {
		    pciConfigInByte(bus,device,function,
					PCI_CFG_IO_BASE, &ioBase);
		    pciConfigInByte(bus,device,function,
					PCI_CFG_IO_LIMIT, &ioLimit);
		    if ( ( ioBase & 0x0f ) == 0x01 )
			{
			/* 32-bit I/O */
			pciConfigInWord(bus,device,function,
					PCI_CFG_IO_BASE_U, &ioBaseU);
			pciConfigInWord(bus,device,function,
					PCI_CFG_IO_LIMIT_U, &ioLimitU);
			printf("\t  I/O=   0x%04x%02x00/0x%04x%02xff\n",
					ioBaseU, (ioBase & 0xf0),
					ioLimitU, (ioLimit | 0x0f));
			}
		    else
			printf("\t  I/O=   0x%02x00/0x%02xff\n",
			    (ioBase & 0xf0), (ioLimit | 0x0f));
		    }
		}
	    else
		printf("\n");

	    break;

	default:
	    printf("UNKNOWN!\n");
        }

    pciStatusWordShow(bus, device, function);
    pciCmdWordShow(bus, device, function);

    pciFuncBarShow(bus, device, function, 0);
    pciFuncBarShow(bus, device, function, 1);
    if ( numBars > 2 )
	{
        pciFuncBarShow(bus, device, function, 2);
        pciFuncBarShow(bus, device, function, 3);
        pciFuncBarShow(bus, device, function, 4);
        pciFuncBarShow(bus, device, function, 5);
	}

    return(OK);
    }

/*********************************************************************
*
* pciConfigTopoShow - show PCI topology
*
* This routine traverses the PCI bus and prints assorted information
* about every device found.  The information is intended to present
* the topology of the PCI bus.  In includes: (1) the device type, (2)
* the command and status words, (3) for PCI to PCI bridges the memory
* and I/O space configuration, and (4) the values of all implemented
* BARs.
*
* RETURNS: N/A.
*/

void pciConfigTopoShow()
    {
    pciConfigFuncShow(0,0,0,NULL);
    pciConfigForeachFunc(0,TRUE,pciConfigFuncShow,NULL);
    }
