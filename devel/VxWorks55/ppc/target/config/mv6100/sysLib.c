/* sysLib.c - Motorola MVME6100 board series system-dependent library */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01iii,07dec04,cak Modifications in anticipation of porting to 6.0.
01hhh,02dec04,cak Added sysApplInit().
01ggg,20oct04,cak Added code to disable the TSi148's outbound mapping 
		  windows, when booting vxWorks via MotLoad.
01fff,16sep04,cak Replace INCLUDE_AUXCLK with INCLUDE_AUX_CLK to match the
		  Tornado Project facility.
01eee,03aug04,scb Added _func_errorHandlerInit.
01ddd,12jul04,cak Added support for PrPMCs.
01ccc,30jun04,cak Added support for the Maxim DS1621 digital thermometer
		  and thermostat.
01bbb,15jun04,cak Added support for all MV64360 Timer/Counters.
01aaa,07jun04,cak Added support for the MV64360 watchdog timer.
01zz,27may04,cak  Added support for programming a boot image in either of
		  the FLASH banks (boot block) from the VxWorks kernel. 
01yy,20may04,cak  Added FLASH interface driver support.
01xx,14may04,scb  Diab warnings.
01ww,05may04,scb  specialMem() support.
01vv,21apr04,scb  Special mvme6100 RMW busTas capability.
01uu,15apr04,cak  Add support for TFFS by enabling writing of the FLASH 
		  banks in system status register 2.
01tt,06apr04,scb  Fill in guts of sysVmeRmwTas().
01tt,06apr04,scb  Fill in guts of sysVmeRmw().
01ss,18may04,scb  Mods to support sysBusProbe().
01rr,12mar04,cak  Removed ethernet entries from sysPhysMemDesc, since the 
		  descriptors were moved into the integrated SRAM. Added
		  an entry to sysPhysMemDesc for the Integrated SRAM.
01qq,26feb04,cak  Fixed diab compiler warning.
01pp,25feb04,scb  Tempe interrupt support.
01oo,19feb04,scb  Fix sysPhysMemDesc[] ethernet descriptors for noncache.
01nn,03feb04,scb  Support for Shared-memory.
01mm,02feb04,cak  Fixed the "internal register set memory mapped BARs"
		  programming in sysHwInit() to properly access the
		  Disco II on the second PCI bus.
01ll,17dec03,cak  BSP update.
01kk,03sep03,scb  Call sysTempeReset().
01jj,03sep03,scb  Tempe PCI-X to VME bridge support support.
01ii,03mar03,cak  Updated processor support in sysModel() and
		  sysCpuCheck(), modifications necessary to allow access
		  to DRAM from the PCI bus, and IPMC SCSI Support.
01hh,05feb03,cak  Created a global cpuBusSpeed variable which is set in 
		  sysHwInit() with a call to sysCpuBusSpd() and replaced
		  all calls to sysCpuBusSpd() with this variable.
01gg,28jan03,cak  Replaced the reference to DEC_CLOCK_FREQ in sysUsDelay()
                  with a call to sysCpuBusSpd().
01ff,24jan03,cak  Added support for Apollo 7 processor.
01ee,10jan03,efb  Added MV64360 system controller ethernet support.
01dd,08jan03,cak  IPMC Serial Support.
01cc,30oct02,cak  Minor modifications to DMA support.
01bb,18oct02,cak  NVRAM/RTC/Failsafe support.
01aa,25sep02,cak  Added sysPciConfigWriteAlt routine for PHB support. 
01z,18sep02,cak  Added an L3 cache packet to temporary VPD programming.
01y,11sep02,cak  SMC support modifications as well as modifications to the
		 sysPhysMemDesc table. 
01x,05sep02,cak  Removed DEBUG_I2C and added code to write a "temporary"
		 VPD to the VPD EEPROM.
01w,03sep02,scb  Fix dynamic patch of sysPhysMemDesc[] to use new macros.
01v,26aug02,cak  Modifications for board bring-up as well as Tornado 2.1 to
		 2.2 conversion. 
01u,21aug02,scb  Propagate "interrupt" code review fixes from hxeb100.
01t,20aug02,scb  Fix some interrupt related issues gleaned from GT-64260 work.
01s,18jul02,yyz	 Added AuxClock driver support and initialization..
01r,11jul02,scb  Interrupt related mnemonic name changes.
01q,11jul02,scb  Adjustment to memory map for flash banks.
01p,26jun02,scb  Register mnemonic name changes.
01o,11Jun12,efb  Added DMA driver support and initialization.
01n,25jun02,scb  Removed some bit shift #defines (now in "mv64360.h").
01m,25jun02,cak  Added VPD support.
01l,14jun02,scb  Host bridge support.
01k,11jub02,cak  Re-ordered the L1, L2, and L3 cache disabling routines in
		 sysToMonitor.
01j,21may02,cak  Added support for PCI auto-configuration for both busses 0.0
		 and 1.0.
01i,17may02,cak  Added calls to sysMv64360SmcShow and sysMv64360SpdShow to
		 sysHwInit, and edited sysDramSize to calculate the top of
		 system memory by reading the chip select registers in the
		 MV64360.
01h,15may02,cak  Removed checks for INCLUDE_CACHE_SUPPORT from the cache
                 support.
01g,25apr02,cak  Added cache support.
01f,25apr02,scb  Mv64360 Device Bus Support.
01e,18apr02,cak  Added serial port support.
01d,15apr02,cak  Added the MV64360 internal registers to sysPhysMemDesc,
                 edited sysToMonitor, added Dynamic Power Management support,
                 and modified the Test-and-Set support routines. 
01c,10apr02,scb  Include interrupt support.
01b,02apr02,cak  Added code at end of sysHwInit() to turn off the board
                 fail LED. 
01a,07mar02,cak  Ported. (from ver 01p, mcp820/sysLib.c)
*/

/*
DESCRIPTION
This library provides board-specific routines.  The chip drivers included are:

    i8250Sio.c      - Intel 8250 UART driver 
    ppcDecTimer.c   - PowerPC decrementer timer library (system clock)
    byteNvRam.c     - byte-oriented generic non-volatile RAM library
    pciConfigLib.c  - PCI configuration library
    sysTempe.c	    - Tempe chip PCI-X to VME bridge support.

INCLUDE FILES: sysLib.h

SEE ALSO:
.pG "Configuration"
*/

/* includes */

#include "vxWorks.h"
#include "pci.h"
#include "memLib.h"
#include "cacheLib.h"
#include "sysLib.h"
#include "config.h"
#include "string.h"
#include "intLib.h"
#include "esf.h"
#include "excLib.h"
#include "logLib.h"
#include "taskLib.h"
#include "vxLib.h"
#include "tyLib.h"
#include "arch/ppc/archPpc.h"
#include "arch/ppc/mmu603Lib.h"
#include "arch/ppc/vxPpcLib.h"
#include "arch/ppc/excPpcLib.h"
#include "private/vmLibP.h"
#include "drv/pci/pciConfigLib.h"
#include "drv/pci/pciAutoConfigLib.h"
#include "end.h"
#include "errorHandler.h"
#include "mv64360.h"
#include "mv64360Smc.h"
#include "mv64360Dma.h"
#if ((defined INCLUDE_PCX7307_SIO) || (defined INCLUDE_Z85230_SIO))
#include "ns8730xSuperIo.h"
#endif /* INCLUDE_PCX7307_SIO, INCLUDE_Z85230_SIO */

#ifdef INCLUDE_ALTIVEC
#include "altivecLib.h"
IMPORT int (* _func_altivecProbeRtn) (void) ;
#endif /* INCLUDE_ALTIVEC */

/* defines */

