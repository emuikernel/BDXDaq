/* mv6100A.h - Motorola MVME6100 board header file */

/* Copyright 1984-2000 Wind River Systems, Inc. */
/* Copyright 1999-2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01ccc,02dec04,cak Modifications for Tornado Project support. 
01bbb,12jul04,cak Added bit definitions for the presence detect register.
01aaa,07jul04,cak Moved IPMC support back to config.h.
01zz,30jun04,cak  Added support for the Maxim DS1621 digital thermometer
		  and thermostat.
01yy,15jun04,cak  Removed the INUM_TO_IVEC from the TMR_CNTR0_INT_VEC
		  definition. 
01xx,04jun04,cak  Added support for MV64360 watchdog timer interrupts.
01ww,01jun04,cak  Added support to automatically mount the FLASH bank
		  devices if FLASH bank (boot block) programming is included.
01vv,11may04,cak  Moved IPMC support from config.h. 
01uu,03may04,cak  Added support to automatically mount the TFFS drives if
		  TFFS support is included.
01tt,21apr04,scb  INT_USR_EXTRA upgrade.
01ss,21apr04,scb  RMW busTas() support.
01rr,15apr04,cak  Added TFFS support.
01qq,09apr04,cak  Added support for the PMCSpan.
01pp,06apr04,scb  Tempe RMW support values.
01oo,29mar04,cak  Added support for the L3 Cache Output Hold Control
		  Register (L3OHCR - SPR 1000). 
01nn,12mar04,cak  Modifications to support placing the ethernet descriptors
		  into the Integrated SRAM. 
01mm,04mar04,cak  Replaced FIRMWARE_RAM_BOOT with FIRMWARE_BOOT to make
		  it more generic since it applies to both the RAM and ROM,
		  and fixed the IPMC support.
01ll,25feb04,scb  Tempe interrupt support.
01kk,17feb04,cak  Modified device bus support and modified the I2C
                  address for MV64360 User Defined Init.
01jj,09feb04,scb  Tempe windowing support, a small part of interrupt support.
01ii,09feb04,cak  Modified PCI_HOST_BRIDGE_DEV_NUM to be 0 rather than 16.
01hh,03nov03,cak  BSP update.
01gg,22sep03,scb  Added #define for TEMPE_PCI_BUS.
01ff,04mar03,cak  Returned DEC_CLOCK_FREQ and added IPMC SCSI support.
01ee,28jan03,cak  Removed DEC_CLOCK_FREQ and DFLT_BUS_CLK definitions.
		  Added a mask definition for the REF_CLK bit in
		  System Status Register 1.  Added BAUDOUT definitions.
01dd,24jan03,cak  Added System Board Register mnemonics & moved PCI
                  window compiler errors to mv6100B.h.
01cc,10jan03,efb  Adjusted ethernet memory space for MV64360.
01bb,08jan03,cak  IPMC Serial Support.
01aa,30oct02,cak  DMA macro definitions.
01z,18oct02,cak  RTC and Failsafe Timer support.
01y,16oct02,cak  Changed ROM_SIZE from 8MB to 1MB.
01x,07oct02,cak  Fixed PCI interrupt routing definitions during auto-config 
		 debug.
01w,30sep02,cak  Fixed PCI definitions during PHB debug.
01v,11sep02,cak  Removed unneeded I2C and VPD definitions. 
01u,05sep02,cak  Modifications to I2C and VPD support.
01t,03sep02,scb  Move base address and size encoding/decoding macros out of 
		 here and into the mv64360.h file.
01s,26aug02,cak  Fixed interrupt controller definitions for board bring-up.
01r,20aug02,scb  Add #defines similar to GPP_INT_LVL_8_15
01q,07aug02,scb  Fix PCI space size compile time error calculation.
01p,22jul02,scb  Fix #define for GPP_IC_INT_NUM_MAX
01o,18jul02,yyz	 Added interrupt level and vector #defines for timer/counter 0.
01n,12jul02,scb  IDSEL changes for intLine specifiers.
01m,12jul02,scb  Spec change results in two COM ports instead of three.
01l,11jul02,scb  Adjustment to memory map for flash banks.
01k,26jun02,scb  Register mnemonic name changes.
01j,25jun02,cak  Added BUS_RATE definitions as well as I2C and VPD support.
01i,14jun02,scb  Host bridge support.
01h,20may02,cak  PCI Auto-configuration support for busses 0.0 and 1.0 of the
		 MV64360.
01g,03may02,cak  Changed DEFAULT_BUS_CLOCK from 100MHz to 133MHz. 
01f,25apr02,scb  Added Mv64360 "bus device" related support.
01e,18apr02,cak  Added support for all three COM ports connected to the MV64360.
01d,15apr02,cak  Added MV64360_REG_SPACE_SIZE definition.
01c,10apr02,scb  Add INT_NUM #defines, PCI & PMC interrupts, max bus.
01b,25mar02,cak  Added some System Status and Control Register definitions,
                 and a macro for computing the size encoding for the CPU
                 Address Decoding Window Size registers. 
01a,07mar02,cak  Ported. (from ver 01p, mcp820/mcpx800.h)
*/

/*
This file contains I/O addresses and related constants for the
Motorola MVME6100 board.  This file must not contain any "dependencies"
(ie. #ifdef's or #ifndef's).
*/

#ifndef	__INCmv6100Ah
#define	__INCmv6100Ah

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* includes */

#include "ppcProc.h"
#include "tempe.h"

/* AltiVec Exceptions */

#define _EXC_VMX_UNAVAIL       0x0f20	/* VMX Unavailable Exception */
#define _EXC_VMX_ASSIST        0x1600	/* VMX Assist Exception */
#define SIZEOF_EXCEPTION       0x0030	/* VxWorks Exc is 48 bytes Max*/

/*
 * sysPhysMemDesc[] MMU Memory Attribute Definitions
 *
 * The following definitions are used in the sysPhysMemDesc[] structure
 * in sysLib.c to initialize the page table entries used by the MMU to
 * translate addresses.
 */

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
 * Non-volatile (NVRAM) defines.  The "base address" of the NVRAM/RTC 
 * is located at MV64360_DEVCS1_BASE_ADDR + 0x10000, (0x100000 offset 
 * into the "device bank"), and extends for 32KB up to 
 * MV64360_DEVCS1_BASE_ADDR + 0x17fff.
 * 
 * The first 256 bytes are skipped, the second 256 bytes are  used to 
 * store the vxWorks boot line parameters.  From the "base address"
 * plus the offset of 0x7ff0 thru an offset of 0x7fff (16 bytes),
 * are the M48T37 registers.
 * 
 * The NVRAM_BASE address is used in the sysNvWrite and sysNvRead
 * routines in sysLib.c.  In order to write to/read from NVRAM using 
 * the sysLib routines, you need only specify an offset. 
 * (ie. NV_RAM_BASE + offset)
 */

#undef  NV_BOOT_OFFSET
#define NV_BOOT_OFFSET          256     /* skip 1st 256 bytes */
#define NV_RAM_SIZE             0x7FF0  /* 32KB Total */
#define NV_RAM_INTRVL           1
#define NV_RAM_BASE             (MV64360_DEVCS1_BASE_ADDR + 0x00010000)        
#define NV_RAM_READ(x)		sysNvRead (x)
#define NV_RAM_WRITE(x,y)	sysNvWrite (x,y)

/* Real-time clock and Alarm clock Support */

#define RTC_SET(x)                 m48t37RtcSet (x)
#define RTC_SHOW()                 m48t37RtcShow ()
#define RTC_GET(x)                 m48t37RtcGet (x)
#define RTC_DATE_TIME_HOOK(x)      m48t37DateTimeHook (x)
#define ALARM_SET(x,y)             m48t37AlarmSet (x,y)
#define ALARM_CANCEL()             m48t37AlarmCancel ()
#define ALARM_GET(x,y)             m48t37AlarmGet (x,y)
#define ALARM_SHOW()               m48t37AlarmShow ()

/* Failsafe Timer Support */

#define FAILSAFE_CANCEL()             m48t37FailsafeCancel ()

#define FAILSAFE_SET(x,y)          m48t37FailsafeSet (x,y)
#define FAILSAFE_GET(x,y)          m48t37FailsafeGet (x,y)
#define FAILSAFE_SHOW()            m48t37FailsafeShow ()
#define FAILSAFE_CAUSED_RESET()    m48t37FailsafeCausedReset ()

/* 
 * TrueFFS Flash File System Support 
 * 
 * The size is set to 32MB.  We do this because the FLASH banks
 * are each 64MB in size, but in order to protect any boot images
 * that may reside in the 63rd MB of the FLASH bank - either firmware
 * or another VxWorks image, and also conform to the TFFS restriction that 
 * the flash bank size must a power of 2 (1,2,4,8,16,32,...) we are not
 * able to utilize more than 32 MB of the FLASH.
 *
 * Support is also provided to mount the TFFS drives automatically
 * during kernel initialization.
 */

#if ((defined INCLUDE_TFFS_FLASHA) || (defined INCLUDE_TFFS_FLASHB))
#   define TFFS_DEVICE_SECTOR_SZ   INTEL28F256_SECTOR_SZ
#   define INCLUDE_TFFS
#   define FLASH_SIZE		0x02000000     /* 32MB */
#   define INCLUDE_MTD_USR
#   define MTD_USR_IDENTIFY	strataFlashIdentify
#   define INCLUDE_FLASH
#   define STRATAFLASH_WRITE_BUFFER_MODE
#endif /* INCLUDE_TFFS_FLASHA, INCLUDE_TFFS_FLASHB */

#if defined INCLUDE_TFFS_FLASHA && defined INCLUDE_TFFS_FLASHB
#   define NUM_TFFS_VOLS 2
#   ifndef INCLUDE_DOSFS
#      define INCLUDE_DOSFS
#   endif
#else
#   if defined INCLUDE_TFFS_FLASHA || defined INCLUDE_TFFS_FLASHB
#      define NUM_TFFS_VOLS 1
#   ifndef INCLUDE_DOSFS
#      define INCLUDE_DOSFS
#   endif
#   else
#      define NUM_TFFS_VOLS 0
#   endif
#endif

#define INCLUDE_USER_APPL
#define USER_APPL_INIT	sysApplInit ()

/*
 * PowerPC Decrementer Timer Support
 *
 * The PowerPC Decrementer timer is used as the system clock.
 * It is always included in this BSP.  The following defines
 * are used by the system clock library.
 */

#define BUS_RATE_100            100000000       /* Bus Rate = 100MHz */
#define BUS_RATE_133            133333333       /* Bus Rate = 133MHz */
#define BUS_RATE_166            166666666       /* Bus Rate = 166MHz */
#define SYS_CLK_RATE_MIN	10		/* minimum system clock rate */
#define SYS_CLK_RATE_MAX	5000		/* maximum system clock rate */
#define CPU_BUS_SPD             (sysCpuBusSpd())
#define DEC_CLOCK_FREQ		CPU_BUS_SPD

/* AuxClock Support */

#define AUX_CLK_RATE_MIN        40               /* min auxiliary clock */
#define AUX_CLK_RATE_MAX        5000             /* max auxiliary clock rate */

/* Baudout Support */

#define BAUDOUT         14400
#define BAUDOUT_FREQ    (BAUDOUT * 2)
#define BAUDOUT_TICKS   2400

/*
 * This macro returns the positive difference between two signed ints.
 * Useful for determining delta between two successive decrementer reads.
 */

#define DELTA(a,b)		( abs((int)a - (int)b) )

/* Translation macro */

#define TRANSLATE(x,y,z)\
        ((UINT)(x) - (UINT)(y) + (UINT)(z))

/* System Memory (DRAM) Support */

#define MAX_DRAM	0x80000000

