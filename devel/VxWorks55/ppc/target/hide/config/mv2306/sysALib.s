/* sysALib.s - Motorola MVME2600 system-dependent assembly routines */

/* Copyright 1984-2000 Wind River Systems, Inc. */
/* Copyright 1996-1999 Motorola, Inc. All Rights Reserved */
        .data
	.globl	copyright_wind_river
	.long	copyright_wind_river

/*
modification history
--------------------
01u,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01t,21jan00,dat  fixed assembler warning
01s,16feb99,mas  Added Raven3, MPC750, HID0 support (SPR 24453).
01r,07aug98,tb   added support for VMEbus DMA
01q,16apr98,dat  moved sysHid1Get() from sysLib.c to here.
01p,14apr98,ms_  merged Motorola mv2700 support
01o,22jan98,rhk  SPR 20104, correct use of HI and HIADJ macros.
01n,17dec97,srr  modified sysInByte to read value into r3.
01m,16oct97,scb  added routines to get L2CR, HID1 and put L2CR.
01l,08oct97,srr  removed second disabling of GLANCE.
01k,05nov97,mas  added eieio/sync pair to sysMemProbeSup() (SPR 9717).
01j,25jul97,srr/ added 604r (Mach 5) support (SPR 8911).
            mas
01i,14jul97,mas  added sysMemProbeSup(), sysIn16(), sysOut16(), sysIn32(),
		 sysOut32() (SPR 8022).
01h,29may97,srr  Duplicate the GLANCE, RAVEN, and SIO code in sysALib.s as in
		 romInit.s. Chg the RAVEN #defines to support vxMemProbe and
		 replace the hard coded values with #defines.  Replace the
		 isync instructions with eieio in sysPci... routines.
		 (MCG MR #67, 69, 74). SPRs 8289, 8560.
01g,06may97,mas  added extended VME support: 0xfef8 -> FALCON_BASE_UPPER_ADRS
		 (SPR 8410).
01f,24apr97,mas  added Moto support for MPIC: sysPciRead32, sysPciWrite32
		 (SPR 8170).
01e,11apr97,mas  added sysPciInByte, sysPciOutByte, sysPciInWord, sysPciOutWord,
                 sysPciInLong, sysPciOutLong; removed doPciCSA, pciIoSync
                 (SPR 8226).
01d,19feb97,mas  in sysInit changed BOOT_COLD to BOOT_WARM_AUTOBOOT (SPR 8024).
01c,02jan97,dat  documentation, mod history fix
01b,17dec96,mas  moved sysPciIo.s contents here (SPR 7525).
01a,01sep96,mot  written (mv1603/sysALib.s ver 01k)
*/

/*
DESCRIPTION
This module contains system-dependent routines written in assembly
language.

This module must be the first specified in the \f3ld\f1 command used to
build the system.  The sysInit() routine is the system start-up code.
*/

#define _ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "regs.h"	
#include "asm.h"

	/* globals */

	FUNC_EXPORT(_sysInit)		/* start of system code */
	FUNC_EXPORT(sysInByte)
	FUNC_EXPORT(sysOutByte)
	FUNC_EXPORT(sysIn16)
	FUNC_EXPORT(sysOut16)
	FUNC_EXPORT(sysIn32)
	FUNC_EXPORT(sysOut32)
        FUNC_EXPORT(sysPciRead32)
        FUNC_EXPORT(sysPciWrite32)
        FUNC_EXPORT(sysPciInByte)
        FUNC_EXPORT(sysPciOutByte)
        FUNC_EXPORT(sysPciInWord)
        FUNC_EXPORT(sysPciOutWord)
        FUNC_EXPORT(sysPciInLong)
        FUNC_EXPORT(sysPciOutLong)
        FUNC_EXPORT(sysMemProbeSup)
        FUNC_EXPORT(sysProbeExc)
	FUNC_EXPORT(sysL2crPut)
	FUNC_EXPORT(sysL2crGet)
	FUNC_EXPORT(sysTimeBaseLGet)
	FUNC_EXPORT(sysHid1Get)


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
* RETURNS: N/A

* sysInit (void)              /@ THIS IS NOT A CALLABLE ROUTINE @/

*/

_sysInit:

	/* disable external interrupts */

	xor	p0, p0, p0
        mtmsr   p0                      /* clear the MSR register  */

        /* Zero-out registers: r0 & SPRGs */

        xor     r0,r0,r0
        mtspr   272,r0
        mtspr   273,r0
        mtspr   274,r0
        mtspr   275,r0

        /*
         *      Set MPU/MSR to a known state
         *      Turn on FP
         */

        andi.   r3, r3, 0
        ori     r3, r3, 0x2000
        sync
        mtmsr   r3
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

        /* Initialize the floating point data registers to a known state */

        bl      ifpdrValue
        .long   0x3f800000      /* 1.0 */
ifpdrValue:
        mfspr   r3,8
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

        /*
         *      Set MPU/MSR to a known state
         *      Turn off FP
         */

        andi.   r3, r3, 0
        sync
        mtmsr   r3
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

#ifndef MV2300
        /* Turn off the GLANCE - L2 Cache */

        lis     r3, HI(MV2600_SXCCR_A)
        ori     r3, r3, (MV2600_SXCCR_A & 0xFFFF) /* Hack to remove warning */
	addis	r4, r0, 0x0
	ori	r4, r4, 0x0070
	stb	r4, 0x0(r3)
#endif  /* MV2300 */

        /* return from decrementer exceptions */

        addis   p1, r0, 0x4c00
        addi    p1, p1, 0x0064          /* load rfi (0x4c000064) to p1      */
        stw     p1, 0x900(r0)           /* store rfi at 0x00000900          */

	/* initialize the stack pointer */
	
	lis     sp, HIADJ(RAM_LOW_ADRS)
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

        mfspr   r28, PVR
        rlwinm  r28, r28, 16, 16, 31

        /* invalidate the MPU's data/instruction caches */

        lis     r3, 0x0
        cmpli   0, 0, r28, CPU_TYPE_750
        beq     cpuIs750
        cmpli   0, 0, r28, CPU_TYPE_603
        beq     cpuIs603
        cmpli   0, 0, r28, CPU_TYPE_603E
        beq     cpuIs603
        cmpli   0, 0, r28, CPU_TYPE_603P
        beq     cpuIs603
        cmpli   0, 0, r28, CPU_TYPE_604R
        bne     cpuNot604R

cpuIs604R:
        lis     r3, 0x0
        mtspr   HID0, r3        /* disable the caches */
        isync
        ori     r4, r4, 0x0002  /* disable BTAC by setting bit 30 */

