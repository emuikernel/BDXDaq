/* 00arch.cdf -  definitions for arches that only support the GNU compiler */

/* Copyright 1998-2002 Wind River Systems, Inc.  */


/*
modification history
--------------------
01a,13mar02,sn  SPR 73723 - wrote
*/

Selection SELECT_COMPILER_INTRINSICS {
	NAME		Compiler support routines
	SYNOPSIS        Compiler support routines needed to support dynamic module download
	COUNT		1-
	CHILDREN	-= INCLUDE_DIAB_INTRINSICS
}
