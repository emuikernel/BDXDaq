/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* config.h - Header File for : BSP configuration
*
* DESCRIPTION:
*       This file contains the configuration parameters for the BSP.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef	__INCconfigh
#define	__INCconfigh

/* BSP version/revision identification, before configAll.h */

#define BSP_VER_1_1	1	/* 1.2 is backward compatible with 1.1 */
#define BSP_VER_1_2	1

#define BSP_VERSION	"2.2"   /* 1.0.1 for Tornado 1.0.1  */
#define BSP_REV		"/5.0"

#include "configAll.h"		/* Set the VxWorks default configuration */

#define DEFAULT_BOOT_LINE       \
       "mgi(0,0) host:/ftproot/vxWorks " \
       "h=10.232.104.228 " \
       "e=10.247.64.117:FFFFFF00 " \
       "u=akb " \
       "pw=akb "


/* memory configuration */

#define LOCAL_MEM_LOCAL_ADRS 0x00000000  /* Start of SDRAM is zero */

#undef CONSOLE_TTY
#ifdef PMC280_DUAL_CPU
#define CONSOLE_TTY             (frcWhoAmI())   /* console channel */
#else
#define CONSOLE_TTY             0  /* console channel */
#endif /* PMC280_DUAL_CPU */

#define LOCAL_MEM_AUTOSIZE               /* run-time memory sizing */

#ifdef PMC280_DUAL_CPU
/*
 * The USER_RESERVED_MEM is being partitioned into three parts:
 *    . Partition 1 = MPSC0 driver data + Ethernet driver data for CPU0.
 *    . Partition 2 = MPSC1 driver data for CPU1.
 *    . Partition 3 = Shared memory region for applications.
 *
 * The sizes of the three partitions are as follows:
 *    . Partition 1 = (SYS_SHMEM_SIZE/2)
 *    . Partition 2 = (SYS_SHMEM_SIZE/2)
 *    . Partition 3 = APP_SHMEM_SIZE
 *
 * Refer to userRsrvMem.c for further details of its usage
 */

/* 
 * BOARD_MEM_SIZE is the total memory on board.
 *
 * APP_SHMEM_SIZE is the total memory for shared memory applications.
 *
 * LOCAL_MEM_SIZE is the total amount of memory that is available to each
 * CPU. In other words, the virtual map of each CPU would start at 0x0 and
 * go upto LOCAL_MEM_SIZE.
 */
#define BOARD_MEM_SIZE       0x20000000  /* Total memory on board       */
#define APP_SHMEM_SIZE       0x02000000  /* Shared memory size for apps  */

/*
 * SYS_SHMEM_SIZE is the memory region that is used by the MPSC
 * and MV65360 Ethernet drivers as well as the PTEs. Since the size
 * reserved for the Page Table is 8MB, we can map memory upto 1GB. For
 * all applicatioNs, this seems to be a reasonable number. There is no reason 
 * to change these sizes. 
 */
#define SYS_PGT_SIZE         0x02000000  /* Size reserved for PTEs */
#define SYS_DRV_SIZE         0x01000000  /* Size reserved for drivers */

#define SYS_SHMEM_SIZE       (SYS_DRV_SIZE + SYS_PGT_SIZE) /* System Shmem */

#define USER_RESERVED_MEM    (SYS_SHMEM_SIZE + APP_SHMEM_SIZE)
#ifdef LOCAL_MEM_AUTOSIZE
#define LOCAL_MEM_SIZE       ((sdramSize/2) + (USER_RESERVED_MEM/2))
#else
#define LOCAL_MEM_SIZE       ((BOARD_MEM_SIZE/2) + (USER_RESERVED_MEM/2))
#endif /* LOCAL_MEM_AUTOSIZE */

/* This is where the physical address of CPU0 starts */
#define PHY_CPU0_MEM_BOT     (LOCAL_MEM_LOCAL_ADRS)
#define VIR_CPU0_MEM_BOT     (LOCAL_MEM_LOCAL_ADRS)

/* This is where the physical address of CPU1 starts */
#define PHY_CPU1_MEM_BOT     (PHY_CPU0_MEM_BOT + LOCAL_MEM_SIZE)
#define VIR_CPU1_MEM_BOT     (LOCAL_MEM_LOCAL_ADRS)

/* System shared memory for CPU0 */
#define PHY_CPU0_SYS_SHMEM_BOT  (LOCAL_MEM_SIZE - USER_RESERVED_MEM)
#define VIR_CPU0_SYS_SHMEM_BOT  (LOCAL_MEM_SIZE - USER_RESERVED_MEM)

/* 
 * The USER_RESERVED_MEM starts from here.
 */
