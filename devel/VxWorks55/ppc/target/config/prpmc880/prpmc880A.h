/* prpmc880A.h - Motorola PRPMC880 board header file */

/* Copyright 1984-2000 Wind River Systems, Inc. */
/* Copyright 1999-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,02Jun03,simon  updated based on peer review results
01a,31mar03,simon  Ported. (from ver 01f, mcpm905/mcpm905A.h)
*/

/*
This file contains I/O addresses and related constants for the
Motorola PRPMC880 board.  This file must not contain any "dependencies"
(ie. #ifdef's or #ifndef's).
*/

#ifndef	__INCprpmc880Ah
#define	__INCprpmc880Ah

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

/* Boot Line parameters are stored in the 2nd 256 byte block */

#define NV_RAM_SIZE    		USR_SROM_SIZE	
#define NV_RAM_ADRS    		USR_BRD_EEPROM_I2C_ADDR
#define NV_RAM_INTRVL           1

/* User configuration SROM */

#define USR_SROM_ADRS	USR_BRD_EEPROM_I2C_ADDR
#define USR_SROM_SIZE   256

#define NV_RAM_READ(x)		sysNvRead (x)
#define NV_RAM_WRITE(x,y)	sysNvWrite (x,y)

/* Real-time clock and Alarm clock Support */

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
#define SYS_BUS_CLK_RATE BUS_RATE_133
#define CPU_BUS_SPD		(sysCpuBusSpd())
#define DEC_CLOCK_FREQ          CPU_BUS_SPD

/* AuxClock Support */

#define AUX_CLK_RATE_MIN        40               /* min auxiliary clock */
#define AUX_CLK_RATE_MAX        5000             /* max auxiliary clock rate */

/*
 * This macro returns the positive difference between two signed ints.
 * Useful for determining delta between two successive decrementer reads.
 */

#define DELTA(a,b)		( abs((int)a - (int)b) )

/* Translation macro */

#define TRANSLATE(x,y,z)\
        ((UINT)(x) - (UINT)(y) + (UINT)(z))

/* System Memory (DRAM) Support */

#define MAX_DRAM	0x40000000

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

/* Boot FLASH Support */
/* Sergey: following line was modified */
#define ROM_BASE_ADRS	0xf5f00000      /* base address of ROM */
/*Sergey: #define ROM_BASE_ADRS	0xfff00000*/      /* base address of ROM */
#define ROM_SIZE	0x00100000      /* 1 MB ROM space */
#define BANKA_FLASH_BASE_ADRS MV64360_DEVCS0_BASE_ADDR /* base address of BANK A, devcs0 */
#define BANKA_SIZE  MV64360_DEVCS0_SIZE
#define BANKB_FLASH_BASE_ADRS MV64360_DEVCS1_BASE_ADDR /* base address of BANK A, devcs0 */
#define BANKB_SIZE  MV64360_DEVCS1_SIZE

/* System Control & Status Register Support */

#define SYSTEM_REG_BASE	 MV64360_DEVCS2_BASE_ADDR   /* base of system regs */
#define SYSTEM_REG_SIZE MV64360_DEVCS2_SIZE

/*
 * System Control & Status Register 1
 *
 * Bit 0 = 0  led0 control   Read/write.
 *                0 - on.
 *                1 - off.
 *      1 = 0  led1 control   Read/write.
 *                0 - on.
 *                1 - off.
 *      2 = 0  Reserved. 
 *      3 = 0  Reserved. 
 *      4 = 0  Reserved. 
 *      5 = 0  Reserved. 
 *      6 = 0  Reserved. 
 *      7 = 0  Reserved. 
 */

#define SYSTEM_STATUS_REG_1	(SYSTEM_REG_BASE + 0x05)
#define SYSTEM_STATUS_LED0_CTRL_BIT	0x01	
#define SYSTEM_STATUS_LED1_CTRL_BIT	0x02	

/*
 * System Control & Status Register 2
 *
 * Bit 0 = 0  I2C serial data out  Read/write.
 *      1 = 0  I2C I2C clock line  Read/write.
 *      2 = 0  I2C DIR  Read/write.
 *                0 - input.
 *                1 - output.
 *      3 = 0  I2C serial data input  Read.
 *      4 = 0  Reserved. 
 *      5 = 0  Reserved. 
 *      6 = 0  Reserved. 
 *      7 = 0  Reserved. 
 */

#define SYSTEM_STATUS_REG_2	(SYSTEM_REG_BASE + 0x6) 
#define SYSTEM_STATUS_REG_2_I2CSDAO_BIT		0x01
#define SYSTEM_STATUS_REG_2_I2CSCL_BIT		0x02
#define SYSTEM_STATUS_REG_2_I2CSDIR_BIT		0x04
#define SYSTEM_STATUS_REG_2_I2CSDAI_BIT		0x08

