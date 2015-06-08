/* config.h - Motorola MVME6100 board configuration header */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2005 Motorola, Inc. */

/*
modification history
--------------------
01qq,14jan05,efb Changed BSP_REV from /0 to /1.
01pp,07dec04,cak Modifications in anticipation of porting to 6.0.
01oo,28oct04,cak Modifications for Tornado Project support.
01nn,15sep04,cak Formatting fixes.
01mm,03aug04,scb Added INCLUDE_ERROR_HANDLING support.
01ll,07jul04,cak Reenable IPMC support.
01kk,30jun04,cak Added support for the Maxim DS1621 digital thermometer
		 and thermostat.
01jj,15jun04,cak Added some new auxClk definitions while extending the 
		 timer/counter support to support all 4 timer/counters.
01ii,04jun04,cak Added support for the mv64360 watchdog timer.
01hh,27may04,cak Added support for programming a boot image into the boot
		 block of either FLASH bank from the vxWorks kernel. 
01gg,24may04,cak Modified WDB_COMM_TYPE to WDB_COMM_NETWORK for shared
		 memory slave.
01ff,11may04,cak Removed IPMC and SCSI support.
01ee,21apr04,scb INT_USR_EXTRA upgrade.
01dd,21apr04,scb sysBusTas() vs. ANY_BRDS_IN_CHASSIS_NOT_RMW
01cc,15apr04,cak Added TFFS support.
01bb,09apr04,cak Removed extra ALL_BRDS_IN_CHASSIS_RMW definition.
01aa,07apr04,cak Modified commentary in system memory controller support. 
01z,22mar04,cak  Changed BSP_REV from "/0.2" to "/0".
01y,15mar04,cak  Added MV64360 Integrated SRAM to the memory map.
01x,10mar04,cak  Modifications to firmware booting support and split the
		 enabling/disabling of the two ethernets into two 
		 separate definitions..
01w,02mar04,cak  Changed BSP_REV from 0.1 to 0.2.
01v,25feb04,cak  Tempe interrupt support.
01u,24feb04,cak  Modified cache support.
01t,17feb04,cak  Modified memory map.
01s,17feb04,scb  Some shared memory support
01r,03nov03,cak  Upgrade Part I.
01q,04mar03,cak  IPMC SCSI Support.
01p,10jan03,efb  Added MV64360 system controller ethernet defines.
01o,08jan03,cak  IPMC Serial Support.
01n,18sep02,cak  Removed DEBUG_CACHE. 
01m,11sep02,cak  Added INCLUDE_MMU_BASIC and removed DEBUG_SMC. 
01l,05sep02,cak  Removed DEBUG_I2C and also #undef'd INCLUDE_ALTIVEC to
		 fix a known WRS problem that causes us to not be able to
		 get to the kernel prompt if INCLUDE_ALTIVEC is defined. 
01k,20aug03,scb  Fixed GPP interrupt priorities to be 0-relative.
01j,18jul02,cak  Added INCLUDE_ECC to allow the user to enable/disable ECC
		 support in the system memory controller, undefined by default.
01i,11jul02,scb  Adjustment to memory map for flash banks.
01h,25jun02,cak  Removed unused VPD definitions.
01g,14jun02,scb  Host bridge support.
01f,20may02,cak  Added PCI memory map commentary. 
01e,09may02,cak  Added system memory controller support for the MV64360.
01d,25apr02,cak  Cache Support.
01c,15apr02,cak  Added INCLUDE_DPM and ALL_BRDS_IN_CHASSIS_RMW definitions.
01b,10apr02,scb  Add interrupt priority list.
01a,06mar02,cak  Ported. (from ver 01n mcp820/config.h)
*/

/*
This file contains the configuration parameters for the MVME6100
Single Board Computer.  This file must not contain any "dependencies"
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
#define BSP_REV         "/1"

/* PRIMARY INCLUDES */

#include "configAll.h"

/* defines */

#define DEFAULT_BOOT_LINE \
	"geisc(0,0)host:/tornado/mv6100/vxWorks h=90.0.0.1 e=90.0.0.2 u=vxworks"
#define WRONG_CPU_MSG "A PPC604 VxWorks image cannot run on a PPC603!\n"

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
 * IMPORTANT: Having either of the ethernets defined below requires that
 * mv64360End.obj be included on the MACH_EXTRA line in the Makefile. 
 */

#define	INCLUDE_MMU_BASIC	/* MMU Support */
#define	INCLUDE_ECC		/* System Memory ECC Support */
#undef	INCLUDE_AUX_CLK		/* MV64360 aux clock support */
#undef	INCLUDE_MV64360_TMRCNTR	/* MV64360 Timer/Counter Support */
#undef	INCLUDE_MV64360_DMA     /* MV64360 DMA */
#undef	INCLUDE_RTC             /* Real-time clock support */
#undef	INCLUDE_FAILSAFE        /* Failsafe (watchdog) timer support */
#undef  INCLUDE_DPM             /* Processor Dynamic Power Management */
#define	INCLUDE_PRIMARY_ENET	/* LAN1 (geisc0) */
#undef	INCLUDE_SECONDARY_ENET	/* LAN2 (geisc1) */
#undef	INCLUDE_ALTIVEC         /* Support for Altivec coprocessor */
#undef	INCLUDE_SM_NET		/* Shared Memory Support */
#undef	INCLUDE_SM_SEQ_ADDR
#undef	INCLUDE_TFFS_FLASHA	/* TrueFFS Support FLASH Bank A */
#undef	INCLUDE_TFFS_FLASHB	/* TrueFFS Support FLASH Bank B */
#undef	INCLUDE_BOOT_IMAGE_PROGRAM /* FLASH Boot Bank Programming Support */
#undef	INCLUDE_MV64360_WDOG    /* MV64360 Watchdog Timer Support */
#undef	INCLUDE_DS1621		/* Maxim DS1621 Temp Sensor Support */
#undef	INCLUDE_IPMC            /* IPMC761 & IPMC712 Support */
#undef  INCLUDE_ERROR_HANDLING  /* Error handling suite */

