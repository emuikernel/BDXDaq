/* sysALib.s - SunOS system-dependent assembly routines */

/* Copyright 1984-1993 Wind River Systems, Inc. */
        .seg    "data"
        .global copyright_wind_river
        .word   copyright_wind_river

/*
modification history
--------------------
01a,02jul93,gae  written.
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


	.global _start          /* UNIX initialization routine */
	.global _sysInit        /* start of system code */


	.seg    "text"
	.align  4

_sysInit:
	set     _start, %g1
	jmp     %g1
	nop
