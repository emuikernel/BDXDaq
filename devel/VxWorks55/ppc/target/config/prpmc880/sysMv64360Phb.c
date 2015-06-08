/* sysMv64360Phb.c - Initialize the Mv64360 Host Bridge functionality */

/* Copyright 2003 Motorola, Inc., All Rights Reserved */

#include "copyright_wrs.h"

/*
modification history
--------------------
01a,23may03,simon  Ported (from version 01a of mcp905/sysMv64360Phb.c).
*/

/*
DESCRIPTION

This module initializes the Mv64360 host bridge support registers.  There
are over 340 of these registers.  The data path that are enabled via code
in this file are the CPU to PCI and PCI to CSx (chip select - DRAM) paths. 
*/

/* includes */

#include "mv64360.h"
#include "drv/pci/pciConfigLib.h"

/* defines */

#define PCI_SIZE_ENCODE(x) ((x - 1) & 0xFFFFF000)

/* typedefs */

/* globals */

/* locals */

/*
 * This table contains static initialization values for the MV64360
 * internal host bridge support registers.  
 */

LOCAL UINT32 mv64360PhbTbl [] =
{


  /*
   * 19:0 = upper 20 bits of 36 bit address.
   * 25:24 = 01 to indicate no byte swapping.
   * All other bits reserved and set to 0.
   */

  YES_INIT_MV64360_REG (CPUIF_PCI0_IO_BASE_ADDR,
			((PCI_IO_LOCAL_START >> 16) | 0x01000000))

  /* 15:0 = encoded size, other bits reserved = 0 */

  YES_INIT_MV64360_REG (CPUIF_PCI0_IO_SIZE,
			  (MV64360_SIZE_FIELD((PCI0_IO_LOCAL_END -
					       PCI_IO_LOCAL_START))))

  /* 15:0 = high order 16 bits of remap value */

  YES_INIT_MV64360_REG (CPUIF_PCI0_IO_ADDR_REMAP,
			  (ISA_MSTR_IO_BUS) >> 16)


    /*
     * 19:0 = upper 20 bits of 36 bit address.
     * 25:24 = 01 to indicate no byte swapping.
     * 26: = 0 Default - do not assert no snoop attribute.
     * 27: = 0 Assert REQ64# only when transaction longer than 64 bits.
     * All other bits reserved and set to 0.
     */

    YES_INIT_MV64360_REG (CPUIF_PCI0_MEM0_BASE_ADDR,
			  ((PCI0_MSTR_MEMIO_LOCAL >> 16) | 0x01000000))
			  
    /* 15:0 = encoded size, other bits reserved = 0 */

    YES_INIT_MV64360_REG (CPUIF_PCI0_MEM0_SIZE,
			  (MV64360_SIZE_FIELD((PCI0_MEMIO_LOCAL_END -
					       PCI0_MSTR_MEMIO_LOCAL))))
  
    /* High remap register not used. */

    YES_INIT_MV64360_REG (CPUIF_PCI0_MEM0_ADDR_REMAP_HI, 0)

    /*
     * 15:0 remap value for upper 16 bits of address.  Remapping
     * does not change the address value (null remap).
     */

    YES_INIT_MV64360_REG (CPUIF_PCI0_MEM0_ADDR_REMAP_LOW,
                          (PCI0_MSTR_MEMIO_BUS >> 16))




    /*
     * Now program the PCI0_MEM1 registers with the MEM (prefetchable)
     * information.
     */


  /* ?? Sergey: map our own memory to PCI ?? (0x84000000) */

  YES_INIT_MV64360_REG (CPUIF_PCI0_MEM1_BASE_ADDR, 
			  ((PCI0_MSTR_MEM_LOCAL >> 16) | 0x01000000))



  YES_INIT_MV64360_REG (CPUIF_PCI0_MEM1_SIZE, 
			  (MV64360_SIZE_FIELD((PCI0_MEM_LOCAL_END -
					       PCI0_MSTR_MEM_LOCAL))))

  YES_INIT_MV64360_REG (CPUIF_PCI0_MEM1_ADDR_REMAP_HI, 0)

  YES_INIT_MV64360_REG (CPUIF_PCI0_MEM1_ADDR_REMAP_LOW, 
                          (PCI0_MSTR_MEM_BUS >> 16))



  /* ?? Sergey: map master's memory (suppose to be at PCI address 0) ??
   (0xc0000000) */

  YES_INIT_MV64360_REG (CPUIF_PCI0_MEM2_BASE_ADDR, 
			  ((PCI0_MSTR_ZERO_LOCAL >> 16) | 0x01000000))
  
  YES_INIT_MV64360_REG (CPUIF_PCI0_MEM2_SIZE, 
			  (MV64360_SIZE_FIELD(PCI0_MSTR_ZERO_SIZE)))

  YES_INIT_MV64360_REG (CPUIF_PCI0_MEM2_ADDR_REMAP_HI, 0)

  YES_INIT_MV64360_REG (CPUIF_PCI0_MEM2_ADDR_REMAP_LOW, (0 >> 16))




    /*
     * The function of the PCI0_MEM 3 based registers will be disabled 
     * via the base address enable register programming 
     * (CPUIF_BASE_ADDR_ENABLE).  Nevertheless we will program these 
     * registers with specific values.
     */

    YES_INIT_MV64360_REG (CPUIF_PCI0_MEM3_BASE_ADDR, 0)
  
    YES_INIT_MV64360_REG (CPUIF_PCI0_MEM3_SIZE, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI0_MEM3_ADDR_REMAP_HI, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI0_MEM3_ADDR_REMAP_LOW, 0)

    /*
     * We don't deal with CPU to PCI1 so we can program
     * the following registers to zero.
     */

    YES_INIT_MV64360_REG (CPUIF_PCI1_IO_BASE_ADDR, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_IO_SIZE, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_IO_ADDR_REMAP, 0)


    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM0_BASE_ADDR, 0)


    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM0_SIZE,0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM0_ADDR_REMAP_HI, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM0_ADDR_REMAP_LOW, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM1_BASE_ADDR, 0)
  
    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM1_SIZE, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM1_ADDR_REMAP_HI, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM1_ADDR_REMAP_LOW, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM2_BASE_ADDR, 0)
  
    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM2_SIZE, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM2_ADDR_REMAP_HI, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM2_ADDR_REMAP_LOW, 0)


    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM3_BASE_ADDR, 0)
  
    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM3_SIZE, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM3_ADDR_REMAP_HI, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_MEM3_ADDR_REMAP_LOW, 0)

    /*
     * Headers retarget control registers are not being used.  The
     * retarget function will be disabled via bit 0 of the "retarget
     * control register" so the programming of "regarget base" registers
     * is moot.  Nevertheless in the interest of putting deterministic
     * values into all registers we will program control registers to
     * zero (thus disabling the base registers) and we will program
     * the base registers to zero as well.
     */

    YES_INIT_MV64360_REG (CPUIF_PCI0_HDRS_RETARG_CTRL, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI0_HDRS_RETARG_BASE, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_HDRS_RETARG_CTRL, 0)

    YES_INIT_MV64360_REG (CPUIF_PCI1_HDRS_RETARG_BASE, 0)

    /*
     * CPU Configuration Register
     *
     *  Bit 7:0 = 11111111  CPU Address Miss Counter
     *        8 = 0         CPU Address Miss Counter Disabled
     *        9 = 0         CPU Address Miss Counter MSB
     *       10 = 0         Reserved
     *       11 = 0         Dual CPU (init value)
     *       12 = 0         CPU Bus Byte Orientation (must be 0)
     *       13 = 1         Pipeline enabled
     *    16:14 = 000       Reserved
     *       17 = 0         keep PCI transactions retry
     *       18 = 0         Normal Address Decoding
     *       19 = 1         CPU DP[0-7] connected
     *    21:20 = 00        Reserved
     *       22 = 0         Parity Error Propagation
     *    24:23 = 01        Reserved
     *       25 = 1         AACK Earliest Assertion 
     *       26 = 1         CPU AP[0-3] Connected
     *       27 = 0         Address Remap Registers Write Control
     *    31:28 = 0000      Reserved
     */

    YES_INIT_MV64360_REG (CPUIF_CPU_CFG, ((0xff << 0) | (0 << 8) | 
			  (0 << 9) | (0 << 10) | (0 << 11) | (0 << 12) |
			  (1 << 13) | (0 << 14) | (0 << 17) | (0 << 18) |
			  (1 << 19) | (0 << 20) | (0 << 22) | (1 << 23) |
			  (1 << 25) | (1 << 26) | (0 << 27) | (0 << 28)))

    /*
     * CPU Master Control Register
     *
     *  Bit  7:0 = 00000000  Reserved
     *         8 = 1         CPU Bus Internal Arbiter enabled
     *         9 = 1         Mask CPU1 Bus Request
     *        10 = 0         Master Write Transaction Trigger
     *        11 = 1         Master Read Response Trigger
     *        12 = 1         Clean Block Snoop Transaction Support
     *        13 = 1         Flush Block Snoop Transaction Support
     *     31:14 = 0         Reserved
     */
 
    YES_INIT_MV64360_REG (CPUIF_CPU_MSTR_CTRL, ((0 << 0) | (1 << 8) |
			  (1 << 9) | (0 << 10) | (1 << 11) | (1 << 12) |
			  (1 << 13) | (0 << 14)))
			  
     /*
      * Base address enable: is dynamically programmed because we need
      * to preserve the programming of some of the bits and change
      * the programming of others.
      */

    NOT_INIT_MV64360_REG (CPUIF_BASE_ADDR_ENABLE, dynamic )

    /*
     * The following BOOTCS, CPU and CSx interface registers are 
     * disabled via the programming in: PCIIF_BASE_ADDR_ENABLE_PCIx.  
     * We will still program them to a known value of zero.
     */

    YES_CNFG_MV64360_REG (PCI_FCN3, PCIIF_BOOTCS_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN3, PCIIF_BOOTCS_BASE_ADDR_LOW_PCICFG, 0)

    YES_INIT_MV64360_REG (PCIIF_BOOTCS_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_BOOTCS_BASE_ADDR_REMAP_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_BOOTCS_BAR_SIZE_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_BOOTCS_BASE_ADDR_REMAP_PCI1, 0)

    YES_INIT_MV64360_REG (PCIIF_CPU_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_CPU_BAR_SIZE_PCI1, 0)

    /*
     * We don't deal with 64-bit CPU addresses so we can program
     * the following registers to zero.
     */

    YES_CNFG_MV64360_REG (PCI_FCN0, PCIIF_CS0_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN0, PCIIF_CS1_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN1, PCIIF_CS2_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN1, PCIIF_CS3_BASE_ADDR_HI_PCICFG, 0)

    /*
     * The following registers have to do with PCI to CSx mapping
     * (address low, bar size, and remap).  we will not program cs0 because it
     * may program by pci master outside, but we will program cs1-cs3 to
     * 0 here because these is diable in Prpmc880.
     */

    NOT_CNFG_MV64360_REG (PCI_FCN0, PCIIF_CS0_BASE_ADDR_LOW_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN0, PCIIF_CS1_BASE_ADDR_LOW_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN1, PCIIF_CS2_BASE_ADDR_LOW_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN1, PCIIF_CS3_BASE_ADDR_LOW_PCICFG, 0)

    YES_INIT_MV64360_REG (PCIIF_CS0_BAR_SIZE_PCI0, 
    				PCI_SIZE_ENCODE (PCI2DRAM_MAP_SIZE))
    YES_INIT_MV64360_REG (PCIIF_CS1_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_CS2_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_CS3_BAR_SIZE_PCI0, 0)

    YES_INIT_MV64360_REG (PCIIF_CS0_BAR_SIZE_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_CS1_BAR_SIZE_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_CS2_BAR_SIZE_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_CS3_BAR_SIZE_PCI1, 0)

    YES_INIT_MV64360_REG (PCIIF_CS0_BASE_ADDR_REMAP_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_CS1_BASE_ADDR_REMAP_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_CS2_BASE_ADDR_REMAP_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_CS3_BASE_ADDR_REMAP_PCI0, 0)

    YES_INIT_MV64360_REG (PCIIF_CS0_BASE_ADDR_REMAP_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_CS1_BASE_ADDR_REMAP_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_CS2_BASE_ADDR_REMAP_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_CS3_BASE_ADDR_REMAP_PCI1, 0)

    /* 
     * PCI to DEVice bus is disabled via programming in
     * PCIIF_BASE_ADDR_ENABLE_PCIx so the programming of the
     * PCI to DEVice bus associated interface registers is moot.  
     * Nevertheless, we will still program these interface 
     * registers with known values - in this case zeros.
     */

    YES_CNFG_MV64360_REG (PCI_FCN2, PCIIF_DEVCS0_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN2, PCIIF_DEVCS0_BASE_ADDR_LOW_PCICFG, 0)
    YES_INIT_MV64360_REG (PCIIF_DEVCS0_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_DEVCS0_BASE_ADDR_REMAP_PCI0, 0)

    YES_CNFG_MV64360_REG (PCI_FCN2, PCIIF_DEVCS1_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN2, PCIIF_DEVCS1_BASE_ADDR_LOW_PCICFG, 0)
    YES_INIT_MV64360_REG (PCIIF_DEVCS1_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_DEVCS1_BASE_ADDR_REMAP_PCI0, 0)

    YES_CNFG_MV64360_REG (PCI_FCN2, PCIIF_DEVCS2_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN2, PCIIF_DEVCS2_BASE_ADDR_LOW_PCICFG, 0)
    YES_INIT_MV64360_REG (PCIIF_DEVCS2_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_DEVCS2_BASE_ADDR_REMAP_PCI0, 0)

    YES_CNFG_MV64360_REG (PCI_FCN2, PCIIF_DEVCS3_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN2, PCIIF_DEVCS3_BASE_ADDR_LOW_PCICFG, 0)
    YES_INIT_MV64360_REG (PCIIF_DEVCS3_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_DEVCS3_BASE_ADDR_REMAP_PCI0, 0)
			  
    /* 
     * PCI to PCI bus is disabled via programming in
     * PCIIF_BASE_ADDR_ENABLE_PCIx so the programming of the
     * PCI to PCI bus associated interface registers is moot.  
     * Nevertheless, we will still program these interface 
     * registers with known values - in this case zeros.
     */

    YES_CNFG_MV64360_REG (PCI_FCN4, PCIIF_P2P_IO_BASE_ADDR_PCICFG, 0)
    YES_INIT_MV64360_REG (PCIIF_P2P_IO_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_P2P_IO_BASE_ADDR_REMAP_PCI0, 0)

    YES_CNFG_MV64360_REG (PCI_FCN4, PCIIF_P2P_MEM0_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN4, PCIIF_P2P_MEM0_BASE_ADDR_LOW_PCICFG, 0)
    YES_INIT_MV64360_REG (PCIIF_P2P_MEM0_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_P2P_MEM0_BASE_ADDR_REMAP_HI_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_P2P_MEM0_BASE_ADDR_REMAP_LOW_PCI0, 0)

    YES_CNFG_MV64360_REG (PCI_FCN4, PCIIF_P2P_MEM1_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN4, PCIIF_P2P_MEM1_BASE_ADDR_LOW_PCICFG, 0)
    YES_INIT_MV64360_REG (PCIIF_P2P_MEM1_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_P2P_MEM1_BASE_ADDR_REMAP_HI_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_P2P_MEM1_BASE_ADDR_REMAP_LOW_PCI0, 0)

    /* 
     * More default programming for disabled interfaces, this one is the
     * PCI - SRAM interface.
     */

    YES_CNFG_MV64360_REG (PCI_FCN1, 
			  PCIIF_INTEGRATED_SRAM_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN1, 
			  PCIIF_INTEGRATED_SRAM_BASE_ADDR_LOW_PCICFG, 0)
    YES_INIT_MV64360_REG (PCIIF_INTEGRATED_SRAM_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_INTEGRATED_SRAM_BASE_ADDR_REMAP_PCI0, 0)

    /* Now for the default (disabled) expansion PCI - expansion ROM */

    YES_CNFG_MV64360_REG (PCI_FCN0, PCIIF_EXPANSION_ROM_BASE_ADDR_PCICFG, 0)
    YES_INIT_MV64360_REG (PCIIF_EXPANSION_ROM_BAR_SIZE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_EXPANSION_ROM_BASE_ADDR_REMAP_PCI0, 0)
     
    /* Disabled PCI - internal I/O reg programming. */

    YES_CNFG_MV64360_REG (PCI_FCN4, 
			  PCIIF_INTERNAL_IO_MAPPED_BASE_ADDR_PCICFG, 0)

    /*
     * Do not program the PCI Internal regs mem mapped base address BAR.
     * We will be programming this ourselves inside of sysHwInit() 
     * immediately after the PCI auto-configuration is complete.  We don't
     * want to wipe out the value that the bootrom placed into these registers
     * during pciAutoConfig().
     */
			  
    NOT_CNFG_MV64360_REG (PCI_FCN0, 
			  PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_LOW_PCICFG, 0)
			  
    NOT_CNFG_MV64360_REG (PCI_FCN0, 
			  PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_HI_PCICFG, 0)

    NOT_CNFG_MV64360_REG_ALT (PCI_FCN0,
			      PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_LOW_PCICFG, 0)

    NOT_CNFG_MV64360_REG_ALT (PCI_FCN0, 
			      PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_HI_PCICFG, 0)
			      
    /* Disabled PCI - CPU reg programming. */

    YES_CNFG_MV64360_REG (PCI_FCN3, PCIIF_CPU_BASE_ADDR_HI_PCICFG, 0)
    YES_CNFG_MV64360_REG (PCI_FCN3, PCIIF_CPU_BASE_ADDR_LOW_PCICFG, 0)
    YES_INIT_MV64360_REG (PCIIF_CPU_BASE_ADDR_REMAP_HI_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_CPU_BASE_ADDR_REMAP_LOW_PCI0, 0)

    /*
     * The PCI interface base enabling register is not programmed from
     * this table but rather it is dynamically programmed in 
     * sysMv64360PhbInit().  This is because we must preserve some of
     * the bits in this register and do not know what those bit settings
     * are at compile time.
     */

    NOT_INIT_MV64360_REG (PCIIF_BASE_ADDR_ENABLE_PCI0, dynamic)
    NOT_INIT_MV64360_REG (PCIIF_BASE_ADDR_ENABLE_PCI1, dynamic)

    /* "Retargeting" is not used so disable these registers (program to 0). */

    YES_INIT_MV64360_REG (PCIIF_HDRS_RETARG_BASE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_HDRS_RETARG_BASE_PCI1, 0)

    YES_INIT_MV64360_REG (PCIIF_HDRS_RETARG_BASE_HI_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_HDRS_RETARG_BASE_HI_PCI1, 0)

    YES_INIT_MV64360_REG (PCIIF_HDRS_RETARG_CTRL_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_HDRS_RETARG_CTRL_PCI1, 0)

    /*
     * PCI address decode control, program with default values:
     * Bit 0: 0 = Writes to BAR update remap register.
     *     3: 1 = Mesg units only accessible as part of MV64360 internal space.
     */

    YES_INIT_MV64360_REG (PCIIF_ADDR_DECODE_CTRL_PCI0, (1 << 3))
    YES_INIT_MV64360_REG (PCIIF_ADDR_DECODE_CTRL_PCI1, (1 << 3))

    /*
     * The following registers have fields sampled at reset and should
     * not be touched - thus they will be programmed in the main body
     * of code.  We will simply flip bit 31 to be 0 which makes the
     * register read-only.
     */

    NOT_INIT_MV64360_REG (PCIIF_DLL_STATUS_AND_CTRL_PCI0, dynamic)
    NOT_INIT_MV64360_REG (PCIIF_DLL_STATUS_AND_CTRL_PCI1, dynamic)

    NOT_INIT_MV64360_REG (PCIIF_MPP_PADS_CALIBRATION_PCI0, dynamic)
    NOT_INIT_MV64360_REG (PCIIF_MPP_PADS_CALIBRATION_PCI1, dynamic)

    /*
     * When reading the spec for the following registers, keep in mind
     * that we are working with PCI in little endian mode, and all
     * other busses in big endian mode.  PCI bus width is 64 bits.
     * A table in the spec says to turn on byte swapping but turn
     * off word swapping.  The following bit settings are relevant:
     *
     *
     *   Bit   00 = 0   Set PCI master byte swap. 
     *      03:01 = 0   3:1 Reserved, set to zero. 
     *         04 = 1   Allows write combining.
     *         05 = 1   Allows read combining.
     *         06 = 1   Allows PCI access on 1st data.
     *         07 = 0   Allows rtn of data on 1st.
     *         08 = 1   Enable PCI mstr mem read enable.
     *         09 = 1   Enable PCI mstr mem read multiple.
     *         10 = 0   Disable PCI master word swap. 
     *         11 = 0   Avoid short word swapping.
     *         12 = 0   Slv rtn after 1st buff valid.
     *         13 = 1   Reserved.
     *         14 = 1   Master drives NS attribute.
     *         15 = 1   REQ64# enable.
     *         16 = 0   Slave byte swapping enabled.
     *         17 = 1   Enable DAC dual address cmd.
     *         18 = 1   REQ64# assert on burst access. 
     *         19 = 0   Fix parity on PAR signal. 
     *         20 = 0   This reg determines short swapping.
     *         21 = 0   This reg determines long swapping.
     *         22 = 0   Master CNFG trans little endian.
     *         23 = 0   Disable loop back.
     *      25:24 = 0   Swap internal reg access.
     *      27:26 = 0   PCI-X little endian bus.
     *         28 = 0   Little endian 32-bit bus.
     *         29 = 0   Reserved.
     */

    YES_INIT_MV64360_REG (PCIIF_PCI_CMD_PCI0, ((0 << 0) | (0 << 1) |
			   (1 << 4) | (1 << 5) | (1 << 6) | (0 << 7) |
			   (1 << 8) | (1 << 9) | (0 << 10) | (0 << 11) |
			   (0 << 12) | (1 << 13) | (1 << 14) | (1 << 15) |
			   (0 << 16) | (1 << 17) | (1 << 18) | (0 << 19) |
			   (0 << 20) | (0 << 21) | (0 << 22) | (0 << 23) |
			   (0 << 24) | (0 << 26) | (0 << 28) | (0 << 29)))

    YES_INIT_MV64360_REG (PCIIF_PCI_CMD_PCI1, ((0 << 0) | (0 << 1) |
			   (1 << 4) | (1 << 5) | (1 << 6) | (0 << 7) |
			   (1 << 8) | (1 << 9) | (0 << 10) | (0 << 11) |
			   (0 << 12) | (1 << 13) | (1 << 14) | (1 << 15) |
			   (0 << 16) | (1 << 17) | (1 << 18) | (0 << 19) |
			   (0 << 20) | (0 << 21) | (0 << 22) | (0 << 23) |
			   (0 << 24) | (0 << 26) | (0 << 28) | (0 << 29)))

    /* The following two registers are read only */

    NOT_INIT_MV64360_REG (PCIIF_MODE_PCI0, readOnly )
    NOT_INIT_MV64360_REG (PCIIF_MODE_PCI1, readOnly )

    /*
     * Bits 23:16 set to zero imply "retry forever".  All the other
     * bits are reserved.
     */

    YES_INIT_MV64360_REG (PCIIF_RETRY_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_RETRY_PCI1, 0)

    /*
     * Bits 15:0 set to zero imply "disable timer".  All the other
     * bits are reserved.
     */

    YES_INIT_MV64360_REG (PCIIF_DISCARD_TIMER_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_DISCARD_TIMER_PCI1, 0)

    /*
     * The following are programmed to default - moot because we are not
     * using the MSI (Message Signaled Interrupts).  Program to default
     * value.
     */

    YES_INIT_MV64360_REG (PCIIF_MSI_TRIGGER_TIMER_PCI0, 0x0000ffff)
    YES_INIT_MV64360_REG (PCIIF_MSI_TRIGGER_TIMER_PCI1, 0x0000ffff)
    
    /* 
     * PCI Arbiter control bits are broken down below:
     *
     *   Bit   00 = 0   Reserved.
     *         01 = 0   "Broken detection" disabled.
     *         02 = 0   Reserved.
     *      06:03 = 6   Moot if "broken detection" disabled.
     *      13:07 = 0   Reserved.
     *      20:14 = 0   Enable parking on associated PCI master.
     *      30:21 = 0   Reserved.
     *         31 = 0   Internal arbiter disabled.
     */

    YES_INIT_MV64360_REG (PCIIF_ARB_CTRL_PCI0, ((0 << 0) | (0 << 1) |
						(0 << 2) | (6 << 3) |
						(0 << 7) | (0 << 14) |
						(0 << 21) | (0 << 31)) )

    YES_INIT_MV64360_REG (PCIIF_ARB_CTRL_PCI1, ((0 << 0) | (0 << 1) |
						(0 << 2) | (6 << 3) |
						(0 << 7) | (0 << 14) |
						(0 << 21) | (0 << 31)) )

    /*
     * PCI interface cross bar control low and high bit breakdown 
     *    Default value,(target)      Our programmed value
     *  Bits 03:00  0x2,(CPU)           0x2
     *       07:04  0x3,(PCI0)          0x3 for PCI0, 0x4 for PCI1
     *       11:08  0x5,(MPSC)          0x6 (IDMA)
     *       15:12  0x6,(IDMA)          0x6
     *       19:16  0x7,(Gb)            0x7
     *       23:20  0x3,(PCI0           0x3 for PCI0 = 0x4 for PCI1
     *       27:24  0x3,(PCI0)          0x3 for PCI0 = 0x4 for PCI1
     *       31:28  0x3,(PCI0)          0x3 for PCI0 = 0x4 for PCI1
     *
     * Low and high programmed identicially.
     */

    YES_INIT_MV64360_REG (PCIIF_INTERFACE_CROSSBAR_CTRL_LOW_PCI0,
                          ( (2 << 0) | (3 << 4) | (6 << 8) | (6 << 12) |
                            (7 << 16) | (3 << 20) | (3 << 24) | (3 << 28)) )

    YES_INIT_MV64360_REG (PCIIF_INTERFACE_CROSSBAR_CTRL_HI_PCI0,
                          ( (2 << 0) | (3 << 4) | (6 << 8) | (6 << 12) |
                            (7 << 16) | (3 << 20) | (3 << 24) | (3 << 28)) )

    YES_INIT_MV64360_REG (PCIIF_INTERFACE_CROSSBAR_CTRL_LOW_PCI1,
                          ( (2 << 0) | (4 << 4) | (6 << 8) | (6 << 12) |
                            (7 << 16) | (4 << 20) | (4 << 24) | (4 << 28)) )

    YES_INIT_MV64360_REG (PCIIF_INTERFACE_CROSSBAR_CTRL_HI_PCI1,
                          ( (2 << 0) | (4 << 4) | (6 << 8) | (6 << 12) |
                            (7 << 16) | (4 << 20) | (4 << 24) | (4 << 28)) )

    /* 
     * PCI Interface crossbar timeout leave default as shown below:
     *
     *   Bit   
     *      07:00 = 0xff Crossbar Arbiter timeout preset value.
     *      15:08 = 0x00 Reserved.
     *         16 = 1    Timer disabled.
     *      31:17 = 0    Reserved.
     */

    YES_INIT_MV64360_REG (PCIIF_INTERFACE_CROSSBAR_TIMEOUT_PCI0,
                          (0xff << 0) | (0 << 8) | (1 << 16) | (0 << 17))

    YES_INIT_MV64360_REG (PCIIF_INTERFACE_CROSSBAR_TIMEOUT_PCI1,
                          (0xff << 0) | (0 << 8) | (1 << 16) | (0 << 17))

    /* 
     * No need to initialize the following registers.  They are used
     * by PCI slave devices to perform synchronization.
     */

    NOT_INIT_MV64360_REG (PCIIF_SYNC_BARRIER_TRIGGER_PCI0, readOnly)
    NOT_INIT_MV64360_REG (PCIIF_SYNC_BARRIER_TRIGGER_PCI1, readOnly)

    NOT_INIT_MV64360_REG (PCIIF_SYNC_BARRIER_VIRTUAL_PCI0, readOnly)
    NOT_INIT_MV64360_REG (PCIIF_SYNC_BARRIER_VIRTUAL_PCI1, readOnly)

   /*
    * Since P2P is disabled, the programming of the PCI P2P configuration
    * registers (used to control config-style accesses between the two
    * PCI busses) is moot.  We're forcing the default value:
    */

    YES_INIT_MV64360_REG (PCIIF_P2P_CFG_PCI0, 0x000000ff)
    YES_INIT_MV64360_REG (PCIIF_P2P_CFG_PCI1, 0x000000ff)

    /* 
     * PCI access control base (low):
     *
     *   Bit    0 = 1   Enable/Disable - program to 1 to enable.
     *          1 = 0   Req64 - program to 0 to enable REQ64.
     *        3:2 = 10  Snoop - program to binary 10 to WB cache coherency.
     *                    Note: (WB = write back, WT = write thru).
     *                    This is required for the GBL pin to
     *                    be asserted which would cause the CPU to snoop
     *                    the transaction.
     *          4 = 0   Access protect - program to 0 to allow access
     *          5 = 0   Write protect - program to 0 to allow PCI writes.
     *        7:6 = 1   PCI slave swap - program to binary 01 for no swapping.
     *        9:8 = 0   Max burst 0 program to 00 for 32-bytes.
     *      11:10 = 0   RdSize 0 program to 00 for 32-byte prefetch.
     *      32:12 = ..  Access base address - program to
     *                  LOCAL_MEM_LOCAL_ADRS & 0xfffff000 to fix the memory
     *                  base address (4k granularity is forced on us
     *                  because we are only allowed to specify bits 32:12
     *                  of the address.
     */

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE0_LOW_PCI0, 
                          ( (1 << 0) | (0 << 1) | (2 << 2) | (0 << 4) |
                            (0 << 5) | (1 << 6) | (0 << 8) | (0 << 10) |
                            ( (LOCAL_MEM_LOCAL_ADRS & 0xfffff000) << 12)) )

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE0_LOW_PCI1, 
                          ( (0 << 0) | (0 << 1) | (2 << 2) | (0 << 4) |
                            (0 << 5) | (1 << 6) | (0 << 8) | (0 << 10) |
                            ( (LOCAL_MEM_LOCAL_ADRS & 0xfffff000) << 12)) )

    /* We don't deal with 64-bit addresses here so zero out the following */

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE0_HI_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE0_HI_PCI1, 0)

    /* The associated size registers will be dynamically programmed */

    NOT_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE0_PCI0, dynamic)
    NOT_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE0_PCI1, dynamic)

    /*
     * Now take care of the associated BASE[1-5] registers
     * which are all disabled.
     */

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE1_LOW_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE1_HI_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE1_PCI0, 0)

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE2_LOW_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE2_HI_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE2_PCI0, 0)

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE3_LOW_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE3_HI_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE3_PCI0, 0)

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE4_LOW_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE4_HI_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE4_PCI0, 0)

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE5_LOW_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE5_HI_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE5_PCI0, 0)

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE1_LOW_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE1_HI_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE1_PCI1, 0)

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE2_LOW_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE2_HI_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE2_PCI1, 0)

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE3_LOW_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE3_HI_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE3_PCI1, 0)

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE4_LOW_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE4_HI_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE4_PCI1, 0)

    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE5_LOW_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_BASE5_HI_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_ACCESS_CTRL_SIZE5_PCI1, 0)

    /*
     * The following registers are not initialized but instead are used
     * to drive configuration accesses.
     */

    NOT_INIT_MV64360_REG (PCIIF_CFG_ADDR_PCI0, configAccess)
    NOT_INIT_MV64360_REG (PCIIF_CFG_DATA_PCI0, configAccess)
    NOT_INIT_MV64360_REG (PCIIF_CFG_ADDR_PCI1, configAccess)
    NOT_INIT_MV64360_REG (PCIIF_CFG_DATA_PCI1, configAccess)

    /* The following registers are read only and require no programming. */

    NOT_INIT_MV64360_REG (PCIIF_INTERRUPT_ACK_PCI0, 0)
    NOT_INIT_MV64360_REG (PCIIF_INTERRUPT_ACK_PCI1, 0)

    /*
     * The SERR# mask registers and PCI interrupt mask must be programmed 
     * with zero to makes out related interrupts.  The interrupt cause 
     * registers must be programmed with  zero to clear the interrupt cause 
     * bits which are currently set.
     */

    YES_INIT_MV64360_REG (PCIIF_SERR_MASK_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_SERR_MASK_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_INTERRUPT_MASK_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_INTERRUPT_MASK_PCI1, 0)
    YES_INIT_MV64360_REG (PCIIF_INTERRUPT_CAUSE_PCI0, 0)
    YES_INIT_MV64360_REG (PCIIF_INTERRUPT_CAUSE_PCI1, 0)

    /*
     * The following registers must be read in order to allow for another
     * address to be latched.  This is done dynamically in the main
     * body of the code.
     */

    NOT_INIT_MV64360_REG (PCIIF_ERR_ADDR_LOW_PCI0, read)
    NOT_INIT_MV64360_REG (PCIIF_ERR_ADDR_LOW_PCI1, read)

    /* 
     * The registers associated with the above (read to unlatch)
     * need no programming.
     */

    NOT_INIT_MV64360_REG (PCIIF_ERR_ADDR_HI_PCI0, noProgram)
    NOT_INIT_MV64360_REG (PCIIF_ERR_ATTR_PCI0, noProgram)
    NOT_INIT_MV64360_REG (PCIIF_ERR_CMD_PCI0, noProgram)
    NOT_INIT_MV64360_REG (PCIIF_ERR_ADDR_HI_PCI1, noProgram)
    NOT_INIT_MV64360_REG (PCIIF_ERR_ATTR_PCI1, noProgram)
    NOT_INIT_MV64360_REG (PCIIF_ERR_CMD_PCI1, noProgram)

    /* The device/vendor ID registers are not programmed. */

    NOT_INIT_MV64360_REG (PCIIF_DEVICE_AND_VENDOR_ID_PCICFG, noProgram)
    NOT_INIT_MV64360_REG (PCIIF_DEVICE_AND_VENDOR_ID_PCICFG_ALT, noProgram)

    /* Leave the following registers in their default state. */

    NOT_INIT_MV64360_REG (PCIIF_CLASS_CODE_AND_REVISION_ID_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_CLASS_CODE_AND_REVISION_ID_PCICFG_ALT, default)

    /* 
     * Modify the cache line size in the following registers - program
     * all other fields to their default value.  Here is the field breakdown:
     *
     * Bit  7:0 = 8     Cache line size (change from 0 to 8).
     *     15:8 = 0x40  Latency timer = 0x40 for PCI-X.
     *    23:16 = 0x80  Config Header Type Init Val = 0x80.
     *    27:24 = x     BIST completion code is read only.
     *    29:28 = x     Reserved.
     *       30 = 0     BIST active bit = 0.
     *       31 = x     BIST capable bit is read only.
     */

    YES_CNFG_MV64360_REG (PCI_FCN0, PCIIF_BIST_LATENCY_CACHE_PCICFG,
                          ((8 << 0) | (0x40 << 8) | (0x80 << 16) | (0 << 30)) )
                        
    /*
     * Leave the following registers in their default value by not 
     * programming them at all.
     */

    NOT_INIT_MV64360_REG (PCIIF_SUBSYSTEM_DEVICE_AND_VENDOR_ID_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_SUBSYSTEM_DEVICE_AND_VENDOR_ID_PCICFG_ALT, 
			  default)
    NOT_INIT_MV64360_REG (PCIIF_CAPABILITY_LIST_PTR_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_CAPABILITY_LIST_PTR_PCICFG_ALT, default)
    NOT_INIT_MV64360_REG (PCIIF_INTERRUPT_PIN_AND_LINE_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_INTERRUPT_PIN_AND_LINE_PCICFG_ALT, default)
    
   /*
    * Since we're disabling the "capabilities list" in the
    * PCIIF_STATUS_AND_CMD_PCIx register, the programming of the following
    * registers is moot.  They will be left in their default state.
    */

    NOT_INIT_MV64360_REG (PCIIF_POWER_MGMT_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_POWER_MGMT_PCICFG_ALT, default)
    NOT_INIT_MV64360_REG (PCIIF_POWER_MGMT_CTRL_AND_STATUS_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_POWER_MGMT_CTRL_AND_STATUS_PCICFG_ALT, default)
    NOT_INIT_MV64360_REG (PCIIF_VPD_DATA_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_VPD_DATA_PCICFG_ALT, default)
    NOT_INIT_MV64360_REG (PCIIF_VPD_ADDR_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_VPD_ADDR_PCICFG_ALT, default)
    NOT_INIT_MV64360_REG (PCIIF_MSI_MSG_CTRL_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_MSI_MSG_CTRL_PCICFG_ALT, default)
    NOT_INIT_MV64360_REG (PCIIF_MSI_MSG_ADDR_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_MSI_MSG_ADDR_PCICFG_ALT, default)
    NOT_INIT_MV64360_REG (PCIIF_MSI_MSG_UPPER_ADDR_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_MSI_MSG_UPPER_ADDR_PCICFG_ALT, default)
    NOT_INIT_MV64360_REG (PCIIF_MSG_DATA_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_MSG_DATA_PCICFG_ALT, default)
    NOT_INIT_MV64360_REG (PCIIF_COMPACTPCI_HOTSWAP_PCICFG, default)
    NOT_INIT_MV64360_REG (PCIIF_COMPACTPCI_HOTSWAP_PCICFG_ALT, default)

    /* 
     * All registers in the "message unit interface" will be left
     * in their default state.  We do not use the message passing
     * interface in this BSP.  The registers are summarized below:
     */

    NOT_INIT_MV64360_REG (MUIF_INBND_MSG0_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_MSG0_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_MSG1_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_MSG1_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_MSG0_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_MSG0_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_MSG1_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_MSG1_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_DOORBELL_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_DOORBELL_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_INTERRUPT_CAUSE_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_INTERRUPT_CAUSE_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_INTERRUPT_MASK_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_INTERRUPT_MASK_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_DOORBELL_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_DOORBELL_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_INTERRUPT_CAUSE_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_INTERRUPT_CAUSE_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_INTERRUPT_MASK_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_INTERRUPT_MASK_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_Q_PORT_VIRTUAL_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_Q_PORT_VIRTUAL_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_Q_PORT_VIRTUAL_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_Q_PORT_VIRTUAL_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_Q_CTRL_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_Q_CTRL_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_Q_BASE_ADDR_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_Q_BASE_ADDR_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_POST_HEAD_PTR_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_POST_HEAD_PTR_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_FREE_HEAD_PTR_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_FREE_HEAD_PTR_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_FREE_TAIL_PTR_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_FREE_TAIL_PTR_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_POST_TAIL_PTR_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_INBND_POST_TAIL_PTR_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_FREE_HEAD_PTR_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_FREE_HEAD_PTR_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_FREE_TAIL_PTR_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_FREE_TAIL_PTR_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_POST_HEAD_PTR_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_POST_HEAD_PTR_PCI1, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_POST_TAIL_PTR_PCI0, default)
    NOT_INIT_MV64360_REG (MUIF_OUTBND_POST_TAIL_PTR_PCI1, default)

    /* 
     * The PCI_X command registers will be programmed to their default
     * values, here is the breakdown:
     *
     * Bit  7:0 = x  Capability ID 0x7 - Read Only register.
     *     15:8 = .. Next item pointer PCI0 = 0x68, PCI1 = 0x00.
     *       16 = 0  Data parity error recovery enable, set to 0 to disable.
     *       17 = 0  Enable relaxed ordering = 0.
     *    19:18 = 0  Max Mem Read Byte count set to 0x0 to encode 512 bytes.
     *    22:20 = 3  Max outstanding split transactions = 0x3 to encode 4 trans.
     *    31:23 = 0  Reserved = 0.
     */

    YES_CNFG_MV64360_REG (PCI_FCN0, PCIIF_PCI_X_CMD_PCICFG,
                          ((0x68 << 8) | (0 << 16) | (0 << 17) |
                           (0 << 18) | (3 << 20) | (0 << 23)) )

    /* Leave PCI_X status regsiters in their default state */

    NOT_INIT_MV64360_REG (PCIIF_PCI_X_STATUS_PCICFG, default) 
    NOT_INIT_MV64360_REG (PCIIF_PCI_X_STATUS_PCICFG_ALT, default) 

    /* Finally, we're at the end of the table. */

    END_INIT_MV64360_REG

    };

