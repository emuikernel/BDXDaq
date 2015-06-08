/* mv5500A.h - Motorola MVME5500 board header file */

/* Copyright 1984-2000 Wind River Systems, Inc. */
/* Copyright 1999-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01t,23jun03,scb Commentary for CPUIF_CPU_CFG_VAL
01s,17jun03,yyz Remove definition of DMA_SRC_BURST_LIMIT and 
                DMA_DEST_BURST_LIMIT.
01r,02jun03,scb Enable block transfers for universe LSI1 window also 128byte
		DMA burst limit.
01q,30apr03,yyz Added #define for DEFAULT_CPU_ID.
01p,08apr03,scb HiNT transparent PCI-PCI bridge support.
01o,01apr03,scb Bus probing support.
01n,10mar03,cak IPMC SCSI and serial support - required moving PCI 16-bit
		I/O from PCI bus 1.0 to PCI bus 0.0.
01m,18feb03,scb Fix botched commentary on memory map.
01l,10feb03,scb Full shared memory support.
01k,29jan03,cak Removed DEC_CLOCK_FREQ and DFLT_BUS_CLK definitions, added
                a mask definition for the REF_CLK bit of System Status Reg 1,
		and added BAUDOUT definitions.
01j,20nov02,scb Preliminary shared memory support.
01i,19nov02,scb Fix for gei buffers in cacheable copyback.
01h,15nov02,scb Reserve non-cached low memory for gei ethernet buffers.
01g,12nov02,scb VME/Universe support.
01f,30oct02,yyz DMA support.
01e,25oct02,scb Set GPP_IO5 to INPUT.
01d,25oct02,scb Set PCI_MEM_LOCAL_START to 0x8a000000 from 0x80000000.
01c,16oct02,yyz System Board Registers Support.
01b,14oct02,yyz NVRAM/RTC/Failsafe Support.
01a,01oct02,scb Adapted from hxeb100 base, (ver 01e of "hxeb100A.h").
*/

/*
This file contains I/O addresses and related constants for the
Motorola MVME5500 board.  This file must not contain any "dependencies"
(ie. #ifdef's or #ifndef's).
*/

#ifndef	__INCmv5500Ah
#define	__INCmv5500Ah

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* includes */

#include "ppcProc.h"

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
 * is located at MV64260_DEVCS1_BASE_ADDR + 0x10000, (0x10000 offset
 * into the "device bank"), and extends for 32KB up to
 * MV64260_DEVCS1_BASE_ADDR + 0x17fff.
 *
 * The first 256 bytes are skipped, the second 256 bytes are  used to
 * store the vxWorks boot line parameters.  From the "base address"
 * plus the offset of 0x7ff0 thru an offset of 0x7fff (16 bytes),
 * are the M48T37Y registers.
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
#define NV_RAM_BASE             (MV64260_DEVCS1_BASE_ADDR + 0x00010000)
#define NV_RAM_READ(x)		sysNvRead (x)
#define NV_RAM_WRITE(x,y)	sysNvWrite (x,y)

/* Real-time clock and Alarm clock Support */

#ifdef INCLUDE_RTC
#   define RTC_SET(time)              m48t37RtcSet ((time))
#   define RTC_SHOW()                 m48t37RtcShow ()
#   define RTC_GET(time)              m48t37RtcGet ((time))
#   define RTC_DATE_TIME_HOOK(hook)   m48t37DateTimeHook ((hook))
#   define ALARM_SET(method, time)    m48t37AlarmSet ((method), (time))
#   define ALARM_CANCEL()             m48t37AlarmCancel ()
#   define ALARM_GET(method, time)    m48t37AlarmGet ((method), (time))
#   define ALARM_SHOW()               m48t37AlarmShow ()
#endif /* INCLUDE_RTC */

/* Failsafe Timer Support */

#ifdef INCLUDE_FAILSAFE
#   define FAILSAFE_SET(seconds, reset) m48t37FailsafeSet ((seconds), (reset))
#   define FAILSAFE_GET(seconds, reset) m48t37FailsafeGet ((seconds), (reset))
#   define FAILSAFE_CANCEL()            m48t37FailsafeCancel ()
#   define FAILSAFE_SHOW()              m48t37FailsafeShow ()
#   define FAILSAFE_CAUSED_RESET()      m48t37FailsafeCausedReset ()
#endif /* INCLUDE_FAILSAFE */

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
#define CPU_BUS_SPD		(sysCpuBusSpd())
#define DEC_CLOCK_FREQ          CPU_BUS_SPD	/* Frequency of decrementer */

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

#define DELTA(a,b)		(abs((int)a - (int)b))

/* Translation macro */

#define TRANSLATE(x,y,z)\
        ((UINT)(x) - (UINT)(y) + (UINT)(z))

/* System Memory (DRAM) Support */

#define MAX_DRAM	0x80000000

#define SIZE_0MB	0x00000000
#define SIZE_256KB	0x00040000
#define SIZE_1MB	0x00100000
#define SIZE_2MB	0x00200000
#define SIZE_4MB	0x00400000
#define SIZE_8MB	0x00800000
#define SIZE_16MB	0x01000000
#define SIZE_32MB	0x02000000
#define SIZE_64MB	0x04000000
#define SIZE_128MB	0x08000000
#define SIZE_256MB	0x10000000
#define SIZE_512MB	0x20000000
#define SIZE_1024MB	0x40000000
#define SIZE_2048MB	0x80000000

/* Boot FLASH Support */

#define ROM_BASE_ADRS	0xfff00000      /* base address of ROM */

/* System Control & Status Register Support */

#define SYSTEM_REG_BASE	 MV64260_DEVCS1_BASE_ADDR   /* base of system regs */

/* System Control & Status Register 1
 *
 * Bit 0 = 0  Reserved.
 *     1 = 0  Reserved.
 *     2 = 1  Fuse Status. Read only.
 * 	      0 - one of the on board fuses is open.
 *	      1 - all on board fuses are functional.
 *     3 = 1  FLASH BUSY. Read only.
 *            This bit provides the current state of the FLASH BANK A
 *            StrataFlash device Status pins.
 *            Refer to the appropriate Intel StrataFlash data sheet for
 *            a description on the function of the Status pin.
 *     4 = X  ABORT. Read/write.
 *            0 - the abort switch is asserted.
 *            1 - the abort switch is not asserted.
 *     5 = X  ENV Safe Start. Read only.
 *            0 - firmware should use the ENV settings programmed in NVRAM.
 *            1 - firmware should use the safe ENV settings.
 *     6 = X  Boot FLASH Bank Select. Read only.
 *	      0 - FLASH Bank A is the boot bank.
 *	      1 - FLASH Bank B is the boot bank.
 *     7 = X  Reference Clock. Read only.
 *            This bit reflects the current state of the 28.8 KHz reference
 *            clock derived from the 1.8432MHz UART oscillator divided by
 *            64.  This clock may be used as a fixed timing reference.
 */

#define SYSTEM_STATUS_REG_1	(SYSTEM_REG_BASE + 0x0)
#define SYSTEM_STATUS_REG_1_BANK_SELECT_MASK	0x40	
#define SYSTEM_STATUS_REG_1_REF_CLK_MASK        0x80

/* System Control & Status Register 2
 *
 * Bit 0 = X  PCI Bus 0.0 M66EN. Read only.
 *            0 - PCI Bus 0.0 is operating at 33 MHz.
 *            1 - PCI Bus 0.0 is operating at 66 MHz.
 *     1 = X  PCI Bus 1.0 M66EN. Read only.
 *            0 - PCI Bus 1.0 is operating at 33 MHz.
 *            1 - PCI Bus 1.0 is operating at 66 MHz.
 *     2 = 0  PCI Bus 0.1 M66EN. Read only.
 *            0 - PCI Bus 0.1 is operating at 33 MHz.
 *            1 - PCI Bus 0.1 is operating at 66 MHz.
 *     3 = 0  Reserved.
 *     4 = 1  Thermostat Mask.  Read/write.
 *            0 - thermostat output will be enabled to generate
 *                an interrupt on GPP 3.
 *            1 - thermostat output is disabled from generating
 *                an interrupt.
 *     5 = 1  FLASH Write Protect.  Read/write.
 *            0 - enable writes to the FLASH memory devices.
 *            1 - write protect the FLASH memory devices.
 *            This bit is set during reset and must be reset by the system
 *            software to enable writing of the FLASH devices.
 *     6 = 1  EEPROM Write Protect.  Read/write.
 *            0 - enable writes to the EEPROM devices.
 *            1 - write protect the EEPROM devices.
 *            The devices are write protected following a reset.
 *     7 = 1  Board Fail.  Read/write.
 *            0 - extinguish the front panel LED.
 *            1 - illuminate the front panel LED.
 */

#define SYSTEM_STATUS_REG_2	(SYSTEM_REG_BASE + 0x1)
#define SYSTEM_STATUS_REG_2_BRDFL_MASK		0x80

/* System Control & Status Register 3
 *
 * Bit 0 = 0  PCI Bus 0 Reset.  Read/write.
 *            0 - deassert the PCI Bus 0 reset.
 *            1 - assert the PCI Bus 0 reset.
 *     1 = 0  PCI Bus 1 Reset.  Read/write.
 *            0 - deassert the PCI Bus 1 reset.
 *            1 - assert the PCI Bus 1 reset.
 *     2 = 0  Reserved.
 *     3 = 1  Abort Interrupt Mask.  Read/write.
 *            0 - the abort interrupt may be generated.
 *            1 - the abort interrupt will be masked.
 *   6:4 = 0  Reserved.
 *     7 = 0  Board Reset.  Write only.
 *            1 - force a hard reset of the MV5500 board.
 *            Clears automatically when the board reset is completed.
 */

#define SYSTEM_STATUS_REG_3	(SYSTEM_REG_BASE + 0x2)