cpuNot604R:
        ori     r3, r3, 0x0C00          /* r3 has invalidate bits set */
cpuIs603:
        ori     r3, r3, 0xC000          /* r3 has enable and bits set */
        or      r4, r4, r3              /* set bits */
        sync
        isync
        mtspr   HID0, r4                /* HID0 = r4 */
        andc    r4, r4, r3              /* clear bits */
        isync
        cmpli   0, 0, r28, CPU_TYPE_604
        beq     cpuIs604
        cmpli   0, 0, r28, CPU_TYPE_604E
        beq     cpuIs604
        cmpli   0, 0, r28, CPU_TYPE_604R
        beq     cpuIs604
        cmpli   0, 0, r28, CPU_TYPE_750
        beq     cpuIs604
        mtspr   HID0, r4
        isync

#ifdef USER_I_CACHE_ENABLE
        b       iCacheOn603
#else
        b       cacheEnableDone
#endif

cpuIs750:
#ifdef USER_I_CACHE_ENABLE
        mfspr   r3,HID0
        sync
        addi    r4,r0,0x0800
        or      r3,r4,r3
        mtspr   HID0,r3         /* set ICFI (bit 16) */
        sync

        andc    r3,r3,r4
        mtspr   HID0,r3         /* clear ICFI (bit 16) */
        sync

        addi    r4,r0,0xFFFFDFFF    /* Clear ILOCK (bit 18) */
        and     r3,r3,r4
        mtspr   HID0,r3
        sync

        ori     r3,r3,0x8000    /* Set ICE (bit 16) */
        mtspr   HID0,r3
        sync
#endif
        b       cacheEnableDone

cpuIs604:
        lis     r5, 0x0
        ori     r5, r5, 0x1000
        mtspr   CTR, r5
loopDelay:
        nop
        bdnz    loopDelay
        isync
        mtspr   HID0, r4
        isync

        /* turn the Instruction cache ON */

#ifdef USER_I_CACHE_ENABLE
        
        ori     r4, r4, 0x8800          /* set ICE & ICFI bit */
        isync                           /* Synchronize for ICE enable */
        b       writeR4
iCacheOn603:
        ori     r4, r4, 0x8800          /* set ICE & ICFI bit */
        rlwinm  r3, r4, 0, 21, 19       /* clear the ICFI bit */

        /*
         * The setting of the instruction cache enable (ICE) bit must be
         * preceded by an isync instruction to prevent the cache from being
         * enabled or disabled while an instruction access is in progress.
         */
        isync
writeR4:
        mtspr   HID0, r4                /* Enable Instr Cache & Inval cache */
        cmpli   0, 0, r28, CPU_TYPE_604
        beq     cacheEnableDone
        cmpli   0, 0, r28, CPU_TYPE_604E
        beq     cacheEnableDone
        cmpli   0, 0, r28, CPU_TYPE_604R
        beq     cacheEnableDone
        cmpli   0, 0, r28, CPU_TYPE_750
        beq     cacheEnableDone

        mtspr   HID0, r3                /* using 2 consec instructions */
                                        /* PPC603 recommendation */
#endif
cacheEnableDone:

        /* Enhance execution based on cpu type */

        cmpli   0, 0, r28, CPU_TYPE_603
        beq     raven3
        cmpli   0, 0, r28, CPU_TYPE_603E
        beq     raven3
        cmpli   0, 0, r28, CPU_TYPE_603P
        beq     raven3

        /* enable branch history table for the 604 and 750 */

        mfspr   r3, HID0
        ori     r3, r3, _PPC_HID0_BHTE
        cmpli   0, 0, r28, CPU_TYPE_750
        beq     enhanceAll

        /*
         * CPU is not 750 or 603x so it must be a 604x.
         * Disable sequential instruction execution (go superscalar) and
         * enable branch history table for the 604.
         */

        ori     r3, r3, _PPC_HID0_SIED

enhanceAll:
        mtspr   HID0, r3

raven3:

/* 
 * Disable Raven3's Watchdog Timers.
 *
 * Note: Both of Raven3's Watchdog timers must be disabled at powerup.
 * Otherwise Watchdog Timer 1 will time out in 512 msec and interrupt the 
 * board, Watchdog Timer 2 will time out in 576 msec and reset the board.
 */
        lis     r3,HI(RAVEN_BASE_ADRS)
        ori     r3, r3, LO(RAVEN_BASE_ADRS)

        isync                                   /* synchronize */

        lbz     r4,RAVEN_MPC_REVID(r3)          /* read REVID register */
        eieio                                   /* synchronize */
        sync                                    /* synchronize */
        cmpli   0,0,r4,0x3                      /* Raven version 3 or above? */
        bc      12,0,nodiswdog                  /* skip if Raven2 or lower */

        addis   r4,r0,0x0000                    /* disable Watchdog Timers */
        ori     r4,r4,0x0055                    /* load PATTERN_1 */
        isync                                   /* synchronize */
        stb     r4, RAVEN_MPC_WDT1CNTL(r3)      /* arm Watchdog Timer 1 */
        eieio                                   /* synchronize */
        sync                                    /* synchronize */

        addis   r4,r0,0x0000                    /* load PATTERN_2 */
        ori     r4,r4,0xaa0f                    /* max resolution */
        isync                                   /* synchronize */
        sth     r4, RAVEN_MPC_WDT1CNTL(r3)      /* disable Timer 1 */
        eieio                                   /* synchronize */
        sync                                    /* synchronize */

        addis   r4,r0,0x0000                    /* load PATTERN_1 */
        ori     r4,r4,0x0055
        isync                                   /* synchronize */
        stb     r4,RAVEN_MPC_WDT2CNTL(r3)       /* arm Watchdog Timer 2 */
        eieio                                   /* synchronize */
        sync                                    /* synchronize */

        addis   r4,r0,0x0000                    /* load PATTERN_2 */
        ori     r4,r4,0xaa0f                    /* max resolution */
        isync                                   /* synchronize */
        sth     r4,RAVEN_MPC_WDT2CNTL(r3)       /* disable Timer2 */
        eieio                                   /* synchronize */
        sync                                    /* synchronize */

