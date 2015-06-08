/* config.h - Motorola PowerPlusIII board configuration header */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
02a,10may02,dtr  Update for shared memory in Tornado 2.2. SPR 73457
01z,11mar02,dtr  Undefining SM_NET SM_SEQ_ADRS and ALTIVEC.
01y,14dec01,dtr  Coding error for SM Bus selection.
01x,06dec01,dtr  Some pre-processor checks don't work.
		 Making INCLUDE_SM_NET conditional on PRPMC_BASE.
		 Adding some comments for DEC2155X define.
01w,25nov01,dtr  Modification for PRPMC_G default PCI_DRAM_MAP_SIZE and 
                 BSP_VERSION.
01v,16nov01,scb  Shared memory fixes using mcpn765 as carrier.
01u,15oct01,scb  Mods to integrate WRS fei driver in lieu of i82559 driver.
01t,10oct01,scb  Modifications for shared-memory support.
01s,31jul01,srr  Added PrPMC-G Gigabit Ethernet and PrPMC Carrier 1 support.
01r,18jun01,srr  Added Harrier DMA support.
01q,14jun01,srr  Fix problem with dec21554 interrupt enabling and updated
                 information about Slave memory in PCI_MSTR_MEMIO space.
01p,12jun01,srr  Removed unneeded PrPMC definitions.
01o,14may01,bvc  Removed unneeded INCLUDE_MOT_BUG_ROM and ATA support.
01n,08jan01,dmw  Changed PCI window sizes for Slave Ethernet support.
01m,08jan01,krp  Changed window size of flash Bank 'A'
01l,05jan01,krp  The erroneous reference to INCLUDE_ATA was removed.
01k,07dec00,krp  Added support for Watchdog Timer
01j,21nov00,dmw  Removed stand-alone defines.
01i,17nov00,dmw  Added slave Ethernet support.
01h,14nov00,dmw  Fixed FLASH Bank 1's size.
01g,27oct00,dmw  Added Xport addresses and sizes.
01f,12oct00,dmw  Documentation cleanup.
01e,09oct00,dmw  Turned on MMU.
01d,08oct00,dmw  Turned off aux clock and MMU.
01c,12sep00,dmw  Added Harrier internal interrupts.
01b,07sep00,dmw  Removed PReP address map for PowerPlusIII.
01a,31jul00,dmw  Written. (from ver 01l, mcpn765/config.h)
*/

/*
This file contains the configuration parameters for the
Motorola PowerPlusIII architecture
*/

#ifndef	INCconfigh
#define	INCconfigh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */




/* Sergey::: */

/* tried to get nanosleep - does not work
#define INCLUDE_POSIX_TIMERS
#define __STDC__
*/

/*low-level debug messages, use with coushion ! */
/*#define MY_AUTO_DEBUG*/

/*
#define PCI_AUTO_DEBUG
#define PCI_AUTO_DEBUG1
*/

/* 'save' messages */
#define MVME5500_DEBUG

/* slave to be installed in mvme5500 */
#define MVME5500










/* The following defines must precede configAll.h */

/* BSP version/revision identification */

#define BSP_VER_1_1     1
#define BSP_VER_1_2     1
#define BSP_VERSION     "1.2"
#define BSP_REV         "/1"

/* PRIMARY INCLUDES */

#include "configAll.h"
#include "sysMotCpci.h"

/* defines */

#define DEFAULT_BOOT_LINE \
	"fei(0,0)host:/tornado/prpmc800/vxWorks h=90.0.0.1 e=90.0.0.2 u=vxworks"
#define WRONG_CPU_MSG "A PPC604 VxWorks image cannot run on a PPC603!\n";

/*
 * CPCI_MSTR_MEM_BUS accesses the beginning of system
 * (cPCI master) DRAM.  This should be set to:
 *     0x80000000 if the cPCI master is an MCP750;
 *     0x00000000 if the cPCI master is a CPV5000.
 * LOCAL_PCI_MSTR_MEM_BUS accesses the beginning of local
 * (PCI master) DRAM.
 * 
 */

#define CPCI_MSTR_MEM_BUS	0x80000000
#define LOCAL_PCI_MSTR_MEM_BUS	0x0

/*
 * Default board configurations
 *
 * If a supported feature is desired,
 *         change to: #define
 * If a feature is not desired or not supported
 *         change to: #undef
 *
 * NOTE: Not all functionality is supported on all boards
 */

#define	INCLUDE_I8250_SIO       /* COM1 thru COM4 are 16C550 UARTS */
#define	INCLUDE_CACHE_L2        /* L2 cache support */
#define	INCLUDE_ECC             /* Harrier SMC ECC */
/* Undefining INCLUDE_BPE will cause machine check, bus parity exceptions 
 * to be disabled in romInit.s */
#define	INCLUDE_BPE             /* Processor bus Parity checking */
#define	INCLUDE_DPM             /* Dynamic Power Management */
#undef	INCLUDE_AUX_CLK          /* Harrier aux clock */
#define	INCLUDE_HARRIER_DMA     /* Harrier DMA */

/*#define INCLUDE_DEC2155X*/        /* Intel/DEC 21554 PCI-to-PCI bridge 
				   Only on PRPMC_BASE which is default*/
#undef  INCLUDE_DEC2155X

/*#undef	INCLUDE_SECONDARY_ENET*/	/* Enable Secondary Ethernet */
#undef	INCLUDE_SECONDARY_ENET

#undef	INCLUDE_TERTIARY_ENET	/* Enable Tertiary Ethernet on Carrier 1 only */

#undef  INCLUDE_PRPMC800XT      /* Extended version of the PrPMC800 */
#define INCLUDE_FEI_END


