/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

 * sysALib.s - System-dependent assembly routines */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1996-1999 Motorola, Inc. */

/*
modification history
--------------------
*/

/*
* DESCRIPTION
* This module contains the entry code, sysInit(), for VxWorks images that start
* running from RAM, such as 'vxWorks'. These images are loaded into memory
* by some external program (e.g., a boot ROM) and then started.
* The routine sysInit() must come first in the text segment. Its job is to
* perform the minimal setup needed to call the generic C Imaginary Buffer Line
* routine usrInit() with parameter BOOT_COLD.
*
* The routine sysInit() typically masks interrupts in the processor, sets the
* initial stack pointer to _sysInit then jumps to usrInit.
* Most other hardware and device initialization is performed later by
* sysHwInit().
*/

#define _ASMLANGUAGE

/* includes */

#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "regs.h"    
#include "asm.h"
#include "gtInitLib.h"
#include "cacheLib.h"
#ifdef INCLUDE_L2_CACHE
#include "sysL2BackCache.h"
#endif /* INCLUDE_L2_CACHE */

/*
 * Some releases of h/arch/ppc/toolPpc.h had a bad definition of
 * LOADPTR. So we will define it correctly. [REMOVE WHEN NO LONGER NEEDED].
 *
 * LOADPTR initializes a register with a 32 bit constant, presumably the
 * address of something.
 */

#undef LOADPTR
#define LOADPTR(reg,const32) \
                lis reg,HI(const32); ori reg,reg,LO(const32)


/* defines */
#define L2PMCR         1016               /* in 745x - LDSTCR */
#define L2CR           1017               /* L2CR register */
#define L3CR           1018               /* L3CR register */

#define INTERNAL_BASE  (INTERNAL_REG_ADRS >> 16)

/* globals */

       FUNC_EXPORT(_sysInit)                /* start of system code */
       FUNC_EXPORT(sysMemProbeSup)
       FUNC_EXPORT(sysGetCpuRev)            /* Get CPU Revision     */
       FUNC_EXPORT(sysGetCpuVer)            /* Get CPU Version      */
       FUNC_EXPORT(sysGetPrid) 
       FUNC_EXPORT(sysSioWrite)
       FUNC_EXPORT(sysSioRead)
       FUNC_EXPORT(sysRdCpuReg)
       FUNC_EXPORT(sysInByte)
	 FUNC_EXPORT(sysOutByte)
	 FUNC_EXPORT( _initialize_dbg_uart)
       FUNC_EXPORT(vxHid1Get)
       FUNC_EXPORT(vxHid1Set)
       FUNC_EXPORT(sysGetL2CR)  /* Get the L2CR register (L2 cache) */ 
       FUNC_EXPORT(sysGetL3CR)  /* Get the L3CR register (L3 cache) */ 
       FUNC_EXPORT(sysL2crPut)
       FUNC_EXPORT(sysL2crGet)

       FUNC_EXPORT(sysGetSDR1)
#ifdef PMC280_DEBUG_LED
       FUNC_EXPORT(dbg_led_23_on)
       FUNC_EXPORT(dbg_led_23_off)
       FUNC_EXPORT(dbg_led_23_blink)
#endif /* PMC280_DEBUG_LED */

       /* externals */

       .extern usrInit

       .text

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
* RETURNS:
*        N/A
* sysInit (void)              /@ THIS IS NOT A CALLABLE ROUTINE @/
*/

_sysInit:

       /* disable external interrupts */
       xor     p0, p0, p0
#ifndef PMC280_DUAL_CPU
       mtmsr   p0                      /* clear the MSR register  */
       mtspr   L2CR,p0
#endif /* !PMC280_DUAL_CPU */


        /* Zero-out registers: r0 & SPRGs */

       xor     r0,r0,r0
       mtspr   SPRG0, r0
       mtspr   SPRG1, r0
       mtspr   SPRG2, r0
       mtspr   SPRG3, r0

#ifndef PMC280_DUAL_CPU
        /*
         *      Set MPU/MSR to a known state
         *      Turn on FP
         */

        andi.   r3, r3, 0
        ori     r3, r3, 0x2000
        sync
        mtmsr   r3
        isync
        sync

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
        sync

        /* Initialize the floating point data registers to a known state */
        bl      ifpdrValue
        .long   0x3f800000      /* 1.0 */

ifpdrValue:
        mfspr   r3,LR
        lfs     f0,0(r3)
        lfs     f1,0(r3)
        lfs     f2,0(r3)
        lfs     f3,0(r3)
        lfs     f4,0(r3)
        lfs     f5,0(r3)
        lfs     f6,0(r3)
        lfs     f7,0(r3)
        lfs     f8,0(r3)
        lfs     f9,0(r3)
        lfs     f10,0(r3)
        lfs     f11,0(r3)
        lfs     f12,0(r3)
        lfs     f13,0(r3)
        lfs     f14,0(r3)
        lfs     f15,0(r3)
        lfs     f16,0(r3)
        lfs     f17,0(r3)
        lfs     f18,0(r3)
        lfs     f19,0(r3)
        lfs     f20,0(r3)
        lfs     f21,0(r3)
        lfs     f22,0(r3)
        lfs     f23,0(r3)
        lfs     f24,0(r3)
        lfs     f25,0(r3)
        lfs     f26,0(r3)
        lfs     f27,0(r3)
        lfs     f28,0(r3)
        lfs     f29,0(r3)
        lfs     f30,0(r3)
        lfs     f31,0(r3)
        sync

      /* Set MPU/MSR to a known state, Turn off FP */
        andi.   r3, r3, 0
        sync
        mtmsr   r3
        isync
        sync

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
#endif /* !PMC280_DUAL_CPU */
  	
	bl		_initialize_dbg_uart 

        /* insert protection from decrementer exceptions */
       xor      p0, p0, p0
       LOADPTR (p1, 0x4c000064)        /* load rfi (0x4c000064) to p1      */
       stw     p1, 0x900(p0)           /* store rfi at 0x00000900          */

       /* initialize the stack pointer */
       lis     sp, HI(RAM_LOW_ADRS)
       addi    sp, sp, LO(RAM_LOW_ADRS)

       /* Turn off data and instruction cache control bits */
       mfspr   r3, HID0
       isync
       rlwinm  r4, r3, 0, 18, 15       /* r4 has ICE and DCE bits cleared */
       sync
       isync
       mtspr   HID0, r4                /* HID0 = r4 */
       isync

       /* Get cpu type */
       mfspr     r28, PVR              /* Get cpu type */
       rlwinm    r28, r28, 16, 16, 31  /* r28 should have 16 bits of CPU type */

       /* invalidate the MPU's data/instruction caches */
       lis      r3, 0x0                    /* clear r3 */
       cmpli    0, 0, r28, CPU_TYPE_750
       beq      cpuIs750
       cmpli    0, 0, r28, CPU_TYPE_750FX
       beq      cpuIs750
       cmpli    0, 0, r28, CPU_TYPE_7400
       beq      cpuIs750
       cmpli    0, 0, r28, CPU_TYPE_7410
       beq      cpuIs7410
       cmpli    0, 0, r28, CPU_TYPE_7450
       beq      cpuIs750
       cmpli    0, 0, r28, CPU_TYPE_7455
       beq      cpuIs750
       cmpli    0, 0, r28, CPU_TYPE_7447
       beq      cpuIs750
       cmpli    0, 0, r28, CPU_TYPE_7447A
       beq      cpuIs750
       b        cacheEnableDone

