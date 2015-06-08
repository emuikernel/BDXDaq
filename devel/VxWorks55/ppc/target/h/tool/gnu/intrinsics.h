/* intrinsics.h - map INCLUDE_INTRINSICS to toolchain specific intrinsics */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,06feb02,sn   wrote
*/

/*
DESCRIPTION
Map INCLUDE_INTRINSICS to INCLUDE_GNU_INTRINSICS. Included when TOOL_FAMILY=gnu.
 
NOMANUAL
*/

#ifdef INCLUDE_INTRINSICS
#ifndef INCLUDE_GNU_INTRINSICS
#define INCLUDE_GNU_INTRINSICS
#endif
#endif
