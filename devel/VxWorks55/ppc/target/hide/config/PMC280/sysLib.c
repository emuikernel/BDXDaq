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

*******************************************************************************
* sysLib.c  - System-dependent routines
*
* DESCRIPTION:
*       This library provides board-specific routines.
*       The chip drivers included are:
*
*       ppcDecTimer.c   - PowerPC decrementer timer library (system clock)
*
* DEPENDENCIES:
*       None.
* Sept 20th 2002
* Added PCI0 mem. and I/O space mapping in the BAT table.
* Sept 28th 2002
* Made SRAM region in the MMU table NON-CACHED,this change was made for 
* performance reasons for mgi driver.mgi driver allocates Tx/RX descriptors
* (not TX/RX buffers) from SRAM.
******************************************************************************/

/* includes */
#include <stdio.h>
#include "vxWorks.h"
#include "memLib.h"
#include "cacheLib.h"
#include "sysLib.h"
#include "config.h"
#include "string.h"
#include "intLib.h"
#include "logLib.h"
#include "taskLib.h"
#include "vxLib.h"
#include "tyLib.h"                  
#include "private/vmLibP.h" /* page 76 Bsp development kit */
#include "arch/ppc/mmu603Lib.h"  
#include "arch/ppc/vxPpcLib.h"
#include "pciScan.h"
#include "gtIntControl.h"
#include "gtInitLib.h"
#include "sysIntCtrl.h"
#include "i2cDrv.h"

#ifdef PMC280_DUAL_CPU

#include "gtSmp.h"
/* #include "sysCache.c"*/

#else

/* #include "sysCache.c" */

#endif /* PMC280_DUAL_CPU */

#include "pciConfigLib.h"
#include "gtDma.h"
#include "gtCore.h"
#include "gtMemory.h"
#include "gtSram.h"
#include "gtPci.h" /* for mapCs */

#include "flashDrv.h"

#ifdef PMC280_DEBUG_UART_VX
#include "dbgprints.h"
#endif /* PMC280_DEBUG_UART_VX */

#if defined (INCLUDE_NETWORK) && defined (INCLUDE_DC_NETIF)
    #include "drv/netif/if_dc.h"        /* netif dc driver */
#endif /* INCLUDE_NETWORK */

#if defined (INCLUDE_ALTIVEC)
    #include "altivecLib.h"
#endif

#ifdef INCLUDE_L2_CACHE
#include "sysL2BackCache.h"
#endif /* INCLUDE_L2_CACHE */


IMPORT int (* _func_altivecProbeRtn) () ;
IMPORT UINT32 vxHid1Get(void);

#ifdef INCLUDE_END_FEI
IMPORT STATUS  sys557PciInit(void);
#endif

#ifdef INCLUDE_GEI_82543_END
IMPORT STATUS  sys543PciInit(void);
#endif

IMPORT unsigned int FREE_RAM_ADRS;

extern  unsigned int frc_end_of_vxworks; 

/* defines */

/* globals */
#ifdef PMC280_DUAL_CPU
#ifdef INCLUDE_FLASH_BOOTLINE
UINT32 nvBootOffset;
#endif /* INCLUDE_FLASH_BOOTLINE */
#endif /* PMC280_DUAL_CPU */

/* SPR#25018, evtTimeStamp not used in WV2.0, but should be defined. */
/* PMC280 Changes */
int evtTimeStamp = (int)NULL; 

/*
 * sysBatDesc[] is used to initialize the block address translation (BAT)
 * registers within the PowerPC 603/604 MMU.  BAT hits take precedence
 * over Page Table Entry (PTE) hits and are faster.  Overlap of memory
 * coverage by BATs and PTEs is permitted in cases where either the IBATs
 * or the DBATs do not provide the necessary mapping (PTEs apply to both
 * instruction AND data space, without distinction).
 *
 * The primary means of memory control for VxWorks is the MMU PTE support
 * provided by vmLib and cacheLib.  Use of BAT registers will conflict
 * with vmLib support.  User's may use BAT registers for i/o mapping and
 * other purposes but are cautioned that conflicts with cacheing and mapping
 * through vmLib may arise.  Be aware that memory spaces mapped through a BAT
 * are not mapped by a PTE and any vmLib() or cacheLib() operations on such
 * areas will not be effective, nor will they report any error conditions.
 *
 * Note: BAT registers CANNOT be disabled - they are always active.
 * For example, setting them all to zero will yield four identical data
 * and instruction memory spaces starting at local address zero, each 128KB
 * in size, and each set as write-back and cache-enabled.  Hence, the BAT regs
 * MUST be configured carefully.
 *
 * With this in mind, it is recommended that the BAT registers be used
 * to map LARGE memory areas external to the processor if possible.
 * If not possible, map sections of high RAM and/or PROM space where
 * fine grained control of memory access is not needed.  This has the
 * beneficial effects of reducing PTE table size (8 bytes per 4k page)
 * and increasing the speed of access to the largest possible memory space.
 * Use the PTE table only for memory which needs fine grained (4KB pages)
 * control or which is too small to be mapped by the BAT regs.
 *
 * The BAT configuration for 4xx/6xx-based PPC boards is as follows:
 * All BATs point to PROM/FLASH memory so that end customer may configure
 * them as required.
 *
 * [Ref: chapter 7, PowerPC Microprocessor Family: The Programming Environments]
 */

UINT32 sysBatDesc [2 * (_MMU_NUM_IBAT + _MMU_NUM_EXTRA_IBAT + 
                        _MMU_NUM_DBAT + _MMU_NUM_EXTRA_DBAT)] =
{
    /* I BAT 0 */
    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
     _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
     _MMU_LBAT_CACHE_INHIBIT),

    /* I BAT 1 */
    0,
    0,

    /* I BAT 2 */
    0,
    0,

    /* I BAT 3 */
    0,
    0,

    /* D BAT 0 */
    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
     _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
     _MMU_LBAT_CACHE_INHIBIT),





/* Fix
     * Initially 1 BAT Register namely DBAT1 was used , and it was mapped to 256 MB, but 
     * we required 128 MB (PCI MEM) + 16MB(PCI IO) i.e overall 144MB.  Hence unnecessarily 
     * it was mapped to Unused space. To overcome that mapping to unused space two BATS 
     * are used DBAT1 for 128 MB and DBAT 2 for 16 MB. 
     */
    /* D BAT 1 for Size of 128MB */

    /* D BAT 1 */
    /*sergey: comment out (see replacement below)
    ((PCI0_MEM0_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_128M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((PCI0_MEM0_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
      _MMU_LBAT_GUARDED |_MMU_LBAT_CACHE_INHIBIT),
	*/

	/* D BAT 2 */
    /*sergey: comment out (see replacement below)
    ((PCI0_IO_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_16M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((PCI0_IO_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW  |
	  _MMU_LBAT_CACHE_INHIBIT),    
	*/
	





