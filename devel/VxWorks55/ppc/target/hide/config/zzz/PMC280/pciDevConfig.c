/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/

/* defines */
#include "pmc280.h"
#include "pmc280_pci.h"
#include "pci.target.h"
#include "pciAutoConfig.h"
#include "pciConfigLib.h"
#include "gtCore.h"
#include "pciScan.h"

#define PCI_NOBODYHOME		0xffffffff

struct _busIoMemRanges
{
	UINT iobase;
	UINT iolimit;
	UINT membase;
	UINT memlimit;
}busIoMemRanges[20];


#define MAX_BUS  20;
#define PRO1000_PCI_VENDOR_ID           0x8086 /* Intel vendor ID */
#define PRO1000_PCI_DEVICE_ID           0x1001 /* device ID */
#define PRO1000F_PCI_SUBSYSTEM_ID       0x1003 /* device subsystem ID */
#define PRO1000T_PCI_SUBSYSTEM_ID       0x1004
#define PRO1000T_PCI_DEVICE_ID          0x1004 /* bizard case */
#define PRO1000F_BOARD                  PRO1000F_PCI_SUBSYSTEM_ID
#define PRO1000T_BOARD                  PRO1000T_PCI_SUBSYSTEM_ID

#define PRO100B_PCI_VENDOR_ID   0x8086  /* PCI vendor ID */
#define PRO100B_PCI_DEVICE_ID   0x1229  /* PCI device ID */
#define PCI_DEVICE_ID_559ER	0x1209	/* Intel 82559 ER */

IMPORT PCI_NUMBER pciActive;  


#if 0
/*
* (DEC21150 PCI-to-PCI Bridge) Configuration Parameter Array
*  Short Version 
*
*/

PCI_SPAN sysPCISpanParmShort[] =
{


 /* Status Reg = ffff {0x06 ,  2,   xxxx   } */
{PCI_CFG_STATUS,  2,   P2P_CLR_STATUS >> 16},

 /* Bridge Control = Bus Reset {0x3e , 2, (0x0040 << 16) } */
{PCI_CFG_BRIDGE_CONTROL, 2, P2P_SEC_BUS_RESET},



 /* Bridge Control {0x3e}:   */
{PCI_CFG_BRIDGE_CONTROL, 2, P2P_SEC_BUS_CONTROL},

 /* Secondary Clock control {0x68};  */
{PCI_CFG_DEC21150_SEC_CLK,   2, P2P_CLK_ENABLE}, 

 /* P_SERR Status {0x6A};   clear status */
{PCI_CFG_DEC21150_SERR_STAT, 1, (P2P_CLR_STATUS >> 16) & 0xFF},

 /* Status {0x02};   clear status */
{PCI_CFG_STATUS,  2,  P2P_CLR_STATUS >> 16},

 /* End of Table */
{END_OFFSET, END_SIZE	, 	END_VALUE	}

};
#endif

/* typedefs */

/* globals */
STATUS sysPciConfigInsertLong
    (

    ULONG busNo,          /* bus number */
    ULONG deviceNo,       /* device number */
    ULONG funcNo,         /* function number */
    ULONG offset,         /* offset into the configuration space */
    ULONG bitMask,        /* Mask which defines field to alter */
    ULONG data            /* data written to the offset */
    );
void pciAutoConfig ();
void pciBusProbe(int bus,int device,PCI_LOC **pPciList,int *fcount);
void pcisecBusProbe(int bus,int device,PCI_LOC **pPciList,int *fcount);
LOCAL void pciListInit(register PCI_LOC *pciList,register UINT nSize);
LOCAL void pciFunctionDisable(register PCI_LOC *pciFunc);
LOCAL void pciDevicesConfig(register UINT bus,register PCI_LOC **pciList,register UINT *nSize);
LOCAL void pciFunctionConfig(register PCI_LOC *pciFunc);
LOCAL UINT pciRegConfig(register PCI_LOC *pciFunc,register UINT baseAddr,register UINT nSize,register UINT addrInfo);
LOCAL UINT pciIospaceSelect (PCI_LOC *pciFunc, UINT **allocP, UINT *boundary);

LOCAL UINT pciMemspaceSelect (PCI_LOC *pciFunc, UINT **allocP, UINT *boundary,
			      UINT addrInfo);

LOCAL void pciBusConfig (PCI_LOC *pciLoc, PCI_LOC **pciList, UINT *nSize);
LOCAL void pciFunctionInit (PCI_LOC *pciFunc);



int nooffeidevices = 0,noofgeidevices = 0;
static int secondary = 0;
struct _ethernetdevice	feidevices[FEI_MAX_UNITS] , geidevices[GEI_MAX_UNITS];
/*int primary[3]={0,11,15};*/


/* locals */
static _PCI_UNIQUE PCI_UNIQUE;

#define PCI_DEVCONFIG_PRINTS		0


