/* config.h - Motorola PRPMC880 board configuration header */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2003 Motorola, Inc. */

/*
modification history
--------------------
01b,02Jun03,simon  updated based on peer review results
01a,31mar03,simon  Ported. (from ver 01d mcp905/config.h)
*/

/*
This file contains the configuration parameters for the PrPMC880 board
This file must not contain any "dependencies"
(ie. #ifdef's or #ifndef's).
*/

#ifndef	__INCconfigh
#define	__INCconfigh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* The following defines must precede configAll.h */

/* BSP version/revision identification */

#define BSP_VER_1_1     1
#define BSP_VER_1_2     1
#define BSP_VERSION     "1.2"	/* Tornado 2.x */
#define BSP_REV         "/0.1"

/* PRIMARY INCLUDES */

#include "configAll.h"
#include "sysMotCpci.h"

/* Sergey: slave to be installed in mvme5500 or mvme6100 (mvme5100 as well ?)*/
/* must be defined for CLAS */
#define MVME5500

#undef MYDEBUG


/* defines */

#define DEFAULT_BOOT_LINE \
	"geisc(0,0)host:/tornado/prpmc880/vxWorks h=90.0.0.1 e=90.0.0.2 \
	 u=vxworks"
#define WRONG_CPU_MSG "A PPC604 VxWorks image cannot run on a PPC603!\n"

/*
 * The following cPCI address accesses the beginning of system
 * (cPCI master) DRAM.  This should be set to:
 *     0x80000000 if the cPCI master is an MCP750;
 *     0x00000000 if the cPCI master is a CPV5000.
 * LOCAL_PCI_MSTR_MEM_BUS accesses the beginning of local
 * (PCI master) DRAM.
 */

#define CPCI_MSTR_MEM_BUS	0x00000000
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
 *
 * IMPORTANT:  Having INCLUDE_GEISC_END defined below requires that
 * mv64360End.obj be included on the MACH_EXTRA line in the Makefile.
 */

#define	INCLUDE_MMU_BASIC	   /* MMU Support */
#define	INCLUDE_CACHE_L2       /* L2 cache support */
/* Sergey: undef following line (otherwise romInit.s hung ... */
#undef	INCLUDE_ECC		/* System Memory ECC Support */
#define INCLUDE_AUXCLK         /* MV64360 aux clock support */
#define	INCLUDE_MV64360_DMA    /* MV64360 DMA */
#undef   INCLUDE_DPM           /* Processor Dynamic Power Management */
#define  INCLUDE_GEISC_END     /* MV64360 system controller gigabit ethernet */
#define  INCLUDE_RTC
#define  INCLUDE_NETWORK
#undef   INCLUDE_DEC2155X      /* Intel/DEC 21554 PCI-to-PCI bridge 
				   				  Only on PRPMC_BASE which is default*/

/*
 * DEBUG NOTE:  INCLUDE_ALTIVEC must remain #undef'ed to work around
 * a Wind River bug which prevents the kernel from coming up to a
 * kernel prompt.  We are awaiting a permanent fix for this problem.
 */

#undef	INCLUDE_ALTIVEC         /* Support for Altivec coprocessor */

#ifdef INCLUDE_DEC2155X
/* Dec2155x (Drawbridge) configuration parameters */

#   define DEC2155X_SUB_VNDR_ID_VAL    MOT_SUB_VNDR_ID_VAL
#   define DEC2155X_SUB_SYS_ID_VAL     PRPMCBASE_SUB_SYS_ID_VAL

#   define DEC2155X_PCI_DEV_NUMBER 0x04
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

    /* Downstream window 1 to map MV64360 doorbell space. */

#   define DEC2155X_DS_IO_OR_MEM1_SIZE   0x00010000 /* 64 KB */
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
 * The following defines set the amount of time allowed for Slave-Mode PrPMC
 * Auto-Configuration. The value of PRPMC_SLAVE_INFINTE_TIMEOUT disables the
 * timeout, resulting in an infinite wait. The PRPMC_SLAVE_SIDE_TIMEOUT controls
 * the amount of time the Slave-Mode device will wait for configuration by the
 * PCI Host.  If an infinite timeout is not specified, the Slave will wait
 * PRPMC_SLAVE_SIDE_TIMEOUT mSecs for the PCI Host to complete the enumeration.
 */