/*sergey: we want to map 512M, so I changed '_MMU_UBAT_BL_128M' to '_MMU_UBAT_BL_512M';
as result only 256M was mapped; it is probably a limit of 256M for single BAT (??); anyway
I decided to use 2 BATs to map our 512M as following:*/

    /* D BAT 1 */
    ((PCI0_MEM2_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_256M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((PCI0_MEM2_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
      _MMU_LBAT_GUARDED |_MMU_LBAT_CACHE_INHIBIT),

	/* D BAT 2 */
    (((PCI0_MEM2_BASE_ADRS+0x10000000) & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_256M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    (((PCI0_MEM2_BASE_ADRS+0x10000000) & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
      _MMU_LBAT_GUARDED |_MMU_LBAT_CACHE_INHIBIT),

/*sergey: we will not map PCI0_IO_BASE_ADRS at all since we do not need it ..*/







    /* D BAT 3 */
	0,
	0,
    /*((CS0_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_32M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((CS0_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW  |
	  _MMU_LBAT_CACHE_INHIBIT),    */



    /* D BAT 3 */

	/*
    ((CS1_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_32M | _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((CS1_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW  |
	  _MMU_LBAT_CACHE_INHIBIT),    

	*/
};

#ifdef INCLUDE_MMU
/*
 * sysPhysMemDesc[] is used to initialize the Page Table Entry (PTE) array
 * used by the MMU to translate addresses with single page (4k) granularity.
 * PTE memory space should not, in general, overlap BAT memory space but
 * may be allowed if only Data or Instruction access is mapped via BAT.
 *
 * Address translations for local RAM, memory mapped PCI bus, memory mapped
 * VME A16 space and local PROM/FLASH are set here.
 *
 * PTEs are held, strangely enough, in a Page Table.  Page Table sizes are
 * integer powers of two based on amount of memory to be mapped and a
 * minimum size of 64 kbytes.  The MINIMUM recommended Page Table sizes
 * for 32-bit PowerPCs are:
 *
 * Total mapped memory        Page Table size
 * -------------------        ---------------
 *        8 Meg                 64 K
 *       16 Meg                128 K
 *       32 Meg                256 K
 *       64 Meg                512 K
 *      128 Meg                  1 Meg
 *     .                .
 *     .                .
 *     .                .
 *
 * [Ref: chapter 7, PowerPC Microprocessor Family: The Programming Environments]
 */

#ifdef PMC280_DUAL_CPU
#ifdef PMC280_DUAL_CPU_PTE_SETUP
/* This is a dummy for unresolved symbols in usrMmuInit and mmuPpcLibInit */
PHYS_MEM_DESC sysPhysMemDesc [] = {
};

/*
 * It is necessary to keep this is sync with earlysysPhysMemDesc0[] defined 
 * in frcMmu.c
 */
PHYS_MEM_DESC sysPhysMemDesc0 [] =
{
    {
    (void *) VIR_CPU0_MEM_BOT,
    (void *) PHY_CPU0_MEM_BOT,
    (UINT) (NULL), /* Will be filled later */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE
    },

    {
    (void *) NULL, /* Will be filled later */
    (void *) NULL, /* Will be filled later */
    USER_RESERVED_MEM,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) ROM_BASE_ADRS,
    (void *) ROM_BASE_ADRS,
    0x100000,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },
    {
    (void *) INTERNAL_REG_ADRS,
    (void *) INTERNAL_REG_ADRS,
    0x10000,
#if FALSE
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_GUARDED,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_GUARDED
#endif /* #if FALSE */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) CS0_BASE_ADRS,
    (void *) CS0_BASE_ADRS,
    CS0_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY
    },

	/*Aravind*/
   

    {
    (void *) CS1_BASE_ADRS,
    (void *) CS1_BASE_ADRS,
    CS1_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY | VM_STATE_GUARDED
    
    },


    {
    (void *) INTERNAL_SRAM_ADRS,
    (void *) INTERNAL_SRAM_ADRS,
    0x40000,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY
    },

    /*
     * Here are a few dummy entries for any dynamic mappings developer
     * may need in the future.  They are used via a call to sysMmuMapAdd().
     * Do ensure that the BSP provides sufficient entries!
     */
	
    {
    (void *) PCI0_IO_BASE_ADRS,
    (void *) PCI0_IO_BASE_ADRS,
    0x1000000,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY
    },
	

    /*  PCI0  */

    DUMMY_MMU_ENTRY,

    /*sergey: copied from single CPU version - see below
    {
    (void *) (PCI0_MEM2_BASE_ADRS),
    (void *) (PCI0_MEM2_BASE_ADRS),
    PCI0_MEM2_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT
    },
	*/

    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,

    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY
};

/*
 * It is necessary to keep this is sync with earlysysPhysMemDesc1[] defined 
 * in frcMmu.c
 */
PHYS_MEM_DESC sysPhysMemDesc1 [] =
{
    {
    (void *) VIR_CPU1_MEM_BOT,
    (void *) NULL, /* Will be filled later */
    (UINT) (NULL), /* Will be filled later */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE
    },

    {
    (void *) NULL, /* Will be filled later */
    (void *) NULL, /* Will be filled later */
    USER_RESERVED_MEM,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) ROM_BASE_ADRS,
    (void *) ROM_BASE_ADRS,
    0x100000,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },
    {
    (void *) INTERNAL_REG_ADRS,
    (void *) INTERNAL_REG_ADRS,
    0x10000,
#if FALSE
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_GUARDED,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_GUARDED
#endif /* #if FALSE */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) CS0_BASE_ADRS,
    (void *) CS0_BASE_ADRS,
    CS0_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY | VM_STATE_GUARDED      
    },

    {
    (void *) INTERNAL_SRAM_ADRS,
    (void *) INTERNAL_SRAM_ADRS,
    0x40000,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY
    },
	
/*Aravind*/
   

    {
    (void *) CS1_BASE_ADRS,
    (void *) CS1_BASE_ADRS,
    CS1_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT | VM_STATE_MEM_COHERENCY | VM_STATE_GUARDED
    
    },


    /*
     * Here are a few dummy entries for any dynamic mappings developer
     * may need in the future.  They are used via a call to sysMmuMapAdd().
     * Do ensure that the BSP provides sufficient entries!
     */

    DUMMY_MMU_ENTRY,

    /*  PCI0  */
    /*sergey: copied from single CPU version - see below
    {
    (void *) (PCI0_MEM2_BASE_ADRS),
    (void *) (PCI0_MEM2_BASE_ADRS),
    PCI0_MEM2_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT
    },
	*/



    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY,

    DUMMY_MMU_ENTRY,
    DUMMY_MMU_ENTRY
};
#endif /* PMC280_DUAL_CPU_PTE_SETUP */

#else /* single CPU version */

PHYS_MEM_DESC sysPhysMemDesc [] =
{
    {  
    /* Vector Table and Interrupt Stack */
    /* Must be sysPhysMemDesc [0] to allow adjustment in sysHwInit() */
    (void *) LOCAL_MEM_LOCAL_ADRS,
    (void *) LOCAL_MEM_LOCAL_ADRS,
    RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE
    },




    /* Sergey: our main memory !!! */
    /* Sergey: Local DRAM - length is dynamically altered */

    {
    /* Must be sysPhysMemDesc [1] to allow adjustment in sysHwInit() */
    (void *) RAM_LOW_ADRS,
    (void *) RAM_LOW_ADRS,
    (UINT) NULL, /* Will be filled later */ /*prpmc880: LOCAL_MEM_SIZE-RAM_LOW_ADRS*/

	/* Sergey: prpmc880 have VM_STATE_MASK_MEM_COHERENCY
       and VM_STATE_MEM_COHERENCY, do we need it here ??? */
	/* and use VM_STATE_CACHEABLE_NOT instead of VM_STATE_CACHEABLE */
	/* host can see all changes immediately but network performance is low
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_MEM_COHERENCY
	*/
	/* original version: good performance, memory does not behaive
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE
	*/
	/* low network performance, host see our memory immediately
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
	*/
    /* performance good but memory does not behaive, although pmc280 seems see host's changes in its
    memory (almost) instantly */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE | VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE | VM_STATE_MEM_COHERENCY
    },     



    {
    /* Must be sysPhysMemDesc [2] to allow adjustment in sysHwInit() */
	(void *) NULL, /* Will be filled later */ /*prpmc880: (LOCAL_MEM_LOCAL_ADRS+LOCAL_MEM_SIZE)*/
    (void *) NULL,  /* Will be filled later */ /*prpmc880: (LOCAL_MEM_LOCAL_ADRS+LOCAL_MEM_SIZE)*/
    USER_RESERVED_MEM,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) BOOTCS_BASE_ADRS,
    (void *) BOOTCS_BASE_ADRS,
    BOOTCS_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) INTERNAL_REG_ADRS,
    (void *) INTERNAL_REG_ADRS,
    INTERNAL_REG_SIZE,  
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE 
    | VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  
    | VM_STATE_GUARDED      
    },

    {
    (void *) INTERNAL_SRAM_ADRS,
    (void *) INTERNAL_SRAM_ADRS,
    INTERNAL_SRAM_SIZE,  
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

	{
    (void *) CS0_BASE_ADRS,  
    (void *) CS0_BASE_ADRS,
    CS0_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE 
    | VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  
    | VM_STATE_GUARDED      
    },

    {
    (void *) CS1_BASE_ADRS,
    (void *) CS1_BASE_ADRS,
    CS1_ADRS_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE 
    | VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  
    | VM_STATE_GUARDED      
    },


    /* 
     * Here are a few dummy entries for any dynamic mappings developer
     * may need in the future. They are used via a call to sysMmuMapAdd().
     * Do ensure that the BSP provides sufficient entries!
     */

#ifndef PMC280_DUAL_CPU


    DUMMY_MMU_ENTRY,    /* Dummy for PCI0 MEM0 auto detection */

	/*Sergey: without following mem2 was mapped, but smaller memory; it was fixed
    by using 2 BATs instead of one (see above) so following piece is removed; I should
    learn more about relations between BATs and current array ...*/

    /* Sergey: access to HOST's memory, which must be mapped at PCI address 0 */
    /* from prpmc880:
     * Access to PCI memory space at PCI address zero.
     * This will allow the PrPMC to access DRAM on the processor board
     * on which the PrPMC is mounted if that processor board maps its
     * DRAM to PCI address zero.
     * Note: Guarding should not be required since
     * memory is well-behaved (no side-effects on read or write)
     */

	/*sergey: seems D BAT 1 is enough ???
    {
    (void *) (PCI0_MEM2_BASE_ADRS),
    (void *) (PCI0_MEM2_BASE_ADRS),
    PCI0_MEM2_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT
    },
	*/



    DUMMY_MMU_ENTRY,    /* Dummy for PCI0 MEM1 auto detection */
    DUMMY_MMU_ENTRY,    /* Dummy for PCI0 MEM2 auto detection */
    DUMMY_MMU_ENTRY,     /* Dummy for PCI0 MEM3 auto detection */
    DUMMY_MMU_ENTRY,     /* Dummy for PCI0 IO   auto detection */
#endif

#ifndef PMC280_DUAL_CPU
    DUMMY_MMU_ENTRY, 
    DUMMY_MMU_ENTRY, 
#endif
};
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DUAL_CPU
unsigned int  frcVirtToPhyCPU1memBase = 0x0;
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DUAL_CPU
/* This is a dummy for unresolved symbols in usrMmuInit and mmuPpcLibInit */
int   sysPhysMemDescNumEnt;      /* calculated in sysHwInit */
int   sysPhysMemDescNumEnt0;      /* calculated in sysHwInit */
int   sysPhysMemDescNumEnt1;      /* calculated in sysHwInit */
#else
int   sysPhysMemDescNumEnt;      /* calculated in sysHwInit */
#endif /* PMC280_DUAL_CPU */

#endif /* INCLUDE_MMU */

int   sysBus      = VME_BUS;        /* legacy */
int   sysCpu      = CPU;            /* system CPU type (MC680x0)    */
char *sysBootLine = BOOT_LINE_ADRS; /* address of boot line         */
char *sysExcMsg   = EXC_MSG_ADRS;   /* catastrophic message area    */
int   sysProcNum  = 0;              /* processor number of this CPU */
int   sysFlags;                     /* boot flags */
char  sysBootHost [BOOT_FIELD_LEN]; /* name of host from which we booted */
char  sysBootFile [BOOT_FIELD_LEN]; /* name of file from which we booted */

/* static locals */

LOCAL char sysModelStr [30] = "";    
LOCAL char wrongCpuMsg []   = WRONG_CPU_MSG; 

/* forward declarations */

char *sysPhysMemTop   (void);        /* Top of physical memory */
char *sysInfo         (void);     
void  sysMemMapDetect (void);        /* detect PREP or CHRP map */
void  sysCpuCheck     (void);        /* detect CPU type */
void  sysDelay        (void);        /* delay 1 millisecond */
void  sysMsDelay      (UINT delay);  /* delay N millisecond(s) */
void  sysHardReset    (void);

#ifdef INCLUDE_MMU        /* add a sysPhysMemDesc entry */
STATUS  sysMmuMapAdd        (void *address, UINT len,   
                             UINT initialStateMask, UINT initialState);
#endif /* INCLUDE_MMU */

/* external imports */
VOID   sysOutWord (ULONG address, UINT16 data);   /* sysALib.s */
VOID   sysOutLong (ULONG address, ULONG data);    /* sysALib.s */
USHORT sysInWord  (ULONG address);                /* sysALib.s */
ULONG  sysInLong  (ULONG address);                /* sysALib.s */
IMPORT VOID   sysOutByte    (ULONG, UCHAR);       /* sysALib.s */
IMPORT UCHAR  sysInByte     (ULONG);              /* sysALib.s */
IMPORT STATUS sysMemProbeSup(int length, char * src, char * dest);
IMPORT STATUS sysGetPvr     (void);

IMPORT VOIDFUNCPTR _pSysL2CacheInvFunc;
IMPORT VOIDFUNCPTR _pSysL2CacheEnable;
IMPORT VOIDFUNCPTR _pSysL2CacheDisable;
IMPORT VOIDFUNCPTR _pSysL2CacheFlush;
IMPORT BOOL snoopEnable;

#if defined INCLUDE_ATA                            /* ATA / EIDE driver */
IMPORT ATA_CTRL   ataCtrl [];                   /* for sysAta.c      */
IMPORT VOIDFUNCPTR _func_sysAtaInit;
#endif

IMPORT int frcFlashBlockErase(unsigned int ,unsigned int);
IMPORT short frcFlashProgram (unsigned *, unsigned *, unsigned int, unsigned int);

/* BSP DRIVERS */

#ifdef INCLUDE_SYSCLK
    #include "./ppcDecTimer.c"    /* decrementer system clock */
#endif /* INCLUDE_SYSCLK */

#if defined(INCLUDE_SERIAL)
#include "sysMpscSerial.c"
#endif


#include "frcBoardId.h"

#define REV_A0 0x0
#define REV_A1 0x1
#define REV_A2 0x2
#define REV_B  0x3

UINT8 frcSysBoardId[MAX_BOARD_ID_SIZE];

/*
 * This function should be called to read the board information from
 * serial EEPROM. This information should not be confused with the BIB
 * information. This is a *very* small subset of the information stored
 * in the BIB is for exclusive use during board debugging, test as
 * as during the initialisation of the BSP (since the sources are 
 * common across all variants of PMC280. This is surely not meant for
 * use by applications.
 */ 
unsigned int frcBoardIdInit()
{
    /* 
     * Read serial EEPROM for information.
     */
    frcEEPROMRead16(BRDID_EE_ADDR, 
                    BRDID_OFFSET, 
                    MAX_BOARD_ID_SIZE, 
                    frcSysBoardId);

    return OK;
}

UINT8 frcGetPcbRev()
{
    UINT8 temp = frcSysBoardId[PCB_REV_BYTE];
    return(temp & PCB_REV_MASK);
}
void frcPrintPcbRev()
{
    switch(frcGetPcbRev())
    {
        case PCB_REV_E0:   printf("PCB Rev: E0 \n"); break;
        case PCB_REV_B:    printf("PCB Rev: B \n"); break;
        case PCB_REV_C:    printf("PCB Rev: C \n"); break;
        case PCB_REV_D:    printf("PCB Rev: D \n"); break;
        case PCB_REV_E:    printf("PCB Rev: E \n"); break;
        case PCB_REV_F:    printf("PCB Rev: F \n"); break;
        case PCB_REV_G:    printf("PCB Rev: G \n"); break;
    }
}    

void frcPrintBomRev()
{
    UINT8 temp = frcSysBoardId[BOM_REV_BYTE];
    switch((temp & BOM_REV_MASK) >> 3)
    {
        case BOM_REV_0:    printf("BOM Rev: 0 \n"); break;
        case BOM_REV_1:    printf("BOM Rev: 1 \n"); break;
        case BOM_REV_2:    printf("BOM Rev: 2 \n"); break;
        case BOM_REV_3:    printf("BOM Rev: 3 \n"); break;
        case BOM_REV_4:    printf("BOM Rev: 4 \n"); break;
        case BOM_REV_5:    printf("BOM Rev: 5 \n"); break;
        case BOM_REV_6:    printf("BOM Rev: 6 \n"); break;
        case BOM_REV_7:    printf("BOM Rev: 7 \n"); break;
        default:           printf("Unknown BOM Rev\n");
    }
}  

UINT8 frcGetUserFlashSize()
{
    UINT8 temp = frcSysBoardId[FLASH_SIZE_BYTE];
    return(temp & FLASH_SIZE_MASK);
}
void frcPrintUserFlashSize()
{
    switch(frcGetUserFlashSize())
    {
        case FLASH_SIZE_0MB:    printf("User Flash Size:   0MB\n"); break;
        case FLASH_SIZE_16MB:   printf("User Flash Size:  16MB\n"); break;
        case FLASH_SIZE_32MB:   printf("User Flash Size:  32MB\n"); break;
        case FLASH_SIZE_64MB:   printf("User Flash Size:  64MB\n"); break;
        case FLASH_SIZE_128MB:  printf("User Flash Size: 128MB\n"); break;
        default:                printf("ERROR: Undefined User Flash Size\n"); break;
    }
}

UINT8 frcGetBank0MemSize()
{
    UINT8 temp = frcSysBoardId[CS0_MEM_SIZE_BYTE];
    return(temp & CS0_MEM_SIZE_MASK);
}
void frcPrintBank0MemSize()
{
    printf("Bank0 ");
    switch(frcGetBank0MemSize())
    {
        case CS0_MEM_SIZE_128MB:  printf("Memory Size: 128MB \n"); break;
        case CS0_MEM_SIZE_256MB:  printf("Memory Size: 256MB \n"); break;
        case CS0_MEM_SIZE_512MB:  printf("Memory Size: 512MB \n"); break;
        case CS0_MEM_SIZE_1GB:    printf("Memory Size:   1GB \n"); break;
        case CS0_MEM_SIZE_2GB:    printf("Memory Size:   2GB \n"); break;
        default:                  printf("ERROR: Undefined Memory Size\n"); break;
    }
}

UINT8 frcGetBank1MemSize()
{
    UINT8 temp = frcSysBoardId[CS1_MEM_SIZE_BYTE];
    return(temp & CS1_MEM_SIZE_MASK);
}
void frcPrintBank1MemSize()
{
    printf("Bank1 ");
    switch(frcGetBank1MemSize())
    {
        case CS1_MEM_SIZE_0MB:    printf("Memory Size: 0MB \n"); break;
        case CS1_MEM_SIZE_128MB:  printf("Memory Size: 128MB \n"); break;
        case CS1_MEM_SIZE_256MB:  printf("Memory Size: 256MB \n"); break;
        case CS1_MEM_SIZE_512MB:  printf("Memory Size: 512MB \n"); break;
        case CS1_MEM_SIZE_1GB:    printf("Memory Size:   1GB \n"); break;
        case CS1_MEM_SIZE_2GB:    printf("Memory Size:   2GB \n"); break;
        default:                  printf("ERROR: Undefined Memory Size\n"); break;
    }
}

UINT8 frcGetVariant()
{
    UINT8 temp = frcSysBoardId[VARIANT_BYTE];
    return(temp & VARIANT_MASK);
}
void frcPrintVariant()
{
    switch(frcGetVariant())
    {
        case VARIANT_A:    printf("Variant: A \n"); break;
        case VARIANT_B:    printf("Variant: B \n"); break;
        case VARIANT_C:    printf("Variant: C \n"); break;
        case VARIANT_D:    printf("Variant: D \n"); break;
        case VARIANT_E:    printf("Variant: E \n"); break;
        case VARIANT_F:    printf("Variant: F \n"); break;
        case VARIANT_G:    printf("Variant: G \n"); break;
        case VARIANT_H:    printf("Variant: H \n"); break;
        case VARIANT_I:    printf("Variant: I \n"); break;
        case VARIANT_J:    printf("Variant: J \n"); break;
        case VARIANT_K:    printf("Variant: K \n"); break;
        case VARIANT_L:    printf("Variant: L \n"); break;
        case VARIANT_M:    printf("Variant: M \n"); break;
        case VARIANT_N:    printf("Variant: N \n"); break;
        case VARIANT_O:    printf("Variant: O \n"); break;
        case VARIANT_P:    printf("Variant: P \n"); break;
        default:   printf("ERROR: Undefined variant \n"); break;
    }
}

UINT8 frcGetFlashOrg() 
{
    UINT8 temp = frcSysBoardId[FLASH_ORG_BYTE];
    return(temp & FLASH_ORG_MASK);
}

UINT8 frcGetExtSramSize() {
    UINT8 temp = frcSysBoardId[EXT_SRAM_BYTE];
    return(temp & EXT_SRAM_MASK);
}


unsigned int frcMv64360SiliconVersion()
{
    unsigned int data = 0;

    pciConfigInLong(0x0, 0x0, 0x0, 0x8, &data);

    switch(data & 0x000000FF)
    {
        case 0: return REV_A0; break;
        case 1: return REV_A1; break;
        case 2: return REV_A2; break;
        case 3: return REV_B; break;
    }
   
    return OK;
}

void frcPrintMv64360Version()
{
    switch(frcMv64360SiliconVersion())
    {
        case 0: printf("MV64360 Revsion: A0\n"); break;
        case 1: printf("MV64360 Revsion: A1\n"); break;
        case 2: printf("MV64360 Revsion: A2\n"); break;
        case 3: printf("MV64360 Revsion: B\n"); break;
    }
}

void frcPrintBoardInfo()
{
    frcPrintMv64360Version();
    frcPrintPcbRev();
    frcPrintBomRev();
    frcPrintUserFlashSize();
    frcPrintBank0MemSize();
    frcPrintBank1MemSize();
    frcPrintVariant();
}


/*******************************************************************************
* sysCpuFreq - return the cpu frequency by reading the Hid1 register.
*
* This routine returns the CPU frequency by reading the Hid1 register's external
* configuration bits which indicate the factor by which the system clock must be 
* multiplied to obtain the cpu clock frequency. 
*
* RETURNS: The CPU clock frequency.
*
********************************************************************************/ 
void frcCPUClockGet()
{
  UINT32 temp;
  float factor,cpuclock;
  
  temp=vxHid1Get();    
  temp = (temp >> 27);  
  factor = ((float)temp)/2;  
  cpuclock = ( factor * SYSCLOCK );
  printf("The CPU clock frequency is: %fMHz\n",cpuclock);
    
}

/******************************************************************************
* sysModel - return the model name of the CPU board
*
* This routine returns the model name of the CPU board.  The returned string
* depends on the board model and CPU version being used, for example,
* "Motorola YK X4 - MPC740/750".
*
* RETURNS: A pointer to the string.
*/
char * sysModel (void)
{
    char tmpStr[15] = "";
    sprintf (sysModelStr, "%s", PMC280_MODEL);    

    switch(frcGetVariant())
    {
        case VARIANT_A: strcat (sysModelStr, "A-"); break;
        case VARIANT_B: strcat (sysModelStr, "B-"); break;
        case VARIANT_C: strcat (sysModelStr, "C-"); break;
        case VARIANT_D: strcat (sysModelStr, "D-"); break;
        case VARIANT_E: strcat (sysModelStr, "E-"); break;
        case VARIANT_F: strcat (sysModelStr, "F-"); break;
        case VARIANT_G: strcat (sysModelStr, "G-"); break;
        case VARIANT_H: strcat (sysModelStr, "H-"); break;
        case VARIANT_I: strcat (sysModelStr, "I-"); break;
        case VARIANT_J: strcat (sysModelStr, "J-"); break;
        case VARIANT_K: strcat (sysModelStr, "K-"); break;
        case VARIANT_L: strcat (sysModelStr, "L-"); break;
        case VARIANT_M: strcat (sysModelStr, "M-"); break;
        case VARIANT_N: strcat (sysModelStr, "N-"); break;
        case VARIANT_O: strcat (sysModelStr, "O-"); break;
        case VARIANT_P: strcat (sysModelStr, "P-"); break;
        default: strcat (sysModelStr, "X-"); break;
    }

    /* The CPU module is indicated in the Processor Version Register (PVR) */
    switch(CPU_TYPE)
    {
        case CPU_TYPE_750:
            if(CPU_VERSION == CPU_MODEL_750)
        strcat (sysModelStr, "MPC750");
            else if(CPU_VERSION == CPU_MODEL_750L)
        strcat (sysModelStr, "IBM750L");
            else if(CPU_VERSION == CPU_MODEL_750CX)
        strcat (sysModelStr, "IBM750cx");
            else if(CPU_VERSION == CPU_MODEL_755)
        strcat (sysModelStr, "MPC755");
            else
            {
                sprintf(tmpStr,"750 (0x%04x)",CPU_VERSION);
                strcat(sysModelStr, tmpStr);
            }
            break;
        case CPU_TYPE_750FX:
            strcat (sysModelStr, "IBM750FX");
            break;
        case CPU_TYPE_7400:
            strcat (sysModelStr, "MPC7400");
            break;
        case CPU_TYPE_7410:
            strcat (sysModelStr, "MPC7410");
            break;
        case CPU_TYPE_7450:
            strcat (sysModelStr, "MPC7450");
            break;
        case CPU_TYPE_7447:
            strcat (sysModelStr, "MPC7447");
            break;
        case CPU_TYPE_7447A:
            strcat (sysModelStr, "MPC7447A");
            break;
        case CPU_TYPE_7455:
            strcat (sysModelStr, "MPC7455");
            break;

        default:
            sprintf(tmpStr,"??? (0x%04x)",CPU_TYPE);
            strcat(sysModelStr, tmpStr);
            break;
    }  /* switch  */
    
    sprintf(tmpStr," (%d.%d)",CPU_SUB_VER1,CPU_SUB_VER2);
    strcat(sysModelStr, tmpStr); 

    return(sysModelStr);
}

#ifdef INCLUDE_ALTIVEC
/*******************************************************************************
*
* sysAltivecProbe - Check if the CPU has ALTIVEC unit.
*
* This routine returns OK it the CPU has an ALTIVEC unit in it.
* Presently it checks for 7400
* RETURNS: OK  -  for 7400 Processor type
*          ERROR - otherwise.
*/

int  sysAltivecProbe (void)
{
    ULONG regVal;
    int altivecUnitPresent = ERROR;

     /* The CPU type is indicated in the Processor Version Register (PVR) */

     regVal = CPU_TYPE;

     switch (regVal)
     {
          case CPU_TYPE_7400:
          case CPU_TYPE_7410:
          case CPU_TYPE_7450:
          case CPU_TYPE_7447:
          case CPU_TYPE_7447A:
          case CPU_TYPE_7455:
             altivecUnitPresent = OK;
            break;

          default:
             break;
     }      /* switch  */

    return (altivecUnitPresent);
 }
#endif /* INCLUDE_ALTIVEC */

/*******************************************************************************
* sysBspRev - return the BSP version and revision number
*
* This routine returns a pointer to a BSP version and revision number, for
* example, 1.1/0. BSP_REV is concatenated to BSP_VERSION and returned.
*
* RETURNS: A pointer to the BSP version/revision string.
*/
char * sysBspRev (void)
{
    return(BSP_VERSION BSP_REV);
}

#ifdef INCLUDE_FLASH_BOOTLINE
/******************************************************************************
* flashBootSectorInit - Init Flash boot line address
* This function assigns EOS to boot line address in flash in case the Flash is 
* empty (full with FFFFs). This is done because the nvRamget function refer
* to EOS sign as an empty memory (the Flash is FFFF when empty).
*
* Inputs: N/A
*
* Outputs: The boot line address is EOS in case flash is empty.
*
* RETURN: N/A
*/
void flashBootSectorInit(void)
{
    UINT32 data;
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI()) {
        nvBootOffset = NV_BOOT_OFFSET_CPU1;    /* CPU 1 */
    }
    else {
        nvBootOffset = NV_BOOT_OFFSET_CPU0;    /* CPU 0 */
    }
    
#endif /* PMC280_DUAL_CPU */

    frcFlashRead((FLASH_BANK0+NV_BOOT_OFFSET), 1, &data);
    if(data == 0xFFFFFFFF) {
        data = EOS;
        frcFlashProgram((unsigned *)(FLASH_BANK0+NV_BOOT_OFFSET), &data, 4, FLASH_BANK0);
    }
}
#endif /* INCLUDE_FLASH_BOOTLINE */

#ifdef PMC280_DUAL_CPU

#ifdef PMC280_PCI_BOOT

#define PCI_BOOT_SYNC_ADRS    0xF1000800
#define PCI_BOOT_CONF_ADRS    0xF1000800
#define PCI_BOOT_SYNC_DATA    (0xA5A6B5B6)
#define PCI_BOOT_CONF_DATA    (0xC5C6D5D6)

/*
 * The signalling mechanism works like this:
 * 
 *    CPU0 waits for PCI_BOOT_SYNC_ADRS to be set. Once CPU1 sets this location
 *    it continues with its boot.
 *
 *    CPU1 (as soon as it enters sysHwInit) sets PCI_BOOT_SYNC_ADRS.
 */
void frcInitPciBootSyncLocation()
{
    unsigned int *addr = (unsigned int *)PCI_BOOT_SYNC_ADRS;
    *addr = 0x0;
}

void frcSetPciBootSyncLocation()
{
    unsigned int *addr = (unsigned int *)PCI_BOOT_SYNC_ADRS;
    while(*addr != 0x0);
    *addr = PCI_BOOT_SYNC_DATA;
}

void frcSetPciConfSyncLocation()
{
    unsigned int *addr = (unsigned int *)PCI_BOOT_CONF_ADRS;
    *addr = PCI_BOOT_CONF_DATA;
}

int frcReadPciBootSyncLocation()
{
    unsigned int *addr = (unsigned int *)PCI_BOOT_SYNC_ADRS;

    if((*addr) == PCI_BOOT_SYNC_DATA) return (1);
    else return (0);
}

int frcReadPciConfSyncLocation()
{
    unsigned int *addr = (unsigned int *)PCI_BOOT_CONF_ADRS;

    if((*addr) == PCI_BOOT_CONF_DATA) return (1);
    else return (0);
}

#else

#ifdef PMC280_BOOTROM_BOOT
/*
 * The Boot Rom Boot workaround involves programming the serial EEPROM
 * to copy the image from boot rom to SRAM (using DMA) and then making 
 * the internal sram as the boot device. The channel used for DMA is 
 * Channel 1, while register at offset 0x800 is used as a temporary 
 * register. To avoid a clash, use register at offset 0x808 instead.
 */
#define BOOT_SYNC_ADRS        0xF1000808
#define BOOT_CONF_ADRS        0xF1000808
#define BOOT_SYNC_DATA        (0xA5A6B5B6)
#define BOOT_CONF_DATA        (0xC5C6D5D6)

void frcInitBootSyncLocation()
{
    unsigned int *addr = (unsigned int *)BOOT_SYNC_ADRS;
    *addr = 0x0;
}

void frcSetBootSyncLocation()
{
    unsigned int *addr = (unsigned int *)BOOT_SYNC_ADRS;
    while((*addr) != 0x0);
    *addr = BOOT_SYNC_DATA;
}

void frcSetConfSyncLocation()
{
    unsigned int *addr = (unsigned int *)BOOT_CONF_ADRS;
    *addr = BOOT_CONF_DATA;
}

int frcReadBootSyncLocation()
{
    unsigned int *addr = (unsigned int *)BOOT_SYNC_ADRS;

    if((*addr) == BOOT_SYNC_DATA) return (1);
    else return (0);
}

int frcReadConfSyncLocation()
{
    unsigned int *addr = (unsigned int *)BOOT_CONF_ADRS;

    if((*addr) == BOOT_CONF_DATA) return (1);
    else return (0);
}
#else
#ifdef PMC280_USERFLASH_BOOT

/*Sergey: copy from previous fragment and change names;
 they maybe identical but we may need some changes ..*/
#define BOOT_SYNC_ADRS        0xF1000808
#define BOOT_SYNC_DATA        (0xA5A6B5B6)
void frcInitUserBootSyncLocation()
{
    unsigned int *addr = (unsigned int *)BOOT_SYNC_ADRS;
    *addr = 0x0;
}
void frcSetUserBootSyncLocation()
{
    unsigned int *addr = (unsigned int *)BOOT_SYNC_ADRS;
    while((*addr) != 0x0);
    *addr = BOOT_SYNC_DATA;
}
int frcReadUserBootSyncLocation()
{
    unsigned int *addr = (unsigned int *)BOOT_SYNC_ADRS;

    if((*addr) == BOOT_SYNC_DATA) return (1);
    else return (0);
}

#else
#error "PMC280_BOOTROM_BOOT or PMC280_PCI_BOOT or PMC280_USERFLASH_BOOT should be defined" 
#endif /* PMC280_USERFLASH_BOOT */
#endif /* PMC280_BOOTROM_BOOT */
#endif /* PMC280_PCI_BOOT */
#endif /* PMC280_DUAL_CPU */

/*******************************************************************************
*
* sysHwInit - initialize the system hardware
*
* This routine initializes various hardware features of the board.
* It is the first board-specific C code executed, and runs with
* interrupts masked in the processor.
* This routine resets all devices to a quiescent state, typically
* by calling driver initialization routines.
*
* NOTE: Because this routine may be called from sysToMonitor, it must
* shutdown all potential DMA master devices.  If a device is doing DMA
* at reboot time, the DMA could interfere with the reboot. For devices
* on non-local busses, this is easy if the bus reset or sysFail line can
* be asserted.
*
* NOTE: This routine should not be called directly by any user application.
*
* RETURNS: N/A
*/
void sysHwInit (void)
{

#ifdef PMC280_PCI_BOOT
    int temp;
#endif

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
        dbg_puts1("CPU1: Entering sysHwInit\n");
    else
        dbg_puts0("CPU0: Entering sysHwInit\n");
#else
    unsigned int val = 0xFF;
    dbg_printf0("Entering sysHwInit\n");
    GT_REG_READ(SMP_WHO_AM_I, &val);
    dbg_printf0("CPU Number = %d\n", val);
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Initialise Board ID stuff here */
    /* Enable TBEN */
    vxHid0Set (vxHid0Get () | 0x04000000);
    frcBoardIdInit();

    /* Init various GT parameters */
    gtInit();

#ifdef LOCAL_MEM_AUTOSIZE
    /* SDRAM auto configueration */
    sdramSize = gtInitSdramSize();
#endif  

#ifdef PMC280_DUAL_CPU
#if defined(INCLUDE_MMU)
    sysPhysMemDesc0[0].len = LOCAL_MEM_SIZE - USER_RESERVED_MEM;
    sysPhysMemDesc0[1].virtualAddr = 
            (void *) VIR_CPU0_USER_RESERVED_MEM_BOT;
    sysPhysMemDesc0[1].physicalAddr = 
            (void *) PHY_CPU0_USER_RESERVED_MEM_BOT;

    sysPhysMemDesc1[0].physicalAddr = 
            (void *) PHY_CPU1_MEM_BOT;
    sysPhysMemDesc1[0].len = LOCAL_MEM_SIZE - USER_RESERVED_MEM;
    sysPhysMemDesc1[1].virtualAddr = (void *)VIR_CPU1_USER_RESERVED_MEM_BOT;
    sysPhysMemDesc1[1].physicalAddr = (void *)PHY_CPU1_USER_RESERVED_MEM_BOT;

    {
        PHYS_MEM_DESC * pMmu;
        int ix;

        /* calculate the number of sysPhysMemDesc entries in use */
        pMmu = &sysPhysMemDesc0[0];

        for(ix = 0; ix < NELEMENTS (sysPhysMemDesc0); ix++)
        {
            if(pMmu->virtualAddr != (void *)DUMMY_VIRT_ADDR)
                pMmu++;     /* used entry */
            else
                break;  /* dummy entry, assume no more */
        }

        /* This is the REAL number of sysPhysMemDesc entries in use. */
        sysPhysMemDescNumEnt0 = ix;

        /* calculate the number of sysPhysMemDesc entries in use */
        pMmu = &sysPhysMemDesc1[0];

        for(ix = 0; ix < NELEMENTS (sysPhysMemDesc1); ix++)
        {
            if(pMmu->virtualAddr != (void *)DUMMY_VIRT_ADDR)
                pMmu++;     /* used entry */
            else
                break;  /* dummy entry, assume no more */
        }

        /* This is the REAL number of sysPhysMemDesc entries in use. */
        sysPhysMemDescNumEnt1 = ix;
    }
#endif /* INCLUDE_MMU */

#else /* SINGLE CPU */

#if defined(INCLUDE_MMU)
    {
        PHYS_MEM_DESC * pMmu;
        int ix;

        /* 
        * run-time update of the sysPhysMemDesc MMU entry for main RAM 
        * This means SDRAM _must_ be represented by sysPhysMemDesc[1]
        */
        sysPhysMemDesc[1].len = (UINT)(sysMemTop () - RAM_LOW_ADRS);

        /* 
        * run-time update of the sysPhysMemDesc MMU entry for Communication  
        * Unit Drivers that uses the User Reserved Memory. This means that 
        * the user reserved memory _must_ be represented by sysPhysMemDesc[2]
        */

        sysPhysMemDesc[2].virtualAddr  = (void*)sysMemTop ();
        sysPhysMemDesc[2].physicalAddr = (void*)sysMemTop ();
        sysPhysMemDesc[2].len = USER_RESERVED_MEM;

        /* calculate the number of sysPhysMemDesc entries in use */

        pMmu = &sysPhysMemDesc[0];

        for(ix = 0; ix < NELEMENTS (sysPhysMemDesc); ix++)
        {
            if(pMmu->virtualAddr != (void *)DUMMY_VIRT_ADDR)
                pMmu++;     /* used entry */
            else
                break;  /* dummy entry, assume no more */
        }

        /* This is the REAL number of sysPhysMemDesc entries in use. */

        sysPhysMemDescNumEnt = ix;
    }
#endif /* INCLUDE_MMU */
#endif /* PMC280_DUAL_CPU */

    /* Validate the CPU type used */
    sysCpuCheck();

#ifdef INCLUDE_SERIAL
    sysSerialHwInit();      /* init COM1 & COM2 serial devices     */
#endif


#if defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_L2_CACHE)
	
    sysL2BackInit();
    
#if (L2CACHE_MODE == L2CACHE_MODE_WRITETHROUGH)    
    sysL2BackSetWT();
#endif /* L2CACHE_MODE_WRITETHROUGH */

    _pSysL2CacheInvFunc = sysL2BackGlobalInv;
    _pSysL2CacheEnable  = sysL2BackEnable;
    _pSysL2CacheDisable = sysL2BackDisable;

#ifdef INCLUDE_SNOOP_SUPPORT
dfkg
    snoopEnable = TRUE;
#else
    snoopEnable = FALSE;
#endif
	
    if((CPU_TYPE == CPU_TYPE_7400) || (CPU_TYPE == CPU_TYPE_7410) ||
       (CPU_TYPE == CPU_TYPE_7450) || (CPU_TYPE == CPU_TYPE_7455) || 
       (CPU_TYPE == CPU_TYPE_7447) || (CPU_TYPE == CPU_TYPE_7447A))
        _pSysL2CacheFlush = sysL2BackHWFlush;
	
#if (L2CACHE_MODE == L2CACHE_MODE_COPYBACK)        
#else
salkdf
    _pSysL2CacheFlush = sysL2BackSWFlush;
#endif /* L2CACHE_MODE_COPYBACK */    
#endif /* INCLUDE_CACHE_SUPPORT && INCLUDE_L2_CACHE */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
        dbg_puts1("CPU1: Exiting sysHwInit\n");
    else
        dbg_puts0("CPU0: Exiting sysHwInit\n");
#else
    dbg_puts0("Exiting sysHwInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
    /*
     * This is used for VIRTUAL_TO_PHY macro.
     */
    if(frcWhoAmI())
    {
        frcVirtToPhyCPU1memBase = PHY_CPU1_MEM_BOT;
    }
    else
    {
        frcVirtToPhyCPU1memBase = PHY_CPU0_MEM_BOT;
    }
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DUAL_CPU
#ifdef PMC280_PCI_BOOT
    /*
     * CPU1 will read PCI configuration information setup by
     * CPU0 in all PCI devices. This means that CPU1 will need to wait
     * till CPU0 sets up all devices appropriately.
     */
    if(frcWhoAmI())
    {
#ifdef PMC280_DEBUG_UART_VX
        dbg_printf1("CPU1: Before PCI Config Wait loop\n");
#endif 
        while(!frcReadPciConfSyncLocation())
        {
#ifdef PMC280_DEBUG_UART_VX
            dbg_printf1("CPU1: waiting...\n");
#endif 
        }
    }
#endif /* PMC280_PCI_BOOT */
#endif /* PMC280_DUAL_CPU */


#ifdef PMC280_PCI_BOOT
    /*
     * For PCI boot to work with ECC enabled, it was necessary to turn off
     * read combining in the serial EEPROM. Since this feature is necessary
     * for good performance, turn it back on *here*. Note that the place
     * where read combining is turned on should be anywhere after both
     * CPUs have started to run from DRAM (and stopped code fetches from PCI)
     */
#ifdef PMC280_DUAL_CPU
    if(!frcWhoAmI())
#endif
    {
        GT_REG_READ(PCI_0_COMMAND, &temp);
        temp |= 0x00000020;  /* Bit [5] */
        GT_REG_WRITE(PCI_0_COMMAND, temp);
    }
#endif

}

/*******************************************************************************
* sysHwInit2 - initialize additional system hardware
*
* This routine connects system interrupt vectors and configures any
* required features not configured by sysHwInit. It is called from usrRoot()
* in usrConfig.c (via sysClkConnect) after the multitasking kernel has started.
*
* RETURNS: N/A
*/
void sysHwInit2 (void)
{
    static bool initialized;         /* Do this ONCE */
    UINT32 temp;   

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering sysHwInit2\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering sysHwInit2\n");
    }
#else
    dbg_puts0("Entering sysHwInit2...\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    if(!initialized )
    {
        initialized = true;

        /* Initialize GT system Interrupt controller first in order to be    */
        /* able to handle the sub units various interrupts (i.e. GPP port)   */
        gtIntCtrlInit();

        /* Initiate the GPP interrupt controller in order to be able to hanle*/
        /* the system device interrupts (i.e. UART)                          */
        sysIntCtrlInit(); 

#ifdef INCLUDE_SERIAL
        sysSerialHwInit2 ();    /* connect serial interrupts */
#endif

#ifdef INCLUDE_FLASH
        if(frcGetUserFlashSize() != FLASH_SIZE_0MB)
        {
#ifdef INCLUDE_FLASH_BOOTLINE
            flashBootSectorInit();
#endif 
        }
#endif /* INCLUDE_FLASH */
    
#ifdef INCLUDE_ALTIVEC
        _func_altivecProbeRtn = sysAltivecProbe;
#endif  /* INCLUDE_ALTIVEC */
    }

#ifdef INCLUDE_PCI    
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering pciScan\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering pciScan\n");
    }
#else
    dbg_puts0("Entering pciScan\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
    if(!frcWhoAmI())
        pciScan(0);
    else
        frcPciSetActive(0);

#else
    pciScan(0);
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DUAL_CPU
#ifdef PMC280_PCI_BOOT
    /*
     *  CPU0 has run through PCI initialisation (setting). 
     *  Let CPU1 continue here.
     */
    if(!frcWhoAmI())
    {
        frcSetPciConfSyncLocation();
    }
#endif /* PMC280_PCI_BOOT */
#endif /* PMC280_DUAL_CPU */

    /*frcVPDInit()*/

    GT_REG_READ(GPP_VALUE, &temp);

    /* Monarch mode:Initialize the PCI drivers*/
    if ((temp & 0x00000010) == 0x00000000) 
    {
#ifdef PMC280_DUAL_CPU
        /* Intialize the drivers only for CPU0 */
        if(!frcWhoAmI())
        {      
#ifdef INCLUDE_END_FEI
            sys557PciInit();
#endif

#ifdef INCLUDE_GEI_82543_END
            sys543PciInit();
#endif
        }  
#else /* PMC280_DUAL_CPU */                       /* Single CPU Support */

#ifdef INCLUDE_END_FEI
        sys557PciInit();
#endif

#ifdef INCLUDE_GEI_82543_END
        sys543PciInit();
#endif

#endif /* PMC280_DUAL_CPU */ 
    }                                  

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting pciScan\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting pciScan\n");
    }
#else
    dbg_puts0("Exiting pciScan\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
#endif /* INCLUDE_PCI */               

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting sysHwInit2\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting sysHwInit2\n");
    }
#else
    dbg_puts0("Exiting sysHwInit2...\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

}


void sysOutWord (ULONG address, UINT16 data)
{
   *(UINT16 *)address = BYTE_SWAP_16(data);
}
void sysOutLong (ULONG address, ULONG data)
{
   *(UINT32 *)address = BYTE_SWAP_32(data);
}
UINT16 sysInWord (ULONG address)
{
   return (BYTE_SWAP_16(*(UINT16 *)address));
}
ULONG sysInLong (ULONG address)
{
   return (BYTE_SWAP_32(*(UINT32 *)address));
}


/*******************************************************************************
* sysPhysMemTop - get the address of the top of physical memory
*
* This routine returns the address of the first missing byte of memory,
* which indicates the top of memory.
*
* Normally, the user specifies the amount of physical memory with the
* macro LOCAL_MEM_SIZE in config.h.  BSPs that support run-time
* memory sizing do so only if the macro LOCAL_MEM_AUTOSIZE is defined.
* If not defined, then LOCAL_MEM_SIZE is assumed to be, and must be, the
* true size of physical memory.
*
* NOTE: Do no adjust LOCAL_MEM_SIZE to reserve memory for application
* use.  See sysMemTop() for more information on reserving memory.
*
* RETURNS: The address of the top of physical memory.
*
* SEE ALSO: sysMemTop()
*/
char *sysPhysMemTop (void)
{
    return(char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);
}

/*******************************************************************************
* sysMemTop - get the address of the top of VxWorks memory
*
* This routine returns a pointer to the first byte of memory not
* controlled or used by VxWorks.
*
* The user can reserve memory space by defining the macro USER_RESERVED_MEM
* in config.h.  This routine returns the address of the reserved memory
* area.  The value of USER_RESERVED_MEM is in bytes.
*
* RETURNS: The address of the top of VxWorks memory.
*/
char * sysMemTop (void)
{
    static char * memTop = NULL;

    if(memTop == NULL)
    {
        memTop = sysPhysMemTop () - USER_RESERVED_MEM;
    }

    return memTop;
}

/*******************************************************************************
* sysToMonitor - transfer control to the ROM monitor
*
* This routine transfers control to the ROM monitor.  Normally, it is called
* only by reboot()--which services ^X--and by bus errors at interrupt level.
* However, in some circumstances, the user may wish to introduce a
* <startType> to enable special boot ROM facilities.
*
* The entry point for a warm boot is defined by the macro ROM_WARM_ADRS
* in config.h.  We do an absolute jump to this address to enter the
* ROM code.
*
* RETURNS: Does not return.
*/
STATUS sysToMonitor
(
    int startType    /* parameter passed to ROM to tell it how to boot */
)
{
    sysHardReset();
    return(OK);                /* In case we continue from ROM monitor */
}

/******************************************************************************
* sysProcNumGet - get the processor number
*
* This routine returns the processor number for the CPU board, which is
* set with sysProcNumSet().
*
* RETURNS: The processor number for the CPU board.
*
* SEE ALSO: sysProcNumSet()
*/
int sysProcNumGet (void)
{
#ifdef PMC280_DUAL_CPU
    return (frcWhoAmI());
#else
    return(sysProcNum);
#endif
}

/******************************************************************************
* sysProcNumSet - set the processor number
*
* This routine sets the processor number for the CPU board.  Processor numbers
* should be unique on a single backplane.
*
* For bus systems, it is assumes that processor 0 is the bus master and
* exports its memory to the bus.
*
* RETURNS: N/A
*
* SEE ALSO: sysProcNumGet()
*/
void sysProcNumSet
(
    int procNum         /* processor number */
)
{
    sysProcNum = procNum;

    if(procNum == 0)
    {
    }
}

/*******************************************************************************
* sysCpuCheck - confirm the CPU type
*
* This routine validates the cpu type.  If the wrong cpu type is discovered
* a message is printed using the serial channel in polled mode.
*
* RETURNS: N/A.
*/
void sysCpuCheck (void)
{
    int msgSize;
    int msgIx;
    SIO_CHAN * pSioChan;        /* serial I/O channel */

    /* Check for a valid CPU type;  If one is found, just return */

#if (CPU == PPC603)    /* three kinds of 603 */
    if((CPU_TYPE == CPU_TYPE_603)  || 
       (CPU_TYPE == CPU_TYPE_603E) ||
       (CPU_TYPE == CPU_TYPE_603P))
    {
        return; 
    }

#elif (CPU == PPC604) /* seven kinds of 604 */
    if((CPU_TYPE == CPU_TYPE_604)  || 
       (CPU_TYPE == CPU_TYPE_604E) ||
       (CPU_TYPE == CPU_TYPE_604R) ||
       (CPU_TYPE == CPU_TYPE_750)  ||
       (CPU_TYPE == CPU_TYPE_750FX)||
       (CPU_TYPE == CPU_TYPE_7400) ||
       (CPU_TYPE == CPU_TYPE_7410) ||
       (CPU_TYPE == CPU_TYPE_7450) ||
       (CPU_TYPE == CPU_TYPE_7447) ||
       (CPU_TYPE == CPU_TYPE_7447A) ||
       (CPU_TYPE == CPU_TYPE_7455))
    {
        return; 
    }
#endif    /* not a valid CPU */

    /* Invalid CPU type; polling print of error message and bail */

    msgSize = strlen (wrongCpuMsg);

    sysSerialHwInit ();

    pSioChan = sysSerialChanGet (0);

    sioIoctl (pSioChan, SIO_MODE_SET, (void *) SIO_MODE_POLL);

    for(msgIx = 0; msgIx < msgSize; msgIx++)
    {
        while(sioPollOutput (pSioChan, wrongCpuMsg[msgIx]) == EAGAIN);
    }

    sysToMonitor (BOOT_NO_AUTOBOOT);
}


/******************************************************************************
* sysDelay - delay for approximately one millisecond
*
* Delay for approximately one milli-second.
*
* RETURNS: N/A
*/
void sysDelay (void)
{
    sysMsDelay (1);
}

/******************************************************************************
* sysMsDelay - delay for the specified amount of time (MS)
*
* This routine will delay for the specified amount of time by counting
* decrementer ticks.
*
* This routine is not dependent on a particular rollover value for
* the decrementer, it should work no matter what the rollover
* value is.
*
* A small amount of count may be lost at the rollover point resulting in
* the sysMsDelay() causing a slightly longer delay than requested.
*
* This routine will produce incorrect results if the delay time requested
* requires a count larger than 0xffffffff to hold the decrementer
* elapsed tick count.  For a System Bus Speed of 67 MHZ this amounts to
* about 258 seconds.
*
* RETURNS: N/A
*/
void sysMsDelay
(
    UINT        delay                   /* length of time in MS to delay */
)
{
    register UINT oldval;               /* decrementer value */
    register UINT newval;               /* decrementer value */
    register UINT totalDelta;           /* Dec. delta for entire delay period */
    register UINT decElapsed;           /* cumulative decrementer ticks */

    /*
     * Calculate delta of decrementer ticks for desired elapsed time.
     * The macro DEC_CLOCK_FREQ MUST REFLECT THE PROPER 6xx BUS SPEED.
     */

    totalDelta = ((DEC_CLOCK_FREQ / 4) / 1000) * delay;

    /*
     * Now keep grabbing decrementer value and incrementing "decElapsed" until
     * we hit the desired delay value.  Compensate for the fact that we may
     * read the decrementer at 0xffffffff before the interrupt service
     * routine has a chance to set in the rollover value.
     */

    decElapsed = 0;

    oldval = vxDecGet ();

    while(decElapsed < totalDelta)
    {
        newval = vxDecGet ();

        if(DELTA(oldval,newval) < 1000)
            decElapsed += DELTA(oldval,newval);  /* no rollover */
        else
            if(newval > oldval)
            decElapsed += abs((int)oldval);  /* rollover */

        oldval = newval;
    }
}

/******************************************************************************
* sysNvRamGet - get the contents of non-volatile RAM
*
* This routine copies the contents of non-volatile memory into a specified
* string.  The string will be terminated with an EOS.
*
* RETURNS: OK, or ERROR if access is outside the non-volatile RAM range.
*
* SEE ALSO: sysNvRamSet()
*/
STATUS sysNvRamGet
(
    char *string,    /* where to copy non-volatile RAM           */
    int  strLen,     /* maximum number of bytes to copy          */
    int  offset      /* byte offset into non-volatile RAM        */
)
{
#ifdef INCLUDE_FLASH_BOOTLINE

    if(frcGetUserFlashSize() == FLASH_SIZE_0MB)
        return(ERROR);
    /* Parameter checking */
    if(offset + NV_BOOT_OFFSET < 0)
        return ERROR;

    frcFlashReadRst(FLASH_BANK0);

    frcFlashRead((FLASH_BANK0+offset+NV_BOOT_OFFSET), (strLen/4)+1, (UINT32 *)string);
    if((*BOOT_LINE_ADRS == EOS))
        return(ERROR);
    else
        return OK;
#else /* INCLUDE_FLASH_BOOTLINE */
    return ERROR;
#endif /* INCLUDE_FLASH_BOOTLINE */
}

/*******************************************************************************
* sysNvRamSet - write to non-volatile RAM
*
* This routine copies a specified string into non-volatile RAM.
*
* RETURNS: OK, or ERROR if access is outside the non-volatile RAM range.
*
* SEE ALSO: sysNvRamGet()
*/
STATUS sysNvRamSet
(
    char *string,     /* string to be copied into non-volatile RAM */
    int strLen,       /* maximum number of bytes to copy           */
    int offset        /* byte offset into non-volatile RAM         */
)
{
#ifdef INCLUDE_FLASH_BOOTLINE

    /* Parameter checking */
    if(offset + NV_BOOT_OFFSET < 0)
        return ERROR;

    if(frcFlashUnlock( (unsigned int *)(FLASH_BANK0+offset+NV_BOOT_OFFSET) ) != 1 )
        return ERROR;

    frcFlashReadRst( FLASH_BANK0 );

    if(frcFlashBlockErase((offset + NV_BOOT_OFFSET)/0x40000, FLASH_BANK0 ) != 0 )
        return ERROR;

    frcFlashReadRst( FLASH_BANK0 );

    if(frcFlashProgram((unsigned *)(FLASH_BANK0+offset+NV_BOOT_OFFSET), (unsigned *)string, 
                       (strLen+0x3) & 0xfffffffc, FLASH_BANK0 ) != 0 )
        return ERROR;
    if(frcFlashProgram((unsigned *)(FLASH_BANK0+offset+NV_BOOT_OFFSET), (unsigned *)string, 
                       (strLen+0x3) & 0xfffffffc, FLASH_BANK0 ) != 0 )
        return ERROR;

    return(OK);
#else /* INCLUDE_FLASH_BOOTLINE */
    return ERROR;
#endif /* INCLUDE_FLASH_BOOTLINE */
}

#ifdef INCLUDE_MMU
/*******************************************************************************
* sysMmuMapAdd - insert a new mmu mapping
*
* This routine adds a new mmu mapping entry to allow dynamic mappings. 
*
* RETURNS: OK or ERROR depending on availability of free mappings.
*/  
STATUS sysMmuMapAdd 
(
    void *address,
    UINT length,
    UINT initialStateMask,
    UINT initialState
)
{
    PHYS_MEM_DESC *pMmu;
    STATUS result = OK;

#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        pMmu = &sysPhysMemDesc1[sysPhysMemDescNumEnt1];
    }
    else
    {
        pMmu = &sysPhysMemDesc0[sysPhysMemDescNumEnt0];
    }
#else
    pMmu = &sysPhysMemDesc[sysPhysMemDescNumEnt];
#endif

    if(pMmu->virtualAddr != (void *)DUMMY_VIRT_ADDR)
        result = ERROR;
    else
    {
        pMmu->virtualAddr = address;
        pMmu->physicalAddr = address;
        pMmu->len = length;
        pMmu->initialStateMask = initialStateMask;
        pMmu->initialState = initialState;
#ifdef PMC280_DUAL_CPU
        if(frcWhoAmI())
        {
            sysPhysMemDescNumEnt1++;
        }
        else
        {
            sysPhysMemDescNumEnt0++;
        }
#else
        sysPhysMemDescNumEnt++;
#endif /* PMC280_DUAL_CPU */
    }

    return(result);
}

void sysMmuPteShow(void)
{
    int i;

#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        for(i=0; i<sysPhysMemDescNumEnt1; i++)
        {
          printf("\nVirtual Address:    0x%x",(UINT)sysPhysMemDesc1[i].virtualAddr); 
          printf("\nPhysical address:   0x%x",(UINT)sysPhysMemDesc1[i].physicalAddr);
          printf("\nSegment size:       0x%x",sysPhysMemDesc1[i].len);
          printf("\nInitial State Mask: 0x%x",sysPhysMemDesc1[i].initialStateMask);
          printf("\nInitial State:      0x%x",sysPhysMemDesc1[i].initialState);
          printf("\n=====================================");
        }
    }
    else
    {
        for(i=0; i<sysPhysMemDescNumEnt0; i++)
        {
          printf("\nVirtual Address:    0x%x",(UINT)sysPhysMemDesc0[i].virtualAddr); 
          printf("\nPhysical address:   0x%x",(UINT)sysPhysMemDesc0[i].physicalAddr);
          printf("\nSegment size:       0x%x",sysPhysMemDesc0[i].len);
          printf("\nInitial State Mask: 0x%x",sysPhysMemDesc0[i].initialStateMask);
          printf("\nInitial State:      0x%x",sysPhysMemDesc0[i].initialState);
          printf("\n=====================================");
        }
    }
#else
    for(i=0; i<sysPhysMemDescNumEnt; i++)
    {
      printf("\nVirtual Address:    0x%x",(UINT)sysPhysMemDesc[i].virtualAddr); 
      printf("\nPhysical address:   0x%x",(UINT)sysPhysMemDesc[i].physicalAddr);
      printf("\nSigment size:       0x%x",sysPhysMemDesc[i].len);
      printf("\nInitial State Mask: 0x%x",sysPhysMemDesc[i].initialStateMask);
      printf("\nInitial State:      0x%x",sysPhysMemDesc[i].initialState);
      printf("\n=====================================");
    }
#endif /* PMC280_DUAL_CPU */
}
#endif /* INCLUDE_MMU */

