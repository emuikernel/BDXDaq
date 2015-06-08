/* sysALib.s - Motorola system-dependent assembly routines */

/* Copyright 1984-2002 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc. All Rights Reserved */
    .data
    .globl   copyright_wind_river
    .long    copyright_wind_river

/*
modification history
--------------------
01i,30jan02,kab  Added MPC7410 (Nitro) support
01h,25oct01,kab  Update use of FUNC_* macros; doc format
		 Replace CPU_TYPE_MAX (confusing codename) w/ CPU_TYPE_7400
01g,09may01,pch  Add .type directives
01f,09mar01,rhk  Additional  coding standards cleanup.
01e,16aug00,dmw  Wind River coding standards review.
01d,12jun00,dmw  Added HW L1 cache flush if MAX.
01c,09may00,dmw  Applied MAX errata changes to 2.7 revision or less.
01b,13apr00,dmw  Added sysSioRead/sysSioWrite
01a,21mar00,dmw  Written (from verison 01c of mcpn765/sysALib.s).
*/

/*
DESCRIPTION
This module contains system-dependent routines written in assembly
language.

This module must be the first specified in the \f3ld\f1 command used to
build the system.  The sysInit() routine is the system start-up code.
*/

#define _ASMLANGUAGE

/* includes */

#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "regs.h"    
#include "asm.h"

/* defines */

#define SGE_CLEAR	0x00000080  /* SGE: clear */
#define MEMSSCR1	0x01000000  /* Memory SubSystem Control Reg 1 */
#define DL1HWFLSH	0x00800000  /* L1 data cache HW flush bit 8 */

/* globals */

    FUNC_EXPORT(_sysInit)        /* start of system code */
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
    FUNC_EXPORT(sysHid1Get)
    FUNC_EXPORT(sysSioRead)
    FUNC_EXPORT(sysSioWrite)

    FUNC_EXPORT(sysInWord)
    FUNC_EXPORT(sysOutWord)
    FUNC_EXPORT(sysInLong)
    FUNC_EXPORT(sysOutLong)

    /* externals */

    FUNC_IMPORT(usrInit)
    
    _WRS_TEXT_SEG_START

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
*
* NOMANUAL
*/

FUNC_BEGIN(_sysInit)

    /* disable external interrupts */

    xor     p0, p0, p0
    mtmsr   p0                      /* clear the MSR register  */
                                     
    /* return from decrementer exceptions */

    addis   p1, r0, 0x4c00
    addi    p1, p1, 0x0064          /* load rfi (0x4c000064) to p1      */
    stw     p1, 0x900(r0)           /* store rfi at 0x00000900          */

    /* initialize the stack pointer */
    
    lis     sp, HIADJ(RAM_LOW_ADRS)
    addi    sp, sp, LO(RAM_LOW_ADRS)
    
    /* Get cpu type */

    mfspr   r28, PVR
    rlwinm  r28, r28, 16, 16, 31
    cmpli   0, 0, r28, CPU_TYPE_7410
    bne     cpuNotNitro

    /* Get the NITRO revision type. */
    mfspr   r29, PVR
    rlwinm  r29, r29, 0, 24, 31

    cmpwi   r29,(CPU_REV_7410+2)
    bc      12,5,cpuNotNitro      /* branch if greater than 0x1102 */

    /* Fix for early Nitro revs */
    lis     r2,HI(MSSCR_NITRO1)
    ori     r2,r2,LO(MSSCR_NITRO1)      /* Nitro errata 1 */
    mtspr   1015,r2

cpuNotNitro:

    cmpwi   r28,CPU_TYPE_7400
    bne     cpuNotMax27

    /* MAX L1 Data Cache HW Flush per Max Book 4, Rev. 2.0 */

    lis     r2,HI(DL1HWFLSH) 
    ori     r2,r2,LO(DL1HWFLSH)
    mtspr   1014,r2                 /* MSSCR0 */

    /* 
     * required sync to guarantee that all data from the dL1 has
     * been written to the system address interface. 
     */

    sync                            

    /* Get the MAX revision type. If 2.7 or less apply errata work-around. */

    mfspr   r29, PVR
    rlwinm  r29, r29, 0, 24, 31

    cmpwi   r29,7
    bc      12,5,cpuNotMax27       /* branch if greater than 7 */ 

    /* MAX errata counter measures.  */

    mfspr   r2,HID0
    lis     r3,HI(SGE_CLEAR)       /* SGE: clear */
    ori     r3,r3,LO(SGE_CLEAR)
    andc    r2,r2,r3
    mtspr   HID0,r2

    li      r2,0x0
    mtspr   1014,r2                 /* MSSCR0 */

    lis     r2,HI(MEMSSCR1)
    ori     r2,r2,LO(MEMSSCR1)      /* MEMSSCR1: Max errata 13 */
    mtspr   1015,r2