#define pVal(r) \
   { \
   printf("%-40s = 0x%08x\n",#r,r); \
   }

#ifdef INCLUDE_DPM    /* dynamic power management */

#   define _PPC_HID0_BIT_DPM    11 /* dynamic power management bit */
#   define _PPC_HID0_DPM        (1<<(31-_PPC_HID0_BIT_DPM))

#endif /* INCLUDE_DPM */

#define SYS_DECS_DRAM_INDEX     1

/* structures */

typedef struct phbPciSpace
    {
    UINT32 pciMstrMemioLocal;
    UINT32 pciMstrMemioSize;
    UINT32 pciMstrMemioBus;
    UINT32 pciMstrMemLocal;
    UINT32 pciMstrMemSize;
    UINT32 pciMstrMemBus;
    UINT32 isaMstrIoLocal;
    UINT32 isaMstrIoSize;
    UINT32 isaMstrIoBus;
    UINT32 pciMstrIoLocal;
    UINT32 pciMstrIoSize;
    UINT32 pciMstrIoBus;
    UCHAR intLine[32][4];
    } PHB_PCI_SPACE;

PHB_PCI_SPACE sysPhbPciSpace [] =
    {
        {   /* Bus Interface 0.0 */
        PCI0_MSTR_MEMIO_LOCAL,
        PCI0_MSTR_MEMIO_SIZE,
        PCI0_MSTR_MEMIO_BUS,

        /*
         * Reserve space for internally mapped MV64360 register set.
         * It will be mapped into PCI prefetchable memory space
         * immediately after executing sysPciAutoConfig().  This
         * mapping is absolutely necessary in order for inbound
         * mapping from PCI bus to DRAM to work.
         */

        PCI0_MSTR_MEM_LOCAL + MV64360_REG_SPACE_SIZE,
        PCI0_MSTR_MEM_SIZE - MV64360_REG_SPACE_SIZE,
        PCI0_MSTR_MEM_BUS + MV64360_REG_SPACE_SIZE,

        0x0,	/* 16-bit I/O bus disabled */
        0x0,
        0x0,
        PCI0_MSTR_IO_LOCAL,
        PCI0_MSTR_IO_SIZE,
        PCI0_MSTR_IO_BUS,
            {   /* PCI interrupt routing for bus 0.0 */
                { PCI0_D00_ROUTE }, { PCI0_D01_ROUTE },
                { PCI0_D02_ROUTE }, { PCI0_D03_ROUTE },
                { PCI0_D04_ROUTE }, { PCI0_D05_ROUTE },
                { PCI0_D06_ROUTE }, { PCI0_D07_ROUTE },
                { PCI0_D08_ROUTE }, { PCI0_D09_ROUTE },
                { PCI0_D10_ROUTE }, { PCI0_D11_ROUTE },
                { PCI0_D12_ROUTE }, { PCI0_D13_ROUTE },
                { PCI0_D14_ROUTE }, { PCI0_D15_ROUTE },
                { PCI0_D16_ROUTE }, { PCI0_D17_ROUTE },
                { PCI0_D18_ROUTE }, { PCI0_D19_ROUTE },
                { PCI0_D20_ROUTE }, { PCI0_D21_ROUTE },
                { PCI0_D22_ROUTE }, { PCI0_D23_ROUTE },
                { PCI0_D24_ROUTE }, { PCI0_D25_ROUTE },
                { PCI0_D26_ROUTE }, { PCI0_D27_ROUTE },
                { PCI0_D28_ROUTE }, { PCI0_D29_ROUTE },
                { PCI0_D30_ROUTE }, { PCI0_D31_ROUTE }
            }
        },
        {   /* Bus Interface 1.0 */
        PCI1_MSTR_MEMIO_LOCAL,
        PCI1_MSTR_MEMIO_SIZE,
        PCI1_MSTR_MEMIO_BUS,
        PCI1_MSTR_MEM_LOCAL + MV64360_REG_SPACE_SIZE,
        PCI1_MSTR_MEM_SIZE - MV64360_REG_SPACE_SIZE,
        PCI1_MSTR_MEM_BUS + MV64360_REG_SPACE_SIZE,
        ISA_MSTR_IO_LOCAL,
        ISA_MSTR_IO_SIZE,
        ISA_MSTR_IO_BUS,
        PCI1_MSTR_IO_LOCAL,
        PCI1_MSTR_IO_SIZE,
        PCI1_MSTR_IO_BUS,
            {   /* PCI interrupt routing for bus 1.0 */
                { PCI1_D00_ROUTE }, { PCI1_D01_ROUTE },
                { PCI1_D02_ROUTE }, { PCI1_D03_ROUTE },
                { PCI1_D04_ROUTE }, { PCI1_D05_ROUTE },
                { PCI1_D06_ROUTE }, { PCI1_D07_ROUTE },
                { PCI1_D08_ROUTE }, { PCI1_D09_ROUTE },
                { PCI1_D10_ROUTE }, { PCI1_D11_ROUTE },
                { PCI1_D12_ROUTE }, { PCI1_D13_ROUTE },
                { PCI1_D14_ROUTE }, { PCI1_D15_ROUTE },
                { PCI1_D16_ROUTE }, { PCI1_D17_ROUTE },
                { PCI1_D18_ROUTE }, { PCI1_D19_ROUTE },
                { PCI1_D20_ROUTE }, { PCI1_D21_ROUTE },
                { PCI1_D22_ROUTE }, { PCI1_D23_ROUTE },
                { PCI1_D24_ROUTE }, { PCI1_D25_ROUTE },
                { PCI1_D26_ROUTE }, { PCI1_D27_ROUTE },
                { PCI1_D28_ROUTE }, { PCI1_D29_ROUTE },
                { PCI1_D30_ROUTE }, { PCI1_D31_ROUTE }
            }
        }
    };

typedef struct phbWinStruct
    {
    UINT32 winType;   /* mem or i/o */
    UINT32 winBase;   /* start of window */
    UINT32 winLimit;  /* end of window */
    } PHB_WIN_STRUCT;

typedef struct phbOffsetsOutbound
    {
    UINT32 base;
    UINT32 size;
    UINT32 remap;
    UINT32 enableBit;
    } PHB_OFFSETS_OUTBOUND;

typedef struct phbOffsetsInbound
    {
    UINT32 base;
    UINT32 function;
    UINT32 size;
    UINT32 remap;
    UINT32 enableReg;
    UINT32 enableBit;
    } PHB_OFFSETS_INBOUND;

PHB_OFFSETS_OUTBOUND    sysPhbCpuWinOff [] =
    {
        {
        CPUIF_PCI0_MEM0_BASE_ADDR,
	CPUIF_PCI0_MEM0_SIZE,
	CPUIF_PCI0_MEM0_ADDR_REMAP_LOW,
	CPUIF_BASE_ADDR_ENABLE_PCI0_MEM0_BIT
        },

        {
        CPUIF_PCI0_MEM1_BASE_ADDR,
	CPUIF_PCI0_MEM1_SIZE,
	CPUIF_PCI0_MEM1_ADDR_REMAP_LOW,
	CPUIF_BASE_ADDR_ENABLE_PCI0_MEM1_BIT
        },

        {
        CPUIF_PCI0_MEM2_BASE_ADDR,
	CPUIF_PCI0_MEM2_SIZE,
	CPUIF_PCI0_MEM2_ADDR_REMAP_LOW, 
	CPUIF_BASE_ADDR_ENABLE_PCI0_MEM2_BIT
        },

        {
        CPUIF_PCI0_MEM3_BASE_ADDR,
	CPUIF_PCI0_MEM3_SIZE,
	CPUIF_PCI0_MEM3_ADDR_REMAP_LOW,
	CPUIF_BASE_ADDR_ENABLE_PCI0_MEM3_BIT
        },

        {
        CPUIF_PCI0_IO_BASE_ADDR,
	CPUIF_PCI0_IO_SIZE,
	CPUIF_PCI0_IO_ADDR_REMAP,
	CPUIF_BASE_ADDR_ENABLE_PCI0_IO_BIT
        },

        {
        CPUIF_PCI1_MEM0_BASE_ADDR,
	CPUIF_PCI1_MEM0_SIZE,
	CPUIF_PCI1_MEM0_ADDR_REMAP_LOW,
	CPUIF_BASE_ADDR_ENABLE_PCI1_MEM0_BIT
        },

        {
        CPUIF_PCI1_MEM1_BASE_ADDR,
	CPUIF_PCI1_MEM1_SIZE,
	CPUIF_PCI1_MEM1_ADDR_REMAP_LOW,
	CPUIF_BASE_ADDR_ENABLE_PCI1_MEM1_BIT
        },

	{
        CPUIF_PCI1_MEM2_BASE_ADDR,
	CPUIF_PCI1_MEM2_SIZE,
	CPUIF_PCI1_MEM2_ADDR_REMAP_LOW,
	CPUIF_BASE_ADDR_ENABLE_PCI1_MEM2_BIT
        },

        {
        CPUIF_PCI1_MEM3_BASE_ADDR,
	CPUIF_PCI1_MEM3_SIZE,
	CPUIF_PCI1_MEM3_ADDR_REMAP_LOW,
	CPUIF_BASE_ADDR_ENABLE_PCI1_MEM3_BIT
        },

        {
        CPUIF_PCI1_IO_BASE_ADDR,
	CPUIF_PCI1_IO_SIZE,
	CPUIF_PCI1_IO_ADDR_REMAP,
	CPUIF_BASE_ADDR_ENABLE_PCI1_IO_BIT
        }

    };

PHB_OFFSETS_INBOUND    sysPhbPciWinOff [] =
    {
	{
	PCIIF_CS0_BASE_ADDR_LOW_PCICFG, 0,
	PCIIF_CS0_BAR_SIZE_PCI0,
	PCIIF_CS0_BASE_ADDR_REMAP_PCI0,
	PCIIF_BASE_ADDR_ENABLE_PCI0, 
	PCIIF_BASE_ADDR_ENABLE_CS0_BIT
	},

	{
	PCIIF_CS1_BASE_ADDR_LOW_PCICFG, 0,
	PCIIF_CS1_BAR_SIZE_PCI0,
	PCIIF_CS1_BASE_ADDR_REMAP_PCI0,
	PCIIF_BASE_ADDR_ENABLE_PCI0, 
	PCIIF_BASE_ADDR_ENABLE_CS1_BIT
	},

	{
	PCIIF_CS2_BASE_ADDR_LOW_PCICFG, 1,
	PCIIF_CS2_BAR_SIZE_PCI0,
	PCIIF_CS2_BASE_ADDR_REMAP_PCI0,
	PCIIF_BASE_ADDR_ENABLE_PCI0, 
	PCIIF_BASE_ADDR_ENABLE_CS2_BIT
	},

	{
	PCIIF_CS3_BASE_ADDR_LOW_PCICFG, 1,
	PCIIF_CS3_BAR_SIZE_PCI0,
	PCIIF_CS3_BASE_ADDR_REMAP_PCI0,
	PCIIF_BASE_ADDR_ENABLE_PCI0, 
	PCIIF_BASE_ADDR_ENABLE_CS3_BIT
	},

	{
	PCIIF_CS0_BASE_ADDR_LOW_PCICFG_ALT, 0,
	PCIIF_CS0_BAR_SIZE_PCI1,
	PCIIF_CS0_BASE_ADDR_REMAP_PCI1,
	PCIIF_BASE_ADDR_ENABLE_PCI1, 
	PCIIF_BASE_ADDR_ENABLE_CS0_BIT
	},

	{
	PCIIF_CS1_BASE_ADDR_LOW_PCICFG_ALT, 0,
	PCIIF_CS1_BAR_SIZE_PCI1,
	PCIIF_CS1_BASE_ADDR_REMAP_PCI1,
	PCIIF_BASE_ADDR_ENABLE_PCI1, 
	PCIIF_BASE_ADDR_ENABLE_CS1_BIT
	},

	{
	PCIIF_CS2_BASE_ADDR_LOW_PCICFG_ALT, 1,
	PCIIF_CS2_BAR_SIZE_PCI1,
	PCIIF_CS2_BASE_ADDR_REMAP_PCI1,
	PCIIF_BASE_ADDR_ENABLE_PCI1, 
	PCIIF_BASE_ADDR_ENABLE_CS2_BIT
	},

	{
	PCIIF_CS3_BASE_ADDR_LOW_PCICFG_ALT, 1,
	PCIIF_CS3_BAR_SIZE_PCI1,
	PCIIF_CS3_BASE_ADDR_REMAP_PCI1,
	PCIIF_BASE_ADDR_ENABLE_PCI1, 
	PCIIF_BASE_ADDR_ENABLE_CS3_BIT
	}
    };

#define PHB_CPU_WIN_CNT \
        (sizeof (sysPhbCpuWinOff)/ sizeof (PHB_OFFSETS_OUTBOUND))
#define PHB_PCI_WIN_CNT \
        (sizeof (sysPhbPciWinOff)/ sizeof (PHB_OFFSETS_INBOUND))
#define PHB_WIN_CNT (PHB_CPU_WIN_CNT + PHB_PCI_WIN_CNT)

/* globals */

ERROR_HANDLER_INIT _func_errorHandlerInit = NULL;
IMPORT int pciMaxBus;
static char * sysPhysMemSize = NULL;    /* ptr to top of mem + 1 */
UINT globalBusIF;
UINT32 cpuBusSpeed;

IMPORT void mmuPpcBatInitMPC74x5(UINT32 *pSysBatDesc); /* MPC7455 BAT Support */
IMPORT FUNCPTR _pSysBatInitFunc;

/*
 * sysBatDesc[] is used to initialize the block address translation (BAT)
 * registers within the PowerPC 603/604 MMU.  BAT hits take precedence
 * over Page Table Entry (PTE) hits and are faster.  Overlap of memory
 * coverage by BATs and PTEs is permitted in cases where either the IBATs
 * or the DBATs do not provide the necessary mapping (PTEs apply to both
 * instruction AND data space, without distinction).
 *
 * The primary means of memory control for VxWorks is the MMU PTE support
 * provided by vmLib and cacheLib.  Use of BAT registers will conflict
 * with vmLib support.  User's may use BAT registers for i/o mapping and
 * other purposes but are cautioned that conflicts with cacheing and mapping
 * through vmLib may arise.  Be aware that memory spaces mapped through a BAT
 * are not mapped by a PTE and any vmLib() or cacheLib() operations on such
 * areas will not be effective, nor will they report any error conditions.
 *
 * Note: BAT registers can be disabled if the VS and VP bits are both clear
 * in the upper BAT register of each pair.  In the default configuration
 * (coded below) the VS and VP bits are cleared and thus the BAT registers
 * are disabled.  To enable the BAT registers, change the construct coded
 * below the upper BAT register:
 *
 *.CS
 *          & ~(_MMU_UBAT_VS | _MMU_UBAT_VP)),
 *          which clears VS and VP
 *                to
 *          | (_MMU_UBAT_VS | _MMU_UBAT_VP)),
 *          which sets VS and VP
 *.CE
 *
 * With this in mind, it is recommended that the BAT registers be used
 * to map LARGE memory areas external to the processor if possible.
 * If not possible, map sections of high RAM and/or PROM space where
 * fine grained control of memory access is not needed.  This has the
 * beneficial effects of reducing PTE table size (8 bytes per 4k page)
 * and increasing the speed of access to the largest possible memory space.
 * Use the PTE table only for memory which needs fine grained (4KB pages)
 * control or which is too small to be mapped by the BAT regs.
 *
 * The BAT configuration for 4xx/6xx-based PPC boards is as follows:
 * All BATs point to PROM/FLASH memory so that end customer may configure
 * them as required.
 *
 * [Ref: chapter 7, PowerPC Microprocessor Family: The Programming Environments]
 */

UINT32 sysBatDesc [2 * (_MMU_NUM_IBAT + _MMU_NUM_DBAT + _MMU_NUM_EXTRA_IBAT +
			_MMU_NUM_EXTRA_DBAT)] =
    {
    /* I BAT 0 */

    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | (_MMU_UBAT_BL_1M &
    ~(_MMU_UBAT_VS & _MMU_UBAT_VP))),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* I BAT 1 */

    0, 0,

    /* I BAT 2 */

    0, 0,

    /* I BAT 3 */

    0, 0,

    /* D BAT 0 */

    0, 0,

    /* D BAT 1 */

    0, 0,

    /* D BAT 2 */

    0, 0,

    /* D BAT 3 */

    0, 0,

    /* I BAT 4 */

    0, 0,

    /* I BAT 5 */

    0, 0,

    /* I BAT 6 */

    0, 0,

    /* I BAT 7 */

    0, 0,

    /* D BAT 4 */

    0, 0,

    /* D BAT 5 */

    0, 0,

    /* D BAT 6 */

    0, 0,

    /* D BAT 7 */

    0, 0
    };

/*
 * sysPhysMemDesc[] is used to initialize the Page Table Entry (PTE) array
 * used by the MMU to translate addresses with single page (4k) granularity.
 * PTE memory space should not, in general, overlap BAT memory space but
 * may be allowed if only Data or Instruction access is mapped via BAT.
 *
 * Address translations for local RAM, memory mapped PCI bus and local
 * PROM/FLASH are set here.
 *
 * PTEs are held, strangely enough, in a Page Table.  Page Table sizes are
 * integer powers of two based on amount of memory to be mapped and a
 * minimum size of 64 kbytes.  The MINIMUM recommended Page Table sizes
 * for 32-bit PowerPCs are:
 *
 *.CS
 * Total mapped memory        Page Table size
 * -------------------        ---------------
 *        8 Meg                 64 K
 *       16 Meg                128 K
 *       32 Meg                256 K
 *       64 Meg                512 K
 *      128 Meg                  1 Meg
 *     .                .
 *     .                .
 *     .                .
 *
 * [Ref: chapter 7, PowerPC Microprocessor Family: The Programming Environments]
 *.CE
 */

