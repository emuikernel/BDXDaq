/* 
 * main.c --
 *
 *	Main program for Tcl shells and other Tcl-based applications.
 *
 * Copyright (c) 1988-1994 The Regents of the University of California.
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 * Copyright (c) 1994-1995 ICEM CFD, Inc
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

static char sccsid[] = "@(#) tclMain.c 1.22 95/09/27 15:32:01";

#include <stdio.h>
#include <errno.h>
#include "tcl.h"
#include "tclInt.h" /*Sergey: prototype description*/
#ifdef NO_STDLIB_H
#   include "compat/stdlib.h"
#else
#   include <stdlib.h>
#endif

/*
 * Declarations for various library procedures and variables (don't want
 * to include tclPort.h here, because people might copy this file out of
 * the Tcl source directory to make their own modified versions).
 * Note:  "exit" should really be declared here, but there's no way to
 * declare it without causing conflicts with other definitions elsewher
 * on some systems, so it's better just to leave it out.
 */

extern int		errno;
extern int		isatty _ANSI_ARGS_((int fd));
extern char *		strcpy _ANSI_ARGS_((char *dst, CONST char *src));

int               forceExit = 0;/* When users quit, forceExit set to 1.
				   This allows graceful exit from tcl */
static Tcl_Interp *interp;	/* Interpreter for application. */
static Tcl_DString command;	/* Used to buffer incomplete commands being
				 * read from stdin. */
#ifdef TCL_MEM_DEBUG
static char dumpFile[100];	/* Records where to dump memory allocation
				 * information. */
#endif

/*
 * Forward references for procedures defined later in this file:
 */

#ifdef TCL_MEM_DEBUG
static int		CheckmemCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char *argv[]));
#endif


void Tcl_UserFinish();