/* Sergey: does not work in Slave mode ! (see target.nr) */
#undef  INCLUDE_DEC_END         /* Support for DEC21x4x driver. */


#undef  INCLUDE_GEI_END
#undef  INCLUDE_ALTIVEC         /* Support for Altivec coprocessor */
#undef  INCLUDE_TFFS

#ifdef  INCLUDE_TFFS
#define INCLUDE_DOSFS
#endif

/*
 * Carrier boards available for selection:
 * PRPMC_BASE (default), PRPMC_CARRIER_1, PRPMC_G
 */

/*#define CARRIER_TYPE		PRPMC_BASE*/
#define CARRIER_TYPE	    PRPMC_CARRIER_1

/*
 * If the Primary Ethernet device on the Carrier 1 board is the
 * PrPMC800 onboard i82559 device, then PCI_CARRIER_1_PRI_BUS must
 * be set to 0 (default).  Otherwise, if the Primary Ethernet device
 * is one of the two Carrier 1 onboard i82559 devices, the value
 * must be set to 1.
 */

#define PCI_CARRIER_1_PRI_BUS	 0  /* PCI Bus Primary i82559 is on */

#   define VM_STATE_MASK_FOR_ALL \
           VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE |VM_STATE_MASK_CACHEABLE
#   define VM_STATE_FOR_IO \
           VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT
#   define VM_STATE_FOR_MEM_OS \
           VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE
#   define VM_STATE_FOR_MEM_APPLICATION \
           VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE
#   define VM_STATE_FOR_PCI \
           VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT

/* 
 *  Watchdog timer support - ONLY on the PrPMC800 Extended board  
 */

#undef  INCLUDE_M48T559WDT      
                               

/* 
 * The following define controls the Slave's use of its onboard Ethernet.
 * Defining SLAVE_OWNS_ETHERNET will allow the Slave to install the
 * Ethernet driver and connect/enable the Ethernet interrupt.
 */

#define  SLAVE_OWNS_ETHERNET       

/*
 * NONFATAL_VPD_ERRORS is a debug build switch which, when defined, allows
 * the BSP to tolerate what would otherwise be fatal VPD errors.
 */

#undef     NONFATAL_VPD_ERRORS  /* define to debug VPD info */

/*
 * BYPASS_VPD_PCO is a debug build switch which, when defined, causes the
 * BSP to ignore the VPD Product Configuration Options bit array and use
 * a default configuration which assumes one serial port (debug console) and
 * one Ethernet device (Unit 0).
 */

#undef     BYPASS_VPD_PCO       /* define to ignore VPD PCO */

/*
 * BYPASS_SPD is a debug build switch which, when defined, forces the BSP
 * to ignore SPD information and assume a 32MB SDRAM array and configures
 * the system memory controller for default (worst case) memory timing. It 
 * will also produce a Group A 0x40000000 Bootrom Error message at startup 
 * as a reminder that default timing is in use.
 */

#undef     BYPASS_SPD	    /* define to debug SPD info */

/*
 * PCI_MSTR_IO_SIZE, PCI_MSTR_MEMIO_SIZE and PCI_MSTR_MEM_SIZE control the
 * sizes of the available PCI address spaces. The windows defined by these
 * parameters must be large enough to accommodate all of the PCI memory and
 * I/O space requests found during PCI autoconfiguration. If they are not,
 * some devices will not be autoconfigured.
 *
 * Be aware that these values alter entries in the sysPhysMemDesc[] array
 * (in sysLib.c) and affect the size of the resulting MMU tables at the
 * rate of 128:1. For each 128K of address space, a 1K piece of RAM will be
 * used for MMU tables.  If the region is very large, modifications to
 * sysLib.c can be made to use BAT (Block Address Translation) registers
 * instead of MMU page tables to map the memory.
 *
 * PCI_MSTR_MEMIO_LOCAL which is defined below with a default value
 * of 0x40000000 (1-GigaByte) defines the beginning local (processor-based)
 * address of PCI memory space.  The value may be altered if desired but
 * be aware that the value of PCI_MSTR_MEMIO_LOCAL should always be greater
 * than or equal to the amount of DRAM on the largest prpmc800 in a
 * Monarch/Slave pairing.  If it is not there will be PCI memory which
 * unaccessible to the prpmc800 with the larger amount of DRAM.
 *
 * NOTE: Since PCI auto-configuration is performed by the bootroms,
 *       changing any of these values requires the creation of new bootroms.
 */