/* Presence Detect Register
 *
 * Bit 0 = X  PMC Module 1 Present.
 *            0 - the PMC Module is installed in position 1.
 *            1 - no PMC Module installed in position 1.
 *     1 = X  PMC Module 2 Present.
 *            0 - the PMC Module is installed in position 2.
 *            1 - no PMC Module installed in position 2.
 *     2 = X  PMC Expansion Module Present.
 *            0 - the PMC Expansion Module is installed.
 *            1 - no PMC Expansion Module installed.
 *   7:3 = 1  Reserved.
 */

#define PRESENCE_DETECT_REG	(SYSTEM_REG_BASE + 0x4)
/* Sergey: patch recommended by MCG */
/* THAT PART DOES NOT WORK FOR mv5500 !!! */
#define PRESENCE_DETECT_EREADY1_MASK    0x10
#define PRESENCE_DETECT_EREADY0_MASK    0x08





/* Configuration Header/Switch Register
 *
 * Bit 7:0 = X  Read only.
 *              0 - the switch is ON for the header position associated
 *                  with the corresponding bit.
 *              1 - the switch is OFF.
 */

#define CFG_HDR_SWITCH_REG	(SYSTEM_REG_BASE + 0x5)

/* Time Base Enable Register
 *
 * Bit 0 = 1  Processor Time Base Enable.  Read/write.
 *            0 - TBEN pin of the processor is driven low.
 *            1 - TBEN pin of the processor is driven high.
 *   7:1 = 0  Reserved.
 */

#define TIME_BASE_ENB_REG	(SYSTEM_REG_BASE + 0x6)

/* Graphical Address Register
 *
 * This register reflects the inverted states of the graphical address
 * pins at the 5-row, 160-pin P1 connector.  Applications not using
 * the 5-row backplane can use a planar switch to assign a graphical
 * address.
 * 0 - indicates that the corresponding switch is ON.
 * 1 - indicates that the corresponding switch is OFF.
 *
 * Bit 0 = X  VMEGAP_.  Read only.
 *   2:1 = X  Reserved.
 *     3 = X  VMEGA4_.  Read only.
 *     4 = X  VMEGA3_.  Read only.
 *     5 = X  VMEGA2_.  Read only.
 *     6 = X  VMEGA1_.  Read only.
 *     7 = X  VMEGA0_.  Read only.
 */

#define GRAPHICAL_ADDR_REG	(SYSTEM_REG_BASE + 0x7)

/* MV64260 - Discovery I Support */

#define MV64260_MAX_BUS		2		/* Max # PCI buses on MV64260 */
#define MV64260_REG_BASE	0xF1000000	/* Base of MV64260 Reg Space */
#define MV64260_REG_SPACE_SIZE	0x10000         /* 64Kb Internal Reg Space */

/* PCI and PCI Auto-Configuration Support */

#define INCLUDE_PCI		/* always include pci */
#define INCLUDE_PCI_AUTOCONF	/* always include pci auto config*/

#define PCI_CLINE_SZ	(_CACHE_ALIGN_SIZE/4)	/* Cache Ln Size = 32 bytes */
#define PCI_LAT_TIMER	0xff	/* Latency Timer Value = 255 PCI clocks */
#define PCI_CFG_BRIDGE_CONTROL_MABORT 0x0020	/* Master abort mode */
#define PCI_CONFIG_ADDR_BASE_0	(MV64260_REG_BASE + PCIIF_CFG_ADDR_PCI0)
#define PCI_CONFIG_DATA_BASE_0	(MV64260_REG_BASE + PCIIF_CFG_DATA_PCI0)
#define PCI_CONFIG_ADDR_BASE_1	(MV64260_REG_BASE + PCIIF_CFG_ADDR_PCI1)
#define PCI_CONFIG_DATA_BASE_1	(MV64260_REG_BASE + PCIIF_CFG_DATA_PCI1)

/* PCI/ function identifiers for configuration access prior to pciAutoConfig */

#define PCI0_BUS	0
#define PCI1_BUS	1

/*
 * PCI Processor (LOCAL) and Bus (BUS) Definitions
 *
 * The following definitions determine the address ranges for PCI Memory
 * and I/O space on both the processor bus and the PCI memory and
 * I/O busses. These definitions are used to configure the PCI-Host
 * Bridge as well as perform PCI Auto-Configuration.  There are two sets
 * of definitions, one for each of the MV64260's busses (0.0 and 1.0).
 *
 * PCI Memory Space:
 * This is a one-to-one mapping.  The address used on the processor bus is the
 * address used on the PCI memory bus.
 *
 * PCIx_MSTR_MEMIO_LOCAL =  PCI Non-Prefetchable Memory Space
 *                          (processor bus view), 'x' = 0 or 1 to indicate
 *                          which of the MV64260 busses are being mapped
 *                          (0.0 or 1.0).
 * PCIx_MSTR_MEMIO_BUS =    PCI Non-Prefetchable Memory Space
 *                          (PCI memory bus view), 'x' = 0 or 1 to indicate
 *                          which of the MV64260 busses are being mapped
 *                          (0.0 or 1.0).
 *
 * PCIx_MSTR_MEM_LOCAL  =   PCI Prefetchable Memory Space
 *                          (processor bus view), 'x' = 0 or 1 to indicate
 *                          which of the MV64260 busses are being mapped
 *                          (0.0 or 1.0).
 * PCIx_MSTR_MEM_BUS  =     PCI Prefetchable Memory Space
 *                          (PCI memory bus view), 'x' = 0 or 1 to indicate
 *                          which of the MV64260 busses are being mapped
 *                          (0.0 or 1.0).
 *
 * PCI I/O Space:
 * This is not a one-to-one mapping.  For Bus 0.0 the I/O space will start
 * at 0x00000000 on the PCI I/O bus.  For Bus 1.0 the I/O space (32-bit only)
 * will not start at 0x00000000, but at the top of Bus 1.0's I/O space.  This
 * is to prevent overlap between bus addresses.
 *
 * NOTE: Bus 1.0 does not contain 16-bit PCI I/O space.
 *
 * ISA_MSTR_IO_LOCAL =   PCI 16-bit I/O Space (processor bus view) - this
 *                       space only exists on bus 0.0.
 *
 * ISA_MSTR_IO_BUS =     PCI 16-bit I/O Space (PCI I/O bus view) - this
 *                       space only exists on bus 0.0.
 *
 * PCIx_MSTR_IO_LOCAL =  PCI 32-bit I/O Space (processor bus view)
 *                       'x' = 0 or 1 to indicate which of the MV64260
 *                       busses are being mapped (0.0 or 1.0).
 *
 * PCIx_MSTR_IO_BUS =    PCI 32-bit I/O Space (PCI I/O bus view)
 *                       'x' = 0 or 1 to indicate which of the MV64260
 *                       busses are being mapped (0.0 or 1.0).
 *
 * Note that PCI window base addresses and sizes must conform to the
 * following two constraints:
 *
 * 1.  The window size must be a power of 2.
 *
 * 2.  The window size must be greater than or equal to 1MB in order
 *     to satisfy host bridge address decoders.
 *
 * 3.  The window base address must be an integral multiple of the
 *     size.
 *
 * The constraints are enforced at compile time with error generation
 * if a violation occurs.
 */

/*
 * Bus 1.0 (PCI1) PCI Memory Definitions -
 * First handle 1.0 (PCI1) memio (nonprefetchable) window.  This
 * window must start on a 'memio size' boundary beyond the end
 * of the PCI0 mem (prefetchable) window.
 */

#define	PCI1_MEM_LOCAL_START	PCI_MEM_LOCAL_START
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

/*
 * PCI I/O Definitions -
 * The PCI I/O space consists of two separate windows.  The first
 * window appears on PCI bus 0.0 (PCI0) and consists of
 * ISA I/O (16-bit I/O) space followed by the rest of PCI bus 0.0 I/O space.
 * Immediately following this window comes the I/O space window
 * for PCI bus 1.0 (PCI1).  Note that the ISA I/O space is totally confined
 * to PCI bus 0.0.  As with PCI memory windows, the two size and base
 * address alignment rules (see preceding comment) are enforced.
 *
 * Let's start with the PCI 0 (PCI0) bus I/O window definitions:
 */

#define PCI_IO_LOCAL_START	(PCI1_MEM_LOCAL_END)

#define	PCI0_IO_LOCAL_START	(PCI_IO_LOCAL_START)
#define PCI0_IO_SIZE		(ISA_MSTR_IO_SIZE + PCI0_MSTR_IO_SIZE)

#define	ISA_MSTR_IO_LOCAL	(PCI0_IO_LOCAL_START)
#define	ISA_MSTR_IO_BUS		(0x00000000)

#define	PCI0_MSTR_IO_LOCAL	(ISA_MSTR_IO_LOCAL + ISA_MSTR_IO_SIZE)
#define	PCI0_MSTR_IO_BUS	(TRANSLATE(PCI0_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_BUS))

#define PCI0_IO_LOCAL_END       (PCI0_MSTR_IO_LOCAL + PCI0_MSTR_IO_SIZE)


/* Now for the PCI bus 1.0 (PCI1) I/O window definitions */

#define PCI1_IO_LOCAL_START	(PCI0_IO_LOCAL_END)
#define PCI1_MSTR_IO_LOCAL	(PCI1_IO_LOCAL_START)
#define PCI1_MSTR_IO_BUS	(TRANSLATE(PCI1_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_BUS))

#define PCI1_IO_LOCAL_END	(PCI1_MSTR_IO_LOCAL + PCI1_MSTR_IO_SIZE)

#define PCI_IO_LOCAL_END	(PCI1_IO_LOCAL_END)

/*
 * Bus 0.0 (PCI0) PCI Memory Definitions -
 * First handle 0.0 (PCI0) mem (prefetchable) window which appears
 * the next 20-bit boundary beyond the end of the PCI0 I/O window.
 * Note: 20-bit boundary is necessary to satisfy host bridge address
 * decoders.
 */