/*******************************************************************************
* sysVirtToPhys - Translate MMU virtual address to physical address
*
* DESCRIPTION:
*        This function translates virtual address to physical.
*
* INPUT:
*        unsigned int addr    input address.
*
* OUTPUT:
*        None.
*
* RETURN:
*        Translated address.
*
*******************************************************************************/
unsigned int  sysVirtToPhys(unsigned int addr)
{
    return addr;
}

/*******************************************************************************
* sysPhysToVirt - Translate MMU physical address to virtual address
*
* DESCRIPTION:
*        This function translates physical address to virtual.
*
* INPUT:
*        unsigned int addr    input address.
*
* OUTPUT:
*        None.
*
* RETURN:
*        Translated address.
*
*******************************************************************************/
unsigned int sysPhysToVirt(unsigned int addr)
{
    return addr;
}
/******************************************************************************
 * GT register manipulating functions implementation                          *
 ******************************************************************************/

/*******************************************************************************
* gtRegReadBlock(offset, *block, length) 
* 
* DESCRIPTION:
*       This function reads a given sequential registers content.
* 
* INPUT:
*       offset  - Block offset in GT internal register address space.
*       *block  - Pointer to a buffer.
*       length  - Length of the block to read.
*
* OUTPUT: 
*       Buffer block is assigned with the sequential register content.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtRegReadBlock(unsigned int offset, unsigned int* block,
                    unsigned int length)
{
    int i;

    for(i = 0 ; i < (length/sizeof(unsigned int)) ; i++)
    {
        GT_REG_READ (offset+i*sizeof(unsigned int),(unsigned int*)(block+i));
    }
}

/*****************************************************************************
* gtRegWriteBlock(offset, *block, length)  
* 
* DESCRIPTION:
*       This function writes a block of data into a given sequential register. 
*
* INPUT:
*       offset  - Block offset in GT internal register address space.
*       *block  - Pointer to the block of data.
*       length  - Length of the block.
*
* OUTPUT: 
*       Sequential register are assign with the content of  block buffer.
*
* RETURN:
*       None.
*
*******************************************************************************/
void gtRegWriteBlock(unsigned int offset, unsigned int* block,
                     unsigned int length) 
{
    int i;

    for(i = 0 ; i < length/sizeof(unsigned int) ; i++)
    {
        GT_REG_WRITE (offset+i*sizeof(unsigned int),block[i]);
    }
}

