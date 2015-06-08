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

 sysGei82543End.c - FORCE COMPUTERS PowerCoreCPCI-680 system-dependent library */

/* Copyright (c) 2001 FORCE COMPUTERS GmbH */

/*
modification history
--------------------
19-March-2001,brh Created
27-June-2001, brh Modified for lastest 82543 Driver Configuration
*/

/*
DESCRIPTION

This library provides board-specific routines for the PMC/Gigabit
Ethernet/82543GC controller. The PMC controller is software compatible
to Intel's PRO/1000T PCI Gigabit adapter. However, the PMC version does
not include a Flash device.

All these routines are necessary for the initialization and use
of the network subsystem.
*/

#include "vxWorks.h"
#include "taskLib.h"
#include "sysLib.h"
#include "config.h"
#include "vmLib.h"
#include "end.h"
#include "intLib.h"
#include "iv.h"
#include "pciConfigLib.h"
#include "pci.target.h"
#include "pmc280.h"
#include "pmc280_pci.h"


#if (defined(INCLUDE_NETWORK) && defined(INCLUDE_GEI_82543_END) \
     && defined (INCLUDE_END))


#include "drv/end/gei82543End.h"

/* PCI memory base addr register configuration type */

#define GEI_CFG_FORCE   0x01    /* overwrite membase addr reg. */
#define GEI_CFG_AUTO    0x02    /* read membase addr reg. */
#define GEI_CFG_TYPE  GEI_CFG_FORCE

/* Default RX descriptor  */

#ifndef GEI_RXDES_NUM
#define GEI_RXDES_NUM              GEI_DEFAULT_RXDES_NUM
#endif

/* Default TX descriptor  */

#ifndef GEI_TXDES_NUM
#define GEI_TXDES_NUM              GEI_DEFAULT_TXDES_NUM
#endif

/* Default User's flags  */

#ifndef GEI_USR_FLAG
#define GEI_USR_FLAG               GEI_DEFAULT_USR_FLAG
#endif

#define VM_STATE_MASK_FOR_ALL \
        VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE

#define VM_STATE_FOR_PCI \
        VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT

#define UNKNOWN                 -1
#define GEI_128KB               (128 * 1024)

#define GEI82543_MEMBASE_MASK       0xfffe0000  
#define GEI82543_FLASHBASE_MASK     0xffff0000

#define GEI82543_LOAD_FUNC          gei82543EndLoad
char geiEndLoadStr [GEI_MAX_UNITS] [END_DESC_MAX] = {{0}};

#define GEI_X86_OFFSET_VALUE        0x0

#define GEI0_INIT_STATE_MASK        (VM_STATE_MASK_FOR_ALL)
#define GEI0_INIT_STATE             (VM_STATE_FOR_PCI)
#define GEI0_SHMEM_BASE             NONE
#define GEI0_SHMEM_SIZE             0
#define GEI0_RXDES_NUM              GEI_RXDES_NUM
#define GEI0_TXDES_NUM              GEI_TXDES_NUM
#define GEI0_USR_FLAG               GEI_USR_FLAG

#define GEI1_INIT_STATE_MASK        (VM_STATE_MASK_FOR_ALL)
#define GEI1_INIT_STATE             (VM_STATE_FOR_PCI)
#define GEI1_SHMEM_BASE             NONE
#define GEI1_SHMEM_SIZE             0
#define GEI1_RXDES_NUM              GEI_RXDES_NUM
#define GEI1_TXDES_NUM              GEI_TXDES_NUM
#define GEI1_USR_FLAG               GEI_USR_FLAG

#define GEI2_INIT_STATE_MASK        (VM_STATE_MASK_FOR_ALL)
#define GEI2_INIT_STATE             (VM_STATE_FOR_PCI)
#define GEI2_SHMEM_BASE             NONE
#define GEI2_SHMEM_SIZE             0
#define GEI2_RXDES_NUM              GEI_RXDES_NUM
#define GEI2_TXDES_NUM              GEI_TXDES_NUM
#define GEI2_USR_FLAG               GEI_USR_FLAG

