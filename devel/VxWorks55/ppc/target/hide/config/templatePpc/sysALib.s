/* sysALib.s - generic PPC 603/604 system-dependent assembly routines */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

modification history
--------------------
01g,17jul02,dat  remove obsolete information
01f,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01e,15jun98,dat  added correct definitions for LOADVAR/LOADPTR, added
		 documentation and chg's r3 -> p0 and r4 -> p1.
01d,23dec97,dat  changed sp initial value to _sysInit.
01b,26feb98,ms   changed STACK_ADRS to _sysInit for initial stack pointer
01c,23dec97,dat  SPR 20104, use LOADPTR for 32 bit constants
01b,25aug97,dat  code review comments from thierrym
01a,08jul97,dat  written
*/

/*
TODO - Update documentation as necessary.

NOTICE
This template is generic only for the 603/604 versions of PPC.  The
820/860 and 403/401 CPUs have PPC cores with other integrated
elements.  Templates for those processors are included in this directory
as sysALib8xx.s and sysALib4xx.s.  Please substitute the appropriate
file for this file, as needed.

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

/* includes */

#define _ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "regs.h"	
#include "asm.h"

/* defines */

/*
 * Some releases of h/arch/ppc/toolPpc.h had bad definitions of
 * LOADPTR and LOADVAR. So we will define it correctly.
 * [REMOVE THESE FOR NEXT MAJOR RELEASE].
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
 * the memory location and not just the address of it.
 */

#undef LOADVAR
#define	LOADVAR(reg,addr32) \
	  addis reg,r0,HIADJ(addr32); lwz reg,LO(addr32)(reg)


/* globals */

	FUNC_EXPORT(_sysInit)		/* start of system code */
	FUNC_EXPORT(sysInByte)
	FUNC_EXPORT(sysOutByte)
        FUNC_EXPORT(sysPciRead32)
        FUNC_EXPORT(sysPciWrite32)
        FUNC_EXPORT(sysPciInByte)
        FUNC_EXPORT(sysPciOutByte)
        FUNC_EXPORT(sysPciInWord)
        FUNC_EXPORT(sysPciOutWord)
        FUNC_EXPORT(sysPciInLong)
        FUNC_EXPORT(sysPciOutLong)
        FUNC_EXPORT(sysMemProbeSup)

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

	/* disable interrupts */

	xor	p0, p0, p0
        mtmsr   p0                      /* clear the MSR register  */

	/* TODO - disable L2 Caches ?? */

        /* insert protection from decrementer exceptions */

	xor	p0, p0, p0
	LOADPTR (p1, 0x4c000064)        /* load rfi (0x4c000064) to p1      */
        stw     p1, 0x900(r0)           /* store rfi at 0x00000900          */

	/* initialize the stack pointer */
	
	LOADPTR (sp, _sysInit)
	addi	sp, sp, -FRAMEBASESZ
	
	/* disable instruction and data caches */

        mfspr   r28, PVR
        rlwinm  r28, r28, 16, 16, 31

        cmpli   0, 0, r28, CPU_TYPE_604R
        beq     cpuType604R

	LOADPTR (p0, (_PPC_HID0_ICFI | _PPC_HID0_DCFI))
	sync
	isync
	mtspr	HID0, p0		/* first invalidate I and D caches */
        b       cacheInvalidateDone

cpuType604R:
        li      p0, 0
        mtspr   HID0, p0        /* disable the caches */
        isync

        /* disable BTAC by setting bit 30 */

	LOADPTR (p0, (_PPC_HID0_ICFI | _PPC_HID0_DCFI | 0x0002))
        mtspr   HID0, p0

