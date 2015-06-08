/* 
 * tclBasic.c --
 *
 *	Contains the basic facilities for TCL command interpretation,
 *	including interpreter creation and deletion, command creation
 *	and deletion, and command parsing and execution.
 *
 * Copyright (c) 1987-1994 The Regents of the University of California.
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
 *     RCS:  $Id: tclBasic.c,v 1.3 1997/04/23 17:36:05 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

static char sccsid[] = "@(#) tclBasic.c 1.171 95/09/05 11:37:34";

#ifdef TCL_VW
#  define TCL_CONTEXT
#endif

#include "tclInt.h"
#ifndef TCL_GENERIC_ONLY
#   include "tclPort.h"
#endif
#include "patchlevel.h"

char *tcl_RcFileName = NULL;

/*
 * The following structure defines all of the commands in the Tcl core,
 * and the C procedures that execute them.
 */

typedef struct {
    char *name;			/* Name of command. */
    Tcl_CmdProc *proc;		/* Procedure that executes command. */
} CmdInfo;

/*
 * Built-in commands, and the procedures associated with them:
 */

#ifdef TCL_VW
  extern int Tcl_VxExecCmd();
#endif

static CmdInfo builtInCmds[] = {
    /*
     * Commands in the generic core:
     */

    {"append",		Tcl_AppendCmd},
    {"array",		Tcl_ArrayCmd},
    {"break",		Tcl_BreakCmd},
    {"case",		Tcl_CaseCmd},
    {"catch",		Tcl_CatchCmd},
    {"catch_c",		Tcl_CatchCmd},
    {"command",		Tcl_CommandCompleteCmd},
    {"concat",		Tcl_ConcatCmd},
    {"concat_v",	Tcl_ConcatCmd_v},
    {"continue",	Tcl_ContinueCmd},
    {"error",		Tcl_ErrorCmd},
    {"eval",		Tcl_EvalCmd},
    {"expr",		Tcl_ExprCmd},
    {"exprbool",	Tcl_ExprBoolCmd},
    {"for",		Tcl_ForCmd},
    {"foreach",		Tcl_ForeachCmd},
    {"format",		Tcl_FormatCmd},
    {"global",		Tcl_GlobalCmd},
    {"history",		Tcl_HistoryCmd},
    {"if",		Tcl_IfCmd},
    {"incr",		Tcl_IncrCmd},
    {"info",		Tcl_InfoCmd},
    {"join",		Tcl_JoinCmd},
    {"join_v",		Tcl_JoinCmd_v},
    {"lappend",		Tcl_LappendCmd},
    {"lappend_v",	Tcl_LappendCmd_v},
    {"lindex",		Tcl_LindexCmd},
    {"lindex_v",	Tcl_LindexCmd_v},
    {"linsert",		Tcl_LinsertCmd},
    {"linsert_v",	Tcl_LinsertCmd_v},
    {"linsertl_v",	Tcl_LinsertCmdl_v},
    {"list",		Tcl_ListCmd},
    {"list_v",		Tcl_ListCmd_v},
    {"llength",		Tcl_LlengthCmd},
    {"llength_v",	Tcl_LlengthCmd_v},
    {"lrange",		Tcl_LrangeCmd},
    {"lrange_v",	Tcl_LrangeCmd_v},
    {"lrangel_v",	Tcl_LrangeCmdl_v},
    {"lreplace",	Tcl_LreplaceCmd},
    {"lreplace_v",	Tcl_LreplaceCmd_v},
    {"lreplacel_v",	Tcl_LreplaceCmdl_v},
    {"lsearch",		Tcl_LsearchCmd},
    {"lsearch_v",	Tcl_LsearchCmd_v},
    {"lsort",		Tcl_LsortCmd},
    {"lsort_v",		Tcl_LsortCmd_v},
    {"proc",		Tcl_ProcCmd},
    {"quit",		Tcl_QuitCmd},
    {"regexp",		Tcl_RegexpCmd},
    {"regsub",		Tcl_RegsubCmd},
    {"rename",		Tcl_RenameCmd},
    {"return",		Tcl_ReturnCmd},
    {"scan",		Tcl_ScanCmd},
    {"set",		Tcl_SetCmd},
    {"split",		Tcl_SplitCmd},
    {"string",		Tcl_StringCmd},
    {"subst",		Tcl_SubstCmd},
    {"switch",		Tcl_SwitchCmd},
    {"trace",		Tcl_TraceCmd},
    {"unset",		Tcl_UnsetCmd},
    {"uplevel",		Tcl_UplevelCmd},
    {"uplevel_c",	Tcl_UplevelCmd},
    {"upvar",		Tcl_UpvarCmd},
    {"while",		Tcl_WhileCmd},

    /*
     * Commands in the UNIX core:
     */

#ifndef TCL_GENERIC_ONLY
    {"cd",		Tcl_CdCmd},
    {"close",		Tcl_CloseCmd},
    {"eof",		Tcl_EofCmd},
#ifndef TCL_VW
    {"exec",		Tcl_ExecCmd},
#else
    {"exec",		Tcl_VxExecCmd},
#endif
    {"exit",		Tcl_ExitCmd},
    {"file",		Tcl_FileCmd},
    {"flush",		Tcl_FlushCmd},
    {"gets",		Tcl_GetsCmd},
    {"glob",		Tcl_GlobCmd},
    {"open",		Tcl_OpenCmd},
    {"pid",		Tcl_PidCmd},
    {"puts",		Tcl_PutsCmd},
    {"pwd",		Tcl_PwdCmd},
    {"read",		Tcl_ReadCmd},
    {"seek",		Tcl_SeekCmd},
    {"source",		Tcl_SourceCmd},
    {"tell",		Tcl_TellCmd},
    {"time",		Tcl_TimeCmd},
#endif /* TCL_GENERIC_ONLY */
    {NULL,		(Tcl_CmdProc *) NULL}
};

#ifdef TCL_VW
  /* Need some global semaphores to protect non-reentrant parts */
  SEM_ID  qsortSem = 0;       /* used as a flag for 1-time init */
  SEM_ID  ckAllocSem;
  SEM_ID  regexpSem;
  SEM_ID  miscSem;
  void Tcl_InitSems(void);
#endif /* TCL_VW */


/*
 *----------------------------------------------------------------------
 *
 * Tcl_CreateInterp --
 *
 *	Create a new TCL command interpreter.
 *
 * Results:
 *	The return value is a token for the interpreter, which may be
 *	used in calls to procedures like Tcl_CreateCmd, Tcl_Eval, or
 *	Tcl_DeleteInterp.
 *
 * Side effects:
 *	The command interpreter is initialized with an empty variable
 *	table and the built-in commands.  SIGPIPE signals are set to
 *	be ignored (see comment below for details).
 *
 *----------------------------------------------------------------------
 */