/* IPMC Support */

#ifdef INCLUDE_IPMC
#   define INCLUDE_PCX7307_SIO  /* Additional 16550-compatible COM Ports */
#   define INCLUDE_Z85230_SIO   /* Z8536/X85230 synchronous COM Ports */
#   define INCLUDE_SCSI         /* SCSI Support */
#endif /* INCLUDE_IPMC */

#if ((defined INCLUDE_PRIMARY_ENET) || (defined INCLUDE_SECONDARY_ENET))
#   define INCLUDE_GEISC_END	/* MV64360 system controller gigabit ethernet */
#   define INCLUDE_END 
#endif

#ifdef INCLUDE_BOOT_IMAGE_PROGRAM
#   define STANDALONE_NET
#endif /* INCLUDE_BOOT_IMAGE_PROGRAM */

/*
 * IPMC SCSI Support
 *
 *     INCLUDE_SCSI2 : Necessary for SCSI2 library.
 * INCLUDE_DISK_UTIL : Necessary for complete file system support.
 * INCLUDE_SCSI_BOOT : Enables booting from a SCSI device.
 *     INCLUDE_DOSFS : Enables DOS file system support.
 *  SCSI_AUTO_CONFIG : Enables an automatic scan of the SCSI bus at
 *                     startup.
 *  SCSI_WIDE_ENABLE : Enable wide SCSI (16-bit data transfers).
 *   SYS_SCSI_CONFIG : Enables you to declare a SCSI peripheral
 *                     configuration, and is meant as a substitute for
 *                     SCSI_AUTO_CONFIG.  You must also edit
 *                     sysScsiConfig in sysScsi.c, so that it reflects
 *                     the actual configuration of your SCSI bus.
 *   INCLUDE_SYM_895 : Provides support for the Symbios/LSI Logic
 *                     SYM53C895A PCI to Ultra2 SCSI controller.
 *       ULTRA2_SCSI : Configures the SYM53C895A for Ultra2 SCSI support.
 */

#ifdef INCLUDE_SCSI
#   define INCLUDE_SCSI2        /* Use SCSI2 library, not SCSI1 */
#   define INCLUDE_DISK_UTIL    /* DOS Utilities */
#   define INCLUDE_SCSI_BOOT    /* include ability to boot from SCSI */
#   define INCLUDE_DOSFS        /* file system to be used */
#   define SCSI_AUTO_CONFIG     /* scan bus for devices on startup */
#   define SCSI_WIDE_ENABLE     /* enable wide SCSI, 16-bit data xfrs */
#   undef SYS_SCSI_CONFIG       /* call sysScsiConfig in sysScsi.c */
#   define INCLUDE_SYM_895      /* Symbios 895 support */
#   define ULTRA2_SCSI          /* Ultra2 SCSI */
#endif /* INCLUDE_SCSI */

/*
 * L1 Cache Support  
 *
 * The symbol INCLUDE_CACHE_SUPPORT (in "configAll.h") is #defined by
 * default and causes L1 cache support (including enabling of the cache)
 * to be provided.  In the event that no L1 cache enabling is desired
 * include a #undef INCLUDE_CACHE_SUPPORT here. However, note that if
 * INCLUDE_CACHE_SUPPORT is not defined (L1 cache not enabled) then
 * L2 cache will not be enabled as well, regardless of the state of the
 * INCLUDE_CACHE_L2 define.  This is because the L2 cache cannot be
 * enabled if the L1 cache is not enabled.  The same holds true for the
 * L3 cache and the INCLUDE_CACHE_L3 and INCLUDE_CACHE_L3_PM defines.
 * If INCLUDE_CACHE_SUPPORT is not defined (L1 cache not enabled) then 
 * the L3 cache cannot be configured as cache or as private memory. 
 */

#undef	USER_D_CACHE_MODE	/* L1 Data Cache Mode */
#define	USER_D_CACHE_MODE       (CACHE_COPYBACK | CACHE_SNOOP_ENABLE)