#define SIZE_256KB	0x00040000
#define SIZE_0MB	0x00000000
#define SIZE_1MB	0x00100000
#define SIZE_2MB	0x00200000
#define SIZE_4MB	0x00400000
#define SIZE_8MB	0x00800000
#define SIZE_32MB	0x02000000
#define SIZE_64MB	0x04000000
#define SIZE_128MB	0x08000000
#define SIZE_256MB	0x10000000
#define SIZE_512MB	0x20000000
#define SIZE_1024MB	0x40000000
#define SIZE_2048MB	0x80000000

/* L3 Cache  - L3 Cache Output Hold Control Register Support */

#define L3OHCR_L3AOH		0x0	/* L3AOH L3 Address Output Hold */
#define L3OHCR_L3CLK0_OH	0x7	/* L3_CLK0 Output Hold Time */
#define L3OHCR_L3CLK1_OH	0x7	/* L3_CLK1 Output Hold Time */
#define L3OHCR_L3DOH0		0x0	/* L3 Data Output Hold */
#define L3OHCR_L3DOH8		0x0	/* L3 Data Output Hold */
#define L3OHCR_L3DOH16		0x0	/* L3 Data Output Hold */
#define L3OHCR_L3DOH24		0x0	/* L3 Data Output Hold */
#define L3OHCR_L3DOH32		0x0	/* L3 Data Output Hold */
#define L3OHCR_L3DOH40		0x0	/* L3 Data Output Hold */
#define L3OHCR_L3DOH48		0x0	/* L3 Data Output Hold */
#define L3OHCR_L3DOH56		0x0	/* L3 Data Output Hold */

/* Discovery II Integrated SRAM */

#define MV64360_INTEGRATED_SRAM_BASE 0xf1100000
#define MV64360_INTEGRATED_SRAM_SIZE 0x40000    /* 256k */

/* Boot FLASH Support */

#ifdef FIRMWARE_BOOT
#   define ROM_BASE_ADRS  0x1ff00000
#else
#   define ROM_BASE_ADRS  0xfff00000      /* base address of ROM */
#endif

/* System Control & Status Register Support */

#define SYSTEM_REG_BASE	 MV64360_DEVCS1_BASE_ADDR   /* base of system regs */

/*
 * System Control & Status Register 1
 *
 * Bit 0 = 0  Reserved.
 *     1 = 0  SROM Init. Read only.
 *            0 - SROM init is disabled.
 *            1 - SROM init is enabled and the MV64360 was initialized
 *                using the MV64360 User Defined Initialization SROM.
 *     2 = 1  Fuse Status. Read only.
 *            0 - One of the fuses is open.
 *            1 - All fuses are functional.
 *     3 = 1  FLASH BUSY. Read only.
 *            This bit provides the current state of the FLASH Bank A
 *            StrataFlash device Status pins.
 *     4 = X  ABORT. Read only.
 *            0 - the abort switch is asserted.
 *            1 - the abort switch is not depressed.
 *     5 = X  ENV Safe Start. Read only.
 *            0 - firmware should use the ENV settings programmed in NVRAM.
 *            1 - firmware should use the safe ENV settings.
 *     6 = X  Boot FLASH Bank Select. Read only.
 *            0 - FLASH Bank A is the boot bank.
 *            1 - FLASH Bank B is the boot bank.
 *     7 = X  Reference Clock. Read only.
 *            This bit reflects the current state of the 28.8 KHz reference
 *            clock derived from the 1.8432MHz UART oscillator divided by
 *            64.  This clock may be used as a fixed timing reference.
 */

#define SYSTEM_STATUS_REG_1     (SYSTEM_REG_BASE + 0x0)
#define SYSTEM_STATUS_REG_1_BANK_SELECT_MASK    0x40
#define SYSTEM_STATUS_REG_1_REF_CLK_MASK	0x80

/*
 * System Control & Status Register 2
 *
 * Bit 0 = X  Reserved.
 *     1 = X  Hardware FLASH BankB Boot Block Write Protect Header Staus.
 *	      Read only. 
 *     2 = X  Hardware FLASH BankA Write Protect Header Status. Read only. 
 *     3 = 1  Software FLASH BankB Boot Block Write Protect.
 *	      Read/write.
 *	      0 - clearing this bit and disabling hardware write protect
 *		  will enable writes to the Bank B top 1MB boot block devices.
 *		  This bit is set during reset and must be cleared by 
 *		  software to enable writing of the FLASH bank B boot block.
 *	      1 - write protect the FLASH Bank B devices. 
 *     4 = 0  Thermostat Mask.
 *            0 - thermostat output is disabled from generating
 *                an interrupt.
 *            1 - thermostat output will be enabled to generate
 *                an interrupt on GPP 3.
 *     5 = 1  Software FLASH BankA Write Protect.  Read/write.
 *            0 - Clearing this bit and disabling hardware write protect
 *		  will enable writes to the bank A Flash devices.  This bit
 *		  is set during reset and must be cleared by the software
 *		  to enable writing of the FLASH bank A devices. 
 *            1 - write protect the FLASH Bank A memory devices.
 *     6 = 1  EEPROM Write Protect.  Read/write. Not Used.
 *     7 = 1  Board Fail.  Read/write.
 *            0 - extinguish the front panel Board Fail LED.
 *            1 - illuminate the front panel Board Fail LED.
 */

#define SYSTEM_STATUS_REG_2     (SYSTEM_REG_BASE + 0x1)
#define SYSTEM_STATUS_REG_2_BRDFL_MASK          0x80
#define SYSTEM_STATUS_REG_2_FLASHA_WP_MASK	0x20
#define SYSTEM_STATUS_REG_2_FBOOTB_WP_MASK	0x08
#define SYSTEM_STATUS_REG_2_TSTAT_MASK		0x10

/*
 * System Control & Status Register 3
 *
 * Bit 6:0 = 0  Reserved.
 *       7 = 0  Board Reset.  Read/write.
 *              1 - force a hard reset of the board.
 *              Clears automatically when the board reset is completed. This
 *              bit will always be cleared during a read.
 */

#define SYSTEM_STATUS_REG_3     (SYSTEM_REG_BASE + 0x2)

/*
 * Presence Detect Register
 *
 * Bit 0 = X  PMC 0 Present. Read only.
 *            0 - PMC module is installed in slot 0.
 *            1 - no PMC module is installed in slot 0.
 *     1 = X  PMC 1 Present. Read only.
 *            0 - PMC module is installed in slot 1.
 *            1 - no PMC module is installed in slot 1.
 *     2 = X  PMCSpan Module Present. Read only.
 *	      0 - PMCSpan module is installed.
 *	      1 - no PMCSpan module is installed.
 *     3 = X  PrPMC module installed in PMC Slot 1 ready for 
 *	      enumeration. Read only.
 *	      0 - PrPMC module is not ready for enumeration.
 *	      1 - PrPMC module is ready for enumeration.
 *     4 = X  PrPMC module installed in PMC Slot 2 ready for 
 *	      enumeration. Read only.
 *	      0 - PrPMC module is not ready for enumeration.
 *	      1 - PrPMC module is ready for enumeration.
 *   7:5 = X  Reserved.
 */

#define PRESENCE_DETECT_REG     (SYSTEM_REG_BASE + 0x4)
#define PRESENCE_DETECT_EREADY1_MASK	0x10
#define PRESENCE_DETECT_EREADY0_MASK	0x08

/*
 * Configuration Header/Switch Register
 *
 * Bit 7:0 = X  Read only.
 *              0 - the switch is ON for the header position associated
 *                  with the corresponding bit.
 *              1 - the switch is OFF.
 */

#define CFG_HDR_SWITCH_REG      (SYSTEM_REG_BASE + 0x5)

/*
 * Time Base Enable Register
 *
 * Bit 0 = 1  Processor Time Base Enable.  Read/write.
 *            0 - TBEN pin of the processor is driven low.
 *            1 - TBEN pin of the processor is driven high.
 *   7:1 = 0  Reserved.
 */

#define TIME_BASE_ENB_REG       (SYSTEM_REG_BASE + 0x6)

/* MV64360 - Discovery II Support */

#define MV64360_MAX_BUS		2		/* Max # PCI buses on MV64360 */
#define MV64360_REG_BASE	0xF1000000	/* Base of MV64360 Reg Space */
#define MV64360_REG_SPACE_SIZE	0x10000         /* 64Kb Internal Reg Space */

/* PCI and PCI Auto-Configuration Support */

#define INCLUDE_PCI		/* always include pci */
#define INCLUDE_PCI_AUTOCONF	/* always include pci auto config*/

#define PCI_CLINE_SZ	(_CACHE_ALIGN_SIZE/4)	/* Cache Ln Size = 32 bytes */
#define PCI_LAT_TIMER	0xff	/* Latency Timer Value = 255 PCI clocks */
#define PCI_CONFIG_ADDR_BASE_0	(MV64360_REG_BASE + PCIIF_CFG_ADDR_PCI0)
#define PCI_CONFIG_DATA_BASE_0	(MV64360_REG_BASE + PCIIF_CFG_DATA_PCI0)
#define PCI_CONFIG_ADDR_BASE_1	(MV64360_REG_BASE + PCIIF_CFG_ADDR_PCI1)
#define PCI_CONFIG_DATA_BASE_1	(MV64360_REG_BASE + PCIIF_CFG_DATA_PCI1)

/* PCI/ function identifiers for configuration access prior to pciAutoConfig */

#define PCI0_BUS	0
#define PCI1_BUS	1
#define PCI_FCN0	0
#define PCI_FCN1	1
#define PCI_FCN2	2
#define PCI_FCN3	3
#define PCI_FCN4	4

/* Memory mapping defines */

#define IS_DRAM_ADDRESS(addr) (((int)addr >= LOCAL_MEM_LOCAL_ADRS) && \
                               ((UINT32)addr < (UINT32)sysPhysMemTop()))

#define VME_MEM_LOCAL_START	0x80000000

#define VME_A32_MSTR_LOCAL	VME_MEM_LOCAL_START
#define VME_A32_MSTR_BUS	(0x08000000)
#define VME_A32_MSTR_END	(VME_A32_MSTR_LOCAL + VME_A32_MSTR_SIZE)

#define VME_A24_MSTR_LOCAL	(VME_A32_MSTR_END)
#define VME_A24_MSTR_BUS	(0x00000000)
#define VME_A24_MSTR_END	(VME_A24_MSTR_LOCAL + VME_A24_MSTR_SIZE)

#define VME_A16_MSTR_LOCAL	(VME_A24_MSTR_END)
#define VME_A16_MSTR_BUS	(0x00000000)
#define VME_A16_MSTR_END	(VME_A16_MSTR_LOCAL + VME_A16_MSTR_SIZE)

/* Make sure VME_LOCAL_MEM_END is rounded to nearest 0x0001000 boundary */

#define VME_MEM_LOCAL_END	(VME_A16_MSTR_END)

/* 
 * VME_MEM_SIZE defines the maximum extent of the VME space.  It must
 * be greater than or equal to the ranged defined by VME_MEM_LOCAL_START
 * and VME_MEM_LOCAL_END.  We can increase VME_A32 space by increasing
 * VME_A32_MSTR_SIZE for example and this will push up the value of 
 * VME_MEM_LOCAL_END but we better not define more space in this extent
 * than is represented by VME_MEM_SIZE.  The space defined by VME_MEM_SIZE
 * will be encoded into a Discovery II PCI decode register set and thus the 
 * additional constraint on VME_MEM_SIZE is that it must be a power of 2 
 * so that the PCI decode size register can be properly programmed.
 */

#define VME_MEM_SIZE 		(0x20000000)	/* Must be power of 2 */