cacheInvalidateDone:

	/*
	 * Disable sequential instruction execution and 
	 * enable branch history table for the 604
	 */
	
	mfspr	p1,HID0
	ori	p1,p1,(_PPC_HID0_SIED | _PPC_HID0_BHTE )
	mtspr	HID0,p1
	
	/* disable instruction and data translations in the MMU */

	sync
	mfmsr	p0			/* get the value in msr *
					/* clear bits IR and DR */
	
	rlwinm	p1, p0, 0, _PPC_MSR_BIT_DR+1, _PPC_MSR_BIT_IR - 1
	
	mtmsr	p1			/* set the msr */
	sync				/* SYNC */

	/* initialize the BAT registers */

	li	p3,0	 		/* clear p3 */
	
	isync
	mtspr	IBAT0U,p3
	isync
	mtspr	IBAT0L,p3
	isync
	mtspr	IBAT1U,p3
	isync
	mtspr	IBAT1L,p3
	isync
	mtspr	IBAT2U,p3
	isync
	mtspr	IBAT2L,p3
	isync
	mtspr	IBAT3U,p3
	isync
	mtspr	IBAT3L,p3
	isync
	mtspr	DBAT0U,p3
	isync
	mtspr	DBAT0L,p3
	isync
	mtspr	DBAT1U,p3
	isync
	mtspr	DBAT1L,p3
	isync
	mtspr	DBAT2U,p3
	isync
	mtspr	DBAT2L,p3
	isync
	mtspr	DBAT3U,p3
	isync
	mtspr	DBAT3L,p3
	isync

	/* TODO - Turn off L2 cache */

	/* TODO - enable memory controllers */

	/* invalidate entries within both TLBs */

	li	p1,128
	xor	p0,p0,p0		/* p0 = 0    */
	mtctr	p1			/* CTR = 128  */

	isync				/* context sync req'd before tlbie */
sysALoop:
	tlbie	p0
	addi	p0,p0,0x1000		/* increment bits 15-19 */
	bdnz	sysALoop		/* decrement CTR, branch if CTR != 0 */
	sync				/* sync instr req'd after tlbie      */

	/* initialize Small Data Area (SDA) start address */

#if	FALSE				/* XXX TPR NO SDA for now */
	LOADPTR (r2, _SDA2_BASE_)
	LOADPTR (r13, _SDA_BASE_)
#endif

	li      p0, BOOT_WARM_AUTOBOOT
	b	usrInit			/* never returns - starts up kernel */
FUNC_END(_sysInit)


/*****************************************************************************
*
* sysInByte - reads a byte from an io address.
*
* This function reads a byte from a specified io address.
*
* RETURNS: byte from address.

* UCHAR sysInByte
*     (
*     UCHAR *  pAddr		/@ Virtual I/O addr to read from @/
*     )

*/

FUNC_BEGIN(sysInByte)
	eieio			/* Sync I/O operation */
	lbzx	p0,r0,p0	/* Read byte from I/O space */
	bclr	20,0		/* Return to caller */
FUNC_END(sysInByte)

/******************************************************************************
*
* sysOutByte - writes a byte to an io address.
*
* This function writes a byte to a specified io address.
*
* RETURNS: N/A

* VOID sysOutByte
*     (
*     UCHAR *  pAddr,		/@ Virtual I/O addr to write to @/
*     UCHAR    data		/@ data to be written @/
*     )

*/

FUNC_BEGIN(sysOutByte)
	stbx	p1,r0,p0	/* Write a byte to PCI space */
	bclr	20,0		/* Return to caller */
FUNC_END(sysOutByte)


/******************************************************************************
*
* sysPciRead32 - read 32 bit PCI data
*
* This routine will read a 32-bit data item from PCI (I/O or
* memory) space.
*
* RETURNS: N/A

* VOID sysPciRead32
*     (
*     ULONG *  pAddr,		/@ Virtual addr to read from @/
*     ULONG *  pResult		/@ location to receive data @/
*     )

*/

FUNC_BEGIN(sysPciRead32)
	eieio			/* Sync I/O operation */
        lwbrx   p0,r0,p0	/* get the data and swap the bytes */
        stw     p0,0(p1)	/* store into address ptd. to by p1 */
        bclr    20,0
FUNC_END(sysPciRead32)


/******************************************************************************
*
* sysPciWrite32 - write a 32 bit data item to PCI space
*
* This routine will store a 32-bit data item (input as big-endian)
* into PCI (I/O or memory) space in little-endian mode.
*
* RETURNS: N/A

* VOID sysPciWrite32
*     (
*     ULONG *  pAddr,		/@ Virtual addr to write to @/
*     ULONG   data		/@ Data to be written @/
*     )

*/

FUNC_BEGIN(sysPciWrite32)
        stwbrx  p1,r0,p0	/* store data as little-endian */
        bclr    20,0
FUNC_END(sysPciWrite32)


