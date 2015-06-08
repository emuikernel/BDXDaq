/* sysLib.c - Motorola PrPMC800 board series system-dependent library */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
02f,17jul02,dtr  Change INCLUDE_HARRIER_AUXCLK to INCLUDE_AUX_CLK.
02e,13may02,dtr  Update for SM for Tornado 2.2. SPR 73457.
02d,31jan02,kab  Remove obsolete vmxExcLoad
02c,17jan02,dtr  Fixing diab warnings.
02b,05dec01,dtr  Wrapping dec2155x functions with ifdef.
02a,25nov01,dtr  Tidy up.
01z,16nov01,scb  Shared memory fixes using mcpn765 as carrier.
01y,10oct01,scb  Fixes for shared memory support.
01x,28sep01,srr  Removed sysEnd.c and cleaned up #include alignment.
01w,17sep01,srr  Removed erroneous INCLUDE_SHOW_ROUTINES.
01v,31jul01,srr  Added 82543 Gigabit Ethernet support.
01u,02jul01,scb  Fix bugs in sysBusToLocalAdrs() and sysLocalToBusAdrs().
01t,18jun01,srr  Added Harrier DMA support.
01s,14jun01,srr  Change address for Slave to wait on host to be volatile.
01r,12jun01,srr  Updated routine for Slave to wait on host enumeration.
01q,17may01,blk  Add support for harrier II.
01p,07dec00,krp  Added support for Watchdog Timer
01o,17nov00,dmw  Added slave Ethernet support.
01n,14nov00,dmw  Added slave PCI initialization.
01m.27oct00,dmw  Added Xport MMU support and fixed sysSysconAsserted.
01l,16oct00,dmw  Enabled PCI bridge init functions.
01k,10oct00,dmw  Added Nitro IDs.
01j,09oct00,dmw  Changed sysPhysMemDesc for MMU support.
01i,08oct00,dmw  Stubbed sysHarrierPhbInit for board bring up.
01h,18sep00,dmw  Removed dependancy on PRPMC800 define.
01g,18sep00,krp  Added support for Harrier MPIC support
01f,12sep00,dmw  Corrected board fail bit.
01e,11sep00,dmw  Changed sense of sysSysconAsserted (for Harrier).
01d,08sep00,dmw  Added dual-address I2C EEPROM accesses.
01c,07sep00,dmw  Updated I2C access function names.
01b,01aug00,dmw  Removed calls to sysNetHwInit and sysNetHwInit2.
01a,01aug00,dmw  Written (from version 01g of mcpn765/sysLib.c).
*/

/*
DESCRIPTION
This library provides board-specific routines.  The chip drivers included are:

    i8250Sio.c      - Intel 8250 UART driver 
    ppcDecTimer.c   - PowerPC decrementer timer library (system clock)
    byteNvRam.c     - byte-oriented generic non-volatile RAM library
    pciConfigLib.c  - PCI configuration library
    dec2155xCpci.c  - Dec/Intel non-transparent PCI-to-PCI bridge library
    HarrierAuxClk.c - Harrier timer library (auxiliary clock)
    ataDrv.o        - ATA/EIDE interface driver
    sysMpic.c       - Mpic Interrupt Controller

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
#include "harrier.h"

#ifdef INCLUDE_DEC2155X
#   include "dec2155xCpci.h"
#endif

#include "drv/pci/pciAutoConfigLib.h"

#include "end.h"
#ifdef INCLUDE_ALTIVEC
#include "altivecLib.h"
IMPORT int       (* _func_altivecProbeRtn) () ;
#endif /* INCLUDE_ALTIVEC */

/* defines */

#define ZERO    0

#define DEFAULT_TAS_CHECKS    10        /* rechecks for soft tas */
#define TAS_CONST             0x80

#ifdef INCLUDE_BPE

#   define _PPC_HID0_DBP _PPC_HID0_ECPC

#endif /* INCLUDE_BPE */

#ifdef INCLUDE_DPM    /* dynamic power management */

#   define _PPC_HID0_BIT_DPM    11 /* dynamic power management bit */
#   define _PPC_HID0_DPM        (1<<(31-_PPC_HID0_BIT_DPM))

#endif /* INCLUDE_DPM */

#define MONARCH_HARRIER_PCI_BUS_NUMBER		0
#define MONARCH_HARRIER_PCI_DEV_NUMBER		0
#define MONARCH_HARRIER_PCI_FCN_NUMBER		0

#define HARRIER_RDWR_ENA        (CPU2PCI_ATTR_REN | CPU2PCI_ATTR_WEN)

#define SYS_DECS_DRAM_INDEX     1

/* structures */

typedef struct mailboxInfo
    {
    BOOL    connected;
    FUNCPTR routine;
    int     arg;
    } MAILBOX_INFO;

typedef struct harrierWinStruct
    {
    UINT32 winType;   /* mem or i/o */
    UINT32 winBase;   /* start of window */
    UINT32 winLimit;  /* end of window */
    } HARRIER_WIN_STRUCT;

typedef struct harrierOffsetsOutbound
    {
    UINT32 range;
    UINT32 offset;
    UINT32 attr;
    } HARRIER_OFFSETS_OUTBOUND;

typedef struct harrierOffsetsInbound
    {
    UINT32 base;
    UINT32 size;
    UINT32 offset;
    UINT32 attr;
    } HARRIER_OFFSETS_INBOUND;

HARRIER_OFFSETS_OUTBOUND    sysHarrierCpuWinOff [] =
    {
        {
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_0_REG),
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_0_REG + 
                     HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET),
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_0_REG +
                     HARRIER_OUTBOUND_TRANSLATION_ATTRIBUTE_OFFSET)
        },

        {
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_1_REG),
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_1_REG +
                     HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET),
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_1_REG + 
                     HARRIER_OUTBOUND_TRANSLATION_ATTRIBUTE_OFFSET)
        },
    
        {
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_2_REG),
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_2_REG +
                     HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET),
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_2_REG +
                     HARRIER_OUTBOUND_TRANSLATION_ATTRIBUTE_OFFSET)
        },
    
        {
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_3_REG),
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_3_REG + 
                     HARRIER_OUTBOUND_TRANSLATION_OFFSETINFO_OFFSET),
        (HARRIER_OUTBOUND_TRANSLATION_ADDR_3_REG +
                     HARRIER_OUTBOUND_TRANSLATION_ATTRIBUTE_OFFSET)
        }

    };

HARRIER_OFFSETS_INBOUND    sysHarrierPciWinOff [] =
    {

        {
        (HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_0), 
        (HARRIER_INBOUND_TRANSLATION_SIZE_0_REG), 
        (HARRIER_INBOUND_TRANSLATION_OFFSET_0_REG), 
        (HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_0)
        },

        {
        (HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_1), 
        (HARRIER_INBOUND_TRANSLATION_SIZE_1_REG), 
        (HARRIER_INBOUND_TRANSLATION_OFFSET_1_REG), 
        (HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_1)
        },

        {
        (HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_2), 
        (HARRIER_INBOUND_TRANSLATION_SIZE_2_REG), 
        (HARRIER_INBOUND_TRANSLATION_OFFSET_2_REG), 
        (HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_2)
        },

        {
        (HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_3), 
        (HARRIER_INBOUND_TRANSLATION_SIZE_3_REG), 
        (HARRIER_INBOUND_TRANSLATION_OFFSET_3_REG), 
        (HARRIER_INBOUND_TRANSLATION_ATTRIBUTE_3)
        }
    };

#define HARRIER_CPU_WIN_CNT \
        (sizeof (sysHarrierCpuWinOff)/ sizeof (HARRIER_OFFSETS_OUTBOUND))