/*
 * System Control & Status Register 3
 *
 * Bit 0 = 0  Eready output.  Read.
 *      1 = 0  Eready input.  Read/write.
 *      2 = 0  Reserved. 
 *      3 = 0  Reserved. 
 *      4 = 0  Reserved. 
 *      5 = 0  Reserved. 
 *      6 = 0  Reserved. 
 *      7 = 0  Reserved. 
 */

#define SYSTEM_STATUS_REG_3     (SYSTEM_REG_BASE + 0x7)
#define SYSTEM_STATUS_REG_3_EREADYI_BIT		0x01
#define SYSTEM_STATUS_REG_3_EREADYO_BIT		0x02

/*
 * System Control & Status Register 4
 *
 * Bit 0 = 0  ge phy0 quality.  Read.
 *      1 = 0  ge phy0 link1.  Read.
 *      2 = 0  ge phy0 link2.  Read.
 *      3 = 0  ge phy1 quality.  Read.
 *      1 = 0  ge phy1 link1.  Read.
 *      2 = 0  ge phy1 link2.  Read.
 *      6 = 0  board power low. 
 *      7 = 0  pci monarch.    0 monarch 1 non-monarch
 */

#define SYSTEM_STATUS_REG_4     (SYSTEM_REG_BASE + 0x8)
#define SYSTEM_STATUS_REG_4_PHY0_QUALITY_L_BIT		0x01
#define SYSTEM_STATUS_REG_4_PHY0_LINK1_L_BIT		0x02
#define SYSTEM_STATUS_REG_4_PHY0_LINK2_L_BIT		0x04
#define SYSTEM_STATUS_REG_4_PHY1_QUALITY_L_BIT		0x08
#define SYSTEM_STATUS_REG_4_PHY1_LINK1_L_BIT		0x10
#define SYSTEM_STATUS_REG_4_PHY1_LINK2_L_BIT		0x20
#define SYSTEM_STATUS_REG_4_FAULT_PWR_BIT		       0x40
#define SYSTEM_STATUS_REG_4_PHY0_MONARCH_L_BIT		0x80

/* MV64360 - Discovery II Support */

#define PRPMC880_MAX_BUS		1		/* Max # PCI buses on MV64360 */
#define MV64360_REG_BASE	0xF1000000	/* Base of MV64360 Reg Space */
#define MV64360_REG_SPACE_SIZE	0x10000         /* 64Kb Internal Reg Space */

/* PCI and PCI Auto-Configuration Support */

#define INCLUDE_PCI		/* always include pci */
#define INCLUDE_PCI_AUTOCONF	/* always include pci auto config*/

#define PCI_CLINE_SZ	(_CACHE_ALIGN_SIZE/4)	/* Cache Ln Size = 32 bytes */
#define PCI_LAT_TIMER	0xff	/* Latency Timer Value = 255 PCI clocks */
#define PCI_CONFIG_ADDR_BASE_0	(MV64360_REG_BASE + PCIIF_CFG_ADDR_PCI0)
#define PCI_CONFIG_DATA_BASE_0	(MV64360_REG_BASE + PCIIF_CFG_DATA_PCI0)

/* PCI/ function identifiers for configuration access prior to pciAutoConfig */

#define PCI0_BUS	0
#define PCI1_BUS	1
#define PCI_FCN0	0
#define PCI_FCN1	1
#define PCI_FCN2	2
#define PCI_FCN3	3
#define PCI_FCN4	4

/* 
 * PCI Processor (LOCAL) and Bus (BUS) Definitions
 *
 * The following definitions determine the address ranges for PCI Memory
 * and I/O space on both the processor bus and the PCI memory and
 * I/O busses. These definitions are used to configure the PCI-Host
 * Bridge as well as perform PCI Auto-Configuration.  There are one sets
 * of definitions for the MV64360's buss 0.0(bus 1.0 is useless in prpmc880 board). 
 *
 * PCI Memory Space:
 * This is a one-to-one mapping.  The address used on the processor bus is the
 * address used on the PCI memory bus.
 *
 * PCI0_MSTR_MEMIO_LOCAL =  PCI Non-Prefetchable Memory Space 
 *                          (processor bus view).
 * PCI0_MSTR_MEMIO_BUS =    PCI Non-Prefetchable Memory Space 
 *                          (PCI memory bus view).
 *
 * PCI0_MSTR_MEM_LOCAL  =   PCI Prefetchable Memory Space
 *                          (processor bus view).
 * PCI0_MSTR_MEM_BUS  =     PCI Prefetchable Memory Space
 *                          (PCI memory bus view).
 *
 * PCI I/O Space:
 * This is not a one-to-one mapping.  For Bus 0.0 the I/O space will start
 * at 0x00000000 on the PCI I/O bus. 
 *
 * ISA_MSTR_IO_LOCAL =   PCI 16-bit I/O Space (processor bus view) 
 *
 * ISA_MSTR_IO_BUS =     PCI 16-bit I/O Space (PCI I/O bus view) 
 *
 * PCI0_MSTR_IO_LOCAL =  PCI 32-bit I/O Space (processor bus view)
 *
 * PCI0_MSTR_IO_BUS =    PCI 32-bit I/O Space (PCI I/O bus view)
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
   
#define PCI_MEM_LOCAL_START	PCI0_MEM_LOCAL_START	/* Bottom of PCI memory space */