/*
 * The Extended PCI map (Pseudo-CHRP) maximizes PCI mapped Memory space.
 *
 * CPU Space   -----------------  PCI Space (these are the values used
 *            |                 |            for autoconfig setup)
 *            |                 |
 * sysMemTop   ----------------- 
 *            :  Unused         :
 *            :                 :
 *             ----------------- PCI_MSTR_MEMIO_LOCAL 
 *            | NPREF PCI MEM   |
 * + 0x800000  ----------------- PCI_MSTR_MEM_LOCAL = PCI_MSTR_MEMIO_LOCAL +
 *            | PCI 32-bit MEM  |                     PCI_MSTR_MEMIO_SIZE
 *            |                 |
 *            |                 | 
 *             ----------------- PCI_MSTR_ZERO_LOCAL = PCI_MSTR_MEM_LOCAL + 
 *            | Maps to Local   |                      PCI_MSTR_MEM_SIZE
 *            | PCI address 0   |
 *             ----------------- PCI_MSTR_ZERO_LOCAL + PCI_MSTR_ZERO_SIZE
 *            :                 :
 *            :                 :
 * 0xF0000000  ----------------- Flash Bank 'A' 32MB
 *            |                 | 
 * 0xF1FFFFFF  ----------------- 
 * 0xFC000000  ----------------- 
 *            | MPIC registers  |
 * 0xFD000000  ----------------- ISA_MSTR_IO_LOCAL
 *            | ISA LEGACY      |
 * 0xFD004000  ----------------- ISA_MSTR_IO_LOCAL + ISA_LEGACY_SIZE
 *            | PCI 16-bit I/O  |
 * 0xFD010000  ----------------- ISA_MSTR_IO_LOCAL + ISA_MSTR_IO_SIZE
 *            | PCI 32-bit I/O  |
 * 0xFE810000  ----------------- PCI_MSTR_IO_LOCAL + PCI_MSTR_IO_SIZE
 * 0xFEFF0000  ----------------- HARRIER_XCSR_BASE
 *            | Harrier XCSRs   |
 * 0xFFEF4000  ----------------- 
 *
 * PCI Mapped Memory consists of non-prefetchable and prefetchable.
 * 
 * The maximum amount of non-prefetchable PCI memory is defined by
 * the value of PCI_MSTR_MEMIO_SIZE.
 *
 * The amount of prefetchable PCI memory is defined by the value of
 * PCI_MSTR_MEM_SIZE.  The map for prefetchable and noprefetchable
 * PCI memory must not extend beyond the flash bank address of
 * 0xF0000000.  
 *
 * That is:
 * PCI_MSTR_MEMIO_LOCAL + PCI_MSTR_MEMIO_SIZE + PCI_MSTR_MEM_SIZE must
 * be less than 0xF0000000.
 *
 * PCI Mapped I/O consists of 16 and 32 bit I/O.
 *
 * The amount of 16 bit I/O is configured by ISA_MSTR_IO_SIZE and defaults
 * to 64KB.
 *
 * The amount of 32 bit I/O is configured by PCI_MSTR_IO_SIZE.  This size
 * is determined by the user but cannot overrun the System Memory 
 * Controller Registers at 0xFEF80000.
 *
 * Therefore:
 * the maximum value of PCI_MSTR_IO_SIZE = 
 *                                    System Memory Controller Registers - 
 *                                    (ISA_MSTR_IO_LOCAL + ISA_MSTR_IO_SIZE)
 *                                        
 * - or -           max PCI_MSTR_IO_SIZE = 0xFEF80000 - 
 *                                                (0xFE000000 + 0x00010000)
 *
 * - or -           max PCI_MSTR_IO_SIZE =  0x00F70000
 *
 * PCI_MSTR_ZERO_SIZE - represents the amount of memory at PCI bus
 * address zero which will be mapped on an outbound window
 * in the host bridge.  See #define of PCI_MSTR_ZERO_LOCAL (in
 * another header file), which represents the local (processor) address 
 * address of this memory region.
 *
 */

#define PCI_MSTR_IO_SIZE     0x00800000           /* 8MB (default) */


#ifdef MVME5500
#define PCI_MSTR_MEMIO_LOCAL ((UINT)0x83000000)     /* 1 GB */
#else
/*#define PCI_MSTR_MEMIO_LOCAL ((UINT)0x40000000)*/     /* 1 GB */
#define PCI_MSTR_MEMIO_LOCAL ((UINT)0x83000000)     /* 1 GB */
#endif


#define PCI_MSTR_MEMIO_SIZE  ((UINT)0x30000000)  /* 256 MB */


#define PCI_MSTR_MEM_SIZE    ((UINT)0x00800000)           /* 8 MB */


/* Sergey: set size of outbound translation window 2 (maps PCI at 0 address) */
#ifdef MVME5500
#define PCI_MSTR_ZERO_SIZE   ((UINT)0x20000000) /* 512 MB */
#else
#define PCI_MSTR_ZERO_SIZE   ((UINT)0x00400000)	/* 4 MB */
#endif

/* 
 * PCI2DRAM_MAP_SIZE governs how large a piece of low-order DRAM will
 * be mapped onto the local PCI bus through the Harrier inbound translation
 * zero register set.  16MB (0x01000000) is the minimum amount which will
 * allow for proper operation of ethernet devices - such devices must use
 * preallocated buffers for their descriptors and these buffers appear
 * between the 8MB and 16MB in DRAM.
 *
 * To handle the start offset of the bootrom, PCI2DRAM_MAP_SIZE must be
 * larger than RAM_HIGH_ADRS to include the Ethernet buffers in the window
 * visible from the PCI bus.
 *
 * For the PRPMC_G the PCI2DRAM_MAP_SIZE cannot be truncated it must be set to
 * size of the local dram to use the gigabit driver.
 */

/* configured for PRPMC_BASE,PRPMC_CARRIER. */
/*#define PCI2DRAM_MAP_SIZE    (RAM_HIGH_ADRS + 0x00800000)*/
/* Sergey: set it to the actual DRAM size - lets map whole memory */
#define PCI2DRAM_MAP_SIZE    ((UINT)0x08000000)


#define PRPMC800_XPORT0_ADDR         0xF0000000   /* Xport0 */
#define PRPMC800_XPORT1_ADDR         0xFF800000   /* Xport1 */
#define PRPMC800_XPORT2_ADDR         0xFF100000   /* Xport2 */
#define PRPMC800_XPORT3_ADDR         0xFF200000   /* Xport3 */

#define PRPMC800_XPORT0_SIZE         0x02000000 
#define PRPMC800_XPORT1_SIZE         0x00700000 
#define PRPMC800_XPORT2_SIZE         0x00200000 
#define PRPMC800_XPORT3_SIZE         0x00080000