#define PRPMC_SLAVE_INFINITE_TIMEOUT	0xffffffff
#define PRPMC_SLAVE_SIDE_TIMEOUT	PRPMC_SLAVE_INFINITE_TIMEOUT

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

/* End of Slave-Mode PrPMC Defines */
/* serial parameters */

#undef	NUM_TTY
#define	NUM_TTY			N_SIO_CHANNELS

/*
 * MV64360 System Memory Controller Support
 *
 * The MV64360's system memory controller (DDR SDRAM) registers
 * contain a number of fields that can be configured for system
 * optimization.  The following definitions are used to configure
 * these fields.  To enable/disable or change these default settings
 * simply modify these definitions.  Only those fields described below
 * can be modified.
 *
 * SDRAM Configuration Register:
 * Bit[14] Enable Physical Banks Interleaving
 * Bit[15] Enable Virtual Banks Interleaving
 * Bits[31:26] Read Buffer Assignment per each interface
 *
 * By default we will enable both physical and virtual bank
 * interleaving, and will set the Gb unit to use read buffer 1 while
 * all other units will use read buffer 0.
 *
 * SDRAM Address Control Register:
 * Bits[3:0] SDRAM Address Select
 *
 * By default this will be set to 0x2.
 *
 * SDRAM Open Pages Control Register:
 * Bits[15:0] Open Page Enable
 *
 * By default this will be set to 0xFFFF.
 *
 * SDRAM Interface Crossbar Control (Low) Register:
 * Bits[3:0]   Slice 0 device controller "pizza" arbiter
 * Bits[7:4]   Slice 1
 * Bits[11:8]  Slice 2
 * Bits[15:12] Slice 3
 * Bits[19:16] Slice 4
 * Bits[23:20] Slice 5
 * Bits[27:24] Slice 6
 * Bits[31:28] Slice 7
 *
 * By default this will be set to 0x00765432.
 *
 * SDRAM Interface Crossbar Control (High) Register:
 * Bits[3:0]   Slice 8
 * Bits[7:4]   Slice 9
 * Bits[11:8]  Slice 10
 * Bits[15:12] Slice 11
 * Bits[19:16] Slice 12
 * Bits[23:20] Slice 13
 * Bits[27:24] Slice 14
 * Bits[31:28] Slice 15
 *
 * By default this will be set to 0x00765432.
 *
 * SDRAM Interface Crossbar Timeout Register:
 * Bits[7:0] Crossbar Arbiter Timeout Preset Value
 * Bit[16]   Crossbar Arbiter Timer Enable
 *
 * By default this will be set to 0x000100FF.
 */

#define MV64360_DDR_SDRAM_CFG_DFLT		0x80000000
#define MV64360_DDR_SDRAM_ADDR_CTRL_DFLT	0x00000002
#define MV64360_DDR_SDRAM_OPEN_PAGES_CTRL_DFLT	0x0000FFFF
#define MV64360_DDR_SDRAM_IF_XBAR_CTRL_LO_DFLT	0x00765432
#define MV64360_DDR_SDRAM_IF_XBAR_CTRL_HI_DFLT	0x00765432
#define MV64360_DDR_SDRAM_IF_XBAR_TMOUT_DFLT	0x000100FF