/*
 * Bus 0.0 (PCI0) PCI Memory Definitions -
 * First handle 0.0 (PCI0) memio (nonprefetchable) window 
 */

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
 * Local address which maps to PCI address zero, so PrPMC can "see"
 * DRAM on CPU card on which it is mounted.  
 * Note: PCI_MSTR_MEM_LOCAL + PCI_MSTR_MEM_SIZE must be <= ISA_MSTR_IO_LOCAL
 */



/* Sergey: PCI0_MSTR_ZERO_LOCAL and PCI0_MSTR_ZERO_SIZE will be used:

1. In sysLib.c in sysPhysMemDesc[] which in its turn will be used in
( ??? usrMmuInit() called from root task usrRoot() (file usrConfig.c) ??? )

2. In sysMv64360Phb.c as outbound translation address space to set CPU->PCI
window (maps local PCI address 0)

   It is very important to have the same parameters passed to those 2 places !
*/



#ifdef MVME5500
#define PCI0_MSTR_ZERO_LOCAL 0xc0000000
/*#define PCI0_MSTR_ZERO_LOCAL (PCI0_MSTR_MEM_LOCAL + PCI0_MSTR_MEM_SIZE)*/
#else
/*                              0x84000000               0x04000000 */
#define PCI0_MSTR_ZERO_LOCAL (PCI0_MSTR_MEM_LOCAL + PCI0_MSTR_MEM_SIZE)
#endif			    
			    
/*
 * PCI I/O Definitions -
 * The PCI I/O space consists of two separate windows.  The first
 * window  consists of  ISA I/O (16-bit I/O) space .  
 * Immediately following this window comes the I/O spaces space window
 * for PCI bus 0.0 (PCI0).  As with PCI memory windows, the two size and base
 * address alignment rules (see preceding comment) are enforced.
 */

#define PCI_IO_LOCAL_START	(ISA_MSTR_IO_START)

#define	ISA_MSTR_IO_LOCAL	(PCI_IO_LOCAL_START)
#define	ISA_MSTR_IO_BUS		(0x00000000)

/* Now for the PCI bus 0.0 (PCI0) I/O window definitions */

#define PCI0_IO_LOCAL_START	(PCI_IO_LOCAL_START + 0x00800000)

#define PCI0_MSTR_IO_LOCAL	(PCI0_IO_LOCAL_START)

#define PCI0_MSTR_IO_BUS	(TRANSLATE(PCI0_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_LOCAL,\
				 ISA_MSTR_IO_BUS))

#define PCI0_IO_LOCAL_END	(PCI0_MSTR_IO_LOCAL + PCI0_MSTR_IO_SIZE)

#define PCI_IO_LOCAL_END	PCI0_IO_LOCAL_END

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

/* memory seen from PCI bus */

#define PCI_SLV_MEM_LOCAL   LOCAL_MEM_LOCAL_ADRS
#define PCI_SLV_MEM_BUS  PCI0_MEM0_BAR_BASE
#define PCI_SLV_PMEP_BUS PCI0_IREG_BAR_BASE
#define PCI_SLV_MEM_SIZE    DRAM_SIZE	/* DRAM size PCI bus = size CPU bus */

/*
 * The following #define is set to indicate that the drawbridge has dynamic
 * downstream translation (translation that cannot be computed until run time).
 * Setting this #define will disable the compile-time downstream 
 * cPCI window size/translation compatibility check.
 */

#define CPCI_DOWNSTREAM_DYNAMIC_TRANSLATION 

/* PCI Device/Vendor IDs */

#define PCI_ID_MV64360	    0x646011AB	    /* Id for MV64360 PHB */
#define PCI_ID_21555        0xB5558086      /* Id for 21555 PCI-PCI Bridge */

/* BSP configuration error policy */

#define CONTINUE_EXECUTION	0       /* Tolerate VPD/Configuration errors */
#define EXIT_TO_SYSTEM_MONITOR	1       /* Transfer to System Monitor */

/* MV64360 I2C Interface Support & VPD Support */

#define SPD_EEPROM_ADDR_0       0xA0    /* I2C address for banks 0  */
#define VPD_BRD_EEPROM_I2C_ADDR           0xA8  /* I2C address of VPD EEPROM */
#define USR_BRD_EEPROM_I2C_ADDR           0xAA  /* I2C address of User EEPROM */