/*
 * NOTE: The following define works around a problem encountered with the
 * 21554's secondary bus arbiter. By default, the Processor Host Bridge will 
 * perform a speculative PCI request when a PCI-bound transaction has been 
 * retried due to PCI lock resolution. This behavior is not compatible with 
 * the 21554's secondary bus arbiter. To prevent arbiter lock-ups, speculative 
 * PCI bus requests should be disabled if a 21554 is used as the PCI bus 
 * arbiter.
 */ 

#define DISABLE_SPRQ	/* undef if not using 21554 as PCI bus arbiter */

/* This table describes the PrPMC800 PCI IDSEL-to-MPIC interrupt routing. */

#define INTERRUPT_ROUTING_TABLE \
static UCHAR intLine [][4] = \
    { \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 0/31, PCI Host bridge */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 1  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 2  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 3  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 4  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 5  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 6  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 7  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 8  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 9  - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 10 - not available */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 11 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 12 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 13 */ \
        { LN2_INT_VEC,              /* device number 14, DEC 21143 Ethernet */\
          0xff,\
          0xff,\
          0xff },\
        { 0xff, 0xff, 0xff, 0xff }, /* device number 15 */ \
        { PCI_INTA_VEC,	            /* device number 16, PMC slot 1 */\
          PCI_INTB_VEC,\
          PCI_INTC_VEC,\
          PCI_INTD_VEC },\
        { LN1_INT_VEC,              /* device number 17, i82559 Ethernet */\
          0xff,\
          0xff,\
          0xff },\
        { SLV_LN1_INT_VEC,          /* device number 18, Slv i82559 Ethernet */\
          PCI_INTD_VEC,\
          PCI_INTA_VEC,\
          PCI_INTB_VEC },\
        { 0xff, 0xff, 0xff, 0xff }, /* device number 19 */ \
        { CPCI_INT_VEC,	            /* device number 20 DEC21554 */ \
          PCI_INTB_VEC,\
          PCI_INTC_VEC,\
          PCI_INTD_VEC },\
        { 0xff, 0xff, 0xff, 0xff }, /* device number 21 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 22 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 23 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 24 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 25 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 26 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 27 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 28 */ \
        { 0xff, 0xff, 0xff, 0xff }, /* device number 29 */ \
        { LN2_INT_VEC,              /* device number 30,PrPMC-G 82543 Gigabit*/\
          0xff,\
          0xff,\
          0xff }\
    };

/*
 * static define for ethernet chip interrupt. Only used if PCI Auto-Config
 * is disabled (debug).
 */

#define LN_INT_LVL	PCI_INT_LVL4

/* MMU and Cache options */

#ifdef MVME5500
#define	INCLUDE_MMU_BASIC       /* bundled mmu support */
#else
#define	INCLUDE_MMU_BASIC       /* bundled mmu support */
#endif

#undef	USER_D_CACHE_MODE
#define	USER_D_CACHE_MODE       (CACHE_COPYBACK | CACHE_SNOOP_ENABLE)
#undef	USER_I_CACHE_MODE
#define	USER_I_CACHE_MODE       (CACHE_COPYBACK | CACHE_SNOOP_ENABLE)

#ifdef  INCLUDE_CACHE_L2
#   define USER_L2_CACHE_ENABLE	/* enable the L2 cache */
#endif  /* INCLUDE_CACHE_L2 */

/* timestamp option not included by default;  #define to include it */

#undef  INCLUDE_TIMESTAMP

/* De-select unused (default) network drivers selected in configAll.h */

#undef  INCLUDE_EX              /* include Excelan Ethernet interface */
#undef  INCLUDE_ENP             /* include CMC Ethernet interface*/

#define	INCLUDE_END		/* Enhanced Network Driver (see configNet.h) */

/* PCI bus numbers for secondary and subordinate buses */

#define PCI_SECONDARY_BUS       1
#define PCI_SUBORD_BUS          1
#define PCI_MAX_BUS             2       /* Max. number of PCI buses in system */

/* Dec2155x (Drawbridge) configuration parameters */

#define DEC2155X_SUB_VNDR_ID_VAL    MOT_SUB_VNDR_ID_VAL
#define DEC2155X_SUB_SYS_ID_VAL     PRPMCBASE_SUB_SYS_ID_VAL

/* Harrier configuration parameters */

#define HARRIER_SUB_VNDR_ID_VAL	    PRPMC800_SUB_SYS_ID_VAL
#define HARRIER_SUB_SYS_ID_VAL      MOT_SUB_VNDR_ID_VAL

#ifdef INCLUDE_DEC2155X
#   define DEC2155X_PCI_DEV_NUMBER 0x14
#   define DEC2155X_PCI_BUS_NUMBER 0x00

    /*
     * the following define assigns all PCI arbiter inputs to the high-priority
     * group. for details, consult the 21554 user's manual.
     */

#   define DEC2155X_ARB_CTRL_VAL    DEC2155X_ARB_CTRL_MSK

    /*
     * a PCI read from the following cPCI memory address is used to flush the
     * Dec2155x write post buffer. It must be a valid location and free of
     * side effects. The default value targets location 0x00000000 in host DRAM.
     */

#   define CPCI_FLUSH_ADDR CPCI_MSTR_MEM_BUS

    /*
     * NOTE: Window sizes must be an integral power of 2 and translation
     * values must be an even multiple of the window size. To enable
     * prefetch on a memory window "or" in PCI_BAR_MEM_PREFETCH.
     */

    /* Downstream windows (for access from Compact PCI backpanel) */

    /*
     * note that downstream translation values are relative to the local PCI
     * memory map not the local processor address map. PCI_SLV_MEM_BUS is
     * the base of the local DRAM as seen from the local PCI bus (secondary
     * side of the 2155x).  The Processor Host bridge will translate this into 
     * a local DRAM address.
     */

    /* 4MB window into local DRAM (first 4KB accesses 2155x CSR reg set). */

