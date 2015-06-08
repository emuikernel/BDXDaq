/* sysAUtil.s - system-dependent assembly routines */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1996-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01g,15sep05,cak  Added utility to read the PVR (Processor Version Register).
01f,29mar04,cak  Added routines for reading from and writing to the 
		 L3 cache output hold control register, and the L3 cache
		 input timing control registers.
01e,05nov03,cak  Added routines for reading from and writing to processor
		 registers. 
01d,28jan03,cak  Added sysTimeBaseLPut() routine.
01c,08jan03,cak  Added sysSioRead and sysSioWrite to read from and write to the
		 PCx7307 Super IO device.
01b,26aug02,cak  Modifications for Tornado 2.1 to 2.2 conversion (added
		 FUNC_... macros).
01a,08may02,cak  Created from ver 01d of sysALib.s.
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
#define L3CR                1018         /* L3 Cache Control Register */
#define L3OHCR		    1000	 /* L3 Cache Output Hold Ctrl Reg */
#define L3ITCR0		    984		 /* L3 Cache Input Timing Ctrl 0 Reg */
#define L3ITCR1		    1001	 /* L3 Cache Input Timing Ctrl 1 Reg */
#define L3ITCR2		    1002	 /* L3 Cache Input Timing Ctrl 2 Reg */
#define L3ITCR3		    1003	 /* L3 Cache Input Timing Ctrl 3 Reg */
#define DEC                 22           /* Decrementer Register */
#define L3PM                983          /* L3 Private Memory Ctrl Register */
#define TBLR_W              284          /* Time Base Lower Register - write */
#define TBLR_R		    268		 /* Time Base Lower Register - read */
#define TBUR_W		    285		 /* Time Base Upper Register - write */
#define TBUR_R		    269		 /* Time Base Upper Register - read */
#define PVR		    287		 /* Processor Version Register */

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
    FUNC_EXPORT(sysTimeBaseUPut)
    FUNC_EXPORT(sysTimeBaseUGet)
    FUNC_EXPORT(sysHid1Get)
    FUNC_EXPORT(sysHid1Put)
    FUNC_EXPORT(sysHid0Get)
    FUNC_EXPORT(sysHid0Put)
    FUNC_EXPORT(sysGetDec)
    FUNC_EXPORT(sysL3pmPut)
    FUNC_EXPORT(sysL3pmGet)
    FUNC_EXPORT(sysMsscr0Put)
    FUNC_EXPORT(sysMsscr0Get)
    FUNC_EXPORT(sysL3crPut)
    FUNC_EXPORT(sysL3crGet)
    FUNC_EXPORT(sysL3ohcrPut)
    FUNC_EXPORT(sysL3ohcrGet)
    FUNC_EXPORT(sysL3itcr0Put)
    FUNC_EXPORT(sysL3itcr0Get)
    FUNC_EXPORT(sysL3itcr1Put)
    FUNC_EXPORT(sysL3itcr1Get)
    FUNC_EXPORT(sysL3itcr2Put)
    FUNC_EXPORT(sysL3itcr2Get)
    FUNC_EXPORT(sysL3itcr3Put)
    FUNC_EXPORT(sysL3itcr3Get)
    FUNC_EXPORT(sysPvrGet)
#ifdef INCLUDE_PCX7307_SIO
    FUNC_EXPORT(sysSioRead)
    FUNC_EXPORT(sysSioWrite)
#endif /* INCLUDE_PCX7307_SIO */

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
* sysL3crPut - write to L3CR register
*
* This routine will write the contents of r3 to the L3CR
* register.
*
* From a C point of view, the routine is defined as follows:
*
* void sysL3crPut
* (
* ULONG       value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysL3crPut)
    sync
    mtspr   L3CR,r3
    isync
    bclr    20,0
FUNC_END(sysL3crPut)