#define PCI0_MEM_LOCAL_START	((PCI_IO_LOCAL_END + (1<<20)-1) & ~((1<<20)-1))
#define PCI0_MSTR_MEM_LOCAL	PCI0_MEM_LOCAL_START
#define	PCI0_MSTR_MEM_BUS	PCI0_MSTR_MEM_LOCAL	/* null	mapping	*/
#define PCI0_MEM_LOCAL_END	(PCI0_MSTR_MEM_LOCAL + PCI0_MSTR_MEM_SIZE)

/*
 * Now handle Bus 0.0 (PCI0) memio (nonprefetchable) window which
 * appears at the next 2^24-byte boundary after the end of the PCI0
 * prefetchable window.
 */

#define	PCI0_MSTR_MEMIO_LOCAL	PCI0_MEM_LOCAL_END
#define	PCI0_MSTR_MEMIO_BUS	PCI0_MSTR_MEMIO_LOCAL	/* null	mapping	*/
#define	PCI0_MEMIO_LOCAL_END 	(PCI0_MSTR_MEMIO_LOCAL + PCI0_MSTR_MEMIO_SIZE)

/*
 * VME Memory mapping defines - we will define VME space at the top
 * of the useable memory map, just below the MV64260 register base.
 */

#define VME_MEM_LOCAL_END	MV64260_REG_BASE

/* VME A16 outbound (master) window */

#define VME_A16_MSTR_LOCAL      ((VME_MEM_LOCAL_END - VME_A16_MSTR_SIZE) &\
				 0xfff00000)
#define VME_A16_MSTR_BUS	0xffff0000



/* VME A16 inbound (slave) window does not exist. (Could use window 3.) */

#define VME_A16_SLV_BUS         0x0
#define VME_A16_SLV_SIZE        0x0     /* 0, window is disabled */
#define VME_A16_SLV_LOCAL       0x0

/* VME A24 outbound (master) */

#define VME_A24_MSTR_LOCAL      ((VME_A16_MSTR_LOCAL - VME_A24_MSTR_SIZE) &\
				 0xfff00000)
#define VME_A24_MSTR_BUS	0xff000000



/* VME A24 inbound (slave) window, default is to not enable */

#define VME_A24_SLV_BUS		VME_A24_MSTR_BUS
#define VME_A24_SLV_LOCAL       LOCAL_MEM_LOCAL_ADRS
#define VME_A24_SLV_SIZE        0x0     	/* 0, Window is disabled */

/* VME A32 outbound (master)  - note ...MSTR_SIZE defined in "config.h" */

#define VME_A32_MSTR_LOCAL      ((VME_A24_MSTR_LOCAL - VME_A32_MSTR_SIZE) &\
				  0xfff00000)

#define VME_A32_MSTR_BUS	0x08000000

/* VME A32 Inbound (slave) mapping for node 0 only - maps all of DRAM */

#define VME_A32_SLV_BUS		VME_A32_MSTR_BUS
#define VME_A32_SLV_LOCAL	LOCAL_MEM_LOCAL_ADRS
#define VME_A32_SLV_SIZE  (((ULONG)sysPhysMemTop()) - (LOCAL_MEM_LOCAL_ADRS))

#define VME_MEM_LOCAL_START	VME_A32_MSTR_LOCAL
#define VME_MEM_LOCAL_SIZE	(VME_MEM_LOCAL_END - VME_MEM_LOCAL_START)

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
 * The following represents a null mapping from CPU space to PCI space
 * in the address ranges that allow access to the VME bus.
 */

#define VME_CPU2PCI_OFFSET	0

/* VME Interrupt Bit definition - used for shared memory mailbox interrupt */

#define SIG1_INTR_SET           0x80

/*
 * LSI1:
 * Outbound access to VME bus (local PCI to VME bus) via LSI1 register set.
 * The access through this window is to low order DRAM of the board
 * representing node zero on the VME bus.  Through this window, nonzero
 * nodes can access the VxWorks shared memory pool.
 *
 * The following outbound mapping is achieved:
 *
 *   CPU range               PCI range		  VME range (A32)
 *   ---------               ---------            ---------------
 *     VME_A32_MSTR_LOCAL      Same as CPU range      VME_A32_MSTR_BUS
 *     (0xe7f00000)                                   (0x08000000)
 *        .                        .                     .
 *        .                        .                     .
 *        .                        .                     .
 *     VME_A32_MSTR_LOCAL +    Same as CPU range      VME_A32_MSTR_BUS +
 *     SM_OBJ_MEM_ADRS +                              SM_OBJ_MEM_ADRS +
 *     SM_OBJ_MEM_SIZE - 1                            SM_OBJ_MEM_SIZE -1
 *     (0xe7f3ffff)                                   (0x0803ffff)
 *
 */

#define	LSI1_BS_LOCAL		VME_A32_MSTR_LOCAL
#define LSI1_BS_BUS		VME_A32_MSTR_BUS
/* Forget shared memory - open up the window to support 8MB*20slots
   = 160MB = 0x0a000000  DJA
#define LSI1_SIZE		(SM_OBJ_MEM_ADRS + SM_OBJ_MEM_SIZE)
*/
#define LSI1_SIZE		0x0a000000


#define	VAL_LSI1_BS		(LSI1_BS_LOCAL)
#define	VAL_LSI1_BD		(VAL_LSI1_BS + LSI1_SIZE)

#define	VAL_LSI1_TO		(LSI1_BS_BUS - LSI1_BS_LOCAL)

#define VAL_LSI1_CTL            (LSI_CTL_EN     | LSI_CTL_WP  |\
                                 LSI_CTL_D64    | LSI_CTL_A32 |\
                                 LSI_CTL_DATA   | LSI_CTL_USR |\
                                 LSI_CTL_BLK    | LSI_CTL_PCI_MEM)

#define LSI1_LOCAL_END 		(LSI1_BS_LOCAL + LSI1_SIZE)

/*
 * LSI0:
 * Outbound access to VME bus (local PCI to VME bus) via LSI0 register set.
 * The access through this window is to other node's Universe CSR register
 * set, in particular the mailbox registers which we must be able to
 * hit in order to generate a mailbox interrupt on the other node.  These
 * "mailbox interrupts" are used in support of VxWorks shared memory.
 *
 * The following outbound mapping is achieved:
 *
 *   CPU range              PCI range		  VME range (A32)
 *   ---------              ---------             ---------------
 *     VAL_LSI0_BS           Same as CPU range      VME_RAI_MSTR_BUS
 *     (0xe7f40000)                                 (0xfb000000)
 *        .                      .                     .
 *        .                      .                     .
 *        .                      .                     .
 *     VME_A32_MSTR_LOCAL +  Same as CPU range      VME_A32_MSTR_BUS +
 *     VME_A32_MSTR_SIZE - 1                        VME_A32_MSTR_SIZE - 1
 *     (0xe7f4ffff)                                 (0xfb00ffff)
 *
 */

#define VME_RAI_MSTR_BUS	0xfb000000
#define VME_RAI_SLV_SIZE        0x1000 		/* Univ reg space 4KB */
#define VME_RAI_MSTR_LOCAL	LSI1_LOCAL_END
#define VME_RAI_MSTR_SIZE       (16 * VME_RAI_SLV_SIZE) /* Room for 16 nodes */

/*
 * The following will allow for 16 uniquely placed CSR spaces among
 * peer nodes.  Each node will claim a unique space in the VME A32
 * region for its own CSR space.  Other nodes will reach into this
 * CSR space to hit the mailbox (doorbell) interrupt.
 */

#define	LSI0_BS_LOCAL		VME_RAI_MSTR_LOCAL
#define LSI0_BS_BUS		VME_RAI_MSTR_BUS

#define LSI0_SIZE		VME_RAI_MSTR_SIZE

#define	VAL_LSI0_BS		(LSI0_BS_LOCAL)
#define	VAL_LSI0_BD		(VAL_LSI0_BS + LSI0_SIZE)

#define	VAL_LSI0_TO		(LSI0_BS_BUS - LSI0_BS_LOCAL)
#define	VAL_LSI0_CTL		(LSI_CTL_EN	| LSI_CTL_WP  |\
			  	 LSI_CTL_D64	| LSI_CTL_A32 |\
			  	 LSI_CTL_DATA	| LSI_CTL_USR |\
			  	 LSI_CTL_SINGLE | LSI_CTL_PCI_MEM)

#define LSI0_LOCAL_END 		(VAL_LSI0_BS + LSI0_SIZE)

/*
 * LSI2:
 * Outbound access to A24 space
 *
 *   CPU range              PCI range		  VME range (A32)
 *   ---------              ---------             ---------------
 *     VME_A24_MSTR_LOCAL     Same as CPU range     VME_A24_MSTR_BUS
 *     (0xeff00000)                                 (0xff000000)
 *        .                      .                     .
 *        .                      .                     .
 *        .                      .                     .
 *     VME_A24_MSTR_LOCAL +   Same as CPU range     VME_A24_MSTR_BUS +
 *     VME_A24_MSTR_SIZE - 1                        VME_A24_MSTR_SIZE - 1
 *     (0xf0efffff)                                 (0xffffffff)
 */

#define LSI2_BS_LOCAL		VME_A24_MSTR_LOCAL
#define LSI2_BS_BUS		VME_A24_MSTR_BUS
#define LSI2_SIZE		VME_A24_MSTR_SIZE

#define	VAL_LSI2_BS		(LSI2_BS_LOCAL)
#define	VAL_LSI2_BD		(VAL_LSI2_BS + LSI2_SIZE)

#define	VAL_LSI2_TO		(LSI2_BS_BUS - LSI2_BS_LOCAL)
#define	VAL_LSI2_CTL		(LSI_CTL_EN	| LSI_CTL_WP  |\
			  	 LSI_CTL_D32/*sergey: was LSI_CTL_D64*/	| LSI_CTL_A24 |\
			  	 LSI_CTL_DATA	| LSI_CTL_USR |\
			  	 LSI_CTL_SINGLE | LSI_CTL_PCI_MEM)

#define LSI2_LOCAL_END		(VAL_LSI2_BS + LSI2_SIZE)

