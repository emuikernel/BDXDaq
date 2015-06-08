/* romInit.s - Motorola MVME2600 ROM initialization module */

/* Copyright 1984-2000 Wind River Systems, Inc. */
/* Copyright 1996,1997,1998 Motorola, Inc. All Rights Reserved */
	.data
	.globl  copyright_wind_river
	.long   copyright_wind_river

/*
modification history
--------------------
01q,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01p,21jan00,dat  fixed assembler warning
01o,11oct99,dmw  Added the ability to come up if VME_A32_MSTR_LOCAL is
                 less than the total DRAM size.
01n,16feb99,mas  Added setting of SIED and BHTE bits in HID0 (SPR 24453);
		 added Raven3 compatibility.
01m,30sep98,tm   Fixed ILOCK bit clear in MPC750 iCache enable (SPR 22121)
01l,14apr98,ms_  merged Motorola mv2700 support
01k,06apr98,scb  Modify to never alter r0, once it is initially set to zero.
01j,02apr98,tb   Turned on Falcon's ECC
01i,22jan98,rhk  SPR 20104, correct use of HI and HIADJ macros.
01h,16jul97,scb  added support for MPC750 (Arthur).
01g,14oct97,srr  added floating point data register initialization.
01f,25jul97,srr/ added 604r (Mach 5) support (SPR 8911).
	    mas
01e,29may97,srr  Chg RAVEN #defines to support vxMemProbe and replace
		 hardcoded values where appropriate. (MCG MR#67,69)
		 SPRs 8289, 8560.
01d,06may97,mas  added extended VME support (SPR 8410).
01c,24apr97,mas  added Motorola MPIC support (SPR 8170).
01b,02jan97,dat  mod history fix
01a,01sep96,mot  written. (from mv1603/romInit.s, ver 01l)
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

#define	_ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "asm.h"
#include "config.h"
#include "regs.h"	

	/* Exported internal functions */

	FUNC_EXPORT(_romInit)		/* start of system code */
	FUNC_EXPORT(romInit)		/* start of system code */

	/* externals */

	.extern romStart	/* system initialization routine */

	.text
	.align 2

		
/******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*

* romInit
*     (
*     int startType	/@ only used by 2nd entry point @/
*     )

*/

_romInit:
romInit:

	bl	cold
	
	bl	warm

	/* copyright notice appears at beginning of ROM (in TEXT segment) */

	.ascii   "Copyright 1984-1999 Wind River Systems, Inc."
	.align 2

cold:
	li	r11, BOOT_COLD
	bl	start		/* skip over next instruction */

			
warm:
	or	r11, r3, r3	/* startType to r11 */

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
	 *	Set HID0 to a known state
	 *	Enable machine check input pin (EMCP) for DRAM ECC detection
	 */

	addis   r3,r0,0x8000
	ori     r3,r3,0x0000
	mtspr   HID0, r3

	/*
	 *	Set MPU/MSR to a known state
	 *	Turn on FP
	 */

	andi.	r3, r3, 0
	ori	r3, r3, 0x2000
	sync
	mtmsr 	r3
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

	bl	ifpdr_value
	.long	0x3f800000	/* 1.0 */
ifpdr_value:
	mfspr	r3,8
	lfs	f0,0(r3)
	lfs	f1,0(r3)
	lfs	f2,0(r3)
	lfs	f3,0(r3)
	lfs	f4,0(r3)
	lfs	f5,0(r3)
	lfs	f6,0(r3)
	lfs	f7,0(r3)
	lfs	f8,0(r3)
	lfs	f9,0(r3)
	lfs	f10,0(r3)
	lfs	f11,0(r3)
	lfs	f12,0(r3)
	lfs	f13,0(r3)
	lfs	f14,0(r3)
	lfs	f15,0(r3)
	lfs	f16,0(r3)
	lfs	f17,0(r3)
	lfs	f18,0(r3)
	lfs	f19,0(r3)
	lfs	f20,0(r3)
	lfs	f21,0(r3)
	lfs	f22,0(r3)
	lfs	f23,0(r3)
	lfs	f24,0(r3)
	lfs	f25,0(r3)
	lfs	f26,0(r3)
	lfs	f27,0(r3)
	lfs	f28,0(r3)
	lfs	f29,0(r3)
	lfs	f30,0(r3)
	lfs	f31,0(r3)
	sync

	/*
	 *	Set MPU/MSR to a known state
	 *	Turn off FP
	 */

	andi.	r3, r3, 0
	sync
	mtmsr 	r3
	isync

	/* Init the Segment registers */

	andi.	r3, r3, 0
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

	lis	r3, HI(MV2600_SXCCR_A)
	ori	r3, r3, LO(MV2600_SXCCR_A)
	addis	r4, r0, 0x0
	ori	r4, r4, 0x0070
	stb	r4, 0x0(r3)
