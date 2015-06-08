/* config.h - Motorola PowerPlus board configuration header */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc. */

/*
modification history
--------------------
01r,17jul02,kab  SPR 76290: Correct INCLUDE_AUXCLK
01q,14jun02,kab  SPR 73457, 73642: SM defs
01p,25oct01,kab	 Update for T2.2
01o,30may01,dtr  Removing define INTERRUPT_ROUTING_TABLE putting in
                 sysBusPci.c.
01n,10may01,pch  Add INCLUDE_ALTIVEC
01m,25apr01,srr  Change revision number to 0.
01l,09mar01,rhk  remove duplicate defines, WRS coding standards cleanup.
01k,31oct00,cak  Corrected BSP version.
01j,12oct00,cak  Bumped revision for new release.
01i,16aug00,dmw  Code review cleanup.
01h,31jul00,cak  Added TrueFFS macros
01g,19jun00,dmw  Added 895 Ultra2 SCSI defines.
01f,08jun00,dmw  Bumped revision for patch.
01e,26may00,dmw  RTC Watchdog support.
01d,19apr00,dmw  Turned on L2 cache.
01c,10apr00,dmw  Added Universe support.
01b,03apr00,dmw  Added PMCSpan interrupt lines to routing table.
01a,09mar00,dmw  Written. (from ver 01i, mcpn765/config.h)
*/

/*
This file contains the configuration parameters for the Motorola mv5100 BSP.
*/

#ifndef	INCconfigh
#define	INCconfigh

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* The following defines must precede configAll.h */

/* BSP version/revision identification */

#define BSP_VER_1_1     1
#define BSP_VER_1_2     1
#define BSP_VERSION     "1.2"
#define BSP_REV         "/2"		/* 0 for first revision */



/*Sergey*/
#undef MYDEBUG



/* PRIMARY INCLUDES */

#include "configAll.h"

/* defines */

#define DEFAULT_BOOT_LINE \
	"fei(0,0)host:/tornado/mv5100/vxWorks h=90.0.0.1 e=90.0.0.2 u=vxworks"
#define WRONG_CPU_MSG "A PPC604 VxWorks image cannot run on a PPC603!\n";

/*
 * If any slave board in the chassis cannot generate a VMEbus
 * RMW, or the master board cannot translate an incoming VMEbus
 * RMW into an atomic operation, define ANY_BRDS_IN_CHASSIS_NOT_RMW
 * else
 * All slave boards are generating a VMEbus RMW, and the master
 * board can translate an incoming VMEbus RMW into an atomic
 * operation, so undef ANY_BRDS_IN_CHASSIS_NOT_RMW
 *
 * Some combinations of UniverseII and Hawk generate a runt or 
 * mistimed bus cycle on 64-bit RMW of VME addrs, leading to lockup
 * (typically of the master).  If this is the case for any board 
 * in the chassis, then ANY_BRDS_IN_CHASSIS_NOT_RMW must be defined
 * for all boards: do not use HW TAS for such a system.
 */

#define	ANY_BRDS_IN_CHASSIS_NOT_RMW

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

#define INCLUDE_ALTIVEC        /* AltiVec support */
#define INCLUDE_CACHE_L2       /* L2 cache support */
#define INCLUDE_ECC            /* Hawk SMC ECC */
#define INCLUDE_BPE            /* Processor bus Parity checking */
#define INCLUDE_DPM            /* Dynamic Power Management */
#define INCLUDE_AUX_CLK        /* Hawk aux clock */
#undef  INCLUDE_SECONDARY_ENET /* Enable Secondary Ethernet */
#undef  INCLUDE_ATA            /* no EIDE support */

#undef  INCLUDE_DEC_END        /* DEC21x4x PMC add-in cards */

#define INCLUDE_NETWORK
#define INCLUDE_FEI_END        /* WRS generic end driver */
#undef  INCLUDE_MCG_END        /* MCG support for i82559 devices */

/* Sergey: undef */
#define EXTENDED_VME           /* CHRP-like addr map for larger VME windows */

/* Sergey: include Universe DMA */
#define INCLUDE_VME_DMA

#undef  INCLUDE_IPMC761        /* IPMC761 support */

