/* mmuArchVars.h - globals for PowerPC MMU */

/* Cpoyright 1998 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01b,12nov98,elg  modified to have coding conventions
01a,06nov98,elg  written
*/

/*
DESCRIPTION
This module contains external variables declarations of MMU library that are
used by other libraries even when MMU is disabled in the BSP.
On CPUs that do not support MMU or when MMU is not used, the following variables
assume that MMU is disabled.
*/

#ifndef __INCmmuArchVarsh
#define __INCmmuArchVarsh

#ifdef __cplusplus
extern "C" {
#endif

/* externals */

IMPORT BOOL	mmuPpcIEnabled;		/* Instruction MMU Status */
IMPORT BOOL	mmuPpcDEnabled;		/* Data MMU Status */

#ifdef __cplusplus
}
#endif

#endif	/* __INCmmuArchVarsh */