#define IS_VME_ADDR_MOD(a) ((a == VME_AM_EXT_SUP_PGM) || \
                            (a == VME_AM_EXT_SUP_DATA) || \
                            (a == VME_AM_EXT_USR_PGM) || \
                            (a == VME_AM_EXT_USR_DATA) || \
                            (a == VME_AM_STD_SUP_PGM) || \
                            (a == VME_AM_STD_SUP_DATA) || \
                            (a == VME_AM_STD_USR_PGM) || \
                            (a == VME_AM_STD_USR_DATA) || \
                            (a == VME_AM_SUP_SHORT_IO) || \
                            (a == VME_AM_USR_SHORT_IO))

/* 
 * Note that VME_CRG_SLV_SIZE is the size of my CRG register group.
 * This group will be mapped onto the VME bus.  VME_CRG_MSTR_SIZE is
 * the size of 16 consecutive CRG groups.  This size will be used to
 * map an outbound window so we can have access to up to 16 different
 * boards's CRG groups (and hence their mailboxes) - allowing VxWorks
 * shared memory to work with mailbox interrupts.
 */

#define VME_CRG_SLV_SIZE        (0x1000)
#define VME_CRG_MSTR_SIZE       (16 * VME_CRG_SLV_SIZE)
#define VME_CRG_MSTR_LOCAL      (VME_A32_MSTR_LOCAL + \
				 VME_A32_MSTR_SIZE - \
				 VME_CRG_MSTR_SIZE)
#define VME_CRG_MSTR_BUS        (0xfb000000)
#define VME_MBOX0_OFFSET	(TEMPE_GCSR_MBOX0 + 3)

/*
 * Allocation of VME space to outbound windows, first set up the out0
 * window to point to the CRG space to allow access to (among other
 * things) other board's mailboxes - necessary for VxWorks shared memory.
 */

#define VME_OUT0_START		(VME_CRG_MSTR_LOCAL)
#define VME_OUT0_SIZE		(VME_CRG_MSTR_SIZE)
#define VME_OUT0_BUS            (VME_CRG_MSTR_BUS)

/* Now out1 will point to some node zero system DRAM */
/* Change to full 160MB  DJA 8/05 */
/* #define VME_DRAM_ACCESS_SIZE (0x00100000) */
#define VME_DRAM_ACCESS_SIZE (0x0a000000)
#define VME_OUT1_START		(VME_A32_MSTR_LOCAL)
#define VME_OUT1_SIZE		(VME_DRAM_ACCESS_SIZE)
#define VME_OUT1_BUS            (VME_A32_MSTR_BUS)

/* Finish up with A24 space (out2) and A16 space (out3) */

#define VME_OUT2_START          (VME_A24_MSTR_LOCAL)
#define VME_OUT2_SIZE           (VME_A24_MSTR_SIZE)
#define VME_OUT2_BUS            (VME_A24_MSTR_BUS)

#define VME_OUT3_START          (VME_A16_MSTR_LOCAL)
#define VME_OUT3_SIZE           (VME_A16_MSTR_SIZE)
#define VME_OUT3_BUS            (VME_A16_MSTR_BUS)

#define VME_OUT0_CFG_PARAMS \
                TRUE,                   /* Window enabled */ \
                0, VME_OUT0_START,      /* Local start addrs (upper = 0) */ \
                0, VME_OUT0_SIZE,       /* Size (upper = 0) */ \
                0, VME_OUT0_BUS,        /* VME bus addr (upper = 0) */ \
                0,                      /* 2eSST broadcast select */ \
                0,                      /* Unused */ \
                FALSE,                  /* Read prefetch enable state */ \
                VME_RD_PREFETCH_2_CACHE_LINES, \
                VME_SST160,             /* 2esst xfer rate */ \
                VME_SCT_OUT,            /* transfer mode */ \
                VME_D32,                /* VME data bus width */ \
                FALSE,                  /* nonsupervisor access */ \
                FALSE,                  /* Not pgm but instead data access */ \
                VME_MODE_A32            /* transfer mode */

#define VME_OUT1_CFG_PARAMS \
                TRUE,                   /* Window enabled */ \
                0, VME_OUT1_START,      /* Local start addrs (upper = 0) */ \
                0, VME_OUT1_SIZE,       /* Size (upper = 0) */ \
                0, VME_OUT1_BUS,        /* VME bus addr (upper = 0) */ \
                0,                      /* 2eSST broadcast select */ \
                0,                      /* Unused */ \
                TRUE,                   /* Read prefetch enable state */ \
                VME_RD_PREFETCH_2_CACHE_LINES, \
                VME_SST160,             /* 2esst xfer rate */ \
                VME_MBLT_OUT,           /* transfer mode */ \
                VME_D32,                /* VME data bus width */ \
                FALSE,                  /* nonsupervisor access */ \
                FALSE,                  /* Not pgm but instead data access */ \
                VME_MODE_A32            /* transfer mode */

#define VME_OUT2_CFG_PARAMS \
                TRUE,                   /* Window enabled */ \
                0, VME_OUT2_START,      /* Local start addrs (upper = 0) */ \
                0, VME_OUT2_SIZE,       /* Size (upper = 0) */ \
                0, VME_OUT2_BUS,        /* VME bus addr (upper = 0) */ \
                0,                      /* 2eSST broadcast select */ \
                0,                      /* Unused */ \
                TRUE,                   /* Read prefetch enable state */ \
                VME_RD_PREFETCH_2_CACHE_LINES, \
                VME_SST160,             /* 2esst xfer rate */ \
                VME_MBLT_OUT,           /* transfer mode */ \
                VME_D32,                /* VME data bus width */ \
                FALSE,                  /* nonsupervisor access */ \
                FALSE,                  /* Not pgm but instead data access */ \
                VME_MODE_A24            /* transfer mode */\

#define VME_OUT3_CFG_PARAMS \
                TRUE,                   /* Window enabled */ \
                0, VME_OUT3_START,      /* Local start addrs (upper = 0) */ \
                0, VME_OUT3_SIZE,       /* Size (upper = 0) */ \
                0, VME_OUT3_BUS,        /* VME bus addr (upper = 0) */ \
                0,                      /* 2eSST broadcast select */ \
                0,                      /* Unused */ \
                TRUE,                   /* Read prefetch enable state */ \
                VME_RD_PREFETCH_2_CACHE_LINES, \
                VME_SST160,             /* 2esst xfer rate */ \
                VME_MBLT_OUT,           /* transfer mode */ \
                VME_D32,                /* VME data bus width */ \
                FALSE,                  /* nonsupervisor access */ \
                FALSE,                  /* Not pgm but instead data access */ \
                VME_MODE_A16            /* transfer mode */

#define VME_OUT4_CFG_PARAMS     VME_OUTx_CFG_DISABLED
#define VME_OUT5_CFG_PARAMS     VME_OUTx_CFG_DISABLED
#define VME_OUT6_CFG_PARAMS     VME_OUTx_CFG_DISABLED
#define VME_OUT7_CFG_PARAMS     VME_OUTx_CFG_DISABLED

#define VME_OUT_WINDOW_CFG_PARAMS \
                VME_OUT0_CFG_PARAMS, \
                VME_OUT1_CFG_PARAMS, \
                VME_OUT2_CFG_PARAMS, \
                VME_OUT3_CFG_PARAMS, \
                VME_OUT4_CFG_PARAMS, \
                VME_OUT5_CFG_PARAMS, \
                VME_OUT6_CFG_PARAMS, \
                VME_OUT7_CFG_PARAMS

#define VME_IN0_CFG_PARAMS      VME_INx_CFG_DISABLED
#define VME_IN1_CFG_PARAMS      VME_INx_CFG_DISABLED
#define VME_IN2_CFG_PARAMS      VME_INx_CFG_DISABLED
#define VME_IN3_CFG_PARAMS      VME_INx_CFG_DISABLED
#define VME_IN4_CFG_PARAMS      VME_INx_CFG_DISABLED
#define VME_IN5_CFG_PARAMS      VME_INx_CFG_DISABLED
#define VME_IN6_CFG_PARAMS      VME_INx_CFG_DISABLED
#define VME_IN7_CFG_PARAMS      VME_INx_CFG_DISABLED

/* 
 * Make shared memory adjustment for inbound window mapping.  This only
 * applies if SM_OFF_BOARD is set to FALSE, meaning that we are the node
 * which is hosting the shared memory pool.  In that case, we must make
 * sure that we have an inbound window mapped to allow access to our
 * shared memory pool from the VME bus, instead of having no inbound
 * windows programmed as would normally be the case if SM_OFF_BOARD
 * is set to FALSE.
 */

#if 0
#if (SM_OFF_BOARD == FALSE)
#   undef  VME_IN0_CFG_PARAMS
#   define VME_IN0_CFG_PARAMS            \
                TRUE,                    \
                0,                       \
                VME_A32_MSTR_BUS,        \
                0,                       \
                (SM_MEM_ADRS +           \
                 SM_MEM_SIZE +           \
                 SM_OBJ_MEM_SIZE),       \
                0,                       \
                LOCAL_MEM_LOCAL_ADRS,    \
                VME_RD_THRESHOLD_EMPTY,  \
                VME_VFS_FIFO_64,         \
                VME_SST160,              \
                (VME_BLT_IN |            \
                 VME_MBLT_IN |           \
                 VME_2eVME_IN |          \
                 VME_2eSST_IN |          \
                 VME_2eSST_BCAST_IN ),   \
                 VME_SPACE_A32,          \
                 TRUE,                   \
                 TRUE,                   \
                 TRUE,                   \
                 TRUE,                   \
                 0
#endif
#endif

      /* Setup a slave window to provide VME access of CPU memory 
 DJA 8/05  - Set to  256MBytes 0x10000000. Put it above the standard
 A32 Master window to avoid self-access*/

#undef VME_A32_SLV_SIZE
#define VME_A32_SLV_SIZE  0x10000000
#   undef  VME_IN1_CFG_PARAMS
#   define VME_IN1_CFG_PARAMS            \
                TRUE,                    \
                0,                       \
                (VME_A32_MSTR_BUS +      \
                 VME_A32_MSTR_SIZE),     \
                0,                       \
                VME_A32_SLV_SIZE,        \
                0,                       \
                LOCAL_MEM_LOCAL_ADRS,    \
                VME_RD_THRESHOLD_EMPTY,  \
                VME_VFS_FIFO_64,         \
                VME_SST160,              \
                (VME_BLT_IN |            \
                 VME_MBLT_IN |           \
                 VME_2eVME_IN |          \
                 VME_2eSST_IN |          \
                 VME_2eSST_BCAST_IN ),   \
                 VME_SPACE_A32,          \
                 TRUE,                   \
                 TRUE,                   \
                 TRUE,                   \
                 TRUE,                   \
                 0




/* 
 * Tempe CRG access values - these define a 64-bit access address and
 * attribute bit field for accessing my Tempe CSR register set from the
 * VME bus.  We're only dealing with a 32-bit address here so the "upper"
 * register (TEMPE_CBAU_VAL) is zero, the "lower" register (TEMPE_CBAL_VAL) 
 * contains the 3 high-order significant digits of the 32-bit address.  The 
 * "attribute" register (TEMPE_CSRAT_VAL) contains the attribute bits which
 * allow for the fillowing accesses to this space: data, pgogram, 
 * nonpriviledged, supervisor, A32 - and "enabled".
 */

#define TEMPE_CBAU_VAL	0		/* Upper 64-bits zero */
#define TEMPE_CBAL_VAL  (VME_CRG_MSTR_BUS+(sysProcNumGet()*VME_CRG_SLV_SIZE))
#define TEMPE_CSRAT_VAL 0x00000AF     /* Enable for A32 and all other access */

/* Support for VME bus locking */

#define LOCK_TIMEOUT		10
#define DECREMENTER_PRESCALE	4
#define USECS_PER_SECOND	1000000

/* Support for sysBusTas() RMW cycle */

#define	VME_RMW_ENABLE_BITS		0x80000000
#define VME_RMW_COMPARE_BITS		0x00000000
#define VME_RMW_SWAP_TO_SET		0x80000000