/*
 * PCI and PCI Auto-Configuration Support
 *
 * Only PCI0 be used in prpmc880, PCI1/2 is useless
 * PCI0_MSTR_IO_SIZE, PCI0_MSTR_MEMIO_SIZE and PCI0_MSTR_MEM_SIZE
 * control the sizes of the available   PCI address spaces.  The windows defined by these
 * parameters must be large enough to accommodate all of the PCI
 * memory and I/O space requests found during PCI autoconfiguration.
 * If they are not, some devices will not be autoconfigured.
 *
 * Be aware that these values alter entries in the sysPhysMemDesc[]
 * array (in sysLib.c) and affect the size of the resulting MMU tables
 * at the rate of 128:1.  For each 128K of address space, a 1K piece
 * of RAM will be used for MMU tables.  If the region is very large,
 * modifications to sysLib.c can be made to use BAT (Block Address
 * Translation) registers instead of MMU page tables to map the
 * memory.
 *
 * NOTE: Since PCI auto-configuration is performed by the bootroms,
 * changing any of these values requires the creation of new bootroms.
 *
 * CPU Space                               PCI Space (these are the values
 * (default values)                        used for autoconfig setup)
 *
 *                              :                 :
 *                              :                 :
 * PCI0_MSTR_MEMIO_LOCAL =       ----------------- PCI0_MSTR_MEMIO_BUS =
 *              (0x80000000)    |  Bus 0.0        |  PCI0_MSTR_MEMIO_LOCAL =
 *                              | NonPrefetch PCI |  (0x80000000)
 *                              | Memory space    |
 *                              |      64MB       |
 *                              | (0x04000000)    |
 *                              |                 |
 * PCI0_MSTR_MEM_LOCAL =         ----------------- PCI0_MSTR_MEM_BUS =
 * PCI0_MSTR_MEMIO_LOCAL +      |  Bus 0.0        |  PCI0_MSTR_MEM_LOCAL
 * PCI0_MSTR_MEMIO_SIZE =       | Prefetch PCI    |  (0x84000000)
 *              (0x84000000)    | Memory space    |
 *                              |      64MB       |
 *                              | (0x04000000)    |
 *       			             ----------------- PCI0_MSTR_ZERO_LOCAL =
 *                              |                 |  PCI0_MSTR_MEM_LOCAL + 
 *    				            | Maps to Local   |    PCI0_MSTR_MEM_SIZE
 *       			            | PCI address 0   |
 *          			         ----------------- PCI0_MSTR_ZERO_LOCAL +
 *                              :                 :  PCI0_MSTR_ZERO_SIZE
 *                              :                 :
 *                              : Not used,       :
 *                              : Available for   :
 *                              : expansion of    :
 *                              : PCI spaces      :
 *                              :                 :
 *                              :                 :
 * MV64360_REG_BASE =            -----------------
 *              (0xf1000000)    | MV64360         :
 *                              | internal regs.  :
 *                              |    64K          :
 *                              | (0x00010000)    :
 *              (0xf1010000)     -----------------
 *                              :     Unused      :
 *                              : 12MB - 960KB    :
 *                              :                 :
 *                              :                 :
 * MV64360_FLSH_BANK_A_ADDR =   :                 :
 *              (0xf2000000)     -----------------
 *                              | Flash bank A    :
 *                              |  64MB    :
 *                              |                 :
 *      (64MB)  (0xf6000000)    |                 :
 *                              : Not used        :
 *                              :        56MB     :
 *                              :                 :
 *                              :                 :
 * ISA_MSTR_IO_LOCAL =           ----------------- ISA_MSTR_IO_BUS =
 *                (0xfe000000)  | Bus 0.0 16-bit  |   (0xfe000000)
 *                              | I/O space       |
 *                              |    64KB         |
 *                              | (0x00010000)    |
 * PCI0_MSTR_IO_LOCAL =          ----------------- PCI0_MSTR_IO_BUS =
 *                 (0xfe800000) | Bus 0.0 32-bit  |   (0xfe800000)
 *                              | I/O space       |
 *                              |    8MB          |
 *                              | (0x00800000)    |
 * MV64360_FLSH_BANK_B_ADDR_     -----------------
 * BOOT_B (0xff100000)          | Flash bank B    :
 *                              |      1MB        :
 *                              |                 :
 *              (0xff1fffff)     -----------------
 *
 * PCI Mapped Memory consists of non-prefetchable and prefetchable.
 * Each bus has its own non-prefetchable and prefetchable memory
 * spaces.
 *
 * The maximum amount of non-prefetchable PCI memory is defined by
 * the user as PCI0_MSTR_MEMIO_SIZE.
 *
 * The maximum amount of prefetchable PCI memory is defined by the
 * user as PCI0_MSTR_MEM_SIZE.
 *
 * PCI Mapped I/O consists of 16 and 32 bit I/O. 
 */

/* Bus 0.0 PCI Space Definitions - must be set to power of 2 */

#define  PCI0_MEM_LOCAL_START  0x80000000   /* Bottom of PCI memory space */
#define	 PCI0_MSTR_MEMIO_SIZE  0x04000000   /* 64 MB */
#define	 PCI0_MSTR_MEM_SIZE    0x04000000   /* 64 MB */
#define	 PCI0_MSTR_IO_SIZE     0x00800000   /* 8 MB */

/* Sergey: set size of outbound translation window 2 (maps PCI at 0 address) */
#ifdef MVME5500
#define  PCI0_MSTR_ZERO_SIZE   0x20000000   /* 512 MB */
#else
#define  PCI0_MSTR_ZERO_SIZE   0x00400000   /* 4 MB */
#endif

/*
 * ISA_MSTR_IO_SIZE, it's sum
 * must be a power of 2
 */

