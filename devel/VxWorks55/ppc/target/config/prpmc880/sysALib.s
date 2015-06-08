/* sysALib.s - sysInit assembly routine */

/* Copyright 1984-2003 Wind River Systems, Inc. */
/* Copyright 1996-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,02Jun03,simon  updated based on peer review results
01a,31mar03,simon  Ported. from ver 01a, mcp905/sysALib.s.
*/

/*
DESCRIPTION
This module contains the sysInit routines written in assembly language.

This module must be the first specified in the \f3ld\f1 command used to
build the system.  The sysInit() routine is the system start-up code.
*/

#define _ASMLANGUAGE

/* includes */

#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "regs.h"
#include "asm.h"

/* defines */

#define DSSALL .long 0x7E00066C  /* explicit encoding for DSSALL instruction */

/* Special Purpose Registers */

#define MSSCR0              1014         /* Memory Subsystem Control Register */
#define L2CR                1017         /* L2 Cache Control Register */
#define PPC_L2CR_L2IO       0x00100000   /* L2 Instruction-Only Mode */

/* globals */

    FUNC_EXPORT(_sysInit)        /* start of system code */

/* externals */

    FUNC_IMPORT(usrInit)
     
    _WRS_TEXT_SEG_START

/*******************************************************************************
*
* sysInit - start after boot
*
* This is the system start-up entry point for VxWorks in RAM, the
* first code executed after booting.  It disables interrupts, sets up
* the stack, and jumps to the C routine usrInit() in usrConfig.c.
*
* The initial stack is set to grow down from the address of sysInit().  This
* stack is used only by usrInit() and is never used again.  Memory for the
* stack must be accounted for when determining the system load address.
*
* NOTE: This routine should not be called by the user.
*
* RETURNS: N/A
*/

FUNC_BEGIN(_sysInit)

    /* disable external interrupts */

    xor     p0, p0, p0
    mtmsr   p0                      /* clear the MSR register  */

    /* return from decrementer exceptions */

    addis   p1, r0, 0x4c00
    addi    p1, p1, 0x0064          /* load rfi (0x4c000064) to p1      */
    stw     p1, 0x900(r0)           /* store rfi at 0x00000900          */

    /* initialize the stack pointer */

    lis     sp, HIADJ(RAM_LOW_ADRS)
    addi    sp, sp, LO(RAM_LOW_ADRS)

    /* Disable the Data Cache */

    bl      dCacheOff

    /* Disable the Instruction Cache */

    bl      iCacheOff

    /*
     * Enable branch history table
     */

    mfspr   r3,HID0
    ori     r3,r3,_PPC_HID0_BHTE
    mtspr   HID0,r3
    isync

    /* Disable the data & instruction translation */

    mfmsr   r3
    rlwinm  r4,r3,0,_PPC_MSR_BIT_DR+1,_PPC_MSR_BIT_IR-1  /* MSR[DR] = 0 */
    DSSALL                                               /* MSR[IR] = 0 */
    sync
    mtmsr   r4
    sync
    isync

    /* initialize the BAT registers */

    xor     r0,r0,r0

    isync
    mtspr   IBAT0U,r0
    isync
    mtspr   IBAT0L,r0
    isync
    mtspr   IBAT1U,r0
    isync
    mtspr   IBAT1L,r0
    isync
    mtspr   IBAT2U,r0
    isync
    mtspr   IBAT2L,r0
    isync
    mtspr   IBAT3U,r0
    isync
    mtspr   IBAT3L,r0
    isync
    mtspr   DBAT0U,r0
    isync
    mtspr   DBAT0L,r0
    isync
    mtspr   DBAT1U,r0
    isync
    mtspr   DBAT1L,r0
    isync
    mtspr   DBAT2U,r0
    isync
    mtspr   DBAT2L,r0
    isync
    mtspr   DBAT3U,r0
    isync
    mtspr   DBAT3L,r0
    isync

    /* Invalidate Translation Lookaside Buffers */

    bl      tlbInval

    addi    sp, sp, -FRAMEBASESZ    	/* get frame stack */
    li      r3, BOOT_WARM_AUTOBOOT
    b       usrInit            		/* never returns - starts up kernel */
FUNC_END(_sysInit)

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

    bclr    0x14,0x0           /* return to caller */
FUNC_END(tlbInval)

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
    and     r4,r4,r5               /* r4 = MSSCR0 with L2PFE bits cleared */
    DSSALL                         /* required before changing L2PFE bits */
    sync
    mtspr   MSSCR0,r4              /* clear L2PFE bits */
    sync                           /* required after changing L2PFE bits */
    isync

    bclr    0x14,0x0               /* return to caller */
FUNC_END(_dCacheOff)
FUNC_END(dCacheOff)

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

    bclr    0x14,0x0               /* return to caller */
FUNC_END(_iCacheOff)
FUNC_END(iCacheOff)
