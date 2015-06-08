/* 
 * tclTest.c --
 *
 *	This file contains C command procedures for a bunch of additional
 *	Tcl commands that are used for testing out Tcl's C interfaces.
 *	These commands are not normally included in Tcl applications;
 *	they're only used for testing.
 *
 * Copyright (c) 1993 The Regents of the University of California.
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 * Copyright (c) 1995 ICEM CFD, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef lint
static char sccsid[] = "@(#) tclTest.c 1.25 95/06/08 10:55:56";
#endif /* not lint */

#include "tclInt.h"
#include <signal.h>

/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	This is the main program for the application.
 *
 * Results:
 *	None: Tcl_Main never returns here, so this procedure never
 *	returns either.
 *
 * Side effects:
 *	Whatever the application does.
 *
 *----------------------------------------------------------------------
 */

int
main(int argc, char **argv)
{
    Tcl_Main(argc, argv, Tcl_AppInit);
    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppInit --
 *
 *	This procedure performs application-specific initialization.
 *	Most applications, especially those that incorporate additional
 *	packages, will have their own version of this procedure.
 *
 * Results:
 *	Returns a standard Tcl completion code, and leaves an error
 *	message in interp->result if an error occurs.
 *
 * Side effects:
 *	Depends on the startup script.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_AppInit(interp)
    Tcl_Interp *interp;		/* Interpreter for application. */
{
    /*
     * Call the init procedures for included packages.  Each call should
     * look like this:
     *
     * if (Mod_Init(interp) == TCL_ERROR) {
     *     return TCL_ERROR;
     * }
     *
     * where "Mod" is the name of the module.
     */

    if (Tcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }

    if (Tcl_TestInit(interp) == TCL_ERROR)
	return TCL_ERROR;
    
    tcl_RcFileName = "~/.tclshrc";
    return TCL_OK;
}
