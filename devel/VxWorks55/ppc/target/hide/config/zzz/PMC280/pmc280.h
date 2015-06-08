/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************


*******************************************************************************
* pmc280.h - Header File for : Development Board BSP definition module.
*
* DESCRIPTION:
*       This file contains I/O addresses and related constants. 
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCk2h
#define __INCk2h

/* includes */
#include "vxWorks.h"

/* defines  */
#define PMC280_MODEL       "PMC280-"

#define INCLUDE_PCI

/* Development board model */
#define DB_ID_BP    0x2
#define DB_ID_PCI   0xFFFF

/* Platform clock settings */
#define SYSCLK_RATE     133333333      /* SysCLK rate on board */
#define SYSCLOCK        133            /* SysCLK rate on board IN Mhz  */
#define TCLK_RATE       SYSCLK_RATE    /* TCLK   rate on board */
#define DEC_CLOCK_FREQ 	SYSCLK_RATE
#define BCLKIN_RATE 	12902400    /* Bclkin rate (OSC3) */

/* BSP defines */
#define N_SIO_CHANNELS  2      		/* Number of serial I/O channels */

/* UnSupported features */
#undef  INCLUDE_SNOOP_SUPPORT 		/* No Snoop support */

/* PPC Decrementer - used as vxWorks system clock */
#define DELTA(a,b)         (abs((int)a - (int)b))

/* BSP timer constants */
#define SYS_CLK_RATE_MIN        3      /* minimum system clock rate */
#define SYS_CLK_RATE_MAX        5000   /* maximum system clock rate */

/* BSP min aux clock rate */
#define AUX_CLK_RATE_MIN        3      /* minimum system clock rate */
#define AUX_CLK_RATE_MAX        8000   /* maximum auxiliary clock rate */


/* create a single macro INCLUDE_MMU */

#if defined(INCLUDE_MMU_BASIC) || defined(INCLUDE_MMU_FULL)
#   define INCLUDE_MMU
#endif

/* Only one can be selected, FULL overrides BASIC */

#ifdef INCLUDE_MMU_FULL
#   undef INCLUDE_MMU_BASIC
#endif

#define WRONG_CPU_MSG "\nThis VxWorks image was not compiled for this CPU!\n"

/* PPC CPU types */
#define CPU_TYPE            ((vxPvrGet() >> 16) & 0xFFFF)
#define CPU_TYPE_601        0x0001  /* PPC  601   CPU */
#define CPU_TYPE_602        0x0002  /* PPC  602   CPU */
#define CPU_TYPE_603        0x0003  /* PPC  603   CPU */
#define CPU_TYPE_603E       0x0006  /* PPC  603e  CPU */
#define CPU_TYPE_603P       0x0007  /* PPC  603p  CPU */
#define CPU_TYPE_604        0x0004  /* PPC  604   CPU */
#define CPU_TYPE_604E       0x0009  /* PPC  604e  CPU */
#define CPU_TYPE_604R       0x000A  /* PPC  604r  CPU */
#define CPU_TYPE_750        0x0008  /* PPC  750  CPU (Arthur) */
#define CPU_TYPE_7400       0x000C  /* PPC 7400   CPU (Max)    */
#define CPU_TYPE_7410       0x800C  /* PPC 7410   CPU (Nitro)  */
#define CPU_TYPE_7450       0x8000  /* PPC 7450   CPU (V'ger)  */
#define CPU_TYPE_7455       0x8001  /* PPC 7455  CPU (Apollo) */
#define CPU_TYPE_7447       0x8002  /* PPC 7447+7457  CPU */
#define CPU_TYPE_7447A      0x8003  /* PPC 7447A CPU */

/* Not supported */
#define CPU_TYPE_750FX      0x7000  /* PPC  750FX CPU */


/* PPC750 CPU versions */
#define CPU_VERSION	        (vxPvrGet() & 0xF0F0)
#define CPU_MODEL_750       0x0000  /* Motorola PPC CPU */
#define CPU_MODEL_750L      0x8000  /* IBM PPC 750L CPU */
#define CPU_MODEL_750CX_D2  0x2010  /* IBM PPC 750CXe DD2.4 CPU */
#define CPU_MODEL_750CX     0x3010  /* IBM PPC 750CXe DD3.1 CPU */