/******************************************************************************
* memoryRead(memOffset, *data, memSize) 
* 
* DESCRIPTION
*       Read a block of data from a memory location.
*
* INPUT:
*       memOffset - the "virtual" memory offset.
*       *data     - the buffer for the output
*       memSize   - how many data to write in a 32 bit units.
*
* OUTPUT:
*       *data     - a buffer that will receive the data.
*
* RETURN:
*       None.
*
*******************************************************************************/
void memoryRead(unsigned int memOffset,unsigned int *data,unsigned int memSize)
{
    int i;

    for(i = 0 ; i < memSize/sizeof(unsigned int) ; i++)
    {
        *(data + i) = *((unsigned int *)
                        (NONE_CACHEABLE(memOffset + i*sizeof(unsigned int))));
    }
}
/******************************************************************************
 *                          U T I L I T Y S                                   *
******************************************************************************/


/******************************************************************************
* showMap - Show memory map
*
* DESCRIPTION:
*       Show memory map for DDR CS's , devices CS and PCI 0/1 IO/MEM 
*       Show base address and sizes
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
******************************************************************************/
void showMap(void)
{
    int i;
    UINT32 temp;

    temp = sysInLong(0xf1000278); /* Base Address Enable Register  */
    

    printf("\nMemory Bank  | Base Addr  | Size       |\n");
    printf("========================================\n");
    for(i=BANK0 ; i<= BANK3;i++)
    {                                   
        if (!(temp&0x1))    
           printf("Bank %d       | 0x%08x | 0x%08x |\n",i, 
               gtMemoryGetBankBaseAddress(i),gtMemoryGetBankSize(i));
        else
           printf("Bank %d  Not Mapped\n",i);  
    
        temp = temp >> 1; 
    }

    printf("\nDevice       | Base Addr  | Size       | Width  |\n");
    printf("=================================================\n");
    for(i=DEVICE0 ; i < BOOT_DEVICE;i++)
    {
                
       if (!(temp&0x1)) 
          printf("Device %d     | 0x%08x | 0x%08x | %2d bit |\n",i, 
               gtMemoryGetDeviceBaseAddress(i),gtMemoryGetDeviceSize(i),
               gtMemoryGetDeviceWidth(i)*8);
       else
          printf("Device %d Not Mapped\n",i);
        
       temp = temp>>1; 
    }
    
    if (!(temp&0x1)) 
       printf("Boot Device  | 0x%08x | 0x%08x | %2d bit |\n", 
             gtMemoryGetDeviceBaseAddress(BOOT_DEVICE),
             gtMemoryGetDeviceSize(BOOT_DEVICE),
             gtMemoryGetDeviceWidth(BOOT_DEVICE)*8);
    else
       printf("Boot Device Not mapped\n");   

    temp = temp >> 1;

    printf("\nPCI 0        | Base Addr  | Size       |\n");
    printf("========================================\n");
    
    if (!(temp&0x1))
        printf("IO space     | 0x%08x | 0x%08x |\n",
             gtPci0GetIOspaceBase(),gtPci0GetIOspaceSize());
    else
        printf("IO space for PCI0 Not Mapped\n");
   
    temp = temp >> 1;
   
    if (!(temp&0x1))
        printf("Mem 0 space  | 0x%08x | 0x%08x |\n",
              gtPci0GetMemory0Base(),gtPci0GetMemory0Size());
    else
        printf("Mem 0 space for PCI0 Not Mapped\n");

    temp = temp >> 1;
  
    if (!(temp&0x1))  
         printf("Mem 1 space  | 0x%08x | 0x%08x |\n",
               gtPci0GetMemory1Base(),gtPci0GetMemory1Size());
    else
         printf("Mem 1 space for PCI0 Not Mapped\n");

    temp = temp >> 1;

    if (!(temp&0x1)) 
         printf("Mem 2 space  | 0x%08x | 0x%08x |\n",
               gtPci0GetMemory2Base(),gtPci0GetMemory2Size());
    else
         printf("Mem 2 space for PCI0 Not Mapped\n");

    temp = temp >> 1;

    if (!(temp&0x1)) 
         printf("Mem 3 space  | 0x%08x | 0x%08x |\n",
               gtPci0GetMemory3Base(),gtPci0GetMemory3Size());
    else
         printf("Mem 3 space for PCI0 Not Mapped\n");
 
    temp = temp >> 1;
                                 
    printf("\nPCI 1        | Base Addr  | Size       |\n");  
    printf("========================================\n");
    
    if (!(temp&0x1)) 
          printf("IO space PCI1 not used\n");
    else
          printf("IO space for PCI1 Not Mapped\n");

    temp = temp >> 1;

    if (!(temp&0x1)) 
          printf("Mem0 space for PCI1 not used\n");
    else
          printf("Mem0 space for PCI1 Not Mapped\n");

    temp = temp >> 1;

    if (!(temp&0x1)) 
          printf("Mem1 space for PCI1 not used\n");
    else
          printf("Mem1 space for PCI1 Not Mapped\n");

    temp = temp >> 1;

    if (!(temp&0x1)) 
          printf("Mem2 space for PCI1 not used\n"); 
    else
          printf("Mem2 space for PCI1 Not Mapped\n"); 

    temp = temp >> 1;

    if (!(temp&0x1)) 
          printf("Mem3 space for PCI1 not used\n"); 
    else
          printf("Mem3 space for PCI1 Not Mapped\n"); 

    temp = temp >> 1;
  
    if (!(temp&0x1))  
          printf("\nInternal Reg | 0x%08x | 0x%08x |\n", 
               gtMemoryGetInternalRegistersSpace(),INTERNAL_REG_SIZE);
    else
          printf("\nInternal Reg Space Not supported\n"); 
       
    temp = temp >> 1;

    if (!(temp&0x1))
          printf("\nInternal SRAM| 0x%08x | 0x%08x |\n", 
              gtMemoryGetInternalSramBaseAddr(),_256K); /* see gtSram.h */
    else
          printf("\nInternal SRAM Space Not Supported\n"); 

#ifdef PMC280_DUAL_CPU
    printf("\nSDRAM Partition Map for CPU-%d \n", frcWhoAmI());
    printf("=====================================\n");
    printf("Base Addr   | End Addr     | Purpose\n");          
    printf("0x%08x  | 0x%08x   | IVT\n", 0x0, 0x3000 - 1);
    printf("0x%08x  | 0x%08x   | Free\n", 0x3000, 0x10000 - 1);
    printf("0x%08x  | 0x%08x   | Malloc\n",
          (unsigned) frc_end_of_vxworks, ((unsigned)sysMemTop() - 1)); 
    printf("0x%08x  | 0x%08x   | Shmem-Drv\n",
          (unsigned) PHY_CPU0_SYS_SHMEM_BOT, 
          (unsigned) PHY_CPU0_SYS_SHMEM_BOT + SYS_DRV_SIZE - 1);

    printf("0x%08x  | 0x%08x   | Shmem-Pte\n",
          (unsigned) PHY_CPU0_SYS_SHMEM_BOT + SYS_DRV_SIZE, 
          (unsigned) PHY_CPU0_SYS_SHMEM_BOT + SYS_DRV_SIZE + SYS_PGT_SIZE - 1);

    printf("0x%08x  | 0x%08x   | Shmem-App\n",
          (unsigned) PHY_CPU0_APP_SHMEM_BOT, 
          (unsigned) PHY_CPU0_APP_SHMEM_BOT + APP_SHMEM_SIZE - 1);
#else             

    printf("\nSDRAM Memory Map for CPU-0\n");
    printf("========================================\n");
    printf("Base Addr   | End Addr     | Purpose\n");          
    printf("0x%08x  | 0x%08x   | IVT\n", 0x0, 0x3000 - 1);
    printf("0x%08x  | 0x%08x   | Free\n", 0x3000, 0x10000 - 1);
    printf("0x%08x  | 0x%08x   | Malloc\n",
          (unsigned) frc_end_of_vxworks, ((unsigned)sysMemTop() - 1)); 
    printf("0x%08x  | 0x%08x   | User reserved\n",
          (unsigned) sysMemTop(), 
          (unsigned) sysMemTop() + USER_RESERVED_MEM - 1);
#endif 
 }

