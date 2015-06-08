/* romInit.s - Motorola PrPMC800 ROM initialization module */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc. All Rights Reserved */
    .data
    .globl  copyright_wind_river
    .long   copyright_wind_river

/*
modification history
--------------------
01l,11mar02,dtr  Adding option for disabling exceptions for bus parity errors.
01k,10oct01,scb  Clear PCI_AUTOCONFIG_FLAG.
01j,09jan01,dmw  Modified ECC code to set/clear ECC enable bit.
01i,05jan01,krp  Fixed compiler warning out of range value
01h,21dec00,krp  Enable COM2 port on the PrPMC800XT board
01g,14nov00,dmw  Cleaned up hardcodes.
01f,27oct00,dmw  Added code to load the Harrier registers base during mem init.
01e,16oct00,dmw  Removed PCI bridge initialization.  Now done in harrierPhb.c.
01d,12oct00,dmw  Moved PCI I/O to 0xFD000000.
01c,09oct00,dmw  Fixed outbound translation register.
01b,08oct00,dmw  Added Nitro support.
01a,09aug00,dmw  Written (from version 01g of mcpn765/romInit.s
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

#define    _ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "asm.h"
#include "config.h"
#include "regs.h"    
#include "harrier.h"    

#include "arch/ppc/mmu603Lib.h"

#define CACHE_STACK_SIZE    (8*1024)
#define MEMSSCR1        0x00040000  /* Memory SubSystem Control Reg 1 */
                                    /* bit 13 for Nitro errata 1 */

    /* Exported internal functions */

    FUNC_EXPORT(_romInit)       /* start of system code */
    FUNC_EXPORT(romInit)        /* start of system code */

    /* externals */

    /* system initialization routine */

    FUNC_IMPORT(romStart)         

    /* harrier memory parameter initialization */

    FUNC_IMPORT(sysHarrierParamConfig)    

    /* harrier memory register save area offsets */

    .set sdramBlkAddrA,0                  /* Base Addressing Block A */
    .set sdramBlkAddrB,sdramBlkAddrA+4    /* Base Addressing Block B */
    .set sdramBlkAddrC,sdramBlkAddrB+4    /* Base Addressing Block C */
    .set sdramBlkAddrD,sdramBlkAddrC+4    /* Base Addressing Block D */
    .set sdramBlkAddrE,sdramBlkAddrD+4    /* Base Addressing Block E */
    .set sdramBlkAddrF,sdramBlkAddrE+4    /* Base Addressing Block F */
    .set sdramBlkAddrG,sdramBlkAddrF+4    /* Base Addressing Block G */
    .set sdramBlkAddrH,sdramBlkAddrG+4    /* Base Addressing Block H */
    .set sdramGeneralControl,sdramBlkAddrH+4     /* SDRAM Gen Cntrl settings */
    .set sdramTimingControl,sdramGeneralControl+4 /* Timing Cntrl settings */
    .set clkFrequency,sdramTimingControl+4        /* Operating clck freq */
    
    _WRS_TEXT_SEG_START

        
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
    isync            /* required for Max errata 2 */

    bl    cold
    
    bl    warm

    /* copyright notice appears at beginning of ROM (in TEXT segment) */

    .ascii   "Copyright 1984-1999 Wind River Systems, Inc."
    .balign _PPC_TEXT_SEG_ALIGN

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

    /* initialize the stack pointer */

    lis     sp, HI(STACK_ADRS)
    ori     sp, sp, LO(STACK_ADRS)

    /*
     *    Set HID0 to a known state
     *    Enable machine check input pin (EMCP)
     *    Disable instruction and data caches
     */

    addis   r3,r0,0x8000
    ori     r3,r3,0x0000
    sync             /* required before changing DCE */
    isync            /* required before chaning ICE */
    mtspr   HID0, r3

    /*
     *    Set MPU/MSR to a known state
     *    Turn on FP
     */

    andi.  r3, r3, 0
    ori    r3, r3, 0x2000
    sync
    mtmsr  r3
    isync

    /* Init the floating point control/status register */

    mtfsfi  7,0x0
    mtfsfi  6,0x0
    mtfsfi  5,0x0
    mtfsfi  4,0x0
    mtfsfi  3,0x0
    mtfsfi  2,0x0
    mtfsfi  1,0x0
    mtfsfi  0,0x0
    isync

    /* Initialize the floating point data regsiters to a known state */

    bl    ifpdr_value
    .long    0x3f800000    /* 1.0 */