/*******************************************************************************
*
* sysL3crGet - read from L3CR register
*
* This routine will read the contents the L3CR register.
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysL3crGet()
*
* RETURNS: value of SPR1018 (in r3)
*/

FUNC_BEGIN(sysL3crGet)
    sync
    mfspr   r3,L3CR
    sync
    bclr    20,0
FUNC_END(sysL3crGet)

/*******************************************************************************
*
* sysL3ohcrPut - write to L3OHCR register
*
* This routine will write the contents of r3 to the L3OHCR
* register.
*
* From a C point of view, the routine is defined as follows:
*
* void sysL3ohcrPut
* (
* ULONG       value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysL3ohcrPut)
    sync
    mtspr   L3OHCR,r3
    isync
    bclr    20,0
FUNC_END(sysL3ohcrPut)

/*******************************************************************************
*
* sysL3ohcrGet - read from L3OHCR register
*
* This routine will read the contents the L3OHCR register.
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysL3ohcrGet()
*
* RETURNS: value of SPR1000 (in r3)
*/

FUNC_BEGIN(sysL3ohcrGet)
    sync
    mfspr   r3,L3OHCR
    sync
    bclr    20,0
FUNC_END(sysL3ohcrGet)

/*******************************************************************************
*
* sysL3itcr0Put - write to L3ITCR0 register
*
* This routine will write the contents of r3 to the L3ITCR0
* register.
*
* From a C point of view, the routine is defined as follows:
*
* void sysL3itcr0Put
* (
* ULONG       value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysL3itcr0Put)
    sync
    mtspr   L3ITCR0,r3
    isync
    bclr    20,0
FUNC_END(sysL3itcr0Put)

/*******************************************************************************
*
* sysL3itcr0Get - read from L3ITCR0 register
*
* This routine will read the contents the L3ITCR0 register.
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysL3itcr0Get()
*
* RETURNS: value of SPR984 (in r3)
*/

FUNC_BEGIN(sysL3itcr0Get)
    sync
    mfspr   r3,L3ITCR0
    sync
    bclr    20,0
FUNC_END(sysL3itcr0Get)

/*******************************************************************************
*
* sysL3itcr1Put - write to L3ITCR1 register
*
* This routine will write the contents of r3 to the L3ITCR1
* register.
*
* From a C point of view, the routine is defined as follows:
*
* void sysL3itcr1Put
* (
* ULONG       value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysL3itcr1Put)
    sync
    mtspr   L3ITCR1,r3
    isync
    bclr    20,0
FUNC_END(sysL3itcr1Put)

/*******************************************************************************
*
* sysL3itcr1Get - read from L3ITCR1 register
*
* This routine will read the contents the L3ITCR1 register.
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysL3itcr1Get()
*
* RETURNS: value of SPR1001 (in r3)
*/

FUNC_BEGIN(sysL3itcr1Get)
    sync
    mfspr   r3,L3ITCR1
    sync
    bclr    20,0
FUNC_END(sysL3itcr1Get)

/*******************************************************************************
*
* sysL3itcr2Put - write to L3ITCR2 register
*
* This routine will write the contents of r3 to the L3ITCR2
* register.
*
* From a C point of view, the routine is defined as follows:
*
* void sysL3itcr2Put
* (
* ULONG       value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysL3itcr2Put)
    sync
    mtspr   L3ITCR2,r3
    isync
    bclr    20,0
FUNC_END(sysL3itcr2Put)

/*******************************************************************************
*
* sysL3itcr2Get - read from L3ITCR2 register
*
* This routine will read the contents the L3ITCR2 register.
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysL3itcr2Get()
*
* RETURNS: value of SPR1002 (in r3)
*/

FUNC_BEGIN(sysL3itcr2Get)
    sync
    mfspr   r3,L3ITCR2
    sync
    bclr    20,0
FUNC_END(sysL3itcr2Get)