void pciAutoConfig()
{
	register struct pciUnique *pciuP;	/* PCI unique structure	*/
	int fcount =0,index = 0;
	PCI_LOC *pciList;
	/* Initialize the list pointer in preparation for probing */

	secondary = 0;
	nooffeidevices = 0;
	noofgeidevices = 0;       

     	pciuP = &PCI_UNIQUE;
     	pciList = pciuP->pciList;
	for(index = 1;index < 32;index++)
	{
		pciBusProbe(0,index,&pciList,&fcount);
	}
	/* Initialize the PCI io/memory space allocation pool pointers */

    	pciuP->pciMem32 = CPU_PCI_MEM_ADRS + CPU_PCI_MEM_SIZE;

#ifdef PCI_DEVCONFIG_PRINTS
	printf("\nMax MEM addr. = %x\n",pciuP->pciMem32);
#endif

    	pciuP->pciIo32 = CPU_PCI_IO_ADRS + CPU_PCI_IO_SIZE;

#ifdef PCI_DEVCONFIG_PRINTS
	printf("\nMax I/O addr. = %x\n",pciuP->pciIo32);
#endif

#ifdef PCI_DEVCONFIG_PRINTS
	printf("\nfcount = %x\n",fcount);
#endif
	pciListInit (pciuP->pciList, fcount);

}
void pciAutoConfigNonMonarch()
{	
        int devnum,busno,funcno,data;
        UINT32 deviceVendorId;
	/* Initialize the list pointer in preparation for probing */

	secondary = 0;
	nooffeidevices = 0;
	noofgeidevices = 0;       
         
        
     	 /* Set the GT PCI0 agent as master on PCI 0 or PCI 1*/    

    for(busno = 0;busno < MAX_BUSES ;busno++)
    {
        for(devnum = 0;devnum < 32;devnum++)
        {
            for(funcno = 0;funcno < 8;funcno++)
            {
                frcPciConfigRead(busno,devnum,funcno,0,&deviceVendorId);
                if(deviceVendorId == 0xffffffff)
		{
		    if(funcno == 0)	
                    	break;
		    else
			continue;	/* It's a multifunction device*/	
		}                
                
/*
	Determining if it's a 82559,82559ER or 82543 device
*/		
		if((deviceVendorId == ((PCI_DEVICE_ID_559ER << 16) | PRO100B_PCI_VENDOR_ID)) || (deviceVendorId == ((PRO100B_PCI_DEVICE_ID << 16) | PRO100B_PCI_VENDOR_ID)))
		{
			if(nooffeidevices < FEI_MAX_UNITS)
			{
				feidevices[nooffeidevices].deviceid = deviceVendorId  >> 16;
				feidevices[nooffeidevices].vendorid = deviceVendorId  & 0xFFFF;
				feidevices[nooffeidevices].deviceno = devnum;
				feidevices[nooffeidevices].busno = busno;
				nooffeidevices++;
			}
	
		}
		else
		{
			if((deviceVendorId== ((PRO1000T_PCI_DEVICE_ID << 16) | PRO1000_PCI_VENDOR_ID)) || (deviceVendorId  == ((PRO1000_PCI_DEVICE_ID << 16) | PRO1000_PCI_VENDOR_ID)) )
			{
				if(noofgeidevices < GEI_MAX_UNITS)
				{
					geidevices[noofgeidevices].deviceid = deviceVendorId  >> 16;
					geidevices[noofgeidevices].vendorid = deviceVendorId  & 0xFFFF;
					geidevices[noofgeidevices].deviceno = devnum;
					geidevices[noofgeidevices].busno = busno;
					noofgeidevices++;
				}
	
			}
		  }
                 frcPciConfigRead(busno,devnum,funcno,0x0c,&data);
                 if((data & 0x00800000) == 0x00000000)
                    break;                    
            }		
	}
    } 			

}
void pciBusProbe(int bus,int device,PCI_LOC **pPciList,int *fcount)
{
	/* Check each function until an unused one is detected */
	int function =0,workvar = 0, nextbus,secdevice , templong;
	int dev_vend;
	UINT16 pciClass , wtemp;
	UINT8  btemp;
		
	for (function = 0; function < MAXPCIFUNC; function++)
	{
	    
	    /* Check for a valid device/vendor number */
	      	pciConfigInLong (bus, device,function,PCI_CFG_VENDOR_ID, &dev_vend);
	    	if (dev_vend == PCI_NOBODYHOME)
		{
			if(function == 0)
				break;	/* Empty position - proceed to next PCI device */
			else
				continue;	/*It's a multifunction device,detected in the Header Type field*/
		}
		(*pPciList)->bus = bus;
		(*pPciList)->device = device;
		(*pPciList)->function = function;
		(*pPciList)++;
		(*fcount)++;
/*
	Determining if it's a 82559,82559ER or 82543 device
*/		
		if((dev_vend == ((PCI_DEVICE_ID_559ER << 16) | PRO100B_PCI_VENDOR_ID)) || (dev_vend == ((PRO100B_PCI_DEVICE_ID << 16) | PRO100B_PCI_VENDOR_ID)))
		{
			if(nooffeidevices < FEI_MAX_UNITS)
			{
				feidevices[nooffeidevices].deviceid = dev_vend >> 16;
				feidevices[nooffeidevices].vendorid = dev_vend & 0xFFFF;
				feidevices[nooffeidevices].deviceno = device;
				feidevices[nooffeidevices].busno = bus;
				nooffeidevices++;
			}
	
		}
		else
		{
			if((dev_vend == ((PRO1000T_PCI_DEVICE_ID << 16) | PRO1000_PCI_VENDOR_ID)) || (dev_vend == ((PRO1000_PCI_DEVICE_ID << 16) | PRO1000_PCI_VENDOR_ID)) )
			{
				if(noofgeidevices < GEI_MAX_UNITS)
				{
					geidevices[noofgeidevices].deviceid = dev_vend >> 16;
					geidevices[noofgeidevices].vendorid = dev_vend & 0xFFFF;
					geidevices[noofgeidevices].deviceno = device;
					geidevices[noofgeidevices].busno = bus;
					noofgeidevices++;
				}
	
			}
		}

		/* Check to see if this function belongs to a PCI-PCI bridge */
#ifdef PCI_DEVCONFIG_PRINTS
		printf("\nbus,device , function = %d %d %d\n",bus,device,function);
#endif

	    	pciConfigInWord (bus,device,function,
#ifdef TL01d
	    PCI_CFG_CLASS, &pciClass);
#else
	    PCI_CFG_SUBCLASS, &pciClass);
#endif

#ifdef PCI_DEVCONFIG_PRINTS
	    printf("\n\nPCICLASS = %x  %x\n",pciClass,((PCI_CLASS_BRIDGE_CTLR << 8) + PCI_SUBCLASS_P2P_BRIDGE));	
#endif
	    if(pciClass == (UINT16)((PCI_CLASS_BRIDGE_CTLR << 8) + PCI_SUBCLASS_P2P_BRIDGE) )
	    {
		secondary++; /*found a bridge so incrementing the secondary bus no.*/
		workvar = (0xFF << 16) + (secondary << 8) + bus; /* subordinate,secondary,primary bus nos. */
		pciConfigInLong(bus,device,function,PCI_CFG_PRIMARY_BUS,&templong);
		templong = (templong & 0xFF000000) | workvar;
		pciConfigOutLong(bus,device,function,PCI_CFG_PRIMARY_BUS,templong);

		pciConfigInLong(bus,device,function,PCI_CFG_COMMAND,&templong);
		templong |= 0x00000007;
		pciConfigOutLong(bus,device,function,PCI_CFG_COMMAND,templong);

		pciConfigInWord(bus,device,function,PCI_CFG_PRIMARY_BUS,&wtemp);

#ifdef PCI_DEVCONFIG_PRINTS
		printf("\nPrimary Secondary bus nos. = %x\n",wtemp);
#endif
		/* Setup and probe this bridge device */
		/*pcisecBusProbe (1, 0,pPciList,fcount);*/
		nextbus = secondary;
		for(secdevice = 0;secdevice < 32; secdevice++)
		{
		 	pciBusProbe (nextbus,secdevice,pPciList,fcount);
		}
		/*Now program the correct Subordinate bus number*/
		templong = secondary << 16;
		sysPciConfigInsertLong (bus, device,function, PCI_CFG_PRIMARY_BUS,
				    0x00FF0000,templong);

	    }

	    /* Proceed to next device if this is a single function device */

	    if(function == 0)
	    {
		pciConfigInByte (bus, device,function,
			 PCI_CFG_HEADER_TYPE,&btemp);
		if ((btemp & PCI_HEADER_MULTI_FUNC) == 0)
	    		break; /* No more functions - proceed to next PCI device */
	    }
	 

	}	
		
}

