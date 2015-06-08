/* sysALib403.s - generic PPC 403 system-dependent assembly routines */

/* Copyright 1984-1997 Wind River Systems, Inc. */
/* .globl	_copyright_wind_river */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01d,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01c,23dec97,dat  changed initial sp to _sysInit
01b,23dec97,dat  SPR 20104, use LOADPTR for 32 bit constants
01a,21aug97,dat  written (from evb403/sysALib.s,ver 01q)
*/

/*
TODO - Update documentation as necessary.

NOTICE
This template is specific to the PPC 40x (403ga,gc,gcx) family of CPUs.

DESCRIPTION
This module contains the entry code, sysInit(), for VxWorks images that start
running from RAM, such as 'vxWorks'. These images are loaded into memory
by some external program (e.g., a boot ROM) and then started.
The routine sysInit() must come first in the text segment. Its job is to perform
the minimal setup needed to call the generic C
routine usrInit() with parameter BOOT_COLD.

The routine sysInit() typically masks interrupts in the processor, sets the
initial stack pointer to _sysInit, then jumps to usrInit.
Most other hardware and device initialization is performed later by
sysHwInit().
*/

#define _ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "asm.h"
#include "regs.h"

	/* globals */

	FUNC_EXPORT(_sysInit)			/* start of system code */
        FUNC_EXPORT(sysFlashWriteEnable)
        FUNC_EXPORT(sysFlashWriteDisable)
        FUNC_EXPORT(sysMemProbeSup)

	/* externals */

	FUNC_IMPORT(usrInit)
	FUNC_IMPORT(cache403ICReset)
	FUNC_IMPORT(cache403DCReset)

	.text

/*******************************************************************************
*
* sysInit - start after boot
*
* This routine is the system start-up entry point for VxWorks in RAM, the
* first code executed after booting.  It disables interrupts, sets up the
* stack, and jumps to the C routine usrInit() in usrConfig.c.
*
* The initial stack is set to grow down from the address of sysInit().  This
* stack is used only by usrInit() and is never used again.  Memory for the
* stack must be accounted for when determining the system load address.
*
* NOTE: This routine should not be called by the user.
*
* RETURNS: N/A

* sysInit (void)	/@ THIS IS NOT A CALLABLE ROUTINE @/

*/

FUNC_BEGIN(_sysInit)
	/* initialize bank registers (BR) */

	/* Bank 0 - flash */

	LOADPTR (p0, FLASH_BR_CONFIG)
	mtdcr   BR0, p0			/* BR0 : flash 29f040 */

	/* Bank 7 - DRAM */

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
#if 	((SYS_CLK_FREQ == 25000000)||(SYS_CLK_FREQ == 50000000))
        ori     p1, p1 , LO(DRAM_BR_CONFIG_25MHZ)
#else   /* 33 MHz or 66 Mhz */
        ori     p1, p1 , LO(DRAM_BR_CONFIG_33MHZ)
#endif	/* SYS_CLK_FREQ == 25000000 */
        mtdcr   BR7, p1                 /* BR7 : <LOCAL_MEM_SIZE> MB of DRAM */

	/* TODO - initialize other banks */

	/* TODO - invalidate unused banks */

	LOADPTR (p0, BR_INVAL_VAL)
	mtdcr   BR1, p0
	mtdcr   BR2, p0
	mtdcr   BR3, p0
	mtdcr   BR4, p0
	mtdcr   BR5, p0
	mtdcr   BR6, p0

	/* disable and clear interrupts/exceptions */
	
	li	p1, 0
	mtdcr   BESR, p1        	/* clear Bus Error Syndrome register */
	mtspr	ESR, p1			/* clear exception syndrome register */

	mtmsr	p1			/* disable all interrupts */

	li	p1, -1			/* p1 = 0xffffffff */
	mtspr	DBSR, p1		/* clear dbsr */
	mtdcr	EXISR, p1		/* clear all pending interrupts */

	li	p0, 0
	mtxer	p0			/* clear XER */

        li	p0, -1			/* p0 = 0xffffffff */
	mtspr	TSR, p0			/* clear timer status */
	mtdcr	DMASR, p0		/* clear dma status */

	li	p0, 0
	mtspr	TCR, p0			/* disable timers */
	mtspr	PIT, p0

	/* initialize 403GC and GCX regs, does not affect GA */

	addi	r4,r0,0x0000
	mtspr	SGR,r4
	mtspr	DCWR,r4

        /* initialize the stack pointer */

	LOADPTR (sp, _sysInit)
	addi	sp, sp, -FRAMEBASESZ

#if     FALSE                           /* SDA not supported */
        /* initialize r2 and r13 according to EABI standard */

	LOADPTR (r2, _SDA2_BASE_)
	LOADPTR (r13, _SDA_BASE_)
#endif

	li      p0, BOOT_WARM_AUTOBOOT  /* set start type arg = WARM_BOOT */
	bl	FUNC(usrInit)
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

FUNC_BEGIN(sysMemProbeSup)
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
FUNC_END(sysMemProbeSup)
