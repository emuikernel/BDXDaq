/* sysMotVpdALib.s - Motorola VPD assembly routines */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc. All Rights Reserved */
        .data
	.globl	copyright_wind_river
	.long	copyright_wind_river

/*
modification history
--------------------
01d,25oct01,kab  Update for T2.2
01c,09may01,pch  Add .type directives
01b,08mar01,rhk  WRS coding standards.
01a,21mar00,dmw  Created (from mcpn765/sysMotVpdALib.s).
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

/* defines */

/* convert bit numbering from 0=LSB to 31=LSB (standard to PPC) */

#define	BAUDOUT	(31-BRD_STATUS_REG_BAUD_BIT)

	/* globals */

	FUNC_EXPORT(sysTimeEdges)
	
	_WRS_TEXT_SEG_START

/******************************************************************************
*
* sysTimeEdges - Time BAUDOUT Signal Edges
*
* This function monitors the BAUDOUT signal in the system status register and
* reports the number of time-base ticks required for the caller-specified
* number of BUADOUT edges to occur. It is used to calculate the processor's
* bus speed.  This is not a user-callable function.
*
* RETURNS: unsigned integer containing number of elapsed time base low 
* register ticks
*
* UINT32 sysTimeEdges (UINT32 edges)
*
* NOMANUAL
*/

FUNC_BEGIN(sysTimeEdges)
        mtctr   r3              /* move number of edges to counter */

        lis     r6,HIADJ(BRD_STATUS_REG_ADRS)	/* address of status reg */
        addi    r6,r6,LO(BRD_STATUS_REG_ADRS)

        lbz     r5,0(r6)        /* read and isolate initial state of BAUDOUT */
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