#if 0
void pcisecBusProbe(int bus,int device,PCI_LOC **pPciList,int *fcount)
{
	int function;
	int dev_vend,btemp;

	for(function = 0;function < MAXPCIFUNC;function++)	
	{
		/* Check for a valid device/vendor number */
	      	pciConfigInLong (bus, device,function,PCI_CFG_VENDOR_ID, &dev_vend);
	    	if (dev_vend == PCI_NOBODYHOME)
		{
			if(function == 0)
			break;	/* Empty position - proceed to next PCI device */
			else
			continue;	
		}
		(*pPciList)->bus = bus;
		(*pPciList)->device = device;
		(*pPciList)->function = function;
		(*pPciList)++;
		(*fcount)++;
		/* Proceed to next device if this is a single function device */
	    	if(function == 0)
	    	{
			pciConfigInByte (bus, device,function,
			 PCI_CFG_HEADER_TYPE, &btemp);
			if ((btemp & PCI_HEADER_MULTI_FUNC) == 0)
	    			break; /* No more functions - proceed to next PCI device */
	    	}

	}
}
#endif



LOCAL
void pciListInit
    (
    	register PCI_LOC *pciList,		/* input: Pointer to first function   */
    	register UINT nSize			/* input: Number of functions to init */
    )
    {

    PCI_LOC *pciFunc;			/* Pointer to next function */
    register UINT nLoop;		/* Loop control variable    */
    UINT nEnd;				/* End of function list     */

    /* Disable all devices before we initialize any */
    pciFunc = pciList;
    for (nLoop = 0; nLoop < nSize /* previously nSize*/; nLoop++)
    {
     	pciFunctionDisable (pciFunc);
	pciFunc++;
    }

    /* Allocate and assign PCI space to each device */

    pciFunc = pciList;/*pciList*/
    nEnd = nSize; /* nEnd = nSize*/
    pciDevicesConfig (pciList->bus, &pciFunc, &nEnd);

    /* Initialize and enable each device */

    pciFunc = /*pciList*/ pciList;
    for (nLoop = 0; nLoop < nSize; nLoop++)
    {
	pciFunctionInit (pciFunc);
	pciFunc++;
    }
    }

