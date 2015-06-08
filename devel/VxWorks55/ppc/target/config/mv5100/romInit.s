/* romInit.s - Motorola ROM initialization module */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc. All Rights Reserved */
    .data
    .globl  copyright_wind_river
    .long   copyright_wind_river

/*
modification history
--------------------
01i,30jan02,kab  Added MPC7410 (Nitro) support
01h,17oct01,kab  Update FUNC_*, _TEXT_SEG_START;
		 Replaced CPU_TYPE_MAX (confusing codename) w/ CPU_TYPE_7400
		 Cleanup dead code.
01g,09may01,pch  Add .type directives
01f,09mar01,rhk  Additional coding standards changes.
01e,12oct00,cak  Added macros to flash bank write enable for TrueFFS. 
01d,16aug00,dmw  Wind River coding standards review.
01c,31jul00,cak  Added Flash bank write enable for TrueFFS.
01b,09may00,dmw  Applied MAX errata work-around for revision 2.7 and less.
01a,21mar00,dmw  Written (from version 01e of mcpn765/romInit.s
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

This code is intended to be generic across PowerPC 603/604 boards.
Hardware that requires special register setting or memory
mapping to be done immediately, may do so here.
*/

#define  _ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "asm.h"
#include "config.h"
#include "regs.h"    

#include "arch/ppc/mmu603Lib.h"

#define CACHE_STACK_SIZE (8*1024)   /* stack size, 8KB */
#define MEMSSCR1         0x01000000 /* Memory SubSystem Control Reg 1 */

    /* Exported internal functions */

    FUNC_EXPORT(_romInit)              /* start of system code */
    FUNC_EXPORT(romInit)               /* start of system code */

    /* externals */

    FUNC_IMPORT(romStart)               /* system initialization routine */
    FUNC_IMPORT(sysHawkParamConfig)     /* hawk smc parameter initialization */

    /* hawk SMC register save area offsets */

    .set   AttrA,0
    .set   BaseA,AttrA+4
    .set   ClkFreq,BaseA+4
    .set   AttrE,ClkFreq+4
    .set   BaseE,AttrE+4
    .set   Ctrl,BaseE+4
    .set   Size,Ctrl+4
    
    _WRS_TEXT_SEG_START
        
/******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*
* SYNOPSIS
* \ss
* void romInit
*     (
*     int startType    /@ only used by 2nd entry point @/
*     )
* \se
*
* RETURNS: N/A
*/

FUNC_LABEL(_romInit)
FUNC_BEGIN(romInit)
    isync                           /* required for Max errata 2 */

    bl    cold
    
    bl    warm

    /* copyright notice appears at beginning of ROM (in TEXT segment) */

    .ascii   "Copyright 1984-2001 Wind River Systems, Inc."
    .align 2

cold:
    li     r31, BOOT_COLD
    bl     start                    /* skip over next instruction */

            
warm:
    or     r31,r3,r3                /* startType to r31 */

start:
    /* Zero-out registers: r0 & SPRGs */

    xor    r0,r0,r0
    mtspr  272,r0
    mtspr  273,r0
    mtspr  274,r0
    mtspr  275,r0

    /* initialize the stack pointer */

    lis    sp,HI(STACK_ADRS)
    ori    sp,sp,LO(STACK_ADRS)

    /*
     *    Set HID0 to a known state
     *    Enable machine check input pin (EMCP)
     *    Disable instruction and data caches
     */

    addis  r3,r0,0x8000
    ori    r3,r3,0x0000
    sync                            /* required before changing DCE */
    isync                           /* required before changing ICE */
    mtspr  HID0, r3

    /*
     *    Set MPU/MSR to a known state
     *    Turn on FP
     */

    andi.  r3,r3,0
    ori    r3,r3,0x2000
    sync
    mtmsr  r3
    isync

    /* Init the floating point control/status register */

    mtfsfi 7,0x0
    mtfsfi 6,0x0
    mtfsfi 5,0x0
    mtfsfi 4,0x0
    mtfsfi 3,0x0
    mtfsfi 2,0x0
    mtfsfi 1,0x0
    mtfsfi 0,0x0
    isync

    /* Initialize the floating point data regsiters to a known state */

    bl     ifpdr_value
    .long  0x3f800000               /* 1.0 */