#ifdef INCLUDE_IPMC761
#    define INCLUDE_I8250_SIO  /* COM1 thru COM4 are 16C550 UARTS */
#    define INCLUDE_Z85230_SIO /* z8536/z85230 async/sync COM ports */
#    define INCLUDE_SCSI       /* SCSI support */
#endif /* INCLUDE_IPMC761 */

#undef INCLUDE_TFFS            /* True Flash File System support */

#undef  INCLUDE_TFFS            /* True Flash File System support */
#ifdef  INCLUDE_TFFS
#    define INCLUDE_SHELL
#    define INCLUDE_NET_SYM_TBL
#    define INCLUDE_LOADER
#endif /* INCLUDE_TFFS */

#undef INCLUDE_M48T559WDT     /* Watchdog timer support. */

    /*
     * NONFATAL_VPD_ERRORS is a debug build switch which, when defined, allows
     * the BSP to tolerate what would otherwise be fatal VPD errors.
     */

#undef  NONFATAL_VPD_ERRORS  /* define to debug VPD info */

    /*
     * BYPASS_VPD_PCO is a debug build switch which, when defined, causes the
     * BSP to ignore the VPD Product Configuration Options bit array and use
     * a default configuration which assumes one serial port (debug console) and
     * one Ethernet device (Unit 0).
     */

#undef  BYPASS_VPD_PCO       /* define to ignore VPD PCO */

    /*
     * BYPASS_SPD is a debug build switch which, when defined, forces the BSP
     * to ignore SPD information and assume a 32MB SDRAM array and configures
     * the Hawk SMC for default (worst case) memory timing. It will also
     * produce a Group A 0x40000000 Bootrom Error message at startup as a
     * reminder that default timing is in use.
     */

#undef  BYPASS_SPD	    /* define to debug SPD info */

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
 * NOTE: Since PCI auto-configuration is performed by the bootroms,
 *       changing any of these values requires the creation of new bootroms.
 */

#ifndef EXTENDED_VME

    /*
     * the pseudo-PReP map (optional) maximizes available PCI space and
     * has the following maximum PCI size limitations:
     *
     * PCI_MSTR_IO_SIZE = PCI_MSTR_MEMIO_LOCAL (0xc0000000)
     *                    - ISA_MSTR_IO_LOCAL  (0x80000000)
     *                    - ISA_MSTR_IO_SIZE   (0x00010000)
     *
     * therefore: the maximum value of PCI_MSTR_IO_SIZE = 0x3fff0000
     *
     * in the PCI memory space:
     *
     * (PCI_MSTR_MEMIO_SIZE
     *  + PCI_MSTR_MEM_SIZE) <= VME_A32_MSTR_LOCAL     (0xd8000000)
     *                          - PCI_MSTR_MEMIO_LOCAL (0xc0000000)
     *
     * therefore: the maximum value of
     * (PCI_MSTR_MEMIO_SIZE + PCI_MSTR_MEM_SIZE) = 0x18000000
     */

#   define PCI_MSTR_IO_SIZE     0x00800000      /* 8MB */
#   define PCI_MSTR_MEMIO_SIZE  0x00800000      /* 8MB */
#   define PCI_MSTR_MEM_SIZE    0x01000000      /* 16MB */


#else /* EXTENDED_VME */

    /*
     * the extended VME map (default) maximizes VME A32 space and has the
     * following maximum PCI size limitations:
     *
     * PCI_MSTR_IO_SIZE = HAWK_SMC_BASE_ADRS   (0xfef80000)
     *                    - ISA_MSTR_IO_LOCAL  (0xfe000000)
     *                    - ISA_MSTR_IO_SIZE   (0x00010000)
     *
     * therefore: the maximum value of PCI_MSTR_IO_SIZE = 0x00f70000
     *
     * in the PCI memory space:
     *
     * (PCI_MSTR_MEMIO_SIZE
     *  + PCI_MSTR_MEM_SIZE) = ISA_MSTR_IO_LOCAL      (0xfe000000)
     *                         - PCI_MSTR_MEMIO_LOCAL (0xfd000000)
     *
     * therefore: the maximum value of
     * (PCI_MSTR_MEMIO_SIZE + PCI_MSTR_MEM_SIZE) = 0x01000000
     */

#   define PCI_MSTR_IO_SIZE     0x00800000      /* 8MB */
#   define PCI_MSTR_MEMIO_SIZE  0x00800000      /* 8MB */


    /* allocate the remaining available space to PCI Memory space */