/* 
 * PCI Processor (LOCAL) and Bus (BUS) Definitions
 *
 * The following definitions determine the address ranges for PCI Memory
 * and I/O space on both the processor bus and the PCI memory and
 * I/O busses. These definitions are used to configure the PCI-Host
 * Bridge as well as perform PCI Auto-Configuration.  There are two sets
 * of definitions, one for each of the MV64360's busses (0.0 and 1.0). 
 *
 * PCI Memory Space:
 * This is a one-to-one mapping.  The address used on the processor bus is the
 * address used on the PCI memory bus.
 *
 * PCIx_MSTR_MEMIO_LOCAL =  PCI Non-Prefetchable Memory Space 
 *                          (processor bus view), 'x' = 0 or 1 to indicate
 *                          which of the MV64360 busses are being mapped
 *                          (0.0 or 1.0).
 * PCIx_MSTR_MEMIO_BUS =    PCI Non-Prefetchable Memory Space 
 *                          (PCI memory bus view), 'x' = 0 or 1 to indicate 
 *                          which of the MV64360 busses are being mapped 
 *                          (0.0 or 1.0).
 *
 * PCIx_MSTR_MEM_LOCAL  =   PCI Prefetchable Memory Space
 *                          (processor bus view), 'x' = 0 or 1 to indicate
 *                          which of the MV64360 busses are being mapped
 *                          (0.0 or 1.0).
 * PCIx_MSTR_MEM_BUS  =     PCI Prefetchable Memory Space
 *                          (PCI memory bus view), 'x' = 0 or 1 to indicate 
 *                          which of the MV64360 busses are being mapped 
 *                          (0.0 or 1.0).
 *
 * PCI I/O Space:
 * This is not a one-to-one mapping.  For Bus 1.0 the I/O space will start
 * at 0x00000000 on the PCI I/O bus.  For Bus 0.0 the I/O space (32-bit only)
 * will not start at 0x00000000, but at the top of Bus 1.0's I/O space.  This 
 * is to prevent overlap between bus addresses. 
 *
 * NOTE: Bus 0.0 does not contain 16-bit PCI I/O space.
 *
 * ISA_MSTR_IO_LOCAL =   PCI 16-bit I/O Space (processor bus view) - this
 *                       space only exists on bus 1.0.
 *
 * ISA_MSTR_IO_BUS =     PCI 16-bit I/O Space (PCI I/O bus view) - this
 *                       space only exists on bus 1.0.
 *
 * PCIx_MSTR_IO_LOCAL =  PCI 32-bit I/O Space (processor bus view)
 *                       'x' = 0 or 1 to indicate which of the MV64360 
 *                       busses are being mapped (0.0 or 1.0).
 *
 * PCIx_MSTR_IO_BUS =    PCI 32-bit I/O Space (PCI I/O bus view)
 *                       'x' = 0 or 1 to indicate which of the MV64360 
 *                       busses are being mapped (0.0 or 1.0).
 * 
 * Note that PCI window base addresses and sizes must conform to the
 * following two constraints:
 *
 * 1.  The window size must be a power of 2.
 *
 * 2.  The window base address must be an integral multiple of the
 *     size.
 * 
 * The constraints are enforced at compile time with error generation
 * if a violation occurs.  
 */
   
/* Start of PCI memory space */

#define PCI_MEM_LOCAL_START	(VME_MEM_LOCAL_START + VME_MEM_SIZE)

/*
 * Bus 0.0 (PCI0) PCI Memory Definitions -
 * First handle 0.0 (PCI0) memio (nonprefetchable) window 
 */

#define PCI0_MEM_LOCAL_START	PCI_MEM_LOCAL_START

#define	PCI0_MSTR_MEMIO_LOCAL	PCI0_MEM_LOCAL_START
#define	PCI0_MSTR_MEMIO_BUS	PCI0_MSTR_MEMIO_LOCAL	/* null	mapping	*/

#define	PCI0_MEMIO_LOCAL_END 	(PCI0_MSTR_MEMIO_LOCAL + PCI0_MSTR_MEMIO_SIZE)

/*
 * PCI0 mem (prefetchable) window appears at the next 'mem size' boundary 
 * beyond the end of the PCI0 memio (nonprefetchable) window.
 */

#define	PCI0_MSTR_MEM_LOCAL	(PCI0_MEMIO_LOCAL_END & \
				 ~((PCI0_MSTR_MEM_SIZE) - 1))

#define	PCI0_MSTR_MEM_BUS	(PCI0_MSTR_MEM_LOCAL)	/* null	mapping	*/

#define	PCI0_MEM_LOCAL_END	(PCI0_MSTR_MEM_LOCAL + PCI0_MSTR_MEM_SIZE)

/*
 * Bus 1.0 (PCI1) PCI Memory Definitions -
 * First handle 1.0 (PCI1) memio (nonprefetchable) window.  This
 * window must start on a 'memio size' boundary beyond the end 
 * of the PCI0 mem (prefetchable) window.
 */

#define	PCI1_MEM_LOCAL_START	(PCI0_MEM_LOCAL_END)

#define	PCI1_MSTR_MEMIO_LOCAL	(PCI1_MEM_LOCAL_START)

#define	PCI1_MSTR_MEMIO_BUS	(PCI1_MSTR_MEMIO_LOCAL)	/* null	mapping	*/

#define	PCI1_MEMIO_LOCAL_END 	(PCI1_MSTR_MEMIO_LOCAL + PCI1_MSTR_MEMIO_SIZE)

/*
 * PCI1 mem (prefetchable) window appears at the next 'mem size' boundary 
 * beyond the end of the PCI1 memio (nonprefetchable) window.
 */

#define PCI1_MSTR_MEM_LOCAL	(PCI1_MEMIO_LOCAL_END)

#define PCI1_MSTR_MEM_BUS	(PCI1_MSTR_MEM_LOCAL)	/* null mapping */

#define PCI1_MEM_LOCAL_END	(PCI1_MSTR_MEM_LOCAL + PCI1_MSTR_MEM_SIZE)

#define PCI_MEM_LOCAL_END	(PCI1_MEM_LOCAL_END)

/*
 * PCI I/O Definitions -
 * The PCI I/O space consists of two separate windows.  The first
 * window appears on PCI bus 1.0 (PCI1) and consists of 
 * ISA I/O (16-bit I/O) space followed by the rest of PCI bus 1.0 I/O space.  
 * Immediately following this window comes the I/O space window
 * for PCI bus 0.0 (PCI0).  Note that the ISA I/O space is totally confined
 * to PCI bus 1.0.  As with PCI memory windows, the two size and base
 * address alignment rules (see preceding comment) are enforced.
 *
 * Let's start with the PCI 1 (PCI1) bus I/O window definitions:
 */

#define PCI_IO_LOCAL_START	(PCI_MEM_LOCAL_END)

#define	PCI1_IO_LOCAL_START	(PCI_IO_LOCAL_START)

#define PCI1_IO_SIZE		(ISA_MSTR_IO_SIZE + PCI1_MSTR_IO_SIZE)

#define	ISA_MSTR_IO_LOCAL	(PCI1_IO_LOCAL_START)
#define	ISA_MSTR_IO_BUS		(0x00000000)

#define	PCI1_MSTR_IO_LOCAL	(ISA_MSTR_IO_LOCAL + ISA_MSTR_IO_SIZE)
#define	PCI1_MSTR_IO_BUS	(TRANSLATE(PCI1_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_BUS))

#define PCI1_IO_LOCAL_END       (PCI1_MSTR_IO_LOCAL + PCI1_MSTR_IO_SIZE)

/* Now for the PCI bus 0.0 (PCI0) I/O window definitions */

#define PCI0_IO_LOCAL_START	(PCI1_IO_LOCAL_END)

#define PCI0_MSTR_IO_LOCAL	(PCI0_IO_LOCAL_START)

#define PCI0_MSTR_IO_BUS	(TRANSLATE(PCI0_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_BUS))

#define PCI0_IO_LOCAL_END	(PCI0_MSTR_IO_LOCAL + PCI0_MSTR_IO_SIZE)

#define PCI_IO_LOCAL_END	PCI0_IO_LOCAL_END

/* 
 * Compiler Errors:
 *
 * Signal a compile time error if:
 *
 * 1. The VME local memory space extends beyond the beginning of the PCI
 *    memory space.
 *
 * 2. The PCI local I/O space extends beyond the beginning of the 
 *    MV64360 internal register space.
 */

#if (VME_LOCAL_END > PCI_MEM_LOCAL_START)
# error local VME memory space extends into local PCI memory space
#endif

#if (PCI_IO_LOCAL_END > MV64360_REG_BASE)
# error PCI I/O space extends beyond MV64360 internal register base
#endif

/*
 * Compiler Warnings:
 *
 * The following is provided to remove compiler warnings when using the
 * compiler flags: 
 * -pedantic 
 * -Wall 
 * -W 
 * -Wmissing-declarations 
 * -Wstrict-prototypes 
 * -Wmissing-prototypes
 *
 * These compiler warnings were due to the fact that "BSP implemented"
 * routines do not always use all of the function arguments specified
 * in the function declarations contained within vxWorks header files.
 */

#if (defined __GNUC__)
#   define ARG_UNUSED __attribute__((unused))
#else
#   define ARG_UNUSED
#endif

/* memory map as seen on the PCI bus */

#define PCI_SLV_MEM_BUS     0x00000000	/* memory seen from PCI bus */
#define PCI_SLV_MEM_SIZE    DRAM_SIZE	/* DRAM size PCI bus = size CPU bus */

/* PCI Device/Vendor IDs */

#define PCI_ID_MV64360		0x646011AB	/* Id for MV64360 PHB */
#define PCI_ID_TEMPE		0x014810e3	/* Id for Tempe VME-PCI Brdg */
#define PCI_ID_PCI6520		0x00303388	/* Id for PMCSpan bridge chip */
#define PCI_ID_IBC		0x056510AD	/* Id for W83C55x PIB */
#define PCI_ID_IDE		0x010510AD	/* Id for W83C55x IDE */
#define PCI_ID_NCR810           0x00011000      /* Id for SYM53C810A Chip */
#define PCI_ID_NCR825           0x00031000      /* Id for SYM53C825 Chip */
#define PCI_ID_NCR860           0x00061000      /* Id for SYM53C860 Chip */
#define PCI_ID_NCR875           0x000f1000      /* Id for SYM53C875 Chip */
#define PCI_ID_NCR895           0x000c1000      /* Id for SYM53C895 Chip */
#define PCI_ID_NCR895A          0x00121000      /* Id for SYM53C895A Chip */
#define PCI_ID_SCSI             PCI_ID_NCR895A  /* Id for Primary SCSI */
#define PCI_ID_SEC_SCSI         PCI_ID_NCR895   /* Id for Secondary SCSI */

/* Hardwired bus numbers */

#define TEMPE_PCI_BUS		0	/* PCI bus of Tempe PCI-VME bridge */
#define TEMPE_PCI_DEV		5	/* Device number of Tempe */
#define TEMPE_PCI_FCN		0	/* Function numbrer of Tempe */

/*
 * Shared memory related parameters and
 * Software Test and Set Support:
 *
 * Note: This BSP requires a modified software Test and Set algorithm.
 * SM_TAS_TYPE is set to SM_TAS_HARD despite the lack of a hardware TAS
 * mechanism to force the use of a BSP-specific software TAS algorithm.
 * Shared Memory Interrupt Type.
 * Interrupt this target with a 1-byte write mailbox.
 * VME_A32 space, address based on procNum, value is SIG1_INTR_SET.
 */

#undef SM_TAS_TYPE
#define SM_TAS_TYPE     SM_TAS_HARD

#define SM_INT_ARG1     VME_AM_EXT_SUP_DATA
#define SM_INT_ARG2     (VME_CRG_MSTR_BUS + VME_MBOX0_OFFSET + \
                         (sysProcNumGet() * VME_CRG_SLV_SIZE))
