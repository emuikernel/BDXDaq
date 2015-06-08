/* sysMv64260Phb.c - Initialize the GT-64260 Host Bridge functionality */

/* Copyright 2003 Motorola, Inc., All Rights Reserved */

#include "copyright_wrs.h"

/*
modification history
--------------------
01i,17jul03,scb PCI max burst is 4 64-bit words.
01h,02jun03,scb DMA speed tuning, among other things program PCI access 
		control regs for aggressive prefetch.
01g,29apr03,scb Minor commentary fixes.
01f,08apr03,scb HiNT transparent PCI-PCI bridge support.
01e,11mar03,cak Tweek programming of PCI I/O (bus 0 & 1) windows after having
		moved 16-bit I/O space from PCI bus 1.0 to PCI bus 0.0.
01d,19feb03,scb Tweek programming for cpu deadlock ordering and sync barrier
		attribute to conform to Marvell suggestion and to improve
		shared memory  reliability.
01d,10feb03,scb Fix several errors in "default" programming of registers.
01c,19nov02,scb Add registers exclusive to GT-64260B and not in GT-64260A. 
01b,12nov02,scb VME/Universe support.
01b,25oct02,scb Minor tweeks to be consistent with MOTLoad.
01a,01oct02,scb Copied from hxeb100 base (ver 01c).
*/

/*
DESCRIPTION

This module initializes the GT-64260 host bridge support registers.
The data paths that are enabled via code in this file are the
CPU to PCI and PCI to CSx (chip select - DRAM) paths.
*/

/* includes */

#include "mv64260.h"
#include "drv/pci/pciConfigLib.h"

/* defines */

/* typedefs */

/* globals */

/* locals */

/*
 * This table contains static initialization values for the MV64260
 * internal host bridge support registers.
 */