#define DEBUG_WAN_CTRL
#ifdef DEBUG_WAN_CTRL
/* just for debug */

bool rdreg (unsigned int offset)
{
    unsigned int Value;
    GT_REG_READ(offset,&Value);

    printf("I-Register # 0x%x has value of 0x%x\n",offset,Value);

    return(true);
}


bool wrreg (unsigned int offset,unsigned int value)
{

    GT_REG_WRITE(offset,value);
    printf("I-Write to reg#0x%x value of 0x%x\n",offset,value);
    return true;
}

void memRead(unsigned int offset,unsigned int size)
{
    unsigned int tempBuffer[100];
    int i;

    memoryRead(offset,tempBuffer,size);
    for(i = 0 ; i < size ; i++)
        printf("%x\n",tempBuffer[i]);

}

void readreg(unsigned int offset)
{
    unsigned int data;

    data = GTREGREAD(offset);
    printf("I-Register # 0x%x has value of 0x%x\n",offset,data);
}

#endif /* DEBUG_WAN_CTRL */

#ifdef PMC280_DUAL_CPU
/*
 * Assumes the MPP[bit] has been configured as GPP.
 */
void dbg_gpp_init(unsigned int bit)
{
    unsigned int data;

    /* Make GPP[bit] as an output pin */
    GT_REG_READ(GPP_IO_CONTROL, &data);
    data |= (0x1 << bit);
    GT_REG_WRITE(GPP_IO_CONTROL, data);
}