/******************************************************************************
*
* pciFunctionDisable - Disable a specific PCI function.
*
* This routine clears the I/O, mem, master, & ROM space enable bits
* for a single PCI function.
*
* The PCI spec says that devices should normally clear these by
* default after reset but in actual practice, some PCI devices
* do not fully comply.	 This routine ensures that the devices
* have all been disabled before configuration is started.
*
* RETURNS: N/A
*/

LOCAL
void pciFunctionDisable
    (
    register PCI_LOC *pciFunc		/* input: Pointer to PCI function */
                                        /* structure			  */
    )
    {
    UCHAR cTemp;			/* Temporary storage */

    /*
     * First, we should make sure the I/O space, memory space, and
     * master bits for this device are disabled.
     */

    pciConfigOutWord (pciFunc->bus, pciFunc->device, pciFunc->function,
		      PCI_CFG_STATUS, 0);

    /* Disable header dependent fields */

    pciConfigInByte (pciFunc->bus, pciFunc->device, pciFunc->function,
		     PCI_CFG_HEADER_TYPE, &cTemp);
    cTemp &= 0x7f;
    switch (cTemp)
	{
	case PCI_HEADER_TYPE0:	/* non PCI-PCI bridge */
	    /*
	     * Disable Expansion ROM address decode for the device.
	     * Note that no mem space is allocated for the Expansion
	     * ROM, so a re-enable later should NOT be done.
	     */

	    sysPciConfigInsertLong (pciFunc->bus, pciFunc->device,
				    pciFunc->function, PCI_CFG_EXPANSION_ROM,
				    0x1, 0);
	    break;

	case PCI_HEADER_PCI_PCI:	/* PCI-PCI bridge */
	    sysPciConfigInsertLong (pciFunc->bus, pciFunc->device,
				    pciFunc->function, PCI_CFG_ROM_BASE,
				    0x1, 0);
	    break;

	default:
	    break;
	}
    }

/******************************************************************************
*
* pciDevicesConfig - Allocate memory and I/O space to PCI function.
*
* This routine allocates memory and I/O space to functions on an
* individual PCI bus.
*
* Warning: do not sort the include function list before this routine is
* called.  This routine requires each function in the list to be in the
* same order as the probe occurred.
*
* RETURNS: N/A
*/

LOCAL
void pciDevicesConfig
    (
    register UINT bus,			/* input: Current bus number	   */
    register PCI_LOC **pciList,		/* input: Pointer to function list */
    register UINT *nSize		/* input: Number of remaining      */
					/* functions			   */
    )
    {
    register PCI_LOC *pciFunc;		/* Pointer to PCI function	   */
    register UINT nextBus;		/* Bus where function is located   */
    short pciClass;			/* Class field of function	   */

    /* Process each function within the list */

    while(*nSize > 0)
    {
	/* Setup local variables */
	pciFunc = *pciList;
	nextBus = pciFunc->bus;

	/* Decrease recursion depth if this function is on a parent bus */

	if (nextBus < bus)
	    {
	    	return;
	    }

	/* Allocate and assign space to functions on this bus */
	pciFunctionConfig (pciFunc);
	(*nSize)--;
	(*pciList)++;

#ifdef DEBUG_PCIAUTO
	printf("DBG pciDevicesConfig  bus %d nsiz %x", bus, *nSize);
#endif
	/* Increase recursion depth if this function is a PCI-PCI bridge */
	pciConfigInWord (pciFunc->bus, pciFunc->device, pciFunc->function,
			 PCI_CFG_SUBCLASS, &pciClass);
	switch (pciClass)
	    {
	    /* PCI-PCI bridge functions */

	    case ((PCI_CLASS_BRIDGE_CTLR << 8) + PCI_SUBCLASS_P2P_BRIDGE):

		/* Increase recursion depth */
#ifdef DEBUG_PCIAUTO
	printf("DBG pciDevicesConfig  found bridge bus %x Dev %x ", 
			pciFunc->bus, pciFunc->device);
	taskDelay (20);
#endif

		pciBusConfig(pciFunc, pciList, nSize);
		break;

	    /* Other PCI functions */

	    default:
		/* Maintain current recursion depth */
#ifdef DEBUG_PCIAUTO
	printf("DBG pciDevicesConfig  found NO bridge : Clase %x bus %x Dev %x ", 
			pciClass, pciFunc->bus, pciFunc->device);
	taskDelay (20);
#endif

		break;
	    }
	}
    }

/******************************************************************************
*
* pciFunctionConfig - Assign memory and/or I/O space to single function.
*
* This routine allocates and assigns memory and/or I/O space to a
* single PCI function.	 Allocations are made for each implemented
* base address register (BAR) in the PCI configuration header.
*
* RETURNS: N/A
*/