/* MAX6900 I2C Interface Support  */

#define MAX6900__RTC_ADDR       0xA0    

/* defined init EEPROM */

#define VALID_I2C_WRT_ADDR      USR_BRD_EEPROM_I2C_ADDR, \
                                0xFFFFFFFF
#define I2C_SINGLE_BYTE_ADDR    1       /* one-byte address devices */
#define I2C_DUAL_BYTE_ADDR      2       /* two-byte address devices */
#define I2C_TRIPLE_BYTE_ADDR    3       /* three-byte address devices */
#define SPD_BYTE_ADDR_SZ                I2C_SINGLE_BYTE_ADDR
#define VPD_BYTE_ADDR_SZ                I2C_DUAL_BYTE_ADDR
#define USR_BYTE_ADDR_SZ                I2C_DUAL_BYTE_ADDR
#define DEFAULT_PRODUCT_ID      "Unknown"
#define DEFAULT_CPU_ID		"Unknown"
#define VPD_BRD_OFFSET      0       /* offset into brd's eeprom for vpd data */
#define VPD_PKT_LIMIT       25      /* Max number of packets expected */
#define VPD_EEPROM_SIZE     512     /* size of VPD serial EEPROM */
#define I2C_SINGLE_ADDRESS  1       /* num addr bytes to write for I2C access */
#define I2C_DUAL_ADDRESS    2       /* num addr bytes to write for I2C access */
#define I2C_ADDRESS_BYTES   I2C_DUAL_ADDRESS /* VPD and user ROM accesses */

/*
 * Address range definitions for PCI bus.
 *
 * Used with vxMemProbe() hook sysBusProbe().
 * The following macros are used to determine whether a given address
 * is located.
 */

#define IS_BUS_0_PCI_ADDRESS(adrs) ((((UINT32)(adrs) >= \
                                      (UINT32)PCI0_MSTR_MEMIO_LOCAL) && \
                                     ((UINT32)(adrs) < \
                                      (UINT32)(PCI0_MEMIO_LOCAL_END))) || \
				    (((UINT32)(adrs) >= \
				      (UINT32)(PCI0_MSTR_MEM_LOCAL) && \
				      (UINT32)(adrs) < \
                                      (UINT32)(PCI0_MEM_LOCAL_END))) || \
                                    (((UINT32)(adrs) >= \
                                      (UINT32)PCI0_MSTR_IO_LOCAL) && \
                                     ((UINT32)(adrs) < \
                                      (UINT32)(PCI0_IO_LOCAL_END))))

#define IS_PCI_ADDRESS(adrs)	   (IS_BUS_0_PCI_ADDRESS(adrs) )

/*
 * Support for determining if we're ROM based or not.  _sysInit
 * saves the startType parameter at location ROM_BASED_FLAG.
 */

#define PCI_AUTOCONFIG_FLAG_OFFSET (0x4c00)
#define PCI_AUTOCONFIG_FLAG        (*(UCHAR *)(LOCAL_MEM_LOCAL_ADRS + \
				     PCI_AUTOCONFIG_FLAG_OFFSET))
#define PCI_AUTOCONFIG_DONE        (PCI_AUTOCONFIG_FLAG != 0)

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

/* 
 * In addition to the interrupts defined above (associated with the main
 * interrupt cause low and high registers), we also have some external
 * interrupt sources which are associated with MPP (Multi-Purpose Pins)
 * which are in turn programmed as GPP (General Purpose Port) pins.  On
 * the PRPMC880, any GPP related interrupt causes one of the 4 bits (24,
 * 25, 26, or 27) in the "main interrupt cause high" register to be set
 * as well as the associated "local" bit in the GPP value register.
 * Here's the selection information for these GPP pins.
 */

#define MPP_SELECT_GPP		(0x0)
#define MPP_SELECT_GNT		(0x1)
#define MPP_SELECT_REQ		(0x1)
#define MPP_SELECT_TXD		(0x2)
#define MPP_SELECT_RXD		(0x2)
#define MPP_SELECT_RTS		(0x2)
#define MPP_SELECT_CTS		(0x2)
#define MPP_INIT_ACT		(0x3)
#define MPP_WDE			(0x4)
#define MPP_WDNMI		(0x4)

#define GPP_INPUT		(0x0)
#define GPP_OUTPUT		(0x1)

#define GPP_ACTIVE_HI		(0x0)
#define GPP_ACTIVE_LOW		(0x1)