nodiswdog:

	/* disable instruction and data translations in the MMU */

	sync
	mfmsr	r3			/* get the value in msr *
					/* clear bits IR and DR */
	
	rlwinm	r4, r3, 0, _PPC_MSR_BIT_DR+1, _PPC_MSR_BIT_IR - 1
	
	mtmsr	r4			/* set the msr */
	sync				/* SYNC */

	/* initialize the BAT register */

	li	p3,0	 		/* clear p0 */
	
	isync
	mtspr	IBAT0U,p3		/* SPR 528 (IBAT0U) */
	isync

	mtspr	IBAT0L,p3		/* SPR 529 (IBAT0L) */
	isync

	mtspr	IBAT1U,p3		/* SPR 530 (IBAT1U) */
	isync

	mtspr	IBAT1L,p3		/* SPR 531 (IBAT1L) */
	isync

	mtspr	IBAT2U,p3		/* SPR 532 (IBAT2U) */
	isync

	mtspr	IBAT2L,p3		/* SPR 533 (IBAT2L) */
	isync

	mtspr	IBAT3U,p3		/* SPR 534 (IBAT3U) */
	isync

	mtspr	IBAT3L,p3		/* SPR 535 (IBAT3L) */
	isync

	mtspr	DBAT0U,p3		/* SPR 536 (DBAT0U) */
	isync

	mtspr	DBAT0L,p3		/* SPR 537 (DBAT0L) */
	isync

	mtspr	DBAT1U,p3		/* SPR 538 (DBAT1U) */
	isync

	mtspr	DBAT1L,p3		/* SPR 539 (DBAT1L) */
	isync

	mtspr	DBAT2U,p3		/* SPR 540 (DBAT2U) */
	isync

	mtspr	DBAT2L,p3		/* SPR 541 (DBAT2L) */
	isync

	mtspr	DBAT3U,p3		/* SPR 542 (DBAT3U) */
	isync

	mtspr	DBAT3L,p3		/* SPR 543 (DBAT3L) */
	isync


	/*
	This following contains the entry code for the initialization code
	for the Raven, a Host PCI Bridge/Memory Controller used in
	Motorola's PowerPC based boards.
	*/

/*
#	Initialize the RAVEN MPC registers.
#	notes:
#               1. For the standard vxWorks configuration the  MPC to
#                  PCI mapping registers are
#                  initialized to the PReP model with some additions:
#
#	MPC Address Range       PCI Address Range       Definition
#       -----------------       -----------------       ------------------------
#	80000000 BF7FFFFF	00000000 3F7FFFFF	ISA/PCI I/O space
#	C0000000 FCFFFFFF	00000000 3CFFFFFF	ISA/PCI Mem space w/MPIC
#	FD000000 FDFFFFFF	00000000 00FFFFFF	ISA/PCI Mem space
#	FE000000 FE7FFFFF	00000000 007FFFFF	ISA/PCI I/O space
#
#		2. This assignments do not include the entire PReP PCI
#		   address space, this is due to the conflicting local
#		   resources of the H/W.
#
#               3. When EXTENDED_VME is defined the mapping is as follows:
#
#       MPC Address Range           PCI Address Range      Definition
# --------------------------- ---------------------------  ---------------------
# VME_A32_MSTR_LOCAL FBFFFFFF VME_A32_MSTR_LOCAL FBFFFFFF  VME address space
#       FC000000 FCFFFFFF           FC000000 FCFFFFFF      MPIC/Reg space
#       FD000000 FDFFFFFF           FD000000 FDFFFFFF      ISA/PCI Memory space
#       FE000000 FE7FFFFF           00000000 007FFFFF      ISA/PCI I/O space
#
*/
	lis	r3, HI(RAVEN_BASE_ADRS)
	ori	r3, r3, LO(RAVEN_BASE_ADRS)

	addis	r4,r0,0x0000		/* GCSR:FLBRD=0,MBT=256us */
	ori	r4,r4,0x0000		/* GCSR:FLBRD=0,MBT=256us */
	sth	r4,RAVEN_MPC_GCSR(r3)	/* write GCSR */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MARB:BRENx=1,PKEN=0,PKMD=0,DEFMx=1 */
	ori	r4,r4,0x0703		/* MARB:BRENx=1,PKEN=0,PKMD=0,DEFMx=1 */
	sth	r4,RAVEN_MPC_MARB(r3)	/* write MARB */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* PADJ:BE=66MHZ */
	ori	r4,r4,0x00BE		/* PADJ:BE=66MHZ */
	stb	r4,RAVEN_MPC_PADJ(r3)	/* write PADJ */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MEREN:(all error enables disabled) */
	ori	r4,r4,0x0000		/* MEREN:(all error enables disabled) */
	sth	r4,RAVEN_MPC_MEREN(r3)	/* write MEREN */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MERST:(clear all error status) */
	ori	r4,r4,0x00ff		/* MERST:(clear all error status) */
	stb	r4,RAVEN_MPC_MERST(r3)	/* write MERST */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MSATTx:(all attributes disabled) */
	ori	r4,r4,0x0000		/* MSATTx:(all attributes disabled) */
	stb	r4,RAVEN_MPC_MSATT0(r3)	/* write MSATT0 */
	eieio				/* synchronize */
	sync				/* synchronize */
	stb	r4,RAVEN_MPC_MSATT1(r3)	/* write MSATT1 */
	eieio				/* synchronize */
	sync				/* synchronize */
	stb	r4,RAVEN_MPC_MSATT2(r3)	/* write MSATT2 */
	eieio				/* synchronize */
	sync				/* synchronize */
	stb	r4,RAVEN_MPC_MSATT3(r3)	/* write MSATT3 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,CPU2PCI_ADDR0_START	/* MSADD0: */
	ori	r4,r4,CPU2PCI_ADDR0_END		/* MSADD0: */
	stw	r4,RAVEN_MPC_MSADD0(r3)	/* write MSADD0 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MSOFF0: */
	ori	r4,r4,CPU2PCI_OFFSET0	/* MSOFF0: */
	sth	r4,RAVEN_MPC_MSOFF0(r3)	/* write MSOFF0 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MSATT0: */
	ori	r4,r4,CPU2PCI_MSATT0	/* MSATT0: */
	stb	r4,RAVEN_MPC_MSATT0(r3)	/* write MSATT0 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,CPU2PCI_ADDR1_START	/* MSADD1: */
	ori	r4,r4,CPU2PCI_ADDR1_END		/* MSADD1: */
	stw	r4,RAVEN_MPC_MSADD1(r3)	/* write MSADD1 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MSOFF1: */
	ori	r4,r4,CPU2PCI_OFFSET1	/* MSOFF1: */
	sth	r4,RAVEN_MPC_MSOFF1(r3)	/* write MSOFF1 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MSATT1: */
	ori	r4,r4,CPU2PCI_MSATT1	/* MSATT1: */
	stb	r4,RAVEN_MPC_MSATT1(r3)	/* write MSATT1 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,CPU2PCI_ADDR2_START	/* MSADD2: */
	ori	r4,r4,CPU2PCI_ADDR2_END		/* MSADD2: */
	stw	r4,RAVEN_MPC_MSADD2(r3)	/* write MSADD2 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MSOFF2: */
	ori	r4,r4,CPU2PCI_OFFSET2	/* MSOFF2: */
	sth	r4,RAVEN_MPC_MSOFF2(r3)	/* write MSOFF2 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MSATT2: */
	ori	r4,r4,CPU2PCI_MSATT2	/* MSATT2: */
	stb	r4,RAVEN_MPC_MSATT2(r3)	/* write MSATT2 */
	eieio				/* synchronize */
	sync				/* synchronize */

