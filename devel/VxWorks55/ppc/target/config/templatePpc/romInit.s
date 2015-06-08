/* romInit.s - general PPC 603/604 ROM initialization module */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01g,17jul02,dat  remove obsolete information
01f,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01e,07nov98,dat  added padding between cold and warm entries. Added note
		 about using eieio. (SPR 22615)
01d,12jun98,dat  added #def for LOADPTR, LOADVAR for old systems with bad
		 definitions in h/arch/ppc/toolPpc.h
01c,22dec97,dat  SPR 20104, using LOADPTR to load 32 constants.
01b,21aug97,dat  added note about romInit8xx.s and romInit4xx.s,
		 code review comments from Thierry M.
01a,08jul97,dat  written
*/

/*
TODO - Update documentation as necessary.

NOTICE
This template is generic only for the 603/604 versions of PPC.  The
820/860 and 403/401 CPUs have PPC cores with other integrated
elements.  Templates for those processors are included in this directory
as romInit8xx.s and romInit4xx.s.  Please substitute the appropriate
file for this file, as needed.

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

WARNING:
For 603 and some other PowerPC processors, the eieio instruction is a no-op.
To achieve synchronization while the CPU is operating in real-access mode (the bootup default) use the 'sync' instruction to prevent re-ordering of I/O operations.  When the MMU is turned on, this behaviour changes and accesses to uncached spaces can not re-ordered.  This only applies from boot time until the MMUs are turned on (mmuLibInit).
*/

#define	_ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "asm.h"
#include "config.h"
#include "regs.h"	

/* defines */

/*
 * Some releases of h/arch/ppc/toolPpc.h had a bad definition of
 * LOADPTR. So we will define it correctly. [REMOVE WHEN NO LONGER NEEDED].
 *
 * LOADPTR initializes a register with a 32 bit constant, presumably the
 * address of something.
 */

#undef LOADPTR
#define	LOADPTR(reg,const32) \
	  addis reg,r0,HIADJ(const32); addi reg,reg,LO(const32)

/*
 * LOADVAR initializes a register with the contents of a specified memory
 * address. The difference being that the value loaded is the contents of
 * the memory location and not just the address of it. [REMOVE WHEN NO LONGER
 * NEEDED].
 */

#undef LOADVAR
#define	LOADVAR(reg,addr32) \
	  addis reg,r0,HIADJ(addr32); lwz reg,LO(addr32)(reg)


/* Can't use ROM_ADRS macro with HIADJ and LO macro functions, for PPC */

	/* Exported internal functions */

	.data
	FUNC_EXPORT(_romInit)		/* start of system code */
	FUNC_EXPORT(romInit)		/* start of system code */
	FUNC_EXPORT(_romInitWarm)	/* start of system code */
	FUNC_EXPORT(romInitWarm)	/* start of system code */

	/* externals */

	FUNC_IMPORT(romStart)		/* system initialization routine */

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

FUNC_BEGIN(_romInit)
FUNC_LABEL(romInit)

	/* This is the cold boot entry (ROM_TEXT_ADRS) */

	bl	cold
	nop
	
	/*
	 * This warm boot entry point is defined as ROM_WARM_ADRS in config.h.
	 * It is defined as an offset from ROM_TEXT_ADRS.  Please make sure
	 * that the offset from _romInit to romInitWarm matches that specified
	 * in config.h
	 */

FUNC_LABEL(_romInitWarm)
FUNC_LABEL(romInitWarm)
	bl	warm

	/* copyright notice appears at beginning of ROM (in TEXT segment) */

	.ascii   "Copyright 1984-2002 Wind River Systems, Inc."
	.align 2

cold:
	li	r11, BOOT_COLD
	bl	start		/* skip over next instruction */

			
warm:
	or	r11, r3, r3	/* startType to r11 */