Tcl_Interp *
Tcl_CreateInterp()
{
    register Interp *iPtr;
    register Command *cmdPtr;
    register CmdInfo *cmdInfoPtr;
    int i;
    char *libDir;
    static int firstInterp = 1;

#ifdef TCL_VW
    /* Global semaphore initialization, only done once */
    if (qsortSem == 0)
    {
        Tcl_InitSems();
    }
    
    /* POTENTIAL BUG -assume one call per VX task for now */
    if (taskVarAdd(0, (int *) &Tcl_ctxt) != OK)
    {
        logMsg( "%s(%d) - can't taskVarAdd Tcl_ctxt\n", __FILE__,
                __LINE__);
        taskSuspend(0);
    }
    
    /* allocate zeroed space, so all pointers start out NULL */
    if (( Tcl_ctxt = (Tcl_Context *) calloc(1, sizeof(Tcl_Context))) == NULL)
    {
        logMsg( "%s(%d) - can't malloc Tcl_ctxt\n", __FILE__, __LINE__);
        taskSuspend(0);
    }
    
#ifdef DEBUG1
    logMsg("context = 0x%x fs = 0x%x\n", Tcl_ctxt, Tcl_ctxt->tcpFirstServer);
#endif

#endif /* TCL_VW */

    iPtr = (Interp *) ckalloc(sizeof(Interp));
    iPtr->result = iPtr->resultSpace;
    iPtr->freeProc = 0;
    iPtr->errorLine = 0;
    
    Tcl_InitHashTable(&iPtr->commandTable, TCL_STRING_KEYS);
    Tcl_InitHashTable(&iPtr->mathFuncTable, TCL_STRING_KEYS);
    Tcl_InitHashTable(&iPtr->globalTable, TCL_STRING_KEYS);
    
    iPtr->numLevels = 0;
    iPtr->maxNestingDepth = 1000;
    iPtr->framePtr = NULL;
    iPtr->varFramePtr = NULL;
    iPtr->activeTracePtr = NULL;
    iPtr->returnCode = TCL_OK;
    iPtr->errorInfo = NULL;
    iPtr->errorCode = NULL;
    iPtr->numEvents = 0;
    iPtr->events = NULL;
    iPtr->curEvent = 0;
    iPtr->curEventNum = 0;
    iPtr->revPtr = NULL;
    iPtr->historyFirst = NULL;
    iPtr->revDisables = 1;
    iPtr->evalFirst = iPtr->evalLast = NULL;
    iPtr->appendResult = NULL;
    iPtr->appendAvl = 0;
    iPtr->appendUsed = 0;
    for (i = 0; i < NUM_REGEXPS; i++) {
	iPtr->patterns[i] = NULL;
	iPtr->patLengths[i] = -1;
	iPtr->regexps[i] = NULL;
    }
    strcpy(iPtr->pdFormat, DEFAULT_PD_FORMAT);
    iPtr->pdPrec = DEFAULT_PD_PREC;
    iPtr->cmdCount = 0;
    iPtr->noEval = 0;
    iPtr->evalFlags = 0;
    iPtr->scriptFile = NULL;
    iPtr->flags = 0;
    iPtr->tracePtr = NULL;
    iPtr->deleteCallbackPtr = NULL;
    iPtr->resultSpace[0] = 0;
    
    /*
     * Initialize the interpreter to recognize namespaces.
     */
    ItclNsInterpInit(iPtr);

    Tcl_tVarInitMACRO(&iPtr->results);

    /*
     * Create the built-in commands.  Do it here, rather than calling
     * Tcl_CreateCommand, because it's faster (there's no need to
     * check for a pre-existing command by the same name).
     */

    for (cmdInfoPtr = builtInCmds; cmdInfoPtr->name != NULL; cmdInfoPtr++) {
	int new;
	Tcl_HashEntry *hPtr;
 	hPtr = Tcl_CreateHashEntry(&iPtr->globalNs->commands,
  		cmdInfoPtr->name, &new);
	if (new) {
	    cmdPtr = (Command *) ckalloc(sizeof(Command));
	    cmdPtr->hPtr = hPtr;
	    cmdPtr->proc       = cmdInfoPtr->proc;
	    cmdPtr->clientData = (ClientData) NULL;
	    cmdPtr->deleteProc = NULL;
	    cmdPtr->deleteData = (ClientData) NULL;
	    cmdPtr->deleted = 0;
	    cmdPtr->protection = ITCL_PUBLIC;
	    cmdPtr->namesp = iPtr->globalNs;
	    cmdPtr->cacheInfo = NULL;
	    Tcl_SetHashValue(hPtr, cmdPtr);
	}
    }

#ifdef TCL_VW
    TclSetupEnv((Tcl_Interp *) iPtr);
#endif

#ifndef TCL_GENERIC_ONLY
#ifndef TCL_VW

    TclSetupEnv((Tcl_Interp *) iPtr);

    /*
     * The code below causes SIGPIPE (broken pipe) errors to
     * be ignored.  This is needed so that Tcl processes don't
     * die if they create child processes (e.g. using "exec" or
     * "open") that terminate prematurely.  The signal handler
     * is only set up when the first interpreter is created; 
     * after this the application can override the handler with
     * a different one of its own, if it wants.
     */

    if (firstInterp) {
	(void) signal(SIGPIPE, SIG_IGN);
	firstInterp = 0;
    }
#endif /* !TCL_VW */
#endif

    /*
     * Set up variables such as tcl_library and tcl_precision.
     */

	/*Sergey: do not use env var*/
    /*libDir = getenv("TCL_LIBRARY");
	if (libDir == NULL) {*/
	  libDir = TCL_LIBRARY;
	/*}*/

    Tcl_SetVar((Tcl_Interp *) iPtr, "tcl_library", libDir, TCL_GLOBAL_ONLY);
    Tcl_SetVar((Tcl_Interp *) iPtr, "tcl_patchLevel", TCL_PATCH_LEVEL,
	    TCL_GLOBAL_ONLY);
    Tcl_SetVar((Tcl_Interp *) iPtr, "tcl_version", TCL_VERSION,
	    TCL_GLOBAL_ONLY);
    Tcl_TraceVar2((Tcl_Interp *) iPtr, "tcl_precision", (char *) NULL,
	    TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
	    TclPrecTraceProc, (ClientData) NULL);
    
    /*
     * Install commands for namespaces and ensembles.
     */
    (void) ItclInitNamespCmds((Tcl_Interp*)iPtr);
    (void) ItclInitEnsemble((Tcl_Interp*)iPtr);

    return (Tcl_Interp *) iPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_Init --
 *
 *	This procedure is typically invoked by Tcl_AppInit procedures
 *	to perform additional initialization for a Tcl interpreter,
 *	such as sourcing the "init.tcl" script.
 *
 * Results:
 *	Returns a standard Tcl completion code and sets interp->result
 *	if there is an error.
 *
 * Side effects:
 *	Depends on what's in the init.tcl script.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_Init(interp)
    Tcl_Interp *interp;		/* Interpreter to initialize. */
{
    static char initCmd[] =
	"if [file exists [info library]/init.tcl] {\n\
	    source [info library]/init.tcl\n\
	} else {\n\
	    set msg \"can't find [info library]/init.tcl; \"\n\
	    append msg \"perhaps you need to install Tcl ?\"\n\
	    error $msg\n\
	}";
	/*Sergey
	    set msg \"can't find [info library]/init.tcl; perhaps you \"\n\
	    append msg \"need to\\ninstall Tcl or set your TCL_LIBRARY \"\n\
	    append msg \"environment variable?\"\n\
	    error $msg\n\
	*/
    return Tcl_Eval(interp, initCmd);
}

/*
 *--------------------------------------------------------------
 *
 * Tcl_CallWhenDeleted --
 *
 *	Arrange for a procedure to be called before a given
 *	interpreter is deleted.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	When Tcl_DeleteInterp is invoked to delete interp,
 *	proc will be invoked.  See the manual entry for
 *	details.
 *
 *--------------------------------------------------------------
 */

void
Tcl_CallWhenDeleted(interp, proc, clientData)
    Tcl_Interp *interp;		/* Interpreter to watch. */
    Tcl_InterpDeleteProc *proc;	/* Procedure to call when interpreter
				 * is about to be deleted. */
    ClientData clientData;	/* One-word value to pass to proc. */
{
    DeleteCallback *dcPtr, *prevPtr;
    Interp *iPtr = (Interp *) interp;

    dcPtr = (DeleteCallback *) ckalloc(sizeof(DeleteCallback));
    dcPtr->proc = proc;
    dcPtr->clientData = clientData;
    dcPtr->nextPtr = NULL;
    if (iPtr->deleteCallbackPtr == NULL) {
	iPtr->deleteCallbackPtr = dcPtr;
    } else {
	prevPtr = iPtr->deleteCallbackPtr;
	while (prevPtr->nextPtr != NULL) {
	    prevPtr = prevPtr->nextPtr;
	}
	prevPtr->nextPtr = dcPtr;
    }
}

/*
 *--------------------------------------------------------------
 *
 * Tcl_DontCallWhenDeleted --
 *
 *	Cancel the arrangement for a procedure to be called when
 *	a given interpreter is deleted.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If proc and clientData were previously registered as a
 *	callback via Tcl_CallWhenDeleted, they are unregistered.
 *	If they weren't previously registered then nothing
 *	happens.
 *
 *--------------------------------------------------------------
 */

void
Tcl_DontCallWhenDeleted(interp, proc, clientData)
    Tcl_Interp *interp;		/* Interpreter to watch. */
    Tcl_InterpDeleteProc *proc;	/* Procedure to call when interpreter
				 * is about to be deleted. */
    ClientData clientData;	/* One-word value to pass to proc. */
{
    DeleteCallback *prevPtr, *dcPtr;
    Interp *iPtr = (Interp *) interp;

    for (prevPtr = NULL, dcPtr = iPtr->deleteCallbackPtr;
	    dcPtr != NULL; prevPtr = dcPtr, dcPtr = dcPtr->nextPtr) {
	if ((dcPtr->proc != proc) || (dcPtr->clientData != clientData)) {
	    continue;
	}
	if (prevPtr == NULL) {
	    iPtr->deleteCallbackPtr = dcPtr->nextPtr;
	} else {
	    prevPtr->nextPtr = dcPtr->nextPtr;
	}
	ckfree((char *) dcPtr);
	break;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteInterp --
 *
 *	Delete an interpreter and free up all of the resources associated
 *	with it.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The interpreter is destroyed.  The caller should never again
 *	use the interp token.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DeleteInterp(interp)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
{
    Interp *iPtr = (Interp *) interp;
    Tcl_HashEntry    *hPtr;
    Tcl_HashSearch   search;
    register Command *cmdPtr;
    DeleteCallback   *dcPtr;
    int i;

    /*
     * If the interpreter is in use, delay the deletion until later.
     */

    iPtr->flags |= DELETED;
    if (iPtr->numLevels != 0) {
	return;
    }

    /*
     * Invoke deletion callbacks.
     */

    while (iPtr->deleteCallbackPtr != NULL) {
	dcPtr = iPtr->deleteCallbackPtr;
	iPtr->deleteCallbackPtr = dcPtr->nextPtr;
	(*dcPtr->proc)(dcPtr->clientData, interp);
	ckfree((char *) dcPtr);
    }

    /*
     * Free up any remaining resources associated with the
     * interpreter.
     */

    for (hPtr = Tcl_FirstHashEntry(&iPtr->commandTable, &search);
	    hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	cmdPtr = (Command *) Tcl_GetHashValue(hPtr);
	if (!cmdPtr->deleted) {
	    cmdPtr->deleted = 1;
	    if (cmdPtr->deleteProc != NULL) {
		(*cmdPtr->deleteProc)(cmdPtr->deleteData);
	    }
            if (cmdPtr->cacheInfo) {
                if (cmdPtr->cacheInfo->usage != 0) {
                    cmdPtr->cacheInfo->element = NULL;
                } else {
                    ckfree((char*)cmdPtr->cacheInfo);
                }
            }
	    ckfree((char *) cmdPtr);
	}
    }
    Tcl_DeleteHashTable(&iPtr->commandTable);
    
    for (hPtr = Tcl_FirstHashEntry(&iPtr->mathFuncTable, &search);
	    hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	ckfree((char *) Tcl_GetHashValue(hPtr));
    }
    Tcl_DeleteHashTable(&iPtr->mathFuncTable);
    
    TclDeleteVars(iPtr, &iPtr->globalTable);
    
    Itcl_DeactivateNamesp((Tcl_Interp*)interp, iPtr->gNsToken);
    Itcl_DeleteNamesp((Itcl_Namespace)iPtr->globalNs);
    
    /*
     * Free up the result *after* deleting variables, since variable
     * deletion could have transferred ownership of the result string
     * to Tcl.
     */

    Tcl_FreeResult(interp);
    if (iPtr->errorInfo != NULL) {
	ckfree(iPtr->errorInfo);
    }
    TVAR_DELETE(&(iPtr->results));
    if (iPtr->errorCode != NULL) {
	ckfree(iPtr->errorCode);
    }
    if (iPtr->events != NULL) {
	int i;

	for (i = 0; i < iPtr->numEvents; i++) {
	    ckfree(iPtr->events[i].command);
	}
	ckfree((char *) iPtr->events);
    }
    while (iPtr->revPtr != NULL) {
	HistoryRev *nextPtr = iPtr->revPtr->nextPtr;

	ckfree(iPtr->revPtr->newBytes);
	ckfree((char *) iPtr->revPtr);
	iPtr->revPtr = nextPtr;
    }
    if (iPtr->appendResult != NULL) {
	ckfree(iPtr->appendResult);
    }
    for (i = 0; i < NUM_REGEXPS; i++) {
	if (iPtr->patterns[i] == NULL) {
	    break;
	}
	ckfree(iPtr->patterns[i]);
	ckfree((char *) iPtr->regexps[i]);
    }
    while (iPtr->tracePtr != NULL) {
	Trace *nextPtr = iPtr->tracePtr->nextPtr;

	ckfree((char *) iPtr->tracePtr);
	iPtr->tracePtr = nextPtr;
    }
    
#ifdef TCL_VW	
 /* free up this task's copy of the TCL context, and
  * de-register the taskVariable
  * Again, we assume that a given task only has a single interpreter
  */
    
    free(Tcl_ctxt);
    if (taskVarDelete(0, (int*) &Tcl_ctxt) != OK)
    {
        logMsg( "%s(%d) - can't taskVarDelete Tcl_ctxt\n", __FILE__,
                __LINE__);
        taskSuspend(0);
    }
#endif

    ckfree((char *) iPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CreateCommand --
 *
 *	Define a new command in a command table.
 *
 * Results:
 
 *	The return value is a token for the command, which can
 *	be used in future calls to Tcl_NameOfCommand.
 *
 * Side effects:
 *	If a command named cmdName already exists for interp, it is
 *	deleted.  In the future, when cmdName is seen as the name of
 *	a command by Tcl_Eval, proc will be called.  When the command
 *	is deleted from the table, deleteProc will be called.  See the
 *	manual entry for details on the calling sequence.
 *
 *----------------------------------------------------------------------
 */

Tcl_Command
Tcl_CreateCommand(interp, cmdName, proc, clientData, deleteProc)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    char *cmdName;		/* Name of command. */
    Tcl_CmdProc *proc;		/* Command procedure to associate with
				 * cmdName. */
    ClientData clientData;	/* Arbitrary one-word value to pass to proc. */
    Tcl_CmdDeleteProc *deleteProc;
				/* If not NULL, gives a procedure to call when
				 * this command is deleted. */
{
    return Tcl_CreateCommandFull(interp, cmdName, proc, clientData,
				 deleteProc, 0);
}

Tcl_Command
Tcl_CreateCommandNs(interp, cmdName, proc, clientData, deleteProc)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    char *cmdName;		/* Name of command. */
    Tcl_CmdProc *proc;		/* Command procedure to associate with
				 * cmdName. */
    ClientData clientData;	/* Arbitrary one-word value to pass to proc. */
    Tcl_CmdDeleteProc *deleteProc;
				/* If not NULL, gives a procedure to call when
				 * this command is deleted. */
{
    return Tcl_CreateCommandFull(interp, cmdName, proc, clientData,
				 deleteProc, 1);
}

Tcl_Command
Tcl_CreateCommandFull(interp, cmdName, proc, clientData, deleteProc,
		      procNameSpace)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    char *cmdName;		/* Name of command. */
    Tcl_CmdProc *proc;		/* Command procedure to associate with
				 * cmdName. */
    ClientData clientData;	/* Arbitrary one-word value to pass to proc. */
    Tcl_CmdDeleteProc *deleteProc;
				/* If not NULL, gives a procedure to call when
				 * this command is deleted. */
    int  procNameSpace;		/* if 1, create a proc style name space
				   descriptor.  For compiled procs */
{
    Interp *iPtr = (Interp *) interp;
    register Command *cmdPtr;
    Tcl_HashEntry *hPtr;
    int new;
    Namespace *cmdNs;
    char *name;
    int specificRef;

    if (iPtr->flags & DELETED) {
	/*
	 * The interpreter is being deleted.  Don't create any new
	 * commands;  it's not safe to muck with the interpreter anymore.
	 */

	return (Tcl_Command) NULL;
    }
    if (ItclFollowNamespPath(interp, iPtr->activeNs, cmdName,
        ITCL_FIND_AUTO_CREATE, &cmdNs,&name,&specificRef) != TCL_OK ||
        cmdNs == NULL || name == NULL) {
        return (Tcl_Command)NULL;
    }
    hPtr = Tcl_CreateHashEntry(&cmdNs->commands, name, &new);

    if (procNameSpace) {
	Proc *procPtr;
	procPtr           = (Proc*)ckalloc(sizeof(Proc));
	procPtr->iPtr     = iPtr;
	procPtr->refCount = 1;
	procPtr->command  = NULL;
	procPtr->argPtr   = NULL;
	procPtr->namesp   = cmdNs;
	clientData = (ClientData) procPtr;
    }

    if (!new) {
	/*
	 * Command already exists:  delete the old one.
	 */
        cmdPtr = (Command*)Tcl_GetHashValue(hPtr);
	Tcl_DeleteCommand2(interp, (Tcl_Command)cmdPtr);
	hPtr = Tcl_CreateHashEntry(&cmdNs->commands, name, &new);
	if (!new) {
	    /*
	     * Drat.  The stupid deletion callback recreated the command.
	     * Just throw away the new command (if we try to delete it again,
	     * we could get stuck in an infinite loop).
	     */
            cmdPtr = (Command*)Tcl_GetHashValue(hPtr);
            if (cmdPtr->cacheInfo) {
                if (cmdPtr->cacheInfo->usage != 0) {
                    cmdPtr->cacheInfo->element = NULL;
                } else {
                    ckfree((char*)cmdPtr->cacheInfo);
                }
            }
	    ckfree((char*)cmdPtr);
	}
    }
    cmdPtr = (Command *) ckalloc(sizeof(Command));
    Tcl_SetHashValue(hPtr, cmdPtr);
    cmdPtr->hPtr = hPtr;
    cmdPtr->proc = proc;
    cmdPtr->clientData = clientData;
    cmdPtr->deleteProc = deleteProc;
    cmdPtr->deleteData = clientData;
    cmdPtr->deleted    = 0;
    cmdPtr->protection = Itcl_CmdProtection(interp,0);
    cmdPtr->namesp     = cmdNs;
    cmdPtr->cacheInfo  = NULL;

    /*
     *  Remove this command from the namespace cache for
     *  the active namespace, or anyone using the active
     *  namespace.  Next time the command is needed, it
     *  will be looked up properly.
     */
    Itcl_NsCacheCmdChanged((Itcl_Namespace)iPtr->activeNs, cmdName);

    return (Tcl_Command) cmdPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetCommandInfo --
 *
 *	Modifies various information about a Tcl command.
 *
 * Results:
 *	If cmdName exists in interp, then the information at *infoPtr
 *	is stored with the command in place of the current information
 *	and 1 is returned.  If the command doesn't exist then 0 is
 *	returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_SetCommandInfo(interp, cmdName, infoPtr)
    Tcl_Interp *interp;			/* Interpreter in which to look
					 * for command. */
    char *cmdName;			/* Name of desired command. */
    Tcl_CmdInfo *infoPtr;		/* Where to store information about
					 * command. */
{
    Command *cmdPtr;
    Tcl_Command cmd;
    
    if (Itcl_FindCommand(interp, cmdName, 0, &cmd) != TCL_OK || cmd == NULL) {
	return 0;
    }
    cmdPtr = (Command*)cmd;
    
    cmdPtr->proc       = infoPtr->proc;
    cmdPtr->clientData = infoPtr->clientData;
    cmdPtr->deleteProc = infoPtr->deleteProc;
    cmdPtr->deleteData = infoPtr->deleteData;
    return 1;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetCommandInfo --
 *
 *	Returns various information about a Tcl command.
 *
 * Results:
 *	If cmdName exists in interp, then *infoPtr is modified to
 *	hold information about cmdName and 1 is returned.  If the
 *	command doesn't exist then 0 is returned and *infoPtr isn't
 *	modified.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetCommandInfo(interp, cmdName, infoPtr)
    Tcl_Interp *interp;			/* Interpreter in which to look
					 * for command. */
    char *cmdName;			/* Name of desired command. */
    Tcl_CmdInfo *infoPtr;		/* Where to store information about
					 * command. */
{
    Command *cmdPtr;
    Tcl_Command cmd;
  
    if (Itcl_FindCommand(interp, cmdName, 0, &cmd) != TCL_OK || cmd == NULL) {
  	return 0;
    }
    cmdPtr = (Command*)cmd;
    
    infoPtr->proc       = cmdPtr->proc;
    infoPtr->clientData = cmdPtr->clientData;
    infoPtr->deleteProc = cmdPtr->deleteProc;
    infoPtr->deleteData = cmdPtr->deleteData;
    return 1;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetCommandName --
 *
 *	Given a token returned by Tcl_CreateCommand, this procedure
 *	returns the current name of the command (which may have changed
 *	due to renaming).
 *
 * Results:
 *	The return value is the name of the given command.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_GetCommandName(interp, command)
    Tcl_Interp *interp;		/* Interpreter containing the command. */
    Tcl_Command command;	/* Token for the command, returned by a
			 * previous call to Tcl_CreateCommand.
			 * The command must not have been deleted. */
{
    Command *cmdPtr = (Command *) command;
    Interp *iPtr = (Interp *) interp;
    
    if (cmdPtr == NULL) {
	return NULL;
    }
    
    return Tcl_GetHashKey(&iPtr->commandTable, cmdPtr->hPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetCommandPath --
 *
 *	Given a token returned by Tcl_CreateCommand, this procedure
 *	returns the full path name of the command (including "::"
 *	scope qualifiers).
 *
 * Results:
 *	The return value is the name of the given command.
 *	The string persists until the next call to this routine.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_GetCommandPath(interp, command)
    Tcl_Interp *interp;		/* Interpreter containing the command. */
    Tcl_Command command;	/* Token for the command, returned by a
				 * previous call to Tcl_CreateCommand.
				 * The command must not have been deleted. */
{
    Command *cmdPtr = (Command *) command;
    Interp *iPtr = (Interp *) interp;
    char *name;

    static Tcl_DString* buffer = NULL;
    if (!buffer) {
	buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
	Tcl_DStringInit(buffer);
    }
    Tcl_DStringTrunc(buffer,0);
    if (cmdPtr == NULL) {
	return NULL;
    }
    if (cmdPtr->namesp != iPtr->globalNs) {
        name = Itcl_GetNamespPath((Itcl_Namespace)cmdPtr->namesp);
        Tcl_DStringAppend(buffer, name, -1);
    }
    Tcl_DStringAppend(buffer, "::", -1);
    name = Tcl_GetHashKey(&cmdPtr->namesp->commands, cmdPtr->hPtr);
    Tcl_DStringAppend(buffer, name, -1);

    return Tcl_DStringValue(buffer);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetVariableName --
 *
 *	Given a token returned by TclNewVar, this procedure
 *	returns the current name of the variable (including its
 *	namespace path).
 *
 * Results:
 *	The return value is the name of the given variable.
 *	The string persists until the next call to this routine.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_GetVariableName(interp, var)
    Tcl_Interp *interp;		/* Interpreter containing the command. */
    Tcl_Var var;		/* Token for the variable, returned by a
				 * previous call to TclNewVar */
{
    Var *varPtr = (Var*)var;
    Interp *iPtr = (Interp*)interp;
    char *name;

    static Tcl_DString* buffer = NULL;
    if (!buffer) {
	buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
	Tcl_DStringInit(buffer);
    }
    Tcl_DStringTrunc(buffer,0);

    if (varPtr == NULL) {
	return NULL;
    }
    if (varPtr->namesp != iPtr->globalNs) {
        name = Itcl_GetNamespPath((Itcl_Namespace)varPtr->namesp);
        Tcl_DStringAppend(buffer, name, -1);
    }
    Tcl_DStringAppend(buffer, "::", -1);
    name = Tcl_GetHashKey(&varPtr->namesp->variables, varPtr->hPtr);
    Tcl_DStringAppend(buffer, name, -1);

    return Tcl_DStringValue(buffer);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteCommand --
 *
 *	Remove the given command from the given interpreter.
 *
 * Results:
 *	0 is returned if the command was deleted successfully.
 *	-1 is returned if there didn't exist a command by that
 *	name.
 *
 * Side effects:
 *	CmdName will no longer be recognized as a valid command for
 *	interp.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_DeleteCommand(interp, cmdName)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    char *cmdName;		/* Name of command to remove. */
{
    Interp *iPtr = (Interp *) interp;
    char *name;
    Command *cmdPtr;
    Tcl_Command cmd;

    if (iPtr->flags & DELETED) {
	/*
	 * The interpreter is being deleted, so this command has already
	 * been deleted, or will be soon.  It's not safe to muck with the
	 * interpreter anymore.
	 */

	return -1;
    }
    if (Itcl_FindCommand(interp, cmdName, 0, &cmd) != TCL_OK || cmd == NULL) {
        return -1;
    }
    cmdPtr = (Command*)cmd;

    /*
     * The code here is tricky.  We can't delete the hash table entry
     * before invoking the deletion callback because there are cases
     * where the deletion callback needs to invoke the command (e.g.
     * object systems such as OTcl).  However, this means that the
     * callback could try to delete or rename the command.  The deleted
     * flag allows us to detect these cases and skip nested deletes.
     */

    if (cmdPtr->deleted) {
	/*
	 * Another deletion is already in progress.  Remove the hash
	 * table entry now, but don't invoke a callback or free the
	 * command structure.
         *
         *  Remove this command from the namespace cache for
         *  the active namespace, or anyone using the active
         *  namespace.  Next time the command is needed, it
         *  will be looked up properly.
         */
        name =  Tcl_GetHashKey(&cmdPtr->namesp->commands, cmdPtr->hPtr);
        Itcl_NsCacheCmdChanged((Itcl_Namespace)cmdPtr->namesp, name);
	Tcl_DeleteHashEntry(cmdPtr->hPtr);
	cmdPtr->hPtr = NULL;
	return 0;
    }
    cmdPtr->deleted = 1;
    if (cmdPtr->deleteProc != NULL) {
        (*cmdPtr->deleteProc)(cmdPtr->deleteData);
    }

    /*
     * Don't use hPtr to delete the hash entry here, because it's
     * possible that the deletion callback renamed the command.
     * Instead, use cmdPtr->hptr, and make sure that no-one else
     * has already deleted the hash entry.
     */

    if (cmdPtr->hPtr) {
        name =  Tcl_GetHashKey(&cmdPtr->namesp->commands, cmdPtr->hPtr);
        Itcl_NsCacheCmdChanged((Itcl_Namespace)cmdPtr->namesp, name);
        Tcl_DeleteHashEntry(cmdPtr->hPtr);
    }

    if (cmdPtr->cacheInfo) {
        if (cmdPtr->cacheInfo->usage != 0) {
            cmdPtr->cacheInfo->element = NULL;
        } else {
            ckfree((char*)cmdPtr->cacheInfo);
        }
    }
    ckfree((char*)cmdPtr);

    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteCommand2 --
 *
 *	Remove the given command from the given interpreter.
 *
 * Results:
 *	0 is returned if the command was deleted successfully.
 *	-1 is returned if anything goes wrong.
 *
 * Side effects:
 *	CmdName will no longer be recognized as a valid command for
 *	interp.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_DeleteCommand2(interp, cmd)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    Tcl_Command cmd;		/* Token for command to remove. */
{
    Interp *iPtr = (Interp*)interp;
    Command *cmdPtr = (Command *) cmd;

    char *name;

    if (iPtr->flags & DELETED) {
	/*
	 * The interpreter is being deleted, so this command has already
	 * been deleted, or will be soon.  It's not safe to muck with the
	 * interpreter anymore.
	 */

	return -1;
    }
    if (cmdPtr == NULL) {
	return -1;
    }

    /*
     * The code here is tricky.  We can't delete the hash table entry
     * before invoking the deletion callback because there are cases
     * where the deletion callback needs to invoke the command (e.g.
     * object systems such as OTcl).  However, this means that the
     * callback could try to delete or rename the command.  The deleted
     * flag allows us to detect these cases and skip nested deletes.
     */
    if (cmdPtr->deleted) {
	/*
	 * Another deletion is already in progress.  Remove the hash
	 * table entry now, but don't invoke a callback or free the
	 * command structure.
         *
         *  Remove this command from the namespace cache for
         *  the active namespace, or anyone using the active
         *  namespace.  Next time the command is needed, it
         *  will be looked up properly.
         */
        name =  Tcl_GetHashKey(&cmdPtr->namesp->commands, cmdPtr->hPtr);
        Itcl_NsCacheCmdChanged((Itcl_Namespace)cmdPtr->namesp, name);
	Tcl_DeleteHashEntry(cmdPtr->hPtr);
	cmdPtr->hPtr = NULL;
	return 0;
    }
    cmdPtr->deleted = 1;
    if (cmdPtr->deleteProc != NULL) {
	(*cmdPtr->deleteProc)(cmdPtr->deleteData);
    }

    /*
     * Don't use hPtr to delete the hash entry here, because it's
     * possible that the deletion callback renamed the command.
     * Instead, use cmdPtr->hptr, and make sure that no-one else
     * has already deleted the hash entry.
     */

    if (cmdPtr->hPtr) {
        name =  Tcl_GetHashKey(&cmdPtr->namesp->commands, cmdPtr->hPtr);
        Itcl_NsCacheCmdChanged((Itcl_Namespace)cmdPtr->namesp, name);
        Tcl_DeleteHashEntry(cmdPtr->hPtr);
    }

    if (cmdPtr->cacheInfo) {
        if (cmdPtr->cacheInfo->usage != 0) {
            cmdPtr->cacheInfo->element = NULL;
        } else {
            ckfree((char*)cmdPtr->cacheInfo);
        }
    }
    ckfree((char*)cmdPtr);
    
    return 0;
}

/*
 *-----------------------------------------------------------------
 *
 * Tcl_Eval --
 *
 *	Parse and execute a command in the Tcl language.
 *
 * Results:
 *	The return value is one of the return codes defined in tcl.hd
 *	(such as TCL_OK), and interp->result contains a string value
 *	to supplement the return code.  The value of interp->result
 *	will persist only until the next call to Tcl_Eval:  copy it or
 *	lose it! *TermPtr is filled in with the character just after
 *	the last one that was part of the command (usually a NULL
 *	character or a closing bracket).
 *
 * Side effects:
 *	Almost certainly;  depends on the command.
 *
 *-----------------------------------------------------------------
 */

int
Tcl_Eval(interp, cmd)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    char *cmd;			/* Pointer to TCL command to interpret. */
{
    return Tcl_Eval_Args(interp, cmd, 0, NULL, NULL, NULL);
}
int
Tcl_Eval_Args(interp, cmd, pargc, pargv, clientData, procEntry)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    char *cmd;			/* Pointer to TCL command to interpret. */
    int  pargc;                 /* Number of arguments in argv */
    char **pargv;               /* Array of arguments */
    ClientData clientData;	/* client data for proc */
    Tcl_CmdProc *procEntry;	/* optional entry for tcl command */
{
    /*
     * The storage immediately below is used to generate a copy
     * of the command, after all argument substitutions.  Pv will
     * contain the argv values passed to the command procedure.
     */

#   define NUM_CHARS 2000
    char copyStorage[NUM_CHARS];
    ParseValue pv;
    char *oldBuffer;

    /*
     * This procedure generates an (argv, argc) array for the command,
     * It starts out with stack-allocated space but uses dynamically-
     * allocated storage to increase it if needed.
     */

#   define NUM_ARGS 10
    char *(argStorage[NUM_ARGS]);
    char **argv = argStorage;
    int argc;
    int argSize = NUM_ARGS;

    register char *src;			/* Points to current character
					 * in cmd. */
    unsigned char aliasSrc = 0xff;	/* Make src valid */	
    char termChar;			/* Return when this character is found
					 * (either ']' or '\0').  Zero means
					 * that newlines terminate commands. */
    int flags;				/* Interp->evalFlags value when the
					 * procedure was called. */
    int result;				/* Return value. */
    register Interp *iPtr = (Interp *) interp;
    Tcl_HashEntry *hPtr;
    Command *cmdPtr;
    char *termPtr=NULL;			/* Contains character just after the
					 * last one in the command. */
    char *cmdStart;			/* Points to first non-blank char. in
					 * command (used in calling trace
					 * procedures). */
    char *ellipsis = "";		/* Used in setting errorInfo variable;
					 * set to "..." to indicate that not
					 * all of offending command is included
					 * in errorInfo.  "" means that the
					 * command is all there. */
    unsigned char parsed;		/* 1 line wonder */
    unsigned char finished;		/* if parsed, we are finished */
    int j;				/* loop variable */
    register Trace *tracePtr;
    int oldCount = iPtr->cmdCount;	/* Used to tell whether any commands
					 * at all were executed. */


    /*
     * Initialize the result to an empty string and clear out any
     * error information.  This makes sure that we return an empty
     * result if there are no commands in the command string.
     */

    Tcl_FreeResult((Tcl_Interp *) iPtr);
    iPtr->result = iPtr->resultSpace;
    iPtr->resultSpace[0] = 0;
    result = TCL_OK;

    src = cmd;
    cmdStart = src;
    finished = 0;
    if (pargv) {
	parsed = 1;
	src = cmdStart + strlen(cmdStart);
    }
    else {
	parsed = 0;
	termPtr = src;
	
	/*
	 * Initialize the area in which command copies will be assembled.
	 */

	pv.buffer = copyStorage;
	pv.end = copyStorage + NUM_CHARS - 1;
	pv.expandProc = TclExpandParseValue;
	pv.clientData = (ClientData) NULL;
    }

    /*
     * Initialize the area in which command copies will be assembled.
     */

    flags = iPtr->evalFlags;
    iPtr->evalFlags = 0;
    if (flags & TCL_BRACKET_TERM) {
	termChar = ']';
    } else {
	termChar = 0;
    }

    /*
     * Check depth of nested calls to Tcl_Eval:  if this gets too large,
     * it's probably because of an infinite loop somewhere.
     */

    iPtr->numLevels++;
    if (iPtr->numLevels > iPtr->maxNestingDepth) {
	iPtr->numLevels--;
	iPtr->result =  "too many nested calls to Tcl_Eval (infinite loop?)";
	iPtr->termPtr = termPtr;
	return TCL_ERROR;
    }

    /*
     * There can be many sub-commands (separated by semi-colons or
     * newlines) in one command string.  This outer loop iterates over
     * individual commands.
     */

    while ((parsed || *src != termChar) && !finished) {
	iPtr->flags &= ~(ERR_IN_PROGRESS | ERROR_CODE_SET);

	/*
	 * Skim off leading white space and semi-colons, and skip
	 * comments.
	 */

	if (!parsed) {
	    while (1) {
		register char c = *src;

		if ((CHAR_TYPE(c) != TCL_SPACE) && (c != ';') && (c != '\n')) {
		    break;
		}
		src += 1;
	    }
	    if (*src == '#') {
		for (src++; *src != 0; src++) {
		    if ((*src == '\n') && (src[-1] != '\\')) {
			src++;
			termPtr = src;
			break;
		    }
		}
		continue;
	    }
	}

	/*
	 * Parse the words of the command, generating the argc and
	 * argv for the command procedure.  May have to call
	 * TclParseWords several times, expanding the argv array
	 * between calls.
	 */

	if (parsed) {
	    argv = pargv;
	    argc = pargc;
	    finished = 1;
	}
	else {
	    pv.next = oldBuffer = pv.buffer;
	    argc = 0;
	    cmdStart = src;
	    while (1) {
		int newArgs, maxArgs;
		char **newArgv;
		int i;

		/*
		 * Note:  the "- 2" below guarantees that we won't use the
		 * last two argv slots here.  One is for a NULL pointer to
		 * mark the end of the list, and the other is to leave room
		 * for inserting the command name "unknown" as the first
		 * argument (see below).
		 */

		maxArgs = argSize - argc - 2;
		result = TclParseWords((Tcl_Interp *) iPtr, src, flags,
				       maxArgs, &termPtr, &newArgs,
				       &argv[argc], &pv);
		src = termPtr;
		if (result != TCL_OK) {
		    ellipsis = "...";
		    goto done;
		}

		/*
		 * Careful!  Buffer space may have gotten reallocated while
		 * parsing words.  If this happened, be sure to update all
		 * of the older argv pointers to refer to the new space.
		 */

		if (oldBuffer != pv.buffer) {
		    int i;

		    for (i = 0; i < argc; i++) {
			argv[i] = pv.buffer + (argv[i] - oldBuffer);
		    }
		    oldBuffer = pv.buffer;
		}
		argc += newArgs;
		if (newArgs < maxArgs) {
		    argv[argc] = (char *) NULL;
		    break;
		}

		/*
		 * Args didn't all fit in the current array.  Make it bigger.
		 */

		argSize *= 2;
		newArgv = (char **)
		    ckalloc((unsigned) argSize * sizeof(char *));
		for (i = 0; i < argc; i++) {
		    newArgv[i] = argv[i];
		}
		if (argv != argStorage) {
		    ckfree((char *) argv);
		}
		argv = newArgv;
	    }
	}

	/*
	 * If this is an empty command (or if we're just parsing
	 * commands without evaluating them), then just skip to the
	 * next command.
	 */

	if ((argc == 0) || iPtr->noEval) {
	    continue;
	}
	argv[argc] = NULL;

	/*
	 * Save information for the history module, if needed.
	 */

	if (flags & TCL_RECORD_BOUNDS) {
	    iPtr->evalFirst = cmdStart;
	    iPtr->evalLast = src-1;
	}

	/*
	 * Find the procedure to execute this command.  If there isn't
	 * one, then see if there is a command "unknown".  If so,
	 * invoke it instead, passing it the words of the original
	 * command as arguments.
	 */
	
	result = Itcl_EvalArgsFull(interp, cmdStart, src, argc, argv,
				   clientData, procEntry);
	
	/* Externally passed procEntry only refers to 1st time thru loop ... */
	
	procEntry = NULL;

	if (result != TCL_OK) {
	    break;
	}
    }

    done:

    /*
     * If no commands at all were executed, check for asynchronous
     * handlers so that they at least get one change to execute.
     * This is needed to handle event loops written in Tcl with
     * empty bodies (I'm not sure that loops like this are a good
     * idea, * but...).
     */

    if ((oldCount == iPtr->cmdCount) && (tcl_AsyncReady)) {
	result = Tcl_AsyncInvoke(interp, result);
    }

    /*
     * Free up any extra resources that were allocated.
     */

    if (!parsed) {
        if (pv.buffer != copyStorage) {
	    ckfree((char *) pv.buffer);
        }
        if (argv != argStorage) {
	    ckfree((char *) argv);
        }
    }
    iPtr->numLevels--;
    if (iPtr->numLevels == 0) {
	if (result == TCL_RETURN) {
	    result = TclUpdateReturnInfo(iPtr);
	}
	if ((result != TCL_OK) && (result != TCL_ERROR)
		&& !(flags & TCL_ALLOW_EXCEPTIONS)) {
	    Tcl_ResetResult(interp);
	    if (result == TCL_BREAK) {
		iPtr->result = "invoked \"break\" outside of a loop";
	    } else if (result == TCL_CONTINUE) {
		iPtr->result = "invoked \"continue\" outside of a loop";
	    } else {
		iPtr->result = iPtr->resultSpace;
		sprintf(iPtr->resultSpace, "command returned bad code: %d cmd >%s<",
			result,cmd);
	    }
	    result = TCL_ERROR;
	}
	if (iPtr->flags & DELETED) {
	    /*
	     * Someone tried to delete the interpreter, but it couldn't
	     * actually be deleted because commands were in the middle of
	     * being evaluated.  Delete the interpreter now.  Also, return
	     * immediately:  we can't execute the remaining code in the
	     * procedure because it accesses fields of the dead interpreter.
	     */

	    Tcl_DeleteInterp(interp);
	    return result;
	}
    }

    /*
     * If an error occurred, record information about what was being
     * executed when the error occurred.
     */

    if ((result == TCL_ERROR) && !(iPtr->flags & ERR_ALREADY_LOGGED)) {
	int numChars;
	register char *p;

	/*
	 * Compute the line number where the error occurred.
	 */

	iPtr->errorLine = 1;
	for (p = cmd; p != cmdStart; p++) {
	    if (*p == '\n') {
		iPtr->errorLine++;
	    }
	}
	for ( ; isspace(UCHAR(*p)) || (*p == ';'); p++) {
	    if (*p == '\n') {
		iPtr->errorLine++;
	    }
	}
	numChars = src - cmdStart;

	/*
	 * Figure out how much of the command to print in the error
	 * message (up to a certain number of characters, or up to
	 * the first new-line).
	 */

	if (numChars > (NUM_CHARS-50)) {
	    numChars = NUM_CHARS-50;
	    ellipsis = " ...";
	}

	if (!(iPtr->flags & ERR_IN_PROGRESS)) {
	    sprintf(copyStorage, "\n    while executing\n\"%.*s%s\"",
		    numChars, cmdStart, ellipsis);
	} else {
	    sprintf(copyStorage, "\n    invoked from within\n\"%.*s%s\"",
		    numChars, cmdStart, ellipsis);
	}
	Tcl_AddErrorInfo(interp, copyStorage);
	iPtr->flags &= ~ERR_ALREADY_LOGGED;
    } else {
	iPtr->flags &= ~ERR_ALREADY_LOGGED;
    }
    iPtr->termPtr = termPtr;
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CreateTrace --
 *
 *	Arrange for a procedure to be called to trace command execution.
 *
 * Results:
 *	The return value is a token for the trace, which may be passed
 *	to Tcl_DeleteTrace to eliminate the trace.
 *
 * Side effects:
 *	From now on, proc will be called just before a command procedure
 *	is called to execute a Tcl command.  Calls to proc will have the
 *	following form:
 *
 *	void
 *	proc(clientData, interp, level, command, cmdProc, cmdClientData,
 *		argc, argv)
 *	    ClientData clientData;
 *	    Tcl_Interp *interp;
 *	    int level;
 *	    char *command;
 *	    int (*cmdProc)();
 *	    ClientData cmdClientData;
 *	    int argc;
 *	    char **argv;
 *	{
 *	}
 *
 *	The clientData and interp arguments to proc will be the same
 *	as the corresponding arguments to this procedure.  Level gives
 *	the nesting level of command interpretation for this interpreter
 *	(0 corresponds to top level).  Command gives the ASCII text of
 *	the raw command, cmdProc and cmdClientData give the procedure that
 *	will be called to process the command and the ClientData value it
 *	will receive, and argc and argv give the arguments to the
 *	command, after any argument parsing and substitution.  Proc
 *	does not return a value.
 *
 *----------------------------------------------------------------------
 */

Tcl_Trace
Tcl_CreateTrace(interp, level, proc, clientData)
    Tcl_Interp *interp;		/* Interpreter in which to create the trace. */
    int level;			/* Only call proc for commands at nesting level
				 * <= level (1 => top level). */
    Tcl_CmdTraceProc *proc;	/* Procedure to call before executing each
				 * command. */
    ClientData clientData;	/* Arbitrary one-word value to pass to proc. */
{
    register Trace *tracePtr;
    register Interp *iPtr = (Interp *) interp;

    tracePtr = (Trace *) ckalloc(sizeof(Trace));
    tracePtr->level = level;
    tracePtr->proc = proc;
    tracePtr->clientData = clientData;
    tracePtr->nextPtr = iPtr->tracePtr;
    iPtr->tracePtr = tracePtr;

    return (Tcl_Trace) tracePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteTrace --
 *
 *	Remove a trace.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	From now on there will be no more calls to the procedure given
 *	in trace.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DeleteTrace(interp, trace)
    Tcl_Interp *interp;		/* Interpreter that contains trace. */
    Tcl_Trace trace;		/* Token for trace (returned previously by
				 * Tcl_CreateTrace). */
{
    register Interp *iPtr = (Interp *) interp;
    register Trace *tracePtr = (Trace *) trace;
    register Trace *tracePtr2;

    if (iPtr->tracePtr == tracePtr) {
	iPtr->tracePtr = tracePtr->nextPtr;
	ckfree((char *) tracePtr);
    } else {
	for (tracePtr2 = iPtr->tracePtr; tracePtr2 != NULL;
		tracePtr2 = tracePtr2->nextPtr) {
	    if (tracePtr2->nextPtr == tracePtr) {
		tracePtr2->nextPtr = tracePtr->nextPtr;
		ckfree((char *) tracePtr);
		return;
	    }
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AddErrorInfo --
 *
 *	Add information to a message being accumulated that describes
 *	the current error.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The contents of message are added to the "errorInfo" variable.
 *	If Tcl_Eval has been called since the current value of errorInfo
 *	was set, errorInfo is cleared before adding the new message.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_AddErrorInfo(interp, message)
    Tcl_Interp *interp;		/* Interpreter to which error information
				 * pertains. */
    char *message;		/* Message to record. */
{
    register Interp *iPtr = (Interp *) interp;

    /*
     * If an error is already being logged, then the new errorInfo
     * is the concatenation of the old info and the new message.
     * If this is the first piece of info for the error, then the
     * new errorInfo is the concatenation of the message in
     * interp->result and the new message.
     */

    if (!(iPtr->flags & ERR_IN_PROGRESS)) {
	Tcl_SetVar2(interp, "errorInfo", (char *) NULL, interp->result,
		TCL_GLOBAL_ONLY);
	iPtr->flags |= ERR_IN_PROGRESS;

	/*
	 * If the errorCode variable wasn't set by the code that generated
	 * the error, set it to "NONE".
	 */

	if (!(iPtr->flags & ERROR_CODE_SET)) {
	    (void) Tcl_SetVar2(interp, "errorCode", (char *) NULL, "NONE",
		    TCL_GLOBAL_ONLY);
	}
    }
    Tcl_SetVar2(interp, "errorInfo", (char *) NULL, message,
	    TCL_GLOBAL_ONLY|TCL_APPEND_VALUE);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_VarEval --
 *
 *	Given a variable number of string arguments, concatenate them
 *	all together and execute the result as a Tcl command.
 *
 * Results:
 *	A standard Tcl return result.  An error message or other
 *	result may be left in interp->result.
 *
 * Side effects:
 *	Depends on what was done by the command.
 *
 *----------------------------------------------------------------------
 */
	/* VARARGS2 */ /* ARGSUSED */
int
#ifdef USEVARARGS
Tcl_VarEval(va_alist)
    va_dcl
#else
Tcl_VarEval(Tcl_Interp *first, ...)
#endif
{
    va_list argList;
    Tcl_DString buf;
    char *string;
    Tcl_Interp *interp;
    int result;

    /*
     * Copy the strings one after the other into a single larger
     * string.  Use stack-allocated space for small commands, but if
     * the command gets too large than call ckalloc to create the
     * space.
     */

#ifdef USEVARARGS
    va_start(argList);
    interp = va_arg(argList, Tcl_Interp *);
#else
    va_start(argList, first);
    interp = first;
#endif
    Tcl_DStringInit(&buf);
    while (1) {
	string = va_arg(argList, char *);
	if(string == NULL)
    {
	  break;
	}
    else
    {
      ;
    }

	Tcl_DStringAppend(&buf, string, -1);
    }
    va_end(argList);

    result = Tcl_Eval(interp, Tcl_DStringValue(&buf));
    Tcl_DStringFree(&buf);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GlobalEval --
 *
 *	Evaluate a command at global level in an interpreter.
 *
 * Results:
 *	A standard Tcl result is returned, and interp->result is
 *	modified accordingly.
 *
 * Side effects:
 *	The command string is executed in interp, and the execution
 *	is carried out in the variable context of global level (no
 *	procedures active), just as if an "uplevel #0" command were
 *	being executed.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GlobalEval(interp, command)
    Tcl_Interp *interp;		/* Interpreter in which to evaluate command. */
    char *command;		/* Command to evaluate. */
{
    register Interp *iPtr = (Interp *) interp;
    int result;
    CallFrame *savedVarFramePtr;
    Itcl_ActiveNamespace nsToken;
    
    savedVarFramePtr = iPtr->varFramePtr;
    iPtr->varFramePtr = NULL;
    nsToken = Itcl_ActivateNamesp(interp, (Itcl_Namespace)iPtr->globalNs);
    if (nsToken == NULL) {
        result = TCL_ERROR;
    } else {
        result = Tcl_Eval(interp, command);
        Itcl_DeactivateNamesp(interp, nsToken);
    }
    
    iPtr->varFramePtr = savedVarFramePtr;
    return result;
}

#ifdef TCL_VW
/*
 *----------------------------------------------------------------------
 *
 * Tcl_InitSems --
 *
 *	Initializes some global vxWorks semaphores
 *
 * Results:
 *	One-time creation of semaphores
 *
 * Side effects:
 *	The global semaphore ids are initialized
 *
 *----------------------------------------------------------------------
 */
void
Tcl_InitSems()
{
    int		options = SEM_Q_PRIORITY | SEM_INVERSION_SAFE |
	SEM_DELETE_SAFE;

    if ( (qsortSem = semMCreate(options)) == NULL ||
	 (regexpSem = semMCreate(options)) == NULL ||
	 (miscSem = semMCreate(options)) == NULL ||
	 (ckAllocSem = semMCreate(options)) == NULL)
    {
	panic("error creating vxWorks semaphores\n");
    }
}
#endif /* TCL_VW */
/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetRecursionLimit --
 *
 *	Set the maximum number of recursive calls that may be active
 *	for an interpreter at once.
 *
 * Results:
 *	The return value is the old limit on nesting for interp.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_SetRecursionLimit(interp, depth)
    Tcl_Interp *interp;			/* Interpreter whose nesting limit
					 * is to be set. */
    int depth;				/* New value for maximimum depth. */
{
    Interp *iPtr = (Interp *) interp;
    int old;

    old = iPtr->maxNestingDepth;
    if (depth > 0) {
	iPtr->maxNestingDepth = depth;
    }
    return old;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AllowExceptions --
 *
 *	Sets a flag in an interpreter so that exceptions can occur
 *	in the next call to Tcl_Eval without them being turned into
 *	errors.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The TCL_ALLOW_EXCEPTIONS flag gets set in the interpreter's
 *	evalFlags structure.  See the reference documentation for
 *	more details.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_AllowExceptions(interp)
    Tcl_Interp *interp;		/* Interpreter in which to set flag. */
{
    Interp *iPtr = (Interp *) interp;

    iPtr->evalFlags |= TCL_ALLOW_EXCEPTIONS;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ResetValueInterp --
 *
 *	Resets the interpreter result structure
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Interpreter result field invalidated
 *
 *----------------------------------------------------------------------
 */

void
Tcl_ResetValueInterp(interp)
    Tcl_Interp *interp;		/* Interpreter in which to set flag. */
{
    Interp *iPtr = (Interp *) interp;
    Tcl_ResetValueMACRO(&(iPtr->results.allValue),
			&(iPtr->results.typeAttr));
}