#endif  /* MV2300 */

	/* Turn off data and instruction cache control bits */
	
	mfspr   r3, HID0
	isync
	rlwinm	r4, r3, 0, 18, 15	/* r4 has ICE and DCE bits cleared */
	sync
	isync
	mtspr	HID0, r4		/* HID0 = r4 */
	isync

	/* Get cpu type */

	mfspr   r28, PVR
	rlwinm  r28, r28, 16, 16, 31

	/* invalidate the MPU's data/instruction caches */

	lis	r3, 0x0
	cmpli   0, 0, r28, CPU_TYPE_750
	beq     CPU_IS_750

	cmpli	0, 0, r28, CPU_TYPE_603
	beq	CPU_IS_603
	cmpli	0, 0, r28, CPU_TYPE_603E
	beq	CPU_IS_603
        cmpli   0, 0, r28, CPU_TYPE_603P
        beq     CPU_IS_603
        cmpli   0, 0, r28, CPU_TYPE_604R
        bne     CPU_NOT_604R

CPU_IS_604R:
        lis     r3, 0x0
        mtspr   HID0, r3        /* disable the caches */
        isync
        ori     r4, r4, 0x0002  /* disable BTAC by setting bit 30 */

CPU_NOT_604R:
	ori	r3, r3, 0x0C00		/* r3 has invalidate bits set */
CPU_IS_603:
	ori	r3, r3, 0xC000		/* r3 has enable and bits set */
	or	r4, r4, r3		/* set bits */
	sync
	isync
	mtspr   HID0, r4		/* HID0 = r4 */
	andc	r4, r4, r3		/* clear bits */
	isync
	cmpli   0, 0, r28, CPU_TYPE_604
	beq	CPU_IS_604
	cmpli   0, 0, r28, CPU_TYPE_604E
	beq	CPU_IS_604
        cmpli   0, 0, r28, CPU_TYPE_604R
        beq     CPU_IS_604
        cmpli   0, 0, r28, CPU_TYPE_750
        beq     CPU_IS_604
	mtspr	HID0, r4
	isync

#ifdef USER_I_CACHE_ENABLE 
	b	I_CACHE_ON_603
#else
	b	CACHE_ENABLE_DONE
#endif

CPU_IS_750:
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
        b       CACHE_ENABLE_DONE

CPU_IS_604:
	lis	r5, 0x0
	ori	r5, r5, 0x1000
	mtspr	CTR, r5
LOOP_DELAY:
	nop
	bdnz	LOOP_DELAY
	isync
	mtspr 	HID0, r4
	isync

	/* turn the Instruction cache ON for faster FLASH ROM boots */

#ifdef USER_I_CACHE_ENABLE 

        ori     r4, r4, 0x8800		/* set ICE & ICFI bit */
        isync                           /* Synchronize for ICE enable */
	b	WRITE_R4
I_CACHE_ON_603:
	ori	r4, r4, 0x8800		/* set ICE & ICFI bit */
        rlwinm  r3, r4, 0, 21, 19	/* clear the ICFI bit */

        /*
         * The setting of the instruction cache enable (ICE) bit must be
         * preceded by an isync instruction to prevent the cache from being
         * enabled or disabled while an instruction access is in progress.
         */
	isync
WRITE_R4:
        mtspr   HID0, r4                /* Enable Instr Cache & Inval cache */
	cmpli   0, 0, r28, CPU_TYPE_604
	beq    	CACHE_ENABLE_DONE 
	cmpli   0, 0, r28, CPU_TYPE_604E
	beq    	CACHE_ENABLE_DONE 
	cmpli   0, 0, r28, CPU_TYPE_604R
	beq    	CACHE_ENABLE_DONE 
	cmpli   0, 0, r28, CPU_TYPE_750
	beq    	CACHE_ENABLE_DONE 

        mtspr   HID0, r3                /* using 2 consec instructions */
                                        /* PPC603 recommendation */
#endif
CACHE_ENABLE_DONE:

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
 * This following contains the entry code for the initialization code
 * for the Raven, a Host PCI Bridge/Memory Controller used in
 * Motorola's PowerPC based boards.
 */

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

/*
# 	DRAM Initialization/Sizing for FALCON Module
#	Initialize the FALCON (DRAM Controller) registers.
#	The registers are initialized with values from the
#	results of the probe (see below).
#
#	algorithm:
#
#	Each memory bank size that is possible is tried.  This starts
#	with the largest to the smallest.   A table is used to inform
#	the sizing code of the needed information.  The information
#	consists of the memory bank size, probe addresses, and the data
#	pattern associated with each of the probe address.  All off the
#	probe addresses are written to first with associative write data
#	pattern, then they are read.  An exact match of the data is
#	required to deem the memory bank size has been found.
#
#	Size			Key Address Lines
#	40000000 (1GB)		A2
#	10000000 (256MB)	A4/A6
#	08000000 (128MB)	A5/A6/A18
#	04000000 (64MB)		A6/A18
#	02000000 (32MB)		A7/A18/A19
#	01000000 (16MB)		N/A
#
#	As you can see, this algorithm is coupled very tightly with the
#	H/W, if it ever changes, this algorithm will need to be modified.
#
#	register-usage:
#		r1	= SP (not modified)
#		r2	= TOC (not modified)
#		r3	= size of DRAM in bytes
#		r4	= DRAM attributes register address
#		r5	= DRAM base-address register address
#		r6	= test pattern's table pointer
#		r7	= DRAM bank index (0 to 3)
#		r8-r10,r12-r19	= run-time (scratch)
#		r11	= VxWorks 
#		r20	= saved return instruction pointer
#		r21	= verify error flag
#		r22	= FALCON base address
#		r23	= run-time (scratch)
#
*/

	.set	NBANKS,4		/* number of DRAM banks */

	xor	r0,r0,r0		/* insure r0 is zero */
	or	r3,r0,r0		/* set to no memory present state */
	mfspr	r20,8			/* save return instruction pointer */

	bl	.falcon_i_ra		/* branch around tables */
        
        /* DRAM controller register addresses table */

	.long	FALCON_BASE_ADRS
	.long	DRAM_REG_BASE		/* DRAM base-address register addr */
	.long	DRAM_REG_SIZE		/* DRAM attributes register address */