ifpdr_value:
    mfspr    r3,8
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
     *    Set MPU/MSR to a known state
     *    Turn off FP
     */

    andi.    r3, r3, 0
    sync
    mtmsr    r3
    isync

    /* Init the Segment registers */

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
     * Disable Harrier's Watchdog Timers.
     *
     * Note: Both of Harrier's Watchdog timers must be disabled at powerup.
     * Otherwise Watchdog Timer 1 will time out in 512 msec and interrupt the 
     * board, Watchdog Timer 2 will time out in 576 msec and reset the board.
     */

    lis     r3,HI(HARRIER_XCSR_BASE)
    ori     r3,r3,LO(HARRIER_XCSR_BASE)

    addis   r4,r0,0x0000            /* disable Watchdog Timers */
    ori     r4,r4,0x0055            /* load PATTERN_1 */
    isync                           /* synchronize */

    /* arm Watchdog Timer 1 (WDT1CNTL) */

    stb     r4,HARRIER_WTCHDG_CNTR0_OFFSET(r3)

    eieio                           /* synchronize */
    sync                            /* synchronize */

    addis   r4,r0,0xAA0F            /* load PATTERN_2 */
    ori     r4,r4,0xFFFF
    isync                           /* synchronize */

    /* disable Timer 1 w max resolution */

    stw     r4,HARRIER_WTCHDG_CNTR0_OFFSET(r3)

    eieio                           /* synchronize */
    sync                            /* synchronize */

    addis   r4,r0,0x0000            /* load PATTERN_1 */
    ori r4,r4,0x0055
    isync                           /* synchronize */

    /* arm Watchdog Timer 2 (WDT2CNTL) */

    stb r4,HARRIER_WTCHDG_CNTR1_OFFSET(r3)

    eieio                           /* synchronize */
    sync                            /* synchronize */

    addis   r4,r0,0xAA0F            /* load PATTERN_2 */
    ori r4,r4,0xFFFF
    isync                           /* synchronize */

    /* disable Timer2 w max resolution */

    stw r4,HARRIER_WTCHDG_CNTR1_OFFSET(r3)
    eieio                           /* synchronize */
    sync                            /* synchronize */

    /* disable PCI bridge configuration cycles */

    addis   r4,r0,HI(HARRIER_BPCS_XCSR_CSH)
    ori r4,r4,LO(HARRIER_BPCS_XCSR_CSH)
    isync                           /* synchronize */

    /* set configuration space hold off */

    stw r4,HARRIER_PCI_CS_REG_OFFSET(r3)
    eieio                           /* synchronize */
    sync                            /* synchronize */
   
    /* Clear the RomStartup error flags.  */

    xor    r4,r4,r4
    stw    r4,HARRIER_GP0_OFFSET(r3)
    eieio                           /* synchronize */
    sync                            /* synchronize */
    stw    r4,HARRIER_GP1_OFFSET(r3)
    eieio                           /* synchronize */
    sync                            /* synchronize */
    stw    r4,HARRIER_GP2_OFFSET(r3)
    eieio                           /* synchronize */
    sync                            /* synchronize */
    stw    r4,HARRIER_GP3_OFFSET(r3)
    eieio                           /* synchronize */
    sync                            /* synchronize */

    /* Get cpu type */

    mfspr   r28, PVR
    rlwinm  r28, r28, 16, 16, 31

    cmpli   0, 0, r28, CPU_TYPE_NITRO
    bne     cpuNotNitro

    /* Nitro errata 1 work-around.  */

    li      r2,0x0
    mtspr   1014,r2                 /* MSSCR0 */

    lis     r2,HI(MEMSSCR1)
    ori     r2,r2,LO(MEMSSCR1)      /* MSSCR1: Nitro Errata 1 */
    mtspr   1015,r2

