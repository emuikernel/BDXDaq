/* sysLib.c - generic PPC  system-dependent library */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01j,15oct01,dat  70855, adding sysNanoDelay
01i,08apr99,dat  SPR 26491, PCI macros
01h,09feb99,mas  negated Guard bit comment for Flash memory region (SPR 24893)
01g,02feb99,tm   added PCI AutoConfig support to template BSPs (SPR 24733)
01f,29jan99,mas  added GUARDED attributes to non-local memory areas mapped in
                 sysPhysMemDesc[]. (SPR 23674)
01e,14jan99,mas  added use of USER_D_CACHE_MODE in sysPhysMemDesc[] (SPR 24319)
01d,21jul98,db   added sysLanIntEnable(), sysLanIntDisable() and
		 sysLanEnetAddrGet(). 
01c,09jun98,dat  changed private/vmLibP.h to vmLib.h, removed motorola
		 copyright
01b,21aug97,dat  code review comments from thierrym
01a,08jul97,dat  written (from mv2603/sysLib.c, ver 01r)
*/

/*
DESCRIPTION
This library provides board-specific routines.  The chip drivers included are:

    templateSio.c - template SIO driver
    templateTimer.c - Zilog Z8536 timer library (auxiliary clock)
    nullNvRam.c - byte-oriented generic non-volatile RAM library
    nullVme.c - byte-oriented generic non-volatile RAM library
    templateIntrCtl.c - template interrrupt controller device

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
#include "logLib.h"
#include "taskLib.h"
#include "vxLib.h"
#include "tyLib.h"
#include "vmLib.h"
#include "arch/ppc/mmu603Lib.h"
#include "arch/ppc/vxPpcLib.h"

/* defines */

#define ZERO	0
#define SYS_MODEL	"template BSP"

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
    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* I BAT 2 */
    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* I BAT 3 */
    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* D BAT 0 */
    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* D BAT 1 */
    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* D BAT 2 */
    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    /* D BAT 3 */
    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_1M |
    _MMU_UBAT_VS | _MMU_UBAT_VP),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT)
    };


/*
 * Decode USER_D_CACHE_MODE to determine main memory cache mode.
 * The macro MEM_STATE will be used to specify the cache state for
 * the sysPhysMemDesc table entry that describes the main memory
 * region.
 */

#if (USER_D_CACHE_MODE & CACHE_COPYBACK)
#   define MEM_STATE VM_STATE_CACHEABLE
#elif (USER_D_CACHE_MODE & CACHE_WRITETHOUGH)
#   define MEM_STATE VM_STATE_CACHEABLE_WRITETHROUGH
#else
#   define MEM_STATE VM_STATE_CACHEABLE_NOT
#endif

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
    /* Local DRAM - MUST be second entry in table, see sysHwInit */

    (void *) RAM_LOW_ADRS,
    (void *) RAM_LOW_ADRS,
    LOCAL_MEM_SIZE -  RAM_LOW_ADRS, /* Dynamically changed by sysHwInit */
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | MEM_STATE
    },

    /* TODO - EPROM space */