/*
 *----------------------------------------------------------------------
 *
 * Tcl_Main --
 *
 *	Main program for tclsh and most other Tcl-based applications.
 *
 * Results:
 *	None. This procedure never returns (it exits the process when
 *	it's done.
 *
 * Side effects:
 *	This procedure initializes the Tk world and then starts
 *	interpreting commands;  almost anything could happen, depending
 *	on the script being interpreted.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_Main(argc, argv, appInitProc)
    int argc;				/* Number of arguments. */
    char **argv;			/* Array of argument strings. */
    Tcl_AppInitProc *appInitProc;	/* Application-specific initialization
					 * procedure to call after most
					 * initialization but before starting
					 * to execute commands. */
{
    char buffer[1000], *cmd, *args, *fileName;
    int code, gotPartial, tty;
    int exitCode = 0;

    interp = Tcl_CreateInterp();
#ifdef TCL_MEM_DEBUG
    Tcl_InitMemory(interp);
    Tcl_CreateCommand(interp, "checkmem", CheckmemCmd, (ClientData) 0,
	    (Tcl_CmdDeleteProc *) NULL);
#endif

    /*
     * Make command-line arguments available in the Tcl variables "argc"
     * and "argv".  If the first argument doesn't start with a "-" then
     * strip it off and use it as the name of a script file to process.
     */

    fileName = NULL;
    if ((argc > 1) && (argv[1][0] != '-')) {
	fileName = argv[1];
	argc--;
	argv++;
    }
    args = Tcl_Merge(argc-1, argv+1);
    Tcl_SetVar(interp, "argv", args, TCL_GLOBAL_ONLY);
    ckfree(args);
    sprintf(buffer, "%d", argc-1);
    Tcl_SetVar(interp, "argc", buffer, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "argv0", (fileName != NULL) ? fileName : argv[0],
	    TCL_GLOBAL_ONLY);

    /*
     * Set the "tcl_interactive" variable.
     */

    tty = isatty(0);
    Tcl_SetVar(interp, "tcl_interactive",
	    ((fileName == NULL) && tty) ? "1" : "0", TCL_GLOBAL_ONLY);

    /*
     * Invoke application-specific initialization.
     */

    if ((*appInitProc)(interp) != TCL_OK) {
	fprintf(stderr, "application-specific initialization failed: %s\n",
		interp->result);
    }

    /*
     * If a script file was specified then just source that file
     * and quit.
     */

    if (fileName != NULL) {
	code = Tcl_EvalFile(interp, fileName);
	if (code != TCL_OK) {
	    fprintf(stderr, "%s\n", interp->result);
	    exitCode = 1;
	}
	goto done;
    }

    /*
     * We're running interactively.  Source a user-specific startup
     * file if the application specified one and if the file exists.
     */

    if (tcl_RcFileName != NULL) {
	Tcl_DString buffer;
	char *fullName;
	FILE *f;

	fullName = Tcl_TildeSubst(interp, tcl_RcFileName, &buffer);
	if (fullName == NULL) {
	    fprintf(stderr, "%s\n", interp->result);
	} else {
	    f = fopen(fullName, "r");
	    if (f != NULL) {
		code = Tcl_EvalFile(interp, fullName);
		if (code != TCL_OK) {
		    fprintf(stderr, "%s\n", interp->result);
		}
		fclose(f);
	    }
	}
	Tcl_DStringFree(&buffer);
    }

    /*
     * Process commands from stdin until there's an end-of-file.
     */

    gotPartial = 0;
    Tcl_DStringInit(&command);
    while (1) {
	clearerr(stdin);
	if (tty) {
	    char *promptCmd;

	    promptCmd = Tcl_GetVar(interp,
		gotPartial ? "tcl_prompt2" : "tcl_prompt1", TCL_GLOBAL_ONLY);
	    if (promptCmd == NULL) {
		defaultPrompt:
		if (!gotPartial) {
		    fputs("% ", stdout);
		}
	    } else {
		code = Tcl_Eval(interp, promptCmd);
		if (code != TCL_OK) {
		    fprintf(stderr, "%s\n", interp->result);
		    Tcl_AddErrorInfo(interp,
			    "\n    (script that generates prompt)");
		    goto defaultPrompt;
		}
	    }
	    fflush(stdout);
	}
	if (fgets(buffer, 1000, stdin) == NULL) {
	    if (ferror(stdin)) {
		if (errno == EINTR) {
		    if (tcl_AsyncReady) {
			(void) Tcl_AsyncInvoke((Tcl_Interp *) NULL, 0);
		    }
		    clearerr(stdin);
		} else {
		    goto done;
		}
	    } else {
		if (!gotPartial) {
		    goto done;
		}
	    }
	    buffer[0] = 0;
	}
	cmd = Tcl_DStringAppend(&command, buffer, -1);
	if ((buffer[0] != 0) && !Tcl_CommandComplete(cmd)) {
	    gotPartial = 1;
	    continue;
	}

	gotPartial = 0;
	code = Tcl_RecordAndEval(interp, cmd, 0);
	
	Tcl_DStringFree(&command);
	if (code != TCL_OK) {
	    fprintf(stderr, "%s\n", interp->result);
	} else if (tty) {
	    /*
	     * Important:  use puts and not printf here.  On Solaris 2.3
	     * and 2.4, and perhaps other systems, printf will dump core
	     * if interp->result is big.
	     */

	    Tcl_OvrLoadResult(interp);
	    if (*interp->result != 0) puts(interp->result);
	}
	if (forceExit) {
	    Tcl_DeleteInterp(interp);
	    Tcl_EnvFinish();
	    Tcl_FreePosixError();
	    Tcl_UserFinish();
#ifdef TCL_MEM_DEBUG
	    Tcl_DumpActiveMemory(dumpFile);
#ifdef TCL_VW
	    fprintf(stderr, "dumped memory stats - now quitting\n");
	    quitFlag = 0;	/* otherwise, we can't ever run again! */
#endif
	    exit(0);
#else
	    exit(quitCode);
#endif
 	}
    }

    /*
     * Rather than calling exit, invoke the "exit" command so that
     * users can replace "exit" with some other command to do additional
     * cleanup on exit.  The Tcl_Eval call should never return.
     */

    done:
#ifdef TCL_VW
    Tcl_DStringFree(&command);
    Tcl_DeleteInterp(interp);
    fprintf(stderr, "EOF\n");
#else
    sprintf(buffer, "exit %d", exitCode);
    Tcl_Eval(interp, buffer);

    /* If do return, then it's test exit that is waiting for
       graceful rundown.  Rundown allocated objects ... */

    Tcl_DeleteInterp(interp);
    Tcl_EnvFinish();
    Tcl_FreePosixError();
    Tcl_UserFinish();
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * CheckmemCmd --
 *
 *	This is the command procedure for the "checkmem" command, which
 *	causes the application to exit after printing information about
 *	memory usage to the file passed to this command as its first
 *	argument.
 *
 * Results:
 *	Returns a standard Tcl completion code.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
#ifdef TCL_MEM_DEBUG

	/* ARGSUSED */
static int
CheckmemCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Not used. */
    Tcl_Interp *interp;			/* Interpreter for evaluation. */
    int argc;				/* Number of arguments. */
    char *argv[];			/* String values of arguments. */
{
    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileName\"", (char *) NULL);
	return TCL_ERROR;
    }
    strcpy(dumpFile, argv[1]);
    forceExit = 1;
    quitCode  = 0;
    return TCL_OK;
}
#endif