cpuIs7410:
       andi.    r3, r3, 0
       mtspr    L2PMCR, r3       /* 7410 bug, L2PMCR not cleared on reset */
       b        cpuIs750

cpuIs750:                               /* for 750, 7400, 7410 */
#ifdef INCLUDE_L2_CACHE
       mfspr   r3,L2CR                 /* Step 1: disable cache */
       lis     r4,0x7FFF               /* Setup mask for L2E   */
       ori     r4,r4,0xFFFF
       and     r3,r3,r4                /* Reset L2E            */
       sync
       mtspr   L2CR,r3
       sync

       oris    r3, r3, 0x0020          /* Step 2: set the      */
       mtspr   L2CR, r3                /* global invalidate bit*/

invalidate_in_progress1:
       sync                            /* Step 3: Wait for     */
       mfspr   r3,L2CR                 /* L2CR[L2I] = 0        */
       andi.   r3, r3, 0x1
       cmpwi   r3,0x1
       beq     invalidate_in_progress1  /* still invalidating   */

       sync                            /* Step 4: Clear L2CR[L2I] */
       lis     r4,0x7FDF               /* Setup mask for L2E and L2I */
       ori     r4,r4,0xFFFF
       mfspr   r3,L2CR
       and     r3,r3,r4                /* Reset bits */
       mtspr   L2CR, r3
       sync
#endif INCLUDE_L2_CACHE
       b       invCacheLayersDone

inv_745x:
#ifdef INCLUDE_L2_CACHE
       /* Invalidate MPC745x L2 cache */
       mfspr   r3,L2CR                 /* Step 1 disable cache */
       lis     r4,0x7FFF               /* Setup mask for L2E   */
       ori     r4,r4,0xFFFF
       and     r3,r3,r4                /* Reset L2E            */
       sync
       mtspr   L2CR,r3
       sync

ilcv_lp:
       mfspr    r3,L2CR
       sync
       andis.    r3,r3,0x0020       /* Still invalidating? */
       bne       ilcv_lp
       sync
#endif

#ifdef INCLUDE_L3_CACHE
       /* Invalidate MPC745x L3 cache */
       mfspr   r3,L3CR                 /* Step 1 disable cache */
       lis     r4,0x7FFF               /* Setup mask for L3E   */
       ori     r4,r4,0xFFFF
       and     r3,r3,r4                /* Reset L3E            */
       sync
       mtspr   L3CR,r3
       sync

ilv_lp:
       mfspr   r3,L3CR
       isync
       andi.   r3,r3,0x0400          /* Still invalidating? */
       bne     ilv_lp
       sync
#endif INCLUDE_L3_CACHE

invCacheLayersDone:
#ifdef USER_I_CACHE_ENABLE
       mfspr   r3,HID0

       addis   r4,r0,0x0000    /* Clear r4 */
       ori     r4,r4,0x8800    /* Setup bit pattern for ICE/ICFI */
       or      r3,r4,r3
       isync
       mtspr   HID0,r3         /* set ICE/ICFI */
       isync

       addis   r4,r0,0x0000    /* Clear r4 */
       ori     r4,r4,0x0800    /* Setup bit pattern for ICFI */
       andc    r3,r3,r4
       isync
       mtspr   HID0,r3         /* clear IFCI (bit 16) */
       isync

       addis   r4,r0,0x0000    /* Clear r4 */
       ori     r4,r4,0x2000    /* Setup bit pattern for ILOCK */
       andc    r3,r3,r4
       isync
       mtspr   HID0,r3         /* clear ILOCK (bit 18) */
       isync
       sync
#endif USER_I_CACHE_ENABLE
       b       cacheEnableDone


cacheEnableDone:
       /* In case of MPC745x Enable MPC745x time base */
       cmpli   0, 0, r28, CPU_TYPE_7450
       beq     init745xHid0
       cmpli   0, 0, r28, CPU_TYPE_7455
       beq     init745xHid0
       cmpli   0, 0, r28, CPU_TYPE_7447
       beq     init745xHid0
       cmpli   0, 0, r28, CPU_TYPE_7447A
       beq     init745xHid0
       b       continue1

init745xHid0:
       mfspr   r3,HID0
       oris    r3,r3, 0x0400            /* Set TBEN */
       oris    r3,r3, 0x0010            /* Set DPM  */
       ori     r3,r3, 0x0010  /* Set Branch target instruction cache enable */ 
       ori     r3,r3, 0x0008  /* Set Link register stack enable             */ 
       isync                            /* For safty */
       mtspr   HID0,r3


       /* Inprove overall system operation using HID0 register */
continue1:
       mfspr   r3,HID0
       ori     r3,r3,_PPC_HID0_BHTE       /* enable the branch history table. */
       ori     r3,r3,0x0080               /* Enable Integer Store Gathering */
       cmpli   0, 0, r28, CPU_TYPE_750
       beq     continue2
       cmpli   0, 0, r28, CPU_TYPE_750FX
       beq     continue2
       ori     r3,r3,0x0020                /* Enable BTIC only for MPC7400. */
                                        /* MPC750 is automatically enabled. */
continue2:
       mtspr   HID0,r3

disableMmu:
       /* disable instruction and data translations in the MMU */
       sync
