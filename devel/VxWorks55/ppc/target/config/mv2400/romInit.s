/* romInit.s - Motorola MVME2400 ROM initialization module */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-1999 Motorola, Inc. All Rights Reserved */
	.data
	.globl  copyright_wind_river
	.long   copyright_wind_river

/*
modification history
--------------------
01n,19sep01,dat  removing sysDebugMsg
01m,22jun01,pch  Make sysHid1Get() available in bootrom
01l,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01k,19nov99,srr  Added the ability to come up if VME_A32_MSTR_LOCAL is
                 less than the total DRAM size.
01j,12may99,rhv  Adding a work-around for an errata related to the Hawk's
                 Watchdog Timers.
01i,28jan99,rhv  Fixing a bug in dCacheOff and a file history typo.
01h,28jan99,rhv  Cleaning up cache configuration code.
01g,26jan99,rhv  Moving default SMC parameter build option to hawkSmc.c.
01f,24jan99,rhv  Making ECC a build option (vs always enabled).
01e,22jan99,rhv  Adding ROM startup error reporting.
01d,20jan99,rhv  Removing legacy processor support and moving Hawk
                 setup to hawkPhb.c.
01c,13jan99,rhv  Re-ordering file history.
01b,13jan99,rhv  Added SPD-based Hawk SMC configuration, moved start type to
                 r31 to survive C code calls and updated copyright.
01a,15dec98,mdp  Written. (from mv2304/romInit.s, ver 01m)
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
	FUNC_EXPORT(sysHid1Get)

	/* externals */

	.extern romStart	/* system initialization routine */
        .extern sysHawkParamConfig	/* hawk smc parameter initialization */

	/* hawk SMC register save area offsets */

        .set    AttrA,0
        .set    BaseA,AttrA+4
        .set    ClkFreq,BaseA+4
        .set    AttrE,ClkFreq+4
        .set    BaseE,AttrE+4
        .set    Ctrl,BaseE+4
        .set    Size,Ctrl+4
	
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

	.ascii   "Copyright 1984-1997 Wind River Systems, Inc."
	.align 2

cold:
	li	r31, BOOT_COLD
	bl	start		/* skip over next instruction */

			
warm:
	or	r31, r3, r3	/* startType to r31 */

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
	 *	Enable machine check input pin (EMCP)
         *      Disable instruction and data caches
	 */

	addis   r3,r0,0x8000
	ori     r3,r3,0x0000
	sync			/* required before changing DCE */
	isync			/* required before chaning ICE */
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

	/* Get cpu type */

	mfspr   r28, PVR
	rlwinm  r28, r28, 16, 16, 31

	/* invalidate the MPU's data/instruction caches */

	cmpli   0, 0, r28, CPU_TYPE_750
	beq     CPU_IS_750
	b	CACHE_ENABLE_DONE

CPU_IS_750:
#ifdef USER_I_CACHE_ENABLE
        mfspr   r3,HID0
	rlwinm	r3,r3,0,19,17	/* clear ILOCK (bit 18) */
        ori     r3,r3,(_PPC_HID0_ICFI | _PPC_HID0_ICE)
	isync	/* required before changing ICE */
        mtspr   HID0,r3         /* set ICFI (bit 20) and ICE (bit 16) */

#endif
CACHE_ENABLE_DONE:

/* 
 * Disable Hawk's Watchdog Timers.
 *
 * Note: Both of Hawk's Watchdog timers must be disabled at powerup.
 * Otherwise Watchdog Timer 1 will time out in 8 Sec and interrupt the 
 * board, Watchdog Timer 2 will time out in 16 Sec and reset the board.
 */
        lis     r3,HI(HAWK_PHB_BASE_ADRS)
        ori     r3, r3, LO(HAWK_PHB_BASE_ADRS)

/* Hawk WDT errata */

	lbz	r5,HAWK_MPC_MARB(r3)	/* save arbiter configuration */
	li	r0,0xa0			/* load pattern to flatten reads */
	sync
	stb	r0,HAWK_MPC_MARB(r3)	/* configure 60x bus arbiter */
	sync

/* End Hawk WDT Errata */

	li	r4,0x55				/* load unlock pattern */
	stb	r4,HAWK_MPC_WDT1CNTL(r3)	/* unlock WDT 1 */
	stb	r4,HAWK_MPC_WDT2CNTL(r3)	/* unlock WDT 2 */
	sync

	lis	r4,0xaa0f			/* load WDT disable */
	ori	r4,r4,0xffff
	stw	r4,HAWK_MPC_WDT1CNTL(r3)	/* disable WDT 1 */
	stw	r4,HAWK_MPC_WDT2CNTL(r3)	/* disable WDT 2 */
	sync

