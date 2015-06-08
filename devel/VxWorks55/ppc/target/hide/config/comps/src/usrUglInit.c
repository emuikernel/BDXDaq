/* usrUglInit.c - Universal Graphic Library initialization */

/* Copyright 2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,05dec00,jlb  Removed init for simulators, now built into vxWorks BSP
01b,25oct00,rbp  Added X11 hook.
01a,04may00,jlb  written.
*/

/*
DESCRIPTION
This file is used to configure and initialize the VxWorks ugl support.

NOMANUAL
*/

/* include */


/*******************************************************************************
*
* usrUglInit - system-dependent UGL initialization
*
* This routine initializes the UGL stack.
*
* RETURNS: OK, or ERROR if there is a problem in initializing UGL.
*
* NOMANUAL
*/

STATUS usrUglInit ()
    {
    return (OK);
    }

/*******************************************************************************
*
* usrUglExit - system-dependent UGL deinitialization
*
* This routine deinitializes the UGL stack.
*
* RETURNS: OK, or ERROR if there is a problem in deinitializing UGL.
*
* NOMANUAL
*/

STATUS usrUglExit ()

    {
    return (OK);
    }