/*******************************************************************************
*
* sysL3itcr3Put - write to L3ITCR3 register
*
* This routine will write the contents of r3 to the L3ITCR3
* register.
*
* From a C point of view, the routine is defined as follows:
*
* void sysL3itcr3Put
* (
* ULONG       value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysL3itcr3Put)
    sync
    mtspr   L3ITCR3,r3
    isync
    bclr    20,0
FUNC_END(sysL3itcr3Put)

/*******************************************************************************
*
* sysL3itcr3Get - read from L3ITCR3 register
*
* This routine will read the contents the L3ITCR3 register.
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysL3itcr3Get()
*
* RETURNS: value of SPR1003 (in r3)
*/

FUNC_BEGIN(sysL3itcr3Get)
    sync
    mfspr   r3,L3ITCR3
    sync
    bclr    20,0
FUNC_END(sysL3itcr3Get)

/*******************************************************************************
*
* sysTimeBaseLPut - write to lower half of Time Base register
*
* This routine will write the contents of r3 to the lower half of the time 
* base register.
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
    mtspr   TBLR_W,r3
    isync
    bclr    20,0
FUNC_END(sysTimeBaseLPut)

/******************************************************************************
*
* sysTimeBaseLGet - Get lower half of Time Base Register
*
* This routine will read the contents the lower half of the Time
* Base Register.
*
* From a C point of view, the routine is defined as follows:
*
* UINT32 sysTimeBaseLGet(void)
*
* RETURNS: value of lower half of time base register (in r3)
*/

FUNC_BEGIN(sysTimeBaseLGet)
    mftb r3,TBLR_R
    bclr 20,0
FUNC_END(sysTimeBaseLGet)

/*******************************************************************************
*
* sysTimeBaseUPut - write to upper half of Time Base register
*
* This routine will write the contents of r3 to the upper half of the Time
* Base register.
*
* From a C point of view, the routine is defined as follows:
*
* void sysTimeBaseUPut
* (
* ULONG       value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysTimeBaseUPut)
    sync
    mtspr   TBUR_W,r3
    isync
    bclr    20,0
FUNC_END(sysTimeBaseUPut)

/******************************************************************************
*
* sysTimeBaseUGet - Get upper half of Time Base Register
*
* This routine will read the contents of the upper half of the Time
* Base Register.
*
* From a C point of view, the routine is defined as follows:
*
* UINT32 sysTimeBaseUGet(void)
*
* RETURNS: value of upper half of time base register (in r3)
*/

FUNC_BEGIN(sysTimeBaseUGet)
    mftb r3,TBUR_R
    bclr 20,0
FUNC_END(sysTimeBaseUGet)

/******************************************************************************
*
* sysHid1Get - read from HID1 register.
*
* This routine will read the contents of the HID1 register. 
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysHid1Get()
*
* RETURNS: value of HID1 register (in r3)
*/

FUNC_BEGIN(sysHid1Get)
    sync
    mfspr r3,HID1
    sync
    bclr 20,0
FUNC_END(sysHid1Get)

/*******************************************************************************
*
* sysHid1Put - write to the HID1 (Hardware Implementation-Dependent
* Register 1) 
*
* This routine will write the contents of r3 to the HID1 register.
*
* From a C point of view,the routine is defined as follows:
*
* void sysHid1Put
* (
* ULONG value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysHid1Put)
    sync
    mtspr   HID1,r3
    isync
    bclr    20,0            /* Return to caller */
FUNC_END(sysHid1Put)

/******************************************************************************
*
* sysHid0Get - read from HID0 register.
*
* This routine will read the contents the HID0
*
* From a C point of view, the routine is defined as follows:
*
* UINT sysHid0Get()
*
* RETURNS: value of HID0 register (in r3)
*/

FUNC_BEGIN(sysHid0Get)
    sync
    mfspr r3,HID0
    sync
    bclr 20,0
FUNC_END(sysHid0Get)