#define ISA_MSTR_IO_START	0xFE000000	/* Bottom of PCI ISA IO space */
#define	ISA_MSTR_IO_SIZE	0x00010000	/* 64 kbytes */

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


/* Sergey: map whole memory - 512MB */
/*#define PCI2DRAM_MAP_SIZE    (RAM_HIGH_ADRS + 0x00800000)*/    /* configured for PRPMC_BASE,PRPMC_CARRIER. */
#define PCI2DRAM_MAP_SIZE    (0x20000000) /* for mv6100 */
	  /*#define PCI2DRAM_MAP_SIZE    (0x007c0000)*/ /* for mv5100 */

/*
 * Cache Support  
 *
 * The symbol INCLUDE_CACHE_SUPPORT (in "configAll.h") is #defined by
 * default and causes L1 cache support (including enabling of the cache)
 * to be provided.  In the event that no L1 cache enabling is desired
 * include a #undef INCLUDE_CACHE_SUPPORT here. However, note that if
 * INCLUDE_CACHE_SUPPORT is not defined (L1 cache not enabled) then
 * L2 cache will not be enabled as well, regardless of the state of the
 * INCLUDE_CACHE_L2 define.  This is because the L2 cache cannot be
 * enabled if the L1 cache is not enabled.  
 */

#undef	USER_D_CACHE_MODE	/* L1 Data Cache Mode */
#define	USER_D_CACHE_MODE       (CACHE_COPYBACK | CACHE_SNOOP_ENABLE)

#ifdef INCLUDE_CACHE_L2

/*
 * L2 Cache Support
 *
 * The following features of the L2 cache can be configured (these are
 * relevant only if INCLUDE_CACHE_L2 is defined):
 *
 * (1) Number of prefetch engines enabled
 * The number of prefetch engines enabled can range from none (disabled)
 * to 3.  This is controlled with the MSSCR0_L2_PREFETCH_ENGINES definition.
 * The default number of prefetch engines is 1 (MSSCR0_L2PFE_ONE). Valid
 * settings include: MSSCR0_L2PFE_NONE, MSSCR0_L2PFE_TWO, and
 * MSSCR0_L2PFE_THREE.
 *
 * (2) Enable/Disable L2 Data Parity Checking
 * L2 data parity checking can be enabled/disabled with the
 * ENABLE_L2_DATA_PARITY definition.  Parity checking is enabled by
 * default.
 *
 * (3) Mode
 * The L2 cache can be configured for Instruction-only mode, Data-only
 * mode, or both (instructions and data) with the L2_MODE definition.
 * The default is to allow both instructions and data (L2_MODE_INST_AND_DATA).
 * Valid settings also include: L2_MODE_I_ONLY and L2_MODE_D_ONLY.
 *
 * (4) L2 Replacement Algorithm
 * The L2 cache replacement algorithm can be configured for Default replacement
 * algorithm or Secondary replacement algorithm.  The default is
 * to use the default replacement algorithm (L2_REP_ALG_DEFAULT).  To use
 * the secondary algorithm set L2_REP_ALG to L2_REP_ALG_SECONDARY.
 */

#define	ENABLE_L2_DATA_PARITY
#define	MSSCR0_L2_PREFETCH_ENGINES   MSSCR0_L2PFE_ONE
#define	L2_MODE                      L2_MODE_INST_AND_DATA
#define	L2_REP_ALG                   L2_REP_ALG_DEFAULT

#endif

/* 
 * Timestamp Support 
 *
 * Timestamp option not included by default:  #define to include it 
 */

#define  INCLUDE_TIMESTAMP

/* 
 * Network Support
 * Include the enhanced network driver.
 */

#ifdef INCLUDE_NETWORK
#    define INCLUDE_END      /* Enhanced Network Driver (see configNet.h) */
#endif /* INCLUDE_NETWORK */

/*
 * System Memory (DRAM) Support 
 *
 * By default, the available DRAM memory is sized at bootup (LOCAL_MEM_AUTOSIZE
 * is defined).  If auto-sizing is not selected, make certain that
 * LOCAL_MEM_SIZE is set to the actual amount of memory on the board.
 * By default, it is set to the minimum memory configuration: 32 MB.
 * Failure to do so can cause unpredictable system behavior!
 */