#ifndef PMC280_DUAL_CPU

       mfmsr     r3               /* get the value in msr *
                                 /* clear bits IR and DR */

       rlwinm    r4, r3, 0, _PPC_MSR_BIT_DR+1, _PPC_MSR_BIT_IR - 1

       mtmsr     r4             /* set the msr */
       isync
       sync                     /* SYNC */

       /* initialize the BAT register */

       li        p3,0               /* clear p0 */

       isync
       mtspr     IBAT0U,p3          /* SPR 528 (IBAT0U) */
       isync

       mtspr     IBAT0L,p3          /* SPR 529 (IBAT0L) */
       isync

       mtspr     IBAT1U,p3          /* SPR 530 (IBAT1U) */
       isync

       mtspr     IBAT1L,p3          /* SPR 531 (IBAT1L) */
       isync

       mtspr     IBAT2U,p3          /* SPR 532 (IBAT2U) */
       isync

       mtspr     IBAT2L,p3          /* SPR 533 (IBAT2L) */
       isync

       mtspr     IBAT3U,p3          /* SPR 534 (IBAT3U) */
       isync

       mtspr     IBAT3L,p3          /* SPR 535 (IBAT3L) */
       isync

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
      /*
       * Since MPC7447 has 8 IBAT pairs we need to initialize the 4 new
       * ones.
       */
       mtspr     560,p3          /* SPR 560 (IBAT4U) */
       isync

       mtspr     561,p3          /* SPR 561 (IBAT4L) */
       isync

       mtspr     562,p3          /* SPR 562 (IBAT5U) */
       isync

       mtspr     563,p3          /* SPR 563 (IBAT5L) */
       isync
        
       mtspr     564,p3          /* SPR 564 (IBAT6U) */
       isync

       mtspr     565,p3          /* SPR 565 (IBAT6L) */
       isync

       mtspr     566,p3          /* SPR 566 (IBAT7U) */
       isync

       mtspr     567,p3          /* SPR 567 (IBAT7L) */
       isync
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

       mtspr     DBAT0U,p3          /* SPR 536 (DBAT0U) */
       isync

       mtspr     DBAT0L,p3          /* SPR 537 (DBAT0L) */
       isync

       mtspr     DBAT1U,p3          /* SPR 538 (DBAT1U) */
       isync

       mtspr     DBAT1L,p3          /* SPR 539 (DBAT1L) */
       isync

       mtspr     DBAT2U,p3          /* SPR 540 (DBAT2U) */
       isync

       mtspr     DBAT2L,p3          /* SPR 541 (DBAT2L) */
       isync

       mtspr     DBAT3U,p3          /* SPR 542 (DBAT3U) */
       isync

       mtspr     DBAT3L,p3          /* SPR 543 (DBAT3L) */
       isync

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
      /*
       * Since MPC7447 has 8 DBAT pairs we need to initialize the 4 new
       * ones.
       */

       mtspr     568,p3          /* SPR 568 (DBAT4U) */
       isync

       mtspr     569,p3          /* SPR 569 (DBAT4L) */
       isync

       mtspr     570,p3          /* SPR 570 (DBAT5U) */
       isync

       mtspr     571,p3          /* SPR 571 (DBAT5L) */
       isync
        
       mtspr     572,p3          /* SPR 572 (DBAT6U) */
       isync

       mtspr     573,p3          /* SPR 573 (DBAT6L) */
       isync

       mtspr     574,p3          /* SPR 574 (DBAT7U) */
       isync

       mtspr     575,p3          /* SPR 575 (DBAT7L) */
       isync
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

       /* invalidate entries within both TLBs */

       li        p1,128
       xor       p0,p0,p0           /* p0 = 0    */
       mtctr     p1                 /* CTR = 32  */

       isync                        /* context sync req'd before tlbie */
sysALoop:
       tlbie     p0
       addi      p0,p0,0x1000       /* increment bits 15-19 */
       bdnz      sysALoop           /* decrement CTR, branch if CTR != 0  */
       sync                         /* sync instr req'd after tlbie       */
#endif /* !PMC280_DUAL_CPU */

       addi      sp, sp, -FRAMEBASESZ           /* get frame stack */
       li        r3, BOOT_WARM_AUTOBOOT
       b         usrInit                /* never returns - starts up kernel */
       /* Initialize MPSC 0/1 as the debug port */
_initialize_dbg_uart:
  
        /* Set MPP[20] as BClkIn */
        lis     r3, INTERNAL_BASE
        ori     r3,r3,0xF008
        lis     r4,0x4
        stwbrx  r4,0,(r3)

        /* Make MPP[0], MPP[1], MPP[3] and MPP[5] as Tx/Rx pins for MPSC 0/1 */
        lis     r3, INTERNAL_BASE
        ori     r3,r3,0xF000
        lis     r4,0x0030
        ori     r4,r4,0x3022
        stwbrx  r4,0,(r3)
 
        /* MRR: Connect MPSC 0/1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0xB400
        lis     r4,0x0000
        ori     r4,r4,0x0000
        stwbrx  r4,0,(r3)
 
        /* RCR = BRG0 for MPSC0, BRG1 for MPSC1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0xB404
        lis     r4,0
        ori     r4,r4,0x100
        stwbrx  r4,0,(r3)
 
        /* TCR = BRG0 for MPSC0, BRG1 for MPSC1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0xB408
        lis     r4,0
        ori     r4,r4,0x100
        stwbrx  r4,0,(r3)
  
        /* BRG0 = 115200 bps@12.5 MHz using BClkIn */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0xB200
        lis     r4,0x0041    /* 0x0061 (SysClk)/0x0041 (BClkIn) */
        ori     r4,r4,/*0x0051*/0x0014/*0x0006*/ /* 115200@66MHz = 0x0022, 115200@12.5 = 0x0006 */
                             /* 9600@12.5MHz = 0x0051, 115200@133MHz = 0x0048 */
        stwbrx  r4,0,(r3)

       /* BRG1 = 115200 bps@12.5 MHz using BClkIn */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0xB208
        lis     r4,0x0041    /* 0x0061 (SysClk)/0x0041 (BClkIn) */
        ori     r4,r4,/*0x0051*/0x0014/*0x0006*/ /* 115200@66MHz = 0x0022, 115200@12.5 = 0x0006 */
                             /* 9600@12.5MHz = 0x0051, 115200@133MHz = 0x0048 */
        stwbrx  r4,0,(r3)
 
        /* MPCR = 8N1 for MPSC 0*/
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x8008
        lis     r4,0
        ori     r4,r4,0x3000
        stwbrx  r4,0,(r3)

        /* MMCRL for MPSC 0 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x8000
        lis     r4,0
        ori     r4,r4,0x444
        stwbrx  r4,0,(r3)

        /* MMCRH for MPSC 0 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x8004
        lis     r4,0x0240
        ori     r4,r4,0x03F8
        stwbrx  r4,0,(r3)

        /* MPCR = 8N1 for MPSC 1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x9008
        lis     r4,0
        ori     r4,r4,0x3000
        stwbrx  r4,0,(r3)
 
        /* MMCRL for MPSC 1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x9000
        lis     r4,0
        ori     r4,r4,0x444
        stwbrx  r4,0,(r3)
 
        /* MMCRH for MPSC 1 */
        lis     r3,INTERNAL_BASE
        ori     r3,r3,0x9004
        lis     r4,0x0240
        ori     r4,r4,0x03F8
        stwbrx  r4,0,(r3)
  
        sync
        blr

/*******************************************************************************
*
* sysMemProbeSup - sysBusProbe support routine
*
* This routine is called to try to read byte, word, or long, as specified
* by length, from the specified source to the specified destination.
*
* RETURNS:
*      OK    - if successful probe, 
*      ERROR - if not successful
* STATUS sysMemProbeSup
*     (
*     int  length,  /@ length of cell to test (1, 2, 4) @/
*     char *src,    /@ address to read @/
*     char *dest    /@ address to write @/
*     )
*/

sysMemProbeSup:
        addi    p7, p0, 0       /* save length to p7 */
        xor     p0, p0, p0      /* set return status */
        cmpwi   p7, 1           /* check for byte access */
        bne     sbpShort        /* no, go check for short word access */
        lbz     p6, 0(p1)       /* load byte from source */
        stb     p6, 0(p2)       /* store byte to destination */
        isync                   /* enforce for immediate exception handling */
        blr
sbpShort:
        cmpwi   p7, 2           /* check for short word access */
        bne     sbpWord         /* no, check for word access */
        lhz     p6, 0(p1)       /* load half word from source */
        sth     p6, 0(p2)       /* store half word to destination */
        isync                   /* enforce for immediate exception handling */
        blr
sbpWord:
        cmpwi   p7, 4           /* check for short word access */
        bne     sysProbeExc     /* no, check for double word access */
        lwz     p6, 0(p1)       /* load half word from source */
        stw     p6, 0(p2)       /* store half word to destination */
        isync                   /* enforce for immediate exception handling */
        blr