/*
 * PCI address space 3 registers supports config. space access and
 * special cycle generation.  It should be configured for I/O space.
 */

	addis	r4,r0,CPU2PCI_ADDR3_START	/* MSADD3: */
	ori	r4,r4,CPU2PCI_ADDR3_END		/* MSADD3: */
	stw	r4,RAVEN_MPC_MSADD3(r3)	/* write MSADD3 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MSOFF3: */
	ori	r4,r4,CPU2PCI_OFFSET3	/* MSOFF3: */
	sth	r4,RAVEN_MPC_MSOFF3(r3)	/* write MSOFF3 */
	eieio				/* synchronize */
	sync				/* synchronize */

	addis	r4,r0,0x0000		/* MSATT3: */
	ori	r4,r4,CPU2PCI_MSATT3	/* MSATT3: */
	stb	r4,RAVEN_MPC_MSATT3(r3)	/* write MSATT3 */
	eieio				/* synchronize */
	sync				/* synchronize */

/* End of Raven Init */

/* 
 * Do the init for the SIO, now that the Raven is configured.
 * This was moved after the Raven init for the Extended VME config.
 */

#ifndef MV2300
        bl .sioInit
#endif  /* MV2300 */

	/* invalidate entries within both TLBs */

	li	p1,128
	xor	p0,p0,p0		/* p0 = 0    */
	mtctr	p1			/* CTR = 32  */

	isync				/* context sync req'd before tlbie */
sysALoop:
	tlbie	p0
	addi	p0,p0,0x1000		/* increment bits 15-19 */
	bdnz	sysALoop		/* decrement CTR, branch if CTR != 0 */
	sync				/* sync instr req'd after tlbie      */

/* workaround for Extended VME config. with 2603 boards */

#ifdef EXTENDED_VME
        lis     r3, HI(RAVEN_BASE_ADRS)
        ori     r3, r3, LO(RAVEN_BASE_ADRS)
        addis   r4,r0,(VME_A32_MSTR_LOCAL>>16)  /* MSADD0: */
        ori     r4,r4,CPU2PCI_ADDR0_END         /* MSADD0: */
        stw     r4,RAVEN_MPC_MSADD0(r3)     /* write MSADD0 */
        eieio                           /* synchronize */
        sync                            /* synchronize */
#endif

	/* initialize Small Data Area (SDA) start address */

#if	FALSE				/* XXX TPR NO SDA for now */
	lis     r2, HIADJ(_SDA2_BASE_)
	addi    r2, r2, LO(_SDA2_BASE_)

	lis     r13, HIADJ(_SDA_BASE_)
	addi    r13, r13, LO(_SDA_BASE_)
#endif

	addi	sp, sp, -FRAMEBASESZ	/* get frame stack */
	li      r3, BOOT_WARM_AUTOBOOT
	b	usrInit			/* never returns - starts up kernel */


/*****************************************************************************
*
* sysInByte - reads a byte from an address.
*
* This function reads a byte from a specified address.
*
* From a C programmers point of view, the routine is defined as follows:
*
*         sysInByte
*             (
*             ULONG       *addr;  - address of data
*             )
*
* INPUTS:
* r3      = address to read data from
*
* OUTPUTS:
* r3      = data
*
* RETURNS: byte from address.
*/

sysInByte:

	/*
	 *	Read byte from given address
	 */
	lbzx	r3,r0,r3
	/*
	 *	Sync I/O operation
	 */
	eieio
	/*
	 *	Return to caller
	 */
	bclr	20,0

/******************************************************************************
*
* sysOutByte - writes a byte to an address.
*
* This function writes a byte to a specified address.
*
* From a C point of view, the routine is defined as follows:
*
*         sysOutByte
*             (
*             ULONG      *addr  - address to write data to
*             UCHAR       data  - 8-bit data
*             )
*
* INPUTS:
* r3      = address to write to
* r4      = data to be written
*
* RETURNS: N/A
*/

sysOutByte:

	/*
	 *	Write a byte to given address
	 */
	stbx	r4,r0,r3
	/*
	 *	Sync I/O operation
	 */
	eieio
	/*
	 *	Return to caller
	 */
	bclr	20,0

/*****************************************************************************
*
* sysIn16 - reads a 16-bit unsigned value from an address.
*
* This function reads a 16-bit unsigned value from a specified address.
*
* From a C programmers point of view, the routine is defined as follows:
*
*         sysIn16
*             (
*             UINT16       *addr;  - address of data
*             )
*
* INPUTS:
* r3      = address to read data from
*
* OUTPUTS:
* r3      = data
*
* RETURNS: 16-bit unsigned value from address.
*/

sysIn16:

	lhz	3,0(3)
	eieio
	bclr	20,0

/******************************************************************************
*
* sysOut16 - writes a 16-bit unsigned value to an address.
*
* This function writes a 16-bit unsigned value to a specified address.
*
* From a C point of view, the routine is defined as follows:
*
*         sysOut16
*             (
*             UINT16      *addr  - address to write data to
*             UINT16       data  - 8-bit data
*             )
*
* INPUTS:
* r3      = address to write to
* r4      = data to be written
*
* RETURNS: N/A
*/

sysOut16:

	sth	4,0(3)
	eieio
	bclr	20,0

/*****************************************************************************
*
* sysIn32 - reads a 32-bit unsigned value from an address.
*
* This function reads a 32-bit unsigned value from a specified address.
*
* From a C programmers point of view, the routine is defined as follows:
*
*         sysIn32
*             (
*             UINT32       *addr;  - address of data
*             )
*
* INPUTS:
* r3      = address to read data from
*
* OUTPUTS:
* r3      = data
*
* RETURNS: 32-bit unsigned value from address.
*/