#define MPP_PIN0_SEL  (MPP_SELECT_TXD) /* UART TXD */
#define MPP_PIN1_SEL  (MPP_SELECT_RXD) /* UART RXD */
#define MPP_PIN2_SEL  (MPP_SELECT_RTS) /* UART RTS */
#define MPP_PIN3_SEL  (MPP_SELECT_CTS) /* UART CTS */
#define MPP_PIN4_SEL  (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN5_SEL  (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN6_SEL  (MPP_SELECT_GPP) /* MV64360 WDNMI# interrupt */
#define MPP_PIN7_SEL  (MPP_SELECT_GPP) /* BCM5421S PHY interrupts ORed */
#define MPP_PIN8_SEL  (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN9_SEL  (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN10_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN11_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN12_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN13_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN14_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN15_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN16_SEL (MPP_SELECT_GPP) /* PCI Bus INTA */
#define MPP_PIN17_SEL (MPP_SELECT_GPP) /* PCI Bus INTB */ 
#define MPP_PIN18_SEL (MPP_SELECT_GPP) /* PCI Bus INTC */
#define MPP_PIN19_SEL (MPP_SELECT_GPP) /* PCI Bus INTD */
#define MPP_PIN20_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN21_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN22_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN23_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN24_SEL (MPP_SELECT_GPP)   /* Unused */
#define MPP_PIN25_SEL (MPP_WDE)        /* WDE# output */
#define MPP_PIN26_SEL (MPP_WDNMI)      /* WDNMI# output */
#define MPP_PIN27_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN28_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN29_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN30_SEL (MPP_SELECT_GPP) /* Unused */
#define MPP_PIN31_SEL (MPP_SELECT_GPP) /* Unused */

#define GPP_IO0       (GPP_OUTPUT)   /* UART TXD */
#define GPP_IO1       (GPP_INPUT)   /* UART RXD */
#define GPP_IO2       (GPP_OUTPUT)   /* UART RTS */
#define GPP_IO3       (GPP_INPUT)   /* UART CTS */
#define GPP_IO4       (GPP_INPUT)   /* Unused */
#define GPP_IO5       (GPP_INPUT)   /* Unused */
#define GPP_IO6       (GPP_INPUT)   /* MV64360 WDNMI# interrupt */
#define GPP_IO7       (GPP_INPUT)   /* BCM5421S PHY interrupts ORed */
#define GPP_IO8       (GPP_INPUT)   /* Unused */
#define GPP_IO9       (GPP_INPUT)   /* Unused */
#define GPP_IO10      (GPP_INPUT)   /* Unused */
#define GPP_IO11      (GPP_INPUT)   /* Unused */
#define GPP_IO12      (GPP_INPUT)   /* Unused */
#define GPP_IO13      (GPP_INPUT)   /* Unused */
#define GPP_IO14      (GPP_INPUT)   /* Unused */
#define GPP_IO15      (GPP_INPUT)   /* Unused */
#define GPP_IO16      (GPP_INPUT)   /* PCI Bus INTA */	
#define GPP_IO17      (GPP_INPUT)   /* PCI Bus INTB */
#define GPP_IO18      (GPP_INPUT)   /* PCI Bus INTC */
#define GPP_IO19      (GPP_INPUT)   /* PCI Bus INTD */
#define GPP_IO20      (GPP_INPUT)   /* Unused */
#define GPP_IO21      (GPP_INPUT)   /* Unused */
#define GPP_IO22      (GPP_INPUT)   /* Unused */
#define GPP_IO23      (GPP_INPUT)   /* Unused */
#define GPP_IO24      (GPP_INPUT)  /* Unused */
#define GPP_IO25      (GPP_OUTPUT)  /* WDE# output */
#define GPP_IO26      (GPP_OUTPUT)  /* WDNMI# output */
#define GPP_IO27      (GPP_INPUT)   /* Unused */
#define GPP_IO28      (GPP_INPUT)   /* Unused */
#define GPP_IO29      (GPP_INPUT)   /* Unused */
#define GPP_IO30      (GPP_INPUT)   /* Unused */
#define GPP_IO31      (GPP_INPUT)   /* Unused */

#define GPP_LEVEL0  (GPP_ACTIVE_HI)	/* UART TXD */
#define GPP_LEVEL1  (GPP_ACTIVE_HI)	/* UART RXD */
#define GPP_LEVEL2  (GPP_ACTIVE_HI)	/* UART RTS */
#define GPP_LEVEL3  (GPP_ACTIVE_HI)	/* UART CTS */
#define GPP_LEVEL4  (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL5  (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL6  (GPP_ACTIVE_LOW)	/* MV64360 WDNMI# interrupt */
#define GPP_LEVEL7  (GPP_ACTIVE_LOW)	/* BCM5421S PHY interrupts ORed */
#define GPP_LEVEL8  (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL9  (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL10 (GPP_ACTIVE_HI)	/* Unused*/
#define GPP_LEVEL11 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL12 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL13 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL14 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL15 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL16 (GPP_ACTIVE_LOW)	/* PCI Bus INTA */	
#define GPP_LEVEL17 (GPP_ACTIVE_LOW)	/* PCI Bus INTB */
#define GPP_LEVEL18 (GPP_ACTIVE_LOW)	/* PCI Bus INTC */
#define GPP_LEVEL19 (GPP_ACTIVE_LOW)	/* PCI Bus INTD */
#define GPP_LEVEL20 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL21 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL22 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL23 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL24 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL25 (GPP_ACTIVE_HI)	/* WDE# output */
#define GPP_LEVEL26 (GPP_ACTIVE_HI)	/* WDNMI# output */
#define GPP_LEVEL27 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL28 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL29 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL30 (GPP_ACTIVE_HI)	/* Unused */
#define GPP_LEVEL31 (GPP_ACTIVE_HI)	/* Unused */

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
 * Interrupt Main Cause High bits associated with MPSC interrupts have
 * the following vectors assigned to them.
 */

