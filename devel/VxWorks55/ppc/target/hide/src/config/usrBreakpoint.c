/* usrBreakpoint.c - user configurable breakpoint management */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,13jan98,dbt  written.
*/

/*
DESCRIPTION

This file contains user configurable breakpoint management routines used 
by target shell debugger and WDB debugger.
This file is included by usrConfig.c.

SEE ALSO: usrExtra.c

NOMANUAL
*/

#ifndef  __INCusrBreakpointc
#define  __INCusrBreakpointc

/* includes */

#include "vxWorks.h"

#include "vxLib.h"
#include "cacheLib.h"

/******************************************************************************
*
* usrBreakpointSet - set a text breakpoint.
*
* This routine is used by both target shell and WDB debuggers to set a 
* text breakpoint in memory.
* This routine can be modified by the user to set a breakpoint differently
* (for example to set a text breakpoint in a ROM emulator).
*
* RETURNS : NA
*
* NOMANUAL
*/ 

void usrBreakpointSet
    (
    INSTR *	addr,		/* breakpoint address */
    INSTR 	value		/* breakpoint instruction */
    )
    {
    void *	pageAddr;	/* page address */
    int		pageSize;	/* page size */

    if ( addr == (INSTR *)NULL )
        return;			/* no error messages available! */

    pageSize = VM_PAGE_SIZE_GET();
    pageAddr = (void *) ((UINT) addr & ~(pageSize - 1));

    VM_TEXT_PAGE_PROTECT(pageAddr, FALSE);
    *addr = value;
    VM_TEXT_PAGE_PROTECT(pageAddr, TRUE);

    CACHE_TEXT_UPDATE (addr, sizeof (INSTR));
    }

#endif /* __INCusrBreakpointc */