#define PHY_CPU0_USER_RESERVED_MEM_BOT    PHY_CPU0_SYS_SHMEM_BOT
#define VIR_CPU0_USER_RESERVED_MEM_BOT    VIR_CPU0_SYS_SHMEM_BOT

/* System shared memory CPU1 */
#define PHY_CPU1_SYS_SHMEM_BOT  (PHY_CPU0_SYS_SHMEM_BOT)
#define VIR_CPU1_SYS_SHMEM_BOT  (VIR_CPU0_SYS_SHMEM_BOT)

/* 
 * The USER_RESERVED_MEM starts from here.
 */
#define PHY_CPU1_USER_RESERVED_MEM_BOT    PHY_CPU1_SYS_SHMEM_BOT
#define VIR_CPU1_USER_RESERVED_MEM_BOT    VIR_CPU1_SYS_SHMEM_BOT

/* Application shared memory for CPU0 */
#define PHY_CPU0_APP_SHMEM_BOT  (PHY_CPU0_SYS_SHMEM_BOT + SYS_SHMEM_SIZE)
#define VIR_CPU0_APP_SHMEM_BOT  (VIR_CPU0_SYS_SHMEM_BOT + SYS_SHMEM_SIZE)

/* Application shared memory for CPU1 */
#define PHY_CPU1_APP_SHMEM_BOT  (PHY_CPU0_APP_SHMEM_BOT)
#define VIR_CPU1_APP_SHMEM_BOT  (VIR_CPU0_APP_SHMEM_BOT)

/*
 * We will reserve I/DBAT for specific purposes:
 *
 *  . IBAT[0] is for mapping VxWorks .text section.
 *  . IBAT[1] is for mapping VxWorks .text section.
 *  . IBAT[2] is FREE.
 *  . IBAT[3] is FREE.
 *  . IBAT[4] is FREE.
 *  . IBAT[5] is FREE.
 *  . IBAT[6] is FREE.
 *  . IBAT[7] is FREE.
 *
 *  . DBAT[0] is for mapping VxWorks .data + .bss section.
 *  . DBAT[1] is for mapping VxWorks .data + .bss section.
 *  . DBAT[2] is for mapping VxWorks .data + .bss section.
 *  . DBAT[3] is for mapping Boot ROM .data area.
 *  . DBAT[4] is for internal register space of MV64360
 *  . DBAT[5] is for system shared area.
 *  . DBAT[6] is for application shared area.
 *  . DBAT[7] is FREE.
 *
 * It is necessary to have the BAT window sizes programmed properly given
 * the data in config.h
 */

/* IBAT sizes */
#define _MMU_IBAT0_BL          _MMU_UBAT_BL_32M
#define _MMU_IBAT0_BL_SIZ      0x02000000

#define _MMU_IBAT1_BL          _MMU_UBAT_BL_8M 
#define _MMU_IBAT1_BL_SIZ      0x00800000

#define _MMU_IBAT2_BL          0x0

/* DBAT sizes */
#define _MMU_DBAT0_BL          _MMU_IBAT0_BL
#define _MMU_DBAT1_BL          _MMU_IBAT1_BL
#define _MMU_DBAT2_BL          _MMU_IBAT2_BL
#define _MMU_DBAT4_BL          _MMU_UBAT_BL_32M
#define _MMU_DBAT5_BL          _MMU_UBAT_BL_16M

#else
#ifdef LOCAL_MEM_AUTOSIZE
#define LOCAL_MEM_SIZE       sdramSize  /* Total of Target memory */
#else
#define LOCAL_MEM_SIZE       0x20000000  /* Total of Target memory */
#endif /* LOCAL_MEM_AUTOSIZE */
/*#define USER_RESERVED_MEM    0x01000 */ /* see sysMemTop() 16M    */

#define USER_RESERVED_MEM    0x4000
#endif




#ifdef PMC280_DUAL_CPU
#define HTABORG_0              (VIR_CPU0_APP_SHMEM_BOT + 0x00)
#define HTABORG_1              (VIR_CPU0_APP_SHMEM_BOT + 0x0C)
#define HTABMASK_0             (VIR_CPU0_APP_SHMEM_BOT + 0x04)
#define HTABMASK_1             (VIR_CPU0_APP_SHMEM_BOT + 0x10)
#define PTETABLESIZE_0         (VIR_CPU0_APP_SHMEM_BOT + 0x08)
#define PTETABLESIZE_1         (VIR_CPU0_APP_SHMEM_BOT + 0x14)
#define READ_DWORD(address)    (*address)
#define WRITE_DWORD(address, data)    (*address = data)
#endif /* PMC280_DUAL_CPU */

   
/*
* The constants ROM_TEXT_ADRS, ROM_SIZE, RAM_HIGH_ADRS, and RAM_LOW_ADRS
* are defined in config.h and Makefile.
* All definitions for these constants must be equal in value.
* Note that the Makefile values do not use the leading 0x prefix.
*/