#   define PCI_MSTR_MEM_SIZE    (ISA_MSTR_IO_LOCAL-PCI_MSTR_MEM_LOCAL)
#endif /* EXTENDED_VME */

/*
 * VME A32 master window,
 *
 * The standard window size (as shipped) is 128 MB.  This is the largest
 * size recommended for use with TLBs and is the largest size allowed with
 * extended VME addressing disabled.  Use of only BAT registers can increase
 * the size to 1 GB.  For larger sizes, TLBs and BATs must be used together.
 *
 * Memory address granularity is 64 KB for TLBs while BATs are restricted to
 * the following fixed sizes: 128 KB, 256 KB, 512 KB, 1 MB, 2 MB, 4 MB, 8 MB,
 * 16 MB, 32 MB, 64 MB, 128 MB or 256 MB each.  Because there are only 4 BATs
 * for data address translation, a maximum of 4 x 256 MB = 1 GB can be
 * mapped this way.
 *
 * Memory usage by TLBs is costly.  For each 128 MB of memory to be mapped,
 * 1 MB of memory is used by TLBs.  A TLB array for 1 GB would require 8 MB!
 */

/* 
 * VME A32 start address.  This value must be larger than the amount of local
 * DRAM when using the Extended VME address map.
 */

#ifdef EXTENDED_VME
#    define VME_A32_MSTR_LOCAL	0x20000000	/* 512MB default */
#else
#    define VME_A32_MSTR_LOCAL	0xd8000000	/* After PCI I/O */
#endif

/* VME Master access windows, set size to 0 to disable a window */

#define	VME_A32_MSTR_BUS	0x08000000	/* Base VME address */
#define VME_A32_MSTR_SIZE	0x08000000
 
/* VME A24 master window, (16MB) */

#define	VME_A24_MSTR_BUS	0x00000000	/* Base VME address */
#define VME_A24_MSTR_SIZE	0x01000000	/* 0 to 16MB, mult of 64K */

/* VME A16 Master window (64KB) */

#define VME_A16_MSTR_SIZE	0x00010000	/* either 0 or 64KB, only */

/* Setup VME slave windows, set size to 0 to disable a window */

/*
 * VME A32 slave window, default is to map all local memory to VMEbus.
 * The window size is set to LOCAL_MEM_SIZE.  This will reduce the window
 * size if LOCAL_MEM_SIZE does not reflect the actual amount of memory.
 * Hence, if all of physical memory is to be addressed in the A32 window,
 * the actual board memory size should be set in LOCAL_MEM_SIZE.
 */

#define VME_A32_SLV_LOCAL    LOCAL_MEM_LOCAL_ADRS
#define VME_A32_SLV_BUS	     VME_A32_MSTR_BUS
#define VME_A32_SLV_SIZE     (((ULONG)sysPhysMemTop()) - (LOCAL_MEM_LOCAL_ADRS))

/* VME A24 slave window, default is to not enable */

#define VME_A24_SLV_LOCAL       LOCAL_MEM_LOCAL_ADRS
#define VME_A24_SLV_BUS         VME_A24_MSTR_BUS
#ifdef A24_SLV_WINDOW
#   define VME_A24_SLV_SIZE     VME_A24_MSTR_SIZE   /* set to max, 16MB */
#else
#   define VME_A24_SLV_SIZE     0x0     /* 0, window is disabled */
#endif

/* VME A16 slave window, no default support */


#ifdef	INCLUDE_VME_DMA
/* Default DMA operating parameters used by sysVmeDmaInit() */

/*
 * VMEbus data transfer type
 * - a combination of VMEbus Max. Datawidth (VDM) and
 *   VMEbus Cycle Type (VCT).
 *
 *	choices:
 *		(DCTL_VDW_8  | DCTL_VCT_SINGLE)
 *		(DCTL_VDW_16 | DCTL_VCT_SINGLE)
 *		(DCTL_VDW_32 | DCTL_VCT_SINGLE)
 *		(DCTL_VDW_64 | DCTL_VCT_SINGLE)
 *		(DCTL_VDW_32 | DCTL_VCT_BLK)	** BLT  **
 *		(DCTL_VDW_64 | DCTL_VCT_BLK)	** MBLT **
 */

