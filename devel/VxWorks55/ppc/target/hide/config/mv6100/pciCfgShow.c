/* pciCfgShow.c - PCI Configuration Space Show Routines */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,01mar04,cak  Ported from pcicfgdump.c and completely overhauled.
*/

#include "vxWorks.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "errno.h"
#include "config.h"
#include "pciCfgShow.h"
#include "drv/pci/pciConfigLib.h"

/* Definitions */

#define LEN 16

/* Locals */

/* Table of PCI class codes and corresponding text description.  */

CLASSCODE_DATA class_initdata[]=
    {      
	{PCIHDR_CLASS_NONE, "Built before Class definitions"},
	{PCIHDR_CLASS_MASS, "Mass Storage Controller"}, 
	{PCIHDR_CLASS_NETWORK, "Network Controller"},
	{PCIHDR_CLASS_DISPLAY, "Display Controller"},
	{PCIHDR_CLASS_MMEDIA, "Multimedia Device"},
	{PCIHDR_CLASS_MEMCTL, "Memory Controller"},
	{PCIHDR_CLASS_BRIDGE, "Bridge Device"},
	{PCIHDR_CLASS_SCC, "Simple Communications Controller"},
	{PCIHDR_CLASS_PER, "Base System Peripheral"},
	{PCIHDR_CLASS_IN, "Input Device"},
	{PCIHDR_CLASS_DOCK, "Docking Station"},
	{PCIHDR_CLASS_PROC, "Processor"},
	{PCIHDR_CLASS_SBUS, "Serial Bus Controller"},
	{PCIHDR_CLASS_NOFIT, "Device not in defined Class Codes"}
     };

#define CLASSCODE_SIZE	sizeof(class_initdata)/sizeof(CLASSCODE_DATA)  

/* Table of PCI subclass codes and corresponding text.  */

