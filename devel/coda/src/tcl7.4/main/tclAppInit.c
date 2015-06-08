/* 
 * tclAppInit.c --
 *
 *	Provides a default version of the main program and Tcl_AppInit
 *	procedure for Tcl applications (without Tk).
 *
 * Copyright (c) 1993 The Regents of the University of California.
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

static char sccsid[] = "@(#) tclAppInit.c 1.13 95/06/08 10:55:54";

#include "tcl.h"

#ifndef TCL_VW

/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 */
extern int matherr();
int *tclDummyMathPtr = (int *) matherr;

#endif


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

#ifdef TCL_VW
tclMain(char *arg1, char *arg2, char *arg3, char *arg4, char *arg5,
	char *arg6, char *arg7, char *arg8)
{
    int argc=1;				/* Number of arguments. */
    char *argv[9];			/* Array of argument strings. */
    
    /* Parse command-line arguments. */

    argv[0] = "tclMain";
    argv[1] = arg1;
    argv[2] = arg2;
    argv[3] = arg3;
    argv[4] = arg4;
    argv[5] = arg5;
    argv[6] = arg6;
    argv[7] = arg7;
    argv[8] = arg8;

    {
      int i;
      for (i=1;i<9;i++)
	if (argv[i] != 0) argc = i+1;
    }
	
#else
main(argc, argv)
      int argc;			/* Number of command-line arguments. */
      char **argv;		/* Values of command-line arguments. */
   {
#endif
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

#ifndef TCL_VW	/* probably don't have "init.tcl" handy */
    if (Tcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
#endif

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

    /*
     * Call Tcl_CreateCommand for application-specific commands, if
     * they weren't already created by the init procedures called above.
     */

    /*
     * Specify a user-specific startup file to invoke if the application
     * is run interactively.  Typically the startup file is "~/.apprc"
     * where "app" is the name of the application.  If this line is deleted
     * then no user-specific startup file will be run under any conditions.
     */

#ifndef TCL_VW
    tcl_RcFileName = "~/.tclshrc";
#endif
    return TCL_OK;
}