cpuNotMax27:

    cmpli   0, 0, r28, CPU_TYPE_7400
    beq     cpu750Max
    cmpli   0, 0, r28, CPU_TYPE_7410
    beq     cpu750Max
    cmpli   0, 0, r28, CPU_TYPE_750
    bne     disableMmu

cpu750Max:

    /* Enable and invalidate both caches */

    mfspr   r3,HID0
    ori     r3,r3,(_PPC_HID0_ICFI | _PPC_HID0_DCFI)
    ori     r3,r3,(_PPC_HID0_ICE | _PPC_HID0_DCE)
    sync                            /* required before changing DCE */
    isync                           /* required before changing ICE */
    mtspr   HID0, r3

    /*
     * enable the branch history table, unlock both caches, disable the
     * data cache and optionally disable the instruction cache.
     */

    ori     r3,r3,_PPC_HID0_BHTE

#ifdef USER_I_CACHE_ENABLE
    rlwinm  r3,r3,0,_PPC_HID0_BIT_DLOCK+1,_PPC_HID0_BIT_DCE-1
#else
    rlwinm  r3,r3,0,_PPC_HID0_BIT_DLOCK+1,_PPC_HID0_BIT_ICE-1
#endif /* USER_I_CACHE_ENABLE */

    sync                            /* required before changing DCE */
    isync                           /* required before changing ICE */
    mtspr    HID0,r3
    
disableMmu:

    /* disable instruction and data translations in the MMU */

    sync
    mfmsr    r3                     /* get the value in msr *
                   
    /* clear bits IR and DR */
    
    rlwinm   r4, r3, 0, _PPC_MSR_BIT_DR+1, _PPC_MSR_BIT_IR - 1
    
    mtmsr    r4                    /* set the msr */
    isync                          /* flush inst. pipe and re-fetch */

    /* initialize the BAT register */

    li       p3,0                  /* clear p0 */
    
    isync
    mtspr    IBAT0U,p3             /* SPR 528 (IBAT0U) */
    isync

    mtspr    IBAT0L,p3             /* SPR 529 (IBAT0L) */
    isync

    mtspr    IBAT1U,p3             /* SPR 530 (IBAT1U) */
    isync

    mtspr    IBAT1L,p3             /* SPR 531 (IBAT1L) */
    isync

    mtspr    IBAT2U,p3             /* SPR 532 (IBAT2U) */
    isync

    mtspr    IBAT2L,p3             /* SPR 533 (IBAT2L) */
    isync

    mtspr    IBAT3U,p3             /* SPR 534 (IBAT3U) */
    isync

    mtspr    IBAT3L,p3             /* SPR 535 (IBAT3L) */
    isync

    mtspr    DBAT0U,p3             /* SPR 536 (DBAT0U) */
    isync

    mtspr    DBAT0L,p3             /* SPR 537 (DBAT0L) */
    isync

    mtspr    DBAT1U,p3             /* SPR 538 (DBAT1U) */
    isync

    mtspr    DBAT1L,p3             /* SPR 539 (DBAT1L) */
    isync

    mtspr    DBAT2U,p3             /* SPR 540 (DBAT2U) */
    isync

    mtspr    DBAT2L,p3             /* SPR 541 (DBAT2L) */
    isync

    mtspr    DBAT3U,p3             /* SPR 542 (DBAT3U) */
    isync

    mtspr    DBAT3L,p3             /* SPR 543 (DBAT3L) */
    isync

    /* invalidate entries within both TLBs */

    li       p1,128
    xor      p0,p0,p0              /* p0 = 0    */
    mtctr    p1                    /* CTR = 32  */

    isync                          /* context sync req'd before tlbie */
sysALoop:
    tlbie    p0
    addi     p0,p0,0x1000          /* increment bits 15-19 */
    bdnz     sysALoop              /* decrement CTR, branch if CTR != 0 */
    sync                           /* sync instr req'd after tlbie      */

    /* initialize Small Data Area (SDA) start address */

#if    FALSE                       /* XXX TPR NO SDA for now */
    lis     r2, HIADJ(_SDA2_BASE_)
    addi    r2, r2, LO(_SDA2_BASE_)

    lis     r13, HIADJ(_SDA_BASE_)
    addi    r13, r13, LO(_SDA_BASE_)
#endif /* if FALSE */

    addi    sp, sp, -FRAMEBASESZ   /* get frame stack */
    li      r3, BOOT_WARM_AUTOBOOT
    b       usrInit                /* never returns - starts up kernel */
FUNC_END(_sysInit)


/*****************************************************************************
*
* sysInByte - reads a byte from an address.
*
* SYNOPSIS
* \ss
* UCHAR sysInByte
*     (
*     ULONG       *addr  /@ address of data @/
*     )
* \se
*
* RETURNS: byte from address.
*/

