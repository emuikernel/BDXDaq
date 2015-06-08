/* sysVxALib.s - Motorola MTX ROM initialization module */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,01nov01,mil  Use of macros for directives and function doc header updates.
01a,19sep01,dat  written
*/

/*
DESCRIPTION
This module provides vxDecGet and vxHid1Get which are normally
part of the vxALib.o module.  However, MCG Hawk boards need
access to these during the board startup code (romInit.s).

These functions are provided in a separate module used with
the BOOT_EXTRA function for building compressed images. They are
not needed for any other build functions because they will be
provided directly from vxALib.o for all other images.
*/

#define	_ASMLANGUAGE
#include "vxWorks.h"
#include "asm.h"
#include "regs.h"	

        /* Exported internal functions */

        FUNC_EXPORT(vxDecGet)		/* read decrementer */
        FUNC_EXPORT(sysDebugMsg)	/* for mv2400/5100 */

        _WRS_TEXT_SEG_START

		
/***************************************************************************
*
* sysDebugMsg - dummy routine, just a symbol to link with
*
* SYNOPSIS
* \ss
* void sysDebugMsg
*     (
*     char *  str,      /@ string to output @/
*     UINT32  recovery  /@ 0 or EXIT_TO_SYSTEM_MONITOR @/
*     )
* \se
*
* INCLUDE FILES: none
*
* RETURNS: N/A
*
*/

FUNC_BEGIN(sysDebugMsg)
        blr
FUNC_END(sysDebugMsg)


/***************************************************************************
*
* vxDecGet - get the value of the decrementer register
*
* SYNOPSIS
* \ss
* UINT32 vxDecGet
*     (
*     void
*     )
* \se
*
* INCLUDE FILES: none
*
* RETURNS: the value of the decremeter register
*
* This assembly code is needed by hawkI2c.o, which is needed to
* link boot code with.
*
*/

FUNC_BEGIN(vxDecGet)
        mfdec p0
        blr
FUNC_END(vxDecGet)


#if ((CPU == PPC603) || (CPU == PPC604) || (CPU == PPCEC603))

        FUNC_EXPORT(vxHid1Get)		/* return HID1 */

/***************************************************************************
*
* vxHid1Get - get the value of the HID1 register
*
* SYNOPSIS
* \ss
* UINT32 vxHid1Get
*     (
*     void
*     )
* \se
*
* INCLUDE FILES: none
*
* RETURNS: the value of the HID1 register
*
* This assembly code is needed by sysSpeed.o, which we need to determine bus
* and dram speed.  Normally it would be provided by arch/vxALib.o, but we
* can't link boot code against that library.
*
*/

FUNC_BEGIN(vxHid1Get)
        mfspr p0,HID1
        blr
FUNC_END(vxHid1Get)

#endif /* (CPU == PPC603) || (CPU == PPC604) || (CPU == PPCEC603) */

