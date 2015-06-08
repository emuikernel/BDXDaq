/* if_pn_s.s - proNet-80 assembly language support routines */

/* Copyright 1984,1985,1986,1987,1988,1989 Wind River Systems, Inc. */
	.data
	.globl	_copyright_wind_river
	.long	_copyright_wind_river

/* @(#)if_pn_s.s	2.1 7/17/86	(c) 1986 Proteon, Inc. */


/*
modification history
--------------------
01d,24mar89,gae  changed movl's to movel's for Apollo's sake.
01c,10dec88,gae  changed labels for HP's sake.
01b,05sep88,gae  cleaned up.
01a,06nov87,rdc  written.
*/


#define ASMLANGUAGE
#include "vxWorks.h"
#include "asm.h"


    .globl	_movep

    .text
    .even


/*******************************************************************************
*
* movep - move bytes into destination
*
* RETURNS: whether to start on a word

* BOOL movep (from, to, nbytes, flag)
*     char *from;	/* source *
*     char *to;		/* destination *
*     int nbytes;	/* number of bytes to copy *
*     BOOL flag;	/* true = start on word *

*/

_movep:
	movel   sp@(4),a0	/* "from" */
	movel   sp@(8),a1	/* "to" */
	movel   sp@(12),d0	/* "nbytes" */
	bles    lab1		/* == 0, bye */
	clrl    d1
	tstl    sp@(16)		/* "flag" */
	beqs    lab2
	movw    a0@+,a1@	/* copy first word */
	subql   #2,d0
lab2:
	btst    #1,d0		/* all longs? */
	beqs    lab3
	movel   #1,d1		/* word ending */
lab3:
	lsrl    #2,d0		/* bytes -> longs */
	bras    lab5
lab4:
	movel   a0@+,a1@	/* do copy */
lab5:
	dbra    d0,lab4
	movel   d1,d0		/* return flag in d0 */
	beqs    lab1
	movw    a0@,a1@		/* copy last word */
lab1:
	rts