.falcon_i_ra:
	mfspr	r4,8			/* load pointer to table */

        /* load register address values */

	lwz	r8,0x00(r4)		/* FALCON register base address */
	lwz	r5,0x04(r4)		/* DRAM base-address register address */
	lwz	r4,0x08(r4)		/* DRAM attributes register address */
	or	r22,r8,r8		/* save it for later use */

	eieio				/* synchronize the data stream */
	sync				/* synchronize the data stream */

        /* initialize to DRAM control registers to a known state */

	/* get the DRAM speed (RSPD, in r3) */

	lis	r9, HI(MV2600_MCR_)	/* load address of Mem Config Reg */
	ori	r9, r9, LO(MV2600_MCR_)
	lwz	r9, 0(r9)
	lis	r10, HI(MV2600_MCR_MSK)	/* load mask for DRAM speed */
	ori	r10, r10, LO(MV2600_MCR_MSK)
	and	r9, r9, r10

	lis	r10, HI(MV2600_MCR_DRAM_60ns)	/* load value for 60 ns DRAM */
	ori	r10, r10, LO(MV2600_MCR_DRAM_60ns)
	cmpw	1,  r9, r10
	bc	12, 6, dram60ns

	lis	r10, HI(MV2600_MCR_DRAM_50ns)	/* load value for 50 ns DRAM */
	ori	r10, r10, LO(MV2600_MCR_DRAM_50ns)
	cmpw	1,  r9, r10
	bc	12, 6, dram50ns
	li	r3, DRAM_70ns			/* default: 70 ns */
	b	endDramSpd

dram60ns:
	li	r3, DRAM_60ns
	b	endDramSpd
dram50ns:
	li	r3, DRAM_50ns

endDramSpd:
	lis	r9, HI(MV2600_MCR_)	/* load address of Mem Config Reg */
	ori	r9, r9, LO(MV2600_MCR_)
	lwz	r9,0(r9)		/* read Memory Configuration Register */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	rlwinm	r9,r9,7,28,28 		/* mask off all but the M_FREF bit */

	or	r9,r3,r9		/* IHEN=0,ADIS=0,RFREF=M_FREF */
	stw	r9,0x08(r8)		/* general control register */
	eieio				/* synchronize the data stream */
	sync				/* synchronize the data stream */

	/* get bus speed in MHZ, r3 has value */

	lis	r9, HI(MV2600_CCR_)	/* load address of System Config Reg */
	ori	r9, r9, LO(MV2600_CCR_)
	lwz	r9, 0(9)
	lis	r10, HI(MV2600_CCR_CLK_MSK)	/* load mask for bus speed */
	ori	r10, r10, LO(MV2600_CCR_CLK_MSK)
	and	r9, r9, r10

	lis	r10, HI(MV2600_CCR_CPU_CLK_60)	/* load value for 60 */
	ori	r10, r10, LO(MV2600_CCR_CPU_CLK_60)
	cmpw	1,  r9, r10
	bc	12, 6, bus60mhz

	lis	r10, HI(MV2600_CCR_CPU_CLK_50)	/* load value for 50 */
	ori	r10, r10, LO(MV2600_CCR_CPU_CLK_50)
	cmpw	1,  r9, r10
	bc	12, 6, bus50mhz
	li	r3, 67					/* default: 67 MHz */
	b	endBusSpd

bus60mhz:
	li	r3, 60
	b	endBusSpd
bus50mhz:
	li	r3, 50