/*
 * LSI3:
 * Outbound access to A16 space
 *
 *   CPU range              PCI range		  VME range (A32)
 *   ---------              ---------             ---------------
 *     VME_A16_MSTR_LOCAL     Same as CPU range     VME_A16_MSTR_BUS
 *     (0xf0f00000)                                 (0xffff0000)
 *        .                      .                     .
 *        .                      .                     .
 *        .                      .                     .
 *     VME_A16_MSTR_LOCAL +   Same as CPU range     VME_A16_MSTR_BUS +
 *     VME_A16_MSTR_SIZE - 1                        VME_A16_MSTR_SIZE - 1
 *     (0xf0f0ffff)                                 (0xffffffff)
 */

#define LSI3_BS_LOCAL		VME_A16_MSTR_LOCAL
#define LSI3_BS_BUS		VME_A16_MSTR_BUS
#define LSI3_SIZE		VME_A16_MSTR_SIZE

#define	VAL_LSI3_BS		(LSI3_BS_LOCAL)
#define	VAL_LSI3_BD		(VAL_LSI3_BS + LSI3_SIZE)

#define	VAL_LSI3_TO		(LSI3_BS_BUS - LSI3_BS_LOCAL)
#define	VAL_LSI3_CTL		(LSI_CTL_EN	| LSI_CTL_WP  |\
			  	 LSI_CTL_D16/*sergey: was LSI_CTL_D64*/	| LSI_CTL_A16 |\
			  	 LSI_CTL_DATA	| LSI_CTL_USR |\
			  	 LSI_CTL_SINGLE | LSI_CTL_PCI_MEM)

#define	LSI3_LOCAL_END		(VAL_LSI3_BS + LSI3_SIZE)

/*
 * VSI1:
 * Inbound access (VME slave window) from VME A32 space.
 *
 * VME bus A32 window to access the master node's local memory.
 * This VME Slave window is only used by the master node.
 *
 *   VME range (A32)        PCI range             CPU range
 *   ---------------        ---------             ---------
 *     VME_A32_SLV_BUS        0x00000000            Same as PCI range
 *     (0x08000000)                                 (0x00000000)
 *        .                     .                      .
 *        .                     .                      .
 *        .                     .                      .
 *     VME_A32_SLV_BUS +
 *     VME_A32_SLV_SIZE - 1
 */

#define VSI1_BS_BUS		VME_A32_SLV_BUS
#define VSI1_BS_LOCAL		PCI_SLV_MEM_BUS
#define VSI1_SIZE		VME_A32_SLV_SIZE

#define VAL_VSI1_BS     	(VSI1_BS_BUS)

#ifdef ANY_BRDS_IN_CHASSIS_NOT_RMW
#    define VAL_VSI1_BD      	(VAL_VSI1_BS + VME_A32_SLV_SIZE)
#else
#    define VAL_VSI1_BD     	(VAL_VSI1_BS + SM_MEM_ADRS)
#endif /* ANY_BRDS_IN_CHASSIS_NOT_RMW */

#define VAL_VSI1_CTL    (VSI_CTL_EN       | VSI_CTL_PREN    | \
                         VSI_CTL_AM_DATA  | VSI_CTL_AM_PGM  | \
                         VSI_CTL_AM_SUPER | VSI_CTL_AM_USER | \
                         VSI_CTL_VAS_A32  | VSI_CTL_LAS_MEM | \
                         VSI_CTL_LD64EN)

#define VAL_VSI1_TO		(VSI1_BS_LOCAL - VSI1_BS_BUS)

/*
 * see "config.h" for an explanation of the inbound mappings involving
 * VSI1, VSI4, and VSI5.
 */

#define VAL_VSI4_BS     (VAL_VSI1_BS + \
                         SM_MEM_ADRS)
#define VAL_VSI4_BD     (VAL_VSI1_BS + \
                         SM_MEM_ADRS + \
                         SM_MEM_SIZE + \
                         SM_OBJ_MEM_SIZE)
#define VAL_VSI4_TO     (PCI_SLV_MEM_BUS - VAL_VSI1_BS + VME_A32_SLV_LOCAL)
#define VAL_VSI4_CTL    (VSI_CTL_EN       | VSI_CTL_PREN    | \
                         VSI_CTL_AM_DATA  | VSI_CTL_AM_PGM  | \
                         VSI_CTL_AM_SUPER | VSI_CTL_AM_USER | \
                         VSI_CTL_VAS_A32  | VSI_CTL_LAS_MEM | \
                         VSI_CTL_LD64EN   | VSI_CTL_PWEN    | \
                         VSI_CTL_LLRMW)

#define VAL_VSI5_BS     (VAL_VSI1_BS + \
                         SM_MEM_ADRS + \
                         SM_MEM_SIZE + \
                         SM_OBJ_MEM_SIZE)
#define VAL_VSI5_BD     (VAL_VSI1_BS + VME_A32_SLV_SIZE)
#define VAL_VSI5_TO     (PCI_SLV_MEM_BUS - VAL_VSI1_BS + VME_A32_SLV_LOCAL)
#define VAL_VSI5_CTL    (VSI_CTL_EN       | VSI_CTL_PREN    | \
                         VSI_CTL_AM_DATA  | VSI_CTL_AM_PGM  | \
                         VSI_CTL_AM_SUPER | VSI_CTL_AM_USER | \
                         VSI_CTL_VAS_A32  | VSI_CTL_LAS_MEM | \
                         VSI_CTL_LD64EN   | VSI_CTL_PWEN)

/*
 * Access to the CSR space is placed on the VMEbus for every node.
 * Each node will place its CSR space at a unique address, determined
 * by the node number (processor number) of the board.  The reason for
 * making the CSR space visible is to allow every node in the chassis
 * access to the mailbox register of everyother node.  Being able to
 * access another node's mailbox enables every node to be able to
 * interrupt every other node, something necessary for VxWorks shared
 * memory support.  The following #defines make the CSR space for
 * this node visible on the VME bus.  Access to other node's CSR
 * space (from this node) is done through the LSI0 outbound window.
 */

#define VME_MBOX0_OFFSET    0x348       /* offset to mailbox 0 reg */

#define VME_A32_REG_SIZE    VME_RAI_SLV_SIZE   /* individual reg space */
#define VME_A32_REG_SPACE   VME_RAI_MSTR_SIZE  /* total reg space */

#define VAL_RAI_CTL             (VRAI_CTL_EN       | \
                                 VRAI_CTL_AM_DATA  | VRAI_CTL_AM_PGM  | \
                                 VRAI_CTL_AM_SUPER | VRAI_CTL_AM_USER | \
                                 VRAI_CTL_VAS_A32)
#define VAL_RAI_BS              (VME_RAI_MSTR_BUS + (sysProcNumGet() * \
                                 VME_RAI_SLV_SIZE))

/* Universe Special Cycle Generator values */

#define VME_SCG_COMPARE_MASK            0xffffffff
#define VME_SCG_COMPARE_TO_SET          0x00000000
#define VME_SCG_SWAP_TO_SET             0x80000000
#define VME_SCG_COMPARE_TO_CLEAR        0x80000000
#define VME_SCG_SWAP_TO_CLEAR           0x00000000

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

/* PCI Device/Vendor ID, bus number and device number */

#define PCI_ID_MV64260	    	0x643011AB	/* Id for MV64260 PHB */
#define PCI_BUSNO_MV64260    	0	    	/* Bus no MV64260 PHB */
#define PCI_DEVNO_MV64260    	0	    	/* Device no MV64260 PHB */
#define PCI_ID_UNIVERSE		0x000010e3	/* Universe PCI-VME chip */
#define PCI_BUSNO_UNIVERSE	1		/* bus number of Universe */
#define PCI_DEVNO_UNIVERSE	0		/* device number of Universe */
#define PCI_ID_IBC              0x056510AD      /* Id for W83C55x PIB */
#define PCI_ID_IDE              0x010510AD      /* Id for W83C55x IDE */
#define PCI_ID_NCR810           0x00011000      /* Id for SYM53C810A Chip */
#define PCI_ID_NCR825           0x00031000      /* Id for SYM53C825 Chip */
#define PCI_ID_NCR860           0x00061000      /* Id for SYM53C860 Chip */
#define PCI_ID_NCR875           0x000f1000      /* Id for SYM53C875 Chip */
#define PCI_ID_NCR895           0x000c1000      /* Id for SYM53C895 Chip */
#define PCI_ID_NCR895A          0x00121000      /* Id for SYM53C895A Chip */
#define PCI_ID_SCSI             PCI_ID_NCR895A  /* Id for Primary SCSI */
#define PCI_ID_SEC_SCSI         PCI_ID_NCR895   /* Id for Secondary SCSI */


/* Sergey */
#define PCI_ID_HARRIER      0x480B1057      /* Id for Harrier PHB */
#define PCI_ID_I82559       0x12298086      /* Id for Intel 82559 */
#define PCI_ID_I82559ER     0x12098086      /* Id for Intel 82559 ER */




/* Bus and device numbers for PCI-PCI transparent bridge chip */

#define PCI_BUSNO_PCI2PCI	0
#define PCI_DEVNO_PCI2PCI	0xA

/*
 * The following define the values that go into the GT-64260's PCI
 * programming interface, sub class code and class code.  Since these
 * bytes are changeable, we will program them to these default values
 * on initialization.  Note that we do not program the revision ID field
 * since we really don't know what its value should be.
 */

#define PCI_PROGRAMMING_IF_MV64260 	0x00
#define PCI_SUBCLASS_MV64260 		0x80
#define PCI_CLASS_MV64260 		0x05

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
#define SM_INT_ARG2     (VME_RAI_MSTR_BUS + VME_MBOX0_OFFSET + \
                         (sysProcNumGet() * VME_A32_REG_SIZE))
#define SM_INT_ARG3     SIG1_INTR_SET

#define SM_BASE (LOCAL_MEM_LOCAL_ADRS + SM_MEM_ADRS)
#define SM_LIMIT (SM_BASE + SM_MEM_SIZE)

/*
 * Semaphore Test-and-Set Register as seen from a slave
 * Only used with a special version of sysBusTas().
 */