#define MPSC0_INT_VEC         ICI_MICH_INT_NUM_8
#define MPSC1_INT_VEC         ICI_MICH_INT_NUM_9

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

/*
 * Assign some specific interrupt numbers  - first generic PCI
 * INTA thru INTD interrupts for both buses.
 */

#define PCI0_INTA	(GPP_IC_INT_NUM_16)
#define PCI0_INTB	(GPP_IC_INT_NUM_17)
#define PCI0_INTC	(GPP_IC_INT_NUM_18)
#define PCI0_INTD	(GPP_IC_INT_NUM_19)

/*
 * The following define causes the trigger offset to track the value of
 * SM_MAILBOX_INT. It set to the inbound doorbell.
 */

#define SM_MAILBOX_INT_OFFSET   ( MV64360_REG_BASE + MUIF_INBND_DOORBELL_PCI0 )
#define SM_MAILBOX_INT_BIT  0
#define SM_MAILBOX_INT_VEC 	ICI_MICL_INT_NUM_20
#define SM_MAILBOX_INT_MASK     0x02

/* interrupt vector definitions */

#define INT_VEC_IRQ0			ICI_MICL_INT_NUM_MIN	/* vector for IRQ0 */

/* Dec2155x (Drawbridge) related defines */

/*
 * The following must be a value between 0 and 15.  It represents the 
 * bit number of the primary doorbell register used to interrupt the 
 * MCP750 for shared memory bus interrupts. 
 */

#define DEC2155X_SM_DOORBELL_BIT 0

/* Drawbridge internal interrupt levels */

#define DEC2155X_INTERRUPT_BASE 0x60

#define DEC2155X_DOORBELL0_INT_LVL  ( 0x00 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL1_INT_LVL  ( 0x01 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL2_INT_LVL  ( 0x02 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL3_INT_LVL  ( 0x03 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL4_INT_LVL  ( 0x04 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL5_INT_LVL  ( 0x05 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL6_INT_LVL  ( 0x06 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL7_INT_LVL  ( 0x07 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL8_INT_LVL  ( 0x08 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL9_INT_LVL  ( 0x09 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL10_INT_LVL ( 0x0a + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL11_INT_LVL ( 0x0b + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL12_INT_LVL ( 0x0c + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL13_INT_LVL ( 0x0d + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL14_INT_LVL ( 0x0e + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_DOORBELL15_INT_LVL ( 0x0f + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_PWR_MGMT_INT_LVL   ( 0x10 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_I2O_INT_LVL		( 0x11 + DEC2155X_INTERRUPT_BASE )
#define DEC2155X_PG_CRSSNG_INT_LVL	( 0x12 + DEC2155X_INTERRUPT_BASE )

/* Drawbridge interrupt vectors */

#define DEC2155X_DOORBELL0_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL0_INT_LVL)
#define DEC2155X_DOORBELL1_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL1_INT_LVL)
#define DEC2155X_DOORBELL2_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL2_INT_LVL)
#define DEC2155X_DOORBELL3_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL3_INT_LVL)
#define DEC2155X_DOORBELL4_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL4_INT_LVL)
#define DEC2155X_DOORBELL5_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL5_INT_LVL)
#define DEC2155X_DOORBELL6_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL6_INT_LVL)
#define DEC2155X_DOORBELL7_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL7_INT_LVL)
#define DEC2155X_DOORBELL8_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL8_INT_LVL)
#define DEC2155X_DOORBELL9_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_DOORBELL9_INT_LVL)
#define DEC2155X_DOORBELL10_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL10_INT_LVL)
#define DEC2155X_DOORBELL11_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL11_INT_LVL)
#define DEC2155X_DOORBELL12_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL12_INT_LVL)
#define DEC2155X_DOORBELL13_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL13_INT_LVL)
#define DEC2155X_DOORBELL14_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL14_INT_LVL)
#define DEC2155X_DOORBELL15_INT_VEC (INT_VEC_IRQ0 + DEC2155X_DOORBELL15_INT_LVL)
#define DEC2155X_PWR_MGMT_INT_VEC   (INT_VEC_IRQ0 + DEC2155X_PWR_MGMT_INT_LVL)
#define DEC2155X_I2O_INT_VEC        (INT_VEC_IRQ0 + DEC2155X_I2O_INT_LVL)
#define DEC2155X_PG_CRSSNG_INT_VEC  (INT_VEC_IRQ0 + DEC2155X_PG_CRSSNG_INT_LVL)

