/* romInit4xx.s - generic PPC 4XX ROM initialization module */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01d,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01c,07nov98,dat  added padding between cold and warm entries
01b,23dec97,dat  SPR 20104, use LOADPTR for 32 bit constants
01a,21aug97,dat  written (from evb403/romInit.s, ver 01k)
*/

/*
TODO - Update documentation as necessary.

NOTICE
This template is generic only for the 4xx versions of PPC (403ga, 403gc,
and 403gcx).

DESCRIPTION
This module contains the entry code for VxWorks images that start
running from ROM, such as 'bootrom' and 'vxWorks_rom'.
The entry point, romInit(), is the first code executed on power-up.
It performs the minimal setup needed to call
the generic C routine romStart() with parameter BOOT_COLD.

RomInit() typically masks interrupts in the processor, sets the initial
stack pointer (to STACK_ADRS which is defined in configAll.h), and
readies system memory by configuring the DRAM controller if necessary.
Other hardware and device initialization is performed later in the
BSP's sysHwInit() routine.

A second entry point in romInit.s is called romInitWarm(). It is called
by sysToMonitor() in sysLib.c to perform a warm boot.
The warm-start entry point must be written to allow a parameter on
the stack to be passed to romStart().

WARNING:
This code must be Position Independent Code (PIC).  This means that it
should not contain any absolute address references.  If an absolute address
must be used, it must be relocated by the macro ROM_ADRS(x).  This macro
will convert the absolute reference to the appropriate address within
ROM space no matter how the boot code was linked. (For PPC, ROM_ADRS does
not work.  You must subtract _romInit and add ROM_TEXT_ADRS to each
absolute address). (NOTE: ROM_ADRS(x) macro does not work for current
PPC compiler).

This code should not call out to subroutines declared in other modules,
specifically sysLib.o, and sysALib.o.  If an outside module is absolutely
necessary, it can be linked into the system by adding the module 
to the makefile variable BOOT_EXTRA.  If the same module is referenced by
other BSP code, then that module must be added to MACH_EXTRA as well.
Note that some C compilers can generate code with absolute addresses.
Such code should not be called from this module.  If absolute addresses
cannot be avoided, then only ROM resident code can be generated from this
module.  Compressed and uncompressed bootroms or VxWorks images will not
work if absolute addresses are not processed by the macro ROM_ADRS.

WARNING:
The most common mistake in BSP development is to attempt to do too much
in romInit.s.  This is not the main hardware initialization routine.
Only do enough device initialization to get memory functioning.  All other
device setup should be done in sysLib.c, as part of sysHwInit().

Unlike other RTOS systems, VxWorks does not use a single linear device
initialization phase.  It is common for inexperienced BSP writers to take
a BSP from another RTOS, extract the assembly language hardware setup
code and try to paste it into this file.  Because VxWorks provides 3
different memory configurations, compressed, uncompressed, and rom-resident,
this strategy will usually not work successfully.

WARNING:
The second most common mistake made by BSP writers is to assume that
hardware or CPU setup functions done by romInit.o do not need to be
repeated in sysALib.s or sysLib.c.  A vxWorks image needs only the following
from a boot program: The startType code, and the boot parameters string
in memory.  Each VxWorks image will completely reset the CPU and all
hardware upon startup.  The image should not rely on the boot program to
initialize any part of the system (it may assume that the memory controller
is initialized).

This means that all initialization done by romInit.s must be repeated in
either sysALib.s or sysLib.c.  The only exception here could be the
memory controller.  However, in most cases even that can be
reinitialized without harm.

Failure to follow this rule may require users to rebuild bootrom's for
minor changes in configuration.  It is WRS policy that bootroms and vxWorks
images should not be linked in this manner.
*/

	.data

#define	_ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "asm.h"
#include "config.h"
#include "regs.h"	

/* Can't use ROM_ADRS macro with HIADJ and LO macro functions, for PPC */

/* Setup default values for MACROS */