#define OFFBRD_VME_SEM_REG1     (VME_RAI_MSTR_LOCAL + \
                                 (CPU_VME_SEM_REG1 - CPU_VME_HW_REGS_BASE))

/* BSP configuration error policy */

#define CONTINUE_EXECUTION	0       /* Tolerate VPD/Configuration errors */
#define EXIT_TO_SYSTEM_MONITOR	1       /* Transfer to System Monitor */

/* MV64260 I2C Interface Support & VPD Support */

#define SPD_EEPROM_ADDR_0       0xA0    /* I2C address of SPD for banks A & B */
#define SPD_EEPROM_ADDR_1	0xA2	/* I2C address of SPD for banks C & D */
#define VPD_BRD_EEPROM_I2C_ADDR            0xA8 /* I2C address of VPD EEPROM */
#define USR_BRD_EEPROM_I2C_ADDR            0xAA /* I2C address of User EEPROM */
#define MV64260_FIXED_INIT_EEPROM_I2C_ADDR 0xA4 /* I2C addr for MV64260 Fixed */
                                                /* init EEPROM */
#define MV64260_USR_INIT_EEPROM_I2C_ADDR   0xA6 /* I2C addr for MV64260 User */
                                                /* defined init EEPROM */
#define DS1621_TEMP_SENSOR_I2C_ADDR        0x90 /* I2C addr for DS1621 */
                                                /* temperature sensor */
#define VALID_I2C_WRT_ADDR	USR_BRD_EEPROM_I2C_ADDR, \
				MV64260_USR_INIT_EEPROM_I2C_ADDR, 0xFFFFFFFF
#define I2C_SINGLE_BYTE_ADDR	1       /* one-byte address devices */
#define I2C_DUAL_BYTE_ADDR	2       /* two-byte address devices */
#define I2C_TRIPLE_BYTE_ADDR	3       /* three-byte address devices */
#define SPD_BYTE_ADDR_SZ                        I2C_SINGLE_BYTE_ADDR
#define VPD_BYTE_ADDR_SZ                        I2C_DUAL_BYTE_ADDR
#define USR_BYTE_ADDR_SZ                        I2C_DUAL_BYTE_ADDR
#define MV64260_FIXED_INIT_BYTE_ADDR_SZ         I2C_SINGLE_BYTE_ADDR
#define MV64260_USR_INIT_BYTE_ADDR_SZ           I2C_DUAL_BYTE_ADDR
#define DEFAULT_PRODUCT_ID      "Unknown"
#define DEFAULT_CPU_ID          "Unknown"
#define VPD_BRD_OFFSET      0       /* offset into brd's eeprom for vpd data */
#define VPD_PKT_LIMIT       25      /* Max number of packets expected */
#define VPD_EEPROM_SIZE 512	    /* size of VPD serial EEPROM */

/*
 * Support for determining if we're ROM based or not.  _sysInit
 * saves the startType parameter at location ROM_BASED_FLAG.
 */

#define PCI_AUTOCONFIG_FLAG_OFFSET (0x4c00)
#define PCI_AUTOCONFIG_FLAG        (*(UCHAR *)(LOCAL_MEM_LOCAL_ADRS + \
				     PCI_AUTOCONFIG_FLAG_OFFSET))
#define PCI_AUTOCONFIG_DONE        (PCI_AUTOCONFIG_FLAG != 0)

/*
 * Support for determining which of the MV64260's PCI interfaces to access
 * based on the PCI bus number specified.  sysPciAutoConfig() called from
 * sysHwInit() saves the highest numbered bus connected to Bus interface 0.0
 * and this bus number, plus 1, is equivalent to bus 0 on Bus interface 1.0.
 */

#define PCI_BUS_ALIAS_OFFSET \
        (ROUND_UP((PCI_AUTOCONFIG_FLAG_OFFSET + sizeof(UCHAR)), sizeof(int)))
#define PCI_BUS_ALIAS              (*(UINT *)(LOCAL_MEM_LOCAL_ADRS + \
                                     PCI_BUS_ALIAS_OFFSET))

/* IPMC SCSI Support */

#define NCR810_DELAY_MULT       10      /* ncr810/ncr825 delay loop count */
#define SYS_REG_SIOP_HW_REGS    {0,0,0,0,0,1,0,0,0,0,0}

/*
 * Interrupt number assignments:
 * Each bit in the mv64260 main cause
 * (low and high) have an associated interrupt bit assigned below.
 * ICI_MICL_INT_NUMx defines interrupt numbers for bits in the MV64260
 * main cause low register.  ICI_MICH_INT_NUMx defines interrupt
 * numbers for the MV64260 main cause high register.  In addition
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

/*
 * In addition to the interrupts defined above (associated with the main
 * interrupt cause low and high registers), we also have some external
 * interrupt sources which are associated with MPP (Multi-Purpose Pins)
 * which are in turn programmed as GPP (General Purpose Port) pins.  On
 * the MVME5500, any GPP related interrupt causes one of the 4 bits (24,
 * 25, 26, or 27) in the "main interrupt cause high" register to be set
 * as well as the associated "local" bit in the GPP value register.
 * Here's the selection information for these GPP pins.
 */

#define GPP_INPUT		(0x0)
#define GPP_OUTPUT		(0x1)

#define GPP_ACTIVE_HI		(0x0)
#define GPP_ACTIVE_LO		(0x1)

/* The following will be used to program the GPP I/O control register */

#define GPP_IO0       (GPP_INPUT)   /* COM0/COM1 ORed */
#define GPP_IO1       (GPP_INPUT)   /* Unused, pulled high */
#define GPP_IO2       (GPP_INPUT)   /* Abort */
#define GPP_IO3       (GPP_INPUT)   /* RTC || thermostat ORed */
#define GPP_IO4       (GPP_OUTPUT)  /* Unused, pulled high, tied to GPP27 */
#define GPP_IO5       (GPP_INPUT)   /* Unused, pulled high, tied to GPP28 */
#define GPP_IO6       (GPP_INPUT)   /* GT-64260A WDNMI# interrupt, tied to 24 */
#define GPP_IO7       (GPP_INPUT)   /* LXT971A interrupt (10/100Mbit PHY) */

#define GPP_IO8       (GPP_INPUT)   /* PMC 1 INTA# */
#define GPP_IO9       (GPP_INPUT)   /* PMC 1 INTB# */
#define GPP_IO10      (GPP_INPUT)   /* PMC 1 INTC# */
#define GPP_IO11      (GPP_INPUT)   /* PMC 1 INTD#, IPMC interrupt */
#define GPP_IO12      (GPP_INPUT)   /* VME interrupt VLINT0 */
#define GPP_IO13      (GPP_INPUT)   /* VME interrupt VLINT1 */
#define GPP_IO14      (GPP_INPUT)   /* VME interrupt VLINT2 */
#define GPP_IO15      (GPP_INPUT)   /* VME interrupt VLINT3 */

#define GPP_IO16      (GPP_INPUT)   /* PMC 2 INTA# */
#define GPP_IO17      (GPP_INPUT)   /* PMC 2 INTB# */
#define GPP_IO18      (GPP_INPUT)   /* PMC 2 INTC# */
#define GPP_IO19      (GPP_INPUT)   /* PMC 2 INTD# */
#define GPP_IO20      (GPP_INPUT)   /* 82544 interrupt */
#define GPP_IO21      (GPP_INPUT)   /* Unused, pulled high */
#define GPP_IO22      (GPP_INPUT)   /* Unused, pulled high */
#define GPP_IO23      (GPP_INPUT)   /* Unused, pulled high */

#define GPP_IO24      (GPP_INPUT)   /* Watchdog NMI output WDNMI#
				     * Note that the spec says that this
				     * pin along with GPP_IO25  are outputs.
				     * We program them as inputs to disable
				     * any effect that the GT64260 watchdog
				     * timer can have on the processor
				     */

#define GPP_IO25      (GPP_INPUT)   /* Watchdog expired WDE# output.  See
				     * note associated with GPP_IO24.
				     */

#define GPP_IO26      (GPP_OUTPUT)  /* GT-64260A SROM init active */
#define GPP_IO27      (GPP_INPUT)   /* Unused, pulled high, tied to GPP6 */
#define GPP_IO28      (GPP_INPUT)   /* Unused, pulled high, tied to GPP5 */
#define GPP_IO29      (GPP_OUTPUT)  /* Optnl ext PPCBus arbiter BG1 enable */
#define GPP_IO30      (GPP_INPUT)   /* Unused, pulled high */
#define GPP_IO31      (GPP_INPUT)   /* Unused, pulled high */

/* Here is the value containing all of the GPP pin programming bits */

#define GPP_IO_CTRL_VAL ((GPP_IO0 << 0)   | (GPP_IO1  << 1)  | \
                         (GPP_IO2 << 2)   | (GPP_IO3  << 3)  | \
                         (GPP_IO4 << 4)   | (GPP_IO5  << 5)  | \
                         (GPP_IO6  << 6)  | (GPP_IO7  << 7)  | \
                         (GPP_IO8  << 8)  | (GPP_IO9  << 9)  | \
                         (GPP_IO10 << 10) | (GPP_IO11 << 11) | \
                         (GPP_IO12 << 12) | (GPP_IO13 << 13) | \
                         (GPP_IO14 << 14) | (GPP_IO15 << 15) | \
                         (GPP_IO16 << 16) | (GPP_IO17 << 17) | \
                         (GPP_IO18 << 18) | (GPP_IO19 << 19) | \
                         (GPP_IO20 << 20) | (GPP_IO21 << 21) | \
                         (GPP_IO22 << 22) | (GPP_IO23 << 23) | \
                         (GPP_IO24 << 24) | (GPP_IO25 << 25) | \
                         (GPP_IO26 << 26) | (GPP_IO27 << 27) | \
                         (GPP_IO28 << 28) | (GPP_IO29 << 29) | \
                         (GPP_IO30 << 30) | (GPP_IO31 << 31))


