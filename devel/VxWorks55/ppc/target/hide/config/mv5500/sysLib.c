/* sysLib.c - Motorola MVME5500 board series system-dependent library */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01s,14jul03,scb Fix compile warnings from diab compiler.
01r,16jun03,scb Fix to print model from VPD instead of "74X[57].
01q,09apr03,yyz Add case CPU_TYPE_74X7 to sysAltivecProbe() and add
                Add (CPU_TYPE == CPU_TYPE_74X7) to sysCpuCheck().
01p,08apr03,scb HiNT transparent PCI-PCI bridge support.
01o,01apr03,scb Bus probing support.
01n,10mar03,cak Added IPMC SCSI and serial support - required moving PCI 16-bit
                I/O from PCI bus 1.0 to PCI bus 0.0.
01m,20feb03,scb Disable only data cache (not instruction cache) for
		  shared memory support.
01l,10feb03,scb Inbound window mapping for GT64260 register set, 
		preliminary shared memory support.
01k,05feb03,cak Created a cpuBusSpeed global variable set in sysHwInit() with
		a call to sysCpuBusSpd() and used this in place of the routine
		throughout this file.
01j,29jan03,cak Replaced the reference to DEC_CLOCK_FREQ in sysUsDelay()
                with a call to sysCpuBusSpd().
01i,02dec02,scb Fix compile time warning regarding sysVmeVownTas.
01h,21nov02,scb Preliminary shared memory support.
01g,19nov02,scb Fix for gei buffers in cacheable copyback.
01f,15nov02,scb Reserve non-cached low memory for gei ethernet buffers.
01e,11nov02,scb Universe support.
01d,30oct02,yyz DMA support.
01c,25oct02,scb Made sysModel() "7457" capable.
01b,14oct02,yyz NVRAM/RTC/Failsafe Support.
01a,01oct02,scb Adapted from hxeb100 base (ver 01g).
*/

/*
DESCRIPTION
This library provides board-specific routines.  The chip drivers included are:

    i8250Sio.c         - Intel 8250 UART driver
    ppcDecTimer.c      - PowerPC decrementer timer library (system clock)
    byteNvRam.c        - byte-oriented generic non-volatile RAM library
    pciConfigLib.c     - PCI configuration library
    universe.c         - Tundra Universe II chp VME-to-PCI support.
    sysMv64260Init.c   - GT-64260 host bridge initialization support.
    sysMv64260Phb.c    - GT-64260 host bridge initialization support.
    sysMv64260Int.c    - GT-64260 interrupt support.
    sysMv64260DevBus.c - GT-64260 device bus support.
    sysMv64260Dma.c    - GT-64260 DMA support.

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
#include "vmLib.h"
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
#include "end.h"
#include "mv64260.h"
#include "mv64260Smc.h"
#include "mv64260Dma.h"
#include "universe.h"
#if ((defined INCLUDE_PCX7307_SIO) || (defined INCLUDE_Z85230_SIO))
#include "ns8730xSuperIo.h"
#endif /* INCLUDE_PCX7307_SIO, INCLUDE_Z85230_SIO */

#ifdef INCLUDE_ALTIVEC
#include "altivecLib.h"
IMPORT int (* _func_altivecProbeRtn) (void) ;
#endif /* INCLUDE_ALTIVEC */

/* defines */

#ifdef INCLUDE_DPM    /* dynamic power management */

#   define _PPC_HID0_BIT_DPM    11 /* dynamic power management bit */
#   define _PPC_HID0_DPM        (1<<(31-_PPC_HID0_BIT_DPM))

#endif /* INCLUDE_DPM */

#define SYS_DECS_DRAM_INDEX     1

/* defines for sysBusTas() and sysBusTasClear() */

#define VMEBUS_OWNER        (*UNIVERSE_MAST_CTL & LONGSWAP(MAST_CTL_VOWN_ACK))
#define CPU_CLOCKS_PER_LOOP	10
#define LOCK_TIMEOUT		10
#define UNLOCK_TIMEOUT		10000
#define DECREMENTER_PRESCALE	4
#define USECS_PER_SECOND	1000000

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
	 * Reserve space for internally mapped GT-64260 register set.
	 * It will be mapped into PCI prefetchable memory space immediately
	 * after executing sysPciAutoConfig().  This mapping is absolutely
	 * necessary in order for inbound mapping from PCI bus to DRAM
	 * to work - a "feature" of the GT-64260.
	 */

        PCI0_MSTR_MEM_LOCAL + MV64260_REG_SPACE_SIZE,
        PCI0_MSTR_MEM_SIZE - MV64260_REG_SPACE_SIZE,
        PCI0_MSTR_MEM_BUS + MV64260_REG_SPACE_SIZE,

	/*
	 * Note that for PCI autoconfig to properly program PCI-PCI
	 * bridge base and limit "upper" BARs, we need to initialize
	 * the isaMstrIoLocal, isaMstrIoSize, and isaMstrIoBus with
	 * all of the I/O space, even that space which exceeds 16-bits
	 * on the PCI I/O bus.  We thus leave the pciMstrIoLocal,
	 * pciMstrIoSize and pciMstrIoBus fields as zero.  Note that
	 * if pciAutoConfig is ever modified so that it references
	 * the 32-bit I/O space parameters (pciMstrIo...) for programming
	 * the PCI-PCI bridge we would need to arrange the following
	 * six entires as follows:
	 *
         *	ISA_MSTR_IO_LOCAL,     
         *	ISA_MSTR_IO_SIZE,
         *	ISA_MSTR_IO_BUS,
         *	PCI0_MSTR_IO_LOCAL,
         * 	PCI0_MSTR_IO_SIZE,
         *	PCI0_MSTR_IO_BUS,
         */

        ISA_MSTR_IO_LOCAL,    /* 16-bit I/O space, all on bus 0 */
        ISA_MSTR_IO_SIZE + PCI0_MSTR_IO_SIZE,
        ISA_MSTR_IO_BUS,
        0x0,
        0x0,
        0x0,

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
        PCI1_MSTR_MEM_LOCAL,
        PCI1_MSTR_MEM_SIZE,
        PCI1_MSTR_MEM_BUS,
        0x0,    /* 16-bit I/O bus disabled */
        0x0,
        0x0,
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
    UINT32 limit;
    UINT32 remap;
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
        CPUIF_PCI0_MEM0_LO_DECODE_ADDR,
	CPUIF_PCI0_MEM0_HI_DECODE_ADDR,
        CPUIF_PCI0_MEM0_LO_RMAP
        },

        {
        CPUIF_PCI0_MEM1_LO_DECODE_ADDR,
	CPUIF_PCI0_MEM1_HI_DECODE_ADDR,
        CPUIF_PCI0_MEM1_LO_RMAP
        },

        {
        CPUIF_PCI0_MEM2_LO_DECODE_ADDR,
	CPUIF_PCI0_MEM2_HI_DECODE_ADDR,
        CPUIF_PCI0_MEM2_LO_RMAP
        },

        {
        CPUIF_PCI0_MEM3_LO_DECODE_ADDR,
 	CPUIF_PCI0_MEM3_HI_DECODE_ADDR,
        CPUIF_PCI0_MEM3_LO_RMAP
        },

        {
        CPUIF_PCI0_IO_LO_DECODE_ADDR,
	CPUIF_PCI0_IO_HI_DECODE_ADDR,
	CPUIF_PCI0_IO_RMAP
        },

        {
        CPUIF_PCI1_MEM0_LO_DECODE_ADDR,
	CPUIF_PCI1_MEM0_HI_DECODE_ADDR,
	CPUIF_PCI1_MEM0_LO_RMAP
        },

        {
        CPUIF_PCI1_MEM1_LO_DECODE_ADDR,
	CPUIF_PCI1_MEM1_HI_DECODE_ADDR,
	CPUIF_PCI1_MEM1_LO_RMAP
        },

        {
        CPUIF_PCI1_MEM2_LO_DECODE_ADDR,
	CPUIF_PCI1_MEM2_HI_DECODE_ADDR,
	CPUIF_PCI1_MEM2_LO_RMAP
        },

        {
        CPUIF_PCI1_MEM3_LO_DECODE_ADDR,
	CPUIF_PCI1_MEM3_HI_DECODE_ADDR,
	CPUIF_PCI1_MEM3_LO_RMAP
        },

        {
        CPUIF_PCI1_IO_LO_DECODE_ADDR,
	CPUIF_PCI1_IO_HI_DECODE_ADDR,
	CPUIF_PCI1_IO_RMAP
        }

    };