LOCAL UINT32 mv64260PhbTbl [] =
    {

    /*
     * "CPU Interface - Address Decode Registers":
     * Registers allow the CPU to access the PCI0 space.  The memory map
     * that we are following is diagrammed in "config.h".  The registers
     * that are programmed are:
     *
     * 1) low decode address
     * 2) high decode address
     * 3) remap register (1 remap for I/O space, two (a low and
     *			  high remap) for memory space)
     *
     * For accesses to PCI memory (non-I/O) space, the
     * remap register(s) will be programmed for non-biased
     * address translation.  That is, the output address from the
     * remap function is the same as the input address.
     *
     * For accesses to PCI I/O space a biased remap will be
     * programmed - see commentary in "config.h" for details.
     * Since we are dealing with 32-bit PCI access, the "high"
     * remap registers in this case will be programmed to 0.
     *
     * First, let's allow access to PCI0 I/O space:
     * Note: bit 24 set is in the PCISwap field and indicates no swapping.
     * this same bit is set in all of the "enabled" memory low decode
     * registers.
     */

    YES_INIT_MV64260_REG (CPUIF_PCI0_IO_LO_DECODE_ADDR,
			  (MV64260_LO_DECODE_ADDR(ISA_MSTR_IO_LOCAL) |
			   (1<<24)))

    YES_INIT_MV64260_REG (CPUIF_PCI0_IO_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_ADDR(ISA_MSTR_IO_LOCAL,
						 (ISA_MSTR_IO_SIZE +
						  PCI0_MSTR_IO_SIZE)))

    YES_INIT_MV64260_REG (CPUIF_PCI0_IO_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(ISA_MSTR_IO_BUS))

    /* Now let's do the PCI0 MEMIO (nonprefetchable memory) space */

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM0_LO_DECODE_ADDR,
			  (MV64260_LO_DECODE_ADDR(PCI0_MSTR_MEMIO_LOCAL) |
			   (1<<24)))

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM0_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_ADDR(PCI0_MSTR_MEMIO_LOCAL,
						 PCI0_MSTR_MEMIO_SIZE))

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM0_LO_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(PCI0_MSTR_MEMIO_BUS))

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM0_HI_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(0))
	





	/* Sergey: map our  memory ??? */
    /* Now for the PCI0 MEM (prefetchable memory) space */

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM1_LO_DECODE_ADDR,
			  (MV64260_LO_DECODE_ADDR(PCI0_MSTR_MEM_LOCAL) |
			   (1<<24)))

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM1_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_ADDR(PCI0_MSTR_MEM_LOCAL,
						 PCI0_MSTR_MEM_SIZE))
    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM1_LO_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(PCI0_MSTR_MEM_BUS))

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM1_HI_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(0))






    /* Do the access to PCI space which opens to the VME windows */

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM2_LO_DECODE_ADDR,
			  (MV64260_LO_DECODE_ADDR(VME_MEM_LOCAL_START) |
			   (1<<24)))

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM2_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_ADDR(VME_MEM_LOCAL_START,
						 VME_MEM_LOCAL_SIZE))

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM2_LO_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(VME_MEM_LOCAL_START +
						       VME_CPU2PCI_OFFSET))

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM2_HI_RMAP, 0)

    /*
     * The remaining CPU to PCI0 access registers will be disabled
     * This is done by programming a "low decode" address of 0xffffffff
     * and a "size" of -1.  The remap registers are really irrelevant
     * here but will be programmed to zero.
     */

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM3_LO_DECODE_ADDR,
			  ((1<<24) | MV64260_LO_DECODE_DISABLE))

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM3_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM3_LO_RMAP, MV64260_LO_RMAP_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PCI0_MEM3_HI_RMAP, MV64260_HI_RMAP_DISABLE)

    /*
     * Now program the registers which will allow the CPU to access
     * PCI1 space.  First let's allow access to PCI1 I/O space.
     */

    YES_INIT_MV64260_REG (CPUIF_PCI1_IO_LO_DECODE_ADDR,
			  (MV64260_LO_DECODE_ADDR(PCI1_MSTR_IO_LOCAL) |
			   (1<<24)))

    YES_INIT_MV64260_REG (CPUIF_PCI1_IO_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_ADDR(PCI1_MSTR_IO_LOCAL,
                                                 PCI1_MSTR_IO_SIZE))

    YES_INIT_MV64260_REG (CPUIF_PCI1_IO_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(PCI1_MSTR_IO_BUS))

    /* Now let's do the PCI1 MEMIO (nonprefetchable memory) space */

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM0_LO_DECODE_ADDR,
			  (MV64260_LO_DECODE_ADDR(PCI1_MSTR_MEMIO_LOCAL) |
			   (1<<24)))

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM0_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_ADDR(PCI1_MSTR_MEMIO_LOCAL,
						 PCI1_MSTR_MEMIO_SIZE))

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM0_LO_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(PCI1_MSTR_MEMIO_BUS))

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM0_HI_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(0))

    /* Now for the PCI1 MEM (prefetchable memory) space */

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM1_LO_DECODE_ADDR,
			  (MV64260_LO_DECODE_ADDR(PCI1_MSTR_MEM_LOCAL) |
			   (1<<24)))

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM1_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_ADDR(PCI1_MSTR_MEM_LOCAL,
						 PCI1_MSTR_MEM_SIZE))

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM1_LO_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(PCI1_MSTR_MEM_BUS))

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM1_HI_RMAP,
			  MV64260_CPU_TO_PCI_ADDR_RMAP(0))

    /*
     * As was done for PCI0, the MEM2 and MEM3 CPU to PCI1 access
     * registers will be disabled by programming a "low decode"
     * address of 0 and a "size" of -1.
     */

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM2_LO_DECODE_ADDR,
			  ((1<<24) | MV64260_LO_DECODE_DISABLE))

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM2_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM2_LO_RMAP, MV64260_LO_RMAP_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM2_HI_RMAP, MV64260_HI_RMAP_DISABLE)

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM3_LO_DECODE_ADDR,
			  ((1<<24) | MV64260_LO_DECODE_DISABLE))

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM3_HI_DECODE_ADDR,
			  MV64260_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM3_LO_RMAP, MV64260_LO_RMAP_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PCI1_MEM3_HI_RMAP, MV64260_HI_RMAP_DISABLE)

    /*
     * The CPU Internal Space Decode Address register should already be
     * OK but we will set bit 24, indicating no PCI swap.
     */

    YES_INIT_MV64260_REG (CPUIF_INTRNL_SPACE_DECODE_ADDR,
		          ((MV64260_LO_DECODE_ADDR(MV64260_REG_BASE)) |
			   (1<<24)))

    /*
     * There are a bunch of CPUIF... registers which we still need to
     * handle.  Most of them serve no function in our BSP.  The are
     * presented below with minimal commentary.
     */

    /* Disable the CPU0 to CPU1 (and vice versa) translation */

    YES_INIT_MV64260_REG (CPUIF_CPU0_LO_DECODE_ADDR, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_CPU0_HI_DECODE_ADDR, MV64260_HI_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_CPU1_LO_DECODE_ADDR, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_CPU1_HI_DECODE_ADDR, MV64260_HI_DECODE_DISABLE)

    /*
     * "CPU Interface - CPU control registers":
     * Note that CPUIF_CPU_CFG (CPU configuration register) must be
     * programmed in a  special way so it is done in the dynamic section
     * inside of sysMv64260PhbInit().  Let's start with the CPU mode
     * register:  We don't really need to initialize this one since it is
     * a read only * register:
     */

    NOT_INIT_MV64260_REG (CPUIF_CPU_MODE, readOnly)

    /*
     * CPU Master control -
     * The following bits are are changed from their default state of '0':
     *  Bit 8  - Set to '1' to use GT-64260A as CPU bus arbiter.
     *  Bit 12 - Set to '1' to support MCP74xx "clean block" snoop transaction.
     *  bit 13 - Set to '1' to support MCP74xx "flush block" snoop transaction.
     */

    YES_INIT_MV64260_REG (CPUIF_CPU_MSTR_CTRL, ((1<<8) | (1<<12) | 1<<13))

    /*
     * Program the low and high the CPU Interface Crossbar registers
     *	  0x0 = SDRAM interface snoop request
     *    0x1 = Reserved
     *    0x2 = NULL request
     *    0x3 = PCI_0 access
     *    0x4 = PCI_1 access
     *    0x5 = Comm unit access
     *    0x6 = IDMA channels 0/1/2/3 access
     *    0x7 = IDMA channels 4/5/6/7 access
     *    0x8 - 0xf Reserved
     */

    YES_INIT_MV64260_REG (CPUIF_XBAR_CTRL_LO, 0x22765430)
    YES_INIT_MV64260_REG (CPUIF_XBAR_CTRL_HI, 0x22765430)

    /*
     * Disable the CPU Interface Crossbar Timeout register - this
     * is the default condition but well will stuff it anyway:
     *
     *    bits 7:0    Crossbar arbiter timeout preset value
     *    bits 16     0 = enable, 1 = disable
     *    All other bits reserved
     */

    YES_INIT_MV64260_REG (CPUIF_XBAR_TMOUT, 0x000100ff)

    /*
     * Program the low and high CPU Read Response Crossbar registers
     *	  0x0 = SDRAM read data
     *    0x1 = Device read data
     *    0x2 = NULL
     *    0x3 = PCI_0 read data
     *    0x4 = PCI_1 read data
     *    0x5 = Comm unit internal registers read data
     *    0x6 = IDMA 0/1/2/3 internal registers read data
     *    0x7 = IDMA 4/5/6/7 internal registers read data
     *    0x8 - 0xf Reserved
     *
     * Each 4-bit nibble takes one of the above values
     */

    YES_INIT_MV64260_REG (CPUIF_READ_RSP_XBAR_CTRL_LO, 0x27654310)
    YES_INIT_MV64260_REG (CPUIF_READ_RSP_XBAR_CTRL_HI, 0x27654310)

    /*
     * "CPU Interface - Sync Barrier Registers":
     * They are are read only.
     */

    NOT_INIT_MV64260_REG (CPUIF_PCI0_SYNC_BARRIER, readOnly)
    NOT_INIT_MV64260_REG (CPUIF_PCI1_SYNC_BARRIER, readOnly)

    /*
     * Note the following (5) registers are present on the GT-64260B
     * but not on the GT-64260A.  
     *
     * We will not program the 'sync barrier trigger' registers since we 
     * don't really want to initiate a sync barrier process at this time.  
     */

    NOT_INIT_MV64260_REG (CPUIF_CPU0_SYNC_BARRIER_TRGR, x)
    NOT_INIT_MV64260_REG (CPUIF_CPU1_SYNC_BARRIER_TRGR, x)

    /* 
     * The 'deadlock and ordering' register will be set to enable
     * PCI0, PCI1, and communication unit deadlock mechanism
     * (bits 18:16), plus overall deadlock enabling (bit 31).
     */

    YES_INIT_MV64260_REG (CPUIF_DEADLOCK_AND_ORDERING, 0x80070000)
     
    /*
     * The 'Counters and Sync Barrier Attribute' register will be programmed
     * to enable "enhanced sync barrier mechanism" as well as
     * enabling the sync barrier mechanism for SDRAM, PCI0, and PCI1.
     * The remaining bits will be left in their default state.
     */

    YES_INIT_MV64260_REG (CPUIF_CNTR_AND_SYNC_BARRIER_ATTR, 0x07fff600)

    /*
     * The 'writeback priority and buffer depth' register will be 
     * programmed SCS[0-3] writeback enable.  
     */

    YES_INIT_MV64260_REG (CPUIF_WRT_BACK_PRI_AND_BUF_DEPTH, 0xc400000f) 

    /*
     * "CPU Interface - Access Protection Registers":
     * Most of them are disabled, however the CPUIF_PICT0... set is
     * dynamically * programmed inside of sysMv64260PhbInit().
     */

    YES_INIT_MV64260_REG (CPUIF_PTCT1_ADDR_LO, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT1_ADDR_HI, MV64260_HI_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT2_ADDR_LO, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT2_ADDR_HI, MV64260_HI_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT3_ADDR_LO, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT3_ADDR_HI, MV64260_HI_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT4_ADDR_LO, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT4_ADDR_HI, MV64260_HI_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT5_ADDR_LO, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT5_ADDR_HI, MV64260_HI_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT6_ADDR_LO, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT6_ADDR_HI, MV64260_HI_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT7_ADDR_LO, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_PTCT7_ADDR_HI, MV64260_HI_DECODE_DISABLE)

    /*
     * "CPU Interface - Snoop Control Registers":
     * Only one of the 4 Snoop Base Address decode registers (set 0) will
     * be enabled and that will be done dynamically in sysMv64260PhbInit().
     * The remaining sets (1 thru 3) will be disabled here.
     */

    YES_INIT_MV64260_REG (CPUIF_SNOOP1_BASE_ADDR, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_SNOOP1_TOP_ADDR, MV64260_HI_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_SNOOP2_BASE_ADDR, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_SNOOP2_TOP_ADDR, MV64260_HI_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_SNOOP3_BASE_ADDR, MV64260_LO_DECODE_DISABLE)
    YES_INIT_MV64260_REG (CPUIF_SNOOP3_TOP_ADDR, MV64260_HI_DECODE_DISABLE)

    /*
     * "CPU Interface - Error Report Registers":
     * the first few are are status in nature and need not be initialized.
     */

    NOT_INIT_MV64260_REG (CPUIF_ERR_ADDR_HI, status)
    NOT_INIT_MV64260_REG (CPUIF_ERR_DATA_LO, status)
    NOT_INIT_MV64260_REG (CPUIF_ERR_DATA_HI, status)
    NOT_INIT_MV64260_REG (CPUIF_ERR_PRTY, status)
    NOT_INIT_MV64260_REG (CPUIF_ERR_CAUSE, status)

    /* Mask all the CPU Errors, we're not really interested in them */

    YES_INIT_MV64260_REG (CPUIF_ERR_MASK, MASK_ALL)

    /* The following register is read to unlatch the error */

    READ_INIT_MV64260_REG (CPUIF_ERR_ADDR_LO)

    /*
     * "PCI Interface - PCI Slave Address Decoding Registers":
     * Program PCIIF BAR sizes to zero - a benign state in which they
     * do not participate in any address transactions.  Note that the
     * _SCSx_ registers will optionally be reprogrammed dynamically in
     * sysMv64260PhbInit().
     */

    YES_INIT_MV64260_REG (PCIIF_SCS0_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS0_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS1_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS1_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS2_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS2_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS3_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS3_BAR_SZ_PCI1, 0)

    YES_INIT_MV64260_REG (PCIIF_CS0_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_CS0_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_CS1_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_CS1_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_CS2_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_CS2_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_CS3_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_CS3_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_BOOTCS_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_BOOTCS_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM0_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM0_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM1_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM1_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_IO_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_IO_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_CPU_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_CPU_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS0_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS0_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS1_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS1_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS2_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS2_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS3_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS3_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS0_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS0_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS1_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS1_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS2_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS2_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS3_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS3_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_BOOTCS_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_BOOTCS_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM0_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM0_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM1_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM1_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CPU_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CPU_BAR_SZ_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_EXP_ROM_BAR_SZ_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_EXP_ROM_BAR_SZ_PCI1, 0)

    /*
     * Disable all PCI source interfaces, individual interfaces will
     * be individually enabled in sysMv64260PhbInit().  Note the
     * testing revealed that HW prevents us from clearing bit 9 (memory
     * mapped internal registers BAR enable), all the other bits
     * can be set, disabling the associated interface.
     */

    YES_INIT_MV64260_REG (PCIIF_BASE_ADDR_REG_ENB_PCI0, 0xffffffff)
    YES_INIT_MV64260_REG (PCIIF_BASE_ADDR_REG_ENB_PCI1, 0xffffffff)

    /*
     * The PCIIF (PCI source) remap (PCIIF_SCSx...) registers will
     * be programmed with zero here and then the appropriate remap
     * registers will later be dynamically programmed in sysMv64260PhbInit()
     * to facilitate a 1:1 mapping between PCI memory bus and DRAM.
     *
     */

    YES_INIT_MV64260_REG (PCIIF_SCS0_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS0_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS1_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS1_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS2_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS2_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS3_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_SCS3_BASE_ADDR_RMAP_PCI1, 0)

    /*
     * The remaining PCI source reamp registers represent interfaces
     * that are not enabled.  They will all be programmed with a
     * default value of zero.
     */

    YES_INIT_MV64260_REG (PCIIF_CS0_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_CS0_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_CS1_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_CS1_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_CS2_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_CS2_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_CS3_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_CS3_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_BOOTCS_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_BOOTCS_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM0_BASE_ADDR_RMAP_LO_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM0_BASE_ADDR_RMAP_LO_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM0_BASE_ADDR_RMAP_HI_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM0_BASE_ADDR_RMAP_HI_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM1_BASE_ADDR_RMAP_LO_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM1_BASE_ADDR_RMAP_LO_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM1_BASE_ADDR_RMAP_HI_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_MEM1_BASE_ADDR_RMAP_HI_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_IO_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_P2P_IO_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_CPU_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_CPU_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS0_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS0_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS1_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS1_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS2_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS2_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS3_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_SCS3_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS0_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS0_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS1_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS1_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS2_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS2_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS3_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CS3_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_BOOTCS_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_BOOTCS_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM0_BASE_ADDR_RMAP_LO_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM0_BASE_ADDR_RMAP_LO_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM0_BASE_ADDR_RMAP_HI_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM0_BASE_ADDR_RMAP_HI_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM1_BASE_ADDR_RMAP_LO_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM1_BASE_ADDR_RMAP_LO_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM1_BASE_ADDR_RMAP_HI_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_P2P_MEM1_BASE_ADDR_RMAP_HI_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CPU_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_DAC_CPU_BASE_ADDR_RMAP_PCI1, 0)
    YES_INIT_MV64260_REG (PCIIF_EXP_ROM_BASE_ADDR_RMAP_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_EXP_ROM_BASE_ADDR_RMAP_PCI1, 0)

    /*
     * Program the PCI access decode control registers.
     * The bits are:
     *
     *   Bit:          Value:	Meaning:
     *   0 (RemapWrDis)   0     Writes to a BAR will result in a write
     *                            to the remap register to retain 1:1 mapping
     *   1 (ExpRomDev)    0     Expansion ROM device = CS[3]* (irrelevant)
     *   2 (VpdDev)       0     VPD device is CS[3]* (irrelevant)
     *   3 (MsgAcc)       1     Messaging unit registers are accessible only
     *                            as part of the GT-64260 internal space
     *   7:4 (Reserved)   0
     *   24:8 (VPDHighAddr) 0   VPD high address bits (irrelevant)
     *   31:25 (Reserved) 0
     */

    YES_INIT_MV64260_REG (PCIIF_ADDR_DECODE_CTRL_PCI0, (1<<3))
    YES_INIT_MV64260_REG (PCIIF_ADDR_DECODE_CTRL_PCI1, (1<<3))

    /* "PCI Interface - PCI Control Registers":
     *
     * The PCI command register is explained below:
     *
     * Bit  Name     setting/meaning
     *
     * 0   MByteSwap  0
     *                    PCI Master Byte Swap When set to '0', the
     *                    GT-64260A PCI master swaps the bytes of the
     *                    incoming and outgoing PCI data (swap the 8 bytes
     *                    of a long-word).  NOTE: GT-64120 and GT-64130
     *                    compatible.
     *
     * 1   EnAggrPre  0
     *			  Aggressive prefetch, when set to '0' aggressive
     *                    prefetch is disabled.
     *
     * 2   Reserved   0
     *                    must be 0.
     *
     * 3   Reserved   0
     *                    Read Only.
     *
     * 4   MWrCom     1
     *                    PCI Master Write Combine Enable When set to '1',
     *                    write combining is enabled.
     *
     * 5   MRdCom     1
     *                    PCI Master Read Combine Enable When set to '1',
     *                    read combining is enabled.
     *
     * 6   MWrTrig    1
     *                    PCI Master Write Trigger
     *                      0 - Accesses the PCI bus only when the whole
     *                        burst is written into the master write buffer.
     *                      1 - Accesses the PCI bus when the first data is
     *                        written into the master write buffer.
     *
     * 7   MRdTrig    0
     *                    PCI Master Read Trigger
     *                      0 - Returns read data to the initiating unit only
     *                            when the whole burst is written into master
     *                            read buffer.
     *                      1 - Returns read data to the initiating unit when
     *                            the first read data is written into master
     *                            read buffer.
     *
     * 8   MRdLine    1
     *                    PCI Master Memory Read Line Enable
     *                      0 - Disable
     *                      1 - Enable
     *
     * 9   MRdMul     1
     *                    PCI Master Memory Read Multiple Enable
     *                      0 - Disable
     *                      1 - Enable
     *
     * 10  MWordSwap  0
     *                    PCI Master Word Swap
     *                      When set to '1', the GT-64260A PCI master swaps
     *                      the 32-bit words of the incoming and outgoing PCI
     *                      data.
     *
     * 11  SWordSwap  0
     *                    PCI Slave Word Swap
     *                      When set to '1', the GT-64260A PCI slave swaps
     *                      the 32-bit words of the incoming and outgoing
     *                      PCI data.  NOTE: GT-64120 and GT-64130
     *                      compatible.
     *
     * 12  Reserved   1
     *                 Reserved.
     *
     * 13  SBDis      0
     *                    PCI Slave Sync Barrier Disable
     *                      When set to '1', the PCI configuration read
     *                      transaction will stop act as sync barrier
     *                      transaction.
     *
     * 14  Reserved   0
     *                    Must be 0 0x0
     *
     * 15  MReq64     1
     *                    PCI Master REQ64* Enable
     *                      0 - Disable
     *                      1 - Enable
     *
     * 16  SByteSwap  0
     *                    PCI Slave Byte Swap
     *                      When set to '0', the GT-64260A PCI slave swaps
     *                      the bytes of the incoming and outgoing PCI data
     *                      (swap the 8 bytes of a long-word).  Sampled at
     *                      reset on AD[4]
     *
     * 17  MDACEn     1
     *                    PCI Master DAC Enable
     *                      0 - Disable
     *                        The PCI master never drives the DAC cycle.
     *                      1 - Enable
     *                        In case the upper 32-bit address is not '0',
     *                        the PCI master drives the DAC cycle.
     *
     * 18  M64Allign  1
     *                    PCI Master REQ64* assertion on non-aligned
     *                      0 - Disable
     *                        The master asserts REQ64* only if the address
     *                        is 64-bit aligned.
     *                      1 - Enable
     *                        The master asserts REQ64* even if the address
     *                        is not 64-bit aligned.
     *
     * 19  PErrProp   0
     *                    Parity/ECC Errors Propagation Enable
     *                      0 - Disable
     *                        The PCI interface always drives correct parity
     *                        on the PAR signal.
     *                      1 - Enable
     *                        In case of slave read bad ECC from SDRAM, or
     *                        master write with bad parity/ECC indication
     *                        from the initiator, the PCI interface drives
     *                        bad parity on the PAR signal.
     *
     * 20  SSwapEn    0
     *                    PCI Slave Swap Enable
     *                      0 - PCI slave data swapping is determined via
     *                        SByteSwap and SWordSwap bits (bits 16 and 11),
     *                        as in the GT-64120/130.
     *                      1 - PCI slave data swapping is determined via
     *                        PCISwap bits [25:24] in the PCI Access Control
     *                        registers.  NOTE: Even if the SSwapEn bit is
     *                        set to '1' and the PCI address does not match
     *                        any of the Access Control registers, slave data
     *                        swapping works according to SByteSwap and
     *                        SWordSwap bits.
     *
     * 21  MSwapEn    0
     *                    PCI Master Swap Enable
     *                      0 - PCI master data swapping is determined via
     *                        MByteSwap and MWordSwap bits (bits 0 and 10), as
     *                        in the GT-64120/130.
     *                      1 - PCI master data swapping is determined via
     *                        PCISwap bits in CPU to PCI Address Decoding
     *                        registers.
     *
     * 22  MIntSwapEn 0
     *                    PCI Master Configuration Transactions Data Swap Enable
     *                      0 - Disable
     *                        The PCI master configuration transaction to the
     *                        PCI bus is always in Little Endian convention.
     *                      1 - Enable
     *                        The PCI master configuration transaction to the
     *                        PCI bus is determined according to the setting
     *                        of MSwapEn bit.
     * 23  LBEn       0
     *                    PCI Loop Backup Enable
     *                      0 - Disable
     *                        The PCI slave does not respond to transactions
     *                        initiated by the PCI master.
     *                      1 - Enable
     *                        The PCI slave does respond to transactions
     *                        initiated by the PCI master, if targeted to the
     *                        slave (address match).
     *                        NOTE: Reserved for Galileo Technology usage.
     *
     * 26:24 SIntSwap 0
     *                    PCI Slave data swap control on PCI accesses to the
     *                        GT-64260A internal and configuration registers.
     *                        Bits encoding are the same as bits[26:24] in
     *                        PCI Access Control registers.
     *
     * 27    Reserved 0
     *                    Must be 0.
     * 31:28 Reserved 0
     *                    Read only.
     */

    YES_INIT_MV64260_REG (PCIIF_PCI_CMD_PCI0, 0x00069370)
    YES_INIT_MV64260_REG (PCIIF_PCI_CMD_PCI1, 0x00069370)

    /*
     * The PCI mode registers serve as status indicators and need
     * not be programmed.
     */

    NOT_INIT_MV64260_REG (PCIIF_MODE_PCI0, status)
    NOT_INIT_MV64260_REG (PCIIF_MODE_PCI1, status)

    /*
     * The PCI timeout and retry register is set for (1) the maximum number
     * of PClk cycles the GT-64260 slave holds the PCI bus before terminating
     * a transaction with RETRY (bits 7:0 = 0xff), (2) maximum number of PClk
     * cycles the GT-64260 slave holds the the PCI bus before terminating
     * a transaction with DISCONNECT (bits 15:8 = 0xff), (3) "retry forever"
     * (bits 23:16 = 0x00)
     */

    YES_INIT_MV64260_REG (PCIIF_TMOUT_RTRY_PCI0, 0x0000ffff)
    YES_INIT_MV64260_REG (PCIIF_TMOUT_RTRY_PCI1, 0x0000ffff)

    /*
     * The PCI read buffer discard timer is set to maximum value
     * (bits 15:0 = 0xffff) and each of the eight read buffers is enabled
     * (bits 23:16) = 0xff.
     */

    YES_INIT_MV64260_REG (PCIIF_DISCARD_TMR_PCI0, 0x00ffffff)
    YES_INIT_MV64260_REG (PCIIF_DISCARD_TMR_PCI1, 0x00ffffff)

    /*
     * PCI MSI trigger timer register is set to its default value indicating
     * the number to TClk cycles between consecutive MSI requests
     * (bits 15:0 = 0xff).  Note that this BSP does not use MSI (Message
     * Signaled Interrupts)
     */

    YES_INIT_MV64260_REG (PCIIF_MSI_TRGR_TMR_PCI0, 0x0000ffff)
    YES_INIT_MV64260_REG (PCIIF_MSI_TRGR_TMR_PCI1, 0x0000ffff)

    /*
     * PCI arbiter control will be programmed to the default value of zero.
     * Note that the programming of all bits (other than bit 31) in this
     * register is moot since PCI arbitration is performed by an external PLD
     * arbiter.  Programming bit 31 to zero disables the GT-64260 arbiter.
     */

    YES_INIT_MV64260_REG (PCIIF_ARB_CTRL_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_ARB_CTRL_PCI1, 0)

    /*
     * Program the low and high PCI interface crossbar control registers
     *	  0x0 = Reserved
     *    0x1 = Reserved
     *    0x2 = CPU access
     *    0x3 = PCI0 - null request, PCI1 - PCI0 access
     *    0x4 = PCI0 - PCI1 access, PCI1 - null request
     *    0x5 = Comm unit access
     *    0x6 = IDMA channels 0/1/2/3 access
     *    0x7 - 0xf Reserved
     *
     * Each 4-bit nibble takes one of the above values.  We will
     * program to the default value.
     */

    YES_INIT_MV64260_REG (PCIIF_IF_XBAR_CTRL_LO_PCI0, 0x33376542)
    YES_INIT_MV64260_REG (PCIIF_IF_XBAR_CTRL_HI_PCI0, 0x33376542)

    YES_INIT_MV64260_REG (PCIIF_IF_XBAR_CTRL_LO_PCI1, 0x44476532)
    YES_INIT_MV64260_REG (PCIIF_IF_XBAR_CTRL_HI_PCI1, 0x44476532)

    /*
     * PCI interface crossbar timeout registers will be programmed for
     * timeout disabled (but 16 = 1), and maximum timeout (bits 7:0 = 0xff).
     */

    YES_INIT_MV64260_REG (PCIIF_IF_XBAR_TMOUT_PCI0, 0x000100ff)
    YES_INIT_MV64260_REG (PCIIF_IF_XBAR_TMOUT_PCI1, 0x000100ff)

    /*
     * Program PCI read response crossbar control registers
     *	  0x0 = SDRAM read data
     *    0x1 = Device read data
     *    0x2 = CPU read data
     *    0x3 = PCI0 - null, PCI1 - PCI0 read data
     *    0x4 = PCI0 - PCI1 read data, PCI0 - null
     *    0x5 = Comm unit internal registers read data
     *    0x6 = IDMA channels 0/1/2/3 internal registers read data
     *    0x7 = IDMA channels 4/5/6/7 internal registers read data
     *    0x8 - 0xf Reserved
     *
     * Each 4-bit nibble takes one of the above values.  We will
     * program to the default value.
     */

    YES_INIT_MV64260_REG (PCIIF_READ_RESP_XBAR_CTRL_LO_PCI0, 0x37654210)
    YES_INIT_MV64260_REG (PCIIF_READ_RESP_XBAR_CTRL_HI_PCI0, 0x37654210)

    YES_INIT_MV64260_REG (PCIIF_READ_RESP_XBAR_CTRL_LO_PCI1, 0x47653210)
    YES_INIT_MV64260_REG (PCIIF_READ_RESP_XBAR_CTRL_HI_PCI1, 0x47653210)

    /*
     * The PCI Sync Barrier Virtual registers are read only and thus
     * need not be programmed.
     */

    NOT_INIT_MV64260_REG (PCIIF_SYNC_BARRIER_PCI0, readOnly)
    NOT_INIT_MV64260_REG (PCIIF_SYNC_BARRIER_PCI1, readOnly)

    /*
     * The PCI P2P configuration registers are left in their default state
     * which disables P2P transactions.  Ditto for the PCI P2P swap control
     * registers.
     */

    YES_INIT_MV64260_REG (PCIIF_P2P_CFG_PCI0, 0x000000ff)
    YES_INIT_MV64260_REG (PCIIF_P2P_CFG_PCI1, 0x000000ff)

    YES_INIT_MV64260_REG (PCIIF_P2P_SWAP_CTRL_PCI0, 0x00119999)
    YES_INIT_MV64260_REG (PCIIF_P2P_SWAP_CTRL_PCI1, 0x00119999)

    /*
     * The PCIIF_ACCESS_CTRL_BASE[0-7]_LO_PCI_[0-1],
     *     PCIIF_ACCESS_CTRL_BASE[0-7]_HI_PCI[0-1], and
     *     PCIIF_ACCESS_CTRL_TOP[4-7]_PCI[0-1] registers are all
     * programmed into a "disabled" state.  Only the [0-3] registers
     * are subject to later dynamic programming inside of
     * sysMv64260PhbInit().
     */

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE0_LO_PCI0,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP4_PCI0,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE0_HI_PCI0, 0)


    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE1_LO_PCI0,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP1_PCI0,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE1_HI_PCI0, 0)


    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE2_LO_PCI0,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP2_PCI0,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE2_HI_PCI0, 0)


    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE3_LO_PCI0,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP3_PCI0,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE3_HI_PCI0, 0)


    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE4_LO_PCI0,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP4_PCI0,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE4_HI_PCI0, 0)


    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE5_LO_PCI0,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP5_PCI0,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE5_HI_PCI0, 0)


    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE5_LO_PCI1,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP5_PCI1,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE5_HI_PCI1, 0)


    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE6_LO_PCI0,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP6_PCI0,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE6_HI_PCI0, 0)


    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE6_LO_PCI1,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP6_PCI1,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE6_HI_PCI1, 0)


    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE7_LO_PCI0,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP7_PCI0,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE7_HI_PCI0, 0)


    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE7_LO_PCI1,
			  MV64260_PCI_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_TOP7_PCI1,
			  MV64260_PCI_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_ACCESS_CTRL_BASE7_HI_PCI1, 0)

    /*
     * "PCI Interface - PCI Snoop Control Registers":
     * They will be disabled here but later will be dynamically
     * enabled inside of sysMv64260PhbInit().  The "lo base" registers
     * will be programmed with MV64260_LO_DECODE_DISABLE, "hi base"
     * registers will be programmed with zero since we do not deal
     * directly with 64-bit addressing in this BSP.  The "top base"
     * registers will be programmed with MV64260_HI_DECODE_DISABLE.
     */

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE0_LO_PCI0,
			  MV64260_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE0_LO_PCI1,
			  MV64260_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE0_HI_PCI0, 0)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE0_HI_PCI1, 0)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_TOP0_PCI0,
			  MV64260_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_TOP0_PCI1,
			  MV64260_HI_DECODE_DISABLE)


    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE1_LO_PCI0,
			  MV64260_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE1_LO_PCI1,
			  MV64260_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE1_HI_PCI0, 0)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE1_HI_PCI1, 0)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_TOP1_PCI0,
			  MV64260_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_TOP1_PCI1,
			  MV64260_HI_DECODE_DISABLE)


    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE2_LO_PCI0,
			  MV64260_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE2_LO_PCI1,
			  MV64260_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE2_HI_PCI0, 0)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE2_HI_PCI1, 0)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_TOP2_PCI0,
			  MV64260_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_TOP2_PCI1,
			  MV64260_HI_DECODE_DISABLE)


    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE3_LO_PCI0,
			  MV64260_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE3_LO_PCI1,
			  MV64260_LO_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE3_HI_PCI0, 0)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_BASE3_HI_PCI1, 0)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_TOP3_PCI0,
			  MV64260_HI_DECODE_DISABLE)

    YES_INIT_MV64260_REG (PCIIF_SNOOP_CTRL_TOP3_PCI1,
			  MV64260_HI_DECODE_DISABLE)

    /*
     * "PCI Interface - PCI Configuration Access Registers":
     * The following "PCI configuration address and data" do not need
     * initialization.  They are used to generate PCI configuration access
     * cycles.
     */

    NOT_INIT_MV64260_REG (PCIIF_CFG_ADDR_PCI0, noInit)
    NOT_INIT_MV64260_REG (PCIIF_CFG_ADDR_PCI1, noInit)
    NOT_INIT_MV64260_REG (PCIIF_CFG_DATA_PCI0, noInit)
    NOT_INIT_MV64260_REG (PCIIF_CFG_DATA_PCI1, noInit)

    /*
     * The PCI interrupt acknowledge and PCI sync barrier virtual registers
     * are read-only and do not need initialization.
     */

    NOT_INIT_MV64260_REG (PCIIF_INT_ACK_PCI0, readOnly)
    NOT_INIT_MV64260_REG (PCIIF_INT_ACK_PCI1, readOnly)

    /*
     * "PCI Interface - PCI Error Report Registers":
     * The PCI SERR* mask registers will be set to mask all conditions,
     * ditto for the PCI error mask registers.  This is the default
     * setting for these registers.
     */

    YES_INIT_MV64260_REG (PCIIF_SERR_MASK_PCI0, MASK_ALL)
    YES_INIT_MV64260_REG (PCIIF_SERR_MASK_PCI1, MASK_ALL)

    YES_INIT_MV64260_REG (PCIIF_ERR_MASK_PCI0, MASK_ALL)
    YES_INIT_MV64260_REG (PCIIF_ERR_MASK_PCI1, MASK_ALL)

    /*
     * The PCI error address "low" registers are read to unlatch the data
     * currently being held.  The associated "high" address registers
     * need not be programmed at all as they reflect status information.
     * A similar situation holds for the PCI error data "low" and "high"
     * registers.
     */

    READ_INIT_MV64260_REG (PCIIF_ERR_ADDR_LO_PCI0)
    NOT_INIT_MV64260_REG (PCIIF_ERR_ADDR_HI_PCI0, status)

    READ_INIT_MV64260_REG (PCIIF_ERR_ADDR_LO_PCI1)
    NOT_INIT_MV64260_REG (PCIIF_ERR_ADDR_HI_PCI1, status)

    NOT_INIT_MV64260_REG (PCIIF_ERR_DATA_LO_PCI0, status)
    NOT_INIT_MV64260_REG (PCIIF_ERR_DATA_HI_PCI0, status)

    NOT_INIT_MV64260_REG (PCIIF_ERR_DATA_LO_PCI1, status)
    NOT_INIT_MV64260_REG (PCIIF_ERR_DATA_HI_PCI1, status)

    /*
     * We will zero out the PCI error cause registers, reading of the
     * PCI error low address registers previously should have unlatched
     * the data.
     */

    YES_INIT_MV64260_REG (PCIIF_ERR_CAUSE_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_ERR_CAUSE_PCI1, 0)

    /*
     * The PCI error command registers will be set to zero (the default
     * value) indicating to "latch everything" (e.g. PCI byte enable
     * error condition, PCI PAR error condition, PCI PAR64 error condition).
     * We don't use these registers in the BSP and the user is free to
     * modify it as he wishes.
     */

    YES_INIT_MV64260_REG (PCIIF_ERR_CMD_PCI0, 0)
    YES_INIT_MV64260_REG (PCIIF_ERR_CMD_PCI1, 0)

    /*
     * Some of the "PCI configurable" registers in the function headers
     * (function 0 thru function 7) of the GT-64260 can be programmed to
     * zero here.  The  _SCS[0-3]_ set will be later dynamically
     * programmed inside of sysMv64260PhbInit() to allow PCI access
     * of DRAM.
     */

    /*
     * Configure the device/vendor ID register. Note that is not a read-only
     * register and could be changed if desired.  It is important to have
     * a recognizable device/vendor ID here since this ID will be used
     * to identify the device as one which we do not PCI autoconfigure.
     */

    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_DEV_AND_VEND_ID_PCICFG, PCI_ID_MV64260)

    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_DEV_AND_VEND_ID_PCICFG, PCI_ID_MV64260)

    /* Disable the bus, memory, I/O, special cycle mastering */

    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_STATUS_AND_CMD_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_STATUS_AND_CMD_PCICFG, 0)

    /*
     * The following register contains the 1-byte fields for revision ID,
     * class code, subclass code, and programming.  We want to initialize
     * these bytes but NOT initialize the revision ID byte (since we don't
     * know what to set it to). Thus this register will not be statically
     * initialized here but instead will be handled inside of
     * sysMv64260PhbInit().
     */

    NOT_CFG_MV64260_REG (PCI_FUNC0,
   		         PCIIF_FUNC0_CLASS_CODE_AND_REV_ID_PCICFG,
			 doLater)

    NOT_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_CLASS_CODE_AND_REV_ID_PCICFG,
			     doLater)

    /* Program the latency and cache line size */

    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_BIST_LAT_CACHE_PCICFG, 0x80800000)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_BIST_LAT_CACHE_PCICFG, 0x80800000)

    /*
     * Here we disable the _SCSx_ registers but we will dynamically
     * program them inside of sysMv64260PhbInit().
     */

    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_SCS0_BASE_ADDR_PCICFG, 0x00000008)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_SCS0_BASE_ADDR_PCICFG, 0x00000008)
    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_SCS1_BASE_ADDR_PCICFG, 0x00000008)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_SCS1_BASE_ADDR_PCICFG, 0x00000008)
    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_SCS2_BASE_ADDR_PCICFG, 0x00000008)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_SCS2_BASE_ADDR_PCICFG, 0x00000008)
    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_SCS3_BASE_ADDR_PCICFG, 0x00000008)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_SCS3_BASE_ADDR_PCICFG, 0x00000008)

    /*
     * Do not program the memory mapped internal BAR.  We will be
     * programming this ouselves inside of sysHwInit() immediately
     * after the PCI autoconfiguration operation is complete.  We
     * don't want to wipe out the value that the bootrom placed into
     * this register during pciAutoConfig().
     */

    NOT_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_INT_REG_MEM_MAPPED_BASE_ADDR_PCICFG, x)
    NOT_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_INT_REG_MEM_MAPPED_BASE_ADDR_PCICFG, x)

    /* I/O BAR will remain umpapped */

    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_INT_REG_IO_MAPPED_BASE_ADDR_PCICFG,
			 PCI_BASE_IO)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_INT_REG_IO_MAPPED_BASE_ADDR_PCICFG,
			     PCI_BASE_IO)

    /*
     * As with similar registers we will not touch the subsystem ID and
     * subsystem Vendor ID registers.
     */

    NOT_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_SUBSYS_DEV_AND_VEND_ID_PCICFG,
			 leaveDefault)

    NOT_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_SUBSYS_DEV_AND_VEND_ID_PCICFG,
			     leaveDefault)

    /*
     * Expansion ROM BAR cannot be changed so we will not attempt
     * to do it here.  Ditto for  capability list pointer.
     */

    NOT_CFG_MV64260_REG (PCI_FUNC0, PCIIF_FUNC0_EXP_ROM_BASE_ADDR_PCICFG,
			 cantChange)

    NOT_CFG_MV64260_REG_ALT (PCI_FUNC0, PCIIF_FUNC0_EXP_ROM_BASE_ADDR_PCICFG,
			     cantChange)

    NOT_CFG_MV64260_REG (PCI_FUNC0, PCIIF_FUNC0_CAPBLTY_LIST_PTR_PCICFG,
			 cantChange)

    NOT_CFG_MV64260_REG_ALT (PCI_FUNC0, PCIIF_FUNC0_CAPBLTY_LIST_PTR_PCICFG,
			     cantChange)

    /* "Default" intPin and intLine */

    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_INT_PIN_AND_LINE_PCICFG, 0x00000100)

    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_INT_PIN_AND_LINE_PCICFG, 0x00000100)

    /*
     * Power management capability register cannot be changed so we will
     * not attempt to do it here.
     */

    NOT_CFG_MV64260_REG (PCI_FUNC0, PCIIF_FUNC0_PWR_MGMT_CAPBLTY_PCICFG,
			 cantChange)

    NOT_CFG_MV64260_REG_ALT (PCI_FUNC0, PCIIF_FUNC0_PWR_MGMT_CAPBLTY_PCICFG,
			     cantChange)

    /* Continue clearing */

    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_PWR_MGMT_STATUS_AND_CTRL_PCICFG, 0)

    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_PWR_MGMT_STATUS_AND_CTRL_PCICFG, 0)

    /* VPD registers cannot be changed */

    NOT_CFG_MV64260_REG (PCI_FUNC0, PCIIF_FUNC0_VPD_ADDR_PCICFG, cantChange)
    NOT_CFG_MV64260_REG_ALT (PCI_FUNC0, PCIIF_FUNC0_VPD_ADDR_PCICFG, cantChange)

    /* Continue clearing */

    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_VPD_DATA_PCICFG, 0)

    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_VPD_DATA_PCICFG, 0)

    /* MSI capability register cannot be changed */

    NOT_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_MSI_MSG_CTRL_PCICFG, cantChange)

    NOT_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_MSI_MSG_CTRL_PCICFG, cantChange)

    /* Continue clearing */

    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_MSI_MSG_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_MSI_MSG_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_MSI_MSG_UPR_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_MSI_MSG_UPR_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_MSG_DATA_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_MSG_DATA_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC0,
			 PCIIF_FUNC0_CPCI_HSWAP_CAPBLTY_PCICFG, 0x00000006)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC0,
			     PCIIF_FUNC0_CPCI_HSWAP_CAPBLTY_PCICFG, 0x00000006)
    YES_CFG_MV64260_REG (PCI_FUNC1,
			 PCIIF_FUNC1_CS0_BASE_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC1,
			     PCIIF_FUNC1_CS0_BASE_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC1,
			 PCIIF_FUNC1_CS1_BASE_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC1,
			     PCIIF_FUNC1_CS1_BASE_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC1,
			 PCIIF_FUNC1_CS2_BASE_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC1,
			     PCIIF_FUNC1_CS2_BASE_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC1,
			 PCIIF_FUNC1_CS3_BASE_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC1,
			     PCIIF_FUNC1_CS3_BASE_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC1,
			 PCIIF_FUNC1_BOOTCS_BASE_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC1,
			     PCIIF_FUNC1_BOOTCS_BASE_ADDR_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC2, PCIIF_FUNC2_P2P_MEM0_BASE_ADDR_PCICFG, 
			 PCI_BASE_PREFETCH)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC2, PCIIF_FUNC2_P2P_MEM0_BASE_ADDR_PCICFG, 
			     PCI_BASE_PREFETCH)
    YES_CFG_MV64260_REG (PCI_FUNC2, PCIIF_FUNC2_P2P_MEM1_BASE_ADDR_PCICFG, 
			 PCI_BASE_PREFETCH)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC2, PCIIF_FUNC2_P2P_MEM1_BASE_ADDR_PCICFG, 
			     PCI_BASE_PREFETCH)
    YES_CFG_MV64260_REG (PCI_FUNC2,
			 PCIIF_FUNC2_P2P_IO_BASE_ADDR_PCICFG, PCI_BASE_IO)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC2,
			     PCIIF_FUNC2_P2P_IO_BASE_ADDR_PCICFG, PCI_BASE_IO)
    YES_CFG_MV64260_REG (PCI_FUNC2,
			 PCIIF_FUNC2_CPU_BASE_ADDR_PCICFG, 
			 PCI_BASE_PREFETCH)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC2,
			     PCIIF_FUNC2_CPU_BASE_ADDR_PCICFG, 
			     PCI_BASE_PREFETCH)
    YES_CFG_MV64260_REG (PCI_FUNC4,
			 PCIIF_FUNC4_DAC_SCS0_BASE_ADDR_LO_PCICFG, 
			 0x0000000c)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC4,
			     PCIIF_FUNC4_DAC_SCS0_BASE_ADDR_LO_PCICFG, 
			     0x0000000c)
    YES_CFG_MV64260_REG (PCI_FUNC4,
			 PCIIF_FUNC4_DAC_SCS0_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC4,
			     PCIIF_FUNC4_DAC_SCS0_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC4,
			 PCIIF_FUNC4_DAC_SCS1_BASE_ADDR_LO_PCICFG, 
			 0x0000000c)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC4,
			     PCIIF_FUNC4_DAC_SCS1_BASE_ADDR_LO_PCICFG, 
			     0x0000000c)
    YES_CFG_MV64260_REG (PCI_FUNC4,
			 PCIIF_FUNC4_DAC_SCS1_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC4,
			     PCIIF_FUNC4_DAC_SCS1_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC4,
			 PCIIF_FUNC4_DAC_P2P_MEM0_BASE_ADDR_LO_PCICFG, 
			 0x0000000c)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC4,
			     PCIIF_FUNC4_DAC_P2P_MEM0_BASE_ADDR_LO_PCICFG, 
			     0x0000000c)
    YES_CFG_MV64260_REG (PCI_FUNC4,
			 PCIIF_FUNC4_DAC_P2P_MEM0_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC4,
			     PCIIF_FUNC4_DAC_P2P_MEM0_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC5,
			 PCIIF_FUNC5_DAC_SCS2_BASE_ADDR_LO_PCICFG, 
			 0x0000000c)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC5,
			     PCIIF_FUNC5_DAC_SCS2_BASE_ADDR_LO_PCICFG, 
			     0x0000000c)
    YES_CFG_MV64260_REG (PCI_FUNC5,
			 PCIIF_FUNC5_DAC_SCS2_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC5,
			     PCIIF_FUNC5_DAC_SCS2_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC5,
			 PCIIF_FUNC5_DAC_SCS3_BASE_ADDR_LO_PCICFG, 
			 0x0000000c)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC5,
			     PCIIF_FUNC5_DAC_SCS3_BASE_ADDR_LO_PCICFG, 
			     0x0000000c)
    YES_CFG_MV64260_REG (PCI_FUNC5,
			 PCIIF_FUNC5_DAC_SCS3_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC5,
			     PCIIF_FUNC5_DAC_SCS3_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC5,
			 PCIIF_FUNC5_DAC_P2P_MEM1_BASE_ADDR_LO_PCICFG, 
			 0x0000000c)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC5,
			     PCIIF_FUNC5_DAC_P2P_MEM1_BASE_ADDR_LO_PCICFG,
			     0x0000000c)
    YES_CFG_MV64260_REG (PCI_FUNC5,
			 PCIIF_FUNC5_DAC_P2P_MEM1_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC5,
			     PCIIF_FUNC5_DAC_P2P_MEM1_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC6,
			 PCIIF_FUNC6_DAC_CS0_BASE_ADDR_LO_PCICFG, 0x00000004)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC6,
			     PCIIF_FUNC6_DAC_CS0_BASE_ADDR_LO_PCICFG, 0x00000004)
    YES_CFG_MV64260_REG (PCI_FUNC6,
			 PCIIF_FUNC6_DAC_CS0_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC6,
			     PCIIF_FUNC6_DAC_CS0_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC6,
			 PCIIF_FUNC6_DAC_CS1_BASE_ADDR_LO_PCICFG, 0x00000004)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC6,
			     PCIIF_FUNC6_DAC_CS1_BASE_ADDR_LO_PCICFG, 0x00000004)
    YES_CFG_MV64260_REG (PCI_FUNC6,
			 PCIIF_FUNC6_DAC_CS1_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC6,
			     PCIIF_FUNC6_DAC_CS1_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC6,
			 PCIIF_FUNC6_DAC_CS2_BASE_ADDR_LO_PCICFG, 0x00000004)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC6,
			     PCIIF_FUNC6_DAC_CS2_BASE_ADDR_LO_PCICFG, 0x00000004)
    YES_CFG_MV64260_REG (PCI_FUNC6,
			 PCIIF_FUNC6_DAC_CS2_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC6,
			     PCIIF_FUNC6_DAC_CS2_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC7,
			 PCIIF_FUNC7_DAC_CS3_BASE_ADDR_LO_PCICFG, 
			 0x00000004)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC7,
			     PCIIF_FUNC7_DAC_CS3_BASE_ADDR_LO_PCICFG, 
			     0x00000004)
    YES_CFG_MV64260_REG (PCI_FUNC7,
			 PCIIF_FUNC7_DAC_CS3_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC7,
			     PCIIF_FUNC7_DAC_CS3_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC7,
			 PCIIF_FUNC7_DAC_BOOTCS_BASE_ADDR_LO_PCICFG, 
			 0x00000004)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC7,
			     PCIIF_FUNC7_DAC_BOOTCS_BASE_ADDR_LO_PCICFG, 
			     0x00000004)
    YES_CFG_MV64260_REG (PCI_FUNC7,
			 PCIIF_FUNC7_DAC_BOOTCS_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC7,
			     PCIIF_FUNC7_DAC_BOOTCS_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG (PCI_FUNC7,
			 PCIIF_FUNC7_DAC_CPU_BASE_ADDR_LO_PCICFG, 
			 0x0000000c)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC7,
			     PCIIF_FUNC7_DAC_CPU_BASE_ADDR_LO_PCICFG, 
			     0x0000000c)
    YES_CFG_MV64260_REG (PCI_FUNC7,
			 PCIIF_FUNC7_DAC_CPU_BASE_ADDR_HI_PCICFG, 0)
    YES_CFG_MV64260_REG_ALT (PCI_FUNC7,
			     PCIIF_FUNC7_DAC_CPU_BASE_ADDR_HI_PCICFG, 0)
    /*
     * "Messaging Unit - Messaging Unit Registers":
     * We don't use the MUIF (Message Unit Interface) in this BSP.  We can
     * program all of the registers to zero which is the default case
     * for most of them.  Those for which zero is not the default programming
     * are listed below:
     *
     *   MUIF_INBND_INT_MASK_PCI0                            0x1c28
     *   MUIF_INBND_INT_MASK_PCI1                            0x1cA8
     *       Inbound interrupt mask registers.  Several bits are set
     *       by default in these registers.  For example, bit 0 is set
     *       to '1' by default and the documentation explains that if set
     *       to '1', the inbound message 0 interrupt is enabled.  There
     *       are several other interrupts in these registers which are
     *       all set to '1' by default - we will be setting them to
     *       zero to mask the associated interrupts.  Bits 15:6 are
     *       reserved and set to 0x3ff so we will preserve
     *       this setting but set the rest of the register to zero.
     *
     *   MUIF_OUTBND_INT_MASK_PCI0                           0x1C34
     *   MUIF_OUTBND_INT_MASK_PCI1                           0x1CB4
     *       A similar situation applies to these registers.  Bits
     *       15:4 are reserved and set to 0xfff.
     *
     *   MUIF_INBND_Q_PORT_VIRT_PCI0
     *   MUIF_INBND_Q_PORT_VIRT_PCI1
     *   MUIF_OUTBND_Q_PORT_VIRT_PCI0
     *   MUIF_OUTBND_Q_PORT_VIRT_PCI1
     *       These registers are left as is.  They are not static
     *       registers in that accessing these registers initiates
     *       a write to an inbound or outbound queue.  We should
     *       avoid this operation during initialization.
     */

    YES_INIT_MV64260_REG (MUIF_INBND_MSG0_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_INBND_MSG0_PCI1 ,0)
    YES_INIT_MV64260_REG (MUIF_INBND_MSG1_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_INBND_MSG1_PCI1 ,0)

    YES_INIT_MV64260_REG (MUIF_OUTBND_MSG0_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_OUTBND_MSG0_PCI1 ,0)
    YES_INIT_MV64260_REG (MUIF_OUTBND_MSG1_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_OUTBND_MSG1_PCI1 ,0)

    YES_INIT_MV64260_REG (MUIF_INBND_DBELL_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_INBND_DBELL_PCI1 ,0)

    /*
     * The bits below to which '1' is written are "write '1' to clear"
     * bits.  The remaining bits are reserved or read only.  Note that
     * these registers will not read back the same value that we
     * write into them.
     */

    YES_INIT_MV64260_REG (MUIF_INBND_INT_CAUSE_PCI0 ,((1<<4) | 1<<5) | (1<<16))
    YES_INIT_MV64260_REG (MUIF_INBND_INT_CAUSE_PCI1 ,((1<<4) | 1<<5) | (1<<16))

    /*
     * The bits below to which zero are written disables the associated
     * interrupt.  The '1's are written to the reserved field.
     */

    YES_INIT_MV64260_REG (MUIF_INBND_INT_MASK_PCI0 ,(MASK_ALL | 0x0003ffff))
    YES_INIT_MV64260_REG (MUIF_INBND_INT_MASK_PCI1 ,(MASK_ALL | 0x0003ffff))

    YES_INIT_MV64260_REG (MUIF_OUTBND_DBELL_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_OUTBND_DBELL_PCI1 ,0)

    NOT_INIT_MV64260_REG (MUIF_OUTBND_INT_CAUSE_PCI0 ,readOnly)
    NOT_INIT_MV64260_REG (MUIF_OUTBND_INT_CAUSE_PCI1 ,readOnly)

    /*
     * The bits below to which zero are written disables the associated
     * interrupt.  The '1's are written to the reserved field.
     */

    YES_INIT_MV64260_REG (MUIF_OUTBND_INT_MASK_PCI0 ,(MASK_ALL | 0x0003ffff))
    YES_INIT_MV64260_REG (MUIF_OUTBND_INT_MASK_PCI1 ,(MASK_ALL | 0x0003ffff))

    NOT_INIT_MV64260_REG (MUIF_INBND_Q_PORT_VIRT_PCI0 ,0)
    NOT_INIT_MV64260_REG (MUIF_INBND_Q_PORT_VIRT_PCI1 ,0)

    NOT_INIT_MV64260_REG (MUIF_OUTBND_Q_PORT_VIRT_PCI0 ,0)
    NOT_INIT_MV64260_REG (MUIF_OUTBND_Q_PORT_VIRT_PCI1 ,0)

    YES_INIT_MV64260_REG (MUIF_Q_CTRL_PCI0 ,(1<<1))
    YES_INIT_MV64260_REG (MUIF_Q_CTRL_PCI1 ,(1<<1))

    YES_INIT_MV64260_REG (MUIF_Q_BASE_ADDR_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_Q_BASE_ADDR_PCI1 ,0)

    YES_INIT_MV64260_REG (MUIF_INBND_FREE_HEAD_PTR_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_INBND_FREE_HEAD_PTR_PCI1 ,0)
    YES_INIT_MV64260_REG (MUIF_INBND_FREE_TAIL_PTR_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_INBND_FREE_TAIL_PTR_PCI1 ,0)

    YES_INIT_MV64260_REG (MUIF_INBND_POST_HEAD_PTR_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_INBND_POST_HEAD_PTR_PCI1 ,0)
    YES_INIT_MV64260_REG (MUIF_INBND_POST_TAIL_PTR_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_INBND_POST_TAIL_PTR_PCI1 ,0)

    YES_INIT_MV64260_REG (MUIF_OUTBND_FREE_HEAD_PTR_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_OUTBND_FREE_HEAD_PTR_PCI1 ,0)
    YES_INIT_MV64260_REG (MUIF_OUTBND_FREE_TAIL_PTR_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_OUTBND_FREE_TAIL_PTR_PCI1 ,0)

    YES_INIT_MV64260_REG (MUIF_OUTBND_POST_HEAD_PTR_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_OUTBND_POST_HEAD_PTR_PCI1 ,0)
    YES_INIT_MV64260_REG (MUIF_OUTBND_POST_TAIL_PTR_PCI0 ,0)
    YES_INIT_MV64260_REG (MUIF_OUTBND_POST_TAIL_PTR_PCI1 ,0)

    /*
     * "Timer/Counters Registers":
     * The timer/counter registers can be initialized to the default values
     * of zero or "all masked" or simply not programmed at all in the
     * case of the "cause" registers.
     */

    YES_INIT_MV64260_REG (TMR_CNTR0, 0)
    YES_INIT_MV64260_REG (TMR_CNTR1, 0)
    YES_INIT_MV64260_REG (TMR_CNTR2, 0)
    YES_INIT_MV64260_REG (TMR_CNTR3, 0)
    YES_INIT_MV64260_REG (TMR_CNTR4, 0)
    YES_INIT_MV64260_REG (TMR_CNTR5, 0)
    YES_INIT_MV64260_REG (TMR_CNTR6, 0)
    YES_INIT_MV64260_REG (TMR_CNTR7, 0)
    YES_INIT_MV64260_REG (TMR_CNTR_CTRL_0_3, 0)
    YES_INIT_MV64260_REG (TMR_CNTR_CTRL_4_7, 0)

    NOT_INIT_MV64260_REG (TMR_CNTR_INT_CAUSE_0_3, status)
    NOT_INIT_MV64260_REG (TMR_CNTR_INT_CAUSE_4_7, status)

    YES_INIT_MV64260_REG (TMR_CNTR_INT_MASK_0_3, MASK_ALL)
    YES_INIT_MV64260_REG (TMR_CNTR_INT_MASK_4_7, MASK_ALL)

    /*
     * "Communication Unit Registers:
     * Those communication unit registers not dealing with the ethernet
     * "..._ENET[0-2]_..." will be programmed here.  The programming will be
     * to the default value except for bit 10, which we will set to
     * indicate level sensitive GPP interrupts.
     */

    YES_INIT_MV64260_REG (CU_MPSC0_ADDR_CTRL_LO, 0x10101010)
    YES_INIT_MV64260_REG (CU_MPSC0_ADDR_CTRL_HI, 0)
    YES_INIT_MV64260_REG (CU_MPSC0_RCV_BUF_PCI_HI_ADDR, 0)
    YES_INIT_MV64260_REG (CU_MPSC0_RCV_DESC_PCI_HI_ADDR, 0)
    YES_INIT_MV64260_REG (CU_MPSC0_XMIT_BUF_PCI_HI_ADDR, 0)
    YES_INIT_MV64260_REG (CU_MPSC0_XMIT_DESC_PCI_HI_ADDR, 0)
    YES_INIT_MV64260_REG (CU_MPSC1_ADDR_CTRL_LO, 0x10101010)
    YES_INIT_MV64260_REG (CU_MPSC1_ADDR_CTRL_HI, 0)
    YES_INIT_MV64260_REG (CU_MPSC1_RCV_BUF_PCI_HI_ADDR, 0)
    YES_INIT_MV64260_REG (CU_MPSC1_RCV_DESC_PCI_HI_ADDR, 0)
    YES_INIT_MV64260_REG (CU_MPSC1_XMIT_BUF_PCI_HI_ADDR, 0)
    YES_INIT_MV64260_REG (CU_MPSC1_XMIT_DESC_PCI_HI_ADDR, 0)

    YES_INIT_MV64260_REG (CU_COMM_UNIT_ARB_CTRL, 0x11FF0400)

    /*
     * An errata states that on Rev B of the GT-64260A, some of the internal
     * registers do not read back correctly. Testing reveals this to be
     * the case and those that read back incorrectly are documented with
     * "rb 0xffffffff" indicating that they read back as 0xffffffff.
     */

    YES_INIT_MV64260_REG (CU_COMM_UNIT_CFG, 0x80000000) /* rb 0xffffffff */

    /* Note: setting bit 16 (below), disables timeout */

    YES_INIT_MV64260_REG (CU_COMM_UNIT_XBAR_TMOUT, 0x000100ff)

    NOT_INIT_MV64260_REG (CU_COMM_UNIT_INT_CAUSE, status)
    YES_INIT_MV64260_REG (CU_COMM_UNIT_INT_MASK, MASK_ALL)

    /* Unlatch error address by reading */

    READ_INIT_MV64260_REG (CU_COMM_UNIT_ERR_ADDR)

    /*
     * "Multi Protocol Serial Controller":
     * We do not use the MPSC (Multi-Protocol Serial Controller) feature
     * of the GT64260-A.  We will either program the register to its
     * default value as documented in the GT_64260 spec or not program it
     * at all, as is done for the protocol dependent channel registers.
     * The critical register is the MPSC routing register which is
     * programmed first in the set below.  Setting both bit sets 2:0 and
     * 8:6 to 0x7 disconnects the MPSC0 and MPSC1 routings.
     */

    YES_INIT_MV64260_REG (MPSC_ROUTG, 0x7fffffff ) /* rb 0xffffffff */

    YES_INIT_MV64260_REG (MPSC_RX_CLK_ROUTG, 0)	   /* rb 0xffffffff */
    YES_INIT_MV64260_REG (MPSC_TX_CLK_ROUTG, 0)	   /* rb 0xffffffff */

    NOT_INIT_MV64260_REG (MPSC_MPSC0_CAUSE, status)
    YES_INIT_MV64260_REG (MPSC_MPSC0_MASK, MASK_ALL)	   /* rb 0xffffffff */
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CAUSE, status)
    YES_INIT_MV64260_REG (MPSC_MPSC1_MASK, MASK_ALL)	   /* rb 0xffffffff */

    YES_INIT_MV64260_REG (MPSC_MPSC0_MAIN_CFG_LO, (1 << 24)) /* rb 0xffffffff */
    YES_INIT_MV64260_REG (MPSC_MPSC0_MAIN_CFG_HI, 0)         /* rb 0xffffffff */
    YES_INIT_MV64260_REG (MPSC_MPSC0_PTCL_CFG, (1 << 12))    /* rb 0xffffffff */
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG1, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG2, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG3, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG4, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG5, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG6, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG7, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG8, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG9, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG10, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC0_CHAN0_REG11, protocolDependent)

    YES_INIT_MV64260_REG (MPSC_MPSC1_MAIN_CFG_LO, (1 << 24)) /* rb 0xffffffff */
    YES_INIT_MV64260_REG (MPSC_MPSC1_MAIN_CFG_HI, 0)         /* rb 0xffffffff */
    YES_INIT_MV64260_REG (MPSC_MPSC1_PTCL_CFG, (1 << 12))    /* rb 0xffffffff */
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG1, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG2, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG3, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG4, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG5, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG6, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG7, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG8, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG9, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG10, protocolDependent)
    NOT_INIT_MV64260_REG (MPSC_MPSC1_CHAN1_REG11, protocolDependent)

    /*
     * "Multi Protocol Serial Controller Serial DMAs":
     * Since we are not using the MPSC units, the MPSCSDMA (MPSC Serial DMA)
     * related registers will not be used either.  The registers will
     * be programmed to their advertised default values, or not programmed
     * at all if they are status in nature.
     */

    YES_INIT_MV64260_REG (MPSCSDMA_CHAN0_CFG, 0x000000fc)
    YES_INIT_MV64260_REG (MPSCSDMA_CHAN0_CMD, 0)
    YES_INIT_MV64260_REG (MPSCSDMA_CHAN0_CURR_RX_DESC_PTR, 0)
    YES_INIT_MV64260_REG (MPSCSDMA_CHAN0_CURR_TX_DESC_PTR, 0)
    YES_INIT_MV64260_REG (MPSCSDMA_CHAN0_FIRST_TX_DESC_PTR, 0)

    YES_INIT_MV64260_REG (MPSCSDMA_CHAN1_CFG, 0x000000fc)
    YES_INIT_MV64260_REG (MPSCSDMA_CHAN1_CMD, 0)
    YES_INIT_MV64260_REG (MPSCSDMA_CHAN1_CURR_RX_DESC_PTR, 0)
    YES_INIT_MV64260_REG (MPSCSDMA_CHAN1_CURR_TX_DESC_PTR, 0)
    YES_INIT_MV64260_REG (MPSCSDMA_CHAN1_FIRST_TX_DESC_PTR, 0)

    YES_INIT_MV64260_REG (MPSCSDMA_MASK, MASK_ALL)	/* rb 0xffffffff */
    NOT_INIT_MV64260_REG (MPSCSDMA_CAUSE, status)

    /*
     * "Baude Rate Generators":
     * The baud-rate generator registers are not touched.  We do not
     * use the GT-64260A baud rate generator.
     */

    NOT_INIT_MV64260_REG (BRG0_BAUDE_TUNING, notUsed)
    NOT_INIT_MV64260_REG (BRG0_CFG, notUsed)
    NOT_INIT_MV64260_REG (BRG1_BAUDE_TUNING, notUsed)
    NOT_INIT_MV64260_REG (BRG1_CFG, notUsed)
    NOT_INIT_MV64260_REG (BRG_CAUSE, notUsed)
    NOT_INIT_MV64260_REG (BRG_MASK, notUsed)

    /*
     * "Serial ROM Initialization Registers":
     * We do not fiddle with the serial SROM initialization registers
     * shown below, they are handled by HW.
     */

    NOT_INIT_MV64260_REG (RC_SERIAL_INIT_PCI_HI_ADDR, HW)
    NOT_INIT_MV64260_REG (RC_SERIAL_INIT_LAST_DATA, HW)
    NOT_INIT_MV64260_REG (RC_SERIAL_INIT_CTRL, HW)
    NOT_INIT_MV64260_REG (RC_SERIAL_INIT_STATUS, HW)

    /* Finally, we're at the end of the table */

    END_INIT_MV64260_REG

    };