#define GEI3_INIT_STATE_MASK        (VM_STATE_MASK_FOR_ALL)
#define GEI3_INIT_STATE             (VM_STATE_FOR_PCI)
#define GEI3_SHMEM_BASE             NONE
#define GEI3_SHMEM_SIZE             0
#define GEI3_RXDES_NUM              GEI_RXDES_NUM
#define GEI3_TXDES_NUM              GEI_TXDES_NUM
#define GEI3_USR_FLAG               GEI_USR_FLAG

/* Alaska PHY's information */

#define MARVELL_OUI_ID              0x5043
#define MARVELL_ALASKA_88E1000      0x5
#define MARVELL_ALASKA_88E1000S     0x4
#define ALASKA_PHY_SPEC_CTRL_REG        0x10
#define ALASKA_PHY_SPEC_STAT_REG        0x11
#define ALASKA_INT_ENABLE_REG           0x12
#define ALASKA_INT_STATUS_REG           0x13
#define ALASKA_EXT_PHY_SPEC_CTRL_REG    0x14
#define ALASKA_RX_ERROR_COUNTER         0x15
#define ALASKA_LED_CTRL_REG             0x18

#define ALASKA_PSCR_ASSERT_CRS_ON_TX    0x0800
#define ALASKA_EPSCR_TX_CLK_25          0x0070

#define ALASKA_PSCR_AUTO_X_1000T        0x0040
#define ALASKA_PSCR_AUTO_X_MODE         0x0060

#define ALASKA_PSSR_DPLX                0x2000
#define ALASKA_PSSR_SPEED               0xC000
#define ALASKA_PSSR_10MBS               0x0000
#define ALASKA_PSSR_100MBS              0x4000
#define ALASKA_PSSR_1000MBS             0x8000

/* assuming 1:1 mapping for virtual:physical address */

#define GEI_SYS_WRITE_REG(unit, reg, value)     \
        ((*(volatile UINT32 *)((geiResources[(unit)].memBaseLow) + (reg))) = \
         (UINT32)LONGSWAP(value))


#define GEI_SYS_READ_REG(unit, reg) \
     ({ UINT32 temp = *(volatile UINT32 *)((geiResources[(unit)].memBaseLow) + (reg)); \
        LONGSWAP(temp); \
      })

/* externs */
extern STATUS sysPciConfigInsertLong
    (

    ULONG busNo,          /* bus number */
    ULONG deviceNo,       /* device number */
    ULONG funcNo,         /* function number */
    ULONG offset,         /* offset into the configuration space */
    ULONG bitMask,        /* Mask which defines field to alter */
    ULONG data            /* data written to the offset */
    );
IMPORT END_TBL_ENTRY    endDevTbl[];    /* end device table */
IMPORT	int noofgeidevices;
IMPORT  struct _ethernetdevice geidevices[GEI_MAX_UNITS];
#define GEI82543_BUFF_LOAN	1


/* typedefs */

typedef struct geiResource      /* GEI_RESOURCE */
    {
    UINT32      memBaseLow;     /* Base Address LOW*/
    UINT32      memBaseHigh;    /* Base Address HIGH */
    UINT32      flashBase;      /* Base Address for FLASH */
    char        irq;            /* Interrupt Request Level */
    BOOL        adr64;          /* True if PCI64  */
    int         boardType;      /* type of LAN board this unit is */
    int         pciBus;         /* PCI Bus number */
    int         pciDevice;      /* PCI Device number */
    int         pciFunc;        /* PCI Function number */
    UINT        memLength;      /* required memory size */
    UINT        initialStateMask; /* mask parameter to vmStateSet */
    UINT        initialState;   /* state parameter to vmStateSet */
    UINT16      eeprom_icw1;    /* EEPROM initialization control word 1 */
    UINT16      eeprom_icw2;    /* EEPROM initialization control word 2 */
    UCHAR       enetAddr[6];    /* MAC address for this adaptor */
    STATUS      iniStatus;      /* initialization perform status */
    UINT32      shMemBase;      /* Share memory address if any */
    UINT32      shMemSize;      /* Share memory size if any */
    UINT32      rxDesNum;       /* RX descriptor for this unit */
    UINT32      txDesNum;       /* TX descriptor for this unit */
    UINT32      usrFlags;       /* user flags for this unit */
    } GEI_RESOURCE;

/* locals */

LOCAL UINT32 geiUnits = 0;     /* number of GEIs found */

