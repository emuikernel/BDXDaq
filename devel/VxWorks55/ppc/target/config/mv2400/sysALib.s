/* sysALib.s - Motorola MVME2400 system-dependent assembly routines */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-1999 Motorola, Inc. All Rights Reserved */
        .data
	.globl	copyright_wind_river
	.long	copyright_wind_river

/*
modification history
--------------------
01g,19sep01,dat  Removing sysHid1Get
01f,08may01,pch  Add assembler abstractions (FUNC_EXPORT, FUNC_BEGIN, etc.)
01e,28jan99,rhv  Cleaning up cache configuration code.
01d,20jan99,rhv  Removing legacy processor support.
01c,13jan99,rhv  Re-ordering file history.
01b,13jan99,rhv  Updated comment on Hawk GCSR setup and copyright.
01a,15dec98,mdp  written (mv2304/sysALib.s ver 01r)
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

        /* return from decrementer exceptions */

        addis   p1, r0, 0x4c00
        addi    p1, p1, 0x0064          /* load rfi (0x4c000064) to p1      */
        stw     p1, 0x900(r0)           /* store rfi at 0x00000900          */

	/* initialize the stack pointer */
	
	lis     sp, HIADJ(RAM_LOW_ADRS)
	addi    sp, sp, LO(RAM_LOW_ADRS)
	
        /* setup caches */

        /* Get cpu type */

        mfspr   r28, PVR
        rlwinm  r28, r28, 16, 16, 31

        cmpli   0, 0, r28, CPU_TYPE_750
	bne	DISABLE_MMU


	/* Enable and invalidate both caches */

	mfspr	r3,HID0
	ori	r3,r3,(_PPC_HID0_ICFI | _PPC_HID0_DCFI)
	ori	r3,r3,(_PPC_HID0_ICE | _PPC_HID0_DCE)
	sync	/* required before changing DCE */
	isync	/* required before changing ICE */
	mtspr	HID0, r3

	/*
         * enable the branch history table, unlock both caches, disable the
         * data cache and optionally disable the instruction cache.
         */

	ori	r3,r3,_PPC_HID0_BHTE

#ifdef USER_I_CACHE_ENABLE
	rlwinm	r3,r3,0,_PPC_HID0_BIT_DLOCK+1,_PPC_HID0_BIT_DCE-1
#else
	rlwinm	r3,r3,0,_PPC_HID0_BIT_DLOCK+1,_PPC_HID0_BIT_ICE-1
#endif

	sync			/* required before changing DCE */
	isync			/* required before changing ICE */
	mtspr	HID0,r3
	
DISABLE_MMU:
	/* disable instruction and data translations in the MMU */

	sync
	mfmsr	r3			/* get the value in msr *
					/* clear bits IR and DR */
	
	rlwinm	r4, r3, 0, _PPC_MSR_BIT_DR+1, _PPC_MSR_BIT_IR - 1
	
	mtmsr	r4			/* set the msr */
	isync				/* flush inst. pipe and re-fetch */

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
* From a C point of view, the routine is defined as follows:
*
*    UINT32 sysTimeBaseLGet(void)
*
* RETURNS: value of TBR 268 (in r3)
*/

sysTimeBaseLGet:
	mftb 3
	bclr 20,0