#define ROM_BASE_ADRS   0xfff00000              /* base address of ROM     */
#define ROM_TEXT_ADRS   (ROM_BASE_ADRS + 0x100) /* with PC & SP            */
#define ROM_WARM_ADRS   (ROM_TEXT_ADRS + 0x008) /* warm entry              */
#define ROM_SIZE        0x00100000              /* 1M ROM space            */
/* #define ROM_SIZE        0x00080000           1M ROM space            */
/*#define RAM_LOW_ADRS    0x00010000            RAM address for vxWorks */
/*#define RAM_HIGH_ADRS   0x00400000             RAM address for bootrom */
/*for power boot*/
#define RAM_LOW_ADRS    0x00800000              /* RAM address for vxWorks */
#define RAM_HIGH_ADRS   0x01800000              /* RAM address for bootrom */

/*#define RAM_LOW_ADRS    0x00100000     */    /* RAM address for vxWorks */
/*#define RAM_HIGH_ADRS   0x01800000 */            /* RAM address for bootrom */

#ifdef PMC280_DUAL_CPU
/* These macros are used for net load of Dual CPU vxWorks */
#define PMC280_SIGNATURE    (PHY_CPU0_SYS_SHMEM_BOT + (USER_RESERVED_MEM/2) + 0x800020)
#define PMC280_VX_ENTRY     (PHY_CPU0_SYS_SHMEM_BOT + (USER_RESERVED_MEM/2) + 0x800024)
#define PMC280_VX_SIZE      (PHY_CPU0_SYS_SHMEM_BOT + (USER_RESERVED_MEM/2) + 0x800028)
#define PMC280_SYNC_SIGN    (PHY_CPU0_SYS_SHMEM_BOT + (USER_RESERVED_MEM/2) + 0x80002c)
#define PMC280_SYNC_SIGN1   (PHY_CPU0_SYS_SHMEM_BOT + (USER_RESERVED_MEM/2) + 0x800030)
#define PMC280_VX_SM_START  (PHY_CPU0_SYS_SHMEM_BOT + (USER_RESERVED_MEM/2) + 0x800040)
#endif /* PMC280_DUAL_CPU */



#define INCLUDE_ALTIVEC               /* Applicable only on 74xx family */

#ifdef INCLUDE_ALTIVEC
    #define INCLUDE_TASK_HOOKS         /* taskHookLib support */
    /* make shure that altivec CDF file - 00altivec.cdf exist */
#endif /* INCLUDE_ALTIVEC */


/* INCLUDES required for running VTS */


/* Serial port configuration */
#define	INCLUDE_SERIAL
#undef	NUM_TTY
#define	NUM_TTY			N_SIO_CHANNELS

/* Console baud rate reconfoguration. */
#undef  CONSOLE_BAUD_RATE
#define CONSOLE_BAUD_RATE 115200	/* Reconfigure default baud rate */

/*
 * Cache configuration
 *
 * Note that when MMU is enabled, cache modes are controlled by
 * the MMU table entries in sysPhysMemDesc[], not the cache mode
 * macros defined here.
 */
#define  INCLUDE_CACHE_SUPPORT	

#define USER_I_CACHE_ENABLE	               /* Enable INSTRUCTION CACHE */

#undef	USER_I_CACHE_MODE
#define USER_I_CACHE_MODE   CACHE_COPYBACK /*select COPYBACK or WRITETHROUGH */

#define USER_D_CACHE_ENABLE	               /* Enable DATA CACHE */

#undef	USER_D_CACHE_MODE
#define USER_D_CACHE_MODE   (CACHE_COPYBACK) /* select COPYBACK or DISABLED     */
#undef	USER_B_CACHE_ENABLE                /* undefined Brunch cache          */


/*
 * L2 Cache
 *
 * Note that L2 cache is not supported by all PPC CPUs. L2 cache is supported
 * on 750, 755, 7400, 7410, but not on sp8240.
 */
#define	INCLUDE_L2_CACHE                /* include L2 cache support     */

#ifdef INCLUDE_L2_CACHE

#define L2CACHE_MODE_WRITETHROUGH       0
#define L2CACHE_MODE_COPYBACK           1

/* Set the default L2 cache mode */
#define L2CACHE_MODE        L2CACHE_MODE_COPYBACK

#endif /* INCLUDE_L2_CACHE */

/* L2 Private Memory NOT Supported (only 755 and 7410 support this feature).*/
#undef	INCLUDE_L2PM    