/* Hawk WDT Errata */

	stb	r5,HAWK_MPC_MARB(r3)	/* restore 60x bus arbiter */
	sync

/* End Hawk WDT Errata */

/*
 * Clear the RomStartup error flags.
 */

	lis	r3,HI(HAWK_PHB_BASE_ADRS)
	ori	r3,r3,LO(HAWK_PHB_BASE_ADRS)
	xor	r4,r4,r4
	stw	r4,0x70(r3)
	stw	r4,0x74(r3)
	stw	r4,0x78(r3)
	stw	r4,0x7c(r3)

/*
 * Hawk System Memory Controller Initialization
 */

hawkSmcInit:
        xor	r0,r0,r0        /* insure r0 is zero */

        lis     r13,0xFEF8      /* load SMC register address */
        ori     r13,r13,0x0000  /* load SMC register address */

        addis   r3,r0,0x0000    /* TBEN=0, IHEN=0 */
        ori     r3,r3,0x0000    /* TBEN=0, IHEN=0 */
        stw     r3,0x08(r13)    /* General Control register */

        addis   r3,r0,0x0100    /* RDIS=0, RWCB=0, DERC=1, xPIEN=0 */
        ori     r3,r3,0x0000    /* MBE_ME=0 */
        stw     r3,0x28(r13)    /* SDRAM ECC Control register */

        addis   r3,r0,0x8000    /* ELOG=1, ESEN=0, SCOF=1 */
        ori     r3,r3,0x0100    /* ELOG=1, ESEN=0, SCOF=1 */
        stw     r3,0x30(r13)    /* Error Logger register */

        addis   r3,r0,0x0000    /* SWEN=0, SCRUB_FREQUENCY=0x00 */
        ori     r3,r3,0x0000    /* SWEN=0, SCRUB_FREQUENCY=0x00 */
        stw     r3,0x40(r13)    /* Scrub/Refresh register */
	eieio
	sync
/*
 * Software must wait for a refresh to occur prior to setting the
 * SDRAM Control Registers. To ensure that at least one refresh has
 * completed allow the 32 bit counter to increment at least 100 times.
 */

        addis   r3,r0,0x0000	/* load counter reset value */
        ori     r3,r3,0x0000	/* load counter reset value */
        stw     r3,0x100(r13)	/* reset 32-Bit counter */
	sync
smc_loop1:
        lwz     r3,0x100(r13)   /* Load the counter value */
        cmpwi   r3,100		/* Check counter value less than 100 */
        blt     smc_loop1	/* loop until refresh complete */

        li      r3,0x00		/* disable all SDRAM banks */
	stw	r3,0x0010(r13)
	stw	r3,0x00c0(r13)

        li      r3,66           /* assume clock frequency is 66 MHz */
        stb     r3,0x0020(r13)  /* to ensure an adequate refresh rate */

	lis	r3,0x1333	/* set SDRAM timing back to default */
	ori	r3,r3,0x1100
        stw     r3,0x00d0(r13)

        li      r3,0x00		/* clear all base addresses */
        stw     r3,0x0018(r13)
        stw     r3,0x00c8(r13)

        li      r3,0x81         /* enable SDRAM bank A, size=4Mx16 (32MB) */
        stb     r3,0x0010(r13)
	eieio			/* force all writes to complete */
	sync

/*
 * Software must wait for a refresh to occur after setting the
 * SDRAM Control Registers. To ensure that at least one refresh has
 * completed allow the 32 bit counter to increment at least 100 times.
 */

        addis   r3,r0,0x0000	/* load counter reset value */
        ori     r3,r3,0x0000	/* load counter reset value */
        stw     r3,0x100(r13)	/* reset 32-Bit counter */
        sync
smc_loop2:
        lwz     r3,0x100(r13)   /* Load the counter value */
        cmpwi   r3,100		/* Check counter value less than 100 */
        blt     smc_loop2	/* loop until refresh complete */

        stwu    sp,-40(sp)	    /* create an ABI stack frame */
        addi    r3,sp,8             /* point to register image area */
	or	r14,r3,r3	    /* save register image address */
        bl      sysHawkParamConfig  /* calculate hawk smc parameters */
	or	r18,r3,r3	    /* setup total DRAM size save register */

        lbz     r4,ClkFreq(r14) /* load the Clock Frequency */
        lwz     r5,Ctrl(r14)    /* load the Sdram Speed Attribute */
        lwz     r6,BaseA(r14)   /* load Base Data (Blocks A/B/C/D) */
        lwz     r7,BaseE(r14)   /* load Base Data (Blocks E/F/G/H) */
        lwz     r8,AttrA(r14)   /* load Attributes (Blocks A/B/C/D) */
        lwz     r9,AttrE(r14)   /* load Attributes (Blocks E/F/G/H) */
        addi    sp,sp,40        /* remove ABI stack frame */