endBusSpd:
	slwi	r3,r3,24		/* move to upper byte */
	stw	r3,0x20(r8)		/* CLK frequency register */
	eieio				/* synchronize the data stream */
	sync				/* synchronize the data stream */

	addis	r9,r0,0x0000		/* SWEN=0,RTEST=000 */
	ori	r9,r9,0x0000		/* SWEN=0,RTEST=000 */
	stw	r9,0x40(r8)		/* DRAM scrub/refresh register */
	eieio				/* synchronize the data stream */
	sync				/* synchronize the data stream */

	addis	r9,r0,0x0100		/* RWCB=0,DERC=1,xIEN=0,MCKEN=0 */
	ori	r9,r9,0x0000		/* RWCB=0,DERC=1,xIEN=0,MCKEN=0 */
	stw	r9,0x28(r8)		/* DRAM ECC control register */
	eieio				/* synchronize the data stream */
	sync				/* synchronize the data stream */

	addis	r9,r0,0x8000		/* ELOG=1,ESEN=0,SCOF=1 */
	ori	r9,r9,0x0100		/* ELOG=1,ESEN=0,SCOF=1 */
	stw	r9,0x30(r8)		/* DRAM ECC error-logger register */
	eieio				/* synchronize the data stream */
	sync				/* synchronize the data stream */

	bl	.falcon_i_tp		/* branch around tables */

        /* write-addresses/data-patterns table (address/hi-data/lo-data) */

	.set	DRAM_SIZE1,0		/* size index */
	.set	DRAM_ATTR1,4		/* attributes index */
	.set	DRAM_CHCK1,8		/* address/pattern starting index */
	.set	TBLEND,-1		/* table end marker */

        /* 1GB */

	.long	0x40000000,0x06
	.long	0x00000000,0x01010101,0xFEFEFEFE
	.long	0x20000000,0x02020202,0xFDFDFDFD
	.long	-1					/* table entry end */

        /* 256MB */

	.long	0x10000000,0x05
	.long	0x00000000,0x03030303,0xFCFCFCFC
	.long	0x02000000,0x04040404,0xFBFBFBFB
	.long	0x08000000,0x05050505,0xFAFAFAFA
	.long	0x0A000000,0x06060606,0xF9F9F9F9
	.long	-1					/* table entry end */

        /* 128MB */

	.long	0x08000000,0x04
	.long	0x00000000,0x07070707,0xF8F8F8F8
	.long	0x00002000,0x08080808,0xF7F7F7F7
	.long	0x02000000,0x09090909,0xF6F6F6F6
	.long	0x02002000,0x0A0A0A0A,0xF5F5F5F5
	.long	0x04000000,0x0B0B0B0B,0xF4F4F4F4
	.long	0x04002000,0x0C0C0C0C,0xF3F3F3F3
	.long	0x06000000,0x0D0D0D0D,0xF2F2F2F2
	.long	0x06002000,0x0E0E0E0E,0xF1F1F1F1
	.long	-1					/* table entry end */

        /* 64MB */

	.long	0x04000000,0x03
	.long	0x00000000,0x0F0F0F0F,0xF0F0F0F0
	.long	0x00002000,0x10101010,0xEFEFEFEF
	.long	0x02000000,0x11111111,0xEEEEEEEE
	.long	0x02002000,0x12121212,0xEDEDEDED
	.long	-1					/* table entry end */

        /* 32MB */

	.long	0x02000000,0x02
	.long	0x00000000,0x13131313,0xECECECEC
	.long	0x00001000,0x14141414,0xEBEBEBEB
	.long	0x00002000,0x15151515,0xEAEAEAEA
	.long	0x00003000,0x16161616,0xE9E9E9E9
	.long	0x01000000,0x17171717,0xE8E8E8E8
	.long	0x01001000,0x18181818,0xE7E7E7E7
	.long	0x01002000,0x19191919,0xE6E6E6E6
	.long	0x01003000,0x1A1A1A1A,0xE5E5E5E5
	.long	-1					/* table entry end */

        /* 16MB */

	.long	0x01000000,0x01
	.long	0x00000000,0x1B1B1B1B,0xE4E4E4E4
	.long	-1					/* table entry end */

	.long	-1					/* table end */

.falcon_i_tp:
	mfspr	r6,8			/* load pointer to table */

/*
 *	disable all DRAM banks and set to "block not present" size
 *	set all DRAM banks base-address register to zero
 */

	addi	r7,r0,0			/* clear DRAM bank index register */
	addi	r8,r0,0x00		/* disable and set to no-size */
.falcon_i_ks:
	stbx	r8,r7,r4		/* write DRAM attributes register */
	stbx	r8,r7,r5		/* write DRAM bank base-address reg */
	addi	r7,r7,1			/* increment bank index */
	cmpli	0,0,r7,NBANKS		/* are we done yet? */
	bc	4,2,.falcon_i_ks	/* if not equal, no, branch */

/*
 *	probe for the existance of all possible DRAM banks
 *
 *	on a DRAM bank basis, the DRAM bank is enabled with the size
 *	as specified by the current table entry, then perform the data
 *	writes at the specified addresses as specified by the current table
 *	entry, then read and verify the locations written with the
 *	expected data
 *
 *	following each probe the DRAM bank will be disabled, if the DRAM
 *	bank is present, its attributes register will be initialized to
 *	the size found (i.e., size-encoding bits set to proper state)
 */

	addi	r23,r0,TBLEND		/* setup table end indicator */
	addi	r7,r0,0			/* clear DRAM bank index register */