cpuNotNitro:
    /* invalidate the MPU's data/instruction caches */

    cmpli   0, 0, r28, CPU_TYPE_750
    beq     cpuIs750
    cmpli   0, 0, r28, CPU_TYPE_NITRO
    beq     cpuIs750
    cmpli   0, 0, r28, CPU_TYPE_MAX
    beq     cpuIs750
    b    cacheEnableDone

cpuIs750:
#ifdef USER_I_CACHE_ENABLE
    mfspr   r3,HID0
    rlwinm  r3,r3,0,19,17         /* clear ILOCK (bit 18) */
    ori     r3,r3,(_PPC_HID0_ICFI | _PPC_HID0_ICE)
    isync                         /* required before changing ICE */
    mtspr   HID0,r3               /* set ICFI (bit 20) and ICE (bit 16) */
#endif
cacheEnableDone:

    /* disable the DBATs */

    xor     r0,r0,r0
    mtspr   DBAT0U,r0
    mtspr   DBAT1U,r0
    mtspr   DBAT2U,r0
    mtspr   DBAT3U,r0
    sync

    xor    r0,r0,r0        /* insure r0 is zero */

    lis     r3,HI(HARRIER_XCSR_BASE)    /* load Harrier's base address */
    ori     r3,r3,LO(HARRIER_XCSR_BASE) /* load Harrier's base address */

    /*
     * Initialize the PPC Clock Frequency Register(XCFR) to 100MHz
     * This is used as the prescale counter for the memory refresher/scrubber.
     */

    addis   r9,r0,0x6400            /* CLK=0x64(100MHZ) */
    ori     r9,r9,0x0000            /* CLK=0x64(100MHZ) */

    /* XCFR: PPC CLK frequency register */

    stw     r9,H_PPCCLKFRQ_OFST(r3)
    eieio
    sync

    lis     r4,0x1200           /* set SDRAM timing to default */
    ori     r4,r4,0x0000        /* MXRR=01,DREF=0,DERC=1,RWCB=0,ENRV=0,SWVT=0 */
    stw     r4,0x0100(r3)
    eieio
    sync

    lis     r4,0x0005           /* enable SDRAM bank A, size=16Mx8 (128MB) */
    ori     r4,r4,0100
    stw     r4,0x0110(r3)
    eieio
    sync

    addi    r4,r0,0x01
    stb     r4,0x112(r3)       /* write DRAM enable bit(disable) */
    eieio
    sync

    /* create an ABI stack frame in data cache */

    stwu    sp,-40(sp)      

    addi    r3,sp,8         /* point to register image area (r3) */
    or      r14,r3,r3       /* save register image address */
    bl      sysHarrierParamConfig  /* calculate hawk smc parameters */
    or      r15,r3,r3       /* save memory size */

    lbz     r4,clkFrequency(r14)          /* load the Clock Frequency */
    lwz     r5,sdramTimingControl(r14)    /* load the Sdram timing control */
    lwz     r6,sdramGeneralControl(r14)   /* load the general control */
    lwz     r7,sdramBlkAddrH(r14)         /* load Base Data Block H */
    lwz     r8,sdramBlkAddrG(r14)         /* load Base Data Block G */
    lwz     r9,sdramBlkAddrF(r14)         /* load Base Data Block F */
    lwz     r10,sdramBlkAddrE(r14)        /* load Base Data Block E */
    lwz     r11,sdramBlkAddrD(r14)        /* load Base Data Block D */
    lwz     r12,sdramBlkAddrC(r14)        /* load Base Data Block C */
    lwz     r13,sdramBlkAddrB(r14)        /* load Base Data Block B */
    lwz     r14,sdramBlkAddrA(r14)        /* load Base Data Block A */

    /* remove ABI stack frame */

    addi    sp,sp,40        

    /*
     * Set the Clock Frequency register and the SDRAM attributes register
     * to the operating values.
     */

    lis     r3,HI(HARRIER_XCSR_BASE)
    ori     r3,r3,LO(HARRIER_XCSR_BASE)

    stb     r4,0x14(r3)      /* Clock Frequency register */
    stw     r5,0x104(r3)     /* SDRAM Timing Attribute Register */
    stw     r6,0x100(r3)     /* SDRAM General Control */
    stw     r7,0x12C(r3)     /* SDRAM Blk AddrH */
    stw     r8,0x128(r3)     /* SDRAM Blk AddrG */
    stw     r9,0x124(r3)     /* SDRAM Blk AddrF */
    stw     r10,0x120(r3)    /* SDRAM Blk AddrE */
    stw     r11,0x11C(r3)    /* SDRAM Blk AddrD */
    stw     r12,0x118(r3)    /* SDRAM Blk AddrC */
    stw     r13,0x114(r3)    /* SDRAM Blk AddrB */
    stw     r14,0x110(r3)    /* SDRAM Blk AddrA */
    eieio
    sync

    bl      dCacheInval
    bl      dCacheOff

    /* Disable the data MMU */

    mfmsr   r3
    rlwinm  r3,r3,0,_PPC_MSR_BIT_DR+1,_PPC_MSR_BIT_DR-1
    sync
    mtmsr   r3
    sync

    or      r3,r15,r15      /* restore memory size */

    xor     r0,r0,r0       /* insure r0 is zero */

