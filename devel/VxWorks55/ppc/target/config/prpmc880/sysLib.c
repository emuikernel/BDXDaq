/* sysLib.c - Motorola PRPMC880 board series system-dependent library */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,02Jun03,simon  updated based on peer review results
01a,02apr03,simon  Ported. from ver 01d, mcpm905/sysLib.c.
*/

/*
DESCRIPTION
This library provides board-specific routines.  The chip drivers included are:

    ppcDecTimer.c   - PowerPC decrementer timer library (system clock)
    byteNvRam.c     - byte-oriented generic non-volatile RAM library
    pciConfigLib.c  - PCI configuration library

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
#include "mv64360.h"
#include "mv64360Smc.h"
#include "mv64360Dma.h"

#ifdef INCLUDE_DEC2155X
#   include "dec2155xCpci.h"
#endif

#ifdef INCLUDE_ALTIVEC
#include "altivecLib.h"
IMPORT int (* _func_altivecProbeRtn) (void) ;
#endif /* INCLUDE_ALTIVEC */

/* defines */

#ifdef INCLUDE_DPM    /* dynamic power management */

#   define _PPC_HID0_BIT_DPM    11 /* dynamic power management bit */
#   define _PPC_HID0_DPM        (1<<(31-_PPC_HID0_BIT_DPM))

#endif /* INCLUDE_DPM */

/* defines for sysBusTas() and sysBusTasClear() */

#define DEFAULT_TAS_CHECKS    10        /* rechecks for soft tas */
#define TAS_CONST             0x80

/* structures */

typedef struct mailboxInfo
    {
    BOOL    connected;
    FUNCPTR routine;
    int     arg;
    } MAILBOX_INFO;

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
        
        ISA_MSTR_IO_LOCAL,	
        ISA_MSTR_IO_SIZE,
        ISA_MSTR_IO_BUS,
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
	PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_LOW_PCICFG, 0,
	0,
	0,
	PCIIF_BASE_ADDR_ENABLE_PCI0, 
	PCIIF_BASE_ADDR_ENABLE_INTMEM_BIT
	}
	
    };

#define PHB_CPU_WIN_CNT \
        (sizeof (sysPhbCpuWinOff)/ sizeof (PHB_OFFSETS_OUTBOUND))
#define PHB_PCI_WIN_CNT \
        (sizeof (sysPhbPciWinOff)/ sizeof (PHB_OFFSETS_INBOUND))
#define PHB_WIN_CNT (PHB_CPU_WIN_CNT + PHB_PCI_WIN_CNT)

/* globals */