/*
 * Software must wait for a refresh to occur prior to setting the
 * SDRAM Control Registers. To ensure that at least one refresh has
 * completed allow the 32 bit counter to increment at least 100 times.
 */

        addis   r3,r0,0x0000	/* load counter reset value */
        ori     r3,r3,0x0000	/* load counter reset value */
        stw     r3,0x100(r13)	/* reset 32-Bit counter */
	sync
smc_loop3:
        lwz     r3,0x100(r13)   /* Load the counter value */
        cmpwi   r3,100		/* Check counter value less than 100 */
        blt     smc_loop3	/* loop until refresh complete */

        addis   r3,r0,0x0000    /* clear the ram enable bits prior to */
        ori     r3,r3,0x0000    /* intializing sdram attributes */
        stw     r3,0x10(r13)    /* SDRAM Attr Register (Blocks A/B/C/D) */
        stw     r3,0xc0(r13)    /* SDRAM Attr Register (Blocks E/F/G/H) */
	eieio
	sync

/*
 * Software must wait for a refresh to occur after setting the
 * SDRAM Control Registers. To ensure that at least one refresh has
 * completed allow the 32 bit counter to increment at least 100 times.
 */

        addis   r3,r0,0x0000	/* load counter reset value */
        ori     r3,r3,0x0000	/* load counter reset value */
        stw     r3,0x100(r13)	/* reset 32-Bit counter */
	sync
smc_loop4:
        lwz     r3,0x100(r13)   /* Load the counter value */
        cmpwi   r3,100		/* Check counter value less than 100 */
        blt     smc_loop4	/* loop until refresh complete */

/*
 * Set the Clock Frequency register and the SDRAM attributes register
 * to the operating values.
 */

        stb     r4,0x20(r13)	/* Clock Frequency register */

        stw     r5,0xd0(r13)    /* SDRAM Speed Attribute Register */

        stw     r6,0x18(r13)    /* SDRAM Base Register (Blocks A/B/C/D) */

        stw     r7,0xc8(r13)    /* SDRAM Base Register (Blocks E/F/G/H) */

        stw     r8,0x10(r13)    /* SDRAM Attr Register (Blocks A/B/C/D) */

        stw     r9,0xc0(r13)    /* SDRAM Attr Register (Blocks E/F/G/H) */
	eieio
	sync

/*
 * Software must wait for a refresh to occur after setting the
 * SDRAM Control Registers. To ensure that at least one refresh has
 * completed allow the 32 bit counter to increment at least 100 times.
 */

        addis   r3,r0,0x0000	/* load counter reset value */
        ori     r3,r3,0x0000	/* load counter reset value */
        stw     r3,0x100(r13)	/* reset 32-Bit counter */
	sync
smc_loop5:
        lwz     r3,0x100(r13)   /* Load the counter value */
        cmpwi   r3,100		/* Check counter value less than 100 */
        blt     smc_loop5	/* loop until refresh complete */

	or	r3,r18,r18      /* restore memory size */

        xor	r0,r0,r0        /* insure r0 is zero */

#ifdef INCLUDE_ECC
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
.hawk_smc_scrub:

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

        stwu    sp, -64(sp)             /* Create an ABI stack frame */
        bl      dCacheInval
        bl      dCacheOn
        addi    sp,sp,64                /* Remove ABI stack frame */

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
        addi    r17,r15,-8              /* starting address - munged */
        mtspr   9,r16                   /* load number of doubles in counter */
hawk_smc_scrub_iloop:
        lfdu    0,8(r17)
        stfd    0,0(r17)
        bc      16,0,hawk_smc_scrub_iloop /* branch till counter == 0 */

        eieio
        sync
/*
 * Loop through the entire DRAM array again.
 * Looping through the entire DRAM array is not necessary just convenient.
 * What is necessary is flushing the data cache from the previous loop
 * so that the last segment (data cache size) of DRAM is initialized.
 */
        addi    r17,r15,-8              /* starting address - munged */
        mtspr   9,r16                   /* load number of doubles in counter */
hawk_smc_scrub_floop:
        lfdu    0,8(r17)
        bc      16,0,hawk_smc_scrub_floop /* branch till counter == 0 */

        eieio
        sync