.falcon_i_pp:
	or	r19,r6,r6		/* copy pointer to table */
	or	r10,r6,r6		/* copy pointer to table */

        /* write test data patterns (as specified by the current table entry)*/

.falcon_i_pp_i:
	or	r19,r10,r10		/* copy current table entry pointer */
	addi	r21,r0,0		/* clear verify error flag */
	lwz	r14,DRAM_ATTR1(r10)	/* load DRAM attributes(encoded size)*/
	ori	r14,r14,0x80		/* set bank enable bit */
	stbx	r14,r7,r4		/* write DRAM attributes register */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	addi	r10,r10,DRAM_CHCK1	/* move pointer to data patterns */
.falcon_i_pp_w:
	lwz	r12,0(r10)		/* load write data pattern address */
	lwz	r13,4(r10)		/* load write data pattern hi */
	lwz	r14,8(r10)		/* load write data pattern lo */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	stw	r13,0(r12)		/* write data pattern hi */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	stw	r14,4(r12)		/* write data pattern lo */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	addi	r10,r10,12		/* increment to next address */
	lwz	r12,0(r10)		/* load write data pattern address */
	cmpl	0,0,r12,r23		/* check for table entry end */
	bc	4,2,.falcon_i_pp_w	/* if not equal, no, branch */

	or	r10,r19,r19		/* copy current table entry pointer */
	addi	r10,r10,DRAM_CHCK1	/* move pointer to data patterns */
.falcon_i_pp_r:
	lwz	r12,0(r10)		/* load write data pattern address */
	lwz	r13,4(r10)		/* load write data pattern hi */
	lwz	r14,8(r10)		/* load write data pattern lo */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	lwz	r15,0(r12)		/* read data pattern hi */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	lwz	r16,4(r12)		/* read data pattern lo */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	cmpl	0,0,r13,r15		/* do they verify? */
	bc	4,2,.falcon_i_pp_f	/* if not equal, no, branch */
	cmpl	0,0,r14,r16		/* do they verify? */
	bc	4,2,.falcon_i_pp_f	/* if not equal, no, branch */
	b	.falcon_i_pp_p		/* verification passed */
.falcon_i_pp_f:
	addi	r21,r21,1		/* increment verify error flag */
.falcon_i_pp_p:
	addi	r10,r10,12		/* increment to next address */
	lwz	r12,0(r10)		/* load write data pattern address */
	cmpl	0,0,r12,r23		/* check for table entry end */
	bc	4,2,.falcon_i_pp_r	/* if not equal, no, branch */

	addi	r10,r10,4		/* increment to one pass the end */
	cmpli	0,0,r21,0		/* any errors? */
	bc	4,2,.falcon_i_pp_d	/* if not equal, yes, branch */
	lbzx	r8,r7,r4		/* read DRAM attributes register */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	andi.	r8,r8,0x7F		/* mask off the bank enable bit */
	stbx	r8,r7,r4		/* write DRAM attributes register */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	b	.falcon_i_pp_n		/* branch to bank by bank loop control*/

.falcon_i_pp_d:
	addi	r8,r0,0x00		/* disable and set to no-size */
	stbx	r8,r7,r4		/* write DRAM attributes register */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	lwz	r12,0(r10)		/* load write data pattern address */
	cmpl	0,0,r12,r23		/* check for table end */
	bc	4,2,.falcon_i_pp_i	/* if not equal, no, branch */

.falcon_i_pp_n:
	addi	r7,r7,1			/* increment bank index */
	cmpli	0,0,r7,NBANKS		/* are we done yet? */
	bc	4,2,.falcon_i_pp	/* if not equal, no, branch */

/*
 *	at this point the DRAM bank attributes registers will be initialized
 *	to the size-encoding of the DRAM which is present (i.e., probed),
 *	else the DRAM attributes registers contain zero indicating no DRAM
 *	was found
 *
 *	the DRAM that was found must be aligned to a boundary of its size
 *
 *	the following code will begin allocating memory address spaces
 *	starting with the largest first, and working itself to the
 *	smallest
 *
 *	memory will start address zero
 */

	bl	.falcon_i_tp_s		/* branch around tables */
	.long	0x40000000,0x00000006	/* 1024MB */
	.long	0x10000000,0x00000005	/* 256MB */
	.long	0x08000000,0x00000004	/* 128MB */
	.long	0x04000000,0x00000003	/* 64MB */
	.long	0x02000000,0x00000002	/* 32MB */
	.long	0x01000000,0x00000001	/* 16MB */
	.long	TBLEND			/* table entry end */

.falcon_i_tp_s:
	mfspr	r6,8			/* load pointer to table */
	or	r10,r6,r6		/* copy table pointer */
	addi	r23,r0,TBLEND		/* setup table end indicator */
	addi	r3,r0,0			/* clear total DRAM size (in bytes) */
	addi	r13,r0,0		/* memory base address (zero) */
	addi	r18,r0,0		/* clear total DRAM size save register */
.falcon_i_sz:
	addi	r7,r0,0			/* clear DRAM bank index register */
	lwz	r8,DRAM_ATTR1(r10)	/* load DRAM attributes for this entry*/