/* forward declarations */

void sysMv64260PhbInit (void);

/******************************************************************************
*
* sysMv64260PhbInit - Dynamic initialization of Mv64260 host bridge interface
*
* This routine performs the dynamic initialization of the Mv64260 host
* bridge support interface registers.  Dynamic programming is required if
* the programming value must be computed at run time or if programming
* the register requires special action such as polling.
*
* RETURNS: NA
*/

void
sysMv64260PhbInit(void)
{
  int      i;
  UINT32   reg1;
  UINT32   reg2;
  UINT32   size;
  UINT32   top = 0;

  /*
   * This following structure holds the register offsets for those
   * registers which must be dynamically accessed and/or programmed.
   */

  struct regParams	
  {
	UINT32 cpuifScsxLoDecodeAddr;
	UINT32 cpuifScsxHiDecodeAddr;
	UINT32 pciifAccessCtrlBasexLoPci0;
	UINT32 pciifAccessCtrlBasexLoPci1;
	UINT32 pciifAccessCtrlBasexHiPci0;
	UINT32 pciifAccessCtrlBasexHiPci1;
	UINT32 pciifAccessCtrlTopxPci0;
	UINT32 pciifAccessCtrlTopxPci1;
	UINT32 pciifSnoopCtrlBasexLoPci0;
	UINT32 pciifSnoopCtrlBasexLoPci1;
	UINT32 pciifSnoopCtrlBasexHiPci0;
	UINT32 pciifSnoopCtrlBasexHiPci1;
	UINT32 pciifSnoopCtrlTopxPci0;
	UINT32 pciifSnoopCtrlTopxPci1;
	UINT32 pciifFuncxScsxBaseAddrPcicfg;
	UINT32 pciifFuncxScsxBaseAddrPcicfgAlt;
	UINT32 pciifScsxBarSzPci0;
	UINT32 pciifScsxBarSzPci1;
	UINT32 pciifScsxBaseAddrRmapPci0;
	UINT32 pciifScsxBaseAddrRmapPci1;
	UINT32 pciifBaseAddrRegEnbScsxBit;
  } r[4] =
	{
	  {
	    CPUIF_SCS0_LO_DECODE_ADDR,
		CPUIF_SCS0_HI_DECODE_ADDR,
	    PCIIF_ACCESS_CTRL_BASE0_LO_PCI0,
		PCIIF_ACCESS_CTRL_BASE0_LO_PCI1,
		PCIIF_ACCESS_CTRL_BASE0_HI_PCI0,
		PCIIF_ACCESS_CTRL_BASE0_HI_PCI1,
		PCIIF_ACCESS_CTRL_TOP0_PCI0,
		PCIIF_ACCESS_CTRL_TOP0_PCI1,
		PCIIF_SNOOP_CTRL_BASE0_LO_PCI0,
		PCIIF_SNOOP_CTRL_BASE0_LO_PCI1,
		PCIIF_SNOOP_CTRL_BASE0_HI_PCI0,
		PCIIF_SNOOP_CTRL_BASE0_HI_PCI1,
		PCIIF_SNOOP_CTRL_TOP0_PCI0,
		PCIIF_SNOOP_CTRL_TOP0_PCI1,
		PCIIF_FUNC0_SCS0_BASE_ADDR_PCICFG,
		PCIIF_FUNC0_SCS0_BASE_ADDR_PCICFG_ALT,
		PCIIF_SCS0_BAR_SZ_PCI0,
		PCIIF_SCS0_BAR_SZ_PCI1,
		PCIIF_SCS0_BASE_ADDR_RMAP_PCI0,
		PCIIF_SCS0_BASE_ADDR_RMAP_PCI1,
		PCIIF_BASE_ADDR_REG_ENB_SCS0_BIT
	  },
	  {
	    CPUIF_SCS1_LO_DECODE_ADDR,
		CPUIF_SCS1_HI_DECODE_ADDR,
	    PCIIF_ACCESS_CTRL_BASE1_LO_PCI0,
		PCIIF_ACCESS_CTRL_BASE1_LO_PCI1,
		PCIIF_ACCESS_CTRL_BASE1_HI_PCI0,
		PCIIF_ACCESS_CTRL_BASE1_HI_PCI1,
		PCIIF_ACCESS_CTRL_TOP1_PCI0,
		PCIIF_ACCESS_CTRL_TOP1_PCI1,
		PCIIF_SNOOP_CTRL_BASE1_LO_PCI0,
		PCIIF_SNOOP_CTRL_BASE1_LO_PCI1,
		PCIIF_SNOOP_CTRL_BASE1_HI_PCI0,
		PCIIF_SNOOP_CTRL_BASE1_HI_PCI1,
		PCIIF_SNOOP_CTRL_TOP1_PCI0,
		PCIIF_SNOOP_CTRL_TOP1_PCI1,
		PCIIF_FUNC0_SCS1_BASE_ADDR_PCICFG,
		PCIIF_FUNC0_SCS1_BASE_ADDR_PCICFG_ALT,
		PCIIF_SCS1_BAR_SZ_PCI0,
		PCIIF_SCS1_BAR_SZ_PCI1,
		PCIIF_SCS1_BASE_ADDR_RMAP_PCI0,
		PCIIF_SCS1_BASE_ADDR_RMAP_PCI1,
		PCIIF_BASE_ADDR_REG_ENB_SCS1_BIT
	  },
	  {
	    CPUIF_SCS2_LO_DECODE_ADDR,
		CPUIF_SCS2_HI_DECODE_ADDR,
	    PCIIF_ACCESS_CTRL_BASE2_LO_PCI0,
		PCIIF_ACCESS_CTRL_BASE2_LO_PCI1,
		PCIIF_ACCESS_CTRL_BASE2_HI_PCI0,
		PCIIF_ACCESS_CTRL_BASE2_HI_PCI1,
		PCIIF_ACCESS_CTRL_TOP2_PCI0,
		PCIIF_ACCESS_CTRL_TOP2_PCI1,
		PCIIF_SNOOP_CTRL_BASE2_LO_PCI0,
		PCIIF_SNOOP_CTRL_BASE2_LO_PCI1,
		PCIIF_SNOOP_CTRL_BASE2_HI_PCI0,
		PCIIF_SNOOP_CTRL_BASE2_HI_PCI1,
		PCIIF_SNOOP_CTRL_TOP2_PCI0,
		PCIIF_SNOOP_CTRL_TOP2_PCI1,
		PCIIF_FUNC0_SCS2_BASE_ADDR_PCICFG,
		PCIIF_FUNC0_SCS2_BASE_ADDR_PCICFG_ALT,
		PCIIF_SCS2_BAR_SZ_PCI0,
		PCIIF_SCS2_BAR_SZ_PCI1,
		PCIIF_SCS2_BASE_ADDR_RMAP_PCI0,
		PCIIF_SCS2_BASE_ADDR_RMAP_PCI1,
		PCIIF_BASE_ADDR_REG_ENB_SCS2_BIT
	  },
	  {
	    CPUIF_SCS3_LO_DECODE_ADDR,
		CPUIF_SCS3_HI_DECODE_ADDR,
	    PCIIF_ACCESS_CTRL_BASE3_LO_PCI0,
		PCIIF_ACCESS_CTRL_BASE3_LO_PCI1,
		PCIIF_ACCESS_CTRL_BASE3_HI_PCI0,
		PCIIF_ACCESS_CTRL_BASE3_HI_PCI1,
		PCIIF_ACCESS_CTRL_TOP3_PCI0,
		PCIIF_ACCESS_CTRL_TOP3_PCI1,
		PCIIF_SNOOP_CTRL_BASE3_LO_PCI0,
		PCIIF_SNOOP_CTRL_BASE3_LO_PCI1,
		PCIIF_SNOOP_CTRL_BASE3_HI_PCI0,
		PCIIF_SNOOP_CTRL_BASE3_HI_PCI1,
		PCIIF_SNOOP_CTRL_TOP3_PCI0,
		PCIIF_SNOOP_CTRL_TOP3_PCI1,
		PCIIF_FUNC0_SCS3_BASE_ADDR_PCICFG,
		PCIIF_FUNC0_SCS3_BASE_ADDR_PCICFG_ALT,
		PCIIF_SCS3_BAR_SZ_PCI0,
		PCIIF_SCS3_BAR_SZ_PCI1,
		PCIIF_SCS3_BASE_ADDR_RMAP_PCI0,
		PCIIF_SCS3_BASE_ADDR_RMAP_PCI1,
		PCIIF_BASE_ADDR_REG_ENB_SCS3_BIT
	  }
	};
		
  /* Initialize static device control parameters */

  sysMv64260RegInit (mv64260PhbTbl);

  /*
   * Program the class code - necessary to do here since only three bytes
   * of the 4-byte register are affected (programming interface,
   * sub class code, class code) but we leave the revision ID alone
   */

  sysPciConfigWrite (0, 0, PCI_FUNC0,	
                     PCI_CFG_PROGRAMMING_IF, 1,
		             PCI_PROGRAMMING_IF_MV64260);

  sysPciConfigWrite (0, 0, PCI_FUNC0,	
                     PCI_CFG_SUBCLASS, 1, PCI_SUBCLASS_MV64260);

  sysPciConfigWrite (0, 0, PCI_FUNC0,	
                     PCI_CFG_CLASS, 1, PCI_CLASS_MV64260);

  /* Now do the same for the PCI1 interface */

  sysPciConfigWriteAlt (0, 0, PCI_FUNC0,	
                        PCI_CFG_PROGRAMMING_IF, 1,
			            PCI_PROGRAMMING_IF_MV64260);

  sysPciConfigWriteAlt (0, 0, PCI_FUNC0,	
                        PCI_CFG_SUBCLASS, 1, PCI_SUBCLASS_MV64260);

  sysPciConfigWriteAlt (0, 0, PCI_FUNC0,	
                        PCI_CFG_CLASS, 1, PCI_CLASS_MV64260);

  /*
   * Special care must be taken in programming the CPU configuration
   * register as outlined in the GT-64260 spec, section 4.19
   * "Programming the CPU Configuration Register".
   */

  reg1 = MV64260_READ32 (MV64260_REG_BASE, CPUIF_CPU_CFG);
  reg1 = reg2 = CPUIF_CPU_CFG_VAL;
  MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_CPU_CFG, reg1)
  while ((reg1 = MV64260_READ32 (MV64260_REG_BASE, CPUIF_CPU_CFG)) !=
         reg2);

  /*
   * Program those registers which require dynamic calculations
   * Start with the PCI to CPU registers.  Here is a list of
   * registers which deal with PCI to SCS0 mapping.  Note that
   * similar registers exist for PCI to SCS1, SCS2, SCS3 mappings.
   *
   * Dynamically programmed:
   *   PCIIF_ACCESS_CTRL_BASE0_LO_PCI0               0x1E00
   *   PCIIF_ACCESS_CTRL_BASE0_HI_PCI0               0x1E04
   *   PCIIF_ACCESS_CTRL_TOP0_PCI0		             0x1E08
   *   PCIIF_SNOOP_CTRL_BASE0_LO_PCI0                0x1F00
   *   PCIIF_SNOOP_CTRL_BASE0_HI_PCI0                0x1F04
   *   PCIIF_SNOOP_CTRL_TOP0_PCI0                    0x1F08
   *   PCIIF_FUNC0_SCS0_BASE_ADDR_PCICFG             0x10
   *   PCIIF_FUNC0_SCS0_BASE_ADDR_PCICFG_ALT         0x90
   *   PCIIF_SCS0_BAR_SZ_PCI0                        0xC08
   *   PCIIF_SCS0_BAR_SZ_PCI1                        0xC88
   *   PCIIF_SCS0_BASE_ADDR_RMAP_PCI0                0xC48
   *   PCIIF_SCS0_BASE_ADDR_RMAP_PCI1                0xCC8
   *   PCIIF_BASE_ADDR_REG_ENB_PCI0		             0xC3C		
   *   PCIIF_BASE_ADDR_REG_ENB_PCI1		             0xCBC
   *
   * Statically programmed to not be involved in address translation
   *   PCIIF_DAC_SCS0_BAR_SZ_PCI0                    0xE00
   *   PCIIF_DAC_SCS0_BAR_SZ_PCI1                    0xE80
   *   PCIIF_DAC_SCS0_BASE_ADDR_RMAP_PCI0            0xF00
   *   PCIIF_DAC_SCS0_BASE_ADDR_RMAP_PCI1            0xF80
   *   PCIIF_FUNC4_DAC_SCS0_BASE_ADDR_LO_PCICFG      0x10
   *   PCIIF_FUNC4_DAC_SCS0_BASE_ADDR_LO_PCICFG_ALT  0x90
   *   PCIIF_FUNC4_DAC_SCS0_BASE_ADDR_HI_PCICFG      0x14
   *   PCIIF_FUNC4_DAC_SCS0_BASE_ADDR_HI_PCICFG_ALT  0x94
   *
   * We will be programming PCI slave access such that the PCI memory
   * address range from zero thru x where "x" is numerically
   * equivalent to the top of DRAM will map straight to DRAM, e.g.
   * a PCI bus memory access of 0x100000 will be routed to access
   * DRAM at 0x100000.  To do this we will initialize the "dynamically
   * programmed" registers mentioned above in such a way that they
   * route PCI memory access to DRAM the same way that the following DRAM
   * chip select registers do:
   *
   *    CPUIF_SCS0_LO_DECODE_ADDR (0x008)
   *    CPUIF_SCS0_HI_DECODE_ADDR (0x010)
   *
   * This process will be repeated four times for the SCS[x] (x=0,1,2,3)
   * registers.  Note than before entering this code we have already
   * statically programmed the PCI slave access related registers in
   * such a way that all accesses are disabled.  Thus we only need
   * to "enable" those sets which correspond to enabled "enabled"
   * CPUIF_SCSx... sets.  For each register set which we program to
   * be "enabled", we must clear the appropriate bit (putting that
   * bit in the "enabled" state) in the PCIIF_BASE_ADDR_REG_ENB_PCI0
   * and PCIIF_BASE_ADDR_REG_ENB_PCI1 registers.
   *
   */