LOCAL
void pciFunctionConfig
    (
    register PCI_LOC *pciFunc	/* input: "Include list" pointer to function */
    )
    {
    register UINT baMax;	/* Total number of base addresses    */
    register UINT baI;		/* Base address register index	     */
    register UINT baseAddr;	/* PCI Offset of base address	     */
    UINT readVar;		/* Contents of base address register */
    register UINT addrInfo;	/* PCI address type information	     */
    register UINT sizeMask;	/* LSbit for size calculation	     */
    UCHAR headerType;		/* Read from PCI config header	     */
    UCHAR classCode;		/* Read from PCI config header	     */

    /* Determine the number of base address registers present */

    pciConfigInByte (pciFunc->bus, pciFunc->device, pciFunc->function,
		     PCI_CFG_HEADER_TYPE, &headerType);
    headerType &= 0x7f;

    switch (headerType)
	{
	case PCI_HEADER_TYPE0:
	    baMax = 6;
	    break;

	case PCI_HEADER_PCI_PCI:
	    baMax = 2;
	    break;

	default:
	    baMax = 0;
	    break;
	}

    /*
     * Write all F's to the base address registers so we can later
     * read them back and determine what type of space the
     * device needs and how much space it needs.
     */

    for (baI = 0; baI < baMax; baI++)
	{
		pciConfigOutLong (pciFunc->bus, pciFunc->device, pciFunc->function,
			  (PCI_CFG_BASE_ADDRESS_0 + (baI * 4)), -1);
	}

    /*
     * Auto-configure the device only if it is not a display device
     * (There is the "DOS compatibility hole" in PCI which requires
     * the PCI graphics device to be located in a particular area
     * for compatibility with the "old DOS world".  (See PCI
     * manuals or VGA driver for more details.)
     */

    pciConfigInByte (pciFunc->bus, pciFunc->device, pciFunc->function,
		    PCI_CFG_CLASS, &classCode);

    if (classCode == PCI_CLASS_DISPLAY_CTLR)
    {
	return;
    }

    /*
     * Allocate memory for each base address register
     */

    for (baI = 0; baI < baMax; baI++)
	{
	/* Get the base address register contents */

	baseAddr = PCI_CFG_BASE_ADDRESS_0 + (baI * 4);
	pciConfigInLong (pciFunc->bus, pciFunc->device, pciFunc->function,
			 baseAddr, &readVar);
	addrInfo = readVar & 0x7;

	/*
	 * Check for type, setup mask variables (based on type)
	 */

	if (addrInfo & PCI_BASEADDR_IO)
	    sizeMask = (1<<2);
	else
	    sizeMask = (1<<4);

	/*
	 * Loop until we find a bit set or until we run out of bits
	 */

	for (; sizeMask; sizeMask <<= 1)
	    {
	    /*
	     * is this bit set? if not, keep looking
	     */

	    if (readVar & sizeMask)
		{
		baI += pciRegConfig (pciFunc, baseAddr, sizeMask, addrInfo);
		break;
		}
	    }
	}
    }

/******************************************************************************
*
* pciRegConfig - Assign PCI space to a single PCI base address register.
*
* This routine allocates and assigns PCI space (either memory or I/O)
* to a single PCI base address register.
*
* RETURNS: 1 if base address register supports mapping anywhere in 64-bit
* address space, 0 otherwise.
*/

LOCAL
UINT pciRegConfig
    (
    register PCI_LOC *pciFunc,	/* input: Pointer to function in "include  */
				/* list"				   */
    register UINT baseAddr,	/* Offset of base PCI address		   */
    register UINT nSize,	/* Size and alignment requirements	   */
    register UINT addrInfo	/* PCI address type information		   */
    )
    {
    UINT *allocP;			/* Pointer to PCI space alloc pointer */
    UINT boundary;			/* PCI space boundary pointer	      */
    register UINT addr;			/* Working address variable	      */
    register UINT spaceEnable;		/* PCI space enable bit	              */
    register UINT baseaddr_mask;	/* Mask for base address register     */
    register UINT register64Bit;	/* 64 bit register flag	              */

    /* Select the appropriate PCI address space for this register */
    if(addrInfo & PCI_BASEADDR_IO)
	{
	/* Configure this register for PCI I/O space */
	spaceEnable = PCI_CMDREG_IOSP;
	baseaddr_mask = 0xFFFFFFFC;
	register64Bit = pciIospaceSelect (pciFunc, &allocP, &boundary);
	}
    else
	{
	/* Configure this register for PCI memory space */
	spaceEnable = PCI_CMDREG_MEMSP;
	baseaddr_mask = 0xFFFFFFF0;
	register64Bit = pciMemspaceSelect (pciFunc, &allocP, &boundary,addrInfo);
	}

    /* Calculate the allocation pointer adjustment */

    addr = (*allocP - nSize) & ~(nSize - 1);

    /*
     * Do not exceed the lower boundary! If this occurs, all we can
     * do here is return, as this is called early in the initialization
     * process, before I/O is available to print error messages.
     */

    if (addr >= boundary)
	{

	/* Program the base address register */

	sysPciConfigInsertLong (pciFunc->bus, pciFunc->device,
			        pciFunc->function, baseAddr, baseaddr_mask,
			       addr);
#ifdef PCI_DEVCONFIG_PRINTS
        printf("\n&&&&&&&&BASE ADDRESS = %x&&&&&&&&&&&",addr);
#endif
	if(register64Bit)
	    {
	    /*
	     * Write the base address for 64-bit addressable memory devices:
	     * initialize the next base address register to zero, the PReP
	     * address map does support physical addresses above 4GB (i.e.,
	     * 32-bit address space)
	     */

	    	pciConfigOutLong (pciFunc->bus, pciFunc->device, pciFunc->function,
			      baseAddr + 4, 0);
	    }

	/* Update the allocation pointer */

	 *allocP = addr;

	/* Set the appropriate enable bit */

	sysPciConfigInsertLong (pciFunc->bus, pciFunc->device,
			        pciFunc->function, PCI_CFG_COMMAND,
			        spaceEnable, spaceEnable);
	}
    return (register64Bit);
    }

