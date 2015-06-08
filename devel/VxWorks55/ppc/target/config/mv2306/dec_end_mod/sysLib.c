/* sysLib.c - Motorola MVME2600 board series system-dependent library */

/* Copyright 1984-2002 Wind River Systems, Inc. */
/* Copyright 1996,1997,1998 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
02t,17jul02,kab  SPR 73642: sysBusTas* cleanup
02s,30apr02,sbs  fixing compiler warnings
02r,26mar02,dtr  Removing compiler warnings.
02q,30nov01,gls  fixed sysBatDesc definition (SPR #20321)
02p,06nov01,dat  Fix to sysBusTasClear, SPR 30848
02o,11oct99,dmw  Added error message if VME_A32_MSTR_LOCAL is less than
                 the total DRAM size.
02n,18aug99,dmw  Added support for mv2300SC.
02m,24feb99,sbs  corrected mod history (SPR #21146).
02l,09feb99,mas  Removed erroneous Guard bit on Flash memory region (SPR 24893)
02k,03dec98,mas  added GUARDED attributes to non-local memory areas mapped in
		 sysPhysMemDesc[]. (SPR 23674)
02j,13oct98,dat  SPR 20654, added ravenAuxClk for mv2300
02i,24aug98,mdp  Fixed support for ECC
02h,07aug98,tb   Added VMEbus DMA support
02g,07aug98,tb   Fixed support for SM_OFF_BOARD
02f,25jun98,mas  all variables except sysPhysMemSize are now conditionally
		 compiled in sysPhysMemTop() (SPR 21271).
02e,28apr98,mas  removed variable sysVmeEnable (SPR 21043).
02d,16apr98,dat  moved sysHid1Get to sysALib.o
02c,15apr98,db   added support for floppy disk driver.
02b,14apr98,ms_  merged Motorola mv2700 support
02a,11feb98,tb   Added TAS support which uses VMEbus RMW
01z,17dec97,tb   Added Universe II support
01y,19aug97,scb  Add MPC750 (Arthur) support.
01x,05nov97,mas  removed intConnect and counter for sysUnivVERRIntr; added
		 EIEIO_SYNC in sysMemProbeBus(); fixed sysVmeProbe(); changed
		 P2P_PREF_MEM_SIZE to P2P_NONPREF_MEM_SIZE (SPR 9654, 9717).
01w,31oct97,mas  added intConnect and counter for sysUnivVERRIntr (SPR 9438).
01v,25jul97,srr  added 604r (Mach 5) support (SPR 8911).
	    mas
01u,24jul97,mas  added support for dynamic memory sizing (SPR 8824).
01t,14jul97,mas  added support for probing all buses w/sysBusProbe() (SPR 8022).
01s,11jul97,tb   fixed Secondary SCSI bug (MR 78).
01r,11jun97,mas  merged w/SENS version of BSP.
01q,05jun97,mas  added support for serial ports 3 & 4 (Z85230 ESCC). (SPR 8566)
01p,02jun97,map  removed sysDec21x40MediaSelect().
01o,29may97,dat  remove sysGetDramSpd to romInit.s. Add reset to MPIC before
		 initializing it.  Modified sysModel for MV3600.
		 (MCG MR #67, 73) SPRs 8289, 8560.
01n,12may97,dat  fixed mangen bugs
01m,09may97,mas  added extended VME support (SPR 8410).
		 now clears BRDFAIL LED after sysHwInit() (SPR 8432).
01l,29apr97,dat  removed old pci.h now using pciIomapLib.h
01k,25apr97,map  added support for dec21x40End driver.
01j,24apr97,mas  added support for MPIC (SPR 8170).
01i,11apr97,dat  using local version of pciIomapLib.c - temporarily.
01h,10apr97,mas  added support for pciIomapLib and PMC Span board (SPR 8226).
01g,01apr97,dat  new VME configuration macros SPR 8271
01f,24mar97,mas  made Lance and Scsi init/control code conditional (SPR 8141).
01e,10jan97,dat  added sysBusClearTas setup, fixed WD_TIMER setup.
            mas  fixed 64K VME problem (from motorola).
01d,02jan97,wlf  doc: cleanup.
01c,02jan97,dat  removed refs to mv1600, fixed mod history
01b,17dec96,mas  removed raven.h, universe.h and ncr810.h includes; added
		 include of pci.h (SPR 7525).
01a,01sep96,mot  written. (from ver 01t of mv1600 bsp)
*/

/*
DESCRIPTION
This library provides board-specific routines.  The chip drivers included are:

    i8250Sio.c - Intel 8250 UART driver 
    z8530Sio.c - Zilog 8530 ESCC driver
    ppcDecTimer.c - PowerPC decrementer timer library (system clock)
    ppcZ8536Timer.c - Zilog Z8536 timer library (auxiliary clock)
    byteNvRam.c - byte-oriented generic non-volatile RAM library
    pciConfigLib.c - PCI configuration library
    universe.c - Tundra Universe chip VME-to-PCI interface library
    ncr810Lib - NCR 53C810 or NCR 53C825 SCSI controller library
    fdcDrv.c - driver for PS2 floppy device controller(FDC)
    isaDma.c - I8237 ISA DMA transfer interface library

    sl82565IntrCtl.c - interrupt controller driver
         or
    ravenMpic.c - raven Mpic / W83C553 PIB/IBC Interrupt Controller

INCLUDE FILES: sysLib.h

SEE ALSO:
.pG "Configuration"
*/

/* includes */

#include "vxWorks.h"
#include "vme.h"
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


/* defines */

#define ZERO	0


/* globals */

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
 * Note: BAT registers CANNOT be disabled - they are always active.
 * For example, setting them all to zero will yield four identical data
 * and instruction memory spaces starting at local address zero, each 128KB
 * in size, and each set as write-back and cache-enabled.  Hence, the BAT regs
 * MUST be configured carefully.
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

    0, 0

    };

/*
 * sysPhysMemDesc[] is used to initialize the Page Table Entry (PTE) array
 * used by the MMU to translate addresses with single page (4k) granularity.
 * PTE memory space should not, in general, overlap BAT memory space but
 * may be allowed if only Data or Instruction access is mapped via BAT.
 *
 * Address translations for local RAM, memory mapped PCI bus, memory mapped
 * VME A16 space and local PROM/FLASH are set here.
 *
 * PTEs are held, strangely enough, in a Page Table.  Page Table sizes are
 * integer powers of two based on amount of memory to be mapped and a
 * minimum size of 64 kbytes.  The MINIMUM recommended Page Table sizes
 * for 32-bit PowerPCs are:
 *
 * Total mapped memory		Page Table size
 * -------------------		---------------
 *        8 Meg			     64 K
 *       16 Meg			    128 K
 *       32 Meg			    256 K
 *       64 Meg			    512 K
 *      128 Meg			      1 Meg
 * 	.				.
 * 	.				.
 * 	.				.
 *
 * [Ref: chapter 7, PowerPC Microprocessor Family: The Programming Environments]
 *
 * *** EXTENDED_VME configuration ***
 *
 * The user can use TLBs, and/or BATs, to map VME A32 space to the processor.
 * The default is to use TLBs (MMU).  Change the table entry below to use
 * a different method. (See "MODIFY A32 VME WINDOW HERE")
 */