.falcon_i_sz_b:
	lbzx	r9,r7,r4		/* read DRAM attributes register */
	andi.	r9,r9,0x7		/* mask off unwanted data bits */
	cmpl	0,0,r8,r9		/* do the attributes match? */
	bc	4,2,.falcon_i_sz_no	/* if not equal, no, branch */

	rlwinm	r9,r13,8,8,31		/* extract PowerPC A0-A7 bits */
	stbx	r9,r7,r5		/* write DRAM bank base-address reg */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	lbzx	r9,r7,r4		/* read DRAM attributes register */
	ori	r9,r9,0x80		/* set bank enable bit */
	stbx	r9,r7,r4		/* write DRAM attributes register */
	eieio				/* data synchronize */
	sync				/* data synchronize */

/*
 *  Update total DRAM size and current top of memory address
 *  When finished, r3 will contain the total DRAM size (in bytes) and
 *  r13 will contain one past the top of memory address.
 */

	lwz	r9,DRAM_SIZE1(r10)	/* load DRAM size for this entry */
	add	r3,r3,r9		/* add to the total DRAM size */
	add	r18,r18,r9		/* add to the total DRAM size save register */
	add	r13,r13,r9		/* update current top of memory addr */

.falcon_i_sz_no:
	addi	r7,r7,1			/* increment bank index */
	cmpli	0,0,r7,NBANKS		/* are we done yet? */
	bc	4,2,.falcon_i_sz_b	/* if not equal, no, branch */

	addi	r10,r10,8		/* increment to next table entry */
	lwz	r14,0(r10)		/* load memory size variable */
	cmpl	0,0,r14,r23		/* check for table end */
	bc	4,2,.falcon_i_sz	/* if not equal, no, branch */

        /* work-around for a scrubber/refresh logic bug */

	lwz	r4,0x10(r22)		/* copy the bank enables/sizes reg */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	addis	r5,r0,0			/* gimme a zero */
	stw	r5,0x10(r22)		/* disable all, set sizes to zero */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	stw	r5,0x48(r22)		/* clr ROW/COL addr of refresh/scrub */
	eieio				/* data synchronize */
	sync				/* data synchronize */
	stw	r4,0x10(r22)		/* restore the bank enables/sizes reg */
	eieio				/* data synchronize */
	sync				/* data synchronize */

/*
 *		The purpose of this section is to enable the ECC of the
 *		DRAM.  To do this, it is first necessary to initialize the
 *		ECC check bits.  The ECC check bits are initialized by
 *		initializing all of DRAM.
 *
 *	input:
 *		r3	= Total DRAM size (in bytes)
 *	notes:
 *		1. must run as a supervisor function
 *		2. interrupts must be masked
 *		3. address translation (MMU) disabled
 *		4. assumes ECC Control register is in a power-up state
 *		5. The scrubber is not enabled (SWEN=0).  This provides
 *		   better performance at the expense of less fault
 *		   tolerance.
 */

.falcon_scrub:

/* setup local variables */

	addi	r15,r0,0		/* load starting addr - always zero */
	or	r16,r3,r3		/* load number of bytes		    */
	rlwinm	r16,r16,29,3,31		/* calculate number of doubles      */

/* Make sure FPU is enabled; it's needed for DRAM loads and stores */

	mfmsr	r14			/* Save current value of MSR in r14 */
	addi	r4,r0,0x2000		/* FP bit definition */
	or	r4,r4,r14
	mtmsr	r4
	isync

/*
 * invalidate/enable the processor data cache, one of the assumptions
 * is that address translation is disabled, the default caching mode
 * is copy-back when address translation is disabled, copy-back mode
 * is the recommended caching mode
 */

	stwu	sp, -64(sp)		/* Create an ABI stack frame */
	bl	dCacheInval
	bl	dCacheOn
	addi	sp,sp,64		/* Remove ABI stack frame */

/*
 * Loop through the entire DRAM array and initialize memory.  This has
 * the side effect of initializing the ECC check bits because they are
 * always updated when the DRAM is written.
 *
 * The data path to DRAM is the size of a cache line (128-bits), this
 * is why the data cache is enabled, the initialization of the ECC check
 * bits are optimized when a cache line write/flush occurs
 */

falcon_scrub_start:
	addi	r17,r15,-8		/* starting address - munged */
	mtspr	9,r16			/* load number of doubles in counter */
falcon_scrub_iloop:
	lfdu	0,8(r17)
	stfd	0,0(r17)
	bc	16,0,falcon_scrub_iloop	/* branch till counter == 0 */

	eieio
	sync
/*
 * Loop through the entire DRAM array again.
 * Looping through the entire DRAM array is not necessary just convenient.
 * What is necessary is flushing the data cache from the previous loop
 * so that the last segment (data cache size) of DRAM is initialized.
 */
	addi	r17,r15,-8		/* starting address - munged */
	mtspr	9,r16			/* load number of doubles in counter */