SUBCLASSCODE_DATA subclass_initdata[]=
    {
	{PCIHDR_CLASS_NONE, PCIHDR_SUBCLASS_NONVGA, "Non VGA device"},
	{PCIHDR_CLASS_NONE, PCIHDR_SUBCLASS_VGADEV, "VGA-compatible device"},
	{PCIHDR_CLASS_MASS, PCIHDR_SUBCLASS_SCSI, "SCSI Controller"},
	{PCIHDR_CLASS_MASS, PCIHDR_SUBCLASS_IDE, "IDE Controller"},
	{PCIHDR_CLASS_MASS, PCIHDR_SUBCLASS_FLOP, "Floppy Disk Controller"},
	{PCIHDR_CLASS_MASS, PCIHDR_SUBCLASS_IPI, "IPI Controller"},
	{PCIHDR_CLASS_MASS, PCIHDR_SUBCLASS_RAID, "RAID Controller"},
	{PCIHDR_CLASS_MASS, PCIHDR_SUBCLASS_OTHER, "Other Mass Storage Controller"},
	{PCIHDR_CLASS_NETWORK, PCIHDR_SUBCLASS_ETHER,"Ethernet Controller"},
	{PCIHDR_CLASS_NETWORK, PCIHDR_SUBCLASS_TOKEN,"Token Ring Controller"},
	{PCIHDR_CLASS_NETWORK, PCIHDR_SUBCLASS_FDDI, "FDDI Controller"},
	{PCIHDR_CLASS_NETWORK, PCIHDR_SUBCLASS_ATM, "ATM Controller"},
	{PCIHDR_CLASS_NETWORK, PCIHDR_SUBCLASS_OTHER, "Other Network Controller"},
	{PCIHDR_CLASS_DISPLAY, PCIHDR_SUBCLASS_VGA, "VGA-compatible Controller"},
	{PCIHDR_CLASS_DISPLAY, PCIHDR_SUBCLASS_XGA, "XGA Controller"},
	{PCIHDR_CLASS_DISPLAY, PCIHDR_SUBCLASS_OTHER, "Other Display Controller"},
	{PCIHDR_CLASS_MMEDIA, PCIHDR_SUBCLASS_VIDEO,"Video Device"},
	{PCIHDR_CLASS_MMEDIA, PCIHDR_SUBCLASS_AUDIO,"Audio Device"},
	{PCIHDR_CLASS_MMEDIA, PCIHDR_SUBCLASS_OTHER, "Other Multimedia Device"},
	{PCIHDR_CLASS_MEMCTL, PCIHDR_SUBCLASS_RAM,"RAM Memory Controller"},
	{PCIHDR_CLASS_MEMCTL, PCIHDR_SUBCLASS_FLASH,"Flash Memory Controller"},
	{PCIHDR_CLASS_MEMCTL, PCIHDR_SUBCLASS_OTHER, "Other Memory Controller"},
	{PCIHDR_CLASS_BRIDGE, PCIHDR_SUBCLASS_HOST,"Host/PCI Bridge"},
	{PCIHDR_CLASS_BRIDGE, PCIHDR_SUBCLASS_ISA, "PCI/ISA Bridge"},
	{PCIHDR_CLASS_BRIDGE, PCIHDR_SUBCLASS_EISA, "PCI/EISA Bridge"},
	{PCIHDR_CLASS_BRIDGE, PCIHDR_SUBCLASS_MC, "PCI/Micro Channel Bridge"},
	{PCIHDR_CLASS_BRIDGE, PCIHDR_SUBCLASS_PCI, "PCI/PCI Bridge"},
	{PCIHDR_CLASS_BRIDGE, PCIHDR_SUBCLASS_PCMCIA, "PCI/PCMCIA Bridge"},
	{PCIHDR_CLASS_BRIDGE, PCIHDR_SUBCLASS_NBUS, "PCI/NuBus Bridge"},
	{PCIHDR_CLASS_BRIDGE, PCIHDR_SUBCLASS_CBUS, "PCI/CardBus Bridge"},
	{PCIHDR_CLASS_BRIDGE, PCIHDR_SUBCLASS_OTHER, "Other Bridge Type"},
	{PCIHDR_CLASS_SCC, PCIHDR_SUBCLASS_SER, "Serial Controller"},
	{PCIHDR_CLASS_SCC, PCIHDR_SUBCLASS_PAR, "Parallel Port"},
	{PCIHDR_CLASS_SCC, PCIHDR_SUBCLASS_OTHER, "Other Communications Device"},
	{PCIHDR_CLASS_PER, PCIHDR_SUBCLASS_PIC, "Programmable Interrupt Controller"},
	{PCIHDR_CLASS_PER, PCIHDR_SUBCLASS_DMA, "DMA Controller"},
	{PCIHDR_CLASS_PER, PCIHDR_SUBCLASS_TIMER, "System Timer"},
	{PCIHDR_CLASS_PER, PCIHDR_SUBCLASS_RTC, "RTC Controller"},
	{PCIHDR_CLASS_PER, PCIHDR_SUBCLASS_OTHER, "Other System Peripheral"},
	{PCIHDR_CLASS_IN, PCIHDR_SUBCLASS_KEY, "Keyboard Controller"},
	{PCIHDR_CLASS_IN, PCIHDR_SUBCLASS_PEN, "Digitizer (pen)"},
	{PCIHDR_CLASS_IN, PCIHDR_SUBCLASS_MOUSE, "Mouse Controller"},
	{PCIHDR_CLASS_IN, PCIHDR_SUBCLASS_OTHER, "Other Input Controller"},
	{PCIHDR_CLASS_DOCK, PCIHDR_SUBCLASS_STN, "Generic Docking Station"},
	{PCIHDR_CLASS_DOCK, PCIHDR_SUBCLASS_OTHER, "Other Docking Station"},
	{PCIHDR_CLASS_PROC, PCIHDR_SUBCLASS_386, "386 Processor"},
	{PCIHDR_CLASS_PROC, PCIHDR_SUBCLASS_486, "486 Processor"},
	{PCIHDR_CLASS_PROC, PCIHDR_SUBCLASS_PENTIUM, "Pentium Processor"},
	{PCIHDR_CLASS_PROC, PCIHDR_SUBCLASS_ALPHA, "Alpha Processor"},
	{PCIHDR_CLASS_PROC, PCIHDR_SUBCLASS_POWERPC, "PowerPC Processor"},
	{PCIHDR_CLASS_PROC, PCIHDR_SUBCLASS_COPROC, "Co-processor"},
	{PCIHDR_CLASS_SBUS, PCIHDR_SUBCLASS_FIRE, "Firewire (IEEE 1394)"},
	{PCIHDR_CLASS_SBUS, PCIHDR_SUBCLASS_ACCESS, "ACCESS.bus"},
	{PCIHDR_CLASS_SBUS, PCIHDR_SUBCLASS_SSA, "Serial Storage Architecture"},
	{PCIHDR_CLASS_SBUS, PCIHDR_SUBCLASS_USB, "Universal Serial Bus"},
	{PCIHDR_CLASS_SBUS, PCIHDR_SUBCLASS_FIBRE, "Fibre Channel"}
    };