IMPORT int pciMaxBus;
static char * sysPhysMemSize = NULL;    /* ptr to top of mem + 1 */
UINT32 cpuBusSpeed = 0;

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
 *
 * The following applies whenever the MV64360 ethernet ports are enabled
 * in config.h by defining INCLUDE_GEISC_END:
 *
 * Because of a MV64260/MV64360 errata ("Res #COMM-2 Communication ports
 * access * to SDRAM coherent regions," in the document "Marvell Errata and
 * Restrictions GT-64260A-B-0, Doc. No. MV-S500071-00 Rev. D") regarding
 * non-functional SDMA snoop, all system controller ethernet descriptors
 * and data buffers must be in non-cacheable memory. An additional
 * sysPhysMemDesc descriptor provides the space in low memory above the
 * vector table. Based on current #defines, each port receives four 64K
 * pages based on 128 four word descriptors per port (64 receive and
 * 64 transmit) with 64 1500 byte receive buffers. This reduces the
 * typical vector table partition size by 512 KBytes (256 Kbytes per port).
 * The number of descriptors and memory placement is not adjustable
 * via the init string in configNet.h.
 */

PHYS_MEM_DESC sysPhysMemDesc [] =
    {

#ifdef INCLUDE_GEISC_END

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

    /* System controller ethernet descriptors and buffers (non-cacheable) */

    (void *) ETHER_MEM_START_ADDR,
    (void *) ETHER_MEM_START_ADDR,
    ETHER_TOTAL_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_MEM_COHERENCY
    },

#else
 
    {

    /* Vector Table and Interrupt Stack */

    (void *) LOCAL_MEM_LOCAL_ADRS,
    (void *) LOCAL_MEM_LOCAL_ADRS,
    RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

#endif /* INCLUDE_GEISC_END */

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
    /*  allow adjustment in sysHwInit() */
    (void *) (LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE),
    (void *) (LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE),
    USER_RESERVED_MEM,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },
    




    {
    /* Access to PCI bus non-prefetchable memory space */

    (void *) PCI0_MSTR_MEMIO_LOCAL,
    (void *) PCI0_MSTR_MEMIO_LOCAL,
    PCI0_MSTR_MEMIO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* Access to PCI bus prefetchable memory space */

    (void *) PCI0_MSTR_MEM_LOCAL,
    (void *) PCI0_MSTR_MEM_LOCAL,
    PCI0_MSTR_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT
    },




/* Sergey: access to HOST's memory, which must be mapped at PCI address 0 */
    /*
     * Access to PCI memory space at PCI address zero.
     * This will allow the PrPMC to access DRAM on the processor board
     * on which the PrPMC is mounted if that processor board maps its
     * DRAM to PCI address zero.
     * Note: Guarding should not be required since
     * memory is well-behaved (no side-effects on read or write)
     */

    {
    (void *) (PCI0_MSTR_ZERO_LOCAL),
    (void *) (PCI0_MSTR_ZERO_LOCAL),
    PCI0_MSTR_ZERO_SIZE,
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

    /* Access to PCI 32-bit I/O space for PCI bus  */

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

    /* BankA Flash (64M) */

    (void *) BANKA_FLASH_BASE_ADRS,	
    (void *) BANKA_FLASH_BASE_ADRS,	
    BANKA_SIZE,					 
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* BankB Flash (1M) */

    (void *) BANKB_FLASH_BASE_ADRS,	
    (void *) BANKB_FLASH_BASE_ADRS,	
    BANKB_SIZE,					
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {

    /* System register */

    (void *) SYSTEM_REG_BASE,	
    (void *) SYSTEM_REG_BASE,	
    SYSTEM_REG_SIZE,					
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

#ifdef INCLUDE_SM_COMMON
    MAILBOX_INFO sysMailbox = {FALSE, NULL, 0};
#endif

UINT32 sysProbeFault = 0;       /* used by dsi exception trap handler */
BOOL   sysMonarchMode = FALSE;  /* TRUE if Monarch */

#ifdef INCLUDE_DEC2155X
UINT PCI0_MEM0_BAR_BASE = 0; /* used by DEC2155x driver */
UINT PCI0_IREG_BAR_BASE = 0;  /* used by DEC2155x driver */
STATUS sysPciConfig21554InLong (int, int, int, int, UINT32 *);
void   prpmcDec2155xWaitConfig (void);
#endif

/* unsigned int gtInternalRegBaseAddr = MV64360_REG_BASE; */
/* locals */

#ifdef  INCLUDE_SM_COMMON
#   if (SYS_SM_BUS_NUMBER == SYS_BACKPLANE_BUS_NUMBER) 
#       define SM_PCI_CONFIG_IN_LONG sysPciConfig21554InLong
#   else
#       define SM_PCI_CONFIG_IN_LONG pciConfigInLong
#   endif
#endif

LOCAL char sysModelStr[80];
LOCAL char sysWrongCpuMsg[] = WRONG_CPU_MSG;

#ifdef SYS_SM_ANCHOR_POLL_LIST

static SYS_SM_ANCHOR_POLLING_LIST sysSmAnchorPollList[] =
    {
    SYS_SM_ANCHOR_POLL_LIST
    { 0xffffffff, 0xffffffff, 0xffffffff }  /* marks end of list */
    };
#endif

LOCAL int    sysSmUtilTasValue = 0;  /* special soft tas value */

/* forward declarations */

void    sysCpuCheck (void);
char *  sysPhysMemTop (void);
UCHAR   sysNvRead (ULONG);
void    sysNvWrite (ULONG,UCHAR);
STATUS  sysBusProbe (char *, int, int, char *);
void    sysUsDelay (UINT);
void    reportBootromErrors (void);
void    sysDelay (void);
LOCAL   void sysPhbCapt (void);
void    sysPciInsertLong (UINT32, UINT32, UINT32);
void    sysPciInsertWord (UINT32, UINT16, UINT16);
void    sysPciInsertByte (UINT32, UINT8, UINT8);
void	sysDebugMsg (char *, UINT32);
int	sysAltivecProbe (void);
void	sysBusTasClear (volatile char *);
STATUS	sysPciWriteFlush (void);
void	sysProbeErrClr (void);
STATUS  sysPciConfigRead (int, int, int, int, int, void *);
STATUS  sysPciConfigWrite (int, int, int, int, int, UINT32);
STATUS  sysPciConfigSpcl (int, UINT32);
STATUS  prpmcSlaveWaitConfig(int bus, int device, int whichBar);
BOOL    sysSysconAsserted (void);
void    sysPciOutWordConfirm (UINT32, UINT16);

#ifdef INCLUDE_DPM
    void  sysConfigDpm (void);
#endif /* INCLUDE_DPM */

#ifdef INCLUDE_SM_COMMON

    int sysSmIntTypeCompute (void);
    int	sysSmArg1Compute (int intType);
    int	sysSmArg2Compute (int intType);
    int	sysSmArg3Compute (void);


#   if (SM_OFF_BOARD == TRUE)
#    ifdef SYS_SM_ANCHOR_POLL_LIST
        LOCAL UINT     sysSmAnchorCandidate ( UINT, UINT, UINT);
#    endif
    LOCAL STATUS sysSmAnchorFind ( int, char **);
    LOCAL char   *sysSmAnchorPoll (void);
    char *    sysSmAnchorAdrs ();    /* Anchor address (dynamic) */
#   endif
#endif

/* externals */

IMPORT void   sysMv64360DevBusInit (void);
IMPORT void   sysMv64360PhbInit (void);
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
IMPORT UCHAR  sysCpuStr[];
IMPORT STATUS sysMotI2cRead (UINT32 addr, UINT32 offset,
                             UINT32 addrBytes, UCHAR *spdData, UINT32 numBytes);
IMPORT STATUS sysMotI2cWrite (UINT32 addr, UINT32 offset,
                              UINT32 addrBytes, UCHAR *spdData,
                              UINT32 numBytes);
IMPORT UINT32 sysCpuBusSpd (void);

/* BSP DRIVERS */

#include "pci/pciConfigLib.c"		/* PCI Support */
#include "pci/pciIntLib.c"		/* PCI Support */
#include "sysBusPci.c"			/* PCI Auto-Configuration Support */
#include "sysMotVpd.c"                  /* VPD Support */
#include "sysMv64360Init.c"		/* MV64360 register init handler */
#include "sysMv64360Int.c"		/* MV64360 main interrupt handler */
#include "sysMv64360Phb.c"		/* MV64360 host bridge support */
#include "sysMv64360DevBus.c"		/* MV64360 device bus support */

#if (defined INCLUDE_CACHE_L2)
#   include "sysL2Cache.c"              /* L2 Cache Support */
#endif /* INCLUDE_CACHE_L2 */

#if (defined INCLUDE_MV64360_DMA)
#   include "sysDma.c"			/* Top-level DMA user routines */
#   include "sysMv64360Dma.c"		/* MV64360 DMA Support */
#endif /* INCLUDE_MV64360_DMA */

#if (defined INCLUDE_RTC)
#   include "PLDI2CDrv.c"   /* low-level I2C driver */
#   include "max6900.c"      /* max6900 user routines */
#endif
#include "sysSerial.c"			/* Serial Support */
#include "mem/byteNvRam.c"		/* NVRAM Support */
#include "timer/ppcDecTimer.c"		/* PPC603 & 604 have on chip timers */
#include "mv64360UartDrv.c"

#ifdef INCLUDE_DEC2155X
#   include "dec2155xCpci.c"
#endif

#ifdef INCLUDE_NETWORK
#   ifdef INCLUDE_END
#       ifdef INCLUDE_GEISC_END
#           include "./sysMv64360End.c"   /* MV64360 ethernet */
#       endif /* INCLUDE_GEISC_END */
#   endif /* INCLUDE_END */
#endif /* INCLUDE_NETWORK */

#ifdef INCLUDE_SHOW_ROUTINES
#   include "sysMotVpdShow.c"		/* VPD Show Routines */
#   include "sysMv64360SmcShow.c"	/* SMC & SPD Show Routines */
#   include "pci/pciConfigShow.c"	/* PCI Show Routines */
#endif /* INCLUDE_SHOW_ROUTINES */

#ifdef INCLUDE_AUXCLK
#   include "sysMv64360AuxClk.c"	/* MV64360 AuxClock support */
#endif /* INCLUDE_AUXCLK */

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




/* Sergey: sysPciConfigReadProbe() will probe function 7 on bus 0 device 0
and return 1 if function does not respond; we have to swap devices in that
case (TEMPORARY UNTIL P2P-based method will be implemented); we are using
following DISCO II feature here: DISCO II respond to 8 PCI functions being
accessed from CPU but to 5 functions being accessed from PCI */
STATUS
sysPciConfigReadProbe()
{
  int pResult;
  UINT32 *retvalLongword = 0;
  int configAddrReg = 0;
  int configDataReg = 0;
  int bus = 0;
  int dev = 0;
  int reg = 0;
  int func = 7;

  configAddrReg = PCI_CONFIG_ADDR_BASE_0;
  configDataReg = PCI_CONFIG_DATA_BASE_0;

  PCI_OUT_LONG(configAddrReg, pciConfigBdfPack(bus, dev, func) |
                 (reg & 0xFC) | 0x80000000);

  *retvalLongword = PCI_IN_LONG(configDataReg);
  pResult = *retvalLongword;

  /*printf("pResult=0x%08x\n",pResult);*/
  if(pResult==0xffffffff) return(1); /* need device swap */
  else return(0);                    /* do not need device swap */
}

#define WORKAROUND_MV6100 \
  int mydevice=device; \
  if(sysPciConfigReadProbe()) \
  { \
    if(device==0)      mydevice=4; \
    else if(device==4) mydevice=0; \
  }



/******************************************************************************
*
* sysPciConfigRead - read from the PCI configuration space
*
* This routine reads from the PCI configuration space.  Reads may be done
* in sizes of one byte, one word, or one longword.  
*
* Returns: OK, or ERROR if size not supported.
*/

STATUS
sysPciConfigRead (
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

WORKAROUND_MV6100;

    configAddrReg = PCI_CONFIG_ADDR_BASE_0;
    configDataReg = PCI_CONFIG_DATA_BASE_0;
    busNo = bus;

    PCI_OUT_LONG(configAddrReg, pciConfigBdfPack(busNo, mydevice, func) |
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
* in sizes of one byte, one word, or one longword.  
*
*/

STATUS
sysPciConfigWrite (
    int bus,
    int device,
    int func,
    int reg,
    int size,
    UINT32 data
    )
    {
    UINT8 * dataByte = 0;
    UINT16 * dataWord = 0;
    UINT32 * dataLongword = 0;
    int configAddrReg = 0;
    int configDataReg = 0;
    int busNo = 0;

WORKAROUND_MV6100;

    configAddrReg = PCI_CONFIG_ADDR_BASE_0;
    configDataReg = PCI_CONFIG_DATA_BASE_0;
    busNo = bus;

    PCI_OUT_LONG(configAddrReg, pciConfigBdfPack(busNo, mydevice, func) |
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
* This routine generates a special cycle with a message.  
*
* RETURNS: OK
*/

STATUS sysPciConfigSpcl
    (
    int bus,
    UINT32 data
    )
    {
    int deviceNo        = 0x0000001f;
    int funcNo          = 0x00000007;
    int configAddrReg = 0;
    int configDataReg = 0;
    int busNo = 0;

    configAddrReg = PCI_CONFIG_ADDR_BASE_0;
    configDataReg = PCI_CONFIG_DATA_BASE_0;
    busNo = bus;

    PCI_OUT_LONG (configAddrReg, pciConfigBdfPack (busNo, deviceNo, funcNo) |
                  0x80000000);
    PCI_OUT_LONG (configDataReg, data);
    return (OK);
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
sysHwInit(void)
{
  UINT   barReg;
  UINT32 temp1, ret, ii, aa[30];
  char txt[80];

/*gorit*/

  /* Determine operating mode */
  sysMonarchMode = sysSysconAsserted ();

/*gorit*/

#ifdef MYDEBUG
  sprintf(txt,"sysHwInit: sysMonarchMode=%d\r\n ",sysMonarchMode);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  /*
   * If mmu tables are used, this is where we would dynamically
   * update the entry describing main memory, using sysPhysMemTop().
   * We must call sysPhysMemTop () at sysHwInit() time to do
   * the memory autosizing if available.
   */

  sysPhysMemTop ();

  /* Validate CPU type */

  sysCpuCheck();

  /* set pointer to bus probing hook */

  _func_vxMemProbeHook = (FUNCPTR)sysBusProbe;
 
  /* set pointer to BAT initialization functions */

  _pSysBatInitFunc = (FUNCPTR) mmuPpcBatInitMPC74x5;

  /* Initialize the VPD information, VPD located on MV64360's I2C bus */

/*gorit*/

  (void) sysMotVpdInit();   

/*gorit*/

  /* Initialize the MV64360 device bus support */

  sysMv64360DevBusInit();  /* Sergey: if commented out, goto VxWorks prompt ! */
	 
/*ne gorit*/

  /* Initialize those MV64360 registers that require static initialization */

  sysMv64360Init();
/*ne gorit*/

  /* Initialize the MV64360 host bridge support */
  sysMv64360PhbInit();

  /* Calculate the CPU Bus Rate */
  cpuBusSpeed = sysCpuBusSpd(); 
 
  /* Initialize PCI driver library */
  if(pciConfigLibInit (PCI_MECHANISM_0, (ULONG)sysPciConfigRead,
        (ULONG)sysPciConfigWrite, (ULONG)sysPciConfigSpcl) != OK)
  {
#ifdef MYDEBUG
    sprintf(txt,"sysHwInit: call sysToMonitor 1 \r\n ");
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
    sysToMonitor (BOOT_NO_AUTOBOOT);
  }

  if((pciIntLibInit ()) != OK)
  {
#ifdef MYDEBUG
    sprintf(txt,"sysHwInit: call sysToMonitor 2 \r\n ");
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
    sysToMonitor (BOOT_NO_AUTOBOOT);
  }



  /* Sergey: patch recommended by MCG */

#ifdef MYDEBUG
  sprintf(txt,"sysHwInit: before: 0x%08x = 0x%02x\r\n ",
    (UCHAR *)(SYSTEM_STATUS_REG_3),*(UCHAR *)(SYSTEM_STATUS_REG_3));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  /* Ready for Enumeration by host board */
  *(UCHAR *)(SYSTEM_STATUS_REG_3) |= SYSTEM_STATUS_REG_3_EREADYO_BIT;

#ifdef MYDEBUG
  sprintf(txt,"sysHwInit: Ready for Enumeration: 0x%08x = 0x%02x\r\n ",
    (UCHAR *)(SYSTEM_STATUS_REG_3),*(UCHAR *)(SYSTEM_STATUS_REG_3));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif




  /************************************/
  /* HARRIER STUFF -  just to compare */

  /* Sergey: must issue EREADY and wait here ??!! EREADY stuff ?! */

  /* Clear PCI configuration cycles hold off bit. */

/*
  *(UINT32 *)HARRIER_BRIDGE_PCI_CONTROLSTATUS_REG &= 
               ~HARRIER_BPCS_XCSR_CSH;
*/

  /* 
   * Set the PCI Bus Enumeration Ready bit and wait for the PCI Bus
   * Enumeration status bit to flip before proceeding.
   */
/*
  *(UINT32 *) (SYSTEM_STATUS_REG_3) |= SYSTEM_STATUS_REG_3_EREADYO_BIT;

  EIEIO_SYNC;

  sprintf(txt,"sysHwInit: wait for Enumeration Ready bit flip\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);

  while((*(volatile UINT32 *) (SYSTEM_STATUS_REG_3)) & 
			        (SYSTEM_STATUS_REG_3_EREADYI_BIT == 0))
  {
	;
  }

  sprintf(txt,"sysHwInit: it fliped !\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
*/

  /************************************/
  /************************************/






/* with mv5500:
[0x00] ===> ret=0, bars=0x84000000 0x00000000 0x00000000 0x00000000 0x88000000 0x0000000b
[0x01] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x02] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x03] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x04] ===> ret=0, bars=0x00000000 0x00000000 0xff010100 0x02a01111 0xf0f08810 0x82818291
[0x05] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x06] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x07] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x08] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x09] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x0a] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x0b] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x0c] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x0d] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x0e] ===> ret=0, bars=0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff 0xffffffff
[0x0f] ===> ret=0, bars=0x00000008 0x10000008 0x00000000 0x00000000 0x82800008 0x00000000
*/







    
#ifdef INCLUDE_DEC2155X 
  sysPciConfigRead (0, 0, 0, PCIIF_CS0_BASE_ADDR_LOW_PCICFG, 4, &PCI0_MEM0_BAR_BASE);
  PCI0_MEM0_BAR_BASE &= PCI_MEMBASE_MASK;
  sysPciConfigRead (0, 0, 0, PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_LOW_PCICFG, 4, &PCI0_IREG_BAR_BASE);	
  PCI0_IREG_BAR_BASE &= PCI_MEMBASE_MASK;

  /* Configure the Dec 2155x setup registers if we are the PCI Host */
  if(sysMonarchMode) sysDec2155xInit();
#endif

  /* perform PCI configuration */
  if(sysMonarchMode)
  {
#ifdef MYDEBUG
    sprintf(txt,"sysHwInit: monarch mode\r\n ");
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

	if( !PCI_AUTOCONFIG_DONE )
    {
      int myPciMaxBus;

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

	  myPciMaxBus = sysPciMaxBus(PRPMC880_MAX_BUS);

	  sysPciAutoConfig (PRPMC880_MAX_BUS, sysPhbPciSpace); 

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
	}
  }
  else
  {
    /* Wait for Host to enumerate my discovery */

#ifdef MYDEBUG
    sprintf(txt,"sysHwInit: slave mode\r\n ");
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

	/*if( !PCI_AUTOCONFIG_DONE )*/ /* Sergey: we do not want to wait second time ? */
    {

#ifdef MYDEBUG
      sprintf(txt,"sysHwInit: slave mode: Wait for Host to enumerate my discovery 0\r\n ");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
      prpmcSlaveWaitConfig(0, 0x0, PCI_CFG_BASE_ADDRESS_0);
#ifdef MYDEBUG
      sprintf(txt,"sysHwInit: done: \r\n ");
      sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

	  /*PCI_AUTOCONFIG_FLAG = TRUE;*/
	}


	        
	/* read the contents of the slave's PCI bar register */
	sysPciConfigRead(0, 0, 0, PCI_CFG_BASE_ADDRESS_0, 4, &barReg);

	temp1 = MV64360_READ32 (MV64360_REG_BASE, PCIIF_ACCESS_CTRL_BASE0_LOW_PCI0);
	MV64360_WRITE32_PUSH (MV64360_REG_BASE, PCIIF_ACCESS_CTRL_BASE0_LOW_PCI0,
				  ((barReg & 0xfffff000) | (temp1 & 0xfff)));
  }

  /* capture Phb window configuration */
  sysPhbCapt ();

#ifdef INCLUDE_DEC2155X 

    /* 
     * Wait for the primary side Dec2155x configuring agent to
     * enable bus mastering.  By the time bus mastering is
     * enabled the primary Dec2155x configuring agent will
     * have filled in the primary side BARs which is necessary
     * to be done before we call sysDec2155xCapt().
     * The call to sysDec2155xInit() will fill
     * the translation tables which are ultimately used by
     * sysBusToLocalAdrs() and sysLocalToBusAdrs().
     */

    prpmcDec2155xWaitConfig();

   /* Capture the Dec 2155x address translation info. */

    sysDec2155xCapt ();

#endif

  /* 
   * Initialize COM Ports  - note this must be done AFTER device
   * bus initialization.
   */

  sysSerialHwInit();


  /* Sergey */
  for(ii=0; ii<0x10; ii++)
  {
    ret = pciConfigInLong(0,ii,0,0x10,&aa[0]);
    ret |= pciConfigInLong(0,ii,0,0x14,&aa[1]);
    ret |= pciConfigInLong(0,ii,0,0x18,&aa[2]);
    ret |= pciConfigInLong(0,ii,0,0x1c,&aa[3]);
    ret |= pciConfigInLong(0,ii,0,0x20,&aa[4]);
    ret |= pciConfigInLong(0,ii,0,0x24,&aa[5]);

#ifdef MYDEBUG
    sprintf(txt,"[0x%02x] ===> ret=%d, bars=0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\r\n ",
        ii,ret,aa[0],aa[1],aa[2],aa[3],aa[4],aa[5]);
    sysDebugMsg(txt,CONTINUE_EXECUTION);  
#endif
	/*
    sprintf(txt,"                           0x%08x 0x%08x 0x%08x 0x%08x\r\n ",
        aa[4],aa[5],aa[6],aa[7]);
    sysDebugMsg(txt,CONTINUE_EXECUTION);  
	*/
  }

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
        sysPhysMemSize = (char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);
#endif /* LOCAL_MEM_AUTOSIZE */

        /* Adjust initial DRAM size to actual physical memory. */

        for (i = 0; i < sizeof(sysPhysMemDesc); i++)
	    {
	    if (sysPhysMemDesc[i].virtualAddr == (void *)RAM_LOW_ADRS)
	        {
                sysPhysMemDesc[i].len = 
                    (ULONG)sysPhysMemSize - 
                    (ULONG)sysPhysMemDesc[i].physicalAddr - USER_RESERVED_MEM;
                
                sysPhysMemDesc[i + 1].len = USER_RESERVED_MEM;
                sysPhysMemDesc[i + 1].virtualAddr = (void*)(sysPhysMemSize - USER_RESERVED_MEM);
                sysPhysMemDesc[i + 1].physicalAddr = (void*)(sysPhysMemSize - USER_RESERVED_MEM);
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

STATUS
sysToMonitor (
  int startType    /* parameter passed to ROM to tell it how to boot */
)
{
    FUNCPTR pRom = (FUNCPTR) (ROM_WARM_ADRS); /* Warm reboot */

#if (defined INCLUDE_CACHE_L2)

        /* disable the L2 cache */

        sysL2CacheDisable();

#endif /* INCLUDE_CACHE_L2 */

    cacheDisable (INSTRUCTION_CACHE);	/* Disable the L1 Instruction Cache */
    cacheDisable (DATA_CACHE);		/* Disable the L1 Data Cache */

    sysSerialReset (); 		/* Reset serial devices */

    vxMsrSet (0);		/* Clear the Machine State Register */

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

  *(unsigned int *)0x10000000 = 1;

    if (!configured)
        {

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

        /* initialize serial interrupts */

        sysSerialHwInit2(); 

#ifdef INCLUDE_DEC2155X 

        /*
         * capture the base address of the 2155x assigned during PCI
         * auto-configuration.
         */

        sysDec2155xAdrsGet ();

        /* if we are the PCI Host */

        if (sysMonarchMode)
            {

            /* Initialize Dec2155x interrupts. */

            sysDec2155xInit2 ();
            }

#endif

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

#ifdef INCLUDE_AUXCLK

	sysAuxClkInit();

#endif /* INCLUDE_AUXCLK */

#if (defined INCLUDE_MV64360_DMA)

       /* initialize the DMA driver */
  *(unsigned int *)0x10000004 = 1;
       sysMv64360DmaInit ();
  *(unsigned int *)0x10000008 = 1;

#endif /* INCLUDE_MV64360_DMA */

#ifdef INCLUDE_NETWORK
#   ifdef INCLUDE_END
#      ifdef INCLUDE_GEISC_END

       /* Initialize the MV64360 ethernet hardware */

       sysMv64360EndHwInit ();

#      endif /* INCLUDE_GEISC_END */
#   endif /* INCLUDE_END */
#endif /* INCLUDE_NETWORK */

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

    /* Init global variable */

    sysProcNum = procNum;
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

LOCAL void
sysPhbCapt (void)
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

    /* point to window save arrays */

    pPhbCpu = &sysPhbCpuToPciWin[0];
    pPhbPci = &sysPhbPciToCpuWin[0];

    /* start with the cpu to pci windows (ppc slaves) */

    pPhbOffO = &sysPhbCpuWinOff[0];
    cpuPciEnab = MV64360_READ32 (MV64360_REG_BASE, CPUIF_BASE_ADDR_ENABLE);


    /* loop through each window */
    for(index = 0; index < PHB_CPU_WIN_CNT; index++)
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
           if (PCIIF_BASE_ADDR_ENABLE_INTMEM_BIT == pPhbOffI->enableBit)
           	{
		    pPhbPci->winLimit = pPhbPci->winBase + 0x10000 - 1;
           	}
           else
           	{
		    size = MV64360_READ32 (MV64360_REG_BASE, (pPhbOffI->size));
		    size = (((size >> 12) + 1) << 12); /* Cvrt encoding to real size */
		    pPhbPci->winLimit = pPhbPci->winBase + size - 1;
           	}

	    /*
	     * Now plug the cpu base and limit fields.  Note that the
	     * remap register in this calculation is formatted differently 
	     * than the remap register used in CPU to PCI outbound 
	     * translations dealt with above.  Here, the remap register needs 
	     * no shifting.  Otherwise the algorithm for base and limit
	     * computation is the same as the one presented above for the
	     * CPU to PCI case.
	     */

           if (PCIIF_BASE_ADDR_ENABLE_INTMEM_BIT == pPhbOffI->enableBit)
           	{
		    pPhbCpu->winBase = MV64360_REG_BASE;
		    pPhbCpu->winLimit = pPhbCpu->winBase + 0x10000 - 1;
           	}
           else
           	{
		    size = pPhbPci->winLimit - pPhbPci->winBase;
		    temp = pPhbPci->winBase & size;
		    remap = MV64360_READ32 (MV64360_REG_BASE, (pPhbOffI->remap));
		    remap &= ~size;
		    pPhbCpu->winBase = remap | temp;
		    pPhbCpu->winLimit = pPhbCpu->winBase + size;
           	}

            /* advance in preparation for next valid window */

            pPhbCpu++;
            pPhbPci++;
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
* Given a CPU address, this routine returns a corresponding local PCI bus
* or Compact (backpanel) PCI bus address provided that such an address exists.
* The target PCI bus (local or backpanel) is selected by the adrsSpace
* parameter. Legal values for this parameter are found in "pci.h". If a
* transparent bridge is used to access the Compact PCI bus, the local PCI bus
* and the backpanel PCI bus share the same address space. In this case, the
* local and backpanel address space designators are synonomous.
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
    char ** pBusAdrs    /* where to return bus address */
    )
    {

    switch (adrsSpace)
        {

        case PCI_SPACE_IO_PRI:
        case PCI_SPACE_IO_SEC:

            /* convert from cpu address space to local pci space */

            if ( sysCpuToPciAdrs (PCI_BAR_SPACE_IO, localAdrs, pBusAdrs) != OK )
                return (ERROR);

#ifdef INCLUDE_DEC2155X

            /*
             * if continuing on to the backpanel using the dec2155x, translate
             * from local pci space to compact pci space.
             */

            if ( PCI_SPACE_IS_CPCI(adrsSpace) )
                return ( sysPciToCpciAdrs (PCI_BAR_SPACE_IO, *pBusAdrs,
                                           pBusAdrs) );

#endif /* INCLUDE_DEC2155X */
            break;

        case PCI_SPACE_MEMIO_PRI:
        case PCI_SPACE_MEM_PRI:
        case PCI_SPACE_MEMIO_SEC:
        case PCI_SPACE_MEM_SEC:

            /* convert from cpu address space to local pci address space */

            if (sysCpuToPciAdrs (PCI_BAR_SPACE_MEM, localAdrs, pBusAdrs) != OK )
                return (ERROR);

#ifdef INCLUDE_DEC2155X

            /*
             * if continuing on to the backpanel using the dec2155x, translate
             * from local pci space to compact pci space.
             */

            if ( PCI_SPACE_IS_CPCI(adrsSpace) )
                return ( sysPciToCpciAdrs(PCI_BAR_SPACE_MEM, *pBusAdrs,
                                          pBusAdrs) );

#endif /* INCLUDE_DEC2155X */
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
* Given a local or Compact (backpanel) PCI address, this routine returns a
* corresponding local CPU bus address provided such an address exists. The
* originating PCI bus (local or backpanel) is selected by the adrsSpace
* parameter. Legal values for this parameter are found in "pci.h". If a
* transparent bridge is used to access the Compact PCI bus, the local PCI bus
* and the Compact PCI bus share the same address space. In this case, the
* local and backpanel address space designators are synonomous.
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
    	STATUS tmp;

    switch (adrsSpace)
        {
        
        case PCI_SPACE_IO_SEC:

#ifdef INCLUDE_DEC2155X

            /*
             * translate from compact PCI address space to local PCI address
             * space.
             */

            if ( sysCpciToPciAdrs (PCI_BAR_SPACE_IO, busAdrs, &busAdrs) != OK )
                return (ERROR);

            /* fall through */

#endif /* INCLUDE_DEC2155X */

        case PCI_SPACE_IO_PRI:

            /*
             * translate from local PCI address space to CPU address
             * space.
             */

            return ( sysPciToCpuAdrs (PCI_BAR_SPACE_IO, busAdrs, pLocalAdrs) );

        case PCI_SPACE_MEMIO_SEC:
        case PCI_SPACE_MEM_SEC:

#ifdef INCLUDE_DEC2155X

            /*
             * Determine, if possible, the local PCI address which
             * would be produced if the given cPCI is intercepted by
             * the drawbridge and passed through to the local PCI bus
             * (downstream transaction).  Note that this call violates
             * the "pure" model of sysBusToLocalAdrs() which, when
             * given a cPCI address, produces the local processor
             * address which would ultimately produce the given cPCI
             * address in an upstream transaction.  The reason for
             * this is to a support shared memory situation in which
             * two nodes communicated over a local PCI bus but a third
             * node in the mix requires over the cPCI bus.  In this
             * case the cPCI bus is the advertised bus (smArg1) and
             * the nodes which communicate with each other over the
             * local PCI bus must be able to translate a cPCI bus
             * address into a local address even though they don't
             * have upstream windows which access the advertised
             * shared memory cPCI bus.
             */

	    if (sysCpciToPciAdrsDs(PCI_BAR_SPACE_MEM, busAdrs, &busAdrs) != OK)
	        {
		if ( sysCpciToPciAdrs (PCI_BAR_SPACE_MEM, busAdrs, &busAdrs) !=
		     OK)
		    {
                    return (ERROR);
		    }
		}

            /* fall through */

#endif /* INCLUDE_DEC2155X */

        case PCI_SPACE_MEMIO_PRI:
        case PCI_SPACE_MEM_PRI:

            /*
             * translate from local PCI address space to CPU address
             * space.
             */
            tmp = sysPciToCpuAdrs (PCI_BAR_SPACE_MEM, busAdrs, pLocalAdrs);
            return tmp; 

        default:
            return (ERROR);
        }

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
* sysBusTas - test and set a location across the bus
*
* The cPCI bridge chips do not support PCI target locking, therefore there is
* no atomic RMW operation. This routine performs a software-based mutual
* exclusion algorithm in place of a true test and set.
*
* NOTE: This algorithm is performed through a PCI-to-PCI bridge to a shared
* location that is subject to unprotected access by multiple simultaneous
* processors. There is the possibility that the bridge will deliver a delayed
* read completion to a PCI bus master which was not the original initiator of
* the delayed read. The bridge delivers the delayed read completion to the new
* requestor because it believes that the new delayed read request is actually
* the original master performing a delayed read retry as required by the PCI
* spec. When the original master comes back with the genuine retry, the bridge
* treats it as a new request. When this "aliasing" occurs, a read performed
* after a write can appear to complete ahead of the write, which is in violation
* of PCI transaction ordering rules. Since this algorithm depends on a strict
* time-ordered sequence of operations, it can deadlock under this condition.
* To prevent the deadlock, a throw-away read must be performed after the initial
* write. Since the bridge only remembers once instance of a queued delayed
* read request, the throw-away read will "consume" the results of a
* mis-directed read completion and subsequent read requests are guaranteed to
* be queued and completed after the write.
*
* RETURNS: TRUE if lock acquired.
* FALSE if lock not acquired.
*
* SEE ALSO: sysBusTasClear()
*/

BOOL sysBusTas
    (
    char * adrs      /* address to be tested and set */
    )
    {
    FAST int    value;    /* value to place in lock variable */
    FAST int    nChecks;  /* number of times to re-check lock */
    FAST int    count;    /* running count of re-checks */
    int     oldLvl;       /* previous interrupt level */

    volatile int * lockAdrs = (int *)adrs;

    if (sysSmUtilTasValue == 0)
        sysSmUtilTasValue =  (TAS_CONST + sysProcNumGet ())<< 24;

    value   = sysSmUtilTasValue;        /* special value to write */
    nChecks = DEFAULT_TAS_CHECKS;        /* number of checks to do */

    /* Lock out interrupts */

    oldLvl = intLock ();

    /* Test that lock variable is initially empty */

    if (*lockAdrs != 0)
        {
        intUnlock (oldLvl);
        return (FALSE);             /* someone else has lock */
        }

    /* Set lock value */

    *lockAdrs = value;

    /* Perform a preliminary read due to PCI bridge issues */

    count = *lockAdrs;

    /* Check that no one else is trying to take lock */

    for (count = 0;  count < nChecks;  count++)
        {
        if (*lockAdrs != value)
            {
            intUnlock (oldLvl);
            return (FALSE);            /* someone else stole lock */
            }
        }

    intUnlock (oldLvl);
    return (TRUE);                /* exclusive access obtained */

    }

/******************************************************************************
*
* sysBusTasClear - clear a location set by sysBusTas()
*
* This routine clears a bus test and set location.  This routine is only
* required if the sysBusTas() routine uses special semaphore techniques (such
* as bus locking).  Since the sysBusTas routine doesn't use any special
* semaphore techniques, this routine is a no-op. 
*
* The BSP activates this routine by placing its address into the global 
* variable 'smUtilTasClearRtn'.
*
* RETURNS: N/A
*
* SEE ALSO: sysBusTas()
*/

void sysBusTasClear
    (
    volatile char * address ARG_UNUSED /* address to be cleared */
    )
    {
    }

/******************************************************************************
*
* sysNvRead - read one byte from NVRAM
*
* This routine reads a single byte from a specified offset in NVRAM (User I2C
* SROM).
*
* RETURNS: The byte from the specified NVRAM offset.
*/

UCHAR sysNvRead
    (
    ULONG    offset    /* NVRAM offset to read the byte from */
    )
    {
    UCHAR temp;

    while (ERROR == sysMotI2cRead(NV_RAM_ADRS, offset, USR_BYTE_ADDR_SZ, &temp, 1)){};

    return (temp);
    }

/******************************************************************************
*
* sysNvWrite - write one byte to NVRAM
*
* This routine writes a single byte to a specified offset in NVRAM.(User I2C
* SROM).
*
* RETURNS: N/A
*/

void sysNvWrite
    (
    ULONG    offset, /* NVRAM offset to write the byte to */
    UCHAR    data    /* datum byte */
    )
    {
    while (ERROR == sysMotI2cWrite (NV_RAM_ADRS, offset, USR_BYTE_ADDR_SZ, &data, 1)){};
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
* sysMemProbeTrap - trap handler for vxMemProbe exception
*
* This routine is called from the excConnectCode stub if sysMemProbeSup
* generates an exception. This code simply increments a static variable each
* time an exception is generated and advances to the next instruction.
*/

LOCAL int sysMemProbeTrap
    (
    ESFPPC *    pEsf        /* pointer to exception stack frame */
    )
    {
    REG_SET *pRegSet = &pEsf->regSet;

    pRegSet->pc += (_RType)4;  /* advance to next instruction */

    sysProbeFault++; /* indicate trap occurred */

    return (0);
    }

/******************************************************************************
*
* sysPciWriteFlush - flush posted PCI writes from buffer
*
* This routine flushes the posted write buffer in the MV64360.
*
* RETURNS: OK if flush succeeded or ERROR if an error occurred.
*/

STATUS sysPciWriteFlush(void)
    {

    /*
     * DEBUG: In the MCP820 the Harrier's posted write buffer was
     *        flushed by doing a dummy read of one of its MPC registers
     *        (HARRIER_DEVICE_ID_REG).
     */
#ifdef INCLUDE_DEC2155X
    char *   temp;
#endif /* INCLUDE_DEC2155X */

#ifdef INCLUDE_DEC2155X

    /* Flush the Dec2155x posted write buffer by reading from a cPCI location */

    if (sysBusToLocalAdrs (PCI_SPACE_MEM_SEC, (char *) CPCI_FLUSH_ADDR,
               &temp) != OK)

    return (ERROR);

    temp = (char *) ((UINT32 *)temp);

#endif /* INCLUDE_DEC2155X */

    return (OK);

    }

/******************************************************************************
*
* sysMemProbeBus - probe an address on a bus.
*
* This routine probes a specified address to see if it is readable or
* writable, as specified by <mode>.  The address will be read or written as
* 1, 2, or 4 bytes as specified by <length> (values other than 1, 2, or 4
* yield unpredictable results).  If the probe is a VX_READ, the value read will
* be copied to the location pointed to by <pVal>.  If the probe is a VX_WRITE,
* the value written will be taken from the location pointed to by <pVal>.
* In either case, <pVal> should point to a value of 1, 2, or 4 bytes, as
* specified by <length>.
*
* This routine probes the specified address with interrupts disabled and
* a special handler for Data Access and Alignment exceptions.
*
* RETURNS: OK if probe succeeded or ERROR if an exception occurred.
*/

LOCAL STATUS sysMemProbeBus
    (
    char   * adrs,    /* address to be probed */
    int      mode,    /* VX_READ or VX_WRITE */
    int      length,  /* 1, 2 or 4 byte probe */
    char   * pVal     /* address of value to write OR */
                      /* address of location to place value read */
    )
    {
    FUNCPTR  oldVec;
    STATUS   status;

    /* clear fault flag */

    sysProbeFault = 0;

    /*
     *    Handle Data Access Exceptions locally
     *
     *    Data Access Exceptions will occur when trying to probe addresses
     *    that have not been mapped by the MMU.
     */

    oldVec = excVecGet ((FUNCPTR *) _EXC_OFF_DATA);
    excVecSet ((FUNCPTR *) _EXC_OFF_DATA, FUNCREF(sysMemProbeTrap));

    /* do probe */

    if (mode == VX_READ)
        {
        status = sysMemProbeSup (length, adrs, pVal);
        }
    else
        {
        status = sysMemProbeSup (length, pVal, adrs);
        }

    /* restore original vector */

    excVecSet ((FUNCPTR *) _EXC_OFF_DATA, oldVec);

    if (status == OK) /* no parameter errors during probe */
        {

        /* if a PCI write was performed, flush the write post buffer(s) */

        if (mode == VX_WRITE)

            status = sysPciWriteFlush ();

        }

    /* check for MMU fault */

    if (sysProbeFault != 0)

        return (ERROR);

    else

        return (status);

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

#ifdef INCLUDE_DEC2155X

    sysDec2155xErrClr ();

#endif /* INCLUDE_DEC2155X */

    /*
     * DEBUG: In the MCP820 the Harrier Error Exception registers
     *        were also cleared. We may need to add code to clear
     *        errors in the Discovery II.
     */

    /* Clear PowerPC Data Access Exception Registers */

    vxDarSet   (0);
    vxDsisrSet (0);
    vxSrr0Set  (0);
    vxSrr1Set  (0);
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

LOCAL STATUS sysPciProbe
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
    
#ifdef INCLUDE_DEC2155X

    UINT16 secCmd;     /* Dec2155x secondary command register image */
    UINT16 chpCtrl0;   /* Dec2155x chip control 0 register image */
    UINT8  secSerr;    /* Dec2155x secondary SERR control */

#endif /* INCLUDE_DEC2155X */

    /* probe performed with interrupts disabled */

    oldLevel = intLock ();

    /* flush PCI posted write buffer(s) */

    if ( (status = sysPciWriteFlush ()) != OK )
        {
        intUnlock (oldLevel);
        return (status);
        }

    /* Clear any existing errors/exceptions */

    sysProbeErrClr ();

#ifdef INCLUDE_DEC2155X

    /* save current Dec2155x error reporting configuraion */

    pciConfigInWord (0, DEC2155X_PCI_DEV_NUMBER, 0, PCI_CFG_COMMAND, &secCmd);
    pciConfigInWord (0, DEC2155X_PCI_DEV_NUMBER, 0, DEC2155X_CFG_CHP_CTRL0,
             &chpCtrl0);
    pciConfigInByte (0, DEC2155X_PCI_DEV_NUMBER, 0,
             DEC2155X_CFG_SEC_SERR_DISABLES, &secSerr);

    /* enable error reporting via Target Abort and SERR */

    pciConfigOutWord (0, DEC2155X_PCI_DEV_NUMBER, 0, PCI_CFG_COMMAND,
              (secCmd | PCI_CMD_SERR_ENABLE));
    pciConfigOutWord (0, DEC2155X_PCI_DEV_NUMBER, 0, DEC2155X_CFG_CHP_CTRL0,
              (chpCtrl0 | DEC2155X_CC0_MSTR_ABRT_MD));
    pciConfigOutByte (0, DEC2155X_PCI_DEV_NUMBER, 0,
              DEC2155X_CFG_SEC_SERR_DISABLES,
              (secSerr & ~DEC2155X_SERR_DIS_PSTD_WRT_MSTR_ABRT));

#endif

    /* 
     * DEBUG: At this point in the MCP820 interrupts and machine 
     *        checks for Target Abort, Master Abort and SERR are
     *        disabled.  Once the probe is complete "error reporting"
     *        is returned to its previous state. Errors reported during
     *        the probe, in the Harrier, result in a return status of ERROR.
     */

    /* Perform probe */

    status = sysMemProbeBus (adrs, mode, length, pVal);

    /* Clear any errors/exceptions */

    sysProbeErrClr ();
    
#ifdef INCLUDE_DEC2155X

    /* restore original Dec2155x error reporting configuration */

    pciConfigOutWord (0, DEC2155X_PCI_DEV_NUMBER, 0, PCI_CFG_COMMAND, secCmd);
    pciConfigOutWord (0, DEC2155X_PCI_DEV_NUMBER, 0, DEC2155X_CFG_CHP_CTRL0,
              chpCtrl0);
    pciConfigOutByte (0, DEC2155X_PCI_DEV_NUMBER, 0,
              DEC2155X_CFG_SEC_SERR_DISABLES, secSerr);

#endif /* INCLUDE_DEC2155X */
    
    /* safe to re-enable interrupts */

    intUnlock (oldLevel);

    return (status);

    }

/******************************************************************************
*
* sysBusProbe - probe a bus address based on bus type.
*
* This routine is a function hook into vxMemProbe.  It determines which bus,
* PCI or local is being probed based on the address to be probed.
* If the PCI bus is being probed, the sysPciProbe() routine is called to do the
* special PCI probing. If the local bus is being probed, the routine calls an
* architecture-specific probe routine. 
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
    STATUS status;

    /* Clear any existing errors/exceptions */

    sysProbeErrClr ();

    /* Handle PCI bus in special manner */

    if (IS_PCI_ADDRESS(adrs))
        status = sysPciProbe (adrs, mode, length, pVal);

    /* Handle local bus in architecture-specific manner */

    else
        status = vxMemArchProbe (adrs, mode, length, pVal);

    /* Clear any errors/exceptions before exiting */

    sysProbeErrClr ();

    return (status);
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

void
sysUsDelay (
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
  SIO_CHAN *pSioChan;        /* serial I/O channel */

  msgSize = strlen(str);

  sysSerialHwInit(); 

  pSioChan = sysSerialChanGet(0);

  sioIoctl(pSioChan, SIO_MODE_SET, (void *) SIO_MODE_POLL);

  for(msgIx = 0; msgIx < msgSize; msgIx++)
  {
    while (sioPollOutput (pSioChan, str[msgIx]) == EAGAIN);
  }

  /* follow caller's error recovery policy. */

  if(recovery == EXIT_TO_SYSTEM_MONITOR) sysToMonitor(BOOT_NO_AUTOBOOT);
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

/*******************************************************************************
* sysVirtToPhys - Translate MMU virtual address to physical address
*
* DESCRIPTION:
*       This function translates virtual address to physical.
*
* INPUT:
*       unsigned int addr   input address.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Translated address.
*
*******************************************************************************/

unsigned int  sysVirtToPhys(unsigned int addr)
{
    return addr;
}

/*******************************************************************************
* sysPhysToVirt - Translate MMU physical address to virtual address
*
* DESCRIPTION:
*       This function translates physical address to virtual.
*
* INPUT:
*       unsigned int addr   input address.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Translated address.
*
*******************************************************************************/

unsigned int sysPhysToVirt(unsigned int addr)
{
    return addr;
}

/*******************************************************************************
*
* prpmcSlaveWaitConfig - Wait for Monarch to enumerate.
*
* This routine holds the Slave PrPMC until the Monarch has enumerated the
* PCI bus.
*
* RETURNS: OK
*/



STATUS
prpmcSlaveWaitConfig(int bus, int device, int whichBar)
{
  UINT timeout = PRPMC_SLAVE_SIDE_TIMEOUT;
  UINT barReg, barReg1, barReg2, barReg3, barReg4;
  char txt[80];

*(unsigned int *)0x12000000 = 0;
*(unsigned int *)0x12000004 = 0;
*(unsigned int *)0x12000008 = 0;
*(unsigned int *)0x1200000c = 0;
*(unsigned int *)0x12000010 = 0;

*(unsigned int *)0x12000100 = 0;
*(unsigned int *)0x12000104 = 0;
*(unsigned int *)0x12000108 = 0;
*(unsigned int *)0x1200010c = 0;
*(unsigned int *)0x12000110 = 0;

  while(timeout != 0)
  {

    /* read the contents of the slave's PCI bar register */
    sysPciConfigRead(bus, device, 0, whichBar, 4, &barReg);
    sysPciConfigRead(bus, device, 0, PCI_CFG_BASE_ADDRESS_1, 4, &barReg1);
    sysPciConfigRead(bus, device, 0, PCI_CFG_BASE_ADDRESS_2, 4, &barReg2);
    sysPciConfigRead(bus, device, 0, PCI_CFG_BASE_ADDRESS_3, 4, &barReg3);
    sysPciConfigRead(bus, device, 0, PCI_CFG_BASE_ADDRESS_4, 4, &barReg4);


    /* if the PCI Host has setup the bar, Host is done with the slave.*/

	/*Sergey: 0xff000000 was shows up when pmc was mapping 16MB of its memory to PCI;
    in this case when host's 'pciAutoFuncConfig' writes 0xffffffff to BAR0, BAR0 reads
    as 0xff000000; soon after that host will set actual mapping address to BAR0; so
    BAR0 will change its contents from 0 to mapped memory size (in vierd form, like
    16MB corresponds to 0xff000000, 512MB corresponds to 0xe0000000 etc) and almost
    immediately change it again to actual address as pmc's memory will be seen from
    host; we can consider first BAR0 change as sign that host finished enumeration,
    although it maybe better to wait for second change ...
	*/

	/* mv5100/mv5500/mv6100 (for mv5100 it fake, see mv5100's sysLib.c) */ 
    if( ((barReg&0xFFFFFFF0)!=0) && ((barReg&0xFFFFFFF0) != 0xe0000000) )

	/* original
    if( (barReg & 0xFFFFFFF0) != 0)
	*/

    {
*(unsigned int *)0x12000100 = barReg;
*(unsigned int *)0x12000104 = barReg1;
*(unsigned int *)0x12000108 = barReg2;
*(unsigned int *)0x1200010c = barReg3;
*(unsigned int *)0x12000110 = barReg4;
      break;
    }
    else
    {
*(unsigned int *)0x12000000 = barReg;
*(unsigned int *)0x12000004 = barReg1;
*(unsigned int *)0x12000008 = barReg2;
*(unsigned int *)0x1200000c = barReg3;
*(unsigned int *)0x12000010 = barReg4;
    }

	/*
      sprintf(txt,"prpmcSlaveWaitConfig: BAR0=0x%08x\r\n ",barReg);
      sysDebugMsg(txt,CONTINUE_EXECUTION);
	*/

    /* Delay 1 mSec */
    sysUsDelay(1000); /* Delay 1 mSec */
    --timeout;
  }

  /* If configuration timeout, report the error and re-start. */
  if(timeout == 0)
  {
    sysDebugMsg("prpmcSlaveWaitConfig: Configuration Timeout.\n\r",
                     EXIT_TO_SYSTEM_MONITOR);
  }

  return(OK);
}

/*****************************************************************************
*
* sysPciOutWordConfirm - Word out to PCI memory space and flush buffers.
*
* This function outputs a word to PCI memory space and then flushes the PCI
* write posting buffers by reading from the target address. Since the PCI
* spec requires the completion of posted writes before the completion of delayed
* reads, when the read completes, the write posting buffers have been flushed.
*
* NOTE: If the write is performed through a PCI-to-PCI bridge to a shared
* location that is subject to unprotected access by multiple simultaneous
* processors, there is the possibility that the bridge will deliver a delayed
* read completion to a PCI bus master which was not the original initiator of
* the delayed read. When this occurs, it appears as if a PCI delayed read had
* passed a posted write, which would violate PCI transaction ordering rules.
* If this is a concern, an additional read must be performed outside of this
* routine to guarantee that the confirming read performed in this routine was
* not aliased.
*
* RETURNS: N/A
*/

void sysPciOutWordConfirm
    (
    UINT32 adrs,       /* PCI address */
    UINT16 data        /* data to be written */
    )
    {
    UINT16 temp;

    sysPciOutWord (adrs, data);
    temp = sysPciInWord (adrs);

    }

/*******************************************************************************
*
* sysBusIntAck - acknowledge a bus interrupt
*
* This routine acknowledges a specified Compact PCI bus interrupt level.
*
* NOTE: This routine is included for BSP compliance only.  Since PCI and Compact
* PCI bus interrupts are routed directly to the interrupt controller, interrupts
* are re-enabled in the interrupt controller's handler and this routine is a
* no-op.
*
* RETURNS: NULL.
*
* SEE ALSO: sysBusIntGen()
*/

int sysBusIntAck
    (
    int intLevel        /* interrupt level to acknowledge */
    )
    {
    return (0);
    }

/*******************************************************************************
*
* sysBusIntGen - generate a bus interrupt
*
* This routine generates an out-bound PCI or Compact PCI backpanel interrupt.
* The method used to generate the interrupt is based on the current
* configuration and operating mode of the BSP. If the BSP was built with
* DEC2155X support and the BSP is operating as the (local) PCI Host device, the
* outbound Compact PCI bus interrupt is generated by calling
* sysDec2155xBusIntGen which sets a bit in the secondary mailbox register of
* the 2155x. If the BSP does not contain DEC2155X support or is operating as a
* PCI Slave device, the outbound PCI interrupt is generated by calling
* sysMpicBusIntGen which sets a bit in the CPU 1 portion of the MPIC's
* Inter-Process Interrupt mechanism. The PRPMC750 hardware ties the CPU 1
* interrupt line to one of the PCI bus interrupt pins which allows this
* mechanism to generate PCI bus interrupts.
* 
* RETURNS: The results of sysDec2155xBusIntGen or sysMpicBusIntGen.
*
* SEE ALSO: sysBusIntAck()
*/

STATUS sysBusIntGen
    (
    int level,          /* interrupt level to generate (not used) */
    int vector          /* interrupt vector for interrupt */
    )
    {
#ifdef INCLUDE_DEC2155X
    return (sysDec2155xBusIntGen (level, vector));
#else
    return(ERROR);
#endif
    }

#ifdef INCLUDE_SM_COMMON

/*****************************************************************************
*
* sysSmIntTypeCompute - Compute shared memory interrupt type
*
* This function computes the shared memory interrupt type. It must
* be computed at run time since this BSP can operate in either PCI Host or
* Slave mode.
*
* RETURNS: SM_INT_ARG1 value.
*/

int sysSmIntTypeCompute (void)
    {
    if (sysMonarchMode)

        return (SM_HOST_INT_TYPE);

    else

        return (SM_SLAVE_INT_TYPE);
    }

/*****************************************************************************
*
* sysSmArg1Compute - Compute dynamic run-time shared memory parameters.
*
* This function computes the shared memory SM_INT_ARG1 parameter. It must
* be computed at run time since this BSP can operate in either PCI Host or
* Slave mode.  When mailbox interrupts are used, this argument refers to
* the PCI address space which must be use with a sysBusToLocalAdrs() in order
* convert the advertised bus address of the mailbox (SM_INT_ARG2) into a 
* local address.
*
* RETURNS: SM_INT_ARG1 value.
*/

int sysSmArg1Compute
    (
    int intType
    )
    {

    /*
     * ARG1 must indicate that the mailbox location it is on the 
     * local PCI bus.
     */

    return (PCI_SPACE_MEM);

    }

/*****************************************************************************
*
* sysSmArg2Compute - Compute dynamic run-time shared memory parameters.
*
* This function computes the shared memory SM_INT_ARG2 parameter. It must
* be computed at run time since this BSP can operate in either PCI Host or
* Slave mode.  When using mailbox interrupts, this argument specifies the
* offset into the PCI bus space (specified by SM_INT_ARG1) at which the
* mailbox resides.
*
* RETURNS: NA
*/

int sysSmArg2Compute
    (
    int intType
    )
    {
    UINT32 smAddr;
    UINT32 pciAddr;
    UINT32 locAddr;

    /*
     * Our discovery's own doorbell points to the PCI location.
     * Convert the value in the PCI doorbell to a local address, 
     * add in the offset to the Inbound Doorbell Register,
     * and bias by the appropriate amount to reach the SM_MAILBOX_INT_BIT.
     */

    sysPciConfigRead (0, 0, PCI_FCN0, 
			     PCIIF_INTERNAL_MEM_MAPPED_BASE_ADDR_LOW_PCICFG, 4, &pciAddr);

    pciAddr &= PCI_MEMBASE_MASK;
    pciAddr += (MUIF_INBND_DOORBELL_PCI0 + (SM_MAILBOX_INT_BIT % 8));
    sysBusToLocalAdrs (PCI_SPACE_MEM_PRI, (char *)pciAddr,
		       (char **)&locAddr);

    /* Wait for Drawbridge to be configured by host */
    while (sysLocalToBusAdrs (PCI_SPACE_MEM, (char *)locAddr, 
			   (char **)&smAddr) != OK);
    	
    return((int)smAddr);
    }

/*****************************************************************************
*
* sysSmArg3Compute - Compute dynamic run-time shared memory parameters.
*
* This function computes the shared memory SM_INT_ARG3 parameter. It must
* be computed at run time since this BSP can operate in either PCI Host or
* Slave mode.  When using mailbox interrupts, this argument specifies the
* value to write to the offset defined by SM_INT_ARG2.
*
* RETURNS: NA
*/

int sysSmArg3Compute (void)
    {

    /*
     * The assumed mailbox type is mailbox_1 (1-byte mailbox).
     * The bit position to set when writing to this mailbox is
     * the byte-relative position, hence the need for the modular 
     * 8 adjustment.
     */

    return (1 << (SM_MAILBOX_INT_BIT % 8));

    }

#ifdef SYS_SM_ANCHOR_POLL_LIST
/*****************************************************************************
*
* sysSmAnchorCandidate - Dynamically poll for the anchor address.
*
* This function will determine whether a given bus, device, and function
* number correspond to a defined device on the SYS_SM_ANCHOR_POLL_LIST
*
* RETURNS: BAR offset address device is on the SYS_SM_ANCHOR_POLL_LIST
* 0xffffffff if device is not on the SYS_SM_ANCHOR_POLL_LIST
*/

LOCAL UINT sysSmAnchorCandidate
    (
    UINT busNo,		/* candidate's PCI bus number */
    UINT deviceNo,	/* candidate's PCI device number */
    UINT funcNo		/* candidate's PCI function number */
    )
    {
    UINT i;
    UINT barOffset = 0xffffffff;
    UINT devVend;
    UINT subIdVend;

    for (i = 0 ; (sysSmAnchorPollList[i].devVend != 0xffffffff) ; i++)
	{
	SM_PCI_CONFIG_IN_LONG (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID,
			       &devVend);

	if (devVend == sysSmAnchorPollList[i].devVend)
	    {

	    SM_PCI_CONFIG_IN_LONG (busNo, deviceNo, funcNo,
                                   PCI_CFG_SUB_VENDER_ID, &subIdVend);

	    if (subIdVend == sysSmAnchorPollList[i].subIdVend)
		{
		barOffset =  sysSmAnchorPollList[i].barOffset;
		break;
		}
	    }
	}
    return (barOffset);
    }
#endif /* SYS_SM_ANCHOR_POLL_LIST */

/*****************************************************************************
*
* smTryAddr - Determine if this address corresponds to an "anchor"
*
* RETURNS: OK if anchor has been found.
* ERROR if anchor has not been found or not yet determined.
*/

LOCAL char * smTryAddr
    (
    SM_ANCHOR * tryAddr		/* address to determine if it's anchor */
    )

    {
    LOCAL   UINT   beat1 = 0;
    LOCAL   UINT   beat2 = 0;

    if (tryAddr->readyValue != SM_READY)
	return ((char *)0xffffffff);

    /*
     * SM_READY has been found, check for a heartbeat that changes
     */

    beat2 = *(UINT *)((UINT)(tryAddr->smPktHeader) + (UINT)tryAddr);

    if (beat1 == 0)
	{
	
        /*
         * First time through we just record the heartbeat value so
         * we can check it on the next call.
         */

        beat1 = beat2;
	return ((char *)0xffffffff);	/* determination no yet made */
	}

    else
        {
	
        /*
         * On second and subsequent calls, we just check for a 
	 * change in the heartbeat
	 */

	if (beat1 == beat2)
	    {
	    return ((char *)0xffffffff); /* determination not yet made */
	    }
        }
    return ((char *)tryAddr);
    }

/*****************************************************************************
*
* sysSmAnchorFind - Dynamically poll for the anchor address.
*
* This function makes one pass on a polling list of devices in an attempt
* to find the shared memory anchor address.  The list of devices polled
* includes the system memory DRAM if SYS_SM_SYSTEM_MEM_POLL is defined.
* In addition, if SYS_SM_ANCHOR_POLL_LIST is defined, the devices on this
* list are also included in the search.  If SYS_SM_ANCHOR_POLL_LIST is not
* defined, then all devices on the "pciBus" parameter are polled for the
* existence of the shared memory anchor.  The "window" which is searched
* corresponds to PCI_CFG_BASE_ADDRESS_0 and the offset queried is
* SM_ANCHOR_OFFSET.  If a value of SM_READY is found, the shared memory
* anchor is assumed to be at the corresponding address.
*
* RETURNS: OK if SM_READY found, 'anchor' parameter will contain local address.
* ERROR if SM_READY not find after one scan of the list.
*/

LOCAL STATUS sysSmAnchorFind
    (
    int     pciBus,	/* PCI bus number */
    char ** anchor	/* return variable */
    )
    {
    SM_ANCHOR * tryAddr;

    UINT32 devVend;
    UINT32 memBarVal;
    UINT32 barAddr;
    int    deviceNo;
    int    maxDevNo;
    char   * anchorAddr;

    if (sysProcNumGet () == 0)
        {
#if       (!SM_OFF_BOARD)
            *anchor = (char *)SM_ANCHOR_ADRS;
#endif
        return (OK);
        }

#ifdef SYS_SM_SYSTEM_MEM_POLL

    /*
     * Look in host DRAM for anchor:
     * We will receive an ERROR return initially if the host has not
     * yet enabled the bus master bit in the primary PCI command register.
     */

    if ( sysBusToLocalAdrs (PCI_SPACE_MEM,
			    (char *)(MSTR_MEM_BUS + SM_ANCHOR_OFFSET),
			    (char **)&tryAddr) == OK )

	{
        anchorAddr = smTryAddr(tryAddr);

        if (anchorAddr != (char *)0xffffffff)
	    {
            *anchor = anchorAddr;	
	    return (OK);
	    }
	}

#endif /* SYS_SM_SYSTEM_MEM_POLL */

#ifdef SYS_SM_ANCHOR_POLL_LIST

    /* if polling list is defined but empty then we're done */

    if (sysSmAnchorPollList[0].devVend != 0xffffffff)
#endif
	{
	if (pciBus == SYS_SM_BUS_NUMBER)
	    maxDevNo = 16;
	else
	    maxDevNo = PCI_MAX_DEV;

	for (deviceNo = 0; deviceNo < maxDevNo; deviceNo++)
	    {
	    if (SM_PCI_CONFIG_IN_LONG (pciBus, deviceNo, 0, 0,
                                       &devVend) != OK)
		continue;

#ifdef SYS_SM_ANCHOR_POLL_LIST
	    barAddr = sysSmAnchorCandidate (pciBus, deviceNo, 0);
	    if (barAddr == 0xffffffff)
		continue;	/* not a candidate */
#else
	    barAddr = PCI_CFG_BASE_ADDRESS_0; /* assume 0th BAR */
#endif

	    /* Check memory through specified BAR */

	    if (SM_PCI_CONFIG_IN_LONG (pciBus, deviceNo, 0,
			               barAddr, &memBarVal) != OK)
	        continue;

            memBarVal &= PCI_MEMBASE_MASK;

	    if ( (memBarVal == (0xffffffff & PCI_MEMBASE_MASK)) || 
	         (memBarVal == (0x00000000 & PCI_MEMBASE_MASK)) )
		continue;

	    if (sysBusToLocalAdrs (PCI_SPACE_MEM,
			           (char *)(memBarVal + SM_ANCHOR_OFFSET),
				   (char **)&tryAddr) != OK)
		continue;

	    anchorAddr = smTryAddr(tryAddr);
	    if (anchorAddr != (char *)0xffffffff)
		{
		*anchor = (char *)tryAddr;
		return (OK);
		}
	    }
	}       
    return (ERROR);
    }

/*****************************************************************************
*
* sysSmAnchorPoll - Dynamically poll for the anchor address.
*
* This function will loop and repeatedly call a polling routine to search
* for the shared memrory anchor on the cPCI bus number indicated by
* SYS_SM_BUS_NUMBER.  It returns with an anchor address only when the
* anchor is actually found.
*
* RETURNS: Anchor address
*/

LOCAL char *sysSmAnchorPoll (void)
    {

    char *anchor;
    int  count = 0;

    while (sysSmAnchorFind (SYS_SM_BUS_NUMBER, &anchor) != OK)
        {
        if (((++count) % 6) == 0)
            printf ("sysSmAnchorPoll() searching for shared memory anchor\n");
        sysUsDelay (1000000);
        }
    return (anchor);
    }

/*****************************************************************************
*
* sysSmAnchorAdrs - Dynamically compute anchor address
*
* This function is called if SM_OFF_BOARD == TRUE, that is the shared memory
* anchor is not on this node.  If the anchor has been found by a previous
* call to this routine, the previously found anchor value is returned.
* Otherwise  a polling routine is called which will dynamically search for
* the anchor.  This routine is aliased to SM_ANCHOR_ADRS and thus is first
* called when SM_ANCHOR_ADRS is first referenced.
*
* RETURNS: Anchor address
*/

char *sysSmAnchorAdrs (void)
    {
    static char *anchorAddr;
    static int anchorFound = FALSE;

    if (anchorFound)
        return (anchorAddr);
    anchorAddr = sysSmAnchorPoll ();
    anchorFound = TRUE;
    return (anchorAddr);
    }

#endif /* (SM_OFF_BOARD == TRUE) */

/*****************************************************************************
*
* sysSysconAsserted - check the state of the syscon bit
*
* This function examines the state of the syscon bit in the board status
* register. If the board is acting as the system controller the syscon bit will
* be a zero (low-true). If the syscon bit is not a zero, then this board is not
* acting as the system controller.
*
* RETURNS: TRUE is SYSCON is asserted, FALSE if not asserted.
*/
BOOL sysSysconAsserted (void)
    {
    return ((!((*(UINT8 *)SYSTEM_STATUS_REG_4) & 
                        SYSTEM_STATUS_REG_4_PHY0_MONARCH_L_BIT) )? TRUE : FALSE);
    }

/*******************************************************************************
*
* sysIntDisable - disable a bus interrupt level (vector)
*
* This routine disables reception of a specified local or Compact PCI bus
* interrupt. 
*
* RETURNS: The results of sysSmIntDisable or intDisable.
*
* SEE ALSO: sysSmIntDisable, sysIntEnable()
*/

STATUS sysIntDisable
    (
    int intLevel        /* interrupt level (vector) */
    )
    {
    return (intDisable (intLevel));
    }

/*******************************************************************************
*
* sysIntEnable - enable a bus interrupt level (vector)
*
* This routine enables reception of a specified local or Compact PCI bus
* interrupt. 
*
* RETURNS: The results of intEnable()
*
* SEE ALSO: intEnable()
*/

STATUS sysIntEnable
    (
    int intLevel        /* interrupt level (vector) */
    )
    {
    return (intEnable (intLevel) );
    }

#ifdef INCLUDE_SM_COMMON

/*******************************************************************************
*
* sysMailboxInt - mailbox interrupt handler
*
* Check to see if the interrupt applies to us.  If it does, clear it and
* call the mailbox routine if it exists.
*/

LOCAL void sysMailboxInt (void)
    {

    UINT32  temp;

    temp = MV64360_READ32 (MV64360_REG_BASE, MUIF_INBND_DOORBELL_PCI0);
    /* Clear the inbound doorbell interrupt */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, 
			      MUIF_INBND_DOORBELL_PCI0, temp);
    

    temp &= (1 << SM_MAILBOX_INT_BIT);

    if (temp != 0)
        {

	/* Call the mailbox routine if it exists */

        if (sysMailbox.routine != NULL)
            (sysMailbox.routine) (sysMailbox.arg);

        }

    }

/*******************************************************************************
*
* sysMailboxConnect - connect a routine to the in-bound mailbox interrupt
*
* This routine connects a local interrupt service routine to a operating mode
* dependent interrupt vector. The in-bound interrupt mechanism differs
* depending on the configuration and operating mode of the BSP. If DEC2155X
* support was included and the BSP is operating as the (local) PCI Host device,
* the interrupt handler is connected to the 2155X's interrupt vector. If 2155X
* support was not included or the BSP is operating as a PCI Slave device, the
* interrupt handler is connected to the MPIC CPU 0 IPI0 vector.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysMailboxEnable()
*/

STATUS sysMailboxConnect
    (
    FUNCPTR routine,    /* routine called at each mailbox interrupt */
    int arg             /* argument with which to call routine      */
    )
    {

    if (!sysMailbox.connected)
        {
        if (intConnect (INUM_TO_IVEC(SM_MAILBOX_INT_VEC),
                        sysMailboxInt, 0) == ERROR)
            return (ERROR);

        /*
         * Save the routine address and argument, then mark the mailbox 
	 * interrupt handler connected.
         */

        sysMailbox.arg       = arg;
        sysMailbox.routine   = routine;
        sysMailbox.connected = TRUE;
        }
    
    return (OK);
    }

/*******************************************************************************
*
* sysMailboxEnable - enable the in-bound mailbox interrupt
*
* This routine enables an operating mode dependent in-bound mailbox interrupt.
*
* RETURNS: OK or ERROR if sysMpicIneEnable fails.
*
* SEE ALSO: sysMailboxConnect(), sysMailboxDisable()
*/

STATUS sysMailboxEnable
    (
    char *mailboxAdrs           /* address of mailbox (ignored) */
    )
    {
    int    oldVal;
    UINT32  temp;
    
    if (intEnable (SM_MAILBOX_INT_VEC) == ERROR)
	return (ERROR);

    oldVal = intLock();
    temp = MV64360_READ32 (MV64360_REG_BASE, MUIF_INBND_INTERRUPT_MASK_PCI0);
    temp &= ~SM_MAILBOX_INT_MASK;
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, 
		      MUIF_INBND_INTERRUPT_MASK_PCI0, temp);
    intUnlock (oldVal);
    
    return (OK);
    }

/*******************************************************************************
*
* sysMailboxDisable - disable the in-bound mailbox interrupt
*
* This routine disables an operating mode dependent in-bound mailbox interrupt.
*
* RETURNS: The results of sysDec2155xIntDisable or sysMpicIntDisable.
*
* SEE ALSO: sysMailboxConnect(), sysMailboxEnable()
*/

STATUS sysMailboxDisable
    (
    char *mailboxAdrs           /* address of mailbox (ignored) */
    )
    {
    UINT32  temp;

    /* mask this one particular doorbell register */

    temp = MV64360_READ32 (MV64360_REG_BASE, MUIF_INBND_INTERRUPT_MASK_PCI0);
    temp |= SM_MAILBOX_INT_MASK;
    MV64360_WRITE32_PUSH (MV64360_REG_BASE, 
		      MUIF_INBND_INTERRUPT_MASK_PCI0, temp);
    
    return (OK);
    }

#endif /* INCLUDE_SM_COMMON */

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

#ifdef INCLUDE_DEC2155X
/*******************************************************************************
*
* prpmcDec2155xWaitConfig - Primary side of Dec2155x to be configured
*
* This routine holds the PrPMC until the Dec2155x primary side bus mastering
* is enabled
*
* RETURNS: NA
*/

void prpmcDec2155xWaitConfig (void)
    {
    int  timeout = PRPMC_DRAWBRIDGE_TIMEOUT;
    int  msgWaitCnt = 0;

    while (timeout != 0)
	{
        if (sysDec2155xPrimaryBusMaster() == TRUE)
	    {

	    /* Primary side bus master is enabled so it's configured */
	    
	    break;
	    }

	if ((timeout % 2000) == 0)
	    {
	    sysDebugMsg("Waiting for Dec2155x primary configuration\r\n",
	                 CONTINUE_EXECUTION); 
            msgWaitCnt++;
            sysUsDelay(1000);			/* Delay 1 mSec */
	    }

            --timeout;
        if (timeout == 0)
            break;

        }

    /* If configuration timeout, report the error but continue executing. */
#if (PRPMC_DRAWBRIDGE_TIMEOUT != PRPMC_DRAWBRIDGE_INFINITE_TIMEOUT)
    if (timeout <= 0)
	{
        sysDebugMsg 
            ("Timed out waiting for Dec2155x Primimary configuration\n\r",
              CONTINUE_EXECUTION);
	}
#endif
    if ((msgWaitCnt != 0) && !(timeout <= 0)) 
	{
         sysDebugMsg
	   ("Resuming after system Dec2155x primary configuration.\r\n\n",
	     CONTINUE_EXECUTION); 
        }
    }
#endif /* INCLUDE_DEC2155X */