/*
 * L2 & L3 Cache Support
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
 * The default is to allow cacheing of data and instructions 
 * (L2_MODE_INST_AND_DATA).
 * Valid settings also include: L2_MODE_I_ONLY and L2_MODE_D_ONLY.
 *
 * (4) L2 Replacement Algorithm
 * The L2 cache replacement algorithm can be configured for Default replacement
 * algorithm or Secondary replacement algorithm.  The default is
 * to use the secondary replacement algorithm (L2_REP_ALG_SECONDARY).  To use
 * the default algorithm set L2_REP_ALG to L2_REP_ALG_DEFAULT.
 *
 * The following features of the L3 Cache can be configured (these are
 * relevant only if INCLUDE_CACHE_L3 is defined):
 *
 * (1) Mode
 * The L3 cache can be configured for Instruction-only mode, Data-only
 * mode, or both (instructions and data) with the L3_MODE definition.
 * The default is to allow cacheing of data and instructions 
 * (L3_MODE_INST_AND_DATA).
 * Valid settings also include: L3_MODE_I_ONLY and L3_MODE_D_ONLY.
 * See "Errata Workaround" below for more information.
 *
 * (2) L3 Replacement Algorithm
 * The L3 cache replacement algorithm can be configured for Default replacement
 * algorithm or Secondary replacement algorithm.  The default is
 * to use the secondary replacement algorithm (L3_REP_ALG_SECONDARY).  To use
 * the default algorithm set L3_REP_ALG to L3_REP_ALG_DEFAULT.
 *
 * Errata Workaround (MPC7455/7457 Revisions 0100 & 0101):
 *
 * Errata #15 "Six outstanding miss requests may stall the processor"
 * in MPC7457CE Rev.5, 12/2003 specifies that a processor configured
 * with 2MB of L3 cache must adhere to either of the following two
 * software workarounds:
 *  (1) Operate in 1MB cache, 1MB private memory mode, in which case
 *	the mode of the cache can be instruction-only, data-only, or
 *	both instruction and data, or
 *  (2) Operate the L3 cache (2MB) in instruction-mode only.
 *
 * To implement these workarounds the definition 
 * ERRATA_WORKAROUND_2MB_IONLY has been created.  If you wish to operate
 * in cache-only mode (ie. no private memory) this definition will
 * determine which of the above two workarounds is chosen.  For example,
 * if ERRATA_WORKAROUND_2MB_IONLY is defined then the full 2MB of
 * cache will be configured to operate in instruction-only mode.  However,
 * if it is not defined (#undef'd), then only 1MB of the cache will be
 * utilized and the mode will be determined by L3_MODE.  If you decide
 * to enable the private memory support then ERRATA_WORKAROUND_2MB_IONLY
 * will be used to determine the initial configuration of the L3 cache,
 * which is necessary to perform prior to configuring and enabling the
 * private memory by calling the sysL3CachePmEnable() routine in
 * sysL3Cache.c.  Ultimately the mode of the non-private memory portion
 * of cache will be set according to L3_MODE as well.
 *
 * BSP Default L3 Cache Configuration:
 *
 * By default, the L3 cache is enabled, private memory is disabled,
 * "data and instructions" mode is specified, and option (2) of the 
 * errata workarounds is chosen by undefining ERRATA_WORKAROUND_2MB_IONLY.  
 * This means that, for revisions "0100" and "0101" of the MPC7455/7457
 * processors, only 1MB of the L3 cache is utilized and it is 
 * configured for data and instructions. 
 */

#define	INCLUDE_CACHE_L2        	/* L2 cache support */
#define	ENABLE_L2_DATA_PARITY
#define	MSSCR0_L2_PREFETCH_ENGINES   MSSCR0_L2PFE_ONE
#define	L2_MODE                      L2_MODE_INST_AND_DATA 
#define	L2_REP_ALG                   L2_REP_ALG_SECONDARY

#define	INCLUDE_CACHE_L3		/* L3 cache support */
#undef	ERRATA_WORKAROUND_2MB_IONLY	/* errata workaround selection */
#undef	INCLUDE_CACHE_L3_PM		/* L3 Cache Private Memory Support */
#define	L3_MODE                      L3_MODE_INST_AND_DATA 
#define	L3_REP_ALG                   L3_REP_ALG_SECONDARY
#ifdef INCLUDE_CACHE_L3_PM
#   define INCLUDE_CACHE_L3_PM_BASE	0xF1200000
#endif /* INCLUDE_CACHE_L3_PM */

/*
 * AuxClk Support
 *
 * There are a total of four identical timer/counters on the MV64360,
 * each 32-bits wide.  One of the timer/counters is used to implement
 * the VxWorks AuxClk.  The following definition specifies which of the
 * four (0 through 3) are used to implement the AuxClk and is user-
 * configurable.
 */

#define MV64360_AUXCLK_TMR	0

/*
 * Error handling support
 * 
 * If error handling support is included, we must also include support
 * for the real-time clock in order to provide time tags for the errors
 * which are logged.
 */

#ifdef INCLUDE_ERROR_HANDLING
#   define INCLUDE_RTC
#endif

/* 
 * There are two methods of "hardware assist" available for
 * implementing the sysBusTas() function.  They are "VME bus locking"
 * and "VME bus RMW".  Which of these two methods is used is goverened
 * by the defined/undefined state of ANY_BRDS_IN_CHASSIS_NOT_RMW.
 * Having this symbol "#undef"ed will cause "VME bus RMW" to be used.
 * This is the required setting for early MVME6100 equipped with Rev 1
 * of the Tempe (Tsi148 PCI-VME) Chip.  When Rev 2 of the Tempe chip
 * is available, then MVME6100s equipped with the new chip also have
 * the option of "#define"ing ANY_BRDS_IN_CHASSIS_NOT_RMW to force the
 * use of VME bus locking to assist sysBusTas().
 */

#undef ANY_BRDS_IN_CHASSIS_NOT_RMW	/* required for Rev 1 of Tempe chip */

#undef VME_TAS_HW_ASSIST
#ifdef ANY_BRDS_IN_CHASSIS_NOT_RMW
#   define VME_TAS_HW_ASSIST	VME_BUS_LOCK
#else
#   define VME_TAS_HW_ASSIST	VME_BUS_RMW
#endif