sysProbeExc:
        li      p0, -1          /* shouldn't ever get here, but... */
        blr

/***************************************************************************
* sysGetCpuVer - Read the content of the PVR register
*      Once the PVR is read, the 16 least significant bits are shifted off.
*
* ULONG sysGetCpuVer
*     (
*     void
*     )
*
* RETURN:
*      upper 16 bits of PVR stored in r3
*
*/
sysGetPrid:
sysGetCpuVer:
       mfspr r4, PVR            /* read PVR  */
       srawi r3, r4, 16         /* shift off the least 16 bits */

        blr

/**********************************************************************
*  sysGetCpuRev - Get the content of the PVR register
*  
*  RETURN: upper 16 bits of PVR stored in r3
*/
sysGetCpuRev:
       mfspr   r4, PVR             /* read PVR  */
       lis     r3, 0
       ori     r3,r3,0xFFFF
       and     r3, r3,r4

       blr

/*******************************************************************************
*
* sysSioRead - this function reads a register from the UIO chip
*
* In order to read data from the desired Super IO register, the index register
* must be written to with the offset of the of the register to be read.  The 
* desired byte of data is then read from the data register.
*
* RETURNS: 
*      byte read from data register
*
* From a C point of view, the routine is defined as follows:
*
UINT8 sysSioRead
    ( 
    UINT8 *pSioIndexReg,     /@ pointer to SIO index register base addr @/
    UINT8 sioRegOffset       /@ offset of register to read from @/
    )

*/

sysSioRead:
        stb     r4,0(r3)        /* write index register with register offset */
        eieio
        sync
        lbz     r3,1(r3)        /* retrieve specified reg offset contents */
        eieio
        sync
        blr                     /* return to caller */

/*******************************************************************************
*
* sysSioWrite - this function writes a register to the UIO chip
*
* In order to write data to the desired Super IO register, the index
* register must be written to with the offset of the of the register to be
* modified.  The desired byte of data can then be written via the data
* register.
*
* RETURNS: N/A
*
* From a C point of view, the routine is defined as follows:

void sysSioWrite
    ( 
    UINT8 * pSioIndexReg,          /@ pointer to SIO index register base addr @/
    UINT8 sioRegOffset,            /@ offset of register to write to @/
    UINT8 data                     /@ 8-bit data to be written @/
    )

*/

sysSioWrite:
        stb     r4,0(r3)        /* write index register with register offset */
        eieio
        sync
        stb     r5,1(r3)        /* 1st write */
        eieio
        sync
        stb     r5,1(r3)        /* 2nd write */
        eieio
        sync
        blr                     /* return to caller */

/* for debug ... */
    
/******************************************************************************
* sysRdCpuReg - read any CPU register 1 till 1023 (only 10 bits)
*
*
* INPUT:
*	reg - cpu register in r3
*
* RETURN:
*	r3 gets the value of cpu register 'reg' if 'reg' is a valid register
*      else you'll get Program Exception on this line !
*	
*/
sysRdCpuReg:
	lis       r6,   chgRdCpuReg@h  /* Load r6 with chgRdCpuReg address     */
	ori       r6,r6,chgRdCpuReg@l
	lwz	r5,0(r6)             /* Load command data from line          */
	rlwinm	r5,r5, 0,21,10       /* clear bits 11-20 for spr number      */
	rlwinm	r7,r3,16,11,15       /* move  bits 27-31 to 11-15 else clear */
	rlwinm	r8,r3, 6,16,20       /* move  bits 22-26 to 16-20 else clear */
	or	r5,r5,r7	  /* or with one part of spr              */
	or	r5,r5,r8	  /* or with second part of spr           */
	stw	r5,0(r6)	  /* store new command in next line       */
	dcbf	r6,r0
	eieio
	icbi	r6,r0
	eieio
	sync
	isync

chgRdCpuReg:
       mfspr	r3,0	/* execute the new command */
       blr                          /* return to caller */
      
/*********************************************************************
*  sysGetL2CR - Read the content of the L2CR register, which 
*  controls the L2 Backside cache on MPC750
*  Input  - None
*  Output - value of l2cr
*/

sysGetL2CR:
       mfspr     r3, L2CR 
       blr 

/*********************************************************************
*  sysGetL3CR - Read the content of the L3R register, which 
*  controls the L3 Backside cache on MPC7450/7455.                         
*  Input  - None
*  Output - value of l3cr
*/

sysGetL3CR:
       mfspr     r3, L3CR 
       blr 

#ifdef PMC280_DUAL_CPU
/*******************************************************************************
*
* sysL2crPut - write to L2CR register of Arthur CPU
*
* This routine will write the contents of r3 to the L2CR
* register.
*             )
* From a C point of view, the routine is defined as follows:
*
*    void sysL2crPut
*             (
*             ULONG       value to write
*             )
*
* RETURNS: NA
*/

sysL2crPut:
      mtspr   L2CR,r3
      blr                     /* return to caller */


/*******************************************************************************
*
* sysL2crGet - read from L2CR register of Arthur CPU
*
* This routine will read the contents the L2CR register.
*
* From a C point of view, the routine is defined as follows:
*
*    UINT sysL2crGet()
*
* RETURNS: value of L2CR (in r3)
*/

sysL2crGet:
      mfspr   r3,L2CR
      blr                     /* return to caller */

#else

/*******************************************************************************
*
* sysL2crPut - write to L2CR register of Arthur CPU
*
* This routine will write the contents of r3 to the L2CR
* register.
*             )
* From a C point of view, the routine is defined as follows:
*
*    void sysL2crPut
*             (
*             ULONG       value to write
*             )
*
* RETURNS: NA
*/
sysL2crPut:
      mtspr   L2CR,r3
      blr                     /* return to caller */

/*******************************************************************************
*
* sysL2crGet - read from L2CR register of Arthur CPU
*
* This routine will read the contents the L2CR register.
*
* From a C point of view, the routine is defined as follows:
*
*    UINT sysL2crGet()
*
* RETURNS: value of L2CR (in r3)
*/

sysL2crGet:
      mfspr   r3,L2CR
      blr                     /* return to caller */

#endif /* PMC280_DUAL_CPU */

/*******************************************************************************
*
* sysGetSDR1 - read Page table register (SDR1)
*
*
* RETURNS: value of SDR1 (in r3)
*/

sysGetSDR1:
      mfspr   r3,SDR1
      blr                     /* return to caller */
    

#ifdef PMC280_DEBUG_LED

        /* Uses r3, r4 */
dbg_led_23_on:
        addis   r3, r0, INTERNAL_BASE
        ori     r3, r3, 0xF104
        addis   r4, r0, 0x0080
        ori     r4, r4, 0x0000
        stwbrx  r4, 0, (r3)                     // Store
        blr

        /* Uses r3, r4 */
dbg_led_23_off:
        addis   r3, r0, INTERNAL_BASE
        ori     r3, r3, 0xF104
        addis   r4, r0, 0x0
        ori     r4, r4, 0x0
        stwbrx  r4, 0, (r3)                     // Store
        blr

        /* Uses r3, r4, r5 */