#ifndef _DCACHE_LINE_NUM_GCX
#   define _DCACHE_LINE_NUM_GCX	256
#endif /* _DCACHE_LINE_NUM_GCX*/

#ifndef _ICACHE_LINE_NUM_GCX
#   define _ICACHE_LINE_NUM_GCX	512
#endif /* _ICACHE_LINE_NUM_GCX*/

#ifndef SYS_CLK_FREQ
#   define SYS_CLK_FREQ	 25000000	/* 25 MHz */
#endif /* SYS_CLK_FREQ */


	/* internals */

	FUNC_EXPORT(romInit)		/* start of system code */
	FUNC_EXPORT(_romInit)		/* start of system code */
	FUNC_EXPORT(romWarmInit)	/* warm start of system code */
	FUNC_EXPORT(_romWarmInit)	/* warm start of system code */

	/* externals */

	FUNC_IMPORT(romStart)		/* system initialization routine */

	.text
	.align 2

/*******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*

* romInit
*     (
*     int startType	/@ only used by 2nd entry point @/
*     )

*/

FUNC_BEGIN(romInit)
FUNC_LABEL(_romInit)
	bl	cold
	nop

	/* warm reboot entry point */

FUNC_LABEL(romInitWarm)
FUNC_LABEL(_romInitWarm)
	bl	warm

	/* copyright notice appears at beginning of ROM (in TEXT segment) */

	.ascii   "Copyright 1984-1998 Wind River Systems, Inc."
	.align 4

cold:
	li	p5, BOOT_COLD
	bl	start			/* skip over next instruction */
			
warm:
	or	p5, p0, p0		/* startType to p5 */

start:
	/* disable external interrupts */

	mfmsr   p0			/* p0 = msr    */
	INT_MASK (p0, p1)		/* mask EE bit */
	mtmsr   p1			/* msr = p1    */
	isync

	/* initialize bank registers (BR) */

	LOADPTR (p0, FLASH_BR_CONFIG)
	mtdcr   BR0, p0			/* BR0 : flash 29f040 */

	/* main DRAM is mapped to bank 7 */

        li      p0, HIADJ(LOCAL_MEM_SIZE)
        srawi   p0, p0, 4		/* p0 = LOCAL_MEM_SIZE in MBytes */
        li      p1, -1
loop:
        srawi   p0, p0, 1
        addi    p1, p1, 1		/* LOCAL_MEM_SIZE (MBytes) = 2^(p1) */
        cmpi    0, 0, p0, 0
        bne     loop			/* set the Bank Register BS field: */
        rlwinm  p1, p1, 21, 8, 10	/* p1 = p1 << 21 */
        oris    p1, p1, HIADJ(DRAM_BR_CONFIG_COM)

#if 	((SYS_CLK_FREQ == 25000000) || (SYS_CLK_FREQ == 50000000))

        ori     p1, p1 , LO(DRAM_BR_CONFIG_25MHZ)

#else   /* 33 MHz or 66 Mhz*/

        ori     p1, p1 , LO(DRAM_BR_CONFIG_33MHZ)