#define SUBCLASSCODE_SIZE	sizeof(subclass_initdata)/sizeof(SUBCLASSCODE_DATA)  

/* Externals */

IMPORT STATUS pciHeaderShow();

/* Forward Declarations */

LOCAL UINT8 * matchClass (UINT8 classcode); 
LOCAL UINT8 * matchSubClass (UINT8 classcode, UINT8 subclasscode);
LOCAL void hDump (UINT32 totalbytes, UINT8 * p, UINT32 offset);
LOCAL void pciFmtOutput (PCI_IOCTL * pPciDevices, UINT8 numPciDevices, 
			 UINT32 nBytes);
LOCAL void pciCfgRead (UINT32 busNo, UINT32 deviceNo, UINT32 funcNo,
		       UINT32 offset, UINT32 nBytes, UINT8 * pData);
LOCAL void pciCfgShow (UINT32 bus, UINT32 dev, UINT32 func, UINT32 offset, 
		       UINT32 nBytes);
void pciCfgShowAll (void);
void pciCfgHeaderShowAll(void);

/*******************************************************************************
*
* pciCfgShow - Read from a PCI device and display the results. 
*
* This function reads from a given PCI device and displays the results.
*
* RETURNS: N/A
*/

LOCAL void pciCfgShow
    (
    UINT32 bus,		/* PCI Bus Number */ 
    UINT32 dev,		/* PCI Device Number */ 
    UINT32 func, 	/* PCI Function Number */
    UINT32 offset, 	/* Offset into Configuration Space */
    UINT32 nBytes	/* Number of Bytes */
    )
    {
    PCI_IOCTL	*pPciDevices;
    PCI_IOCTL	*pStartDevices;
    UINT8	numPciDevices;

    numPciDevices = 1;

    /* Malloc a structure */

    pPciDevices = (PCI_IOCTL *)malloc((numPciDevices+1)*sizeof(PCI_IOCTL));
    pStartDevices = pPciDevices;

    /* Clear out the structure */

    memset(pPciDevices, 0, numPciDevices*sizeof(PCI_IOCTL));

    /* Save the values passed in for later use */

    pPciDevices->bus = bus;
    pPciDevices->device = dev;
    pPciDevices->function = func;
    pPciDevices->offset = offset;
    pPciDevices->value = 0x00;
    pPciDevices->bytes = nBytes;

    /* Do a little parameter checking.  */

    if ((pPciDevices->bus < 256) && (pPciDevices->device < MAXPCIDEV) &&
	(pPciDevices->function < MAXPCIFUNC) && 
	(pPciDevices->offset < 256) &&
	(pPciDevices->bytes <= 256)) 
	{

	/* Now read N bytes of the specified PCI device */

	pciCfgRead (bus, dev, func, offset, nBytes, pPciDevices->confRegs);

	/* Take what was read format it and display it. */

	pciFmtOutput(pPciDevices, numPciDevices, pPciDevices->bytes);
    	} 
     else 
	{
	printf("ERROR: Invalid parameter\n");
    	}

    /* Free the memory we allocated earlier */

    free(pStartDevices);
    }

/*******************************************************************************
*
* pciCfgShowAll - Display configuration space for all PCI devices
*
* This function displays the configuration space (256 bytes) for all PCI devices
* found within the system.
*
* RETURNS: N/A
*/