PHYS_MEM_DESC sysPhysMemDesc [] =
    {

    {

    /* Vector Table and Interrupt Stack */

    (void *) LOCAL_MEM_LOCAL_ADRS,
    (void *) LOCAL_MEM_LOCAL_ADRS,
    (((LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET) & 0xfffff000) - 
       LOCAL_MEM_LOCAL_ADRS),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    {

    /*
     * Shared memory anchor 
     * Note that if SM_OFF_BOARD is TRUE we really don't need to carve out
     * sections for shared-memory anchor and shared-memory pool.  It
     * does not hurt to do so, however and doing so makes the coding
     * simpler.  If SM_OFF_BOARD is TRUE, then the sections of memory
     * represented by this entry and the subsequent entry (shared-memory
     * pool) are available for other uses.  The appropriate cacheing
     * attribute is automatically taken care of by the SM_CACHE_STATE
     * #define (see config.h for the setting of this #define).
     * related entries.
     */

    (void *) ((LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET) & 0xfffff000),
    (void *) ((LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET) & 0xfffff000),
    SM_BASE - ((LOCAL_MEM_LOCAL_ADRS + SM_ANCHOR_OFFSET) & 0xfffff000),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | SM_CACHE_STATE |
    VM_STATE_MEM_COHERENCY
    },

    {

    /* Shared-memory pool */

    (void *) SM_BASE,
    (void *) SM_BASE,
    SM_MEM_SIZE + SM_OBJ_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | SM_CACHE_STATE |
    VM_STATE_MEM_COHERENCY
    },

    {

    /* Remaining DRAM to RAM_LOW_ADRS */

    (void *) (SM_BASE + SM_MEM_SIZE + SM_OBJ_MEM_SIZE),
    (void *) (SM_BASE + SM_MEM_SIZE + SM_OBJ_MEM_SIZE),
    (RAM_LOW_ADRS - (SM_BASE + SM_MEM_SIZE + SM_OBJ_MEM_SIZE)),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    {

    /* Local DRAM - length is dynamically altered if Auto Sizing */

    (void *) RAM_LOW_ADRS,
    (void *) RAM_LOW_ADRS,
    LOCAL_MEM_SIZE -  RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    {

    /* Access to VME A32 space */

    (void *) VME_A32_MSTR_LOCAL,
    (void *) VME_A32_MSTR_LOCAL,
    VME_A32_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Access to VME A24 space */

    (void *) VME_A24_MSTR_LOCAL,
    (void *) VME_A24_MSTR_LOCAL,
    VME_A24_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Access to VME A16 space */

    (void *) VME_A16_MSTR_LOCAL,
    (void *) VME_A16_MSTR_LOCAL,
    VME_A16_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Access to PCI bus 0.0 non-prefetchable memory space */

    (void *) PCI0_MSTR_MEMIO_LOCAL,
    (void *) PCI0_MSTR_MEMIO_LOCAL,
    PCI0_MSTR_MEMIO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Access to PCI bus 0.0 prefetchable memory space */

    (void *) PCI0_MSTR_MEM_LOCAL,
    (void *) PCI0_MSTR_MEM_LOCAL,
    PCI0_MSTR_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT
    },

    {

    /* Access to PCI bus 1.0 non-prefetchable memory space */

    (void *) PCI1_MSTR_MEMIO_LOCAL,
    (void *) PCI1_MSTR_MEMIO_LOCAL,
    PCI1_MSTR_MEMIO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Access to PCI bus 1.0 prefetchable memory space */

    (void *) PCI1_MSTR_MEM_LOCAL,
    (void *) PCI1_MSTR_MEM_LOCAL,
    PCI1_MSTR_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT
    },

    {

    /* Access to PCI 16-bit ISA I/O space (16-bit I/O is on PCI bus 1.0 only) */

    (void *) ISA_MSTR_IO_LOCAL,
    (void *) ISA_MSTR_IO_LOCAL,
    ISA_MSTR_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Access to PCI 32-bit I/O space for PCI bus 1.0 */

    (void *) PCI1_MSTR_IO_LOCAL,
    (void *) PCI1_MSTR_IO_LOCAL,
    PCI1_MSTR_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Access to PCI 32-bit I/O space for PCI bus 0.0 */

    (void *) PCI0_MSTR_IO_LOCAL,
    (void *) PCI0_MSTR_IO_LOCAL,
    PCI0_MSTR_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* MV64360 Internal Register Space (64 Kb) */

    (void *) MV64360_REG_BASE,
    (void *) MV64360_REG_BASE,
    MV64360_REG_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Integrated SRAM */

    (void *) MV64360_INTEGRATED_SRAM_BASE,
    (void *) MV64360_INTEGRATED_SRAM_BASE,
    MV64360_INTEGRATED_SRAM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

#ifdef INCLUDE_CACHE_L3_PM
    {

    /*
     * L3 Cache Private Memory
     *
     * The L3 cache private memory should be configured as not
     * Write-Through (W = 0), not Cache-Inhibited (I = 0), and
     * not Guarded (G = 0).
     */

    (void *) INCLUDE_CACHE_L3_PM_BASE,
    (void *) INCLUDE_CACHE_L3_PM_BASE,
    SIZE_2MB,   /* maximum size of L3 private memory */
    VM_STATE_MASK_VALID |  VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE   | VM_STATE_CACHEABLE
    },
#endif /* INCLUDE_CACHE_L3_PM */

    {

    /* Devices on Device Bus - dynamically adjusted */

    (void *) CPUIF_DEVCS1_BASE_ADDR,	/* dynamic adjustment in sysHwInit() */
    (void *) CPUIF_DEVCS1_BASE_ADDR,	/* dynamic adjustment in sysHwInit() */
    0,					/* dymamically filled in sysHwInit() */ 
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Flash Nonboot Bank on Device Bus (devCS0) - dynamically adjusted */

    (void *) CPUIF_DEVCS0_BASE_ADDR,	/* dynamic adjustment in sysHwInit() */
    (void *) CPUIF_DEVCS0_BASE_ADDR,	/* dynamic adjustment in sysHWInit() */
    0,					/* dynamically filled in sysHwInit() */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Flash Boot Bank on Device Bus (BootCS) - dynamically adjusted */

    (void *) CPUIF_BOOTCS_BASE_ADDR,	/* dynamic adjustment in sysHwInit() */
    (void *) CPUIF_BOOTCS_BASE_ADDR,	/* dynamic adjustment in sysHwInit() */
    0,					/* dynamically filled in sysHwInit() */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    }
    };

int sysPhysMemDescNumEnt = NELEMENTS (sysPhysMemDesc);

int   sysBus       = BUS_TYPE_PCI;   /* system bus type */
int   sysCpu       = CPU;            /* CPU type */
char * sysBootLine = BOOT_LINE_ADRS; /* address of boot line */
char * sysExcMsg   = EXC_MSG_ADRS;   /* catastrophic message area */
int    sysProcNum;                   /* processor number of this CPU */
int    sysFlags;                     /* boot flags */
char   sysBootHost [BOOT_FIELD_LEN]; /* name of host from which we booted */
char   sysBootFile [BOOT_FIELD_LEN]; /* name of file from which we booted */
UINT   sysVectorIRQ0  = 0x00;	     /* vector for IRQ0 */

PHB_WIN_STRUCT sysPhbCpuToPciWin [PHB_WIN_CNT] = { { 0,0,0 } };
PHB_WIN_STRUCT sysPhbPciToCpuWin [PHB_WIN_CNT] = { { 0,0,0 } };
int   sysValidPhbWindows = 0;    /* number of valid entries */
int   phbToPciWinCount = 0;      /* number of valid Phb to PCI windows */
int   pciToPhbWinCount = 0;      /* number of valid PCI to PHB windows */

UINT32 sysProbeFault = 0;       /* used by dsi exception trap handler */

/* locals */

LOCAL char sysModelStr[80];
LOCAL char sysWrongCpuMsg[] = WRONG_CPU_MSG;

/* forward declarations */

void    sysCpuCheck (void);
char *  sysPhysMemTop (void);
UCHAR   sysNvRead (ULONG);
void    sysNvWrite (ULONG,UCHAR);
STATUS  sysBusProbe (char *, int, int, char *);
LOCAL   STATUS  checkHbStat (void);
STATUS  sysPciProbe (char *, int, int, char *);
STATUS  sysVMEProbe (char *, int, int, char *);
BOOL    isPciAdrs (char *, char **);
BOOL    isVmeAdrs (char *, char **, UINT32 *);
void    sysUsDelay (UINT);
void    reportBootromErrors (void);
void    sysDelay (void);
LOCAL   void sysPhbCapt (void);
void    sysPciInsertLong (UINT32, UINT32, UINT32);
void    sysPciInsertWord (UINT32, UINT16, UINT16);
void    sysPciInsertByte (UINT32, UINT8, UINT8);
void	sysDebugMsg (char *, UINT32);
int	sysAltivecProbe (void);
void	sysProbeErrClr (void);
UCHAR	sysInByte (ULONG);
void	sysOutByte (ULONG, UCHAR);
UINT16	sysIn16 (UINT32);
void	sysOut16 (UINT32, UINT16);
UINT32	sysIn32 (UINT32);
void	sysOut32 (UINT32, UINT32);
BOOL    sysBusTas (char *);
LOCAL   void sysBusTasClear (char *);
STATUS  sysPciConfigRead (int, int, int, int, int, void *);
STATUS  sysPciConfigWrite (int, int, int, int, int, UINT32);
STATUS  sysPciConfigSpcl (int, UINT32);
STATUS  sysPciConfigWriteAlt (int, int, int, int, int, UINT32);

LOCAL BOOL sysVmeBusLockTas (char *);
LOCAL BOOL sysVmeRmwTas (char *);
LOCAL BOOL sysVmeRmw (char *);
void  sysApplInit (void);

#ifdef INCLUDE_DPM
    void  sysConfigDpm (void);
#endif /* INCLUDE_DPM */

/* externals */

IMPORT void   sysMv64360DevBusInit (void);
IMPORT void   sysMv64360PhbInit (void);
IMPORT void   sysTempeReset (void);
IMPORT void   sysTempeInit (void);
IMPORT void   sysClkIntCIO (void);
IMPORT VOIDFUNCPTR  smUtilTasClearRtn;
IMPORT UINT32 sysTimeBaseLGet (void);
IMPORT UINT   sysHid1Get (void);
IMPORT UINT   sysHid0Get (void);
IMPORT void   sysHid0Put (UINT32);
IMPORT UINT32 sysDramSize (void);
IMPORT END_TBL_ENTRY endDevTbl[];
IMPORT int    pciConfigBdfPack (int busNo, int deviceNo, int funcNo);
IMPORT UCHAR  sysProductStr[];
IMPORT UCHAR  sysCpuStr[];
IMPORT UINT32 sysCpuBusSpd (void);
#if (( defined INCLUDE_RTC ) && ( defined INCLUDE_DOSFS ))
    IMPORT void dosFsDateTimeInstall(FUNCPTR);
#endif /* INCLUDE_RTC, INCLUDE_DOSFS */
IMPORT STATUS sysVmeToPciAdrs (int, char *, char **);
IMPORT STATUS sysPciToVmeAdrs (int, char *, char **);
IMPORT BOOL   sysVmeBusLock (void);
IMPORT void   sysVmeBusUnlock (void);
IMPORT BOOL   isMappedToVME (char *, char **, UINT32 *);
IMPORT STATUS sysVMEProbe (char *, int, int, char *);

/* BSP DRIVERS */

#include "pci/pciConfigLib.c"		/* PCI Support */
#include "pci/pciIntLib.c"		/* PCI Support */
#include "sysBusPci.c"			/* PCI Auto-Configuration Support */
#include "sysMotVpd.c"                  /* VPD Support */
#include "sysMv64360Init.c"		/* MV64360 register init handler */
#include "sysMv64360Int.c"		/* MV64360 main interrupt handler */
#include "sysMv64360Phb.c"		/* MV64360 host bridge support */
#include "sysMv64360DevBus.c"		/* MV64360 device bus support */
#include "sysMv64360SpecialMem.c"	/* "Special Memory" carving */
#include "sysTempe.c"			/* Tempe PCI-x to VME support */
#ifdef INCLUDE_BOOT_IMAGE_PROGRAM
#   include "sysFlashUtil.c"		/* FLASH driver interface support */
#   include "sysProgramBootImage.c"	/* Boot Image Programming support */
#endif /* INCLUDE_BOOT_IMAGE_PROGRAM */

#ifdef INCLUDE_DS1621
#   include "sysDs1621TempSensor.c"	/* DS1621 Temp Sensor Support */
#endif /* INCLUDE_DS1621 */

#if (defined INCLUDE_CACHE_L2)
#   include "sysL2Cache.c"              /* L2 Cache Support */
#endif /* INCLUDE_CACHE_L2 */

#if ((defined INCLUDE_CACHE_L3) || (defined INCLUDE_CACHE_L3_PM))
#   include "sysL3Cache.c"              /* L3 Cache Support */
#endif /* INCLUDE_CACHE_L3 */

#if (defined INCLUDE_MV64360_DMA)
#   include "sysDma.c"			/* Top-level DMA user routines */
#   include "sysMv64360Dma.c"		/* MV64360 DMA Support */
#endif /* INCLUDE_MV64360_DMA */

#include "sysSerial.c"			/* Serial Support */
#include "mem/byteNvRam.c"		/* NVRAM Support */
#include "timer/ppcDecTimer.c"		/* PPC603 & 604 have on chip timers */

#ifdef INCLUDE_SCSI
#   include "sysScsi.c"                 /* sysScsiInit routine */
#endif /* INCLUDE_SCSI */

#ifdef INCLUDE_RTC              	/* RTC and alarm clock routines */
#  include "sysRtc.c"
#endif /* INCLUDE_RTC */

#ifdef INCLUDE_FAILSAFE         	/* Failsafe timer routines */
#  include "sysFailsafe.c"
#endif /* INCLUDE_FAILSAFE */

#ifdef INCLUDE_ERROR_HANDLING
#   include "sysErrorHandler.c"
#endif

#include "m48t37.c"             	/* M48T37 Timekeeper SRAM */

#ifdef INCLUDE_GEISC_END
#    include "sysMv64360End.c"   /* MV64360 ethernet */
#endif /* INCLUDE_GEISC_END */

#ifdef INCLUDE_SHOW_ROUTINES
#   include "sysMotVpdShow.c"		/* VPD Show Routines */
#   include "sysMv64360SmcShow.c"	/* SMC & SPD Show Routines */
#   include "pci/pciConfigShow.c"	/* PCI Show Routines */
#   include "pciCfgShow.c"		/* PCI Config Space Show Routines */
#   include "mv64360DumpRegs.c"		/* Disco II Register Show Routine */
#   include "mv64360CPUWindows.c"	/* Disco II CPU Interface Show Routine */
#endif /* INCLUDE_SHOW_ROUTINES */
 
#if (defined INCLUDE_MV64360_TMRCNTR) || (defined INCLUDE_AUX_CLK)
#   include "sysMv64360TmrCntr.c"	/* MV64360 Timer/Counter Support */
#endif /* INCLUDE_MV64360_TMRCNTR, INCLUDE_AUX_CLK */

#ifdef INCLUDE_AUX_CLK
#   include "sysMv64360AuxClk.c"	/* MV64360 AuxClock support */
#endif /* INCLUDE_AUX_CLK */

#ifdef INCLUDE_PCX7307_SIO
#   include "ns8730xSuperIo.c"		/* National PCx7307 SIO Support */
#endif /* INCLUDE_PCX7307_SIO */

#if ((defined INCLUDE_PCX7307_SIO) || (defined INCLUDE_Z85230_SIO))
#include "sysIbc.c"			/* IBC Interrupt Controller */
#endif /* INCLUDE_PCX7307_SIO, INCLUDE_Z85230_SIO */

#ifdef INCLUDE_MV64360_WDOG
#   include "sysMv64360Wdog.c"          /* MV64360 Watchdog Timer Support */
#endif /* INCLUDE_MV64360_WDOG */

/*******************************************************************************
*
* sysApplInit - routines to be called upon kernel initialization
*
* This routine is used to call user applications during kernel
* initialization.
* 
* RETURNS: NA
*/

void sysApplInit (void)
    {
#if (defined INCLUDE_TFFS_FLASHA) || (defined INCLUDE_TFFS_FLASHB)
#   if (defined INCLUDE_BOOT_IMAGE_PROGRAM)
	sysTffsMountDevs(); 
	sysFlashDevsMount();
#   else
	sysTffsMountDevs();
#   endif
#else
#   if (defined INCLUDE_BOOT_IMAGE_PROGRAM)
	sysFlashDevsMount();
#   endif
#endif
    }

#if defined (INCLUDE_ALTIVEC)
/*******************************************************************************
*
* sysAltivecProbe - Check if the CPU has ALTIVEC unit.
*
* This routine returns OK if the CPU has an ALTIVEC unit in it.
* Presently it checks for 74xx
* RETURNS: OK  -  for 74xx Processor type
*          ERROR - otherwise.
*/

int  sysAltivecProbe (void)
    {
    ULONG regVal;
    int altivecUnitPresent = ERROR;

     /* The CPU type is indicated in the Processor Version Register (PVR) */

     regVal = CPU_TYPE;

     switch (regVal)
       {
          case CPU_TYPE_7410:
          case CPU_TYPE_7400:
	  case CPU_TYPE_7450:
	  case CPU_TYPE_74X5:
          case CPU_TYPE_74X7:
             altivecUnitPresent = OK;
            break;

          default:
             break;
         }      /* switch  */

    return (altivecUnitPresent);
    }
#endif /* INCLUDE_ALTIVEC */

/******************************************************************************
*
* sysModel - return the model name of the CPU board
*
* This routine returns the model name of the CPU board.  The returned string
* depends on the board model and CPU version being used.
*
* RETURNS: A pointer to the string.
*/

char * sysModel (void)
    {
    UINT cpu;
    char cpuStr[80];

    /* Determine CPU type and build display string */

    cpu = CPU_TYPE;
    switch (cpu)
        {
        case CPU_TYPE_604E:
            sprintf(cpuStr, "604e");
            break;
        case CPU_TYPE_604R:
            sprintf(cpuStr, "604r");
            break;
        case CPU_TYPE_603P:
            sprintf(cpuStr, "603p");
            break;
        case CPU_TYPE_603E:
            sprintf(cpuStr, "603e");
            break;
        case CPU_TYPE_750:
            sprintf(cpuStr, "750");
            break;
        case CPU_TYPE_7410:
            sprintf(cpuStr, "7410");
            break;
        case CPU_TYPE_7400:
            sprintf(cpuStr, "7400");
            break;
        case CPU_TYPE_7450:
            sprintf(cpuStr, "7450");
            break;
        case CPU_TYPE_74X5:

            /*
             * The MPC7445/7455 processors have identical PVR values,
             * therefore it is necessary to read the "microprocessor type"
             * packet in the VPD to differentiate the processors.
             */

            sprintf(cpuStr, "%s", sysCpuStr);
            break;
        case CPU_TYPE_74X7:

            /*
             * The MPC7447/7457 processors have identical PVR values,
             * therefore it is necessary to read the "microprocessor type"
             * packet in the VPD to differentiate the processors.
             */

            sprintf(cpuStr, "%s", sysCpuStr);
            break;
        default:
            sprintf (cpuStr, "60%d", cpu);
            break;
        }

    sprintf (sysModelStr, "Motorola %s - MPC %s", sysProductStr, cpuStr);

    return (sysModelStr);
    }

/*******************************************************************************
*
* sysBspRev - return the BSP version and revision number
*
* This routine returns a pointer to a BSP version and revision number, for
* example, 1.1/0. BSP_REV is concatenated to BSP_VERSION and returned.
*
* RETURNS: A pointer to the BSP version/revision string.
*/

char * sysBspRev (void)
    {
    return (BSP_VERSION BSP_REV);
    }

/******************************************************************************
*
* sysPciConfigRead - read from the PCI configuration space
*
* This routine reads from the PCI configuration space.  Reads may be done
* in sizes of one byte, one word, or one longword.  In order to determine
* which Host Bridge's PCI configuration space to read from, a global Bus 
* Interface variable (globalBusIF) and a global bus alias variable 
* (PCI_BUS_ALIAS) are used.  Before and during PCI auto-configuration, 
* ie. before PCI_AUTOCONFIG_FLAG is set, globalBusIF is used.  Once PCI 
* auto-configuration has been completed and the highest numbered bus 
* connected to the first bus interface has been determined and stored in 
* PCI_BUS_ALIAS, this value is used to determine which configuration space 
* to access.
*
* Returns: OK, or ERROR if size not supported.
*/

STATUS sysPciConfigRead
    (
    int bus,
    int device,
    int func,
    int reg,
    int size,
    void * pResult
    )
    {
    UINT8 * retvalByte = 0;
    UINT16 * retvalWord = 0;
    UINT32 * retvalLongword = 0;
    int configAddrReg = 0;
    int configDataReg = 0;
    int busNo = 0;

    if (PCI_AUTOCONFIG_FLAG == TRUE)
        {
        if ((UINT32)bus <= PCI_BUS_ALIAS)
            {
            configAddrReg = (int)PCI_CONFIG_ADDR_BASE_0;
            configDataReg = (int)PCI_CONFIG_DATA_BASE_0;
            busNo = bus;
            }
        else if ((UINT32)bus > PCI_BUS_ALIAS)
            {
            configAddrReg = (int)PCI_CONFIG_ADDR_BASE_1;
            configDataReg = (int)PCI_CONFIG_DATA_BASE_1;
            busNo = (bus - PCI_BUS_ALIAS - 1);
            }
        }
    else
        {
        if (globalBusIF == 0)
            {
            configAddrReg = (int)PCI_CONFIG_ADDR_BASE_0;
            configDataReg = (int)PCI_CONFIG_DATA_BASE_0;
            }
        else if (globalBusIF == 1)
            {
            configAddrReg = (int)PCI_CONFIG_ADDR_BASE_1;
            configDataReg = (int)PCI_CONFIG_DATA_BASE_1;
            }
        else
            return (ERROR);
        busNo = bus;
        }

    PCI_OUT_LONG(configAddrReg, pciConfigBdfPack(busNo, device, func) |
                 (reg & 0xFC) | 0x80000000);

    switch(size)
        {
        case 1:
                *retvalByte = PCI_IN_BYTE(configDataReg + (reg & 0x3));
                *(UINT8 *) pResult = *retvalByte;
                break;

        case 2:
                *retvalWord = PCI_IN_WORD(configDataReg + (reg & 0x2));
                *(UINT16 *)pResult = *retvalWord;
                break;

        case 4:
                *retvalLongword = PCI_IN_LONG(configDataReg);
                *(UINT32 *)pResult = *retvalLongword;
                break;

        default:
                return (ERROR);
        }
    return (OK);
    }

/******************************************************************************
*
* sysPciConfigWrite - write to the PCI configuration space
*
* This routine writes to the PCI configuration space.  Writes may be done
* in sizes of one byte, one word, or one longword.  In order to determine
* which PCI Bus Interface's PCI configuration space to write to, a global Bus 
* Interface variable (globalBusIF) and a global bus alias variable 
* (PCI_BUS_ALIAS) are used.  Before and during PCI auto-configuration, 
* ie. before PCI_AUTOCONFIG_FLAG is set, globalBusIF is used.  Once PCI 
* auto-configuration has been completed and the highest numbered bus 
* connected to Bus Interface 0.0 has been determined and stored in 
* PCI_BUS_ALIAS, this value is used to determine which configuration space 
* to access.
*
* RETURNS: OK, or ERROR if size not supported.
*/

STATUS sysPciConfigWrite
    (
    int bus,		/* PCI bus number */
    int device,		/* PCI device number */
    int func,		/* PCI function number */
    int reg,		/* PCI register offset */
    int size,		/* Size of transfer (1,2, or 4 bytes) */
    UINT32 data		/* data to write */
    )
    {
    UINT8 * dataByte = 0;
    UINT16 * dataWord = 0;
    UINT32 * dataLongword = 0;
    int configAddrReg = 0;
    int configDataReg = 0;
    int busNo = 0;

    if (PCI_AUTOCONFIG_FLAG == TRUE)
        {
        if ((UINT32)bus <= PCI_BUS_ALIAS)
            {
            configAddrReg = (int)PCI_CONFIG_ADDR_BASE_0;
            configDataReg = (int)PCI_CONFIG_DATA_BASE_0;
            busNo = bus;
            }
        else if ((UINT32)bus > PCI_BUS_ALIAS)
            {
            configAddrReg = (int)PCI_CONFIG_ADDR_BASE_1;
            configDataReg = (int)PCI_CONFIG_DATA_BASE_1;
            busNo = (bus - PCI_BUS_ALIAS - 1);
            }
        }
    else
        {
        if (globalBusIF == 0)
            {
            configAddrReg = (int)PCI_CONFIG_ADDR_BASE_0;
            configDataReg = (int)PCI_CONFIG_DATA_BASE_0;
            }
        else if (globalBusIF == 1)
            {
            configAddrReg = (int)PCI_CONFIG_ADDR_BASE_1;
            configDataReg = (int)PCI_CONFIG_DATA_BASE_1;
            }
        else
            return (ERROR);
        busNo = bus;
        }

    PCI_OUT_LONG(configAddrReg, pciConfigBdfPack(busNo, device, func) |
                 (reg & 0xFC) | 0x80000000);

    switch(size)
        {
        case 1:
                *dataByte = data;
                PCI_OUT_BYTE((configDataReg + (reg & 0x3)), *dataByte);
                break;

        case 2:
                *dataWord = data;
                PCI_OUT_WORD((configDataReg + (reg & 0x2)), *dataWord);
                break;

        case 4:
                *dataLongword = data;
                PCI_OUT_LONG(configDataReg, *dataLongword);
                break;

        default:
                return (ERROR);
        }
    return (OK);
    }

/******************************************************************************
*
* sysPciConfigSpcl - generate a special cycle with a message
*
* This routine generates a special cycle with a message.  The PCI_BUS_ALIAS
* variable is used to determine which configuration space to access when
* performing the special cycle.
*
* RETURNS: OK
*/

STATUS sysPciConfigSpcl
    (
    int bus,		/* PCI bus number */
    UINT32 data		/* message data */
    )
    {
    int deviceNo        = 0x0000001f;
    int funcNo          = 0x00000007;
    int configAddrReg = 0;
    int configDataReg = 0;
    int busNo = 0;

    if ((UINT32)bus <= PCI_BUS_ALIAS)
        {
        configAddrReg = (int)PCI_CONFIG_ADDR_BASE_0;
        configDataReg = (int)PCI_CONFIG_DATA_BASE_0;
        busNo = bus;
        }
    else if ((UINT32)bus > PCI_BUS_ALIAS)
        {
        configAddrReg = (int)PCI_CONFIG_ADDR_BASE_1;
        configDataReg = (int)PCI_CONFIG_DATA_BASE_1;
        busNo = (bus - PCI_BUS_ALIAS - 1);
        }

    PCI_OUT_LONG (configAddrReg, pciConfigBdfPack (busNo, deviceNo, funcNo) |
                  0x80000000);
    PCI_OUT_LONG (configDataReg, data);
    return (OK);
    }

/******************************************************************************
*
* sysPciConfigWriteAlt - Perform PCI configuration write to PCI1 interface
*
* This routine is called with exactly the same parameters as
* sysPciConfigWrite.  It will force the globalBusIF variable to 1 so
* as to perform the configuration write to the PCI1 interface rather than
* the PCI0 interface.  It makes no sense to call this routine after
* sysPciAutoConfig() has been called to initialize the PCI interface.
*
* RETURNS: OK, or ERROR if size not supported.
*/
STATUS sysPciConfigWriteAlt
    (
    int bus,            /* PCI bus number */
    int device,         /* PCI device number */
    int func,           /* PCI function number */
    int reg,            /* PCI register offset */
    int size,           /* Size of transfer (1,2, or 4 bytes) */
    UINT32 data         /* Data to write */
    )
    {
    STATUS stat;
    globalBusIF = 1;
    stat = sysPciConfigWrite(bus,device,func,reg,size,data);
    globalBusIF = 0;
    return(stat);
    }

/******************************************************************************
*
* sysHwInit - initialize the system hardware
*
* This routine initializes various features of the CPU board.  It is called
* by usrInit() in usrConfig.c.  This routine sets up the control registers
* and initializes various devices if they are present.
*
* NOTE: This routine should not be called directly by the user application.  It
* cannot be used to initialize interrupt vectors.
*
* RETURNS: N/A
*/

void sysHwInit (void)
    {
    UINT32 reg1;
    UINT32 reg2;
    UINT i;
    UINT32 hid0;
    UINT32 timeOut;
    UINT32 tempeCgr;
    UINT x;
    int pciBusNo;       /* PCI bus number */
    int pciDevNo;       /* PCI device number */
    int pciFuncNo;      /* PCI function number */
char txt[80];

    /*
     * If mmu tables are used, this is where we would dynamically
     * update the entry describing main memory, using sysPhysMemTop().
     * We must call sysPhysMemTop () at sysHwInit() time to do
     * the memory autosizing if available.
     */

    sysPhysMemTop ();

    /* Validate CPU type */

    sysCpuCheck();

    /*
     * Branch Target Instruction Cache Enable in HID0
     *
     * According to errata #20 "BTIC must not be enabled by
     * software" in the MPC7457CE Rev.5, 12/2003 errata
     * document, BTIC must not be enabled for processor 
     * revisions 1.0 and 1.1.  We will set this bit for
     * all other revisions in anticipation of this being
     * fixed in subsequent revisions of the processor.
     */

    if ((CPU_REV != CPU_7457_REV_1_0) && (CPU_REV != CPU_7457_REV_1_1))
	{
	hid0 = sysHid0Get();
	hid0 |= (1 << 5);
	sysHid0Put(hid0);
	} 

    /* set pointer to bus probing hook */

    _func_vxMemProbeHook = (FUNCPTR)sysBusProbe;
 
    /* set pointer to BAT initialization functions */

    _pSysBatInitFunc = (FUNCPTR) mmuPpcBatInitMPC74x5;

    /* Initialize the VPD information, VPD located on MV64360's I2C bus */

    (void) sysMotVpdInit();

#if (defined INCLUDE_CACHE_L2)

        /* initialize and enable L2 cache */

        sysL2CacheInit();

#endif /* INCLUDE_CACHE_L2 */

#if (defined INCLUDE_CACHE_L3)

        /* initialize and enable the L3 cache */

        sysL3CacheInit();

#endif /* INCLUDE_CACHE_L3 */

    /* Initialize those MV64360 registers that require static initialization */

    sysMv64360Init ();

    /* Initialize the MV64360 host bridge support */

    sysMv64360PhbInit ();

    /* Initialize the MV64360 device bus support */

    sysMv64360DevBusInit ();

    /* Calculate the CPU Bus Rate */

    cpuBusSpeed = sysCpuBusSpd();

    /* Setup the dynamic "device bus" entry sizes in sysPhysMemDesc[] */

    for (i = 0; i < (sizeof(sysPhysMemDesc)/sizeof(PHYS_MEM_DESC)); i++)
	{
	switch ((UINT32)sysPhysMemDesc[i].virtualAddr)
	    {
    	    case CPUIF_DEVCS1_BASE_ADDR:

		reg1 = MV64360_READ32 (MV64360_REG_BASE,CPUIF_DEVCS1_BASE_ADDR);
		sysPhysMemDesc[i].virtualAddr = 
				(char *)MV64360_BASE_DECODE_ADDR_INV(reg1);
		sysPhysMemDesc[i].physicalAddr = sysPhysMemDesc[i].virtualAddr;

		reg2 = MV64360_READ32(MV64360_REG_BASE, CPUIF_DEVCS1_SIZE);

	 	sysPhysMemDesc[i].len = MV64360_SIZE_FIELD_INV(reg2);

		break;


    	    case CPUIF_DEVCS0_BASE_ADDR:

		reg1 = MV64360_READ32 (MV64360_REG_BASE,CPUIF_DEVCS0_BASE_ADDR);
		sysPhysMemDesc[i].virtualAddr = 
				(char *)MV64360_BASE_DECODE_ADDR_INV(reg1);
		sysPhysMemDesc[i].physicalAddr = sysPhysMemDesc[i].virtualAddr;

		reg2 = MV64360_READ32(MV64360_REG_BASE, CPUIF_DEVCS0_SIZE);

	 	sysPhysMemDesc[i].len = MV64360_SIZE_FIELD_INV(reg2);

		break;

    	    case CPUIF_BOOTCS_BASE_ADDR:

		reg1 = MV64360_READ32 (MV64360_REG_BASE,CPUIF_BOOTCS_BASE_ADDR);
		sysPhysMemDesc[i].virtualAddr = 
				(char *)MV64360_BASE_DECODE_ADDR_INV(reg1);
		sysPhysMemDesc[i].physicalAddr = sysPhysMemDesc[i].virtualAddr;

		reg2 = MV64360_READ32(MV64360_REG_BASE, CPUIF_BOOTCS_SIZE);

	 	sysPhysMemDesc[i].len = MV64360_SIZE_FIELD_INV(reg2);

		break;

	    default:
		;
	    }
	}

    /* Disable the failsafe timer */

    FAILSAFE_CANCEL();

    /* capture Phb window configuration */

    sysPhbCapt ();

#ifdef INCLUDE_PCX7307_SIO

    /* PCx7307 Super I/O init */

     ns8730xSuperIoInit ();
#endif /* INCLUDE_PCX7307_SIO */

    /* Initialize PCI driver library. */

    if (pciConfigLibInit (PCI_MECHANISM_0, (ULONG)sysPciConfigRead,
        (ULONG)sysPciConfigWrite, (ULONG)sysPciConfigSpcl) != OK)
        sysToMonitor (BOOT_NO_AUTOBOOT);

    if ((pciIntLibInit ()) != OK)
        {
        sysToMonitor (BOOT_NO_AUTOBOOT);
        }

    /* 
     * PrPMC Enumeration
     *
     * Poll EREADY1 and EREADY0 in the Presence Detect Register
     * to wait for any installed PrPMCs to signal that they are
     * ready for enumeration.
     */

    for (timeOut = 10; timeOut > 0; timeOut--)
        {
        if ((*(UCHAR *)(PRESENCE_DETECT_REG) & PRESENCE_DETECT_EREADY1_MASK) &&
	    (*(UCHAR *)(PRESENCE_DETECT_REG) & PRESENCE_DETECT_EREADY0_MASK))
            break;
        sysUsDelay (1000000);
        }


	/*Sergey: my version was slightly different:

  if( !( (*(UCHAR *)(PRESENCE_DETECT_REG)) & PRESENCE_DETECT_PMC0_MASK) )
  {
sprintf(txt,"sysHwInit: PMC installed in slot 1: 0x%08x=0x%02x\r\n ",
(UCHAR *)(PRESENCE_DETECT_REG),*(UCHAR *)(PRESENCE_DETECT_REG));
sysDebugMsg(txt,CONTINUE_EXECUTION);
    for(timeOut = 20; timeOut > 0; timeOut--)
    {
      if( !((*(UCHAR *)(PRESENCE_DETECT_REG)) & PRESENCE_DETECT_EREADY0_MASK) )
      {
        break;
      }
sprintf(txt,"sysHwInit: waiting for EREADY0 [%02d] 0x%08x=0x%02x\r\n ",
timeOut,(UCHAR *)(PRESENCE_DETECT_REG),*(UCHAR *)(PRESENCE_DETECT_REG));
sysDebugMsg(txt,CONTINUE_EXECUTION);
      sysUsDelay(1000000);
    }
  }


  if( !( (*(UCHAR *)(PRESENCE_DETECT_REG)) & PRESENCE_DETECT_PMC1_MASK) )
  {
sprintf(txt,"sysHwInit: PMC installed in slot 2: 0x%08x=0x%02x\r\n ",
(UCHAR *)(PRESENCE_DETECT_REG),*(UCHAR *)(PRESENCE_DETECT_REG));
sysDebugMsg(txt,CONTINUE_EXECUTION);
    for(timeOut = 1; timeOut > 0; timeOut--)
    {
      if( !((*(UCHAR *)(PRESENCE_DETECT_REG)) & PRESENCE_DETECT_EREADY1_MASK) )
      {
        break;
      }
sprintf(txt,"sysHwInit: waiting for EREADY1 [%02d] 0x%08x=0x%02x\r\n ",
timeOut,(UCHAR *)(PRESENCE_DETECT_REG),*(UCHAR *)(PRESENCE_DETECT_REG));
sysDebugMsg(txt,CONTINUE_EXECUTION);
      sysUsDelay(1000000);
    }
  }

sprintf(txt,"sysHwInit: proceed: 0x%08x=0x%02x\r\n ",
(UCHAR *)(PRESENCE_DETECT_REG),*(UCHAR *)(PRESENCE_DETECT_REG));
sysDebugMsg(txt,CONTINUE_EXECUTION);

	 */


    /* perform PCI configuration */
 
    if ( !PCI_AUTOCONFIG_DONE )
        {

        int myPciMaxBus;

	/*
	 * Disable TSi148 Outbound Mapping Windows
	 *
	 * When using the MotLoad firmware to boot vxWorks we must
	 * disable the TSi148's outbound mapping windows before
	 * reconfiguring the TSi148's register access BAR and attempting
	 * to access the windows from this new address.  This is 
	 * necessary because MotLoad configures the TSi148's outbound 
	 * windows for a region of memory that overlaps the region of 
	 * memory where vxWorks wants to place the TSi148's register 
	 * set, which would result in overlapping windows and an 
	 * inability to access the registers at all. 
	 */

	if (pciFindDevice ((PCI_ID_TEMPE & 0xFFFF), (PCI_ID_TEMPE >> 16) 
		   & 0xFFFF, 0, &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR)
	{
	  pciConfigInLong(pciBusNo, pciDevNo, pciFuncNo,
			    PCI_CFG_BASE_ADDRESS_0, &tempeCgr);
	  tempeCgr &= PCI_MEMBASE_MASK;
	  if (tempeCgr != 0x0)
	  {
		for (x = 0; x < 8; x++)
		{

		  /* 
		   * Clear all 8 outbound translation attribute
		   * registers.  This will disable each of the
	       * outbound translation windows.
	       */

		  TEMPE_WRITE32_PUSH(tempeCgr,(TEMPE_OTAT0 + (x*0x20)),
				       0x00000000);
		}
	  }
	}


        /*
         * pciMaxBus ruminations:
         *
         * The global variable pciMaxBus is used by pciFindDevice().
         * It indicates the highest numbered bus in our system.  The
         * routine sysPciMaxBus() calculates this maximum bus number
         * using a function inside of pciAutoConfigLib.  The function
         * used inside of pciAutoConfigLib to assist sysPciMaxBus()
         * turns off PCI-PCI bridge bus mastering as a side effect.
         * It is thus necessary to call sysPciMaxBus() before calling
         * sysPciAutoConfig() so that we guarantee that bus mastering,
         * which is turned on by sysPciAutoConfig(), stays on.  Note
         * also that sysPciAutoConfig() destroys the value of
         * pciMaxBus.  Thus we use a local variable to hold the maximum
         * bus count so we can restore it to the correct value after
         * the call to sysPciAutoConfig().
         */

        myPciMaxBus = sysPciMaxBus(MV64360_MAX_BUS);

        sysPciAutoConfig (MV64360_MAX_BUS, sysPhbPciSpace);

        pciMaxBus = myPciMaxBus;

        /* Remember that PCI is configured */

        PCI_AUTOCONFIG_FLAG = TRUE;

        /*
         * We have configured all of the PCI devices except for the
         * host bridge, which was excluded from pciAutoConfig().  We
         * must now program the internal register set memory mapped BARs
         * on the MV64360 to allow access to the internal register
         * set from the PCI bus.  Although no agent on the PCI bus
         * really needs access to this register space, the programming
         * of these BARs is necessary in order for inbound mappings from
         * PCI bus to DRAM to work.
         */

        pciConfigOutLong (0,0,0,
                          PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_LOW_PCICFG,
                  	  (PCI0_MSTR_MEM_LOCAL | 0x00000008));
        pciConfigOutLong (0,0,0,
                          PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_HI_PCICFG,
                          0x00000000);

	/*
	 * After PCI autoconfiguration the Disco II is located on Busses
	 * 0 & 2, rather than busses 0 & 1.  This is due to the PCI-PCI
	 * bridge chip located on bus 0.
	 */
	 
        pciConfigOutLong (2,0,0,
                          PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_LOW_PCICFG,
                          (PCI1_MSTR_MEM_LOCAL | 0x00000008));
        pciConfigOutLong (2,0,0,
                          PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_HI_PCICFG,
                          0x00000000);


        /* Reset and Initialize the Tempe PCI-VME bridge chip */
        sysTempeReset ();
        sysTempeInit ();


        }

#if ((defined INCLUDE_PCX7307_SIO) || (defined INCLUDE_Z85230_SIO))

    /*
     *  The IBC powers up with most of the PCI Header values
     *  being correct.  So, only registers that do not power-up
     *  in the correct state are modified here.
     */

    if (pciFindDevice ((PCI_ID_IBC & 0xFFFF), (PCI_ID_IBC >> 16) & 0xFFFF, 0,
                       &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR)
        {

        /*
         * Enable Guaranteed Access Timing (default), the Arbiter Timeout
         * Timer, and  Bus Lock (locks PCI Bus when PCI LOCK# signal asserted)
         */

        pciConfigOutByte (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_IBC_ARB_CTL,
                          (ARB_CTL_GAT |
                           ARB_CTL_TIMEOUT_TIMER |
                           ARB_CTL_BUS_LOCK));

        /*
         *  Initialize the non-PCI Config Space registers of the
         *  IBC which doesn't have a true device driver.
         */

        sysIbcInit();

        }
#endif /* INCLUDE_PCX7307_SIO, INCLUDE_Z85230_SIO */

    /* 
     * Initialize COM Ports  - note this must be done AFTER device
     * bus initialization.
     */

    sysSerialHwInit();

    /* Turn software write-protect off for FLASH bank A, and boot block */

    *(UCHAR *)(SYSTEM_STATUS_REG_2) &= 
	~(SYSTEM_STATUS_REG_2_FLASHA_WP_MASK |
	  SYSTEM_STATUS_REG_2_FBOOTB_WP_MASK);

    /* Upon completion, clear BFL (Board Fail) LED */

    *(UCHAR *)(SYSTEM_STATUS_REG_2) &= ~SYSTEM_STATUS_REG_2_BRDFL_MASK;
    }

/*******************************************************************************
*
* sysDramSize - returns the real top of local DRAM.
*
* This routine reads the Base Address Enable register of the MV64360 to 
* determine which banks of memory are enabled.  Then for each bank that is
* enabled the associated size register is read, its value is converted to
* the size in bytes, and this value is added to the total amount of system
* memory.
*
* RETURNS: The address of the top of DRAM.
*
*/

UINT32 sysDramSize (void)
    {
    UINT32 localDram = 0x00000000;	
    UINT32 base = MV64360_REG_BASE;	/* base of MV64360 register set */
    UINT32 baseAddrEnable;		/* Base Address Enable register */
    UINT bank;				/* bank index counter */

    static UINT32 chipSelectSize[] = 
	{
	CPUIF_CS0_SIZE,		/* Chip Select 0 Size register offset */
	CPUIF_CS1_SIZE,		/* Chip Select 1 Size register offset */
	CPUIF_CS2_SIZE,		/* Chip Select 2 Size register offset */
	CPUIF_CS3_SIZE		/* Chip Select 3 Size register offset */
	};
 
    baseAddrEnable = MV64360_READ32(base, CPUIF_BASE_ADDR_ENABLE);

    for (bank = 0; bank < NELEMENTS(chipSelectSize); bank++)
	{
	if (~baseAddrEnable & (1 << bank))  
	     {
	     localDram += (MV64360_SIZE_FIELD_INV(MV64360_READ32(base, 
			    chipSelectSize[bank])));
	     }
	}
    return (localDram);
    }

/*******************************************************************************
*
* sysPhysMemTop - get the address of the top of physical memory
*
* This routine returns the address of the first missing byte of memory,
* which indicates the top of memory.
*
* Normally, the user specifies the amount of physical memory with the
* macro LOCAL_MEM_SIZE in config.h.  BSPs that support run-time
* memory sizing do so only if the macro LOCAL_MEM_AUTOSIZE is defined.
* If not defined, then LOCAL_MEM_SIZE is assumed to be, and must be, the
* true size of physical memory.
*
* NOTE: Do not adjust LOCAL_MEM_SIZE to reserve memory for application
* use.  See sysMemTop() for more information on reserving memory.
*
* RETURNS: The address of the top of physical memory.
*
* SEE ALSO: sysMemTop()
*/

char * sysPhysMemTop (void)
    {
    UINT32 localDram = 0;
    UINT   i;

    if (sysPhysMemSize == NULL)
        {
        localDram = sysDramSize();

#ifdef LOCAL_MEM_AUTOSIZE
        sysPhysMemSize = (char *)localDram;
#else /* not LOCAL_MEM_AUTOSIZE, use defined constants. */
bla
        sysPhysMemSize = (char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);
#endif /* LOCAL_MEM_AUTOSIZE */

        /* Adjust initial DRAM size to actual physical memory. */

        for (i = 0; i < (sizeof(sysPhysMemDesc)/sizeof(PHYS_MEM_DESC)); i++)
	    {
	    if (sysPhysMemDesc[i].virtualAddr == (void *)RAM_LOW_ADRS)
	        {
                sysPhysMemDesc[i].len = 
                    (ULONG)sysPhysMemSize - 
                    (ULONG)sysPhysMemDesc[i].physicalAddr;
		break; 
	    	}
	    }
        }
    return sysPhysMemSize;
    }

/*******************************************************************************
*
* sysMemTop - get the address of the top of VxWorks memory
*
* This routine returns a pointer to the first byte of memory not
* controlled or used by VxWorks.
*
* The user can reserve memory space by defining the macro USER_RESERVED_MEM
* in config.h.  This routine returns the address of the reserved memory
* area.  The value of USER_RESERVED_MEM is in bytes.
*
* RETURNS: The address of the top of VxWorks memory.
*/

char * sysMemTop (void)
    {
    static char * memTop = NULL;

    if (memTop == NULL)
	{
	memTop = sysPhysMemTop () - USER_RESERVED_MEM;

#ifdef INCLUDE_EDR_PM

	/* account for ED&R persistent memory */

	memTop = memTop - PM_RESERVED_MEM;
#endif
	}

    return (memTop);
    }

/******************************************************************************
*
* sysToMonitor - transfer control to the ROM monitor
*
* This routine transfers control to the ROM monitor.  Normally, it is called
* only by reboot()--which services ^X--and by bus errors at interrupt level.
* However, in some circumstances, the user may wish to introduce a
* <startType> to enable special boot ROM facilities.
*
* RETURNS: Does not return.
*/

STATUS sysToMonitor
    (
    int startType    /* parameter passed to ROM to tell it how to boot */
    )
    {
    FUNCPTR pRom = (FUNCPTR) (ROM_WARM_ADRS); /* Warm reboot */

    cacheDisable (INSTRUCTION_CACHE);	/* Disable the L1 Instruction Cache */
    cacheDisable (DATA_CACHE);		/* Disable the L1 Data Cache */

    sysSerialReset ();		/* Reset serial devices */

    vxMsrSet (0);		/* Clear the Machine State Register */

    /* Turn on the board Fail LED */

    *(UCHAR *)(SYSTEM_STATUS_REG_2) |= SYSTEM_STATUS_REG_2_BRDFL_MASK;

    /* Reset PCI Autoconfig Flag */

    PCI_AUTOCONFIG_FLAG = FALSE;

    (*pRom) (startType);

    return (OK);    /* in case we ever continue from ROM monitor */
    }

/******************************************************************************
*
* sysHwInit2 - initialize additional system hardware
*
* This routine connects system interrupt vectors and configures any 
* required features not configured by sysHwInit().
*
* RETURNS: N/A
*/

void sysHwInit2 (void)
    {
    static BOOL configured = FALSE;

    if (!configured)
        {

#ifdef INCLUDE_ERROR_HANDLING

        /* Set pointer to error handling code */

        _func_errorHandlerInit = (ERROR_HANDLER_INIT) sysErrorHandlerInit;
        (void) sysErrorHandlerInit (0, NULL, NULL);
#endif /* INCLUDE_ERROR_HANDLING */

        /* Initialize the interrupt handling */

        sysMv64360IntInit();

        /*
         * Enable the GPP interrupts, all interrupting devices (including
         * serial ports) which are not an integral part of the MV64360 will
         * be using the GPP interrupts.  Note that on this board
         * GPP_INT_LVL_8_15 is not used so we will not enable it.
         */

        intEnable (GPP_INT_LVL_0_7);
        intEnable (GPP_INT_LVL_16_23);
	intEnable (GPP_INT_LVL_24_31);

        /* Connect Tempe (PCI-VME bridge) interrupts */
	
	sysTempeIntInit ();
	
        /* Enable Tempe interrupts at the GPP level */

        intEnable (TEMPE_INT_LVL);

        /* initialize serial interrupts */

        sysSerialHwInit2();

#if ((defined INCLUDE_PCX7307_SIO) || (defined INCLUDE_Z85230_SIO))

        /* connect the PIB to MPIC, before any other connections */

        sysIbcMpicConnect ();
        
#endif /* INCLUDE_PCX7307_SIO, INCLUDE_Z85230_SIO */

#ifdef INCLUDE_DPM

        /* enable dynamic power management */

        sysConfigDpm ();

#endif /* INCLUDE_DPM */

#if     defined (INCLUDE_ALTIVEC)
       _func_altivecProbeRtn = sysAltivecProbe;
#endif  /* INCLUDE_ALTIVEC */

#if (defined INCLUDE_MV64360_DMA)

       /* initialize the DMA driver */

       sysMv64360DmaInit ();

#endif /* INCLUDE_MV64360_DMA */

#ifdef INCLUDE_GEISC_END

       /* Initialize the MV64360 ethernet hardware */
  
       sysMv64360EndHwInit ();

#endif /* INCLUDE_GEISC_END */
       
#if (( defined INCLUDE_RTC ) || ( defined INCLUDE_FAILSAFE ))

     /* initialize the RTC device */

     m48t37Init();
#endif /* INCLUDE_RTC, INCLUDE_FAILSAFE */

#if (( defined INCLUDE_RTC ) && ( defined INCLUDE_DOSFS ))

     dosFsDateTimeInstall((FUNCPTR)sysRtcDateTimeHook);

#endif /* INCLUDE_RTC, INCLUDE_DOSFS */

        configured = TRUE;
	}

    /* 2nd phase of Tempe chip init */

    sysTempeInit2 ();

    /* Set function hook for special sysBusTasClear routine */

    smUtilTasClearRtn = sysBusTasClear;
    }

/******************************************************************************
*
* sysProcNumGet - get the processor number
*
* This routine returns the processor number for the CPU board, which is
* set with sysProcNumSet().
*
* RETURNS: The processor number for the CPU board.
*
* SEE ALSO: sysProcNumSet()
*/

int sysProcNumGet (void)
    {
    return (sysProcNum);
    }

/******************************************************************************
*
* sysProcNumSet - set the processor number
*
* This routine sets the processor number for the CPU board.  Processor numbers
* should be unique on a single backplane.  
*
* RETURNS: N/A
*
* SEE ALSO: sysProcNumGet()
*
*/

void sysProcNumSet
    (
    int     procNum            /* processor number */
    )
    {

    LOCAL int procNumSet = FALSE;

    sysProcNum = procNum;

    /*
     * Note that sysProcNumSet() is always called during bootrom
     * initialization from bootConfig.c, function bootLoad().  It is
     * only called from the kernel initialization if INCLUDE_NET_INIT
     * is #define'd.  That particular call is done indirectly through
     * the line "usrNetInit (BOOT_LINE_ADRS);" inside of the
     * usrConfig.c file, function usrRoot().  Note that this line of
     * code in usrConfig.c is NOT compiled if INCLUDE_NET_INIT is not
     * #define'd.  Since we must know the node (processor) number in
     * order to correctly configure the Tempe ITx slave decoders
     * (see commentary below), we must call the 2nd phase of Tempe
     * initialization code here.  If we don't have INCLUDE_NET_INIT
     * #define'd, this 2nd phase of Tempe initialization is only
     * done once (during bootrom initialization).  If INCLUDE_NET_INIT
     * is #defined, the 2nd phase of initialization will be done twice,
     * once during bootrom initialization and once during kernel
     * initialization.
     */

    if (procNumSet == FALSE)
        {

        /*
         * Set up the Tempe's CRG slave decoders.  We have to wait until
         * we know our processor number to do this since the decoder
	 * address location is dependent on the processor number.
         */

        if (TEMPE_REG_BASE != 0)
            {
            sysTempeInit2 ();
            }
        }

    procNumSet = TRUE;
    }

/* miscellaneous support routines */

/******************************************************************************
*
* sysPhbWinCapt - capture translation window information
*
* This routine captures the configuration of the host bridge PPC and PCI slave
* registers. This information is used to perform address translations from
* CPU to PCI addresses and vice versa.  
*
* RETURNS: N/A
*
* SEE ALSO: sysBusToLocalAdrs(), sysLocalToBusAdrs(), sysCpuToPciAdrs(),
* sysPciToCpuAdrs(), and sysPhbTransAdrs().
*/

LOCAL void sysPhbCapt (void)
    {
    UINT32 index;       /* window counter   */
    UINT32 size;	/* working variable */
    UINT32 base;	/* working variable */
    UINT32 cpuPciEnab;  /* enabling bits cpu -> pci */
    UINT32 pciCpuEnab;  /* enabling bits pci -> cpu */
    UINT32 remap;	/* hold for remapping register */
    UINT32 temp;
    PHB_OFFSETS_OUTBOUND * pPhbOffO; /* ptr to host bridge addr/offsets */
    PHB_OFFSETS_INBOUND  * pPhbOffI; /* ptr to host bridge addr/offsets */
    PHB_WIN_STRUCT * pPhbCpu; /* pointer to cpu windows */
    PHB_WIN_STRUCT * pPhbPci; /* pointer to pci windows */

    /* Two arrays are initialized:
     * sysPhbPciToCpuWin[] and sysPhbCpuToPciWin[].  Each
     * array element is of type PHB_WIN_STRUCT which consists
     * of three fields representing type, base address and limit
     * address.  Host bridge inbound translation windows are queried
     * first and an associated sysPhbPciToCpuWin[] entry is
     * filled in.  With PCI-based type, base and limit entries.
     * For each sysPhbPciToCpuWin[] entry made an  associated 
     * sysPhbCpuToPciWin[] will be  made at the same array index
     * value but this one will contain the CPU-based view of this
     * window.  Thus, the two arrays will contain encoding for 
     * identical length windows but one will be the PCI-bus view
     * of the address base and limit and the other will be the
     * CPU-based view of the address base and limit.  When a
     * sysBusToLocalAdrs() call is eventually made with an address
     * which falls within the mapping of this inbound address
     * window, the two arrays will be queried to perform the
     * associated address translation.
     *
     * In a similar fashion, after all the inbound windows are
     * queried and associated entries made in the two above-mentioned
     * arrays, the outbound windows will be queried and associated
     * entries again made first in sysPhbCpuToPciWin[] and
     * next in sysPhbPciToCpuWin[].  
     */

    /* initialize number of valid windows found */

    sysValidPhbWindows = 0;
    phbToPciWinCount = 0;
    pciToPhbWinCount = 0;

    /* point to window save arrays */

    pPhbCpu = &sysPhbCpuToPciWin[0];
    pPhbPci = &sysPhbPciToCpuWin[0];

    /* start with the cpu to pci windows (ppc slaves) */

    pPhbOffO = &sysPhbCpuWinOff[0];
    cpuPciEnab = MV64360_READ32 (MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE);

    /* loop through each window */

    for (index = 0; index < PHB_CPU_WIN_CNT; index++)
        {

	/* If the window is enabled ... */

	if ((~(cpuPciEnab) & (1 << pPhbOffO->enableBit)) != 0)
            {

            /* active window found, bump valid window counter */

            sysValidPhbWindows++;

            /* determine the window type (memory or i/o) */

	    if ( (pPhbOffO->base == CPUIF_PCI0_IO_BASE_ADDR) ||
		 (pPhbOffO->base == CPUIF_PCI1_IO_BASE_ADDR) )
	        pPhbCpu->winType = PCI_BAR_SPACE_IO;
            else
	        pPhbCpu->winType = PCI_BAR_SPACE_MEM;

            pPhbPci->winType = pPhbCpu->winType;

            /* Plug the window base and limit addresses */

	    pPhbCpu->winBase = (MV64360_READ32(MV64360_REG_BASE,
					       (pPhbOffO->base)) << 16);

	    size = MV64360_SIZE_FIELD_INV 
		   (MV64360_READ32 (MV64360_REG_BASE, pPhbOffO->size));
	    pPhbCpu->winLimit = pPhbCpu->winBase + size - 1;

	    /* 
             * The computation for the translated window base is a bit
             * tricky.  Let's take an example to show how the computation
             * is made - this is a contrived example just to illustrate
             * the computation and does not relate to the actual
             * programming on this board.
             *  
             * Assume the CPU base address is 0xe0000000 for 8MB
             * (0x00800000).  The pPhbCpu->winBase and pPhbCpu->winLimit
             * values would be 0xe0000000 and 0xe07fffff respectively.
             * Now suppose that this address translates to a PCI range of
             * 0x01000000 thru 0x01800000.  The register which gets
             * involved here is the remap register which would have the
             * value of 0x00000180.  To compute the PCI window base
             * perform the following:
             *
             * 1.  Compute the (size - 1) of the CPU window:
             *          (pPhbCpu->winLimit - pPhbCpu->winBase)
             *             0xe07fffff - 0xe0000000 = 0x007fffff
             *
             * 2.  Using this value computed in (1), mask the cpu base
             *     address:
             *           pPhbCpu->winBase & (value from (1))
             *              0xe0000000 & 0x007fffff  = 0x00000000
             *
             * 3.  Shift the remap register left 16 bits to align
             *     the remap value to the actual address:
             *
             *           remap << 16
             *           0x00000180 << 16 = 0x01800000
             *
             * 4.  Mask the value found in (3) with the inverted
             *     (size - 1) value:
             *
             *        (value from 3) &= ~(0x007fffff)
             *        0x01800000     &=   0xff800000 = 0x01800000
             *
             * 5.  Finally 'or' the value from (2) with the value
             *     from (4) to find the PCI window base:
             *       (value from 2) | (value from 4)
             *        0x00000000    |  0x01800000 = 0x018000000
	     *
	     * Note: we are only using the "low" remap register
	     * in this calculation.  The "high" remap register holds
	     * the high-order 32 bits of a 64-bit address and is
	     * assumed to be zero.
             */
                 
	    size = pPhbCpu->winLimit - pPhbCpu->winBase;
	    temp = pPhbCpu->winBase & size;
            remap = MV64360_READ32 (MV64360_REG_BASE, pPhbOffO->remap);
	    remap <<= 16;
	    remap &= ~size;
	    pPhbPci->winBase = remap | temp;

	    pPhbPci->winLimit = pPhbPci->winBase + size;

            /* advance in preparation for next valid window */

            pPhbCpu++;
            pPhbPci++;
	    phbToPciWinCount++;
            }

        /* advance to next set of Phb offsets */

        pPhbOffO++;
        }

    /* switch to Phb PCI to CPU windows (PCI slaves) */

    pPhbOffI = &sysPhbPciWinOff[0];

    /* loop through each window */

    for (index = 0; index < PHB_PCI_WIN_CNT; index++)
        {
        pciCpuEnab = MV64360_READ32 (MV64360_REG_BASE, pPhbOffI->enableReg);

	/* If the window is enabled ... */

	if ((~(pciCpuEnab) & (1 << pPhbOffI->enableBit)) != 0)
            {

            /* active window found, bump valid window counter */

            sysValidPhbWindows++;

            /* set the window type to memory or I/O based on MEM bit setting */

	    sysPciConfigRead (0, 0, pPhbOffI->function, 
			      pPhbOffI->base, 4, &base);

	    pPhbPci->winType = base & 1;
	    pPhbCpu->winType = pPhbPci->winType;

            /* Plug the window base and limit addresses */

	    pPhbPci->winBase = base & PCI_MEMBASE_MASK;
	    size = MV64360_READ32 (MV64360_REG_BASE, (pPhbOffI->size));
	    size = (((size >> 12) + 1) << 12); /* Cvrt encoding to real size */
	    pPhbPci->winLimit = pPhbPci->winBase + size - 1;

	    /*
	     * Now plug the cpu base and limit fields.  Note that the
	     * remap register in this calculation is formatted differently 
	     * than the remap register used in CPU to PCI outbound 
	     * translations dealt with above.  Here, the remap register needs 
	     * no shifting.  Otherwise the algorithm for base and limit
	     * computation is the same as the one presented above for the
	     * CPU to PCI case.
	     */

	    size = pPhbPci->winLimit - pPhbPci->winBase;
	    temp = pPhbPci->winBase & size;
	    remap = MV64360_READ32 (MV64360_REG_BASE, (pPhbOffI->remap));
	    remap &= ~size;
	    pPhbCpu->winBase = remap | temp;
	    pPhbCpu->winLimit = pPhbCpu->winBase + size;

            /* advance in preparation for next valid window */

            pPhbCpu++;
            pPhbPci++;
	    pciToPhbWinCount++;
            }

        /* advance to next set of Phb offsets */

        pPhbOffI++;
        }
    }

/******************************************************************************
*
* sysPhbTransAdrs - translate an address that passes through the Phb.
*
* This routine converts an address from a cpu to pci address or vice versa. It
* uses a pair of window arrays built during sysHwInit2 to guide the
* translation. The adrs parameter is the address to convert.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysPhbCapt(), sysPciToCpuAdrs(), sysBusToLocalAdrs(), 
* sysLocalToBusAdrs(),  and sysPhbTransAdrs().
*
*/

LOCAL STATUS sysPhbTransAdrs
    (
    UINT32            adrsSpace,   /* address space (memory or i/o ) */
    UINT32            adrs,        /* known address */
    UINT32 *          pTransAdrs,  /* pointer to the translated address */
    UINT32            winCnt,      /* number of open windows */
    PHB_WIN_STRUCT * pSrc,         /* pointer to the source windows */
    PHB_WIN_STRUCT * pDest         /* pointer to the destination windows */
    )
    {

    while (winCnt--)
        {

        /* check for a match on window type and in bounds */

        if ( (pSrc->winType == adrsSpace) &&
             (adrs >= pSrc->winBase) &&
             (adrs <= pSrc->winLimit) )
            {
            *pTransAdrs = ( adrs - pSrc->winBase + pDest->winBase );

            return (OK);
            };

        /* advance to next window */

        pSrc++;
        pDest++;
        }

    /*
     * no window was found to contain adrs. indicate that translation was
     * not possible.
     */

    return (ERROR);

    }

/******************************************************************************
*
* sysCpuToPciAdrs - translate a cpu address to a pci bus address
*
* This routine converts an address as seen from the cpu to the equivalent pci
* address, if it exists. The input address is the address as seen by the cpu.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* 
* SEE ALSO: sysPciToCpuAdrs(), sysBusToLocalAdrs(), sysLocalToBusAdrs()
* sysPhbCapt(), and sysPhbTransAdrs().
*/

LOCAL STATUS sysCpuToPciAdrs
    (
    int     adrsSpace,    /* bus address space where busAdrs resides */
    char *  localAdrs,    /* local address to convert */
    char ** pBusAdrs      /* where to return bus address */
    )
    {
    return (sysPhbTransAdrs (adrsSpace, (UINT32)localAdrs, 
                             (UINT32 *)pBusAdrs, sysValidPhbWindows, 
                             &sysPhbCpuToPciWin[0],
                             &sysPhbPciToCpuWin[0]) );
    }

/******************************************************************************
*
* sysPciToCpuAdrs - translate a pci bus address to a cpu address
*
* This routine converts an address as seen from the pci bus to the equivalent
* cpu address, if it exists. The input address is the address as seen by the
* pci bus.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysCpuToPciAdrs(), sysBusToLocalAdrs(), sysLocalToBusAdrs()
* sysPhbCapt(), and sysPhbTransAdrs().
*/

LOCAL STATUS sysPciToCpuAdrs
    (
    int     adrsSpace,    /* bus address space where busAdrs resides */
    char *  localAdrs,    /* local address to convert */
    char ** pBusAdrs      /* where to return bus address */
    )
    {
    return (sysPhbTransAdrs (adrsSpace, (UINT32)localAdrs, 
                             (UINT32 *)pBusAdrs, sysValidPhbWindows, 
                             &sysPhbPciToCpuWin[0], 
                             &sysPhbCpuToPciWin[0]) );
    }

/******************************************************************************
*
* sysLocalToBusAdrs - convert a local CPU address to a PCI bus address
*
* Given a CPU address, this routine returns a corresponding local PCI
* bus address provided that such an address exists.  The target PCI
* bus is selected by the adrsSpace parameter.  Legal values for this
* parameter are found in "pci.h".
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysBusToLocalAdrs()
*/

STATUS sysLocalToBusAdrs
    (
    int     adrsSpace,    /* bus address space where busAdrs resides */
    char *  localAdrs,    /* local address to convert */
    char ** pBusAdrs      /* where to return bus address */
    )
    {
    char * pciBusAdrs;

    if (IS_VME_ADDR_MOD(adrsSpace)) /* Is it a VME address? ... */
	{
	if (sysLocalToBusAdrs (PCI_SPACE_MEM_PRI, localAdrs,
			       &pciBusAdrs) == ERROR)
	    return (ERROR);
        else
	    return (sysPciToVmeAdrs (adrsSpace, pciBusAdrs, pBusAdrs));
	}

    switch (adrsSpace)
        {

        case PCI_SPACE_IO_PRI:

            /* convert from cpu address space to local pci space */

            if (sysCpuToPciAdrs (PCI_BAR_SPACE_IO, localAdrs, pBusAdrs) != OK )
                return (ERROR);

            break;

        case PCI_SPACE_MEMIO_PRI:
        case PCI_SPACE_MEM_PRI:

            /* convert from cpu address space to local pci address space */

            if (sysCpuToPciAdrs (PCI_BAR_SPACE_MEM, localAdrs, pBusAdrs) != OK )
                return (ERROR);

            break;

        default:
            return (ERROR);
        }
    return (OK);

    }

/******************************************************************************
*
* sysBusToLocalAdrs - convert a PCI bus address to a local CPU address
*
* Given a PCI address, this routine returns a corresponding local CPU
* bus address provided such an address exists.  The originating PCI
* bus is selected by the adrsSpace parameter.  Legal values for this
* parameter are found in "pci.h".
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysLocalToBusAdrs()
*/

STATUS sysBusToLocalAdrs
    (
    int     adrsSpace,    /* bus address space where busAdrs resides */
    char *  busAdrs,      /* bus address to convert */
    char ** pLocalAdrs    /* where to return local address */
    )
    {
    char * pciBusAdrs;

    if (IS_VME_ADDR_MOD(adrsSpace)) /* Is it a VME address? ... */
	{
	if (sysVmeToPciAdrs (adrsSpace, busAdrs, &pciBusAdrs) == OK)
	    return (sysBusToLocalAdrs(PCI_SPACE_MEMIO_PRI, pciBusAdrs,
		    pLocalAdrs));
        else
	    return (ERROR);
	}

    switch (adrsSpace)
        {

        case PCI_SPACE_IO_PRI:

            /*
             * translate from local PCI address space to CPU address
             * space.
             */

            return ( sysPciToCpuAdrs (PCI_BAR_SPACE_IO, busAdrs, pLocalAdrs) );

        case PCI_SPACE_MEMIO_PRI:
        case PCI_SPACE_MEM_PRI:

            /*
             * translate from local PCI address space to CPU address
             * space.
             */

            return (sysPciToCpuAdrs (PCI_BAR_SPACE_MEM, busAdrs, pLocalAdrs) );

        default:
            return (ERROR);
        }
    }

/*******************************************************************************
*
* sysInByte - reads a byte from an address.
*
* This routine reads a byte from a specified address.
*
* RETURNS: byte read.
*/

UCHAR sysInByte
    (
    ULONG addr	/* address of byte to be read */
    )
    {
    UCHAR data;

    data = *(UCHAR *)addr;
    return(data);
    }

/*******************************************************************************
*
* sysOutByte - writes a byte to an address.
*
* This routine writes a byte to a specified address.
*
* RETURNS: N/A.
*/

void sysOutByte
    (
    ULONG addr,	/* address of byte to write to */
    UCHAR data	/* data to be written */
    )
    {
    *(UCHAR *)addr = data;
    SYNC;
    }

/*******************************************************************************
*
* sysIn16 - reads a 16-bit unsigned value from an address.
*
* This routine reads a 16-bit unsigned value from a specified address.
*
* RETURNS: 16-bit unsigned value read.
*/

UINT16 sysIn16
    (
    UINT32 addr /* address of 16-bit value to read */
    )
    {
    UINT16 data;

    data = *(UINT16 *)addr;
    return(data);
    }

/*******************************************************************************
*
* sysOut16 - writes a 16-bit unsigned value to an address.
*
* This routine writes a 16-bit unsigned value to a specified address.
*
* RETURNS: N/A.
*/

void sysOut16
    (
    UINT32 addr,	/* address to be written to */
    UINT16 data		/* data to be written */
    )
    {
    *(UINT16 *)addr = data;
    SYNC;
    }

/*******************************************************************************
*
* sysIn32 - reads a 32-bit unsigned value from an address.
*
* This routine reads a 32-bit unsigned value from a specified address.
*
* RETURNS: 32-bit unsigned value read.
*/

UINT32 sysIn32
    (
    UINT32 addr	/* address of 32-bit value to read */
    )
    {
    UINT32 data;

    data = *(UINT32 *)addr;
    return(data);
    }

/*******************************************************************************
*
* sysOut32 - writes a 32-bit unsigned value to an address.
*
* This routine writes a 32-bit unsigned value to a specified address.
*
* RETURNS: N/A.
*/

void sysOut32
    (
    UINT32 addr,	/* address to be written to */
    UINT32 data		/* data to be written */
    )
    {
    *(UINT32 *)addr = data;
    SYNC;
    }

#ifdef INCLUDE_DPM
/*****************************************************************************
*
* sysConfigDpm - configure processor dynamic power management
*
* This function enables the dynamic power management feature.
*
* RETURNS: N/A
*/
void sysConfigDpm (void)
    {

    /* enable dynamic power management  */

    vxHid0Set ( vxHid0Get () | _PPC_HID0_DPM );

    }
#endif /* INCLUDE_DPM */

/*******************************************************************************
*
* sysBusTas - test and set a location across the VME bus
*
* This is a required VxWorks function which supports a test and set across the
* VME bus, a necessary component of shared-memory operation.  MVME6100s 
* which have Rev 1 of the Tempe (Tsi148) chip on them have only one viable way
* of performing the busTas and that is through generation of a RMW cycle 
* across the VME bus.  MVME6100s with Rev 2 or later chips will be able
* to be configured to use bus locking as part of the busTas function.
*
* RETURNS: TRUE if the value had not been set but is now, or
* FALSE if the value was set already.
*/

BOOL sysBusTas
    (
    char * adrs      /* address to be tested and set */
    )
    {
    BOOL state = FALSE;
    UINT32 vmeTasHwAssist = VME_TAS_HW_ASSIST;

    if ((vmeTasHwAssist - VME_BUS_RMW) == 0)
        state = sysVmeRmwTas(adrs);

    else if ((vmeTasHwAssist - VME_BUS_LOCK) == 0)
        state = sysVmeBusLockTas(adrs);

    return (state);
    }

/*******************************************************************************
*
* sysVmeBusLockTas - test and set a location across the VMEbus with bus locking
*
* This routine performs a test-and-set (TAS) instruction on the specified
* address.  Atomicity is enforced by locking processor interrupts during
* the operation (which keeps other processor resident agents from 
* simultaneously affecting the semaphore) and by locking the VME bus (which 
* keeps another VME agent from simultaneously updating the semaphore).
*
* RETURNS: TRUE if the value had not been set but is now.
* FALSE if the VMEbus cannot be locked or the value was already set.
*
* SEE ALSO: sysBusTasClear
*/

LOCAL BOOL sysVmeBusLockTas
    (
    char * adrs          /* address to be tested and set */
    )
    {
    BOOL state = FALSE;  /* semaphore state */
    int  lockKey;	 /* interrupt lock key */
    UINT32 lockValue;
    volatile UINT32 * lockAdrs = (UINT32 *)adrs;

    lockKey = intLock();

    if ((state = sysVmeBusLock ()) == TRUE)
        {

	state = FALSE;
        lockValue =  ((0x80 + sysProcNumGet ())<< 24);

        if (*(UINT32 *)lockAdrs == 0)
            {

	    /* No one else has the lock, grab it */

            while (*(UINT32 *)lockAdrs != lockValue)

		/* Reread it to push it out */

                *(UINT32 *)lockAdrs = lockValue;
            state = TRUE;
            }

	sysVmeBusUnlock ();
	intUnlock (lockKey);

        }

    return (state);

    }

/******************************************************************************
*
* sysBusTasClear - Clear a semaphore over the VME bus
*
* Using a VME bus access, unlock the semaphore at the specified address by
* writing a zero to it.
*
* RETURNS: NA
*
* SEE ALSO: sysBusTas
*/

LOCAL void sysBusTasClear(char * locAdrs)
    {
    char * vmeAdrs;
    char * adrs;

    adrs = locAdrs;

    /*
     * If the address to clear is a local DRAM address then we must first
     * convert it to a VME bus address.
     */

    if (IS_DRAM_ADDRESS(locAdrs))
	{

	/* First convert local DRAM access to a VME bus address */

	if (sysLocalToBusAdrs (VME_AM_EXT_SUP_DATA, locAdrs,
				&vmeAdrs) == ERROR)
	    return;

	/*
	 * Now convert VME bus address to a local address which provides
	 * access over the VME bus.
         */

        if (sysBusToLocalAdrs (VME_AM_EXT_SUP_DATA, vmeAdrs, &adrs) == ERROR) 
	    return;

	}

    *(UINT32 *)adrs = 0;	/* Clear the semaphore */
    (void) sysInLong ((UINT32)adrs);	/* Push it out */
    }

/*******************************************************************************
*
* sysVmeRmwTas - test and set a location across the VMEbus using RMW.
*
* This routine performs a test-and-set (TAS) instruction on the specified
* address.  Atomicity is enforced by using the Tempe's RMW capability to
* perform an indivisible operation across the VME bus.  To be successful
* managing semaphores across the bus it is necessary for both the setting
* (acquiring) and clearing (releasing) to be done across the VME bus.
*  Setting (acquiring) is done with a Tempe RMW cycle.  Clearing (releasing)
* is done with a simple write of zero done across the VME bus.
*
* RETURNS: TRUE if the value had not been set but is now, FALSE otherwise.
*
* SEE ALSO: sysBusTasClear
*/

LOCAL BOOL sysVmeRmwTas
    (
    char * locAdrs		/* local address of lock variable */
    )
    {
    char *      vmeAdrs;
    char *      adrs;

    adrs = locAdrs;

    /*
     * If the address to clear is a local DRAM address then we must first
     * convert it to a VME bus address.
     */

    if (IS_DRAM_ADDRESS(locAdrs))
	{

	/* First convert local DRAM access to a VME bus address */

	if (sysLocalToBusAdrs (VME_AM_EXT_SUP_DATA, locAdrs,
				&vmeAdrs) == ERROR)
	    return (FALSE);

	/*
	 * Now convert VME bus address to a local address which provides
	 * access over the VME bus.
         */

        if (sysBusToLocalAdrs (VME_AM_EXT_SUP_DATA, vmeAdrs, &adrs) == ERROR) 
	    return (FALSE);

	}

    return (sysVmeRmw(adrs));
    }

/******************************************************************************
*
* sysVmeRmw - Perform RMW cycle accross the VME bus
*
* This routine performs a RMW cycle across the VME bus.  To prevent 
* deadlocks, interrupts are disabled during the RMW operation.
*
* RETURNS: TRUE if the value had not been set but is now, FALSE otherwise.
*/

LOCAL BOOL sysVmeRmw
    (
    char * vmeAdrs          /* VME address to be tested and set */
    )
    {

    BOOL state = FALSE;  /* semaphore state */
    int  lockKey;        /* interrupt lock key */

    lockKey = intLock ();

    /* Enable RMW cycle */

    sysBusRmwEnable(VME_RMW_ENABLE_BITS,		/* enable bits */
                    VME_RMW_COMPARE_BITS,		/* cmpare */
                    VME_RMW_SWAP_TO_SET,		/* swap */
                    (char *)vmeAdrs);

    /* perform RMW to try and set TAS location */

    state = *((UINT32 *)vmeAdrs);
    EIEIO_SYNC;

    /* Disable RMW cycle */

    sysBusRmwDisable();

    /* unlock the interrupt */

    intUnlock (lockKey);

    /* return TAS test result */

    return (state ? FALSE : TRUE);
    }

/******************************************************************************
*
* sysNvRead - read one byte from NVRAM
*
* This routine reads a single byte from a specified offset in NVRAM.
*
* RETURNS: The byte from the specified NVRAM offset.
*/

UCHAR sysNvRead
    (
    ULONG    offset    /* NVRAM offset to read the byte from */
    )
    {
    return (sysInByte (NV_RAM_BASE + offset));
    }

/******************************************************************************
*
* sysNvWrite - write one byte to NVRAM
*
* This routine writes a single byte to a specified offset in NVRAM.
*
* RETURNS: N/A
*/

void sysNvWrite
    (
    ULONG    offset, /* NVRAM offset to write the byte to */
    UCHAR    data    /* datum byte */
    )
    {
    sysOutByte (NV_RAM_BASE + offset, data);
    }

/*******************************************************************************
*
* sysCpuCheck - confirm the CPU type
*
* This routine validates the cpu type.  If the wrong cpu type is discovered
* a message is printed using the serial channel in polled mode.
*
* RETURNS: N/A.
*/

void sysCpuCheck (void)
    {

    /* Check for a valid CPU type;  If one is found, just return */

#if    (CPU == PPC603)

    if ((CPU_TYPE == CPU_TYPE_603) || (CPU_TYPE == CPU_TYPE_603E) ||
        (CPU_TYPE == CPU_TYPE_603P))
        {
        return;
        }

#else    /* (CPU == PPC604) */

    if ((CPU_TYPE == CPU_TYPE_604) || (CPU_TYPE == CPU_TYPE_604E) ||
        (CPU_TYPE == CPU_TYPE_604R) || (CPU_TYPE == CPU_TYPE_750) ||
        (CPU_TYPE == CPU_TYPE_7400) || (CPU_TYPE == CPU_TYPE_7410) ||
        (CPU_TYPE == CPU_TYPE_7450) || (CPU_TYPE == CPU_TYPE_74X5) ||
        (CPU_TYPE == CPU_TYPE_74X7))
        {
        return;
        }

#endif    /* (CPU == PPC604) */

    /* Invalid CPU type; print error message and terminate */

    sysDebugMsg (sysWrongCpuMsg, EXIT_TO_SYSTEM_MONITOR); /* does not return */
    }

/******************************************************************************
*
* sysDelay - delay for approximately one millisecond
*
* Delay for approximately one milli-second.
*
* RETURNS: N/A
*/

void sysDelay (void)
    {
    sysUsDelay (1000);
    }

/******************************************************************************
*
* isPciAdrs - determine if local address is a PCI address.
*
* This routine determines if a given address maps to the PCI interface
* through a host bridge PCI mapping.
*
* RETURNS: TRUE if address is mapped to PCI interface, FALSE if not.
*/

BOOL isPciAdrs
    (
    char * adrs,	/* Local address to examine to see if it's PCI */
    char ** pciTrans    /* If it's PCI this will be the PCI bus address */
    )
    {
    int i;
    BOOL stat = FALSE;

    for (i = 0; i < phbToPciWinCount; i++)
        {
        if ( ((UINT32)adrs >= sysPhbCpuToPciWin[i].winBase) &&        
             ((UINT32)adrs <=  sysPhbCpuToPciWin[i].winLimit) )
            {
            stat = TRUE;
	    *(UINT32 *)(pciTrans) = 
		    sysPhbPciToCpuWin[i].winBase +
		    ((UINT32)adrs - sysPhbCpuToPciWin[i].winBase);
            break;
            }
        }
    return (stat);
    }

/******************************************************************************
*
* isVmeAdrs - determine if local address is a VME address.
*
* This routine determines if a given address maps to the VME interface
* through a Tempe outbound window.
*
* RETURNS: TRUE if address is mapped to VME interface, FALSE if not.
* If return value is TRUE, the VME bus translated address and the address
* of the associated Outbound control register is returned.
*/

BOOL isVmeAdrs
    (
    char   * adrs,      /* Local address to examine to see if it's VME */
    char  ** vmeTrans,  /* If it's VME, this will be the VME bus address */
    UINT32 * outRegSet  /* Number of outbound register set */
    )
    {
    BOOL stat = FALSE;
    char * pciTrans;

    /*
     * First determine if it's a PCI address since on this board,
     * any VME address must first be a PCI address.  If it is not
     * a PCI address we can return FALSE right away.
     */

    if (!isPciAdrs(adrs, &pciTrans))
        return FALSE;

    /* Now check the legitimate PCI address to see if it maps to VME */

    stat = isMappedToVME (adrs, vmeTrans, outRegSet);

    return (stat);
    }

/******************************************************************************
*
* sysProbeErrClr - clear errors associated with probing an address on a bus.
*
* This routine clears the error flags and conditions in the DAR, DSISR, SRR0
* and SRR1 PowerPC registers arising from probing addresses.
*
* RETURNS: N/A
*/

void sysProbeErrClr (void)
    {

    /* Clear relevant host bridge error registers */

    MV64360_WRITE32_PUSH(MV64360_REG_BASE, PCIIF_INTERRUPT_CAUSE_PCI0, 0);
    MV64360_WRITE32_PUSH(MV64360_REG_BASE, PCIIF_INTERRUPT_CAUSE_PCI1, 0);

    /* Clear PowerPC Data Access Exception Registers */

    vxDarSet   (0);
    vxDsisrSet (0);
    vxSrr0Set  (0);
    vxSrr1Set  (0);
    }

/******************************************************************************
*
* checkHbStat - check for master abort or target abort on host bridge.
*
* This routine checks the appropriate host bridge registers to determine
* if a master abort or target abort has occurred since the last time these
* error status were cleared.
*
* RETURNS: ERROR if master or target abort is indicated, OK otherwise.
*/

LOCAL STATUS checkHbStat (void)
    {
    UINT32 reg1;
    UINT32 reg2;
    STATUS stat;
  
    reg1 = MV64360_READ32(MV64360_REG_BASE, PCIIF_INTERRUPT_CAUSE_PCI0);
    reg2 = MV64360_READ32(MV64360_REG_BASE, PCIIF_INTERRUPT_CAUSE_PCI1);

    if (((reg1 | reg2) & ((PCIIF_INTERRUPT_CAUSE_PCIx_MMABORT_MASK) |
                          (PCIIF_INTERRUPT_CAUSE_PCIx_MTABORT_MASK))) == 0)
        stat = OK;
    else
        stat = ERROR;

    return (stat);
    }

/******************************************************************************
*
* sysPciProbe - probe a PCI bus address
*
* This routine probes an address on the PCI bus. All probing is done with
* interrupts disabled.
*
* RETURNS: OK or ERROR if address cannot be probed
*/

STATUS sysPciProbe
    (
    char   * adrs,    /* address to be probed */
    int      mode,    /* VX_READ or VX_WRITE */
    int      length,  /* 1, 2 or 4 byte probe */
    char   * pVal     /* address of value to write OR */
                      /* address of location to place value read */
    )
    {
    STATUS status = ERROR;
    int    oldLevel;
    char   reg8;

    /* probe performed with interrupts disabled */

    oldLevel = intLock ();

    /* flush PCI posted write buffer(s) */

    if ((status = vxMemArchProbe (adrs, mode, length, pVal)) == OK )
        {
	if (mode == VX_WRITE)
	    reg8 = *adrs;		/* flush the write */
	status = checkHbStat();
        }

    /* safe to re-enable interrupts */

    intUnlock (oldLevel);

    return (status);
    }

/******************************************************************************
*
* sysBusProbe - probe a bus address based on bus type.
*
* This routine is a function hook into vxMemProbe.  It determines which bus,
* PCI, VME or local is being probed based on the address to be probed.
* If the PCI bus is being probed, the sysPciProbe() routine is called to do the
* special PCI probing. If the local bus is being probed, the routine calls an
* architecture-specific probe routine. If the VME bus is being probed, the
* sysVMEProbe() routine is called to do the special VME probing.
*
* RETURNS: ERROR if the probed address does not respond or causes a MMU fault.
* Returns OK if the probed address responds.
*/

STATUS  sysBusProbe
    (
    char   * adrs,    /* address to be probed */
    int      mode,    /* VX_READ or VX_WRITE */
    int      length,    /* 1, 2 or 4 byte probe */
    char   * pVal    /* address of value to write OR */
            /* address of location to place value read */
    )
    {
    STATUS stat;
    UINT32   outRegSet;
    char   * pciTrans;
    char   * vmeTrans;
    int	   key;

    key = intLock ();	/* Perform probe with interrupts locked */

    /* Clear any existing errors/exceptions */

    sysProbeErrClr ();

    /* If the address is a DRAM address ... */

    if (((adrs == LOCAL_MEM_LOCAL_ADRS) ||
         (adrs >  LOCAL_MEM_LOCAL_ADRS)) &&
         (adrs < sysPhysMemTop()))
        stat = vxMemArchProbe (adrs, mode, length, pVal);

    /* ... else if the address is a VME address */

    else if (isVmeAdrs(adrs, &vmeTrans, &outRegSet))
        stat = sysVMEProbe(adrs, mode, length, pVal);

    /* ... else if the address is a PCI address */

    else if (isPciAdrs(adrs, &pciTrans))
        stat = sysPciProbe (adrs, mode, length, pVal);

    /*
     * ... else, the address is not DRAM, VME, or PCI but it could be
     * something like host bridge register space, an address which is none
     * of the above but it is still mapped and will respond to a probe.
     * If that's the case then vxMemArchProbe() should tell us so.
     */

    else
        stat = vxMemArchProbe (adrs, mode, length, pVal);

    /* Clear any errors/exceptions before exiting */

    sysProbeErrClr ();

    intUnlock (key);

    return (stat);
    }

/******************************************************************************
*
* sysUsDelay - delay at least the specified amount of time (in microseconds)
*
* This routine will delay for at least the specified amount of time using the
* lower 32 bit "word" of the Time Base register as the timer.  The accuracy of
* the delay increases as the requested delay increases due to a certain amount
* of overhead.  As an example, a requested delay of 10 microseconds is
* accurate within approximately twenty percent, and a requested delay of 100
* microseconds is accurate within approximately two percent.
*
* NOTE:  This routine will not relinquish the CPU; it is meant to perform a
* busy loop delay.  The minimum delay that this routine will provide is
* approximately 10 microseconds.  The maximum delay is approximately the
* size of UINT32; however, there is no roll-over compensation for the total
* delay time, so it is necessary to back off two times the system tick rate
* from the maximum.
*
* RETURNS: N/A
*/

void sysUsDelay
    (
    UINT32    delay        /* length of time in microsec to delay */
    )
    {
    UINT32 baselineTickCount;
    UINT32 ticksToWait;

    /*
     * Get the Time Base Lower register tick count, this will be used
     * as the baseline.
     */

    baselineTickCount = sysTimeBaseLGet();

    /*
     * Convert delay time into ticks
     *
     * The Time Base register and the Decrementer count at the same rate:
     * once per 4 System Bus cycles.
     *
     * e.g., 66666666 cycles     1 tick      1 second             16 tick
     *       ---------------  *  ------   *  --------          =  ----------
     *       second              4 cycles    1000000 microsec    microsec
     */

    if ((ticksToWait = delay * ((cpuBusSpeed / 4) / 1000000)) == 0)
        return;

    while ((sysTimeBaseLGet() - baselineTickCount) < ticksToWait);
    }

/******************************************************************************
*
* sysDebugMsg - print a debug string to the console in polled mode.
*
* This routine prints a message to the system console in polled mode and
* optionally exits to the monitor.
*
* RETURNS: N/A
*
*/

void
sysDebugMsg(char *str, UINT32 recovery)
{
  int msgSize;
  int msgIx;
  SIO_CHAN * pSioChan;        /* serial I/O channel */

  msgSize = strlen(str);

  sysSerialHwInit ();

  pSioChan = sysSerialChanGet (0);

  sioIoctl (pSioChan, SIO_MODE_SET, (void *) SIO_MODE_POLL);

  for(msgIx = 0; msgIx < msgSize; msgIx++)
  {
    while (sioPollOutput (pSioChan, str[msgIx]) == EAGAIN);
  }

  /* follow caller's error recovery policy. */
  if (recovery == EXIT_TO_SYSTEM_MONITOR)
    sysToMonitor (BOOT_NO_AUTOBOOT);
}

/*****************************************************************************
*
* sysPciInsertLong - Insert field into PCI data long
*
* This function writes a field into a PCI data long without altering any bits
* not present in the field.  It does this by first doing a PCI long read
* (into a temporary location) of the PCI data long which contains the field
* to be altered. It then alters the bits in the temporary location to match
* the desired value of the field.  It then writes back the temporary location
* with a PCI long write.  All PCI accesses are byte and the field to alter is
* specified by the "1" bits in the 'bitMask' parameter.
*
* RETURNS: N/A
*/

void sysPciInsertLong
    (
    UINT32 adrs,     /* PCI address */
    UINT32 bitMask,     /* Mask which defines field to alter */
    UINT32 data      /* data written to the offset */
    )
    {
    UINT32 temp;
    int key;

    key = intLock ();
    temp = sysPciInLong (adrs);
    temp = (temp & ~bitMask) | (data & bitMask);
    sysPciOutLong (adrs, temp);
    intUnlock (key);
    }

/*****************************************************************************
*
* sysPciInsertWord - Insert field into PCI data word
*
* This function writes a field into a PCI data word without altering any bits
* not present in the field.  It does this by first doing a PCI word read
* (into a temporary location) of the PCI data word which contains the field
* to be altered. It then alters the bits in the temporary location to match
* the desired value of the field.  It then writes back the temporary location
* with a PCI word write.  All PCI accesses are word and the field to alter is
* specified by the "1" bits in the 'bitMask' parameter.
*
* RETURNS: N/A
*/

void sysPciInsertWord
    (
    UINT32 adrs,       /* PCI address */
    UINT16 bitMask,     /* Mask which defines field to alter */
    UINT16 data      /* data written to the offset */
    )
    {
    UINT16 temp;
    int key;

    key = intLock ();
    temp = sysPciInWord (adrs);
    temp = (temp & ~bitMask) | (data & bitMask);
    sysPciOutWord (adrs, temp);
    intUnlock (key);
    }

/*****************************************************************************
*
* sysPciInsertByte - Insert field into PCI data byte
*
* This function writes a field into a PCI data byte without altering any bits
* not present in the field.  It does this by first doing a PCI byte read
* (into a temporary location) of the PCI data byte which contains the field
* to be altered. It then alters the bits in the temporary location to match
* the desired value of the field.  It then writes back the temporary location
* with a PCI byte write.  All PCI accesses are byte and the field to alter is
* specified by the "1" bits in the 'bitMask' parameter.
*
* RETURNS: N/A
*/

void sysPciInsertByte
    (
    UINT32 adrs,       /* PCI address */
    UINT8  bitMask,    /* Mask which defines field to alter */
    UINT8  data        /* data written to the offset */
    )
    {
    UINT8 temp;
    int key;

    key = intLock ();
    temp = sysPciInByte (adrs);
    temp = (temp & ~bitMask) | (data & bitMask);
    sysPciOutByte (adrs, temp);
    intUnlock (key);
    }

#ifdef BOOTROM_ERROR_MESSAGES
/*****************************************************************************
*
* reportBootromErrors - report errors detected during rom start.
*
* This function reports error bits left behind by the rom start sequence. These
* bits are stored during start-up and are printed by this routine.
*
* RETURNS: N/A
*/

void reportBootromErrors (void)
    {
    }
#endif /* BOOTROM_ERROR_MESSAGES */

#ifdef INCLUDE_SHOW_ROUTINES
/*****************************************************************************
*
* sysPhysMemDescShow - Display the entries in the sysPhysMemDesc[] table
*
* This function displays the entries in the sysPhysMemDesc[] table. 
*
* RETURNS: N/A
*/

void sysPhysMemDescShow ()
    {
    int i;

    for (i=0; i<sysPhysMemDescNumEnt; i++)
        {
        printf("\nEntry %d:\n",i);
        printf("  void *virtualAddr     = 0x%08x\n",
               (UINT32)sysPhysMemDesc[i].virtualAddr);
        printf("  void *physicalAddr    = 0x%08x\n",
               (UINT32)sysPhysMemDesc[i].physicalAddr);
        printf("  void len              = 0x%08x\n",
               (UINT32)sysPhysMemDesc[i].len);
        printf("  void initialStateMask = 0x%08x\n",
               (UINT32)sysPhysMemDesc[i].initialStateMask);
        printf("  void initialState     = 0x%08x\n",
               (UINT32)sysPhysMemDesc[i].initialState);
        }
     }

/*****************************************************************************
*
* memoryMapShow - Display the memory map
*
* This function displays the memory map as defined by the PCI and VME
* definitions in config.h and mv6100A.h. 
*
* RETURNS: N/A
*/

void memoryMapShow ()
    {
    pVal(VME_MEM_LOCAL_START);
    printf("\n\r");
    pVal(VME_A32_MSTR_LOCAL);
    pVal(VME_A32_MSTR_BUS);
    pVal(VME_A32_MSTR_SIZE);
    pVal(VME_A32_MSTR_END);
    printf("\n\r");
    pVal(VME_A24_MSTR_LOCAL);
    pVal(VME_A24_MSTR_BUS);
    pVal(VME_A24_MSTR_SIZE);
    pVal(VME_A24_MSTR_END);
    printf("\n\r");
    pVal(VME_A16_MSTR_LOCAL);
    pVal(VME_A16_MSTR_BUS);
    pVal(VME_A16_MSTR_SIZE);
    pVal(VME_A16_MSTR_END);
    printf("\n\r");
    pVal(VME_MEM_LOCAL_END);
    pVal(VME_MEM_SIZE);
    printf("\n\r");
    printf("\n\r");
    pVal(PCI_MEM_LOCAL_START)
    printf("\n\r");
    pVal(PCI0_MEM_LOCAL_START)
    printf("\n\r");
    pVal(PCI0_MSTR_MEMIO_LOCAL)
    pVal(PCI0_MSTR_MEMIO_BUS)
    pVal(PCI0_MSTR_MEMIO_SIZE)
    pVal(PCI0_MEMIO_LOCAL_END)
    printf("\n\r");
    pVal(PCI0_MSTR_MEM_LOCAL)
    pVal(PCI0_MSTR_MEM_BUS)
    pVal(PCI0_MSTR_MEM_SIZE)
    pVal(PCI0_MEM_LOCAL_END)
    printf("\n\r");
    pVal(PCI1_MEM_LOCAL_START)
    printf("\n\r");
    pVal(PCI1_MSTR_MEMIO_LOCAL)
    pVal(PCI1_MSTR_MEMIO_BUS)
    pVal(PCI1_MSTR_MEMIO_SIZE)
    pVal(PCI1_MEMIO_LOCAL_END )
    printf("\n\r");
    pVal(PCI1_MSTR_MEM_LOCAL)
    pVal(PCI1_MSTR_MEM_BUS)
    pVal(PCI1_MSTR_MEM_SIZE)
    pVal(PCI1_MEM_LOCAL_END)
    printf("\n\r");
    pVal(PCI_MEM_LOCAL_END)
    printf("\n\r");
    pVal(PCI_IO_LOCAL_START)
    printf("\n\r");
    pVal(PCI1_IO_LOCAL_START)
    printf("\n\r");
    pVal(ISA_MSTR_IO_LOCAL)
    pVal(ISA_MSTR_IO_BUS)
    pVal(ISA_MSTR_IO_SIZE)
    printf("\n\r");
    pVal(PCI1_MSTR_IO_LOCAL)
    pVal(PCI1_MSTR_IO_BUS)
    pVal(PCI1_IO_SIZE)
    pVal(PCI1_IO_LOCAL_END)
    printf("\n\r");
    pVal(PCI0_IO_LOCAL_START)
    pVal(PCI0_MSTR_IO_LOCAL)
    pVal(PCI0_MSTR_IO_BUS)
    pVal(PCI0_MSTR_IO_SIZE)
    printf("\n\r");
    pVal(PCI0_IO_LOCAL_END)
    printf("\n\r");
    pVal(PCI_IO_LOCAL_END)
    }

/*****************************************************************************
*
* translationWindowShow - Display PHB window translation values 
*
* This function displays the translation values which are used in the
* sysBusToLocalAdrs() and sysLocalToBusAdrs() routines.
*
* RETURNS: N/A
*/

void translationWindowShow ()
    {
    int i;

    printf("           sysPhbCpuToPciWin       sysPhbPciToCpuWin\n\n");

    for (i=0; i< sysValidPhbWindows; i++)
        {
        printf("Window %d\n",i);
        printf("  winType    0x%08x              0x%08x\n",
               sysPhbCpuToPciWin[i].winType, sysPhbPciToCpuWin[i].winType);
        printf("  winBase    0x%08x              0x%08x\n",
               sysPhbCpuToPciWin[i].winBase, sysPhbPciToCpuWin[i].winBase);
        printf("  winLimit   0x%08x              0x%08x\n",
               sysPhbCpuToPciWin[i].winLimit, sysPhbPciToCpuWin[i].winLimit);
        printf("\n");
        }
    }
 
#endif /* INCLUDE_SHOW_ROUTINES */