#define SIG1_INTR_SET	0x80	/* Shared mem Mailbox interrupt "write" value */
#define SM_INT_ARG3     SIG1_INTR_SET

#define SM_BASE 	(LOCAL_MEM_LOCAL_ADRS + SM_MEM_ADRS)
#define SM_LIMIT 	(SM_BASE + SM_MEM_SIZE)

/* BSP configuration error policy */

#define CONTINUE_EXECUTION	0       /* Tolerate VPD/Configuration errors */
#define EXIT_TO_SYSTEM_MONITOR	1       /* Transfer to System Monitor */

/* MV64360 I2C Interface Support & VPD Support */

#define SPD_EEPROM_ADDR_0       0xA0    /* I2C address for banks 0 & 1 */
#define SPD_EEPROM_ADDR_1       0xA2    /* I2C address for banks 2 & 3 */
#define VPD_BRD_EEPROM_I2C_ADDR           0xA8  /* I2C address of VPD EEPROM */
#define USR_BRD_EEPROM_I2C_ADDR           0xAA  /* I2C address of User EEPROM */
#define MV64360_USR_INIT_EEPROM_I2C_ADDR  0xA6  /* I2C addr for MV64360 User */
                                                /* defined init EEPROM */
#define DS1621_TEMP_SENSOR_I2C_ADRS       0x90  /* I2C addr for DS1621 */
                                                /* temperature sensor */
#define VALID_I2C_WRT_ADDR      USR_BRD_EEPROM_I2C_ADDR, \
                                MV64360_USR_INIT_EEPROM_I2C_ADDR, \
                                0xFFFFFFFF
#define I2C_SINGLE_BYTE_ADDR    1       /* one-byte address devices */
#define I2C_DUAL_BYTE_ADDR      2       /* two-byte address devices */
#define I2C_TRIPLE_BYTE_ADDR    3       /* three-byte address devices */
#define SPD_BYTE_ADDR_SZ                I2C_SINGLE_BYTE_ADDR
#define VPD_BYTE_ADDR_SZ                I2C_DUAL_BYTE_ADDR
#define USR_BYTE_ADDR_SZ                I2C_DUAL_BYTE_ADDR
#define MV64360_USR_INIT_BYTE_ADDR_SZ   I2C_DUAL_BYTE_ADDR
#define DEFAULT_PRODUCT_ID      "Unknown"
#define DEFAULT_CPU_ID		"Unknown"
#define VPD_BRD_OFFSET      0       /* offset into brd's eeprom for vpd data */
#define VPD_PKT_LIMIT       25      /* Max number of packets expected */
#define VPD_EEPROM_SIZE     512     /* size of VPD serial EEPROM */

/*
 * Support for determining if we're ROM based or not.  _sysInit
 * saves the startType parameter at location ROM_BASED_FLAG.
 */

#define PCI_AUTOCONFIG_FLAG_OFFSET (0x4c00)
#define PCI_AUTOCONFIG_FLAG        (*(UCHAR *)(LOCAL_MEM_LOCAL_ADRS + \
				     PCI_AUTOCONFIG_FLAG_OFFSET))
#define PCI_AUTOCONFIG_DONE        (PCI_AUTOCONFIG_FLAG != 0)

/*
 * Support for determining which of the MV64360's PCI interfaces to access 
 * based on the PCI bus number specified.  sysPciAutoConfig() called from
 * sysHwInit() saves the highest numbered bus connected to Bus interface 0.0 
 * and this bus number, plus 1, is equivalent to bus 0 on Bus interface 1.0.
 */

#define PCI_BUS_ALIAS_OFFSET \
        (ROUND_UP((PCI_AUTOCONFIG_FLAG_OFFSET + sizeof(UCHAR)), sizeof(int)))
#define PCI_BUS_ALIAS              (*(UINT *)(LOCAL_MEM_LOCAL_ADRS + \
                                     PCI_BUS_ALIAS_OFFSET))

/* IPMC SCSI Support */

#define NCR810_DELAY_MULT       10	/* ncr810/ncr825 delay loop count */
#define SYS_REG_SIOP_HW_REGS	{0,0,0,0,0,1,0,0,0,0,0}

/* 64-bit address support (Tempe) */

#define MAKE64(high32,low32) (((UINT64)high32<<32)+((UINT64)low32))
#define UNMAKE64(val64,high32,low32) { high32 = (val64>>32) & 0xffffffff; \
				       low32  = (val64) & 0xffffffff; }


/* 
 * Interrupt number assignments: 
 * Each bit in the mv64360 main cause
 * (low and high) have an associated interrupt bit assigned below.
 * ICI_MICL_INT_NUMx defines interrupt numbers for bits in the MV64360
 * main cause low register.  ICI_MICH_INT_NUMx defines interrupt
 * numbers for the MV64360 main cause high register.  In addition
 * separate interrupt numbers are assigned for the GPP pins
 * GPP_IC_INT_NUM_xx.  In this architecture, VxWorks interrupt vector
 * numbers and interrupt level numbers are numerically equivalent and
 * equal to the "interrupt number" assignments coded below.
 */

/* "Main Interrupt Cause Low" register interrupt bit numbers */

#define ICI_MICL_INT_NUM_MIN	(0)
#define ICI_MICL_INT_NUM_0	(0 +  ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_1	(1 +  ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_2	(2 +  ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_3	(3 +  ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_4	(4 +  ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_5	(5 +  ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_6	(6 +  ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_7	(7 +  ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_8	(8 +  ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_9	(9 +  ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_10	(10 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_11	(11 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_12	(12 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_13	(13 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_14	(14 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_15	(15 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_16	(16 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_17	(17 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_18	(18 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_19	(19 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_20	(20 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_21	(21 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_22	(22 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_23	(23 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_24	(24 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_25	(25 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_26	(26 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_27	(27 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_28	(28 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_29	(29 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_30	(30 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_31	(31 + ICI_MICL_INT_NUM_MIN)
#define ICI_MICL_INT_NUM_MAX    (ICI_MICL_INT_NUM_31)

/* "Main Interrupt Cause High" register interrupt bit numbers */

#define ICI_MICH_INT_NUM_MIN	(1 +  ICI_MICL_INT_NUM_MAX)
#define ICI_MICH_INT_NUM_0	(0 +  ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_1	(1 +  ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_2	(2 +  ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_3	(3 +  ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_4	(4 +  ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_5	(5 +  ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_6	(6 +  ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_7	(7 +  ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_8	(8 +  ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_9	(9 +  ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_10	(10 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_11	(11 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_12	(12 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_13	(13 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_14	(14 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_15	(15 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_16	(16 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_17	(17 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_18	(18 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_19	(19 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_20	(20 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_21	(21 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_22	(22 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_23	(23 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_24	(24 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_25	(25 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_26	(26 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_27	(27 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_28	(28 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_29	(29 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_30	(30 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_31	(31 + ICI_MICH_INT_NUM_MIN)
#define ICI_MICH_INT_NUM_MAX    (ICI_MICH_INT_NUM_31)

/* Interrupt vector (number) assignments  - GPP cause register */

#define GPP_IC_INT_NUM_MIN	(1 +  ICI_MICH_INT_NUM_MAX)
#define GPP_IC_INT_NUM_0	(0 +  GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_1	(1 +  GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_2	(2 +  GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_3	(3 +  GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_4	(4 +  GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_5	(5 +  GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_6	(6 +  GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_7	(7 +  GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_8	(8 +  GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_9	(9 +  GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_10	(10 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_11	(11 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_12	(12 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_13	(13 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_14	(14 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_15	(15 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_16	(16 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_17	(17 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_18	(18 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_19	(19 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_20	(20 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_21	(21 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_22	(22 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_23	(23 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_24	(24 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_25	(25 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_26	(26 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_27	(27 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_28	(28 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_29	(29 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_30	(30 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_31	(31 + GPP_IC_INT_NUM_MIN)
#define GPP_IC_INT_NUM_MAX	(GPP_IC_INT_NUM_31)

/* Tempe PCI-VME chip interrupt support */

#define TEMPE_INT_NUM_MIN	(GPP_IC_INT_NUM_MAX + 1)

/* Define Tempe interrupt numbers (assiciated with intConnect()) */

#define TEMPE_INT_NUM_IRQ1	(TEMPE_INT_NUM_MIN + 0)
#define TEMPE_INT_NUM_IRQ2	(TEMPE_INT_NUM_MIN + 1)
#define TEMPE_INT_NUM_IRQ3	(TEMPE_INT_NUM_MIN + 2)
#define TEMPE_INT_NUM_IRQ4	(TEMPE_INT_NUM_MIN + 3)
#define TEMPE_INT_NUM_IRQ5	(TEMPE_INT_NUM_MIN + 4)
#define TEMPE_INT_NUM_IRQ6	(TEMPE_INT_NUM_MIN + 5)
#define TEMPE_INT_NUM_IRQ7	(TEMPE_INT_NUM_MIN + 6)
#define TEMPE_INT_NUM_ACFL	(TEMPE_INT_NUM_MIN + 7)
#define TEMPE_INT_NUM_SYSFL	(TEMPE_INT_NUM_MIN + 8)
#define TEMPE_INT_NUM_IACK	(TEMPE_INT_NUM_MIN + 9)
#define TEMPE_INT_NUM_VIEEN	(TEMPE_INT_NUM_MIN + 10)
#define TEMPE_INT_NUM_VERR	(TEMPE_INT_NUM_MIN + 11)
#define TEMPE_INT_NUM_PERR	(TEMPE_INT_NUM_MIN + 12)
#define TEMPE_INT_NUM_MB0	(TEMPE_INT_NUM_MIN + 13)
#define TEMPE_INT_NUM_MB1	(TEMPE_INT_NUM_MIN + 14)
#define TEMPE_INT_NUM_MB2	(TEMPE_INT_NUM_MIN + 15)
#define TEMPE_INT_NUM_MB3	(TEMPE_INT_NUM_MIN + 16)
#define TEMPE_INT_NUM_LM0	(TEMPE_INT_NUM_MIN + 17)
#define TEMPE_INT_NUM_LM1	(TEMPE_INT_NUM_MIN + 18)
#define TEMPE_INT_NUM_LM2	(TEMPE_INT_NUM_MIN + 19)
#define TEMPE_INT_NUM_LM3	(TEMPE_INT_NUM_MIN + 20)
#define TEMPE_INT_NUM_DMA0	(TEMPE_INT_NUM_MIN + 21)
#define TEMPE_INT_NUM_DMA1	(TEMPE_INT_NUM_MIN + 22)

#define TEMPE_INT_NUM_MAX	(TEMPE_INT_NUM_DMA1)

/*
 * Define the entry into INT_UTIL_DESC intUtil [] which allows us
 * to hook Tempe (Tsi148) interrupt management and ISA interrupt management
 * into the mv64360 interrupt handler.  The INT_USR_EXTRA component
 * can be initialized inside of "config.h" to refer to any special
 * intEnable()/intDisable() code that the user wants to invoke for
 * the user reserved interrupt vector numbers.
 */

#ifndef _ASMLANGUAGE
    STATUS sysTempeIntEnable (int intLevel);
    STATUS sysTempeIntDisable (int intLevel);
#endif

#define INT_UTIL_EXTRA	,{TEMPE_INT_NUM_MIN,TEMPE_INT_NUM_MAX, \
			  sysTempeIntEnable,sysTempeIntDisable} \
			  INT_USR_EXTRA

/*
 * Define the minimum and maximum interrupt vector numbers available for user
 * applications.  Note: See INT_USR_EXTRA in "config.h".
 */

#define USR_INT_NUM_MIN		0xc0
#define USR_INT_NUM_MAX		0xff

/* IPMC (Winbond) Interrupt Support */