GEI_RESOURCE geiResources [] =
    {
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, GEI_128KB,
     GEI0_INIT_STATE_MASK, GEI0_INIT_STATE, 0, 0, {UNKNOWN},
     ERROR, GEI0_SHMEM_BASE, GEI0_SHMEM_SIZE,
     GEI0_RXDES_NUM, GEI0_TXDES_NUM, GEI0_USR_FLAG},
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, GEI_128KB,
     GEI1_INIT_STATE_MASK, GEI1_INIT_STATE, 0, 0, {UNKNOWN},
     ERROR, GEI1_SHMEM_BASE, GEI1_SHMEM_SIZE,
     GEI1_RXDES_NUM, GEI1_TXDES_NUM, GEI1_USR_FLAG},
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, GEI_128KB,
     GEI2_INIT_STATE_MASK, GEI2_INIT_STATE, 0, 0, {UNKNOWN},
     ERROR, GEI2_SHMEM_BASE, GEI2_SHMEM_SIZE,
     GEI2_RXDES_NUM, GEI2_TXDES_NUM, GEI2_USR_FLAG},
    {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
     UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, GEI_128KB,
     GEI3_INIT_STATE_MASK, GEI3_INIT_STATE, 0, 0, {UNKNOWN},
     ERROR, GEI3_SHMEM_BASE, GEI3_SHMEM_SIZE,
     GEI3_RXDES_NUM, GEI3_TXDES_NUM, GEI3_USR_FLAG},
     };

/* forward declarations */

LOCAL int       sys543IntEnable  (int vector);
LOCAL int       sys543IntDisable (int vector);
LOCAL int       sys543IntAck     (int unit);
LOCAL STATUS    sys543eepromCheckSum (int unit);
LOCAL UINT16    sys543eepromReadWord (int unit,UINT32);
LOCAL STATUS    sys543EtherAdrGet (int unit);
LOCAL void      sys543PhySpecRegsInit(PHY_INFO *, UINT8);
LOCAL STATUS sys543LoadStrCompose
    (
    int unit
    );

/*******************************************************************************
* sys543PciInit - Initialize and get the PCI configuration for 82543 Chips
*
* This routine finds the PCI device, and maps its PCI memory address.
* It must be done prior to initializing the 82543, sys543Init().  Also
* must be done prior to MMU initialization, usrMmuInit().
*
* RETURNS: N/A
*/