*(UINT32 *)0x10000000 = 0x12345678;

  for (i=0; i<=3; i++)
  {
	reg1 = MV64260_READ32 (MV64260_REG_BASE, r[i].cpuifScsxLoDecodeAddr);
	reg2 = MV64260_READ32 (MV64260_REG_BASE, r[i].cpuifScsxHiDecodeAddr);

*(UINT32 *)(0x10000004+0x100*i) = i+1;  /* 1, 2, 3, 4 */
*(UINT32 *)(0x10000008+0x100*i) = reg1; /* 0x0, 0x100, 0xfff, 0xfff */
*(UINT32 *)(0x1000000c+0x100*i) = reg2; /* 0xff, 0x1ff, 0x0, 0x0 */

	/* If the decoder is not disabled ... */

	if(((reg1 == MV64260_LO_DECODE_DISABLE) &&
	    (reg2 == MV64260_HI_DECODE_DISABLE)) == FALSE)
    {
      /* Obtain the size of the target DRAM window */

      size = MV64260_CPU_ADDR_WNDW_SZ(reg1, reg2);

      /* Obtain the base address of the DRAM window */

      reg1 = MV64260_LO_DECODE_ADDR_INV(reg1);

      /* Keep track of top address encountered so far */

      if (top < (reg1 + size)) top = reg1 + size;

*(UINT32 *)(0x10000010+0x100*i) = 0x87654321;
*(UINT32 *)(0x10000014+0x100*i) = size; /* 0x10000000 , 0x10000000, 0, 0*/
*(UINT32 *)(0x10000018+0x100*i) = reg1; /* 0, 0x10000000, 0, 0 */
*(UINT32 *)(0x1000001c+0x100*i) = top; /* 0x10000000, 0x20000000, 0, 0 */

      /*
       * Now program the 'Base[0-3]' PCI access control registers to
	   * control access to this DRAM.  In addition to the encoded
       * "low" address, we are setting two bits in the 'Base[0-3]' low
       * access control register:
       *
       *   Bit 12 =    1, PCI read prefetch enable (single word).
       *   Bit 16 =    1, PCI read aggressive prefetch enable.
       *   Bit 17 =    1, PCI read line aggressive prefetch enable.
       *   Bit 18 =    1, PCI read multiple aggressive prefetch enable.
	   *   Bit 21:20 = 00, PCI max burst is 4 64-bit words.
       *                     Note this is necessary for 32-byte
       *                     cache lines - don't mess with this or you
       *                     will wreck things like gigabit ethernet !!!
       *   Bit 25:24 = 01, No swapping
       *
       * The 'Base[0-3]' PCI access control high register is programmed
	   * to zero since we are only dealing with 32-bit addresses.
       *
       * Finally the 'Top[0-3]' PCI access control top register is
	   * programmed with the encoded "top" address of this DRAM
       * segment.  We do the programming described above to both the
       * PCI0 and PCI1 registers.
       */

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				            r[i].pciifAccessCtrlBasexLoPci0,
                            (MV64260_LO_DECODE_ADDR(reg1) |
				   (1<<12) | (1<<16) | (1<<17) | (1<<18) |
				   (0x0 << 20) | (01<<24)));

*(UINT32 *)(0x10000020+0x100*i) = (MV64260_LO_DECODE_ADDR(reg1) |
				   (1<<12) | (1<<16) | (1<<17) | (1<<18) |
				   (0x0 << 20) | (01<<24)); /* */

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifAccessCtrlBasexLoPci1,
                                  (MV64260_LO_DECODE_ADDR(reg1) |
				   (1<<12) | (1<<16) | (1<<17) | (1<<18) |
				   (0x0 << 20) | (01<<24)));

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                                  r[i].pciifAccessCtrlBasexHiPci0, 0);

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
                                  r[i].pciifAccessCtrlBasexHiPci1, 0);

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifAccessCtrlTopxPci0,
                                  (MV64260_HI_DECODE_ADDR(reg1, size)));

