/* sysAUtil.s - system-dependent assembly routines */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1996-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,02Jun03,simon  updated based on peer review results
01a,31mar03,simon  Ported. from ver 01a, mcp905/sysAUtil.s.
*/

/*
DESCRIPTION
This module contains system-dependent routines written in assembly
language.
*/

#define _ASMLANGUAGE

/* includes */

#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "regs.h"
#include "asm.h"

/* defines */

/* Special Purpose Registers */

#define MSSCR0              1014         /* Memory Subsystem Control Register */
#define L2CR                1017         /* L2 Cache Control Register */
#define DEC                 22           /* Decrementer Register */
#define TBLR		    284		 /* Time Base Lower Register */

/* globals */

    FUNC_EXPORT(sysPciInByte)
    FUNC_EXPORT(sysPciOutByte)
    FUNC_EXPORT(sysPciInWord)
    FUNC_EXPORT(sysPciOutWord)
    FUNC_EXPORT(sysPciInLong)
    FUNC_EXPORT(sysPciOutLong)
    FUNC_EXPORT(sysInWord)
    FUNC_EXPORT(sysOutWord)
    FUNC_EXPORT(sysInLong)
    FUNC_EXPORT(sysOutLong)
    FUNC_EXPORT(sysMemProbeSup)
    FUNC_EXPORT(sysProbeExc)
    FUNC_EXPORT(sysL2crPut)
    FUNC_EXPORT(sysL2crGet)
    FUNC_EXPORT(sysTimeBaseLPut)
    FUNC_EXPORT(sysTimeBaseLGet)
    FUNC_EXPORT(sysHid1Get)
    FUNC_EXPORT(sysGetDec)
    FUNC_EXPORT(sysMsscr0Put)
    FUNC_EXPORT(sysMsscr0Get)

    /* externals */
     
    _WRS_TEXT_SEG_START

/*****************************************************************************
*
* sysPciInByte - reads a byte from PCI Config Space.
*
* This function reads a byte from a specified PCI Config Space address.
*
* ARGUMENTS:
* r3 = Config Space address
*
* RETURNS:
* r3 = byte from address.
*/

FUNC_BEGIN(sysPciInByte)
    lbzx    r3,r0,r3                /* Read byte from PCI space */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysPciInByte)

/*****************************************************************************
*
* sysPciInWord - reads a word (16-bit big-endian) from PCI Config Space.
*
* This function reads a word from a specified PCI Config Space (little-endian)
* address.
*
* ARGUMENTS:
* r3 = Config Space address
*
* RETURNS:
* r3 = word (16-bit big-endian) from address.
*/

FUNC_BEGIN(sysInWord)
FUNC_BEGIN(sysPciInWord)
    lhbrx   r3,r0,r3                /* Read 16 bit byte reversed */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysInWord)
FUNC_END(sysPciInWord)

/*****************************************************************************
*
* sysPciInLong - reads a long (32-bit big-endian) from PCI Config Space.
*
* This function reads a long from a specified PCI Config Space (little-endian)
* address.
*
* ARGUMENTS:
* r3 = Config Space address
*
* RETURNS:
* r3 = long (32-bit big-endian) from address.
*/

FUNC_BEGIN(sysInLong)
FUNC_BEGIN(sysPciInLong)
    lwbrx   r3,r0,r3                /* Read 32 bit byte reversed */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysInLong)
FUNC_END(sysPciInLong)

/******************************************************************************
*
* sysPciOutByte - writes a byte to PCI Config Space.
*
* This function writes a byte to a specified PCI Config Space address.
*
* ARGUMENTS:
* r3 = Config Space address
* r4 = byte to write
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysPciOutByte)
    stbx    r4,r0,r3                /* Write a byte to PCI space */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysPciOutByte)

/******************************************************************************
*
* sysPciOutWord - writes a word (16-bit big-endian) to PCI Config Space.
*
* This function writes a word to a specified PCI Config Space (little-endian)
* address.
*
* ARGUMENTS:
* r3 = Config Space address
* r4 = word (16-bit big-endian) to write
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysOutWord)
FUNC_BEGIN(sysPciOutWord)
    sthbrx  r4,r0,r3                /* Write byte-reversed 16 bit value */
    eieio
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysOutWord)
FUNC_END(sysPciOutWord)

/******************************************************************************
*
* sysPciOutLong - writes a long (32-bit big-endian) to PCI Config Space.
*
* This function writes a long to a specified PCI Config Space (little-endian)
* address.
*
* ARGUMENTS:
* r3 = Config Space address
* r4 = long (32-bit big-endian) to write
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysOutLong)
FUNC_BEGIN(sysPciOutLong)
    stwbrx  r4,r0,r3                /* Write byte-reversed long */
    eieio
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysOutLong)
FUNC_END(sysPciOutLong)