ifpdr_value:
    mfspr  r3,8
    lfs    f0,0(r3)
    lfs    f1,0(r3)
    lfs    f2,0(r3)
    lfs    f3,0(r3)
    lfs    f4,0(r3)
    lfs    f5,0(r3)
    lfs    f6,0(r3)
    lfs    f7,0(r3)
    lfs    f8,0(r3)
    lfs    f9,0(r3)
    lfs    f10,0(r3)
    lfs    f11,0(r3)
    lfs    f12,0(r3)
    lfs    f13,0(r3)
    lfs    f14,0(r3)
    lfs    f15,0(r3)
    lfs    f16,0(r3)
    lfs    f17,0(r3)
    lfs    f18,0(r3)
    lfs    f19,0(r3)
    lfs    f20,0(r3)
    lfs    f21,0(r3)
    lfs    f22,0(r3)
    lfs    f23,0(r3)
    lfs    f24,0(r3)
    lfs    f25,0(r3)
    lfs    f26,0(r3)
    lfs    f27,0(r3)
    lfs    f28,0(r3)
    lfs    f29,0(r3)
    lfs    f30,0(r3)
    lfs    f31,0(r3)

    /*
     *     Set MPU/MSR to a known state
     *     Turn off FP
     */

    andi.  r3,r3,0
    sync
    mtmsr  r3
    isync

    /* Init the Segment registers */

    andi.  r3,r3,0
    isync
    mtsr   0,r3
    isync
    mtsr   1,r3
    isync
    mtsr   2,r3
    isync
    mtsr   3,r3
    isync
    mtsr   4,r3
    isync
    mtsr   5,r3
    isync
    mtsr   6,r3
    isync
    mtsr   7,r3
    isync
    mtsr   8,r3
    isync
    mtsr   9,r3
    isync
    mtsr   10,r3
    isync
    mtsr   11,r3
    isync
    mtsr   12,r3
    isync
    mtsr   13,r3
    isync
    mtsr   14,r3
    isync
    mtsr   15,r3
    isync

    /* 
     * Disable Hawk's Watchdog Timers.
     *
     * Note: Both of Hawk's Watchdog timers must be disabled at powerup.
     * Otherwise Watchdog Timer 1 will time out in 8 Sec and interrupt the 
     * board, Watchdog Timer 2 will time out in 16 Sec and reset the board.
     */

    lis    r3,HI(HAWK_PHB_BASE_ADRS)
    ori    r3,r3,LO(HAWK_PHB_BASE_ADRS)

    /* Hawk WDT errata */

    lbz    r5,HAWK_MPC_MARB(r3)     /* save arbiter configuration */
    li     r0,0xa0                  /* load pattern to flatten reads */
    sync
    stb    r0,HAWK_MPC_MARB(r3)     /* configure 60x bus arbiter */
    sync

    /* End Hawk WDT Errata */

    li     r4,0x55                  /* load unlock pattern */
    stb    r4,HAWK_MPC_WDT1CNTL(r3) /* unlock WDT 1 */
    stb    r4,HAWK_MPC_WDT2CNTL(r3) /* unlock WDT 2 */
    sync

    lis    r4,0xaa0f                /* load WDT disable */
    ori    r4,r4,0xffff
    stw    r4,HAWK_MPC_WDT1CNTL(r3) /* disable WDT 1 */
    stw    r4,HAWK_MPC_WDT2CNTL(r3) /* disable WDT 2 */
    sync

    /* Hawk WDT Errata */

    stb    r5,HAWK_MPC_MARB(r3)     /* restore 60x bus arbiter */
    sync

    /* End Hawk WDT Errata */

    /* Clear the RomStartup error flags.  */

    lis    r3,HI(HAWK_PHB_BASE_ADRS)
    ori    r3,r3,LO(HAWK_PHB_BASE_ADRS)
    xor    r4,r4,r4
    stw    r4,0x70(r3)
    stw    r4,0x74(r3)
    stw    r4,0x78(r3)
    stw    r4,0x7c(r3)

    /* Get cpu type */

    mfspr  r28, PVR
    rlwinm r28, r28, 16, 16, 31

    cmpwi  r28,CPU_TYPE_7400
    bne    cpuNotMax27

    /* Get the MAX revision type. If 2.7 or less apply errata fix. */

    mfspr  r29, PVR
    rlwinm r29, r29, 0, 24, 31

    cmpwi  r29,7
    bc     12,5,cpuNotMax27       /* branch if greater than 7 */ 

    /* MAX errata 13 work-around.  */

    li     r2,0x0
    mtspr  1014,r2                 /* MSSCR0 */

    lis    r2,HI(MEMSSCR1)
    ori    r2,r2,LO(MEMSSCR1)      /* MSSCR1: L1OPQ_SIZE=0x01000000 */
    mtspr  1015,r2