/*
 *              The purpose of this section is to enable the ECC of the
 *              DRAM.  To do this, it is first necessary to initialize the
 *              ECC check bits.  The ECC check bits are initialized by
 *              initializing all of DRAM.
 *
 *      input:
 *              r3      = Total DRAM size (in bytes)
 *      notes:
 *              1. must run as a supervisor function
 *              2. interrupts must be masked
 *              3. address translation (MMU) disabled
 *              4. assumes ECC Control register is in a power-up state
 *              5. The scrubber is not enabled (SWEN=0).  This provides
 *                 better performance at the expense of less fault
 *                 tolerance.
 */
.harrier_mem_scrub:

    /* setup local variables */

    addi    r15,r0,0                /* load starting addr - always zero */
    or      r16,r3,r3               /* load number of bytes             */
    rlwinm  r16,r16,29,3,31         /* calculate number of doubles      */

    /* Make sure FPU is enabled; it's needed for DRAM loads and stores */

    mfmsr   r14                     /* Save current value of MSR in r14 */
    addi    r4,r0,0x2000            /* FP bit definition */
    or      r4,r4,r14
    mtmsr   r4
    isync

    /*
     * invalidate/enable the processor data cache, one of the assumptions
     * is that address translation is disabled, the default caching mode
     * is copy-back when address translation is disabled, copy-back mode
     * is the recommended caching mode
     */
   
    bl      dCacheInval
    bl      dCacheOn

    /*
     * Loop through the entire DRAM array and initialize memory.  This has
     * the side effect of initializing the ECC check bits because they are
     * always updated when the DRAM is written.
     *
     * The data path to DRAM is the size of a cache line (128-bits), this
     * is why the data cache is enabled, the initialization of the ECC check
     * bits are optimized when a cache line write/flush occurs
     */
    
harrier_mem_scrub_start:
    addi    r17,r15,-8              /* starting address - munged */
    mtspr   9,r16                   /* load number of doubles in counter */
harrier_mem_scrub_iloop:
    lfdu    0,8(r17)
    stfd    0,0(r17)
    bc      16,0,harrier_mem_scrub_iloop          /* branch till counter == 0 */

    eieio
    sync

    /*
     * Flush L1 data cache so that the last segment (data cache size) of
     * DRAM is initialized.  This is done by flushing 2 x the data cache
     * size (32K).
     */

    lis     r16,HI(0x10000/_CACHE_ALIGN_SIZE)     /* load number of cache */
    ori     r16,r16,(0x10000/_CACHE_ALIGN_SIZE)   /* lines in 64KB        */

    mtspr   9,r16                                 /* move to counter */
    eieio
    sync