#if ((defined INCLUDE_PCX7307_SIO) || (defined INCLUDE_Z85230_SIO))
#   define PIB_INT_LVL		(GPP_IC_INT_NUM_5)
#   define PIB_INT_VEC		(0x00 + PIB_INT_LVL)
#   define ISA_INT_NUM_MIN      (1 + TEMPE_INT_NUM_MAX)
#   define COM2_INT_LVL		(0x04 + ISA_INT_NUM_MIN)
#   define COM3_INT_LVL		(0x03 + ISA_INT_NUM_MIN)
#   define COM4_INT_LVL		(0x09 + ISA_INT_NUM_MIN)
#   define COM5_INT_LVL		(0x09 + ISA_INT_NUM_MIN)	
#   define Z85230_INT_LVL	(COM4_INT_LVL)
#   define ISA_INT_NUM_MAX	(COM5_INT_LVL)
#   define COM2_INT_VEC		(0x00 + COM2_INT_LVL)
#   define COM3_INT_VEC		(0x00 + COM3_INT_LVL)
#   define COM4_INT_VEC		(0x00 + COM4_INT_LVL)
#   define COM5_INT_VEC		(0x00 + COM5_INT_LVL)
#   define Z85230_INT_VEC	(0x00 + Z85230_INT_LVL)
#   ifndef _ASMLANGUAGE
       int sysIbcIntEnable (int);
       int sysIbcIntDisable (int);
#   endif /* _ASMLANGUAGE */
#   undef INT_UTIL_EXTRA
#   define INT_UTIL_EXTRA       ,{TEMPE_INT_NUM_MIN,TEMPE_INT_NUM_MAX, \
                                  sysTempeIntEnable,sysTempeIntDisable} \
                                ,{ISA_INT_NUM_MIN,ISA_INT_NUM_MAX, \
                                  sysIbcIntEnable,sysIbcIntDisable} \
				  INT_USR_EXTRA
#   define PCI_MSTR_IACK_REG	(MV64360_REG_BASE + PCIIF_INTERRUPT_ACK_PCI1)
#endif /* INCLUDE_PCX7307_SIO, INCLUDE_Z85230_SIO */

/* MV64360 Watchdog Timer Interrupt Support */

#define WDOG_INT_LVL    (GPP_IC_INT_NUM_6)
#define WDOG_INT_VEC    (INUM_TO_IVEC(GPP_IC_INT_NUM_6))

/* 
 * In addition to the interrupts defined above (associated with the main
 * interrupt cause low and high registers), we also have some external
 * interrupt sources which are associated with MPP (Multi-Purpose Pins)
 * which are in turn programmed as GPP (General Purpose Port) pins.  On
 * the MVME6100, any GPP related interrupt causes one of the 4 bits (24,
 * 25, 26, or 27) in the "main interrupt cause high" register to be set
 * as well as the associated "local" bit in the GPP value register.
 * Here's the selection information for these GPP pins.
 */

#define MPP_SELECT_GPP		(0x0)
#define MPP_SELECT_GNT		(0x1)
#define MPP_SELECT_REQ		(0x1)
#define MPP_INIT_ACT		(0x3)
#define MPP_WDE			(0x4)
#define MPP_WDNMI		(0x4)

#define GPP_INPUT		(0x0)
#define GPP_OUTPUT		(0x1)

#define GPP_ACTIVE_HI		(0x0)
#define GPP_ACTIVE_LOW		(0x1)

#define MPP_PIN0_SEL  (MPP_SELECT_GPP) /* COM0/COM1 ORed */
#define MPP_PIN1_SEL  (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN2_SEL  (MPP_SELECT_GPP) /* Abort */
#define MPP_PIN3_SEL  (MPP_SELECT_GPP) /* RTC & thermostat ORed */
#define MPP_PIN4_SEL  (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN5_SEL  (MPP_SELECT_GPP) /* IPMC761 */
#define MPP_PIN6_SEL  (MPP_SELECT_GPP) /* MV64360 WDNMI# interrupt */
#define MPP_PIN7_SEL  (MPP_SELECT_GPP) /* BCM5421S PHY interrupts ORed */
#define MPP_PIN8_SEL  (MPP_SELECT_GNT) /* PCI Bus 1.0 PMC s0 agent grant */
#define MPP_PIN9_SEL  (MPP_SELECT_REQ) /* PCI Bus 1.0 PMC s0 agent req */
#define MPP_PIN10_SEL (MPP_SELECT_GNT) /* PCI Bus 1.0 PMC s1 agent grant */
#define MPP_PIN11_SEL (MPP_SELECT_REQ) /* PCI Bus 1.0 PMC s1 agent req */
#define MPP_PIN12_SEL (MPP_SELECT_GNT) /* PCI Bus 1.0 PMC s0 grant */
#define MPP_PIN13_SEL (MPP_SELECT_REQ) /* PCI Bus 1.0 PMC s0 req */
#define MPP_PIN14_SEL (MPP_SELECT_GNT) /* PCI Bus 1.0 PMC s1 grant */
#define MPP_PIN15_SEL (MPP_SELECT_REQ) /* PCI Bus 1.0 PMC s1 req */
#define MPP_PIN16_SEL (MPP_SELECT_GPP) /* PCI Bus 1.0 PMC INTA#, PMC1 INTC# */
#define MPP_PIN17_SEL (MPP_SELECT_GPP) /* PCI Bus 1.0 PMC INTB#, PMC1 INTD# */
#define MPP_PIN18_SEL (MPP_SELECT_GPP) /* PCI Bus 1.0 PMC INTC#, PMC1 INTA# */
#define MPP_PIN19_SEL (MPP_SELECT_GPP) /* PCI Bus 1.0 PMC INTD#, PMC1 INTA# */
#define MPP_PIN20_SEL (MPP_SELECT_GPP) /* PCI Bus 0.0 VME LINT0# */
#define MPP_PIN21_SEL (MPP_SELECT_GPP) /* PCI Bus 0.0 VME LINT1# */
#define MPP_PIN22_SEL (MPP_SELECT_GPP) /* PCI Bus 0.0 VME LINT2# */
#define MPP_PIN23_SEL (MPP_SELECT_GPP) /* PCI Bus 0.0 VME LINT3# */
#define MPP_PIN24_SEL (MPP_INIT_ACT)   /* SROM initialization active */
#define MPP_PIN25_SEL (MPP_WDE)        /* WDE# output */
#define MPP_PIN26_SEL (MPP_WDNMI)      /* WDNMI# output */
#define MPP_PIN27_SEL (MPP_SELECT_GPP) /* Reserved for interrupt */
#define MPP_PIN28_SEL (MPP_SELECT_GNT) /* Tempe ASIC (VMEbus) grant */
#define MPP_PIN29_SEL (MPP_SELECT_REQ) /* Tempe ASIC (VMEbus) request */
#define MPP_PIN30_SEL (MPP_SELECT_GNT) /* PCI6520 (PMCSpan bridge) grant */
#define MPP_PIN31_SEL (MPP_SELECT_REQ) /* PCI6520 (PMCSpan bridge) request */

#define MPP_CTRL0_VAL ((MPP_PIN0_SEL  << 0)  | \
                       (MPP_PIN1_SEL  << 4)  | \
                       (MPP_PIN2_SEL  << 8)  | \
                       (MPP_PIN3_SEL  << 12) | \
                       (MPP_PIN4_SEL  << 16) | \
                       (MPP_PIN5_SEL  << 20) | \
                       (MPP_PIN6_SEL  << 24) | \
                       (MPP_PIN7_SEL  << 28))
#define MPP_CTRL1_VAL ((MPP_PIN8_SEL  << 0)  | \
                       (MPP_PIN9_SEL  << 4)  | \
                       (MPP_PIN10_SEL << 8)  | \
                       (MPP_PIN11_SEL << 12) | \
                       (MPP_PIN12_SEL << 16) | \
                       (MPP_PIN13_SEL << 20) | \
                       (MPP_PIN14_SEL << 24) | \
                       (MPP_PIN15_SEL << 28))
#define MPP_CTRL2_VAL ((MPP_PIN16_SEL << 0)  | \
                       (MPP_PIN17_SEL << 4)  | \
                       (MPP_PIN18_SEL << 8)  | \
                       (MPP_PIN19_SEL << 12) | \
                       (MPP_PIN20_SEL << 16) | \
                       (MPP_PIN21_SEL << 20) | \
                       (MPP_PIN22_SEL << 24) | \
                       (MPP_PIN23_SEL << 28))
#define MPP_CTRL3_VAL ((MPP_PIN24_SEL << 0)  | \
                       (MPP_PIN25_SEL << 4)  | \
                       (MPP_PIN26_SEL << 8)  | \
                       (MPP_PIN27_SEL << 12) | \
                       (MPP_PIN28_SEL << 16) | \
                       (MPP_PIN29_SEL << 20) | \
                       (MPP_PIN30_SEL << 24) | \
                       (MPP_PIN31_SEL << 28))

#define GPP_IO0       (GPP_INPUT)   /* COM0/COM1 ORed */
#define GPP_IO1       (GPP_INPUT)   /* Unused */
#define GPP_IO2       (GPP_INPUT)   /* Abort */
#define GPP_IO3       (GPP_INPUT)   /* RTC & thermostat ORed */
#define GPP_IO4       (GPP_INPUT)   /* Unused */
#define GPP_IO5       (GPP_INPUT)   /* IPMC761 */
#define GPP_IO6       (GPP_INPUT)   /* MV64360 WDNMI# interrupt */
#define GPP_IO7       (GPP_INPUT)   /* BCM5421S PHY interrups ORed */
#define GPP_IO8       (GPP_INPUT)   /* PCI Bus 1.0 PMC s0 agent grant */
#define GPP_IO9       (GPP_INPUT)   /* PCI Bus 1.0 PMC s0 agent req */
#define GPP_IO10      (GPP_INPUT)   /* PCI Bus 1.0 PMC s1 agent grant */
#define GPP_IO11      (GPP_INPUT)   /* PCI Bus 1.0 PMC s1 agent req */
#define GPP_IO12      (GPP_INPUT)   /* PCI Bus 1.0 PMC s0 grant */
#define GPP_IO13      (GPP_INPUT)   /* PCI Bus 1.0 PMC s0 req */
#define GPP_IO14      (GPP_INPUT)   /* PCI Bus 1.0 PMC s1 grant */
#define GPP_IO15      (GPP_INPUT)   /* PCI Bus 1.0 PMC s1 req */
#define GPP_IO16      (GPP_INPUT)   /* PCI Bus 1.0 PMC INTA#, PMC1 INTC# */	
#define GPP_IO17      (GPP_INPUT)   /* PCI Bus 1.0 PMC INTB#, PMC1 INTD# */
#define GPP_IO18      (GPP_INPUT)   /* PCI Bus 1.0 PMC INTC#, PMC1 INTA# */
#define GPP_IO19      (GPP_INPUT)   /* PCI Bus 1.0 PMC INTD#, PMC1 INTA# */
#define GPP_IO20      (GPP_INPUT)   /* PCI Bus 0.0 VME LINT0# */
#define GPP_IO21      (GPP_INPUT)   /* PCI Bus 0.0 VME LINT1# */
#define GPP_IO22      (GPP_INPUT)   /* PCI Bus 0.0 VME LINT2# */
#define GPP_IO23      (GPP_INPUT)   /* PCI Bus 0.0 VME LINT3# */
#define GPP_IO24      (GPP_INPUT)   /* SROM initialization active */
#define GPP_IO25      (GPP_INPUT)   /* WDE# output */
#define GPP_IO26      (GPP_INPUT)   /* WDNMI# output */
#define GPP_IO27      (GPP_INPUT)   /* Reserved for interrupt */
#define GPP_IO28      (GPP_INPUT)   /* Tempe ASIC (VMEbus) grant */
#define GPP_IO29      (GPP_INPUT)   /* Tempe ASIC (VMEbus) request */
#define GPP_IO30      (GPP_INPUT)   /* PCI6520 (PMCSpan bridge) grant */
#define GPP_IO31      (GPP_INPUT)   /* PCI6520 (PMCSpan bridge) request */