/* Not supported */
#define CPU_MODEL_755 		0x3000  /* Motorola PPC 755 CPU */


/* PPC/MPC CPU sub-Version */
#define CPU_SUB_VER1	   ((vxPvrGet() & 0xF00) >> 8)
#define CPU_SUB_VER2        (vxPvrGet() & 0xF)
                                 
/*
 * sysPhysMemDesc[] dummy entries:
 * these create space for updating sysPhysMemDesc table at a later stage
 * mainly to provide plug and play
 */

#define DUMMY_PHYS_ADDR         -1
#define DUMMY_VIRT_ADDR         -1
#define DUMMY_LENGTH            -1
#define DUMMY_INIT_STATE_MASK   -1
#define DUMMY_INIT_STATE        -1

#define DUMMY_MMU_ENTRY {                         \
                         (void *) DUMMY_PHYS_ADDR,\
                         (void *) DUMMY_VIRT_ADDR,\
                         DUMMY_LENGTH,            \
                         DUMMY_INIT_STATE_MASK,   \
                         DUMMY_INIT_STATE         \
                        }

/* GPP Interrupt causes attached to the GPP port */

#undef  RS232_INT_A
#undef  RS232_INT_B
#define WD_NMI_INT      18 /* GPP pin 18 connected to WD MNI          */ 
#undef  RTC_INT


/* PCI. Same interrupt pin describe PCI Int A ,B, C, D */ 
#define	PCI_IV_UNMAPPED -1
#define	PCI0_INTR_A_VEC 27 /* GPP pin 27 connected to PCI0 int A */
#define	PCI0_INTR_B_VEC 29 /* GPP pin 29 connected to PCI0 int B */
#define	PCI0_INTR_C_VEC 16 /* GPP pin 16 connected to PCI0 int C */
#define	PCI0_INTR_D_VEC 17 /* GPP pin 17 connected to PCI0 int D */

/* Board settings */

/* 
 * These are not used anywhere!
 */
#undef SDRAM_BANK0_ADRS
#undef SDRAM_BANK1_ADRS
#undef SDRAM_BANK2_ADRS
#undef SDRAM_BANK3_ADRS

#define INTERNAL_REG_ADRS   0xF1000000

#define INTERNAL_SRAM_ADRS  0xF2000000

/* Chip Select Base Address */
/* Boot device base address depends on the default main flash size to enable  */
/* easy boot_CS and CS[3] swapping. This allows the boot to run from either   */
/* boot flash or main flash without changing the code.			  */
#define CS0_BASE_ADRS       0xA0000000
#define CS1_BASE_ADRS       0xA2000000
#define CS2_BASE_ADRS       0xF2040000
#undef  CS3_BASE_ADRS
#define BOOTCS_BASE_ADRS    0xFF800000

#define GEI_MAX_UNITS          1                /* Maximum device units */
#define FEI_MAX_UNITS	2		/* max units supported */

#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_A  0xB2831146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_B  0xB2811146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_C  0xB2801146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_D  0xB2821146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_E  0xB2841146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_F  0xB2851146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_G  0xB2861146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_H  0xB2871146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_I  0xB2881146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_J  0xB2891146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_K  0xB28A1146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_L  0xB28B1146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_M  0xB28C1146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_N  0xB28D1146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_O  0xB28E1146
#define PMC280_SUBSYSTEM_DEV_AND_VENDOR_ID_P  0xB28F1146

#define PCI0_IO_BASE_ADRS               0x88000000
#define PCI0_MEM0_BASE_ADRS             0x80000000
#define PCI0_MEM1_BASE_ADRS             0x82000000
#define PCI0_MEM2_BASE_ADRS             0x84000000
#define PCI0_MEM3_BASE_ADRS             0x86000000
#define CPU_PCI_MEM_ADRS                PCI0_MEM0_BASE_ADRS
#define CPU_PCI_MEM_SIZE                (PCI0_MEM0_SIZE + PCI0_MEM1_SIZE + PCI0_MEM2_SIZE + PCI0_MEM3_SIZE)
#define CPU_PCI_IO_ADRS                 PCI0_IO_BASE_ADRS
#define CPU_PCI_IO_SIZE                 PCI0_IO_SIZE