/*
 * Firmware RAM & ROM Booting
 *
 * #define'ing FIRMWARE_BOOT allows one to use the MOTLoad firmware
 * command 'execProgram' to begin execution of VxWorks code rather
 * than having the processor, upon powerup, automatically begin
 * executing code from the cold boot location.  This provides
 * flexibility in just where the VxWorks image can reside and
 * still be invoked for execution.
 *
 * The code which is executed via MOTLoad's execProgram command
 * must have been previously loaded into RAM or alternatively
 * placed into flash memory.  The commentary which follows explains
 * this process.
 *
 * To execute a RAM-resident VxWorks "boot" image (rather than
 * a flash resident "boot" image), perform the following:
 *
 * 1. Change #undef FIRMWARE_BOOT to #define FIRMWARE_BOOT.
 * 2. In mv6100A.h, ROM_BASE_ADRS may need to be adjusted.  It is
 *    currently set, for RAM booting, based on the assumption that
 *    there is 512MB of DRAM.  However, this should be adjusted
 *    to the actual amount of DRAM on the board minus 1MB, in order to
 *    leave room for the vxWorks boot image at the top of DRAM.
 *      DRAM - 0x100000  (currently set to 0x1ff00000, if FIRMWARE_BOOT
 *                        is #define'd)
 * 3. Since ROM_TEXT_ADRS and ROM_WARM_ADRS are defined in both the
 *    Makefile(hard-coded) and config.h(based on the value of ROM_BASE_ADRS)
 *    it is necessary to adjust the values of ROM_TEXT_ADRS and
 *    ROM_WARM_ADRS in the Makefile.  The values used are dependent upon
 *    the value of ROM_BASE_ADRS as discussed in #2 above.  For a board
 *    with 512MB of DRAM they would be 0x1ff00000 and 0x1ff00004, respectively.
 * 
 * NOTE: If FIRMWARE_BOOT is defined then USER_RESERVED_MEM will be
 *       defined as 0x100000 (1MB) in order to preserve the boot image
 *       in the top 1 MB of the DRAM.
 *
 * To use the firmware to boot vxWorks from DRAM use the following commands
 * (substitute the appropriate values where necessary).  The values used
 * for the tftp download address and the execP load address are based on a
 * board with 512MB of DRAM:
 *
 * tftpGet -cXXX.XXX.XXX.XXX -fboot.bin -sXXX.XXX.XXX.XXX -d/dev/enetx -a1FF00000
 * execProgram -l1FF00000 -s100000
 *
 * A "cold boot" (powerup boot) always causes the image flashed into
 * the last megabyte of the boot flash to be executed.  The execution
 * address for cold boot start is the PPC reset vector at 0xfff00100
 * which is in the last megabyte of the 4-gigabyte address space.  We
 * may want to use a flash bank to hold an image for execution but we
 * also want to protect the "cold boot" image that is present in the last
 * megabyte of flash memory in this bank.  We can use the first
 * megabyte of flash memory for our executable image and use the
 * MOTLoad "execProgram" command to execute that image - thus
 * protecting the "cold boot" image which is located in the last
 * megabyte of flash memory.  The explanation which follows describes
 * how to do this.
 *
 * Remember that no matter how the flash bank selection jumper is set, the
 * base address (address of first megabyte) of the boot flash bank is
 * always 0xf8000000.  Likewise the address of the non-boot flash bank is
 * always 0xf4000000 (see the memory map diagram contained in this file).
 *
 * To execute a vxWorks "boot" image from ROM, perform the following:
 * 
 * 1. Change #undef FIRMWARE_BOOT to #define FIRMWARE_BOOT.
 *
 * 2. In mv6100A.h, ROM_BASE_ADRS will need to be set to the start address
 *    of the flash bank from which the code will be executed via the MOTLoad
 *    "execProgram" command.  Since we are specifying the start address
 *    (first megabyte) of the flash bank, the image that is resident in
 *    the last megabyte (cold boot image) will be protected.  So, if your
 *    image to execute is located in the first megabyte of the boot flash
 *    bank you would set ROM_BASE_ADRS to be 0xf8000000.  If the image is
 *    located in the first megabyte of the non-boot flash bank you would
 *    set ROM_BASE_ADRS to be 0xf4000000.
 * 3. Since ROM_TEXT_ADRS and ROM_WARM_ADRS are defined in both the
 *    Makefile(hard-coded) and config.h(based on the value of ROM_BASE_ADRS)
 *    it is necessary to adjust the values of ROM_TEXT_ADRS and
 *    ROM_WARM_ADRS in the Makefile.  The values used are dependent upon
 *    the value of ROM_BASE_ADRS as discussed in #2 above.  When booting from
 *    the non-boot flash bank they would be 0xf4000000 and 0xf4000004,
 *    respectively.  When booting from the boot flash bank they would be
 *    0xf8000000 and 0xf8000004, respectively.
 * 4. If FIRMWARE_BOOT is defined, USER_RESERVED_MEM will be set to
 *    0x100000 (1MB).  The reason for this is to setup the firmware booting
 *    for DRAM booting by default, rather than ROM booting.  If left as
 *    is the top 1MB of DRAM will be reserved, which is not necessary when
 *    booting from ROM.  To change this simply change USER_RESERVED_MEM,
 *    located near the end of this file, to 0x0.
 *
 * To use the firmware to boot vxWorks from ROM use the following commands
 * (substitute the appropriate values where necessary).  The execP load address
 *  is dependent upon which flash bank the VxWorks image is located in):
 *
 * tftpGet -cXXX.XXX.XXX.XXX -fboot.bin -sXXX.XXX.XXX.XXX -d/dev/enetx
 * flashProgram -o0 -nfff00 -d/dev/flashx
 * execProgram -lF4000000 -s100000
 */

#undef	FIRMWARE_BOOT

/*
 * MV64360 DMA Support
 *
 * IDMA snooping is enabled by default to achieve cache coherency
 * for IDMA on MV64360.  Be aware that turning off IDMA snooping
 * will result in disabling snoop transaction even in the cache
 * coherency regions.  Undef IDMA_SNOOP_ON if snoop transaction
 * is not necessary to achieve better IDMA performance.
 *
 * WARNING: turning off IDMA snooping will destroy data integrity
 * if the destination buffer resides on the cacheable memory which
 * requires cache coherency!
 */