/* forward declarations */

void sysMv64360PhbInit (void);

/******************************************************************************
*
* sysMv64360PhbInit - Dynamic initialization of Mv64360 host bridge interface.
*
* This routine performs the dynamic initialization of the Mv64360 device bus
* interface registers.  
*
* RETURNS: NA
*/

void
sysMv64360PhbInit(void)
{
  UINT32 baseEnable;
  UINT32 temp1;
  UINT32 temp2;
  UINT32 pciCmdStatus;
  char txt[100];

#ifdef MYDEBUG
  sprintf(txt,"sysMv64360PhbInit: reached\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  sprintf(txt,"sysMv64360PhbInit: we will map 0x%08x of our memory to PCI \r\n ",
    PCI2DRAM_MAP_SIZE);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  /* check if PCI0 is configured to PCI-X mode */ 
  temp2 = MV64360_READ32 (MV64360_REG_BASE, PCIIF_MODE_PCI0);
  if(temp2 & 0x30)
  {
	sysPciConfigRead (0xff, 0x1f, PCI_FCN0, PCIIF_PCI_X_STATUS_PCICFG, 
			       MV64360_REG_SIZE, &temp1); 
	temp1 &= ~((0x1F << 3) | (0xFF << 8));  /* set bus number and device num to zero */
	sysPciConfigWrite (0xff, 0x1f, PCI_FCN0, PCIIF_PCI_X_STATUS_PCICFG, 
			       MV64360_REG_SIZE, temp1); 
  }

  /* Initialize static device control parameters */
  sysMv64360RegInit(mv64360PhbTbl);

  /*
   * Program those registers which require dynamic calculations
   * Start with the PCI to CPU registers, in particular the following
   *   PCIIF_CSx_BASE_ADDR_REMAP_PCI0
   *   PCIIF_CSx_BASE_ADDR_LOW_PCI0
   *   PCIIF_CSx_BAR_SIZE_PCI0
   *     x = [0|1|2|3]
   * Note that PCIIF_CSx_BASE_ADDR_HI_PCI0 has already been programmed
   * to zero via entry in the mv64360PhbTbl.
   */
  if(sysMonarchMode)
  {
	temp1 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE);

	if(~temp1 & (1 << CPUIF_BASE_ADDR_ENABLE_CS0_BIT))
	{
	  temp2 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS0_BASE_ADDR) << 16;
	  MV64360_WRITE32_PUSH (MV64360_REG_BASE, 
				      PCIIF_CS0_BASE_ADDR_REMAP_PCI0, temp2);
	  temp2 |= PCI_BASE_PREFETCH;	/* Set prefetch enable indicator. */

	  sysPciConfigWrite (0, 0, PCI_FCN0, PCIIF_CS0_BASE_ADDR_LOW_PCICFG, 
				   MV64360_REG_SIZE, temp2); 

	  temp2 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS0_SIZE);
	        MV64360_WRITE32_PUSH (MV64360_REG_BASE, PCIIF_CS0_BAR_SIZE_PCI0,
	            PCI_SIZE_ENCODE (MV64360_SIZE_FIELD_INV(temp2)));
	}

	if(~temp1 & (1 << CPUIF_BASE_ADDR_ENABLE_CS1_BIT))
	{
	  temp2 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS1_BASE_ADDR) << 16;
	  MV64360_WRITE32_PUSH (MV64360_REG_BASE, 
				      PCIIF_CS1_BASE_ADDR_REMAP_PCI0, temp2);
	  temp2 |= PCI_BASE_PREFETCH;	/* Set prefetch enable indicator. */

	  sysPciConfigWrite (0, 0, PCI_FCN0, PCIIF_CS1_BASE_ADDR_LOW_PCICFG,
				   MV64360_REG_SIZE, temp2); 

	  temp2 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS1_SIZE);
	        MV64360_WRITE32_PUSH (MV64360_REG_BASE,  PCIIF_CS1_BAR_SIZE_PCI0,
	            		      PCI_SIZE_ENCODE (MV64360_SIZE_FIELD_INV(temp2)));
	}

	if(~temp1 & (1 << CPUIF_BASE_ADDR_ENABLE_CS2_BIT))
	{
	  temp2 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS2_BASE_ADDR) << 16;
	  MV64360_WRITE32_PUSH (MV64360_REG_BASE, 
				      PCIIF_CS2_BASE_ADDR_REMAP_PCI0, temp2);
	  temp2 |= PCI_BASE_PREFETCH;	/* Set prefetch enable indicator. */

	  sysPciConfigWrite (0, 0, PCI_FCN1, PCIIF_CS2_BASE_ADDR_LOW_PCICFG,
				   MV64360_REG_SIZE, temp2); 

	  temp2 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS2_SIZE);
	        MV64360_WRITE32_PUSH (MV64360_REG_BASE,  PCIIF_CS2_BAR_SIZE_PCI0,
	                              PCI_SIZE_ENCODE (MV64360_SIZE_FIELD_INV(temp2)));
	}

	if(~temp1 & (1 << CPUIF_BASE_ADDR_ENABLE_CS3_BIT))
	{
	  temp2 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS3_BASE_ADDR) << 16;
	  MV64360_WRITE32_PUSH (MV64360_REG_BASE, 
				      PCIIF_CS3_BASE_ADDR_REMAP_PCI0, temp2);
	  temp2 |= PCI_BASE_PREFETCH;	/* Set prefetch enable indicator. */

	  sysPciConfigWrite (0, 0, PCI_FCN1, PCIIF_CS3_BASE_ADDR_LOW_PCICFG, 
				   MV64360_REG_SIZE, temp2); 

	  temp2 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CS3_SIZE);
	        MV64360_WRITE32_PUSH (MV64360_REG_BASE,  PCIIF_CS3_BAR_SIZE_PCI0,
	                              PCI_SIZE_ENCODE (MV64360_SIZE_FIELD_INV(temp2)));
	}
  }


  /*
   * Program the PCI_ACCESS_CTRL_SIZE0_PCIx registers to allow access
   * to DRAM.
   *
   * Bits 32:12 need to be programmed with the size of DRAM in
   * the standard PCI 0x000fff... format.  Since a power of 2
   * is required for this programming, the plan is to round up
   * the memory size to the nearest power of two and convert
   * that into the requisite size.
   */
    
  if(sysMonarchMode)  
  {
	temp1 = (UINT32) sysPhysMemTop();	 /* Top of physical memory */
  }
  else
  {
	temp1 = PCI2DRAM_MAP_SIZE;
  }	

  temp2 = 1;
  while(temp1 > 1)	/* Round temp1 to next power of 2 - put in temp2 */
  {
	temp2 <<= 1; 
	temp1 >>= 1;
  }

  temp1 = PCI_SIZE_ENCODE (temp2);

  MV64360_WRITE32_PUSH (MV64360_REG_BASE, PCIIF_ACCESS_CTRL_SIZE0_PCI0,
	                      temp1);

  /* "Unlatch" by reading the following registers */
  (void) MV64360_READ32 (MV64360_REG_BASE, PCIIF_ERR_ADDR_LOW_PCI0);

  /* 
   * Now enable the appropriate PCI windows by tweeking the "PCI Base 
   * Address Enable" register.  First read the base enable 
   * register, then disable all DEVCS[0-3], IntIO, P2P, CPU, SDRAM
   * windows by setting the appropriate bits in the image and 
   * then copy the bits for CS0, CS1, CS2, CS3 from the CPU base
   * address enable register.
   */

  baseEnable = 0xffffffff;	/* Start by disabling everything */

  temp1 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE);

  if ((temp1 & (1 << CPUIF_BASE_ADDR_ENABLE_CS0_BIT)) == 0)
	baseEnable &= ~(1 << PCIIF_BASE_ADDR_ENABLE_CS0_BIT);
  if ((temp1 & (1 << CPUIF_BASE_ADDR_ENABLE_CS1_BIT)) == 0)
	baseEnable &= ~(1 << PCIIF_BASE_ADDR_ENABLE_CS1_BIT);
  if ((temp1 & (1 << CPUIF_BASE_ADDR_ENABLE_CS2_BIT)) == 0)
	baseEnable &= ~(1 << PCIIF_BASE_ADDR_ENABLE_CS2_BIT);
  if ((temp1 & (1 << CPUIF_BASE_ADDR_ENABLE_CS3_BIT)) == 0)
	baseEnable &= ~(1 << PCIIF_BASE_ADDR_ENABLE_CS3_BIT);

  /* enable PCI access internal register for mailbox */
  baseEnable &= ~(1 << PCIIF_BASE_ADDR_ENABLE_INTMEM_BIT);
		
  MV64360_WRITE32_PUSH (MV64360_REG_BASE, PCIIF_BASE_ADDR_ENABLE_PCI0,
			  baseEnable);

  MV64360_WRITE32_PUSH (MV64360_REG_BASE, PCIIF_BASE_ADDR_ENABLE_PCI1,
			  0xffffffff);

  /* 
   * Now enable the appropriate CPU windows by tweeking the "CPU Base 
   * Address Enable" register.  First read the base enable 
   * register, then disable PCI_0_MEM[1-3] windows and all PCI_1 windows by
   * setting the appropriate bits in the image and then clear 
   * the bits for PCI_0_IO and PCI_0_MEM[0] to enable
   * these windows.
   */

  baseEnable = MV64360_READ32 (MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE);

  baseEnable |=  ( (1 << CPUIF_BASE_ADDR_ENABLE_PCI0_MEM3_BIT) |
					(1 << CPUIF_BASE_ADDR_ENABLE_PCI1_IO_BIT)   |
					(1 << CPUIF_BASE_ADDR_ENABLE_PCI1_MEM0_BIT) |
					(1 << CPUIF_BASE_ADDR_ENABLE_PCI1_MEM1_BIT) |
					(1 << CPUIF_BASE_ADDR_ENABLE_PCI1_MEM2_BIT) |
					(1 << CPUIF_BASE_ADDR_ENABLE_PCI1_MEM3_BIT));

  baseEnable &= ~( (1 << CPUIF_BASE_ADDR_ENABLE_PCI0_IO_BIT)   |
					(1 << CPUIF_BASE_ADDR_ENABLE_INTRNL_SPACE_BIT) |
					(1 << CPUIF_BASE_ADDR_ENABLE_PCI0_MEM0_BIT) |
					(1 << CPUIF_BASE_ADDR_ENABLE_PCI0_MEM1_BIT) |
					(1 << CPUIF_BASE_ADDR_ENABLE_PCI0_MEM2_BIT));

  MV64360_WRITE32_PUSH (MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE,
	                      baseEnable);

  /*
   * The following registers have fields sampled at reset and should
   * not be touched.  We will simply flip bit 31 to be 0 which makes the
   * register read-only.
   */

  temp1 = MV64360_READ32 (MV64360_REG_BASE, PCIIF_DLL_STATUS_AND_CTRL_PCI0);
  temp1 &= 0x7fffffff; 	/* clear bit 31 */
  MV64360_WRITE32_PUSH (MV64360_REG_BASE, PCIIF_DLL_STATUS_AND_CTRL_PCI0,
			  temp1);

  temp1 = MV64360_READ32 (MV64360_REG_BASE, PCIIF_MPP_PADS_CALIBRATION_PCI0);
  temp1 &= 0x7fffffff; 	/* clear bit 31 */
  MV64360_WRITE32_PUSH (MV64360_REG_BASE, PCIIF_MPP_PADS_CALIBRATION_PCI0,
			  temp1);

  /*
   * The following programming should be performed on the
   * PCI status and command registers.
   *
   * Bit 0 = 1  I/O Enable change from default 0 to 1 (enabled).
   *     1 = 1  Memory access enable chg from def 0 to 1 (enabled).
   *     2 = 1  Bus mastering enable chg from def 0 to 1 (enabled).
   *     3 = x  Read-only 0 - no programming required.
   *     4 = 1  Memory write & invalidate enable (0 to 1) (enabled).
   *     5 = x  Read only 0.
   *     6 = 0  Parity enable program to 0 (disable).
   *     7 = x  Apparently this is read-only 0.
   *     8 = 0  SErrEn program to 0 (disable).
   *     9 = 0  FastBTBEn leave as 0 (disabled).
   * 19:10 = 0  Reserved, leave as 0.
   *    20 = 1  CapList, set to 1 to enable.
   *    21 = 1  66MhzEn, Set to 1 to enable.
   *    22 = 0  Reserved, read only - leave as 0.
   *    23 = 1  TarFastBB set as 1, enabling receipt of fastBB.
   *    24 = 1  DataPerr, status bit clear by writing 1.
   * 26:25 = 1  DevSelTim timing (medium), leave as 1.
   *    27 = 1  SlaveTabort, status bit clear by writing 1.
   *    28 = 1  MasterTabort, status bit clear by writing 1.
   *    29 = 1  MAbort, status bit clear by writing 1.
   *    30 = 1  SysErr, status bit clear by writing 1.
   *    31 = 1  DetParErr, status bit clear by writing 1.
   */

  pciCmdStatus = ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 4) |
	                (0 << 6) | (0 << 8) | (0 << 9) | (0 << 10) |
	                (1 << 20) | (1 << 21) | (0 << 22) | (1 << 23) |
	                (1 << 24) | (1 << 25) | (1 << 27) | (1 << 28) |
	                (1 << 29) | (1 << 30) | (1 << 31)); 

  sysPciConfigWrite (0, 0, PCI_FCN0, PCIIF_STATUS_AND_CMD_PCICFG, 
		       MV64360_REG_SIZE, pciCmdStatus); 
	
  temp2 = MV64360_READ32 (MV64360_REG_BASE, CPUIF_CPU_CFG);
  temp2 |= (1 << 17);	/* stop PCI transactions retry */
  MV64360_WRITE32_PUSH (MV64360_REG_BASE,  CPUIF_CPU_CFG, temp2);
}