void dbg_gpp_on(unsigned int bit)
{
    unsigned int data;
    GT_REG_READ(GPP_VALUE, &data);
    data |= (0x1 << bit);
    GT_REG_WRITE(GPP_VALUE, data);
}

void dbg_gpp_off(unsigned int bit)
{
    unsigned int data;
    GT_REG_READ(GPP_VALUE, &data);
    data &= ((0xFFFFFFFF << (bit + 1)) | (0xFFFFFFFF >> (32 - bit)));
    GT_REG_WRITE(GPP_VALUE, data);
}

void dbg_gpp_blink(unsigned int bit)
{
    unsigned int i;

    dbg_gpp_on(bit);
    for(i = 0; i < 0x1000; i++);
    dbg_gpp_off(bit);
}
#endif /* PMC280_DUAL_CPU */

/*
 * This causes PMC280 to reboot via a cold reset. This will cause 
 * SysReset to be asserted, which results in a complete board reset.
 */
void sysHardReset()
{
    unsigned int temp; 
  
    /* Make MPP[19] as GPP */ 
    GT_REG_READ(MPP_CONTROL2, &temp);
    temp &= 0xFFFF0FFF;
    GT_REG_WRITE(MPP_CONTROL2, temp);

    /* Make GPP[19] as GPO */
    GT_REG_READ(GPP_IO_CONTROL, &temp);
    temp |= 0x00080000;
    GT_REG_WRITE(GPP_IO_CONTROL, temp);

    /* Make GPP[19] as an active high signal */
    GT_REG_READ(GPP_LEVEL_CONTROL, &temp);
    temp &= 0xFFF7FFFF;
    GT_REG_WRITE(GPP_LEVEL_CONTROL, temp);

    /* Clear GPP[19] */
    GT_REG_WRITE(GPP_VALUE, 0x0);

    return;
}
void pciIsr () {
    sysOutLong(0xf100f100, (sysInLong(0xf100f100) | 0x04800000));
    sysOutLong(0xf100f104, (sysInLong(0xf100f104) & 0xfbffffff));
}