start:
	/* Zero-out registers */

	addis   r0,r0,0
	mtspr   SPRG0,r0
	mtspr   SPRG1,r0
	mtspr   SPRG2,r0
	mtspr   SPRG3,r0

        /* initialize the stack pointer */

	LOADPTR (sp, STACK_ADRS)

	/* Set MPU/MSR to a known state. Turn on FP */

	LOADPTR (r3, _PPC_MSR_FP)
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

	/* Set MPU/MSR to a known state. Turn off FP */

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

	/* TODO - disable external caches */

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
	cmpli	0, 0, r28, CPU_TYPE_603
	beq	cpuIs603
	cmpli	0, 0, r28, CPU_TYPE_603E
	beq	cpuIs603
        cmpli   0, 0, r28, CPU_TYPE_603P
        beq     cpuIs603
        cmpli   0, 0, r28, CPU_TYPE_604R
        bne     cpuNot604R

cpuIs604R:
        lis     r3, 0x0
        mtspr   HID0, r3        	/* disable the caches */
        isync
        ori     r4, r4, 0x0002  	/* disable BTAC by setting bit 30 */

cpuNot604R:
        ori     r3, r3, (_PPC_HID0_ICFI | _PPC_HID0_DCFI) 

cpuIs603:
        ori     r3, r3, (_PPC_HID0_ICE | _PPC_HID0_DCE) 
	or	r4, r4, r3		/* set bits */
	sync
	isync
	mtspr   HID0, r4		/* HID0 = r4 */
	andc	r4, r4, r3		/* clear bits */
	isync
	cmpli   0, 0, r28, CPU_TYPE_604
	beq	cpuIs604
	cmpli   0, 0, r28, CPU_TYPE_604E
	beq	cpuIs604
        cmpli   0, 0, r28, CPU_TYPE_604R
        beq     cpuIs604
	mtspr	HID0, r4
	isync

#ifdef USER_I_CACHE_ENABLE 
	b	instCacheOn603
#else
	b	cacheEnableDone
#endif

cpuIs604:
	LOADPTR (r5, 0x1000)		/* loop count, 0x1000 */
	mtspr	CTR, r5
loopDelay:
	nop
	bdnz	loopDelay
	isync
	mtspr 	HID0, r4
	isync

	/* turn the Instruction cache ON for faster FLASH ROM boots */

#ifdef USER_I_CACHE_ENABLE 

        ori     r4, r4, (_PPC_HID0_ICE | _PPC_HID0_ICFI) 
        isync                           /* Synchronize for ICE enable */
	b	writeReg4
instCacheOn603:
        ori     r4, r4, (_PPC_HID0_ICE | _PPC_HID0_ICFI) 
        rlwinm  r3, r4, 0, 21, 19	/* clear the ICFI bit */

        /*
         * The setting of the instruction cache enable (ICE) bit must be
         * preceded by an isync instruction to prevent the cache from being
         * enabled or disabled while an instruction access is in progress.
         */
	isync
writeReg4:
        mtspr   HID0, r4                /* Enable Instr Cache & Inval cache */
	cmpli   0, 0, r28, CPU_TYPE_604
	beq    	cacheEnableDone 
	cmpli   0, 0, r28, CPU_TYPE_604E
	beq    	cacheEnableDone 

        mtspr   HID0, r3                /* using 2 consec instructions */
                                        /* PPC603 recommendation */
#endif
cacheEnableDone:

	/* TODO - setup memory controllers */

	/* TODO - setup other necessary controllers */

#if	FALSE	/* EABI SDA not supported yet */

        /* initialize r2 and r13 according to EABI standard */

	LOADPTR (r2, _SDA2_BASE_)
	LOADPTR (r13, _SDA_BASE_)
#endif

	/* go to C entry point */

	or	r3, r11, r11		/* put startType in r3 (p0) */
	addi	sp, sp, -FRAMEBASESZ	/* save one frame stack */

	LOADPTR (r6, romStart)
	LOADPTR (r7, romInit)
	LOADPTR (r8, ROM_TEXT_ADRS)

	sub	r6, r6, r7
	add	r6, r6, r8 

	mtlr	r6		/* romStart - romInit + ROM_TEXT_ADRS */
	blr
FUNC_END(_romInit)