PHB_OFFSETS_INBOUND    sysPhbPciWinOff [] =
    {
	{
	  PCIIF_FUNC0_SCS0_BASE_ADDR_PCICFG, /* 0x10 */
	  PCI_FUNC0,                         /* 0x0 */
	  PCIIF_SCS0_BAR_SZ_PCI0,            /* 0xC08 */
	  PCIIF_SCS0_BASE_ADDR_RMAP_PCI0,    /* 0xC48 */
	  PCIIF_BASE_ADDR_REG_ENB_PCI0,      /* 0xC3C */
	  PCIIF_BASE_ADDR_REG_ENB_SCS0_BIT   /* 0x0 */
	},

	{
	  PCIIF_FUNC0_SCS1_BASE_ADDR_PCICFG, /* 0x14 */
	  PCI_FUNC0,                         /* 0x0 */
	  PCIIF_SCS1_BAR_SZ_PCI0,            /* 0xD08 */
	  PCIIF_SCS1_BASE_ADDR_RMAP_PCI0,    /* 0xD48 */
	  PCIIF_BASE_ADDR_REG_ENB_PCI0,      /* 0xC3C */
	  PCIIF_BASE_ADDR_REG_ENB_SCS1_BIT   /* 0x1 */
	},

	{
	PCIIF_FUNC0_SCS2_BASE_ADDR_PCICFG,
	PCI_FUNC0,
	PCIIF_SCS2_BAR_SZ_PCI0,
	PCIIF_SCS2_BASE_ADDR_RMAP_PCI0,
	PCIIF_BASE_ADDR_REG_ENB_PCI0,
	PCIIF_BASE_ADDR_REG_ENB_SCS2_BIT
	},

	{
	PCIIF_FUNC0_SCS3_BASE_ADDR_PCICFG,
	PCI_FUNC0,
	PCIIF_SCS3_BAR_SZ_PCI0,
	PCIIF_SCS3_BASE_ADDR_RMAP_PCI0,
	PCIIF_BASE_ADDR_REG_ENB_PCI0,
	PCIIF_BASE_ADDR_REG_ENB_SCS3_BIT
	},

	{
	PCIIF_FUNC0_SCS0_BASE_ADDR_PCICFG_ALT,
	PCI_FUNC0,
	PCIIF_SCS0_BAR_SZ_PCI1,
	PCIIF_SCS0_BASE_ADDR_RMAP_PCI1,
	PCIIF_BASE_ADDR_REG_ENB_PCI1,
	PCIIF_BASE_ADDR_REG_ENB_SCS0_BIT
	},

	{
	PCIIF_FUNC0_SCS1_BASE_ADDR_PCICFG_ALT,
	PCI_FUNC0,
	PCIIF_SCS1_BAR_SZ_PCI1,
	PCIIF_SCS1_BASE_ADDR_RMAP_PCI1,
	PCIIF_BASE_ADDR_REG_ENB_PCI1,
	PCIIF_BASE_ADDR_REG_ENB_SCS1_BIT
	},

	{
	PCIIF_FUNC0_SCS2_BASE_ADDR_PCICFG_ALT,
	PCI_FUNC0,
	PCIIF_SCS2_BAR_SZ_PCI1,
	PCIIF_SCS2_BASE_ADDR_RMAP_PCI1,
	PCIIF_BASE_ADDR_REG_ENB_PCI1,
	PCIIF_BASE_ADDR_REG_ENB_SCS2_BIT
	},

	{
	PCIIF_FUNC0_SCS3_BASE_ADDR_PCICFG_ALT,
	PCI_FUNC0,
	PCIIF_SCS3_BAR_SZ_PCI1,
	PCIIF_SCS3_BASE_ADDR_RMAP_PCI1,
	PCIIF_BASE_ADDR_REG_ENB_PCI1,
	PCIIF_BASE_ADDR_REG_ENB_SCS3_BIT
	},
    };

#define PHB_CPU_WIN_CNT \
        (sizeof (sysPhbCpuWinOff)/ sizeof (PHB_OFFSETS_OUTBOUND))
#define PHB_PCI_WIN_CNT \
        (sizeof (sysPhbPciWinOff)/ sizeof (PHB_OFFSETS_INBOUND))
#define PHB_WIN_CNT (PHB_CPU_WIN_CNT + PHB_PCI_WIN_CNT)

/*
 * Windows for Universe inbound/outbound transactions used in bus to local
 * and local to bus translation algorithms.
 */

typedef struct _vmeVsiWin
    {
    UINT32 bd;
    UINT32 bs;
    UINT32 to;
    UINT32 ctl;
    } VME_VSI_WIN;

typedef struct _vmeLsiWin
    {
    UINT32 bd;
    UINT32 bs;
    UINT32 to;
    UINT32 ctl;
    } VME_LSI_WIN;

VME_VSI_WIN vmeVsiWin [NUM_VSI_WIN] = { { 0,0,0,0 } };
VME_LSI_WIN vmeLsiWin [NUM_LSI_WIN] = { { 0,0,0,0 } };

#define UNIV_VSI_CAPT(i,s,d,o,c) { vmeVsiWin[i].bs = sysInLong((UINT32)(s)); \
				   vmeVsiWin[i].bd = sysInLong((UINT32)(d)); \
				   vmeVsiWin[i].to = sysInLong((UINT32)(o)); \
				   vmeVsiWin[i].ctl = sysInLong((UINT32)(c)); }

#define UNIV_LSI_CAPT(i,s,d,o,c) { vmeLsiWin[i].bs = sysInLong((UINT32)(s)); \
				   vmeLsiWin[i].bd = sysInLong((UINT32)(d)); \
				   vmeLsiWin[i].to = sysInLong((UINT32)(o)); \
				   vmeLsiWin[i].ctl = sysInLong((UINT32)(c)); }

/* globals */

IMPORT int pciMaxBus;
static char * sysPhysMemSize = NULL;    /* ptr to top of mem + 1 */
UINT globalBusIF;
UINT32 cpuBusSpeed;