PHYS_MEM_DESC sysPhysMemDesc [] =
    {
    {
    /* Vector Table and Interrupt Stack */

    (void *) LOCAL_MEM_LOCAL_ADRS,
    (void *) LOCAL_MEM_LOCAL_ADRS,
    RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    /* Local DRAM */

    (void *) RAM_LOW_ADRS,
    (void *) RAM_LOW_ADRS,
    LOCAL_MEM_SIZE -  RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE
    },

    {
    /* PCI address spaces */

    (void *) CPU_PCI_ISA_IO_ADRS,
    (void *) CPU_PCI_ISA_IO_ADRS,
    CPU_PCI_ISA_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    (void *) CPU_PCI_IO_ADRS,
    (void *) CPU_PCI_IO_ADRS,
    CPU_PCI_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

#ifndef INCLUDE_MPIC
    {
    (void *) CPU_PCI_IACK_ADRS,
    (void *) CPU_PCI_IACK_ADRS,
    CPU_PCI_IACK_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },
#endif /* INCLUDE_MPIC */

    {
    (void *) CPU_PCI_MEM_ADRS,
    (void *) CPU_PCI_MEM_ADRS,
    CPU_PCI_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    (void *) CPU_PCI_ISA_MEM_ADRS,
    (void *) CPU_PCI_ISA_MEM_ADRS,
    CPU_PCI_ISA_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },


    {
    /* MODIFY A32 VME WINDOW HERE */
    (void *) VME_A32_MSTR_LOCAL,
    (void *) VME_A32_MSTR_LOCAL,
    VME_A32_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    (void *) VME_A24_MSTR_LOCAL,
    (void *) VME_A24_MSTR_LOCAL,
    VME_A24_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    (void *) VME_A16_MSTR_LOCAL,
    (void *) VME_A16_MSTR_LOCAL,
    VME_A16_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    /* Off-board VME LM/SIG/Semaphore Regs */

    (void *) CPU_VME_WINDOW_REG_BASE,
    (void *) CPU_VME_WINDOW_REG_BASE,
    VME_A32_REG_SPACE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    /* MPIC Regs */

    (void *) MPIC_BASE_ADRS,
    (void *) MPIC_BASE_ADRS,
    MPIC_REG_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    (void *) FALCON_BASE_ADRS,
    (void *) FALCON_BASE_ADRS,
    FALCON_REG_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    (void *) RAVEN_BASE_ADRS,
    (void *) RAVEN_BASE_ADRS,
    RAVEN_REG_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT  |
    VM_STATE_GUARDED
    },

    {
    (void *) FLASH_BASE_ADRS,
    (void *) FLASH_BASE_ADRS,
    FLASH_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    }
    };

int sysPhysMemDescNumEnt = NELEMENTS (sysPhysMemDesc);

int    sysBus      = VME_BUS;		/* system bus type */
int    sysCpu      = CPU;		/* system CPU type (MC680x0) */
char * sysBootLine = BOOT_LINE_ADRS;	/* address of boot line */
char * sysExcMsg   = EXC_MSG_ADRS;	/* catastrophic message area */
int    sysProcNum;			/* processor number of this CPU */
int    sysFlags;			/* boot flags */
char   sysBootHost [BOOT_FIELD_LEN];	/* name of host from which we booted */
char   sysBootFile [BOOT_FIELD_LEN];	/* name of file from which we booted */
UINT   sysVectorIRQ0  = INT_VEC_IRQ0;	/* vector for IRQ0 */
static int   ravPciBusNo;               /* Raven Config Space BDF address */
static int   ravPciDevNo;
static int   ravPciFuncNo;
int    pciToVmeDev;			/* PCI to VME Bridge Device */


/* Global area to dump diagnostic info in */
char davedump [1024];


/* last 5 nibbles are board specific, initialized in sysHwInit */

unsigned char lnEnetAddr [6] = { 0x08, 0x00, 0x3e, 0x00, 0x00, 0x00 };
unsigned char clearWd [1]  = { 0x00 };

#ifdef INCLUDE_PMC_SPAN
/*
 * PMC Span (DEC21150 PCI-to-PCI Bridge) Configuration Parameter Array
 *
 * This array MUST contain the parameters in the order which they will
 * be set.
 */

PMC_SPAN sysPmcSpanParm [] =
{
  {PCI_CFG_COMMAND, 2,   P2P_PMC_DISABLE},
  {PCI_CFG_STATUS,  2,   P2P_CLR_STATUS >> 16},
  {PCI_CFG_BRIDGE_CONTROL, 2, P2P_SEC_BUS_RESET},
  {PCI_CFG_CACHE_LINE_SIZE, 2, P2P_CACHE_LINE_SIZE | P2P_PRIM_LATENCY},
  {PCI_CFG_PRIMARY_BUS,     1, PCI_PRIMARY_BUS},
  {PCI_CFG_SECONDARY_BUS,   1, PCI_SECONDARY_BUS},
  {PCI_CFG_SUBORDINATE_BUS, 1, PCI_SUBORD_BUS},
  {PCI_CFG_SEC_STATUS, 2,   P2P_CLR_STATUS >> 16},
  {PCI_CFG_IO_BASE, 2,   ((P2P_IO_BASE & 0x0000F000) >> 8) |
                         ((P2P_IO_BASE + P2P_IO_SIZE - 1) & 0x0000F000)},
  {PCI_CFG_MEM_BASE, 2,  ((P2P_NONPREF_MEM_BASE & 0xFFF00000) >> 16)},
  {PCI_CFG_MEM_LIMIT, 2, ((P2P_NONPREF_MEM_BASE + P2P_NONPREF_MEM_SIZE - 1) & \
                          0xFFF00000) >> 16},
  {PCI_CFG_PRE_MEM_BASE, 2, ((P2P_PREF_MEM_BASE & 0xFFF00000) >> 16)},
  {PCI_CFG_PRE_MEM_LIMIT, 2, ((P2P_PREF_MEM_BASE + P2P_PREF_MEM_SIZE - 1) & \
                              0xFFF00000) >> 16},
  {PCI_CFG_PRE_MEM_BASE_U,  4, P2P_PREF_HI32_BASE},  /* only < 4GB space */
  {PCI_CFG_PRE_MEM_LIMIT_U, 4, P2P_PREF_HI32_BASE},
  {PCI_CFG_IO_BASE_U,  2, ((P2P_IO_BASE & 0xFFFF0000) >> 16)},
  {PCI_CFG_IO_LIMIT_U, 2, ((P2P_IO_BASE + P2P_IO_SIZE - 1) & 0xFFFF0000) >> 16},
  {PCI_CFG_BRIDGE_CONTROL, 2, 0},
  {PCI_CFG_DEC21150_SEC_CLK,   2, P2P_CLK_ENABLE},
  {PCI_CFG_DEC21150_SERR_STAT, 1, (P2P_CLR_STATUS >> 16) & 0xFF},
  {PCI_CFG_STATUS,  2,  P2P_CLR_STATUS >> 16},
  {PCI_CFG_COMMAND, 2,  P2P_PMC_ENABLE}
};

#define NUM_PMC_SPAN_PARMS      (sizeof(sysPmcSpanParm) / sizeof(PMC_SPAN))
#endif /* INCLUDE_PMC_SPAN */

/* locals */

LOCAL char sysModelStr[80];
LOCAL char wrongCpuMsg[] = WRONG_CPU_MSG; 
LOCAL UINT tableArthur[] = 	/* PLL multiplier times ten */
    {
    00,  75,  70,  10,  20,  65,  25,  45,
    30,  55,  40,  50,  15,  60,  35,  00
    };


/* forward declarations */

void	sysSpuriousIntHandler(void);
void	sysCpuCheck (void);
char *  sysPhysMemTop (void);
STATUS	sysPciExtRavenInit (int, int, int);
UCHAR	sysNvRead(ULONG);
void	sysNvWrite(ULONG,UCHAR);
int	sysGetBusSpd (void);
void	sysBusTasClear (volatile char *);
void    sysRavenErrClr (void);
STATUS  sysPmcSpanConfig (int, int, int, PMC_SPAN *);
STATUS  sysBusProbe (char *, int, int, char *);
void	sysDebugMsg (char * str);
IMPORT UINT sysHid1Get(void);
LOCAL	BOOL sysVmeVownTas(char *);
LOCAL	BOOL sysVmeRmwTas(char *);

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
IMPORT void   sysBusRmwEnable(UINT, UINT, UINT, char *);
IMPORT void   sysBusRmwDisable(void);
IMPORT UINT32 sysTimeBaseLGet (void);

/* BSP DRIVERS */

#include "pci/pciConfigLib.c"

/*  DJA 7/2003 Modification for use of mv2400 style
    DEC End Network driver.
#include "./sysEnd.c"
*/
#ifdef INCLUDE_NETWORK
#   include "./sysNet.c"
#   ifdef INCLUDE_END
#      include "./sysDec21x40End.c"
#   endif /* INCLUDE_END */
#endif /* INCLUDE_NETWORK */


#include "sysSerial.c"
#include "mem/byteNvRam.c"
#include "timer/ppcDecTimer.c"		/* PPC603 & 604 have on chip timers */
#include "sysScsi.c"                    /* sysScsiInit routine */
#include "sysCache.c"
#include "universe.c"

#ifdef INCLUDE_MPIC
#  include "ravenMpic.c"
#else
#  include "sl82565IntrCtl.c"
#endif /* INCLUDE_MPIC */

#ifndef MV2300
#  include "timer/ppcZ8536Timer.c"
#  include "fdcDrv.c"	/* include floppy disk driver */
#  include "isaDma.c"   /* include DMA driver */
#endif

#ifdef INCLUDE_RAVEN_AUXCLK
#  include "ravenAuxClk.c"
#endif  /* INCLUDE_RAVEN_AUXCLK */

/* defines for sysBusTas() and sysBusTasClear() */

#define VMEBUS_OWNER	(*UNIVERSE_MAST_CTL & LONGSWAP(MAST_CTL_VOWN_ACK))
#define CPU_CLOCKS_PER_LOOP	10
#define LOCK_TIMEOUT		10
#define UNLOCK_TIMEOUT		10000


/******************************************************************************
*
* sysModel - return the model name of the CPU board
*
* This routine returns the model name of the CPU board.  The returned string
* depends on the board model and CPU version being used, for example,
* "Motorola MVME2600 - MPC 604e".
*
* RETURNS: A pointer to the string.
*/

char * sysModel (void)
    {
    char   base_type;
    char * pBrdType;
    int    cpu;

    /* Determine board type */

    base_type = *MV2600_BMSR;
    switch (base_type)
        {
        case MV2600_BMSR_2300:
                pBrdType = "MVME2300";
                break;
        case MV2600_BMSR_2300_SC:
            if (((*MV2600_CCR) & MV2600_CCR_ID_MSK) == MV2600_CCR_2300)
			    pBrdType = "MV2300SC";
            else
			    pBrdType = "MVME2600";
            break;
        case MV2600_BMSR_SG712:
        case MV2600_BMSR_SG761:
#ifdef MV2700
                pBrdType = "MVME2700";
#else
                pBrdType = "MVME2600";
#endif
                break;
        case MV2600_BMSR_FG712:
        case MV2600_BMSR_FG761:
                pBrdType = "MVME3600";
                break;
        default:
                pBrdType = "Unknown";
                break;
        }

    /* Determine CPU type and build display string */

    cpu = CPU_TYPE;
    switch (cpu)
	{
	case CPU_TYPE_604E:
	    sprintf (sysModelStr, "Motorola %s - MPC 604e", pBrdType);
	    break;
        case CPU_TYPE_604R:
            sprintf (sysModelStr, "Motorola %s - MPC 604r", pBrdType);
            break;
	case CPU_TYPE_603P:
	    sprintf (sysModelStr, "Motorola %s - MPC 603p", pBrdType);
	    break;
	case CPU_TYPE_603E:
	    sprintf (sysModelStr, "Motorola %s - MPC 603e", pBrdType);
	    break;
        case CPU_TYPE_750:
	    sprintf (sysModelStr, "Motorola %s - MPC 750", pBrdType);
	    break;
	default:
	    sprintf (sysModelStr, "Motorola %s - MPC 60%d", pBrdType, cpu);
	    break;
	}

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
    int         pciBusNo;       /* PCI bus number */
    int         pciDevNo;       /* PCI device number */
    int         pciFuncNo;      /* PCI function number */

    /*
     *	Validate CPU type
     */
    sysCpuCheck();

    /*
     *  Initialize PCI driver library.
     */
    if (pciConfigLibInit (PCI_MECHANISM_1, PCI_PRIMARY_CAR, PCI_PRIMARY_CDR, 0)
        != OK)
        {
        sysToMonitor (BOOT_NO_AUTOBOOT);
        }

    /*
     *  Due to a bug in the Universe I, the vme chip needs to be
     *  the first PCI device probed and configured.
     *
     *  Initialize the Standard PCI Header of the VME device if present
     */
    if ((DEVICE_PRESENT(MV2600_BMFR_VMEP) == TRUE) &&
        (pciFindDevice ((PCI_ID_UNIVERSE & 0xFFFF),
                        (PCI_ID_UNIVERSE >> 16) & 0xFFFF, 0,
                        &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR))
        {
        UCHAR pciRev;

        pciConfigInByte (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_REVISION,
                 &pciRev);
        if (pciRev == 0)
            {
            pciToVmeDev = UNIVERSE_I;

            /*
	     *  Due to a bug in the Universe I, it is necessary to disable the
	     *  PCI Slave Image 0 before accessing other PCI devices.  Refer to
	     *  Universe Device Errata #11 and the MVME2600 Series Single Board
	     *  Computer Programmer's Reference Guide, Chapter 4, "Universe
	     *  Chip Problems after a PCI Reset".
             */

            sysUniverseReset ();
            }
        else
            {
            pciToVmeDev = UNIVERSE_II;
            }

        (void)pciDevConfig (pciBusNo, pciDevNo, pciFuncNo,
                            0,
                            PCI_MEM_UNIVERSE_ADRS,
                            (PCI_CMD_MASTER_ENABLE | PCI_CMD_MEM_ENABLE));
        }

    /*
     *  The Raven powers up with the correct PCI Header values.
     *  So, no initialization of the PCI Header is performed.
     */
 
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
        }

#ifdef INCLUDE_FD
    /*
     * IBC initialization for floppy disk driver support, modify the
     * ISA-to-PCI address decoder register to map ISA addresses 0x100000
     * to 0x1000000 to appear on the PCI Bus.
     */

    if (pciFindDevice ((PCI_ID_IBC & 0xFFFF), (PCI_ID_IBC >> 16) & 0xFFFF, 0,
                       &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR)
        {
        pciConfigOutByte (pciBusNo, pciDevNo, pciFuncNo, 0x48, 0xf1 );
        }
#endif
 
#ifdef INCLUDE_NETWORK
    /*
     *  Initialize the Standard PCI Header of the Ethernet device if present
     */
     if ((DEVICE_PRESENT(MV2600_BMFR_LANP) == TRUE) )
        {
        if ((pciFindDevice ((PCI_ID_LN_DEC21140 & 0xFFFF),
                            (PCI_ID_LN_DEC21140 >> 16) & 0xFFFF, 0,
                            &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR))
            {
            (void)pciDevConfig (pciBusNo, pciDevNo, pciFuncNo,
                                PCI_IO_LN_ADRS,
                                0,
                                (PCI_CMD_MASTER_ENABLE | PCI_CMD_IO_ENABLE));
            }
        else
            {
            sysToMonitor (BOOT_NO_AUTOBOOT);
            }
        }

#endif /* INCLUDE_NETWORK */
 
#ifdef INCLUDE_SCSI
    /*
     *  Initialize the Standard PCI Header of the SCSI device if present
     */
    if ((DEVICE_PRESENT(MV2600_BMFR_SCIP) == TRUE) &&
        (pciFindDevice ((PCI_ID_SCSI & 0xFFFF), (PCI_ID_SCSI >> 16) & 0xFFFF,
                        0, &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR))
        {
        (void)pciDevConfig (pciBusNo, pciDevNo, pciFuncNo,
                            PCI_IO_SCSI_ADRS, 0,
                            (PCI_CMD_MASTER_ENABLE | PCI_CMD_IO_ENABLE));
        }
#endif /* INCLUDE_SCSI */
 
#ifdef  INCLUDE_SECONDARY_SCSI
    /*
     *  Initialize the Standard PCI Header of the Secondary SCSI device
     *  (SYM53C825A) if present.
     */
     if (pciFindDevice (PCI_ID_NCR825 & 0xFFFF, (PCI_ID_NCR825 >> 16) & 0xFFFF,
                        1, &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR)
        {
        (void)pciDevConfig (pciBusNo, pciDevNo, pciFuncNo,
                            PCI_IO_SCSI_ADRS2, 0,
                            (PCI_CMD_MASTER_ENABLE | PCI_CMD_IO_ENABLE));
        }
#endif  /* INCLUDE_SECONDARY_SCSI */

    /*
     *  Initialize the extended portion of the Raven's PCI Header.
     *	The non-PCI Config Space registers have already been
     *	initialized in the initial startup code.
     */
    if (pciFindDevice ((PCI_ID_RAVEN & 0xFFFF), (PCI_ID_RAVEN >> 16) & 0xFFFF,
                       0, &ravPciBusNo, &ravPciDevNo, &ravPciFuncNo) != ERROR)
        {
        (void)sysPciExtRavenInit(ravPciBusNo, ravPciDevNo, ravPciFuncNo);
        }

#ifndef INCLUDE_MPIC
    /* Initialize the extended portion of the IBC's PCI Header.  */

    if (pciFindDevice ((PCI_ID_IBC & 0xFFFF), (PCI_ID_IBC >> 16) & 0xFFFF, 0,
                       &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR)
        {
        (void)sysPciExtIbcInit(pciBusNo, pciDevNo, pciFuncNo);
        }
#endif /* INCLUDE_MPIC */

    /*
     *  Initialize the non-PCI Config Space registers of the
     *  IBC which doesn't have a true device driver.
     */
    sysIbcInit();

    /*
     *  Reset the MPIC.
     */
    sysPciWrite32( MPIC_ADDR(MPIC_GLOBAL_CONFIG_REG), RESET_CNTRLR );
 
#ifdef INCLUDE_MPIC
    /*
     *  Initialize the non-PCI Config Space registers of the
     *  Raven Mpic.
     */
    sysMpicInit();
#endif /* INCLUDE_MPIC */

#ifdef INCLUDE_PMC_SPAN
    /*
     * Initialize and configure PMC Span (bridge) to secondary PCI bus.
     */
    if ((DEVICE_PRESENT(MV2600_BMFR_BRGP) == TRUE) &&
        (pciFindDevice ((PCI_ID_BR_DEC21150 & 0xFFFF),
                        (PCI_ID_BR_DEC21150 >> 16) & 0xFFFF, 0,
                        &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR))
        {
        (void)sysPmcSpanConfig (pciBusNo, pciDevNo, pciFuncNo, sysPmcSpanParm);
        }
#endif  /* INCLUDE_PMC_SPAN */

    /*
     *  The LANCE has a real driver associated with it, so
     *  no additional initialization is done here.  It's done
     *  at kernel init time.
     */

    /*
     *  The SCSI has a real driver associated with it, so
     *  no additional initialization is done here.  It's done
     *  at kernel init time.
     */

    /*
     *  Initialize the non-PCI Config Space registers of the
     *  Universe which doesn't have a true device driver.
     */
    sysUniverseInit();
 
#ifdef  INCLUDE_VME_DMA

    /*  Initialize the VMEbus DMA driver */

    sysVmeDmaInit();

#endif  /* INCLUDE_VME_DMA */

    /* DJA 7/2003 Call sysNetHwInit */
#ifdef INCLUDE_NETWORK
    sysNetHwInit();
#endif

 
#ifdef  INCLUDE_SECONDARY_ENET
    /*
     *  The dec21040 (Secondary Ethernet) has a real driver
     *  associated with it, so
     *  no additional initialization is done here.  For now,
     *  the application must create, configure, and establish
     *  a route for the device.
     */
#endif  /* INCLUDE_SECONDARY_ENET */

#ifdef  INCLUDE_SECONDARY_SCSI
    /*
     *  The Secondary SCSI (SYM53C825A on the mezzanine) has a
     *  real driver associated with it, so no additional
     *  initialization is done here.  To access it, use
     *  pSysScsiCtrl2 when issuing the appropriate calls.
     */
#endif  /* INCLUDE_SECONDARY_SCSI */

    /* set shared memory TAS Clear routine pointer */

    smUtilTasClearRtn = (VOIDFUNCPTR)sysBusTasClear;

    /* set pointer to bus probing hook */

    _func_vxMemProbeHook = (FUNCPTR)sysBusProbe;

    /* Initialize COM1 and COM2 serial channels */

    sysSerialHwInit();

    /* Extract the Ethernet address out of non-volatile RAM.
     * The Motorola convention for the Ethernet address is that they only
     * save the low 3 bytes in BBRAM.  The high three bytes are the
     * manufacturers code, and Motorola software knows its own.
     * The Motorola code is 0x08003Exxx.
     */
    sysNvRamGet ((char *)lnEnetAddr, 6, ((int) BB_ENET - NV_BOOT_OFFSET));

    /* Disable the watchdog timer */

    sysNvWrite ((ULONG)WD_TIMER, 0);

    /*
     * If mmu tables are used, this is where we would dynamically
     * update the entry describing main memory, using sysPhysMemTop().
     * We must call sysPhysMemTop () at sysHwInit() time to do
     * the memory autosizing if available.
     */

    sysPhysMemTop ();

    /* clear Raven error conditions */

    sysRavenErrClr ();

    /* Upon completion, clear BFL (Board Fail) LED */

    *(UINT8 *)z8536_PORTA_DATA &= ~z8536_PORTA_BRDFAIL;
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
* NOTE: Do no adjust LOCAL_MEM_SIZE to reserve memory for application
* use.  See sysMemTop() for more information on reserving memory.
*
* RETURNS: The address of the top of physical memory.
*
* SEE ALSO: sysMemTop()
*/

char * sysPhysMemTop (void)
    {
    static char * sysPhysMemSize = NULL;	/* ptr to top of mem + 1 */
#ifdef LOCAL_MEM_AUTOSIZE
    UINT32        dramAttr;
    UINT32        dramIndex;
    int           i;
    static UINT32 dramSize[8] =
    /*   0,         16MB,       32MB,       64MB,
       128MB,      256MB,        1GB,         0   */
    {0x00000000, 0x01000000, 0x02000000, 0x04000000,
     0x08000000, 0x10000000, 0x40000000, 0x00000000};
#endif /* LOCAL_MEM_AUTOSIZE */

    if (sysPhysMemSize == NULL)
	{
#ifdef LOCAL_MEM_AUTOSIZE
	/*
	 * Do dynamic memory sizing.
	 *
	 * Since Falcon memory controller chip has already been set to
	 * control all memory, just read and interpret its DRAM Attributes
	 * Register.
	 */

	dramAttr = sysIn32 ((UINT32 *)FALCON_DRAM_ATTR);
	for (i = 0; i < 4; ++i)
	    {
	    if ((dramAttr & 0x80) != 0)
	        {
		dramIndex = dramAttr & 0x07;
		sysPhysMemSize = (char *)((UINT32)sysPhysMemSize + dramSize [dramIndex]);
		}
	    dramAttr >>= 8;
	    }

        /* Adjust initial DRAM size to actual physical memory. */

        sysPhysMemDesc[1].len = (ULONG)sysPhysMemSize -
			        (ULONG)sysPhysMemDesc[1].physicalAddr;

#else /* not LOCAL_MEM_AUTOSIZE */
	/* Don't do auto-sizing, use defined constants. */

	sysPhysMemSize = (char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);
#endif /* LOCAL_MEM_AUTOSIZE */

#ifdef EXTENDED_VME
        /* check for invalid DRAM/VME A32 configuration */

        if ((UINT32)sysPhysMemSize > VME_A32_MSTR_LOCAL)
            {
            int  dscEn;
	    char dbgMsg [80];

	    sprintf (dbgMsg,
	    "\r\n\aERROR: Increase VME_A32_MSTR_LOCAL to at least 0x%x.\r\n",
	    (UINT)sysMemTop()); 

	    sysDebugMsg (dbgMsg);

            /* 
             * Find and adjust initial starting addresses of VME PCI memory 
             * to allow board to come up. 
             */

            for (dscEn = 0; dscEn < sysPhysMemDescNumEnt; dscEn++)
                {
                if ((UINT32)sysPhysMemDesc[dscEn].virtualAddr ==
                    VME_A32_MSTR_LOCAL)
                    {
                    sysPhysMemDesc[dscEn].virtualAddr = (void *)sysPhysMemSize;
                    sysPhysMemDesc[dscEn].physicalAddr = (void *)sysPhysMemSize;
                    break;
                    }
                }
	    }
#endif /* EXTENDED_VME */

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
	}

    return memTop;
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
    int startType	/* parameter passed to ROM to tell it how to boot */
    )
    {
    FUNCPTR pRom = (FUNCPTR) (ROM_TEXT_ADRS + 4); /* Warm reboot */

#if defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2)
    sysL2CacheDisable();	/* Disable the L2 Cache */
#endif
    cacheDisable (0);   	/* Disable the Instruction Cache */
    cacheDisable (1);   	/* Disable the Data Cache */

#if     (CPU == PPC604)
    vxHid0Set (vxHid0Get () & ~_PPC_HID0_SIED);	/* Enable Serial Instr Exec */
#endif  /* (CPU == PPC604) */

    sysUniverseReset ();	/* reset Universe chip */
    sysSerialReset ();		/* reset serial devices */
    sysRavenErrClr ();		/* clear Raven error conditions */

    /* Clear the MSR */

    vxMsrSet (0);

    (*pRom) (startType);

    return (OK);	/* in case we ever continue from ROM monitor */
    }

/******************************************************************************
*
* sysDebugMsg - print a debug string to the console in polled mode.
*
* This routine prints a message to the system console in polled mode.
*
* RETURNS: N/A
*
*/

void sysDebugMsg
    (
    char * str
    )
    {
    int msgSize;
    int msgIx;
    SIO_CHAN * pSioChan;        /* serial I/O channel */

    msgSize = strlen (str);

    pSioChan = sysSerialChanGet (0);

    sioIoctl (pSioChan, SIO_MODE_SET, (void *) SIO_MODE_POLL);

    for (msgIx = 0; msgIx < msgSize; msgIx++)
        {
        while (sioPollOutput (pSioChan, str[msgIx]) == EAGAIN);
        }
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
	/* connect Vme to PCI interrupt */

	intConnect (INUM_TO_IVEC(UNIV_INT_VEC), sysUnivVmeIntr, 0);

	/* interrupts can only be turned on ultimately by the PIC int ctrlr */

	intEnable (UNIV_INT_LVL);

#ifndef MV2300
	sysZ8536Init();
	intConnect (INUM_TO_IVEC(Z8536_INT_VEC), sysClkIntCIO, 0);
	intEnable (Z8536_INT_LVL);
#else  /* MV2300 */

#   ifdef INCLUDE_RAVEN_AUXCLK
	sysAuxClkInit();
	intConnect (INUM_TO_IVEC(TIMER0_INT_VEC), sysAuxClkInt, 0);
	intEnable(TIMER0_INT_LVL);
#   endif /* INCLUDE_RAVEN_AUXCLK */

#endif /* MV2300 */

	/* initialize serial interrupts */

	sysSerialHwInit2();

        /* connect a dummy routine for the spurious interrupt (0x07) */

        intConnect (INUM_TO_IVEC(PP_INT_LVL), sysSpuriousIntHandler, 0);

#ifdef INCLUDE_CACHE_L2
#ifdef INCLUDE_CACHE_SUPPORT
#ifdef USER_L2_CACHE_ENABLE
	/* initialize the L2 cache */

	sysL2CacheInit();
#else
	sysL2CacheDisable();
#endif
#endif
#endif

	/* DJA 7/2003 - Call sysNetHwInit */
#ifdef INCLUDE_NETWORK
        sysNetHwInit2 ();
#endif

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
* the VMEbus.
*
* RETURNS: N/A
*
* SEE ALSO: sysProcNumGet()
*
*/

void sysProcNumSet
    (
    int 	procNum			/* processor number */
    )
    {

    /*
     *	Init global variable - this needs to be done before
     *	calling sysUniverseInit2() because it calls sysProcNumGet()
     *	via the MACRO definition.
     */
    sysProcNum = procNum;

    /*
     *	Set up the node's VME slave decoders.
     */
    sysUniverseInit2(procNum);

    }


/* miscellaneous support routines */

/******************************************************************************
*
* sysLocalToBusAdrs - convert a local address to a bus address
*
* This routine returns a VMEbus address as it would be seen on the bus.
* The local address that is passed into this routine is the address of
* the local resource as seen by the CPU.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysBusToLocalAdrs()
*/
 
STATUS sysLocalToBusAdrs
    (
    int 	adrsSpace,	/* bus address space where busAdrs resides */
    char *	localAdrs,	/* local address to convert */ 
    char **	pBusAdrs	/* where to return bus address */ 
    )
    {

    switch (adrsSpace)
        {
        case VME_AM_EXT_SUP_PGM:
        case VME_AM_EXT_SUP_DATA:
        case VME_AM_EXT_USR_PGM:
        case VME_AM_EXT_USR_DATA:
	    if ((VME_A32_SLV_SIZE != 0) &&
		((ULONG)localAdrs >= VME_A32_SLV_LOCAL) &&
		((ULONG)localAdrs < (VME_A32_SLV_LOCAL + VME_A32_SLV_SIZE)))
		{
                *pBusAdrs = localAdrs + (VME_A32_SLV_BUS - VME_A32_SLV_LOCAL);
                return (OK);
                }

	    /* localAdrs within VME Hardware register range? */

	    if (((ULONG)localAdrs >= CPU_VME_HW_REGS_BASE) &&
		((ULONG)localAdrs < (CPU_VME_HW_REGS_BASE +
					 CPU_VME_HW_REGS_SZ)))
		 {
		 *pBusAdrs = (localAdrs - CPU_VME_HW_REGS_BASE) +
			      (VME_A32_REG_BASE + (sysProcNumGet() *
						   VME_A32_REG_SIZE));
		 return (OK);
		 }
            else
                return (ERROR);

        case VME_AM_STD_SUP_PGM:
        case VME_AM_STD_SUP_DATA:
        case VME_AM_STD_USR_PGM:
        case VME_AM_STD_USR_DATA:
#               if (VME_A24_SLV_SIZE != 0) 
	            if(((ULONG)localAdrs >= VME_A24_SLV_LOCAL) &&
		       ((ULONG)localAdrs < (VME_A24_SLV_LOCAL + VME_A24_SLV_SIZE)))
		      {
			*pBusAdrs = localAdrs + (VME_A24_SLV_BUS - VME_A24_SLV_LOCAL);
			return (OK);
		      }
#	        endif

                return (ERROR);

        case VME_AM_SUP_SHORT_IO:
        case VME_AM_USR_SHORT_IO:
#	    if (VME_A16_SLV_SIZE != 0) 
		if(((ULONG)localAdrs >= VME_A16_SLV_LOCAL) &&
		   ((ULONG)localAdrs < (VME_A16_SLV_LOCAL + VME_A16_SLV_SIZE)))
		  {
		    *pBusAdrs = localAdrs + (VME_A16_SLV_BUS - VME_A16_SLV_LOCAL);
		    return (OK);
		  }
#	    endif
            return (ERROR);
        default:
            return (ERROR);
        }
    }


/******************************************************************************
*
* sysBusToLocalAdrs - convert a bus address to a local address
*
* This routine returns a local address that is used to access the VMEbus.
* The bus address that is passed into this routine is the VMEbus address
* as it would be seen on the bus.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysLocalToBusAdrs()
*/

STATUS sysBusToLocalAdrs
    (
    int  	adrsSpace, 	/* bus address space where busAdrs resides */
    char *	busAdrs,   	/* bus address to convert */
    char **	pLocalAdrs 	/* where to return local address */
    )
    {

    switch (adrsSpace)
	{
	case VME_AM_EXT_SUP_PGM:
	case VME_AM_EXT_USR_PGM:
	case VME_AM_EXT_SUP_DATA:
	case VME_AM_EXT_USR_DATA:
	    if ((VME_A32_MSTR_SIZE != 0) &&
		((ULONG) busAdrs >= VME_A32_MSTR_BUS) &&
		((ULONG) busAdrs < (VME_A32_MSTR_BUS + VME_A32_MSTR_SIZE)))
		{
	    	*pLocalAdrs = (char *)busAdrs + (VME_A32_MSTR_LOCAL -
				                 VME_A32_MSTR_BUS);
	    	return (OK);
		}

	    /*
	     *	This handles the VME LM/SIG/SEM regs window
	     */
	    if (((ULONG) busAdrs >= VME_A32_REG_BASE) &&
		((ULONG) busAdrs < (VME_A32_REG_BASE +
				   VME_A32_REG_SPACE)))
		{
	    	*pLocalAdrs = (char *)busAdrs + (CPU_VME_WINDOW_REG_BASE -
				                 VME_A32_REG_BASE);
	    	return (OK);
		}

	    return (ERROR);
	    break;

	case VME_AM_STD_SUP_PGM:
	case VME_AM_STD_USR_PGM:
	case VME_AM_STD_SUP_DATA:
	case VME_AM_STD_USR_DATA:
	    if ((VME_A24_MSTR_SIZE == 0) ||
		((ULONG) busAdrs < VME_A24_MSTR_BUS) ||
		((ULONG) busAdrs >= (VME_A24_MSTR_BUS + VME_A24_MSTR_SIZE)))
		{
		return (ERROR);
		}

	    *pLocalAdrs = (char *) busAdrs +
			    (VME_A24_MSTR_LOCAL - VME_A24_MSTR_BUS);

	    return (OK);
	    break;

        case VME_AM_SUP_SHORT_IO:
        case VME_AM_USR_SHORT_IO:
#	    if (VME_A16_MSTR_SIZE == 0)
	        return (ERROR);
#           else
	    
	    if(((ULONG) busAdrs < VME_A16_MSTR_BUS) ||
	       ((ULONG) busAdrs >= (VME_A16_MSTR_BUS + VME_A16_MSTR_SIZE)))
	       {
		return (ERROR);
	       }

            *pLocalAdrs = (char *) busAdrs + 
			    (VME_A16_MSTR_LOCAL - VME_A16_MSTR_BUS);
            return (OK);
#endif
	default:
	    return (ERROR);
	}
    }


/*******************************************************************************
*
* sysBusTas - test and set a specified location
*
* This routine performs a test-and-set (TAS) instruction on the specified
* address.  To prevent deadlocks, interrupts are disabled during the
* test-and-set operation.  The following table defines the method used to
* insure an atomic operation.
*
* .CS
*		Master		Slave_1		Slave_2
*
* VME Chip	Don't Care	U1		U1
*		----------	----		----
* Method	VOWN		VOWN		VOWN
*
* VME Chip	Don't Care	U1		U2
*		----------	----		----
* Method	VOWN		VOWN		RMW
*
* VME Chip	U1 or		U2		U2
*		U2+
*		----------	----		----
* Method	VOWN+++		RMW		RMW
*
* VME Chip	U2++		U2		U2
* PCI Bridge	or Raven3
*		----------	----		----
* Method	lwarx/stwcx	RMW		RMW
*
*  +   = Refer to target.txt file for explaination of older boards
*	 with Universe II (U2).
*  ++  = Refer to target.txt file for explaination of newer boards
*	 with Universe II (U2).
*  +++ = The master's hardware does not preserve the atomic RMW.
* ++++ = If SM_OFF_BOARD == TRUE,  the method used will be the same
*	 as if the master board is acting like a slave board; namely:
*	 	RMW for UNIVERSE_II and VOWN for UNIVERSE_I
* .CE
*
* NOTE: Although the address passed-in to sysBusTas() is defined as
* "char *", vxTas() operates on the address as a "void *".
* For PowerPC, this implies that the location tested-and-set is
* actually a 32-bit entity.
* 
* RETURNS: TRUE if the value had not been set but is now, or
* FALSE if the value was set already.
*
* SEE ALSO: vxTas(), sysBusTasClear()
*/

BOOL sysBusTas
    (
    char * adrs          /* address to be tested and set */
    )
    {
    /* Check for master node */

    if (sysProcNumGet() == 0)
	{
#ifdef	ANY_BRDS_IN_CHASSIS_NOT_RMW

	/*
	 * A slave board in the chassis cannot generate a VMEbus RMW,
	 * and/or the master board cannot translate an incoming VMEbus
	 * RMW into an atomic operation, and/or the master board cannot
	 * generate a VMEbus RMW; therefore, the master must set
	 * VOWN before doing a TAS.
	 */

	return (sysVmeVownTas(adrs));

#else
#if	(SM_OFF_BOARD == FALSE)
        BOOL state = FALSE;  /* semaphore state */
        int  lockKey;        /* interrupt lock key */

	/* 
	 * All slave boards in the chassis are generating a VMEbus RMW, and
	 * the master board can translate an incoming VMEbus RMW into an
	 * atomic operation; therefore, the master can simply call vxTas()
	 * (lwarx/stwcx sequence) because the write portion of the RMW will
	 * be detected.
	 */

        /* lock interrupts so that vxTas() can execute without preemption */

        lockKey = intLock ();

	/* Perform TAS on local address */

	state =  vxTas ((UINT *)adrs);
	EIEIO_SYNC;

        intUnlock (lockKey);

        /* return TAS test result */

        return (state);

#else
	/* The master board can generate a VMEbus RMW */

	return (sysVmeRmwTas(adrs));

#endif	/* SM_OFF_BOARD */

#endif	/* ANY_BRDS_IN_CHASSIS_NOT_RMW */
	}
    else	/* Slave node */
	{
	if (pciToVmeDev == UNIVERSE_II)
	    {
	    /* A slave board with the UNIVERSE_II can generate a VMEbus RMW */

	    return (sysVmeRmwTas(adrs));
	        }
	else	/* UNIVERSE_I */
	    {
	    /*
	     * A slave board with the UNIVERSE_I cannot generate a VMEbus
	     * RMW; therefore, it must set VOWN before doing a TAS.
	     */

	    return (sysVmeVownTas(adrs));

	    }
        }

    }


/******************************************************************************
*
* sysBusTasClear - clear a location set by sysBusTas()
*
* This routine clears the specified 32-bit location typically set
* by sysBusTas().  The following table defines the method used to
* insure an atomic operation.
*
* .CS
*		Master		Slave_1		Slave_2
*
* VME Chip	Don't Care	U1		U1
*		----------	----		----
* Method	VOWN		VOWN		VOWN
*
* VME Chip	Don't Care	U1		U2
*		----------	----		----
* Method	VOWN		VOWN		RMW
*
* VME Chip	U1 or		U2		U2
*		U2+
*		----------	----		----
* Method	VOWN+++		RMW		RMW
*
* VME Chip	U2++		U2		U2
* PCI Bridge	or Raven3
*		------------	------------	------------
* Method	simple clear	simple clear	simple clear
*
*
*  +   = Refer to target.txt file for explaination of older boards
*	 with Universe II (U2).
*  ++  = Refer to target.txt file for explaination of newer boards
*	 with Universe II (U2).
*  +++ = The master's hardware does not preserve the atomic RMW.
* ++++ = If SM_OFF_BOARD == TRUE,  no special/additional processing
*	 is required.
* .CE
*
* RETURNS: N/A
*
* SEE ALSO: sysBusTas()
*/

void sysBusTasClear
    (
    volatile char * adrs	/* Address of semaphore to be cleared */
    )
    {
    BOOL state;
    int  lockKey;        /* interrupt lock key */

#ifdef	ANY_BRDS_IN_CHASSIS_NOT_RMW

    /*
     * A slave board in the chassis cannot generate a VMEbus RMW,
     * and/or the master board cannot translate an incoming VMEbus
     * RMW into an atomic operation; therefore, the master must set
     * VOWN before clearing a local TAS location.
     */

    /* Check for master node or slave node with UNIVERSE_I */

    if ((sysProcNumGet() == 0) ||
	(pciToVmeDev == UNIVERSE_I))
	{
        /*
         * Compute a 10 microsecond delay count
         *
         * NotTimeOut(loops) = timeout(usec)/clks/loop/CPU_SPEED(clks/usec)
         */

        int  NotTimeOut = ((MEMORY_BUS_SPEED/1000000) * UNLOCK_TIMEOUT)
			    / CPU_CLOCKS_PER_LOOP;

        /* do clearing with no interference */

        lockKey = intLock ();

        /* request ownership of the VMEbus */

        *UNIVERSE_MAST_CTL |= LONGSWAP(MAST_CTL_VOWN);
        EIEIO_SYNC;

        /* wait for the VME controller to give you the BUS */

        while (!VMEBUS_OWNER && NotTimeOut)
	    {
	    --NotTimeOut;
	    }

        /* clear the location */

        *(UINT *)adrs = 0;
        EIEIO_SYNC;

        /* release the VME BUS */

        *UNIVERSE_MAST_CTL &= LONGSWAP(~MAST_CTL_VOWN);

        /* enable normal operation */

        intUnlock (lockKey);
        }
    else	/* Slave node == UNIVERSE_II */
	{
        /* Lock interrupts so that setting up SCG and issuing RMW are atomic */

        lockKey = intLock ();

        /* Enable RMW cycle */

        sysBusRmwEnable(VME_SCG_COMPARE_MASK,
			VME_SCG_COMPARE_TO_CLEAR,
			VME_SCG_SWAP_TO_CLEAR,
			(char *)adrs);

        /* perform RMW to clear TAS location */

        state = *((UINT *)adrs);
	EIEIO_SYNC;

        /* Disable RMW cycle */

        sysBusRmwDisable();

        /* unlock the interrupt */

        intUnlock (lockKey);
        }
#else
#if	(SM_OFF_BOARD == FALSE)
    /*
     * All slave boards in the chassis can generate a VMEbus RMW, and
     * the master board can translate an incoming VMEbus RMW into an
     * atomic operation therefore, all boards can do a simple clear.
     */

    EIEIO_SYNC;		/* SPR 30848, sync before clearing */
    *(UINT *)adrs = 0;

#else	/* SM_OFF_BOARD == TRUE */

    /* Check for master node or slave node with UNIVERSE_I */

    if ((sysProcNumGet() == 0) ||
	(pciToVmeDev == UNIVERSE_I))
	{
        /*
         * Compute a 10 microsecond delay count
         *
         * NotTimeOut(loops) = timeout(usec)/clks/loop/CPU_SPEED(clks/usec)
         */

        int  NotTimeOut = ((MEMORY_BUS_SPEED/1000000) * UNLOCK_TIMEOUT)
			    / CPU_CLOCKS_PER_LOOP;

        /* do clearing with no interference */

        lockKey = intLock ();

        /* request ownership of the VMEbus */

        *UNIVERSE_MAST_CTL |= LONGSWAP(MAST_CTL_VOWN);
        EIEIO_SYNC;

        /* wait for the VME controller to give you the BUS */

        while (!VMEBUS_OWNER && NotTimeOut)
	    {
	    --NotTimeOut;
	    }

        /* clear the location */

        *(UINT *)adrs = 0;
        EIEIO_SYNC;

        /* release the VME BUS */

        *UNIVERSE_MAST_CTL &= LONGSWAP(~MAST_CTL_VOWN);

        /* enable normal operation */

        intUnlock (lockKey);
        }
    else	/* Slave node == UNIVERSE_II */
	{
        /* Lock interrupts so that setting up SCG and issuing RMW are atomic */

        lockKey = intLock ();

        /* Enable RMW cycle */

        sysBusRmwEnable(VME_SCG_COMPARE_MASK,
			VME_SCG_COMPARE_TO_CLEAR,
			VME_SCG_SWAP_TO_CLEAR,
			(char *)adrs);

        /* perform RMW to clear TAS location */

        state = *((UINT *)adrs);
	EIEIO_SYNC;

        /* Disable RMW cycle */

        sysBusRmwDisable();

        /* unlock the interrupt */

        intUnlock (lockKey);
        }

#endif	/* SM_OFF_BOARD */
#endif	/* ANY_BRDS_IN_CHASSIS_NOT_RMW */
    }


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

    /*
     * Compute a 10 microsecond delay count
     *
     * NotTimeOut(loops) = timeout(usec)/clks/loop/CPU_SPEED(clks/usec)
     */

    int  NotTimeOut = ((MEMORY_BUS_SPEED/1000000) * LOCK_TIMEOUT)
			/ CPU_CLOCKS_PER_LOOP;

    /* lock interrupts so there will be no TAS interference */

    lockKey = intLock ();

    /* Request ownership of the VMEbus */

    *UNIVERSE_MAST_CTL |= LONGSWAP(MAST_CTL_VOWN);
    EIEIO_SYNC;

    /* Wait for the VME controller to give you the BUS */

    while (!VMEBUS_OWNER && NotTimeOut)
	{
	--NotTimeOut;
	}

    /* perform the TAS */

    if (VMEBUS_OWNER)
	{
	state =  vxTas ((UINT *)adrs);
	EIEIO_SYNC;
	}

    /* release the VME BUS */
    
    *UNIVERSE_MAST_CTL &= LONGSWAP(~MAST_CTL_VOWN);

    /* unlock the interrupt */

    intUnlock (lockKey);

    /* return TAS test result */

    return (state);

    }


/*******************************************************************************
*
* sysVmeRmwTas - test and set a location across the VMEbus utilizing RMW
*
* This routine performs a test-and-set (TAS) instruction on the specified
* address.  To prevent deadlocks, interrupts are disabled and the VMEbus is
* locked during the test-and-set operation.
*
* NOTE: Although the address passed-in to sysBusTas() is defined as
*	"char *", vxTas() operates on the address as a "void *".
*	For PowerPC, this implies that the location tested-and-set is
*	actually a 32-bit entity.
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

    if (state)
        {
	return (FALSE);
	}
    else
        {
        return (TRUE);
        }
    }


/* DJA 7/2003 - Removed sysLanIntEnable() sysLanIntDisable() 
   They are in sysNet.c now */


/******************************************************************************
*
* sysSpuriousIntHandler - spurious interrupt handler
*
* This is the entry point for spurious interrupts.
*
* NOTE: This routine has no effect.
*
* This routine catches all spurious interrupts.  It does nothing at all.
*
* RETURNS: N/A.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void sysSpuriousIntHandler (void)
    {
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
    ULONG	offset	/* NVRAM offset to read the byte from */
    )
    {
    sysOutByte (NV_RAM_LSB_REG, LSB(offset));
    sysOutByte (NV_RAM_MSB_REG, MSB(offset));
    return sysInByte (NV_RAM_DAT_REG);
    }


/******************************************************************************
*
* sysNvWrite - write one byte to NVRAM
*
* This routine writes a single byte to a specified offset in NVRAM.  The
* MVME2600 uses a 48T18 device w/fast write times, no wait.
*
* RETURNS: N/A
*/

void sysNvWrite
    (
    ULONG	offset,	/* NVRAM offset to write the byte to */
    UCHAR	data	/* datum byte */
    )
    {
    sysOutByte (NV_RAM_LSB_REG, LSB(offset));
    sysOutByte (NV_RAM_MSB_REG, MSB(offset));
    sysOutByte (NV_RAM_DAT_REG, data);
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
    int msgSize;
    int msgIx;
    SIO_CHAN * pSioChan;        /* serial I/O channel */

    /* Check for a valid CPU type;  If one is found, just return */

#if	(CPU == PPC603)

    if	((CPU_TYPE == CPU_TYPE_603) || (CPU_TYPE == CPU_TYPE_603E) ||
         (CPU_TYPE == CPU_TYPE_603P))
	{
	return;
	}

#else	/* (CPU == PPC604) */

    if	((CPU_TYPE == CPU_TYPE_604) || (CPU_TYPE == CPU_TYPE_604E) ||
	 (CPU_TYPE == CPU_TYPE_604R) || (CPU_TYPE == CPU_TYPE_750))
	{
	return;
	}

#endif	/* (CPU == PPC604) */

    /* Invalid CPU type; print error message and terminate */

    msgSize = strlen (wrongCpuMsg);

    sysSerialHwInit ();

    pSioChan = sysSerialChanGet (0);

    sioIoctl (pSioChan, SIO_MODE_SET, (void *) SIO_MODE_POLL);

    for (msgIx = 0; msgIx < msgSize; msgIx++)
    	{
    	while (sioPollOutput (pSioChan, wrongCpuMsg[msgIx]) == EAGAIN);
    	}

    sysToMonitor (BOOT_NO_AUTOBOOT);
    }


/******************************************************************************
*
* sysRavenErrClr - Clear error conditions in Raven
*
* This routine clears any existing errors in the Motorola Raven PCI Host Bridge
* Controller.
*
* RETURNS: N/A
*/

void sysRavenErrClr (void)
    {
    /* Clear MPC Error Status register */

    sysOutByte ((RAVEN_BASE_ADRS + RAVEN_MPC_MERST), RAVEN_MPC_MERST_CLR);

    /* get and clear Raven PCI status reg */

    pciConfigOutWord (ravPciBusNo, ravPciDevNo, ravPciFuncNo, PCI_CFG_STATUS,
                      RAVEN_PCI_CFG_STATUS_DPAR | RAVEN_PCI_CFG_STATUS_SIGTA |
                      RAVEN_PCI_CFG_STATUS_RCVTA | RAVEN_PCI_CFG_STATUS_RCVMA |
                      RAVEN_PCI_CFG_STATUS_SIGSE | RAVEN_PCI_CFG_STATUS_RCVPE);
    }


/******************************************************************************
*
* sysPciExtRavenInit - initialize the extended portion of the Raven PCI header
*
* This routine initializes the extended portion of the PCI header for the 
* Motorola Raven ISA Bridge Controller (IBC).
*
* RETURNS: OK, or ERROR if...
*
* SEE ALSO: sysPciExtIbcInit()
*/

STATUS sysPciExtRavenInit
    (
    int busNo,          /* bus number */
    int deviceNo,       /* device number */
    int funcNo          /* function number */
    )
    {
    /*
     * Init Raven's MPIC control register access addresses in I/O and
     * memory spaces:
     *
     * IOSPACE  - 0x00000000  [no access]
     * MEMSPACE - 0x3C000000  [MPIC_PCI_BASE_ADRS]
     */

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_0,
                      0x00000000);
    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_1,
                      MPIC_PCI_BASE_ADRS);
 
    /*
     *  Init Raven's Slave decoders (range/offset/attributes)
     *
     *  These decoders map addresses on the PCI bus to the CPU for
     *  access to local DRAM.
     *
     *  Because hardware can read past real memory, it is necessary to disable
     *  Read Ahead for the last 64k of physical memory (DRAM).
     */

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSADD0,
                      PCI2CPU_ADDR0_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSOFF0,
                      PCI2CPU_OFFSET0);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSATT0,
                      PCI2CPU_ATT0);

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSADD1,
                      PCI2CPU_ADDR1_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSOFF1,
                      PCI2CPU_OFFSET1);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSATT1,
                      PCI2CPU_ATT1);

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSADD2,
                      PCI2CPU_ADDR2_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSOFF2,
                      PCI2CPU_OFFSET2);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSATT2,
                      PCI2CPU_ATT2);

    pciConfigOutLong (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSADD3,
                      PCI2CPU_ADDR3_RANGE);
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSOFF3,
                      PCI2CPU_OFFSET3);
    pciConfigOutByte (busNo, deviceNo, funcNo, PCI_CFG_RAVEN_PSATT3,
                      PCI2CPU_ATT3);
 
    /*
     *  Enable Raven's PCI master capability and MEM space
     *  (i.e., enable PCI space decoders).
     */
    pciConfigOutWord (busNo, deviceNo, funcNo, PCI_CFG_COMMAND, 0x0006);

    return (OK);
    }


