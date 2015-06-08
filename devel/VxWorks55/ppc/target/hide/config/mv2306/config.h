/* config.h - Motorola PowerPlus board configuration header */

/* Copyright 1984-2002 Wind River Systems, Inc. */
/* Copyright 1996-1998 Motorola, Inc. */

/*
modification history
--------------------
02p,25jun02,kab  SPR 77883: Workaround for SECONDARY_ENET (temp)
02o,15may02,sbs  changed INCLUDE_AUXCLK to INCLUDE_AUX_CLK
02n,06may02,sbs  updating bsp revision
02m,16jan02,yvp  Increased RAM_HIGH_ADRS to 0x00300000 - prevent build errors.
02l,11oct99,dmw  Moved VME_A32_MSTR_LOCAL here from mv2600.h
02k,18aug99,dmw  Added PRIMARY_ENET_ID and SECONDARY_ENET_ID
02j,30mar99,dat  chg'd P2P_PRIM_LATENCY to 0xff. (SPR 24507)
02i,09mar99,sbs  corrected documentation for Raven Mpic LEVEL_SENSE
                 default value (SPR #21995) 
02h,07aug98,tb   Added VMEbus DMA support
02g,07aug98,tb   Fixed support for SM_OFF_BOARD
02f,07aug98,tb	 Turned on Falcon's ECC
02e,07aug98,tb   Added TAS support which uses the VMEbus RMW
02d,13oct98,dat  SPR 20654, added ravenAuxClk for mv2300
02c,11aug98,sbs  changed WDB_COMM_TYPE to WDB_COMM_END.
02b,21jul98,db   changed BSP_VERSION to "1.2" and BSP_REVISION to "/0".
		 added BSP_VER_1_2 macro, defined INCLUDE_END, removed
		 END_OVERRIDE(Tornado 2.0 release).
02a,18may98,ms_  change ROM_SIZE to 1 Meg (0x00100000) (SPR 21240)
01z,15apr98,db   added support for floppy disk driver.
01y,13apr98,ms_  merged Motorola mv2700 support
01x,08apr98,dat  REV 5, new generic pciConfigLib and if_dc.o modules
01w,03oct97,mas  assigned non-zero MPIC priorities to all PCI ints (SPR 9388).
01v,18sep97,dds  added macro SCSI_WIDE_ENABLE.
01u,14aug97,mas  Extended VME addressing is now default; no slave VME A24 space
		 (for cumulative patch release 9/97; BSP_REV 4).
01t,24jul97,mas  added MPIC priority scheme (SPR 8956).
01s,20jul97,dat  REV 3 for jumbo patch 2, chg'd SM_ANCHOR_ADRS (SPR 9019).
01r,17jul97,mas  added undef of LOCAL_MEM_AUTOSIZE, refined definition of
		 VME_A32_SLV_SIZE (SPR 8824).
01q,09jul97,mas  NCR810_DELAY_MULT now defined in mv2600.h (SPR 8842).
01p,17jun97,mas  merged w/SENS version of BSP.
01o,05jun97,mas  added support for serial ports 3 & 4 (Z85230 ESCC). (SPR 8566)
01n,29may97,srr  Added comments about shared memory setup (MCG MR #74)
01m,12may97,dat  moving things between config.h and mv2600.h
01l,09may97,mas  added extended VME addressing (SPR 8410).
		 added NCR810_DELAY_MULT (SPR 8430).
01k,28apr97,gnn  Added documentation for END_OVERRIDE and INCLUDE_END
01j,25apr97,map  BSP_REV is 2. Added SENS support.
01i,23apr97,mas  added MPIC support from Motorola (SPR 8170).
01h,11apr97,dat  moved macros around, user changeable macros are here,
		 not-changeable ones are in mv2600.h
01g,10apr97,mas  added PCI I/O function defines and PMC Span support (SPR 8226).
01f,01apr97,dat  new VME window configuration macros, SPR 8271
01e,18feb97,mas  added undef of INCLUDE_TIMESTAMP (SPR 7772, 7811).
01d,03jan97,mas  BSP_REV set to "/0".
01c,02jan97,dat  default does not include SCSI, fixed mod history
01b,19oct96,srr  Replaced CPU_SPEED with MEMORY_BUS_SPEED to be compatible
		 with change made by Wind River.
01a,01oct96,mot  written. (from ver 01q, mv1600/config.h)
*/

/*
This file contains the configuration parameters for the
Motorola PowerPlus architecture
*/

#ifndef	INCconfigh
#define	INCconfigh

