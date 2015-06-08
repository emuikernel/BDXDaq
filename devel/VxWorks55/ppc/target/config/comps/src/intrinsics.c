/* intrinsics.c - force-link of toolchain dependent C intrinsics */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,22apr02,sn   SPR 76106 - added common (toolchain independent) intrinsics
01a,04feb02,sn   wrote
*/

/*
DESCRIPTION
This file is used to include support for toolchain dependent 
intrinsics.
 
NOMANUAL
*/

#include "vxWorks.h"

/* pull in intrinsics if dynamic module downloading is enabled */
#if (defined(INCLUDE_WDB) || defined(INCLUDE_LOADER))
#define INCLUDE_INTRINSICS
#endif

#include TOOL_HEADER(intrinsics.h)

extern char __common_intrinsicsInit;

#ifdef INCLUDE_GNU_INTRINSICS
extern char __gcc_intrinsicsInit;

char * __includeGnuIntrinsics [] = 
    {
    &__gcc_intrinsicsInit,
    &__common_intrinsicsInit
    };
#endif

#ifdef INCLUDE_DIAB_INTRINSICS
extern char __diab_intrinsics_impfpInit;
extern char __diab_intrinsics_implInit;
extern char __diab_intrinsics_tornInit;

char * __includeDiabIntrinsics [] =
    {
    &__diab_intrinsics_impfpInit,
    &__diab_intrinsics_implInit,
    &__diab_intrinsics_tornInit,
    &__common_intrinsicsInit
    };
#endif