STATUS sys543PciInit(void)
{
    GEI_RESOURCE *pReso;         /* chip resources */
    int pciBus = 0;                /* PCI bus number */
    int pciDevice = 0;             /* PCI device number */
    int pciFunc = 0;               /* PCI function number */
    int unit = 0;                /* unit number */
    int pro1000TDevUnit=0;       /* count of Intel Pro1000T */
    int pro1000DevUnit =0;       /* count of Intel Pro1000F/T */
    UINT32 bar0;                 /* PCI BAR_0 */
    UINT32 memBaseLow;           /* mem base low */
    UINT32 memBaseHigh;          /* mem base High */
    UINT32 flashBase;            /* flash base */
    UINT16 boardId =0;           /* adaptor Id */

    
 /*   pciIntLibInit();*/

    for (unit = 0; unit < noofgeidevices; unit++)
    {

	boardId = PRO1000T_BOARD;

      /* 
       * The following PCI information is based on adapters (T/F) 
       * from Intel. 
       * PCI Device ID    SUB-SYSTEM ID    ADAPTOR
       *  0x1001           0x1003          INTEL PRO1000 F
       *  0x1001           0x1004          INTEL PRO1000 T
       *  0x1004             X             INTEL PRO1000 T
       */        

	if(geidevices[unit].deviceid == PRO1000T_PCI_DEVICE_ID)
	{
		pro1000TDevUnit++;
		boardId = PRO1000T_BOARD;	
		pciBus = geidevices[unit].busno;
		pciDevice = geidevices[unit].deviceno;
		pciFunc = 0;
	}
	else
	{

		/* Detect possible PRO1000T/F adaptors */
		if(geidevices[unit].deviceid == PRO1000_PCI_DEVICE_ID)
		{
			pro1000DevUnit++;
			pciConfigInWord(pciBus, pciDevice, pciFunc,PCI_CFG_SUB_SYSTEM_ID, &boardId);
			pciBus = geidevices[unit].busno;
			pciDevice = geidevices[unit].deviceno;
			pciFunc = 0;

			if (boardId == (UINT16)PRO1000F_PCI_SUBSYSTEM_ID)
				boardId = PRO1000F_BOARD;

		}
        	else
          		break;
	}
  
      /* found the right one */

      pReso = &geiResources [unit];
      pReso->boardType        = boardId;
      pReso->pciBus           = pciBus;
      pReso->pciDevice        = pciDevice;
      pReso->pciFunc          = pciFunc;
      pReso->memLength        = GEI_128KB;

      printf("\nGEI DEVICE ID , BUS NO = %x %x",pReso->pciDevice,pReso->pciBus); 	
      /* 
       * BAR0: [32:17]: memory base
       *       [16:4] : read as "0"; 
       *       [3]    : 0 - device is not prefetchable
       *       [2:1]  : 00b - 32-bit address space, or
       *                01b - 64-bit address space
       *       [0]    : 0 - memory map decoded
       *
       * BAR1: if BAR0[2:1] == 00b, optional flash memory base
       *       if BAR0[2:1] == 01b, high portion of memory base 
       *                            for 64-bit address space        
       *
       * BAR2: if BAR0[2:1] == 01b, optional flash memory base
       *       if BAR0[2:1] == 00b, behaves as BAR-1 when BAR-0 is 
       *                            a 32-bit value 
       * Look at power up condition of Bar0 to determine PCI64
       * NOTE: This only works from POWERUP and not RESTART
       */

      pciConfigInLong(pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                            PCI_CFG_BASE_ADDRESS_0, &bar0);
      pReso->adr64 = ((bar0 & BAR0_64_BIT) ? TRUE : FALSE);

        /*
         * For PCI32, bar0 represents the CSR base address and
         *            bar 1 and bar 2 represents Flash address.
         * For PCI64, bar 0 represents low 32bits of CSR address
         *            bar 1 represents high 32 bits of CSR address
         *            bar 2 represents Flash address
         * This gigabit controller does not suppport a Flash ROM,
         * thus, the Flash base will not be configured.
         */

   
      /*
       * get memory base addresses for CSR, No Flash Memory for PMC
       */

	pciConfigInLong(pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                            PCI_CFG_BASE_ADDRESS_0, &memBaseLow);

	if (pReso->adr64)
	{        
		pciConfigInLong(pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                              PCI_CFG_BASE_ADDRESS_1, &memBaseHigh);
		pciConfigInLong(pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                              PCI_CFG_BASE_ADDRESS_2, &flashBase);
	}
	else
	{
		pciConfigInLong(pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                              PCI_CFG_BASE_ADDRESS_1, &flashBase);
		memBaseHigh = 0x0;
	}

	        /* Initialize the cache line size (32/4 = 8) */
        pciConfigOutByte(pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
                              PCI_CFG_CACHE_LINE_SIZE, 8);
	sysPciConfigInsertLong (pReso->pciBus, pReso->pciDevice, pReso->pciFunc,
				   PCI_CFG_COMMAND,
				    0x6, 6);	/*Enable Bus mastering and PCI Mem accesses*/

	printf("\n1)GEI MEM BASE = %x GEI FLASH BASE = %x",memBaseLow,flashBase);
	memBaseLow = memBaseLow & GEI82543_MEMBASE_MASK; 
	flashBase  = flashBase & GEI82543_FLASHBASE_MASK;

	/* over write the resource table with read value */
	pReso->memBaseLow   = memBaseLow;
	pReso->memBaseHigh  = memBaseHigh;
	pReso->flashBase    = flashBase;
	pReso->irq          = (char)TARGET_INTR_A_VEC;

	printf("\n2)GEI MEM BASE LOW = %x MEM BASE HIGH = %x",pReso->memBaseLow,pReso->memBaseHigh);

	/* compose loadString in endDevTbl for this unit */        
	if(sys543LoadStrCompose (unit) == ERROR)
		return(ERROR);
	geiUnits++;

    } /* end of for */

    if (geiUnits)
       return(OK);
    return(ERROR);
}


/****************************************************************************
*
* sys543LoadStrCompose - Compose the END load string for the device
*
* The END device load string formed by this routine is in the following
* following format.
* <shMemBase>:<shMemSize>:<rxDesNum>:<txDesNum>:<usrFlags>:<offset>
*
* RETURN: N/A
*/

