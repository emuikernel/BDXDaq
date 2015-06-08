/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

********************************************************************************
* pciScan.c
*
* DESCRIPTION:
*	This module provides a facility for automated PCI device scanning and
*	configuration on PCI-based systems.
*       Resource requirements of each device are identified and allocated 
*	according to system resource pools that are automatically detected 
*	(the GT PCI address space available). For example PCI0 MEM0 address
*	space is defined as a pool.
*	This driver also enables access to the address spaces allocated by the 
*	driver in case of the system supports MMU.
*		
*   	This driver is designed to deal with PCI0 only. In order to 
*	refer to any PCI (0/1) the frcPciSetActive() routine is called. 
*	This routine set WRS pciConfigLib with the appropriate read/write 
*	routine (according to the routine's argument). This enables the use of 
*	WRS PCI API on both PCI buses. 
*   	Thus, in case of reading/writing to a PCI, first set the active 
*	PCI number by using the frcPciSetActive(PCI_NUMBER) function and only 
*	then use the functions frcPciConfigRead to read from PCI and 
*	frcPciConfigWrite for write.
*
* RESTRICTIONS:
*	- This driver supports read/write transaction to PCI bus segment 0 only.
*	- Using the driver debug facility requires the pciScan() to be called 
*	from a point where the console driver is already installed to allow
*	debug print. 
* 	Note: In MMU enabled systems this will cause the PCI address space 
*		NOT to be accessible !! Modification to sysPhysMemDesc[] are 
*		applicable only within sysHwInit() in sysLib.c. 
*	Note: Operate the driver debug facility only when the pciScan call 
*		is out of sysHwInit() routine !!
*
* DEPENDENCIES:
*		WRS pciConfigLib.
* Sept. 20th 2002
* 1) Changed pciScan function. Now calling pciAutoConfig in pciScan to Scan all PCI buses and allocating PCI resources to all PCI devices.
*******************************************************************************/

/* includes */ 
#include <vxWorks.h>
#include <intLib.h>
#include <sysLib.h>
#include <stdio.h>

#include "frcBoardId.h"
extern unsigned char frcSysBoardId[];

/* This MUST always be 1 (used in pci.target.h) */
#define	PCI_TARGET_SPECIFIC_FUNCTIONS	1

#include "gtCore.h"
#include "gtPci.h"
#include "pciScan.h"
#include "pciConfigLib.h"
#include "pciAutoConfig.h"

/* defines */
		
/* Driver debug control */
#define DRV_DEBUG

/* Driver debug control */
#ifdef DRV_DEBUG
	#define DRV_DEBUG_OFF		0x00
	#define DRV_DEBUG_INIT		0x01
	#define DRV_DEBUG_MEM_ALLOC	0x02
	#define DRV_DEBUG_SCAN	   	0x04
	#define DRV_DEBUG_READ	   	0x08
	#define DRV_DEBUG_WRITE	   	0x10
	#define DRV_DEBUG_ALL    	0x1f

	int	pciDebug = DRV_DEBUG_ALL;

	#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)				\
		{															\
		if (pciDebug & FLG)											\
			if (_func_logMsg != NULL)								\
				_func_logMsg (X0, X1, X2, X3, X4, X5, X6);			\
		}

	#define DRV_PRINT(FLG, X)										\
		{															\
		if (pciDebug & FLG) printf X;								\
		}

#else /* DRV_DEBUG */

	#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)
	#define DRV_PRINT(FLG, X)

#endif /* DRV_DEBUG */
		
#define MINIMUM_BAR_SIZE		0x1000
#define PCI_MEMIO_ADDR_SPACES		5	/* MEM0, MEM1, MEM2, MEM3, IO */

#define POOL_BASE(poolNum)  pPciMemIoPool[poolNum].baseAddr
#define POOL_NEXT(poolNum)  pPciMemIoPool[poolNum].nextFreeBlk
#define POOL_TOP(poolNum)   pPciMemIoPool[poolNum].topAddr
#define POOL_SIZE(poolNum)	(POOL_NEXT(poolNum) - POOL_BASE(poolNum))