/* The following will be used to program the GPP level control register */

#define GPP_LEVEL0  (GPP_ACTIVE_HI)	/* COM0/COM1 ORed */
#define GPP_LEVEL1  (GPP_ACTIVE_LO)	/* Unused, pulled high */
#define GPP_LEVEL2  (GPP_ACTIVE_LO)	/* Abort */
#define GPP_LEVEL3  (GPP_ACTIVE_LO)	/* RTC || thermostat ORed */
#define GPP_LEVEL4  (GPP_ACTIVE_LO)     /* Unused, pulled high, tied to GPP27 */
#define GPP_LEVEL5  (GPP_ACTIVE_LO)	/* Unused, pulled high, tied to GPP27 */
#define GPP_LEVEL6  (GPP_ACTIVE_LO)	/* WDNMI# interrupt */
#define GPP_LEVEL7  (GPP_ACTIVE_LO)	/* LXT971A interrupt (10/100Mbit PHY) */

#define GPP_LEVEL8  (GPP_ACTIVE_LO)	/* PMC 1 INTA# */
#define GPP_LEVEL9  (GPP_ACTIVE_LO)	/* PMC 1 INTB# */
#define GPP_LEVEL10 (GPP_ACTIVE_LO)	/* PCI 1 INTC# */
#define GPP_LEVEL11 (GPP_ACTIVE_LO)	/* PCI 1 INTD#, IPMC interrupt */
#define GPP_LEVEL12 (GPP_ACTIVE_LO)	/* VME interrupt VLINT0 */
#define GPP_LEVEL13 (GPP_ACTIVE_LO)	/* VME interrupt VLINT1 */
#define GPP_LEVEL14 (GPP_ACTIVE_LO)	/* VME interrupt VLINT2 */
#define GPP_LEVEL15 (GPP_ACTIVE_LO)	/* VME interrupt VLINT3 */

#define GPP_LEVEL16 (GPP_ACTIVE_LO)	/* PMC 2 INTA# */
#define GPP_LEVEL17 (GPP_ACTIVE_LO)	/* PMC 2 INTB#  */
#define GPP_LEVEL18 (GPP_ACTIVE_LO)	/* PMC 2 INTC# */
#define GPP_LEVEL19 (GPP_ACTIVE_LO)	/* PMC 2 INTD# */
#define GPP_LEVEL20 (GPP_ACTIVE_LO)	/* 82544 */
#define GPP_LEVEL21 (GPP_ACTIVE_LO)	/* Unused, pulled high */
#define GPP_LEVEL22 (GPP_ACTIVE_LO)	/* Unused, pulled high */
#define GPP_LEVEL23 (GPP_ACTIVE_LO)	/* Unused, pulled high */

#define GPP_LEVEL24 (GPP_ACTIVE_LO)	/* Reserved for Watchdog WDNMI#
					 * output.  We will program all
					 * "reserved" or "unused" pins as
					 * "active high".
					 */

#define GPP_LEVEL25 (GPP_ACTIVE_LO)	/* Reserved for Watchdog WDE# output */
#define GPP_LEVEL26 (GPP_ACTIVE_HI)	/* SROM INIT active output */
#define GPP_LEVEL27 (GPP_ACTIVE_LO)	/* Cross-proc interrupt from CPU0 */
#define GPP_LEVEL28 (GPP_ACTIVE_LO)	/* Res. cross-proc int from CPU1 */
#define GPP_LEVEL29 (GPP_ACTIVE_LO)	/* Optional ext PPCbus arb BG1 enbl */
#define GPP_LEVEL30 (GPP_ACTIVE_LO)	/* Unused */
#define GPP_LEVEL31 (GPP_ACTIVE_LO)	/* Unused */

/*
 * Interrupt Main Cause High bits associated with GPP interrupts have
 * the following vectors assigned to them.
 */

#define GPP_INT_LVL_0_7		(ICI_MICH_INT_NUM_24)
#define GPP_INT_LVL_8_15	(ICI_MICH_INT_NUM_25)
#define GPP_INT_LVL_16_23	(ICI_MICH_INT_NUM_26)
#define GPP_INT_LVL_24_31	(ICI_MICH_INT_NUM_27)

#define GPP_INT_VEC_0_7		(INUM_TO_IVEC(GPP_INT_LVL_0_7))
#define GPP_INT_VEC_8_15	(INUM_TO_IVEC(GPP_INT_LVL_8_15))
#define GPP_INT_VEC_16_23	(INUM_TO_IVEC(GPP_INT_LVL_16_23))
#define GPP_INT_VEC_24_31	(INUM_TO_IVEC(GPP_INT_LVL_24_31))

/*
 * Formulas for converting main cause related interrupt numbers to
 * register,bit or from register,bit to interrupt number.
 */

#define MAIN_CAUSE_INT_NUM(reg,bit)	(((reg) * 32) + (bit))
#define MAIN_CAUSE_BIT_NUM(intNum)	((intNum) % 32)
#define MAIN_CAUSE_REG_NUM(intNum)	((intNum) / 32)

/* Formula for GPP local value bit number and interrupt  number */

#define GPP_LOCAL_INT_NUM(bit)		((bit) + GPP_IC_INT_NUM_MIN)
#define GPP_LOCAL_BIT_NUM(intNum)	((intNum) - GPP_IC_INT_NUM_MIN)

/* Define the host bridge device number */

#define PCI_HOST_BRIDGE_DEV_NUM		0

/*
 * Assign some specific interrupt numbers  - first generic PCI
 * INTA thru INTB interrupts for both buses.
 */

#define PCI0_INTA	(GPP_IC_INT_NUM_8)
#define PCI0_INTB	(GPP_IC_INT_NUM_9)
#define PCI0_INTC	(GPP_IC_INT_NUM_10)
#define PCI0_INTD	(GPP_IC_INT_NUM_11)

#define PCI1_INTA	(GPP_IC_INT_NUM_16)
#define PCI1_INTB	(GPP_IC_INT_NUM_17)
#define PCI1_INTC	(GPP_IC_INT_NUM_18)
#define PCI1_INTD	(GPP_IC_INT_NUM_19)

/* PCI 0 bridge base interrupt assignments */

#define PCI0_BR_INTA    (GPP_IC_INT_NUM_12)
#define PCI0_BR_INTB    (GPP_IC_INT_NUM_13)
#define PCI0_BR_INTC    (GPP_IC_INT_NUM_14)
#define PCI0_BR_INTD    (GPP_IC_INT_NUM_15)

/*
 * PCI interrupt routing PCIx_Dyy_ROUTE (x = bus, yy = device)
 * 0xff,0xff,0xff,0xff = not wired to PCI interrupts
 */

#define PCI0_D00_ROUTE  0xff,0xff,0xff,0xff
#define PCI0_D01_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D02_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D03_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D04_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D05_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D06_ROUTE	PCI0_INTA, PCI0_INTB, \
                             PCI0_INTC, PCI0_INTD	/* PMC 1 */


/* Sergey(it is my guess!): interrupt for PRPMC800 Ethernet must be 0x1a;
it can be set here, then it will go to pci header and prpmc800
will pick it up from there; or it can be hardprogrammed in 
/prpmc800/sysFei82557End.c, then pci header value will be ignored so
keep it as is: PCI0_INTB */
#ifdef PRPMC800
#define PCI0_D07_ROUTE	PCI0_INTB/*0x1a*/, PCI0_INTC, \
                             PCI0_INTD, PCI0_INTA	/* PMC 1 IDSEL B */
#else
#define PCI0_D07_ROUTE	PCI0_INTB, PCI0_INTC, \
                             PCI0_INTD, PCI0_INTA	/* PMC 1 IDSEL B */
#endif


#define PCI0_D08_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D09_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D10_ROUTE 	PCI0_BR_INTA, PCI0_BR_INTB, \
		    	     PCI0_BR_INTC, PCI0_BR_INTD  /* 21154 PCI bridge */
#define PCI0_D11_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D12_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D13_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D14_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D15_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D16_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D17_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D18_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D19_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D20_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D21_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D22_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D23_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D24_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D25_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D26_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D27_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D28_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D29_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D30_ROUTE	0xff,0xff,0xff,0xff
#define PCI0_D31_ROUTE	0xff,0xff,0xff,0xff

#define PCI1_D00_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D01_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D02_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D03_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D04_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D05_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D06_ROUTE	PCI1_INTA, PCI1_INTB, \
                             PCI1_INTC, PCI1_INTD	/* PMC 2 */
#define PCI1_D07_ROUTE	PCI1_INTB, PCI1_INTC, \
                             PCI1_INTD, PCI1_INTA	/* PMC 2  IDSEL B */
#define PCI1_D08_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D09_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D10_ROUTE	GPP_IC_INT_NUM_20, 0xff, \
			     0xff,0xff          /* 82C544 ethernet */
#define PCI1_D11_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D12_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D13_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D14_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D15_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D16_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D17_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D18_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D19_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D20_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D21_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D22_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D23_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D24_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D25_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D26_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D27_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D28_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D29_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D30_ROUTE	0xff,0xff,0xff,0xff
#define PCI1_D31_ROUTE	0xff,0xff,0xff,0xff

/* Define some specific "main cause" bits */

#define ICI_CPU0_GPP0_7_BIT	24
#define ICI_CPU0_GPP8_15_BIT	25
#define ICI_CPU0_GPP16_23_BIT	26
#define ICI_CPU0_GPP24_31_BIT	27

/*
 * Universe (PCI-VME) bridge interrupt support.
 * First define the GT-64260 GPP interrupt associated with Universe:
 */

#define UNIV_INT_LVL0	GPP_IC_INT_NUM_12	/* VLINT0 */
#define UNIV_INT_LVL1	GPP_IC_INT_NUM_13	/* VLINT1 */
#define UNIV_INT_LVL2	GPP_IC_INT_NUM_14	/* VLINT2 */
#define UNIV_INT_LVL3	GPP_IC_INT_NUM_15	/* VLINT3 */

/* We will route all specific Universe Interrupts through VLINT0 */