*(UINT32 *)(0x10000024+0x100*i) = MV64260_HI_DECODE_ADDR(reg1, size);

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifAccessCtrlTopxPci1,
                                  (MV64260_HI_DECODE_ADDR(reg1, size)));

      /*
       * In a manner analogous to the 'Base[0-3]' and 'Top[0-3]' PCI
	   * access control registers, we will program associated PCI
	   * snoop control registers.  Setting bits 13:12 to '10' in the
	   * 'low' 'Base[0-3]' register indicates 'snoop to WB
       * (write-back) region.
       */

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifSnoopCtrlBasexLoPci0,
                                  (MV64260_LO_DECODE_ADDR(reg1) | (2<<12)));

*(UINT32 *)(0x10000028+0x100*i) = (MV64260_LO_DECODE_ADDR(reg1) | (2<<12));

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifSnoopCtrlBasexLoPci1,
                                  (MV64260_LO_DECODE_ADDR(reg1) | (2<<12)));

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifSnoopCtrlBasexHiPci0, 0);

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifSnoopCtrlBasexHiPci1, 0);


      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifSnoopCtrlTopxPci0,
                                  (MV64260_HI_DECODE_ADDR(reg1, size)));

*(UINT32 *)(0x1000002c+0x100*i) = MV64260_HI_DECODE_ADDR(reg1, size);

      MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifSnoopCtrlTopxPci1,
                                  (MV64260_HI_DECODE_ADDR(reg1, size)));

      /* Write the base address into both PCI0 and PCI1 BARs */

      sysPciConfigWrite (0, 0, PCI_FUNC0,
                               r[i].pciifFuncxScsxBaseAddrPcicfg,
                               MV64260_REG_SIZE, (reg1 | PCI_BASE_PREFETCH));