dbg_led_23_blink:
        mflr    r5
        bl      dbg_led_23_on
        nop
        nop
        bl      dbg_led_23_off
        nop
        nop
        mtlr    r5
        blr
#endif /* PMC280_DEBUG_LED */

#if defined(PMC280_DEBUG_UART) || defined(PMC280_DEBUG_UART_VX)

        /* Put a character from MPSC 0 (gets the character in r3) */
        /* Uses r4, r5 */
        .global _dbg_transmit0
_dbg_transmit0:
 
        lis     r4,0
#ifdef PMC280_DUAL_CPU

        ori     r4,r4,0xFFFF

#else

#ifdef INCLUDE_CACHE_SUPPORT
        ori     r4,r4,0xFFFF
#else
        ori     r4,r4,0xFFF
#endif 
#endif /* PMC280_DUAL_CPU */
        lis     r5,0
 
_dbg_tx0_delay:
        addi    r5,r5,1
        cmp     0, 1, r5, r4
        bne     _dbg_tx0_delay
 
        /* Store char. (in r3) to CH1 register TCS field */
        lis     r4,INTERNAL_BASE
        ori     r4,r4,0x800C
        stwbrx  r3,0,(r4)
 
        /* Give Transmit TCS char. command */
        lis     r4,INTERNAL_BASE
        ori     r4,r4,0x8010
        lis     r5,0x0
        ori     r5,r5,0x200
        stwbrx  r5,0,(r4)
 
        sync
        blr
 
        /* Put a character from MPSC 1 (gets the character in r3) */
        /* Uses r4, r5 */
        .global _dbg_transmit1
_dbg_transmit1:
 
        lis     r4,0
#ifdef PMC280_DUAL_CPU

        ori     r4,r4,0xFFFF

#else

#ifdef INCLUDE_CACHE_SUPPORT
        ori     r4,r4,0xFFFF
#else
        ori     r4,r4,0xFFF
#endif 

#endif /* PMC280_DUAL_CPU */
        lis     r5,0
 
_dbg_tx1_delay:
        addi    r5,r5,1
        cmp     0, 1, r5, r4
        bne     _dbg_tx1_delay
 
        /* Store char. (in r3) to CH1 register TCS field */
        lis     r4,INTERNAL_BASE
        ori     r4,r4,0x900C
        stwbrx  r3,0,(r4)
 
        /* Give Transmit TCS char. command */
        lis     r4,INTERNAL_BASE
        ori     r4,r4,0x9010
        lis     r5,0x0
        ori     r5,r5,0x200
        stwbrx  r5,0,(r4)
 
        sync
        blr
#endif  /* PMC280_DEBUG_UART || PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU

        /*
         * Name  : frcWhoAmI
         *
         *         This function returns the ID of the CPU on which this
         *         code runs.
         *
         * Input : NONE
         * Output: 0(if running on CPU0)/1 (if running on CPU1)
         * Uses  : r3, r4
         */
        .global frcWhoAmI
frcWhoAmI:

        lis     r4, INTERNAL_BASE
        ori     r4, r4, SMP_WHO_AM_I
        lwbrx   r3,0,(r4)
        sync
        blr

        .global frcPermitCPU1ToBoot
frcPermitCPU1ToBoot:

        /* Permit CPU 1 to boot */
        lis     r3, INTERNAL_BASE
        ori     r3, r3, CPU_MASTER_CONTROL
        lwbrx   r4,0,(r3)

        /* r5 = r5 & (~0x00000200) since bit[9] = MaskBR1 */
        lis     r5, 0x0
        ori     r5, r5, 0x0200
        andc    r4, r4, r5
        stwbrx  r4, 0, (r3)                     // Store
        sync
        blr

        /* IBAT */
        .global read_ibat0u
read_ibat0u:
        mfspr   r3, 528
        blr

        .global read_ibat0l
read_ibat0l:
        mfspr   r3, 529
        blr

        .global read_ibat1u
read_ibat1u:
        mfspr   r3, 530
        blr

        .global read_ibat1l
read_ibat1l:
        mfspr   r3, 531
        blr

        .global read_ibat2u
read_ibat2u:
        mfspr   r3, 532
        blr

        .global read_ibat2l
read_ibat2l:
        mfspr   r3, 533
        blr

        .global read_ibat3u
read_ibat3u:
        mfspr   r3, 534
        blr

        .global read_ibat3l
read_ibat3l:
        mfspr   r3, 535
        blr

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        .global read_ibat4u
read_ibat4u:
        mfspr   r3, 560 
        blr

        .global read_ibat4l
read_ibat4l:
        mfspr   r3, 561
        blr

        .global read_ibat5u
read_ibat5u:
        mfspr   r3, 562 
        blr

        .global read_ibat5l
read_ibat5l:
        mfspr   r3, 563
        blr

        .global read_ibat6u
read_ibat6u:
        mfspr   r3, 564 
        blr

        .global read_ibat6l
read_ibat6l:
        mfspr   r3, 565
        blr

        .global read_ibat7u
read_ibat7u:
        mfspr   r3, 566 
        blr

        .global read_ibat7l
read_ibat7l:
        mfspr   r3, 567
        blr
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */

        /* DBAT */
        .global read_dbat0u
read_dbat0u:
        mfspr   r3, 536
        blr

        .global read_dbat0l
read_dbat0l:
        mfspr   r3, 537
        blr

        .global read_dbat1u
read_dbat1u:
        mfspr   r3, 538
        blr

        .global read_dbat1l
read_dbat1l:
        mfspr   r3, 539
        blr

        .global read_dbat2u
read_dbat2u:
        mfspr   r3, 540
        blr

        .global read_dbat2l
read_dbat2l:
        mfspr   r3, 541
        blr

        .global read_dbat3u
read_dbat3u:
        mfspr   r3, 542
        blr

        .global read_dbat3l
read_dbat3l:
        mfspr   r3, 543
        blr

#ifdef PMC280_MMU_SUPPORT_FOR_MPC7447
        .global read_dbat4u
read_dbat4u:
        mfspr   r3, 568 
        blr

        .global read_dbat4l
read_dbat4l:
        mfspr   r3, 569
        blr

        .global read_dbat5u
read_dbat5u:
        mfspr   r3, 570 
        blr

        .global read_dbat5l
read_dbat5l:
        mfspr   r3, 571
        blr

        .global read_dbat6u
read_dbat6u:
        mfspr   r3, 572 
        blr

        .global read_dbat6l
read_dbat6l:
        mfspr   r3, 573
        blr

        .global read_dbat7u
read_dbat7u:
        mfspr   r3, 574 
        blr

        .global read_dbat7l
read_dbat7l:
        mfspr   r3, 575
        blr
#endif /* PMC280_MMU_SUPPORT_FOR_MPC7447 */
#endif /* PMC280_DUAL_CPU */



/*****************************************************************************
*
* sysInByte - reads a byte from an I/O address.
*
* This function reads a byte from a specified I/O address.
*
* RETURNS: byte from address.
*/
sysInByte:

	/* Read byte from I/O space */
	lbzx	r4,r0,r3

	/* Sync I/O operation */
	eieio
	sync

	/* Move data read to return register */
	or	r3,r4,r4

	/* Return to caller */
	bclr	20,0