/******************************************************************************
*
* pciIospaceSelect - Select appropriate I/O space for device.
*
* This routine determines which PCI I/O space (16-bit or 32-bit) to assign
* to a particular function.  Note that functions located on subordinate
* busses must be  assigned to the 16 bit PCI I/O space due to 16 bit
* decoder limitations of certain bridges and functions.  The PCI
* specification limits each I/O base address register to 256 bytes, so
* this size should not be a problem.
*
* RETURNS: 0, indicating I/O space not located anywhere in 64-bit space.
*/

LOCAL
UINT pciIospaceSelect
    (
    register PCI_LOC *pciFunc,	/* input: Pointer to PCI function element     */
    register UINT **allocP,	/* output: Pointer to PCI space alloc pointer */
    register UINT *boundary	/* output: PCI space bottom boundary pointer  */
    )
    {
    register struct pciUnique *pciuP;	/* PCI unique structure	  */
    register UINT   register64Bit = 0;	/* 64 bit register flag	  */
    

    /* Get access to the PCI function array */

    pciuP = &PCI_UNIQUE;

    /* Assign this register to PCI I/O space */

    *allocP = &pciuP->pciIo32;
    *boundary = CPU_PCI_IO_ADRS;
		

    return register64Bit;
    }

/******************************************************************************
*
* pciMempaceSelect - Select appropriate memory space for device.
*
* This routine determines which PCI memory space pool to use for assignment
* to a particular function.  Note that functions located on subordinate
* busses must be  assigned to the 32 bit PCI memory space due to 32 bit
* requirements of functions using more than 1MB memory space.
*
* RETURNS: 1 if 64-bit addressable memory space, 0 otherwise.
*/

LOCAL
UINT pciMemspaceSelect
    (
    register PCI_LOC *pciFunc,	/* input: Pointer to PCI function element     */
    register UINT **allocP,	/* output: Pointer to PCI space alloc pointer */
    register UINT *boundary,	/* output: PCI space bottom boundary pointer  */
    register UINT addrInfo	/* input: PCI address type information	      */
    )
    {
    register struct pciUnique *pciuP;	/* PCI unique structure */
    register UINT register64Bit = 0;	/* 64 bit register flag */

    /* Get access to the PCI function array */

    pciuP = &PCI_UNIQUE;

    /* Assign this register to PCI memory space */

    switch (addrInfo)
	{
	case (PCI_BASEADDR_MEM_64BIT | PCI_BASEADDR_MEM):
	    /*
	     * Check for a 64-bit addressable memory device, currently
	     * the PReP address map does not support physical addresses
	     * above 4Gb (i.e., 32-bits), so the configuration process
	     * will initialize the upper base register to zero (i.e.,
	     * the safe thing to do), so for right now we'll skip the
	     * next base address register which belongs to the 64-bit
	     * pair of 32-bit base address registers.
	     */

	    register64Bit = 1;
	    /* Fall through */

	case (PCI_BASEADDR_MEM_32BIT | PCI_BASEADDR_MEM):
	    *allocP = &pciuP->pciMem32;
	    *boundary =	 CPU_PCI_MEM_ADRS;
	    break;

	default:
	    /* Do not configure unknown types */

	    *allocP = &pciuP->pciMem32;
	    *boundary = 0xFFFFFFFF;
	    break;
	}
    return register64Bit;
    }

/******************************************************************************
*
* pciBusConfig - Set mem and I/O registers for a single PCI-PCI bridge.
*
* This routine	sets up memory and I/O base/limit registers for
* an individual PCI-PCI bridge.
*
* PCI/PCI bridges require that memory space assigned to functions
* on the secondary bus reside in one common block defined by a memory
* base register and a memory limit register, and I/O space assigned to
* functions on the secondary bus must reside in one common block defined
* by an I/O base register and an I/O limit register.  This effectively
* prevents the use of 32 bit PCI I/O space on subordinate (non-zero)
* busses due to the 16 bit decoder limitations of certain bridges and
* functions.
*
* Note that functions which use 20 bit memory decoders must be placed
* on bus zero, or this code will have to be modified to impose a 20 bit
* restriction on all subordinate busses (since some PCI devices allocate
* huge memory spaces, a 1MB total restriction is unacceptable).  Also note
* that this code does not provide prefetchable memory support at this time.
*
* Warning: do not sort the include function list before this routine is
* called.  This routine requires each function in the list to be in the
* same order as the probe occurred.
*
* RETURNS: N/A
*/

