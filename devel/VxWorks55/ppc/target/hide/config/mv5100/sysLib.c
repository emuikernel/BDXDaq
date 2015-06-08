/* sysLib.c - Motorola MVME5100 board series system-dependent library */

/* Copyright 1984-2002 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01q,17jun02,kab  Clean up from review:
		 SPR 73642: sysBusTas* undefined symbols
01p,14mar02,kab  SPR 73218: ^X does not reboot
01o,30jan02,kab  Added MPC7410 (Nitro) support
01n,08nov01,kab  Changed CPU_TYPE_MAX (confusing codename) to CPU_TYPE_7400
                 Changed from Location Monitor label names to Unverise II
                 register access image naming (LM --> RAI) of labels.
		 Cleanup for T2.2
01m,15may01,pch  Move call of usrAltivecInit() from sysLib.c to usrConfig.c,
                 to fix an undefined reference when building bootrom.
01l,10may01,kab  Replaced calls to altivecInit(); altivecShowInit() with
		 usrAltivecInit()
01k,25apr01,srr  Change reference to global pciAutoConfigLib.
01l,16apr01,pcs  Moved altivecInit to sysHwInit2.
01k,02apr01,dtr  Porting altivec bsp changes from sp7400.
01j,09mar01,rhk  Additional Wind River coding standards changes.
01j,03nov00,rcs  fix path to pciAutoConfigLib.c
01i,16aug00,dmw  Wind River coding standards review.
01h,26may00,dmw  RTC Wathcdog support.
01g,12may00,dmw  Turned on board fail LED in sysToMonitor.
01f,08may00,dmw  Moved ROM point up by 4 in sysToMonitor for MAX errata.
01e,28apr00,srr  Removed debug definition of INCLUDE_SHOW_ROUTINES.
01d,19apr00,dmw  Added Max errata fix, vmxExcLoad.
01c,10apr00,dmw  Added mv2100 BSP's version of VME support.
01b,03apr00,dmw  Included "latest" ./pci/pciAutoConfigLib.h.
01a,10mar00,dmw  written. (from ver 01a of mv2400 bsp)
*/

/*
DESCRIPTION
This library provides board-specific routines.  The chip drivers included are:

    i8250Sio.c - Intel 8250 UART driver 
    ns8730xSuperIo.c	- Super I/O chip initialization
    ppcDecTimer.c - PowerPC decrementer timer library (system clock)
    byteNvRam.c - byte-oriented generic non-volatile RAM library
    pciConfigLib.c - PCI configuration library
    universe.c - Tundra Universe chip VME-to-PCI interface library
    HawkAuxClk.c - Hawk timer library (auxiliary clock)
    hawkMpic.c - Hawk Mpic / W83C553 PIB/IBC Interrupt Controller

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

#ifdef INCLUDE_ALTIVEC
#include "altivecLib.h"
IMPORT int       (* _func_altivecProbeRtn) () ;
#endif /* INCLUDE_ALTIVEC */

/* defines */

#define ZERO    0

#define DECREMENTER_PRESCALE    4
#define USECS_PER_SECOND        1000000

#define HAWK_PHB_ADDR( reg ) ( HAWK_PHB_BASE_ADRS + reg )

#ifdef INCLUDE_BPE
#   define HAWK_SMC_ADDR( reg ) ( HAWK_SMC_BASE_ADRS + reg )
#   define _PPC_HID0_DBP _PPC_HID0_ECPC
#endif /* INCLUDE_BPE */

/* globals */

/******************************************************************************
*
* NOMANUAL
*
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

    ((ROM_BASE_ADRS & _MMU_UBAT_BEPI_MASK) | (_MMU_UBAT_BL_1M & /* I BAT 0 */
    ~(_MMU_UBAT_VS & _MMU_UBAT_VP))),
    ((ROM_BASE_ADRS & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |
    _MMU_LBAT_CACHE_INHIBIT),

    0, 0, /* I BAT 1 */

    0, 0, /* I BAT 2 */

    0, 0, /* I BAT 3 */

    0, 0, /* D BAT 0 */

    0, 0, /* D BAT 1 */

    0, 0, /* D BAT 2 */

    0, 0  /* D BAT 3 */
    };


/******************************************************************************
*
* NOMANUAL
*
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
*
* *** EXTENDED_VME configuration ***
*
* The user can use TLBs, and/or BATs, to map VME A32 space to the processor.
* The default is to use TLBs (MMU).  Change the table entry below to use
* a different method. (See "MODIFY A32 VME WINDOW HERE")
*/