sysIn32:

	lwz	3,0(3)
	eieio
	bclr	20,0

/******************************************************************************
*
* sysOut32 - writes a 32-bit unsigned value to an address.
*
* This function writes a 32-bit unsigned value to a specified address.
*
* From a C point of view, the routine is defined as follows:
*
*         sysOut32
*             (
*             UINT32      *addr  - address to write data to
*             UINT32       data  - 32-bit data
*             )
*
* INPUTS:
* r3      = address to write to
* r4      = data to be written
*
* RETURNS: N/A
*/

sysOut32:

	stw	4,0(3)
	eieio
	bclr	20,0


/******************************************************************************
*
* sysPciRead32 - read 32 bit PCI data
*
* This routine will read a 32-bit data item from PCI ( I/O or
* memory ) space.  From a C programmers point of view, the routine
* is defined as follows:
*
*         sysPciRead32
*             (
*             UINT32      *addr;  - address of data in PCI space
*             UINT32      *pdata  - pointer to data being returned
*             )                     by the read call ( data is converted
*                                   to big-endian )
*
* INPUTS:
* r3      = PCI address to read data from
* r4      = pointer to store data to
*
* RETURNS: N/A
*/

sysPciRead32:
        lwbrx   r3,r0,r3        /* get the data and swap the bytes */
	/*
	 *	Sync I/O operation
	 */
	eieio
        stw     r3,0(r4)        /* store into address ptd. to by r4 */
        bclr    20,0


/******************************************************************************
*
* sysPciWrite32 - write a 32 bit data item to PCI space
*
* This routine will store a 32-bit data item ( input as big-endian )
* into PCI ( I/O or memory ) space in little-endian mode.  From a
* C point of view, the routine is defined as follows:
*
*         sysPciWrite32
*             (
*             UINT32      *addr  - address to write data to
*             UINT32       data  - 32-bit big-endian data
*             )
*
* INPUTS:
* r3      = PCI address to write to
* r4      = data to be written
*
* RETURNS: N/A
*/

sysPciWrite32:
        stwbrx  r4,r0,r3        /* store data as little-endian */
	/*
	 *	Sync I/O operation
	 */
	eieio
        bclr    20,0


/*****************************************************************************
*
* sysPciInByte - reads a byte from PCI Config Space.
*
* This function reads a byte from a specified PCI Config Space address.
*
* ARGUMENTS:
*       r3      = Config Space address
*
* RETURNS:
*       r3      = byte from address.
*/

sysPciInByte:

        /*
         *      Read byte from PCI space
         */
        lbzx    r3,r0,r3
        /*
         *      Sync I/O operation
         */
	eieio
        /*
         *      Return to caller
         */
        bclr    20,0

/*****************************************************************************
*
* sysPciInWord - reads a word (16-bit big-endian) from PCI Config Space.
*
* This function reads a word from a specified PCI Config Space (little-endian)
* address.
*
* ARGUMENTS:
*       r3      = Config Space address
*
* RETURNS:
*       r3      = word (16-bit big-endian) from address.
*/

sysPciInWord:

        /*
         *      Read big-endian word from little-endian PCI space
         */
        lhbrx   r3,r0,r3
        /*
         *      Sync I/O operation
         */
	eieio
        /*
         *      Return to caller
         */
        bclr    20,0

/*****************************************************************************
*
* sysPciInLong - reads a long (32-bit big-endian) from PCI Config Space.
*
* This function reads a long from a specified PCI Config Space (little-endian)
* address.
*
* ARGUMENTS:
*       r3      = Config Space address
*
* RETURNS:
*       r3      = long (32-bit big-endian) from address.
*/

sysPciInLong:

        /*
         *      Read big-endian long from little-endian PCI space
         */
        lwbrx   r3,r0,r3
        /*
         *      Sync I/O operation
         */
	eieio
        /*
         *      Return to caller
         */
        bclr    20,0

/******************************************************************************
*
* sysPciOutByte - writes a byte to PCI Config Space.
*
* This function writes a byte to a specified PCI Config Space address.
*
* ARGUMENTS:
*       r3      = Config Space address
*       r4      = byte to write
*
* RETURNS: N/A
*/

sysPciOutByte:

        /*
         *      Write a byte to PCI space
         */
        stbx    r4,r0,r3
        /*
         *      Sync I/O operation
         */
	eieio
        /*
         *      Return to caller
         */
        bclr    20,0

/******************************************************************************
*
* sysPciOutWord - writes a word (16-bit big-endian) to PCI Config Space.
*
* This function writes a word to a specified PCI Config Space (little-endian)
* address.
*
* ARGUMENTS:
*       r3      = Config Space address
*       r4      = word (16-bit big-endian) to write
*
* RETURNS: N/A
*/

sysPciOutWord:

        /*
         *      Write a big-endian word to little-endian PCI space
         */
        sthbrx  r4,r0,r3
        /*
         *      Sync I/O operation
         */
	eieio
        /*
         *      Return to caller
         */
        bclr    20,0

/******************************************************************************
*
* sysPciOutLong - writes a long (32-bit big-endian) to PCI Config Space.
*
* This function writes a long to a specified PCI Config Space (little-endian)
* address.
*
* ARGUMENTS:
*       r3      = Config Space address
*       r4      = long (32-bit big-endian) to write
*
* RETURNS: N/A
*/

sysPciOutLong:

        /*
         *      Write a big-endian long to little-endian PCI space
         */
        stwbrx  r4,r0,r3
        /*
         *      Sync I/O operation
         */
        mr      r3,r4
	eieio
        /*
         *      Return to caller
         */
        bclr    20,0

/*******************************************************************************
*
* sysMemProbeSup - sysBusProbe support routine
*
* This routine is called to try to read byte, word, or long, as specified
* by length, from the specified source to the specified destination.
*
* RETURNS: OK if successful probe, else ERROR

STATUS sysMemProbeSup (length, src, dest)
    (
    int         length, // length of cell to test (1, 2, 4, 8, 16) *
    char *      src,    // address to read *
    char *      dest    // address to write *
    )

*/

sysMemProbeSup:
        addi    p7, p0, 0       /* save length to p7 */
        xor     p0, p0, p0      /* set return status */
        cmpwi   p7, 1           /* check for byte access */
        bne     sbpShort        /* no, go check for short word access */
        lbz     p6, 0(p1)       /* load byte from source */
        eieio
        sync
        stb     p6, 0(p2)       /* store byte to destination */
        eieio
        sync
        isync                   /* enforce for immediate exception handling */
        blr