void pciCfgShowAll(void)
    {
    UINT32 busNo;	/* PCI Bus Number */
    UINT32 devNo;	/* PCI Device Number */
    UINT32 func;	/* PCI Function Number */
    UINT16 vendorId;	/* Vendor ID */
    UINT16 headerType;	/* Header Type */
    UINT32 nBytes=256;	/* Number of Bytes */

    /* Traverse all possible PCI busses looking a devices and functions */

    for (busNo = 0; busNo < 256; busNo++) 
	{
	for (devNo = 0; devNo < MAXPCIDEV; devNo++) 
    	    {
	    for (func = 0; func < MAXPCIFUNC; func++) 
		{
		pciConfigInWord (busNo, devNo, func, 
				PCI_CFG_VENDOR_ID, &vendorId);

		if (vendorId != 0xffff) 
		    {
		    pciCfgShow(busNo, devNo, func, 0, nBytes);

		    if (func == 0) 
		        {
			pciConfigInWord (busNo, devNo, 
					func, PCI_CFG_HEADER_TYPE, &headerType);
			if (!(headerType & 0x80))
			    break;
			}							
		    }
    	        }
    	    }
    	}
    }

/*******************************************************************************
*
* pciCfgHeaderShowAll - Call pciHeaderShow for all PCI devices
*
* This function calls pciHeaderShow() for all PCI devices found within the
* system. 
*
* RETURNS: N/A
*/

void pciCfgHeaderShowAll(void)
    {
    UINT32 busNo;	/* PCI Bus Number */
    UINT32 devNo;	/* PCI Device Number */
    UINT32 func;	/* PCI Function Number */
    UINT16 vendorId;	/* Vendor ID */
    UINT16 headerType;	/* Header Type */

    /* Traverse all possible PCI busses looking a devices and functions */

    for (busNo = 0; busNo < 256; busNo++) 
	{
	for (devNo = 0; devNo < MAXPCIDEV; devNo++) 
	    {
	    for (func = 0; func < MAXPCIFUNC; func++) 
		{
		pciConfigInWord (busNo, devNo, func, 
			PCI_CFG_VENDOR_ID, &vendorId);

		if (vendorId != 0xffff) 
		    {
		    printf("\r\n\r\nPCI Device bus=%d dev=%d func=%d\r\n",
					(int)busNo, (int)devNo, (int)func);
		    pciHeaderShow(busNo, devNo, func);

		    if (func == 0) 
			{
			pciConfigInWord (busNo, devNo, func,
					 PCI_CFG_HEADER_TYPE, &headerType);
			if (!(headerType & 0x80))
			    break;
			}							
    	            }
    	        }
    	    }
    	}
    }

/*******************************************************************************
*
* pciCfgRead - Read from a given PCI device.
*
* This function reads a specified number of bytes, beginning at a specified
* offset, from a given PCI device.
*
* RETURNS: N/A
*/

LOCAL void pciCfgRead
    (
    UINT32 bus, 	/* PCI Bus Number */
    UINT32 dev, 	/* PCI Device Number */
    UINT32 func, 	/* PCI Function Number */
    UINT32 offset, 	/* Offset into Configuration Space */
    UINT32 nBytes, 	/* Number of Bytes */
    UINT8 * p_buffer	/* Variable to hold result */
    )
    {
    UINT32 i;
    UINT8 val;

    /* Convert bytes to longs and read the appropriate number of longs */

    for (i=0; i<nBytes; i++) 
	{
        pciConfigInByte (bus, dev, func, i+offset, &val);
	*p_buffer++ = val;
    	}
    }

/*******************************************************************************
*
* pciFmtOutput - Display formatted PCI configuration space data
*
* This function displays the configuration space for PCI devices.
*
* RETURNS: N/A
*/ 

LOCAL void pciFmtOutput
    (
    PCI_IOCTL * pPciDevices, 	/* PCI Devices */
    UINT8 numPciDevices, 	/* Number of PCI Devices */
    UINT32 nBytes		/* Number of Bytes */
    )
    {
    UINT32 i;
    UINT8 classCode;
    UINT8 subClassCode;
    UINT32 numBytes = nBytes;
    UINT32 *pConfRegs;

    for (i = 0; i < numPciDevices; i++) 
    	{
	printf("\r\nPCI Device bus=%d dev=%d func=%d\r\n",
		(int)pPciDevices->bus, (int)pPciDevices->device,
		(int)pPciDevices->function);
		
	/* Access class code */

	pciConfigInByte(pPciDevices->bus, 
			pPciDevices->device, 
			pPciDevices->function, 
			PCI_CFG_SUBCLASS, 
			&subClassCode);

	pciConfigInByte(pPciDevices->bus, 
			pPciDevices->device, 
			pPciDevices->function, 
			PCI_CFG_CLASS, &classCode);

	pConfRegs = (UINT32 *)pPciDevices->confRegs;


	/*
	 * Call routines which return text associated with
	 * class/subClassCode so we can display it for the user.
	 */

	printf("Class: %s  Subclass: %s\r\n",matchClass(classCode),
		matchSubClass(classCode,subClassCode)); 

#if (_BYTE_ORDER != _BIG_ENDIAN)
	for (i = 0; i < numBytes/4; i++) 
	    {
	    tmp = *pConfRegs;
	    *pConfRegs++ = ULONG_LE2BE(tmp);
    	    }
#endif

	hDump(numBytes, pPciDevices->confRegs, pPciDevices->offset);

	pPciDevices++;
    	}
    }