LOCAL STATUS sys543LoadStrCompose
    (
    int unit
    )
    {
    END_TBL_ENTRY *     pDevTbl;
    int i;	
    

    if(unit >= GEI_MAX_UNITS)
    {
		return(ERROR);
    }			    
    for (i=0,pDevTbl = endDevTbl; pDevTbl->endLoadFunc != END_TBL_END;
         pDevTbl++,i++);

	 if(i<6) 
         {
		 	    	         
	     pDevTbl->unit = unit;
	     (UINT32)pDevTbl->endLoadFunc = (UINT32)GEI82543_LOAD_FUNC;
	     pDevTbl->endLoadString = geiEndLoadStr[unit];	
	     pDevTbl->endLoan = GEI82543_BUFF_LOAN;
	     pDevTbl->pBSP	= NULL;
	     pDevTbl->processed = FALSE;				
             sprintf (pDevTbl->endLoadString,
                     "0x%x:0x%x:0x%x:0x%x:0x%x:%d",
                      geiResources [unit].shMemBase, /* share memory base */                 
                      geiResources [unit].shMemSize, /* share memory size */
                      geiResources [unit].rxDesNum,  /* RX Descriptor Number*/
                      geiResources [unit].txDesNum,  /* TX Descriptor Number*/
                      geiResources [unit].usrFlags,  /* user's flags */
                      GEI_X86_OFFSET_VALUE           /* offset value */                 
                     );
             return OK;
          }
          
     return OK;
     }

/*****************************************************************************
*
* sys82543BoardInit - Adaptor initialization for 82543 chip
*
* This routine is expected to perform any adapter-specific or target-specific
* initialization that must be done prior to initializing the 82543 chip.
*
* The 82543 driver calls this routine from the driver load routine before
* any other routines.
*
* RETURNS: OK or ERROR
*/

IMPORT STATUS pciIntConnect();
IMPORT STATUS pciIntDisconnect();

STATUS sys82543BoardInit
    (
    int           unit,         /* unit number */
    ADAPTOR_INFO  *pBoard       /* board information for the GEI driver */
    )
    {
    GEI_RESOURCE *pReso = &geiResources [unit];

    /* sanity check */

    if (unit >= geiUnits)
      return (ERROR);

    if (pReso->boardType !=  PRO1000F_BOARD && 
	pReso->boardType !=  PRO1000T_BOARD)
      return ERROR;

    /* perform EEPROM checksum */

    if (sys543eepromCheckSum (unit) != OK)
        {
        printf ("GEI82543:unit=%d, EEPROM checksum Error!\n", unit);
        }

    /* get the Ethernet Address from eeprom */

    if (sys543EtherAdrGet (unit) != OK)
        {
        printf ("GEI82543:unit=%d, Invalid Ethernet Address!\n", unit);
        }       

    /* get the initialization control word 1 (ICW1) in EEPROM */

    geiResources[unit].eeprom_icw1 = sys543eepromReadWord (unit, EEPROM_ICW1);

    /* get the initialization control word 2 (ICW2) in EEPROM */

    geiResources[unit].eeprom_icw2 = sys543eepromReadWord (unit, EEPROM_ICW2);

    /* initializes the board information structure */

    pBoard->boardType     = pReso->boardType;
    pBoard->vector        = pReso->irq;

    pBoard->regBaseLow    = pReso->memBaseLow;
    pBoard->regBaseHigh   = pReso->memBaseHigh;
    pBoard->flashBase     = pReso->flashBase;
    pBoard->adr64         = pReso->adr64;

    pBoard->intEnable     = sys543IntEnable;
    pBoard->intDisable    = sys543IntDisable;
    pBoard->intAck        = sys543IntAck;

    if (pBoard->boardType == PRO1000T_BOARD)
        pBoard->phyType   = GEI_PHY_GMII_TYPE;      

    pBoard->phySpecInit   = sys543PhySpecRegsInit;
    pBoard->delayFunc     = (FUNCPTR)sysDelay;
    pBoard->delayUnit     = 720;  /* sysDelay() takes about 720ns delay */
    pBoard->sysLocalToBus = NULL; 
    pBoard->sysBusToLocal = NULL; 

    /* specify the interrupt connect/disconnect routines to be used */

    pBoard->intConnect    = (FUNCPTR) intConnect;
    pBoard->intDisConnect = (FUNCPTR) NULL;

    /* get the ICW1 and ICW2 */

    pBoard->eeprom_icw1   = geiResources[unit].eeprom_icw1;
    pBoard->eeprom_icw2   = geiResources[unit].eeprom_icw2;

    /* copy Ether address */

    memcpy (&pBoard->enetAddr[0], &geiResources[unit].enetAddr[0], 
            ETHER_ADDRESS_SIZE);

    /* we finished adaptor initialization */

    pReso->iniStatus = OK;

    /* enable adaptor interrupt */

    intEnable(pReso->irq);

    return (OK);
    }