#ifdef INCLUDE_PMC_SPAN
/******************************************************************************
*
* sysPmcSpanConfig - configure the PMC Span (DEC21150 PCI-to-PCI Bridge)
*
* This routine configures the DEC21150 PCI-to-PCI Bridge on the PMC Span.
*
* RETURNS: OK or ERROR if pciConfigLib has not been initialized.
*/

STATUS sysPmcSpanConfig
    (
    int         pciBusNo,       /* PCI bus number */
    int         pciDevNo,       /* PCI device number */
    int         pciFuncNo,      /* PCI function number */
    PMC_SPAN *  pmcSpan         /* pointer to PMC Span config array */
    )
    {
    STATUS      result = OK;
    FAST        i;

    /* Write all parameters in pcmSpan in the order given */

    for (i = 0; i < NUM_PMC_SPAN_PARMS && result == OK; ++i)
        {
        switch (pmcSpan[i].parmSize)
            {
            case 1:
                result = pciConfigOutByte (pciBusNo, pciDevNo, pciFuncNo,
                                           pmcSpan[i].parmOffset,
                                           pmcSpan[i].parmValue);
                break;

            case 2:
                result = pciConfigOutWord (pciBusNo, pciDevNo, pciFuncNo,
                                           pmcSpan[i].parmOffset,
                                           pmcSpan[i].parmValue);
                break;

            case 4:
                result = pciConfigOutLong (pciBusNo, pciDevNo, pciFuncNo,
                                           pmcSpan[i].parmOffset,
                                           pmcSpan[i].parmValue);
                break;
            }
        }

    return (result);
    }