harrier_mem_scrub_floop:
    dcbf    r0,r17                                /* flush line */
    addi    r17,r17,-(_CACHE_ALIGN_SIZE)          /* next (previous) line */
    bc      16,0,harrier_mem_scrub_floop          /* branch till counter == 0 */

    eieio
    sync

    /* disable the data cache */

    bl      dCacheInval
    bl      dCacheOff

    /* Restore original value of MSR */

    mtmsr   r14
    isync

    /* Clear any possible error conditions that occurred during init */

    lis     r14,HI(HARRIER_XCSR_BASE)
    ori     r14,r14,LO(HARRIER_XCSR_BASE)

    addis   r9,r0,0x00F3            /* WRITE-CLEAR any possible errors */
    ori     r9,r9,0x0000            /* PPC60x bits
    stw     r9,0x58(r14)            /* EECL: clear error bits */
    eieio
    sync

    /* clear the single bit error count register */

    addis   r9,r0,0x0000            /* read-only bits */
    ori     r9,r9,0x0000            /* SECNT=0 */
    stw     r9,0x140(r14)           /* SDSES: clear SBE count */
    eieio
    sync

    /* disable the DRAM scrubber */

    addis   r9,r0,0x0000            /* SCWE=0,SCPA=00 */
    ori     r9,r9,0x0000            /* reserved */
    stw     r9,0x130(r14)           /* SDSC: set scrubber to disabled */
    eieio                           /* synchronize the data stream */
    sync                            /* synchronize the data stream */

    /*
     *      Error Exception Enable Register: enable SSE, SSC, SMX, SMS, SOF
     *      SSOF, and SMOF memory controller single/multi-bit machine check
     *      on error enable bits.
     */

    lwz     r10,0x0050(r14)         /* load EEEN current settings */
    eieio
    sync
    addis   r9,r0,0x00F0            /* set all SDRAM exception enables */
    ori     r9,r9,0x0000            /* leave as is */
    or      r9,r9,r10               /* set */
    stw     r9,0x0050(r14)          /* EEEN: write changes */
    eieio
    sync

    /*
     *      Error Exception Clear Register: clear any outstanding
     *      SDRAM ECC logged errors so we start fresh.
     */

    addis   r9,r0,0x00F3            /* clear all SDRAM ECC error counts */
    ori     r9,r9,0x0000            /* clear SSE,SSC,SMX,SMS,SSOF,SMOF */
    stw     r9,0x0058(r14)          /* EECL: write changes */
    eieio
    sync

    /*
     *      Error Exception Machine Check 0 Enable: enable machine check
     *      pulse if Single or Multi Bit Errors to CPU0.
     *
     */

    lwz     r10,0x0060(r14)         /* load EEMCK0 current settings */
    eieio
    sync
    addis   r9,r0,0x00F0            /* SDRAM machine chk enables */
    ori     r9,r9,0x0000            /* PPC60x enables */
    or      r9,r9,r10               /* set */
    stw     r9,0x0060(r14)          /* EEMCK0: write enables */
    eieio
    sync

    /* enable DRAM */

    lhz     r10,0x0100(r14)         /* load SDGC current settings */
    eieio
    sync
#ifdef INCLUDE_ECC
    ori     r9,r0,0xFDFF            /* DREF=0,DERC=1,RWCB=0 */
#else /* INCLUDE_ECC */
    ori     r9,r0,0xFFFF            /* DREF=0,DERC=0,RWCB=0 */
#endif /* INCLUDE_ECC */
    and     r9,r9,r10               /* clearing DERC enables ECC */
    sth     r9,0x0100(r14)          /* write SDGC */
    eieio                           /* synchronize the data stream */
    sync                            /* synchronize the data stream */

    lis     r5,HI(HARRIER_XCSR_BASE)
    ori     r5,r5,LO(HARRIER_XCSR_BASE)

    /* setup UART0 */

    addis   r10,r0,0x00000
    ori     r10,r10,0x001B
    stb     r10,0x00D3(r5)          /* set the BAUD rate */
    eieio
    sync