/*****************************************************************************
*
* sysPciInByte - reads a byte from PCI Config Space.
*
* This function reads a byte from a specified PCI Config Space address.
*
* RETURNS:
* Returns 8 bit data from the specified register.  Note that for PCI systems
* if no target responds, the data returned to the CPU will be 0xff.

* UINT8 sysPciInByte
*     (
*     UINT8 *  pAddr,		/@ Virtual addr to read from @/
*     )

*/

FUNC_BEGIN(sysPciInByte)
	eieio			/* Sync I/O operation */
        lbzx    p0,r0,p0	/* Read byte from PCI space */
        bclr    20,0		/* Return to caller */
FUNC_END(sysPciInByte)

/*****************************************************************************
*
* sysPciInWord - reads a word (16-bit big-endian) from PCI Config Space.
*
* This function reads a word from a specified PCI Config Space (little-endian)
* address.
*
* RETURNS:
* Returns 16 bit data from the specified register.  Note that for PCI systems
* if no target responds, the data returned to the CPU will be 0xffff.

* USHORT sysPciInWord
*     (
*     USHORT *  pAddr,		/@ Virtual addr to read from @/
*     )

*/

FUNC_BEGIN(sysPciInWord)
	eieio			/* Sync I/O operation */
        lhbrx   p0,r0,p0	/* Read and swap from PCI space */
        bclr    20,0		/* Return to caller */
FUNC_END(sysPciInWord)

/*****************************************************************************
*
* sysPciInLong - reads a long (32-bit big-endian) from PCI Config Space.
*
* This function reads a long from a specified PCI Config Space (little-endian)
* address.
*
* RETURNS:
* Returns 32 bit data from the specified register.  Note that for PCI systems
* if no target responds, the data returned to the CPU will be 0xffffffff.

* ULONG sysPciInLong
*     (
*     ULONG *  pAddr,		/@ Virtual addr to read from @/
*     )

*/

FUNC_BEGIN(sysPciInLong)
	eieio			/* Sync I/O operation */
        lwbrx   p0,r0,p0	/* Read and swap from PCI space */
        bclr    20,0		/* Return to caller */
FUNC_END(sysPciInLong)

/******************************************************************************
*
* sysPciOutByte - writes a byte to PCI Config Space.
*
* This function writes a byte to a specified PCI Config Space address.
*
* RETURNS: N/A

* VOID sysPciOutByte
*     (
*     UINT8 *  pAddr,		/@ Virtual addr to write to @/
*     UINT8  data		/@ Data to be written       @/
*     )

*/

FUNC_BEGIN(sysPciOutByte)
        stbx    p1,r0,p0	/* Write a byte to PCI space */
        bclr    20,0		/* Return to caller */
FUNC_END(sysPciOutByte)

/******************************************************************************
*
* sysPciOutWord - writes a word (16-bit big-endian) to PCI Config Space.
*
* This function writes a word to a specified PCI Config Space (little-endian)
* address.
*
* RETURNS: N/A

* VOID sysPciOutWord
*     (
*     USHORT *  pAddr,		/@ Virtual addr to write to @/
*     USHORT  data		/@ Data to be written       @/
*     )

*/

FUNC_BEGIN(sysPciOutWord)
        sthbrx  p1,r0,p0	/* Write with swap to PCI space */
        bclr    20,0		/* Return to caller */
FUNC_END(sysPciOutWord)

/******************************************************************************
*
* sysPciOutLong - writes a long (32-bit big-endian) to PCI Config Space.
*
* This function writes a long to a specified PCI Config Space (little-endian)
* address.
*
* RETURNS: N/A

* VOID sysPciOutLong
*     (
*     ULONG *  pAddr,		/@ Virtual addr to write to @/
*     ULONG  data		/@ Data to be written       @/
*     )

*/

FUNC_BEGIN(sysPciOutLong)
        stwbrx  p1,r0,p0	/* Write big-endian long to little-endian */
        mr      p0,p1		/* PCI space */
        bclr    20,0		/* Return to caller */
FUNC_END(sysPciOutLong)

/*******************************************************************************
*
* sysMemProbeSup - sysBusProbe support routine
*
* This routine is called to try to read byte, word, or long, as specified
* by length, from the specified source to the specified destination.
*
* RETURNS: OK if successful probe, else ERROR

* STATUS sysMemProbeSup
*     (
*     int         length, /@ length of cell to test (1, 2, 4) @/
*     char *      src,    /@ address to read @/
*     char *      dest    /@ address to write @/
*     )

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