/******************************************************************************
*
* sysOutByte - writes a byte to an I/O address.
*
* This function writes a byte to a specified I/O address.
*
* RETURNS: N/A
*/
sysOutByte:

	/* Write a byte to I/O space */
	stbx	r4,r0,r3

	/* Sync I/O operation */
	eieio
	sync

	/* Return to caller */
	bclr	20,0
	

/*******************************************************************************
*
* vxHid1Get - read HID1 register (SPR1009)
*
*
* RETURNS: value of SPR1009 (in r3)
*
       	.globl	vxHid1Get
vxHid1Get:
        mfspr   r3,1009
        bclr    20,0                    * Return to caller */
	


vxHid1Set:
	sync
        mtspr   1009,r3 
        sync
        blr                             # back to C-code

        .global GET_THRM1
GET_THRM1:
        sync
        mfspr   r3,1020                 # get THRM1 CPU register
        blr                             # back to C-code

        .global GET_THRM3
GET_THRM3:
        sync
        mfspr   r3,1022                 # get THRM1 CPU register
        blr                             # back to C-code

        .global SET_THRM1
SET_THRM1:
        sync
        mtspr   1020,r3                 # write THRM1 CPU register
        sync
        blr                             # back to C-code

        .global SET_THRM3
SET_THRM3:
        sync
        mtspr   1022,r3                 # write THRM2 CPU register
        sync
        blr                             # back to C-code

        .global __WRITE_FLASH64
__WRITE_FLASH64:
        lfd           f1,0(r4)
        stfd          f1,0(r3)
        eieio
        sync
        blr

        .global __READ_FLASH64
__READ_FLASH64:
        lfd           f1,0(r3)
        stfd          f1,0(r4)
        eieio
        sync
        blr

/******************************************************************************
*
* frccachePpcEnable - Enable the PowerPC Instruction or Data cache
*
* This routine enables the instruction or data cache as selected by the
* <cache> argument.
*
* NOTE:
* This routine should never be called by the user application. Only
* the cacheArchEnable() can call this routine. If the user application calls
* this routine directly the result is impredictable and can crash VxWorks
* kernel.
*
* RETURNS: OK or ERROR if the cache can not be enabled.
*
* SEE ALSO:
*.I "PPC403GA Embedded Controller User's Manual"
*.I "PowerPC 601 RISC Microprocessor User's Manual"
*.I "PowerPC 603 RISC Microprocessor User's Manual"
*.I "PowerPC 604 RISC Microprocessor User's Manual"
* STATUS frccachePpcEnable
*     (
*     CACHE_TYPE  cache,          /@ cache to enable @/
*     )
*/
        .global frccachePpcEnable
frccachePpcEnable:

#if     (CPU == PPC601)
#error
        /*
         * PPC601 caches are always on. The only way to disable the caches is
         * via the MMU.
         */

        b       frccacheArchOK                     /* return OK */

#elif   (CPU == PPC403)
        /* get type of 403 CPU */

        li      p3, 0                           /* p3 = multiplication factor */        mfspr   p1, PVR                         /* for _CACHE_LINE_NUM */
        rlwinm  p1, p1, 24, 24, 31              /* extract CPU ID PVR[16:23] */
        cmpwi   p1, _PVR_CONF_403GCX
        bne     enableNo403gcx                  /* 8 for 403GCX which has */
        li      p3, 3                           /* cache 8 times larger */


enableNo403gcx:
        cmpwi   p0, _DATA_CACHE                 /* if _DATA_CACHE then */
        beq     cachePpc403DataEnable           /* enable Data Cache */

        mfspr   p1, _PPC403_ICCR                /* move _PPC403_ICCR to p1 */

        lis     p2, HIADJ(ppc403IccrVal)
        lwz     p2, LO(ppc403IccrVal)(p2)       /* load ppc403IccrVal value */
        cmpw    p1, p2                          /* if instr. cache is already */        beq     frccacheArchOK                     /* enabled just return OK */

        /* reset the instruction cache */

        li      p0, 0                           /* clear p0 */
        li      p1, _ICACHE_LINE_NUM            /* load number of cache lines */        slw     p1, p1, p3                      /* adjust with mult. factor */
        mtctr   p1
cachePpc403IInvalidate:
        iccci   r0, p0
        addi    p0, p0, _CACHE_ALIGN_SIZE       /* bump to next line */
        bdnz    cachePpc403IInvalidate          /* go to invalidate */

        /* enable instruction cache */


        mtspr   _PPC403_ICCR, p2                /* load _PPC403_ICCR with p2 */
        isync

        b       frccacheArchOK                     /* return OK */

cachePpc403DataEnable:
        mfspr   p1, _PPC403_DCCR                /* move _PPC403_DCCR to P1 */
        lis     p2, HIADJ(ppc403DccrVal)
        lwz     p2, LO(ppc403DccrVal)(p2)       /* load ppc403DccrVal value */
        cmpw    p1, p2                          /* if data cache is already */
        beq     frccacheArchOK                     /* enabled just return OK */

        /* reset the data cache */

        li      p0, 0                           /* clear p0 */
        li      p1, _DCACHE_LINE_NUM            /* load number of cache lines */        slw     p1, p1, p3                      /* adjust with mult. factor */
        mtctr   p1
cachePpc403DInvalidate:
        dccci   r0, p0
        addi    p0, p0, _CACHE_ALIGN_SIZE       /* bump to next line */
        bdnz    cachePpc403DInvalidate          /* go to invalidate */

        /* enable data cache */

        mtspr   _PPC403_DCCR, p2                /* load _PPC403_DCCR with p2 */
        isync

        b       frccacheArchOK                     /* return OK */

#else   /* CPU == PPC403 */

        mfspr   p1, HID0                        /* move HID0 to P1 */

        /* Select if the cache to enable is the Data or the Instruction cache */
        cmpwi   p0, _DATA_CACHE                 /* if _DATA_CACHE then*/
        beq     frccachePpcDataEnable           /* enable Data Cache */

        /* enable an flush the Instruction cache */

        andi.   p2, p1, _PPC_HID0_ICE         /* if instr. cache is already */
        bne     frccacheArchOK                   /* enabled just return OK */
        ori     p1, p1, _PPC_HID0_ICE | _PPC_HID0_ICFI  /* set ICE & ICFI bit */
#if     ((CPU == PPC603) || (CPU == PPCEC603))
        rlwinm  p2, p1, 0, 21, 19               /* clear the ICFI bit */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        /*
         * The setting of the instruction cache enable (ICE) bit must be
         * preceded by an isync instruction to prevent the cache from being
         * enabled or disabled while an instruction access is in progress.
         * XXX TPR to verify.
         */

        isync                           /* Synchronize for ICE enable */
        mtspr   HID0, p1                /* Enable Instr Cache & Inval cache */
        sync

#if     ((CPU == PPC603) || (CPU == PPCEC603))
        mtspr   HID0, p2                /* using 2 consec instructions */
                                        /* PPC603 recommendation */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        b       frccacheArchOK             /* return OK */