/*************************************************************************
*
* sys543eepromReadBits - read bits from EEPROM
*
* This routine reads bit data from EEPROM
*
* RETURNS: value in WORD size
*/

LOCAL UINT16 sys543eepromReadBits
    (
    int          unit,
    int          bitsNum
    )
    {
    int      count;
    UINT32   ix;
    UINT16   val = 0;

    for (ix = 0; ix < bitsNum; ix++)
        {
        /* raise the clk */ 

        GEI_SYS_WRITE_REG(unit, INTEL_82543GC_EECD, 
                                (EECD_CS_BIT | EECD_SK_BIT));

        /* wait 2000ns */

        for (count = 0; count < 3; count++)
             sysDelay ();

        val = ( val << 1) | 
          ((GEI_SYS_READ_REG(unit, INTEL_82543GC_EECD) & EECD_DO_BIT)? 1 : 0);

        /* lower the clk */

        GEI_SYS_WRITE_REG(unit, INTEL_82543GC_EECD, EECD_CS_BIT);

        /* wait 2000ns */

        for (count = 0; count < 3; count++)
             sysDelay ();   
        }      

    return (val);
    }

/*************************************************************************
*
* sys543eepromWriteBits - write bits out to EEPROM
*
* This routine writes bits out to EEPROM
*
* RETURNS: N/A
*/

LOCAL void sys543eepromWriteBits
    (
    int          unit,
    UINT16       value,
    UINT16       bitNum
    )
    {
    int             count;
    volatile UINT16 data;

    if (bitNum == 0)
           return;

    while (bitNum--)
    {
    data = (value & (0x1 << bitNum )) ? EECD_DI_BIT : 0;

    data |=  EECD_CS_BIT;

    /* write the data */

    GEI_SYS_WRITE_REG(unit, INTEL_82543GC_EECD, data);

    /* wait 1000ns */

    for (count = 0; count < 2; count++)
         sysDelay ();    

    /* raise the clk */ 

    GEI_SYS_WRITE_REG(unit, INTEL_82543GC_EECD, (data | EECD_SK_BIT));

    /* wait 1000ns */

    for (count = 0; count < 2; count++)
         sysDelay ();

    /* lower the clk */

    GEI_SYS_WRITE_REG(unit, INTEL_82543GC_EECD, data);

    /* wait 1000ns */

    for (count = 0; count < 2; count++)
         sysDelay ();   
    }

    return;
    }

/*************************************************************************
*
* sys543eepromReadWord - Read a word from EEPROM
*
* RETURNS: value in WORD size
*/

LOCAL UINT16 sys543eepromReadWord
    (
    int          unit,
    UINT32       index
    )
    {
    int    count;
    UINT16 val;
    UINT32 tmp;

    if (index >= EEPROM_WORD_SIZE)
        {
        printf ("Invalid index:%d to EEPROM\n", index);
        return 0;
        }

    tmp = GEI_SYS_READ_REG(unit, INTEL_82543GC_EECD);

    GEI_SYS_WRITE_REG(unit, INTEL_82543GC_EECD, EECD_CS_BIT);

    /* wait 1000ns */

    for (count = 0; count < 2; count++)
         sysDelay ();

    /* write the opcode out */

    sys543eepromWriteBits (unit, EEPROM_READ_OPCODE, EEPROM_CMD_BITS);

    /* write the index out */

    sys543eepromWriteBits (unit, index, EEPROM_INDEX_BITS);

    GEI_SYS_READ_REG(unit, INTEL_82543GC_EECD);
    
    /* read the data */

    val = sys543eepromReadBits (unit, EEPROM_DATA_BITS);

    /* clean up access to EEPROM */      

    tmp &= ~(EECD_DI_BIT | EECD_DO_BIT | EECD_CS_BIT);

    GEI_SYS_WRITE_REG(unit, INTEL_82543GC_EECD, tmp);

    return val;
    }