#   define VME_DMA_XFER_TYPE	(DCTL_VDW_32 | DCTL_VCT_SINGLE)

/*
 * VMEbus Address Space type
 *	choices:
 *		DCTL_VAS_A16
 *		DCTL_VAS_A24
 *		DCTL_VAS_A32
 */

#   define VME_DMA_ADDR_SPACE	DCTL_VAS_A32

/*
 * Program/Data Address Modifier (AM) Code
 *	choices:
 *		DCTL_PGM_DATA
 *		DCTL_PGM_PRGM
 */

#   define VME_DMA_DATA_TYPE	DCTL_PGM_DATA

/*
 * Supervisor/User Address Modifier (AM) Code
 *	choices:
 *		DCTL_SUPER_USER
 *		DCTL_SUPER_SUP
 */

#   define VME_DMA_USER_TYPE	DCTL_SUPER_SUP

/*
 * VMEbus Aligned DMA Transfer Count (in bytes)
 * - Max. data transferred before giving up the VMEbus
 *	choices:
 *		DGCS_VON_DONE
 *		DGCS_VON_256
 *		DGCS_VON_512
 *		DGCS_VON_1024
 *		DGCS_VON_2048
 *		DGCS_VON_4096
 *		DGCS_VON_8192
 *		DGCS_VON_16384
 */

#   define VME_DMA_MAX_BURST		DGCS_VON_DONE

/*
 * Minimum period the DMA is off the VMEbus between tenures (in microseconds)
 *	choices:
 *		DGCS_VOFF_0
 *		DGCS_VOFF_16
 *		DGCS_VOFF_32
 *		DGCS_VOFF_64
 *		DGCS_VOFF_128
 *		DGCS_VOFF_256
 *		DGCS_VOFF_512
 *		DGCS_VOFF_1024
 */

#   define VME_DMA_MIN_TIME_OFF_BUS	DGCS_VOFF_0

#endif	/* INCLUDE_VME_DMA */

#undef DISABLE_SPRQ	/* not using 21554 as PCI bus arbiter (don't have one) */

/*
 * static define for ethernet chip interrupt. Only used if PCI Auto-Config
 * is disabled (debug).
 */
 
#define LN_INT_LVL	PCI_INT_LVL4

/* MMU and Cache options */

#define	INCLUDE_MMU_BASIC       /* bundled mmu support */
#undef	USER_D_CACHE_MODE
#define	USER_D_CACHE_MODE       (CACHE_COPYBACK | CACHE_SNOOP_ENABLE)

#ifdef  INCLUDE_CACHE_L2
#   define USER_L2_CACHE_ENABLE	/* enable the L2 cache */
#endif  /* INCLUDE_CACHE_L2 */

/* timestamp option not included by default;  #define to include it */

#undef  INCLUDE_TIMESTAMP

/* De-select unused (default) network drivers selected in configAll.h */

#undef  INCLUDE_EX              /* include Excelan Ethernet interface */
#undef  INCLUDE_ENP             /* include CMC Ethernet interface*/

#ifdef INCLUDE_NETWORK
#define	INCLUDE_END		/* Enhanced Network Driver (see configNet.h) */
#endif /* INCLUDE_NETWORK */

/* Primary SCSI support */

#ifdef INCLUDE_SCSI
#   define  INCLUDE_SCSI2       /* Use SCSI2 library, not SCSI1 */
#   define  INCLUDE_SCSI_BOOT   /* include ability to boot from SCSI */
#   define  INCLUDE_DOSFS       /* file system to be used */
#   undef   SCSI_AUTO_CONFIG    /* scan bus for devices on startup */
#   undef   SCSI_WIDE_ENABLE	/* enable wide SCSI, 16-bit data xfrs */
#   define  SYS_SCSI_CONFIG     /* call sysScsiConfig in sysScsi.c */
#   define  INCLUDE_SYM_895     /* Symbios 895 support */
#   undef   ULTRA2_SCSI         /* Ultra2 SCSI */
#endif /* INCLUDE_SCSI */

/* PCI bus numbers for secondary and subordinate buses */

#define PCI_SECONDARY_BUS       1
#define PCI_SUBORD_BUS          1
#define PCI_MAX_BUS             2       /* Max. number of PCI buses in system */