frccachePpcDataEnable:                     /* enable data cache code */

        andi.   p2, p1, _PPC_HID0_DCE   /* if data cache already enabled */
        bne     frccacheArchOK          /* then exit with OK */

        ori     p1, p1, _PPC_HID0_DCE | _PPC_HID0_DCFI  /* set DCE & DCFI bit */
#if     ((CPU == PPC603) || (CPU == PPCEC603))
        rlwinm  p2, p1, 0, 22, 20       /* clear the DCFI bit */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        /*
         * The setting of the data cache enable (DCE) bit must be
         * preceded by an sync instruction to prevent the cache from being
         * enabled or disabled in the middle of a data access.
         */
        sync                            /* Synchronize for DCE enable */
        mtspr   HID0, p1                /* Enable Data Cache & Inval cache */
        sync
#if     ((CPU == PPC603) || (CPU == PPCEC603))
        mtspr   HID0, p2                /* using 2 consecutive instructions */
                                        /* PPC603 recommendation */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        b       frccacheArchOK             /* return OK */

#endif  /* (CPU == PPC601) */

/******************************************************************************
*
* frccacheArchOK - return OK
*
* To save space, several routines exit normally through cacheArchOK().
*
* NOMANUAL
*/
frccacheArchOK:
        sync                    /* SYNC for good measure (multiprocessor?) */
        li      p0,0x0          /* return OK */
        blr


#ifdef PMC280_DUAL_CPU
#define MPC_IMP_BLOCK_SIZE  32
#define MPC_IMP_DCACHE_SIZE 32768

         /*
          * r3 = Pointer to PTE to be updated(address of location to be update)
          * r4 = Pointer to location containing new PTE value.
          */
        .global frcUpdatePTEntry

frcUpdatePTEntry:

        /* Flush the L2 cache */

        isync
        sync
        mfspr   r11,L2CR_REG
        sync

        andis.  r12,r11,0x8000            # If L2 is not on, dont
        beq     fL2end_1          # flush

        ori     r11,r11,0x0800            # Set the HWF bit.

        sync
        mtspr   L2CR_REG,r11
        sync

fl2v_wait_1:
        mfspr   r11,L2CR_REG
        sync
        andi.   r11,r11,0x0800            # L2HWF still set?
        bne     fl2v_wait_1
fL2end_1:
        sync
        isync

        /* Disable the L2 cache */

        sync
        mfspr   r11, L2CR_REG                    /* Read L2 control register */
        andis.  r11, r11, L2CR_DISABLE_MASK_U     /* Disable L2 backside */
        sync

        b  preFetchL2DisableCode_1

codeIsPrefetched_1:
        mtspr   L2CR_REG, r11
        sync
        isync

        b pastFetchL2DisableCode_1

preFetchL2DisableCode_1:
        sync
        isync
        b codeIsPrefetched_1

pastFetchL2DisableCode_1:

        /* Turn off Caches (I + D) */
        mfspr   r6, HID0                /* save HID0 to p1 */

        /* disable instruction cache */

        rlwinm. r7, r6, 0, 16, 16       /* test if cache already disabled */
        beq     __frccacheArchOK        /* if yes, do nothing and return OK */

        ori     r7,r6,0x0800            /* set the ICFI bit */
        rlwinm  r6,r6,0,21,19           /* Turn off the ICFI bit */
        rlwinm  r6,r6,0,17,15           /* Turn off Cache enable bit */

        isync                           /* Synchronize for ICE disable */
        mtspr   HID0,r7                 /* Invalidate Instr Cache using two */
        sync
        mtspr   HID0,r6                 /* consec instructions per manual */
        sync

        sync                            /* ADDED */

        /* disable data cache */
        mfspr   r6, HID0                /* save HID0 to p1 */

        rlwinm. r7,r6,0,17,17           /* test if cache already disabled */
        beq     __frccacheArchOK        /* if yes, do nothing and return OK */

        /* Get the cpuType from the processor version register */

        mfspr   r7,PVR                /* get the Processor Version Register */
        rlwinm  r7,r7,16,16,31        /* and extract the Processor Version */

        cmpwi   r7,4                  /* check if PPC604 */
        beq     frccachePpcDataDisable604

        li      r8,(MPC_IMP_DCACHE_SIZE*2)/MPC_IMP_BLOCK_SIZE   /* 128 index */
        b       loop1

frccachePpcDataDisable604:
        li      r8,(MPC_IMP_DCACHE_SIZE*2)/MPC_IMP_BLOCK_SIZE   /* 256 index */

loop1:
        mtspr   CTR,r8                  /* load CTR with the number of index */

        /*
         * load up p2 with the buffer address minus
         * one cache block size
         */

        lis     r7,HI(buffer-MPC_IMP_BLOCK_SIZE)
        ori     r7,r7,LO(buffer-MPC_IMP_BLOCK_SIZE)

frccachePpcDisableFlush:

        lbzu    r9,MPC_IMP_BLOCK_SIZE(r7)   /* flush the data cache block */
        bdnz    frccachePpcDisableFlush    /* */

        rlwinm   r6,r6,0,18,16          /* Turn off Cache enable bit */

        sync                            /* Synchronize for DCE disable */
        mtspr   HID0,r6
        sync

        sync                            /* ADDED */

        /* Turn off MMU (I + D) */
        mfmsr     r5
        rlwinm    r6, r5, 0, _PPC_MSR_BIT_DR+1, _PPC_MSR_BIT_IR - 1
        mtmsr     r6 
        isync
        sync

        /* Update */
        li      r5, 0x2
        mtspr   CTR, r5                  /* load CTR with the sizeof(PTE)/4 */
__copyloop:
        lwz    r5, 0(r4)
        stw    r5, 0(r3)
        addi   r3, r3, 0x4
        addi   r4, r4, 0x4
        bdnz  __copyloop
        sync

        /* Turn on MMU (I + D) */
        mfmsr   r5
        ori     r5, r5, _PPC_MSR_IR|_PPC_MSR_DR /* set the IR bit of the MSR */
        mtmsr   r5
        isync
        sync

        /* Turn on Caches (I + D) */
        mfspr   r6, HID0                        /* move HID0 to P1 */

        /* enable an flush the Instruction cache */

        andi.   r7, r6, _PPC_HID0_ICE      /* if instr. cache is already */
        bne     __frccacheArchOK           /* enabled just return OK */

        ori     r6, r6, _PPC_HID0_ICE | _PPC_HID0_ICFI  /* set ICE & ICFI bit */
#if     ((CPU == PPC603) || (CPU == PPCEC603))
        rlwinm  r7, r6, 0, 21, 19               /* clear the ICFI bit */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        /*
         * The setting of the instruction cache enable (ICE) bit must be
         * preceded by an isync instruction to prevent the cache from being
         * enabled or disabled while an instruction access is in progress.
         * XXX TPR to verify.
         */

        isync                           /* Synchronize for ICE enable */
        mtspr   HID0, r6                /* Enable Instr Cache & Inval cache */
        sync