/*******************************************************************************
*
* matchSubClass - Translate from subclasscode to String Description 
*
* This function uses the PCI classcode and the PCI subclasscode to
* look up the appropriate text to describe the subclasscode.  The
* function returns a pointer to a string containing the subclass
* description.
*
* RETURNS: pointer to a string containing the subclass description.
*/

LOCAL UINT8 *matchSubClass
    ( 
    UINT8 classcode, 	/* Class Code */
    UINT8 subclasscode	/* Subclass Code */
    )
    {
    UINT32 x;
    SUBCLASSCODE_DATA *subclass_p;	/* ptr to subclass table */
    UINT8 *defaulttext;			/* default text */

    subclass_p = (SUBCLASSCODE_DATA *)&subclass_initdata[0];

    /*
     * If we don't find a matching entry in the table, this
     * will be the default text.
     */

    defaulttext = "Unidentifiable Subclass Code";

    /*
     * Loop through all of the subclass entries in the table until an
     * entry is found where both the class and the subclass match.
     */

    for (x = SUBCLASSCODE_SIZE; x; x--, subclass_p++)
        {
        if ((subclass_p->classcode == classcode) && 
            (subclass_p->subclasscode == subclasscode)) 
 	    return (subclass_p->subclasstext);
        }

	/* no match, return default text */

    return(defaulttext);		
    }


/*******************************************************************************
*
* matchClass - Translate from classcode to String Description. 
*
* This function uses the class PCI code to look up in a table the 
* appropriate matching text to be displayed for this PCI class.
* A pointer to the matching string is returned by the function.
*
* RETURNS: pointer to the matching string.
*/

LOCAL UINT8 *matchClass
    (
    UINT8 classcode	/* Class Code */
    )
    {
    UINT32 x;
    CLASSCODE_DATA *class_p;	/* ptr to class code table */
    UINT8 *defaulttext;		/* default text */

    class_p = (CLASSCODE_DATA *)&class_initdata[0];

    /*
     * If no match is found in the class table, then this
     * default text is returned.
     */

    defaulttext = "Unidentifiable Class Code";

    /*
     * Loop through the class table looking for a classcode match
     * so we can return the matching string.
     */

    for (x = CLASSCODE_SIZE; x; x--, class_p++)
        {
        if (class_p->classcode == classcode)
	    return(class_p->classtext);
        }

	/* no match, return the default */

    return(defaulttext);		
    }

/*******************************************************************************
* 
* hDump - Display formatted output 
*
* This function displays a PCI device's configuration space using a consistent
* format.
*
* RETURNS: N/A
*/

LOCAL void hDump
    (
    UINT32 total_bytes, /* Number of bytes to display */
    UINT8 * p,		/* Pointer to Configuration Data */
    UINT32 offset	/* Initial Offset */
    )
    {
    int addr;
    register int i;
    register int j;
    register int c;
    register int count;

    addr = offset;

    if (total_bytes < LEN)
	count = total_bytes;
    else
	count = LEN;

    for (j = 0; j < total_bytes; j+=LEN) 
	{
	printf("%8.8x: ", addr);
	addr += count;

	for (i=0; i < count; i++) 
	    {
	    printf("%2.2x ", p[i] & 0xff);
	    if (i == 7)
		printf(" ");
	    }
		
	printf("  ");
			
	for (i=0; i < count; i++)
	    {
	    c = p[i] & 0xff;
	    if (c < ' ' || c >= 0x7f)
		c = '.';
			
 	    printf("%c", c);
	    }

	printf("\r\n");

	p += LEN;
	}
    }