#define   IDMA_SNOOP_ON

/*
 * Burst limit choices are 8, 16, 32, 64 or 128 bytes.  The higher the
 * burst limit, the faster the transfer.  DMA buffer address should be
 * aligned to burst limit granularity for best performance.
 *
 * Since snooping is working on cache line basis, any access to SDRAM
 * interface unit that requires snoop must not cross the cache line
 * boundary.  Therefore, if snooping is turned on, the IDMA burst limit
 * must not exceed 32 bytes.
 */

#ifdef   IDMA_SNOOP_ON
#    define  IDMA_DST_BURST_LIMIT IDMA_CNTL_DBURST_LIMIT_32B
#    define  IDMA_SRC_BURST_LIMIT IDMA_CNTL_SBURST_LIMIT_32B
#else
#    define  IDMA_DST_BURST_LIMIT IDMA_CNTL_DBURST_LIMIT_128B
#    define  IDMA_SRC_BURST_LIMIT IDMA_CNTL_SBURST_LIMIT_128B
#endif   /* IDMA_SNOOP_ON */

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
 * Bits[31:26] Read Buffer Assignment per each interface
 *
 * By default we will set the Gb unit to use read buffer 1 while
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
 * By default this will be set to 0x0.
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
#define MV64360_DDR_SDRAM_OPEN_PAGES_CTRL_DFLT	0x00000000
#define MV64360_DDR_SDRAM_IF_XBAR_CTRL_LO_DFLT	0x00765432
#define MV64360_DDR_SDRAM_IF_XBAR_CTRL_HI_DFLT	0x00765432
#define MV64360_DDR_SDRAM_IF_XBAR_TMOUT_DFLT	0x000100FF