/* serial parameters */

#undef	NUM_TTY
#define	NUM_TTY			N_SIO_CHANNELS

/*
 * Auxiliary Clock support is an optional feature that is not supported
 * by all BSPs.  Since it requires the MPIC timers, the Hawk Auxiliary Clock
 * is only supported in MPIC mode.
 */

#if defined(INCLUDE_AUX_CLK) 
#    define INCLUDE_HAWK_AUXCLK                /* specify aux clock device */
#else
#    undef INCLUDE_SPY
#endif /* INCLUDE_AUX_CLK */

/*
 * INCLUDE_SM_NET and INCLUDE_SM_SEQ_ADDR are the shared memory backplane
 * driver and the auto address setup which are excluded by default.
 * To exclude them, uncomment the following lines:
 *
 * #define INCLUDE_SM_NET
 * #define INCLUDE_SM_SEQ_ADDR
 */

#if defined(INCLUDE_SM_NET)
#   define INCLUDE_BSD
#endif /* defined(INCLUDE_SM_NET) */

#if defined(INCLUDE_END) && !defined(INCLUDE_SM_NET)
#   undef  WDB_COMM_TYPE   /* WDB agent communication path is END device */ 
#   define WDB_COMM_TYPE        WDB_COMM_END    
#endif  /* defined(INCLUDE_END) && !defined(INCLUDE_SM_NET) */

/*
 * Changing SM_OFF_BOARD to TRUE also requires changing
 * SM_ANCHOR_ADRS and SM_MEM_ADRS appropriately.
 */

#define SM_OFF_BOARD	FALSE

#undef  SM_ANCHOR_ADRS
#define SM_ANCHOR_ADRS  ((sysProcNumGet() == 0) ? \
        ((char*) (LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET)) : \
        ((char*) (VME_A32_MSTR_LOCAL + SM_ANCHOR_OFFSET)))

#define SM_INT_TYPE		SM_INT_MAILBOX_1	 /* or SM_INT_NONE */

/*
 * The following defines are only used by the master.
 * If SM_OFF_BOARD == FALSE, these values are used
 * by the master's UNIVERSE_II window definitions, so
 * they need to be on a 64K byte boundary.
 * The slave only uses the "Anchor" address.
 */

#define SM_MEM_ADRS		0x00010000	/* Start of actual SM region */
#define SM_MEM_SIZE		0x00020000
#define SM_OBJ_MEM_ADRS		(SM_MEM_ADRS+SM_MEM_SIZE) /* SM Objects pool */
#define SM_OBJ_MEM_SIZE		0x00010000

#define	INCLUDE_VME		/* undef to exclude VME support */

/*
 * Local Memory definitions
 *
 * By default, the available DRAM memory is sized at bootup (LOCAL_MEM_AUTOSIZE
 * is defined).  If auto-sizing is not selected, make certain that
 * LOCAL_MEM_SIZE is set to the actual amount of memory on the board.
 * By default, it is set to the minimum memory configuration: 32 MB.
 * Failure to do so can cause unpredictable system behavior!
 */

#define	LOCAL_MEM_AUTOSIZE			/* undef for fixed size */
#define LOCAL_MEM_LOCAL_ADRS	0x00000000	/* fixed at zero */
#define LOCAL_MEM_SIZE		0x04000000	/* Default memory: 64MB */

#define RAM_HIGH_ADRS		0x00800000  	/* RAM address for ROM boot */
#define RAM_LOW_ADRS		0x00100000	/* RAM address for kernel */

/* user reserved memory, see sysMemTop() */

#define USER_RESERVED_MEM	(0)	/* number of reserved bytes */

/*
 * The constants ROM_TEXT_ADRS, ROM_SIZE, RAM_LOW_ADRS and RAM_HIGH_ADRS 
 * are defined in config.h, Makefile.
 * All definitions for these constants must be identical.
 */

#undef INCLUDE_MOT_BUG_ROM			/* define this to use */

#ifdef INCLUDE_MOT_BUG_ROM
#   define	ROM_BASE_ADRS	(LOCAL_MEM_SIZE)
#   define	ROM_TEXT_ADRS	ROM_BASE_ADRS
#else
#   define	ROM_BASE_ADRS	0xfff00000	/* base address of ROM */
#   define	ROM_TEXT_ADRS	(ROM_BASE_ADRS + 0x100)
#endif /* INCLUDE_MOT_BUG_ROM */