#   define DEC2155X_CSR_AND_DS_MEM0_SIZE 0x00400000

#   define DEC2155X_CSR_AND_DS_MEM0_TYPE (PCI_BAR_SPACE_MEM | \
                                          PCI_BAR_MEM_ADDR32)

#   define DEC2155X_CSR_AND_DS_MEM0_TRANS PCI_SLV_MEM_BUS

    /* Downstream window 1 to map Harrier Message Passing (doorbell) space. */

#   define DEC2155X_DS_IO_OR_MEM1_SIZE   0x04000000	/* 64 MB */
#   define DEC2155X_DS_IO_OR_MEM1_TYPE   (PCI_BAR_SPACE_MEM | \
                                          PCI_BAR_MEM_ADDR32)
#   define DEC2155X_DS_IO_OR_MEM1_TRANS  PCI_SLV_PMEP_BUS

    /* Downstream windows 2 and 3 not used (disabled) */

#   define DEC2155X_DS_MEM2_SIZE         0x00000000
#   define DEC2155X_DS_MEM2_TYPE         (PCI_BAR_SPACE_MEM| \
                                          PCI_BAR_MEM_ADDR32 )
#   define DEC2155X_DS_MEM2_TRANS        0x00000000

#   define DEC2155X_DS_MEM3_SIZE         0x00000000
#   define DEC2155X_DS_MEM3_TYPE         (PCI_BAR_SPACE_MEM| \
                                          PCI_BAR_MEM_ADDR32 )
#   define DEC2155X_DS_MEM3_TRANS        0x00000000

    /* Upstream windows (for access to Compact PCI backpanel) */

    /*
     * note that upstream translation values are relative to the Compact PCI
     * memory map not the local processor address map. 0x00000000 is the
     * base of PCI memory space as seen from the Compact PCI bus (primary
     * side of the 2155x).
     */

    /* 4MB window into host (system slot) DRAM. */

#    define DEC2155X_US_IO_OR_MEM0_SIZE 0x00400000
#    define DEC2155X_US_IO_OR_MEM0_TYPE (PCI_BAR_SPACE_MEM | \
                                         PCI_BAR_MEM_ADDR32)

#   define DEC2155X_US_IO_OR_MEM0_TRANS CPCI_MSTR_MEM_BUS /* map to DRAM */

    /*
     * 512 MB window into Compact PCI memory space to access non-system boards
     * NOTE: this window must be large enough to cover the pci memory area
     * configured in the host for dynamic pci device allocation. the
     * translation value for this window should equal the pci memory base
     * address for this area.
     */

#   define DEC2155X_US_MEM1_SIZE 0x20000000
#   define DEC2155X_US_MEM1_TYPE (PCI_BAR_SPACE_MEM | \
                                  PCI_BAR_MEM_ADDR32)

#   define DEC2155X_US_MEM1_TRANS 0x00000000 

#   define DEC2155X_US_MEM2_PG_SZ 0x00000000 /* close window */

#endif /* INCLUDE_DEC2155X */










/*
 * Slave-Mode PrPMC Parameters
 *
 * These parameters are used by the BSP when it is operating as a PCI
 * Slave device (non-monarch mode).
 */

#define PRPMC_SLAVE_SUB_VNDR_ID_VAL	MOT_SUB_VNDR_ID_VAL
#define PRPMC_SLAVE_SUB_SYS_ID_VAL	PRPMC800_SUB_SYS_ID_VAL

/*
 * The following defines set the amount of time allowed for Slave-Mode PrPMC
 * Auto-Configuration. The value of PRPMC_SLAVE_INFINTE_TIMEOUT disables the
 * timeout, resulting in an infinite wait. The PRPMC_SLAVE_SIDE_TIMEOUT controls
 * the amount of time the Slave-Mode device will wait for configuration by the
 * PCI Host.  If an infinite timeout is not specified, the Slave will wait
 * PRPMC_SLAVE_SIDE_TIMEOUT mSecs for the PCI Host to complete the enumeration.
 */

#define PRPMC_SLAVE_INFINITE_TIMEOUT	0xffffffff
/*#define PRPMC_SLAVE_SIDE_TIMEOUT	PRPMC_SLAVE_INFINITE_TIMEOUT*/
#define PRPMC_SLAVE_SIDE_TIMEOUT	10000

/*
 * The following defines the amount of time allowed for system controller
 * initialization of the primary side of the Dec2155x (drawbridge) non-
 * transparent bridge.  The value of  PRPMC_DRAWBRIDGE_INFINITE_TIMEOUT 
 * disables the timeout, resulting in an infinite wait.  The 
 * PRPMC_DRAWBRIDGE_TIMEOUT controls the amount of time the PrPMC will
 * wait for the configuration of the Dec2155x drawbridge by the system
 * controller.  If an infinite timeout is not specified, the PrPMC will wait
 * PRPMC_DRAWBRIDGE_TIMEOUT mSecs for the system controller to complete
 * the drawbridge configuration and it will then proceed whether or not
 * the configuration is complete.
 */

#define PRPMC_DRAWBRIDGE_INFINITE_TIMEOUT  0xffffffff
#define PRPMC_DRAWBRIDGE_TIMEOUT	   PRPMC_DRAWBRIDGE_INFINITE_TIMEOUT