/*****************************************************************************/
/*****************************************************************************/
/******** Sergey: some functions for backward compartibility *****************/
/*****************************************************************************/
/*****************************************************************************/
/******************************************************************************
* sysGetBusSpd - for compartibility with mv5100 and earlier boards
* RETURNS: The bus speed (in Hz).
*/
UINT32
sysGetBusSpd(void)
{
  return(DEC_CLOCK_FREQ);
}

/* Sergey - just to resolve reference in libcodatt.ppcsl (ttutils.c) */
int
intDisconnect(int dummy)
{
  return(0);
}

/* TO DO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
STATUS
sysBusToLocalAdrs (
  int     adrsSpace,    /* bus address space where busAdrs resides */
  char *  busAdrs,    /* bus address to convert */
  char ** pLocalAdrs    /* where to return local address */
)
{
  STATUS tmp;

  /*tmp = (unsigned int)busAdrs;*/

  return(tmp);
}

/* converted from sysMsDelay by decreasing 'totalDelta' by factor 1000 */
void
sysUsDelay (
  UINT delay /* length of time in US to delay */
)
{
  register UINT oldval;               /* decrementer value */
  register UINT newval;               /* decrementer value */
  register UINT totalDelta;           /* Dec. delta for entire delay period */
  register UINT decElapsed;           /* cumulative decrementer ticks */

  /*
   * Calculate delta of decrementer ticks for desired elapsed time.
   * The macro DEC_CLOCK_FREQ MUST REFLECT THE PROPER 6xx BUS SPEED.
   */

  totalDelta = ((DEC_CLOCK_FREQ / 4) / 1000000) * delay;

  /*
   * Now keep grabbing decrementer value and incrementing "decElapsed" until
   * we hit the desired delay value.  Compensate for the fact that we may
   * read the decrementer at 0xffffffff before the interrupt service
   * routine has a chance to set in the rollover value.
   */

  decElapsed = 0;

  oldval = vxDecGet ();

  while(decElapsed < totalDelta)
  {
    newval = vxDecGet ();

    if(DELTA(oldval,newval) < 1000)
      decElapsed += DELTA(oldval,newval);  /* no rollover */
    else
      if(newval > oldval)
        decElapsed += abs((int)oldval);  /* rollover */

    oldval = newval;
  }
}