/*************************************************************************
*
* sys543EtherAdrGet - Get Ethernet address from EEPROM
*
* This routine get an Ethernet address from EEPROM
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS sys543EtherAdrGet
    (
    int unit
    )
    {
    UINT32 ix, count = 0 ;
    UINT16 val;
    UCHAR  adr [ETHER_ADDRESS_SIZE];

    for (ix = 0; ix < ETHER_ADDRESS_SIZE / sizeof(UINT16); ix++) 
        {
        /* get word i from EEPROM */

        val = sys543eepromReadWord (unit, (UINT16)(EEPROM_IA_ADDRESS + ix));

        adr [count++] = (UCHAR)val;

        adr [count++] = (UCHAR) (val >> 8);
        }

    /* check IA is UCAST  */

    if (adr[0] & 0x1)
        return (ERROR);

    memcpy (&geiResources[unit].enetAddr[0], adr, ETHER_ADDRESS_SIZE);

    return (OK);
    }

/**************************************************************************
* 
* sys543eepromCheckSum - calculate checksum 
*
* This routine perform EEPROM checksum
*
* RETURNS: N/A
*/

LOCAL STATUS sys543eepromCheckSum
    (
    int unit
    )
    {
    UINT16 checkSum = 0 ;
    UINT32 ix;

    for (ix = 0; ix < EEPROM_WORD_SIZE; ix++) 
        checkSum += sys543eepromReadWord (unit, ix);
 
    if (checkSum == (UINT16)EEPROM_SUM)
        return OK;
 
    return ERROR;
    }

/**************************************************************************
*
* sys543PhySpecRegsInit - Initialize PHY specific registers
*
* This routine initialize PHY specific registers
*
* RETURN: N/A
*/

LOCAL void sys543PhySpecRegsInit
    (
    PHY_INFO *  pPhyInfo,    /* PHY's info structure pointer */
    UINT8     phyAddr       /* PHY's bus number */
    )
    {
    UINT16 regVal;          /* register value */
    UINT16 phyId1;          /* phy Id 1 */
    UINT16 phyId2;          /* phy ID 2 */
    UINT32 retVal;          /* return value */
    UINT32 phyOui = 0;      /* PHY's manufacture ID */
    UINT32 phyMode;         /* PHY mode number */

    /* Intel Pro1000T adaptor uses Alaska transceiver */

    /* read device ID to check Alaska chip available */

    MII_READ (phyAddr, MII_PHY_ID1_REG, &phyId1, retVal);
    
    MII_READ (phyAddr, MII_PHY_ID2_REG, &phyId2, retVal);
       
    phyOui =  phyId1 << 6 | phyId2 >> 10;

    phyMode = (phyId2 & MII_ID2_MODE_MASK) >> 4;

    if (phyOui == MARVELL_OUI_ID && (phyMode == MARVELL_ALASKA_88E1000 || 
                                     phyMode == MARVELL_ALASKA_88E1000S))
        {
         /* This is actually a Marvell Alaska 1000T transceiver */

         /* disable PHY's interrupt */         

         MII_READ (phyAddr, ALASKA_INT_ENABLE_REG, &regVal, retVal);
         regVal = 0;
         MII_WRITE (phyAddr, ALASKA_INT_ENABLE_REG, regVal, retVal);

         /* CRS assert on transmit */

         MII_READ (phyAddr, ALASKA_PHY_SPEC_CTRL_REG, &regVal, retVal);
         regVal |= ALASKA_PSCR_ASSERT_CRS_ON_TX;
         MII_WRITE (phyAddr, ALASKA_PHY_SPEC_CTRL_REG, regVal, retVal);

        /* set the clock rate when operate in 1000T mode */

         MII_READ (phyAddr, ALASKA_EXT_PHY_SPEC_CTRL_REG, &regVal, retVal);
         regVal |= ALASKA_EPSCR_TX_CLK_25;
         MII_WRITE (phyAddr, ALASKA_EXT_PHY_SPEC_CTRL_REG, regVal, retVal);
        }

     /* other PHYS .... */

     return;
     }