/*
 * The following define controls the interrupt routing used for generating 
 * outbound PCI bus interrupts. The default value matches the routing required
 * by the PCI Spec for a single interrupt PCI device (INTA).
 */

#define PRPMC_SLAVE_BUS_INT_ROUTE	0

/*
 * The following define sets the address space advertised for the Slave-Mode
 * in-bound mailbox and is currently not used. The default value is
 * PCI_SPACE_MEM_PRI.
 */

#define PRPMC_SLAVE_MAILBOX_SPACE       PCI_SPACE_MEM_PRI


/* End of Slave-Mode PrPMC Defines */
















/* serial parameters */

#undef	NUM_TTY
#define	NUM_TTY			N_SIO_CHANNELS

/*
 * Auxiliary Clock support is an optional feature that is not supported
 * by all BSPs.  Since it requires the MPIC timers, the Auxiliary Clock
 * is only supported in MPIC mode.
 */

#if defined(INCLUDE_SPY)
#    define INCLUDE_AUX_CLK                /* specify aux clock device */
#endif /* INCLUDE_SPY */

/*
 * Local Memory definitions
 *
 * By default, the available DRAM memory is sized at bootup (LOCAL_MEM_AUTOSIZE
 * is defined).  If auto-sizing is not selected, make certain that
 * LOCAL_MEM_SIZE is set to the actual amount of memory on the board.
 * By default, it is set to the minimum memory configuration: 32 MB.
 * Failure to do so can cause unpredictable system behavior!
 */

#define	LOCAL_MEM_AUTOSIZE	  /* undef for fixed size */
#define LOCAL_MEM_LOCAL_ADRS	0x00000000  /* fixed at zero */
#define LOCAL_MEM_SIZE		    0x08000000  /* set to 128MB */

#define RAM_HIGH_ADRS		    0x00800000  /* RAM address for ROM boot */
#define RAM_LOW_ADRS		    0x00100000  /* RAM address for kernel */

/* user reserved memory, see sysMemTop() */

#define USER_RESERVED_MEM	(0)	/* number of reserved bytes */

/*
 * The constants ROM_TEXT_ADRS, ROM_SIZE, RAM_LOW_ADRS and RAM_HIGH_ADRS 
 * are defined in config.h, Makefile.
 * All definitions for these constants must be identical.
 */

#define	ROM_BASE_ADRS		0xfff00000	/* base address of ROM */
#define	ROM_TEXT_ADRS		(ROM_BASE_ADRS + 0x100)

#define	ROM_SIZE		0x00100000	/* 1 Meg ROM space */

/* Shared-memory parameters */

/*
 * INCLUDE_SM_NET and INCLUDE_SM_SEQ_ADDR are the shared memory backplane
 * driver and the auto address setup.  These must be #define'd here in
 * order to configure this BSP for participation in a shared-memory
 * network. These must be undefined for CARRIER_1 and PRPMC_G as there is no
 * shared memory capability.
 */

#undef INCLUDE_SM_NET
#undef INCLUDE_SM_SEQ_ADDR

#ifdef  INCLUDE_SM_NET
#define INCLUDE_SM_COMMON
#define INCLUDE_SM_OBJ
#endif  /* INCLUDE_SM_NET */

/*
 * The following defines are used when generating or receiving shared memory
 * bus interrupts. Depending on configuration, the bus interrupt may be
 * generated using the Dec21554 (to drive a cPCI bus interrupt) or may be
 * generated by the CPU1 portion of the MPIC Inter-Processor Interrupt mechanism
 * (to drive a local PCI bus interrupt). For convenience, the same vector
 * number is used in both environments. Note that the same IPI vector (0-3)
 * cannot be used for both in-bound and out-bound interrupts simultaneously.
 * This constraint is driven by the MPIC IPI implementation which shares a
 * common enable bit for both Processor 0 and Process 1 interrupts.
 */

/*
 * The following defines are used by the Monarch to control PCI Bus interrupts
 * generated by non-Monarch PrPMCs. The default values configure IPI3
 * as the shared memory bus interrupt.
 *
 * NOTE: In this case, interrupt level (0x27) does not equal interrupt vector
 * (0x60).
 */

#define SM_BUS_INT_LVL		    IPI3_INT_LVL
#define SM_BUS_INT_VEC		    DEC2155X_MAILBOX_INT_VEC

/*
 * The following defines are used by non-Monarch PrPMCs to control in-bound
 * shared memory mailbox interrupts. The default values configure IPI0
 * as the in-bound shared memory mailbox interrupt with a priority level of 7.
 */

#define SM_MAILBOX_INT_LVL	IPI0_INT_LVL
#define SM_MAILBOX_INT_PRIORITY	PRIORITY_LVL7

/* Local and backplane bus numbers - see SYS_SM_BUS_NUMBER */

#define SYS_LOCAL_PCI_BUS_NUMBER 	0
#define SYS_BACKPLANE_BUS_NUMBER    	1 

#if defined(INCLUDE_SM_COMMON)

#   define STANDALONE_NET
#   define INCLUDE_NET_SHOW
#   define INCLUDE_BSD

#   define SM_OFF_BOARD		TRUE       /* Memory pool is off-board */

   /* 
    * When shared memory anchor polling is enabled, the following defines the
    * PCI bus number on which to poll devices for the shared memory anchor.
    * If the PrPCM800 must cross over the backplane bus to access the anchor,
    * then #define  SYS_SM_BUS_NUMBER SYS_BACKPLANE_BUS_NUMBER .
    * If the PrPMC800 can stay on the local PCI bus to reach the anchor
    * then #define  SYS_SM_BUS_NUMBER SYS_LOCAL_PCI_BUS_NUMBER .
    */