#define HARRIER_PCI_WIN_CNT \
        (sizeof (sysHarrierPciWinOff)/ sizeof (HARRIER_OFFSETS_INBOUND))
#define HARRIER_WIN_CNT (HARRIER_CPU_WIN_CNT + HARRIER_PCI_WIN_CNT)

/* globals */

UINT    mpicBaseAdrs;
static char * sysPhysMemSize = NULL;    /* ptr to top of mem + 1 */

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

UINT32 sysBatDesc [2 * (_MMU_NUM_IBAT + _MMU_NUM_DBAT)] =
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
 */

PHYS_MEM_DESC sysPhysMemDesc [] =
    {
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

    {
    /* 
     * Local DRAM - Must be SYS_DECS_DRAM_INDEX entry in sysPhysMemDesc for 
     * Auto Sizing 
     */

    (void *) RAM_LOW_ADRS,
    (void *) RAM_LOW_ADRS,
    LOCAL_MEM_SIZE -  RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    /*
     * Access to PCI ISA memory space.
     */

    {
    (void *) PCI_MSTR_MEMIO_LOCAL,
    (void *) PCI_MSTR_MEMIO_LOCAL,
    PCI_MSTR_MEMIO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /*
     * Access to PCI memory space.
     * Note: Guarding should not be required since
     * memory is well-behaved (no side-effects on read or write)
     */

    {
    (void *) PCI_MSTR_MEM_LOCAL,
    (void *) PCI_MSTR_MEM_LOCAL,
    PCI_MSTR_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT
    },

    /*
     * Access to PCI memory space at PCI address zero.
     * This will allow the PrPMC to access DRAM on the processor board
     * on which the PrPMC is mounted if that processor board maps its
     * DRAM to PCI address zero.
     * Note: Guarding should not be required since
     * memory is well-behaved (no side-effects on read or write)
     */

    {
    (void *) (PCI_MSTR_ZERO_LOCAL),
    (void *) (PCI_MSTR_ZERO_LOCAL),
    PCI_MSTR_ZERO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) PRPMC800_XPORT0_ADDR,
    (void *) PRPMC800_XPORT0_ADDR,
    PRPMC800_XPORT0_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) PRPMC800_XPORT1_ADDR,
    (void *) PRPMC800_XPORT1_ADDR,
    PRPMC800_XPORT1_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

#ifdef  INCLUDE_PRPMC800XT    
    {
    (void *) PRPMC800_XPORT2_ADDR,
    (void *) PRPMC800_XPORT2_ADDR,
    PRPMC800_XPORT2_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },
 
    {
    (void *) PRPMC800_XPORT3_ADDR,
    (void *) PRPMC800_XPORT3_ADDR,
    PRPMC800_XPORT3_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },
#endif /* INCLUDE_PRPMC800XT */

    {
    /* MPIC Regs */

    (void *) MPIC_BASE_ADRS,
    (void *) MPIC_BASE_ADRS,
    MPIC_REG_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /* Access to PCI ISA I/O space */

    {
    (void *) ISA_MSTR_IO_LOCAL,
    (void *) ISA_MSTR_IO_LOCAL,
    ISA_MSTR_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /* Access to PCI I/O space */

    {
    (void *) PCI_MSTR_IO_LOCAL,
    (void *) PCI_MSTR_IO_LOCAL,
    PCI_MSTR_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID    | VM_STATE_WRITABLE     | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    {
    (void *) HARRIER_XCSR_BASE,
    (void *) HARRIER_XCSR_BASE,
    HARRIER_XCSR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },
    /*
     * Apart from DRAM, virtual = physical addresses.
     *
     * ROM (in fact, Bank 1 of the Flash memory used as ROM)
     *
     * ROM is normally marked as uncacheable by VxWorks. We leave it
     * like that for the time being, even though this has a severe impact
     * on execution speed from ROM.
     *
     * In order to use the Flash as NVRAM, the Flash area must be
     * writable.  Strictly speaking, only the first and last pages need
     * be marked as writable.
     */
#ifdef INCLUDE_TFFS
    {
    (void *) FLASH_BASE_ADRS, 
    (void *) FLASH_BASE_ADRS,
    SIZE_32MB,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },
#endif
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
UINT   sysVectorIRQ0  = INT_VEC_IRQ0;/* vector for IRQ0 */

HARRIER_WIN_STRUCT sysHarrierCpuToPciWin[HARRIER_WIN_CNT] = { { 0 } };
HARRIER_WIN_STRUCT sysHarrierPciToCpuWin[HARRIER_WIN_CNT] = { { 0 } };
int   sysValidHarrierWindows = 0;    /* number of valid entries */

#ifdef INCLUDE_SM_COMMON
    MAILBOX_INFO sysMailbox = {FALSE, NULL, 0};
#endif

UINT32 sysProbeFault = 0;            /* used by dsi exception trap handler */
BOOL   sysMonarchMode = FALSE;       /* TRUE if Monarch */

#ifdef INCLUDE_DEC2155X
    STATUS sysPciConfig21554InLong (int, int, int, int, UINT32 *);
    void   prpmcDec2155xWaitConfig (void);
#endif

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
void    sysModeCheck (void);
char *  sysPhysMemTop (void);
STATUS  sysPciExtHarrierInit (int, int, int);
UCHAR   sysNvRead (ULONG);
void    sysNvWrite (ULONG,UCHAR);
STATUS  sysBusProbe (char *, int, int, char *);
void    sysUsDelay (UINT);
void    sysConfigBpe (void);
void    sysConfigDpm (void);
void    reportBootromErrors (void);
BOOL    sysSysconAsserted (void);
void    sysDelay (void);
LOCAL   void    sysHarrierCapt (void);
void    sysPciInsertLong (UINT32, UINT32, UINT32);
void    sysPciInsertWord (UINT32, UINT16, UINT16);
void    sysPciInsertByte (UINT32, UINT8, UINT8);
void    sysPciOutLongConfirm (UINT32, UINT32);
void    sysPciOutWordConfirm (UINT32, UINT16);
void    sysPciOutByteConfirm (UINT32, UINT8);
UCHAR   inboundSizeCode (UINT32 dramSize);
void    prpmcSlaveWaitConfig (void);


#ifdef INCLUDE_BPE
    void  sysConfigBpe (void);
#endif

#ifdef INCLUDE_DPM
    void  sysConfigDpm (void);
#endif

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

IMPORT UCHAR  sysInByte (ULONG);
IMPORT void   sysOutByte (ULONG, UCHAR);
IMPORT UINT16 sysIn16 (UINT16 *);
IMPORT void   sysOut16 (UINT16 *, UINT16);
IMPORT UINT32 sysIn32 (UINT32 *);
IMPORT void   sysOut32 (UINT32 *, UINT32);
IMPORT void   sysPciRead32 (UINT32, UINT32 *);
IMPORT void   sysPciWrite32 (UINT32, UINT32);
IMPORT void   sysClkIntCIO (void);
IMPORT STATUS sysMemProbeSup (int length, char * src, char * dest);
IMPORT int    sysProbeExc();
IMPORT VOIDFUNCPTR      smUtilTasClearRtn;
IMPORT UINT32 sysTimeBaseLGet (void);
IMPORT UINT   sysHid1Get (void);
IMPORT UCHAR  sysProductStr[];
IMPORT STATUS sysMotI2cRead (UCHAR, UCHAR, UCHAR *, UCHAR);
IMPORT STATUS sysMotI2cWrite (UCHAR, UCHAR, UCHAR *, UCHAR);
IMPORT UINT32 sysDramSize (void);
IMPORT END_TBL_ENTRY endDevTbl[];
IMPORT STATUS harrierDmaInit (UINT32, UINT32 *);

/* BSP DRIVERS */

#include "pci/pciConfigLib.c"

#ifdef INCLUDE_NETWORK
#   include "./sysNet.c"
#   ifdef INCLUDE_END
#      include "./sysGei82543End.c"
#      include "./sysFei82557End.c"
#      include "./sysDec21x40End.c"
#   endif /* INCLUDE_END */
#endif /* INCLUDE_NETWORK */

#include "sysSerial.c"
#include "mem/byteNvRam.c"
#include "sysMotVpd.c"
#include "timer/ppcDecTimer.c"       /* PPC603 & 604 have on chip timers */

#ifdef INCLUDE_SHOW_ROUTINES
#   include "pci/pciConfigShow.c"    /* display of PCI config space */
#   include "sysMotVpdShow.c"
#   include "harrierSmcShow.c"
#endif /* INCLUDE_SHOW_ROUTINES */

#include "sysBusPci.c"

#include "sysCache.c"

#include "harrierPhb.c"

#include "harrierMpic.h"
#include "sysMpic.c"

#ifdef INCLUDE_AUX_CLK
#   include "harrierAuxClk.c"
#endif  /* INCLUDE_AUX_CLK */

#ifdef INCLUDE_HARRIER_DMA
#   include "harrierDma.c"
#endif  /* INCLUDE_HARRIER_DMA */

/* M48t59 watchdog timer support. */
 
#ifdef INCLUDE_M48T559WDT
#   include "m48tWdt.c"
#endif

#ifdef INCLUDE_DEC2155X
#   include "dec2155xCpci.c"
#endif

#if (CARRIER_TYPE == PRPMC_G)
#   include "w83601Gpio.h"
#   include "w83782HwMon.h"
#endif /* (CARRIER_TYPE == PRPMC_G) */

#if     defined (INCLUDE_ALTIVEC)
/*******************************************************************************
*
* sysAltivecProbe - Check if the CPU has ALTIVEC unit.
*
* This routine returns OK it the CPU has an ALTIVEC unit in it.
* Presently it checks for 7400
* RETURNS: OK  -  for 7400 Processor type
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
          case CPU_TYPE_NITRO:
          case CPU_TYPE_MAX:
             altivecUnitPresent = OK;
            break;

          default:
             break;
         }      /* switch  */

    return (altivecUnitPresent);
    }
#endif  /* INCLUDE_ALTIVEC */

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
        case CPU_TYPE_NITRO:
            sprintf(cpuStr, "7410");
            break;
        case CPU_TYPE_MAX:
            if (CPU_REV < CPU_REV_NITRO)
                sprintf(cpuStr, "7400");
            else
                sprintf(cpuStr, "7410");
            break;
        default:
            sprintf (cpuStr, "60%d", cpu);
            break;
        }

    sprintf (sysModelStr, "Motorola %s- MPC %s", sysProductStr, cpuStr);

    return (sysModelStr);
    }

/******************************************************************************
*
* sysBspRev - return the BSP version and revision number
*
* This routine returns a pointer to a BSP version and revision number, for
* example, 1.1/0. BSP_REV is concatenated to BSP_VERSION and returned.
*
* RETURNS: A pointer to the BSP version/revision string.
*/

char *
sysBspRev (void)
{
  return (BSP_VERSION BSP_REV);
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
  /*#ifdef PCI_AUTO_DEBUG*/
  char txt[100];
  /*#endif*/

  /* Determine operating mode */
  sysMonarchMode = sysSysconAsserted ();
#ifdef PCI_AUTO_DEBUG
  sprintf(txt,"sysHwInit: sysMonarchMode=%d\r\n",sysMonarchMode);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  /*
   * If mmu tables are used, this is where we would dynamically
   * update the entry describing main memory, using sysPhysMemTop().
   * We must call sysPhysMemTop () at sysHwInit() time to do
   * the memory autosizing if available.
   */
  sysPhysMemTop ();

  /* Do the Harrier PCI-Host Bridge phase 1 init */
  (void) sysHarrierPhbInit ();

  /* Initialize the VPD information */
  (void) sysVpdInit ();

  /* Do the Harrier PCI-Host Bridge phase 2 init */

/*boy*/(void) sysHarrierPhbInit2 ();

  /* Validate CPU type */
  sysCpuCheck();
    
  /*  Report the operating mode. */
  sysModeCheck();

  /*
   * If MPC7400 (Max):
   *     Setup  exception addresses.
   *     Disable & invalidate if L2 enabled.
   */
  if(CPU_TYPE == CPU_TYPE_MAX || CPU_TYPE == CPU_TYPE_NITRO)
  {
#if defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2)
    sysL2CacheDisable ();
#endif 
  }
  

  /* Initialize PCI driver library. */
  if(pciConfigLibInit (PCI_MECHANISM_1, PCI_PRIMARY_CAR, PCI_PRIMARY_CDR, 0)
     != OK)
  {
    sysDebugMsg ("!!! ERROR in pciConfigLibInit() !!!\r\n", CONTINUE_EXECUTION);
    sysToMonitor (BOOT_NO_AUTOBOOT);
  }

  /* Initialize the extended portion of the Harrier's PCI Header. */
  /*boy*/(void)sysHarrierInitPhbExt (); /* Sergey: wait for EREADY inside ! */

#ifdef INCLUDE_DEC2155X
  /* Configure the Dec 2155x setup registers if we are the PCI Host */
  if(sysMonarchMode) sysDec2155xInit ();
#endif

  if(sysMonarchMode)
  {
    /* perform PCI configuration */
    sysDebugMsg ("==> We are in Monarch mode\r\n", CONTINUE_EXECUTION);

    if( !PCI_AUTOCONFIG_DONE )
    {
      /* in ROM boot phase, OK to continue and configure PCI busses. */

	  /*sysDebugMsg ("==> Configure PCI busses ..\r\n", CONTINUE_EXECUTION);*/
      sysPciAutoConfig();
      /*sysDebugMsg ("==> .. done.\r\n", CONTINUE_EXECUTION);*/

      /* Remember that PCI is configured */

      PCI_AUTOCONFIG_FLAG = TRUE;     
    }

  }
  else
  {
        
    sysDebugMsg ("==> We are in Slave mode\r\n", CONTINUE_EXECUTION);

#ifndef SLAVE_OWNS_ETHERNET

    /* Disable End interface for slaves */
    endDevTbl[0].endLoadFunc = END_TBL_END; 

#endif /* SLAVE_OWNS_ETHERNET */

    /* Wait for Host to enumerate my harrier */
    prpmcSlaveWaitConfig();
  }

  /* Now fix up the input translation offsets for Harrier, if necessary */
  (void) sysHarrierFixInp ();


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
   *  Initialize the non-PCI Config Space registers of the
   *  Harrier Mpic.
   */
  sysMpicInit();

  /* set pointer to bus probing hook */
  _func_vxMemProbeHook = (FUNCPTR)sysBusProbe;

  /* Initialize COM1 serial channel */
  sysSerialHwInit();

  /* clear Harrier error conditions */
  sysHarrierErrClr ();

  /* Upon completion, clear BFL (Board Fail) LED */
  *(UINT32 *)HARRIER_MISC_CONTROL_STATUS_REG &= ~HARRIER_MCSR_BRDFL;


#ifdef MVME5500_DEBUG
  sysDebugMsg ("sysHwInit done.\r\n", CONTINUE_EXECUTION);
#endif
  
  {
    int ii, ret, a1, a2, a3, a4;
    volatile UINT32 dummy;
    char txt[1000];
	
#ifdef MVME5500_DEBUG
    for(ii=0; ii<20; ii++)
    {
      ret = pciConfigInLong(0,ii,0,0x10,&a1);
      ret |= pciConfigInLong(0,ii,0,0x14,&a2);
      ret |= pciConfigInLong(0,ii,0,0x18,&a3);
      ret |= pciConfigInLong(0,ii,0,0x3c,&a4);

	  /*
      if(a4==0x38080149) pciConfigOutLong(0,ii,0,0x3c,0x38080119);
	  */

/*#ifdef PCI_AUTO_DEBUG*/
      sprintf(txt,"[0x%02x] ===> ret=%d, bars=0x%08x 0x%08x 0x%08x 0x%08x\r\n",
        ii,ret,a1,a2,a3,a4);
      sysDebugMsg(txt,CONTINUE_EXECUTION);
/*#endif*/
    }
#endif

/*
pciHeaderShow(0,0x11,0);
pciHeaderShow(0,0x10,0);
pciHeaderShow(0,0x0,0);
*/

#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"Outbound registers:\r\n");
	/*
    sprintf(txt,"0x%08x\r\n",LONGSWAP(*(volatile UINT32 *) 0xfeff0220);
	*/
    dummy = sysInLong (0xfeff0220);
    sprintf(txt,"dummy(0xfeff0220)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0224);
    sprintf(txt,"dummy(0xfeff0224)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0228);
    sprintf(txt,"dummy(0xfeff0228)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff022c);
    sprintf(txt,"dummy(0xfeff022c)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0230);
    sprintf(txt,"dummy(0xfeff0230)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0234);
    sprintf(txt,"dummy(0xfeff0234)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0238);
    sprintf(txt,"dummy(0xfeff0238)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff023c);
    sprintf(txt,"dummy(0xfeff023c)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0248);
    sprintf(txt,"dummy(0xfeff0248)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);

    sprintf(txt,"Inbound registers:\r\n");
    dummy = sysInLong (0xfeff0348);
    sprintf(txt,"dummy(0xfeff0348)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff034c);
    sprintf(txt,"dummy(0xfeff034c)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0350);
    sprintf(txt,"dummy(0xfeff0350)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0354);
    sprintf(txt,"dummy(0xfeff0354)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0358);
    sprintf(txt,"dummy(0xfeff0358)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff035c);
    sprintf(txt,"dummy(0xfeff035c)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0360);
    sprintf(txt,"dummy(0xfeff0360)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0364);
    sprintf(txt,"dummy(0xfeff0364)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0314);
    sprintf(txt,"dummy(0xfeff0314)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0318);
    sprintf(txt,"dummy(0xfeff0318)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff031c);
    sprintf(txt,"dummy(0xfeff031c)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xfeff0320);
    sprintf(txt,"dummy(0xfeff0320)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
	/* read local bridge registers */


#endif



#ifdef MVME5500
/* Sergey: those registers are set Ok now; maybe problem was because of
a lot of debug messages were printed out in mvme5500 so boot process of
mvme5500 was slow, and prpmc800 was ahead of time */
    /**(volatile UINT32 *)0xfeff0220 = 0x8300c37f;*/ /* did not set by BSP ... */
    /**(volatile UINT32 *)0xfeff0230 = 0xc380c3bf;*/ /* did not set by BSP ... */
    /**(volatile UINT32 *)0xfeff0234 = 0x3c800782;*/ /* did not set by BSP ... */

#ifdef PCI_AUTO_DEBUG
    dummy = sysInLong (0xfeff0220);
    sprintf(txt,"dummy(0xfeff0220)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

#endif



#ifdef PCI_AUTO_DEBUG
    dummy = sysInLong (0x9000000c);
    sprintf(txt,"dummy(0x9000000c)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInWord (0x9000000e);
    sprintf(txt,"dummy(0x9000000e)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#ifndef MVME5500
    dummy = sysInLong (0xa000000c);
    sprintf(txt,"dummy(0xa000000c)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInWord (0xa000000e);
    sprintf(txt,"dummy(0xa000000e)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInLong (0xa004000c);
    sprintf(txt,"dummy(0xa004000c)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
    dummy = sysInWord (0xa004000e);
    sprintf(txt,"dummy(0xa004000e)=0x%08x\r\n",dummy);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif
#endif


  }

}

/*******************************************************************************
*
* sysDramSize - returns the real top of local DRAM.
*
* RETURNS: The address of the top of DRAM.
*
*/

UINT32 sysDramSize (void)
{
  UINT32        dramIndex;
  int           i;
  UINT32        localDram = 0;

  static UINT32 dramSize[] =
        {
        0x00000000,             /*  0:    0MB          */
        0x02000000,             /*  1:   32MB (4Mx16)  */
        0x04000000,             /*  2:   64MB (8Mx8)   */
        0x04000000,             /*  3:   64MB (8Mx16)  */
        0x08000000,             /*  4:  128MB (16Mx4)  */
        0x08000000,             /*  5:  128MB (16Mx8)  */
        0x08000000,             /*  6:  128MB (16Mx16) */
        0x10000000,             /*  7:  256MB (32Mx4)  */
        0x10000000,             /*  8:  256MB (32Mx8)  */
        0x10000000,             /*  9:  256MB (32Mx16) */
        0x20000000,             /* 10:  512MB (64Mx4)  */
        0x20000000,             /* 11:  512MB (64Mx8)  */
        0x20000000,             /* 12:  512MB (64Mx16) */
        0x40000000,             /* 13: 1024MB (128Mx4) */
        0x40000000,             /* 14: 1024MB (128Mx8) */
        0x80000000,             /* 15: 2048MB (256Mx4) */
        };

    static UINT32 *dramAttr[] =
        {
        (UINT32 *) HARRIER_REG_SDRAM_BLOCK_ADDRESSING_A,
        (UINT32 *) HARRIER_REG_SDRAM_BLOCK_ADDRESSING_B,
        (UINT32 *) HARRIER_REG_SDRAM_BLOCK_ADDRESSING_C,
        (UINT32 *) HARRIER_REG_SDRAM_BLOCK_ADDRESSING_D,
        (UINT32 *) HARRIER_REG_SDRAM_BLOCK_ADDRESSING_E,
        (UINT32 *) HARRIER_REG_SDRAM_BLOCK_ADDRESSING_F,
        (UINT32 *) HARRIER_REG_SDRAM_BLOCK_ADDRESSING_G,
        (UINT32 *) HARRIER_REG_SDRAM_BLOCK_ADDRESSING_H
        };

    /*
     * Since Harrier memory controller chip has already been set to
     * control all memory, just read and interpret its DRAM Attributes
     * Registers.
     */

    for (i = 0; i < NELEMENTS(dramAttr); i++)
    {
      if (*dramAttr[i] & (UINT32)HARRIER_SDBA_ENB)
      {
        dramIndex = (*dramAttr[i] & (UINT32)HARRIER_SDBA_SIZE_MASK) >> 16;
        localDram += dramSize [dramIndex];
      }
    }

  return(localDram);
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

char *
sysPhysMemTop (void)
{
  UINT32 localDram = 0;

  if (sysPhysMemSize == NULL)
  {
    localDram = sysDramSize();

#ifdef LOCAL_MEM_AUTOSIZE
    sysPhysMemSize = (char *)localDram;
#else /* not LOCAL_MEM_AUTOSIZE, use defined constants. */
    sysPhysMemSize = (char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);
#endif /* LOCAL_MEM_AUTOSIZE */

    /* Adjust initial DRAM size to actual physical memory. */

    sysPhysMemDesc[SYS_DECS_DRAM_INDEX].len = 
            (ULONG)sysPhysMemSize - 
            (ULONG)sysPhysMemDesc[SYS_DECS_DRAM_INDEX].physicalAddr;

  }

  return(sysPhysMemSize);
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
    FUNCPTR pRom = (FUNCPTR) (ROM_TEXT_ADRS + 8); /* Warm reboot (for Max) */

#if (defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2))
    sysL2CacheDisable ();    /* Disable the L2 Cache */
#endif
#if defined(INCLUDE_CACHE_SUPPORT)
    cacheDisable (0);       /* Disable the Instruction Cache */
    cacheDisable (1);       /* Disable the Data Cache */
#endif
    sysSerialReset ();        /* reset serial devices */
    sysHarrierErrClr ();        /* clear Harrier error conditions */

    /* Clear the MSR */

    vxMsrSet (0);

    /* Turn on the board fail light */

    *(UINT32 *)HARRIER_MISC_CONTROL_STATUS_REG |= HARRIER_MCSR_BRDFL;

    WRS_ASM("sync");/* Ensure all data transfers and instructions completed */

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

    /* Int connects for various devices */

    if (!configured)
        {

#ifdef INCLUDE_AUX_CLK
        sysAuxClkInit ();
        intConnect (INUM_TO_IVEC(TIMER0_INT_VEC), sysAuxClkInt, 0);
        intEnable (TIMER0_INT_LVL);
#endif /* INCLUDE_AUX_CLK */

#ifdef INCLUDE_HARRIER_DMA
	harrierDmaInit(0, (UINT32 *)HARRIER_XCSR_BASE);
#endif /* INCLUDE_HARRIER_DMA */

        /* initialize serial interrupts */

        sysSerialHwInit2();

        /* capture harrier window configuration */

        sysHarrierCapt ();

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

#ifdef INCLUDE_BPE

        /* enable processor data and address bus parity checking */

        sysConfigBpe ();

#endif

#ifdef INCLUDE_DPM

        /* enable dynamic power management */

        sysConfigDpm ();

#endif

#ifdef INCLUDE_CACHE_L2
#ifdef INCLUDE_CACHE_SUPPORT
#ifdef USER_L2_CACHE_ENABLE
        /* initialize the L2 cache */

        sysL2CacheInit();
#else   /* USER_L2_CACHE_ENABLE */
        sysL2CacheDisable();
#endif  /* USER_L2_CACHE_ENABLE */
#endif  /* INCLUDE_CACHE_SUPPORT */
#endif  /* INCLUDE_CACHE_L2 */

#if (CARRIER_TYPE == PRPMC_G)
	/* Initialize the gigabit chip */

	sys543PciInit ();
#endif /* (CARRIER_TYPE == PRPMC_G) */

#if     defined (INCLUDE_ALTIVEC)
       _func_altivecProbeRtn = sysAltivecProbe;
#endif  /* INCLUDE_ALTIVEC */

        /* report any errors detected before they could be reported. */

        reportBootromErrors ();

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
* should be unique on a single backplane.  It also maps local resources onto
* the Compact PCI Bus.
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
* sysHarrierCapt - capture Harrier window information
*
* This routine captures the configuration of the Harrier PPC and PCI slave
* registers. This information is used to perform address translations from
* CPU to PCI addresses and vice versa.  
*
* RETURNS: N/A
*
* SEE ALSO: sysBusToLocalAdrs(), sysLocalToBusAdrs(), sysCpuToPciAdrs(),
* sysPciToCpuAdrs(), and sysHarrierTransAdrs().
*/

LOCAL void sysHarrierCapt (void)
    {
    UINT32 index;       /* window counter   */
    UINT32 attr;        /* window attribute */
    UINT32 range;       /* working variable */
    UINT32 size;	/* working variable */
    UINT32 base;	/* working variable */
    UINT32 offset;	/* working variable */
    UINT16 trans;        /* window translation value */
    HARRIER_OFFSETS_OUTBOUND * pHarrierOffO; /* ptr to harrier addr/offsets */
    HARRIER_OFFSETS_INBOUND * pHarrierOffI; /* ptr to harrier addr/offsets */
    HARRIER_WIN_STRUCT * pHarrierCpu; /* pointer to cpu windows */
    HARRIER_WIN_STRUCT * pHarrierPci; /* poiner to pci windows */

    /* Two arrays are initialized:
     * sysHarrierPciToCpuWin[] and sysHarrierCpuToPciWin[].  Each
     * array element is of type HARRIER_WIN_STRUCT which consists
     * of three fields representing type, base address and limit
     * address.  Harrier inbound translation windows are queried
     * first and an associated sysHarrierPciToCpuWin[] entry is
     * filled in.  With PCI-based type, base and limit entries.
     * For each sysHarrierCpuToPciWin[] entry made an  associated 
     * sysHarrierCpuToPciWin[] will be  made at the same array index
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
     * entries again made first in sysHarrierCpuToPciWin[] and
     * next in sysHarrierPciToCpuWin[].  
     */

    /* initialize number of valid windows found */

    sysValidHarrierWindows = 0;

    /* point to window save arrays */

    pHarrierCpu = &sysHarrierCpuToPciWin[0];
    pHarrierPci = &sysHarrierPciToCpuWin[0];

    /* start with the cpu to pci windows (ppc slaves) */

    pHarrierOffO = &sysHarrierCpuWinOff[0];

    /* loop through each window */

    for(index = 0; index < HARRIER_CPU_WIN_CNT; index++)
    {

        /* read the window attributes */

        attr = * (UINT16 *) (pHarrierOffO->attr);

        attr &= HARRIER_PCFS_OTAT_ENA_MASK;

        if (attr & HARRIER_PCFS_OTAT_ENA)
        {

            /* active window found, bump valid window counter */

            sysValidHarrierWindows++;

            /* determine the window type (memory or i/o) */

            pHarrierCpu->winType = 
                (attr & HARRIER_PCFS_OTAT_MEM) ? PCI_BAR_SPACE_MEM :
                 PCI_BAR_SPACE_IO;

	    /*
	     * Outbound window 3 requires special handling since it is
	     * always I/O space.
             */

	    if (pHarrierOffO->range == HARRIER_OUTBOUND_TRANSLATION_ADDR_3_REG)
	        pHarrierCpu->winType = PCI_BAR_SPACE_IO;

            pHarrierPci->winType = pHarrierCpu->winType;

            /* read the window range */

            range =  * (UINT32 *) (pHarrierOffO->range);
            trans = * (UINT16 *) (pHarrierOffO->offset);

            pHarrierCpu->winBase = range & ~0xffff;
            pHarrierCpu->winLimit = (range << 16) | 0xffff;

            /* calculate translated values */

            pHarrierPci->winBase = pHarrierCpu->winBase + (trans << 16);
            pHarrierPci->winLimit = pHarrierCpu->winLimit + (trans << 16);

            /* advance in preparation for next valid window */

            pHarrierCpu++;
            pHarrierPci++;
            }

        /* advance to next set of harrier offsets */

        pHarrierOffO++;
        }

    /* switch to harrier pci to cpu windows (pci slaves) */

    pHarrierOffI = &sysHarrierPciWinOff[0];

    /* loop through each window */

    for (index = 0; index < HARRIER_PCI_WIN_CNT; index++)
        {

        /* read the window attributes */

        attr = * (UINT32 *) (pHarrierOffI->attr);

        if (attr & HARRIER_ITAT_ENA)
            {

            /* active window found, bump valid window counter */

            sysValidHarrierWindows++;

            /* set the window type to memory or I/O based on MEM bit setting */

	    if (attr & HARRIER_ITAT_MEM)
                pHarrierCpu->winType = PCI_BAR_SPACE_MEM;
            else
                pHarrierCpu->winType = PCI_BAR_SPACE_IO; 
	    pHarrierPci->winType = pHarrierCpu->winType;

            /* read the window base */

	    base = sysPciInLong (pHarrierOffI->base);
	    base = base & ~0xf;

	    size = sysPciInByte (pHarrierOffI->size);
	    size = 0x1000 << size; 	/* Convert code to real size */

	    offset = sysPciInWord(pHarrierOffI->offset);
	    offset = offset << 16;

            /* isolate the window base (start) and limit (end) */

            pHarrierPci->winBase = base & ~0xf;
            pHarrierPci->winLimit = base + size - 1;

           /* calculate translated values */

            pHarrierCpu->winBase = (pHarrierPci->winBase & 0x0000ffff) | offset;
            pHarrierCpu->winLimit = pHarrierCpu->winBase + size - 1;

            /* advance in preparation for next valid window */

            pHarrierCpu++;
            pHarrierPci++;
            }

        /* advance to next set of harrier offsets */

        pHarrierOffI++;
        }
    }


/******************************************************************************
*
* sysHarrierTransAdrs - translate an address that passes through the harrier.
*
* This routine converts an address from a cpu to pci address or vice versa. It
* uses a pair of window arrays built during hardware init2 to guide the
* translation. The adrs parameter is the address to convert.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysHarrierCapt(), sysPciToCpuAdrs(), sysBusToLocalAdrs(), 
* sysLocalToBusAdrs(),  and sysHarrierTransAdrs().
*
*/

LOCAL STATUS sysHarrierTransAdrs
    (
    UINT32            adrsSpace,    /* address space (memory or i/o ) */
    UINT32            adrs,        /* known address */
    UINT32 *          pTransAdrs,   /* pointer to the translated address */
    UINT32            winCnt,        /* number of open windows */
    HARRIER_WIN_STRUCT * pSrc,        /* pointer to the source windows */
    HARRIER_WIN_STRUCT * pDest        /* pointer to the destination windows */
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
* sysHarrierCapt(), and sysHarrierTransAdrs().
*/

LOCAL STATUS sysCpuToPciAdrs
    (
    int     adrsSpace,    /* bus address space where busAdrs resides */
    char *  localAdrs,    /* local address to convert */
    char ** pBusAdrs    /* where to return bus address */
    )
    {
    return (sysHarrierTransAdrs (adrsSpace, (UINT32)localAdrs, 
                                 (UINT32 *)pBusAdrs, sysValidHarrierWindows, 
                                 &sysHarrierCpuToPciWin[0],
                                 &sysHarrierPciToCpuWin[0]) );
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
* sysHarrierCapt(), and sysHarrierTransAdrs().
*/

LOCAL STATUS sysPciToCpuAdrs
    (
    int     adrsSpace,    /* bus address space where busAdrs resides */
    char *  localAdrs,    /* local address to convert */
    char ** pBusAdrs    /* where to return bus address */
    )
    {
    return (sysHarrierTransAdrs (adrsSpace, (UINT32)localAdrs, 
                                 (UINT32 *)pBusAdrs, sysValidHarrierWindows, 
                                 &sysHarrierPciToCpuWin[0], 
                                 &sysHarrierCpuToPciWin[0]) );
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

            return (sysPciToCpuAdrs (PCI_BAR_SPACE_MEM, busAdrs, pLocalAdrs) );

        default:
            return (ERROR);
        }

    }

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
* as bus locking). Since the sysBusTas routine doesn't use any special
* semaphore techniques, this routine is a no-op.
*
* If used, the BSP activates this routine by placing its address into the
* global variable 'smUtilTasClearRtn'.
*
* RETURNS: N/A
*
* SEE ALSO: sysBusTas()
*/

void sysBusTasClear
    (
    volatile char * address    /* address to be tested-and-cleared */
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

    (void) sysMotI2cRead(USR_SROM_ADRS, offset, &temp, I2C_ADDRESS_BYTES);

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
    (void) sysMotI2cWrite (USR_SROM_ADRS, offset, &data, I2C_ADDRESS_BYTES);
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

void
sysCpuCheck (void)
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
        (CPU_TYPE == CPU_TYPE_MAX) || (CPU_TYPE == CPU_TYPE_NITRO))
        {
        return;
        }

#endif    /* (CPU == PPC604) */

    /* Invalid CPU type; print error message and terminate */

    sysDebugMsg (sysWrongCpuMsg, EXIT_TO_SYSTEM_MONITOR); /* does not return */
    }


/*******************************************************************************
*
* sysModeCheck - confirm the operating mode
*
* This routine validates the operating mode.  If the wrong mode is discovered
* a message is printed using the serial channel in polled mode.
*
* RETURNS: N/A.
*/

void
sysModeCheck (void)
{
#ifdef PCI_AUTO_DEBUG
  char txt[100];
#endif

  /*
   * If board status register indicates target mode (SYSCON bit set),
   * report the condition (for information only) and return.
   */

  if (!sysMonarchMode)
  {
    ;
#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"sysModeCheck: SYSCON negated, Operating in Slave-Mode\r\n");
    sysDebugMsg(txt,CONTINUE_EXECUTION);    
#endif
  }
  else
  {
    ;
#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"sysModeCheck: Operating in Monarch-Mode\r\n");
    sysDebugMsg(txt,CONTINUE_EXECUTION);    
#endif
  }

  return;
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
* This routine flushes the posted write buffer in the Harrier and the Dec2155x if
* it is present.
*
* RETURNS: OK if flush succeeded or ERROR if an error occured.
*/

STATUS sysPciWriteFlush(void)
    {
    UINT16   devId;

#ifdef INCLUDE_DEC2155X
    char *   temp;
#endif /* INCLUDE_DEC2155X */

    /*
     * Flush Harrier posted write buffer by doing a dummy read of one
     * of its MPC registers
     */

    devId = sysIn16 ((UINT16 *)(HARRIER_DEVICE_ID_REG));

#ifdef INCLUDE_DEC2155X

    /* Flush the Dec2155x posted write buffer by reading from a cPCI location */

    if (sysBusToLocalAdrs (PCI_SPACE_MEM_SEC, (char *) CPCI_FLUSH_ADDR,
               &temp) != OK)

    return (ERROR);

    temp = (char *)sysIn32 ((UINT32 *)temp);

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
* RETURNS: OK if probe succeeded or ERROR if an exception occured.
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
* and SRR1 PowerPC registers arising from probing addresses as well as the
* Harrier MERST and PCI_CFG_STATUS registers and the Universe PCI_CSR and
* V_AMERR registers.
*
* RETURNS: N/A
*/

void sysProbeErrClr (void)
    {

#ifdef INCLUDE_DEC2155X

    sysDec2155xErrClr ();

#endif /* INCLUDE_DEC2155X */

    /* Clear Harrier Error Exception Register */

    *(UINT32 *)HARRIER_ERROR_EXCEPTION_CLEAR_REG = 
               (HARRIER_EECL_PMA | HARRIER_EECL_PTA | HARRIER_EECL_PAP |
                HARRIER_EECL_PDP | HARRIER_EECL_PDT | HARRIER_EECL_PSE |
                HARRIER_EECL_PPE | HARRIER_EECL_POF);

    /* Clear Harrier's Cnfg Hdr Status Reg */

    *(UINT32 *)HARRIER_PHB_STATUS_REG = 
               (HARRIER_STAT_RCVPE | HARRIER_STAT_SIGSE | HARRIER_STAT_RCVMA |
                HARRIER_STAT_RCVTA | HARRIER_STAT_SIGTA | HARRIER_STAT_DPAR);

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
    UINT32 mpcErrEnbl; /* Harrier MPC Error Enable reg */
    UINT8  mpcErrStat; /* Harrier MPC Error Status reg */

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

    /* save Harrier error reporting configuration */

    mpcErrEnbl = *(UINT32 *)HARRIER_ERROR_EXCEPTION_INT_ENABLE_REG;

    /*
     * disable interrupts and machine checks for Target Abort, Master Abort and
     * SERR
     */

    *(UINT32 *)HARRIER_ERROR_EXCEPTION_INT_ENABLE_REG = 
                (mpcErrEnbl & ~(HARRIER_EEINT_PTA | HARRIER_EEINT_PMA |
                HARRIER_EEINT_PSE));

    /* Perform probe */

    status = sysMemProbeBus (adrs, mode, length, pVal);

    /* capture harrier error indications */

    mpcErrStat = *(UINT32 *)(HARRIER_ERROR_EXCEPTION_STATUS_REG);

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

    /* restore original harrier error reporting configuration */

    *(UINT32 *)HARRIER_ERROR_EXCEPTION_INT_ENABLE_REG = mpcErrEnbl;

    /* safe to re-enable interrupts */

    intUnlock (oldLevel);

    /* check for harrier error indications */

    if (mpcErrStat & (HARRIER_EEST_PMA | HARRIER_EEST_PTA | HARRIER_EEST_PSE))
        status = ERROR;

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

void sysUsDelay
    (
    UINT32    delay        /* length of time in microsec to delay */
    )
    {
    FAST UINT32 baselineTickCount;
    FAST UINT32 ticksToWait;

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

    if ((ticksToWait = delay * ((DEC_CLOCK_FREQ / 4) / 1000000)) == 0)
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
* sysPciOutByteConfirm - Byte out to PCI memory space and flush buffers.
*
* This function outputs a byte to PCI memory space and then flushes the PCI
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

void sysPciOutByteConfirm
    (
    UINT32 adrs,       /* PCI address */
    UINT8  data        /* data to be written */
    )
    {
    UINT8 temp;

    sysPciOutByte (adrs, data);
    temp = sysPciInByte (adrs);

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

/*****************************************************************************
*
* sysPciOutLongConfirm - Long word out to PCI memory space and flush buffers.
*
* This function outputs a long word to PCI memory space and then flushes the
* PCI write posting buffers by reading from the target address. Since the PCI
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

void sysPciOutLongConfirm
    (
    UINT32 adrs,       /* PCI address */
    UINT32 data        /* data to be written */
    )
    {
    UINT32 temp;

    sysPciOutLong (adrs, data);
    temp = sysPciInLong (adrs);

    }

#ifdef INCLUDE_BPE
/*****************************************************************************
*
* sysConfigBpe - configure processor bus parity checking
*
* This function configures processor bus parity checking. Parity checking must
* be enabled in 3 places: 1) Processor, 2) Harrier SMC, and 3) Harrier PHB. It 
* assumes that the default VxWorks machine check exception handler is installed
* to catch and display processor bus parity errors.
*
* RETURNS: N/A
*/

void sysConfigBpe(void)
    {
    UINT32 parEna;

    /* clear harrier data and address processor bus parity errors. */

    parEna = *(UINT32 *)HARRIER_ERROR_EXCEPTION_ENABLE_REG;
    parEna &= ~(HARRIER_EEEN_XAP | HARRIER_EEEN_XDP);
    *(UINT32 *)HARRIER_ERROR_EXCEPTION_ENABLE_REG = parEna;

    /*
     * enable machine check exceptions here so a processor detected error
     * will cause a machine check instead of a check stop.
     */

    vxMsrSet ( vxMsrGet () | _PPC_MSR_ME);

    /* enable CPU processor parity generation */

    vxHid0Set ( vxHid0Get () & ~_PPC_HID0_DBP );

    /*
     * enable CPU processor data and address bus parity checking and the
     * machine check pin.
     */

    vxHid0Set ( vxHid0Get () | _PPC_HID0_EBA | _PPC_HID0_EBD | _PPC_HID0_EMCP );

    /* enable harrier processor address and data bus parity checking */

    parEna |= (HARRIER_EEEN_XAP | HARRIER_EEEN_XDP);
    *(UINT32 *)HARRIER_ERROR_EXCEPTION_ENABLE_REG = parEna;

    }
#endif /* INCLUDE_DPE */

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
     * Our harrier's own MPBAR points to the PMEP location.
     * Convert the value in the MPBAR to a local address, 
     * add in the offset to the MGID (Generic Inbound Doorbell Register),
     * and bias by the appropriate amount to reach the SM_MAILBOX_INT_BIT.
     */

    pciAddr = LONGSWAP(*(UINT32 *)HARRIER_XCSR_MPBAR);
    pciAddr &= PCI_MEMBASE_MASK;
    pciAddr += (HARRIER_PMEP_MGID_OFFSET + (SM_MAILBOX_INT_BIT % 8));
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
* reportBootromErrors - report errors detected during rom start.
*
* This function reports error bits left behind by the rom start sequence. These
* bits are stored in the Harrier PHB general purpose registers and are read out
* and printed by this routine.
*
* RETURNS: N/A
*/

void reportBootromErrors (void)
    {
    UINT32 flags;
    UINT32 regIdx;
    BOOL   printStarted = FALSE;
    UCHAR  group;
    UCHAR  tempStr[80];

    /*
     * loop through the first 4 general purpose registers and print any
     * non-zero values detected.
     */

    group = 'A';
    for (regIdx = HARRIER_GENERAL_PURPOSE0_REG; 
         regIdx <= HARRIER_GENERAL_PURPOSE3_REG; regIdx += 4)
        {

        if ((flags = *(UINT32 *)regIdx) != 0)
            {

            /* print a blank line ahead of the first error line. */

            if (!printStarted)
                {
                sysDebugMsg ("\r\n", CONTINUE_EXECUTION);
                printStarted = TRUE;
                }

            sprintf (tempStr,
    "Bootrom Error: Group = %c, Code = 0x%08x, Refer to BSP Documentation.\r\n",
                    group, flags);

            sysDebugMsg (tempStr, CONTINUE_EXECUTION);
            }
        group++;
        }

    /* if any messages were printed, print an extra blank seperator line. */

    if (printStarted)
        sysDebugMsg ("\r\n", CONTINUE_EXECUTION);
        
    }


/*****************************************************************************
*
* sysSysconAsserted - check the state of the syscon bit
*
* This function examines the state of the syscon bit in the board status
* register. If the board is acting as the system controller the syscon bit will
* be a one (high-true). If the syscon bit is a zero, then this board is not
* acting as the system controller.
*
* RETURNS: TRUE is SYSCON is asserted, FALSE if not asserted.
*/
BOOL sysSysconAsserted (void)
    {
    return (((*(UINT32 *)HARRIER_MISC_CONTROL_STATUS_REG) & 
                        HARRIER_MCSR_SCON) ? TRUE : FALSE);
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
/*******************************************************************************
*
* sysMailboxInt - mailbox interrupt handler
*
* Check to see if the interrupt applies to us.  If it does, clear it and
* call the mailbox routine if it exists.
*/

LOCAL void sysMailboxInt (void)
    {

    UINT8  temp;
    UINT8 * byteAddr;

    byteAddr = (UINT8 *)( HARRIER_MP_GENERIC_INBOUNDDOORBELL_REG + 
	                 (SM_MAILBOX_INT_BIT % 8) );

    temp = *byteAddr;

    temp &= (1 << SM_MAILBOX_INT_BIT);

    if (temp != 0)
        {

	/* Clear the inbound doorbell interrupt */

	*byteAddr = (1 << SM_MAILBOX_INT_BIT);

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
* NOTE: The mailbox interrupt is DEC2155X_MAILBOX_INT_VEC.
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
    UINT8  temp8;
    UINT16 temp16;
    UINT32 maskAddr;

    if (sysMpicIntEnable (SM_MAILBOX_INT_VEC) == ERROR)
	return (ERROR);

    /* Enable "inbound message passing doorbell" in FEEN register */

    temp16 = *(UINT16 *)HARRIER_XCSR_FEEN;
    temp16 |= HARRIER_FEEN_IMDB;
    *(UINT16 *)HARRIER_XCSR_FEEN = temp16;

    /* Enable "inbound message passing doorbell" in FEMA register */

    temp16 = *(UINT16 *)HARRIER_XCSR_FEMA;
    temp16 &= ~HARRIER_FEMA_IMDB;
    *(UINT16 *)HARRIER_XCSR_FEMA = temp16;

    /* Unmask this one particular doorbell register */

    maskAddr = HARRIER_XCSR_MGIDM + (SM_MAILBOX_INT_BIT % 8);
    oldVal = intLock();
    temp8 = *(UINT8 *)maskAddr;
    temp8 &= ~(1 << SM_MAILBOX_INT_BIT);
    *(UINT8 *)maskAddr = temp8;
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
    UINT32 maskAddr;
    UINT8  temp8;

    /* Unmask this one particular doorbell register */

    maskAddr = HARRIER_XCSR_MGIDM + (SM_MAILBOX_INT_BIT % 8);
    temp8 = *(UINT8 *)maskAddr;
    temp8 |= (1 << SM_MAILBOX_INT_BIT);
    *(UINT8 *)maskAddr = temp8;

    return (OK);
    }

#endif /* INCLUDE_SM_COMMON */


/*******************************************************************************
*
* inboundSizeCode - Return UCHAR encoding for the total amount of DRAM.
*
* This routine returns the UCHAR encoding for the total amount of DRAM 
* for the ITSZx field of Harrier's ITSZ/ITOF registers.  See table 4.4.3.10-1
* of the Harrier Programming Model.
*
* RETURNS: UCHAR encoding of DRAM.
*/

UCHAR
inboundSizeCode(UINT32 dramSize)
{
  char sizeCode;
  UINT sizeMask = 0x80000000;                 /* 2 Gigabyte */

  for(sizeCode = 0x13; sizeCode > 0x0; sizeCode--)
  {
    if(dramSize & sizeMask) break;
    else                    sizeMask >>= 1;
  }

  return(sizeCode);
}

/*******************************************************************************
*
* prpmcSlaveWaitConfig - Wait for Host to enumerate PCI Bus.
*
* This routine holds the Slave PrPMC until the Host has configured
* our Harrier.
*
* RETURNS: NA
*/

void
prpmcSlaveWaitConfig (void)
{
  int    timeout = PRPMC_SLAVE_SIDE_TIMEOUT;
  UINT32 bar;
  int	 msgWaitCnt = 0;
  char txt[100];

  /* If we've already waited and our config is done, just return */
  if( PCI_AUTOCONFIG_DONE ) return;

  /* Wait until Memory Access has been enabled on device */
  while(timeout != 0)
  {
    /* if the PCI Host has enabled Memory Access on the bridge,
       the host is done with the bridge enumeration */

    bar = LONGSWAP(*(volatile UINT32 *) 
		           HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_0);
	bar &= PCI_MEMBASE_MASK;

#ifdef PCI_AUTO_DEBUG
    sprintf(txt,"prpmcSlaveWaitConfig: adr=0x%08x, bar=0x%08x, barmask=0x%08x\r\n",
      HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_0,
      LONGSWAP(*(volatile UINT32 *) HARRIER_INBOUND_TRANSLATION_BASE_ADDRESS_0),
      bar);
    sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

    /* if BAR is configured, assume Harrier is configured */
	if( (bar != 0) /*mv5500: && ((bar&0xF0000000) != 0xF0000000)*/ )
    {
      sysDebugMsg("prpmcSlaveWaitConfig: Bridge is configured.\r\n",
        CONTINUE_EXECUTION);
 	  break;
    }

	if ((msgWaitCnt % 2000) == 0)
	{
      sysDebugMsg("prpmcSlaveWaitConfig: Slave waiting for Host to configure host bridge.\r\n",
		         CONTINUE_EXECUTION);
	  msgWaitCnt++;
	}

	msgWaitCnt++;
    sysUsDelay(1000);			/* Delay 1 mSec */

    if (timeout != PRPMC_SLAVE_INFINITE_TIMEOUT) --timeout;
  }

  /* If configuration timeout, report the error and re-start. */
#if (PRPMC_SLAVE_SIDE_TIMEOUT != PRPMC_SLAVE_INFINITE_TIMEOUT)
  if(timeout <= 0)
  {
	sysDebugMsg ("prpmcSlaveWaitConfig: Slave timeout waiting for Monarch initialization.\n\r",
		EXIT_TO_SYSTEM_MONITOR);
  }
#endif

  if(msgWaitCnt != 0) 
  {
    sysDebugMsg
	("prpmcSlaveWaitConfig: Slave resuming execution after Monarch Initialization.\r\n\n",
	CONTINUE_EXECUTION);
  }

  /* Sergey: delay 10 sec to give more time for Monarch to configure
  remaining pci devices if any (ethernet for example) */
  sysUsDelay(10000000);

  return;

}

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

	if ((msgWaitCnt % 2000) == 0)
	    {
	    sysDebugMsg("Waiting for Dec2155x primary configuration\r\n",
	                 CONTINUE_EXECUTION);
            msgWaitCnt++;
            sysUsDelay(1000);			/* Delay 1 mSec */
	    }

        if (timeout != PRPMC_DRAWBRIDGE_INFINITE_TIMEOUT)
            --timeout;

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

#if (CARRIER_TYPE == PRPMC_G)
/*******************************************************************************
*
* w83601Read - Read register from W83601 GPI/O
*
* This routine reads the requested W83601 GPI/O register and returns the
* value to the user.
*
* RETURNS: OK, or ERROR.
*/

STATUS w83601Read
    (
    USHORT  regOffset, /* offset within target device to read */
    UCHAR  *pBfr       /* pointer to data byte */
    )
    {
    if (sysMotI2cRead(GPIO_I2C_ADRS, regOffset, pBfr, I2C_SINGLE_ADDRESS) == OK)
	return (OK);
    else
	return (ERROR);
    }


/*******************************************************************************
*
* w83601Write - Write register to W83601 GPI/O
*
* This routine writes the specified value to the requested W83601 GPI/O
* register.
*
* RETURNS: OK, or ERROR.
*/

STATUS w83601Write
    (
    USHORT  regOffset, /* offset within target device to read */
    UCHAR  *pBfr       /* pointer to data byte */
    )
    {
    if (sysMotI2cWrite(GPIO_I2C_ADRS, regOffset, pBfr, I2C_SINGLE_ADDRESS) ==OK)
	return (OK);
    else
	return (ERROR);
    }


/*******************************************************************************
*
* w83782RegRead - Read from internal W83782 H/W Monitor register
*
* This routine reads the requested W83782 H/W Monitor register and returns the
* value to the user.
*
* RETURNS: OK, or ERROR.
*/

STATUS w83782RegRead
    (
    USHORT  regOffset, /* offset within target device to read */
    UCHAR  *pBfr       /* pointer to data byte */
    )
    {
    if (sysMotI2cRead(W83782_I2C_ADRS, regOffset, pBfr,
                      I2C_SINGLE_ADDRESS) == OK)
	return (OK);
    else
	return (ERROR);
    }


/*******************************************************************************
*
* w83782RegWrite - Write to internal W83782 H/W Monitor register
*
* This routine writes the specified value to the requested W83782 H/W Monitor
* register.
*
* RETURNS: OK, or ERROR.
*/

STATUS w83782RegWrite
    (
    USHORT  regOffset, /* offset within target device to read */
    UCHAR  *pBfr       /* pointer to data byte */
    )
    {
    if (sysMotI2cWrite(W83782_I2C_ADRS, regOffset, pBfr,
                       I2C_SINGLE_ADDRESS) ==OK)
	return (OK);
    else
	return (ERROR);
    }
#endif /* (CARRIER_TYPE == PRPMC_G) */

