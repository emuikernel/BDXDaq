/* usrConfig.c - user-defined system configuration library */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
19w,15may02,sbs  changing default sys clock rate to a macro (SPR #1918)
19v,24apr02,j_s  make WDB Banner the same as in
                 target/config/comps/src/usrWdbBanner.c (SPR #76099)
		 updated 2001 to 2002
19u,19apr02,pch  SPR 74348: Enable PPC Machine Check exception ASAP
19t,05apr02,hbh  Fixed SPR# 30368 and improved ntPassFs initialization.
19s,04mar02,pch  SPR 73609
19r,11feb02,jkf  SPR#73173, adding INCLUDE_DOSFS_MAIN init section.
19q,21jan02,sn   added INCLUDE_CTORS_DTORS
19p,10dec01,jlb  Change UGL to WindML product
19o,07dec01,wef  add USB initialization code.
19n,07dec01,sn   added INCLUDE_CPLUS_DEMANGLER
19m,08nov01,jmp  removed useless routeAdd() for SIMNT.
19l,31oct01,gls  added pthread code
19k,27oct01,dat  Adding trap for misaligned data section SPR 9006
19j,21sep01,aeg  updated initialization sequence of selectLib (SPR #31319).
19i,17sep01,pcm  added excShowInit() (SPR 7333), and lstLibInit () (SPR 20698)
19h,15may01,pch  Move call of usrAltivecInit() from sysLib.c to usrConfig.c,
                 to fix an undefined reference when building bootrom.
19g,20jan99,dbs  change VXCOM to COM
19f,18dec98,dbs  add VXCOM as separate item
19e,06nov98,dbs  add DCOM init when INCLUDE_DCOM is set
19g,22feb01,scm  add fast dram support capabilities for XScale...
19f,16mar99,jpd  changed copyright yearfrom 1998 to 1999.
19e,13nov98,jpd  call usrMmuInit if INCLUDE_MMU_MPU defined; conditionally
		 add sysHwInit0() call before cacheLibInit(); added
		 INCLUDE_INITIAL_MEM_ALLOCATION
	    cdp	 make Thumb support for ARM CPUs dependent on ARM_THUMB.
19j,27feb01,hk   update INCLUDE_WDB_BANNER copyright notice to cover 2001.
19i,08jun00,csi  Merginf usrConfig.c for vxfusion
19h,19apr00,zl   updated copyright year to 2000.
19g,02mar00,zl   merged SH support into T2.
01g,07mar01,sn   simplify job of host val by using creationDate (which
                 is overridable) rather than __DATE__
19g,14oct99,jk   added conditional init code for sound driver support.
19i,30jul99,jkf  fixed SPR#4429. ideDrv, ataDrv, fdDrv, warn on ERROR.
19h,16mar99,jpd  changed copyright yearfrom 1998 to 1999.
19g,10mar99,drm  Changing WINDMP to VXFUSION to reflect new product name.
19f,13nov98,jpd  call usrMmuInit if INCLUDE_MMU_MPU defined; conditionally
		 add sysHwInit0() call before cacheLibInit(); added
		 INCLUDE_INITIAL_MEM_ALLOCATION
	    cdp	 make Thumb support for ARM CPUs dependent on ARM_THUMB.
19e,08oct98,rlp  added support for UGL and HTML.
19d,17sep98,ms   call cplusDemanglerInit if INCLUDE_CPLUS and INCLUDE_LOADER
19c,17sep98,cym  added code to calculate proper IP address for PCSIM.
19b,01sep98,drm  added code to initialize WindMP
19a,10aug98,cym  added code to inform tornado when the NT simulator is up.
18z,23apr98,yp   merged in TrueFFS support
18y,22apr98,elp  created group 1 module when standalone (fixed SPR# 20301).
18w,22apr98,hdn  added comment for tffsDrv().
18t,17apr98,nps  added support for rBuffShow.
18s,09mar98,sjw  merge in HTTP and JAVA changes for conflict resolution
18r,18dec97,cth  replaced windview, timestamp initialization with windviewConfig
18q,13nov97,cth  changed WV evtInstInit to wvLibInit2; removed connRtnSet,
		 evtTaskInit; added include wvLib.h, rBuffLib.h
18p,18aug97,nps  changed WV buffer initialization with the rBuff API.
18x,06mar98,ms   changed copyright year from 1996 to 1998.
18v,04feb98,jpd  fix unsigned/signed warning (usrRoot); updated copyright date.
18u,25feb98,jgn  removed call to excShowInit(), it is called by excInit()
                 if needed (SPR #20625)
18t,19jan98,hdn  changed a condition macro to include tffsDrv().
18p,06jan98,cym  added SIMNT as processor not to clear bss.
18s,23sep97,yp   added support for TFFS.
18r,13aug97,cdp  add Thumb (ARM7TDMI_T) support;
                 'fix' unsigned/signed comparison in usrRoot.
18q,26feb97,dat  added INCLUDE_USER_APPL and USER_APPL_INIT, spr 8071.
18p,05feb97,cdp  remove ARM test hook (_func_armStartup).
18o,12dec96,cdp  added _func_armStartup for ARM platforms.
18n,13dec96,elp	 changed syncLibInit() into symSyncLibInit().
18m,18nov96,dds  SPR 2529: return status for sysScsiInit is now checked.
18l,06nov96,ms   removed 17n hack to usrClock.
18k,01nov96,hdn  added support for PCMCIA.
18j,21oct96,hdn  changed paramerters for new LPT driver.
18i,04oct96,elp  added INCLUDE_SYM_TBL_SYNC (SPR# 6775).
18h,11jul96,tam  changed copyright year from 1995 to 1996.
18l,19jul96,hdn  added support for ATA driver.
18k,25jun96,jmb  Eliminated alternative WDB macro names (SPR #5707).
18j,25jun96,dbt  Initialized _func_selWakeupListInit before tyLib (SPR #3314).
18i,21jun96,jmb  update copyright. SPR #6364
18h,21jun96,jmb  long modhist -- deleted entries prior to 1994.  SPR #6528
18g,12mar96,tam  removed all #if (CPU == PPC403).
18p,06jun96,ism  bumped copyright to 1996.
18o,26jan96,ism  cleaned up vxsim/solaris version
18f,02oct95,kvk  added sysHwInit2() for PPC603.
18e,27sep95,tpr  removed #if (CPU_FAMILY != PPC) staff.
18n,07nov95,srh  fixed C++ support comments.
18m,29oct95,dat  fixed warnings about printf arguments
18l,11oct95,jdi  doc: changed .pG to .tG.
18k,10oct95,dat	 new BSP revision id. Added WDB Banner printing
18j,15jun95,ms	 updated for new serial drivers.
18i,09jun95,ms	 cleaned up console initialization
18h,30may95,p_m  added initialization of formatted I/O library.
		 replaced spyStop() by spyLibInit().
18g,22may95,p_m  no longer initialize object format specific libraries if
		 loader is not included.
18f,22may95,myz  modified new serial device initialization for x86 support
18e,22may95,ms   added some WDB agent support
18e,30oct95,ism  updated for SIMSPARCSOLARIS
18d,28mar95,kkk  added scalability support, changed edata and end to arrays
		 (SPR #3917), changed baud rate to be a macro in configAll.h
18g,25may95,yao  changed to bzero bss segment for PPC.
18f,04apr95,caf  added PPC cache support.
18e,03mar95,yao  added shsHwInit2 () for PPC403.
18d,24feb95,caf  added PPC support.
18c,20nov94,kdl  added hashLibInit() when using dosFs.
18b,09nov94,jds  additions for scsi backward compatability ; scsi[12]IfInit()
18a,04nov94,kdl	 merge cleanup.
17l,02aug94,tpr  added cacheEnable (BRANCH_CACHE) for the MC68060.
17m,15oct94,hdn  added LPT driver.
17o,20jul94,ms   changed INCLUDE_AOUT_HPPA to INCLUDE_SOM_COFF
17n,02may94,ms   added VxSim HP hack to usrClock().
17q,15mar94,smb  renamed tEvtTask parameters.
17p,15feb94,smb  defined WV_MODE, EVTBUFFER_ADDRESS for wvInstInit().
17o,12jan94,kdl  modified posix initialization; added queued signal init.
*/

/*
DESCRIPTION
This library is the WRS-supplied configuration module for VxWorks.  It
contains the root task, the primary system initialization routine, the
network initialization routine, and the clock interrupt routine.

The include file config.h includes a number of system-dependent parameters used
in this file.

In an effort to simplify the presentation of the configuration of vxWorks,
this file has been split into smaller files.  These additional configuration
source files are located in ../../src/config/usr[xxx].c and are #included into
this file below.  This file contains the bulk of the code a customer is
likely to customize.

The module usrDepend.c contains checks that guard against unsupported
configurations such as INCLUDE_NFS without INCLUDE_RPC.  The module
usrKernel.c contains the core initialization of the kernel which is rarely
customized, but provided for information.  The module usrNetwork.c now
contains all network initialization code.  Finally, the module usrExtra.c
contains the conditional inclusion of the optional packages selected in
configAll.h.

The source code necessary for the configuration selected is entirely
included in this file during compilation as part of a standard build in
the board support package.  No other make is necessary.

INCLUDE FILES:
config.h

SEE ALSO:
.tG "Getting Started, Cross-Development"
*/

#include "vxWorks.h"			/* always first */
#include "config.h"			/* board support configuration header */
#include "usrConfig.h"			/* general configuration header */
#include "usrDepend.c"			/* include dependency rules */
#include "usrKernel.c"			/* kernel configuration */
#include "usrExtra.c"			/* conditionally included packages */
#include "gtCore.h"
#ifdef PMC280_DEBUG_UART_VX
#include "dbgprints.h"
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
#include "gtSmp.h"
#endif /* PMC280_DUAL_CPU */

#if CPU==SIMNT
extern int simUpMutex;
extern int win_ReleaseMutex(int hMutex);
#endif

#ifdef INCLUDE_HTTP
#include "http/httpLib.h"		/* Wind Web Server interfaces */
#endif

#ifdef INCLUDE_COM
extern int comLibInit ();
#endif

#ifdef INCLUDE_DCOM
extern int dcomLibInit ();
#endif

/*sergey*/
void usrDmaInit();

/* defines - must be after include of usrDepend.c */

#ifdef	INCLUDE_WDB
#define FREE_MEM_START_ADRS (FREE_RAM_ADRS + WDB_POOL_SIZE)
#else
#define FREE_MEM_START_ADRS FREE_RAM_ADRS
#endif

#ifdef	INCLUDE_INITIAL_MEM_ALLOCATION
#define MEM_POOL_START_ADRS \
	    (ROUND_UP(FREE_MEM_START_ADRS, (INITIAL_MEM_ALIGNMENT)) + \
	    (INITIAL_MEM_SIZE))
#else	/* INCLUDE_INITIAL_MEM_ALLOCATION */
#define MEM_POOL_START_ADRS FREE_MEM_START_ADRS
#endif	/* INCLUDE_INITIAL_MEM_ALLOCATION */

#define BANK0_SIZE_REG      DMA_CHANNEL0_SOURCE_ADDR
#define BANK1_SIZE_REG      DMA_CHANNEL0_BYTE_COUNT
#define DMA_CHANNEL0_SOURCE_ADDR     0x810
#define BYTE_SWAP_32(data) \
((((data) & 0x000000ff) << 24) | (((data) & 0x0000ff00) << 8) | (((data) & 0x00ff0000) >> 8) | \
   (((data) & 0xff000000) >> 24))
/* PMC280 */
IMPORT STATUS  cacheArchEnable (CACHE_TYPE cache);
/* PMC280 */

#ifdef PMC280_DUAL_CPU_MMU_SETUP
IMPORT MMU_LIB_FUNCS mmuLibFuncs, frcmmuLibFuncs;
IMPORT u_int mmuPpcSelected;
IMPORT int mmuPageBlockSize;
#endif /* PMC280_DUAL_CPU_MMU_SETUP */

#ifdef ROHS
IMPORT void cpldGppDisablePin13(void);
#endif 
IMPORT void sysMachChkExcpInit(void); /* import Machine Check Exception Routine */
#ifdef PMC280_DUAL_CPU_PTE_SETUP
IMPORT int vmPageSize;
IMPORT VM_LIB_INFO vmLibInfo;
IMPORT int cacheMmuAvailable;
IMPORT STATUS vmBaseTranslate(VM_CONTEXT_ID context,
                              void *virtualAddr,
                              void **physicalAddr);
IMPORT STATUS  vmBaseEnable (BOOL enable);
void frcSetupVmContext(void);
/* IMPORT STATUS  cacheArchEnable (CACHE_TYPE cache); */
IMPORT void mmuPpcTlbie (void *);
void frcUpdatePTEntry();
#endif /* PMC280_DUAL_CPU_PTE_SETUP */
 

/* global variables */

int		consoleFd;		/* fd of initial console device */
char		consoleName[20];	/* console device name, eg. "/tyCo/0" */
SYMTAB_ID	statSymTbl;		/* system error status symbol table id*/
SYMTAB_ID	standAloneSymTbl;	/* STANDALONE version symbol table id */
SYMTAB_ID	sysSymTbl;		/* system symbol table id */
BOOT_PARAMS	sysBootParams;		/* parameters from boot line */
int		sysStartType;		/* type of boot (WARM, COLD, etc) */

/* Two magic cookies used to detect data section misalignment */

#define TRAP_VALUE_1	0x12348765
#define TRAP_VALUE_2	0x5a5ac3c3
LOCAL volatile UINT32	trapValue1	= TRAP_VALUE_1;
LOCAL volatile UINT32	trapValue2	= TRAP_VALUE_2;


/* PMC280 */

#define MMU_I_ADDR_TRANS             0   
#define MMU_D_ADDR_TRANS             1   
 
IMPORT BOOL mmuPpcIEnabled, mmuPpcDEnabled;
IMPORT UINT32 sdramSize;
unsigned int lockKey;
 
int frc_end_of_vxworks = (int) FREE_RAM_ADRS;
char *buffer = (char *)0x10000;

#ifdef PMC280_DUAL_CPU
LOCAL UINT32 getSdramSize(void);
#endif
/* PMC280 */

/*******************************************************************************
*
* usrInit - user-defined system initialization routine
*
* This is the first C code executed after the system boots.  This routine is
* called by the assembly language start-up routine sysInit() which is in the
* sysALib module of the target-specific directory.  It is called with
* interrupts locked out.  The kernel is not multitasking at this point.
*
* This routine starts by clearing BSS; thus all variables are initialized to 0,
* as per the C specification.  It then initializes the hardware by calling
* sysHwInit(), sets up the interrupt/exception vectors, and starts kernel
* multitasking with usrRoot() as the root task.
*
* RETURNS: N/A
*
* SEE ALSO: kernelLib
*
* ARGSUSED0
*/

void usrInit
    (
    int startType
    )
    {
#ifdef PMC280_DUAL_CPU
    UINT32 *frcSyncSign = (UINT32 *)PMC280_SYNC_SIGN;
	sdramSize = getSdramSize();

	frcSyncSign = (UINT32 *)PMC280_SYNC_SIGN;
#endif /* K2_DUAL_CPU */
    /*
     * This trap will catch improper loading of the data section.
     * We check the magic cookie values to make sure the data section is
     * in the expected memory location. We do not want
     * to proceed further if the data segment is not correct.
     *
     * It should be easy to detect entry into the trap using an ICE, JTAG,
     * or logic analyzer. Without the trap, the processor is likely to run
     * away out of control.
     *
     * Data section misalignment can occur when there is a change in tool
     * chain, build rules, compiler, host utilites,  etc. 
     */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering usrInit\n");
    }
    else
    {   
        dbg_puts0("CPU0: Entering usrInit\n");
    }
#else
    dbg_puts0("Entering usrInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    while (trapValue1 != TRAP_VALUE_1 || trapValue2 != TRAP_VALUE_2)
	{
	/* infinite loop */;
	}
    
#if	(CPU_FAMILY == SPARC)
    excWindowInit ();				/* SPARC window management */
#endif

#ifdef INCLUDE_SYS_HW_INIT_0
    /*
     * Perform any BSP-specific initialisation that must be done before
     * cacheLibInit() is called and/or BSS is cleared.
     */

    SYS_HW_INIT_0 ();
#endif /* INCLUDE_SYS_HW_INIT_0 */

    /* configure data and instruction cache if available and leave disabled */

#ifdef  INCLUDE_CACHE_SUPPORT
    /*
     * SPR 73609:  If a cache is not to be enabled, don't require
     * its mode to be defined.  Instead, default it to disabled.
     */
# if (!defined(USER_D_CACHE_ENABLE) && !defined(USER_D_CACHE_MODE))
#  define USER_D_CACHE_MODE CACHE_DISABLED
# endif	/* !USER_D_CACHE_ENABLE && !USER_D_CACHE_MODE */
# if (!defined(USER_I_CACHE_ENABLE) && !defined(USER_I_CACHE_MODE))
#  define USER_I_CACHE_MODE CACHE_DISABLED
# endif	/* !USER_I_CACHE_ENABLE && !USER_I_CACHE_MODE */

    cacheLibInit (USER_I_CACHE_MODE, USER_D_CACHE_MODE);
#endif  /* INCLUDE_CACHE_SUPPORT */

#if	CPU_FAMILY!=SIMNT && CPU_FAMILY!=SIMSPARCSUNOS && CPU_FAMILY!=SIMHPPA && CPU_FAMILY!=SIMSPARCSOLARIS
    /* don't assume bss variables are zero before this call */

    bzero (edata, end - edata);		/* zero out bss variables */
#endif	/* CPU_FAMILY!=SIMNT && CPU_FAMILY!=SIMSPARCSUNOS && CPU_FAMILY!=SIMHPPA && CPU_FAMILY!=SIMSPARCSOLARIS */

#if (CPU_FAMILY == PPC)
    /*
     * Immediately after clearing the bss, ensure global stdin
     * etc. are ERROR until set to real values.  This is used in
     * target/src/arch/ppc/excArchLib.c to improve diagnosis of
     * exceptions which occur before I/O is set up.
     */
    ioGlobalStdSet (STD_IN,  ERROR);
    ioGlobalStdSet (STD_OUT, ERROR);
    ioGlobalStdSet (STD_ERR, ERROR);
#endif  /* CPU_FAMILY == PPC */

    sysStartType = startType;			/* save type of system start */

    intVecBaseSet ((FUNCPTR *) VEC_BASE_ADRS);	/* set vector base table */

#if (CPU_FAMILY == AM29XXX)
    excSpillFillInit ();			/* am29k stack cache managemt */
#endif

#ifdef  INCLUDE_EXC_HANDLING
# if (CPU_FAMILY == PPC) && defined(INCLUDE_EXC_SHOW)
    /*
     * Do this ahead of excVecInit() to set up _func_excPanicHook, in case
     * the enabling of Machine Check there allows a pending one to occur.
     * excShowInit() will be called again later, harmlessly.
     */
    excShowInit ();
# endif  /* CPU_FAMILY == PPC && defined(INCLUDE_EXC_SHOW) */
    excVecInit ();				/* install exception vectors */
#endif  /* INCLUDE_EXC_HANDLING */

#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI()) {
        *frcSyncSign = 0xdeadbeef;
    }
#endif /* PMC280_DUAL_CPU */

    sysHwInit ();				/* initialize system hardware */

    usrKernelInit ();				/* configure the Wind kernel */

#ifdef INCLUDE_USB 
#   ifdef INCLUDE_OHCI_PCI_INIT
        sysUsbPciOhciInit ();
#   endif
#endif

#ifdef  INCLUDE_CACHE_SUPPORT
#ifdef 	USER_I_CACHE_ENABLE
    cacheEnable (INSTRUCTION_CACHE);		/* enable instruction cache */
#endif	/* USER_I_CACHE_ENABLE */

#ifdef	USER_D_CACHE_ENABLE
    cacheEnable (DATA_CACHE);			/* enable data cache */
#endif 	/* USER_D_CACHE_ENABLE */

#if (CPU == MC68060)
#ifdef 	USER_B_CACHE_ENABLE
    cacheEnable (BRANCH_CACHE);			/* enable branch cache */
#endif	/* USER_B_CACHE_ENABLE */
#endif	/* (CPU == MC68060) */
#endif  /* INCLUDE_CACHE_SUPPORT */

    /* start the kernel specifying usrRoot as the root task */
    kernelInit ((FUNCPTR) usrRoot, ROOT_STACK_SIZE,
		(char *) MEM_POOL_START_ADRS,
		sysMemTop (), ISR_STACK_SIZE, INT_LOCK_LEVEL);
    }

/*******************************************************************************
*
* usrRoot - the root task
*
* This is the first task to run under the multitasking kernel.  It performs
* all final initialization and then starts other tasks.
*
* It initializes the I/O system, installs drivers, creates devices, and sets
* up the network, etc., as necessary for a particular configuration.  It
* may also create and load the system symbol table, if one is to be included.
* It may then load and spawn additional tasks as needed.  In the default
* configuration, it simply initializes the VxWorks shell.
*
* RETURNS: N/A
*/

void usrRoot
    (
    char *	pMemPoolStart,		/* start of system memory partition */
    unsigned	memPoolSize		/* initial size of mem pool */
    )
    {
    char tyName [20];
    int  ix;

    /* Initialize the memory pool before initializing any other package.
     * The memory associated with the root task will be reclaimed at the
     * completion of its activities.
     */
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering usrRoot\n");
    }
    else
    {   
        dbg_puts0("CPU0: Entering usrRoot\n");
    }
#else
    dbg_puts0("Entering usrRoot\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */


#ifdef INCLUDE_MEM_MGR_FULL
    memInit (pMemPoolStart, memPoolSize);	/* initialize memory pool */
#else
    memPartLibInit (pMemPoolStart, memPoolSize);/* initialize memory pool */
#endif /* INCLUDE_MEM_MGR_FULL */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed memInit\n");
    }
    else
    {   
        dbg_puts0("CPU0: Passed memInit\n");
    }
#else    
    dbg_puts0("Passed memInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
 
#ifdef	INCLUDE_SHOW_ROUTINES
    memShowInit ();			/* initialize memShow routine */
#endif	/* INCLUDE_SHOW_ROUTINES */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed memShowInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed memShowInit\n");
    }
#else    
    dbg_puts0("Passed memShowInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
    /*
     * MMU is set up and initialized in bootInit.c. There may be a few
     * globals(and exports) that are not setup if usrMmuInit is not called.
     * These have to be setup outside usrMmuInit. This is done here.
     */  
 
    mmuPpcIEnabled = TRUE;
    mmuPpcDEnabled = TRUE;
    mmuPpcSelected |= MMU_INST;
    mmuPpcSelected |= MMU_DATA;
 
#ifdef PMC280_DUAL_CPU_MMU_SETUP
    mmuPageBlockSize = 4096; /* PAGE_SIZE */
    mmuLibFuncs = frcmmuLibFuncs;
 
#ifdef PMC280_DEBUG_UART_VX
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed mmuSetup\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed mmuSetup\n");
    }
#endif /* PMC280_DEBUG_UART_VX */
#endif /* PMC280_DUAL_CPU_MMU_SETUP */
 
#ifdef PMC280_DUAL_CPU_PTE_SETUP
    frcSetupVmContext();
    vmPageSize = 4096;
    vmLibInfo.pVmStateSetRtn     = vmBaseStateSet;
    vmLibInfo.pVmEnableRtn       = vmBaseEnable;
    vmLibInfo.pVmPageSizeGetRtn  = vmBasePageSizeGet;
    vmLibInfo.pVmTranslateRtn    = vmBaseTranslate;
    vmLibInfo.vmBaseLibInstalled = TRUE;
    cacheMmuAvailable = TRUE;
    cacheFuncsSet();
 
#ifdef PMC280_DEBUG_UART_VX
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed vmSetup\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed vmSetup\n");
    }
#endif /* PMC280_DEBUG_UART_VX */
#endif /* PMC280_DUAL_CPU_PTE_SETUP */
 
#else 
#if	defined(INCLUDE_MMU_BASIC) || defined(INCLUDE_MMU_FULL) || \
	defined(INCLUDE_MMU_MPU)
    usrMmuInit ();				/* initialize the mmu */
#endif	/* defined(INCLUDE_MMU_BASIC, INCLUDE_MMU_FULL, INCLUDE_MMU_MPU) */
#endif /* PMC280_DUAL_CPU */

/* 280 */

    lockKey = intLock ();                   /* lock the interrupt */
    cacheArchEnable(_DATA_CACHE);
    intUnlock (lockKey);                    /* unlock the interrupt */

/* 280 */
 
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed cache enable\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed cache enable\n");
    }
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
 
/* 280 */

    /* set up system timer */

    sysClkConnect ((FUNCPTR) usrClock, 0);	/* connect clock ISR */
    sysClkRateSet (SYS_CLK_RATE);	/* set system clock rate */
    sysClkEnable ();				/* start it */

/* 280 */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed sysClkEnable\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed sysClkEnable\n");
    }
#else    
    dbg_puts0("Passed sysClkEnable\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef INCLUDE_FAST_DRAM
/*
 * make use of data cache as fast DRAM,
 * establish parameters in config.h, MMU
 * must be initialed before data cache is
 * initialized as data ram...
 */
  cacheCreateInternalDataRAM((UINT32 *)FD_ORIGIN, FD_NUMLINES);
#endif

    /*
     * The select library needs to be initialized before the tyLib module
     * since the _func_selWakeupListInit FUNCPTR is required (SPR #3314).
     * The installation of the select task delete hooks is performed
     * later in usrRoot() after NFS and RPC have been initialized.
     */

#ifdef  INCLUDE_SELECT
    selectInit (NUM_FILES);
#endif	/* INCLUDE_SELECT */


    /* initialize I/O system */

#ifdef  INCLUDE_IO_SYSTEM
    iosInit (NUM_DRIVERS, NUM_FILES, "/null");

    consoleFd = NONE;                           /* assume no console device */

#ifdef  INCLUDE_TYCODRV_5_2
#ifdef  INCLUDE_TTY_DEV
    if (NUM_TTY > 0)
	{
	tyCoDrv ();				/* install console driver */

	for (ix = 0; ix < NUM_TTY; ix++)	/* create serial devices */
	    {
	    sprintf (tyName, "%s%d", "/tyCo/", ix);

	    (void) tyCoDevCreate (tyName, ix, 512, 512);

	    if (ix == CONSOLE_TTY)
		strcpy (consoleName, tyName);	/* store console name */
	    }

	consoleFd = open (consoleName, O_RDWR, 0);

	/* set baud rate */

	(void) ioctl (consoleFd, FIOBAUDRATE, CONSOLE_BAUD_RATE);
	(void) ioctl (consoleFd, FIOSETOPTIONS, OPT_TERMINAL);
	}
#endif	/* INCLUDE_TTY_DEV */

#else   /* !INCLUDE_TYCODRV_5_2 */
#ifdef  INCLUDE_TTY_DEV
    if (NUM_TTY > 0)
	{
	ttyDrv();				/* install console driver */

#ifdef PMC280_DUAL_CPU
        /*
         * Get rid of the loop. Each CPU on to his own!
         */
        if(frcWhoAmI())
            ix = 1;
        else
            ix = 0;
#else
	for (ix = 0; ix < NUM_TTY; ix++)	/* create serial devices */
	    {
#endif /* PMC280_DUAL_CPU */

#if     (defined(INCLUDE_WDB) && (WDB_COMM_TYPE == WDB_COMM_SERIAL))
	    if (ix == WDB_TTY_CHANNEL)		/* don't use WDBs channel */
		continue;
#endif

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
            if(frcWhoAmI())
            {
                dbg_puts1("CPU1: Before ttyDevCreate\n");
            }
            else
            {   
                dbg_puts0("CPU0: Before ttyDevCreate\n");
            }
#else     
            dbg_puts0("Before ttyDevCreate\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

	    sprintf (tyName, "%s%d", "/tyCo/", ix);
#ifdef PMC280_DUAL_CPU_DBG
            if(ttyDevCreate (tyName, sysSerialChanGet(ix), 512, 512) == ERROR)
            {
#ifdef PMC280_DEBUG_UART_VX
                if(frcWhoAmI())
                {
                    dbg_puts1("CPU1: Error!\n");
                }
                else
                {
                    dbg_puts1("CPU0: Error!\n");
                }
#endif /* PMC280_DEBUG_UART_VX */
            }
#else
	    (void) ttyDevCreate (tyName, sysSerialChanGet(ix), 512, 512);
#endif /* PMC280_DUAL_CPU_DBG */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
            if(frcWhoAmI())
            {
                dbg_puts1("CPU1: After ttyDevCreate\n");
            }
            else
            {
                dbg_puts0("CPU0: After ttyDevCreate\n");
            }
#else    
            dbg_puts0("After ttyDevCreate\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

	    if (ix == CONSOLE_TTY)		/* init the tty console */
		{
		strcpy (consoleName, tyName);
		consoleFd = open (consoleName, O_RDWR, 0);
		(void) ioctl (consoleFd, FIOBAUDRATE, CONSOLE_BAUD_RATE);
		(void) ioctl (consoleFd, FIOSETOPTIONS, OPT_TERMINAL);
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
                if(frcWhoAmI())
                {
                    dbg_puts1("CPU1: After open/ioctl\n");
                    dbg_printf1("ix = %d\n", ix);
                }
                else
                {
                    dbg_puts0("CPU0: After open/ioctl\n");
                    dbg_printf0("ix = %d\n", ix);
                }
#else    
                dbg_puts0("After open/ioctl\n");
                dbg_printf0("ix = %d\n", ix);
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
		}
#ifndef PMC280_DUAL_CPU
	    }
#endif /* ! PMC280_DUAL_CPU */
	}
#endif  /* INCLUDE_TTY_DEV */

#ifdef INCLUDE_PC_CONSOLE
    pcConDrv ();
    for (ix = 0; ix < N_VIRTUAL_CONSOLES; ix++)
	{
	sprintf (tyName, "%s%d", "/pcConsole/", ix);
	(void) pcConDevCreate (tyName,ix, 512, 512);
	if (ix == PC_CONSOLE)		/* init the console device */
	    {
	    strcpy (consoleName, tyName);
	    consoleFd = open (consoleName, O_RDWR, 0);
	    (void) ioctl (consoleFd, FIOBAUDRATE, CONSOLE_BAUD_RATE);
	    (void) ioctl (consoleFd, FIOSETOPTIONS, OPT_TERMINAL);
	    }
	}
#endif	/* INCLUDE_PC_CONSOLE */

#endif  /* !INCLUDE_TYCODRV_5_2 */

    ioGlobalStdSet (STD_IN,  consoleFd);
    ioGlobalStdSet (STD_OUT, consoleFd);
    ioGlobalStdSet (STD_ERR, consoleFd);
#endif  /* INCLUDE_IO_SYSTEM */

    /* initialize symbol table facilities */

#ifdef	INCLUDE_SYM_TBL
    hashLibInit ();			/* initialize hash table package */
    symLibInit ();			/* initialize symbol table package */
#ifdef 	INCLUDE_SHOW_ROUTINES
    symShowInit ();			/* initialize symbol table show */
#endif	/* INCLUDE_SHOW_ROUTINES */
#endif	/* INCLUDE_SYM_TBL */

    /* initialize exception handling */

#if     defined(INCLUDE_EXC_HANDLING) && defined(INCLUDE_EXC_TASK)
#ifdef	INCLUDE_EXC_SHOW
    excShowInit ();
#endif
    excInit ();				/* initialize exception handling */
 sysMachChkExcpInit(); 
#endif  /* defined(INCLUDE_EXC_HANDLING) && defined(INCLUDE_EXC_TASK) */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed excInit\n");
    }
    else
    {   
        dbg_puts0("CPU0: Passed excInit\n");
    }
#else
    dbg_puts0("Passed excInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef	INCLUDE_LSTLIB
    lstLibInit ();
#endif

#ifdef	INCLUDE_LOGGING
    logInit (consoleFd, MAX_LOG_MSGS);	/* initialize logging */
# ifdef	INCLUDE_LOG_STARTUP
    logMsg ("logging started to %s [%d], queue size %d\n",
		consoleName, consoleFd, MAX_LOG_MSGS, 4,5,6);
    taskDelay (2);	/* allow time for message to be displayed */
# endif	/* INCLUDE_LOG_STARTUP */
#endif	/* INCLUDE_LOGGING */

#ifdef	INCLUDE_SIGNALS
    sigInit ();				/* initialize signals */
#endif	/* INCLUDE_SIGNALS */

    /* initialize debugging */

#ifdef	INCLUDE_DEBUG
    dbgInit ();				/* initialize debugging */
#endif	/* INCLUDE_DEBUG */


    /* initialize pipe driver */

#ifdef	INCLUDE_PIPES
    pipeDrv ();				/* install pipe driver */
#endif	/* INCLUDE_PIPES */


    /* initialize standard I/O package */

#ifdef	INCLUDE_STDIO
    stdioInit ();			/* initialize standard I/O library */
#ifdef  INCLUDE_SHOW_ROUTINES
    stdioShowInit ();
#endif  /* INCLUDE_SHOW_ROUTINES */
#endif	/* INCLUDE_STDIO */

    /* initialize POSIX queued signals */

#if defined(INCLUDE_POSIX_SIGNALS) && defined(INCLUDE_SIGNALS)
    sigqueueInit (NUM_SIGNAL_QUEUES); /* initialize queued signals */
#endif

    /* initialize POSIX semaphores */

#ifdef  INCLUDE_POSIX_SEM
    semPxLibInit ();
#ifdef INCLUDE_SHOW_ROUTINES
    semPxShowInit ();
#endif  /* INCLUDE_SHOW_POUTINES */
#endif  /* INCLUDE_POSIX_SEM */

    /* initialize POSIX threads */

#ifdef INCLUDE_POSIX_PTHREADS
    pthreadLibInit ();
#endif  /* INCLUDE_POSIX_PTHREADS */

    /* initialize POSIX message queues */

#ifdef INCLUDE_POSIX_MQ
    mqPxLibInit (MQ_HASH_SIZE);
#ifdef INCLUDE_SHOW_ROUTINES
    mqPxShowInit ();
#endif  /* INCLUDE_SHOW_ROUTINES */
#endif  /* INCLUDE_POSIX_MQ */

    /* initialize POSIX async I/O support */

#ifdef INCLUDE_POSIX_AIO
    aioPxLibInit (MAX_LIO_CALLS);
#ifdef INCLUDE_POSIX_AIO_SYSDRV
    aioSysInit (MAX_AIO_SYS_TASKS, AIO_TASK_PRIORITY, AIO_TASK_STACK_SIZE);
#endif  /* INCLUDE_POSIX_AIO_SYSDRV */
#endif  /* INCLUDE_POSIX_AIO */

#ifdef INCLUDE_CBIO /* init main CBIO module, cbioLib() */
    cbioLibInit();
#endif INCLUDE_CBIO

    /* initialize filesystems and disk drivers */

#ifdef INCLUDE_DOSFS_MAIN  /* dosFs2 file system initialization */

    hashLibInit ();			/* initialize hash table package */

    /* First initialize the main dosFs module */

    dosFsLibInit( 0 );

    /* Initialize sub-modules */

    /* ensure that at least one directory handler is defined */

#   if ((!defined INCLUDE_DOSFS_DIR_VFAT) && \
        (!defined INCLUDE_DOSFS_DIR_FIXED))

#       define INCLUDE_DOSFS_DIR_VFAT  

#   endif 

    /* init VFAT (MS long file names) module */

#   ifdef INCLUDE_DOSFS_DIR_VFAT

    /* Sub-module: VFAT Directory Handler */

    dosVDirLibInit();

#   endif /* INCLUDE_DOSFS_DIR_VFAT */

    /* init strict 8.3 and vxLongNames handler */

#   ifdef INCLUDE_DOSFS_DIR_FIXED

    /* Sub-module: Vintage 8.3 and VxLong Directory Handler */

    dosDirOldLibInit();

#   endif /* INCLUDE_DOSFS_DIR_FIXED */

    /* Sub-module: FAT12/FAT16/FAT32 FAT Handler */

    dosFsFatInit();

#   ifdef INCLUDE_DOSFS_CHKDSK 

    /* Sub-module: Consistency check handler */

    dosChkLibInit();

#   endif /* INCLUDE_DOSFS_CHKDSK */

#   ifdef INCLUDE_DOSFS_FMT

    /* Sub-module: Formatter */

    dosFsFmtLibInit();           /* init dosFs scalable formatter */

#   endif /* INCLUDE_DOSFS_FMT */

#endif /* INCLUDE_DOSFS_MAIN */

    /* dosFs1 legacy code, dosFsInit(), usrDosFsOld.c */

#ifdef	INCLUDE_DOSFS
    hashLibInit ();			/* initialize hash table package */
    dosFsInit (NUM_DOSFS_FILES); 	/* init dosFs filesystem */
#endif	/* INCLUDE_DOSFS */

#ifdef	INCLUDE_RAWFS
    rawFsInit (NUM_RAWFS_FILES); 	/* init rawFs filesystem */
#endif	/* INCLUDE_RAWFS */

#ifdef	INCLUDE_RT11FS
    rt11FsInit (NUM_RT11FS_FILES); 	/* init rt11Fs filesystem */
#endif	/* INCLUDE_RT11FS */

#ifdef	INCLUDE_RAMDRV
    ramDrv ();				/* initialize ram disk driver */
#endif	/* INCLUDE_RAMDRV */

    /* initialize USB components */

#ifdef INCLUDE_USB_INIT
    usbInit (); 		/* USB Host Stack Initialization */
#endif

#ifdef INCLUDE_UHCI_INIT
    usrUsbHcdUhciAttach (); 	/* UHCI Initialization */
#endif

#ifdef INCLUDE_OHCI_INIT
    usrUsbHcdOhciAttach (); 	/* OHCI Initialization */
#endif

#ifdef INCLUDE_USB_MOUSE_INIT
    usrUsbMseInit (); 		/* Mouse Driver Initialization */
#endif

#ifdef INCLUDE_USB_KEYBOARD_INIT
    usrUsbKbdInit (); 		/* Keyboard Driver Initialization */
#endif

#ifdef INCLUDE_USB_PRINTER_INIT
    usrUsbPrnInit (); 		/* Printer Driver Initialization */
#endif

#ifdef INCLUDE_USB_SPEAKER_INIT
    usrUsbSpkrInit (); 		/* Speaker Driver Initialization */
#endif

#ifdef INCDLUE_USB_AUDIO_DEMO
    usrUsbAudioDemo (); 	/* USB Audio Demo */
#endif

#ifdef INCLUDE_USB_MS_BULKONLY_INIT
    usrUsbBulkDevInit(); 	/* Bulk Driver Initialization */
#endif

#ifdef INCLUDE_USB_MS_CBI_INIT
    usrUsbCbiUfiDevInit (); 	/* CBI Driver Initialization */
#endif

#ifdef INCLUDE_USB_PEGASUS_END_INIT
    usrUsbPegasusEndInit (); 	/* Pegasus Driver Initialization */
#endif


#ifdef	INCLUDE_SCSI

    /*
     * initialize either the SCSI1 or SCSI2 interface; initialize SCSI2 when
     * the SCSI2 interface is available.
     */

#ifndef INCLUDE_SCSI2
    scsi1IfInit ();
#else
    scsi2IfInit ();
#endif

    /* initialize SCSI controller */
    if (sysScsiInit () == OK)
	{
	usrScsiConfig ();			/* configure SCSI peripherals */
        }
    else
        {
#ifdef INCLUDE_STDIO
        printf ("sysScsiInit() Failed, SCSI system not initialized\n");
#endif  /* INCLUDE_STDIO */
        }

#endif	/* INCLUDE_SCSI */

#ifdef  INCLUDE_FD     /* initialize floppy disk driver */
    if ((fdDrv (FD_INT_VEC, FD_INT_LVL)) == ERROR)
    	{
#ifdef INCLUDE_STDIO
	printf ("fdDrv returned ERROR from usrRoot.\n");
#endif /* INCLUDE_STDIO */
	}
#endif /* INCLUDE_FD */

#ifdef  INCLUDE_IDE
    /* init IDE disk driver */
    if ((ideDrv (IDE_INT_VEC, IDE_INT_LVL, IDE_CONFIG)) == ERROR)
	{
#ifdef INCLUDE_STDIO
	printf ("ideDrv returned ERROR from usrRoot.\n");
#endif /* INCLUDE_STDIO */
	}

#endif  /* INCLUDE_IDE */

#ifdef  INCLUDE_ATA
    {                                   /* initialize hard disk driver */
    IMPORT ATA_RESOURCE ataResources[];
    ATA_RESOURCE *pAtaResource;
    for (ix = 0; ix < ATA_MAX_CTRLS; ix++)
        {
        pAtaResource = &ataResources[ix];
        if (pAtaResource->ctrlType == IDE_LOCAL)
            if ((ataDrv (ix, pAtaResource->drives, pAtaResource->intVector,
		   pAtaResource->intLevel, pAtaResource->configType,
                   pAtaResource->semTimeout, pAtaResource->wdgTimeout))
		== ERROR)
		{
#ifdef INCLUDE_STDIO
		printf ("ataDrv returned ERROR from usrRoot.\n");
#endif /* INCLUDE_STDIO */
		}
        }
    }

#ifdef  INCLUDE_SHOW_ROUTINES
    ataShowInit ();                     /* install ATA/IDE show routine */
#endif  /* INCLUDE_SHOW_ROUTINES */
#endif  /* INCLUDE_ATA */

#ifdef  INCLUDE_LPT
    {
    IMPORT LPT_RESOURCE lptResources[];
    lptDrv (LPT_CHANNELS, &lptResources[0]); /* init LPT parallel driver */
    }
#endif  /* INCLUDE_LPT */

#ifdef  INCLUDE_PCMCIA
#ifdef  INCLUDE_SHOW_ROUTINES
    pcmciaShowInit ();			/* install PCMCIA show routines */
#endif  /* INCLUDE_SHOW_ROUTINES */
    pcmciaInit ();			/* init PCMCIA Lib */
#endif  /* INCLUDE_PCMCIA */

#ifdef	INCLUDE_TFFS
    tffsDrv ();				/* it should be after pcmciaInit() */
#endif	/* INCLUDE_TFFS */

#ifdef  INCLUDE_FORMATTED_IO
    fioLibInit ();			/* initialize formatted I/O */
#endif  /* INCLUDE_FORMATTED_IO */

    /* initialize floating point facilities */

#ifdef	INCLUDE_FLOATING_POINT
    floatInit ();			/* initialize floating point I/O */
#endif	/* INCLUDE_FLOATING_POINT */

    /* install software floating point emulation (if applicable) */

#ifdef	INCLUDE_SW_FP
    mathSoftInit ();		/* use software emulation for fp math */
#endif	/* INCLUDE_SW_FP */

    /* install hardware floating point support (if applicable) */

#ifdef	INCLUDE_HW_FP
    mathHardInit (); 		/* do fppInit() & install hw fp math */

#ifdef	INCLUDE_SHOW_ROUTINES
    fppShowInit ();			/* install hardware fp show routine */
#endif	/* INCLUDE_SHOW_ROUTINES */
#endif	/* INCLUDE_HW_FP */

    /* install dsp support (if applicable) */

#ifdef	INCLUDE_DSP
    usrDspInit (); 			/* do dspInit() */
#endif	/* INCLUDE_DSP */

    /* initialize AltiVec library (if applicable) */

#ifdef	INCLUDE_ALTIVEC
    usrAltivecInit ();
#endif	/* INCLUDE_ALTIVEC */

    /* initialize performance monitoring tools */

#ifdef	INCLUDE_SPY
    spyLibInit ();			/* install task cpu utilization tool */
#endif	/* INCLUDE_SPY */

#ifdef	INCLUDE_TIMEX
    timexInit ();			/* install function timing tool */
#endif	/* INCLUDE_TIMEX */

#ifdef  INCLUDE_ENV_VARS
    envLibInit (ENV_VAR_USE_HOOKS);	/* initialize environment variable */
#endif	/* INCLUDE_ENV_VARS */

#ifdef INCLUDE_NTPASSFS
    {
    IMPORT int	    ntPassFsInit();
    IMPORT void *   ntPassFsDevInit();
    IMPORT char	    fullExePath[];
    char	    passName [MAX_FILENAME_LENGTH];
    char *	    defPathEnd;
    
    if (ntPassFsInit(1) == OK)
    	{	
    	if (ntPassFsDevInit("host:") == NULL)
    	    {
#ifdef INCLUDE_STDIO
	    printf ("ntPassFsDevInit failed for host: \n");
#endif /* INCLUDE_STDIO */
	    }
	else
	    {
    	    sprintf (passName, "host:%s", fullExePath);

            /* Remove bootFile name at the end of the string */

    	    defPathEnd = strrchr (passName, '\\');
    	    if (defPathEnd != NULL)
    		*defPathEnd = '\0';

    	    ioDefPathSet (passName);
	    }
	}
    else	
    	{
#ifdef INCLUDE_STDIO
	printf ("ntPassFsInit failed\n");
#endif /* INCLUDE_STDIO */
	}
    }
#endif /* INCLUDE_NTPASSFS */

    /* initialize object module loader */

#ifdef	INCLUDE_LOADER
    moduleLibInit ();			/* initialize module manager */

#if	defined(INCLUDE_AOUT)
    loadAoutInit ();				/* use a.out format */
#else	/* coff or ecoff */
#if	defined(INCLUDE_ECOFF)
    loadEcoffInit ();				/* use ecoff format */
#else	/* ecoff */
#if	defined(INCLUDE_COFF)
    loadCoffInit ();				/* use coff format */
#else   /* coff */
#if	defined(INCLUDE_ELF)
    loadElfInit ();				/* use elf format */
#else
#if	defined(INCLUDE_SOM_COFF)
    loadSomCoffInit ();
#else
#if	defined(INCLUDE_PECOFF)
    {
    extern int loadPecoffInit();

    loadPecoffInit ();
    }
#endif
#endif
#endif
#endif
#endif
#endif

#endif	/* INCLUDE_LOADER */

    /* initialize wtx client to synchronize host and target symbol tables */
#ifdef	INCLUDE_SYM_TBL_SYNC
    symSyncLibInit ();
#endif /* INCLUDE_SYM_TBL_SYNC */

    /* initialize network */
#ifdef PMC280_DUAL_CPU
    /*    
     * We are not interested in initialising the network on CPU1.
     */   
    if(!frcWhoAmI())
    {

#ifdef PMC280_DEBUG_UART_VX
        printf("CPU0: Before usrNetInit\n");
#endif /* PMC280_DEBUG_UART_VX */

#ifdef  INCLUDE_NET_INIT  
    usrBootLineInit (sysStartType);     /* crack the bootline */
# if defined(INCLUDE_LOGGING) && defined(INCLUDE_LOG_STARTUP)
    logMsg ("before usrNetInit()\n", 1,2,3,4,5,6);
    taskDelay (2);      /* allow time for message to be displayed */
# endif /* INCLUDE_LOGGING && INCLUDE_LOG_STARTUP */
    usrNetInit (BOOT_LINE_ADRS);        /* initialize network support */
# if defined(INCLUDE_LOGGING) && defined(INCLUDE_LOG_STARTUP)
    logMsg ("after usrNetInit()\n", 1,2,3,4,5,6);
    taskDelay (2);      /* allow time for message to be displayed */
# endif /* INCLUDE_LOGGING && INCLUDE_LOG_STARTUP */
#endif  /* INCLUDE_NET_INIT */

#ifdef PMC280_DEBUG_UART_VX
       printf("CPU0: After usrNetInit\n");
#endif /* PMC280_DEBUG_UART_VX */

    }

#else /* PMC280_DUAL_CPU */

#ifdef  INCLUDE_NET_INIT
    usrBootLineInit (sysStartType);	/* crack the bootline */
# if defined(INCLUDE_LOGGING) && defined(INCLUDE_LOG_STARTUP)
    logMsg ("before usrNetInit()\n", 1,2,3,4,5,6);
    taskDelay (2);	/* allow time for message to be displayed */
# endif	/* INCLUDE_LOGGING && INCLUDE_LOG_STARTUP */
    usrNetInit (BOOT_LINE_ADRS);	/* initialize network support */
# if defined(INCLUDE_LOGGING) && defined(INCLUDE_LOG_STARTUP)
    logMsg ("after usrNetInit()\n", 1,2,3,4,5,6);
    taskDelay (2);	/* allow time for message to be displayed */
# endif	/* INCLUDE_LOGGING && INCLUDE_LOG_STARTUP */
#endif	/* INCLUDE_NET_INIT */

#endif /* PMC280_DUAL_CPU */

#ifdef	INCLUDE_PASSFS
    {
    extern STATUS passFsInit ();
    extern void *passFsDevInit ();
    char passName [256];

    if (passFsInit (1) == OK)
	{
	extern char vxsim_hostname[];
	extern char vxsim_cwd[];

	sprintf (passName, "%s:", vxsim_hostname);
	if (passFsDevInit (passName) == NULL)
	    {
#ifdef INCLUDE_STDIO
	    printf ("passFsDevInit failed for <%s>\n", passName);
#endif /* INCLUDE_STDIO */
	    }
	else
	    {
	    sprintf (passName, "%s:%s", vxsim_hostname, vxsim_cwd);
	    ioDefPathSet (passName);
	    }
	}
    else
#ifdef INCLUDE_STDIO
	printf ("passFsInit failed\n");
#endif /* INCLUDE_STDIO */
    }
#endif	/* INCLUDE_PASSFS */

#ifdef	INCLUDE_DOS_DISK
    {
    char unixName [80];
    extern void unixDrv ();
    extern void unixDiskInit ();
    extern char *u_progname;  /* home of executable */
    char *pLastSlash;

    unixDrv ();

    pLastSlash = strrchr (u_progname, '/');
    pLastSlash = (pLastSlash == NULL) ? u_progname : (pLastSlash + 1);
    sprintf (unixName, "/tmp/%s%d.dos", pLastSlash, sysProcNumGet());
    unixDiskInit (unixName, "A:", 0);
    }
#endif	/* INCLUDE_DOS_DISK */

    /* initialize shared memory objects */

#ifdef INCLUDE_SM_OBJ			/* unbundled shared memory objects */
    usrSmObjInit (BOOT_LINE_ADRS);
#endif /* INCLUDE_SM_OBJ */

   /* initialize WindMP */

#ifdef INCLUDE_VXFUSION			/* unbundled VxFusion (distributed objects) */
    usrVxFusionInit (BOOT_LINE_ADRS);
#ifdef INCLUDE_SHOW_ROUTINES
    {
    extern void msgQDistShowInit();
    extern void distNameShowInit ();
    extern void distIfShowInit ();
    extern void msgQDistGrpShowInit ();

    msgQDistShowInit();
    distNameShowInit ();
    distIfShowInit ();
    msgQDistGrpShowInit ();
    }
#endif /* INCLUDE_SHOW_ROUTINES */
#endif /* INCLUDE_WINDMP */

    /* write protect text segment & vector table only after bpattach () */

#ifdef	INCLUDE_MMU_FULL		/* unbundled mmu product */
#ifdef	INCLUDE_PROTECT_TEXT
    if (vmTextProtect () != OK)
	printf ("\nError protecting text segment. errno = %x\n", errno);
#endif	/* INCLUDE_PROTECT_TEXT */

#ifdef	INCLUDE_PROTECT_VEC_TABLE
    if (intVecTableWriteProtect () != OK)
	printf ("\nError protecting vector table. errno = %x\n", errno);
#endif	/* INCLUDE_PROTECT_VEC_TABLE */
#endif	/* INCLUDE_MMU_FULL */

    /* install select hook only after NFS/RPC for proper delete hook order */

#ifdef	INCLUDE_SELECT
    selTaskDeleteHookAdd ();
#endif	/* INCLUDE_SELECT */




    /* create system and status symbol tables */

#ifdef  INCLUDE_STANDALONE_SYM_TBL
# if defined(INCLUDE_LOGGING) && defined(INCLUDE_LOG_STARTUP)
    logMsg ("before symTblCreate()\n", 1,2,3,4,5,6);
    taskDelay (2);	/* allow time for message to be displayed */
# endif	/* INCLUDE_LOGGING && INCLUDE_LOG_STARTUP */
    sysSymTbl = symTblCreate (SYM_TBL_HASH_SIZE_LOG2, TRUE, memSysPartId);

#ifdef	INCLUDE_SYM_TBL_SYNC
    /* create a module (group 1) that is not synchronized (SPR# 20301) */

    moduleCreate ("vxWorks.sym",
#if (defined INCLUDE_AOUT)
		  MODULE_A_OUT,
#elif ((defined INCLUDE_COFF) || (defined INCLUDE_ECOFF) || \
       (defined INCLUDE_SOMCOFF))
		  MODULE_ECOFF,
#elif (defined INCLUDE_ELF)
		  MODULE_ELF,
#endif /* INCLUDE_AOUT */
		  HIDDEN_MODULE|LOAD_NO_SYMBOLS);
#endif /* INCLUDE_SYM_TBL_SYNC */

    printf ("\nAdding %ld symbols for standalone.\n", standTblSize);

    /* fill in from built in table*/

    for (ix = 0; (ULONG) ix < standTblSize; ix++)
#if	((CPU_FAMILY == ARM) && ARM_THUMB)
	thumbSymTblAdd (sysSymTbl, &(standTbl[ix]));
#else
	symTblAdd (sysSymTbl, &(standTbl[ix]));
#endif	/* CPU_FAMILY == ARM */
# if defined(INCLUDE_LOGGING) && defined(INCLUDE_LOG_STARTUP)
    logMsg ("sysSymTbl complete.\n", 1,2,3,4,5,6);
    taskDelay (2);	/* allow time for message to be displayed */
# endif	/* INCLUDE_LOGGING && INCLUDE_LOG_STARTUP */
#endif	/* INCLUDE_STANDALONE_SYM_TBL */

#ifdef  INCLUDE_NET_SYM_TBL
    sysSymTbl = symTblCreate (SYM_TBL_HASH_SIZE_LOG2, TRUE, memSysPartId);

    netLoadSymTbl ();				/* fill in table from host */
#endif	/* INCLUDE_NET_SYM_TBL */

#ifdef  INCLUDE_STAT_SYM_TBL
    statSymTbl = symTblCreate (STAT_TBL_HASH_SIZE_LOG2, FALSE, memSysPartId);

    for (ix = 0; (ULONG)ix < statTblSize; ix ++) /* fill in from builtin table*/
	symTblAdd (statSymTbl, &(statTbl [ix]));
#endif	/* INCLUDE_STAT_SYM_TBL */


    /* initialize C++ support library */

#if	defined (INCLUDE_CPLUS) && defined (INCLUDE_CPLUS_MIN)
#error	Define only one of INCLUDE_CPLUS or INCLUDE_CPLUS_MIN, not both
#endif

#if	defined (INCLUDE_CPLUS) || defined (INCLUDE_CPLUS_MIN)
#ifndef INCLUDE_CTORS_DTORS
#define INCLUDE_CTORS_DTORS
#endif
#endif

#ifdef  INCLUDE_CTORS_DTORS 
  /* 
   * call compiler generated init functions (usually - but not necessarily -
   * C++ related)
   */
   cplusCtorsLink ();
#endif

#ifdef	INCLUDE_CPLUS			/* all standard C++ runtime support */
    cplusLibInit ();
#endif

#ifdef	INCLUDE_CPLUS_MIN		/* minimal C++ runtime support */
    cplusLibMinInit ();
#endif

#ifdef INCLUDE_CPLUS_DEMANGLER
    cplusDemanglerInit ();
#endif

    /* initialize COM/DCOM runtime support */
#ifdef INCLUDE_COM
    comLibInit ();
#endif
#ifdef INCLUDE_DCOM
    dcomLibInit ();
#endif

    /* initialize Wind Web Server */

#ifdef INCLUDE_HTTP
    httpd ();
#endif /* INCLUDE_HTTP */

#ifdef   INCLUDE_RBUFF                  /* install rBuff support */
    rBuffLibInit();
#ifdef  INCLUDE_SHOW_ROUTINES
    rBuffShowInit ();                   /* install rBuff show routine */
#endif  /* INCLUDE_SHOW_ROUTINES */
#endif /* INCLUDE_RBUFF */

#ifdef INCLUDE_WINDVIEW
    windviewConfig ();
#endif /* INCLUDE_WINDVIEW */


    /* initialize the WDB debug agent */

#ifdef  INCLUDE_WDB

#ifdef PMC280_DUAL_CPU
    /*    
     * WDB again runs only on CPU0.
     */   
    if(!frcWhoAmI())
    {
#ifdef PMC280_DEBUG_UART_VX
        printf("CPU0: Before wdbConfig\n");
#endif /* PMC280_DEBUG_UART_VX */
 
# if defined(INCLUDE_LOGGING) && defined(INCLUDE_LOG_STARTUP)
    logMsg ("before wdbConfig()\n", 1,2,3,4,5,6);
    taskDelay (2);	/* allow time for message to be displayed */
# endif	/* INCLUDE_LOGGING && INCLUDE_LOG_STARTUP */
    wdbConfig();
# if defined(INCLUDE_LOGGING) && defined(INCLUDE_LOG_STARTUP)
    logMsg ("after wdbConfig()\n", 1,2,3,4,5,6);
    taskDelay (2);	/* allow time for message to be displayed */
# endif	/* INCLUDE_LOGGING && INCLUDE_LOG_STARTUP */
#ifdef PMC280_DEBUG_UART_VX
        printf("CPU0: After wdbConfig\n");
#endif /* PMC280_DEBUG_UART_VX */
    }

#else

# if defined(INCLUDE_LOGGING) && defined(INCLUDE_LOG_STARTUP)
    logMsg ("before wdbConfig()\n", 1,2,3,4,5,6);
    taskDelay (2);	/* allow time for message to be displayed */
# endif	/* INCLUDE_LOGGING && INCLUDE_LOG_STARTUP */
    wdbConfig();
# if defined(INCLUDE_LOGGING) && defined(INCLUDE_LOG_STARTUP)
    logMsg ("after wdbConfig()\n", 1,2,3,4,5,6);
    taskDelay (2);	/* allow time for message to be displayed */
# endif	/* INCLUDE_LOGGING && INCLUDE_LOG_STARTUP */
#endif /* PMC280_DUAL_CPU */

#ifdef	INCLUDE_WDB_BANNER

#if (WDB_COMM_TYPE == WDB_COMM_NETWORK)
#define WDB_COMM_TYPE_STR "WDB_COMM_NETWORK"
#endif /* WDB_COMM_TYPE == WDB_COMM_NETWORK */

#if (WDB_COMM_TYPE == WDB_COMM_SERIAL)
#define WDB_COMM_TYPE_STR "WDB_COMM_SERIAL"
#endif /* WDB_COMM_TYPE == WDB_COMM_SERIAL */

#if (WDB_COMM_TYPE == WDB_COMM_TYCODRV_5_2)
#define WDB_COMM_TYPE_STR "WDB_COMM_TYCODRV_5_2"
#endif /* WDB_COMM_TYPE == WDB_COMM_TYCODRV_5_2 */

#if (WDB_COMM_TYPE ==  WDB_COMM_NETROM)
#define WDB_COMM_TYPE_STR "WDB_COMM_NETROM"
#endif /* WDB_COMM_TYPE == WDB_COMM_NETROM */

#if (WDB_COMM_TYPE ==  WDB_COMM_VTMD)
#define WDB_COMM_TYPE_STR "WDB_COMM_VTMD"
#endif /* WDB_COMM_TYPE == WDB_COMM_VTMD */

#if (WDB_COMM_TYPE ==  WDB_COMM_END)
#define WDB_COMM_TYPE_STR "WDB_COMM_END"
#endif /* WDB_COMM_TYPE == WDB_COMM_END */

#if (WDB_COMM_TYPE ==  WDB_COMM_CUSTOM)
#define WDB_COMM_TYPE_STR "WDB_COMM_CUSTOM"
#endif /* WDB_COMM_TYPE == WDB_COMM_CUSTOM */

#if (WDB_COMM_TYPE ==  WDB_COMM_PIPE)
#define WDB_COMM_TYPE_STR "WDB_COMM_PIPE"
#endif /* WDB_COMM_TYPE == WDB_COMM_PIPE */

#ifndef WDB_COMM_TYPE_STR
#define WDB_COMM_TYPE_STR "Unknown"
#endif /* WDB_COMM_TYPE_STR */

#ifndef INCLUDE_SHELL
    /* WDB banner same as printed by usrWdbBanner */
    printf ("\n\n");
    printf ("%23s\n\n", runtimeName);
    printf ("Copyright 1984-2002  Wind River Systems, Inc.\n\n");
    printf ("            CPU: %s\n", sysModel ());
    printf ("   Runtime Name: %s\n", runtimeName);
    printf ("Runtime Version: %s\n", runtimeVersion);
    printf ("    BSP version: " BSP_VERSION BSP_REV "\n");
    printf ("        Created: %s\n", creationDate);
    printf ("  WDB Comm Type: %s\n", WDB_COMM_TYPE_STR);
#ifdef PMC280_DUAL_CPU
    if(!frcWhoAmI())
    {
    printf ("            WDB: %s.\n\n",
	    ((wdbRunsExternal () || wdbRunsTasking ()) ?
		 "Ready" : "Agent configuration failed") );
    }
#else /* PMC280_DUAL_CPU */
    printf ("            WDB: %s.\n\n",
	    ((wdbRunsExternal () || wdbRunsTasking ()) ?
		 "Ready" : "Agent configuration failed") );
#endif /* PMC280_DUAL_CPU */
#endif /*INCLUDE_SHELL*/

#endif /*INCLUDE_WDB_BANNER*/

#endif  /* INCLUDE_WDB */
#ifdef ROHS 
 cpldGppDisablePin13(); /* Added to pull down the GPP pin 13 after stable booting */
#endif
    /* initialize interactive shell */

#ifdef  INCLUDE_SHELL
#ifdef	INCLUDE_SECURITY			/* include shell security */
    if ((sysFlags & SYSFLG_NO_SECURITY) == 0)
	{
        loginInit ();				/* initialize login table */
        shellLoginInstall (loginPrompt, NULL);	/* install security program */

	/* add additional users here as required */

        loginUserAdd (LOGIN_USER_NAME, LOGIN_PASSWORD);
	}
#endif	/* INCLUDE_SECURITY */

    printLogo ();				/* print out the banner page */

    printf ("                               ");
    printf ("CPU: %s.  Processor #%d.\n", sysModel (), sysProcNumGet ());
    printf ("                              ");
#ifdef	HITACHI_SH_KERNEL_ON_SDRAM
    printf ("Memory Size: 0x%x.", (UINT)(sysMemTop () - (char *)FREE_RAM_ADRS));
#else	/* HITACHI_SH_KERNEL_ON_SDRAM */
    printf ("Memory Size: 0x%x.", (UINT)(sysMemTop () - (char *)LOCAL_MEM_LOCAL_ADRS));
#endif	/* HITACHI_SH_KERNEL_ON_SDRAM */
    printf ("  BSP version " BSP_VERSION BSP_REV ".");
#if defined(INCLUDE_WDB) && defined(INCLUDE_WDB_BANNER)
#ifdef PMC280_DUAL_CPU
    /*
     * We are not interested in running WDB agent on CPU1.
     */
    if(!frcWhoAmI())
    { 
    printf ("\n                             ");
    printf ("WDB Comm Type: %s", WDB_COMM_TYPE_STR);
    printf ("\n                            ");
    printf ("WDB: %s.",
	    ((wdbRunsExternal () || wdbRunsTasking ()) ?
		 "Ready" : "Agent configuration failed") );
    }
#else
    printf ("\n                             ");
    printf ("WDB Comm Type: %s", WDB_COMM_TYPE_STR);
    printf ("\n                            ");
    printf ("WDB: %s.",
	    ((wdbRunsExternal () || wdbRunsTasking ()) ?
		 "Ready" : "Agent configuration failed") );
#endif /* PMC280_DUAL_CPU */
#endif /*INCLUDE_WDB && INCLUDE_WDB_BANNER*/
    printf ("\n\n");

#ifdef	INCLUDE_STARTUP_SCRIPT			/* run a startup script */
    if (sysBootParams.startupScript [0] != EOS)
	usrStartupScript (sysBootParams.startupScript);
#endif	/* INCLUDE_STARTUP_SCRIPT */

	
    shellInit (SHELL_STACK_SIZE, TRUE);		/* create the shell */


    /* only include the simple demo if the shell is NOT included */

#else
#if defined(INCLUDE_DEMO)			/* create demo w/o shell */
    taskSpawn ("demo", 20, 0, 2000, (FUNCPTR)usrDemo, 0,0,0,0,0,0,0,0,0,0);
#endif						/* mips cpp no elif */

#endif	/* INCLUDE_SHELL */

#ifdef  INCLUDE_WINDML
    usrWindMlInit ();
#endif  /* INCLUDE_WINDML */

#if defined (INCLUDE_SOUND) && defined(INCLUDE_SB16)
    sb16Drv ();            /* install sound driver SB16 */

    sb16DevCreate ("/sound", 0x220, 5, 1, 5);
#endif /* INCLUDE_SOUND && INCLUDE_SB16 */

#if defined(INCLUDE_JAVA)
    javaConfig ();
#endif	/* INCLUDE_JAVA */

#ifdef INCLUDE_HTML
    usrHtmlInit ();
#endif /* INCLUDE_HTML */

#if CPU==SIMNT
    win_ReleaseMutex(simUpMutex);
#endif

#ifdef INCLUDE_USER_APPL
    /* Startup the user's application */

    USER_APPL_INIT;	/* must be a valid C statement or block */
#endif

	/*sergey: call my Dma initialization*/
    usrDmaInit();

    }

/*******************************************************************************
*
* usrClock - user-defined system clock interrupt routine
*
* This routine is called at interrupt level on each clock interrupt.
* It is installed by usrRoot() with a sysClkConnect() call.
* It calls all the other packages that need to know about clock ticks,
* including the kernel itself.
*
* If the application needs anything to happen at the system clock interrupt
* level, it can be added to this routine.
*
* RETURNS: N/A
*/

void usrClock ()

    {
    tickAnnounce ();	/* announce system tick to kernel */
    }

#ifdef	INCLUDE_DEMO

/********************************************************************************
* usrDemo - example application without shell
*
* This routine is spawned as a task at the end of usrRoot(), if INCLUDE_DEMO
* is defined, and INCLUDE_SHELL is NOT defined in configAll.h or config.h.
* It is intended to indicate how a shell-less application can be linked,
* loaded, and ROMed.
*
* NOMANUAL
*/

void usrDemo (void)

    {
    char string [40];

    printf ("VxWorks (for %s) version %s.\n", sysModel (), vxWorksVersion);
    printf ("Kernel: %s.\n", kernelVersion ());
    printf ("Made on %s.\n", creationDate);

    FOREVER
        {
        printf ("\nThis is a test.  Type something: ");
        fioRdString (STD_IN, string, sizeof (string));
	printf ("\nYou typed \"%s\".\n", string);

	if (strcmp (string, "0") == 0)
	    memShow (0);

	if (strcmp (string, "1") == 0)
	    memShow (1);
        }
    }

#endif	/* INCLUDE_DEMO */

#ifdef PMC280_DUAL_CPU
/*
 * The PTEs are all setup in frcMmu.c (or in the boot loader) and this
 * needs to be communicated to VxWorks. This is being done using locations
 * in the shared memory, in particular the application shared memory area.
 * After this initialisation is done, shared memory applications are free to
 * use this particular region.
 */

#include "private/vmLibP.h"
#define NUM_PAGE_STATES 256

LOCAL STATE_TRANS_TUPLE mmuStateTransArrayLocal [] =
    {
    {VM_STATE_MASK_VALID, MMU_STATE_MASK_VALID,
     VM_STATE_VALID, MMU_STATE_VALID},

    {VM_STATE_MASK_VALID, MMU_STATE_MASK_VALID,
     VM_STATE_VALID_NOT, MMU_STATE_VALID_NOT},

    {VM_STATE_MASK_WRITABLE, MMU_STATE_MASK_WRITABLE,
     VM_STATE_WRITABLE, MMU_STATE_WRITABLE},

    {VM_STATE_MASK_WRITABLE, MMU_STATE_MASK_WRITABLE,
     VM_STATE_WRITABLE_NOT, MMU_STATE_WRITABLE_NOT},

    {VM_STATE_MASK_CACHEABLE, MMU_STATE_MASK_CACHEABLE,
     VM_STATE_CACHEABLE, MMU_STATE_CACHEABLE},

    {VM_STATE_MASK_CACHEABLE, MMU_STATE_MASK_CACHEABLE,
     VM_STATE_CACHEABLE_NOT, MMU_STATE_CACHEABLE_NOT},

    {VM_STATE_MASK_CACHEABLE, MMU_STATE_MASK_CACHEABLE,
     VM_STATE_CACHEABLE_WRITETHROUGH, MMU_STATE_CACHEABLE_WRITETHROUGH},
 
    {VM_STATE_MASK_MEM_COHERENCY, MMU_STATE_MASK_MEM_COHERENCY,
     VM_STATE_MEM_COHERENCY, MMU_STATE_MEM_COHERENCY},
 
    {VM_STATE_MASK_MEM_COHERENCY, MMU_STATE_MASK_MEM_COHERENCY,
     VM_STATE_MEM_COHERENCY_NOT, MMU_STATE_MEM_COHERENCY_NOT},
 
    {VM_STATE_MASK_GUARDED, MMU_STATE_MASK_GUARDED,
     VM_STATE_GUARDED, MMU_STATE_GUARDED},
 
    {VM_STATE_MASK_GUARDED, MMU_STATE_MASK_GUARDED,
     VM_STATE_GUARDED_NOT, MMU_STATE_GUARDED_NOT},
    };
 
/* Globals defined elsewhere */
IMPORT VM_CONTEXT *currentContext;
IMPORT int mutexOptionsVmBaseLib;
IMPORT unsigned int vmStateTransTbl[];
IMPORT unsigned int vmMaskTransTbl[];
IMPORT STATE_TRANS_TUPLE *mmuStateTransArray;
IMPORT int mmuStateTransArraySize;
 
/* Global within this file */
#ifdef PMC280_DEBUG_CACHE
static unsigned int dbgAddr = 0x0;
static PTE *temp;
#endif /* PMC280_DEBUG_CACHE */
 
static VM_CONTEXT  frcsysVmContext;
static MMU_TRANS_TBL  frcmmuGlobalTransTbl;
static PTE  pte;
 
/* Routines called here but defined elsewhere */
extern STATUS mmuPpcEnable(BOOL);
extern STATUS mmuPpcPteGet(MMU_TRANS_TBL *, void *, PTE **);
 
void frcSetupVmContext()
{
    unsigned int i, j, newState, newMask;
 
    /* Load from application shared memory area */
    if(frcWhoAmI())
    {
      frcmmuGlobalTransTbl.hTabOrg = READ_DWORD((int *)HTABORG_1);
      frcmmuGlobalTransTbl.hTabMask = READ_DWORD((int *)HTABMASK_1);
      frcmmuGlobalTransTbl.pteTableSize = READ_DWORD((int *)PTETABLESIZE_1);
#ifdef PMC280_DEBUG_CACHE
      dbg_printf1("hTabOrg = 0x%08x\n", frcmmuGlobalTransTbl.hTabOrg);
      dbg_printf1("hTabMask = 0x%08x\n", frcmmuGlobalTransTbl.hTabMask);
      dbg_printf1("pteTableSize = 0x%08x\n", frcmmuGlobalTransTbl.pteTableSize);
#endif /* PMC280_DEBUG_CACHE */
    }
    else
    {
      frcmmuGlobalTransTbl.hTabOrg = READ_DWORD((int *)HTABORG_0);
      frcmmuGlobalTransTbl.hTabMask = READ_DWORD((int *)HTABMASK_0);
      frcmmuGlobalTransTbl.pteTableSize = READ_DWORD((int *)PTETABLESIZE_0);
#ifdef PMC280_DEBUG_CACHE
      dbg_printf0("hTabOrg = 0x%08x\n", frcmmuGlobalTransTbl.hTabOrg);
      dbg_printf0("hTabMask = 0x%08x\n", frcmmuGlobalTransTbl.hTabMask);
      dbg_printf0("pteTableSize = 0x%08x\n", frcmmuGlobalTransTbl.pteTableSize);
#endif /* PMC280_DEBUG_CACHE */
    }
      
    currentContext = &frcsysVmContext;
 
    objCoreInit (&currentContext->objCore, (CLASS_ID) vmContextClassId);
 
    semMInit (&currentContext->sem, mutexOptionsVmBaseLib);
 
    currentContext->mmuTransTbl = &frcmmuGlobalTransTbl;
 
    mmuStateTransArray = &mmuStateTransArrayLocal [0];
 
    mmuStateTransArraySize =
                sizeof (mmuStateTransArrayLocal) / sizeof (STATE_TRANS_TUPLE);
 
#ifdef PMC280_DEBUG_CACHE
    if(frcWhoAmI())
    { 
        dbg_printf1("mmuStateTransArraySize = %d\n", mmuStateTransArraySize);
    }
    else
    {
        dbg_printf0("mmuStateTransArraySize = %d\n", mmuStateTransArraySize);
    }
#endif /* PMC280_DEBUG_CACHE */
 
    /* initialize the page state translation table.  This table is used to
     * translate betseen architecture independent state values and architecture     * dependent state values */
 
    for (i = 0; i < NUM_PAGE_STATES; i++)
        {
        newState = 0;
         
        for (j = 0; j < mmuStateTransArraySize; j++)
            {
            STATE_TRANS_TUPLE *thisTuple = &mmuStateTransArray[j];
            UINT archIndepState = thisTuple->archIndepState;
            UINT archDepState = thisTuple->archDepState;
            UINT archIndepMask = thisTuple->archIndepMask;
 
            if ((i & archIndepMask) == archIndepState)
                newState |= archDepState;
            }
 
        vmStateTransTbl [i] = newState;
        }
 
    /* initialize the page state mask translation table.  This table is used to     * translate betseen architecture independent state masks and architecture
     * dependent state masks */
 
    for (i = 0; i < NUM_PAGE_STATES; i++)
        {
        newMask = 0;
        for (j = 0; j < mmuStateTransArraySize; j++)
            {
            STATE_TRANS_TUPLE *thisTuple = &mmuStateTransArray[j];
            UINT archIndepMask = thisTuple->archIndepMask;
            UINT archDepMask = thisTuple->archDepMask;
 
            if ((i & archIndepMask) == archIndepMask)
                newMask |= archDepMask;
            }
 
        vmMaskTransTbl [i] = newMask;
        }
 
    return;
}

/********************************************************************************
* frcmmuPpcPteUpdate - update a PTE value
*
*/
LOCAL void  frcmmuPpcPteUpdate
    (
    PTE *       pteAddr,        /* address of the PTE to update */
    PTE *       pteVal          /* PTE value */
    )
{
    int lockKey;

#ifdef PMC280_DEBUG_CACHE
    int i;
    printf("*frcmmuPpcPteUpdate: pteAddr = 0x%08x pteVal = 0x%08x
            pteVal = 0x%08x!\n", pteAddr, pteVal,
          ((char *)pteVal + PHY_CPU1_MEM_BOT));

#endif /* PMC280_DEBUG_CACHE */

    lockKey = intLock();

    if ((mmuPpcIEnabled) | (mmuPpcDEnabled))
        {
            if(frcWhoAmI())
            {
                /*
                 * Since MMU needs to be turned off for this operation, we
                 * do all the three operations below, namely disabling MMU,
                 * updating the page table entry and enabling MMU in a single
                 * assembly routine. This way we will not need to adjust stack
                 * pointers or any link addresses.
                 */
                 frcUpdatePTEntry(pteAddr,(((char*)pteVal) + PHY_CPU1_MEM_BOT));
            }
            else
            {
                frcUpdatePTEntry(pteAddr, pteVal);
            }
 
#ifdef PMC280_DEBUG_CACHE
            mmuPpcPteShow(currentContext->mmuTransTbl, dbgAddr);
#endif /* PMC280_DEBUG_CACHE */
        }
    else
        memcpy ((void *) pteAddr, pteVal, sizeof (PTE));
 
    intUnlock(lockKey);
 
#ifdef PMC280_DEBUG_CACHE
    printf("*frcmmuPpcPteUpdate: Exiting!\n");
#endif
}

/*******************************************************************************
* frcmmuPpcStateSet - set state of virtual memory page
*
*
* MMU_STATE_VALID       MMU_STATE_VALID_NOT     valid/invalid
* MMU_STATE_WRITABLE    MMU_STATE_WRITABLE_NOT  writable/writeprotected
* MMU_STATE_CACHEABLE   MMU_STATE_CACHEABLE_NOT cachable/notcachable
* MMU_STATE_CACHEABLE_WRITETHROUGH
* MMU_STATE_CACHEABLE_COPYBACK
* MMU_STATE_MEM_COHERENCY       MMU_STATE_MEM_COHERENCY_NOT
* MMU_STATE_GUARDED             MMU_STATE_GUARDED_NOT
* MMU_STATE_NO_ACCESS
* MMU_STATE_NO_EXECUTE
*/
STATUS frcmmuPpcStateSet
    (
    MMU_TRANS_TBL *     pTransTbl,      /* translation table */
    void *              effectiveAddr,  /* page whose state to modify */
    UINT                stateMask,      /* mask of which state bits to modify */    UINT                state           /* new state bit values */
    )
    {
    PTE *       pPte;           /* PTE address */
     
#ifdef PMC280_DEBUG_CACHE
    printf("*frcmmuPpcStateSet: effectiveAddr = 0x%08x state = 0x%08x\n",
            effectiveAddr, state);
#endif /* PMC280_DEBUG_CACHE */
 
    /*
     * Try to find in the PTEG table pointed to by the pTransTbl structure,
     * the PTE corresponding to the <effectiveAddr>.
     * If this PTE can not be found then return ERROR.
     */
    if (mmuPpcPteGet (pTransTbl, effectiveAddr, &pPte) != OK)
        {
        errno = S_mmuLib_NO_DESCRIPTOR;
        return (ERROR);
        }
 
#ifdef PMC280_DEBUG_CACHE
    printf ("v    = %d\n", pPte->field.v);
    printf ("vsid = 0x%x\n", pPte->field.vsid);
    printf ("h    = %d\n", pPte->field.h);
    printf ("api  = 0x%x\n", pPte->field.api);
    printf ("rpn  = 0x%x\n", pPte->field.rpn);
    printf ("r    = %d\n", pPte->field.r);
    printf ("c    = %d\n", pPte->field.c);
    printf ("wimg = 0x%x\n", pPte->field.wimg);
    printf ("pp   = 0x%x\n", pPte->field.pp);
    printf ("effectiveAddr = 0x%08x\n", effectiveAddr);
    dbgAddr = effectiveAddr;
#endif /* PMC280_DEBUG_CACHE */
 
    /*
     * Check if the state to set page corresponding to <effectiveAddr> will
     * not set the cache in inhibited and writethrough mode. This mode is not
     * supported by the cache.
     */  

    if ((stateMask & MMU_STATE_MASK_CACHEABLE) &&
        (state & MMU_STATE_CACHEABLE_NOT) &&
        (state & MMU_STATE_CACHEABLE_WRITETHROUGH))
        {
        return (ERROR);
        }
 
    /* load the value of the PTE pointed to by pPte to pte */
 
    pte = *pPte;
 
    /* set or reset the VALID bit if requested */
 
    pte.bytes.word0 = (pte.bytes.word0 & ~(stateMask & MMU_STATE_MASK_VALID)) |                        (state & stateMask & MMU_STATE_MASK_VALID);
 
    /* set or reset the WIMG bit if requested */
 
    pte.bytes.word1 = (pte.bytes.word1 &
                        ~(stateMask & MMU_STATE_MASK_WIMG_AND_WRITABLE)) |
                         (state & stateMask & MMU_STATE_MASK_WIMG_AND_WRITABLE);
 
    /* update the PTE in the table */
 
#ifdef PMC280_DEBUG_CACHE
    temp = &pte;
    printf ("v    = %d\n", temp->field.v);
    printf ("vsid = 0x%x\n", temp->field.vsid);
    printf ("h    = %d\n", temp->field.h);
    printf ("api  = 0x%x\n", temp->field.api);
    printf ("rpn  = 0x%x\n", temp->field.rpn);
    printf ("r    = %d\n", temp->field.r);
    printf ("c    = %d\n", temp->field.c);
    printf ("wimg = 0x%x\n", temp->field.wimg);
    printf ("pp   = 0x%x\n", temp->field.pp);
#endif /* PMC280_DEBUG_CACHE */
 
    frcmmuPpcPteUpdate (pPte, &pte);
 
    /* update the Translation Lookside Buffer */
 
    mmuPpcTlbie (effectiveAddr);
 
#ifdef PMC280_DEBUG_CACHE
    printf("*frcmmuPpcStateSet exited!\n");
#endif /* PMC280_DEBUG_CACHE */
 
    return (OK);
}

UINT32 getSdramSize(void) {
    unsigned int bank0size, bank1size;
    bank0size=BYTE_SWAP_32(*(UINT32 *)(INTERNAL_REG_ADRS | BANK0_SIZE_REG));
    bank1size=BYTE_SWAP_32(*(UINT32 *)(INTERNAL_REG_ADRS | BANK1_SIZE_REG));
    return((bank0size + bank1size) * _1M);
} 
#endif /* PMC280_DUAL_CPU */