#define MV_DEV_NUM(pciNum)	((GTREGREAD((PCI_0_P2P_CONFIG + (0x80 * pciNum))) \
																>> 24) & 0x1F)

/* typedefs */
typedef struct _pciMemIoPool
{
	unsigned int baseAddr;
	unsigned int nextFreeBlk;
	unsigned int topAddr;
} PCI_MEMIO_POOL;

typedef enum _pciMemIoPools {MEM0, MEM1, MEM2, MEM3, IO } PCI_MEMIO_POOLS;


/* Locals */

/* Local routines */

/* PCI_MECHANIZM_0 - user PCI configuration read and write */
LOCAL STATUS pci0SetConfigReg(int, int, int, int, UINT32);
LOCAL STATUS pci0GetConfigReg(int, int, int, int, UINT32*);

extern FUNCPTR frcPciConfigRead;
extern FUNCPTR frcPciConfigWrite;
extern FUNCPTR pciConfigSpcl;

IMPORT UINT8 frcGetVariant(void);

extern int pciConfigLibInit
    (
    int mechanism, /* configuration mechanism: 0, 1, 2 */
    ULONG addr0,   /* config-addr-reg / CSE-reg */
    ULONG addr1,   /* config-data-reg / Forward-reg */
    ULONG addr2	   /* none            / Base-address */
    );

extern int pciDevConfig
    (
    int pciBusNo,          /* PCI bus number */
    int pciDevNo,          /* PCI device number */
    int pciFuncNo,         /* PCI function number */
    UINT32 devIoBaseAdrs,  /* device IO base address */
    UINT32 devMemBaseAdrs, /* device memory base address */
    UINT32 command         /* command to issue */
    );

LOCAL void 	 pciScanInit        (void); 
/**********************IMPORTS************************************/

IMPORT void vxHid1Set(UINT32);
IMPORT UINT32 vxHid1Get();

/* Global variables */
PCI_NUMBER pciActive;      /* Represents the active PCI number 0 or 1	*/

#define MAX_BUSES	10