*(UINT32 *)(0x10000030+0x100*i) = (reg1 | PCI_BASE_PREFETCH);

/* Sergey's test
if(i==1)
{
      sysPciConfigWrite (0, 0, PCI_FUNC0,
                               r[i].pciifFuncxScsxBaseAddrPcicfg,
                               MV64260_REG_SIZE, 0xa0000008);
}
*/



      sysPciConfigWriteAlt (0, 0, PCI_FUNC0,
                                  r[i].pciifFuncxScsxBaseAddrPcicfgAlt,
                                  MV64260_REG_SIZE,
			          (reg1 | PCI_BASE_PREFETCH));

      /* Program the BAR sizes for both PCI0 and PCI1 */

      MV64260_WRITE32_PUSH (MV64260_REG_BASE, r[i].pciifScsxBarSzPci0,
                                  MV64260_PCI_BAR_SZ(size));

*(UINT32 *)(0x10000034+0x100*i) = MV64260_PCI_BAR_SZ(size);

      MV64260_WRITE32_PUSH (MV64260_REG_BASE, r[i].pciifScsxBarSzPci1,
                                  MV64260_PCI_BAR_SZ(size));

     /*
      * Since we are doing a null translation, we can program
      * the remap register with the save base address in it that
	  * we used for the base address BAR.
	  */

	  MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifScsxBaseAddrRmapPci0,reg1);