/* Sergey: is it right substitute for sysTimeBaseLGet() ??? */
/* in prpmc880 it is located in sysAUtil.s; it simply read the contents
   of the lower half of the Time Base Register (TBL - TBR 268) like this:

#define TBLR             284   Time Base Lower Register 
FUNC_BEGIN(sysTimeBaseLGet)
    mftb 3
    bclr 20,0
FUNC_END(sysTimeBaseLGet)
*/
/* see sysAUtil.s
UINT32
sysTimeBaseLGet(void)
{
  register UINT val;

  val = vxDecGet();
  sysUsDelay(1);

  return(val);
}
*/
/**********************/
/* Sergey: users code */


#include <bootLib.h>

/* local memory */
#define BOOTLEN 1024
static char boot_line[BOOTLEN];
static BOOT_PARAMS boot_params;
static char targetname[128];
static char hostname[128];

char *
hostName()
{
  /*printf("hostName reached\n");fflush(stdout);*/
  sysNvRamGet(boot_line,BOOTLEN,0);
  /*printf("len1=%d\n",strlen(boot_line));*/
  bootStringToStruct(boot_line,&boot_params);
  strcpy(hostname,boot_params.hostName);  

  return((char*)hostname);
}

char *
targetName()
{
  /*printf("targetName reached\n");fflush(stdout);*/
  sysNvRamGet(boot_line,BOOTLEN,0);
  /*printf("len2=%d\n",strlen(boot_line));*/
  /*printf(">>>%s<\n",boot_line);*/
  bootStringToStruct(boot_line,&boot_params);
  strcpy(targetname,boot_params.targetName);
  /*printf("targetName reached >%s<\n",targetname);fflush(stdout);*/

  return((char*)targetname);
}