#if     ((CPU == PPC603) || (CPU == PPCEC603))
        mtspr   HID0, r7                /* using 2 consec instructions */
                                        /* PPC603 recommendation */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        /* enable data cache code */

        mfspr   r6, HID0                        /* move HID0 to P1 */
        andi.   r7, r6, _PPC_HID0_DCE   /* if data cache already enabled */
        bne    __frccacheArchOK             /* then exit with OK */

        ori     r6, r6, _PPC_HID0_DCE | _PPC_HID0_DCFI  /* set DCE & DCFI bit */
#if     ((CPU == PPC603) || (CPU == PPCEC603))
        rlwinm  r7, r6, 0, 22, 20       /* clear the DCFI bit */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        /*
         * The setting of the data cache enable (DCE) bit must be
         * preceded by an sync instruction to prevent the cache from being
         * enabled or disabled in the middle of a data access.
         */

        sync                            /* Synchronize for DCE enable */
        mtspr   HID0, r6                /* Enable Data Cache & Inval cache */
        sync
#if     ((CPU == PPC603) || (CPU == PPCEC603))
        mtspr   HID0, r7                /* using 2 consecutive instructions */
                                        /* PPC603 recommendation */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        /* Enable L2 Cache */
        sync
        mfspr   r12, L2CR_REG
        oris    r12, r12, L2CR_EN_U
        mtspr   L2CR_REG, r12
        sync

__frccacheArchOK:
        sync                            /* ADDED */
        blr

        /*
         * These are separate snippets of the code above to verify that
         * they work independently.
         */
        .global dbg_update
dbg_update:
        li      r5, 0x10
        mtspr   CTR, r5                  /* load CTR with the sizeof(PTE)/4 */
_copyloop:
        lwz    r5, 0(r4)
        stw    r5, 0(r3)
        addi   r3, r3, 0x4
        addi   r4, r4, 0x4
        bdnz  _copyloop
        sync
        blr

       .global dbg_cache_disable
dbg_cache_disable:

        /* Turn off Caches (I + D) */
        mfspr   r6, HID0                /* save HID0 to p1 */

        /* disable instruction cache */

        rlwinm. r7, r6, 0, 16, 16       /* test if cache already disabled */
        beq     __frccacheArchOK        /* if yes, do nothing and return OK */

        ori     r7,r6,0x0800            /* set the ICFI bit */
        rlwinm  r6,r6,0,21,19           /* Turn off the ICFI bit */
        rlwinm  r6,r6,0,17,15           /* Turn off Cache enable bit */

        isync                           /* Synchronize for ICE disable */
        mtspr   HID0,r7                 /* Invalidate Instr Cache using two */
        sync
        mtspr   HID0,r6                 /* consec instructions per manual */
        sync

        /* disable data cache */
        mfspr   r6, HID0                /* save HID0 to p1 */

        rlwinm. r7,r6,0,17,17           /* test if cache already disabled */
        beq     __frccacheArchOK        /* if yes, do nothing and return OK */

        /* Get the cpuType from the processor version register */

        mfspr   r7,PVR                /* get the Processor Version Register */
        rlwinm  r7,r7,16,16,31        /* and extract the Processor Version */

        cmpwi   r7,4                  /* check if PPC604 */
        beq     _frccachePpcDataDisable604

        li      r8,(MPC_IMP_DCACHE_SIZE*2)/MPC_IMP_BLOCK_SIZE   /* 128 index */
        b       _loop1

_frccachePpcDataDisable604:
        li      r8,(MPC_IMP_DCACHE_SIZE*2)/MPC_IMP_BLOCK_SIZE   /* 256 index */

_loop1:
        mtspr   CTR,r8                  /* load CTR with the number of index */

        /*
         * load up p2 with the buffer address minus
         * one cache block size
         */

        lis     r7,HI(buffer-MPC_IMP_BLOCK_SIZE)
        ori     r7,r7,LO(buffer-MPC_IMP_BLOCK_SIZE)

_frccachePpcDisableFlush:

        lbzu    r9,MPC_IMP_BLOCK_SIZE(r7)   /* flush the data cache block */
        bdnz    _frccachePpcDisableFlush    /* */

        rlwinm   r6,r6,0,18,16          /* Turn off Cache enable bit */

        sync                            /* Synchronize for DCE disable */
        mtspr   HID0,r6
        sync
        blr

       .global dbg_mmu_disable
dbg_mmu_disable:
        /* Turn off MMU (I + D) */
        mfmsr     r5
        rlwinm    r6, r5, 0, _PPC_MSR_BIT_DR+1, _PPC_MSR_BIT_IR - 1
        mtmsr     r6 
        isync
        sync
        blr

       .global dbg_mmu_enable
dbg_mmu_enable:
        /* Turn on MMU (I + D) */
        mfmsr   r5
        ori     r5, r5, _PPC_MSR_IR|_PPC_MSR_DR /* set the IR bit of the MSR */
        mtmsr   r5
        isync
        blr

       .global dbg_cache_enable
dbg_cache_enable:

        /* Turn on Caches (I + D) */
        mfspr   r6, HID0                        /* move HID0 to P1 */

        /* enable an flush the Instruction cache */

        andi.   r7, r6, _PPC_HID0_ICE      /* if instr. cache is already */
        bne     ___frccacheArchOK           /* enabled just return OK */

        ori     r6, r6, _PPC_HID0_ICE | _PPC_HID0_ICFI  /* set ICE & ICFI bit */
#if     ((CPU == PPC603) || (CPU == PPCEC603))
        rlwinm  r7, r6, 0, 21, 19               /* clear the ICFI bit */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        /*
         * The setting of the instruction cache enable (ICE) bit must be
         * preceded by an isync instruction to prevent the cache from being
         * enabled or disabled while an instruction access is in progress.
         * XXX TPR to verify.
         */

        isync                           /* Synchronize for ICE enable */
        mtspr   HID0, r6                /* Enable Instr Cache & Inval cache */
        sync

#if     ((CPU == PPC603) || (CPU == PPCEC603))
        mtspr   HID0, r7                /* using 2 consec instructions */
                                        /* PPC603 recommendation */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        /* enable data cache code */

        mfspr   r6, HID0                        /* move HID0 to P1 */
        andi.   r7, r6, _PPC_HID0_DCE   /* if data cache already enabled */
        bne    ___frccacheArchOK             /* then exit with OK */

        ori     r6, r6, _PPC_HID0_DCE | _PPC_HID0_DCFI  /* set DCE & DCFI bit */
#if     ((CPU == PPC603) || (CPU == PPCEC603))
        rlwinm  r7, r6, 0, 22, 20       /* clear the DCFI bit */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

        /*
         * The setting of the data cache enable (DCE) bit must be
         * preceded by an sync instruction to prevent the cache from being
         * enabled or disabled in the middle of a data access.
         */

        sync                            /* Synchronize for DCE enable */
        mtspr   HID0, r6                /* Enable Data Cache & Inval cache */
        sync
#if     ((CPU == PPC603) || (CPU == PPCEC603))
        mtspr   HID0, r7                /* using 2 consecutive instructions */
                                        /* PPC603 recommendation */
#endif  /* ((CPU  == PPC603) || (CPU == PPCEC603)) */

___frccacheArchOK:
        blr

#endif /* PMC280_DUAL_CPU */

#if defined(INCLUDE_L2_CACHE)
#include "sysL2BackCache.s"
#endif