*(UINT32 *)(0x10000038+0x100*i) = reg1;

	  MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  r[i].pciifScsxBaseAddrRmapPci1,reg1);

      /* Finally, enable the interface */

	  reg1 = MV64260_READ32 (MV64260_REG_BASE,
				   PCIIF_BASE_ADDR_REG_ENB_PCI0);
*(UINT32 *)(0x1000003c+0x100*i) = reg1;
            reg1 &= ~( 1 << r[i].pciifBaseAddrRegEnbScsxBit);
	  MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  PCIIF_BASE_ADDR_REG_ENB_PCI0, reg1);

*(UINT32 *)(0x10000040+0x100*i) = reg1;

	  reg1 = MV64260_READ32 (MV64260_REG_BASE,
				   PCIIF_BASE_ADDR_REG_ENB_PCI1);
            reg1 &= ~( 1 << r[i].pciifBaseAddrRegEnbScsxBit);
	  MV64260_WRITE32_PUSH (MV64260_REG_BASE,
				  PCIIF_BASE_ADDR_REG_ENB_PCI1, reg1);

    }
  }

  /*
   * Now we should have the "top" address computed so we can program
   * the first set (of the 8-set group) of "CPU Protect Access"
   * registers to allow CPU access from 0 through the top of DRAM.  The
   * other 7 sets are disabled (already done in the static table
   * programming).  Setting only the base address encoding into bits 11:0
   * and setting the rest of the register to zero allows:
   *  "Low CPU access protect programming":
   *     bit 16 = 0 -> CPU access allowed.
   *     bit 17 = 0 -> CPU write protect allowed.
   *     bit 18 = 0 -> CPU cacheing (block read) allowed.
   * The rest of the low CPU protect register is reserved and programmed
   * to zero.
   */

  MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_PTCT0_ADDR_LO,
  		  MV64260_LO_DECODE_ADDR(0));