LOCAL
void pciBusConfig
    (
    register PCI_LOC *pciLoc,	/* input: PCI address of this bridge	   */
    register PCI_LOC **pciList,	/* input: Pointer to function list pointer */
    register UINT *nSize	/* input: Number of remaining functions	   */
    )
    {
    register struct pciUnique *pciuP;	/* PCI unique structure	      */
    UCHAR bus;				/* Bus number for current bus */
    STATUS stat;

    /* Get a pointer to the PCI unique structure */

    pciuP = &PCI_UNIQUE;

    /*
     * Adjust I/O & mem pointers to granularity of bridge
     * Bridges have a 4K I/O granularity and a 1Meg memory granularity.
     */


#ifdef TL01d
    pciuP->pciIo32 &= ~0xFFF;
    pciuP->pciMem32 &= ~0xFFFFF;
#else
    pciuP->pciIo32 &= ~0x0FFF;
    pciuP->pciMem32 &= ~0x0FFFFF;
#endif


    /* Shut off PCI memory space prefetching */
    sysPciConfigInsertLong (pciLoc->bus, pciLoc->device, pciLoc->function,
			    PCI_CFG_PRE_MEM_BASE, 0xfff0fff0, 0x0000fff0);
    pciConfigOutLong (pciLoc->bus, pciLoc->device, pciLoc->function,
		      PCI_CFG_PRE_MEM_BASE_U, 0xffffffff);
    pciConfigOutLong (pciLoc->bus, pciLoc->device, pciLoc->function,
		      PCI_CFG_PRE_MEM_LIMIT_U, 0);



     /*
     * Write starting I/O pointer as I/O limit and starting mem pointer
     * as mem limit
     */


    stat = sysPciConfigInsertLong (pciLoc->bus, pciLoc->device, pciLoc->function,
			    PCI_CFG_IO_BASE, 0x0000f000, pciuP->pciIo32-1);


    pciConfigOutWord (pciLoc->bus, pciLoc->device, pciLoc->function,
		      PCI_CFG_IO_LIMIT_U, (pciuP->pciIo32 - 1) >> 16);

    stat = sysPciConfigInsertLong (pciLoc->bus, pciLoc->device, pciLoc->function,
			    PCI_CFG_MEM_BASE, 0xfff00000, pciuP->pciMem32 - 1);

#ifdef PCI_DEVCONFIG_PRINTS
	printf("\nPCI-PCI bridge PCI mem. limit = %x",(pciuP->pciMem32 - 1)& 0xFFF00000);
#endif

#ifdef DEBUG_PCIAUTO
	printf("DBG pciBusConfig bus did MEM_LIMIT ");
	if (stat != OK)
		printf(" ERROR from InsertLong "  );
        taskDelay (20);
#endif


    /* Allocate and assign space to each device on this bus */

    pciConfigInByte (pciLoc->bus, pciLoc->device, pciLoc->function,
		     PCI_CFG_SECONDARY_BUS, &bus);
    pciDevicesConfig (bus, pciList, nSize);

    /* Adjust the pointer alignments again to reserve these two blocks */

#ifdef TL01d
    pciuP->pciIo16 &= ~0xFFF;
    pciuP->pciMem32 &= ~0xFFFFF;
#else
    pciuP->pciIo32 &= ~0x0FFF;
    pciuP->pciMem32 &= ~0x0FFFFF;
#endif

    /* Write ending I/O pointer as I/O base & ending mem pointer as mem base */

    stat = sysPciConfigInsertLong (pciLoc->bus, pciLoc->device, pciLoc->function,
			    PCI_CFG_IO_BASE, 0x000000F0, pciuP->pciIo32 >> 8);

#ifdef DEBUG_PCIAUTO
	printf("DBG pciBCfg2 bus %x dev %x func %x",
    		pciLoc->bus, pciLoc->device, pciLoc->function );
	printf(" IOBASE %x ",  pciuP->pciIo16 );
	if (stat != OK)
		printf(" ERROR from InsertLong "  );
        taskDelay (20);
#endif

    pciConfigOutWord (pciLoc->bus, pciLoc->device, pciLoc->function,
		      PCI_CFG_IO_BASE_U, pciuP->pciIo32 >> 16);

    stat = sysPciConfigInsertLong (pciLoc->bus, pciLoc->device, pciLoc->function,
			    PCI_CFG_MEM_BASE, 0x0000fff0,
			   pciuP->pciMem32 >> 16);

#ifdef PCI_DEVCONFIG_PRINTS
    printf("\nPCI-PCI bridge PCI mem. base = %x",(pciuP->pciMem32 >> 16));
#endif

#ifdef DEBUG_PCIAUTO
	printf("DBG pciBCfg3 ");
	if (stat != OK)
		printf(" ERROR from InsertLong "  );
        taskDelay (20);
#endif

#if 0
    /* Added for local config */
    sysPciConfigLocalBridge ( pciLoc->bus, pciLoc->device, pciLoc->function);
#endif

    }

/******************************************************************************
*
* pciFunctionInit - Initialize several config registers on single PCI function.
*
* This routine initializes a single PCI function as follows:
* .IP "   -"
* Initialize the cache line size register to 8 (32 bytes cache line)
* .IP "   -"
* Initialize the PCI-PCI bridge latency timers to 128
* .IP "   -"
* Enable the master PCI bit for non-display devices
* .IP "   -"
* (Note: interrupt line is initialized later in pciAllIntlinesSet())
* .LP
*
* RETURNS: N/A
*
*/