#endif /* INCLUDE_PMC_SPAN */


/******************************************************************************
*
* sysGetBusSpd - get the speed of the 60x processor bus
*
* This routine returns the speed (in MHz) of the 60x system bus.
*
* RETURNS: The bus speed (inMHz).
*/

int sysGetBusSpd (void)
    {
    int busSpd;
    int speed;

    busSpd = (( *MV2600_CCR ) & MV2600_CCR_CLK_MSK );

    switch (busSpd)
        {
        case MV2600_CCR_CPU_CLK_66:
            speed = 67;
	    break;
	case MV2600_CCR_CPU_CLK_60:
	    speed = 60;
	    break;
	case MV2600_CCR_CPU_CLK_50:
	    speed = 50;
	    break;
	default:
	    speed = 67;
        }
    return (speed);
    }


/* DJA 7/2003 - Removed sysDec21x40EnetAddrGet - It is in sysDec21x40End.c now */


/******************************************************************************
*
* sysMemProbeTrap - trap handler for vxMemProbe exception
*
* This routine is called from the excConnectCode stub if sysMemProbeSup
* generates an exception. By default, sysMemProbeSup returns OK.
* This code changes the PC value to "sysProbeExc" (within the sysMemProbeSup
* routine), and sysProbeExc sets the return value to ERROR.
*/