cpuNotMax27:

    /* Check for required Nitro errata workaround */
    cmpli   0, 0, r28, CPU_TYPE_7410
    bne     cpuNotNitro

    /* Nitro errata 1 work-around.  */

    li      r2,0x0
    mtspr   1014,r2                 /* MSSCR0 */

    lis     r2,HI(MSSCR_NITRO1)
    ori     r2,r2,LO(MSSCR_NITRO1)      /* MSSCR1: Nitro Errata 1 */
    mtspr   1015,r2

cpuNotNitro:

    /* invalidate the MPU's data/instruction caches */

    cmpli  0, 0, r28, CPU_TYPE_750
    beq    cpuIs750
    cmpli  0, 0, r28, CPU_TYPE_7400
    beq    cpuIs750
    cmpli  0, 0, r28, CPU_TYPE_7410
    beq    cpuIs750
    b      cacheEnableDone

cpuIs750:
#ifdef USER_I_CACHE_ENABLE
    mfspr  r3,HID0
    rlwinm r3,r3,0,19,17    /* clear ILOCK (bit 18) */
    ori    r3,r3,(_PPC_HID0_ICFI | _PPC_HID0_ICE)
    isync                   /* required before changing ICE */
    mtspr  HID0,r3          /* set ICFI (bit 20) and ICE (bit 16) */