sbpShort:
        cmpwi   p7, 2           /* check for short word access */
        bne     sbpWord         /* no, check for word access */
        lhz     p6, 0(p1)       /* load half word from source */
        eieio
        sync
        sth     p6, 0(p2)       /* store half word to destination */
        eieio
        sync
        isync                   /* enforce for immediate exception handling */
        blr
sbpWord:
        cmpwi   p7, 4           /* check for short word access */
        bne     sysProbeExc     /* no, check for double word access */
        lwz     p6, 0(p1)       /* load half word from source */
        eieio
        sync
        stw     p6, 0(p2)       /* store half word to destination */
        eieio
        sync
        isync                   /* enforce for immediate exception handling */
        blr
sysProbeExc:
        li      p0, -1          /* shouldn't ever get here, but... */
        blr

#ifndef MV2300

	.set	SIO_LUNINDEX,0x07	/* SIO LUN index register */
	.set	SIO_CNFG1,0x21		/* SIO configuration #1 register */
	.set	SIO_CNFG2,0x22		/* SIO configuration #2 register */
	.set	SIO_PCSCI,0x23		/* SIO PCS configuration index reg */
	.set	SIO_PCSCD,0x24		/* SIO PCS configuration data reg */
	.set	SIO_SID,0x08		/* SIO identifier register */
	.set	SIO_SIDPNP,0x20		/* SIO identifier register - PnP */
	.set	SIO_SIDMASK,0xF8	/* SIO identifier mask */
	.set	SIO_SID87303,0x30	/* SIO identifier - 87303 */
	.set	SIO_SID87323,0x20	/* SIO identifier - 87323 */
	.set	SIO_SID87308,0xA0	/* SIO identifier - 87308 */
	.set	SIO_ACTIVATE,0x30	/* SIO activate register */
	.set	SIO_IOBASEHI,0x60	/* SIO I/O port base address, 15:8 */
	.set	SIO_IOBASELO,0x61	/* SIO I/O port base address,  7:0 */
	.set	SIO_DBASEHI,0x60	/* SIO KBC data base address, 15:8 */
	.set	SIO_DBASELO,0x61	/* SIO KBC data base address,  7:0 */
	.set	SIO_CBASEHI,0x62	/* SIO KBC command base addr, 15:8 */
	.set	SIO_CBASELO,0x63	/* SIO KBC command base addr,  7:0 */
	.set	SIO_LUNENABLE,0x01	/* SIO LUN enable */
	.set	SIO_LUNDISABLE,0x00	/* SIO LUN disable */
	.set	SIO_LUNCNFGR,0xF0	/* SIO LUN configuration register */
        .set    PID_CLARIION,0xC0       /* processor identifier, CLARIION */
        .set    PID_VIPER,0xD0          /* processor identifier, VIPER */
        .set    PID_GENESIS2,0xE0       /* processor identifier, GENESIS2 */
        .set    PID_MASK,0xF0           /* processor identifier mask */

/*
;
;	This function initializes the superio chip to a functional 
;	state.
;
;	Upon completion, SIO resource registers are mapped as follows:
;	Resource	Enabled		Address
;	FDC		Yes		PRI	3F0-3F7
;	IDE		Yes		PRI	1F0-1F7 3F6, 3F7
;	UART1		Yes		COM1	3F8-3FF
;	UART2		Yes		COM2	2F8-2FF
;	||PORT		Yes		LPT1	3BC-3BE
;	RTC		Yes			070, 071
;	KBC		Yes			060, 064
;
;
*/

.sioInit:
	mfspr	r7,8			/* save link register */
/*
;
;	check the type of superI/O that we're dealing with (read the
;	identifier register of the 87303/87323), all possible locations
;	are checked, this is done in the case the configuration straps
;	are not installed correctly, this will give us the ability to
;	have basic serial COM for console I/O
;
*/
	/*
	 *	Get base addr of ISA I/O space
	 */
	lis	r6,HI(CPU_PCI_ISA_IO_ADRS)
	ori	r6,r6,LO(CPU_PCI_ISA_IO_ADRS)
/*
 *	Probe the 4 possible locations of the SIO's Index Register
 *	in ISA I/O space.
 *		398/399
 *		26E/26F
 *		15C/15D
 *		02E/02F
 */
	addi	r3,r6,0x0398		/* Get superI/O 87303/87323 base */
	addi	r4,r0,SIO_SID		/* load identifier register offset */
	bl	.sio_br
	addi	r4,r0,0x0398		/* Get superI/O 87303/87323 base */
	andi.	r3,r3,SIO_SIDMASK	/* mask to identifier bits */
	cmpli	0,0,r3,SIO_SID87303	/* is it a 87303? */
	bc	12,2,.sioInit_87303	/* if equal, yes, goto to 87303 init */
	cmpli	0,0,r3,SIO_SID87323	/* is it a 87323? */
	bc	12,2,.sioInit_87303	/* if equal, yes, goto to 87303 init */

	addi	r3,r6,0x026E		/* Get superI/O 87303/87323 base */
	addi	r4,r0,SIO_SID		/* load identifier register offset */
	bl	.sio_br
	addi	r4,r0,0x026E		/* Get superI/O 87303/87323 base */
	andi.	r3,r3,SIO_SIDMASK	/* mask to identifier bits */
	cmpli	0,0,r3,SIO_SID87303	/* is it a 87303? */
	bc	12,2,.sioInit_87303	/* if equal, yes, goto to 87303 init */
	cmpli	0,0,r3,SIO_SID87323	/* is it a 87323? */
	bc	12,2,.sioInit_87303	/* if equal, yes, goto to 87303 init */

	addi	r3,r6,0x015C		/* Get superI/O 87303/87323 base */
	addi	r4,r0,SIO_SID		/* load identifier register offset */
	bl	.sio_br
	addi	r4,r0,0x015C		/* Get superI/O 87303/87323 base */
	andi.	r3,r3,SIO_SIDMASK	/* mask to identifier bits */
	cmpli	0,0,r3,SIO_SID87303	/* is it a 87303? */
	bc	12,2,.sioInit_87303	/* if equal, yes, goto to 87303 init */
	cmpli	0,0,r3,SIO_SID87323	/* is it a 87323? */
	bc	12,2,.sioInit_87303	/* if equal, yes, goto to 87303 init */

	addi	r3,r6,0x002E		/* Get superI/O 87303/87323 base */
	addi	r4,r0,SIO_SID		/* load identifier register offset */
	bl	.sio_br
	addi	r4,r0,0x002E		/* Get superI/O 87303/87323 base */
	andi.	r3,r3,SIO_SIDMASK	/* mask to identifier bits */
	cmpli	0,0,r3,SIO_SID87303	/* is it a 87303? */
	bc	12,2,.sioInit_87303	/* if equal, yes, goto to 87303 init */
	cmpli	0,0,r3,SIO_SID87323	/* is it a 87323? */
