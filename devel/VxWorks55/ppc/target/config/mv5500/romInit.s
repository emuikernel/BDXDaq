/* romInit.s - Motorola MVME5500 ROM initialization module */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1996-2002 Motorola, Inc. All Rights Reserved */

    .data
    .globl  copyright_wind_river
    .long   copyright_wind_river

/*
modification history
--------------------
01j,03jul03,yyz Select InitAct for MPP control register 3 pin 26 to fix the
		booting when SROM initialization is enabled (J17).
01i,03jul03,scb Optimize values for HID0 for performance and cache consistency.
01h,21mar03,scb Remove the setting of L2CR[IONLY] and L3CR[IONLY] from
	        the dCacheOff routine if L2 cacheing not defined.
01g,30apr03,yyz Removed iCacheOff() before going to the C Entry, and
		change to read only 64KB in memoryScrub to flush data cache.
01f,16apr03,scb Set EIDIS outside of dCacheOff() function.
01e,09apr03,yyz Fixed formatting for T2.2 compliance.
01d,08apr03,scb Set EIDIS in MSSCR0 register for MPX mode to work.
01c,02dec02,scb Clean up declaration of externals and imported functions.
01b,25oct02,scb Turn of watch dog correctly now that errata of GT-64260A
		is fixed int GT-64260B.
01a,01oct02,scb Port from hxeb100 base (ver 01f).
*/

/*
DESCRIPTION
This module contains the entry code for the VxWorks bootrom.
The entry point romInit, is the first code executed on power-up.
It sets the BOOT_COLD parameter to be passed to the generic
romStart() routine.

The routine sysToMonitor() jumps to the location 4 bytes
past the beginning of romInit, to perform a "warm boot".
This entry point allows a parameter to be passed to romStart().

This code is intended to be generic across PowerPC 745x boards.
Hardware that requires special register setting or memory
mapping to be done immediately, may do so here.
*/

#define    _ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "asm.h"
#include "config.h"
#include "regs.h"    
#include "mv64260.h"
#include "mv5500A.h"

#include "arch/ppc/mmu603Lib.h"
#include "arch/ppc/ppc603.h"

#define DSSALL .long 0x7E00066C  /* explicit encoding for DSSALL instruction */

#define MSSCR0		    1014	 /* Memory Subsystem Control Register */
#define _PPC_MSCCR0_BIT_ID  26		 /* ID bit of MSCCR0 register */
#define L2CR		    1017	 /* L2 Cache Control Register */
#define L3CR		    1018	 /* L3 Cache Control Register */
#define PPC_L2CR_L2IO	    0x00100000   /* L2 Instruction-Only Mode */
#define PPC_L3CR_L3IO	    0x00400000	 /* L3 Instruction-Only Mode */

#define PPC_HID0_TBEN	    0x04000000	 /* Time Base Enable - bit 5 */
#define PPC_HID0_BTIC	    0x00000020	 /* Branch history table enable */
#define PPC_HID0_LRSTK	    0x00000010	 /* Link register stack enable */
#define PPC_HID0_FOLD	    0x00000008	 /* Branch folding enable */

#define PPC_HID1_EMCP	    0x80000000	 /* Machine Check Signal Enable - */
                                         /* Bit 0 */ 

#define DEFAULT_HID0_SET    (_PPC_HID0_EMCP | PPC_HID0_TBEN | \
			     _PPC_HID0_SIED | PPC_HID0_BTIC | \
			     PPC_HID0_LRSTK | PPC_HID0_FOLD | \
			     _PPC_HID0_BHTE )
			     
#define CACHE_STACK_ADRS    (0x00003000) /* No-RAM cache initial stack top */
#define CACHE_STACK_SIZE    (8*1024)

/* 
 * The following register is zero on powerup.  It is used as a software lock 
 * to allow one processor to claim the machine while the other processor
 * sleeps.
 */

#define CLAIM_REG	    IDMA_SRC_ADDR_CHAN0

    /* Exported internal functions */

    FUNC_EXPORT(_romInit)       	/* start of system code */
    FUNC_EXPORT(romInit)        	/* start of system code */

    /* externals */

    FUNC_IMPORT(romStart)		/* system initialization routine */ 
    FUNC_IMPORT(sysMv64260SdramInit)	/* Memory Controller Initialization */

    _WRS_TEXT_SEG_START
    .balign 2

/******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*
* romInit
*     (
*     int startType    /@ only used by 2nd entry point @/
*     )
*/

FUNC_LABEL(_romInit)
FUNC_BEGIN(romInit)

    /* This is the cold boot entry (ROM_TEXT_ADRS) */

    bl    cold

_romInitWarm:
romInitWarm:

    /* This is the warm boot entry (ROM_WARM_ADRS) */

    bl    warm

    /* copyright notice appears at beginning of ROM (in TEXT segment) */

    .ascii   "Copyright 1984-1999 Wind River Systems, Inc."
    .balign 2

cold:
    li    r31, BOOT_COLD
    bl    start          /* skip over next instruction */

            
warm:
    or    r31, r3, r3    /* startType to r31 */