#ifdef INCLUDE_PRPMC800XT
    addis   r10,r0,0x0600           /* set the ENABLE COM1 & COM2 bits */
#else  /* INCLUDE_PRPMC800XT */
    addis   r10,r0,0x0400           /* set the ENABLE COM1 bit */
#endif /* INCLUDE_PRPMC800XT */
    ori     r10,r10,0x0000
    stw     r10,0x0040(r5)          /* write to FEEN function exception register */
    eieio
    sync
    lwz     r9,0x0048(r5)
    eieio
    sync

#ifdef INCLUDE_PRPMC800XT
    addis   r10,r0,0xF9FF           /* remove UART0 & UART1 mask bits */
#else  /* INCLUDE_PRPMC800XT */
    addis   r10,r0,0xFBFF           /* remove UART0 mask bit */
#endif /* INCLUDE_PRPMC800XT */
    ori     r10,r10,0xFFFF
    and     r10,r10,r9
    stw     r10,0x0048(r5)          /* write to FEMA mask register */
    eieio
    sync

/*
 *   1. Exception Interrupts are NOT enabled by this code.
 *   2. This code must be placed AFTER the memory probing is
 *      complete to prevent 60x bus timeouts caused by probing,
 *      which may cause a Machine Check pulse.
 *
 * Error Exception Clear Register: clear any outstanding errors
 * so we start fresh, like a fine mist on a summers day...
 */
    addis   r9,r0,0x0000        /* PCI/SDRAM error status */
    ori     r9,r9,0xFF00        /* clear XBT,XAP,XDP,XDT,XOF */
    stw     r9,0x0058(r5)       /* EECL: clear */
    eieio               /* synchronize the data stream */
    sync                /* synchronize the data stream */

/*
 * Error Exception Enable Register: enable the PPC60x Bus Timeout Error,
 * Address Parity Error, Data Parity Error, and Delayed Transaction Timeout.
 *
 */
    lwz r9,0x0050(r5)       /* load EEEN current settings */
    eieio
    sync
    addis   r10,r0,0x0000       /* PCI/SDRAM error enables */
    ori r10,r10,0xF000      /* XBT=1,XAP=1,XDP=1,XDT=1 */
    or  r9,r9,r10       /* set */
    stw r9,0x0050(r5)       /* Error Exception Enable register */
    eieio               /* synchronize the data stream */
    sync                /* synchronize the data stream */

/*
 * Enable machine check pulse if PPC60x Bus Timeout, Address Parity
 * Error, Data Parity Error, or Delayed Transaction Timeout occurs.
 *  Note:
 *   - This enables machine check pulse for CPU0 ONLY.
 */
    lwz r9,0x0060(r5)       /* load EEMCK0 current settings */
    eieio
    sync
    addis   r10,r0,0x0000       /* PCI/SDRAM enables */
    ori r10,r10,0xF000      /* XBT=1,XAP=1,XDP=1,XDT=1 */
    or  r9,r9,r10       /* set */
    stw r9,0x0060(r5)       /* EEMCK0: write enables */
    eieio               /* synchronize the data stream */
    sync                /* synchronize the data stream */

    addis   r9,r0,0xaa01
    ori r9,r9,0x0000
    stw     r9,0x0094(r5)       /* XARB: set */
    eieio
    sync

/*  Zero PCI_AUTOCONFIG_FLAG here */

    xor     r5,r5,r5
    xor     r10,r10,r10
    ori     r5,r5,PCI_AUTOCONFIG_FLAG_OFFSET
    stb     r10,0(r5)
    sync

#ifndef INCLUDE_BPE

 /* mask machine checks (EMCP), address parity (EBA), data parity (EBD) */

    mfspr r3,HID0
    lis r4,HI(~(_PPC_HID0_EMCP | _PPC_HID0_EBA | _PPC_HID0_EBD))
    ori r4,r4,LO(~(_PPC_HID0_EMCP | _PPC_HID0_EBA | _PPC_HID0_EBD))
    and r4,r4,r3
    mtspr HID0,r4