#define GPP_LEVEL0  (GPP_ACTIVE_HI)	/* COM0/COM1 ORed */
#define GPP_LEVEL1  (GPP_ACTIVE_LOW)	/* Unused */
#define GPP_LEVEL2  (GPP_ACTIVE_LOW)	/* Abort */
#define GPP_LEVEL3  (GPP_ACTIVE_LOW)	/* RTC & thermostat ORed */
#define GPP_LEVEL4  (GPP_ACTIVE_LOW)	/* Unused */
#define GPP_LEVEL5  (GPP_ACTIVE_HI)	/* IPMC761 */
#define GPP_LEVEL6  (GPP_ACTIVE_LOW)	/* MV64360 WDNMI# interrupt */
#define GPP_LEVEL7  (GPP_ACTIVE_LOW)	/* BCM5421S PHY interrups ORed */
#define GPP_LEVEL8  (GPP_ACTIVE_HI)	/* PCI Bus 1.0 PMC s0 agent grant */
#define GPP_LEVEL9  (GPP_ACTIVE_HI)	/* PCI Bus 1.0 PMC s0 agent req */
#define GPP_LEVEL10 (GPP_ACTIVE_HI)	/* PCI Bus 1.0 PMC s1 agent grant */
#define GPP_LEVEL11 (GPP_ACTIVE_HI)	/* PCI Bus 1.0 PMC s1 agent req */
#define GPP_LEVEL12 (GPP_ACTIVE_HI)	/* PCI Bus 1.0 PMC s0 grant */
#define GPP_LEVEL13 (GPP_ACTIVE_HI)	/* PCI Bus 1.0 PMC s0 req */
#define GPP_LEVEL14 (GPP_ACTIVE_HI)	/* PCI Bus 1.0 PMC s1 grant */
#define GPP_LEVEL15 (GPP_ACTIVE_HI)	/* PCI Bus 1.0 PMC s1 req */
#define GPP_LEVEL16 (GPP_ACTIVE_LOW)	/* PCI Bus 1.0 PMC INTA#, PMC1 INTC# */	
#define GPP_LEVEL17 (GPP_ACTIVE_LOW)	/* PCI Bus 1.0 PMC INTB#, PMC1 INTD# */
#define GPP_LEVEL18 (GPP_ACTIVE_LOW)	/* PCI Bus 1.0 PMC INTC#, PMC1 INTA# */
#define GPP_LEVEL19 (GPP_ACTIVE_LOW)	/* PCI Bus 1.0 PMC INTD#, PMC1 INTA# */
#define GPP_LEVEL20 (GPP_ACTIVE_LOW)	/* PCI Bus 0.0 VME LINT0# */
#define GPP_LEVEL21 (GPP_ACTIVE_LOW)	/* PCI Bus 0.0 VME LINT1# */
#define GPP_LEVEL22 (GPP_ACTIVE_LOW)	/* PCI Bus 0.0 VME LINT2# */
#define GPP_LEVEL23 (GPP_ACTIVE_LOW)	/* PCI Bus 0.0 VME LINT3# */
#define GPP_LEVEL24 (GPP_ACTIVE_HI)	/* SROM initialization active */
#define GPP_LEVEL25 (GPP_ACTIVE_HI)	/* WDE# output */
#define GPP_LEVEL26 (GPP_ACTIVE_HI)	/* WDNMI# output */
#define GPP_LEVEL27 (GPP_ACTIVE_HI)	/* Reserved for interrupt */
#define GPP_LEVEL28 (GPP_ACTIVE_HI)	/* Tempe ASIC (VMEbus) grant */
#define GPP_LEVEL29 (GPP_ACTIVE_HI)	/* Tempe ASIC (VMEbus) request */
#define GPP_LEVEL30 (GPP_ACTIVE_HI)	/* PCI6520 (PMCSpan bridge) grant */
#define GPP_LEVEL31 (GPP_ACTIVE_HI)	/* PCI6520 (PMCSpan bridge) request */

/*
 * Interrupt Main Cause High bits associated with GPP interrupts have
 * the following vectors assigned to them.
 */

#define GPP_INT_LVL_0_7         (ICI_MICH_INT_NUM_24)
#define GPP_INT_LVL_8_15        (ICI_MICH_INT_NUM_25)
#define GPP_INT_LVL_16_23       (ICI_MICH_INT_NUM_26)
#define GPP_INT_LVL_24_31       (ICI_MICH_INT_NUM_27)

#define GPP_INT_VEC_0_7         (INUM_TO_IVEC(GPP_INT_LVL_0_7))
#define GPP_INT_VEC_8_15        (INUM_TO_IVEC(GPP_INT_LVL_8_15))
#define GPP_INT_VEC_16_23       (INUM_TO_IVEC(GPP_INT_LVL_16_23))
#define GPP_INT_VEC_24_31       (INUM_TO_IVEC(GPP_INT_LVL_24_31))

/*
 * Interrupt category discriminators  - tell us if the interrupt
 * number is associated with a "main cause low or high" register
 * or with a GPP interrupt.
 */

#define IS_MAIN_CAUSE_INT(num)	((num >= ICI_MICL_INT_NUM_MIN) && \
				  (num <= ICI_MICH_INT_MAX))

#define IS_GPP_INT(num)       	((num >= GPP_IC_INT_NUM_MIN) && \
				  (num <= ICI_MICH_INT_NUM_MAX))

/*
 * Formulas for converting main cause related interrupt numbers to
 * register,bit or from register,bit to interrupt number.
 */

#define MAIN_CAUSE_INT_NUM(reg,bit)	( (reg * 32) + bit )
#define MAIN_CAUSE_BIT_NUM(intNum)	(intNum % 32) 
#define MAIN_CAUSE_REG_NUM(intNum)	(intNum / 32)

/* Formula for GPP local value bit number and interrupt  number */

#define GPP_LOCAL_INT_NUM(bit)		( bit + GPP_IC_INT_NUM_MIN )
#define GPP_LOCAL_BIT_NUM(intNum)	( intNum - GPP_IC_INT_NUM_MIN )

/* Define the host bridge device number */

#define PCI_HOST_BRIDGE_DEV_NUM		0	

/*
 * Assign some specific interrupt numbers  - first generic PCI
 * INTA thru INTB interrupts for both buses.
 */

#define PCI1_INTA	(GPP_IC_INT_NUM_16)
#define PCI1_INTB	(GPP_IC_INT_NUM_17)
#define PCI1_INTC	(GPP_IC_INT_NUM_18)
#define PCI1_INTD	(GPP_IC_INT_NUM_19)

#define PCI0_INTA	(GPP_IC_INT_NUM_20)
#define PCI0_INTB	(GPP_IC_INT_NUM_21)
#define PCI0_INTC	(GPP_IC_INT_NUM_22)
#define PCI0_INTD	(GPP_IC_INT_NUM_23)

/* Now relate the above PCI interrupts to PMC interrupts. */

#define PCI_PMC0_INTA	(PCI1_INTA)
#define PCI_PMC0_INTB	(PCI1_INTB)
#define PCI_PMC0_INTC	(PCI1_INTC)
#define PCI_PMC0_INTD	(PCI1_INTD)

#define PCI_PMC1_INTA	(PCI1_INTC)
#define PCI_PMC1_INTB	(PCI1_INTD)
#define PCI_PMC1_INTC	(PCI1_INTA)
#define PCI_PMC1_INTD	(PCI1_INTB)

/*
 * Assign the PCI interrupts to the PCI6520 Transparent bridge
 * which provides the interface between the PMCSpan bus and PCI
 * bus 0.
 */

#define PCI_BR_INT0     (PCI0_INTC)
#define PCI_BR_INT1     (PCI0_INTD)
#define PCI_BR_INT2     (PCI0_INTA)
#define PCI_BR_INT3     (PCI0_INTB)

/* Assign the PCI interrupts to the proper VME interrupts */

#define PCI_VME_LINT0	(PCI0_INTA)
#define PCI_VME_LINT1	(PCI0_INTB)
#define PCI_VME_LINT2	(PCI0_INTC)
#define PCI_VME_LINT3	(PCI0_INTD)

/* 
 * PCI interrupt routing PCI_x_Dyy_ROUTE (x = bus, yy = device)
 * 0xff,0xff,0xff,0xff = not wired to PCI interrupts
 */

#define PCI0_D00_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D01_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D02_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D03_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D04_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D05_ROUTE		PCI_VME_LINT0, PCI_VME_LINT1, \
				    PCI_VME_LINT2, PCI_VME_LINT3
#define PCI0_D06_ROUTE          PCI_BR_INT0, PCI_BR_INT1, \
                                    PCI_BR_INT2, PCI_BR_INT3
#define PCI0_D07_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D08_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D09_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D10_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D11_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D12_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D13_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D14_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D15_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D16_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D17_ROUTE		0xff,0xff,0xff,0xff
#define PCI0_D18_ROUTE		0xff,0xff,0xff,0xff
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

#define PCI1_D00_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D01_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D02_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D03_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D04_ROUTE          PCI_PMC0_INTA, PCI_PMC0_INTB, \
                                    PCI_PMC0_INTC, PCI_PMC0_INTD
#define PCI1_D05_ROUTE          PCI_PMC0_INTB, PCI_PMC0_INTC, \
                                    PCI_PMC0_INTD, PCI_PMC0_INTA
#define PCI1_D06_ROUTE          PCI_PMC1_INTA, PCI_PMC1_INTB, \
                                    PCI_PMC1_INTC, PCI_PMC1_INTD
#define PCI1_D07_ROUTE          PCI_PMC1_INTB, PCI_PMC1_INTC, \
                                    PCI_PMC1_INTD, PCI_PMC1_INTA
#define PCI1_D08_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D09_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D10_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D11_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D12_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D13_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D14_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D15_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D16_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D17_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D18_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D19_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D20_ROUTE          0xff,0xff,0xff,0xff
#define PCI1_D21_ROUTE          0xff,0xff,0xff,0xff
#define PCI1_D22_ROUTE          0xff,0xff,0xff,0xff
#define PCI1_D23_ROUTE          0xff,0xff,0xff,0xff
#define PCI1_D24_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D25_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D26_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D27_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D28_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D29_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D30_ROUTE		0xff,0xff,0xff,0xff
#define PCI1_D31_ROUTE		0xff,0xff,0xff,0xff

/* Define some specific "main cause" bits */

#define ICI_CPU0_GPP0_7_BIT	24
#define ICI_CPU0_GPP8_15_BIT	25
#define ICI_CPU0_GPP16_23_BIT	26
#define ICI_CPU0_GPP24_31_BIT	27

/*
 * Tempe (Tsi148) PCI-VME bridge interrupt support.
 * First define the GT-64360 GPP interrupt associated with Tempe:
 */

#define TEMPE_INT_LVL0   GPP_IC_INT_NUM_20       /* LINT0 */
#define TEMPE_INT_LVL1   GPP_IC_INT_NUM_21       /* LINT1 */
#define TEMPE_INT_LVL2   GPP_IC_INT_NUM_22       /* LINT2 */
#define TEMPE_INT_LVL3   GPP_IC_INT_NUM_23       /* LINT3 */

/* We will route all specific Tempe interrupts through LINT0 */

#define TEMPE_INT_LVL	TEMPE_INT_LVL0		/* Tempe interrupt level */

/* 
 * Specific Mv64360 device support register programming values
 */

#define MV64360_DEVCS1_BASE_ADDR  0xf1d00000 /* Device bank 1 base */
#define MV64360_DEVCS1_SIZE 	  0x00100000 /* Device bank size */
#define MV64360_DEVCS0_BASE_ADDR  0xf4000000 /* Device (non-boot) bank 0 base */
#define MV64360_BOOTCS_BASE_ADDR  0xf8000000 /* Boot bank base */