/*
 * mmuPcpBatInitMPC74x5 initializes the standard 4 (0-3) I/D BATs &
 * the additional 4 (4-7) I/D BATs present on the MPC74[45]5.
 */

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

    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* I BAT 1 */

    0, 0,

    /* I BAT 2 */

    0, 0,

    /* I BAT 3 */

    0, 0,

    /* D BAT 0 */

    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* D BAT 1 */

    0, 0,

    /* D BAT 2 */

    0, 0,

    /* D BAT 3 */

    0, 0,

    /*
     * These entries are for the I/D BATs (4-7) on the MPC7455/755.
     * They should be defined in the following order.
     * IBAT4U,IBAT4L,IBAT5U,IBAT5L,IBAT6U,IBAT6L,IBAT7U,IBAT7L,
     * DBAT4U,DBAT4L,DBAT5U,DBAT5L,DBAT6U,DBAT6L,DBAT7U,DBAT7L,
     */

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
 *
 * The following applies whenever the MV64260 ethernet ports are enabled
 * in config.h by defining INCLUDE_WANCOM_END:
 *
 * Because of a MV64260 errata ("Res #COMM-2 Communication ports access
 * to SDRAM coherent regions," in the document "Marvell Errata and
 * Restrictions GT-64260A-B-0, Doc. No. MV-S500071-00 Rev. D") regarding
 * non-functional SDMA snoop, all system controller ethernet descriptors
 * and data buffers must be in non-cacheable memory. An additional
 * sysPhysMemDesc descriptor provides the space in low memory above the
 * vector table. Based on current #defines, each port receives two 64K
 * pages based on 64 four word descriptors per port (32 receive and
 * 32 transmit) with 32 1500 byte receive buffers. This reduces the
 * typical vector table partition size by 256 KBytes (128 Kbytes per port).
 * The number of descriptors and memory placement is not adjustable
 * via the init string in configNet.h.
 */

PHYS_MEM_DESC sysPhysMemDesc [] =
    {

#ifdef INCLUDE_WANCOM_END

    {

    /* Vector Table and Interrupt Stack */

    (void *) LOCAL_MEM_LOCAL_ADRS,
    (void *) LOCAL_MEM_LOCAL_ADRS,
    ETHER_MEM_START_ADDR,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    {

    /* if applicalable - Shared-memory pool */

    (void *) SM_BASE,
    (void *) SM_BASE,
    SM_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    {

    /* A little more DRAM */

    (void *) (LOCAL_MEM_LOCAL_ADRS + SM_MEM_ADRS + SM_MEM_SIZE),
    (void *) (LOCAL_MEM_LOCAL_ADRS + SM_MEM_ADRS + SM_MEM_SIZE),
    (ETHER_MEM_START_ADDR - SM_LIMIT),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    {

    /* System controller ethernet descriptors and buffers (non-cacheable) */

    (void *) ETHER_MEM_START_ADDR,
    (void *) ETHER_MEM_START_ADDR,
    ETHER_TOTAL_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_MEM_COHERENCY
    },

#else /* ! INCLUDE_WANCOM_END */

    {

    /* Vector Table and Interrupt Stack */

    (void *) LOCAL_MEM_LOCAL_ADRS,
    (void *) LOCAL_MEM_LOCAL_ADRS,
    (SM_MEM_ADRS - LOCAL_MEM_LOCAL_ADRS),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    {

    /* if applicalable - Shared-memory pool */

    (void *) SM_BASE,
    (void *) SM_BASE,
    SM_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    {

    /* Remaining DRAM to RAM_LOW_ADRS */

    (void *) (LOCAL_MEM_LOCAL_ADRS + SM_MEM_ADRS + SM_MEM_SIZE),
    (void *) (LOCAL_MEM_LOCAL_ADRS + SM_MEM_ADRS + SM_MEM_SIZE),
    (RAM_LOW_ADRS - (LOCAL_MEM_LOCAL_ADRS + SM_MEM_ADRS + SM_MEM_SIZE)),
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

#endif /* INCLUDE_WANCOM_END */

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

    /* Access to PCI 16-bit ISA I/O space (16-bit I/O is on PCI bus 0.0 only) */

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

    /* MV64260 Internal Register Space (64 Kb) */

    (void *) MV64260_REG_BASE,
    (void *) MV64260_REG_BASE,
    MV64260_REG_SPACE_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Devices on Device Bus - dynamically adjusted */

    (void *) CPUIF_CS1_LO_DECODE_ADDR,	/* dynamic adjustment in sysHwInit() */
    (void *) CPUIF_CS1_LO_DECODE_ADDR,	/* dynamic adjustment in sysHwInit() */
    0,					/* dynamic fill in sysHwInit() */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Flash Nonboot Bank on Device Bus (devCS0) - dynamically adjusted */

    (void *) CPUIF_CS0_LO_DECODE_ADDR,	/* dynamic adjustment in sysHwInit() */
    (void *) CPUIF_CS0_LO_DECODE_ADDR,	/* dynamic adjustment in sysHwInit() */
    0,					/* dynamic fillin sysHwInit() */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Flash Boot Bank (BootCS) - dynamically adjusted */

    (void *) CPUIF_BOOTCS_LO_DECODE_ADDR, /* dynamic adj in sysHwInit() */
    (void *) CPUIF_BOOTCS_LO_DECODE_ADDR, /* dynamic adj in sysHwInit() */
    0,					  /* dynamic fill in sysHwInit() */
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
int   phbToPciWinCount = 0;	 /* number of valid Phb to PCI windows */
int   pciToPhbWinCount = 0;	 /* number of valid PCI to PHB windows */

UINT32 sysProbeFault = 0;       /* used by dsi exception trap handler */

/* locals */

LOCAL char sysModelStr[80];
LOCAL char sysWrongCpuMsg[] = WRONG_CPU_MSG;

/* forward declarations */

char *  sysModel (void);
char *  sysBspRev (void);
STATUS  sysPciConfigRead (int, int, int, int, int, void *);
STATUS  sysPciConfigWrite (int, int, int, int, int, UINT32);
STATUS  sysPciConfigSpcl (int, UINT32);
STATUS  sysPciConfigWriteAlt (int, int, int, int, int, UINT32);
void    sysHwInit (void);
LOCAL   void bridgeToUniverseProg(void);
LOCAL   UINT32  getUnivBaseAdrs(void);
UINT32  sysDramSize (void);
char *  sysPhysMemTop (void);
char *  sysMemTop (void);
STATUS  sysToMonitor (int);
void    sysHwInit2 (void);
int     sysProcNumGet (void);
void    sysProcNumSet (int);
LOCAL   void sysPhbCapt (void);
LOCAL   STATUS sysPhbTransAdrs (UINT32, UINT32, UINT32 *, UINT32,
                                PHB_WIN_STRUCT *, PHB_WIN_STRUCT *);
LOCAL   STATUS sysCpuToPciAdrs (int, char *, char **);
LOCAL   STATUS sysPciToCpuAdrs (int, char *, char **);
LOCAL   void sysUniverseCapt (void);
STATUS  sysLocalToBusAdrs (int, char *, char **);
STATUS  sysBusToLocalAdrs (int, char *, char **);
UCHAR   sysInByte (ULONG);
void    sysOutByte (ULONG, UCHAR);
UINT16  sysIn16 (UINT32);
void    sysOut16 (UINT32, UINT16);
UINT32  sysIn32 (UINT32);
void    sysOut32 (UINT32, UINT32);
#ifdef INCLUDE_DPM
    void    sysConfigDpm (void);
#endif
BOOL    sysBusTas (char *);
void    sysBusTasClear (volatile char *);
#ifdef ANY_BRDS_IN_CHASSIS_NOT_RMW
    LOCAL   BOOL sysVmeVownTas (char *);
#else
    LOCAL BOOL sysVmeRmwTas (char *);
#endif
UCHAR   sysNvRead (ULONG);
void    sysNvWrite (ULONG,UCHAR);
void    sysCpuCheck (void);
void    sysUsDelay (UINT);
void    reportBootromErrors (void);
void    sysDelay (void);
LOCAL   void sysProbeErrClr (void);
LOCAL   void sysProbeErrClr (void);
LOCAL   STATUS  checkHbStat (void);
LOCAL   STATUS  sysPciProbe (char *, int, int, char *);
LOCAL   STATUS  sysVmeProbe (char *, int, int, char *, char *);
BOOL    isPciAdrs (char *, char **);
BOOL    isVmeAdrs (char *, char **, char **);
STATUS  sysBusProbe (char *, int, int, char *);
void    sysUsDelay (UINT32);
void    sysDebugMsg (char *, UINT32);
void    sysPciInsertLong (UINT32, UINT32, UINT32);
void    sysPciInsertWord (UINT32, UINT16, UINT16);
void    sysPciInsertByte (UINT32, UINT8, UINT8);
void    vmxExcLoad (void);

/* externals */

IMPORT void   sysMv64260DevBusInit (void);
IMPORT void   sysMv64260PhbInit (void);
IMPORT void   sysClkIntCIO (void);
IMPORT STATUS sysMemProbeSup (int length, char * src, char * dest);
IMPORT int    sysProbeExc (void);
IMPORT VOIDFUNCPTR      smUtilTasClearRtn;
IMPORT UINT32 sysTimeBaseLGet (void);
IMPORT UINT   sysHid1Get (void);
IMPORT UINT32 sysDramSize (void);
IMPORT END_TBL_ENTRY endDevTbl[];
IMPORT int    pciConfigBdfPack (int busNo, int deviceNo, int funcNo);
IMPORT UCHAR  sysProductStr[];
IMPORT UINT32 sysCpuBusSpd (void);

/* BSP DRIVERS */

/*
 * Included as ".obj" in Makefile
 *
 * #include "i8250Sio.c"
 */

#include "pci/pciConfigLib.c"		/* PCI Support */
#include "pci/pciIntLib.c"		/* PCI Support */
#include "sysBusPci.c"			/* PCI Auto-Configuration Support */
#include "sysMotVpd.c"                  /* VPD Support */
#include "sysMv64260Init.c"		/* MV64260 register init handler */
#include "sysMv64260Int.c"		/* MV64260 main interrupt handler */
#include "sysMv64260DevBus.c"		/* MV64260 device bus support */
#include "sysMv64260Phb.c"		/* MV64260 host bridge support */
#include "universe.c"			/* Tundra Universe II support */

/*Sergey: copied from mv6100*/
#ifdef INCLUDE_DS1621
#   include "sysDs1621TempSensor.c"	/* DS1621 Temp Sensor Support */
#endif /* INCLUDE_DS1621 */

#if (defined INCLUDE_CACHE_L2)
#   include "sysL2Cache.c"              /* L2 Cache Support */
#endif /* INCLUDE_CACHE_L2 */

#if (defined INCLUDE_CACHE_L3)
#   include "sysL3Cache.c"              /* L3 Cache Support */
#endif /* INCLUDE_CACHE_L3 */

#if (defined INCLUDE_MV64260_DMA)
#   include "sysDma.c"                  /* Top-level DMA user routines */
#   include "sysMv64260Dma.c"           /* MV64260 8 Channel DMA Support */
#endif /* INCLUDE_MV64260_DMA */

#include "sysSerial.c"			/* Serial Support */
#include "mem/byteNvRam.c"		/* NVRAM Support */
#include "timer/ppcDecTimer.c"		/* PPC603 & 604 have on chip timers */

#ifdef INCLUDE_SCSI
#   include "sysScsi.c"                 /* sysScsiInit routine */
#endif /* INCLUDE_SCSI */

#ifdef INCLUDE_SHOW_ROUTINES
#   include "pci/pciConfigShow.c"	/* Display of PCI config space */
#   include "sysMotVpdShow.c"           /* VPD Show Routines */
#   include "sysMv64260SmcShow.c"	/* SMC & SPD Show Routines */
#endif /* INCLUDE_SHOW_ROUTINES */

#ifdef INCLUDE_AUX_CLK
#   include "sysMv64260AuxClk.c"	/* MV64260 AuxClock support */
#endif /* INCLUDE_AUX_CLK */

#ifdef INCLUDE_RTC
#   include "sysRtc.c"
#endif /* INCLUDE_RTC */		/* Real-time clock support */

#ifdef INCLUDE_FAILSAFE
#   include "sysFailsafe.c"
#endif /* INCLUDE_FAILSAFE */		/* Failsafe (watchdog) timer support */

#include "m48t37.c"			/* M48T37 Timekeeper SRAM */

#ifdef INCLUDE_NETWORK
#   ifdef INCLUDE_END
#       ifdef INCLUDE_GEI_END
#           include "sysGei82543End.c"  /* Gigabit 82544 etherent */
#       endif /* INCLUDE_GEI_END */
#       ifdef INCLUDE_WANCOM_END
#           include "sysWancomEnd.c"    /* MV64260 10/100 Mbit ethernet */
#       endif /* INCLUDE_WANCOM_END */
#   endif /* INCLUDE_END */
#endif /* INCLUDE_NETWORK */

#ifdef INCLUDE_PCX7307_SIO
#   include "ns8730xSuperIo.c"          /* National PCx7307 SIO Support */
#endif /* INCLUDE_PCX7307_SIO */

#if ((defined INCLUDE_PCX7307_SIO) || (defined INCLUDE_Z85230_SIO))
#   include "sysIbc.c"                     /* IBC Interrupt Controller */
#endif /* INCLUDE_PCX7307_SIO, INCLUDE_Z85230_SIO */

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
	     * The MPC7445/7455	processors have	identical PVR values,
	     * therefore it is necessary to read the "microprocessor type"
	     * packet in the VPD to differentiate the processors.
	     */

	    sprintf(cpuStr, "%s", sysCpuStr);
	    break;
	case CPU_TYPE_74X7:

	    /*
	     * The MPC7447/7457	processors have	identical PVR values,
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
* RETURNS: OK, or ERROR if size not supported.
*/

STATUS sysPciConfigRead
    (
    int bus,		/* PCI bus number */
    int device,		/* PCI device number */
    int func,		/* PCI function number */
    int reg,		/* PCI register offset */
    int size,		/* Size of transfer (1,2, or 4 bytes) */
    void * pResult	/* Value is returned here */
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
            configAddrReg = PCI_CONFIG_ADDR_BASE_0;
            configDataReg = PCI_CONFIG_DATA_BASE_0;
            busNo = bus;
            }
        else if ((UINT32)bus > PCI_BUS_ALIAS)
            {
            configAddrReg = PCI_CONFIG_ADDR_BASE_1;
            configDataReg = PCI_CONFIG_DATA_BASE_1;
            busNo = (bus - PCI_BUS_ALIAS - 1);
            }
        }
    else
        {
        if (globalBusIF == 0)
            {
            configAddrReg = PCI_CONFIG_ADDR_BASE_0;
            configDataReg = PCI_CONFIG_DATA_BASE_0;
            }
        else if (globalBusIF == 1)
            {
            configAddrReg = PCI_CONFIG_ADDR_BASE_1;
            configDataReg = PCI_CONFIG_DATA_BASE_1;
            }
        else
            return (ERROR);
        busNo = bus;
        }

    PCI_OUT_LONG(configAddrReg, pciConfigBdfPack(busNo, device, func) |
                 (reg & 0xFC) | 0x80000000);

/*Sergey
{
  char txt[100];
  sprintf(txt,"+++> %d %d %d %d %d %d\r\n",
PCI_BUS_ALIAS,PCI_CONFIG_ADDR_BASE_0,PCI_CONFIG_DATA_BASE_0,
PCI_CONFIG_ADDR_BASE_1,PCI_CONFIG_DATA_BASE_1,globalBusIF);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
  sprintf(txt,"===> 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\r\n",
PCI_BUS_ALIAS,PCI_CONFIG_ADDR_BASE_0,PCI_CONFIG_DATA_BASE_0,
PCI_CONFIG_ADDR_BASE_1,PCI_CONFIG_DATA_BASE_1,globalBusIF);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
}
*/
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
            configAddrReg = PCI_CONFIG_ADDR_BASE_0;
            configDataReg = PCI_CONFIG_DATA_BASE_0;
            busNo = bus;
            }
        else if ((UINT32)bus > PCI_BUS_ALIAS)
            {
            configAddrReg = PCI_CONFIG_ADDR_BASE_1;
            configDataReg = PCI_CONFIG_DATA_BASE_1;
            busNo = (bus - PCI_BUS_ALIAS - 1);
            }
        }
    else
        {
        if (globalBusIF == 0)
            {
            configAddrReg = PCI_CONFIG_ADDR_BASE_0;
            configDataReg = PCI_CONFIG_DATA_BASE_0;
            }
        else if (globalBusIF == 1)
            {
            configAddrReg = PCI_CONFIG_ADDR_BASE_1;
            configDataReg = PCI_CONFIG_DATA_BASE_1;
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
    UINT32 data		/* Message data */
    )
    {
    int deviceNo        = 0x0000001f;
    int funcNo          = 0x00000007;
    int configAddrReg = 0;
    int configDataReg = 0;
    int busNo = 0;

    if ((UINT32)bus <= PCI_BUS_ALIAS)
        {
        configAddrReg = PCI_CONFIG_ADDR_BASE_0;
        configDataReg = PCI_CONFIG_DATA_BASE_0;
        busNo = bus;
        }
    else if ((UINT32)bus > PCI_BUS_ALIAS)
        {
        configAddrReg = PCI_CONFIG_ADDR_BASE_1;
        configDataReg = PCI_CONFIG_DATA_BASE_1;
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
* the PCI0 interface.  This routine is only useful before sysPciAutoConfig()
* has run.  It makes no sense to call this routine after sysPciAutoConfig()
* has been called to initialize the PCI interface.
*
* RETURNS: OK, or ERROR if size not supported.
*/
STATUS sysPciConfigWriteAlt
    (
    int bus,		/* PCI bus number */
    int device,		/* PCI device number */
    int func,		/* PCI function number */
    int reg,		/* PCI register offset */
    int size,		/* Size of transfer (1,2, or 4 bytes) */
    UINT32 data		/* Data to write */
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

void
sysHwInit (void)
{
  UINT i, timeOut;
  UINT32 reg1;
  UINT32 reg2;
#if ((defined INCLUDE_PCX7307_SIO) || (defined INCLUDE_Z85230_SIO))
  int         pciBusNo;       /* PCI bus number */
  int         pciDevNo;       /* PCI device number */
  int         pciFuncNo;      /* PCI function number */
#endif /* INCLUDE_PCX7307_SIO, INCLUDE_Z85230_SIO */
  char txt[100];

    /*
     * If mmu tables are used, this is where we would dynamically
     * update the entry describing main memory, using sysPhysMemTop().
     * We must call sysPhysMemTop () at sysHwInit() time to do
     * the memory autosizing if available.
     */

    sysPhysMemTop ();

    /* Validate CPU type */

    sysCpuCheck();

    /* set pointer to BAT initialization functions */

    _pSysBatInitFunc = (FUNCPTR) mmuPpcBatInitMPC74x5;

    /* Setup Altivec Exception Addresses */

    vmxExcLoad ();

    /* set pointer to bus probing hook */

    _func_vxMemProbeHook = (FUNCPTR)sysBusProbe;

    /* Initialize the VPD information, VPD located on MV64260's I2C bus */

    (void) sysMotVpdInit();

    /* Initialize those MV64260 registers that require static initialization */

    sysMv64260Init ();

    /* Initialize the MV64260 device bus support */

    sysMv64260DevBusInit ();

    /* Initialize the MV64260 host bridge support */

    sysMv64260PhbInit ();

    /* Calculate CPU Bus Rate */

    cpuBusSpeed = sysCpuBusSpd();

    /* Setup the dynamic "device bus" entry sizes in sysPhysMemDesc[] */

    for(i = 0; i < sizeof(sysPhysMemDesc); i++)
	{
	  switch ((UINT32)sysPhysMemDesc[i].virtualAddr)
	  {
    	    case CPUIF_CS1_LO_DECODE_ADDR:

		reg1 = MV64260_READ32(MV64260_REG_BASE,
				      CPUIF_CS1_LO_DECODE_ADDR);
		sysPhysMemDesc[i].virtualAddr =
				     (char *)MV64260_LO_DECODE_ADDR_INV(reg1);
		sysPhysMemDesc[i].physicalAddr = sysPhysMemDesc[i].virtualAddr;

		reg2 = MV64260_READ32(MV64260_REG_BASE,
				      CPUIF_CS1_HI_DECODE_ADDR);
                sysPhysMemDesc[i].len = MV64260_CPU_ADDR_WNDW_SZ (reg1, reg2);

		break;

    	    case CPUIF_CS0_LO_DECODE_ADDR:

		reg1 = MV64260_READ32(MV64260_REG_BASE,
				      CPUIF_CS0_LO_DECODE_ADDR);
		sysPhysMemDesc[i].virtualAddr =
				     (char *)MV64260_LO_DECODE_ADDR_INV(reg1);
		sysPhysMemDesc[i].physicalAddr = sysPhysMemDesc[i].virtualAddr;

		reg2 = MV64260_READ32(MV64260_REG_BASE,
				      CPUIF_CS0_HI_DECODE_ADDR);
                sysPhysMemDesc[i].len = MV64260_CPU_ADDR_WNDW_SZ (reg1, reg2);

		break;

    	    case CPUIF_BOOTCS_LO_DECODE_ADDR:

		reg1 = MV64260_READ32(MV64260_REG_BASE,
				      CPUIF_BOOTCS_LO_DECODE_ADDR);
		sysPhysMemDesc[i].virtualAddr =
				     (char *)MV64260_LO_DECODE_ADDR_INV(reg1);
		sysPhysMemDesc[i].physicalAddr = sysPhysMemDesc[i].virtualAddr;

		reg2 = MV64260_READ32(MV64260_REG_BASE,
				      CPUIF_BOOTCS_HI_DECODE_ADDR);
                sysPhysMemDesc[i].len = MV64260_CPU_ADDR_WNDW_SZ (reg1, reg2);

		break;

	    default:
		;
	  }
	}


    /*
     * Disable the failsafe timer.
     * This must be called before sysSerialHwInit().
     */

    m48t37FailsafeCancel();

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





	/* Sergey: patch recommended by MCG */
    /* THAT PART DOES NOT WORK FOR mv5500 !!! */

    /*
     * PrPMC Enumeration
     *
     * Poll EREADY1 and EREADY0 in the Presence Detect Register
     * to wait for any installed PrPMCs to signal that they are
     * ready for enumeration.
     */
	/*
    for(timeOut = 10; timeOut > 0; timeOut--)
    {
      if( (*(UCHAR *)(PRESENCE_DETECT_REG) & PRESENCE_DETECT_EREADY1_MASK) &&
          (*(UCHAR *)(PRESENCE_DETECT_REG) & PRESENCE_DETECT_EREADY0_MASK) )
      {
        break;
      }
      sysUsDelay(1000000);
    }
*/



    /* perform PCI configuration */

    if ( !PCI_AUTOCONFIG_DONE )
    {

        int myPciMaxBus;

	/* just wait for a while so prpmc880 will complete its initialization */
sprintf(txt,"sysHwInit: wait 40 sec for prpmc880 initialization \r\n ");
sysDebugMsg(txt,CONTINUE_EXECUTION);
    sysUsDelay(40000000);
	/*
sprintf(txt,"sysHwInit: continue \r\n ");
sysDebugMsg(txt,CONTINUE_EXECUTION);
	*/
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

        myPciMaxBus = sysPciMaxBus(MV64260_MAX_BUS);

        sysPciAutoConfig (MV64260_MAX_BUS, sysPhbPciSpace);

        pciMaxBus = myPciMaxBus;

        /* Remember that PCI is configured */

        PCI_AUTOCONFIG_FLAG = TRUE;

	/* 
	 * We have configured all of the PCI devices except for the
	 * host bridge, which was excluded from pciAutoConfig().  We
	 * must now program the internal register set memory mapped BAR 
	 * on the GT-64260 to allow access to the internal register
	 * set from the PCI bus.  Although no agent on the PCI bus
	 * really needs access to this register space, the programming
	 * of the BAR is necessary in order for inbound mappings from
	 * PCI bus to DRAM to work.  This requirement appears to be
	 * a "feature" of the GT-64260.
	 */

        /* pciConfigOutLong(0,0,0,0x20,0x82800000) */

        pciConfigOutLong (PCI_BUSNO_MV64260, PCI_DEVNO_MV64260, PCI_FUNC0,
		          PCIIF_FUNC0_INT_REG_MEM_MAPPED_BASE_ADDR_PCICFG,
		          (PCI0_MSTR_MEM_LOCAL | PCI_BASE_PREFETCH));

	/*
	 * The Universe PCI-VME bridge chip is behind a transparent
	 * PCI-PCI bridge chip.  The PCI-PCI bridge chip has been
	 * configured to cover all of the space requrested by devices
	 * subordinate to this bridge.  Unfortunately, this does not
	 * cover all of the space that we need.  In particular, the
	 * PCI space which accesses VME is not included in the address
	 * range covered by the PCI-PCI bridge since the Universe chip
	 * only requests enough PCI space to cover its own UCSR register
	 * set.  All of the PCI space which allows for access to VME
	 * has not been requested in a PCI-centric way and we need to
	 * open up this PCI space here.  This is done by simply moving
	 * the bottom of the memory base register on the transparent
	 * PCI-PCI bridge chip to a higher address which will cover
	 * the PCI to VME local address space.
	 */

	 bridgeToUniverseProg();

	/*
         * Get the base address for the Universe registers.  Save
	 * this address in the global variable 'univBaseAdrs'.  This
	 * global variable will be referenced for all future accesses
	 * to the CSR space of the Universe chip.
         */

	univBaseAdrs = getUnivBaseAdrs();

	/*
	 * Perform 1st phase of Universe initialization if we found the chip.
         * This initialization consists of settup up the the non-PCI Config
	 * Space registers of the Universe.
         */

	if (univBaseAdrs != 0)
	    sysUniverseInit();
	}

    univBaseAdrs = getUnivBaseAdrs();

    /*
     * There are three possible cases:
     *
     * 1) We're in the bootrom and the following call to sysUniverseCapt()
     *    is superfluous since sysProcNumSet() will be called a bit later and
     *    we will be calling sysUniverseCapt() at that time to capture the
     *    "final" values for the Universe windows.
     *
     * 2) We're in the kernel and INCLUDE_NET_INIT is #defined, which means
     *    that the bootrom has already run and initialized the Universe.  We
     *    will capture the Universe window values here but again, this is
     *    superfluous since sysProcNumSet() will be called in just a little
     *    while and we will again capture the the Universe window values -
     *    no harm done.
     *
     * 3) Here's the reason we call sysUniverseCapt() here.  If we are
     *    running in the kernel and INCLUDE_NET_INIT is not #define'd the
     *    sysProcNumSet() will never be called and the sysUniverseCapt()
     *    which resides in sysProcNumSet() will, of course, not get called.
     *    Thus the call to sysUniverseCapt() which resides below will be
     *    the last such call and we will capture the window values which
     *    the bootroom left in the chip.
     */

    sysUniverseCapt();

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



/*Sergey*/
#ifdef  INCLUDE_VME_DMA
    /*  Initialize the VMEbus DMA driver */
    sysVmeDmaInit();
#endif  /* INCLUDE_VME_DMA */



    /*
     * Initialize COM Ports  - note this must be done AFTER device
     * bus initialization.
     */

    sysSerialHwInit();

    /* Upon completion, clear BFL (Board Fail) LED */

    *(UCHAR *)(SYSTEM_STATUS_REG_2) &= ~SYSTEM_STATUS_REG_2_BRDFL_MASK;
}


/*******************************************************************************
* 
* bridgeToUniverseProg - program transparent PCI-PCI bridge.
*
* The function performs programming of the PCI-PCI transparent bridge beyond
* that done by pciAutoConfig().  The following additional programming is done:
*
* The window for nonprefetchable memory is expanded to cover the addresses
* up to VME_MEM_LOCAL_END.  Since the Universe PCI-VME bridge chip is behind
* the transparent PCI-PCI bridge this nonprefetchable memory expansion allows
* access through the transparent bridge to the Universe bridge and ultimately
* out onto the VME bus.
*
* The "master abort" bit of the "bridge control register" is set to allow
* master aborts and target aborts to be registered with the GT-64260B
* host bridge.  This aids sysBusProbe() when PCI or VME addresses are being
* probed.
*
* Bits are set in the transparent PCI-PCI bridge to disable the 
* "flush-before-reads" capability for both the primary and secondary sides 
* of the bridge.  This allows reads to be resolved before flushing all the 
* writes in the bridge's write-post buffers.  This programming is required
* to prevent certain lockup conditions between Universe, PCI-PCI bridge
* and GT-64260B.
*
* RETURNS: NA
*/

LOCAL void bridgeToUniverseProg(void)
    {
    UINT16 reg16;
    UINT32 reg32;

    /*
     * Bump the nonprefetch memory limit register of the PCI-PCI transparent
     * bridge to cover all of the space up to VME_MEM_LOCAL_END.
     */

    pciConfigOutWord(PCI_BUSNO_PCI2PCI, PCI_DEVNO_PCI2PCI, 0, PCI_CFG_MEM_LIMIT,
                     (((VME_MEM_LOCAL_END) >> 16) - 1));

    /*
     * Set the master abort bit control register of the PCI-PCI transparent
     * bridge to aid sysBusProbe().  This will allow us to detect master
     * and target aborts on the PCI busses via status register in the
     * GT64260B.
     */

    pciConfigInWord(PCI_BUSNO_PCI2PCI, PCI_DEVNO_PCI2PCI, 0, 
		     PCI_CFG_BRIDGE_CONTROL, &reg16);

    reg16 |= PCI_CFG_BRIDGE_CONTROL_MABORT;

    pciConfigOutWord(PCI_BUSNO_PCI2PCI, PCI_DEVNO_PCI2PCI, 0, 
		     PCI_CFG_BRIDGE_CONTROL, reg16);

    /*
     * In the "Arbiter Control Register" (offset 0x42) of the 
     * transparent PCI-PCI bridge set the following bits:
     *
     *   12 - disable "flush before read" on the primary side
     *   13 - disable "flush before read" on the secondary side
     */

    pciConfigInLong(PCI_BUSNO_PCI2PCI, PCI_DEVNO_PCI2PCI, 0, 0x42, &reg32);
    reg32 |= (1<<12 | 1<<13);
    pciConfigOutLong(PCI_BUSNO_PCI2PCI, PCI_DEVNO_PCI2PCI, 0, 0x42, reg32);
    }

/*******************************************************************************
*
* getUnivBaseAdrs() - return base address of Universe CSR registers.
*
* This routine searches for the Universe VME-PCI bridge chip and if it
* is found, returns the base address of the CSR register set.  Note that
* this function must wait to be called until the Universe has been
* configured via pciAutoConfig().  If the Universe chip cannot be found,
* then zero is returned.
*
* RETURNS: Universe base address or zero if Universe chip cannot be found.
*/

LOCAL UINT32 getUnivBaseAdrs(void)
    {
    int    pciBusNo;
    int    pciDevNo;
    int    pciFuncNo;
    UINT32 reg1;
    char * baseAddr = 0;

    if (pciFindDevice ((PCI_ID_UNIVERSE & 0xFFFF),
	               (PCI_ID_UNIVERSE >> 16) & 0xFFFF, 0,
		       &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR)
        {
        pciConfigInLong(pciBusNo, pciDevNo, pciFuncNo,
                        PCI_CFG_BASE_ADDRESS_0, &reg1);

        /*
         * We have the "bus address" of the Universe CSR space,
         * now convert it to a local address.
         */

        (void)sysBusToLocalAdrs (PCI_SPACE_MEM_PRI, (char *)reg1,
		                 &baseAddr);

        }

    return((UINT32)baseAddr);
    }

/*******************************************************************************
*
* sysDramSize - returns the real top of local DRAM.
*
* This routine reads the Base Address Enable register of the MV64260 to
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
    UINT32 base = MV64260_REG_BASE;	/* base of MV64260 register set */
    UINT bank;				/* bank index counter */
    UINT32 loDecodeAddr = 0x0;
    UINT32 hiDecodeAddr = 0x0;

    static UINT32 loDecodeAddrOffset[] =
	{
	CPUIF_SCS0_LO_DECODE_ADDR,	/* Chip Select 0 Low Decode Address  */
	CPUIF_SCS1_LO_DECODE_ADDR,	/* Chip Select 1 Low Decode Address  */
	CPUIF_SCS2_LO_DECODE_ADDR,	/* Chip Select 2 Low Decode Address  */
	CPUIF_SCS3_LO_DECODE_ADDR	/* Chip Select 3 Low Decode Address  */
	};

    static UINT32 hiDecodeAddrOffset[] =
	{
	CPUIF_SCS0_HI_DECODE_ADDR,	/* Chip Select 0 High Decode Address */
	CPUIF_SCS1_HI_DECODE_ADDR,	/* Chip Select 1 High Decode Address */
	CPUIF_SCS2_HI_DECODE_ADDR,	/* Chip Select 2 High Decode Address */
	CPUIF_SCS3_HI_DECODE_ADDR	/* Chip Select 3 High Decode Address */
	};

    for (bank = 0; bank < NELEMENTS(loDecodeAddrOffset); bank++)
	{
	loDecodeAddr = MV64260_READ32(base, loDecodeAddrOffset[bank]);
	if (loDecodeAddr != 0x00000FFF)
	     {
	     hiDecodeAddr = MV64260_READ32(base, hiDecodeAddrOffset[bank]);
	     hiDecodeAddr = MV64260_HI_DECODE_ADDR_INV(hiDecodeAddr);
	     if (hiDecodeAddr > localDram)
		 localDram = hiDecodeAddr;
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
        sysPhysMemSize = (char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);
#endif /* LOCAL_MEM_AUTOSIZE */

        /* Adjust initial DRAM size to actual physical memory. */

        for (i = 0; i < sizeof(sysPhysMemDesc); i++)
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
    return (sysPhysMemSize);
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
    return ((char *)(sysPhysMemTop () - USER_RESERVED_MEM));
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

#if (defined INCLUDE_CACHE_L3)

        /* disable the L3 cache */

        sysL3CacheFlushDisable();

#endif /* INCLUDE_CACHE_L3 */

#if (defined INCLUDE_CACHE_L2)

        /* disable the L2 cache */

        sysL2CacheDisable();

#endif /* INCLUDE_CACHE_L2 */

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

void
sysHwInit2 (void)
{
  static BOOL configured = FALSE;
  char txt[100];
  /*
  sprintf(txt,"sysHwInit2 reached\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
  */
  if(!configured)
  {
	/*
  sprintf(txt,"sysHwInit2 configuring\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
	*/
	/* Initialize the interrupt handling */

	sysMv64260IntInit();

	/*
	 * Enable the GPP interrupts, all interrupting devices (including
	 * serial ports) which are not an integral part	of the GT-64260	will
	 * be using the	GPP interrupts.	 Note that on this board
	 * GPP_INT_LVL_24_31 is	not used so we will not	enable it.
	 */

	intEnable (GPP_INT_LVL_0_7);
	intEnable (GPP_INT_LVL_8_15);
	intEnable (GPP_INT_LVL_16_23);

	/* Connect Universe (PCI-VME bridge) interrupts */

	intConnect (INUM_TO_IVEC(UNIV_INT_LVL), sysUnivVmeIntr, 0);

        /* Enable Universe interrupts at the GPP level */

	intEnable (UNIV_INT_LVL);

	/* initialize serial interrupts	*/

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

#if (defined INCLUDE_CACHE_L2)

        /* initialize and enable L2 cache */

        sysL2CacheInit();

#endif /* INCLUDE_CACHE_L2 */

#if (defined INCLUDE_CACHE_L3)

        /* initialize and enable the L3 cache */

        sysL3CacheInit();

#endif /* INCLUDE_CACHE_L3 */

#ifdef INCLUDE_NETWORK
#ifdef INCLUDE_END
#ifdef INCLUDE_GEI_END

	sys543PciInit();

#endif /* INCLUDE_GEI_END */
#endif /* INCLUDE_END */
#endif /* INCLUDE_NETWORK */

#ifdef INCLUDE_AUX_CLK

	sysAuxClkInit();

#endif /* INCLUDE_AUX_CLK */
	/*
sprintf(txt,"call sysDmaInit() ???\r\n ");
sysDebugMsg(txt,CONTINUE_EXECUTION);
	*/
#if (defined INCLUDE_MV64260_DMA)

       /* initialize the DMA driver */
	/*
sprintf(txt,"call sysDmaInit() !!!\r\n ");
sysDebugMsg(txt,CONTINUE_EXECUTION);
	*/
       sysDmaInit ();

#endif /* INCLUDE_MV64260_DMA */

#if (( defined INCLUDE_RTC ) || ( defined INCLUDE_FAILSAFE ))

        /* initialize the RTC device */

        m48t37Init();

#endif /* INCLUDE_RTC, INCLUDE_FAILSAFE */

#if (( defined INCLUDE_RTC ) && ( defined INCLUDE_DOSFS ))

        dosFsDateTimeInstall((FUNCPTR)sysRtcDateTimeHook);

#endif /* INCLUDE_RTC, INCLUDE_DOSFS */

        configured = TRUE;
	}
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

    /*
     * Init global variable - this needs to be done before calling
     * sysUniverseInit2() because sysUniverseInit2() calls sysProcNumGet()
     * via the macro definition.
     */

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
     * order to correctly configure the universe VSIx slave decoders
     * (see commentary below), we must call the 2nd phase of universe
     * initialization code here.  If we don't have INCLUDE_NET_INIT
     * #define'd, this 2nd phase of Universe initialization is only
     * done once (during bootrom initialization).  If INCLUDE_NET_INIT
     * is #defined, the 2nd phase of initialization will be done twice,
     * once during bootrom initialization and once during kernel
     * initialization.
     */

    if (procNumSet == FALSE)
	{

        /*
         * Set up the Uinverse's VME slave decoders.  We have to wait until
         * sysProcNumSet() is called since the initialization of the
         * inbound VSI1, VSI4 and VSI5 windows is dependent on whether
         * or not we are operating as node zero or not.
         */

	if (univBaseAdrs != 0)
	    {
            sysUniverseInit2(procNum);
	    sysUniverseCapt();
	    }
	}

    procNumSet = TRUE;
    }

/* miscellaneous support routines */

/******************************************************************************
*
* sysPhbCapt - capture translation window information
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

LOCAL void
sysPhbCapt (void)
{
  UINT32 index;       /* window counter   */
  UINT32 size;	/* working variable */
  UINT32 base;	/* working variable */
  UINT32 pciCpuEnab;  /* enabling bits pci -> cpu */
  UINT32 remap;	/* hold for remapping register */
  UINT32 temp;
  UINT32 reg1;
  UINT32 reg2;
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
  phbToPciWinCount = 0;	 /* number of valid Phb to PCI windows */
  pciToPhbWinCount = 0;	 /* number of valid PCI to PHB windows */

  /* point to window save arrays */

  pPhbCpu = &sysPhbCpuToPciWin[0];
  pPhbPci = &sysPhbPciToCpuWin[0];

  /* start with the cpu to pci windows (ppc slaves) */

  pPhbOffO = &sysPhbCpuWinOff[0];

  /* loop through each window */

  for(index = 0; index < PHB_CPU_WIN_CNT; index++)
  {
	reg1 = MV64260_READ32 (MV64260_REG_BASE, pPhbOffO->base);
	reg2 = MV64260_READ32 (MV64260_REG_BASE, pPhbOffO->limit);
	/*
logMsg("MV64260_REG_BASE=0x%08x, pPhbOffO->base=0x%08x, reg1=0x%08x\n",
MV64260_REG_BASE, pPhbOffO->base, reg1,0,0,0);
logMsg("MV64260_REG_BASE=0x%08x, pPhbOffO->limit=0x%08x, reg2=0x%08x\n",
MV64260_REG_BASE, pPhbOffO->limit, reg2,0,0,0);
	*/
	/* If the window is disabled, skip it ... */

	if(((reg1 == MV64260_LO_DECODE_DISABLE) &&
	    (reg2 == MV64260_HI_DECODE_DISABLE)) == FALSE)
    {
      /* active window found, bump valid window counter */
	  /*
logMsg("found active window\n",0,0,0,0,0,0);
	  */
      sysValidPhbWindows++;

      /* determine the window type (memory or i/o) */

	  if ( (pPhbOffO->base == CPUIF_PCI0_IO_LO_DECODE_ADDR) ||
		   (pPhbOffO->base == CPUIF_PCI1_IO_LO_DECODE_ADDR) )
	  {
	    pPhbCpu->winType = PCI_BAR_SPACE_IO;
	  }
      else
	  {
	    pPhbCpu->winType = PCI_BAR_SPACE_MEM;
	  }

      pPhbPci->winType = pPhbCpu->winType;

      /* Plug the window base and limit addresses */

	  reg1 = MV64260_READ32(MV64260_REG_BASE, pPhbOffO->base);
	  pPhbCpu->winBase = MV64260_LO_DECODE_ADDR_INV(reg1);

      reg2 = MV64260_READ32(MV64260_REG_BASE, pPhbOffO->limit);
      pPhbCpu->winLimit = MV64260_HI_DECODE_ADDR_INV(reg2) - 1;

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
         * value of 0x00000018.  To compute the PCI window base
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
         * 3.  Shift the remap register left 20 bits to align
         *     the remap value to the actual address. Note that
	     *     this can be done with the MV64260_LO_DECODE_ADDR_INV
	     *     macro as well:
         *
         *           remap << 20
         *           0x00000018 << 20 = 0x01800000
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
            remap = MV64260_READ32 (MV64260_REG_BASE, pPhbOffO->remap);
	    remap = MV64260_LO_DECODE_ADDR_INV(remap);
	    remap &= ~size;

	    pPhbPci->winBase = remap | temp;
	    pPhbPci->winLimit = pPhbPci->winBase + size;

            /* advance in preparation for next valid window */

            pPhbCpu++;
            pPhbPci++;
            }

        /* advance to next set of Phb offsets */

        pPhbOffO++;
	phbToPciWinCount++;
        }

    /* switch to Phb PCI to CPU windows (PCI slaves) */

    pPhbOffI = &sysPhbPciWinOff[0];

    /* loop through each window */

    for (index = 0; index < PHB_PCI_WIN_CNT; index++)
        {

        pciCpuEnab = MV64260_READ32 (MV64260_REG_BASE, pPhbOffI->enableReg);

	/* If the window is enabled ... */

	if ((pciCpuEnab & (1 << pPhbOffI->enableBit)) == 0)
            {

            /* active window found, bump valid window counter */

            sysValidPhbWindows++;

            /* set the window type to memory or I/O based on MEM bit setting */

	    (void)sysPciConfigRead (0, 0, pPhbOffI->function,
				    pPhbOffI->base, 4, &base);

	    pPhbPci->winType = base & 1;
	    pPhbCpu->winType = pPhbPci->winType;

            /* Plug the window base and limit addresses */

	    pPhbPci->winBase = base & PCI_MEMBASE_MASK;
	    size = MV64260_READ32 (MV64260_REG_BASE, (pPhbOffI->size));
	    size = MV64260_PCI_BAR_SZ_INV(size);
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
	    remap = MV64260_READ32 (MV64260_REG_BASE, (pPhbOffI->remap));
	    remap &= ~size;
	    pPhbCpu->winBase = remap | temp;
	    pPhbCpu->winLimit = pPhbCpu->winBase + size;

            /* advance in preparation for next valid window */

            pPhbCpu++;
            pPhbPci++;
            }

        /* advance to next set of Phb offsets */

        pPhbOffI++;
	pciToPhbWinCount++;
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
* sysUniverseCapt - Capture Universe address mapping information.
*
* This routine captures the outbound (LSIx) and inbound (VSIx) address
* translation and control information from the unvierse chip.
*
* RETURNS: N/A
*
* SEE ALSO: sysBusToLocalAdrs(), sysLocalToBusAdrs(), sysCpuToPciAdrs(),
* sysPciToCpuAdrs(), and sysPhbTransAdrs().
*/

LOCAL void sysUniverseCapt (void)
    {
    UNIV_VSI_CAPT(0,UNIVERSE_VSI0_BS,UNIVERSE_VSI0_BD, \
		    UNIVERSE_VSI0_TO,UNIVERSE_VSI0_CTL)
    UNIV_VSI_CAPT(1,UNIVERSE_VSI1_BS,UNIVERSE_VSI1_BD, \
		    UNIVERSE_VSI1_TO,UNIVERSE_VSI1_CTL)
    UNIV_VSI_CAPT(2,UNIVERSE_VSI2_BS,UNIVERSE_VSI2_BD, \
		    UNIVERSE_VSI2_TO,UNIVERSE_VSI2_CTL)
    UNIV_VSI_CAPT(3,UNIVERSE_VSI3_BS,UNIVERSE_VSI3_BD, \
		    UNIVERSE_VSI3_TO,UNIVERSE_VSI3_CTL)
    UNIV_VSI_CAPT(4,UNIVERSE_VSI4_BS,UNIVERSE_VSI4_BD, \
		    UNIVERSE_VSI4_TO,UNIVERSE_VSI4_CTL)
    UNIV_VSI_CAPT(5,UNIVERSE_VSI5_BS,UNIVERSE_VSI5_BD, \
		    UNIVERSE_VSI5_TO,UNIVERSE_VSI5_CTL)
    UNIV_VSI_CAPT(6,UNIVERSE_VSI6_BS,UNIVERSE_VSI6_BD, \
		    UNIVERSE_VSI6_TO,UNIVERSE_VSI6_CTL)
    UNIV_VSI_CAPT(7,UNIVERSE_VSI7_BS,UNIVERSE_VSI7_BD, \
		    UNIVERSE_VSI7_TO,UNIVERSE_VSI7_CTL)

    UNIV_LSI_CAPT(0,UNIVERSE_LSI0_BS,UNIVERSE_LSI0_BD, \
		    UNIVERSE_LSI0_TO,UNIVERSE_LSI0_CTL)
    UNIV_LSI_CAPT(1,UNIVERSE_LSI1_BS,UNIVERSE_LSI1_BD, \
		    UNIVERSE_LSI1_TO,UNIVERSE_LSI1_CTL)
    UNIV_LSI_CAPT(2,UNIVERSE_LSI2_BS,UNIVERSE_LSI2_BD, \
		    UNIVERSE_LSI2_TO,UNIVERSE_LSI2_CTL)
    UNIV_LSI_CAPT(3,UNIVERSE_LSI3_BS,UNIVERSE_LSI3_BD, \
		    UNIVERSE_LSI3_TO,UNIVERSE_LSI3_CTL)
    UNIV_LSI_CAPT(4,UNIVERSE_LSI4_BS,UNIVERSE_LSI4_BD, \
		    UNIVERSE_LSI4_TO,UNIVERSE_LSI4_CTL)
    UNIV_LSI_CAPT(5,UNIVERSE_LSI5_BS,UNIVERSE_LSI5_BD, \
		    UNIVERSE_LSI5_TO,UNIVERSE_LSI5_CTL)
    UNIV_LSI_CAPT(6,UNIVERSE_LSI6_BS,UNIVERSE_LSI6_BD, \
		    UNIVERSE_LSI6_TO,UNIVERSE_LSI6_CTL)
    UNIV_LSI_CAPT(7,UNIVERSE_LSI7_BS,UNIVERSE_LSI7_BD, \
		    UNIVERSE_LSI7_TO,UNIVERSE_LSI7_CTL)

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

    int  i;
    char * pciBusAdrs;
    UINT32  vsiCtlAxx = 0;

    if (IS_VME_ADDR_MOD(adrsSpace))
        {
	if (sysLocalToBusAdrs(PCI_SPACE_MEM_PRI, localAdrs,
                              &pciBusAdrs) == ERROR)
	    return (ERROR);
	else
	    {
            switch (adrsSpace)
	        {
                case VME_AM_EXT_SUP_PGM:
                case VME_AM_EXT_SUP_DATA:
                case VME_AM_EXT_USR_PGM:
                case VME_AM_EXT_USR_DATA:

	            /* Now check the VSI (inbound) windows. */

		    vsiCtlAxx = VSI_CTL_VAS_A32;
		    break;

        	case VME_AM_STD_SUP_PGM:
        	case VME_AM_STD_SUP_DATA:
        	case VME_AM_STD_USR_PGM:
        	case VME_AM_STD_USR_DATA:
			vsiCtlAxx = VSI_CTL_VAS_A24;
		        break;

        	case VME_AM_SUP_SHORT_IO:
        	case VME_AM_USR_SHORT_IO:
			vsiCtlAxx = VSI_CTL_VAS_A16;
		        break;
	        }

	    for (i = 0; i <= NUM_VSI_WIN; i++)
		{
		if (((vmeVsiWin[i].ctl & VSI_CTL_EN) != 0) &&
		    ((vmeVsiWin[i].ctl & DCTL_VAS_MSK) == vsiCtlAxx))
		    {
		    if (((vmeVsiWin[i].bs + vmeVsiWin[i].to) <=
			(UINT32)pciBusAdrs) &&
			 ((vmeVsiWin[i].bd + vmeVsiWin[i].to - 1) >=
			(UINT32)pciBusAdrs))
			{
			*pBusAdrs = (char *)((UINT32)pciBusAdrs -
					      vmeVsiWin[i].to);
			return (OK);
			}
		    }
		}
	    return (ERROR);
	    }
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
    char *  busAdrs,    /* bus address to convert */
    char ** pLocalAdrs    /* where to return local address */
    )
    {
    int i;
    char * pciBusAdrs;

    if (IS_VME_ADDR_MOD(adrsSpace))
	{
	  /* This is a VME Address so now check for A24 or A16 
             and mask the address accordingly   DJA 9/2003 */
	if((adrsSpace==VME_AM_STD_USR_DATA)||(adrsSpace==VME_AM_STD_USR_PGM)||
            (adrsSpace==VME_AM_STD_SUP_DATA)||(adrsSpace==VME_AM_STD_SUP_PGM))
	  (UINT32)busAdrs |= 0xff000000;
	if((adrsSpace==VME_AM_SUP_SHORT_IO)||(adrsSpace==VME_AM_USR_SHORT_IO))
	  (UINT32)busAdrs |= 0xffff0000;

	for (i = 0; i <= NUM_LSI_WIN; i++)
	    {
	    if ((vmeLsiWin[i].ctl & LSI_CTL_EN) != 0)
		{
		if  (((vmeLsiWin[i].bs + vmeLsiWin[i].to) <=
		       (UINT32)busAdrs)	&&
		     ((vmeLsiWin[i].bd + vmeLsiWin[i].to - 1) >=
		       (UINT32)busAdrs))
		    {
		    pciBusAdrs = (char *)((UINT32)busAdrs - vmeLsiWin[i].to);

		    switch (adrsSpace)
			{
			case VME_AM_EXT_SUP_PGM:
			case VME_AM_EXT_SUP_DATA:
			case VME_AM_EXT_USR_PGM:
			case VME_AM_EXT_USR_DATA:
			    if ((vmeLsiWin[i].ctl & DCTL_VAS_MSK) ==
				 LSI_CTL_A32)
				break;
                            else
				continue;

			case VME_AM_STD_SUP_PGM:
			case VME_AM_STD_SUP_DATA:
			case VME_AM_STD_USR_PGM:
			case VME_AM_STD_USR_DATA:
			    if ((vmeLsiWin[i].ctl & DCTL_VAS_MSK) ==
				 LSI_CTL_A24)
				break;
                            else
				continue;

			case VME_AM_SUP_SHORT_IO:
			case VME_AM_USR_SHORT_IO:
			    if ((vmeLsiWin[i].ctl & DCTL_VAS_MSK) ==
				 LSI_CTL_A16)
				break;
                            else
				continue;
			}

		    return (sysBusToLocalAdrs(PCI_SPACE_MEM_PRI,
					      pciBusAdrs, pLocalAdrs));
		    }
		}
	    }
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
* sysBusTas - test and set a specified location
*
* This routine performs a test-and-set (TAS) instruction on the specified
* address.  To prevent deadlocks, interrupts are disabled during the
* test-and-set operation.  
*/

BOOL sysBusTas
    (
    char * adrs          /* address to be tested and set */
    )
    {
    BOOL state;
#ifdef ANY_BRDS_IN_CHASSIS_NOT_RMW
    state = sysVmeVownTas(adrs);
#else
    state = sysVmeRmwTas(adrs); 
#endif /* ANY_BRDS_IN_CHASSIS_NOT_RMW */
    return (state);
    }

/******************************************************************************
*
* sysBusTasClear - clear a location set by sysBusTas()
*
* This routine clears the specified 32-bit location typically set
* by sysBusTas(). 
*
* RETURNS: N/A
*
* SEE ALSO: sysBusTas()
*/

void sysBusTasClear
    (
    volatile char * adrs    /* Address of semaphore to be cleared */
    )
    {
    *(UINT *)adrs = 0;
    EIEIO_SYNC;
    }

#ifdef ANY_BRDS_IN_CHASSIS_NOT_RMW
/*******************************************************************************
*
* sysVmeVownTas - test and set a location across the VMEbus
*
* This routine performs a test-and-set (TAS) instruction on the specified
* address.  To prevent deadlocks, interrupts are disabled and the VMEbus is
* locked during the test-and-set operation.
*
* NOTE: Although the address passed-in to sysBusTas() is defined as
* "char *", vxTas() operates on the address as a "void *".
* For PowerPC, this implies that the location tested-and-set is
* actually a 32-bit entity.
* 
* RETURNS: TRUE if the value had not been set but is now
* FALSE if the VMEbus cannot be locked or the value was already set.
*
* SEE ALSO: vxTas(), sysVmeVownTasClear()
*/

LOCAL BOOL sysVmeVownTas
    (
    char * adrs          /* address to be tested and set */
    )
    {
    BOOL state = FALSE;  /* semaphore state */
    int  lockKey;        /* interrupt lock key */
    UINT32 ticksToWait;  /* number of time base ticks to wait */
    UINT32 startTick;    /* starting timebase low tick value */
    UINT32 lockValue;
    volatile UINT32 * lockAdrs = (UINT32 *)adrs;

    /*
     * convert LOCK_TIMEOUT to timebase ticks as follows:
     *
     * ticks/Usec = (bus frequency / decrementer pre-scaler) / 1000000
     *
     * ticks to wait = requested delay (in Usecs) * ticks/USec. 
     */

    ticksToWait = LOCK_TIMEOUT * 
                  ((cpuBusSpeed/DECREMENTER_PRESCALE)/USECS_PER_SECOND);


    /* lock interrupts so there will be no TAS interference */

    lockKey = intLock ();

    /* Request ownership of the VMEbus */

    *UNIVERSE_MAST_CTL |= LONGSWAP(MAST_CTL_VOWN);
    EIEIO_SYNC;

    /* snapshot starting time base low value */

    startTick = sysTimeBaseLGet ();

    /* Wait for the VME controller to give us the BUS or timeout */

    while (!VMEBUS_OWNER && (sysTimeBaseLGet () - startTick) < ticksToWait);

    /* perform the TAS */

    if (VMEBUS_OWNER)
        {
        lockValue =  ((0x80 + sysProcNumGet ())<< 24);

        if (*(UINT32 *)lockAdrs == 0)
            {

	    /* No one else has the lock, grab it */

            while (*(UINT32 *)lockAdrs != lockValue)
                *(UINT32 *)lockAdrs = lockValue;
            state = TRUE;
            }

        }

    /* release the VME BUS */
    
    *UNIVERSE_MAST_CTL &= LONGSWAP(~MAST_CTL_VOWN);

    /* unlock the interrupt */

    intUnlock (lockKey);

    /* return TAS test result */

    return (state);

    }

#else /* ANY_BRDS_IN_CHASSIS_NOT_RMW */

/*******************************************************************************
*
* sysVmeRmwTas - test and set a location across the VMEbus utilizing RMW
*
* This routine performs a test-and-set (TAS) instruction on the specified
* address.  To prevent deadlocks, interrupts are disabled and the VMEbus is
* locked during the test-and-set operation.
*
* NOTE: Although the address passed-in to sysBusTas() is defined as
*    "char *", vxTas() operates on the address as a "void *".
*    For PowerPC, this implies that the location tested-and-set is
*    actually a 32-bit entity.
*
* RETURNS: TRUE if the value had not been set but is now
*          FALSE if the VMEbus cannot be locked or the value was already set.
*
* SEE ALSO: vxTas()
*/

LOCAL BOOL sysVmeRmwTas
    (
    char * adrs          /* address to be tested and set */
    )
    {
    BOOL state = FALSE;  /* semaphore state */
    int  lockKey;        /* interrupt lock key */

    /* A board with the UNIVERSE_II can generate a VMEbus RMW */

    /*
     * Lock interrupts so that setting up SCG and issuing RMW
     * are atomic
     */

    lockKey = intLock ();

    /* Enable RMW cycle */

    sysBusRmwEnable(VME_SCG_COMPARE_MASK,
                    VME_SCG_COMPARE_TO_SET,
                    VME_SCG_SWAP_TO_SET,
                    (char *)adrs);

    /* perform RMW to try and set TAS location */

    state = *((UINT *)adrs);
    EIEIO_SYNC;

    /* Disable RMW cycle */

    sysBusRmwDisable();

    /* unlock the interrupt */

    intUnlock (lockKey);

    /* return TAS test result */

    return (state ? FALSE : TRUE);
    }
#endif /*  ! ANY_BRDS_IN_CHASSIS_NOT_RMW */

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
* sysProbeErrClr - clear errors associated with probing an address on a bus.
*
* This routine clears the error flags and conditions in the DAR, DSISR, SRR0
* and SRR1 PowerPC registers arising from probing addresses.
*
* RETURNS: N/A
*/

LOCAL void sysProbeErrClr (void)
    {
    UINT32 pciCsr;

    /* Deal with universe registers */

    pciCsr = sysPciInLong ((UINT32)(UNIVERSE_PCI_CSR));

    /* Clear PCI_CSR */

    sysPciOutLong ((UINT32)(UNIVERSE_PCI_CSR), pciCsr);

    /* Clear any VME address error */

    sysPciOutLong ((UINT32)UNIVERSE_V_AMERR, V_AMERR_V_STAT);

    /* Force write due to Write-Posting and get updated status */

    pciCsr = sysPciInLong((UINT32)(UNIVERSE_PCI_CSR));

    /* Clear relevant host bridge error registers */

    MV64260_WRITE32_PUSH(MV64260_REG_BASE, PCIIF_ERR_CAUSE_PCI0, 0);
    MV64260_WRITE32_PUSH(MV64260_REG_BASE, PCIIF_ERR_CAUSE_PCI1, 0);

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
    
    reg1 = MV64260_READ32(MV64260_REG_BASE, PCIIF_ERR_CAUSE_PCI0);
    reg2 = MV64260_READ32(MV64260_REG_BASE, PCIIF_ERR_CAUSE_PCI1);

    if (((reg1 | reg2) & ((PCIIF_ERR_CAUSE_PCIx_MMABORT_MASK) |
                          (PCIIF_ERR_CAUSE_PCIx_MTABORT_MASK))) == 0)
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
* RETURNS: OK or ERROR if address probed has no responder.
*/

LOCAL STATUS sysPciProbe
    (
    char   * adrs,
    int      mode,    /* VX_READ or VX_WRITE */
    int      length,  /* 1, 2 or 4 byte probe */
    char   * pVal     /* address of value to write OR */
    )
    {
    STATUS stat;
    int    oldLevel;
    char   reg8;

    /* probe performed with interrupts disabled */

    oldLevel = intLock ();

    if ((stat = vxMemArchProbe (adrs, mode, length, pVal)) == OK)
        {
	if (mode == VX_WRITE)
	    reg8 = * adrs;		/* This should flush the write */
        stat = checkHbStat();
        }

    /* safe to restore original interrupt state */

    intUnlock (oldLevel);

    return (stat);
    }

/******************************************************************************
*
* sysVmeProbe - probe a VME bus address
*
* This routine probes an address on the VME bus.  The PCI bridge must have a 
* special setup to enable generation of Master Abort cycles on write probes 
* and reception of Target Abort cycles on read probes.  if the probe is a 
* write, the Universe must be configured to disable Posted Writes.  All 
* probing is done with interrupts disabled.
*
* RETURNS: OK or ERROR if address cannot be probed
*/

LOCAL STATUS sysVmeProbe
    (
    char   * adrs,     /* address to be probed */
    int      mode,     /* VX_READ or VX_WRITE */
    int      length,   /* 1, 2 or 4 byte probe */
    char   * pVal,     /* address of value to write OR */
                       /* address of location to place value read */
    char   * lsiCtlReg /* address of LSI ctl register for appropriate window */
    )
    {
    STATUS stat = ERROR;
    UINT32 pciSlv1Ctl;   /* Universe PCI Slave Image Control reg */
    int    oldLevel;

    /* probe performed with interrupts disabled */

    oldLevel = intLock ();

    pciSlv1Ctl = sysPciInLong ((UINT32)lsiCtlReg);

    /* Make sure  posted writes in the Universe are disabled */

    if (mode == VX_WRITE)
        {
        sysPciOutLong ((UINT32)lsiCtlReg, (pciSlv1Ctl & ~LSI_CTL_WP));
        }

    /* Probe as a PCI probe since all VME is PCI on this board */

    stat = sysPciProbe (adrs, mode, length, pVal);

    /* Restore Posted Writes by Universe if previously enabled */

    if ((mode == VX_WRITE) && (pciSlv1Ctl & LSI_CTL_WP))
        {
        sysPciOutLong ((UINT32)lsiCtlReg, pciSlv1Ctl);
	}

    /* safe to restore original interrupt state */

    intUnlock (oldLevel);

    return (stat);

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
* through a Universe II LSIx (outbound) window.
*
* RETURNS: TRUE if address is mapped to VME interface, FALSE if not.
* If return value is TRUE, the VME bus translated address and the address
* of the associated LSI control register is returned.
*/

BOOL isVmeAdrs 
    (
    char  * adrs,	/* Local address to examine to see if it's VME */
    char  ** vmeTrans,	/* If it's VME, this will be the VME bus address */
    char  ** lsiCtlReg  /* if it's VME, this will be the LSI control reg */
    )
    {
    int i;
    BOOL stat = FALSE;
    char * pciTrans;

    /*
     * First determine if it's a PCI address since on this board,
     * any VME address must first be a PCI address.  If it is not
     * a PCI address we can return FALSE right away.
     */

    if (!isPciAdrs(adrs, &pciTrans))
	return FALSE;

    /*
     * It is a PCI address, now lets take the PCI translated address
     * and see if it hits inside any of the Universe LSI windows which
     * would make it a VME address.
     */

    for (i = 0; i < NUM_LSI_WIN; i++)
	{

        /* If window is enabled ... */

        if ((vmeLsiWin[i].ctl & LSI_CTL_EN) != 0)
            {

            /* If address hits inside of this window */

            if  ((vmeLsiWin[i].bs <= (UINT32)pciTrans) &&
                 (vmeLsiWin[i].bd  > (UINT32)pciTrans))

                {
		stat = TRUE;
		*(UINT32 *)vmeTrans = (UINT32)pciTrans + vmeLsiWin[i].to;

		switch (i)
		    {
		    case 0: 
			*lsiCtlReg = (char *)UNIVERSE_LSI0_CTL;
			break;
		    case 1: 
			*lsiCtlReg = (char *)UNIVERSE_LSI1_CTL;
			break;
		    case 2: 
			*lsiCtlReg = (char *)UNIVERSE_LSI2_CTL;
			break;
		    case 3: 
			*lsiCtlReg = (char *)UNIVERSE_LSI3_CTL;
			break;
		    case 4: 
			*lsiCtlReg = (char *)UNIVERSE_LSI4_CTL;
			break;
		    case 5: 
			*lsiCtlReg = (char *)UNIVERSE_LSI5_CTL;
			break;
		    case 6: 
			*lsiCtlReg = (char *)UNIVERSE_LSI6_CTL;
			break;
		    case 7: 
			*lsiCtlReg = (char *)UNIVERSE_LSI7_CTL;
			break;
		    }
                break;
                }
	    }
        }
    return (stat);
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
* sysVmeProbe() routine is called to do the special VME probing.
*
* RETURNS: ERROR if the probed address does not respond or causes a MMU fault.
* Returns OK if the probed address responds.
*/

STATUS  sysBusProbe
    (
    char   * adrs,    /* address to be probed */
    int      mode,    /* VX_READ or VX_WRITE */
    int      length,  /* 1, 2 or 4 byte probe */
    char   * pVal     /* address of value to write OR */
                      /* address of location to place value read */
    )
    {
    STATUS stat;
    char   * lsiCtlReg;
    char   * pciTrans;
    char   * vmeTrans;

    /* Clear any existing errors/exceptions */

    sysProbeErrClr ();

    /* If the address is a DRAM address ... */

    if (((adrs == LOCAL_MEM_LOCAL_ADRS) ||
	 (adrs >  LOCAL_MEM_LOCAL_ADRS)) &&
	 (adrs < sysPhysMemTop()))
        stat = vxMemArchProbe (adrs, mode, length, pVal);

    /* ... else if the address is a VME address */

    else if (isVmeAdrs(adrs, &vmeTrans, &lsiCtlReg))
        stat = sysVmeProbe(adrs, mode, length, pVal, lsiCtlReg);

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

void sysDebugMsg
    (
    char * str,
    UINT32  recovery
    )
    {
    int msgSize;
    int msgIx;
    SIO_CHAN * pSioChan;        /* serial I/O channel */

    msgSize = strlen (str);

    sysSerialHwInit ();

    pSioChan = sysSerialChanGet (0);

    sioIoctl (pSioChan, SIO_MODE_SET, (void *) SIO_MODE_POLL);

    for (msgIx = 0; msgIx < msgSize; msgIx++)
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

/*****************************************************************************
*
* vmxExcLoad - Loads exception code for Altivec Exceptions.
*
* This function copies the exception code located at address 0x100 and copies
* it to offsets 0xf20 and 0x1600 for Altivec Unavailable and Altivec Assist,
* respectively.
*
* The purpose of this is to make sure that vxWorks continues to run in case
* an exception occurs on either of these new sources.  This will be removed
* when vxWorks arch/ppc support is upgraded to fully support the Altivec
* exceptions.
*
* RETURNS: N/A
*/

void vmxExcLoad (void)
    {
    bcopy ((char*) (LOCAL_MEM_LOCAL_ADRS + 0x0100),
           (char*) (LOCAL_MEM_LOCAL_ADRS + _EXC_VMX_UNAVAIL), SIZEOF_EXCEPTION);
    bcopy ((char*) (LOCAL_MEM_LOCAL_ADRS + 0x0100),
           (char*) (LOCAL_MEM_LOCAL_ADRS + _EXC_VMX_ASSIST), SIZEOF_EXCEPTION);
    }