/*
 * PCI and PCI Auto-Configuration Support
 *
 * PCIx_MSTR_IO_SIZE, PCIx_MSTR_MEMIO_SIZE and PCIx_MSTR_MEM_SIZE
 * control the sizes of the available   PCI address spaces.  ('x' is
 * '0' for bus 0.0 and '1' for bus 1.0) The windows defined by these
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
 * CPU Space                                PCI (|) or VME (}) Space
 * (default values)                       
 *
 *                              :                 :
 *                              :                 :
 * VME_A32_MSTR_LOCAL =          -----------------  VME_A32_MSTR_BUS
 *              (0x80000000)    |                 } (0x08000000)  
 *                              | VME A32 space   } .
 *                              |    256MB        } . Tempe outbound window 1
 *                              | (0x10000000)    } .
 *              (0x87ffffff)    |                 } (0x0fffffff) 
 *                              |.................}
 *                              | Unused A32      }
 *              (0x8fff0000)    |.................} (0xfb000000)
 *                              |                 } .
 *                              |                 } . Tempe outbound window 0
 *                              |                 } .
 *              (0x8fffffff)    |                 } (0xfb00ffff)
 * VME_A24_MSTR_LOCAL =          -----------------
 * VME_A32_MSTR_LOCAL +         |                 } (0x00000000)
 * VME_A32_MSTR_SIZE =          | VME A24 space   } .
 *              (0x90000000)    |    16MB         } . Tempe outbound window 2
 *                              | (0x01000000)    } .
 *              (0x90ffffff)    |                 } (0x00ffffff)
 * VME_A16_MSTR_LOCAL =          -----------------
 * VME_A24_MSTR_LOCAL +         |                 } (0x00000000)
 * VME_A24_MSTR_SIZE =          | VME A16 space   } .
 *              (0x91000000)    |    64KB         } . Tempe outbound window 3
 *                              | (0x00010000)    } . 
 *              (0x910fffff)    |                 } (0x0000ffff)
 *                               -----------------
 *                              :                 :
 *                              :  Not used       :
 *                              :  239 MB         :
 *                              : (0x0ef00000)    :
 *                              : Available for   :
 *                              : expansion of    :
 *                              : VME space       :
 * PCI0_MSTR_MEMIO_LOCAL =       ----------------- PCI0_MSTR_MEMIO_BUS =
 *              (0xa0000000)    |  Bus 0.0        |  PCI0_MSTR_MEMIO_LOCAL =
 *                              | NonPrefetch PCI |  (0xa0000000)
 *                              | Memory space    |
 *                              |      8MB        |
 *                              | (0x00800000)    |
 *                              |                 |
 * PCI0_MSTR_MEM_LOCAL =         ----------------- PCI0_MSTR_MEM_BUS =
 * PCI0_MSTR_MEMIO_LOCAL +      |  Bus 0.0        |  PCI0_MSTR_MEM_LOCAL
 * PCI0_MSTR_MEMIO_SIZE =       | Prefetch PCI    |  (0xa0800000)
 *              (0xa0800000)    | Memory space    |
 *                              |    8MB          |
 *                              | (0x00800000)    |
 * PCI1_MSTR_MEMIO_LOCAL =       ----------------- PCI1_MSTR_MEMIO_BUS
 * PCI0_MSTR_MEM_LOCAL +        |  Bus 1.0        |  PCI1_MSTR_MEMIO_LOCAL
 * PCI0_MSTR_MEM_SIZE           | NonPrefetch PCI |  (0xa1000000)
 *              (0xa1000000)    | Memory space    |
 *                              |    16MB         |
 *                              | (0x00800000)    |
 * PCI1_MSTR_MEM_LOCAL =         ----------------- PCI1_MSTR_MEM_BUS =
 * PCI1_MSTR_MEMIO_LOCAL +      |  Bus 1.0        |  PCI1_MSTR_MEM_LOCAL
 * PCI1_MSTR_MEMIO_SIZE =       | Prefetch PCI    |  (0xa2000000)
 *              (0xa2000000)    | Memory space    |
 *                              |    8MB          |
 *                              | (0x00800000)    |
 * ISA_MSTR_IO_LOCAL =           ----------------- ISA_MSTR_IO_BUS =
 * PCI1_MSTR_MEM_LOCAL +        | Bus 1.0 16-bit  |   (0x00000000)
 * PCI1_MSTR_MEM_SIZE           | I/O space       |
 *              (0xa2800000)    |    64KB         |
 *                              | (0x00010000)    |
 * PCI1_MSTR_IO_LOCAL =          ----------------- PCI1_MSTR_IO_BUS =
 * ISA_MSTR_IO_LOCAL +          | Bus 1.0 32-bit  |   (0x00010000)
 * ISA_MSTR_IO_SIZE =           |   I/O space     |
 *              (0xa2810000)    |   8MB - 64KB    |
 *                              | (0x007f0000)    |
 * PCI0_MSTR_IO_LOCAL            ----------------- PCI0_MSTR_IO_BUS =
 * PCI1_MSTR_IO_LOCAL +         | Bus 0.0 32-bit  |   (0x00800000)
 * PCI1_MSTR_IO_SIZE =          | I/O space       |
 *              (0xa3000000)    |    8MB          |
 *                              | (0x00800000)    |
 *              (0xa3800000)     ----------------- (0x01000000)
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
 * MV64360_INTEGRATED_SRAM_BASE  -----------------
 *            = (0xf1100000)    : Integrated SRAM :
 *                              :     256K        :
 *                              : (0x00040000)    :
 *                              : Used for Enet   :
 *                              : Descriptors     :
 *              (0xf1140000)     -----------------
 *                              :     Unused      :
 * L3_CACHE_PRIVATE_MEMORY_BASE  -----------------
 *              (0xf1200000)    :   L3 Cache      :
 *                              : Private Memory  :
 *                              :   (OPTIONAL)    :
 *                              :  1MB - 2MB      :
 *              (0xf1400000)     -----------------
 *                              :     Unused      : 
 *                              :                 :
 *                              :                 :
 * MV64360_DEVCS1_BASE_ADDR =    -----------------
 *              (0xf1d00000)    | Device bank     :
 *                              |    1MB          :
 *                              | (0x00100000)    :
 *                              |                 :
 *              (0xf1e00000)     -----------------
 *                              : Not used        :
 *                              :     34MB        :
 *                              :                 :
 * MV64360_DEVCS0_BASE_ADDR =    -----------------
 *              (0xf4000000)    | Non-Boot Flash  :
 *                              |     Bank        :
 *                              |     64MB        :
 *                              |                 :
 *                              |                 :
 * MV64360_BOOTCS_BASE_ADDR =    -----------------
 *              (0xf8000000)    | Boot Flash Bank :
 *                              |     128MB       :
 *                              |                 :
 *                              |                 :
 *                              |                 :
 *              (0xffffffff)     -----------------
 *
 * Notes in inbound translations from VME bus:
 *
 * Tempe inbound window 0 in support of shared-memory:
 *
 * On the processor node compiled with SM_OFF_BOARD set to FALSE, an
 * inbound window is opened using Tempe's inbound window 0 to map the
 * shared-memory anchor and shared-memory pool.
 *
 * The inbound accesses is shown here (output from sysTempeInWinShow(0)):
 *
 * Inbound window 0: ENABLED,  Size = 00000000_00040000
 * 
 * raw: 00000000 08000000 00000000 0803fff0 ffffffff f8000000 80000faf
 * VME Base:  00000000_08000000    PCI Base:  00000000_00000000
 * VME Limit: 00000000_0803ffff    PCI Limit: 00000000_0003ffff
 *   Attributes:
 *      Read-ahead threshold - when FIFO competely empty
 *      Virtual FIFO size    - 64 bytes
 *      2eSST Mode           - 160 MB/s
 *      Cycle response       - 2eSSTB (Two Edge Source Synchronous Broadcast)
 *                             2eSST (Two Edge Source Synchronous nonBroadcast)
 *                             2eVME (Two Edge VMEbus)
 *                             MBLT (Multiplexed Block Transfer)
 *                             BLT (Block Transfer)
 *      Address space        - A32
 *      VME AM response      - Supervisor, nonSupervisor, Program, Data
 *
 * Here's a recap:
 *
 * If this is not the anchor node (SM_OFF_BOARD == TRUE)
 * then
 *     No A32 inbound mappings are programmed for access to DRAM.
 *     Note that all nodes must have inbound mapping to reach 
 *     their mailbox's so shared memory doorbells can
 *     be hit by any other node.  This is taken care of by the
 *     RAI register programming on Universe equipped boards and
 *     the CRG programming by Tempe equipped boards.
 * else
 *     (this is the anchor node ...)
 *     A single inbound mapping through Tempe window 0 is configured,
 *     mapping DRAM address 0x00000000 through 0x0003ffff to VME
 *     bus address 0x08000000 through 0x0803ffff.
 *
 * PCI Mapped Memory consists of non-prefetchable and prefetchable.
 * Each bus has its own non-prefetchable and prefetchable memory
 * spaces.
 *
 * The maximum amount of non-prefetchable PCI memory is defined by
 * the user as PCI0_MSTR_MEMIO_SIZE and PCI1_MSTR_MEMIO_SIZE.
 *
 * The maximum amount of prefetchable PCI memory is defined by the
 * user as PCI0_MSTR_MEM_SIZE and PCI1_MSTR_MEM_SIZE.
 *
 * PCI Mapped I/O consists of 16 and 32 bit I/O. Only bus 1.0 contains
 * 16 bit I/O space.
 *
 * The amount of 16 bit I/O, for bus 1.0, is configured by
 * ISA_MSTR_IO_SIZE and defaults to 64KB in size. This area begins at
 * 0x0 on the PCI I/O bus (1.0), and ISA_MSTR_IO_LOCAL on 
 * the processor bus.
 *
 * The amount of 32 bit I/O is configured by PCI0_MSTR_IO_SIZE and
 * PCI1_MSTR_IO_SIZE.
 *
 * PCI_MEM_LOCAL_START:
 *
 * Local (CPU referenced) base address of all PCI space.  This must satisfy
 * the following conditions:
 *
 * 1) It must have a value greater than the highest DRAM address.
 * 2) It must be a multiple of 0x00100000 to satisfy host bridge address
 *       decoders.
 *
 * The space beginning with this address and ending with the base address
 * of the host bridge registers (MV64360_REG_BASE = 0xf1000000) is used
 * for local access to PCI space.
 */