#define DEC2155X_MAILBOX_INT_VEC (DEC2155X_DOORBELL0_INT_VEC + \
				  DEC2155X_SM_DOORBELL_BIT)

#ifdef INCLUDE_DEC2155X
#   define DEC2155X_BIST_VAL         0x00
#   define DEC2155X_PRI_PRG_IF_VAL   0x00
#   define DEC2155X_PRI_SUBCLASS_VAL 0x20
#   define DEC2155X_PRI_CLASS_VAL    0x0b
#   define DEC2155X_SEC_PRG_IF_VAL   0x00
#   define DEC2155X_SEC_SUBCLASS_VAL 0x80
#   define DEC2155X_SEC_CLASS_VAL    0x06
#   define DEC2155X_MAX_LAT_VAL      0x00
#   define DEC2155X_MIN_GNT_VAL      0xff

#   define DEC2155X_CHP_CTRL0_VAL 0x0000
#   define DEC2155X_CHP_CTRL1_VAL 0x0000

#   define DEC2155X_PRI_SERR_VAL (DEC2155X_SERR_DIS_DLYD_TRNS_MSTR_ABRT | \
			          DEC2155X_SERR_DIS_DLYD_RD_TRNS_TO     | \
			          DEC2155X_SERR_DIS_DLYD_WRT_TRNS_DISC  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_DATA_DISC  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_TRGT_ABRT  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_MSTR_ABRT  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_PAR_ERROR)

#   define DEC2155X_SEC_SERR_VAL (DEC2155X_SERR_DIS_DLYD_TRNS_MSTR_ABRT | \
			          DEC2155X_SERR_DIS_DLYD_RD_TRNS_TO     | \
			          DEC2155X_SERR_DIS_DLYD_WRT_TRNS_DISC  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_DATA_DISC  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_TRGT_ABRT  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_MSTR_ABRT  | \
			          DEC2155X_SERR_DIS_PSTD_WRT_PAR_ERROR)
#endif /* INCLUDE_DEC2155X */

/* 
 * Specific Mv64360 device support register programming values
 */

#define MV64360_DEVCS0_BASE_ADDR  0xf2000000 /* Device bank base */
#define MV64360_DEVCS0_SIZE 	  0x04000000 /* Device bank size */
#define MV64360_DEVCS1_BASE_ADDR  0xf1200000 /* Device bank base */
#define MV64360_DEVCS1_SIZE 	  0x00100000 /* Device bank size */
#define MV64360_DEVCS2_BASE_ADDR  0xf1100000 /* Device bank base */
#define MV64360_DEVCS2_SIZE 	  0x00100000 /* Device bank size */

/* Specific Mv64360 Bank parameter register values */

/*
 * Boot Bank parameters register value
 * DVCTL_DEVICE_BOOT_BANK_PARAMS  offset: 0x46C
 *
 * Name                Value    Bit positions
 *
 * TURNOFF      	0x6	2:0
 * ACC2FIRST    	0xf	6:3
 * ACC2NEXT     	0x3	10:7
 * ALE2WR       	0x3	13:11
 * WRLOW        	0x7	16:14
 * WRHIGH       	0x3	19:17
 * DEVWIDTH     	0x2	21:20
 * TURNOFFEXT   	0x1	22
 * ACC2FIRSTEXT 	0x0	23
 * ACC2NEXTEXT  	0x0	24
 * ALE2WREXT    	0x0	25
 * WRLOWEXT     	0x0	26
 * WRHIGHEXT    	0x0	27
 * BADRSKEW     	0x0	29:28
 * DPEN	       		0x0	30
 * RESERVED     	0x1	31
 */

#define DVCTL_DEVICE_BOOT_BANK_PARAMS_VAL		0x8067d9fe