#    define SYS_SM_BUS_NUMBER	SYS_BACKPLANE_BUS_NUMBER

#if (SYS_SM_BUS_NUMBER == SYS_LOCAL_PCI_BUS_NUMBER)
#   define PCI_SPACE_MEM         PCI_SPACE_MEM_PRI
#   define MSTR_MEM_BUS          LOCAL_PCI_MSTR_MEM_BUS
#else
#   define PCI_SPACE_MEM         PCI_SPACE_MEM_SEC
#   define MSTR_MEM_BUS          CPCI_MSTR_MEM_BUS
#endif                             


   /*
    * If the anchor is offboard (SM_OFF_BOARD == TRUE) then place the
    * anchor SM_ANCHOR_OFFSET at 0x4100 if the actual anchor is on an
    * MCP750 or MCPN750, or place it at 0x1100 if the actual anchor is
    * on a CPV5000.
    */

#   undef SM_ANCHOR_ADRS
#   if (SM_OFF_BOARD == TRUE)
#      undef SM_ANCHOR_OFFSET
#      define SM_ANCHOR_OFFSET 0x4100  /* define as 0x1100 for CPV5000 master */
#      define SM_ANCHOR_ADRS		(sysSmAnchorAdrs())
#      define SM_MEM_ADRS    (SM_ANCHOR_ADRS + (0x4d00 - SM_ANCHOR_OFFSET))
#   else /* (SM_OFF_BOARD == TRUE) */
#      define SM_MEM_ADRS    0x00004d00
#      define SM_ANCHOR_ADRS ((char *)(LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET))
#   endif /* (SM_OFF_BOARD == TRUE) */

    /*
     * The following defines are only used by the master.
     * The slave only uses the "Anchor" address.
     */

#   define SM_MEM_SIZE		0x00010000
#   define SM_OBJ_MEM_ADRS	(SM_MEM_ADRS+SM_MEM_SIZE) /* SM Objects pool */

    /*
     * Finding the shared memory anchor:
     *
     * There are three ways to communicate the location of the anchor to the
     * initialization code:
     * 
     * 1) If "sm=xxxxxxxx" is specified as a boot parameter, then "xxxxxxxx"
     *    is used as the local address of the anchor.
     * 
     * 2) If case (1) above is not satisfied, then if SM_OFF_BOARD is FALSE,
     *    the address LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET is used as the
     *    local address of the anchor.
     * 
     * 3) If neither (1) or (2) above is satisfied (that is "sm=xxxxxxxx" is
     *    NOT specified AND SM_OFF_BOARD is defined as TRUE) then the shared
     *    memory anchor is found via a polling algorithm as described below:
     *
     *    Devices on the compactPCI bus (defined by SYS_BACKPLANE_BUS_NUMBER)
     *    are queried through the first memory BAR.  Memory at offset
     *    SM_ANCHOR_OFFSET is examined to determine if the anchor is there.  
     *
     *    If SYS_SM_ANCHOR_POLL_LIST is defined then only those
     *    devices whose device/vendorID and subsystem device/vendorID
     *    are defined in this list are queried.  If SYS_SM_ANCHOR_POLL_LIST
     *    is NOT defined then ALL devices found on SYS_BACKPLANE_BUS_NUMBER
     *    are queried.
     *
     *    In addition if SYS_SM_SYSTEM_MEM_POLL is defined, the
     *    system memory (at compact PCI address CPCI_MSTR_MEM_BUS + 
     *    SM_ANCHOR_OFFSET) is also queried for a possible location for 
     *    the anchor.  If SYS_SM_SYSTEM_MEM_POLL is not defined, then 
     *    system memory is not polled.  This option would typically be 
     *    used if the anchor resided on an system slot controller and the 
     *    initialization code was running on the non-system slot controller.
     */

#   define SYS_SM_SYSTEM_MEM_POLL

#   ifndef _ASMLANGUAGE
        IMPORT  char * sysSmAnchorAdrs();
        int     sysSmIntTypeCompute (void);
        int     sysSmArg1Compute (int intType);
        int     sysSmArg2Compute (int intType);
        int     sysSmArg3Compute (void);
#   endif /* _ASMLANGUAGE */

    /* 
     * Legal settings for the following interrupt types are
     * either SM_INT_MAILBOX_1 or SM_INT_NONE
     */

#   define  SM_HOST_INT_TYPE  SM_INT_MAILBOX_1
#   define  SM_SLAVE_INT_TYPE SM_INT_MAILBOX_1

    /*
     * Because this BSP can operate in Monarch or non-Monarch mode, the
     * shared memory parameters must be calculated at run-time based on the
     * operating mode.
     */

#   define SM_INT_TYPE  (sysSmIntTypeCompute())
#   define SM_INT_ARG1  (sysSmArg1Compute(SM_INT_TYPE))
#   define SM_INT_ARG2  (sysSmArg2Compute(SM_INT_TYPE))
#   define SM_INT_ARG3  (sysSmArg3Compute())

#   define SYS_SM_ANCHOR_POLL_LIST \
	    SYS_MOT_SM_ANCHOR_POLL_LIST	/* shared memory boards ID list */

#endif /* defined(INCLUDE_SM_COMMON) */


#ifdef INCLUDE_SM_OBJ
#   define SM_OBJ_MEM_SIZE	0x00010000
#endif /* INCLUDE_SM_OBJ */