/*
;
;	if we make it here, the SIO device is not a 87303/87323 type,
;	check for an 87308 SIO device type (PnP capable), for right
;	now only motherboard mode addresses will be probed
;
;
;	87308 initialization
;
*/
.sioInit_87308:

/*
;
;	determine the base address (motherboard mode - 15C/15D, 2E/2F)
;
*/
	/*
	 *	Get base addr of ISA I/O space
	 */
	lis	r6,HI(CPU_PCI_ISA_IO_ADRS)
	ori	r6,r6,LO(CPU_PCI_ISA_IO_ADRS)

	addi	r3,r6,0x002E		/* adjust to superI/O 87308 base */
	addi	r4,r0,SIO_SIDPNP	/* select SID register */
	bl	.sio_br
	andi.	r3,r3,SIO_SIDMASK	/* mask to identifier bits */
	cmpli	0,0,r3,SIO_SID87308	/* is it a 87308? */
	addi	r4,r0,0x002E		/* adjust to superI/O 87308 base */
	bc	12,2,.sioInit_87308_fa	/* if equal, yes, goto to 87308 init */

	addi	r3,r6,0x015C		/* adjust to superI/O 87308 base */
	addi	r4,r0,SIO_SIDPNP	/* select SID register */
	bl	.sio_br
	andi.	r3,r3,SIO_SIDMASK	/* mask to identifier bits */
	cmpli	0,0,r3,SIO_SID87308	/* is it a 87308? */
	addi	r4,r0,0x015C		/* adjust to superI/O 87308 base */
	bc	12,2,.sioInit_87308_fa	/* if equal, yes, goto to 87308 init */

	b	.sioInit_done		/* don't know what to do... */

.sioInit_87308_fa:
	add	r6,r6,r4		/* add offset to base */
	or	r3,r6,r6		/* make a copy */
/*
;	enable PS/2 mode in SIO configuration register #1
*/
	addi	r4,r0,SIO_CNFG1		/* select CNFG1 */
	bl	.sio_br
	andi.	r5,r3,0xFB		/* keep all but PS/2-AT mode bit */
	addi	r4,r0,SIO_CNFG1		/* select CNFG1 */
	or	r3,r6,r6		/* make a copy */
	bl	.sio_bw
/*
;	KBC (LUN 0)
*/
	addi	r4,r0,SIO_LUNINDEX	/* select KBC LUN */
	addi	r5,r0,0x0
	bl	.sio_bw
	addi	r4,r0,SIO_ACTIVATE	/* disable KBC */
	addi	r5,r0,SIO_LUNDISABLE
	bl	.sio_bw
	addi	r4,r0,SIO_LUNCNFGR	/* initialize KBC clock to 8 Mhz */
	addi	r5,r0,0x0
	bl	.sio_bw
	addi	r4,r0,SIO_DBASEHI	/* initialize KBC data base address to 0x060 */
	addi	r5,r0,0x00
	bl	.sio_bw
	addi	r4,r0,SIO_DBASELO
	addi	r5,r0,0x60
	bl	.sio_bw
	addi	r4,r0,SIO_CBASEHI	/* initialize KBC command base address to 0x064 */
	addi	r5,r0,0x00
	bl	.sio_bw
	addi	r4,r0,SIO_CBASELO
	addi	r5,r0,0x64
	bl	.sio_bw
	addi	r4,r0,SIO_ACTIVATE	/* enable KBC */
	addi	r5,r0,SIO_LUNENABLE
	bl	.sio_bw
/*
;	FDC (LUN 3)
*/
	addi	r4,r0,SIO_LUNINDEX	/* select FDC LUN */
	addi	r5,r0,0x3
	bl	.sio_bw
	addi	r4,r0,SIO_IOBASEHI	/* initialize FDC address to 0x3F0 */
	addi	r5,r0,0x03
	bl	.sio_bw
	addi	r4,r0,SIO_IOBASELO
	addi	r5,r0,0xF0
	bl	.sio_bw
	addi	r4,r0,SIO_LUNCNFGR	/* initialize FDC to PS-2 mode */
	addi	r5,r0,0x40
	bl	.sio_bw
	addi	r4,r0,SIO_ACTIVATE	/* enable FDC */
	addi	r5,r0,SIO_LUNENABLE
	bl	.sio_bw
/*
;	COM2 (LUN 5)
*/
	addi	r4,r0,SIO_LUNINDEX	/* select COM2 LUN */
	addi	r5,r0,0x5
	bl	.sio_bw
	addi	r4,r0,SIO_IOBASEHI	/* initialize COM2 address to 0x2F8 */
	addi	r5,r0,0x02
	bl	.sio_bw
	addi	r4,r0,SIO_IOBASELO
	addi	r5,r0,0xF8
	bl	.sio_bw
	addi	r4,r0,SIO_ACTIVATE	/* enable COM2 */
	addi	r5,r0,SIO_LUNENABLE
	bl	.sio_bw
/*
;	COM1 (LUN 6)
*/
	addi	r4,r0,SIO_LUNINDEX	/* select COM1 LUN */
	addi	r5,r0,0x6
	bl	.sio_bw
	addi	r4,r0,SIO_IOBASEHI	/* initialize COM1 address to 0x3F8 */
	addi	r5,r0,0x03
	bl	.sio_bw
	addi	r4,r0,SIO_IOBASELO
	addi	r5,r0,0xF8
	bl	.sio_bw
	addi	r4,r0,SIO_ACTIVATE	/* enable COM1 */
	addi	r5,r0,SIO_LUNENABLE
	bl	.sio_bw
/*
;	LPT (LUN 4)
*/
	addi	r4,r0,SIO_LUNINDEX	/* select LPT LUN */
	addi	r5,r0,0x4
	bl	.sio_bw
	addi	r4,r0,SIO_IOBASEHI	/* initialize LPT address to 0x3BC */
	addi	r5,r0,0x03
	bl	.sio_bw
	addi	r4,r0,SIO_IOBASELO
	addi	r5,r0,0xBC
	bl	.sio_bw
	addi	r4,r0,SIO_LUNCNFGR	/* Put in 87303 compatible mode. */
	addi	r5,r0,0x12
	bl	.sio_bw
	addi	r4,r0,SIO_ACTIVATE	/* enable LPT */
	addi	r5,r0,SIO_LUNENABLE
	bl	.sio_bw
	b	.sioInit_done		/* branch to done */