PHYS_MEM_DESC sysPhysMemDesc [] =
    {

    /* Vector Table and Interrupt Stack */

    {
    (void *) LOCAL_MEM_LOCAL_ADRS,
    (void *) LOCAL_MEM_LOCAL_ADRS,
    RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    /* Local DRAM - Must be second entry in sysPhysMemDesc for Auto Sizing */

    {
    (void *) RAM_LOW_ADRS,
    (void *) RAM_LOW_ADRS,
    LOCAL_MEM_SIZE -  RAM_LOW_ADRS,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_MEM_COHERENCY,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE |
    VM_STATE_MEM_COHERENCY
    },

    /* Access to PCI ISA I/O space */

    {
    (void *) ISA_MSTR_IO_LOCAL,
    (void *) ISA_MSTR_IO_LOCAL,
    ISA_MSTR_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /* Access to PCI I/O space */

    {
    (void *) PCI_MSTR_IO_LOCAL,
    (void *) PCI_MSTR_IO_LOCAL,
    PCI_MSTR_IO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /*
     * Access to PCI memory space. Note: Guarding should not be required since
     * memory is well-behaved (no side-effects on read or write)
     */

    {
    (void *) PCI_MSTR_MEM_LOCAL,
    (void *) PCI_MSTR_MEM_LOCAL,
    PCI_MSTR_MEM_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    /* Access to PCI memory I/O space */

    {
    (void *) PCI_MSTR_MEMIO_LOCAL,
    (void *) PCI_MSTR_MEMIO_LOCAL,
    PCI_MSTR_MEMIO_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /* MODIFY A32 VME WINDOW HERE */

    {
    (void *) VME_A32_MSTR_LOCAL,
    (void *) VME_A32_MSTR_LOCAL,
    VME_A32_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /* A24 VME WINDOW */

    {
    (void *) VME_A24_MSTR_LOCAL,
    (void *) VME_A24_MSTR_LOCAL,
    VME_A24_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /* Off-board VME Universe Register Access Images */

    {
    (void *) VME_RAI_MSTR_LOCAL,
    (void *) VME_RAI_MSTR_LOCAL,
    VME_RAI_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT
    },

    /* A16 VME WINDOW */

    {
    (void *) VME_A16_MSTR_LOCAL,
    (void *) VME_A16_MSTR_LOCAL,
    VME_A16_MSTR_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /* MPIC Regs */

    {
    (void *) MPIC_BASE_ADRS,
    (void *) MPIC_BASE_ADRS,
    MPIC_REG_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /* HAWK SMC Registers */

    {
    (void *) HAWK_SMC_BASE_ADRS,
    (void *) HAWK_SMC_BASE_ADRS,
    HAWK_SMC_REG_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
    VM_STATE_GUARDED
    },

    /* HAWK PHB Registers */

    {
    (void *) HAWK_PHB_BASE_ADRS,
    (void *) HAWK_PHB_BASE_ADRS,
    HAWK_PHB_REG_SIZE,
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE |
    VM_STATE_MASK_GUARDED,
    VM_STATE_VALID      | VM_STATE_WRITABLE      | VM_STATE_CACHEABLE_NOT |
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

int    sysBus      = VME_BUS;         /* system bus type */
int    sysCpu      = CPU;             /* system CPU type (MC680x0) */
char * sysBootLine = BOOT_LINE_ADRS;  /* address of boot line */
char * sysExcMsg   = EXC_MSG_ADRS;    /* catastrophic message area */
int    sysProcNum;                    /* processor number of this CPU */
int    sysFlags;                      /* boot flags */
char   sysBootHost [BOOT_FIELD_LEN];  /* name of host from which we booted */
char   sysBootFile [BOOT_FIELD_LEN];  /* name of file from which we booted */
UINT   sysVectorIRQ0  = INT_VEC_IRQ0; /* vector for IRQ0 */
BOOL   sysVmeEnable   = FALSE;        /* by default no VME */
static int   sysHawkPciBusNo;         /* Hawk Config Space BDF address */
static int   sysHawkPciDevNo;
static int   sysHawkPciFuncNo;

unsigned char clearWd [1]  = { 0x00 };

/* locals */

LOCAL char sysModelStr[80];
LOCAL char wrongCpuMsg[] = WRONG_CPU_MSG; 

/* forward declarations */

void    sysSpuriousIntHandler (void);
void    sysCpuCheck (void);
char *  sysPhysMemTop (void);
STATUS  sysPciExtHawkInit (int, int, int);
UCHAR   sysNvRead (ULONG);
void    sysNvWrite (ULONG, UCHAR);
void    sysBusTasClear (volatile char *);
STATUS  sysBusProbe (char *, int, int, char *);
IMPORT UINT sysHid1Get (void);

#ifdef ANY_BRDS_IN_CHASSIS_NOT_RMW
LOCAL BOOL sysVmeVownTas (char *);
#endif /* ANY_BRDS_IN_CHASSIS_NOT_RMW */

LOCAL   BOOL sysVmeRmwTas (char *);
void    sysConfigBpe (void);
void    reportBootromErrors (void);

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
IMPORT UCHAR  sysProductStr[];

/* BSP DRIVERS */


#if defined (INCLUDE_PCI)
#    include "pci/pciConfigLib.c"       /* PCI config space access */
#    include "pci/pciIntLib.c"          /* PCI int support */
#    ifdef INCLUDE_SHOW_ROUTINES
#       include "pci/pciConfigShow.c"   /* PCI config space display */
#    endif /* INCLUDE_SHOW_ROUTINES */
#    ifdef INCLUDE_PCI_AUTOCONF         /* Yellowknife uses autoconfig */
#       include "./pci/pciAutoConfigLib.c"/* automatic PCI configuration */
#       include "sysBusPci.c"           /* pciAutoConfig BSP support file */
#    endif /*  INCLUDE_PCI_AUTOCONF */
#endif /* INCLUDE_PCI */



#ifdef INCLUDE_NETWORK
#   include "./sysNet.c"
#   ifdef INCLUDE_END
#       ifdef INCLUDE_MCG_END
#      include "./sysEnd.c"
#       endif /* INCLUDE_MCG_END */
#       ifdef INCLUDE_FEI_END
#           include "./sysFei82557End.c"
#       endif /* INCLUDE_FEI_END */
#   endif /* INCLUDE_END */
#endif /* INCLUDE_NETWORK */

#include "sysSerial.c"
#include "mem/byteNvRam.c"
#include "sysMotVpd.c"
#include "timer/ppcDecTimer.c"      /* PPC603 & 604 have on chip timers */

#ifdef INCLUDE_SCSI
#   include "sysScsi.c"			/* sysScsiInit routine */
#endif /* INCLUDE_SCSI */

#ifdef INCLUDE_SHOW_ROUTINES
#   include "sysMotVpdShow.c"
#   include "hawkSmcShow.c"
#endif /* INCLUDE_SHOW_ROUTINES */

#include "sysCache.c"
#include "ns8730xSuperIo.c"
#include "universe.c"
#include "hawkPhb.c"
#include "hawkMpic.c"

#ifdef INCLUDE_HAWK_AUXCLK
#  include "hawkAuxClk.c"
#endif  /* INCLUDE_HAWK_AUXCLK */

/* M48t59 watchdog timer support. */

#ifdef INCLUDE_M48T559WDT
#  include "m48tWdt.c"
#endif /* INCLUDE_M48T559WDT */

/* defines for sysBusTas() and sysBusTasClear() */

#define VMEBUS_OWNER        (*UNIVERSE_MAST_CTL & LONGSWAP(MAST_CTL_VOWN_ACK))
#define CPU_CLOCKS_PER_LOOP 10
#define LOCK_TIMEOUT        10
#define UNLOCK_TIMEOUT      10000

#if     defined (INCLUDE_ALTIVEC)
/*******************************************************************************
*
* sysAltivecProbe - Check if the CPU has ALTIVEC unit.
*
* This routine returns OK it the CPU has an ALTIVEC unit in it.
* Presently it checks for 7400 & 7410
* RETURNS: OK  -  for 7400/7410 Processor type
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
          case CPU_TYPE_7400:
          case CPU_TYPE_7410:
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
        case CPU_TYPE_7400:
	    if (CPU_REV < CPU_REV_7410)
	        sprintf(cpuStr, "7400");
	    else
                sprintf(cpuStr, "7410");
            break;
	case CPU_TYPE_7410:
	    sprintf(cpuStr, "7410");
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
  int         pciBusNo;       /* PCI bus number */
  int         pciDevNo;       /* PCI device number */
  int         pciFuncNo;      /* PCI function number */
  char txt[100];

    /* Do the Hawk PCI-Host Bridge phase 1 init */

    (void) sysHawkPhbInit ();

    /* Initialize the VPD information */

    (void) sysVpdInit ();

    /* Do the Hawk PCI-Host Bridge phase 2 init */

    (void) sysHawkPhbInit2 ();

    /* Validate CPU type */

    sysCpuCheck();

    /*
     * If MPC7400/7410 (Max/Nitro):
     *     Setup  exception addresses.
     *     Disable & invalidate if L2 enabled.
     */

    if ((CPU_TYPE == CPU_TYPE_7400) || (CPU_TYPE == CPU_TYPE_7410))
        {
#if defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2)
        sysL2CacheDisable ();
#endif /* defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2) */
        }
	
    /* Super I/O init */

    ns8730xSuperIoInit ();

    /* Initialize PCI driver library. */

    if (pciConfigLibInit (PCI_MECHANISM_1, PCI_PRIMARY_CAR, PCI_PRIMARY_CDR, 0)
        != OK)
        {
        sysToMonitor (BOOT_NO_AUTOBOOT);
        }

    if ((pciIntLibInit ()) != OK)
        {
        sysToMonitor (BOOT_NO_AUTOBOOT);
        }

#ifdef INCLUDE_PCI_AUTOCONF
    /*
     * Test to determine if we need to configure the PCI busses with
     * sysPciAutoConfig().  If we are coming up from a ROM-based image
     * then we need to reconfigure.  If we have been booted from a ROM
     * image then we don't need to reconfigure since the bootrom will
     * already have reconfigured the PCI busses.  We must avoid
     * configuring the PCI busses twice on startup.
     */

    if ( !PCI_AUTOCONFIG_DONE )
    {
        /* in ROM boot phase, OK to continue and configure PCI busses. */



/* Sergey: just wait for a while so prpmc880 will complete its initialization */
sprintf(txt,"sysHwInit: wait 40 sec for prpmc880 initialization \r\n ");
sysDebugMsg(txt,CONTINUE_EXECUTION);
        sysUsDelay(40000000);
sprintf(txt,"sysHwInit: continue \r\n ");
sysDebugMsg(txt,CONTINUE_EXECUTION);



        sysPciAutoConfig();

/* Sergey: fake prpmc880 enumeration - until sysPciAutoConfig() will do it */
/* assume that prpmc880 is bus=0 dev=0x10 (write something but not 0xe0000000) */
pciConfigOutLong(0,0x10,0,0x10,0xc0000000);

        PCI_AUTOCONFIG_FLAG = TRUE;     /* Remember that PCI is configured */
        }
#endif /*  INCLUDE_PCI_AUTOCONF */

    /* Get the base address for the Universe registers if present */

    if (pciFindDevice ((PCI_ID_UNIVERSE & 0xFFFF),
                        (PCI_ID_UNIVERSE >> 16) & 0xFFFF, 0,
                        &pciBusNo, &pciDevNo, &pciFuncNo) != ERROR)
        {
        pciConfigInLong(pciBusNo, pciDevNo, pciFuncNo,
                        PCI_CFG_BASE_ADDRESS_0,
                         &univBaseAdrs);
        if (univBaseAdrs & PCI_BAR_SPACE_IO)
            {
            univBaseAdrs = (univBaseAdrs & PCI_IOBASE_MASK) - PCI_MSTR_IO_BUS
                            + PCI_MSTR_IO_LOCAL;
            }
        else
            {
            univBaseAdrs = (univBaseAdrs & PCI_MEMBASE_MASK) - PCI_MSTR_MEM_BUS
                            + PCI_MSTR_MEM_LOCAL;
            }

        /*
         *  Initialize the non-PCI Config Space registers of the
         *  Universe which doesn't have a true device driver.
         */

        sysUniverseInit();
        }

    /*
     *  The Hawk powers up with the correct PCI Header values.
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

        /*
         *  Initialize the non-PCI Config Space registers of the
         *  IBC which doesn't have a true device driver.
         */

        sysIbcInit();

        }

    /*
     *  Initialize the extended portion of the Hawk's PCI Header.
     *    The non-PCI Config Space registers have already been
     *    initialized in the initial startup code.
     */

    if (pciFindDevice ((PCI_ID_HAWK & 0xFFFF), (PCI_ID_HAWK >> 16) & 0xFFFF,
                       0, &sysHawkPciBusNo, &sysHawkPciDevNo,
                       &sysHawkPciFuncNo) != ERROR)
        {
        (void)sysHawkInitPhbExt (sysHawkPciBusNo, sysHawkPciDevNo, 
                                 sysHawkPciFuncNo);
        }

    /* Reset the MPIC. */

    sysPciWrite32( MPIC_ADDR(MPIC_GLOBAL_CONFIG_REG), RESET_CNTRLR );
 
    /* Initialize the non-PCI Config Space registers of the Hawk Mpic. */

    sysMpicInit();

#ifdef  INCLUDE_VME_DMA

    /*  Initialize the VMEbus DMA driver */

    sysVmeDmaInit();

#endif  /* INCLUDE_VME_DMA */

    /* set shared memory TAS Clear routine pointer */

    smUtilTasClearRtn = (VOIDFUNCPTR)sysBusTasClear;

    /* set pointer to bus probing hook */

    _func_vxMemProbeHook = (FUNCPTR)sysBusProbe;

    /* Initialize COM1 and COM2 serial channels */

    sysSerialHwInit();

    /* Disable the watchdog timer */

    sysNvWrite ((ULONG)WD_TIMER, 0);

    /*
     * If mmu tables are used, this is where we would dynamically
     * update the entry describing main memory, using sysPhysMemTop().
     * We must call sysPhysMemTop () at sysHwInit() time to do
     * the memory autosizing if available.
     */

    sysPhysMemTop ();

    /* clear Hawk error conditions */

    sysHawkErrClr ();

    /* Upon completion, clear BFL (Board Fail) LED */

    *(UINT8 *)BRD_MOD_FAIL_REG_ADRS &= ~BRD_MOD_FAIL_MASK;
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
    static char * sysPhysMemSize = NULL;    /* ptr to top of mem + 1 */
#ifdef LOCAL_MEM_AUTOSIZE
    UINT32        dramIndex;
    int           i, j;

    static UINT32 dramSize[] =
        {
        0x00000000,            /*   0MB */
        0x02000000,            /*  32MB (4Mx16) */
        0x04000000,            /*  64MB (8Mx8) */
        0x04000000,            /*  64MB (8Mx16) */
        0x08000000,            /* 128MB (16Mx4) */
        0x08000000,            /* 128MB (16Mx8) */
        0x08000000,            /* 128MB (16Mx16) */
        0x10000000,            /* 256MB (32Mx4) */
        0x10000000,            /* 256MB (32Mx8) */
        0x20000000,            /* 512MB (64Mx4) */
        0x40000000,            /*   1GB (64Mx8) */
        0x80000000,            /*   2GB (128x4) */
        0x00000000,            /*   0MB reserved */
        0x00000000,            /*   0MB reserved */
        0x00000000,            /*   0MB reserved */
        0x00000000,            /*   0MB reserved */
        };

    static UINT8 *dramAttr[] =
        {
        (UINT8 *) HAWK_SMC_SDRAM_ATTR_AD,
        (UINT8 *) HAWK_SMC_SDRAM_ATTR_EH,
        };
#endif /* LOCAL_MEM_AUTOSIZE */

    if (sysPhysMemSize == NULL)
        {
#ifdef LOCAL_MEM_AUTOSIZE
        /*
         * Do dynamic memory sizing.
         *
         * Since Hawk memory controller chip has already been set to
         * control all memory, just read and interpret its DRAM Attributes
         * Register.
         */

        for (i = 0; i < NELEMENTS(dramAttr); i++)
            {
            for (j = 0; j < 4; j++)
                {
                if (dramAttr[i][j] & 0x80)
                    {
                    dramIndex = dramAttr[i][j] & 0x0F;
                    (UINT32)sysPhysMemSize += dramSize [dramIndex];
                    }
                }
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
                "\r\nERROR: Increase VME_A32_MSTR_LOCAL to at least 0x%x.\r\n",
                (UINT)sysMemTop()); 

            sysDebugMsg (dbgMsg, CONTINUE_EXECUTION);

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
    int startType    /* parameter passed to ROM to tell it how to boot */
    )
    {
    FUNCPTR pRom = (FUNCPTR) (ROM_TEXT_ADRS + 8); /* Warm reboot (for MAX) */

    /* L2 disabled via cacheLib */
    cacheDisable (0);        /* Disable the Instruction Cache */
    cacheDisable (1);        /* Disable the Data Cache */

    sysUniverseReset ();     /* reset Universe chip */
    sysSerialReset ();       /* reset serial devices */
    sysHawkErrClr ();        /* clear Hawk error conditions */

    /* Clear the MSR */

    vxMsrSet (0);

    /* set the Board Fail LED */

    *(UINT8 *)BRD_MOD_FAIL_REG_ADRS |= BRD_MOD_FAIL_MASK;

    (*pRom) (startType);

    return (OK);             /* in case we ever continue from ROM monitor */
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

#ifdef INCLUDE_HAWK_AUXCLK
        sysAuxClkInit ();
        intConnect (INUM_TO_IVEC(TIMER0_INT_VEC), sysAuxClkInt, 0);
        intEnable (TIMER0_INT_LVL);
#endif /* INCLUDE_HAWK_AUXCLK */

       /* initialize fei network interface */
#ifdef INCLUDE_FEI_END
	sys557PciInit ();
#endif

        /* initialize serial interrupts */

        sysSerialHwInit2();

        /* connect a dummy routine for the spurious interrupt (0x07) */

        intConnect (INUM_TO_IVEC(PP_INT_LVL), sysSpuriousIntHandler, 0);

#ifdef INCLUDE_BPE

        /* enable processor data and address bus parity checking */

        sysConfigBpe ();
#endif /* INCLUDE_BPE */

#if defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2)
#   ifdef USER_L2_CACHE_ENABLE
        /* initialize the L2 cache */

        sysL2CacheInit();
#   else
        sysL2CacheDisable();
#   endif /* USER_L2_CACHE_ENABLE */
#endif /* defined(INCLUDE_CACHE_SUPPORT) && defined(INCLUDE_CACHE_L2) */

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
* the VMEbus.
*
* RETURNS: N/A
*
* SEE ALSO: sysProcNumGet()
*/

void sysProcNumSet
    (
    int     procNum            /* processor number */
    )
    {

    /*
     *    Init global variable - this needs to be done before
     *    calling sysUniverseInit2() because it calls sysProcNumGet()
     *    via the MACRO definition.
     */

    sysProcNum = procNum;

    /* Set up the node's VME slave decoders. */

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
    int     adrsSpace,    /* bus address space where busAdrs resides */
    char *    localAdrs,    /* local address to convert */ 
    char **    pBusAdrs    /* where to return bus address */ 
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
            else
                return (ERROR);

        case VME_AM_STD_SUP_PGM:
        case VME_AM_STD_SUP_DATA:
        case VME_AM_STD_USR_PGM:
        case VME_AM_STD_USR_DATA:
            if ((VME_A24_SLV_SIZE != 0) &&
                ((ULONG)localAdrs >= VME_A24_SLV_LOCAL) &&
                ((ULONG)localAdrs < (VME_A24_SLV_LOCAL + VME_A24_SLV_SIZE)))
                {
                *pBusAdrs = localAdrs + (VME_A24_SLV_BUS - VME_A24_SLV_LOCAL);
                return (OK);
                }
            else
                return (ERROR);

        case VME_AM_SUP_SHORT_IO:
        case VME_AM_USR_SHORT_IO:
            if ((VME_A16_SLV_SIZE != 0) &&
                ((ULONG)localAdrs >= VME_A16_SLV_LOCAL) &&
                ((ULONG)localAdrs < (VME_A16_SLV_LOCAL + VME_A16_SLV_SIZE)))
                {
                *pBusAdrs = localAdrs + (VME_A16_SLV_BUS - VME_A16_SLV_LOCAL);
                return (OK);
                }
            else
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
    int      adrsSpace,     /* bus address space where busAdrs resides */
    char *    busAdrs,       /* bus address to convert */
    char **    pLocalAdrs     /* where to return local address */
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

        /* This handles the VME register access image window */

        if (((ULONG) busAdrs >= VME_RAI_MSTR_BUS) &&
            ((ULONG) busAdrs < (VME_RAI_MSTR_BUS +
                   VME_RAI_MSTR_SIZE)))
            {
            *pLocalAdrs = (char *)busAdrs + (VME_RAI_MSTR_LOCAL -
                                 VME_RAI_MSTR_BUS);
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
            if ((VME_A16_MSTR_SIZE == 0) ||
                ((ULONG) busAdrs < VME_A16_MSTR_BUS) ||
                ((ULONG) busAdrs >= (VME_A16_MSTR_BUS + VME_A16_MSTR_SIZE)))
                {
                return (ERROR);
                }

            *pLocalAdrs = (char *) busAdrs + 
                (VME_A16_MSTR_LOCAL - VME_A16_MSTR_BUS);
            return (OK);

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
*        Master        Slave_1        Slave_2
*
* VME Chip    Don't Care    U1        U1
*        ----------    ----        ----
* Method    VOWN        VOWN        VOWN
*
* VME Chip    Don't Care    U1        U2
*        ----------    ----        ----
* Method    VOWN        VOWN        RMW
*
* VME Chip    U1 or        U2        U2
*        U2+
*        ----------    ----        ----
* Method    VOWN+++        RMW        RMW
*
* VME Chip    U2++        U2        U2
* PCI Bridge    or Hawk
*        ----------    ----        ----
* Method    lwarx/stwcx    RMW        RMW
*
*  +   = Refer to target.txt file for explaination of older boards
*     with Universe II (U2).
*  ++  = Refer to target.txt file for explaination of newer boards
*     with Universe II (U2).
*  +++ = The master's hardware does not preserve the atomic RMW.
* ++++ = If SM_OFF_BOARD == TRUE,  the method used will be the same
*     as if the master board is acting like a slave board; namely:
*         RMW for UNIVERSE_II and VOWN for UNIVERSE_I
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
#ifdef ANY_BRDS_IN_CHASSIS_NOT_RMW

        /*
         * A slave board in the chassis cannot generate a VMEbus RMW,
         * and/or the master board cannot translate an incoming VMEbus
         * RMW into an atomic operation, and/or the master board cannot
         * generate a VMEbus RMW; therefore, the master must set
         * VOWN before doing a TAS.
         */

        return (sysVmeVownTas(adrs));

#else /* ANY_BRDS_IN_CHASSIS_NOT_RMW */
#   if (SM_OFF_BOARD == FALSE)
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

#   else /* SM_OFF_BOARD */
        /* The master board can generate a VMEbus RMW */

        return (sysVmeRmwTas(adrs));

#   endif /* SM_OFF_BOARD */

#endif /* ANY_BRDS_IN_CHASSIS_NOT_RMW */
        }
    else    /* Slave node */
        {

        /* A board with the UNIVERSE_II can generate a VMEbus RMW */

        return (sysVmeRmwTas(adrs));
        }
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
    UINT32 ticksToWait;      /* number of time base ticks to wait */
    UINT32 startTick;     /* starting timebase low tick value */
    BOOL state;
    int  lockKey;        /* interrupt lock key */

#ifdef    ANY_BRDS_IN_CHASSIS_NOT_RMW

    /*
     * A slave board in the chassis cannot generate a VMEbus RMW,
     * and/or the master board cannot translate an incoming VMEbus
     * RMW into an atomic operation; therefore, the master must set
     * VOWN before clearing a local TAS location.
     */

    /* Check for master node or slave node with UNIVERSE_I */

    if (sysProcNumGet() == 0)
        {

        /*
         * convert LOCK_TIMEOUT to timebase ticks as follows:
         *
         * ticks/Usec = (bus frequency / decrementer pre-scaler) / 1000000
         *
         * ticks to wait = LOCK_TIMEOUT (in Usecs) * ticks/USec. 
         */

        ticksToWait = LOCK_TIMEOUT * 
                      ((DEC_CLOCK_FREQ/DECREMENTER_PRESCALE)/USECS_PER_SECOND);

        /* do clearing with no interference */

        lockKey = intLock ();

        /* request ownership of the VMEbus */

        *UNIVERSE_MAST_CTL |= LONGSWAP(MAST_CTL_VOWN);
        EIEIO_SYNC;

        /* snapshot starting time base low value */

        startTick = sysTimeBaseLGet ();

        /* Wait for the VME controller to give you the BUS  or timeout */

        while (!VMEBUS_OWNER && (sysTimeBaseLGet () - startTick) < ticksToWait);

        /* clear the location */

        *(UINT *)adrs = 0;
        EIEIO_SYNC;

        /* release the VME BUS */

        *UNIVERSE_MAST_CTL &= LONGSWAP(~MAST_CTL_VOWN);

        /* enable normal operation */

        intUnlock (lockKey);
        }
    else    /* Slave node == UNIVERSE_II */
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
#   if (SM_OFF_BOARD == FALSE)
    /*
     * All slave boards in the chassis can generate a VMEbus RMW, and
     * the master board can translate an incoming VMEbus RMW into an
     * atomic operation therefore, all boards can do a simple clear.
     */

    *(UINT *)adrs = 0;
    EIEIO_SYNC;

#   else    /* SM_OFF_BOARD == TRUE */

    /* Check for master node or slave node with UNIVERSE_I */

    if (sysProcNumGet() == 0)
        {

        /*
         * convert LOCK_TIMEOUT to timebase ticks as follows:
         *
         * ticks/Usec = (bus frequency / decrementer pre-scaler) / 1000000
         *
         * ticks to wait = LOCK_TIMEOUT (in Usecs) * ticks/USec. 
         */

        ticksToWait = LOCK_TIMEOUT * 
                      ((DEC_CLOCK_FREQ/DECREMENTER_PRESCALE)/USECS_PER_SECOND);

        /* do clearing with no interference */

        lockKey = intLock ();

        /* request ownership of the VMEbus */

        *UNIVERSE_MAST_CTL |= LONGSWAP(MAST_CTL_VOWN);
        EIEIO_SYNC;

        /* snapshot starting time base low value */

        startTick = sysTimeBaseLGet ();

        /* Wait for the VME controller to give you the BUS  or timeout */

        while (!VMEBUS_OWNER && (sysTimeBaseLGet () - startTick) < ticksToWait);

        /* clear the location */

        *(UINT *)adrs = 0;
        EIEIO_SYNC;

        /* release the VME BUS */

        *UNIVERSE_MAST_CTL &= LONGSWAP(~MAST_CTL_VOWN);

        /* enable normal operation */

        intUnlock (lockKey);
        }
    else    /* Slave node == UNIVERSE_II */
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

#   endif    /* SM_OFF_BOARD */
#endif    /* ANY_BRDS_IN_CHASSIS_NOT_RMW */
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
    UINT32 ticksToWait;      /* number of time base ticks to wait */
    UINT32 startTick;     /* starting timebase low tick value */

    /*
     * convert LOCK_TIMEOUT to timebase ticks as follows:
     *
     * ticks/Usec = (bus frequency / decrementer pre-scaler) / 1000000
     *
     * ticks to wait = requested delay (in Usecs) * ticks/USec. 
     */

    ticksToWait = LOCK_TIMEOUT * 
                  ((DEC_CLOCK_FREQ/DECREMENTER_PRESCALE)/USECS_PER_SECOND);

    /* lock interrupts so there will be no TAS interference */

    lockKey = intLock ();

    /* Request ownership of the VMEbus */

    *UNIVERSE_MAST_CTL |= LONGSWAP(MAST_CTL_VOWN);
    EIEIO_SYNC;

    /* snapshot starting time base low value */

    startTick = sysTimeBaseLGet ();

    /* Wait for the VME controller to give you the BUS  or timeout */

    while (!VMEBUS_OWNER && (sysTimeBaseLGet () - startTick) < ticksToWait);

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
#endif /* ANY_BRDS_IN_CHASSIS_NOT_RMW */


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
    ULONG    offset    /* NVRAM offset to read the byte from */
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
* MVME5100 uses a 48T37 device w/fast write times, no wait.
*
* RETURNS: N/A
*/

void sysNvWrite
    (
    ULONG    offset,    /* NVRAM offset to write the byte to */
    UCHAR    data    /* datum byte */
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

#if    (CPU == PPC603)

    if ((CPU_TYPE == CPU_TYPE_603) || (CPU_TYPE == CPU_TYPE_603E) ||
        (CPU_TYPE == CPU_TYPE_603P))
        {
        return;
        }

#else    /* (CPU == PPC604) */

    if ((CPU_TYPE == CPU_TYPE_604) || (CPU_TYPE == CPU_TYPE_604E) ||
        (CPU_TYPE == CPU_TYPE_604R) || (CPU_TYPE == CPU_TYPE_750) ||
        (CPU_TYPE == CPU_TYPE_7400) || (CPU_TYPE == CPU_TYPE_7410))
        {
        return;
        }

#endif    /* (CPU == PPC604) */

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
* sysMemProbeTrap - trap handler for vxMemProbe exception
*
* This routine is called from the excConnectCode stub if sysMemProbeSup
* generates an exception. By default, sysMemProbeSup returns OK.
* This code changes the PC value to "sysProbeExc" (within the sysMemProbeSup
* routine), and sysProbeExc sets the return value to ERROR.
*/

static int sysMemProbeTrap
    (
    ESFPPC *    pEsf        /* pointer to exception stack frame */
    )
    {
    REG_SET *pRegSet = &pEsf->regSet;

    pRegSet->pc = (_RType)sysProbeExc;  /* sysProbeExc forces an ERROR return */
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
    char   * adrs,    /* address to be probed */
    int      mode,    /* VX_READ or VX_WRITE */
    int      length,  /* 1, 2 or 4 byte probe */
    char   * pVal,    /* address of value to write OR */
                      /* address of location to place value read */
    UINT16   hawkCnfg /* Hawk probe configuration */
    )
    {
    int      oldLevel;
    FUNCPTR  oldVec1;
    FUNCPTR  oldVec2;
    STATUS   status;
    UINT16   devId;
    UINT16   mpcErrEnbl;/* Hawk MPC Error Enable reg */
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

    /* Enable Abort Machine Check Exception generation by Hawk */

    mpcErrEnbl = sysIn16 ((UINT16 *)(HAWK_PHB_BASE_ADRS + HAWK_MPC_MEREN));
    sysOut16 ((UINT16 *)(HAWK_PHB_BASE_ADRS + HAWK_MPC_MEREN),
              mpcErrEnbl | hawkCnfg);

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
         *  Flush the probe instruction out of the Hawk by
         *  doing a dummy read of one of its' MPC registers
         */
 
        devId = sysIn16 ((UINT16 *)(HAWK_PHB_BASE_ADRS + HAWK_MPC_DEVID));
        }

    /* Disable Machine Check Exceptions */

    vxMsrSet(ppcMsr);

    /* Disable Machine Check Pin (EMCP) */

    vxHid0Set(ppcHid0);

    /* Restore previous Hawk configuration */

    sysOut16 ((UINT16 *)(HAWK_PHB_BASE_ADRS + HAWK_MPC_MEREN), mpcErrEnbl);

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
* Hawk MERST and PCI_CFG_STATUS registers and the Universe PCI_CSR and
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

    /* Clear Hawk MPC MERST Register */

    sysOutByte ((ULONG)(HAWK_PHB_BASE_ADRS + HAWK_MPC_MERST),
        HAWK_MPC_MERST_CLR);

    /* Clear Hawk's Cnfg Hdr Status Reg */

    pciConfigOutWord (sysHawkPciBusNo, sysHawkPciDevNo, sysHawkPciFuncNo, 
                      PCI_CFG_STATUS,
                      HAWK_PCI_CFG_STATUS_CLR);

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
* This routine probes an address on the VME bus.  The PCI bridge (Hawk chip)
* must have a special setup to enable generation of Master Abort cycles on
* write probes and reception of Target Abort cycles on read probes.  The Hawk
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
*   LSI0 - Universe Register Access Image (for mailbox)
*   LSI1 - A32
*   LSI2 - A24
*   LSI3 - A16
* .CE
*
* RETURNS: OK or ERROR if address cannot be probed
*/

STATUS sysVmeProbe
    (
    char   * adrs,    /* address to be probed */
    int      mode,    /* VX_READ or VX_WRITE */
    int      length,    /* 1, 2 or 4 byte probe */
    char   * pVal    /* address of value to write OR */
            /* address of location to place value read */
    )
    {
    STATUS status = ERROR;
    UINT32 lsiCtlReg;    /* adress of Local Control register in Universe */
    UINT32 pciSlv1Ctl;    /* Universe PCI Slave Image Control reg */

    /* Determine which Control register controls this address */

    if ((VME_RAI_MSTR_SIZE != 0) && ((UINT32)adrs >= VME_RAI_MSTR_BUS) &&
        ((UINT32)adrs <= (VME_RAI_MSTR_BUS + VME_RAI_MSTR_SIZE)))
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

    pciSlv1Ctl = sysPciInLong (lsiCtlReg);
    if (mode == VX_WRITE)
        {
        sysPciOutLong (lsiCtlReg, (pciSlv1Ctl & ~LSI_CTL_WP));
        }

    /* Perform probe */

    status = sysMemProbeBus (adrs, mode, length, pVal, HAWK_MPC_MEREN_RTAM);

    /* Restore Posted Writes by Universe if previously enabled */

    if ((mode == VX_WRITE) && (pciSlv1Ctl & LSI_CTL_WP))
        {
        sysPciOutLong (lsiCtlReg, pciSlv1Ctl);
        }

    return (status);
    }


/******************************************************************************
*
* sysPciProbe - probe a PCI bus address
*
* This routine probes an address on the PCI bus.  The PCI bridge (Hawk chip)
* must have a special setup to enable generation of Master Abort cycles on
* write probes and reception of Target Abort cycles on read probes.  The Hawk
* MPC must be configured to generate Machine Check interrupts on Master Abort
* or Target Abort cycles.  The CPU must be configured to enable Machine Check
* exceptions.  All probing is done with interrupts disabled.
*
* RETURNS: OK or ERROR if address cannot be probed
*/

STATUS sysPciProbe
    (
    char   * adrs,    /* address to be probed */
    int      mode,    /* VX_READ or VX_WRITE */
    int      length,    /* 1, 2 or 4 byte probe */
    char   * pVal    /* address of value to write OR */
            /* address of location to place value read */
    )
    {
    STATUS status = ERROR;

    /* Perform probe */

    status = sysMemProbeBus (adrs, mode, length, pVal, HAWK_MPC_MEREN_SMAM);

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
    while (TRUE); /* breaks above when delay time is met */
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
    char * str,      /* string to be displayed */
    UINT32  recovery /* recover method */
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


#ifdef INCLUDE_BPE
/*****************************************************************************
*
* sysConfigBpe - configure processor bus parity checking
*
* This function configures processor bus parity checking. Parity checking must
* be enabled in 3 places: 1) Processor, 2) Hawk SMC, and 3) Hawk PHB. It assumes
* that the default VxWorks machine check exception handler is installed to catch
* and display processor bus parity errors.
*
* RETURNS: OK if parity enabled, ERROR otherwise.
*/

void sysConfigBpe(void)
    {
    UINT8 parEna;
    UINT16 meren;

    /* clear hawk SMC and PHB processor bus parity errors. */

    sysOutByte (HAWK_SMC_ADDR(HAWK_SMC_DPE_LOG_REG), HAWK_SMC_DPELOG);
    sysOutByte (HAWK_SMC_ADDR(HAWK_SMC_APE_LOG_REG), HAWK_SMC_APELOG);
    sysOutByte (HAWK_PHB_ADDR(HAWK_MPC_MERST), HAWK_MPC_MERST_XDPE);

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

    /* enable hawk SMC processor data bus parity checking */

    parEna = sysInByte (HAWK_SMC_ADDR(HAWK_SMC_DPE_ENA_REG));
    parEna |= (HAWK_SMC_DPE_CKALL | HAWK_SMC_DPE_ME);
    sysOutByte (HAWK_SMC_ADDR(HAWK_SMC_DPE_ENA_REG), parEna);

    /* enable hawk SMC processor address bus parity checking */

    parEna = sysInByte (HAWK_SMC_ADDR(HAWK_SMC_APE_ENA_REG));
    parEna |= HAWK_SMC_APE_ME;
    sysOutByte (HAWK_SMC_ADDR(HAWK_SMC_APE_ENA_REG), parEna);

    /* enable hawk PHB processor data bus parity checking */

    meren = sysIn16 ((UINT16 *)HAWK_PHB_ADDR(HAWK_MPC_MEREN));
    meren |= HAWK_MPC_MEREN_XDPEM;
    sysOut16 ((UINT16 *)HAWK_PHB_ADDR(HAWK_MPC_MEREN), meren);

    }
#endif /* INCLUDE_DPE */


/*****************************************************************************
*
* reportBootromErrors - report errors detected during rom start.
*
* This function reports error bits left behind by the rom start sequence. These
* bits are stored in the Hawk PHB general purpose registers and are read out and
* printed by this routine.
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
     * loop through each of the 4 general purpose registers and print any
     * non-zero values detected.
     */

    group = 'A';
    for (regIdx = HAWK_MPC_GPREG0_U; regIdx <= HAWK_MPC_GPREG1_L; regIdx += 4)
        {

        if ((flags = sysIn32 ((UINT32 *)HAWK_PHB_ADDR(regIdx))) != 0)
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
* be a zero (low-true). If the syscon bit is a one, then this board is not
* acting as the system controller.
*
* RETURNS: TRUE is SYSCON is asserted, FALSE if not asserted.
*/

BOOL sysSysconAsserted (void)
    {
    return ((*(UCHAR *)BRD_STATUS_REG_ADRS & BRD_STATUS_REG_SYSCON_MASK) == 0);
    }

