/* 
 * tclProc.c --
 *
 *	This file contains routines that implement Tcl procedures,
 *	including the "proc" and "uplevel" commands.
 *
 * Copyright (c) 1987-1993 The Regents of the University of California.
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 * Copyright (c) 1994-1995 ICEM CFD Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * ========================================================================
 * >>>>>>>>>>>>>>>> INCLUDES MODIFICATIONS FOR [incr Tcl] <<<<<<<<<<<<<<<<<
 *
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: tclProc.c,v 1.1.1.1 1996/08/21 19:30:13 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

static char sccsid[] = "@(#) tclProc.c 1.71 95/03/29 11:24:22";

#include "tclInt.h"

/*
 * Forward references to procedures defined later in this file:
 */

static  void	ProcDeleteProc _ANSI_ARGS_((ClientData clientData));

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ProcCmd --
 *
 *	This procedure is invoked to process the "proc" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	A new procedure gets created.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */

int
Tcl_ProcCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Interp *iPtr = (Interp *) interp;
    Proc *procPtr;
    Namespace *nsPtr;
    char *name;
    int specificRef;
    
    int cont = 1;
    char **targv = argv;
    char **argArray = NULL;
    Arg *lastArgPtr;
    register Arg *argPtr = NULL;	/* Initialization not needed, but
					 * prevents compiler warning. */

    argc--;
    targv++;
    while (argc > 0 && cont) {
	if (**targv == '-') {
	    targv++;
	    argc--;
	}
	else cont = 0;
    }
    if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" option_list name args body\"", (char *) NULL);
	return TCL_ERROR;
    }
 
    if (ItclFollowNamespPath(interp, iPtr->activeNs, argv[1],
        ITCL_FIND_AUTO_CREATE, &nsPtr,&name,&specificRef) != TCL_OK)
        return TCL_ERROR;
    if (nsPtr == NULL) return TCL_ERROR;
    
    if (name == NULL) {
        Tcl_AppendResult(interp,
            "can't create proc \"", targv[0], "\": missing proc name",
            (char*)NULL);
        return TCL_ERROR;
    }

    if (TclCreateProc(interp,nsPtr,targv[0],targv[1],targv[2],
        &procPtr) != TCL_OK) {
        return TCL_ERROR;
    }

    Tcl_CreateCommand(interp, targv[0], TclInterpProc, (ClientData) procPtr,
	    ProcDeleteProc);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * TclGetFrame --
 *
 *	Given a description of a procedure frame, such as the first
 *	argument to an "uplevel" or "upvar" command, locate the
 *	call frame for the appropriate level of procedure.
 *
 * Results:
 *	The return value is -1 if an error occurred in finding the
 *	frame (in this case an error message is left in interp->result).
 *	1 is returned if string was either a number or a number preceded
 *	by "#" and it specified a valid frame.  0 is returned if string
 *	isn't one of the two things above (in this case, the lookup
 *	acts as if string were "1").  The variable pointed to by
 *	framePtrPtr is filled in with the address of the desired frame
 *	(unless an error occurs, in which case it isn't modified).
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclGetFrame(interp, string, framePtrPtr)
    Tcl_Interp *interp;		/* Interpreter in which to find frame. */
    char *string;		/* String describing frame. */
    CallFrame **framePtrPtr;	/* Store pointer to frame here (or NULL
				 * if global frame indicated). */
{
    register Interp *iPtr = (Interp *) interp;
    int curLevel, level, result;
    CallFrame *framePtr;

    /*
     * Parse string to figure out which level number to go to.
     */

    result = 1;
    curLevel = (iPtr->varFramePtr == NULL) ? 0 : iPtr->varFramePtr->level;
    if (*string == '#') {
	if (Tcl_GetInt(interp, string+1, &level) != TCL_OK) {
	    return -1;
	}
	if (level < 0) {
	    levelError:
	    Tcl_AppendResult(interp, "bad level \"", string, "\"",
		    (char *) NULL);
	    return -1;
	}
    } else if (isdigit(UCHAR(*string))) {
	if (Tcl_GetInt(interp, string, &level) != TCL_OK) {
	    return -1;
	}
	level = curLevel - level;
    } else {
	level = curLevel - 1;
	result = 0;
    }

    /*
     * Figure out which frame to use, and modify the interpreter so
     * its variables come from that frame.
     */

    if (level == 0) {
	framePtr = NULL;
    } else {
	for (framePtr = iPtr->varFramePtr; framePtr != NULL;
		framePtr = framePtr->callerVarPtr) {
	    if (framePtr->level == level) {
		break;
	    }
	}
	if (framePtr == NULL) {
	    goto levelError;
	}
    }
    *framePtrPtr = framePtr;
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_UplevelCmd --
 *
 *	This procedure is invoked to process the "uplevel" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_UplevelCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Interp *iPtr = (Interp *) interp;
    int result;
    CallFrame *savedVarFramePtr, *framePtr;
    
    Itcl_Namespace ns;
    ClientData cdata;
    Itcl_ActiveNamespace nsToken;

    if (argc < 2) {
	uplevelSyntax:
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?level? command ?arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Find the level to use for executing the command.
     */

    result = TclGetFrame(interp, argv[1], &framePtr);
    if (result == -1) {
	return TCL_ERROR;
    }
    argc -= (result+1);
    if (argc == 0) {
	goto uplevelSyntax;
    }
    argv += (result+1);

    /*
     * Modify the interpreter state to execute in the given frame.
     */
    
    if (Itcl_InitUplevel(interp, framePtr, &savedVarFramePtr, &nsToken)
	!= TCL_OK) return TCL_ERROR;
    
    /*
     * Execute the residual arguments as a command.
     */

    if (argc == 1) {
	result = Tcl_Eval(interp, argv[0]);
    } else {
	char *cmd;

	cmd = Tcl_Concat(argc, argv);
	result = Tcl_Eval(interp, cmd);
	ckfree(cmd);
    }
    if (result == TCL_ERROR) {
	char msg[60];
	sprintf(msg, "\n    (\"uplevel\" body line %d)", interp->errorLine);
	Tcl_AddErrorInfo(interp, msg);
    }

    Itcl_FinishUplevel(interp, savedVarFramePtr, nsToken);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * TclFindProc --
 *
 *	Given the name of a procedure, return a pointer to the
 *	record describing the procedure.
 *
 * Results:
 *	NULL is returned if the name doesn't correspond to any
 *	procedure.  Otherwise the return value is a pointer to
 *	the procedure's record.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Proc *
TclFindProc(iPtr, procName)
    Interp *iPtr;		/* Interpreter in which to look. */
    char *procName;		/* Name of desired procedure. */
{
    Tcl_Command cmd;
    Command *cmdPtr;
    
    if (Itcl_FindCommand((Tcl_Interp*)iPtr, procName, 0, &cmd) == TCL_OK) {
	cmdPtr = (Command*)cmd;
	if (cmdPtr && cmdPtr->proc == TclInterpProc) {
	    return (Proc*)cmdPtr->clientData;
	}
    }
    return NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * TclIsProc --
 *
 *	Tells whether a command is a Tcl procedure or not.
 *
 * Results:
 *	If the given command is actuall a Tcl procedure, the
 *	return value is the address of the record describing
 *	the procedure.  Otherwise the return value is 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Proc *
TclIsProc(cmdPtr)
    Command *cmdPtr;		/* Command to test. */
{
    if (cmdPtr->proc == TclInterpProc) {
	return (Proc *) cmdPtr->clientData;
    }
    return (Proc *) 0;
}

/*
 *----------------------------------------------------------------------
 *
 * TclCreateProc --
 *
 *	Creates the data associated with a Tcl procedure definition.
 *
 * Results:
 *	Argument procPtrPtr returns a pointer to a Tcl procedure
 *	definition.  This should be freed by calling TclCleanupProc()
 *	when no longer needed.
 *
 *	Returns TCL_OK for success, or TCL_ERROR to indicate failure.
 *
 * Side effects:
 *	Returns an error message in the interp along with TCL_ERROR.
 *
 *----------------------------------------------------------------------
 */

int
TclCreateProc(interp, nsPtr, name, args, body, procPtrPtr)
    Tcl_Interp *interp;		/* interpreter containing proc */
    Namespace *nsPtr;           /* namespace containing this proc */
    char *name;			/* name of this proc */
    char *args;			/* description of arguments */
    char *body;			/* command body */
    Proc **procPtrPtr;		/* returns:  pointer to proc data */
{
    Interp *iPtr = (Interp*)interp;
    register Proc *procPtr;
    int result = TCL_OK;

    int i, argCount;
    char **argArray = NULL;
    Arg *lastArgPtr;
    register Arg *argPtr = NULL;	/* Initialization not needed, but
					 * prevents compiler warning. */

    procPtr = (Proc*)ckalloc(sizeof(Proc));
    procPtr->iPtr = iPtr;
    procPtr->refCount = 1;
    procPtr->command = (char*)ckalloc((unsigned)strlen(body) + 1);
    strcpy(procPtr->command, body);
    procPtr->argPtr = NULL;
    procPtr->namesp = nsPtr;

    /*
     * Break up the argument list into argument specifiers, then process
     * each argument specifier.
     */

    result = Tcl_SplitList(interp, args, &argCount, &argArray);
    if (result != TCL_OK) {
	goto procError;
    }
    lastArgPtr = NULL;
    for (i = 0; i < argCount; i++) {
	int fieldCount, nameLength, valueLength;
	char **fieldValues;

	/*
	 * Now divide the specifier up into name and default.
	 */

	result = Tcl_SplitList(interp, argArray[i], &fieldCount,
		&fieldValues);
	if (result != TCL_OK) {
	    goto procError;
	}
	if (fieldCount > 2) {
	    ckfree((char *) fieldValues);
	    Tcl_AppendResult(interp,
		    "too many fields in argument specifier \"",
		    argArray[i], "\"", (char *) NULL);
	    result = TCL_ERROR;
	    goto procError;
	}
	if ((fieldCount == 0) || (*fieldValues[0] == 0)) {
	    ckfree((char *) fieldValues);
	    Tcl_AppendResult(interp, "procedure \"", name,
		    "\" has argument with no name", (char *) NULL);
	    result = TCL_ERROR;
	    goto procError;
	}
	nameLength = strlen(fieldValues[0]) + 1;
	if (fieldCount == 2) {
	    valueLength = strlen(fieldValues[1]) + 1;
	} else {
	    valueLength = 0;
	}
	argPtr = (Arg *) ckalloc((unsigned)
		(sizeof(Arg) - sizeof(argPtr->name) + nameLength
		+ valueLength));
	if (lastArgPtr == NULL) {
	    procPtr->argPtr = argPtr;
	} else {
	    lastArgPtr->nextPtr = argPtr;
	}
	lastArgPtr = argPtr;
	argPtr->nextPtr = NULL;
	strcpy(argPtr->name, fieldValues[0]);
	if (fieldCount == 2) {
	    argPtr->defValue = argPtr->name + nameLength;
	    strcpy(argPtr->defValue, fieldValues[1]);
	} else {
	    argPtr->defValue = NULL;
	}
	ckfree((char *) fieldValues);
    }

    *procPtrPtr = procPtr;
    ckfree((char*)argArray);

    return TCL_OK;

procError:
    ckfree(procPtr->command);
    while (procPtr->argPtr != NULL) {
	argPtr = procPtr->argPtr;
	procPtr->argPtr = argPtr->nextPtr;
	ckfree((char*)argPtr);
    }
    ckfree((char*)procPtr);

    if (argArray != NULL) {
	ckfree((char*)argArray);
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * TclInterpProc --
 *
 *	When a Tcl procedure gets invoked, this routine gets invoked
 *	to interpret the procedure.
 *
 * Results:
 *	A standard Tcl result value, usually TCL_OK.
 *
 * Side effects:
 *	Depends on the commands in the procedure.
 *
 *----------------------------------------------------------------------
 */

int
TclInterpProc(clientData, interp, argc, argv)
    ClientData clientData;	/* Record describing procedure to be
				 * interpreted. */
    Tcl_Interp *interp;		/* Interpreter in which procedure was
				 * invoked. */
    int argc;			/* Count of number of arguments to this
				 * procedure. */
    char **argv;		/* Argument values. */
{
    register Proc        *procPtr = (Proc *) clientData;
    register Arg         *argPtr;
    register Interp      *iPtr;
    char                 **args;
    CallFrame            frame;
    char                 *value;
    int                  result;
    Itcl_ActiveNamespace nsToken;
    
    /*
     * Set up a call frame for the new procedure invocation.  Also
     * increments the reference counter
     */

    if (Itcl_InitProc(interp, clientData, &frame, argc, argv, &nsToken)
	!= TCL_OK) return TCL_ERROR;

    /*
     * Match the actual arguments against the procedure's formal
     * parameters to compute local variables.
     */

    iPtr = procPtr->iPtr;
    for (argPtr = procPtr->argPtr, args = argv+1, argc -= 1;
	    argPtr != NULL;
	    argPtr = argPtr->nextPtr, args++, argc--) {

	/*
	 * Handle the special case of the last formal being "args".  When
	 * it occurs, assign it a list consisting of all the remaining
	 * actual arguments.
	 */

	if ((argPtr->nextPtr == NULL)
		&& (strcmp(argPtr->name, "args") == 0)) {
	    if (argc < 0) {
		argc = 0;
	    }
	    value = Tcl_Merge(argc, args);
	    Tcl_SetVar(interp, argPtr->name, value, 0);
	    ckfree(value);
	    argc = 0;
	    break;
	} else if (argc > 0) {
	    value = *args;
	} else if (argPtr->defValue != NULL) {
	    value = argPtr->defValue;
	} else {
	    Tcl_AppendResult(interp, "no value given for parameter \"",
		    argPtr->name, "\" to \"", argv[0], "\"",
		    (char *) NULL);
	    result = TCL_ERROR;
	    goto procDone;
	}
	Tcl_SetVar(interp, argPtr->name, value, 0);
    }
    if (argc > 0) {
	Tcl_AppendResult(interp, "called \"", argv[0],
		"\" with too many arguments", (char *) NULL);
	result = TCL_ERROR;
	goto procDone;
    }

    /*
     * Invoke the commands in the procedure's body.
     */

    procPtr->refCount++;
    result = Tcl_Eval(interp, procPtr->command);
    procPtr->refCount--;
    if (procPtr->refCount <= 0) {
	TclCleanupProc(procPtr);
    }
    if (result == TCL_RETURN) {
	result = TclUpdateReturnInfo(iPtr);
    } else if (result == TCL_ERROR) {
	char msg[100];

	/*
	 * Record information telling where the error occurred.
	 */

	sprintf(msg, "\n    (procedure \"%.50s\" line %d)", argv[0],
		iPtr->errorLine);
	Tcl_AddErrorInfo(interp, msg);
    } else if (result == TCL_BREAK) {
	iPtr->result = "invoked \"break\" outside of a loop";
	result = TCL_ERROR;
    } else if (result == TCL_CONTINUE) {
	iPtr->result = "invoked \"continue\" outside of a loop";
	result = TCL_ERROR;
    }

    /*
     * Delete the call frame for this procedure invocation (it's
     * important to remove the call frame from the interpreter
     * before deleting it, so that traces invoked during the
     * deletion don't see the partially-deleted frame).
     */

    procDone:

    /* Validate the return if the return is in the overloaded
       result structure ... */
    
    if (iPtr->results.typeAttr & TCL_TYPE_RETOVR)
	Tcl_OvrLoadResult(interp);
    
    Itcl_FinishProc(interp, clientData, &frame, nsToken);
    return result;
}


/*
 *----------------------------------------------------------------------
 *
 * ProcDeleteProc --
 *
 *	This procedure is invoked just before a command procedure is
 *	removed from an interpreter.  Its job is to release all the
 *	resources allocated to the procedure.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory gets freed, unless the procedure is actively being
 *	executed.  In this case the cleanup is delayed until the
 *	last call to the current procedure completes.
 *
 *----------------------------------------------------------------------
 */

static void
ProcDeleteProc(clientData)
    ClientData clientData;		/* Procedure to be deleted. */
{
    Proc *procPtr = (Proc *) clientData;

    procPtr->refCount--;
    if (procPtr->refCount <= 0) {
	TclCleanupProc((ClientData)procPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TclCleanupProc --
 *
 *	This procedure does all the real work of freeing up a Proc
 *	structure.  It's called only when the structure's reference
 *	count becomes zero.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory gets freed.
 *
 *----------------------------------------------------------------------
 */
void
TclCleanupProc(cdata)
    ClientData cdata;       	/* Procedure to be deleted. */
{
    register Proc *procPtr = (Proc*)cdata;
    register Arg *argPtr;

    if (procPtr->command) ckfree((char *) procPtr->command);
    for (argPtr = procPtr->argPtr; argPtr != NULL; ) {
	Arg *nextPtr = argPtr->nextPtr;

	ckfree((char *) argPtr);
	argPtr = nextPtr;
    }
    ckfree((char *) procPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * TclUpdateReturnInfo --
 *
 *	This procedure is called when procedures return, and at other
 *	points where the TCL_RETURN code is used.  It examines fields
 *	such as iPtr->returnCode and iPtr->errorCode and modifies
 *	the real return status accordingly.
 *
 * Results:
 *	The return value is the true completion code to use for
 *	the procedure, instead of TCL_RETURN.
 *
 * Side effects:
 *	The errorInfo and errorCode variables may get modified.
 *
 *----------------------------------------------------------------------
 */

int
TclUpdateReturnInfo(iPtr)
    Interp *iPtr;		/* Interpreter for which TCL_RETURN
				 * exception is being processed. */
{
    int code;

    code = iPtr->returnCode;
    iPtr->returnCode = TCL_OK;
    if (code == TCL_ERROR) {
	Tcl_SetVar2((Tcl_Interp *) iPtr, "errorCode", (char *) NULL,
		(iPtr->errorCode != NULL) ? iPtr->errorCode : "NONE",
		TCL_GLOBAL_ONLY);
	iPtr->flags |= ERROR_CODE_SET;
	if (iPtr->errorInfo != NULL) {
	    Tcl_SetVar2((Tcl_Interp *) iPtr, "errorInfo", (char *) NULL,
		    iPtr->errorInfo, TCL_GLOBAL_ONLY);
	    iPtr->flags |= ERR_IN_PROGRESS;
	}
    }
    return code;
}