start:

    /* Zero-out registers: r0 & SPRGs */

    xor     r0,r0,r0
    mtspr   272,r0
    mtspr   273,r0
    mtspr   274,r0
    mtspr   275,r0
    mtspr   276,r0
    mtspr   277,r0
    mtspr   278,r0
    mtspr   279,r0

    /* initialize the stack pointer */

    lis     sp, HI(CACHE_STACK_ADRS)
    ori     sp, sp, LO(CACHE_STACK_ADRS)

    /*
     * Set HID0 to a known state by setting the values defined by 
     * DEFAULT_HID0_SET.
     *
     * Synchronization requirements: After setting the HID0, an
     * isync must follow.
     */

    lis     r3,HI(DEFAULT_HID0_SET)
    ori     r3,r3,LO(DEFAULT_HID0_SET)
    isync
    mtspr   HID0,r3
    isync

    /*
     * At this point we would normally want to set the Machine Check 
     * Signal Enable (EMCP) in HID1.  However, on the boards which use
     * a Discovery system controller, the MCP signal is connected to a 
     * pull-up and therefore not used as it is on other boards.
     */

    /*
     * Set MSR to a known state 
     *
     * At power-up the IP bit is set to 1.  The IP (Exception Prefix) 
     * bit specifies the physical address for exceptions.  If set to 0, 
     * exceptions are vectored to the physical address 0x000n_nnnn, 
     * which is the setting we desire.  If set to 1, exceptions are 
     * vectored to the physical address 0xFFFn_nnnn.  
     *
     * Synchronization requirements: there is no specific synchronization
     * requirement for clearing all bits in the MSR, therefore the 
     * strictest synchronization will be applied.  This means a DSSALL and
     * sync preceding, as well as a sync and isync following.
     */

    andi.   r3, r3, 0
    DSSALL
    sync
    mtmsr   r3
    sync
    isync

    /* Set the EIDIS bit in the MSSCR0 so MPX mode works */

    mfspr   r4,MSSCR0              /* r4 = MSSCR0 */
    addis   r5,r0,0x0100           /* Set EIDIS bit (for MPX mode to work) */
    or      r4,r4,r5
    mtspr   MSSCR0,r4              
    sync                          
    isync 

    /* 
     * Init the MMU Segment registers 
     *
     * Synchronization requirements: According to the MPC7450 RISC
     * Microprocessor Family User's Manual a dssall and sync must precede
     * a mtsr, and a sync and isync must follow.  But since this is 
     * initialization code an isync before and after each mtsr will be 
     * considered sufficient.
     */

    andi.   r3, r3, 0
    isync
    mtsr    0,r3
    isync
    mtsr    1,r3
    isync
    mtsr    2,r3
    isync
    mtsr    3,r3
    isync
    mtsr    4,r3
    isync
    mtsr    5,r3
    isync
    mtsr    6,r3
    isync
    mtsr    7,r3
    isync
    mtsr    8,r3
    isync
    mtsr    9,r3
    isync
    mtsr    10,r3
    isync
    mtsr    11,r3
    isync
    mtsr    12,r3
    isync
    mtsr    13,r3
    isync
    mtsr    14,r3
    isync
    mtsr    15,r3
    isync

    /*
     * Load the MV64620 internal register base address into r3, it will
     * be used as the base for the rest of this routine.
     */

    lis     r3,HI(MV64260_REG_BASE)   /* r3 = MV64260 Internal Registers Base */
    ori     r3,r3,LO(MV64260_REG_BASE)  /* address at power-up */

    /* 
     * Disable MV64260 Watchdog Timer 
     *
     * A write sequence of '01' followed by '10' into CTL1(25:24) of the
     * Watchdog Configuration Register will disable the watchdog timer
     * when the EN bit is equal to 1 (ie. the watchdog is enabled),
     * otherwise this write sequence will enable the watchdog timer.
     * Note that rev A of the Discovery I prevents us from successfully
     * reading the watchdog - we always read all F's which would cause
     * the following algorithm to loop forever.  Hence we have cut
     * out the instruction which causes us to loop (#ifdef 0 below).
     * It's all probably a moot point however because we are also
     * going to be programming the GPP I/O control register bits
     * 24 (WDNMI#) and 25 (WDE#) to have a value of 0 (input).  This
     * will prevent the watchdog from affecting the processor even
     * it it does pop.  When subsequent revs of the Discovery I are 
     * release the instruction that is cut out could be reinserted.
     */