#endif	/* SYS_CLK_FREQ == 25000000 */

        mtdcr   BR7, p1                 /* BR7 : <LOCAL_MEM_SIZE> MB of DRAM */

	/* TODO - map external devices to appropriate banks */

	/* TODO - invalidate unused banks */

	LOADPTR (p0, BR_INVAL_VAL)
	mtdcr   BR1, p0
	mtdcr   BR2, p0
	mtdcr   BR3, p0
	mtdcr   BR4, p0
	mtdcr   BR5, p0
	mtdcr   BR6, p0

        li      p1,0
	mtdcr   BESR, p1		/* clear Bus Error Syndrome register */
	mtspr	ESR, p1			/* clear exception syndrome register */

	li 	p1, -1			/* set p1 to 0xffffffff */
	mtspr	DBSR, p1		/* clear dbsr */
	mtdcr	EXISR, p1		/* clear all pending interrupts */

	li	p0, 0
	mtxer	p0			/* clear XER */

	li      r0, 0
	lwarx   p0, r0, r0      	/* get some data/set resv bit */
	stwcx.  p0, r0, r0		/* store out and clear resv bit */

        /* Enable core clocking for PPC403GCX - no harm for GA & GC */

	mfdcr	p1, IOCR
	ori	p1, p1, _PPC403_IOCR_2XC
	ori	p1, p1, _PPC403_IOCR_DRCM
	mtdcr	IOCR, p1

	/* rom403ICReset - reset the instruction cache */

	mfpvr	p0
	rlwinm	p0, p0, 24, 24, 31	/* use bits 16:23 of PVR */
	cmpi	cr0, 0, p0, _PVR_CONF_403GCX
	bne	rom403Inotgcx

	li	p0, 0			/* clear p0 */
					/*load number of cache lines*/
	li	p1, _ICACHE_LINE_NUM_GCX 
	mtctr	p1
	b	rom403Icci

rom403Inotgcx:
	li	p0, 0			/* clear p0 */
	li	p1, _ICACHE_LINE_NUM	/* load number of cache lines to p0 */
	mtctr	p1

rom403Icci:
	iccci   r0, p0
	addi	p0, p0, _CACHE_ALIGN_SIZE /* bump to next line */
	bdnz	rom403Icci		/* go to invalidate */

	/* rom403DCReset - reset the data cache */

	mfpvr	p0
	rlwinm	p0, p0, 24, 24, 31	/* use bits 16:23 of PVR */
	cmpi	cr0, 0, p0, _PVR_CONF_403GCX
	bne	rom403Dnotgcx

	li	p0, 0			/* clear p0 */
	li	p1, _DCACHE_LINE_NUM_GCX        /* load number of cache lines */
	mtctr	p1
	b	rom403Dcci

rom403Dnotgcx:
	li	p0, 0			/* clear p0 */
	li	p1, _DCACHE_LINE_NUM	/* load number of cache lines to p0 */
	mtctr	p1

rom403Dcci:
	dccci	r0, p0
	addi	p0, p0, _CACHE_ALIGN_SIZE /* bump to next line */
	bdnz	rom403Dcci		/* go to invalidate */

        /* 
	 * turn instruction cache on for faster boot-up
	 * adress space 0x00000000-0x07ffffff is cached
	 * adress space 0xf8000000-0xffffffff is cached
	 */

	LOADPTR (p0, _PPC403_ICCR_DEFAULT_VAL)
        mtspr   _PPC403_ICCR, p0
        isync

	/* timer initialization */

	li	p0, -1 			/* p0 = 0xffffffff */
	mtspr	TSR, p0			/* clear timer status */
	mtdcr 	DMASR, p0		/* clear dma status */

	li	p0, 0
	mtspr	TCR, p0			/* disable timers */
	mtspr	PIT, p0

	/* PPC403GC & GCX initialization no harm for GA */

	addi	r4,r0,0x0000
	mtspr	SGR,r4
	mtspr	DCWR,r4

	/* initialize the stack pointer */

	LOADPTR (sp, STACK_ADRS)

#if     FALSE                           /* SDA not supported  */
        /* initialize r2 and r13 according to EABI standard */

	LOADPTR (r2, _SDA2_BASE_)
	LOADPTR (r13, _SDA_BASE_)
#endif

	/* calculate C entry point: (routine - romInit + ROM base */

	LOADPTR (p1, romStart)
	LOADPTR (p2, romInit)
	LOADPTR (p3, ROM_TEXT_ADRS)

	subf	p1, p2, p1
	add	p1, p1, p3
	mtlr	p1			/* link register = C entry point */

	or	p0, p5, p5		/* p0 = startType */
	addi	sp, sp, -FRAMEBASESZ	/* get frame stack */

	blr				/* branch to link register */
FUNC_END(romInit)