/*******************************************************************************
*
* sys543IntAck - acknowledge an 82543 interrupt
*
* This routine performs any 82543 interrupt acknowledge that may be
* required.  This typically involves an operation to some interrupt
* control hardware.
*
* This routine gets called from the 82543 driver's interrupt handler.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if the interrupt could not be acknowledged.
*/

LOCAL STATUS sys543IntAck
    (
    int unit   /* unit number */
    )
{
  GEI_RESOURCE *pReso = &geiResources [unit];

  switch (pReso->boardType) {
/* 280     case PRO1000F_BOARD:        * handle PRO1000F/T LAN Adapter */
    case PRO1000T_BOARD:
      /* no additional work necessary for the PRO1000F/T */
      break;

    default:
      return (ERROR);
  }

  return (OK);
}

/*******************************************************************************
*
* sys543IntEnable - enable 82543 interrupts
*
* This routine enables 82543 interrupts.  This may involve operations on
* interrupt control hardware.
*
* The 82543 driver calls this routine throughout normal operation to terminate
* critical sections of code.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if interrupts could not be enabled.
*/

LOCAL STATUS sys543IntEnable
    (
    int vector    /* vector number */
    )
{
  /*
   * sys82543BoardInit enables the adaptor interrupt. No need to do it here.
   * The irq is also a shared resource with other adaptors.
   *      intEnable(IVEC_TO_INUM(vector));
   */
  return (OK);
}

/*******************************************************************************
*
* sys543IntDisable - disable 82543 interrupts
*
* This routine disables 82543 interrupts.  This may involve operations on
* interrupt control hardware.
*
* The 82543 driver calls this routine throughout normal operation to enter
* critical sections of code.
*
* This routine assumes that the PCI configuration information has already
* been setup.
*
* RETURNS: OK, or ERROR if interrupts could not be disabled.
*/

LOCAL STATUS sys543IntDisable
    (
    int vector    /* vector number */
    )
{
  /*
   * sys82543BoardInit enables the adaptor interrupt.
   * The irq is a shared resource with other adaptors. It can not be disabled
   * without platform support code that monitors the users of the resource.
   *      intDisable(IVEC_TO_INUM(vector));
   */
  return (OK);
}

/*******************************************************************************
*
* sys543Show - shows 82543 configuration 
*
* this routine shows (PMC/Gigabit Ethernet/82543GC) configuration 
*
* RETURNS: N/A
*/

void sys543Show
    (
    int unit    /* unit number */
    )
{
  GEI_RESOURCE *pReso = &geiResources [unit];
  int          ix;

  if (unit >= geiUnits) {
    printf ("invalid unit number: %d\n", unit);
    return;
  }

  if (pReso->boardType == PRO1000F_BOARD)
    printf ("********* Intel PRO1000 F Adapter ***********\n");

  else if (pReso->boardType == PRO1000T_BOARD)
    printf ("********* Intel PRO1000 T Adapter ***********\n");
      
  else
    printf ("********* UNKNOWN Adaptor ************ \n");
 
  printf ("  CSR PCI Membase address  Low = 0x%x\n", pReso->memBaseLow);
  printf ("  CSR PCI Membase address High = 0x%x\n", pReso->memBaseHigh);
  printf ("  Flash PCI Membase address = 0x%x\n",    pReso->flashBase);

  printf ("  PCI bus no.= 0x%x, device no.= 0x%x, function no.= 0x%x IRQ = 0x%x\n", 
	  pReso->pciBus, pReso->pciDevice, pReso->pciFunc, pReso->irq);

  if (pReso->iniStatus == ERROR)
    return;

  printf ("  Adapter Ethernet Address");

  for (ix = 0; ix < 6; ix ++)
    printf (":%2.2X", (UINT32)pReso->enetAddr[ix]);

  printf ("\n");

  printf ("  EEPROM Initialization Control Word 1 = 0x%4.4X\n", 
	  pReso->eeprom_icw1);

  printf ("  EEPROM Initialization Control Word 2 = 0x%4.4X\n", 
	  pReso->eeprom_icw2);

  printf ("*********************************************\n");

}

#endif /* (INCLUDE_NETWORK & INCLUDE_GEI_82543_END & INCLUDE_END) */