/*
 * Bank	0 parameters register value
 * DVCTL_DEVICE_BANK0_PARAMS  offset: 0x45C
 *
 * Name                Value    Bit positions
 *
 * TURNOFF		0x6	2:0
 * ACC2FIRST		0xf	6:3
 * ACC2NEXT		0x3	10:7
 * ALE2WR		0x3	13:11
 * WRLOW		0x7	16:14
 * WRHIGH		0x3	19:17
 * DEVWIDTH		0x2	21:20
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

#define	DVCTL_DEVICE_BANK0_PARAMS_VAL			0x8067d9fe

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
 * DEVWIDTH		0x2	21:20
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

#define DVCTL_DEVICE_BANK1_PARAMS_VAL			0x8067d9fe

/*
 * Default (unused) bank parameters register value.
 *
 * Name                Value    Bit positions
 *
 * TURNOFF		0x7	2:0
 * ACC2FIRST		0xf	6:3
 * ACC2NEXT		0xf	10:7
 * ALE2WR		0x7	13:11
 * WRLOW		0x7	16:14
 * WRHIGH		0x7	19:17
 * DEVWIDTH		0x0	21:20
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

#define DVCTL_DEVICE_BANK_DEFAULT_PARAMS_VAL		0x8fcfffff 

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
 * ARB2       0x1     11:8             NULL acc.
 * ARB3       0x5     15:12            MPSC acc.
 * ARB4       0x6     19:16            IDMA acc.
 * ARB5       0x7     23:20            Enet acc.
 * ARB6       0x1     27:24            NULL req.
 * ARB7       0x1     31:28            NULL req.
 */

#define DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_LOW_VAL 	0x11765132

/*
 * Mv64360 cross bar arbiter register (high) values
 * DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_HI offset: 0x4CC 
 * 
 * Name       Value    Bit positions  Description
 *
 * ARB8       0x2      3:0             CPU acc.
 * ARB9       0x3      7:4             PCI0 acc.
 * ARB10      0x4     11:8             NULL acc.
 * ARB11      0x5     15:12            MPSC acc.
 * ARB12      0x6     19:16            IDMA acc.
 * ARB13      0x7     23:20            Enet acc.
 * ARB14      0x1     27:24            NULL req.
 * ARB15      0x1     31:28            NULL req.
 */

#define DVCTL_DEVICE_INTERFACE_CROSSBAR_CTRL_HI_VAL	 0x11765132

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
 * The board use the mv64360 internal MPSC0 to support one async serial
 * ports (COM0).  
 */

#define N_UART_CHANNELS		1		/* No. serial I/O channels */
#define N_SIO_CHANNELS		N_UART_CHANNELS /* No. serial I/O channels */
#undef  NUM_TTY
#define NUM_TTY         N_SIO_CHANNELS
#define UART_BAUD_RATE 9600

/*
 * Interrupt vector and interrupt level assigned to timer/counter 0.
 */

#define TMR_CNTR0_INT_VEC      (INUM_TO_IVEC(ICI_MICL_INT_NUM_8))
#define TMR_CNTR0_INT_LVL      (ICI_MICL_INT_NUM_8)

/* MV64360 DMA Driver Macro Definitions */

#ifdef INCLUDE_MV64360_DMA
#   define DMA_STATUS IDMA_STATUS   /* Structure used to return DMA status */
#   define DMA_INIT() sysMv64360DmaInit ()
#   define DMA_START(chan, desc, attrib, stat)                   \
           sysMv64360DmaStart ((UINT32)(chan),                   \
                               (IDMA_DESCRIPTOR *)(desc),        \
                               (IDMA_ATTRIBUTES *)(attrib),      \
                               (DMA_STATUS *)(stat))
#   define DMA_STATUS_GET(chan, stat)                            \
           sysMv64360DmaStatus ((UINT32)(chan), (DMA_STATUS *)(stat))
#   define DMA_ABORT(chan) sysMv64360DmaAbort ((UINT32)(chan))
#   define DMA_PAUSE(chan) sysMv64360DmaPause ((UINT32)(chan))
#   define DMA_RESUME(chan) sysMv64360DmaResume ((UINT32)(chan))
#   define DMA_SRC_BURST_LIMIT  32              /* Minimum Transfer in bytes */
#   define DMA_DEST_BURST_LIMIT 32              /* Minimum Transfer in bytes */
#   define DMA_MAX_CHANS        IDMA_CHANNEL_COUNT /* Max available channels */
#endif /* INCLUDE_MV64360_DMA */

/* Ethernet unit */

#define MAX_ENET_PORTS       2
#define MAX_ENET_PORT_NUM    1

#define PHY_ADDR_ENET_PORT0  1
#define PHY_ADDR_ENET_PORT1  2

/*
 * To ensure cache/memory coherency, ethernet descriptors and buffers
 * are placed into a specific area of non-cacheable low memory.
 * This area is between the low memory vector area and the start
 * of available RAM. This is a work around for MV64260/MV64360 errata
 * describing problems with ethernet DMA and cacheable coherent memory.
 */

#define ETHER_PORT_MEM_SIZE     0x40000  /* 256 KBytes per port */
#define ETHER_TOTAL_MEM_SIZE    (ETHER_PORT_MEM_SIZE * MAX_ENET_PORTS)
#define ETHER_MEM_START_ADDR    (RAM_LOW_ADRS - ETHER_TOTAL_MEM_SIZE)

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* __INCprpmc880Ah */
