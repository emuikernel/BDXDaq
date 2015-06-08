/* sysALib8xx.s - generic PPC 8xx system-dependent assembly routines */

/* Copyright 1984-1997 Wind River Systems, Inc. */
        .data
	.globl	copyright_wind_river
	.long	copyright_wind_river

/*
modification history
--------------------
01c,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01b,23dec97,dat  changed initial sp to _sysInit
01b,23dec97,dat  SPR 20104, use LOADPTR for 32 bit constants
01a,25aug97,dat  written (from ads860/sysALib.s, ver 01c)
*/

/*
TODO - Update documentation as necessary.

NOTICE
This template is specific to the PPC 8xx (821, 823, 860) family of CPUs.

DESCRIPTION
This module contains the entry code, sysInit(), for VxWorks images that start
running from RAM, such as 'vxWorks'. These images are loaded into memory
by some external program (e.g., a boot ROM) and then started.
The routine sysInit() must come first in the text segment. Its job is to perform
the minimal setup needed to call the generic C
routine usrInit() with parameter BOOT_COLD.

The routine sysInit() typically masks interrupts in the processor, sets the
initial stack pointer to _sysInit then jumps to usrInit.
Most other hardware and device initialization is performed later by
sysHwInit().
*/

#define _ASMLANGUAGE
#include "vxWorks.h"
#include "asm.h"
#include "cacheLib.h"
#include "config.h"
#include "regs.h"	
#include "sysLib.h"

	/* globals */

	FUNC_EXPORT(_sysInit)			/* start of system code */
	FUNC_EXPORT(_sysMemProbeSup)		/* memProbe support rtn */
	FUNC_EXPORT(sysMemProbeSup)		/* memProbe support rtn */

	/* externals */

	FUNC_IMPORT(usrInit)
	
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

FUNC_BEGIN(_sysInit)
	/*
	 * disable external interrupts and Instruction/Data MMU, set
	 * the exception prefix 
	 */

        mfmsr   p0                      /* p0 = msr    */
        INT_MASK(p0, p1)                /* mask EE bit */
	rlwinm	p1, p1, 0, _PPC_MSR_BIT_DR + 1, _PPC_MSR_BIT_IR - 1
	rlwinm  p1, p1, 0, _PPC_MSR_BIT_IP + 1, _PPC_MSR_BIT_IP - 1
        mtmsr   p1                      /* msr = p1    */
	isync				/* ISYNC */

	/* disable instruction and data caches */

	LOADPTR (p1, CACHE_CMD_DISABLE)
	mtspr	IC_CST, p1				/* Disable I cache */
	mtspr	DC_CST, p1				/* Disable D cache */

	/* unlock instruction and data caches */

	LOADPTR (p1, CACHE_CMD_UNLOCK_ALL)
        mtspr   IC_CST, p1				/* Unlock I cache */
        mtspr   DC_CST, p1				/* Unlock D cache */

	/* invalidate instruction and data caches */

	LOADPTR (p1, CACHE_CMD_INVALIDATE)
        mtspr   IC_CST, p1				/* Invalidate I cache */
        mtspr   DC_CST, p1				/* Invalidate D cache */

	/* invalidate entries within both TLBs */

	tlbia

	/* TODO - reset all devices */

	/* initialize Small Data Area (SDA) start address */

#if	FALSE				/* SDA not supported yet */
	LOADPTR (r2, _SDA2_BASE_)
	LOADPTR (r13, _SDA_BASE_)
#endif

	/* initialize the stack pointer */
	
	LOADPTR (sp, _sysInit)
	addi	sp, sp, -FRAMEBASESZ	/* get frame stack */

	/* set the default boot code */
	
	LOADPTR (r3, BOOT_WARM_AUTOBOOT)

	/* jump to usrInit */

	b	usrInit			/* never returns - starts up kernel */
FUNC_END(_sysInit)

/*******************************************************************************
*
* sysMemProbeSup - sysBusProbe support routine
*
* This routine is called to try to read byte, word, or long, as specified
* by length, from the specified source to the specified destination.
*
* RETURNS: OK if successful probe, else ERROR

* STATUS sysMemProbeSup
*    (
*    int         length, /@ length of cell to test (1, 2, 4, 8, 16) @/
*    char *      src,    /@ address to read @/
*    char *      dest    /@ address to write @/
*    )

*/

FUNC_BEGIN(_sysMemProbeSup)
FUNC_LABEL(sysMemProbeSup)
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
FUNC_END(_sysMemProbeSup)