#define LOCAL_MEM_AUTOSIZE			/* undef for fixed size */
#define LOCAL_MEM_LOCAL_ADRS	0x00000000	/* fixed at zero */
#define LOCAL_MEM_SIZE		0x02000000	/* Default: Min memory: 32MB */

#define RAM_HIGH_ADRS		0x00800000  	/* RAM address for ROM boot */
#define RAM_LOW_ADRS		0x00100000	/* RAM address for kernel */

/* user reserved memory, see sysMemTop() */
#define USER_RESERVED_MEM	(0x0)		/* number of reserved bytes */

/*
 * NOTE: The constants ROM_TEXT_ADRS, ROM_WARM_ADRS, ROM_SIZE (defined in
 * prpmc880A.h), RAM_LOW_ADRS and RAM_HIGH_ADRS are defined in Makefile as well.
 * All definitions for these constants must be identical.
 *
 * ROM_WARM_ADRS is the boot ROM warm entry address which is set to 4 bytes
 * beyond the cold boot entry point ROM_TEXT_ADRS.
 */

/* System (Socketed) FLASH Support */

#define ROM_TEXT_ADRS	(ROM_BASE_ADRS + 0x100)
#define ROM_WARM_ADRS	(ROM_TEXT_ADRS + 0x004)	

/*
 * Main cause interrupt priority table defines the order in which bits
 * in the two "interrupt main cause" registers (low and high) are checked
 * when an interrupt occurs.  The table is organized with paired-entries.
 * The first component of the pair is the low or high cause register
 * designation, the second component of the pair is the bit number within
 * that register.  The ordering of the table specifies the order in which
 * bits are checked in the cause registers when an interrupt occurs.  
 * Entries at the beginning of the table are checked first so to give an
 * interrupt cause bit a higher software priority, put it near the beginning
 * of the list.
 */

#define ICI_MAIN_INT_PRIORITIES           \
	    ICI_MICL_INT_NUM_0,  ICI_MICL_INT_NUM_1, \
	    ICI_MICL_INT_NUM_2,  ICI_MICL_INT_NUM_3, \
	    ICI_MICL_INT_NUM_4,  ICI_MICL_INT_NUM_5, \
	    ICI_MICL_INT_NUM_6,  ICI_MICL_INT_NUM_7, \
	    ICI_MICL_INT_NUM_8,  ICI_MICL_INT_NUM_9, \
	    ICI_MICL_INT_NUM_10, ICI_MICL_INT_NUM_11, \
	    ICI_MICL_INT_NUM_12, ICI_MICL_INT_NUM_13, \
	    ICI_MICL_INT_NUM_14, ICI_MICL_INT_NUM_15, \
	    ICI_MICL_INT_NUM_16, ICI_MICL_INT_NUM_17, \
	    ICI_MICL_INT_NUM_18, ICI_MICL_INT_NUM_19, \
	    ICI_MICL_INT_NUM_20, ICI_MICL_INT_NUM_21, \
	    ICI_MICL_INT_NUM_22, ICI_MICL_INT_NUM_23, \
	    ICI_MICL_INT_NUM_24, ICI_MICL_INT_NUM_25, \
	    ICI_MICL_INT_NUM_26, ICI_MICL_INT_NUM_27, \
	    ICI_MICL_INT_NUM_28, ICI_MICL_INT_NUM_29, \
	    ICI_MICL_INT_NUM_30, ICI_MICL_INT_NUM_31, \
	    ICI_MICH_INT_NUM_0,  ICI_MICH_INT_NUM_1, \
	    ICI_MICH_INT_NUM_2,  ICI_MICH_INT_NUM_3, \
	    ICI_MICH_INT_NUM_4,  ICI_MICH_INT_NUM_5, \
	    ICI_MICH_INT_NUM_6,  ICI_MICH_INT_NUM_7, \
	    ICI_MICH_INT_NUM_8,  ICI_MICH_INT_NUM_9, \
	    ICI_MICH_INT_NUM_10, ICI_MICH_INT_NUM_11, \
	    ICI_MICH_INT_NUM_12, ICI_MICH_INT_NUM_13, \
	    ICI_MICH_INT_NUM_14, ICI_MICH_INT_NUM_15, \
	    ICI_MICH_INT_NUM_16, ICI_MICH_INT_NUM_17, \
	    ICI_MICH_INT_NUM_18, ICI_MICH_INT_NUM_19, \
	    ICI_MICH_INT_NUM_20, ICI_MICH_INT_NUM_21, \
	    ICI_MICH_INT_NUM_22, ICI_MICH_INT_NUM_23, \
	    ICI_MICH_INT_NUM_24, ICI_MICH_INT_NUM_25, \
	    ICI_MICH_INT_NUM_26, ICI_MICH_INT_NUM_27, \
	    ICI_MICH_INT_NUM_28, ICI_MICH_INT_NUM_29, \
	    ICI_MICH_INT_NUM_30, ICI_MICH_INT_NUM_31, \
	    0xff
	    