#define	ROM_SIZE		0x00100000	/* 1 Meg ROM space */

/* PMC Span (PCI Bridge) configuration parameters */

#undef	INCLUDE_PMC_SPAN        /* PMC Span support not included by default */
 
/* PMC Span configuration values */

#define	P2P_IO_BASE              0x01060000	/* PCI I/O window */
#define	P2P_IO_SIZE              0x00040000
#define	P2P_PREF_MEM_BASE        0x02000000	/* PCI prefetch mem window */
#define	P2P_PREF_MEM_SIZE        0x01000000
#define	P2P_PREF_HI32_BASE       0x00000000	/* hi 32 bits of address */
#define	P2P_NONPREF_MEM_BASE     0x03000000	/* PCI non-prefech mem window */
#define P2P_NONPREF_MEM_SIZE	 0x01000000
#define P2P_CACHE_LINE_SIZE      8		/* cache line size */
#define P2P_PRIM_LATENCY         0		/* latency */

/*
 * These defines are used to initialize the External Source
 * Vector/Priority registers in the MPIC.  The following can
 * be defined: interrupt sensitivity, polarity and interrupt priority.
 *
 * Note: by default a 0 into the sense bit(22) will setup for level
 * sensitive, and a 0 into the polarity bit(23) will setup for low
 * polarity.
 *
 * At initialization all external interrupt sources are disabled
 * except for the 8259 input, which is enabled in the MPIC driver.
 *
 * All currently unused interrupt sources are set to a priority of
 * 0, which will not allow them to be enabled.  If any one of these
 * levels is to be used, the priority value must be changed here.
 */

#define INIT_EXT_SRC0           ( INT_MASK_BIT | HIGH_POLARITY | LEVEL_SENSE |\
                                  PRIORITY_LVL9 )   /* PIB (8259) */

#define INIT_EXT_SRC1           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL8 )   /* Com1 */
 
#define INIT_EXT_SRC2           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL14 )  /* PCI-Ethernet */
 
#define INIT_EXT_SRC3           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* Hawk WDT1/WDT2 */
 
#define INIT_EXT_SRC4           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* not used */

#define INIT_EXT_SRC5           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL10 )  /* Universe LINT0 */
 
#define INIT_EXT_SRC6           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* Universe LINT1 */
 
#define INIT_EXT_SRC7           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* Universe LINT2 */
 
#define INIT_EXT_SRC8           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* Universe LINT3 */
 
#define INIT_EXT_SRC9           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL7 )  /* PCI PMC1A/PMC2B/PCIXA */

#define INIT_EXT_SRC10          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL7 )  /* PCI PMC1B/PMC2C/PCIXB */

#define INIT_EXT_SRC11          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL7 )  /* PCI PMC1C/PMC2D/PCIXC */

#define INIT_EXT_SRC12          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL7 )  /* PCI PMC1D/PMC2A/PCIXD */
 
#define INIT_EXT_SRC13          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL13 )  /* PCI-Ethernet 2 */
 
#define INIT_EXT_SRC14          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* Abort_L */
 
#define INIT_EXT_SRC15          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL6 )   /* RTC - Alarm */

#ifdef INCLUDE_TFFS
#    define INCLUDE_SHOW_ROUTINES
#    define INCLUDE_TFFS_SHOW
#    define INCLUDE_DOSFS
#endif  /* INCLUDE_TFFS */

/* BSP-specific includes */

#include "mv5100.h"

/* 
 * The mv5100 has two Intel 82559 Ethernet devices on-board.  The "switchable"
 * (front or rear panel) connected Ethernet device's IDSEL is AD14 and is 
 * intended to be the primary Ethernet device.  The rear panel only Ethernet 
 * device's IDSEL is AD19.  The following macro is left for the user to 
 * determine the primary Ethernet device to be used as the "boot" device.
 */

#define PCI_IDSEL_PRI_LAN	14
#define PCI_IDSEL_SEC_LAN	19

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif	/* INCconfigh */

/*#if defined(PRJ_BUILD)*/
#include "prjParams.h"
/*#endif*/ /* PRJ_BUILD */