#endif /* USER_I_CACHE_ENABLE */
cacheEnableDone:

    /* disable the DBATs */

    xor    r0,r0,r0
    mtspr  DBAT0U,r0
    mtspr  DBAT1U,r0
    mtspr  DBAT2U,r0
    mtspr  DBAT3U,r0
    sync

    bl     setDbats

    /* DBAT contents arranged DBAT0L, DBAT0H, DBAT1L, ... */

    /* first 256 MB of RAM starting at 0 */

    .long  ((0x00000000 & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |\
            _MMU_LBAT_GUARDED)
    .long  ((0x00000000 &_MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_256M |\
            _MMU_UBAT_VS | _MMU_UBAT_VP)

    /* 256 MB of PCI I/O space (including Config space access) */

    .long  ((0x80000000 & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |\
            _MMU_LBAT_CACHE_INHIBIT | _MMU_LBAT_GUARDED)
    .long  ((0x80000000 & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_256M |\
            _MMU_UBAT_VS | _MMU_UBAT_VP)

    /* 256 MB of PCI MEM and MEM I/O space */

    .long  ((0xc0000000 & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |\
            _MMU_LBAT_CACHE_INHIBIT | _MMU_LBAT_GUARDED)
    .long  ((0xc0000000 & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_256M |\
            _MMU_UBAT_VS | _MMU_UBAT_VP)

    /* 256 MB of Hawk register and FLASH ROM space */

    .long  ((0xf0000000 & _MMU_LBAT_BRPN_MASK) | _MMU_LBAT_PP_RW |\
            _MMU_LBAT_CACHE_INHIBIT | _MMU_LBAT_GUARDED)
    .long  ((0xf0000000 & _MMU_UBAT_BEPI_MASK) | _MMU_UBAT_BL_256M |\
            _MMU_UBAT_VS | _MMU_UBAT_VP)

    /* load DBATs from table above */

setDbats:
    mfspr  r4,LR
    addi   r4,r4,-4
    lwzu   r3,4(r4)
    mtspr  DBAT0L,r3
    lwzu   r3,4(r4)
    mtspr  DBAT0U,r3
    lwzu   r3,4(r4)
    mtspr  DBAT1L,r3
    lwzu   r3,4(r4)
    mtspr  DBAT1U,r3
    lwzu   r3,4(r4)
    mtspr  DBAT2L,r3
    lwzu   r3,4(r4)
    mtspr  DBAT2U,r3
    lwzu   r3,4(r4)
    mtspr  DBAT3L,r3
    lwzu   r3,4(r4)
    mtspr  DBAT3U,r3
    sync

    bl     dCacheInval    /* invalidate the data cache contents */

    xor    r0,r0,r0
    sync
    mtspr  SDR1,r0
    sync

    /* invalidate entries within both TLBs */

    li     r4,128
    xor    r3,r3,r3                /* p0 = 0    */
    mtctr  r4                      /* CTR = 64  */
    
    isync                          /* context sync req'd before tlbie */
tlbLoop:
    tlbie  r3
    addi   r3,r3,0x1000            /* increment bits 14-19 */
    bdnz   tlbLoop                 /* decrement CTR, branch if CTR != 0 */
    sync                           /* sync instr req'd after tlbie      */

    /* Enable the data MMU */

    mfmsr  r3
    lis    r4,HI(1<<(31-_PPC_MSR_BIT_DR))
    ori    r4,r4,LO(1<<(31-_PPC_MSR_BIT_DR))
    or     r4,r3,r4
    sync
    mtmsr  r4
    sync

    bl     dCacheOn                /* enable the data cache */

    lis    sp,HI(0x10000000)     /* initialize the stack pointer */
    ori    sp,sp,LO(0x10000000)

    lis    r3,HI(CACHE_STACK_SIZE)
    ori    r3,r3,LO(CACHE_STACK_SIZE)
    subf   r3,r3,sp

fillLoop:
    dcbz   r0,r3
    addi   r3,r3,_CACHE_ALIGN_SIZE
    cmplw  r3,sp
    blt    fillLoop

    /* Hawk System Memory Controller Initialization */

hawkSmcInit:
    xor    r0,r0,r0            /* insure r0 is zero */

    lis    r13,0xFEF8          /* load SMC register address */
    ori    r13,r13,0x0000      /* load SMC register address */

    addis  r3,r0,0x0000        /* TBEN=0, IHEN=0 */
    ori    r3,r3,0x0000        /* TBEN=0, IHEN=0 */
    stw    r3,0x08(r13)        /* General Control register */

    addis  r3,r0,0x0100        /* RDIS=0, RWCB=0, DERC=1, xPIEN=0 */
    ori    r3,r3,0x0000        /* MBE_ME=0 */
    stw    r3,0x28(r13)        /* SDRAM ECC Control register */

    addis  r3,r0,0x8000        /* ELOG=1, ESEN=0, SCOF=1 */
    ori    r3,r3,0x0100        /* ELOG=1, ESEN=0, SCOF=1 */
    stw    r3,0x30(r13)        /* Error Logger register */

    addis  r3,r0,0x0000        /* SWEN=0, SCRUB_FREQUENCY=0x00 */
    ori    r3,r3,0x0000        /* SWEN=0, SCRUB_FREQUENCY=0x00 */
    stw    r3,0x40(r13)        /* Scrub/Refresh register */

#ifdef INCLUDE_TFFS
    lis    r3,HI(HAWK_SMC_ROMA_VALUE)     /* Enable and Conf Bank A */
    ori    r3,r3,LO(HAWK_SMC_ROMA_VALUE)  /* Enable and Conf Bank A */
    stw    r3,HAWK_SMC_ROMA_BS_REG(r13)   /* ROM A Base/Size Register */
#endif /* INCLUDE_TFFS */
    eieio
    sync

    /*
     * Software must wait for a refresh to occur prior to setting the
     * SDRAM Control Registers. To ensure that at least one refresh has
     * completed allow the 32 bit counter to increment at least 100 times.
     */

    addis  r3,r0,0x0000        /* load counter reset value */
    ori    r3,r3,0x0000        /* load counter reset value */
    stw    r3,0x100(r13)       /* reset 32-Bit counter */
    sync
smc_loop1:
    lwz    r3,0x100(r13)       /* Load the counter value */
    cmpwi  r3,100              /* Check counter value less than 100 */
    blt    smc_loop1           /* loop until refresh complete */

    li     r3,0x00             /* disable all SDRAM banks */
    stw    r3,0x0010(r13)
    stw    r3,0x00c0(r13)

#ifdef BYPASS_SPD              /* we'll need some memory later */
    li     r3,66               /* assume clock frequency is 66 MHz */
    stb    r3,0x0020(r13)      /* to ensure an adequate refresh rate */

    lis    r3,0x1333           /* set SDRAM timing back to default */
    ori    r3,r3,0x1100
    stw    r3,0x00d0(r13)

    li     r3,0x00             /* clear all base addresses */
    stw    r3,0x0018(r13)
    stw    r3,0x00c8(r13)

    li     r3,0x81             /* enable SDRAM bank A, size=4Mx16 (32MB) */
    stb    r3,0x0010(r13)

    eieio                      /* force all writes to complete */
    sync

    /*
     * Software must wait for a refresh to occur after setting the
     * SDRAM Control Registers. To ensure that at least one refresh has
     * completed allow the 32 bit counter to increment at least 100 times.
     */

    addis  r3,r0,0x0000        /* load counter reset value */
    ori    r3,r3,0x0000        /* load counter reset value */
    stw    r3,0x100(r13)       /* reset 32-Bit counter */
    sync
smc_loop2:
    lwz    r3,0x100(r13)       /* Load the counter value */
    cmpwi  r3,100              /* Check counter value less than 100 */
    blt    smc_loop2           /* loop until refresh complete */
#endif  /* BYPASS_SPD */

    stwu   sp,-40(sp)          /* create an ABI stack frame in data cache */
    addi   r3,sp,8             /* point to register image area */
    or     r14,r3,r3           /* save register image address */
    bl     sysHawkParamConfig  /* calculate hawk smc parameters */
    or     r15,r3,r3           /* save memory size */

    lbz    r4,ClkFreq(r14)     /* load the Clock Frequency */
    lwz    r5,Ctrl(r14)        /* load the Sdram Speed Attribute */
    lwz    r6,BaseA(r14)       /* load Base Data (Blocks A/B/C/D) */
    lwz    r7,BaseE(r14)       /* load Base Data (Blocks E/F/G/H) */
    lwz    r8,AttrA(r14)       /* load Attributes (Blocks A/B/C/D) */
    lwz    r9,AttrE(r14)       /* load Attributes (Blocks E/F/G/H) */
    addi   sp,sp,40            /* remove ABI stack frame */

    /*
     * Software must wait for a refresh to occur prior to setting the
     * SDRAM Control Registers. To ensure that at least one refresh has
     * completed allow the 32 bit counter to increment at least 100 times.
     */

    addis  r3,r0,0x0000        /* load counter reset value */
    ori    r3,r3,0x0000        /* load counter reset value */
    stw    r3,0x100(r13)       /* reset 32-Bit counter */
    sync
smc_loop3:
    lwz    r3,0x100(r13)       /* Load the counter value */
    cmpwi  r3,100              /* Check counter value less than 100 */
    blt    smc_loop3           /* loop until refresh complete */

    addis  r3,r0,0x0000        /* clear the ram enable bits prior to */
    ori    r3,r3,0x0000        /* intializing sdram attributes */
    stw    r3,0x10(r13)        /* SDRAM Attr Register (Blocks A/B/C/D) */
    stw    r3,0xc0(r13)        /* SDRAM Attr Register (Blocks E/F/G/H) */
    eieio
    sync

    /*
     * Software must wait for a refresh to occur after setting the
     * SDRAM Control Registers. To ensure that at least one refresh has
     * completed allow the 32 bit counter to increment at least 100 times.
     */

    addis  r3,r0,0x0000        /* load counter reset value */
    ori    r3,r3,0x0000        /* load counter reset value */
    stw    r3,0x100(r13)       /* reset 32-Bit counter */
    sync
smc_loop4:
    lwz    r3,0x100(r13)       /* Load the counter value */
    cmpwi  r3,100              /* Check counter value less than 100 */
    blt    smc_loop4           /* loop until refresh complete */

    /*
     * Set the Clock Frequency register and the SDRAM attributes register
     * to the operating values.
     */

    stb    r4,0x20(r13)        /* Clock Frequency register */
    stw    r5,0xd0(r13)        /* SDRAM Speed Attribute Register */
    stw    r6,0x18(r13)        /* SDRAM Base Register (Blocks A/B/C/D) */
    stw    r7,0xc8(r13)        /* SDRAM Base Register (Blocks E/F/G/H) */
    stw    r8,0x10(r13)        /* SDRAM Attr Register (Blocks A/B/C/D) */
    stw    r9,0xc0(r13)        /* SDRAM Attr Register (Blocks E/F/G/H) */
    eieio
    sync

    /*
     * Software must wait for a refresh to occur after setting the
     * SDRAM Control Registers. To ensure that at least one refresh has
     * completed allow the 32 bit counter to increment at least 100 times.
     */

    addis  r3,r0,0x0000        /* load counter reset value */
    ori    r3,r3,0x0000        /* load counter reset value */
    stw    r3,0x100(r13)       /* reset 32-Bit counter */
    sync
smc_loop5:
    lwz    r3,0x100(r13)       /* Load the counter value */
    cmpwi  r3,100              /* Check counter value less than 100 */
    blt    smc_loop5           /* loop until refresh complete */

    bl     dCacheInval
    bl     dCacheOff

    /* Disable the data MMU */

    mfmsr  r3
    rlwinm r3,r3,0,_PPC_MSR_BIT_DR+1,_PPC_MSR_BIT_DR-1
    sync
    mtmsr  r3
    sync

    /* initialize the stack pointer */

    lis    sp,HI(STACK_ADRS)
    ori    sp,sp,LO(STACK_ADRS)


    or     r3,r15,r15          /* restore memory size */

    xor    r0,r0,r0            /* insure r0 is zero */

#ifdef INCLUDE_ECC
    /*
     *  The purpose of this section is to enable the ECC of the
     *  DRAM.  To do this, it is first necessary to initialize the
     *  ECC check bits.  The ECC check bits are initialized by
     *  initializing all of DRAM.
     *
     *  input:
     *      r3      = Total DRAM size (in bytes)
     *  notes:
     *      1. must run as a supervisor function
     *      2. interrupts must be masked
     *      3. address translation (MMU) disabled
     *      4. assumes ECC Control register is in a power-up state
     *      5. The scrubber is not enabled (SWEN=0).  This provides
     *      better performance at the expense of less fault
     *      tolerance.
     */

.hawk_smc_scrub:

    /* setup local variables */

    addi   r15,r0,0            /* load starting addr - always zero */
    or     r16,r3,r3           /* load number of bytes             */
    rlwinm r16,r16,29,3,31     /* calculate number of doubles      */

    /* Make sure FPU is enabled; it's needed for DRAM loads and stores */

    mfmsr  r14                 /* Save current value of MSR in r14 */
    addi   r4,r0,0x2000        /* FP bit definition */
    or     r4,r4,r14
    mtmsr  r4
    isync

    /*
     * invalidate/enable the processor data cache, one of the assumptions
     * is that address translation is disabled, the default caching mode
     * is copy-back when address translation is disabled, copy-back mode
     * is the recommended caching mode
     */
    
    bl     dCacheInval
    bl     dCacheOn

    /*
     * Loop through the entire DRAM array and initialize memory.  This has
     * the side effect of initializing the ECC check bits because they are
     * always updated when the DRAM is written.
     *
     * The data path to DRAM is the size of a cache line (128-bits), this
     * is why the data cache is enabled, the initialization of the ECC check
     * bits are optimized when a cache line write/flush occurs
     */
    
hawk_smc_scrub_start:
    addi   r17,r15,-8          /* starting address - munged */
    mtspr  9,r16               /* load number of doubles in counter */
hawk_smc_scrub_iloop:
    lfdu   0,8(r17)
    stfd   0,0(r17)
    bc     16,0,hawk_smc_scrub_iloop /* branch till counter == 0 */

    eieio
    sync

    /*
     * Flush L1 data cache so that the last segment (data cache size) of 
     * DRAM is initialized.  This is done by flushing 2 x the data cache 
     * size (32K).
     */

    lis    r16,HI(0x10000/_CACHE_ALIGN_SIZE)     /* load number of cache */
    ori    r16,r16,(0x10000/_CACHE_ALIGN_SIZE)   /* lines in 64KB        */

    mtspr  9,r16               /* move to counter */
hawk_smc_scrub_floop:
    dcbf   r0,r17              /* flush line */
    addi   r17,r17,-(_CACHE_ALIGN_SIZE) /* next (previous) line */
    bc     16,0,hawk_smc_scrub_floop    /* branch till counter == 0 */

    eieio
    sync

    /* disable the data cache */

    bl     dCacheInval
    bl     dCacheOff

    /* Restore original value of MSR */

    mtmsr  r14
    isync

    /* Clear any possible error conditions that occurred during init */

    lis    r14,HI(HAWK_SMC_BASE_ADRS)
    ori    r14,r14,LO(HAWK_SMC_BASE_ADRS)

    addis  r9,r0,0x8000        /* ELOG=1,ESEN=0,SCOF=1 */
    ori    r9,r9,0x0100
    stw    r9,0x30(r14)        /* Update Error Logger register */
    eieio
    sync

    /* Enable ECC and multiple-bit error detection */

    addis  r9,r0,0x0000        /* RWCB=0,DERC=0,xIEN=0,MCKEN=1 */
    ori    r9,r9,0x0001
    stw    r9,0x28(r14)        /* Update ECC Control register */
    eieio
    sync

#endif /* INCLUDE_ECC */

#ifdef EXTENDED_VME
    /*
     * Verify that the end of DRAM does not overlap VME_A32_MSTR_LOCAL.
     * If it does, set MSADD0 to the size of DRAM.  A message will be displayed
     * in sysPhysMemTop prompting the user to increase VME_A32_MSTR_LOCAL.
     * 
     * The size of DRAM is saved in r18.
     */

    lis    r3,HI(HAWK_PHB_BASE_ADRS)
    ori    r3,r3,LO(HAWK_PHB_BASE_ADRS)

    lis    r5,HI(VME_A32_MSTR_LOCAL)
    ori    r5,r5,LO(VME_A32_MSTR_LOCAL)

    cmpl   0,0,r18,r5          /* r18 < VME_A32_MSTR_LOCAL */
    bc     12,0,memMapGood     /* Set MSADD0 to DRAM size */

    ori    r18,r18,CPU2PCI_ADDR0_END /* MSADD0: */
    stw    r18,HAWK_MPC_MSADD0(r3)   /* write MSADD0 w DRAM size */

    b      doneMSAddr0Calc

memMapGood:
    addis  r4,r0,(VME_A32_MSTR_LOCAL>>16) /* MSADD0: */
    ori    r4,r4,CPU2PCI_ADDR0_END        /* MSADD0: */
    stw    r4,HAWK_MPC_MSADD0(r3)         /* write MSADD0 */
    
doneMSAddr0Calc:
    eieio                      /* synchronize */
    sync                       /* synchronize */
#endif /* EXTENDED_VME */

#if FALSE                   /* XXX TPR SDA not supported yet */

    /* initialize r2 and r13 according to EABI standard */

    lis    r2,HI(_SDA2_BASE_)
    ori    r2,r2,LO(_SDA2_BASE_)
    lis    r13,HI(_SDA_BASE_)
    ori    r13,r13,LO(_SDA_BASE_)
#endif /*  FALSE */

    /* go to C entry point */

    or     r3,r31,r31
    addi   sp,sp,-FRAMEBASESZ  /* get frame stack */

    lis    r6,HI(romStart)
    ori    r6,r6,LO(romStart)

    lis    r7,HI(romInit)
    ori    r7,r7,LO(romInit)

    lis    r8,HI(ROM_TEXT_ADRS)
    ori    r8,r8,LO(ROM_TEXT_ADRS)

    sub    r6,r6,r7
    add    r6,r6,r8 

    mtlr   r6
    blr
FUNC_END(romInit)

/******************************************************************************
*
* dCacheOn - Turn Data Cache On
*
* void dCacheOn (void)
*
* NOMANUAL
*/

FUNC_LABEL(_dCacheOn)
FUNC_BEGIN(dCacheOn)
    /* Get cpu type */

    mfspr  r3,PVR
    rlwinm r3,r3,16,16,31

    cmpli  0,0,r3,CPU_TYPE_750
    bc     12,2,ccdataon
    cmpli  0,0,r3,CPU_TYPE_7400
    bc     12,2,ccdataon
    cmpli  0,0,r3,CPU_TYPE_7410
    bc     12,2,ccdataon
    bclr   0x14,0x0            /* invalid cpu type */

ccdataon:
    mfspr  r4,HID0             /* Modify HID0 to enable D cache (DCE) */
    ori    r4,r4,_PPC_HID0_DCE
    sync                       /* required before changing DCE */
    mtspr  HID0,r4
    bclr   0x14,0x0            /* return to caller */
FUNC_END(dCacheOn)

/******************************************************************************
*
* dCacheOff - Turn Data Cache Off
*
* void dCacheOff (void)
*
* NOMANUAL
*/

FUNC_LABEL(_dCacheOff)
FUNC_BEGIN(dCacheOff)
    /* Get cpu type */

    mfspr  r3,PVR
    rlwinm r3,r3,16,16,31

    cmpli  0,0,r3,CPU_TYPE_750
    bc     12,2,ccDataOff
    cmpli  0,0,r3,CPU_TYPE_7400
    bc     12,2,ccDataOff
    cmpli  0,0,r3,CPU_TYPE_7410
    bc     12,2,ccDataOff
    bclr   0x14,0x0            /* invalid cpu type */

ccDataOff:
    mfspr  r4,HID0             /* Modify HID0 to disable D cache (DCE) */
    rlwinm r4,r4,0,_PPC_HID0_BIT_DCE+1,_PPC_HID0_BIT_DCE-1    /* clear DCE */
    sync                       /* required before changing DCE */
    mtspr  HID0,r4
    bclr   0x14,0x0            /* return to caller */
FUNC_END(dCacheOff)

/******************************************************************************
*
* dCacheInval - Invalidate Data Cache
*
* void dCacheInval (void)
*
* NOMANUAL
*/

FUNC_LABEL(_dCacheInval)
FUNC_BEGIN(dCacheInval)
    /* Get cpu type */

    mfspr  r3,PVR
    rlwinm r3,r3,16,16,31

    cmpli  0,0,r3,CPU_TYPE_750
    bc     12,2,ccDataInvl
    cmpli  0,0,r3,CPU_TYPE_7400
    bc     12,2,ccDataInvl
    cmpli  0,0,r3,CPU_TYPE_7410
    bc     12,2,ccDataInvl
    bclr   0x14,0x0            /* invalid cpu type */

    /*
     * To invalidate the Data Cache on a 750, it's necessary
     * to set the DCFI bit while the Data Cache is enabled (DCE).
     */
    
ccDataInvl:
    addis  r3,r0,0x0000        /* Setup bit pattern for DCFI + DCE */
    ori    r3,r3,(_PPC_HID0_DCE | _PPC_HID0_DCFI)

    mfspr  r4,HID0             /* Modify HID0 to SET DCFI + DCE bits */
    or     r4,r4,r3
    sync                       /* required before changing DCE */
    mtspr  HID0,r4

    andc   r4,r4,r3            /* Modify HID0 to CLEAR DCFI + DCE bits */
    sync                       /* required before changing DCE */
    mtspr  HID0,r4
    bclr   0x14,0x0            /* return to caller */
FUNC_END(dCacheInval)
