/* 
 * panic.c --
 *
 *	Source code for the "panic" library procedure for Tcl;
 *	individual applications will probably override this with
 *	an application-specific panic procedure.
 *
 * Copyright (c) 1988-1993 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

static char sccsid[] = "@(#) panic.c 1.6 94/12/17 16:14:02";

#include <stdio.h>

#ifdef NO_STDLIB_H
#   include "compat/stdlib.h"
#else
#   include <stdlib.h>
#endif

/*
 *----------------------------------------------------------------------
 *
 * panic --
 *
 *	Print an error message and kill the process.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The process dies, entering the debugger if possible.
 *
 *----------------------------------------------------------------------
 */

	/* VARARGS ARGSUSED */
void
panic(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
    char *format;		/* Format string, suitable for passing to
				 * fprintf. */
    char *arg1, *arg2, *arg3;	/* Additional arguments (variable in number)
				 * to pass to fprintf. */
    char *arg4, *arg5, *arg6, *arg7, *arg8;
{
    (void) fprintf(stderr, format, arg1, arg2, arg3, arg4, arg5, arg6,
	    arg7, arg8);
    (void) fflush(stderr);
    
#ifndef TCL_VW
    abort();
#else
    /* Probably best to hang around so the problem can be diagnosed
     * 
     * We're probably out of luck getting this to log right, given
     * TCL's string-based style and the fact that logMsg can't guarantee
     * to print strings correctly (and it only takes 6 args!)...
     */
    logMsg(format, arg1, arg2, arg3, arg4, arg5, arg6);
    taskSuspend(0);
#endif
}