disableWatchdog:
    addis   r4,r0,0x0000              /* r4 = 0x00000599 */        
    ori     r4,r4,0x0599	      /* MPP24 = WDNMI*, MPP25 = WDE*,
					 MPP26 = InitAct */
    addis   r5,r0,0x0000              /* r5 = MPP Control 3 */
    ori     r5,r5,MPP_CTRL3           /*      Register offset */
    isync
    stwbrx  r4,r3,r5                  /* write r4 to MPP Control 3 reg */ 
    eieio                             /* synchronize */
    sync
    addis   r4,r0,0xFCFF              /* r4 = mask of 0xFCFFFFFF */        
    ori     r4,r4,0xFFFF
    addis   r5,r0,0x0000              /* r5 = Watchdog Configuration */
    ori     r5,r5,WDOG_CFG            /*      Register offset */
    isync
    lwbrx   r10,r3,r5                 /* r10 = watchdog configuration reg */ 
    sync
    and     r10,r10,r4                /* r10 = watchdog config reg */
    sync                              /* with CTL1 bits set to '00' */
    oris    r6,r10,0x0100             /* r6 = watchdog config reg */ 
                                      /* with CTL1 bits set to '01' */
    stwbrx  r6,r3,r5                  /* write r6 to watchdog config reg */ 
    eieio                             /* synchronize */
    sync
    oris    r6,r10,0x0200             /* r6 = watchdog config reg */
                                      /* with CTL1 bits set to '10' */
    stwbrx  r6,r3,r5                  /* write r6 to watchdog config reg */
    eieio                             /* synchronize */
    sync
    lwbrx   r10,r3,r5                 /* Read register to ensure disabled */ 
    sync
    addis   r4,r0,0x8000              /* r4 = mask 0f 0x80000000 */
    ori     r4,r4,0x0000
    and.    r10,r10,r4                /* r10 = watchdog config reg with */
                                      /* all bits except 31 (EN) masked */
    /* 
     * Note that the 1st rev of the Discovery I will cause a forever loop 
     * if the following instruction is included.  For now we are disabling
     * the loop, assuming that the watchdog comes up enabled and that one
     * pass through the loop disables the watchdog.  This will be the
     * standard board configuration.  As further protection however,
     * GPP pins 24 and 25 are programmed as inputs (during interrupt
     * controller initialization in sysHwInit2()).  Programming these
     * two pins as inputs should nullify any effect that the GT-64260
     * watchdog timer has on the processor.
     */

    bne     disableWatchdog       /* 1st rev of Discovery I loops forever */

    lis	   r5,HI(GPP_IO_CTRL)
    ori	   r5,r5,LO(GPP_IO_CTRL)

    lis	   r6,HI(GPP_IO_CTRL_VAL)
    ori	   r6,r6,LO(GPP_IO_CTRL_VAL)

    stwbrx r6,r3,r5		/* Stuff the GPP I/O control register */
    eieio  
    sync
    lwbrx  r6,r3,r5		/* Read register to push it out. */

    /*
     * Let's be completely sure of the watch dog value register by 
     * stuffing it with zero.
     */

    lis	   r5,HI(WDOG_VAL)
    ori	   r5,r5,LO(WDOG_VAL)
    stwbrx r0,r3,r5
    eieio
    sync
    lwbrx  r6,r3,r5		/* Read register to push it out. */

    /* Program the address mapping registers of the MV64260 */

    bl	    addrMapInit

    /*
     * System Memory Configuration and Initialization
     *
     * On a cold boot only, the system memory controller must be
     * configured and the DRAM must be scrubbed.  On a warm boot this 
     * is not necessary so proceed to the "C Entry Point".
     */