/*
;	87303/87323 initialization
*/
.sioInit_87303:
	add	r6,r6,r4		/* add offset to base */
	or	r3,r6,r6		/* make a copy */

	addi	r4,r0,0			/* initialize FER with 0x4F */
	addi	r5,r0,0x4F
	bl	.sio_bw
	addi	r4,r0,1			/* initialize FAR with 0x11 */
	addi	r5,r0,0x11
	bl	.sio_bw
	addi	r4,r0,9			/* initialize ASC to PS-2 mode */
	addi	r5,r0,0x40
	bl	.sio_bw
	addi	r4,r0,5			/* retrieve KBC/RTC control register */
	bl	.sio_br
	andi.	r5,r3,0xD0		/* keep KBC_CSS, CSE */
	ori	r5,r5,0x0F		/* add RTC_E, PAE, KBC_SC, KBC_E */
	or	r3,r6,r6		/* load base address of SIO device */
	bl	.sio_bw
.sioInit_done:
/*
;	for VIPER H/W we need to enable the CSx registers of the SIO
;	to point to the MCG classic RTC/NVRAM device
*/
	addis	r3,r0,CPUCRA_HI		/* load addr to CPU config reg */
	ori	r3,r3,CPUCRA_LO
	lbz	r3,0(r3)		/* read CPU config reg */
	andi.	r3,r3,PID_MASK		/* mask off unwanted data bits */
	cmpli	0,0,r3,PID_VIPER	/* is it a VIPER? */
	bc	4,2,.sioInit_viper_no	/* if not equal, no, branch */

	/*
	 *	Get base addr of ISA I/O space
	 */
	lis	r3,HI(CPU_PCI_ISA_IO_ADRS)
	ori	r3,r3,LO(CPU_PCI_ISA_IO_ADRS)

	addi	r3,r3,0x015C		/* adjust to superI/O 87308 base */
	or	r6,r3,r3		/* make a copy */
/*
;	CS0
*/
	addi	r4,r0,SIO_PCSCI		/* select PCSCIR */
	addi	r5,r0,0x00
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCD		/* select PCSCDR */
	addi	r5,r0,0x00
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCI		/* select PCSCIR */
	addi	r5,r0,0x01
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCD		/* select PCSCDR */
	addi	r5,r0,0x76
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCI		/* select PCSCIR */
	addi	r5,r0,0x02
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCD		/* select PCSCDR */
	addi	r5,r0,0x40
	bl	.sio_bw
/*
;	CS1
*/
	addi	r4,r0,SIO_PCSCI		/* select PCSCIR */
	addi	r5,r0,0x05
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCD		/* select PCSCDR */
	addi	r5,r0,0x00
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCI		/* select PCSCIR */
	addi	r5,r0,0x05
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCD		/* select PCSCDR */
	addi	r5,r0,0x70
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCI		/* select PCSCIR */
	addi	r5,r0,0x06
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCD		/* select PCSCDR */
	addi	r5,r0,0x1C
	bl	.sio_bw
/*
;	CS2
*/
	addi	r4,r0,SIO_PCSCI		/* select PCSCIR */
	addi	r5,r0,0x08
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCD		/* select PCSCDR */
	addi	r5,r0,0x00
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCI		/* select PCSCIR */
	addi	r5,r0,0x09
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCD		/* select PCSCDR */
	addi	r5,r0,0x71
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCI		/* select PCSCIR */
	addi	r5,r0,0x0A
	bl	.sio_bw
	addi	r4,r0,SIO_PCSCD		/* select PCSCDR */
	addi	r5,r0,0x1C
	bl	.sio_bw
/*
;	PMC (LUN 8)
*/
	addi	r4,r0,SIO_LUNINDEX	/* select PMC LUN */
	addi	r5,r0,0x8
	bl	.sio_bw
	addi	r4,r0,SIO_IOBASEHI	/* initialize PMC address to 0x460 */
	addi	r5,r0,0x04
	bl	.sio_bw
	addi	r4,r0,SIO_IOBASELO
	addi	r5,r0,0x60
	bl	.sio_bw
	addi	r4,r0,SIO_ACTIVATE	/* enable PMC */
	addi	r5,r0,SIO_LUNENABLE
	bl	.sio_bw
.sioInit_viper_no:
	mtspr	8,r7			/* restore link register */
	bclr	20,0			/* return to caller */
/*
; routine: sio_bw
;	this function writes a register to the SIO chip
; call:
;	sio_bw(sioaddr, regnum, value)
; return:
;	none
*/
.sio_bw:
	stb	r4,0(r3)	/* write index register with register offset */
	eieio
	sync
	stb	r5,1(r3)	/* 1st write */
	eieio
	sync
	stb	r5,1(r3)	/* 2nd write */
	eieio
	sync
	bclr	20,0		/* return to caller */
/*
; routine: sio_br
;	this function reads a register from the SIO chip
; call:
;	sioInit(sioaddr, regnum)
; return:
;	register value
*/
.sio_br:
	stb	r4,0(r3)	/* write index register with register offset */
	eieio
	sync
	lbz	r3,1(r3)	/* retrieve specified reg offset contents */
	eieio
	sync
	bclr	20,0		/* return to caller */

#endif  /* MV2300 */
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
	mtspr	1017,r3
	bclr	20,0

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
* RETURNS: value of SPR1017 (in r3)
*/

sysL2crGet:
	mfspr	r3,1017
	bclr	20,0

/******************************************************************************
*
* sysTimeBaseLGet - Get lower half of Time Base Register
*
* This routine will read the contents the lower half of the Time
* Base Register (TBL - TBR 268).
*
* This is not a standard routine.  The library call vxTimeBaseGet returns a
* 64-bit quantity.  This is a special fix to get access to just the lower
* 32-bits of the timebase.
*
* From a C point of view, the routine is defined as follows:
*
*    UINT32 sysTimeBaseLGet(void)
*
* RETURNS: value of TBR 268 (in r3)
*/

sysTimeBaseLGet:
	mftb 3
	bclr 20,0

/******************************************************************************
*
* sysHid1Get - read from HID1 register SPR1009.
*
* This routine will read the contents the HID1 (SPR1009)
*
* From a C point of view, the routine is defined as follows:
*
*    UINT sysHid1Get()
*
* RETURNS: value of SPR1009 (in r3)
*/

sysHid1Get:
	mfspr r3,1009
	bclr 20,0