/* VME Space Size Definitions */

#define	VME_A64_MSTR_SIZE	0x00000000	/* 0 MB */
#define	VME_A32_MSTR_SIZE	0x10000000	/* 256 MB */
#define	VME_A24_MSTR_SIZE	0x01000000	/* 16 MB */
#define	VME_A16_MSTR_SIZE	0x00010000	/* 64 KB */

/* Bus 0.0 PCI Space Definitions - must be set to power of 2 */

#define	PCI0_MSTR_MEMIO_SIZE	0x00800000	/* 8 MB */
#define	PCI0_MSTR_MEM_SIZE	0x00800000	/* 8 MB	*/
#define	PCI0_MSTR_IO_SIZE	0x00800000	/* 8 MB	*/

/*
 * Bus 1.0 PCI Space Definitions - must be set to power of 2, in
 * the case of ISA_MSTR_IO_SIZE and PCI1_MSTR_IO_SIZE, their sum
 * must be a power of 2
 */

#define	PCI1_MSTR_MEMIO_SIZE	0x01000000	/* 16 MB */
#define	PCI1_MSTR_MEM_SIZE	0x00800000	/* 8 MB	*/
#define	ISA_MSTR_IO_SIZE	0x00010000	/* 64 kbytes */
#define	PCI1_MSTR_IO_SIZE	0x007f0000	/* 8 MB	- 64 KB	*/

/* 
 * Timestamp Support 
 *
 * Timestamp option not included by default:  #define to include it 
 */

#undef  INCLUDE_TIMESTAMP

/*
 * System Memory (DRAM) Support 
 *
 * By default, the available DRAM memory is sized at bootup (LOCAL_MEM_AUTOSIZE
 * is defined).  If auto-sizing is not selected, make certain that
 * LOCAL_MEM_SIZE is set to the actual amount of memory on the board.
 * By default, it is set to the minimum memory configuration: 32 MB.
 * Failure to do so can cause unpredictable system behavior!
 */

#define	LOCAL_MEM_AUTOSIZE			/* undef for fixed size */
#define LOCAL_MEM_LOCAL_ADRS	0x00000000	/* fixed at zero */
#define LOCAL_MEM_SIZE		0x02000000	/* Default: Min memory: 32MB */

/* user reserved memory, see sysMemTop() */

#ifdef FIRMWARE_BOOT
#   define USER_RESERVED_MEM    (0x100000)	/* number of reserved bytes */
#else
#   define USER_RESERVED_MEM    (0)		/* number of reserved bytes */
#endif

/*
 * NOTE: the following are #define'd in two separate places.  Make sure
 * that the definitions in both places agree:
 *
 *   ROM_TEXT_ADRS    #define'd in "config.h" and "Makefile".
 *   ROM_WARM_ADRS    #define'd in "config.h" and "Makefile".
 *   RAM_LOW_ADRS     #define'd in "config.h" and "Makefile".
 *   RAM_HIGH_ADRS    #define'd in "config.h" and "Makefile".
 *   ROM_SIZE	      #define'd in "config.h" and "Makefile".
 *
 * ROM_WARM_ADRS is the boot ROM warm entry address which is set to 4 bytes
 * beyond the cold boot entry point ROM_TEXT_ADRS.
 */

#ifdef FIRMWARE_BOOT
#   define ROM_TEXT_ADRS	(ROM_BASE_ADRS)
#else
#   define ROM_TEXT_ADRS	(ROM_BASE_ADRS + 0x100)
#endif

#define RAM_HIGH_ADRS		0x00800000  	/* RAM address for ROM boot */
#define RAM_LOW_ADRS		0x00100000	/* RAM address for kernel */
#define ROM_SIZE		0x00100000	/* 1 MB ROM space */
#define ROM_WARM_ADRS		(ROM_TEXT_ADRS + 0x004)	

/*
 * There are two PCI busses off the host bridge which allow access
 * to DRAM (bus 0 and bus 1). The following two #define's control the 
 * snooping attribute for the PCI access control base 0 registers 
 * (programmed inside of "sysMv64260Phb.c", function sysMv64360PhbInit(), 
 * registers: PCIIF_ACCESS_CTRL_BASE0_LOW_PCI0 and 
 * PCIIF_ACCESS_CTRL_BASE0_LOW_PCI1).  If snooping is turned off for either 
 * bus by #undef'ing the appropriate #define below, it is important to either 
 * turn off cacheing for that region of memory or make  sure that all 
 * software which accesses the region of memory covered by  the associated 
 * PCI access control register performs explicit cache control directives 
 * (e.g. flusing, invalidating) to maintain cache coherency.  When shared
 * memory services are enabled, the snooping #define for bus 0 is turned
 * off - see note below.
 */

