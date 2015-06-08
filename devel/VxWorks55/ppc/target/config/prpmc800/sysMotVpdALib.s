/* sysMotVpdALib.s - Motorola PPMC750 VPD assembly routines */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1999 Motorola, Inc. All Rights Reserved */
    .data
	.globl	copyright_wind_river
	.long	copyright_wind_river

/*
modification history
--------------------
01a,31aug00,dmw  Written (from version 0b of mcpn765/sysMotVpdALib.s).
*/

/*
DESCRIPTION
This module contains VPD routines written in assembly
language.

*/

#define _ASMLANGUAGE
#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "regs.h"	
#include "asm.h"
#include "harrier.h"

    /* defines */

#define HARRIER_U0BO 6  /* bit 6 of UCTL */

    /* convert bit numbering from 0=LSB to 31=LSB (standard to PPC) */

#define BAUDOUT      (31-HARRIER_U0BO)

	/* globals */
	FUNC_EXPORT(sysTimeEdges)
        FUNC_EXPORT(sysMotGetDec)
	
	_WRS_TEXT_SEG_START
	
/******************************************************************************
*
* sysTimeEdges - Time HARRIER_UCTL_U0BO Signal Edges
*
* This function monitors the HARRIER_UCTL_U0BO signal in the system status 
* register and reports the number of time-base ticks required for the caller-
* specified number of HARRIER_UCTL_U0BO edges to occur. It is used to 
* calculate the processor's bus speed.
*
* From a C programmers point of view, the routine is defined as follows:
* SYNOPSIS
* \ss
*       UINT32 sysTimeEdges (UINT32 edges);
* \se
* INPUTS:
* r3    = number of edges to time
*
* OUTPUTS
* r3    = number of elapsed time base low register ticks
*
*/
FUNC_BEGIN(sysTimeEdges)
        mtctr   r3              /* move number of edges to counter */

        lis     r6,HIADJ(HARRIER_UART_CONTROL_REG)	/* address of UART reg */
        addi    r6,r6,LO(HARRIER_UART_CONTROL_REG)

        lbz     r5,0(r6)        /* read initial state of HARRIER_UCTL_U0BO */
        rlwinm  r5,r5,0,BAUDOUT,BAUDOUT
loop1:
        lbz     r4,0(r6)        /* find an edge */
        rlwinm  r4,r4,0,BAUDOUT,BAUDOUT
        cmplw   r4,r5
        beq     loop1

        mftb    r3              /* found edge, snapshot time base low */
loop2:
        mr      r5,r4           /* save new reference state */
loop3:
        lbz     r4,0(r6)        /* find an edge */
        rlwinm  r4,r4,0,BAUDOUT,BAUDOUT
        cmplw   r4,r5
        beq     loop3

        bdnz    loop2           /* found edge, fall through if last one */
        mftb    r4              /* snapshot time base low */
        subf    r3,r3,r4        /* calculate elapsed time */
        blr                     /* return */
FUNC_END(sysTimeEdges)

/******************************************************************************
*
* sysMotGetDec - read from the Decrementer register SPR22.
*
* This routine will read the contents the decrementer (SPR22)
*
* From a C point of view, the routine is defined as follows:
* SYNOPSIS:
* \ss
*    UINT sysMotGetDec()
* \se
* RETURNS: value of SPR22 (in r3)
*/

FUNC_BEGIN(sysMotGetDec)
    mfspr r3,22
    bclr 20,0
FUNC_END(sysMotGetDec)