falcon_scrub_floop:
	lfdu	0,8(r17)
	bc	16,0,falcon_scrub_floop	/* branch till counter == 0 */

	eieio
	sync

/* disable the data cache */

	stwu	sp, -64(sp)		/* Create an ABI stack frame */
	bl	dCacheInval
	bl	dCacheOff
	addi	sp,sp,64		/* Remove ABI stack frame */

/* Restore original value of MSR */

	mtmsr	r14
	isync

/* Clear any possible error conditions that occurred during init */

	lis	r14, HI(FALCON_BASE_ADRS)
	ori	r14, r14, LO(FALCON_BASE_ADRS)

	addis	r9,r0,0x8000		/* ELOG=1,ESEN=0,SCOF=1 */
	ori	r9,r9,0x0100
	stw	r9,0x30(r14)		/* Update Error Logger register */
	eieio
	sync

/* Enable ECC and multiple-bit error detection */

	addis	r9,r0,0x0000		/* RWCB=0,DERC=0,xIEN=0,MCKEN=1 */
	ori	r9,r9,0x0001
	stw	r9,0x28(r14)		/* Update ECC Control register */
	eieio
	sync

/* End of Falcon Init */

#ifdef EXTENDED_VME
/*
 * Verify that the end of DRAM does not overlap VME_A32_MSTR_LOCAL.
 * If it does, set MSADD0 to the size of DRAM.  A message will be displayed
 * in sysPhysMemTop prompting the user to increase VME_A32_MSTR_LOCAL.
 * 
 * The size of DRAM is saved in r18 from the memory probe.
 */
	lis	r3, HI(RAVEN_BASE_ADRS)
	ori	r3, r3, LO(RAVEN_BASE_ADRS)

	lis	r5, HI(VME_A32_MSTR_LOCAL)
	ori	r5, r5, LO(VME_A32_MSTR_LOCAL)

	cmpl	0,0,r18,r5		/* If r18 < VME_A32_MSTR_LOCAL */
	bc	12,0,memMapGood		/* Set MSADD0 to DRAM size */

	ori	r18,r18,CPU2PCI_ADDR0_END	/* MSADD0: */
	stw	r18,RAVEN_MPC_MSADD0(r3)	/* write MSADD0 w DRAM size */

	b	doneMSAddr0Calc

memMapGood:
	addis	r4,r0,(VME_A32_MSTR_LOCAL>>16)	/* MSADD0: */
	ori	r4,r4,CPU2PCI_ADDR0_END		/* MSADD0: */
	stw	r4,RAVEN_MPC_MSADD0(r3)		/* write MSADD0 */
	
doneMSAddr0Calc:
	eieio					/* synchronize */
	sync					/* synchronize */
#endif

#if	FALSE				/* XXX TPR SDA not supported yet */
        /* initialize r2 and r13 according to EABI standard */

	lis     r2, HI(_SDA2_BASE_)
	ori	r2, r2, LO(_SDA2_BASE_)
	lis     r13, HI(_SDA_BASE_)
	ori	r13, r13, LO(_SDA_BASE_)
#endif

	/* go to C entry point */

	or	r3, r11, r11
	addi	sp, sp, -FRAMEBASESZ	/* get frame stack */

        lis     r6, HI(romStart)
        ori	r6, r6, LO(romStart)

        lis     r7, HI(romInit)
        ori	r7, r7, LO(romInit)

        lis     r8, HI(ROM_TEXT_ADRS)
        ori	r8, r8, LO(ROM_TEXT_ADRS)

	sub	r6, r6, r7
	add	r6, r6, r8 

	mtlr	r6
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

		
/******************************************************************************
*
* dCacheOn - Turn Data Cache On
*
* void dCacheOn (void)
*/
_dCacheOn:
dCacheOn:
	/* Get cpu type */

	mfspr	r3,PVR
	rlwinm	r3,r3,16,16,31

	cmpli	0,0,r3,CPU_TYPE_603
	bc	12,2,ccdataon_603
	cmpli	0,0,r3,CPU_TYPE_604
	bc	12,2,ccdataon_604
	cmpli	0,0,r3,CPU_TYPE_603E
	bc	12,2,ccdataon_603
        cmpli   0,0,r3,CPU_TYPE_603P
        bc      12,2,ccdataon_603
        cmpli   0,0,r3,CPU_TYPE_750
        bc	12,2,ccdataon_603
        cmpli   0,0,r3,CPU_TYPE_604E
        bc      12,2,ccdataon_604
        cmpli   0,0,r3,CPU_TYPE_604R
        bc      12,2,ccdataon_604
	bclr	0x14,0x0		/* invalid cpu type */

ccdataon_603:
	addis	r3,r0,0x0000	/* Setup bit pattern for DCE */
	ori	r3,r3,0x4000

	mfspr	r4,HID0		/* Modify HID0 to enable D cache (DCE) */
	or	r4,r4,r3
	mtspr	HID0,r4
	isync			/* may not be needed - precaution */
	bclr	0x14,0x0	/* return to caller */