#ifdef __cplusplus
extern "C" {
#endif

/* The following defines must precede configAll.h */

/* BSP version/revision identification */

#define BSP_VER_1_1	1	/* 1.2 is backward compatible with 1.1 */
#define BSP_VER_1_2	1
#define BSP_VERSION     "1.2"
#define BSP_REV         "/1"		/* 0 for first revision */

/* PRIMARY INCLUDES */

#include "configAll.h"

/* defines */

#if (CPU == PPC603)
#   define DEFAULT_BOOT_LINE \
	"dc(0,0)host:/tornado/mv2603/vxWorks h=90.0.0.1 e=90.0.0.2 u=vxworks"
#   define WRONG_CPU_MSG "A PPC603 VxWorks image cannot run on a PPC604!\n";

#elif (CPU == PPC604)

#   define DEFAULT_BOOT_LINE \
	"dc(0,0)host:/tornado/mv2604/vxWorks h=90.0.0.1 e=90.0.0.2 u=vxworks"
#   define WRONG_CPU_MSG "A PPC604 VxWorks image cannot run on a PPC603!\n";
#endif	/* (CPU == PPC604) */

/*
 * If any slave board in the chassis cannot generate a VMEbus
 * RMW, or the master board cannot translate an incoming VMEbus
 * RMW into an atomic operation, define ANY_BRDS_IN_CHASSIS_NOT_RMW
 * else
 * All slave boards are generating a VMEbus RMW, and the master
 * board can translate an incoming VMEbus RMW into an atomic
 * operation, so undef ANY_BRDS_IN_CHASSIS_NOT_RMW
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

#ifdef  MV2600
#    define	INCLUDE_CACHE_L2	/* L2 cache support */
#    define	INCLUDE_I8250_SIO       /* COM1 and COM2 via i8250 */
#    define	INCLUDE_Z85230_SIO      /* serial ports 3 & 4 via Z85230 */
#    define	INCLUDE_PRIMARY_ENET	/* DEC 21140 */
#    undef	INCLUDE_SECONDARY_ENET	/* [NOT AVAILABLE] */
#    undef	INCLUDE_PRIMARY_SCSI	/* NCR 825 */
#    undef	INCLUDE_SECONDARY_SCSI	/* [NOT AVAILABLE] */
#    define	INCLUDE_AUX_CLK		/* Z 8536 */
#    undef	INCLUDE_VME_DMA		/* VME DMA support */
#    undef 	INCLUDE_FD		/* Floppy Disk  */
#endif

#ifdef  MV2700
#    define     INCLUDE_CACHE_L2        /* L2 cache support */
#    define     INCLUDE_I8250_SIO       /* COM1 and COM2 via i8250 */
#    define     INCLUDE_Z85230_SIO      /* serial ports 3 & 4 via Z85230 */
#    define     INCLUDE_PRIMARY_ENET    /* DEC 21140 */
#    undef      INCLUDE_SECONDARY_ENET  /* [NOT AVAILABLE] */
#    define     INCLUDE_PRIMARY_SCSI    /* NCR 825 */
#    undef      INCLUDE_SECONDARY_SCSI  /* [NOT AVAILABLE] */
#    define     INCLUDE_AUX_CLK          /* Z 8536 */
#    undef	INCLUDE_VME_DMA		/* VME DMA support */
#    undef 	INCLUDE_FD		/* Floppy Disk */
#endif

#ifdef  MV3600
#    define	INCLUDE_CACHE_L2	/* L2 cache support */
#    define	INCLUDE_I8250_SIO       /* COM1 and COM2 via i8250 */
#    define	INCLUDE_Z85230_SIO      /* serial ports 3 & 4 via Z85230 */
#    define	INCLUDE_PRIMARY_ENET	/* DEC 21140 */
#    undef	INCLUDE_SECONDARY_ENET	/* DEC 21040: SPR 77883 */
#    define	INCLUDE_PRIMARY_SCSI	/* NCR 825 */
#    define	INCLUDE_SECONDARY_SCSI	/* NCR 825 */
#    define	INCLUDE_AUX_CLK		/* Z 8536 */
#    undef	INCLUDE_VME_DMA		/* VME DMA support */
#    undef 	INCLUDE_FD		/* Floppy Disk */
#endif

#ifdef  MV2300
#    undef	INCLUDE_CACHE_L2	/* [NOT AVAILABLE] */
#    define	INCLUDE_I8250_SIO       /* Debug port via i8250 */
#    undef	INCLUDE_Z85230_SIO      /* [NOT AVAILABLE] */
#    define	INCLUDE_PRIMARY_ENET	/* Primary Ethernet */
#    undef	INCLUDE_SECONDARY_ENET	/* [NOT AVAILABLE] */
#    undef	INCLUDE_PRIMARY_SCSI	/* [NOT AVAILABLE] */
#    undef	INCLUDE_SECONDARY_SCSI	/* [NOT AVAILABLE] */
#    define	INCLUDE_AUX_CLK		/* ravenAuxClk.c */
#    undef	INCLUDE_VME_DMA		/* VME DMA support */
#    undef 	INCLUDE_FD		/* [NOT AVAILABLE] */
#endif

#define	PRIMARY_ENET_ID		0 /* Primary Ethernet. first DEC device found */
#define	SECONDARY_ENET_ID	1 /* Secondary Ethernet, second device found */

/* MMU and Cache options */

#define	INCLUDE_MMU_BASIC       /* bundled mmu support */
#undef	USER_D_CACHE_MODE
#define	USER_D_CACHE_MODE       (CACHE_COPYBACK | CACHE_SNOOP_ENABLE)

#ifdef  INCLUDE_CACHE_L2
#    define USER_L2_CACHE_ENABLE	/* enable the L2 cache */
#endif  /* INCLUDE_CACHE_L2 */

/* timestamp option not included by default;  #define to include it */

#undef  INCLUDE_TIMESTAMP

/* De-select unused (default) network drivers selected in configAll.h */

#undef  INCLUDE_EX              /* include Excelan Ethernet interface */
#undef  INCLUDE_ENP             /* include CMC Ethernet interface*/

/* De-select unused advanced network driver support */

#define	INCLUDE_END		/* Enhanced Network Driver (see configNet.h) */

#ifdef  INCLUDE_END
#undef  WDB_COMM_TYPE   /* WDB agent communication path is END device */ 
#define WDB_COMM_TYPE        WDB_COMM_END    
#endif  /* INCLUDE_END */

/*
 * Primary Ethernet = DEC21140 = device unit #0
 *
 * The if_dc driver can handle up to 4 devices.
 * Currently, it is only possible to boot from unit #0.
 */

#ifdef  INCLUDE_PRIMARY_ENET
#define INCLUDE_DC
#define DC_POOL_ADRS    (RAM_LOW_ADRS - DC_POOL_SIZE)   /* memory pool adrs */
#define DC_POOL_SIZE    0x0000C000                      /* memory pool size */
#define DC_MODE         0x08                            /* Dec21140 */
#endif  /* INCLUDE_PRIMARY_ENET */

/* Secondary Ethernet = DEC21040 = device unit #1 */

#ifdef  INCLUDE_SECONDARY_ENET
#define DC1_POOL_ADRS   NONE                            /* memory pool adrs */
#define DC1_POOL_SIZE   NONE                            /* memory pool size */
#define DC1_MODE        NONE                            /* Default: dec21040 */
#endif  /* INCLUDE_SECONDARY_ENET */

/* Primary SCSI support */

#define INCLUDE_SCSI2		/* Use SCSI2 library, not SCSI1 */

#ifdef  INCLUDE_PRIMARY_SCSI
#define INCLUDE_SCSI
#define INCLUDE_SCSI_BOOT	/* include ability to boot from SCSI */
#define INCLUDE_DOSFS		/* file system to be used */
#undef	SCSI_AUTO_CONFIG	/* scan bus on startup */
#undef	SCSI_WIDE_ENABLE	/* enable wide SCSI on MVME761-011 only */
#define	SYS_SCSI_CONFIG		/* call sysScsiConfig in sysScsi.c */
#endif  /* INCLUDE_PRIMARY_SCSI */

/* PCI bus numbers for secondary and subordinate buses */

#define PCI_SECONDARY_BUS       1
#define PCI_SUBORD_BUS          1
#define PCI_MAX_BUS             2       /* Max. number of PCI buses in system */

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
#define P2P_PRIM_LATENCY         0xff		/* latency */

/* serial parameters */

#undef	NUM_TTY
#define	NUM_TTY			N_SIO_CHANNELS

/*
 * Auxiliary Clock support is an optional feature that is not supported
 * by all BSPs.
 */

#ifdef  INCLUDE_AUX_CLK
#ifdef  MV2300
#    define INCLUDE_RAVEN_AUXCLK                /* specify aux clock device */
#else
#    define INCLUDE_Z8536_AUXCLK                /* specify aux clock device */
#endif /* MV2300 */
#else
#    undef INCLUDE_SPY
#endif /* INCLUDE_AUX_CLK */

/* Shared-memory Backplane Network parameters */

/*
 * INCLUDE_SM_NET and INCLUDE_SM_SEQ_ADDR are the shared memory backplane
 * driver and the auto address setup which are defined in configAll.h.  To
 * excluded them uncomment the following lines:
 *
 * #undef  INCLUDE_SM_NET
 * #undef  INCLUDE_SM_SEQ_ADDR
 */

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
 * By default, it is set to the minimum memory configuration: 16 MB.
 * Failure to do so can cause unpredictable system behavior!
 */

#define	LOCAL_MEM_AUTOSIZE			/* undef for fixed size */
#define LOCAL_MEM_LOCAL_ADRS	0x00000000	/* fixed at zero */
#define LOCAL_MEM_SIZE		0x01000000	/* Default: Min memory: 16MB */

#define RAM_HIGH_ADRS		0x00300000  	/* RAM address for ROM boot */
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

/* Extended VME A32 space;  enabled by default, #undef disables it */

#define	EXTENDED_VME

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
#    define VME_A32_MSTR_LOCAL	0x10000000	/* 256MB default */
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

/* VME A24 slave window, no default support */

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

#define	VME_DMA_XFER_TYPE	(DCTL_VDW_32 | DCTL_VCT_SINGLE)

/*
 * VMEbus Address Space type
 *	choices:
 *		DCTL_VAS_A16
 *		DCTL_VAS_A24
 *		DCTL_VAS_A32
 */

#define	VME_DMA_ADDR_SPACE	DCTL_VAS_A32

/*
 * Program/Data Address Modifier (AM) Code
 *	choices:
 *		DCTL_PGM_DATA
 *		DCTL_PGM_PRGM
 */

#define	VME_DMA_DATA_TYPE	DCTL_PGM_DATA

/*
 * Supervisor/User Address Modifier (AM) Code
 *	choices:
 *		DCTL_SUPER_USER
 *		DCTL_SUPER_SUP
 */

#define	VME_DMA_USER_TYPE	DCTL_SUPER_SUP

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

#define	VME_DMA_MAX_BURST		DGCS_VON_DONE

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

#define	VME_DMA_MIN_TIME_OFF_BUS	DGCS_VOFF_0

#endif	/* INCLUDE_VME_DMA */

/* Include MPIC support by default */

#define	INCLUDE_MPIC

/*
 * These defines are used to initialize the External Source
 * Vector/Priority registers in the MPIC.  The following can
 * be defined: interrupt sensitivity, polarity and interrupt priority.
 *
 * Note: by default a 1 into the sense bit(22) will setup for level
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
                                  PRIORITY_LVL8 )   /* PIB (8259) */
 
#define INIT_EXT_SRC1           ( INT_MASK_BIT | PRIORITY_LVL0 ) /* Falcon ECC*/
 
#define INIT_EXT_SRC2           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL14 )  /* primary ethernet */
 
#define INIT_EXT_SRC3           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL3 )   /* primary SCSI */
 
#define INIT_EXT_SRC4           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* graphics */

#define INIT_EXT_SRC5           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL10 )  /* Universe LINT0 */
 
#define INIT_EXT_SRC6           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* Universe LINT1 */
 
#define INIT_EXT_SRC7           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* Universe LINT2 */
 
#define INIT_EXT_SRC8           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* Universe LINT3 */
 
#define INIT_EXT_SRC9           ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL7 )   /* PCI PMC1/PMC2 INTA */

#ifdef INCLUDE_SECONDARY_ENET
# define INIT_EXT_SRC10         ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL13 )  /* PCI INTB/sec ethernet */
#else
# define INIT_EXT_SRC10         ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL6 )  /* PCI INTB */
#endif /* INCLUDE_SECONDARY_ENET */

#ifdef INCLUDE_SECONDARY_SCSI
# define INIT_EXT_SRC11         ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL2 )   /* PCI INTC/sec SCSI */
#else
# define INIT_EXT_SRC11         ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL5 )   /* PCI INTC */
#endif /* INCLUDE_SECONDARY_SCSI */

#define INIT_EXT_SRC12          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL4 )   /* PCI PMC1/PMC2 INTD */
 
#define INIT_EXT_SRC13          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* LM/SIG int 0 */
 
#define INIT_EXT_SRC14          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL15 )  /* LM/SIG int 0/mailbox */
 
#define INIT_EXT_SRC15          ( INT_MASK_BIT | LEVEL_SENSE |\
                                  PRIORITY_LVL0 )   /* NOT USED */
 
 
/* BSP-specific includes */

#include "mv2600.h"


#ifdef __cplusplus
}
#endif

#endif	/* INCconfigh */
/*#if defined(PRJ_BUILD)*/
#include "prjParams.h"
/*#endif*/