FUNC_BEGIN(sysInByte)
    lbzx    r3,r0,r3                /* Read byte from given address */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysInByte)


/******************************************************************************
*
* sysOutByte - writes a byte to an address.
*
* SYNOPSIS
* \ss
* void sysOutByte
*             (
*             ULONG      *addr  /@ address to write data to @/
*             UCHAR       data  /@ 8-bit data @/
*             )
* \se
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysOutByte)
    stbx    r4,r0,r3                /* Write a byte to given address */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysOutByte)


/*****************************************************************************
*
* sysIn16 - reads a 16-bit unsigned value from an address.
*
* SYNOPSIS
* \ss
* UINT16 sysIn16
*     (
*     UINT16       *addr  /@ address of data @/
*     )
* \se
*
* RETURNS: 16-bit unsigned value from address.
*/

FUNC_BEGIN(sysIn16)
    lhz     3,0(3)                  /* Read a 16 bit value to r3 */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysIn16)


/******************************************************************************
*
* sysOut16 - writes a 16-bit unsigned value to an address.
*
* SYNOPSIS
* \ss
* void sysOut16
*     (
*     UINT16      *addr  - address to write data to
*     UINT16       data  - 8-bit data
*     )
* \se
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysOut16)
    sth     4,0(3)                  /* Write 16 bit value */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysOut16)


/*****************************************************************************
*
* sysIn32 - reads a 32-bit unsigned value from an address.
*
* SYNOPSIS
* \ss
* UINT32 sysIn32
*     (
*     UINT32  *addr  /@ address of data @/
*     )
* \se
*
* RETURNS: 32-bit unsigned value from address.
*/

FUNC_BEGIN(sysIn32)
    lwz     3,0(3)                  /* Read 32 bit value */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysIn32)


/******************************************************************************
*
* sysOut32 - writes a 32-bit unsigned value to an address.
*
* This function writes a 32-bit unsigned value to a specified address.
*
* From a C point of view, the routine is defined as follows:
*
* SYNOPSIS
* \ss
* void sysOut32
*     (
*     UINT32      *addr,  /@ address to write data to @/
*     UINT32       data   /@ 32-bit data @/
*     )
* \se
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysOut32)
    stw    4,0(3)                   /* Write 32 bit value */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysOut32)


/******************************************************************************
*
* sysPciRead32 - read 32 bit PCI (I/O or memory) data
*
* SYNOPSIS
* \ss
* void sysPciRead32
*     (
*     UINT32      *addr,  /@ address of data in PCI space @/
*     UINT32      *pdata  /@ pointer to data being returned @/
*     )                   /@ by the read call ( data is converted @/
*                         /@ to big-endian ) @/
* \se
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysPciRead32)
    lwbrx   r3,r0,r3                /* get the data and swap the bytes */
    sync                            /* Sync I/O operation */
    stw     r3,0(r4)                /* store into address ptd. to by r4 */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysPciRead32)


/******************************************************************************
*
* sysPciWrite32 - write a 32 bit data item to PCI space
*
* SYNOPSIS
* \ss
* void sysPciWrite32
*     (
*     UINT32      *addr,  /@ address to write data to @/
*     UINT32       data   /@ 32-bit big-endian data @/
*     )
* \se
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysPciWrite32)
    stwbrx  r4,r0,r3                /* store data as little-endian */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysPciWrite32)


/*****************************************************************************
*
* sysPciInByte - reads a byte from PCI Config Space.
*
* SYNOPSIS
* \ss
* UCHAR sysPciInByte
*     (
*     UCHAR  *addr	/@ address to read data from @/
*     )
* \se
*
* RETURNS: byte from address.
*/

FUNC_BEGIN(sysPciInByte)
    lbzx    r3,r0,r3                /* Read byte from PCI space */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysPciInByte)


/*****************************************************************************
*
* sysPciInWord - reads a word (16-bit big-endian) from PCI Config Space.
*
* SYNOPSIS
* \ss
* UINT16 sysPciInWord
*     (
*     UCHAR  *addr	/@ address to read data from @/
*     )
* \se
*
* RETURNS: word (16-bit big-endian) from address.
*/

FUNC_BEGIN(sysInWord)
FUNC_BEGIN(sysPciInWord)
    lhbrx   r3,r0,r3                /* Read 16 bit byte reversed */
    sync                            /* Sync I/O operation */
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
* SYNOPSIS
* \ss
* UINT32 sysPciInLong
*     (
*     UCHAR  *addr	/@ address to read data from @/
*     )
* \se
*
* RETURNS: long (32-bit big-endian) from address.
*/