/* 
 * PCI interrupt routing PCI_x_Dyy_ROUTE (x = bus, yy = device)
 * 0xff,0xff,0xff,0xff = not wired to PCI interrupts
 */

#define PCI0_D00_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D01_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D02_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D03_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D04_ROUTE		PCI0_INTA,PCI0_INTB,PCI0_INTC,PCI0_INTD   /* device number 20 DEC PCI-PCI bridge */
#define PCI0_D05_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D06_ROUTE		PCI0_INTD,0xff,0xff,0xff  /* device number 6 for GEI82543 in tundra */
#define PCI0_D07_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D08_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D09_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D10_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D11_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D12_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D13_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D14_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D15_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D16_ROUTE		PCI0_INTB, PCI0_INTC, PCI0_INTD, PCI0_INTA  /* device number 16, PMC slot 1 */
#define PCI0_D17_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D18_ROUTE		PCI0_INTC,0xff,0xff,0xff  /* device number 18 PMC 1 Alt Device */
#define PCI0_D19_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D20_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D21_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D22_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D23_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D24_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D25_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D26_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D27_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D28_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D29_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D30_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D31_ROUTE		0xff,0xff,0xff,0xff


/* Shared-memory Backplane Network parameters */

/*
 * INCLUDE_SM_NET and INCLUDE_SM_SEQ_ADDR are the shared memory backplane
 * driver and the auto address setup which are defined in configAll.h.
 * To exclude them, uncomment the following lines:
 *
 * #undef INCLUDE_SM_NET
 * #undef INCLUDE_SM_SEQ_ADDR
 */
#undef INCLUDE_SM_NET
#undef INCLUDE_SM_SEQ_ADDR
 
#ifdef  INCLUDE_SM_NET
#define INCLUDE_SM_COMMON
/* #define INCLUDE_SM_OBJ */
#endif  /* INCLUDE_SM_NET */
 
/* Local and backplane bus numbers - see SYS_SM_BUS_NUMBER */

#define SYS_LOCAL_PCI_BUS_NUMBER 	0
#define SYS_BACKPLANE_BUS_NUMBER    1 

#if defined(INCLUDE_SM_COMMON)

#   define STANDALONE_NET
#   define INCLUDE_NET_SHOW
#   define INCLUDE_BSD

#   define SM_OFF_BOARD		false       /* Memory pool is off-board */

   /* 
    * When shared memory anchor polling is enabled, the following defines the
    * PCI bus number on which to poll devices for the shared memory anchor.
    * If the PrPCM880 must cross over the backplane bus to access the anchor,
    * then #define  SYS_SM_BUS_NUMBER SYS_BACKPLANE_BUS_NUMBER .
    * If the PrPMC800 can stay on the local PCI bus to reach the anchor
    * then #define  SYS_SM_BUS_NUMBER SYS_LOCAL_PCI_BUS_NUMBER .
    */

#    define SYS_SM_BUS_NUMBER	SYS_LOCAL_PCI_BUS_NUMBER

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
 * In a similar manner to the "main interrupt priorities", below
 * is a list representing the GPP interrupt priorities.  Each element in the
 * list is a GPP interrupt number.  Each number represents a bit
 * position in the GPP interrupt cause register.  When a GPP interrupt
 * occurs (via bits 24, 25, or 26 of the main interrupt cause register),
 * the GPP interrupt handler will examine the GPP local cause bits in
 * the order specified by the following list.  By rearranging the
 * ordering of bits below the priority in which GPP interrupts are
 * checked can be modified.
 */

#define	GPP_LOCAL_INT_PRIORITIES \
	    0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7, \
	    0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf, \
	    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17, \
	    0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f, \
	    0xff		/* end of list */

#include "prpmc880A.h"	/* board specific definitions */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

/*#if defined(PRJ_BUILD)*/
#include "prjParams.h"
/*#endif*/ /* PRJ_BUILD */

#include "prpmc880B.h"	  /* definition dependencies */

#endif	/* __INCconfigh */