memConfig:
    cmpwi   r31,BOOT_COLD           /* check for warm boot */
    bne     goCEntry                /* if warm boot, skip memory config */

    /*
     * In order to program the memory controller we need to have
     * access to some sort of memory in order for our C routine,
     * to compute and then store our memory controller parameters.  
     * Since the memory controller controls DRAM, and since we are 
     * initializing the controller, we cannot use DRAM for this, we 
     * must use some other memory.  The memory that we will use is 
     * the L1 cache in write-back mode.  All references within this 
     * space will not be flushed out to DRAM.  We accomplish this in 
     * the following manner:
     *
     * 1. Program the data BATs for memory management.  We
     *    must have some type of memory management operating
     *    if we are going to use data cache in copyback mode.
     *    Use the DBAT0 to access the first 256MB of system
     *    memory (cacheing allowed) and DBAT1 to access the MV64260 
     *    registers and FLASH bank A (cache inhibited).
     *    All other DBATs are disabled (2 through 7).
     *    This is necessary in order to configure the controller.  Once
     *    the controller is programmed we no longer need the DBATs.
     *
     * 2. Invalidate the data cache, instruction cache, and TLB entries.
     *
     * 4. Enable the data MMU.
     *
     * 5. Turn on the data cache and instruction cache.
     *
     * 6. Zero out some of the L1 data cache in the address range
     *    occupied by the stack so that the cache tag entires are
     *    present and all future stack access hit the L1 cache and
     *    don't go out to DRAM.
     */

    bl      setDbats

    /* DBAT contents arranged DBAT0L, DBAT0H, DBAT1L, ... */

    /* first 256 MB of RAM starting at 0x00000000 */

    .long   ((0x00000000    & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |\
            _MMU_LBAT_GUARDED)
    .long   ((0x00000000    &_MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_256M |\
            _MMU_UBAT_VS | _MMU_UBAT_VP)

    /* 256 MB MV64260 register and FLASH bank A and Device Bus (CS1) */

    .long   ((0xF0000000    & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |\
            _MMU_LBAT_CACHE_INHIBIT | _MMU_LBAT_GUARDED)
    .long   ((0xF0000000    & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_256M |\
            _MMU_UBAT_VS | _MMU_UBAT_VP)

    /* Disable DBAT2 */

    .long   (0x00000000)
    .long   (0x00000000)

    /* Disable DBAT3 */

    .long   (0x00000000)
    .long   (0x00000000) 

    /* 
     * DBATs 4 - 7
     *
     * It is not necessary to explicitly disable DBATs 4 through 7.  At 
     * power-up these DBATs are disabled - HID0[HIGH_BAT_EN] = 0 by
     * default.
     */

    /* 
     * load DBATs from table above 
     *
     * Synchronization requirements: A DSSALL and sync must precede the
     * mtspr and then a sync and isync must follow.
     */

setDbats:
    DSSALL
    sync
    mfspr   r4,LR
    addi    r4,r4,-4
    lwzu    r3,4(r4)
    mtspr   DBAT0L,r3
    lwzu    r3,4(r4)
    mtspr   DBAT0U,r3
    lwzu    r3,4(r4)
    mtspr   DBAT1L,r3
    lwzu    r3,4(r4)
    mtspr   DBAT1U,r3
    lwzu    r3,4(r4)
    mtspr   DBAT2L,r3
    lwzu    r3,4(r4)
    mtspr   DBAT2U,r3
    lwzu    r3,4(r4)
    mtspr   DBAT3L,r3
    lwzu    r3,4(r4)
    mtspr   DBAT3U,r3
    sync
    isync

    /*
     * Zero out the SDR1 register.  We do this since we do not wish to
     * use the "page table" part of the MMU, but rather the DBATs.
     *
     * Synchronization requirements: A DSSALL and sync must precede a
     * mtspr and then a sync and isync must follow.
     */

    xor     r0,r0,r0
    DSSALL
    sync
    mtspr   SDR1,r0       /* SDR1 - Address of Page Table */
    sync
    isync

    /* invalidate entries within both Translation Lookaside Buffers */

    bl      tlbInval

    /*
     * Now that the MMU has been configured we can enable the data
     * MMU by turning on data address translation.
     *
     * Synchronization requirements: A DSSALL and sync must precede
     * a mtmsr and then a sync and isync must follow.
     */

    mfmsr   r3
    lis     r4,HI(1<<(31-_PPC_MSR_BIT_DR))     /* MSR[DR] = 1 */
    ori     r4,r4,LO(1<<(31-_PPC_MSR_BIT_DR))
    or      r4,r3,r4
    DSSALL
    sync
    mtmsr   r4
    sync
    isync

    /* Turn on data cache */

    bl      dCacheOn

    /* Turn on Instruction Cache */

    bl      iCacheOn

    /* Initialize some memory in the cache stack */

    lis     r3,HI(CACHE_STACK_SIZE)
    ori     r3,r3,LO(CACHE_STACK_SIZE)
    subf    r3,r3,sp

fillLoop:
    dcbz    r0,r3
    addi    r3,r3,_CACHE_ALIGN_SIZE
    cmplw   r3,sp
    blt     fillLoop

    xor     r0,r0,r0                     /* insure r0 is zero */

    /* Program the MV64260 System Memory Controller */

    bl      sysMv64260SdramInit
    or      r15,r3,r3			/* save memory size */

    /*
     * We have been using DBATs up until now.  We will disable the
     * DBATs by turning off address translation.  First we must disable
     * the data cache.  Once the data MMU is disabled we will enable the 
     * data cache once again.  The instruction cache is still enabled.  Then
     * we will scrub the DRAM before disabling both the data and instruction
     * caches - the caches will be disabled when we leave romInit. 
     */

    bl      dCacheOff     /* Disable the data cache */

    /* Disable the data MMU */

    mfmsr   r3
    rlwinm  r3,r3,0,_PPC_MSR_BIT_DR+1,_PPC_MSR_BIT_DR-1  /* MSR[DR] = 0 */
    DSSALL
    sync
    mtmsr   r3
    sync
    isync

    /*
     * Program the SDRAM Operation Mode register of the MV64260 
     *
     * Once the SDRAM registers are programmed the SDRAM Operation Mode
     * register must be programmed before the SDRAM can be used.  This
     * must occur after the MMU is disabled since this involves "dummy"
     * writes to the SDRAM which must be pushed out to the SDRAM - not
     * to the cache.
     */

    bl	    sdramOperMode

#ifdef INCLUDE_ECC

    /* 
     * Turn the data cache back on for the DRAM scrub 
     *
     * This will reduce the time to scrub DRAM.  
     */

    bl      dCacheOn

    /*
     * Scrub memory so that it is in a known state for error
     * correction/detection.
     */

    xor     r3,r3,r3			/* memory starting address = 0 */
    add     r4,r3,r15			/* memory ending address + 1 */
    bl      memoryScrub			/* scrub system memory */

    bl      dCacheOff

#endif /* INCLUDE_ECC */

goCEntry:

    /* go to C entry point */

    or      r3,r31,r31
    addi    sp,sp,-FRAMEBASESZ      /* get frame stack */

    lis     r6,HI(romStart)
    ori     r6,r6,LO(romStart)

    lis     r7,HI(romInit)
    ori     r7,r7,LO(romInit)

    lis     r8,HI(ROM_TEXT_ADRS)
    ori     r8,r8,LO(ROM_TEXT_ADRS)

    sub     r6,r6,r7
    add     r6,r6,r8 

    mtlr    r6
    blr
FUNC_END(romInit)

/******************************************************************************
*
* tlbInval - Invalidate the Translation Lookaside Buffers
*
* The MPC7455 implements separate 128-entry data and instruction TLBs to
* maximize performance.  Each TLB contains 128 entries organized as a two-way
* set-associative array with 64 sets (64 indexes).  Each tlbie instruction
* invalidates four entries, two for ITLB and two for DTLB.  Since there are
* 64 indexes, we must execute the tlbie instruction 64 times, incrementing
* the index value by one each time, in order to invalidate both TLBs.  
* According to page 2-92 of the MPC7450 RISC Microprocessor Family User's
* Manual, a tlbsync instruction must follow the sequence of 64 tlbie
* instructions.
*
* RETURNS: N/A. 
*/

FUNC_BEGIN(tlbInval)
    li      r4,128
    xor     r3,r3,r3           /* p0 = 0 */
    mtctr   r4                 /* CTR = 64 */

    isync                      /* context sync req'd before tlbie */
tlbLoop:
    tlbie   r3
    sync                       /* sync instr req'd after tlbie */
    addi    r3,r3,0x1000       /* increment bits 14-19 */
    bdnz    tlbLoop            /* decrement CTR, branch if CTR != 0 */
    tlbsync

    bclr    0x14,0x0	       /* return to caller */
FUNC_END(tlbInval)

/******************************************************************************
*
* iCacheOn - Turn Instruction Cache On
*
* This routine enables the instruction cache by setting the Instruction
* Cache Enable (ICE) bit in HID0.  It also flash invalidates the instruction
* cache, via the Instruction Cache Flash Invalidate (ICFI) bit in HID0.
*
* RETURNS: N/A. 
*/

FUNC_BEGIN(_iCacheOn)
FUNC_BEGIN(iCacheOn)
    mfspr   r4, HID0               /* r4 = HID0 */
    ori     r4,r4,(_PPC_HID0_ICE+_PPC_HID0_ICFI) /* Set ICE & ICFI */
    isync
    mtspr   HID0, r4               /* Enable Instr Cache & Inval cache */
    isync                   

    rlwinm  r3,r4,0,(_PPC_HID0_BIT_ICFI+1),(_PPC_HID0_BIT_ICFI-1) /* Clear */
                                                                  /* ICFI */
    mtspr   HID0, r3        
    isync

    bclr    0x14,0x0	           /* return to caller */
FUNC_END(_iCacheOn)
FUNC_END(iCacheOn)

/******************************************************************************
*
* iCacheOff - Turn Instruction Cache Off
*
* This routine disables the instruction cache by unsetting the Instruction
* Cache Enable (ICE) bit in HID0.  It also flash invalidates the instruction
* cache, via the Instruction Cache Flash Invalidate (ICFI) bit in HID0, prior 
* to disabling the instruction cache.
*
* RETURNS: N/A. 
*/

FUNC_BEGIN(_iCacheOff)
FUNC_BEGIN(iCacheOff)
    mfspr   r4,HID0                /* r4 = HID0 */
    ori     r4,r4,(_PPC_HID0_ICE + _PPC_HID0_ICFI) /* Set ICE & ICFI */
    DSSALL                         /* required before changing ICE or ICFI */
    sync                           
    mtspr   HID0,r4                /* Enable inst cache & invalidate cache */
    sync                           /* required after changing ICE or ICFI */
    isync

    rlwinm  r4,r4,0,_PPC_HID0_BIT_ICE+1,_PPC_HID0_BIT_ICE-1    /* clear ICE */
    rlwinm  r3,r4,0,(_PPC_HID0_BIT_ICFI+1),(_PPC_HID0_BIT_ICFI-1) /* clear */
                                                                  /* ICFI */
    DSSALL                         /* required before changing ICE or ICFI */
    sync                      
    mtspr   HID0,r4                /* Disable instruction cache, clear ICFI */
    sync                           /* required after changing ICE or ICFI */
    isync

    bclr    0x14,0x0	           /* return to caller */
FUNC_END(_iCacheOff)
FUNC_END(iCacheOff)

/******************************************************************************
*
* dCacheOn - Turn Data Cache On
*
* This routine enables the data cache by setting the Data Cache Enable (DCE) 
* bit in HID0.  It also flash invalidates the data cache, via the Data Cache 
* Flash Invalidate (DCFI) bit in HID0.
*
* RETURNS: N/A. 
*/

FUNC_BEGIN(_dCacheOn)
FUNC_BEGIN(dCacheOn)
    mfspr   r4,HID0                /* r4 = HID0 */ 
    ori     r4,r4,(_PPC_HID0_DCE + _PPC_HID0_DCFI) /* Set DCE & DCFI */
    DSSALL                         /* required before changing DCE or DCFI */
    sync                           
    mtspr   HID0,r4                /* Enable data cache & invalidate cache */
    sync                           /* required after changing DCE or DCFI */
    isync

    rlwinm  r3,r4,0,(_PPC_HID0_BIT_DCFI+1),(_PPC_HID0_BIT_DCFI-1) /* Clear */
                                                                  /* DCFI */
    DSSALL                         /* required before changing DCE or DCFI */
    sync
    mtspr   HID0, r3               /* Clear data cache invalidate bit */ 
    sync                           /* required after changing DCE or DCFI */
    isync

    bclr    0x14,0x0               /* return to caller */
FUNC_END(_dCacheOn)
FUNC_END(dCacheOn)

/******************************************************************************
*
* dCacheOff - Turn Data Cache Off
*
* This routine disables the data cache by unsetting the Data Cache Enable (DCE)
* bit in HID0.  It also flash invalidates the data cache, via the Data Cache 
* Flash Invalidate (DCFI) bit in HID0, prior to disabling the data cache.
*
* Errata: Error No. 21694 PTE changed (C) bit corruption when data L1
*         cache disabled
* Errata Workaround: Disable the L2 hardware prefetch engine (MSSCR0[L2PFE]=0)
*                    and set L2CR[IONLY] and L3CR[IONLY] when the L1 data
*                    cache is disabled.
*
* RETURNS: N/A. 
*/

FUNC_BEGIN(_dCacheOff)
FUNC_BEGIN(dCacheOff)
    mfspr   r4,HID0                /* r4 = HID0 */
    ori     r4,r4,(_PPC_HID0_DCE + _PPC_HID0_DCFI) /* Set DCE & DCFI */
    DSSALL                         /* required before changing DCE or DCFI */
    sync                           
    mtspr   HID0,r4                /* Enable data cache & invalidate cache */
    sync                           /* required after changing DCE or DCFI */
    isync

    rlwinm  r4,r4,0,_PPC_HID0_BIT_DCE+1,_PPC_HID0_BIT_DCE-1    /* clear DCE */
    rlwinm  r3,r4,0,(_PPC_HID0_BIT_DCFI+1),(_PPC_HID0_BIT_DCFI-1) /* clear */
                                                                  /* DCFI */
    DSSALL                         /* required before changing DCE or DCFI */
    sync                      
    mtspr   HID0,r4                /* Disable data cache, clear DCFI */
    sync                           /* required after changing DCE or DCFI */
    isync

    addis   r5,r0,0xFFFF           /* r5 = mask for MSSCR0[L2PFE] bits */
    ori     r5,r5,0xFFFC
    mfspr   r4,MSSCR0              /* r4 = MSSCR0 */
    and     r4,r4,r5	           /* r4 = MSSCR0 with L2PFE bits cleared */
    DSSALL                         /* required before changing L2PFE bits */
    sync
    mtspr   MSSCR0,r4              /* clear L2PFE bits */
    sync                           /* required after changing L2PFE bits */
    isync 

#ifdef INCLUDE_CACHE_L2
    mfspr   r4,L2CR                /* r4 = L2CR */
    lis     r5,HI(PPC_L2CR_L2IO)   /* r5 = L2IO bit */
    ori     r5,r5,LO(PPC_L2CR_L2IO)
    or      r6,r4,r5               /* r6 = L2CR with the L2IO bit now set */
    mtspr   L2CR,r6                /* Enable inst. only mode in L2 cache */
    isync

    mfspr   r4,L3CR                /* r4 = L3CR */
    lis     r5,HI(PPC_L3CR_L3IO)   /* r5 = L3IO bit */
    ori     r5,r5,LO(PPC_L3CR_L3IO)
    or      r6,r4,r5               /* r6 = L3CR with the L3IO bit now set */
    mtspr   L3CR,r6                /* Enable inst. only mode in L3 cache */
    isync
#endif /* INCLUDE_CACHE_L2 */

    bclr    0x14,0x0               /* return to caller */
FUNC_END(_dCacheOff)
FUNC_END(dCacheOff)

/******************************************************************************
*
* memoryScrub - DRAM initialization.
*
* This routine's purpose is to initialize (i.e., scrub) DRAM, the MV64260 
* protects DRAM by utilizing ECC, so the scrub insures that the entire 
* DRAM array's check bits are initialized to a known state.
*
* ARGUMENTS:
* r3 = starting address of DRAM
* r4 = ending address of DRAM (plus 1)
*
* RETURNS: N/A
*/

FUNC_BEGIN(memoryScrub)
        mfspr   r16,LR                  /* save return instruction pointer */
        or      r13,r3,r3               /* save argument #1, start address */
        or      r14,r4,r4               /* save argument #2, end address */
        xor     r0,r0,r0                /* clear r0 */

        /*
         * Enable floating point support in the processor so that
         * we can scrub memory using 64 bit reads and writes.
         */

        mfmsr   r3
	lis     r4,HI(1<<(31-_PPC_MSR_BIT_FP))
        ori     r4,r4,LO(1<<(31-_PPC_MSR_BIT_FP))
	or      r4,r3,r4
	DSSALL
	sync
        mtmsr   r4
	sync
	isync

        bl      loadScrubData

        .long   0x00000000,0x00000000

loadScrubData:

        mfspr   r4,LR                   /* address of the above table of 0's */
        lfd     0,0(r4)                 /* load the 0's into fpr0 */


        /* Setup scrub loop specifics */

        subf    r18,r13,r14             /* calculate number of bytes */
        rlwinm  r18,r18,29,3,31         /* calculate number of doubles */
        addi    r17,r13,-8              /* starting address munged */
        mtspr   CTR,r18                 /* load number of doubles/words */

        /* Loop through the entire DRAM array, initialize memory */

scrubLoop:

        stfdu   0,8(r17)                /* write contents of fpr0 to memory */
        bc      16,0,scrubLoop          /* branch until counter == 0 */

        sync

    	lis     r18,HI(0x8000/_CACHE_ALIGN_SIZE)    /* load number of cache */
    	ori     r18,r18,(0x8000/_CACHE_ALIGN_SIZE)  /* lines in 32KB */
    	mtspr   CTR,r18               /* move to counter */

readLoop:

        /*
         * Flush L1 data cache to ensure that all data has been flushed 
         * from the cache and has been written to memory.
         */

        dcbf    r0,r17              /* flush line */
        addi    r17,r17,-(_CACHE_ALIGN_SIZE) /* next (previous) line */
        bc      16,0,readLoop           /* branch until counter == 0 */

scrubExit:

	/* Disable floating point support in the processor before exiting */

	mfmsr   r3
	rlwinm  r3,r3,0,_PPC_MSR_BIT_FP+1,_PPC_MSR_BIT_FP-1
	DSSALL
	sync
	mtmsr   r3
	sync
	isync

        mtspr   LR,r16                  /* restore return instruction pointer */
        bclr    0x14,0x0                /* return to caller */
FUNC_END(memoryScrub)

/******************************************************************************
*
* addrMapInit - Program the MV64260 address mapping registers.
*
* This routine enables/disables the appropriate MV64260 address mapping 
* registers.  All registers are 32-bit.
*
* The MV64260 has 22 windows in its CPU interface:
* -> 4 for SDRAM chip selects (SCS[0-3])
* -> 5 for device chip selects (CS[0-3],boot)
* -> 5 for PCI_0 interface
* -> 5 for PCI_1 interface
* -> 1 for internal registers 
* -> 2 for PCI to CPU mapping
*
* This routine will enable/disable the following:
*   Enable CS1 to support the device bus.
*   Disable CS0, CS2, CS3 
*   Disable all of the PCI interfaces.
*   Disable PCI to CPU mapping.
*
* NOTE: According to page 97 of the MV64260 System Controller Manual,
*       the internal registers of the MV64260 are always programmed in
*       Little Endian.  This requires us to perform byte-swapping when
*       reading from and/or writing to the internal registers.
*
*
* RETURNS: N/A.
*/

addrMapInit:

    /* NOTE: We assume r3 points to the MV64260_REG_BASE prior to entry */

    mfspr   r20,LR                      /* save return instruction pointer */
    bl      mv64260AddrMapTablePtr      /* branch around tables */

    /* register data table (initialization values) */

mv64260AddrMapTable:

 /*
  *        register offset            data
  *        ===============            ==== 
  */

/* Disable CS0 - it will later be programmed to point to alternate flash bank */

    .long CPUIF_CS0_LO_DECODE_ADDR,	0x00000fff	/* disable */
    .long CPUIF_CS0_HI_DECODE_ADDR,	0x00000000

/* 
 * CS1 must be enabled for device bank (0xf1d00000 to 0xf1e00000).  Note
 * that this programming is also done during sysMv64260DevBusInit() called
 * during sysHwInit() so that this programming technically need not be
 * done here.  We nevertheless leave it here as a possible future debugging
 * aid since access to the fail LED (a primitive debugging tool) is 
 * not possible without this programming in place.
 */

    .long CPUIF_CS1_LO_DECODE_ADDR, \
       MV64260_LO_DECODE_ADDR(MV64260_DEVCS1_BASE_ADDR)

    .long CPUIF_CS1_HI_DECODE_ADDR, \
       MV64260_HI_DECODE_ADDR(MV64260_DEVCS1_BASE_ADDR,MV64260_DEVCS1_SIZE)

/* Disable the CS2 and CS3 */

    .long CPUIF_CS2_LO_DECODE_ADDR,	0x00000fff
    .long CPUIF_CS2_HI_DECODE_ADDR,	0x00000000
    .long CPUIF_CS3_LO_DECODE_ADDR,	0x00000fff
    .long CPUIF_CS3_HI_DECODE_ADDR,	0x00000000

/*
 * The following address decoding registers can be left in the default state:
 *
 * CPUIF_BOOTCS_LO_DECODE_ADDR
 * CPUIF_BOOTCS_HI_DECODE_ADDR
 */

/* Disable the remaining mapping registers */

    .long CPUIF_PCI0_IO_LO_DECODE_ADDR,		0x00000fff
    .long CPUIF_PCI0_IO_HI_DECODE_ADDR,		0x00000000
    .long CPUIF_PCI0_MEM0_LO_DECODE_ADDR,	0x00000fff
    .long CPUIF_PCI0_MEM0_HI_DECODE_ADDR,	0x00000000
    .long CPUIF_PCI0_MEM1_LO_DECODE_ADDR,	0x00000fff
    .long CPUIF_PCI0_MEM1_HI_DECODE_ADDR,	0x00000000
    .long CPUIF_PCI0_MEM2_LO_DECODE_ADDR,	0x00000fff
    .long CPUIF_PCI0_MEM2_HI_DECODE_ADDR,	0x00000000
    .long CPUIF_PCI0_MEM3_LO_DECODE_ADDR,	0x00000fff
    .long CPUIF_PCI0_MEM3_HI_DECODE_ADDR,	0x00000000
    .long CPUIF_PCI1_IO_LO_DECODE_ADDR,		0x00000fff
    .long CPUIF_PCI1_IO_HI_DECODE_ADDR,		0x00000000
    .long CPUIF_PCI1_MEM0_LO_DECODE_ADDR,	0x00000fff
    .long CPUIF_PCI1_MEM0_HI_DECODE_ADDR,	0x00000000
    .long CPUIF_PCI1_MEM1_LO_DECODE_ADDR,	0x00000fff
    .long CPUIF_PCI1_MEM1_HI_DECODE_ADDR,	0x00000000
    .long CPUIF_PCI1_MEM2_LO_DECODE_ADDR,	0x00000fff
    .long CPUIF_PCI1_MEM2_HI_DECODE_ADDR,	0x00000000
    .long CPUIF_PCI1_MEM3_LO_DECODE_ADDR,	0x00000fff
    .long CPUIF_PCI1_MEM3_HI_DECODE_ADDR,	0x00000000
    .long CPUIF_CPU0_LO_DECODE_ADDR,		0x00000fff
    .long CPUIF_CPU0_HI_DECODE_ADDR,		0x00000000
    .long CPUIF_CPU1_LO_DECODE_ADDR,		0x00000fff
    .long CPUIF_CPU1_HI_DECODE_ADDR,		0x00000000

    .long DEVCTRL_DEV_BANK1_PARAMS,		0x8fcfffff /* Device bus bank */
    .long DEVCTRL_DEV_BOOT_BANK_PARAMS,		0x8fefffff /* Boot bank	*/
    .long DEVCTRL_DEV_IF_CTRL,			0x0007ffff
    .long DEVCTRL_DEV_IF_XBAR_CTRL_LO,		0x11765432
    .long DEVCTRL_DEV_IF_XBAR_CTRL_HI,		0x11765432
    .long DEVCTRL_DEV_IF_XBAR_TMOUT,		0x000100ff
    .long DEVCTRL_DEV_BANK1_PARAMS,		0x8fcfffff /* Device bus bank */
    .long DEVCTRL_DEV_BOOT_BANK_PARAMS,		0x8FEFFFFF /* Boot bank	*/

    .long -1, -1					/* table end marker */

mv64260AddrMapTablePtr:
    mfspr   r21,LR                  /* load pointer to table */

mv64260InitLoop:
    lwz     r4,0(r21)               /* load register offset */
    lwz     r5,4(r21)               /* load data */
    cmpwi   r5,-1                   /* table end? */
    beq     mv64260InitDone         /* if equal, yes, branch */
    stwbrx  r5,r3,r4                /* if not, store data into register */
    lwbrx   r5,r3,r4		    /* read back to push data out */
    addi    r21,r21,8               /* bump to next entry in table */
    b       mv64260InitLoop         /* repeat for next register */

mv64260InitDone:
    mtspr   LR,r20                  /* restore return instruction pointer */
    bclr    20,0                    /* return to caller */

/******************************************************************************
*
* sdramOperMode - SDRAM Operation Mode Register Programming
*
* This is necessary for changing the CAS Latency.
* Programming the SDRAM Operation Mode register requires some extra steps
* according to section "5.11 SDRAM Operation Mode Register" of the
* GT-64260A Manual (rev A):
*
* (1) Write the "Mode Register Command Enable" command to the
*     SDRAM Operation Mode register.
* (2) Read the SDRAM Operation Mode register.
* (3) Dummy word (32-bit) writes to each enabled SDRAM bank.
* (4) Poll the SDRAM Operation Mode register until the activate bit [31]
*     is set.
* (5) Write the "Normal SDRAM Mode" command to the SDRAM Operation Mode
*     register.
* (6) Read the SDRAM Operation Mode register.
*
* RETURNS: N/A
*/

sdramOperMode:

    mfspr   r19,LR                      /* save return instruction pointer */
    bl      dscSmcInit_6
    .long   (MV64260_REG_BASE | CPUIF_SCS0_LO_DECODE_ADDR)
    .long   (MV64260_REG_BASE | CPUIF_SCS1_LO_DECODE_ADDR)
    .long   (MV64260_REG_BASE | CPUIF_SCS2_LO_DECODE_ADDR)
    .long   (MV64260_REG_BASE | CPUIF_SCS0_LO_DECODE_ADDR)
    .long   0                       	/* end-of-table */

dscSmcInit_6:
    mfspr   r4,LR                      	/* base addr of low decoders table */

    subi    r4,r4,4                 	/* munge address for 'lwzu' instr */
    lis     r7,HI(MV64260_REG_BASE | SDRAM_OPER_MODE)
    ori     r7,r7,LO(MV64260_REG_BASE | SDRAM_OPER_MODE)

dscSmcInit_7:
    lwzu    r5,4(r4)                	/* read low decoder addr from table */
    cmplwi  r5,0                    	/* check for end-of-table */
    beq     dscSmcInit_9            	/* exit loop if end-of-table */

    lwbrx   r6,r0,r5                	/* read contents of low decoder */
    eieio
    sync
    cmplwi  r6,0x0FFF               	/* if decoder == 0x00000FFF, not set */
    beq     dscSmcInit_7            	/* branch if decoder not set */
    rlwinm  r6,r6,20,0,11           	/* convert to a RAM address */

    li      r3,0x3                  	/* opcode */
    stwbrx  r3,r0,r7                	/* write opcode to OPMODE register */
    eieio
    sync
    lwbrx   r3,r0,r7                	/* read to ensure write is complete */
    eieio
    sync

    /*
     * doing these writes with the OPMODE register set to
     * 'register command enable' should force an update to
     * the mode register on the SDRAM
     */

    li      r3,0
    stw     r3,0x00(r6)             	/* write anything to SDRAM */
    sync
    stw     r3,0x04(r6)             	/* write anything to SDRAM */
    sync
    stw     r3,0x08(r6)             	/* write anything to SDRAM */
    sync
    stw     r3,0x0C(r6)             	/* write anything to SDRAM */
    sync

    li      r8,0x100                	/* delay loop count */
    mtctr   r8                      	/* load 'ctr' with delay count */

dscSmcInit_8:
    sync
    bdnz    dscSmcInit_8            	/* wait a little */
    lwbrx   r3,r0,r7                	/* read OPMODE register */
    sync
    eieio
    mtctr   r8                      	/* reload 'ctr' */
    andis.  r3,r3,0x8000            	/* check for completion of operation */
    beq     dscSmcInit_8            	/* check again if not done */

    li      r3,0                    	/* encoding for normal SDRAM mode */
    stwbrx  r3,r0,r7                	/* restore to normal SDRAM mode */
    eieio
    sync
    lwbrx   r3,r0,r7                	/* read to ensure write is complete */
    eieio
    sync

    b       dscSmcInit_7            	/* on to next bank */

dscSmcInit_9:
    mtspr   LR,r19                     /* restore link register */
    bclr    20,0                       /* return */