#define UNIV_INT_LVL	UNIV_INT_LVL0   	/* universe int level */

/*
 * Define the interrupt numbers associated with Universe interrupts
 */

#define UNIV_INT_NUM_MIN	 (GPP_IC_INT_NUM_MAX + 1)
#define UNIV_DMA_INT_VEC         (UNIV_INT_NUM_MIN + 0)
#define UNIV_VME_SW_IACK_INT_VEC (UNIV_INT_NUM_MIN + 1)
#define UNIV_PCI_SW_INT_VEC      (UNIV_INT_NUM_MIN + 2)
#define UNIV_VOWN_INT_VEC        (UNIV_INT_NUM_MIN + 3)
#define UNIV_LERR_INT_VEC        (UNIV_INT_NUM_MIN + 4)
#define UNIV_VERR_INT_VEC        (UNIV_INT_NUM_MIN + 5)
#define UNIV_SYSFAIL_INT_VEC     (UNIV_INT_NUM_MIN + 6)
#define UNIV_ACFAIL_INT_VEC      (UNIV_INT_NUM_MIN + 7)
#define UNIV_MBOX0_INT_VEC       (UNIV_INT_NUM_MIN + 9)    /* Mailbox 0 */
#define UNIV_MBOX1_INT_VEC       (UNIV_INT_NUM_MIN + 10)   /* Mailbox 1 */
#define UNIV_MBOX2_INT_VEC       (UNIV_INT_NUM_MIN + 11)   /* Mailbox 2 */
#define UNIV_MBOX3_INT_VEC       (UNIV_INT_NUM_MIN + 12)   /* Mailbox 3 */
#define UNIV_LM0_INT_VEC         (UNIV_INT_NUM_MIN + 13)   /* Loc. Mon. 0 */
#define UNIV_LM1_INT_VEC         (UNIV_INT_NUM_MIN + 14)   /* Loc. Mon. 1 */
#define UNIV_LM2_INT_VEC         (UNIV_INT_NUM_MIN + 15)   /* Loc. Mon. 2 */
#define UNIV_LM3_INT_VEC         (UNIV_INT_NUM_MIN + 16)   /* Loc. Mon. 3 */
#define UNIV_INT_NUM_MAX	 (UNIV_LM3_INT_VEC)

/* IPMC (Winbond) Interrupt Support */

#if ((defined INCLUDE_PCX7307_SIO) || (defined INCLUDE_Z85230_SIO))
#   define PIB_INT_LVL          (GPP_IC_INT_NUM_11)
#   define PIB_INT_VEC          (0x00 + PIB_INT_LVL)
#   define ISA_INT_NUM_MIN      (1 + UNIV_INT_NUM_MAX)
#   define COM2_INT_LVL         (0x04 + ISA_INT_NUM_MIN)
#   define COM3_INT_LVL         (0x03 + ISA_INT_NUM_MIN)
#   define COM4_INT_LVL         (0x09 + ISA_INT_NUM_MIN)
#   define COM5_INT_LVL         (0x09 + ISA_INT_NUM_MIN)
#   define Z85230_INT_LVL       (COM4_INT_LVL)
#   define ISA_INT_NUM_MAX      (COM5_INT_LVL)
#   define COM2_INT_VEC         (0x00 + COM2_INT_LVL)
#   define COM3_INT_VEC         (0x00 + COM3_INT_LVL)
#   define COM4_INT_VEC         (0x00 + COM4_INT_LVL)
#   define COM5_INT_VEC         (0x00 + COM5_INT_LVL)
#   define Z85230_INT_VEC       (0x00 + Z85230_INT_LVL)
#   define PCI_MSTR_IACK_REG    (MV64260_REG_BASE + PCIIF_INT_ACK_PCI0)
#endif /* INCLUDE_PCX7307_SIO, INCLUDE_Z85230_SIO */

/*
 * Define the entry into INT_UTIL_DESC intUtil [] which allows us
 * to hook Universe interrupt management and ISA interrupt management
 * into the mv64260 interrupt handler.
 * We need to put the function declarations here as well.
 */

#ifndef _ASMLANGUAGE
    STATUS sysUnivIntEnable (int intLevel);
    STATUS sysUnivIntDisable (int intLevel);
#endif

#if ((defined INCLUDE_PCX7307_SIO) || (defined INCLUDE_Z85230_SIO))
#   ifndef _ASMLANGUAGE
       int sysIbcIntEnable (int);
       int sysIbcIntDisable (int);
#   endif /* _ASMLANGUAGE */
#   define INT_UTIL_EXTRA ,{UNIV_INT_NUM_MIN,UNIV_INT_NUM_MAX, \
			    sysUnivIntEnable,sysUnivIntDisable}, \
			   {ISA_INT_NUM_MIN,ISA_INT_NUM_MAX, \
                            sysIbcIntEnable,sysIbcIntDisable}
#else
#   define INT_UTIL_EXTRA ,{UNIV_INT_NUM_MIN,UNIV_INT_NUM_MAX, \
			    sysUnivIntEnable,sysUnivIntDisable}
#endif /* INCLUDE_PCX7307_SIO, INCLUDE_Z85230_SIO */

/*
 * Specific GT-64260 device support register programming values
 * Note that we don't have a #define for the MV64260
 * DEVCS0_BASE_ADDR here since this value varies, depending on which
 * bank were are booting from.  A similar comment applies to the
 * BOOTCS_BASE_ADDR.
 */

#define MV64260_DEVCS1_BASE_ADDR  0xf1d00000 /* Device bank base */
#define MV64260_DEVCS1_SIZE 	  0x00100000 /* Device bank size */

/*
 * Flash bank A always starts at the same address, regardless of which
 * bank we are booting from.
 */

#define MV64260_FLSH_BANK_A_ADDR  0xf8000000 /* Flash bank A address */

/*
 * Unlike flash bank A, flash bank B's starting address depends on which
 * bank we are booting from.  Let's first define the starting address of
 * flash bank B if booting from flash bank A.
 */

#define MV64260_FLSH_BANK_B_ADDR_BOOT_A		0xf2000000

/* Now for the starting address of flash bank B if booting from flash bank B */

#define MV64260_FLSH_BANK_B_ADDR_BOOT_B		0xfe000000

/* Specific MV64260 Bank parameter register values */

/*
 * Default bank parameters register value.
 *
 * Name                Value    Bit positions
 *
 * TURNOFF              0x7     2:0
 * ACC2FIRST            0xf     6:3
 * ACC2NEXT             0xf     10:7
 * ALE2WR               0x7     13:11
 * WRLOW                0x7     16:14
 * WRHIGH               0x7     19:17
 * DEVWIDTH             0x2     21:20 (0x2 = 32-bit width, 0x0 = 8bit width)
 * TURNOFFEXT           0x1     22
 * ACC2FIRSTEXT         0x1     23
 * ACC2NEXTEXT          0x1     24
 * ALE2WREXT            0x1     25
 * WRLOWEXT             0x1     26
 * WRHIGHEXT            0x1     27
 * BADRSKEW             0x0     29:28
 * RESERVED             0x3     31:30
 */

/*
 * Use "worst case" values for default programming - these are ok for
 * general use since the device bus bandwith is tiny.
 */

#define DEVCTRL_DEV_BANK_DEFAULT_PARAMS_VAL 0x8fefffff

/* Boot bank parameter value */

#define DEVCTRL_DEV_BOOT_BANK_PARAMS_VAL  DEVCTRL_DEV_BANK_DEFAULT_PARAMS_VAL

/* Bank 0 (nonboot bank) parameter value - width dynamically calculated */

#define	DEVCTRL_DEV_BANK0_PARAMS_VAL   DEVCTRL_DEV_BANK_DEFAULT_PARAMS_VAL

/*
 * Bank 1 (device controller) parameter value - differs from the default
 * value only in the "width" field.  The width for this bank (bits 22:20)
 * is set to 00 to encode an 8-bit width.
 */

#define DEVCTRL_DEV_BANK1_PARAMS_VAL   0x8fcfffff

#define DEVCTRL_DEV_BANK2_PARAMS_VAL   DEVCTRL_DEV_BANK_DEFAULT_PARAMS_VAL

#define DEVCTRL_DEV_BANK3_PARAMS_VAL   DEVCTRL_DEV_BANK_DEFAULT_PARAMS_VAL

/*
 * Mv64260 interface control register value
 * DEVCTRL_DEV_IF_CTRL offset: 0x4C0
 *
 * Name                Value    Bit positions
 *
 * TIMEOUT              0xffff  15:0
 * RDTRIG               0x1     16
 * RESERVED             0x1     17
 * READYS               0x1     18
 * RESERVED             0..0    19:31
 */

#define DVCTRL_DEV_IF_CTRL_VAL				0x0007ffff

/*
 * Mv64260 cross bar arbiter register (low) values
 * DEVCTRL_DEV_IF_XBAR_CTRL_LO offset: 0x4C8
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

#define DEVCTRL_DEV_IF_XBAR_CTRL_LO_VAL 		0x11765432

/*
 * Mv64260 cross bar arbiter register (high) values
 * DEVCTRL_DEV_IF_XBAR_CTRL_HI offset: 0x4CC
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

#define DEVCTRL_DEV_IF_XBAR_CTRL_HI_VAL	 		0x11765432

/*
 * Mv64260 cross bar timeout register value
 * DEVCTRL_DEV_IF_XBAR_TMOUT offset: 0x4C4
 *
 * Name          Value    Bit positions
 *
 * PRESET         0xff      7:0
 * RESERVED (0)   0x0       15:8
 * TIMEOUTEN      0x1       16
 * RESERVED (0)   0x0       31:17
 */

#define  DEVCTRL_DEV_IF_XBAR_TMOUT_VAL			0x000100ff

