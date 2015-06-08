/* usrEntry.c - user entry point for compressed images */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,02jun98,ms   written
*/

/*
DESCRIPTION
The routine usrEntry() is the entry point for compressed images.
The decompression stub inflates the main VxWorks code into RAM,
and then jumps to it. This is the entry point (i.e., the first address).
It is important not to add any other routines before this one.
*/

#include "vxWorks.h"
#include "usrConfig.h"

/******************************************************************************
*
* usrEntry - entry point for _romCompress and _rom images.
*/ 

void usrEntry (int startType)
    {
#if     (CPU_FAMILY==I960)
    sysInitAlt (startType);             /* jump to the i960 entry point */
#endif

#if (CPU_FAMILY==MIPS) && __GNUC__
    __asm volatile (".extern _gp,0; la $gp,_gp");
#endif

    usrInit (startType);                /* all others procede below */
    }