#endif /* !INCLUDE_BPE */

    /* go to C entry point */

    or      r3,r31,r31
    addi    sp,sp,-FRAMEBASESZ    /* get frame stack */

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
* dCacheOn - Turn Data Cache On
*
* void dCacheOn (void)
*/

FUNC_LABEL(_dCacheOn)
FUNC_BEGIN(dCacheOn)
    /* Get cpu type */

    mfspr   r3,PVR
    rlwinm  r3,r3,16,16,31

    cmpli   0,0,r3,CPU_TYPE_750
    bc      12,2,ccdataon
    cmpli   0,0,r3,CPU_TYPE_NITRO
    bc      12,2,ccdataon
    cmpli   0,0,r3,CPU_TYPE_MAX
    bc      12,2,ccdataon
    bclr    0x14,0x0        /* invalid cpu type */

ccdataon:
    mfspr   r4,HID0        /* Modify HID0 to enable D cache (DCE) */
    ori     r4,r4,_PPC_HID0_DCE
    sync                   /* required before changing DCE */
    mtspr   HID0,r4
    bclr    0x14,0x0    /* return to caller */
FUNC_END(dCacheOn)

/******************************************************************************
*
* dCacheOff - Turn Data Cache Off
*
* void dCacheOff (void)
*/
FUNC_LABEL(_dCacheOff)
FUNC_BEGIN(dCacheOff)
    /* Get cpu type */

    mfspr   r3,PVR
    rlwinm  r3,r3,16,16,31

    cmpli   0,0,r3,CPU_TYPE_750
    bc      12,2,ccDataOff
    cmpli   0,0,r3,CPU_TYPE_NITRO
    bc      12,2,ccDataOff
    cmpli   0,0,r3,CPU_TYPE_MAX
    bc      12,2,ccDataOff
    bclr    0x14,0x0        /* invalid cpu type */

ccDataOff:
    mfspr   r4,HID0        /* Modify HID0 to disable D cache (DCE) */
    rlwinm  r4,r4,0,_PPC_HID0_BIT_DCE+1,_PPC_HID0_BIT_DCE-1    /* clear DCE */
    sync            /* required before changing DCE */
    mtspr   HID0,r4
    bclr    0x14,0x0    /* return to caller */
FUNC_END(dCacheOff)


/******************************************************************************
*
* dCacheInval - Invalidate Data Cache
*
* void dCacheInval (void)
*/

FUNC_LABEL(_dCacheInval)
FUNC_BEGIN(dCacheInval)
    /* Get cpu type */

    mfspr   r3,PVR
    rlwinm  r3,r3,16,16,31

    cmpli   0,0,r3,CPU_TYPE_750
    bc      12,2,ccDataInvl
    cmpli   0,0,r3,CPU_TYPE_NITRO
    bc      12,2,ccDataInvl
    cmpli   0,0,r3,CPU_TYPE_MAX
    bc      12,2,ccDataInvl
    bclr    0x14,0x0        /* invalid cpu type */

    /*
     * To invalidate the Data Cache on a 750, it's necessary
     * to set the DCFI bit while the Data Cache is enabled (DCE).
     */
    
ccDataInvl:
    addis   r3,r0,0x0000    /* Setup bit pattern for DCFI + DCE */
    ori     r3,r3,(_PPC_HID0_DCE | _PPC_HID0_DCFI)

    mfspr   r4,HID0        /* Modify HID0 to SET DCFI + DCE bits */
    or      r4,r4,r3
    sync                   /* required before changing DCE */
    mtspr   HID0,r4

    andc    r4,r4,r3      /* Modify HID0 to CLEAR DCFI + DCE bits */
    sync                  /* required before changing DCE */
    mtspr   HID0,r4
    bclr    0x14,0x0    /* return to caller */
FUNC_END(dCacheInval)