/* Memory segments size */
/* Boot device and Flash have the same size to allow easy boot_CS and CS[3]   */
/* swapping. This allows the boot to run from either boot flash or main flash */
/* without changing the code. Cavity: Boot flash has a virtual size of 16M.   */
#define INTERNAL_REG_SIZE   	_64K

/* Chip Select Address Space */
#ifdef ROHS
#define CS0_ADRS_SPACE_SIZE     _64M /*_32M*/	
#else
#define CS0_ADRS_SPACE_SIZE    _32M
#define CS1_ADRS_SPACE_SIZE      _32M
#endif 
#undef CS2_ADRS_SPACE_SIZE      
#undef CS3_ADRS_SPACE_SIZE    
#define BOOTCS_ADRS_SPACE_SIZE   _8M

#define PCI0_IO_SIZE            _16M
#define PCI0_MEM0_SIZE          _32M
#define PCI0_MEM1_SIZE          _32M
#define PCI0_MEM2_SIZE          _32M
#define PCI0_MEM3_SIZE          _32M

/* Device address mappings */
#undef SRAM_BASE_ADRS
#undef RTC_BASE_ADRS
#undef SERIAL_DEVICE_ADRS
#define	FLASH_BASE_ADRS		  CS0_BASE_ADRS

/* MPP pin Configuration */
/*  MPP[0] = TxD0                MPP[1] = RxD0             */
/*  MPP[2] = GPP[2]              MPP[3] = TxD1             */
/*  MPP[4] = GPP[4]              MPP[5] = RxD1             */
/*  MPP[6] = GPP[6]              MPP[7] = GPP[7]           */
#define MPP_CONTROL0_VALUE 0x00303022

/*  MPP[8]  = GPP[8]           MPP[9]  = GPP[9]  */
/*  MPP[10] = not usedGPP[10]  MPP[11] = GPP[11] */
/*  MPP[12] = GPP[12]          MPP[13] = GPP[13] */
/*  MPP[14] = not usedGPP[14]  MPP[15] = not usedGPP[15] */
#define MPP_CONTROL1_VALUE 0x00000000
    
/*  MPP[16] = GPP[16]          MPP[17] = GPP[17] */
/*  MPP[18] = GPP[18]          MPP[19] = WDExpir */
/*  MPP[20] = BClkIn           MPP[21] = GPP[21] */
/*  MPP[22] = GPP[22]          MPP[23] = GPP[23] */
#define MPP_CONTROL2_VALUE 0x00044000


/*  MPP[24] = WDNMI              MPP[25] = GPP[25]           */
/*  MPP[26] = GPP[26]            MPP[27] = GPP[27]           */
/*  MPP[28] = InitAct            MPP[29] = GPP[29]           */
/*  MPP[30] = PME                MPP[31] = GPP[31]           */
#define MPP_CONTROL3_VALUE 0x00030004

#ifdef INCLUDE_PCI  
#define GPP_LEVEL_CONTROL_VALUE 0x28030000    /* Set all GPPs active High, 
                                                 except 27,29,16 & 17(PCI). */
#else
#define GPP_LEVEL_CONTROL_VALUE	0x00000000 /* Set GPP's to be active High. */
#endif 

#ifdef PMC280_PCI_BOOT
#define GPP_IO_CONTROL_VALUE    0x00000200 /* Set GPP's to be Input       */ 
#else
#define GPP_IO_CONTROL_VALUE    0x00000000 /* Set GPP's to be Input       */ 
#endif


/* Device bank parameters */ 
#define CS0_PARAMETERS          0x8fefffff  /* 32 bit */
#define CS1_PARAMETERS          0x8fefffff  /* 32 bit */
#undef  CS2_PARAMETERS
#undef  CS3_PARAMETERS
#define BOOTCS_PARAMETERS       0x8fcfffff  /*  8 bit */

/* Ethernet Ports Board depended information. */
#define PORT_0_PHY_ADDR         1
#define PORT_1_PHY_ADDR		3

#define MARVELL_PHY_ID  0x0141  /* PHY device & vendor ID */

/* Serial */
#define RS232_CHAN_B      SERIAL_DEVICE_ADRS        
#define RS232_CHAN_A      (SERIAL_DEVICE_ADRS + 0x20)
#define RS232_CLOCK       3686400         /* UART clock */

#define RTC_I2C_ADDR         0xA0

#endif	/* __INCk2h */