/*
 * Serial Port Support
 *
 * The MVME5500 uses the PC16550 Universal Receiver/Transmitter (UART)
 * on the MV64260 device controller bus to support two async serial
 * ports (COM0, COM1).  Interrupts for COM0/COM1 share GPP pin 0.
 * In order to enable the COM port interrupts on the MVME5500 we must
 * unmask the GPP interrupt in the GPP interrupt mask register.  We must also
 * enable this GPP interrupt in the Main Interrupt High Cause register,
 * by calling intEnable with GPP_INT_LVL_0_7 as the argument in sysHwInit2().
 *
 * Installing an IPMC761/712 transition module (and MVME761/712 transition 
 * board) adds an additional 4 serial ports.  Two 16550-compatible async 
 * serial ports and two async/sync serial ports.
 */

#ifdef INCLUDE_PCX7307_SIO
#   define N_UART_CHANNELS      4               /* No. serial I/O channels */
#else
#   define N_UART_CHANNELS      2               /* No. serial I/O channels */
#endif /* INCLUDE_PCX7307_SIO */

/* total number of serial ports */

#ifdef INCLUDE_Z85230_SIO
#   define N_SIO_CHANNELS       (N_UART_CHANNELS  + 2)
#else
#   define N_SIO_CHANNELS       N_UART_CHANNELS
#endif /* INCLUDE_Z85230_SIO */

#undef  NUM_TTY
#define NUM_TTY         N_SIO_CHANNELS

#define UART_REG_ADDR_INTERVAL  1       	/* addr diff of adjacent regs */
#define COM0_BASE_ADR		(MV64260_DEVCS1_BASE_ADDR + 0x20000)	
#define COM1_BASE_ADR		(MV64260_DEVCS1_BASE_ADDR + 0x21000)
#ifdef INCLUDE_PCX7307_SIO
#   define PCX7307_UART1        (ISA_MSTR_IO_LOCAL + SIO_UART1)
#   define PCX7307_UART2        (ISA_MSTR_IO_LOCAL + SIO_UART2)
#   define COM2_BASE_ADR        PCX7307_UART1
#   define COM3_BASE_ADR        PCX7307_UART2
#endif /* INCLUDE_PCX7307_SIO */

#define COM0_INT_LVL		GPP_IC_INT_NUM_0
#define COM0_INT_VEC		COM0_INT_LVL

#define COM1_INT_LVL		COM0_INT_LVL	
#define COM1_INT_VEC		COM1_INT_LVL

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

#define TMR_CNTR0_INT_VEC      (INUM_TO_IVEC(ICI_MICL_INT_NUM_8))
#define TMR_CNTR0_INT_LVL      (ICI_MICL_INT_NUM_8)

/*
 * Interrupt vector and interrupt level assigned to RTC.
 */

#define RTC_INT_VEC		(INUM_TO_IVEC(GPP_IC_INT_NUM_3))
#define RTC_INT_LVL		(GPP_IC_INT_NUM_3)

/*
 * Construct the "value" to be programmed into the GT-64260 CPU Configuration
 * register here:
 *
 *  Bit    Field Name     Description                       Programmed value
 *
 *  7:0    NoMatchCnt     CPU Address Miss Counter              0xff
 *
 *    8    NoMatchCntEn   CPU Address Miss Counter              0x0
 *                          Enable - relevant only
 *                          if multi_GT is enabled.
 *                            0 = disabled
 *                            1 = enabled
 *
 *    9    NoMatchCntExt  CPU address miss counter MSB          0x0
 *
 *   10    Reserved       Reserved                              0x0
 *
 *   11    AACK Delay     Address Acknowledge Delay             0x1
 *                            0 - AACK* is asserted one
 *                                  cycle after TS*
 *                            1 - AACK* is asserted two
 *                                  cycles after TS*
 *
 *   12    Endianess       Must be 0                            0x0
 *
 *   13    Pipeline        Pipeline enable                      0x1
 *                             0 = disabled.  The GT-64260
 *                                   will not respond with
 *                                   AACK* to a new CPU
 *                                   transaction, before the
 *                                   previous transaction data
 *                                   phase completed.
 *                             1 = enabled
 *
 *   14    Reserved        Reserved                             0x0
 *
 *   15    TADelay         Transfer Acknowledge Delay           0x1
 *                             0 = earliest TA* assertion
 *                                   is in the same cycle
 *                                   with AACK*
 *                             1 = earliest TA* assertion
 *                                   is two cycles after
 *                                   AACK*
 *
 *   16    RdOOO           Read Out of Order Completion         0x0
 *                             0 = not supported, data is
 *                                   always returned in
 *                                   order (DTI[0-2] is
 *                                   always drive 0x0)
 *                             1 = supported
 *
 *   17    Stop Retry      Relavent only if PCI retry is        0x1
 *                           enabled
 *                             0 = keep retry all PCI
 *                                   transactions targeted
 *                                   to the GT-64260A
 *                             1 = stop retry of PCI
 *                                   transactions
 *
 *   18    MultiGTDec      Multi-GT Address Decode              0x0
 *                             0 = normal address decoding
 *                             1 = multi-gt address decoding
 *
 *   19    DPValid         CPU DP[0-7] Connection               0x1
 *                             0 = not connected.  CPU write
 *                                 parity is not checked
 *                             1 = connected
 *
 * 21:20   Reserved        Reserved                             0x0
 *
 *   22    PErrProp        Parity error propagation             0x0
 *                             0 = GT-64260A always drives
 *                                   good parity on DP[0-7]
 *                                   during CPU reads.
 *                             1 = GT-64260A drives bad parity
 *                                   on DP[0-7] in case the
 *                                   read response from the
 *                                   target interface comes
 *                                   with erroneous data
 *                                   indication (e.g. ECC
 *                                   error from SDRAM
 *                                   interface)
 *
 *   23    FastClk         The number of pipe stages in the     0x1
 *                           CPU interface
 *                               0 = two pipe states
 *                               1 = additional pipe stage
 *
 *   24    Reserved         Reserved                            0x0
 *
 *   25    AACK Delay_2     Together with AACK delay [11],      0x1
 *                            defines the earliest assertion
 *                            of AACK*.
 *                              0 = earliest assertion of
 *                                    AACK* is defined by
 *                                    the AACK_Del bit.
 *                              1 = earliest assertion is
 *                                    three cycles after TS*
 *                                    assertion
 *
 *   26    APValid          CPU AP[0-3] connection              0x1
 *                              0 = not connected, the CPU
 *                                    address parity is not
 *                                    checked
 *                              1 = connected
 *
 *   27    RemapWrDis       Address REmap Registers Write       0x0
 *                            control
 *                              0 = write to low address decode
 *                                    register results in
 *                                    writing to the
 *                                    corresponding remap
 *                                    register
 *                              1 = write to low address decode
 *                                    register has no effect on
 *                                    the corresponding remap
 *                                    register
 *
 *   28    ConfSBDis        Configuration Read Sync Barrier     0x1
 *                            Disable
 *                              0 = sync barrier enabled
 *                              1 = sysc barrier disabled
 *
 *   29    IOSBDis          I/O read sync barrier disable       0x1
 *                              0 = sync barrier enabled
 *                              1 = sync barrier disabled
 *
 *
 *   30    ClkSync          Clock synchronization               0x1
 *                              0 = the CPU interface is
 *                                    running with SysClk,
 *                                    which is asynchronous
 *                                    to TClk
 *                              1 = the CPU interface is
 *                                    running with TClk.
 *
 *   31    Reserved         Reserved                            0x0
 */

#define CPUIF_CPU_CFG_VAL 0x768aa8ff  /* Set according to above description */

/* MV64260 Ethernet unit (wancom interface) */

/*
 * The MV64260 supports two "standard" ethernet ports, the following
 * two #define's encode this fact as well as supplying the input to the
 * computation of ETHER_TOTAL_MEM_SIZE (see comment below regarding the
 * MV64260 errata).  If either MAX_ENET_PORTS or MAX_ENET_PORT_NUM is
 * modified then wancomEnd.obj must be rebuilt (difficult to do if
 * source is not available).  Thus it is highly recommended that neither
 * of the two #define's below be changed.
 */

#define MAX_ENET_PORTS       2
#define MAX_ENET_PORT_NUM    1

#define PHY_ADDR_ENET_PORT0  4
#define PHY_ADDR_ENET_PORT1  5
#define PHY_ADDR_ENET_PORT2  0    /* Not Used */

/*
 * To ensure cache/memory coherency, ethernet descriptors and buffers
 * are placed into a specific area of non-cacheable low memory.
 * This area is between the low memory vector area and the start
 * of available RAM. This is a work around for MV64260 errata
 * describing problems with ethernet DMA and cacheable coherent memory.
 */

#define ETHER_PORT_MEM_SIZE     0x20000      /* 128 K per port */
#define ETHER_TOTAL_MEM_SIZE    (ETHER_PORT_MEM_SIZE * MAX_ENET_PORTS)
#define ETHER_MEM_START_ADDR    (RAM_LOW_ADRS - ETHER_TOTAL_MEM_SIZE)

/* MV64260 DMA Driver Macro Definitions */

#ifdef INCLUDE_MV64260_DMA
#   define DMA_STATUS IDMA_STATUS   /* Structure used to return DMA status */

#   define DMA_INIT() sysMv64260DmaInit ()
#   define DMA_START(chan, desc, attrib, stat)                   \
           sysMv64260DmaStart ((UINT32)(chan),                   \
                               (IDMA_DESCRIPTOR *)(desc),        \
                               (IDMA_ATTRIBUTES *)(attrib),      \
                               (DMA_STATUS *)(stat))
#   define DMA_STATUS_GET(chan, stat)                            \
            sysMv64260DmaStatus ((UINT32)(chan), (DMA_STATUS *)(stat))
#   define DMA_ABORT(chan) sysMv64260DmaAbort ((UINT32)(chan))
#   define DMA_PAUSE(chan) sysMv64260DmaPause ((UINT32)(chan))
#   define DMA_RESUME(chan) sysMv64260DmaResume ((UINT32)(chan))

#   define DMA_MAX_CHANS        IDMA_CHAN_CNT   /* Max available channels */
#endif /* INCLUDE_MV64260_DMA */

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* __INCmv5500Ah */