ccdataon_604:
	addis	r3,r0,0x0000	/* Setup bit pattern for DCE */
	ori	r3,r3,0x4000

	mfspr	r4,HID0		/* Modify HID0 to enable D cache (DCE) */
	or	r4,r4,r3
	mtspr	HID0,r4
	isync			/* may not be needed - precaution */
	bclr	0x14,0x0	/* return to caller */


/******************************************************************************
*
* dCacheOff - Turn Data Cache Off
*
* void dCacheOff (void)
*/
_dCacheOff:
dCacheOff:
	/* Get cpu type */

	mfspr	r3,PVR
	rlwinm	r3,r3,16,16,31

	cmpli	0,0,r3,CPU_TYPE_603
	bc	12,2,ccdataoff_603
	cmpli	0,0,r3,CPU_TYPE_604
	bc	12,2,ccdataoff_604
	cmpli	0,0,r3,CPU_TYPE_603E
	bc	12,2,ccdataoff_603
        cmpli   0,0,r3,CPU_TYPE_603P
        bc      12,2,ccdataoff_603
        cmpli   0,0,r3,CPU_TYPE_750
        bc	12,2,ccdataoff_603
        cmpli   0,0,r3,CPU_TYPE_604E
        bc      12,2,ccdataoff_604
        cmpli   0,0,r3,CPU_TYPE_604R
        bc      12,2,ccdataoff_604
	bclr	0x14,0x0		/* invalid cpu type */

ccdataoff_603:
	addis	r3,r0,0x0000	/* Setup bit pattern for DCE */
	ori	r3,r3,0x4000	

	mfspr	r4,HID0		/* Modify HID0 to disable D cache (DCE) */
	andc	r4,r4,r3
	mtspr	HID0,r4
	isync			/* may not be needed - precaution */
	bclr	0x14,0x0	/* return to caller */

ccdataoff_604:
	addis	r3,r0,0x0000	/* Setup bit pattern for DCE */
	ori	r3,r3,0x4000

	mfspr	r4,HID0		/* Modify HID0 to disable D cache (DCE) */
	andc	r4,r4,r3
	mtspr	HID0,r4
	isync			/* may not be needed - precaution */
	bclr	0x14,0x0	/* return to caller */


/******************************************************************************
*
* dCacheInval - Invalidate Data Cache
*
* void dCacheInval (void)
*/
_dCacheInval:
dCacheInval:
	/* Get cpu type */

	mfspr	r3,PVR
	rlwinm	r3,r3,16,16,31

	cmpli	0,0,r3,CPU_TYPE_603
	bc	12,2,ccdatainvl_603
	cmpli	0,0,r3,CPU_TYPE_604
	bc	12,2,ccdatainvl_604
	cmpli	0,0,r3,CPU_TYPE_603E
	bc	12,2,ccdatainvl_603
        cmpli   0,0,r3,CPU_TYPE_603P
        bc      12,2,ccdatainvl_603
        cmpli   0,0,r3,CPU_TYPE_750
        bc	12,2,ccdatainvl_603
        cmpli   0,0,r3,CPU_TYPE_604E
        bc      12,2,ccdatainvl_604
        cmpli   0,0,r3,CPU_TYPE_604R
        bc      12,2,ccdatainvl_604
	bclr	0x14,0x0		/* invalid cpu type */

/*
 * To invalidate the Data Cache on a 603/750, it's necessary
 * to toggle the DCFI bit.
 */

ccdatainvl_603:
	addis	r3,r0,0x0000	/* Setup bit pattern for DCFI */
	ori	r3,r3,0x0400

	mfspr	r4,HID0		/* Modify HID0 to SET DCFI bit */
	or	r4,r4,r3
	mtspr	HID0,r4
	isync			/* may not be needed - precaution */

	andc	r4,r4,r3	/* Modify HID0 to CLEAR DCFI bit */
	mtspr	HID0,r4
	isync			/* may not be needed - precaution */
	bclr	0x14,0x0	/* return to caller */

/*
 * To invalidate the Data Cache on a 604, it's necessary
 * to toggle the DCFI bit while the Data Cache is enabled (DCE).
 * It is also necessary to delay between setting and clearing DCFI.
 */

ccdatainvl_604:
	addis	r3,r0,0x0000	/* Setup bit pattern for DCFI + DCE */
	ori	r3,r3,0x4400

	mfspr	r4,HID0		/* Modify HID0 to SET DCFI + DCE bits */
	or	r4,r4,r3
	mtspr	HID0,r4
	isync			/* may not be needed - precaution */

	addis	r5,r0,0x0000		/* Setup for small delay */
	ori	r5,r5,0x1000
	mtspr	CTR,r5			/* Load PPC Counter reg */
ccdatainvl_604_d:
	nop
	bdnz	ccdatainvl_604_d	/* branch till counter reaches zero */

	andc	r4,r4,r3	/* Modify HID0 to CLEAR DCFI + DCE bits */
	mtspr	HID0,r4
	isync			/* may not be needed - precaution */
	bclr	0x14,0x0	/* return to caller */