/*
 * Note: This BSP requires a modified software Test and Set algorithm.
 * SM_TAS_TYPE is set to SM_TAS_HARD despite the lack of a hardware TAS
 * mechanism to force the use of a BSP-specific software TAS algorithm. The
 * modified algorithm is required to work around a problem encountered with
 * PCI-to-PCI bridges.
 */

#undef SM_TAS_TYPE
#define SM_TAS_TYPE 	SM_TAS_HARD

/*
 * The following must be a value between 0 and 15.  It represents the 
 * bit number of the primary doorbell register used to interrupt the 
 * MCP750 for shared memory bus interrupts. 
 */

#define DEC2155X_SM_DOORBELL_BIT 0

/*
 * These defines are used to initialize the External Source
 * Vector/Priority registers in the MPIC.  The following can
 * be defined: interrupt sensitivity, polarity and interrupt priority.
 *
 * Note: by default a 1 into the sense bit(22) will set up for active
 * low (interrupt sources 1 thru 15).  A 1 into the polarity bit
 * affects only interrupt source zero and sets it up for high level
 * sensitive interrupts.
 *
 * At initialization all external interrupt sources are disabled
 * except for the Comm1 input, which is enabled in the MPIC driver.
 *
 * All currently unused interrupt sources are set to a priority of
 * 0, which will not allow them to be enabled.  If any one of these
 * levels is to be used, the priority value must be changed here.
 */

#define INIT_EXT_SRC0           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* Host Bus INT0 */

#define INIT_EXT_SRC1           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* not used */
 
#define INIT_EXT_SRC2           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL8 )   /* DEBUG */
 
#define INIT_EXT_SRC3           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* WDT1/WDT2 */
 
#define INIT_EXT_SRC4           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL4 )   /* M48T37V */

#define INIT_EXT_SRC5           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* not used */
 
#define INIT_EXT_SRC6           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL14 )  /* Host Bus INT1 */
 
#define INIT_EXT_SRC7           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL14 )  /* Host Bus INT2 */
 
#define INIT_EXT_SRC8           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL14 )  /* Host Bus INT3 */

#define INIT_EXT_SRC9           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL3 )   /* PMC1A || DEC21554 */
 
#define INIT_EXT_SRC10          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL3 )   /* PMC1B || i82559 */
 
#ifdef SLAVE_OWNS_ETHERNET
#   define INIT_EXT_SRC11       ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* PMC1C */
 
#   if (CARRIER_TYPE == PRPMC_BASE) && defined(INCLUDE_DEC_END)
#      define INIT_EXT_SRC12    ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL3 )   /* PMC1D */
#   else
#      define INIT_EXT_SRC12    ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* PMC1D */
#   endif /* (CARRIER_TYPE == PRPMC_BASE) && defined(INCLUDE_DEC_END) */

#else  /* !SLAVE_OWNS_ETHERNET */
#   define INIT_EXT_SRC11       ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL3 )   /* PMC1C */
 
#   define INIT_EXT_SRC12       ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL3 )   /* PMC1D */
#endif /* SLAVE_OWNS_ETHERNET */
 
#define INIT_EXT_SRC13          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* not used */
 
#define INIT_EXT_SRC14          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* not used */
 
#define INIT_EXT_SRC15          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* not used */

/* Internal Harrier Functional Interrupt Sources */

#define INIT_INT_SRC0           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL8 )
#define INIT_INT_SRC1           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL9 )

 
/* BSP-specific includes */

#include "prpmc800.h"

/* 
 * The prpmc800 has two Ethernet devices. One on-board and one on the 
 * base-board.  The following macro is to determine which is the primary 
 * Ethernet device to be used as the "boot" device.
 * Slave Ethernet support is only supported on the PrPMCBase board (Sergey: ??!!)
 */

#if (CARRIER_TYPE == PRPMC_BASE)
#   define PCI_IDSEL_PRI_LAN	17  /* On-board i82559 Ethernet device */
#   define PCI_IDSEL_SEC_LAN	14  /* Base board DEC21143 Ethernet device */
#   define PCI_IDSEL_SLAVE_LAN	18  /* Slave's On-board i82559 Ethernet device*/

#elif (CARRIER_TYPE == PRPMC_CARRIER_1)
#   define PCI_IDSEL_PRI_LAN	17  /* On-board i82559 Ethernet device*/

/* Sergey: Slave's On-board i82559 Ethernet device*/
#ifdef MVME5500
#   define PCI_IDSEL_SLAVE_LAN 17
#else
#   define PCI_IDSEL_SLAVE_LAN 18
#endif

#   define PCI_IDSEL_SEC_LAN	 2  /* First i82559 on PrPMC Carrier 1*/
#   define PCI_IDSEL_TER_LAN	 5  /* Second i82559 on PrPMC Carrier1*/

#elif (CARRIER_TYPE == PRPMC_G)

    /*
     * The Primary Ethernet on the PrPMC-G is the 82543 Gigabit Ethernet.
     *
     * The Secondary Ethernet is the PrPMC800 on-board i82559 Ethernet,
     * but the Gigabit Ethernet driver doesn't use PCI_IDSEL_PRI_LAN,
     * so it is simpler to leave sysFei82557End.c alone and let it "think"
     * the i82559 is the Primary, when it is actually the Secondary Ethernet.
     */

#   define PCI_IDSEL_PRI_LAN	17  /* On-board i82559 Ethernet device*/
#   define PCI_IDSEL_SEC_LAN	PCI_IDSEL_PRI_LAN

#endif /* (CARRIER_TYPE == PRPMC_BASE) */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

/*#if defined(PRJ_BUILD)*/
    #include "prjParams.h"
/*#endif*/ /* PRJ_BUILD */


#endif	/* INCconfigh */