/*******************************************************************************
*
* sysMemProbeSup - sysBusProbe support routine
*
* This routine is called to try to read byte, word, or long, as specified
* by length, from the specified source to the specified destination.
*
* STATUS sysMemProbeSup 
* (
* int         length of cell to test (1, 2, 4, 8, 16) 
* char *      address to read 
* char *      address to write 
* )
*
* RETURNS: OK if successful probe, else ERROR
*/

FUNC_BEGIN(sysMemProbeSup)
    addi    r10, r3, 0              /* save length to p7 */
    xor     r3, r3, r3              /* set return status */
    cmpwi   r10, 1                  /* check for byte access */
    bne     sbpShort                /* no, go check for short word access */
    lbz     r9, 0(r4)               /* load byte from source */
    eieio
    sync
    stb     r9, 0(r5)               /* store byte to destination */
    eieio
    sync
    isync                           /* flush instruction pipe */
    blr
sbpShort:
    cmpwi   r10, 2                  /* check for short word access */
    bne     sbpWord                 /* no, check for word access */
    lhz     r9, 0(r4)               /* load half word from source */
    eieio
    sync
    sth     r9, 0(r5)               /* store half word to destination */
    eieio
    sync
    isync                           /* flush instruction pipe */
    blr
sbpWord:
    cmpwi   r10, 4                  /* check for short word access */
    bne     sysProbeExc             /* no, check for double word access */
    lwz     r9, 0(r4)               /* load half word from source */
    eieio
    sync
    stw     r9, 0(r5)               /* store half word to destination */
    eieio
    sync
    isync                           /* flush instruction pipe */
    blr
sysProbeExc:
    li      r3, -1                  /* shouldn't ever get here, but... */
    blr                             /* Return to caller */
FUNC_END(sysMemProbeSup)

/*******************************************************************************
*
* sysL2crPut - write to L2CR register
*
* This routine will write the contents of r3 to the L2CR
* register.
*
* From a C point of view, the routine is defined as follows:
*
* void sysL2crPut
* (
* ULONG       value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysL2crPut)
    sync
    mtspr   L2CR,r3
    isync
    bclr    20,0
FUNC_END(sysL2crPut)

/*******************************************************************************
*
* sysL2crGet - read from L2CR register
*
* This routine will read the contents the L2CR register.
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysL2crGet()
*
* RETURNS: value of SPR1017 (in r3)
*/

FUNC_BEGIN(sysL2crGet)
    sync
    mfspr   r3,L2CR
    sync
    bclr    20,0
FUNC_END(sysL2crGet)

/*******************************************************************************
*
* sysMsscr0Put - write to MSSCR0 register
*
* This routine will write the contents of r3 to the MSSCR0
* register.
*
* From a C point of view, the routine is defined as follows:
*
* void sysMsscr0Put
* (
* ULONG       value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysMsscr0Put)
    sync
    mtspr   MSSCR0,r3
    isync
    bclr    20,0
FUNC_END(sysMsscr0Put)

/*******************************************************************************
*
* sysMsscr0Get - read from MSSCR0 register
*
* This routine will read the contents of the MSSCR0 register.
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysMsscr0Get()
*
* RETURNS: value of SPR1014 (in r3)
*/

FUNC_BEGIN(sysMsscr0Get)
    sync
    mfspr   r3,MSSCR0
    sync
    bclr    20,0
FUNC_END(sysMsscr0Get)

/*******************************************************************************
*
* sysTimeBaseLPut - write to lower half of Time Base register
*
* register.
*
* From a C point of view, the routine is defined as follows:
*
* void sysTimeBaseLPut
* (
* ULONG       value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysTimeBaseLPut)
    sync
    mtspr   TBLR,r3
    isync
    bclr    20,0
FUNC_END(sysTimeBaseLPut)

/******************************************************************************
*
* sysTimeBaseLGet - Get lower half of Time Base Register
*
* This routine will read the contents the lower half of the Time
* Base Register (TBL - TBR 268).
*
* From a C point of view, the routine is defined as follows:
*
* UINT32 sysTimeBaseLGet(void)
*
* RETURNS: value of TBR 268 (in r3)
*/

FUNC_BEGIN(sysTimeBaseLGet)
    mftb 3
    bclr 20,0
FUNC_END(sysTimeBaseLGet)

/******************************************************************************
*
* sysHid1Get - read from HID1 register SPR1009.
*
* This routine will read the contents the HID1 (SPR1009)
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysHid1Get()
*
* RETURNS: value of SPR1009 (in r3)
*/

FUNC_BEGIN(sysHid1Get)
    sync
    mfspr r3,HID1
    sync
    bclr 20,0
FUNC_END(sysHid1Get)

/******************************************************************************
*
* sysGetDec - read from the Decrementer register SPR22.
*
* This routine will read the contents the decrementer (SPR22)
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysGetDec()
*
* RETURNS: value of SPR22 (in r3)
*/

FUNC_BEGIN(sysGetDec)
    sync
    mfspr r3,DEC
    sync
    bclr 20,0
FUNC_END(sysGetDec)