/*******************************************************************************
* pciScan - Scans the PCI0/1 and perform basic devices configuration.
*
* DESCRIPTION:
*	This function initiates the PCI0 scan process. 
*	The function has five phases:
* 	1) Init stage. Clean data structures and connect to WRS	pciConfigLib 
*		with PCI0 Read/Write routines (See pciScanInit).
* 	2) Scanning PCI0 for PCI devices.
* 	3) Allocating system PCI0 address space resource according to PCI0 
*		scan results.
* 	4) Configure the PCI devices found on the system PCI0.
*	5) In case the system supports MMU enable access to the allocated PCI 
*		memory spaces.
*
* INPUT:
*	None.
*
* OUTPUT:
*	When the routine is done all PCI0/1 devices are accessible using the 
*	address space allocated.
*
* RETURN:
*	OK    if scan passed and all resources are available and accessible.
*	ERROR if the system is out of address space resource or the address 
*			  space allocated is not accessible (MMU enable failed).
*
*******************************************************************************/
STATUS pciScan(int bus)
{
    UINT32 temp;
 
    /* Make sure that MPP[4] is made GPP[4] */
    GT_REG_READ(MPP_CONTROL0,&temp);
    temp = temp & 0xfff0ffff;
    GT_REG_WRITE(MPP_CONTROL0,temp);

    /* Make sure that GPP[4] is an Input pin */
    GT_REG_READ(GPP_IO_CONTROL,&temp);
    temp = temp & 0xffffffef;
    GT_REG_WRITE(GPP_IO_CONTROL, temp);

    /* Make sure GPP[4] is active high */
    GT_REG_READ(GPP_LEVEL_CONTROL,&temp);
    temp = temp & 0xffffffef;
    GT_REG_WRITE(GPP_LEVEL_CONTROL, temp);

    /* Check wether GT is in monarch mode using GPP[4] */
    GT_REG_READ(GPP_VALUE, &temp);
    if ((temp & 0x00000010) == 0x00000000) {
        /* Monarch mode */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
        if(frcWhoAmI())
            dbg_puts1("CPU1:The GT is in the Monarch mode\n");
        else
            dbg_puts0("CPU0:The GT is in the Monarch mode \n");
#else
        dbg_puts0("The GT is in the Monarch mode\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

        printf("The GT is in Monarch mode \n");   

        /* Make sure MPP[31] is made as GPP[31] */
        GT_REG_READ(MPP_CONTROL3, &temp);
        temp = temp & 0x0fffffff;
        GT_REG_WRITE(MPP_CONTROL3, temp);
	
        /* Configure the EREADY-IN signal ( GPP[31]) as input */
        GT_REG_READ(GPP_IO_CONTROL, &temp);
        temp = temp & 0x7fffffff;
        GT_REG_WRITE(GPP_IO_CONTROL, temp);
	
        /* Make sure GPP[31] is active high */
        GT_REG_READ(GPP_LEVEL_CONTROL, &temp);
        temp = temp & 0x7fffffff;
        GT_REG_WRITE(GPP_LEVEL_CONTROL, temp);
	
        /* Wait for EREADY signal to go HIGH to do the pciScan, 
           if not be in the loop */
        printf("Waiting for EREADY to go High.......\n");
        	
        do {
            GT_REG_READ(GPP_VALUE, &temp);    	    	     
        } while ((temp & 0x80000000) == 0x00000000 ); 
	
        printf("EREADY pulled High.\n"); 
        	
        pciScanInit();
        frcPciSetActive(PCI_NUM0);

        if(pciActive == PCI_NUM0)
            pci0SetConfigReg(PCI_BUS0,0,0,PCI0_STATUS_COMMAND_REG,0x2800007);
        else
        {
        }     
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
        if(frcWhoAmI())
            dbg_puts1("CPU1: Entering pciautoconfigMonarch\n");
        else
            dbg_puts0("CPU0: Entering pciautoconfigmonarch\n");
#else
        dbg_puts0("Entering pciautoconfigMonarch\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
        if(!frcWhoAmI())
    	    pciAutoConfig();     /* Allocate resources only if it Monarch and it is CPU0*/
	else
            pciAutoConfigNonMonarch();   /* Scan the bus if it is CPU1 */
#else
        pciAutoConfig();         /* Allocate resources  if it is a single CPU
                                    in the Monarch mode */
#endif  /* PMC280_DUAL_CPU */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
        if(frcWhoAmI())
            dbg_puts1("CPU1: Exiting pciautoconfigMonarch\n");
        else
            dbg_puts0("CPU0: Exiting pciautoconfigmonarch\n");
#else
        dbg_puts0("Exiting pciautoconfigMonarch\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

        /* Make sure the MV is enabled on the PCI 0*/
    	pci0SetConfigReg(PCI_BUS0,0,0,PCI0_STATUS_COMMAND_REG,0x02800007);   

        /* Set the intenal register space from the PCI side */
        gtPci0MapInternalRegSpace(INTERNAL_REG_ADRS);

    }
    else
    {
        /* Non Monarch mode */
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
        if(frcWhoAmI())
            dbg_puts1("CPU1: The GT is in the NonMonarch mode\n");
        else
            dbg_puts0("CPU0: The GT is in the NonMonarch mode\n");
#else
        dbg_puts0("The GT is in the NonMonarch mode\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_VX */
        printf("GT is in Non-monarch mode \n");

        /* Make sure MPP[8] is GPP[8] */
        GT_REG_READ(MPP_CONTROL1, &temp);
        temp = temp & 0xfffffff0;
        GT_REG_WRITE(MPP_CONTROL1, temp);

        /* Make MPP[8] as O/P pin */
        GT_REG_READ (GPP_IO_CONTROL, &temp);
        temp = temp | 0x00000100;
        GT_REG_WRITE (GPP_IO_CONTROL, temp);

        /* Make GPP[8] as ative high */
        GT_REG_READ(GPP_LEVEL_CONTROL, &temp);
        temp = temp & 0xfffffeff;
        GT_REG_WRITE(GPP_LEVEL_CONTROL, temp);

        /* Drive EREADY OUT  low */
        GT_REG_READ(GPP_VALUE, &temp);
        temp = temp & 0xfffffeff;
        GT_REG_WRITE(GPP_VALUE, temp);

        printf("Drove EreadyOut pin low.\n");        

       	pciScanInit();
        frcPciSetActive(PCI_NUM0);
        if(pciActive == PCI_NUM0)
            pci0SetConfigReg(PCI_BUS0,0,0,PCI0_STATUS_COMMAND_REG,0x2800007);
        else
        {
        }

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
        if(frcWhoAmI())
            dbg_puts1("CPU1: ENTERING pciautoconfigNonmonarch\n");
        else
            dbg_puts0("CPU0: Entering pciautoconfignonmonarch\n");
#else
        dbg_puts0("Entering pciautoconfignonmonarch\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    	pciAutoConfigNonMonarch();

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
        if(frcWhoAmI())
            dbg_puts1("CPU1: Exiting pciautoconfignonmonarch\n");
        else
            dbg_puts0("CPU0: Exiting pciautoconfignonmonarch\n");
#else
        dbg_puts0("Exiting pciautoconfignonmonarch\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */    
    }

    if(bus == PCI_NUM0)
          pci0SetConfigReg(PCI_BUS0, 0, 0, PCI_STATUS_AND_COMMAND, 0x2800007);

    /* Configure PMC280's subsystem device and vendor IDs */
    switch(frcGetVariant())
    {
        case VARIANT_A: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_A; break;
        case VARIANT_B: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_B; break;
        case VARIANT_C: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_C; break;
        case VARIANT_D: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_D; break;
        case VARIANT_E: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_E; break;
        case VARIANT_F: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_F; break;
        case VARIANT_G: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_G; break;
        case VARIANT_H: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_H; break;
        case VARIANT_I: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_I; break;
        case VARIANT_J: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_J; break;
        case VARIANT_K: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_K; break;
        case VARIANT_L: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_L; break;
        case VARIANT_M: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_M; break;
        case VARIANT_N: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_N; break;
        case VARIANT_O: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_O; break;
        case VARIANT_P: temp = PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_P; break;
        default:        temp = 0xFFFFFFFF; break; /* Default value of MV64360 */
    }

    if(bus == PCI_NUM0)
        pci0SetConfigReg(PCI_BUS0, 0, 0, 
                         PCI_SUBSYSTEM_ID_AND_SUBSYSTEM_VENDOR_ID, 
                         temp);

    DRV_PRINT(DRV_DEBUG_INIT, ("Starting PCI scanning...\n"))

    DRV_PRINT(DRV_DEBUG_INIT, ("PCI scan done.\n"))

    return OK;
}

/*******************************************************************************
* pciScanInit - Prepare PCI0/1 for scan and configuration operation.
*
* DESCRIPTION:
*	In order to prepare the driver for operation this function perform 
*	the following steps:
*	1) Initialize WRS PCI library "pciConfigLib" with configuration 
*	access method mechanism 0. Assign PCI0 read/write routines as user
*	defined read/write configuration routines.
*	2) Clear the data structures which will host the scan results.
*	3) Initialize the PCI0/1 system resource pools.
*
* INPUT:
*	None.
*
* OUTPUT:
*	1) After using this function, it is possible to use WRS PCI read/write 
*	configuration API. 
*       2) System memory pools ready for allocation.
*
* RETURN:
*	None.
*
*******************************************************************************/
LOCAL void pciScanInit(void)
{
    /* Set pciConfigLib with the user read/write functions for PCI0. */
    pciConfigLibInit(PCI_MECHANISM_0, (ULONG)pci0GetConfigReg, 
                     (ULONG)pci0SetConfigReg, 0);
}


/*******************************************************************************
* frcPciSetActive - Set PCI driver to perform PCI0 transactions
*
* DESCRIPTION:
* 	This function enables the WRS PCI configuration API to address 
*	PCI0 only.
*
* INPUT:
*	pciNumber  -  PCI number. See PCI_NUMBER enumerator.
*
* OUTPUT:
*	1) Set the pciConfigLib routines pointers for configuration read/write 
*	routines according to the given PCI number.
* 	For example, when pciNum = 1: 
*    	frcPciConfigRead  = (FUNCPTR)pci1GetConfigReg; 
* 	frcPciConfigWrite = (FUNCPTR)pci1SetConfigReg;
* 	2) Set pciActive global variable which indicate the active PCI .  
*
* RETURN:
*	None.
*
*******************************************************************************/
void frcPciSetActive(PCI_NUMBER pciNumber)
{
    frcPciConfigRead  = (FUNCPTR)pci0GetConfigReg;
    frcPciConfigWrite = (FUNCPTR)pci0SetConfigReg;
    pciActive = PCI_NUM0;
}

/******************************************************************************
* Function     : pci0SetConfigReg                                  
*
* Description  : Write to a device PCI configuration register.
*                                                                             
* Inputs       : bus - PCI bus number (bus number, not PCI number 0 or 1)    
*                dev - PCI device number.                                           
*                RegNum - The register number to write to.                          
*                RegData - Data to write.                                           
* Outputs      : None.
*
* Returns      : OK on success                                                                                                                       
*********************************************************************************/
int pci0SetConfigReg(int bus, int devin,int func, int RegNum, UINT32 RegData)
{
    unsigned int data;
    int key;
    int dev = devin;


 /* Check for validity of BDF */
    if( (bus  < 0) || (bus  > MAX_BUSES)  ||  (dev  < 0) || (dev  > MAXPCIDEV) ||  (func < 0) || (func > MAXPCIFUNC) )
    {
#ifdef DEBUG_PCI
	    printf("Error bus = %d,dev = %d,func = %d\n",bus,dev,func);
#endif
	    return ERROR;
    }
key = intLock();


    data = (bus << 16) | (dev << 11) | (func << 8) | (RegNum & 0x1fc) | 0x80000000;

    if((data%4)!=0)
    {
#ifdef DEBUG_PCI
        printf("ERROR - illigal unalligned configuration write\n");  
#endif /* DEBUG_PCI */
        intUnlock(key);
        return ERROR;
    }

    GT_REG_WRITE(PCI_0_CONFIG_ADDR, data);
 /*   sysDelay();*/
    GT_REG_WRITE(PCI_0_CONFIG_DATA_VIRTUAL_REG, RegData);

    intUnlock(key);
    return OK;
}

/******************************************************************************
* function     : pci0GetConfigReg                                
*
* Description  : Reads from a device PCI configuration register.                                                                              
*
* Inputs       :  bus - PCI bus number                                                
*                 dev - PCI device number.                                            
*                 RegNum - The register number to read from.                          
*
* Outputs      : RegData - Data read.                                                
*
* Returns      : OK on success
*******************************************************************************/
int pci0GetConfigReg(int bus, int devin, int func, int RegNum,UINT32 *RegData)
{
    unsigned int data;
    int key;
    int dev = devin;


   /* Check for validity of BDF */
    if( (bus  < 0) || (bus  > MAX_BUSES)  ||  (dev  < 0) || (dev  > MAXPCIDEV) ||  (func < 0) || (func > MAXPCIFUNC) )
    {
#ifdef DEBUG_PCI
	    printf("Error bus = %d,dev = %d,func = %d\n",bus,dev,func);
#endif
	    return ERROR;
    }

    key = intLock();

    
    data = (bus << 16) | (dev << 11)| (func << 8) | (RegNum & 0xfc) | 0x80000000;
    if((data%4)!=0)
    {
#ifdef DEBUG_PCI
        printf("ERROR - illigal unalligned configuration read on PCI0\n");
#endif /* DEBUG_PCI */
        intUnlock(key);
        return ERROR;
    }

/*    pciResetMasterAbortBit(PCI_NUM0);*/

    GT_REG_WRITE(PCI_0_CONFIG_ADDR, data);
    
    sysDelay();
    GT_REG_READ(PCI_0_CONFIG_DATA_VIRTUAL_REG, RegData);

if((~(*RegData)) && (*(UINT32*)(0xF1001D58) == 8) )
    {
#ifdef DEBUG_PCI
        printf("changing value of RegData from 0x%x to 0xffffffff\n",*RegData);
#endif /* DEBUG_PCI */
        *RegData = 0xffffffff;
    	*(UINT32*)(0xF1001D58) = 0x0;
        intUnlock(key);
        return ERROR;
    }

    intUnlock(key);
    return OK;
}


/*******************************************************************************
* frcPciConfigReadVPD - PCI0 configuration read routine for the VPD area.
*
* DESCRIPTION:
*        This function interfaces the BSP configuration read routine (located
*        in pci.c module) for the VPD area to read the contents of the VPD
*        register without a swap.
*
* INPUT:
*
*        bus    - PCI bus segment number (Not PCI bus number !!).
*        dev    - PCI device number.
*        funcNo - PCI function number.
*        offset - offset into the configuration space.
*        *pData - 32 bit data read from the offset.
*
* OUTPUT:
*        The status of the function.
*
* RETURN:
*        OK.
*
*******************************************************************************/
STATUS frcPciConfigReadVPD (int bus, int dev,int funcNo, int offset,UINT32 *pData)
{
   unsigned int data, temp;

    offset &= 0xFC;
    data = (bus << 16)|(dev << 11)|(funcNo << 8)|(offset)|PCI_CONFIG_ENABLE;
    GT_REG_WRITE(PCI_0_CONFIG_ADDR, data);
    GT_REG_READ(PCI_0_CONFIG_ADDR, &temp);
    if (data != temp)
        return 0xFFFFFFFF;

    GT_REG_READ_VPD(PCI_0_CONFIG_DATA_VIRTUAL_REG, &data);

    *pData=data;

    return OK;
}
/*******************************************************************************
* frcPciConfigWriteVPD - PCI0 configuration read routine for the VPD area.
*
* DESCRIPTION:
*        This function interfaces the BSP configuration write routine (located
*        in pci.c module) for the VPD area to write the contents of the VPD register
*        without a swap to fix the bug in the VPD data register wherein the contents
*	 are stored in the big-endian format.
*
* INPUT:
*
*        bus    - PCI bus segment number (Not PCI bus number !!).
*        dev    - PCI device number.
*        funcNo - PCI function number.
*        offset - offset into the configuration space.
*        *pData - 32 bit data read from the offset.
*
* OUTPUT:
*        The status of the function.
*
* RETURN:
*        OK.
*
*******************************************************************************/
STATUS frcPciConfigWriteVPD(int bus, int dev,int func, int offset,UINT32 data)
{
    volatile unsigned int addr;

    offset &= 0xFC;
    addr = (bus << 16)|(dev << 11)|(func << 8)|(offset) | PCI_CONFIG_ENABLE;

    GT_REG_WRITE(PCI_0_CONFIG_ADDR, addr);
    GT_REG_WRITE_VPD(PCI_0_CONFIG_DATA_VIRTUAL_REG, data);
    return OK;
}


/******************************************************************************
* frcPciShow - Display the PCI device data structure.
*
* DESCRIPTION:
*	This function displays pci0/1Device data structure which holds the
*	pci0/1ScanDevices() results.
*
* INPUT:
*	pciNumber  -  PCI number. See PCI_NUMBER enumerator.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
******************************************************************************/

void frcPciShow()
{
    int devnum,busno,funcno,data;
    UINT32 deviceVendorId , temp;
    UINT32 regVal;

    /* Disable Machine Check Exception */

	UINT32 udwMcpFlag=0,udwhid0;

    /*
     * Disable the Machine Check Before Reading this, because
     * if we read this , it generates the IACK cycle and it will
     * generate a Machine Check Exception
     */

    udwhid0 = vxHid1Get();
    if(udwhid0 & 0x30000000)
    {
	    vxHid1Set(udwhid0 & ~0xb0000000);
	    udwMcpFlag = TRUE;
    }


    GT_REG_READ(GPP_VALUE, &temp);
    if ((temp & 0x00000010) == 0x00000000)
    {
 	/* Monarch mode */
	printf("\nThe MV64360 is in Monarch mode");
    }
    else
	printf("\nThe MV64360 is in Non Monarch mode");


    /* Set the GT PCI0 agent as master on PCI 0 or PCI 1*/
    if(pciActive == PCI_NUM0)
        pci0SetConfigReg(PCI_BUS0,0,0,PCI0_STATUS_COMMAND_REG,0x2800007);
    else
    {
    }

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
                else
                {
#if 0	/*Configuring BRIDGE device done in pciAutoConfig*/
                    frcPciConfigRead(busno,devnum,funcno,8,&classCode);
                    if((classCode >> 24) == 6) /*Bridge device*/
                    {
                        data = busno | (secondary << 8) | (0xFF << 16);
                        frcPciConfigWrite(busno,devnum,funcno,24,data);
                        frcPciConfigRead(busno,devnum,funcno,4,&data);
                        data = data | 0x07;
                        frcPciConfigWrite(busno,devnum,funcno,4,data);
                        secondary++;
                    }
#endif

                    printf("\nBus no. 0x%x Device no. 0x%x Function 0x%x :Device Id 0x%x Vendor Id 0x%x",busno,devnum,funcno,(deviceVendorId >> 16),(deviceVendorId & 0xFFFF));
                }
                frcPciConfigRead(busno,devnum,funcno,0x0c,&data);
                if((data & 0x00800000) == 0x00000000)
                    break;
            }
	}
    }

    /*
     * Clear all the error registers related to PCI
     */

     /* clear pending PCI error interrupts */
     GT_REG_WRITE (PCI_0_ERROR_CAUSE, 0);
     GT_REG_WRITE (PCI_1_ERROR_CAUSE, 0);

     /*
      * Clear PCI Bus 0 and 1  events
      */
     GT_REG_WRITE(MAIN_INTERRUPT_CAUSE_LOW, ~(1<<12));
     GT_REG_READ (MAIN_INTERRUPT_CAUSE_LOW, &regVal);

     GT_REG_WRITE(MAIN_INTERRUPT_CAUSE_LOW, ~(1<<16));
     GT_REG_READ (MAIN_INTERRUPT_CAUSE_LOW, &regVal);

     /*
      * Clear all the Error bits forcefully
      * in PCI Command and Status Register
      */
    pciConfigInLong(0,0,0,PCI_CFG_COMMAND,&regVal);
    regVal = (regVal & 0x06FFFFFF);  /* Extract only RW Bits */
    regVal |= 0xF9000000; /* Force the RWC bits to Zeros by writing 1's */

    pciConfigOutLong(0,0,0,PCI_CFG_COMMAND,regVal);

	/* Enable Machine Check Exception */
    if(udwMcpFlag == TRUE)
    {
   	vxHid1Set(udwhid0 | 0xb0000000);
	udwMcpFlag = FALSE;
    }

    printf("\n");
    fflush (stdout);  /* Dummy print  to prevent the print of previuos call function on shell */

    return;
}