FUNC_BEGIN(sysInLong)
FUNC_BEGIN(sysPciInLong)
    lwbrx   r3,r0,r3                /* Read 32 bit byte reversed */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysInLong)
FUNC_END(sysPciInLong)


/******************************************************************************
*
* sysPciOutByte - writes a byte to PCI Config Space.
*
* This function writes a byte to a specified PCI Config Space address.
*
* SYNOPSIS
* \ss
* void sysPciOutByte
*     (
*     UCHAR  *addr,    /@ PCI config space address to write @/
*     UCHAR  data      /@ byte to write @/
*     )
* \se
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
* SYNOPSIS
* \ss
* void sysPciOutWord
*     (
*     UCHAR  *addr,    /@ PCI config space address to write @/
*     UINT16  data     /@ word (16-bit big-endian) to write @/
*     )
* \se
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysOutWord)
FUNC_BEGIN(sysPciOutWord)
    sthbrx  r4,r0,r3                /* Write byte-reversed 16 bit value */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysOutWord)
FUNC_END(sysPciOutWord)


/******************************************************************************
*
* sysPciOutLong - writes a long (32-bit big-endian) to PCI Config Space.
*
* SYNOPSIS
* \ss
* void sysPciOutByte
*     (
*     UCHAR  *addr,    /@ PCI config space address to write @/
*     UINT32  data     /@ long (32-bit big-endian) to write @/
*     )
* \se
*
* RETURNS: N/A
*/

FUNC_BEGIN(sysOutLong)
FUNC_BEGIN(sysPciOutLong)
    stwbrx  r4,r0,r3                /* Write byte-reversed long */
    sync                            /* Sync I/O operation */
    bclr    20,0                    /* Return to caller */
FUNC_END(sysOutLong)
FUNC_END(sysPciOutLong)


/*******************************************************************************
*
* sysMemProbeSup - sysBusProbe support routine
*
* SYNOPSIS
* \ss
* STATUS sysMemProbeSup
*     (
*     UINT         length,  /@ length of cell to test (1, 2, 4, 8, 16) @/
*     char         *src,    /@ address to read @/
*     char         *dest    /@ address to write @/
*     )
* \se
*
* This routine is called to try to read byte, word, or long, as specified
* by length, from the specified source to the specified destination.
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
* sysL2crPut - write to L2CR register of PPC7400 (Arthur) CPU
*
* SYNOPSIS
* \ss
* void sysL2crPut
*     (
*     ULONG    data     /@  value to write @/
*     )
* \se
*
* RETURNS: NA
*/

FUNC_BEGIN(sysL2crPut)
    mtspr   1017,r3
    bclr    20,0                    /* Return to caller */
FUNC_END(sysL2crPut)


/*******************************************************************************
*
* sysL2crGet - read from L2CR register of PPC7400 (Arthur) CPU
*
* SYNOPSIS
* \ss
* UINT32 sysL2crGet(void)
* \se
*
* RETURNS: UINT32 value of SPR1017 (in r3)
*/

FUNC_BEGIN(sysL2crGet)
    mfspr   r3,1017
    bclr    20,0                    /* Return to caller */
FUNC_END(sysL2crGet)


/******************************************************************************
*
* sysTimeBaseLGet - Get lower half of Time Base Register
*
* SYNOPSIS
* \ss
* UINT32 sysTimeBaseLGet(void)
* \se
*
* This routine will read the contents the lower half of the Time
* Base Register (TBL - TBR 268).
*
* RETURNS: value of TBR 268 (in r3)
*/

FUNC_BEGIN(sysTimeBaseLGet)
    mftb	3
    bclr	20,0                    /* Return to caller */
FUNC_END(sysTimeBaseLGet)


/******************************************************************************
*
* sysHid1Get - read from HID1 register SPR1009.
*
* SYNOPSIS
* \ss
* UINT32 sysHid1Get(void)
* \se
*
* RETURNS: value of SPR1009 (in r3)
*/

FUNC_BEGIN(sysHid1Get)
    mfspr r3,1009
    bclr  20,0                    /* Return to caller */
FUNC_END(sysHid1Get)

/*******************************************************************************
*
* sysSioRead - this function reads a register from the UIO chip
*
* SYNOPSIS
* \ss
* UINT8 sysSioRead
*    ( 
*    UINT8 * pSioIndexReg, 	/@ pointer to SIO index register base addr @/
*    UINT8 sioRegOffset		/@ offset of register to read from @/
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
* register must be written to with the offset of the of the register to be
* modified.  The desired byte of data can then be written via the data
* register.
*
* SYNOPSIS
* \ss
* void sysSioWrite
*    ( 
*    UINT8 * pSioIndexReg, 	/@ pointer to SIO index register base addr @/
*    UINT8 sioRegOffset, 	/@ offset of register to write to @/
*    UINT8 data			/@ 8-bit data to be written @/
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