#define PCI_DMA_SNOOP_BUS_0_ON
#define PCI_DMA_SNOOP_BUS_1_ON

/*
 * Changing SM_OFF_BOARD to TRUE also requires changing  SM_ANCHOR_ADRS and 
 * SM_MEM_ADRS appropriately, as the following #define's do:
 */

#define SM_OFF_BOARD	FALSE 

#undef  SM_ANCHOR_ADRS
#if (SM_OFF_BOARD == TRUE)
#   define SM_ANCHOR_ADRS ((char*) (VME_A32_MSTR_LOCAL + SM_ANCHOR_OFFSET))
#else
#   define SM_ANCHOR_ADRS (char*) (LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET)
#endif

#define SM_INT_TYPE             SM_INT_MAILBOX_1         /* or SM_INT_NONE */

/*
 * The following must be #define'd the same on all nodes.
 */

#define SM_MEM_ADRS             0x00010000      /* Start of actual SM region */
#define SM_MEM_SIZE             0x00020000
#define SM_OBJ_MEM_ADRS         (SM_MEM_ADRS+SM_MEM_SIZE) /* SM Objects pool */
#define SM_OBJ_MEM_SIZE         0x00010000

/*
 * Because of a limitation of the Discovery II host bridge chip, snooping
 * on BUS 0 (the bus that the Tempe PCI-X to VME bridge is on) must be
 * turned off to prevent a potential PCI lock for a snoop cycle which
 * cannot be serviced (see Discovery II App-note 85), hence the setting
 * for PCI_DMA_SNOOP_BUS_0_ON below.  Also we will set up a #define for
 * SM_CACHE_STATE which will control the cacheing attribute for the area 
 * occupied by the shared memory anchor and memory pool.
 */

#if defined(INCLUDE_SM_NET)
#   define INCLUDE_SM_COMMON
#   define INCLUDE_BSD
#   define INCLUDE_SM_NET_SHOW
#   define INCLUDE_NET_DRV
#   define INCLUDE_NET_LIB
#   ifdef  STANDALONE
#          define STANDALONE_NET
#   endif  /* STANDALONE */
#   undef  PCI_DMA_SNOOP_BUS_0_ON
#endif /* defined(INCLUDE_SM_NET) */

#if ((defined INCLUDE_SM_NET) && (SM_OFF_BOARD == FALSE))
#   define SM_CACHE_STATE	VM_STATE_CACHEABLE_NOT
#else
#   define SM_CACHE_STATE	VM_STATE_CACHEABLE
#endif

/* WDB_COMM_TYPE = WDB_COMM_NETWORK for Shared Memory Slave */
 
#if defined(INCLUDE_SM_NET) && (SM_OFF_BOARD == TRUE)
#   undef  WDB_COMM_TYPE   
#   define WDB_COMM_TYPE	WDB_COMM_NETWORK
#endif  /* defined(INCLUDE_SM_NET) && (SM_OFF_BOARD == TRUE) */

/* BSP-specific includes */

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

/*
 * Define the priority list for the Tempe interrupt support.  List items
 * correspond to bit postions in the interrupt enable register.  The
 * first item in the list is the first one scanned when a Tempe interrupt
 * occurs.
 */

#define TEMPE_INT_PRIORITIES \
	TEMPE_INT_NUM_IRQ1_BIT, TEMPE_INT_NUM_IRQ2_BIT, \
	TEMPE_INT_NUM_IRQ3_BIT, TEMPE_INT_NUM_IRQ4_BIT, \
	TEMPE_INT_NUM_IRQ5_BIT, TEMPE_INT_NUM_IRQ6_BIT, \
	TEMPE_INT_NUM_IRQ7_BIT, TEMPE_INT_NUM_ACFL_BIT, \
	TEMPE_INT_NUM_SYSFL_BIT, TEMPE_INT_NUM_IACK_BIT, \
	TEMPE_INT_NUM_VIEEN_BIT, TEMPE_INT_NUM_VERR_BIT, \
	TEMPE_INT_NUM_PERR_BIT, TEMPE_INT_NUM_MB0_BIT, \
	TEMPE_INT_NUM_MB1_BIT, TEMPE_INT_NUM_MB2_BIT, \
	TEMPE_INT_NUM_MB3_BIT, TEMPE_INT_NUM_LM0_BIT, \
	TEMPE_INT_NUM_LM1_BIT, TEMPE_INT_NUM_LM2_BIT, \
	TEMPE_INT_NUM_LM3_BIT, TEMPE_INT_NUM_DMA0_BIT, \
	TEMPE_INT_NUM_DMA1_BIT, 0xff	/* end of list */

/*
 * Here we define the intEnable and intDisable routines associated with
 * the user reserved interrupt vectors.  These are filled with NULL
 * for now but should be filled with a valid intEnable and intDisable
 * function pointer when the user actually needs to hook functions to
 * these interrupt numbers.  See INT_UTIL_EXTRA inside of "mv6100A.h".
 */

#define USR_INT_ENABLE	NULL	/* Replace with valid intEnable fcn ptr */
#define USR_INT_DISABLE NULL	/* Replace with valid intDisable fcn ptr */

#define INT_USR_EXTRA ,{ USR_INT_NUM_MIN,USR_INT_NUM_MAX, \
			 USR_INT_ENABLE, USR_INT_DISABLE }

#include "mv6100A.h"	/* board specific definitions */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

/*sergey #if defined(PRJ_BUILD)*/
    #include "prjParams.h"
/*sergey #endif*/ /* PRJ_BUILD */

#include "mv6100B.h"	/* definition dependencies */

#endif	/* __INCconfigh */