LOCAL
void pciFunctionInit
    (
    register PCI_LOC *pciFunc	/* input: Pointer to PCI function structure */
    )
    {
    short pciClass;		/* PCI class/subclass contents */

    /* Initialize the cache line size register */

    sysPciConfigInsertLong (pciFunc->bus, pciFunc->device, pciFunc->function,
			    PCI_CFG_CACHE_LINE_SIZE, 0xff, PCI_CLINE_SZ);

    /* Initialize the latency timer */

    pciConfigOutByte (pciFunc->bus, pciFunc->device, pciFunc->function,
		      PCI_CFG_LATENCY_TIMER, PCI_LAT_TIMER);

    /* Get the PCI class code */

    pciConfigInWord (pciFunc->bus, pciFunc->device,
#ifdef TL01d
    pciFunc->function, PCI_CFG_CLASS, &pciClass);
#else
    pciFunc->function, PCI_CFG_SUBCLASS, &pciClass);
#endif


    /* Initialize specific subclasses */

    switch (pciClass)
	{
	case ((PCI_CLASS_BRIDGE_CTLR << 8) + PCI_SUBCLASS_P2P_BRIDGE):

	    /* Initialize primary and secondary PCI-PCI bridge latency timers */

	    pciConfigOutByte (pciFunc->bus, pciFunc->device,
			      pciFunc->function, PCI_CFG_SEC_LATENCY,
			      PCI_LAT_TIMER);

	    /* Enable I/O and memory space decoders for subordinate bus */

	    sysPciConfigInsertLong (pciFunc->bus, pciFunc->device,
				   pciFunc->function, PCI_CFG_COMMAND, 3, 3);
	    break;

	default:
	    break;
	}

    /* Enable mastering */

    sysPciConfigInsertLong (pciFunc->bus, pciFunc->device, pciFunc->function,
			    PCI_CFG_COMMAND, PCI_CMD_MASTER_ENABLE,
			    PCI_CMD_MASTER_ENABLE);
    }


/*****************************************************************************
*
* sysPciConfigInsertLong - Insert field into PCI configuration header
*
* This function writes a field into a PCI configuration header without
* altering any bits not present in the field.  It does this by first
* doing a PCI configuration read (into a temporary location) of the PCI
* configuration header word which contains the field to be altered.
* It then alters the bits in the temporary location to match the desired
* value of the field.  It then 
* writes back the temporary location with
* a configuration write.  All configuration accesses are long and the
* field to alter is specified 
* by the "1" bits in the 'bitMask' parameter.
*
* RETURNS: OK if operation succeeds or ERROR if operation fails.
*/

STATUS sysPciConfigInsertLong
    (

    ULONG busNo,          /* bus number */
    ULONG deviceNo,       /* device number */
    ULONG funcNo,         /* function number */
    ULONG offset,         /* offset into the configuration space */
    ULONG bitMask,        /* Mask which defines field to alter */
    ULONG data            /* data written to the offset */
    )
    {
    UINT temp;
    STATUS stat;
    

   if (offset & 3)
   {
	stat = ERROR;
	goto InsertLongX;
   }


    stat = pciConfigInLong(busNo, deviceNo, funcNo, offset, &temp);
    if (stat != OK)
        goto InsertLongX;
    temp = (temp & ~bitMask) | (data & bitMask);
    stat = pciConfigOutLong(busNo, deviceNo, funcNo, offset, temp);
InsertLongX:
    return(stat);
    }

#if 0
/******************************************************************************
*
* sysPciBridgeMap - configure the DEC21150 PCI-to-PCI Bridge
*
* This routine configures the DEC21150 PCI-to-PCI Bridge on the PCI Span.
*
* RETURNS: OK or ERROR if pciIomapLib has not been initialized.
*/

STATUS
sysPciBridgeMap 
	(
	int         pciBusNo,       /* PCI bus number */
	int         pciDevNo,       /* PCI device number */
	int         pciFuncNo,      /* PCI function number */
	PCI_SPAN *  pciSpan         /* pointer to PCI Span config array */
	)
   {
	STATUS      result = OK;
	FAST        i;





/* Write all parameters in pcmSpan in the order given */

   for (i = 0; (pciSpan[i].parmSize != END_SIZE) && (result == OK); ++i)
   {
	switch (pciSpan[i].parmSize)
	{
	case 1:
	   result = pciConfigOutByte (pciBusNo, pciDevNo, pciFuncNo, 
		  		   pciSpan[i].parmOffset, pciSpan[i].parmValue);
	   break;

	case 2:
	   result = pciConfigOutWord (pciBusNo, pciDevNo, pciFuncNo, 
				   pciSpan[i].parmOffset, pciSpan[i].parmValue);
	   break;

	case 4:
	   result = pciConfigOutLong (pciBusNo, pciDevNo, pciFuncNo, 
					pciSpan[i].parmOffset,
					pciSpan[i].parmValue);
	   break;
	}
   }

   return (result);
}

/***********************************************************************
*
* sysPciConfigLocalBridge - Custom Config local for bridge on bus zero 
*
* RETURNS: N/A.
*/

void sysPciConfigLocalBridge ( int bus, int device, int func )
 {

	/* Exit if bridge not on bus zero */
	if (bus != 0) return;


	if ( sysPciBridgeMap ( bus, device, func, &sysPCISpanParmShort[0] ) 
		== ERROR)
        {
#ifdef DEBUG
          logMsg ("sysPciConfigLocalBridge: ERROR configuring Bridge ", 1, 2, 3, 4, 5, 6);
          logMsg (" bus %x device %x func %x \n", bus, device, func , 4,5,6);
#endif
        }
 }
#endif