#ifdef INCLUDE_PCI

    /* PCI I/O space */

    {
    (void *) PCI_MSTR_IO_LOCAL,
    (void *) PCI_MSTR_IO_LOCAL,
    PCI_MSTR_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    /* PCI MEMIO space */

    {
    (void *) PCI_MSTR_MEMIO_LOCAL,
    (void *) PCI_MSTR_MEMIO_LOCAL,
    PCI_MSTR_MEMIO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    /* PCI MEM space */

    {
    (void *) PCI_MSTR_MEM_LOCAL,
    (void *) PCI_MSTR_MEM_LOCAL,
    PCI_MSTR_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    /*
     * TODO: Add additional mappings as needed to support special PCI
     * functions like memory mapped configuration space, or special IACK
     * register space.
     */

#endif


#ifdef INCLUDE_VME
    {
    (void *) VME_A32_MSTR_LOCAL,
    (void *) VME_A32_MSTR_LOCAL,
    VME_A32_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) VME_A24_MSTR_LOCAL,
    (void *) VME_A24_MSTR_LOCAL,
    VME_A24_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    {
    (void *) VME_A16_MSTR_LOCAL,
    (void *) VME_A16_MSTR_LOCAL,
    VME_A16_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },
#endif

    /* TODO - other I/O register spaces with GUARDED bit set */

#ifdef INCLUDE_FLASH
    /* TODO - flash memory space with *NO* GUARDED bit set */
#endif

    };

int sysPhysMemDescNumEnt = NELEMENTS (sysPhysMemDesc);

int   sysBus      = VME_BUS;            /* system bus type */
int   sysCpu      = CPU;                /* system CPU type (MC680x0) */
char *sysBootLine = BOOT_LINE_ADRS;	/* address of boot line */
char *sysExcMsg   = EXC_MSG_ADRS;	/* catastrophic message area */
int   sysProcNum;			/* processor number of this CPU */
int   sysFlags;				/* boot flags */
char  sysBootHost [BOOT_FIELD_LEN];	/* name of host from which we booted */
char  sysBootFile [BOOT_FIELD_LEN];	/* name of file from which we booted */

/* locals */

LOCAL char sysModelStr[80] = SYS_MODEL;
LOCAL char wrongCpuMsg[] = WRONG_CPU_MSG; 

/* forward declarations */

void	sysSpuriousIntHandler(void);
void	sysCpuCheck (void);
char *  sysPhysMemTop (void);
int	sysGetBusSpd (void);

/* externals */

IMPORT void   sysOutByte (ULONG, UCHAR);
IMPORT UCHAR  sysInByte (ULONG);


/* BSP DRIVERS */

#ifdef INCLUDE_PCI
#   include "pci/pciConfigLib.c"
#   include "pci/pciIntLib.c"
#   ifdef INCLUDE_PCI_AUTOCONF
#       include "pci/pciAutoConfigLib.c"
#       include "./sysBusPci.c"
#   endif
#endif

#include "mem/nullNvRam.c"
#include "vme/nullVme.c"
#include "timer/templateTimer.c"

#include "sysSerial.c"
#include "sysScsi.c"
#include "sysNet.c"

#include "sysCache.c"	/* special cache setup */


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


/*******************************************************************************
*
* sysHwInit - initialize the system hardware
*
* This routine initializes various features of the board.
* It is the first board-specific C code executed, and runs with
* interrupts masked in the processor.
* This routine resets all devices to a quiescent state, typically
* by calling driver initialization routines.
*
* NOTE: Because this routine will be called from sysToMonitor, it must
* shutdown all potential DMA master devices.  If a device is doing DMA
* at reboot time, the DMA could interfere with the reboot. For devices
* on non-local busses, this is easy if the bus reset or sysFail line can
* be asserted.
*
* NOTE: This routine should not be called directly by the user application.
*
* RETURNS: N/A
*/

void sysHwInit (void)
    {
    /*
     * TODO - add initialization code for all devices and
     * for the interrupt controller.
     * Note: the interrupt controller should mask all interrupts here.
     * Interrupts are unmasked later on a per-device basis:
     *     device                     where unmasked
     *     ------                     --------------
     * abort switch, clock            sysHwInit2()
     * serial                         sysSerialHwInit2()
     * SCSI                           sysScsiInit()
     * LAN                            sysLanIntEnable()
     */

    /* Validate CPU type */

    sysCpuCheck();

#if defined(INCLUDE_MMU)
    /* run-time update of the MMU entry for main RAM */

    sysPhysMemDesc[1].len = (UINT)(sysPhysMemTop () - RAM_LOW_ADRS);
#endif

#ifdef INCLUDE_NETWORK
    sysNetHwInit();		/* network interface */
#endif

#ifdef INCLUDE_SERIAL
    sysSerialHwInit();		/* serial devices */
#endif

#ifdef INCLUDE_VME
    /* TODO - any VME hardware setup/reset */
#endif

#ifdef INCLUDE_PCI
    /* TODO - PCI hardware setup/reset */
#endif

#ifdef INCLUDE_FLASH
    /* TODO - any Flash ROM hardware setup/reset */
#endif
    }

/*******************************************************************************
*
* sysHwInit2 - initialize additional system hardware
*
* This routine connects system interrupt vectors and configures any
* required features not configured by sysHwInit. It is called from usrRoot()
* in usrConfig.c after the multitasking kernel has started.
*
* RETURNS: N/A
*/

void sysHwInit2 (void)
    {
    static int	initialized;	/* must protect against double call! */

    if (!initialized)
	{
	initialized = TRUE;

	/*
         * TODO - connect timer and abort vector interrupts and enable
	 * those interrupts as needed.
         */

#ifdef INCLUDE_NETWORK
	sysNetHwInit2 ();	/* network interface */
#endif

#ifdef INCLUDE_SERIAL
	sysSerialHwInit2 ();	/* connect serial interrupts */
#endif

#ifdef INCLUDE_VME
	/* TODO - any secondary VME setup */
#endif

#ifdef INCLUDE_PCI
	/* TODO - any secondary PCI setup */
#ifdef INCLUDE_PCI_AUTOCONF
	sysPciAutoConfig();
#if defined(INCLUDE_NETWORK) && defined(INCLUDE_END)
       /* TODO - update load string routine */
#endif /* defined(INCLUDE_NETWORK) && defined(INCLUDE_END) */
#endif /* INCLUDE_PCI_AUTOCONF */
#endif /* INCLUDE_PCI */

	/* L2 Cache setup */

#if	defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2)
#  ifdef USER_L2_CACHE_ENABLE
	sysL2CacheInit ();
#  else
	sysL2CacheDisable ();
#  endif
#endif /*(INCLUDE_CACHE_SUPPORT) && (INCLUDE_CACHE_L2)*/

	}
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
    static char * physTop = NULL;

    if (physTop == NULL)
	{
#ifdef LOCAL_MEM_AUTOSIZE

	/* TODO - If auto-sizing is possible, this would be the spot.  */

#	error	"Dynamic memory sizing not supported"

#else
	/* Don't do autosizing, if size is given */

	physTop = (char *)(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE);

#endif /* LOCAL_MEM_AUTOSIZE */
	}

    return physTop;
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

/*******************************************************************************
*
* sysToMonitor - transfer control to the ROM monitor
*
* This routine transfers control to the ROM monitor.  Normally, it is called
* only by reboot()--which services ^X--and by bus errors at interrupt level.
* However, in some circumstances, the user may wish to introduce a
* <startType> to enable special boot ROM facilities.
*
* The entry point for a warm boot is defined by the macro ROM_WARM_ADRS
* in config.h.  We do an absolute jump to this address to enter the
* ROM code.
*
* RETURNS: Does not return.
*/

STATUS sysToMonitor
    (
    int startType    /* parameter passed to ROM to tell it how to boot */
    )
    {
    FUNCPTR pRom = (FUNCPTR) (ROM_WARM_ADRS);

    intLock ();			/* disable interrupts */

    cacheDisable (0);   	/* Disable the Instruction Cache */
    cacheDisable (1);   	/* Disable the Data Cache */

#if     (CPU == PPC604)
    vxHid0Set (vxHid0Get () & ~_PPC_HID0_SIED);	/* Enable Serial Instr Exec */
#endif  /* (CPU == PPC604) */

    sysHwInit ();		/* disable all sub systems to a quiet state */
    vxMsrSet (0);		/* Clear the MSR */

    (*pRom) (startType);	/* jump to romInit.s */

    return (OK);		/* in case we continue from ROM monitor */
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
* For bus systems, it is assumes that processor 0 is the bus master and
* exports its memory to the bus.
*
* RETURNS: N/A
*
* SEE ALSO: sysProcNumGet()
*/

void sysProcNumSet
    (
    int procNum			/* processor number */
    )
    {
    sysProcNum = procNum;

    if (procNum == 0)
        {
#ifdef INCLUDE_VME
	/* TODO - one-time setup of slave window control registers */
#endif

#ifdef INCLUDE_PCI
	/* TODO - Enable/Initialize the interface as bus slave */
#endif
	}
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

    if	((CPU_TYPE == CPU_TYPE_604) || (CPU_TYPE == CPU_TYPE_604E))
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
* sysLanIntEnable - enable the LAN interrupt
*
* This routine enables interrupts at a specified level for the on-board LAN
* chip.
*
* RETURNS: OK, or ERROR if network support not included.
*
* SEE ALSO: sysLanIntDisable()
*/

STATUS sysLanIntEnable
    (
    int intLevel 		/* interrupt level to enable */
    )
    {
#ifdef INCLUDE_NETWORK

    /* TODO - enable lan interrupt */
    
    return (OK);

#else

    return (ERROR);

#endif /* INCLUDE_NETWORK */
    }


/******************************************************************************
*
* sysLanIntDisable - disable the LAN interrupt
*
* This routine disables interrupts for the on-board LAN chip. 
*
* RETURNS: OK, or ERROR if network support not included.
*
* SEE ALSO: sysLanIntEnable()
*/

STATUS sysLanIntDisable
    (
    int intLevel 		/* interrupt level to enable */
    )
    {
#ifdef INCLUDE_NETWORK

    /* TODO - disable lan interrupt */
    
    return (OK);

#else

    return (ERROR);

#endif /* INCLUDE_NETWORK */
    }

/******************************************************************************
*
* sysLanEnetAddrGet - retrieve ethernet address.
*
* This routine returns a six-byte ethernet address for a given ethernet unit.
* The template END driver uses this routine to obtain the ethernet address if
* indicated by a user-flag in END_LOAD_STRING in configNet.h; or if the
* reading the ethernet address ROM is unsuccessful.
*
* RETURNS: OK or ERROR if ROM has valid standard ethernet address
*/

STATUS sysLanEnetAddrGet
    (
    int		unit,
    char *	enetAdrs
    )
    {
    /* TODO -The default return code is ERROR, change function if necessary. */
    
    return (ERROR);
    }


volatile static int sysNanoDummy = 1; /* dummy variable for spin loop */

/******************************************************************************
*
* sysNanoDelay - delay for specified number of nanoseconds
*
* This function implements a spin loop type delay for at
* least the specified number of nanoseconds.  This is not a task delay,
* control of the processor is not given up to another task.  The actual delay
* will be equal to or greater than the requested number of nanoseconds.
*
* The purpose of this function is to provide a reasonably accurate time delay
* of very short duration.  It should not be used for any delays that are much
* greater than two system clock ticks in length.  For delays of a full clock
* tick, or more, the use of taskDelay() is recommended.
*
* This routine is interrupt safe.
*
* .nT
*
* RETURNS: N/A.
*
* SEE ALSO:
* .sA
*/ 

void sysNanoDelay
    (
    UINT32 nanoseconds	/* nanoseconds to delay */
    )
    {
    int i;

    /*
     * TODO - setup a calibrated spin loop.
     *
     * In this example code, we assume that one pass of the spin
     * loop takes 1 microsecond for this BSP and processor.  Be sure
     * to use 'volatile' as appropriate to force the compiler to 
     * not optimize your code.
     *
     * Must be interrupt safe.
     *
     * To calibrate, set SYS_LOOP_NANO to 1.  Measure the elapsed time
     * for sysNanoDelay(1000000) to execute.  Set SYS_NANO_LOOP to the
     * the number of milliseconds measured (round up to integer!).
     * (or measure time for sysNanoDelay(1000) and set SYS_NANO_LOOP
     * to the measured number of microseconds, rounded up to an integer).
     */

#define SYS_LOOP_NANO	1000	/* loop time in nanoseconds */

    sysNanoDummy = 1;
    for (i = 0; i < (nanoseconds/SYS_LOOP_NANO) + 1; i++)
	{
	sysNanoDummy += 1;
	sysNanoDummy *= 2;
	}
    }