/* L3 Cache
 *    NOTE: L3 cache is present only on the MPC7450 or MPC7455. 
 */
#undef INCLUDE_L3_CACHE             /* Only MPC7450/7455 has L3 cache ! */

/* L3 Private Memory NOT Supported (only 7450/7455 support this feature). */
#undef	INCLUDE_L3PM    

/* MMU configuration */
#define	INCLUDE_MMU_BASIC   /* MMU support */
#undef	INCLUDE_MMU_FULL	/* MMU support */

/* Network driver configuration */
#define	INCLUDE_NETWORK
#define	INCLUDE_NET_INIT
#undef	INCLUDE_EX
#undef	INCLUDE_ENP
#undef	INCLUDE_SM_NET

/* Enhanced Network Driver (END) support */

#define	INCLUDE_END		/* Enhanced Network Driver (see configNet.h) */
#undef  INCLUDE_BSD		/* BSD 4.4 drivers */
#define  INCLUDE_END_MGI	
#define	INCLUDE_END_FEI
#define	INCLUDE_GEI_82543_END

#if defined(INCLUDE_END) && !defined(INCLUDE_SM_NET)
   	#undef  WDB_COMM_TYPE   /* WDB agent communication path is END device */ 
   	#define WDB_COMM_TYPE        WDB_COMM_END    
#endif  /* defined(INCLUDE_END) && !defined(INCLUDE_SM_NET) */

/* Optional drivers support */
#define INCLUDE_PCI
/* timestamp support for WindView - undefined by default */
#undef INCLUDE_TIMESTAMP
#undef INCLUDE_USER_TIMESTAMP 

/* user definition timestamp routine */
#ifdef  INCLUDE_USER_TIMESTAMP
    #include "vxTimestamp.h"
#endif

/* Flash support */
#define NV_RAM_SIZE     NONE

#define INCLUDE_FLASH         

/* Save boot line in Flash memory */
#ifdef  INCLUDE_FLASH
    #define  INCLUDE_FLASH_BOOTLINE 
#ifdef PMC280_DUAL_CPU
    #define   NV_BOOT_OFFSET_CPU0   0
#ifdef ROHS
#define   NV_BOOT_OFFSET_CPU1   0xff
#else
    #define   NV_BOOT_OFFSET_CPU1   0x40000
#endif
    #undef   NV_BOOT_OFFSET
    #define  NV_BOOT_OFFSET  nvBootOffset   /* User defined */
#else
    #define  NV_BOOT_OFFSET  0   /* User defined */
#endif /* PMC280_DUAL_CPU */
#endif
/* Added to handle Machine Check Excpetion */
#define PCI_ERROR_REPORT
#define SDRAM_ECC_ERROR_PROPAGATE
#define SRAM_ERROR_PROPAGATE
/* Add the Slave address Decode Base Address here */
#define PCI0_CS0_SLAVE_ADDRESS   (0x80000000)

/* WDB Serial *
#undef WDB_COMM_TYPE WDB_COMM_SERIAL	
#undef WDB_TTY_CHANNEL		1	
#define WDB_COMM_TYPE WDB_COMM_SERIAL	
#define WDB_TTY_CHANNEL		0	
 WDB */

/* debugging aids */
#if	DEBUG_CONFIG_ALL
    #define	PRINTF(x)			printf x
#else	
    #define	PRINTF(x)
#endif	
    
/*                              
 * If we are building a ".st" image (vxWorks.st, vxWorks.st_rom, etc)
 * then STANDALONE is defined, and if WDB is using a network device
 * like netif or END then we need to define STANDALONE_NET to start
 * the network. SPR#23450.  Not needed in T2.
 */

#if defined (STANDALONE) 
    #if defined(INCLUDE_WDB) && ((WDB_COMM_TYPE == WDB_COMM_NETWORK) || \
                                (WDB_COMM_TYPE == WDB_COMM_END))
        #define	STANDALONE_NET
    #endif 
#endif /* STANDALONE */
            
            
/*
 * User application initialization
 *
 * USER_APPL_INIT must be a valid C statement or block.  It is
 * included in the usrRoot() routine only if INCLUDE_USER_APPL is
 * defined.  The code for USER_APPL_INIT is only an example.  The
 * user is expected to change it as needed.  The use of taskSpawn
 * is recommended over direct execution of the user routine.
 */
#define USER_APPL_INIT 				\
    { 								\
        IMPORT int userAppInit(); 	\
        userAppInit();  			\
    }
			
			
#endif	/* __INCconfigh */

#include "gtSysGal.h"

/* The following line has been added by WindConfig */
#include "configdb.h"

#if defined(PRJ_BUILD)
#include "prjParams.h"
#endif