/* disable the data cache */

        stwu    sp, -64(sp)             /* Create an ABI stack frame */
        bl      dCacheInval
        bl      dCacheOff
        addi    sp,sp,64                /* Remove ABI stack frame */

/* Restore original value of MSR */

        mtmsr   r14
        isync

/* Clear any possible error conditions that occurred during init */

        lis     r14, HI(HAWK_SMC_BASE_ADRS)
        ori     r14, r14, LO(HAWK_SMC_BASE_ADRS)

        addis   r9,r0,0x8000            /* ELOG=1,ESEN=0,SCOF=1 */
        ori     r9,r9,0x0100
        stw     r9,0x30(r14)            /* Update Error Logger register */
        eieio
        sync

/* Enable ECC and multiple-bit error detection */

        addis   r9,r0,0x0000            /* RWCB=0,DERC=0,xIEN=0,MCKEN=1 */
        ori     r9,r9,0x0001
        stw     r9,0x28(r14)            /* Update ECC Control register */
        eieio
        sync

#endif /* End of ECC Init */

#ifdef EXTENDED_VME
/*
 * Verify that the end of DRAM does not overlap VME_A32_MSTR_LOCAL.
 * If it does, set MSADD0 to the size of DRAM.  A message will be displayed
 * in sysPhysMemTop prompting the user to increase VME_A32_MSTR_LOCAL.
 * 
 * The size of DRAM is saved in r18.
 */
        lis     r3, HI(HAWK_PHB_BASE_ADRS)
        ori     r3, r3, LO(HAWK_PHB_BASE_ADRS)

	lis	r5, HI(VME_A32_MSTR_LOCAL)
	ori	r5, r5, LO(VME_A32_MSTR_LOCAL)

	cmpl	0,0,r18,r5		/* If r18 < VME_A32_MSTR_LOCAL */
	bc	12,0,memMapGood		/* Set MSADD0 to DRAM size */

	ori	r18,r18,CPU2PCI_ADDR0_END	/* MSADD0: */
	stw	r18,HAWK_MPC_MSADD0(r3)	/* write MSADD0 w DRAM size */

	b	doneMSAddr0Calc

memMapGood:
	addis	r4,r0,(VME_A32_MSTR_LOCAL>>16)	/* MSADD0: */
	ori	r4,r4,CPU2PCI_ADDR0_END		/* MSADD0: */
	stw	r4,HAWK_MPC_MSADD0(r3)		/* write MSADD0 */
	
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

	or	r3, r31, r31
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

        cmpli   0,0,r3,CPU_TYPE_750
        bc	12,2,ccdataon
	bclr	0x14,0x0		/* invalid cpu type */

ccdataon:
	mfspr	r4,HID0		/* Modify HID0 to enable D cache (DCE) */
	ori	r4,r4,_PPC_HID0_DCE
	sync			/* required before changing DCE */
	mtspr	HID0,r4
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

        cmpli   0,0,r3,CPU_TYPE_750
        bc	12,2,ccdataoff
	bclr	0x14,0x0		/* invalid cpu type */

ccdataoff:
	mfspr	r4,HID0		/* Modify HID0 to disable D cache (DCE) */
	rlwinm	r4,r4,0,_PPC_HID0_BIT_DCE+1,_PPC_HID0_BIT_DCE-1	/* clear DCE */
	sync			/* required before changing DCE */
	mtspr	HID0,r4
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

        cmpli   0,0,r3,CPU_TYPE_750
        bc	12,2,ccdatainvl
	bclr	0x14,0x0		/* invalid cpu type */

/*
 * To invalidate the Data Cache on a 750, it's necessary
 * to set the DCFI bit while the Data Cache is enabled (DCE).
 */

ccdatainvl:
	addis	r3,r0,0x0000	/* Setup bit pattern for DCFI + DCE */
	ori	r3,r3,(_PPC_HID0_DCE | _PPC_HID0_DCFI)

	mfspr	r4,HID0		/* Modify HID0 to SET DCFI + DCE bits */
	or	r4,r4,r3
	sync			/* required before changing DCE */
	mtspr	HID0,r4

	andc	r4,r4,r3	/* Modify HID0 to CLEAR DCFI + DCE bits */
	sync			/* required before changing DCE */
	mtspr	HID0,r4
	bclr	0x14,0x0	/* return to caller */

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

FUNC_BEGIN(sysHid1Get)
	mfspr r3,HID1
	bclr 20,0
FUNC_END(sysHid1Get)