/*******************************************************************************
*
* sysHid0Put - write to the HID0 (Hardware Implementation-Dependent
* Register 0) 
*
* This routine will write the contents of r3 to the HID0 register.
*
* From a C point of view,the routine is defined as follows:
*
* void sysHid0Put
* (
* ULONG value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysHid0Put)
    sync
    mtspr   HID0,r3
    isync
    bclr    20,0            /* Return to caller */
FUNC_END(sysHid0Put)

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

/*******************************************************************************
*
* sysL3pmPut - write to the L3PM (Private Memory Control Register) of
* Apollo CPU
*
* This routine will write the contents of r3 to the L3PM
* register.
*
* From a C point of view,the routine is defined as follows:
*
* void sysL3pmPut
* (
* ULONG value to write
* )
*
* RETURNS: NA
*/

FUNC_BEGIN(sysL3pmPut)
    sync
    mtspr   L3PM,r3
    isync
    bclr    20,0            /* Return to caller */
FUNC_END(sysL3pmPut)

/*******************************************************************************
*
* sysL3pmGet - read from L3PM (Private Memory Control Register)
* of the Apollo CPU
*
* This routine will read the contents the L3PM register.
*
* From a C point of view,the routine is defined as follows:
*
* UINT sysL3pmGet()
*
* RETURNS: value of SPR983 (in r3)
*/

FUNC_BEGIN(sysL3pmGet)
    sync
    mfspr   r3,L3PM
    sync
    bclr    20,0            /* Return to caller */
FUNC_END(sysL3pmGet)

/*******************************************************************************
*
* sysPvrGet - read from PVR (Processor Version Register) 
*
* This routine will read the contents of the PVR register.
*
* From a C point of view,the routine is defined as follows:
*
* UINT sysPvrGet()
*
* RETURNS: value of SPR287 (in r3)
*/

FUNC_BEGIN(sysPvrGet)
    sync
    mfspr   r3,PVR
    sync
    bclr    20,0            /* Return to caller */
FUNC_END(sysPvrGet)

#ifdef INCLUDE_PCX7307_SIO
/*******************************************************************************
*
* sysSioRead - this function reads a register from the UIO chip
*
* SYNOPSIS
* \ss
* UINT8 sysSioRead
*    (
*    UINT8 * pSioIndexReg,      /@ pointer to SIO index register base addr @/
*    UINT8 sioRegOffset         /@ offset of register to read from @/
*    )
* \se
*
* In order to read data from the desired Super IO register, the index register
* must be written to with the offset of the of the register to be read.  The
* desired byte of data is then read from the data register.
*
* RETURNS: byte read from data register
*/

FUNC_BEGIN(sysSioRead)
    stb     r4,0(r3)        /* write index register with register offset */
    eieio
    sync
    lbz     r3,1(r3)        /* retrieve specified reg offset contents */
    eieio
    sync
    bclr    20,0            /* return to caller */
FUNC_END(sysSioRead)

/*******************************************************************************
*
* sysSioWrite - this function writes a register to the UIO chip
*
* In order to write data to the desired Super IO register, the index
* register must be written to with the offset of the register to be
* modified.  The desired byte of data can then be written via the data
* register.
*
* SYNOPSIS
* \ss
* void sysSioWrite
*    (
*    UINT8 * pSioIndexReg,      /@ pointer to SIO index register base addr @/
*    UINT8 sioRegOffset,        /@ offset of register to write to @/
*    UINT8 data                 /@ 8-bit data to be written @/
*    )
* \se
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysSioWrite)
    stb     r4,0(r3)        /* write index register with register offset */
    eieio
    sync
    stb     r5,1(r3)        /* 1st write */
    eieio
    sync
    stb     r5,1(r3)        /* 2nd write */
    eieio
    sync
    bclr    20,0            /* return to caller */
FUNC_END(sysSioWrite)
#endif /* INCLUDE_PCX7307_SIO */
