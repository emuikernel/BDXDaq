/* Sergey: add this file to implement sysTimeBaseLGet() etc */
/* copied from prpmc880 and modified */
/* sysAUtil.s - system-dependent assembly routines */

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

#define TBLR		    284		 /* Time Base Lower Register */


/* globals */

    FUNC_EXPORT(sysTimeBaseLPut)
    FUNC_EXPORT(sysTimeBaseLGet)


/* externals */
     
    _WRS_TEXT_SEG_START

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