static int sysMemProbeTrap
    (
    ESFPPC *    pEsf		/* pointer to exception stack frame */
    )
    {
    REG_SET *pRegSet = &pEsf->regSet;

    pRegSet->pc = (_RType)sysProbeExc;	/* sysProbeExc forces an ERROR return */
    return (0);
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
* a special handler for Machine Check, Data Access and Alignment exceptions.
*
* RETURNS: OK if probe succeeded or ERROR if an exception occured.
*/

static STATUS sysMemProbeBus
    (
    char   * adrs,	/* address to be probed */
    int      mode,	/* VX_READ or VX_WRITE */
    int      length,	/* 1, 2 or 4 byte probe */
    char   * pVal,	/* address of value to write OR */
			/* address of location to place value read */
    UINT16   ravenCnfg  /* raven probe configuration */
    )
    {
    int      oldLevel;
    FUNCPTR  oldVec1;
    FUNCPTR  oldVec2;
    STATUS   status;
    UINT16   devId;
    UINT16   mpcErrEnbl;/* Raven MPC Error Enable reg */
    UINT32   ppcHid0;   /* H/W Implementation Dependent reg (PPC60x) */
    UINT32   ppcMsr;    /* PPC Machine Status Reg */

    /* Probes performed with interrupts disabled */

    oldLevel = intLock ();

    /* Handle Machine Check Exceptions locally */

    oldVec1 = excVecGet ((FUNCPTR *) _EXC_OFF_MACH);
    excVecSet ((FUNCPTR *) _EXC_OFF_MACH, FUNCREF(sysMemProbeTrap)); 

    /*
     *  Handle Data Access Exceptions locally
     *
     *  Data Access Exceptions will occur when trying to probe addresses
     *  that have not been mapped by the MMU.
     */

    oldVec2 = excVecGet ((FUNCPTR *) _EXC_OFF_DATA);
    excVecSet ((FUNCPTR *) _EXC_OFF_DATA, FUNCREF(sysMemProbeTrap));

    /* Enable Abort Machine Check Exception generation by Raven */

    mpcErrEnbl = sysIn16 ((UINT16 *)(RAVEN_BASE_ADRS + RAVEN_MPC_MEREN));
    sysOut16 ((UINT16 *)(RAVEN_BASE_ADRS + RAVEN_MPC_MEREN),
              mpcErrEnbl | ravenCnfg);

    /* Enable Machine Check Pin (EMCP) */

    ppcHid0 = vxHid0Get();
    vxHid0Set(ppcHid0 | _PPC_HID0_EMCP);

    /* Enable Machine Check exception */

    ppcMsr = vxMsrGet();
    vxMsrSet(ppcMsr | _PPC_MSR_ME);

    /* do probe */

    if (mode == VX_READ)
        {
        status = sysMemProbeSup (length, adrs, pVal);
        EIEIO_SYNC;
        }
    else
        {
        status = sysMemProbeSup (length, pVal, adrs);
        EIEIO_SYNC;

        /*
         *  Flush the probe instruction out of the Raven by
         *  doing a dummy read of one of its' MPC registers
         */
 
        devId = sysIn16 ((UINT16 *)(RAVEN_BASE_ADRS + RAVEN_MPC_DEVID));
        }

    /* Disable Machine Check Exceptions */

    vxMsrSet(ppcMsr);

    /* Disable Machine Check Pin (EMCP) */

    vxHid0Set(ppcHid0);

    /* Restore previous Raven configuration */

    sysOut16 ((UINT16 *)(RAVEN_BASE_ADRS + RAVEN_MPC_MEREN), mpcErrEnbl);

    /* restore original vectors and unlock */

    excVecSet ((FUNCPTR *) _EXC_OFF_DATA, oldVec2);

    excVecSet ((FUNCPTR *) _EXC_OFF_MACH, oldVec1);

    intUnlock (oldLevel);

    return (status);
    }


/******************************************************************************
*
* sysProbeErrClr - clear errors associated with probing an address on a bus.
*
* This routine clears the error flags and conditions in the DAR, DSISR, SRR0
* and SRR1 PowerPC registers arising from probing addresses as well as the
* Raven MERST and PCI_CFG_STATUS registers and the Universe PCI_CSR and
* V_AMERR registers.
*
* RETURNS: N/A
*/

void sysProbeErrClr (void)
    {
    UINT32  pciCsr;

    /* Get current status */

    sysPciRead32 ((UINT32)(UNIVERSE_PCI_CSR), &pciCsr);

    /* Clear PCI_CSR */

    sysPciWrite32 ((UINT32)(UNIVERSE_PCI_CSR), pciCsr);

    /* Clear any VME address error */

    sysPciWrite32 ((UINT32)UNIVERSE_V_AMERR, V_AMERR_V_STAT);

    /* Force write due to Write-Posting and get updated status */

    sysPciRead32 ((UINT32)(UNIVERSE_PCI_CSR), &pciCsr);

    /* Clear Raven MPC MERST Register */

    sysOutByte ((ULONG)(RAVEN_BASE_ADRS + RAVEN_MPC_MERST),
		RAVEN_MPC_MERST_CLR);

    /* Clear Raven's Cnfg Hdr Status Reg */

    pciConfigOutWord (ravPciBusNo, ravPciDevNo, ravPciFuncNo, PCI_CFG_STATUS,
                      RAVEN_PCI_CFG_STATUS_CLR);

    /* Clear PowerPC Data Access Exception Registers */

    vxDarSet   (0);
    vxDsisrSet (0);
    vxSrr0Set  (0);
    vxSrr1Set  (0);
    }


/******************************************************************************
*
* sysVmeProbe - probe a VME bus address
*
* This routine probes an address on the VME bus.  The PCI bridge (Raven chip)
* must have a special setup to enable generation of Master Abort cycles on
* write probes and reception of Target Abort cycles on read probes.  The Raven
* MPC must be configured to generate Machine Check interrupts on Master Abort
* or Target Abort cycles.  The CPU must be configured to enable Machine Check
* exceptions.  In addition, if the probe is a write, the Universe must be
* configured to disable Posted Writes.  All probing is done with interrupts
* disabled.
*
* NOTE: This routine assumes that the Universe Local Control registers are
* dedicated to these VME address spaces:
*
* .CS
*   LSI0 - LM/SIG (mailbox)
*   LSI1 - A32
*   LSI2 - A24
*   LSI3 - A16
* .CE
*
* RETURNS: OK or ERROR if address cannot be probed
*/

STATUS sysVmeProbe
    (
    char   * adrs,	/* address to be probed */
    int      mode,	/* VX_READ or VX_WRITE */
    int      length,	/* 1, 2 or 4 byte probe */
    char   * pVal	/* address of value to write OR */
			/* address of location to place value read */
    )
    {
    STATUS status = ERROR;
    UINT32 lsiCtlReg;	/* adress of Local Control register in Universe */
    UINT32 pciSlv1Ctl;	/* Universe PCI Slave Image Control reg */

    /* Determine which Control register controls this address */

    if ((VME_A32_REG_SPACE != 0) && ((UINT32)adrs >= VME_A32_REG_BASE) &&
        ((UINT32)adrs <= (VME_A32_REG_BASE + VME_A32_REG_SPACE)))
        lsiCtlReg = (UINT32)(UNIVERSE_LSI0_CTL);
    else if ((VME_A32_MSTR_SIZE != 0) && ((UINT32)adrs >= VME_A32_MSTR_LOCAL) &&
             ((UINT32)adrs <= (VME_A32_MSTR_LOCAL + VME_A32_MSTR_SIZE)))
        lsiCtlReg = (UINT32)(UNIVERSE_LSI1_CTL);
    else if ((VME_A24_MSTR_SIZE != 0) && ((UINT32)adrs >= VME_A24_MSTR_LOCAL) &&
             ((UINT32)adrs <= (VME_A24_MSTR_LOCAL + VME_A24_MSTR_SIZE)))
        lsiCtlReg = (UINT32)(UNIVERSE_LSI2_CTL);
    else if ((VME_A16_MSTR_SIZE != 0) && ((UINT32)adrs >= VME_A16_MSTR_LOCAL) &&
             ((UINT32)adrs <= (VME_A16_MSTR_LOCAL + VME_A16_MSTR_SIZE)))
        lsiCtlReg = (UINT32)(UNIVERSE_LSI3_CTL);
    else
        return (ERROR);

    /* If write probe, disable Posted Writes in Universe */

    if (mode == VX_WRITE)
        {
        sysPciRead32 (lsiCtlReg, &pciSlv1Ctl);
        sysPciWrite32 (lsiCtlReg, (pciSlv1Ctl & ~LSI_CTL_WP));
        }

    /* Perform probe */

    status = sysMemProbeBus (adrs, mode, length, pVal, RAVEN_MPC_MEREN_RTAM);

    /* Restore Posted Writes by Universe if previously enabled */

    if ((mode == VX_WRITE) && (pciSlv1Ctl & LSI_CTL_WP))
        {
        sysPciWrite32 (lsiCtlReg, pciSlv1Ctl);
        }

    return (status);
    }


/******************************************************************************
*
* sysPciProbe - probe a PCI bus address
*
* This routine probes an address on the PCI bus.  The PCI bridge (Raven chip)
* must have a special setup to enable generation of Master Abort cycles on
* write probes and reception of Target Abort cycles on read probes.  The Raven
* MPC must be configured to generate Machine Check interrupts on Master Abort
* or Target Abort cycles.  The CPU must be configured to enable Machine Check
* exceptions.  All probing is done with interrupts disabled.
*
* RETURNS: OK or ERROR if address cannot be probed
*/

STATUS sysPciProbe
    (
    char   * adrs,	/* address to be probed */
    int      mode,	/* VX_READ or VX_WRITE */
    int      length,	/* 1, 2 or 4 byte probe */
    char   * pVal	/* address of value to write OR */
			/* address of location to place value read */
    )
    {
    STATUS status = ERROR;

    /* Perform probe */

    status = sysMemProbeBus (adrs, mode, length, pVal, RAVEN_MPC_MEREN_SMAM);

    return (status);
    }


/******************************************************************************
*
* sysBusProbe - probe a bus address based on bus type.
*
* This routine is a function hook into vxMemProbe.  It determines which bus,
* VME, PCI or local is being probed based on the address to be probed.
* If the VME bus is being probed, the sysVmeProbe() routine is called to do
* the special VME probing.  If the PCI bus is being probed, the sysPciProbe()
* routine is called to do the special PCI probing.  If the local bus is being
* probed, the routine returns ERROR which indicates that the default local
* bus probe in vxMemProbe() should be used.
*
* RETURNS: ERROR if local bus is being probed, OK if VME or PCI bus.
*/

STATUS  sysBusProbe
    (
    char   * adrs,	/* address to be probed */
    int      mode,	/* VX_READ or VX_WRITE */
    int      length,	/* 1, 2 or 4 byte probe */
    char   * pVal	/* address of value to write OR */
			/* address of location to place value read */
    )
    {
    STATUS status;

    /* Clear any existing errors/exceptions */

    sysProbeErrClr ();

    /* Handle VME bus in special manner */

    if (IS_VME_ADDRESS(adrs))
        status = sysVmeProbe (adrs, mode, length, pVal);

    /* Handle PCI bus in special manner */

    else if (IS_PCI_ADDRESS(adrs))
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
* sysGetMpuSpd - get the speed of the MPC750 processor.
*
* This routine returns the speed (in MHz) of the 60x processor
*
* RETURNS: The approximate CPU speed (inMHz).
*
* Note:  This speed returned is an approximation based on the
* accuracy of the value returned by sysGetBusSpd().
* For a 267MHz Arthur system, running with a bus clock
* of 67 MHz, the actual speed returned is 268MHz.
*
* This function is dependent upon proper bus speed being
* returned via call to sysGetBusSpd().  In addition to this
* dependency, the function will only work for CPUs whose
* speed is dependent upon the bus speed and the value in
* the PLL bits of the HID1 register.  CPUs which HAVE this
* property are 602, Arthur, 604ev and 604r.  CPUs which
* do NOT HAVE this property (and for which this function
* will NOT work) are: 601, 603, 603e, 603p, and 604.
*/

UINT sysGetMpuSpd(void)
    {
    UINT speed;
    UINT busMultiplier;	 
 
    busMultiplier = tableArthur[sysHid1Get() >> 28];
    speed = (sysGetBusSpd() * busMultiplier)/10;
    return(speed);
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
    UINT32	delay		/* length of time in microsec to delay */
    )
    {
    register UINT baselineTickCount;
    register UINT curTickCount;
    register UINT terminalTickCount;
    register int actualRollover = 0;
    register int calcRollover = 0;
    UINT ticksToWait;
    UINT requestedDelay;
    UINT oneUsDelay;

    /* Exit if no delay count */

    if ((requestedDelay = delay) == 0)
	return;

    /*
     * Get the Time Base Lower register tick count, this will be used
     * as the baseline.
     */

    baselineTickCount = sysTimeBaseLGet();

    /*
     * Calculate number of ticks equal to 1 microsecond
     *
     * The Time Base register and the Decrementer count at the same rate:
     * once per 4 System Bus cycles.
     *
     * e.g., 66666666 cycles     1 tick      1 second             16 tick
     *       ---------------  *  ------   *  --------          =  ----------
     *       second              4 cycles    1000000 microsec    microsec
     */

    oneUsDelay = ((DEC_CLOCK_FREQ / 4) / 1000000);

    /* Convert delay time into ticks */

    ticksToWait = requestedDelay * oneUsDelay;

    /* Compute when to stop */

    terminalTickCount = baselineTickCount + ticksToWait;

    /* Check for expected rollover */

    if (terminalTickCount < baselineTickCount)
	{
	calcRollover = 1;
	}

    do
	{
	/*
	 * Get current Time Base Lower register count.
	 * The Time Base counts UP from 0 to
	 * all F's.
	 */

	curTickCount = sysTimeBaseLGet();

        /* Check for actual rollover */

	if (curTickCount < baselineTickCount)
	    {
	    actualRollover = 1;
	    }

	if (((curTickCount >= terminalTickCount)
	    && (actualRollover == calcRollover)) ||
	    ((curTickCount < terminalTickCount)
            && (actualRollover > calcRollover)))
	    {
	    /* Delay time met */

	    break;
	    }

	}
    while (TRUE);
    }