*(UINT32 *)(0x10000044+0x100*i) = MV64260_LO_DECODE_ADDR(0);
  MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_PTCT0_ADDR_HI,
  		  MV64260_HI_DECODE_ADDR(0,top));
*(UINT32 *)(0x10000048+0x100*i) = MV64260_HI_DECODE_ADDR(0,top);

  /*
   * In a similar manner we will program set 0 of the 4-set Snoop Base
   * and Top registers.  Setting 0x2 in bits 17:16 of the Snoop Base
   * address registers signifies: "snoop to writeback region".
   */

  MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_SNOOP0_BASE_ADDR,
		        (MV64260_LO_DECODE_ADDR(0) | (2 << 16)));

*(UINT32 *)(0x1000004c+0x100*i) = (MV64260_LO_DECODE_ADDR(0) | (2 << 16));

  MV64260_WRITE32_PUSH (MV64260_REG_BASE, CPUIF_SNOOP0_TOP_ADDR,
  	          MV64260_HI_DECODE_ADDR(0,top));

*(UINT32 *)(0x10000050+0x100*i) = MV64260_HI_DECODE_ADDR(0,top);

   /*
    * Enable bus I/O access, memory access, bus mastering, monitoring of
    *	special cycles, and write and invalidate.
    */

  sysPciConfigWrite (0, 0, PCI_FUNC0,
                     PCIIF_FUNC0_STATUS_AND_CMD_PCICFG, MV64260_REG_SIZE,
                     (PCI_CMD_MEM_ENABLE | PCI_CMD_MASTER_ENABLE));

*(UINT32 *)(0x10000054+0x100*i) = (PCI_CMD_MEM_ENABLE | PCI_CMD_MASTER_ENABLE);

  sysPciConfigWriteAlt (0, 0, PCI_FUNC0,
                        PCIIF_FUNC0_STATUS_AND_CMD_PCICFG_ALT,
                        MV64260_REG_SIZE,
                        (PCI_CMD_MEM_ENABLE | PCI_CMD_MASTER_ENABLE));

*(unsigned int *)0x11000000 = PCI0_MSTR_MEM_LOCAL;
*(unsigned int *)0x11000004 = PCI0_MSTR_MEM_SIZE;
*(unsigned int *)0x11000008 = PCI0_MSTR_MEM_BUS;

}