/* Specific Mv64360 Bank parameter register values */

/*
 * Default bank parameters register value.
 *
 * Name                Value    Bit positions
 *
 * TURNOFF		0x7	2:0
 * ACC2FIRST		0xf	6:3
 * ACC2NEXT		0xf	10:7
 * ALE2WR		0x7	13:11
 * WRLOW		0x7	16:14
 * WRHIGH		0x7	19:17
 * DEVWIDTH		0x2	21:20
 * TURNOFFEXT		0x1	22
 * ACC2FIRSTEXT		0x1	23
 * ACC2NEXTEXT		0x1	24
 * ALE2WREXT		0x1	25
 * WRLOWEXT		0x1	26
 * WRHIGHEXT		0x1	27
 * BADRSKEW		0x0	29:28
 * DPEN			0x0	30
 * RESERVED		0x1	31
 */

#define DVCTL_DEVICE_BANK_DEFAULT_PARAMS_VAL		0x8fefffff 

#define DVCTL_DEVICE_BOOT_BANK_PARAMS_VAL \
		DVCTL_DEVICE_BANK_DEFAULT_PARAMS_VAL

#define	DVCTL_DEVICE_BANK0_PARAMS_VAL \
		DVCTL_DEVICE_BANK_DEFAULT_PARAMS_VAL

/*
 * Bank	1 parameters register value
 * DVCTL_DEVICE_BANK1_PARAMS  offset: 0x460 
 *
 * Name                Value    Bit positions
 *
 * TURNOFF		0x6	2:0
 * ACC2FIRST		0xf	6:3
 * ACC2NEXT		0x3	10:7
 * ALE2WR		0x3	13:11
 * WRLOW		0x7	16:14
 * WRHIGH		0x3	19:17
 * DEVWIDTH		0x0	21:20
 * TURNOFFEXT		0x1	22
 * ACC2FIRSTEXT		0x0	23
 * ACC2NEXTEXT		0x0	24
 * ALE2WREXT		0x0	25
 * WRLOWEXT		0x0	26
 * WRHIGHEXT		0x0	27
 * BADRSKEW		0x0	29:28
 * DPEN			0x0	30
 * RESERVED		0x1	31
 */

#define DVCTL_DEVICE_BANK1_PARAMS_VAL			0x8047d9fe

/* DVCTL_DEVICE_BANK2_PARAMS  offset: 0x464 */

#define DVCTL_DEVICE_BANK2_PARAMS_VAL \
		DVCTL_DEVICE_BANK_DEFAULT_PARAMS_VAL

/* DVCTL_DEVICE_BANK3_PARAMS  offset: 0x468 */

#define DVCTL_DEVICE_BANK3_PARAMS_VAL \
		DVCTL_DEVICE_BANK_DEFAULT_PARAMS_VAL

/*
 * Mv64360 interface control register value
 * DVCTL_DEVICE_INTERFACE_CTRL offset: 0x4C0
 * 
 * Name                Value    Bit positions
 *
 * TIMEOUT	   	0xffff  15:0
 * RDTRIG     		0x0     16
 * RESERVED (0)  	0x0     17
 * RESERVED (1)  	0x1     18
 * PERRPROP   		0x0     19
 * PARSEL     		0x0     20
 * FORCEPAREN 		0x0     21
 * RESERVED (0)  	0x0     23:22
 * FORCEPAR   		0x0     27:24
 * RESERVED (0)    	0x0	29:28
 * RESERVED (0)    	0x0	31:30
 */

#define DVCTL_DEVICE_INTERFACE_CTRL_VAL			0x0004ffff

/* 
 * Mv64360 cross bar arbiter register (low) values
 * DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_LOW offset: 0x4C8
 *
 * Name       Value    Bit positions  Description
 *
 * ARB0       0x2      3:0             CPU acc.
 * ARB1       0x3      7:4             PCI0 acc.
 * ARB2       0x4     11:8             PCI1 acc.
 * ARB3       0x5     15:12            MPSC acc.
 * ARB4       0x6     19:16            IDMA acc.
 * ARB5       0x7     23:20            Enet acc.
 * ARB6       0x1     27:24            NULL req.
 * ARB7       0x1     31:28            NULL req.
 */

#define DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_LOW_VAL 	0x11765432

/*
 * Mv64360 cross bar arbiter register (high) values
 * DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_HI offset: 0x4CC 
 * 
 * Name       Value    Bit positions  Description
 *
 * ARB8       0x2      3:0             CPU acc.
 * ARB9       0x3      7:4             PCI0 acc.
 * ARB10      0x4     11:8             PCI1 acc.
 * ARB11      0x5     15:12            MPSC acc.
 * ARB12      0x6     19:16            IDMA acc.
 * ARB13      0x7     23:20            Enet acc.
 * ARB14      0x1     27:24            NULL req.
 * ARB15      0x1     31:28            NULL req.
 */

#define DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_HI_VAL	 0x11765432

/*
 * Mv64360 cross bar timeout register value 
 * DVCTL_DEVICE_INTERFACE_CROSSBAR_TIMEOUT offset: 0x4C4
 * 
 *
 * Name          Value    Bit positions
 *
 * PRESET         0xff      7:0
 * RESERVED (0)   0x0       15:8
 * TIMEOUTEN      0x1       16
 * RESERVED (0)   0x0       31:17
 */

#define  DVCTL_DEVICE_INTERFACE_CROSSBAR_TIMEOUT_VAL	0x000100ff

/* 
 * Serial Port Support 
 *
 * The MVME6100 contains one EXAR ST16C554D Quad UART device connected
 * to the MV64360 device controller bus to support two async serial
 * ports (COM0, COM1).  Interrupts for COM0/COM1 share GPP pin 0.
 * In order to enable the COM port interrupts on the MVME6100 we must enable 
 * the GPP interrupt in the GPP interrupt mask register.  We must also 
 * enable this GPP interrupt in the Main Interrupt High Cause register, 
 * by calling intEnable with GPP_INT_LVL_0_7 as the argument in sysHwInit2().
 *
 * Installing an IPMC transition module (and transition board) adds
 * an additional 4 serial ports.  Two 16550-compatible async serial ports and
 * two async/sync serial ports. 
 */

#ifdef INCLUDE_PCX7307_SIO
#   define N_UART_CHANNELS	4		/* No. serial I/O channels */
#else 
#   define N_UART_CHANNELS	2		/* No. serial I/O channels */
#endif /* INCLUDE_PCX7307_SIO */

/* total number of serial ports */

#ifdef INCLUDE_Z85230_SIO
#   define N_SIO_CHANNELS	(N_UART_CHANNELS  + 2)
#else
#   define N_SIO_CHANNELS	N_UART_CHANNELS
#endif /* INCLUDE_Z85230_SIO */

#undef  NUM_TTY
#define NUM_TTY         N_SIO_CHANNELS

#define UART_REG_ADDR_INTERVAL  1   /* addr diff of adjacent regs */
#define COM0_BASE_ADR		(MV64360_DEVCS1_BASE_ADDR + 0x20000)	
#define COM1_BASE_ADR		(MV64360_DEVCS1_BASE_ADDR + 0x21000)
#ifdef INCLUDE_PCX7307_SIO
#   define PCX7307_UART1        (ISA_MSTR_IO_LOCAL + SIO_UART1)
#   define PCX7307_UART2        (ISA_MSTR_IO_LOCAL + SIO_UART2)
#   define COM2_BASE_ADR	PCX7307_UART1
#   define COM3_BASE_ADR	PCX7307_UART2
#endif /* INCLUDE_PCX7307_SIO */

#define COM0_INT_LVL		(GPP_IC_INT_NUM_0)
#define COM1_INT_LVL		(GPP_IC_INT_NUM_0)	
#define COM0_INT_VEC		(0x00 + COM0_INT_LVL)
#define COM1_INT_VEC		(0x00 + COM1_INT_LVL)

/* Z85230 asyncronous serial ports (COM4 & COM5) */

#ifdef INCLUDE_Z85230_SIO
#   define Z85230_PORTB_CTRL       (ISA_MSTR_IO_LOCAL + 0x0840)    /* serial */
#   define Z85230_PORTB_DATA       (ISA_MSTR_IO_LOCAL + 0x0841)
#   define Z85230_PORTA_CTRL       (ISA_MSTR_IO_LOCAL + 0x0842)    /* serial */
#   define Z85230_PORTA_DATA       (ISA_MSTR_IO_LOCAL + 0x0843)
#   define BAUD_CLK_FREQ            10000000    /* 10 MHz "P Clock" (fixed) */
#   define REG_8530_WRITE(reg,val) sysOutByte((UINT32)(reg), (UINT8)(val))
#   define REG_8530_READ(reg,pVal) *(UINT8 *)pVal = sysInByte((UINT32)reg)
#   define DATA_REG_8530_DIRECT
#   define Z8530_RESET_DELAY \
        { \
        int i; \
        for (i = 0; i < Z8530_RESET_DELAY_COUNT; i++) \
                ; /* do nothing */ \
        }
#endif /* INCLUDE_Z85230_SIO */

/*
 * Interrupt vector and interrupt level assigned to timer/counter 0.
 */

#define TMR_CNTR0_INT_VEC      (ICI_MICL_INT_NUM_8)
#define TMR_CNTR0_INT_LVL      (ICI_MICL_INT_NUM_8)

/*
 * Interrupt vector and interrupt level assigned to RTC.
 */

#define RTC_INT_VEC		(INUM_TO_IVEC(GPP_IC_INT_NUM_3))
#define RTC_INT_LVL		(GPP_IC_INT_NUM_3)

/*
 * Interrupt vector and interrupt level assigned to the DS1621
 * temperature sensor.  This interrupt is shared with the RTC.
 */

#define DS1621_INT_VEC		(INUM_TO_IVEC(GPP_IC_INT_NUM_3))
#define DS1621_INT_LVL		(GPP_IC_INT_NUM_3)

/* MV64360 DMA Driver Macro Definitions */

#define DMA_STATUS IDMA_STATUS   /* Structure used to return DMA status */
#define DMA_INIT() sysMv64360DmaInit ()
#define DMA_START(chan, desc, attrib, stat)                   \
        sysMv64360DmaStart ((UINT32)(chan),                   \
                            (IDMA_DESCRIPTOR *)(desc),        \
                            (IDMA_ATTRIBUTES *)(attrib),      \
                            (DMA_STATUS *)(stat))
#define DMA_STATUS_GET(chan, stat)                            \
        sysMv64360DmaStatus ((UINT32)(chan), (DMA_STATUS *)(stat))
#define DMA_ABORT(chan) sysMv64360DmaAbort ((UINT32)(chan))
#define DMA_PAUSE(chan) sysMv64360DmaPause ((UINT32)(chan))
#define DMA_RESUME(chan) sysMv64360DmaResume ((UINT32)(chan))
#define DMA_MAX_CHANS        IDMA_CHANNEL_COUNT /* Max available channels */

/* Ethernet unit */

#define MAX_ENET_PORTS       2
#define MAX_ENET_PORT_NUM    1

#define PHY_ADDR_ENET_PORT0  1
#define PHY_ADDR_ENET_PORT1  2
#define PHY_ADDR_ENET_PORT2  0    /* Not Used */

/*
 * The ethernet receive and transmit descriptors are place in the 
 * MV64360 Integrated SRAM, of which there is 256 KBytes.  The 
 * buffers are located in cacheable memory (as malloc'ed by the
 * ethernet driver). 
 */

#define ETHER_PORT_MEM_SIZE     0x20000  /* 128 KBytes per port */   
#define ETHER_MEM_START_ADDR    (MV64360_INTEGRATED_SRAM_BASE)

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* __INCmv6100Ah */
