/*
 * ------------------------------------------------------------------------
 *      PACKAGE:  [incr Tcl]
 *  DESCRIPTION:  Object-Oriented Extensions to Tcl
 *
 *  [incr Tcl] provides object-oriented extensions to Tcl, much as
 *  C++ provides object-oriented extensions to C.  It provides a means
 *  of encapsulating related procedures together with their shared data
 *  in a local namespace that is hidden from the outside world.  It
 *  promotes code re-use through inheritance.  More than anything else,
 *  it encourages better organization of Tcl applications through the
 *  object-oriented paradigm, leading to code that is easier to
 *  understand and maintain.
 *  
 *  This part provides Tcl commands to access/modify namespaces.
 *  Commands include the following:
 *
 *    info ......... modified to work with namespaces
 *    namespace .... used to create/modify namespaces
 *    variable ..... used to declare variables
 *    import ....... used to change the import list for a namespace
 *    public ....... used to create public commands/variables
 *    protected .... used to create protected commands/variables
 *    private ...... used to create private commands/variables
 *    delete ....... ensemble used to delete namespaces/classes/objects
 *    @scope ....... used to evaluate scoped command scripts
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl_cmds.c,v 1.1.1.1 1996/08/21 19:30:11 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 *
 * Some of the procedures in this file are based on others in the
 * standard Tcl distribution, having the following copyright notice:
 *
 *  Copyright (c) 1987-1994 The Regents of the University of California.
 *  Copyright (c) 1994-1995 Sun Microsystems, Inc.
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include <assert.h>
#include "tclInt.h"

/*
 * ------------------------------------------------------------------------
 *  ItclInitNamespCmds()
 *
 *  Convenience procedure for initializing an interpreter.  Adds
 *  commands associated with namespaces to a Tcl interpreter.
 *
 *  Invoked within Tcl_CreateInterp() just after the usual Tcl
 *  built-in commands are installed.  Changes the usual "info"
 *  command so that it is implemented as an ensemble, and installs
 *  various other commands for manipulating namespaces.
 *
 *  Returns TCL_OK on success, or TCL_ERROR (along with an error message
 *  in the interp->result) if something goes wrong.
 * ------------------------------------------------------------------------
 */
int
ItclInitNamespCmds(interp)
    Tcl_Interp *interp;            /* interpreter to be updated */
{
    int status;
    Itcl_Namespace ns;

    /*
     *  Install simple commands for manipulating namespaces.
     */
    Tcl_CreateCommand(interp, "::namespace", Itcl_NamespaceCmd,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::variable", Itcl_VariableCmd,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::public", Itcl_ProtectionCmd,
        (ClientData)ITCL_PUBLIC, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::protected", Itcl_ProtectionCmd,
        (ClientData)ITCL_PROTECTED, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::private", Itcl_ProtectionCmd,
        (ClientData)ITCL_PRIVATE, (Tcl_CmdDeleteProc*)NULL);

    /*
     *  Add the "import" ensemble.
     */
    if (Itcl_CreateEnsemble(interp, "::import") != TCL_OK) {
        return TCL_ERROR;
    }
    if (Itcl_AddEnsembleOption(interp, "::import", "list",
            0, 1, "?importList?",
            Itcl_ImportListCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::import", "all",
            0, 1, "?name?",
            Itcl_ImportAllCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::import", "add",
            1, ITCL_VAR_ARGS, "name ?name...? ?-where pos...?",
            Itcl_ImportAddCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::import", "remove",
            1, ITCL_VAR_ARGS, "name ?name...?",
            Itcl_ImportRemoveCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK
    ) {
        return TCL_ERROR;
    }

    /*
     *  Add the "delete" ensemble.
     */
    if (Itcl_CreateEnsemble(interp, "::delete") != TCL_OK) {
        return TCL_ERROR;
    }
    if (Itcl_AddEnsembleOption(interp, "::delete", "namespace",
            1, ITCL_VAR_ARGS, "name ?name...?",
            Itcl_DelNamespCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK
    ) {
        return TCL_ERROR;
    }

    /*
     *  Create a ::tcl namespace to wrap up odds and ends.
     */
    status = Itcl_CreateNamesp(interp, "::tcl",
        (ClientData)NULL, (Itcl_DeleteProc*)NULL, &ns);

    if (status != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  Add stuff for creating/decoding @scope declarations.
     */
    Tcl_CreateCommand(interp, "::scope", Itcl_ScopeCmd,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::code", Itcl_CodeCmd,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::@scope", Itcl_AtScopeCmd,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    status = Itcl_CreateNamesp(interp, "::tcl::scope-parser",
        (ClientData)NULL, (Itcl_DeleteProc*)NULL, &ns);

    if (status != TCL_OK) {
        return TCL_ERROR;
    }
    Itcl_ClearImportNamesp(ns);  /* allow only commands in this namespace */

    Tcl_CreateCommand(interp, "::tcl::scope-parser::@scope",
        Itcl_DecodeScopeCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    /*
     *  Create an "info" ensemble in the global namespace, and
     *  install subcommands which support namespaces.
     *  Use a generic handler to wrap the usual "info" options.
     */
    if (Itcl_CreateEnsemble(interp, "::info") != TCL_OK) {
        return TCL_ERROR;
    }
    if (Itcl_AddEnsembleOption(interp, "::info", "commands",
            0, 1, "?pattern?",
            Itcl_InfoCommandsCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "procs",
            0, 1, "?pattern?",
            Itcl_InfoProcsCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "globals",
            0, 1, "?pattern?",
            Itcl_InfoGlobalsCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "vars",
            0, 1, "?pattern?",
            Itcl_InfoVarsCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "exists",
            1, 1, "varName",
            Itcl_InfoExistsCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "context",
            0, 0, (char*)NULL,
            Itcl_InfoContextCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "protection",
            1, ITCL_VAR_ARGS, "?-command? ?-variable? name",
            Itcl_InfoProtectionCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "which",
            1, ITCL_VAR_ARGS, "?-command? ?-variable? ?-namespace? name",
            Itcl_InfoWhichCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::info", "args",
            1, 1, "procname",
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "body",
            1, 1, "procname",
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "cmdcount",
            0, 0, (char*)NULL,
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "complete",
            1, 1, "command",
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "default",
            3, 3, "procname arg varname",
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "level",
            0, 1, "?number?",
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "library",
            0, 0, (char*)NULL,
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "locals",
            0, 1, "?pattern?",
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "patchlevel",
            0, 0, (char*)NULL,
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "script",
            0, 0, (char*)NULL,
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info", "tclversion",
            0, 0, (char*)NULL,
            Itcl_InfoOtherCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK
    ) {
        return TCL_ERROR;
    }

    /*
     *  Add the "info namespace" ensemble.
     */
    if (Itcl_CreateEnsemble(interp, "::info namespace") != TCL_OK) {
        return TCL_ERROR;
    }
    if (Itcl_AddEnsembleOption(interp, "::info namespace", "all",
            0, ITCL_VAR_ARGS, "?pattern?",
            Itcl_InfoNamespAllCmd, (ClientData)NULL,(Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info namespace", "qualifiers",
            1, 1, "string",
            Itcl_InfoNamespQualCmd, (ClientData)NULL,(Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info namespace", "tail",
            1, 1, "string",
            Itcl_InfoNamespTailCmd, (ClientData)NULL,(Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info namespace", "parent",
            0, 1, "?name?",
            Itcl_InfoNamespParentCmd, (ClientData)NULL,(Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::info namespace", "children",
            0, 1, "?name?",
            Itcl_InfoNamespChildCmd, (ClientData)NULL,(Tcl_CmdDeleteProc*)NULL)
            != TCL_OK
    ) {
        return TCL_ERROR;
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_NamespaceCmd()
 *
 *  Invoked by Tcl whenever the user issues a "namespace" command
 *  to create a namespace or add things to it.  Handles the following
 *  syntax:
 *
 *    namespace <name> ?-local? ?-enforced val? ?<commands>?
 *
 *  The namespace <name> does not exist, then it is automatically
 *  created.  By default, <name> can reference an imported namespace.
 *  If the "-local" flag is specified, then the lookup procedure will
 *  only search the current namespace context for <name>, and if it
 *  is not found, a namespace <name> will be created in the local
 *  context.  If "-enforced" is specified, then "val" is a boolean
 *  value specifying whether or not "enforce_cmd" and "enforce_var"
 *  procedures should be used to enforce command/variable resolution.
 *
 *  The namespace <name> is installed as the active namespace,
 *  and the <commands> are invoked in that context.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_NamespaceCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    int flags = 0;       /* by default, look for imported namespaces */
    int doEnforce = -1;  /* by default, leave cmd/var enforcement alone */

    int pos, status, val;
    Itcl_Namespace ns;

    if (argc < 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name ?-local? ?-enforced val? ?--?",
            " ?commands?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Handle flags like "-local"...
     */
    for (pos=2; pos < argc; pos++) {
        if (*argv[pos] == '-') {
            if (strcmp(argv[pos],"-local") == 0) {
                flags = ITCL_FIND_LOCAL_ONLY;
            }
            else if (strcmp(argv[pos],"-enforced") == 0) {
                if (pos+1 < argc) {
                    if (Tcl_GetBoolean(interp, argv[pos+1], &val) != TCL_OK) {
                        return TCL_ERROR;
                    }
                    doEnforce = val;
                }
                else {
                    Tcl_AppendResult(interp, "missing value ",
                        "for option \"-enforced\"",
                        (char*)NULL);
                    return TCL_ERROR;
                }
                pos++;  /* skip over value */
            }
            else if (strcmp(argv[pos],"--") == 0) {
                pos++;  /* skip over -- */
                break;
            }
            else {
                Tcl_AppendResult(interp, "bad option \"", argv[pos],
                    "\": should be \"", argv[0], " name",
                    " ?-local? ?-enforced val? ?--?",
                    " ?commands?\"",
                    (char*)NULL);
                return TCL_ERROR;
            }
        }
        else
            break;
    }

    if (pos != argc-1 && pos != argc) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name ?-local? ?-enforced val? ?--?",
            " ?commands?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Look for the specified namespace, and if it is not found,
     *  create it.
     */
    if (Itcl_FindNamesp(interp, argv[1], flags, &ns) != TCL_OK) {
        return TCL_ERROR;
    }
    if (ns == NULL) {
        if (Itcl_CreateNamesp(interp, argv[1], (ClientData)NULL,
            (Itcl_DeleteProc*)NULL, &ns) != TCL_OK) {
            return TCL_ERROR;
        }
    }

    /*
     *  If a "-enforced" flag was specified, then install or remove
     *  enforcement procs.
     */
    if (doEnforce >= 0) {
        if (doEnforce) {
            Itcl_SetCmdEnforcer(ns, Itcl_NamespTclCmdEnf);
            Itcl_SetVarEnforcer(ns, Itcl_NamespTclVarEnf);
        } else {
            Itcl_SetCmdEnforcer(ns, (Itcl_CmdEnforcerProc*)NULL);
            Itcl_SetVarEnforcer(ns, (Itcl_VarEnforcerProc*)NULL);
        }
    }

    /*
     *  If there is a code body, then activate the namespace and
     *  evaluate the commands within it.
     */
    if (pos < argc) {
        status = Itcl_NamespEval(interp, ns, argv[pos]);

        if (status != TCL_OK) {
            char mesg[512];
            sprintf(mesg, "\n    (in namespace \"%.400s\" body line %d)",
                Itcl_GetNamespPath(ns), interp->errorLine);
            Tcl_AddErrorInfo(interp, mesg);
        }
        return status;
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_NamespTclCmdEnf()
 *
 *  Optional "enforcer" procedure for namespaces created by the
 *  "namespace" command.  When the "-enforced" option is turned on,
 *  this procedure is put in place as the command enforcer for the
 *  namespace.
 *
 *  Whenever a command cannot be found in the local cache, this
 *  procedure looks for a command called "enforce_cmd", and if found,
 *  invokes with the command name as an argument to resolve the
 *  command reference.  If the Tcl procedure returns an error, then
 *  this procedure returns the same error, thereby denying access
 *  to the command.  If the Tcl procedure returns "", then this
 *  procedure returns a NULL command reference, and command lookup
 *  continues unimpeded for this namespace.  Otherwise, the Tcl
 *  procedure should return a substitute name for the command.
 *  If it is found, then the reference is returned.  Otherwise, this
 *  returns an error, denying access.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_NamespTclCmdEnf(interp, name, cmdPtr)
    Tcl_Interp *interp;    /* current interpreter */
    char* name;            /* name of the variable being accessed */
    Tcl_Command* cmdPtr;   /* returns Tcl_Command for desired command or NULL */
{
    Itcl_Namespace ns = Itcl_GetActiveNamesp(interp);

    int status;
    Itcl_InterpState istate;
    Itcl_CmdEnforcerProc* savedCmdEnf;
    Itcl_VarEnforcerProc* savedVarEnf;
    Tcl_DString buffer;

    /*
     *  Disable any further enforcement while this is in progress.
     */
    *cmdPtr = NULL;
    status = TCL_OK;

    Tcl_DStringInit(&buffer);

    istate = Itcl_SaveInterpState(interp, 0);
    savedCmdEnf = Itcl_SetCmdEnforcer(ns, (Itcl_CmdEnforcerProc*)NULL);
    savedVarEnf = Itcl_SetVarEnforcer(ns, (Itcl_VarEnforcerProc*)NULL);

    /*
     *  Invoke the "enforce_cmd" proc to resolve the reference.
     */
    Tcl_DStringAppendElement(&buffer, "enforce_cmd");
    Tcl_DStringAppendElement(&buffer, name);

    status = Tcl_Eval(interp, Tcl_DStringValue(&buffer));

    Tcl_DStringTrunc(&buffer, 0);
    Tcl_DStringAppend(&buffer, interp->result, -1);

    if (status != TCL_OK) {
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, "can't access \"", name, "\": ",
            Tcl_DStringValue(&buffer),
            (char*)NULL);
    }

    /*
     *  If "enforce_cmd" comes back with something other than a
     *  null string, then look up that command.
     */
    else if (*interp->result != '\0') {
        status = Itcl_FindCommand(interp,Tcl_DStringValue(&buffer),0,cmdPtr);
    }

    /*
     *  Put enforcer procs back in place and return.
     */
    Tcl_DStringFree(&buffer);

    if (status == TCL_OK) {
        Itcl_RestoreInterpState(interp, istate);
    } else {
        Itcl_DiscardInterpState(istate);
    }
    Itcl_SetCmdEnforcer(ns, savedCmdEnf);
    Itcl_SetVarEnforcer(ns, savedVarEnf);

    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_NamespTclVarEnf()
 *
 *  Optional "enforcer" procedure for namespaces created by the
 *  "namespace" command.  When the "-enforced" option is turned on,
 *  this procedure is put in place as the variable enforcer for the
 *  namespace.
 *
 *  Whenever a variable cannot be found in the local cache, this
 *  procedure looks for a command called "enforce_var", and if found,
 *  invokes with the variable name as an argument to resolve the
 *  variable reference.  If the Tcl procedure returns an error, then
 *  this procedure returns the same error, thereby denying access
 *  to the variable.  If the Tcl procedure returns "", then this
 *  procedure returns a NULL variable reference, and variable lookup
 *  continues unimpeded for this namespace.  Otherwise, the Tcl
 *  procedure should return a substitute name for the variable.
 *  If it is found, then the reference is returned.  Otherwise, this
 *  returns an error, denying access.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_NamespTclVarEnf(interp, name, varPtr, flags)
    Tcl_Interp *interp;    /* current interpreter */
    char* name;            /* name of the variable being accessed */
    Tcl_Var* varPtr;       /* returns Tcl_Var for desired variable or NULL */
    int flags;             /* ITCL_GLOBAL_VAR => global variable desired */
{
    Itcl_Namespace ns = Itcl_GetActiveNamesp(interp);

    int status;
    Itcl_InterpState istate;
    Itcl_CmdEnforcerProc* savedCmdEnf;
    Itcl_VarEnforcerProc* savedVarEnf;
    Tcl_DString buffer;

    /*
     *  Disable any further enforcement while this is in progress.
     */
    *varPtr = NULL;
    status = TCL_OK;

    Tcl_DStringInit(&buffer);

    istate = Itcl_SaveInterpState(interp, 0);
    savedCmdEnf = Itcl_SetCmdEnforcer(ns, (Itcl_CmdEnforcerProc*)NULL);
    savedVarEnf = Itcl_SetVarEnforcer(ns, (Itcl_VarEnforcerProc*)NULL);

    /*
     *  If this is a global variable, invoke the "enforce_var" proc
     *  to resolve the reference.
     */
    if ((flags & ITCL_GLOBAL_MASK) != 0) {
        Tcl_DStringAppendElement(&buffer, "enforce_var");
        Tcl_DStringAppendElement(&buffer, name);

        status = Tcl_Eval(interp, Tcl_DStringValue(&buffer));

        Tcl_DStringTrunc(&buffer, 0);
        Tcl_DStringAppend(&buffer, interp->result, -1);

        if (status != TCL_OK) {
            Tcl_ResetResult(interp);
            Tcl_AppendResult(interp, "can't access \"", name, "\": ",
                Tcl_DStringValue(&buffer),
                (char*)NULL);
        }

        /*
         *  If "enforce_var" comes back with something other than a
         *  null string, then look up that variable.
         */
        else if (*interp->result != '\0') {
            status = Itcl_FindVariable(interp, Tcl_DStringValue(&buffer),
                0, varPtr);
        }
    }

    /*
     *  Put enforcer procs back in place and return.
     */
    Tcl_DStringFree(&buffer);

    if (status == TCL_OK) {
        Itcl_RestoreInterpState(interp, istate);
    } else {
        Itcl_DiscardInterpState(istate);
    }
    Itcl_SetCmdEnforcer(ns, savedCmdEnf);
    Itcl_SetVarEnforcer(ns, savedVarEnf);

    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_VariableCmd()
 *
 *  Invoked by Tcl whenever the user issues a "variable" command
 *  to create a global variable.  Handles the following syntax:
 *
 *    variable <varName> ?<value>?
 *
 *  The variable <varName> does not exist, then it is created and given
 *  the optional <value>.  If it already exists, then its protection
 *  level is updated to the current protection level.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_VariableCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp *iPtr = (Interp*)interp;

    int status, specificRef;
    char *name;
    Tcl_Var var;
    Var* varPtr;
    Namespace* varNs;
    Tcl_HashEntry *entry;

    if (argc < 2 || argc > 3) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name ?value?",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  If the specified variable already exists, then simply
     *  change its protection status.  Otherwise create it
     *  and set its protection status.
     */
    if (Itcl_FindVariable(interp, argv[1], ITCL_FIND_LOCAL_ONLY,
        &var) != TCL_OK) {
        return TCL_ERROR;
    }
    if (var != NULL) {
        varPtr = (Var*)var;
        varPtr->protection = Itcl_VarProtection(interp,0);
        varNs = varPtr->namesp;
    }
    else {
        if (ItclFollowNamespPath(interp, iPtr->activeNs, argv[1],
            ITCL_FIND_AUTO_CREATE, &varNs,&name,&specificRef) != TCL_OK ||
            varNs == NULL) {
            return TCL_ERROR;
        }
        if (name == NULL) {
            Tcl_AppendResult(interp, "can't create \"", argv[1],
                "\": missing variable name",
                (char*)NULL);
            return TCL_ERROR;
        }
        if (strstr(name,"(")) {
            Tcl_AppendResult(interp, "can't create \"", argv[1],
                "\": looks like an array reference",
                (char*)NULL);
            return TCL_ERROR;
        }
        entry = Tcl_CreateHashEntry(&varNs->variables, name, &status);
        varPtr = TclNewVar(interp);
        Tcl_SetHashValue(entry, varPtr);
        varPtr->hPtr = entry;
        varPtr->name = Tcl_GetHashKey(&varNs->variables, entry);
    }

    /*
     *  Any change to a variable in this namespace may change
     *  the way other variables are imported.  Signal the change.
     */
    Itcl_NsCacheVarChanged((Itcl_Namespace)varNs, argv[1]);

    /*
     *  If an initial value is specified, then activate the
     *  namespace and set the value.
     */
    if (argc == 3 && Tcl_SetVar2(interp, argv[1], (char*)NULL,
        argv[2], ITCL_GLOBAL_VAR|TCL_LEAVE_ERR_MSG) == NULL) {
        return TCL_ERROR;
    }
    return TCL_OK;
}



/*
 * ------------------------------------------------------------------------
 *  Itcl_ImportListCmd()
 *
 *  Invoked by Tcl whenever the user issues an "import list" command
 *  to query the import list for a namespace.  Handles the following
 *  syntax:
 *
 *    import list ?<namespaceList>?
 *
 *  If no <namespaceList> is specified, this command returns the
 *  current list of imported namespaces.  Otherwise, it sets the
 *  current list to <namespaceList>.
 *
 *  Imported namespaces are always reported as a list of two
 *  elements:  the namespace name and the import protection level
 *  (public or protected).  If an import reference (e.g., an element
 *  in <namespaceList>) only contains the namespace name, the
 *  protection level is assumed to be public.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_ImportListCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp *iPtr = (Interp*)interp;

    int i;
    int listc;
    char **listv;
    Itcl_List importList;
    Itcl_ListElem *elem;
    NamespImportRef *nsref, nsr;

    /*
     *  Handle an import list specification:
     *    import list {{foo public} {bar protected}... }
     */
    if (argc == 2) {
        if (Tcl_SplitList(interp, argv[1], &listc, &listv) != TCL_OK) {
            return TCL_ERROR;
        }
        Itcl_ClearImportNamesp((Itcl_Namespace)iPtr->activeNs);
        for (i=0; i < listc; i++) {
            if (ItclDecodeImportRef(interp, listv[i], &nsr) != TCL_OK) {
                ckfree((char*)listv);
                return TCL_ERROR;
            }
            Itcl_AppendImportNamesp((Itcl_Namespace)iPtr->activeNs,
                (Itcl_Namespace)nsr.namesp, nsr.protection);
        }
        ckfree((char*)listv);
    }

    /*
     *  Return the current import list.
     */
    Itcl_GetImportNamesp((Itcl_Namespace)iPtr->activeNs, &importList);
    elem = Itcl_FirstListElem(&importList);
    while (elem) {
        nsref = (NamespImportRef*)Itcl_GetListValue(elem);
        Tcl_AppendElement(interp, ItclEncodeImportRef(nsref));
        elem = Itcl_NextListElem(elem);
    }
    Itcl_DeleteList(&importList);

    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_ImportAllCmd()
 *
 *  Invoked by Tcl whenever the user issues an "import all" command
 *  to query the list of all namespaces imported directly or indirectly
 *  by a namespace.  Handles the following syntax:
 *
 *    import all ?<namespace>?
 *
 *  If no <namespace> is specified, this command returns the information
 *  for the current namespace.  Returns a list of namespace references
 *  starting with <namespace> or the current namespace, and following
 *  the import list exactly as it is done for command/variable lookup.
 *  Imported namespaces are always reported as a list of two
 *  elements:  the namespace name and the import protection level
 *  (public or protected).
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_ImportAllCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Itcl_Namespace ns;
    Itcl_List imports;
    Itcl_ListElem *elem;
    NamespImportRef *nsref;

    /*
     *  If a namespace is specified, then see if it can be found.
     */
    if (argc == 2) {
        if (Itcl_FindNamesp(interp, argv[1], 0, &ns) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (ns == NULL) {
            Tcl_AppendResult(interp, "namespace \"",
                argv[1], "\" not found in context \"",
                Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
                (char*)NULL);
            return TCL_ERROR;
        }
    }
    else {
        ns = Itcl_GetActiveNamesp(interp);
    }

    Itcl_GetAllImportNamesp(ns, &imports);
    elem = Itcl_FirstListElem(&imports);

    while (elem) {
        nsref = (NamespImportRef*)Itcl_GetListValue(elem);
        Tcl_AppendElement(interp, ItclEncodeImportRef(nsref));
        elem = Itcl_NextListElem(elem);
    }
    Itcl_DeleteList(&imports);

    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_ImportAddCmd()
 *
 *  Invoked by Tcl whenever the user issues an "import add" command
 *  to modify the import list for a namespace.  Handles the following
 *  syntax:
 *
 *    import add <namespace> ?<namespace>...? ?-where pos...?
 *
 *  Each <namespace> is either a namespace name, or a list with the
 *  namespace name and its import protection level (public/protected).
 *  If no "-where" clause is specified, the namespaces are appended
 *  to the current import list.  Otherwise their insertion position
 *  is determined as follows:
 *
 *    -after <namespace> ?<namespace>...?
 *       Namespaces are inserted after the specified namespace.  If
 *       more than one <namespace> position is specified, then 
 *       namespaces are inserted after whichever one appears nearest
 *       the end of the import list.
 *
 *    -before <namespace> ?<namespace>...?
 *       Namespaces are inserted before the specified namespace.  If
 *       more than one <namespace> position is specified, then 
 *       namespaces are inserted before whichever one appears nearest
 *       the beginning of the import list.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_ImportAddCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp *iPtr = (Interp*)interp;
    int status = TCL_OK;

    int i;
    Itcl_List nsList;
    NamespImportRef nsr, *nsref, *importRef;
    Itcl_ListElem *elem, *nsPos, *importPos;
    Itcl_List importList;

    Itcl_InitList(&nsList);

    /*
     *  Process all arguments up to the "-where" clause as
     *  namespace references for the list.
     */
    for (i=1; i < argc; i++) {
        if (*argv[i] == '-')
            break;

        nsref = (NamespImportRef*)ckalloc(sizeof(NamespImportRef));
        Itcl_AppendList(&nsList, (ClientData)nsref);

        if (ItclDecodeImportRef(interp, argv[i], nsref) != TCL_OK) {
            status = TCL_ERROR;
            goto importAddDone;
        }
    }

    /*
     *  If no "-where" clause was specified, then add the
     *  namespaces to the end of the import list.
     */
    if (i >= argc) {
        nsPos = Itcl_FirstListElem(&nsList);
        while (nsPos) {
            nsref = (NamespImportRef*)Itcl_GetListValue(nsPos);
            Itcl_AppendImportNamesp((Itcl_Namespace)iPtr->activeNs,
                (Itcl_Namespace)nsref->namesp, nsref->protection);

            nsPos = Itcl_NextListElem(nsPos);
        }
    }

    /*
     *  Check for any specification for where to add these elements:
     *    -before <namesp> <namesp>...
     */
    else if (strcmp(argv[i],"-before") == 0) {
        Itcl_GetImportNamesp((Itcl_Namespace)iPtr->activeNs, &importList);
        importPos = NULL;
        for (i++; i < argc; i++) {
            if (ItclDecodeImportRef(interp, argv[i], &nsr) != TCL_OK) {
                Itcl_DeleteList(&importList);
                status = TCL_ERROR;
                goto importAddDone;
            }

            /*
             *  Find the topmost namespace reference.
             */
            elem = Itcl_FirstListElem(&importList);
            while (elem && elem != importPos) {
                nsref = (NamespImportRef*)Itcl_GetListValue(elem);
                if (nsref->namesp == nsr.namesp) {
                    break;
                }
                elem = Itcl_NextListElem(elem);
            }
            if (elem) {
                importPos = elem;
            }
            else {
                Tcl_AppendResult(interp, "namespace \"",
                    argv[i], "\" not on import list for \"",
                    Itcl_GetNamespPath((Itcl_Namespace)iPtr->activeNs), "\"",
                    (char*)NULL);
                status = TCL_ERROR;
                goto importAddDone;
            }
        }
        Itcl_DeleteList(&importList);

        importRef = (NamespImportRef*)Itcl_GetListValue(importPos);

        elem = Itcl_FirstListElem(&nsList);
        while (elem) {
            nsref = (NamespImportRef*)Itcl_GetListValue(elem);
            Itcl_InsertImportNamesp2((Itcl_Namespace)iPtr->activeNs,
                (Itcl_Namespace)nsref->namesp, nsref->protection,
                (Itcl_Namespace)importRef->namesp);

            elem = Itcl_NextListElem(elem);
        }
    }

    /*
     *  Check for any specification for where to add these elements:
     *    -after <namesp> <namesp>...
     */
    else if (strcmp(argv[i],"-after") == 0) {
        Itcl_GetImportNamesp((Itcl_Namespace)iPtr->activeNs, &importList);
        importPos = NULL;
        for (i++; i < argc; i++) {
            if (ItclDecodeImportRef(interp, argv[i], &nsr) != TCL_OK) {
                Itcl_DeleteList(&importList);
                status = TCL_ERROR;
                goto importAddDone;
            }

            /*
             *  Find the bottommost namespace reference.
             */
            elem = Itcl_LastListElem(&importList);
            while (elem && elem != importPos) {
                nsref = (NamespImportRef*)Itcl_GetListValue(elem);
                if (nsref->namesp == nsr.namesp) {
                    break;
                }
                elem = Itcl_PrevListElem(elem);
            }
            if (elem) {
                importPos = elem;
            }
            else {
                Tcl_AppendResult(interp, "namespace \"",
                    argv[i], "\" not on import list for \"",
                    Itcl_GetNamespPath((Itcl_Namespace)iPtr->activeNs), "\"",
                    (char*)NULL);
                status = TCL_ERROR;
                goto importAddDone;
            }
        }
        Itcl_DeleteList(&importList);

        importRef = (NamespImportRef*)Itcl_GetListValue(importPos);

        elem = Itcl_FirstListElem(&nsList);
        while (elem) {
            nsref = (NamespImportRef*)Itcl_GetListValue(elem);
            Itcl_AppendImportNamesp2((Itcl_Namespace)iPtr->activeNs,
                (Itcl_Namespace)nsref->namesp, nsref->protection,
                (Itcl_Namespace)importRef->namesp);

            elem = Itcl_NextListElem(elem);
        }
    }
    else {
        Tcl_AppendResult(interp, "bad import specification \"",
            argv[i], "\": should be -after or -before",
            (char*)NULL);
        status = TCL_ERROR;
    }

importAddDone:
    elem = Itcl_FirstListElem(&nsList);
    while (elem) {
        nsref = (NamespImportRef*)Itcl_GetListValue(elem);
        ckfree((char*)nsref);
        elem = Itcl_NextListElem(elem);
    }
    Itcl_DeleteList(&nsList);

    return status;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_ImportRemoveCmd()
 *
 *  Invoked by Tcl whenever the user issues an "import remove" command
 *  to modify the import list for a namespace.  Handles the following
 *  syntax:
 *
 *    import remove <namespace> ?<namespace>...?
 *
 *  Each <namespace> is removed from the import list.  If a <namespace>
 *  is not on the import list, it is ignored.
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_ImportRemoveCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp *iPtr = (Interp*)interp;
    int i;
    NamespImportRef nsr;

    for (i=1; i < argc; i++) {
        if (ItclDecodeImportRef(interp, argv[i], &nsr) != TCL_OK) {
            return TCL_ERROR;
        }
        Itcl_RemoveImportNamesp((Itcl_Namespace)iPtr->activeNs,
            (Itcl_Namespace)nsr.namesp);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ProtectionCmd()
 *
 *  Invoked by Tcl whenever the user issues a protection setting
 *  command like "public" or "private".  Creates commands and
 *  variables, and assigns a protection level to them.  Protection
 *  levels are defined as follows:
 *
 *    public    => accessible from any namespace
 *    protected => accessible from selected namespaces
 *    private   => accessible only in the namespace where it was defined
 *
 *  Handles the following syntax:
 *
 *    public <command> ?<arg> <arg>...?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
int
Itcl_ProtectionCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* protection level (public/protected/private) */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    int pLevel = (int)clientData;

    int status;
    int oldCmdLevel, oldVarLevel;
    char *cmdStr;

    if (argc < 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " command ?arg arg...?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    oldCmdLevel = Itcl_CmdProtection(interp, pLevel);
    oldVarLevel = Itcl_VarProtection(interp, pLevel);

    if (argc == 2) {
        status = Tcl_Eval(interp, argv[1]);
    } else {
        cmdStr = Tcl_Merge(argc-1,argv+1);
        status = Itcl_EvalArgsFull(interp, cmdStr, (char*)NULL, argc-1, argv+1,
				   NULL, NULL);
        ckfree(cmdStr);
    }

    if (status == TCL_BREAK) {
        Tcl_SetResult(interp, "invoked \"break\" outside of a loop",
            TCL_STATIC);
        status = TCL_ERROR;
    }
    else if (status == TCL_CONTINUE) {
        Tcl_SetResult(interp, "invoked \"continue\" outside of a loop",
            TCL_STATIC);
        status = TCL_ERROR;
    }
    else if (status != TCL_OK) {
        char mesg[256];
        sprintf(mesg, "\n    (%.100s body line %d)",
            argv[0], interp->errorLine);
        Tcl_AddErrorInfo(interp, mesg);
    }

    Itcl_CmdProtection(interp, oldCmdLevel);
    Itcl_VarProtection(interp, oldVarLevel);

    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_DelNamespCmd()
 *
 *  Part of the "delete" ensemble.  Invoked by Tcl whenever the
 *  user issues a "delete namespace" command to delete namespaces.
 *  Handles the following syntax:
 *
 *    delete namespace <name> ?<name>...?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_DelNamespCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    int i;
    Itcl_Namespace ns;

    if (argc < 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            Itcl_EnsembleInvoc(), " name ?name...?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Destroying one namespace may cause another to be destroyed.
     *  Break this into two passes:  first check to make sure that
     *  all namespaces on the command line are valid, then delete them.
     */
    for (i=1; i < argc; i++) {
        if (Itcl_FindNamesp(interp, argv[i], 0, &ns) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (ns == NULL) {
            Tcl_AppendResult(interp, "namespace \"",
                argv[i], "\" not found in context \"",
                Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
                (char*)NULL);
            return TCL_ERROR;
        }
    }
    for (i=1; i < argc; i++) {
        if (Itcl_FindNamesp(interp, argv[i], 0, &ns) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (ns != NULL) {
            Itcl_DeleteNamesp(ns);
        }
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ScopeCmd()
 *
 *  Invoked by Tcl whenever the user issues a "scope" command to
 *  create a "scoped" value.  Handles the following syntax:
 *
 *    scope <string>
 *
 *  If the input string is of the form:
 *
 *    @scope <name> <value>
 *
 *  then it is already a scoped value.  It is returned exactly
 *  as it stands.  Otherwise, it is treated as an ordinary <value>
 *  string and this procedure encodes it by adding the "@scope"
 *  and current namespace <name> parts.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_ScopeCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Itcl_ScopedVal sval;

    if (argc != 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " string\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  If this looks like a scoped value already, then make
     *  sure it decodes successfully.  Return it as is.
     */
    if (strncmp(argv[1], "@scope", 6) == 0) {
        Itcl_ScopedValInit(&sval);
        if (Itcl_ScopedValDecode(interp, argv[1], &sval) != TCL_OK) {
            return TCL_ERROR;
        }
        Itcl_ScopedValFree(&sval);

        Tcl_SetResult(interp, argv[1], TCL_VOLATILE);
        return TCL_OK;
    }

    /*
     *  Otherwise, construct a scoped value by integrating the
     *  current namespace context.
     */
    if (*argv[1] != '\0') {
        Tcl_AppendElement(interp, "@scope");
        Tcl_AppendElement(interp,
            Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)));
        Tcl_AppendElement(interp, argv[1]);
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_CodeCmd()
 *
 *  Invoked by Tcl whenever the user issues a "code" command to
 *  create a "scoped" command string.  Handles the following syntax:
 *
 *    code ?-namespace foo? arg ?arg arg ...?
 *
 *  Unlike the scope command, the code command DOES NOT look for
 *  scoping information at the beginning of the command.  So scopes
 *  will nest in the code command.
 *
 *  Also, the code command preserves the list structure of the input
 *  commands.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_CodeCmd(dummy, interp, argc, argv)
  ClientData dummy;        /* unused */
  Tcl_Interp *interp;      /* current interpreter */
  int argc;                /* number of arguments */
  char **argv;             /* argument strings */
{
    char *argv0 = argv[0];
    char *namesp = NULL;
    int i;
    Tcl_DString result;

    /*
     *  Handle flags like "-namespace"...
     */
    while (argc > 1 && *argv[1] == '-') {
        if (strcmp(argv[1],"-namespace") == 0) {
            if (argc == 2) {
                Tcl_AppendResult(interp, "wrong # args: should be \"",
                    argv0, " ?-namespace name? command ?arg arg...?",
                    (char*)NULL);
                return TCL_ERROR;
            } else {
                namesp = argv[2];
                argv += 2;
                argc -= 2;
            }
        }
        else if (strcmp(argv[1],"--") == 0) {
            argv += 1;
            argc -= 1;
            break;
        }
        else {
            Tcl_AppendResult(interp,"bad option \"", argv[1],
                "\": should be -namespace or --",
                (char*)NULL);
            return TCL_ERROR;
        }
    }

    if (argc < 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv0, " ?-namespace name? command ?arg arg...?",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Now construct a scoped command by integrating the
     *  current namespace context, and appending the remaining
     *  arguments AS A LIST...
     */
    Tcl_DStringInit(&result);
    Tcl_DStringAppendElement(&result, "@scope");

    if (namesp) {
        Tcl_DStringAppendElement(&result,namesp);
    } else {
        Tcl_DStringAppendElement(&result,
            Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp))
        );
    }

    if (argc == 2) {
        Tcl_DStringAppendElement(&result, argv[1]);
    } else {
        Tcl_DStringStartSublist(&result);
        for (i=1; i < argc; i++) {
            Tcl_DStringAppendElement(&result, argv[i]);
        }
        Tcl_DStringEndSublist(&result);
    }

    Tcl_DStringResult(interp,&result);
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_AtScopeCmd()
 *
 *  Invoked by Tcl whenever the user issues a "@scope" command to
 *  execute a script in the context of a namespace.  The "@scope"
 *  command is a lot like the "namespace" command, except that
 *  the namespace must already exist, and any extra arguments are
 *  appended onto the command string before evaluation.  This
 *  command is used to support the Itcl_ScopeVal mechanism for
 *  widgets and extensions that work with namespaces.  Handles
 *  the following syntax:
 *
 *    @scope <name> <commands> ?<arg> <arg>...?
 *
 *  All extra <arg> arguments are appended onto the <command> string
 *  as list elements, and the resulting script is executed in the
 *  context of the <name> namespace.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_AtScopeCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    int i, status;
    Itcl_Namespace ns;
    Tcl_DString buffer;
    char *cmdstr;

    if (argc < 3) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name command ?arg arg ...?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Look for the specified namespace, and if it is not found,
     *  return an error.
     */
    if (Itcl_FindNamesp(interp, argv[1], 0, &ns) != TCL_OK) {
        return TCL_ERROR;
    }
    if (ns == NULL) {
        Tcl_AppendResult(interp, "namespace \"",
            argv[1], "\" not found in context \"",
            Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Load the command string into the buffer, and if there
     *  are any extra arguments, append them onto the command
     *  string as list elements.
     */
    if (argc > 3) {
        Tcl_DStringInit(&buffer);
        Tcl_DStringAppend(&buffer, argv[2], -1);
        for (i=3; i < argc; i++) {
            Tcl_DStringAppendElement(&buffer, argv[i]);
        }
        cmdstr = Tcl_DStringValue(&buffer);
    }
    else {
        cmdstr = argv[2];
    }

    /*
     *  Activate the desired namespace and evaluate the commands
     *  within it.
     */
    status = Itcl_NamespEval(interp, ns, cmdstr);

    if (status != TCL_OK) {
        char mesg[512];
        sprintf(mesg, "\n    (in namespace \"%.400s\" body line %d)",
            Itcl_GetNamespPath(ns), interp->errorLine);
        Tcl_AddErrorInfo(interp, mesg);
    }

    if (argc > 3) {
        Tcl_DStringFree(&buffer);
    }
    return status;
}


/*
 * ------------------------------------------------------------------------
 *  ItclEncodeImportRef()
 *
 *  Converts a reference on a namespace importList to its character
 *  string description.  Returns a pointer to a string in the following
 *  form:
 *
 *      "namespaceName protectionLevel"
 *
 *  This string remains valid until the next call to this procedure.
 * ------------------------------------------------------------------------
 */
char*
ItclEncodeImportRef(nsref)
    NamespImportRef* nsref;  /* reference to imported namespace */
{
    static Tcl_DString *buffer = NULL;

    /*
     *  If the result buffer does not exist, then create
     *  one now.  Otherwise, clear the existing buffer.
     */
    if (buffer == NULL) {
        buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
        Tcl_DStringInit(buffer);
    }
    else {
        Tcl_DStringTrunc(buffer,0);
    }

    Tcl_DStringAppendElement(buffer,
        Itcl_GetNamespPath((Itcl_Namespace)nsref->namesp));

    switch (nsref->protection) {
    case ITCL_PUBLIC:
        Tcl_DStringAppendElement(buffer, "public");
        break;
    case ITCL_PROTECTED:
        Tcl_DStringAppendElement(buffer, "protected");
        break;
    case ITCL_PRIVATE:
        Tcl_DStringAppendElement(buffer, "private");
        break;
    default:
        assert(0);
    }
    return Tcl_DStringValue(buffer);
}

/*
 * ------------------------------------------------------------------------
 *  ItclDecodeImportRef()
 *
 *  Decodes a string description of an imported namespace, extracting
 *  both the namespace name and the import protection level.  If
 *  successful, this procedure stores the results in the given namespace
 *  reference structure and returns TCL_OK.  Otherwise, an error message
 *  is returned as the interpreter result, and this procedure returns
 *  TCL_ERROR.
 * ------------------------------------------------------------------------
 */
int
ItclDecodeImportRef(interp,str,nsref)
    Tcl_Interp* interp;           /* interpreter managing input string */
    char* str;                    /* input string to be decoded */
    NamespImportRef* nsref;       /* returns: namespace reference info */
{
    int status = TCL_OK;

    int refc;
    char** refv;
    Itcl_Namespace ns;

    if (Tcl_SplitList(interp, str, &refc, &refv) != TCL_OK) {
        return TCL_ERROR;
    }

    if (refc < 1 || refc > 2) {
        Tcl_AppendResult(interp, "invalid import reference \"",
            str, "\": should be \"namespace protection\"",
            (char*)NULL);
        status = TCL_ERROR;
        goto decodeDone;
    }
    if (Itcl_FindNamesp(interp, refv[0], 0, &ns) != TCL_OK) {
        status = TCL_ERROR;
        goto decodeDone;
    }
    else if (ns == NULL) {
        Tcl_AppendResult(interp, "namespace \"",
            refv[0], "\" not found in context \"",
            Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
            (char*)NULL);
        status = TCL_ERROR;
        goto decodeDone;
    }
    nsref->namesp = (Namespace*)ns;

    if (refc == 1) {
        nsref->protection = ITCL_PUBLIC;
    }
    else if (strcmp(refv[1],"public") == 0) {
        nsref->protection = ITCL_PUBLIC;
    }
    else if (strcmp(refv[1],"protected") == 0) {
        nsref->protection = ITCL_PROTECTED;
    }
    else if (strcmp(refv[1],"private") == 0) {
        nsref->protection = ITCL_PRIVATE;
    }
    else {
        Tcl_AppendResult(interp, "invalid protection level \"",
            refv[1], "\": should be public, protected or private",
            (char*)NULL);
        status = TCL_ERROR;
        goto decodeDone;
    }

decodeDone:
    ckfree((char*)refv);
    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoCommandsCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info commands" command
 *  to query the commands available in the current namespace.
 *  Handles the following syntax:
 *
 *    info commands ?<pattern>?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoCommandsCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp *iPtr = (Interp*)interp;

    char *name;
    Tcl_HashTable *cmdTable;
    Tcl_HashEntry *entry;
    Tcl_HashSearch search;

    cmdTable = &iPtr->activeNs->commands;
    entry = Tcl_FirstHashEntry(cmdTable, &search);

    while (entry != NULL) {
        name = Tcl_GetHashKey(cmdTable, entry);
        if ((argc != 2) || Tcl_StringMatch(name, argv[1])) {
            Tcl_AppendElement(interp, name);
        }
        entry = Tcl_NextHashEntry(&search);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoProcsCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info procs" command
 *  to query the Tcl procedures defined in the current namespace.
 *  Handles the following syntax:
 *
 *    info procs ?<pattern>?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoProcsCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp *iPtr = (Interp*)interp;

    char *name;
    Tcl_HashTable *cmdTable;
    Tcl_HashEntry *entry;
    Tcl_HashSearch search;
    Command *cmdPtr;

    cmdTable = &iPtr->activeNs->commands;
    entry = Tcl_FirstHashEntry(cmdTable, &search);

    while (entry != NULL) {
        name = Tcl_GetHashKey(cmdTable, entry);
        cmdPtr = (Command*)Tcl_GetHashValue(entry);
        if (TclIsProc(cmdPtr)) {
            if ((argc != 2) || Tcl_StringMatch(name, argv[1])) {
                Tcl_AppendElement(interp, name);
            }
        }
        entry = Tcl_NextHashEntry(&search);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoGlobalsCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info globals" command
 *  to query the global variables defined in the current namespace.
 *  Handles the following syntax:
 *
 *    info globals ?<pattern>?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoGlobalsCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp *iPtr = (Interp*)interp;

    char *name;
    Tcl_HashTable *varTable;
    Tcl_HashEntry *entry;
    Tcl_HashSearch search;
    Var *varPtr;

    varTable = &iPtr->activeNs->variables;
    entry = Tcl_FirstHashEntry(varTable, &search);

    while (entry != NULL) {
        varPtr = (Var*)Tcl_GetHashValue(entry);
        if ( !(varPtr->flags & VAR_UNDEFINED) ) {
            name = Tcl_GetHashKey(varTable, entry);
            if ((argc != 2) || Tcl_StringMatch(name, argv[1])) {
                Tcl_AppendElement(interp, name);
            }
        }
        entry = Tcl_NextHashEntry(&search);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoVarsCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info vars" command
 *  to query the variables defined in the current callframe.
 *  Handles the following syntax:
 *
 *    info vars ?<pattern>?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoVarsCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp *iPtr = (Interp*)interp;

    char *name;
    Tcl_HashTable *varTable;
    Tcl_HashEntry *entry;
    Tcl_HashSearch search;
    Var *varPtr;

    if (iPtr->varFramePtr &&
        (iPtr->varFramePtr->flags&ITCL_GLOBAL_VAR) == 0) {
        varTable = &iPtr->varFramePtr->varTable;
    }
    else {
        varTable = &iPtr->activeNs->variables;
    }
    entry = Tcl_FirstHashEntry(varTable, &search);

    while (entry != NULL) {
        varPtr = (Var*)Tcl_GetHashValue(entry);
        if ( !(varPtr->flags & VAR_UNDEFINED) ) {
            name = Tcl_GetHashKey(varTable, entry);
            if ((argc != 2) || Tcl_StringMatch(name, argv[1])) {
                Tcl_AppendElement(interp, name);
            }
        }
        entry = Tcl_NextHashEntry(&search);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoExistsCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info exists" command
 *  to see if a variable has been defined.
 *  Handles the following syntax:
 *
 *    info exists <varName>
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoExistsCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp* iPtr = (Interp*)interp;

    char *p;
    Tcl_HashTable *varTable;
    Tcl_HashEntry *entry;
    Tcl_Var var;
    Var *varPtr;

    /*
     * The code below handles the special case where the name is for
     * an array:  Tcl_GetVar will reject this since you can't read
     * an array variable without an index.
     */
    p = Tcl_GetVar(interp, argv[1], 0);
    if (p == NULL) {
        if (strchr(argv[1], '(') != NULL) {
        noVar:
            iPtr->result = "0";
            return TCL_OK;
        }

        if (iPtr->varFramePtr &&
            (iPtr->varFramePtr->flags&ITCL_GLOBAL_VAR) == 0) {
            varTable = &iPtr->varFramePtr->varTable;
            entry = Tcl_FindHashEntry(varTable, argv[1]);
            if (entry == NULL) {
                goto noVar;
            }
            var = (Tcl_Var)Tcl_GetHashValue(entry);
        }
        else if (Itcl_FindVariable(interp, argv[1], 0, &var) != TCL_OK) {
            goto noVar;
        }
        else if (var == NULL) {
            goto noVar;
        }

        varPtr = (Var*)var;
        if (varPtr->flags & VAR_UPVAR) {
            varPtr = varPtr->value.upvarPtr;
        }
        if (!(varPtr->flags & VAR_ARRAY)) {
            goto noVar;
        }
    }
    iPtr->result = "1";
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoContextCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info context" command
 *  to query the current namespace context.
 *  Handles the following syntax:
 *
 *    info context
 *
 *  Examines the current namespace context and returns its full path
 *  name.  The global namespace ("::") is represented as a null
 *  string, so that it is easy to do things like this:
 *
 *    namespace [info context]::child { ... }
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoContextCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp *iPtr = (Interp*)interp;
    char *path;

    path = Itcl_GetNamespPath((Itcl_Namespace)iPtr->activeNs);
    if (strcmp(path,"::") != 0) {
        Tcl_SetResult(interp, path, TCL_VOLATILE);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoWhichCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info which" command
 *  to resolve the complete namespace path to a particular command
 *  or global variable.  Handles the following syntax:
 *
 *    info which ?-command? ?-variable? ?-namespace? <name>
 *
 *  By default, <name> is treated as a command name.  The flags
 *  used to explicitly request different lookup treatments.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoWhichCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    int lookup = 0;  /* assume command lookup by default */
    int i;
    Tcl_Command cmd;
    Tcl_Var var;
    Itcl_Namespace ns;

    /*
     *  Look for extra flags controlling lookup.
     */
    for (i=1; i < argc; i++) {
        if (*argv[i] == '-') {
            if (strcmp(argv[i],"-command") == 0) {
                lookup = 0;
            }
            else if (strcmp(argv[i],"-variable") == 0) {
                lookup = 1;
            }
            else if (strcmp(argv[i],"-namespace") == 0) {
                lookup = 2;
            }
            else {
                Tcl_AppendResult(interp, "bad option \"",
                    argv[i], "\": should be -command, -variable or -namespace",
                    (char*)NULL);
                return TCL_ERROR;
            }
        }
        else
            break;
    }

    if (i != argc-1) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            Itcl_EnsembleInvoc(), " ?-command? ?-variable? ?-namespace? name\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    switch (lookup) {
    case 0:
        if (Itcl_FindCommand(interp, argv[i], 0, &cmd) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (cmd != NULL) {
            Tcl_SetResult(interp, Tcl_GetCommandPath(interp,cmd), TCL_VOLATILE);
        }
        break;

    case 1:
        if (Itcl_FindVariable(interp, argv[i], 0, &var) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (var != NULL) {
            Tcl_SetResult(interp, Tcl_GetVariableName(interp,var), TCL_VOLATILE);
        }
        break;

    case 2:
        if (Itcl_FindNamesp(interp, argv[i], 0, &ns) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (ns != NULL) {
            Tcl_SetResult(interp, Itcl_GetNamespPath(ns), TCL_VOLATILE);
        }
        break;
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoProtectionCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info protection" command
 *  to query the protection level for a particular command or variable.
 *  Handles the following syntax:
 *
 *    info protection ?-command? ?-variable? <name>
 *
 *  By default, <name> is treated as a command name.  The "-command"
 *  and "-variable" flags can be used to explicitly request command
 *  or variable lookup.  If the <name> is not recognized, this command
 *  returns an error.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoProtectionCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    int cmdLookup = 1;  /* assume command lookup by default */
    int i, pLevel;
    Tcl_Command cmd;
    Command *cmdPtr;
    Tcl_Var var;
    Var *varPtr;

    /*
     *  Look for the "-command" and "-variable" flags.
     */
    for (i=1; i < argc; i++) {
        if (*argv[i] == '-') {
            if (strcmp(argv[i],"-command") == 0) {
                cmdLookup = 1;
            }
            else if (strcmp(argv[i],"-variable") == 0) {
                cmdLookup = 0;
            }
            else {
                Tcl_AppendResult(interp, "bad option \"",
                    argv[i], "\": should be -command or -variable",
                    (char*)NULL);
                return TCL_ERROR;
            }
        }
        else
            break;
    }

    if (i != argc-1) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            Itcl_EnsembleInvoc(), " ?-command? ?-variable? name\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    if (cmdLookup) {
        if (Itcl_FindCommand(interp, argv[i], 0, &cmd) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (cmd == NULL) {
            Tcl_AppendResult(interp, "invalid command name \"", argv[i], "\"",
                (char*)NULL);
            return TCL_ERROR;
        }
        cmdPtr = (Command*)cmd;
        pLevel = cmdPtr->protection;
    }
    else {
        if (Itcl_FindVariable(interp, argv[i], 0, &var) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (var == NULL) {
            Tcl_AppendResult(interp, "can't read \"", argv[i],
                "\": no such variable",
                (char*)NULL);
            return TCL_ERROR;
        }
        varPtr = (Var*)var;
        pLevel = varPtr->protection;
    }

    /*
     *  Convert the protection level to a human-readable string.
     */
    switch (pLevel) {
    case ITCL_PUBLIC:
        Tcl_SetResult(interp, "public", TCL_STATIC);
        break;
    case ITCL_PROTECTED:
        Tcl_SetResult(interp, "protected", TCL_STATIC);
        break;
    case ITCL_PRIVATE:
        Tcl_SetResult(interp, "private", TCL_STATIC);
        break;
    default:
        assert(0);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoNamespAllCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info namespace all"
 *  command to query the list of all known namespaces.
 *  Handles the following syntax:
 *
 *    info namespace all ?<pattern>?
 *
 *  Namespaces are only reported if their name matches the optional
 *  "pattern".
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoNamespAllCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Interp *iPtr = (Interp*)interp;

    char *pattern, *name;
    Tcl_HashEntry *entry;
    Tcl_HashSearch search;
    Itcl_List imports;
    Itcl_ListElem *elem;
    Namespace* nsPtr;
    NamespImportRef* nsref;

    if (argc == 1) {
        pattern = NULL;
    }
    else if (argc == 2) {
        pattern = argv[1];
    }
    else {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            Itcl_EnsembleInvoc(), " ?pattern?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Return a list of classes matching the given pattern.
     *  Start with child namespaces in the active namespace...
     */
    entry = Tcl_FirstHashEntry(&iPtr->activeNs->children, &search);
    while (entry) {
        nsPtr = (Namespace*)Tcl_GetHashValue(entry);
        name = Tcl_GetHashKey(&iPtr->activeNs->children, entry);
        if (!pattern || Tcl_StringMatch(name,pattern)) {
            Tcl_AppendElement(interp, name);
        }
        entry = Tcl_NextHashEntry(&search);
    }

    /*
     *  ...then include namespaces that are visible because their
     *  parent namespace was imported.  Distinguish these using
     *  their full namespace name.
     *
     *  NOTE:  Since Itcl_GetAllImportNamesp() always returns the
     *         original namespace as the first element, this one
     *         is not really imported at all.  Skip it.
     */
    Itcl_GetAllImportNamesp((Itcl_Namespace)iPtr->activeNs, &imports);
    elem = Itcl_FirstListElem(&imports);
    if (elem) elem = Itcl_NextListElem(elem);

    while (elem) {
        nsref = (NamespImportRef*)Itcl_GetListValue(elem);

        entry = Tcl_FirstHashEntry(&nsref->namesp->children, &search);
        while (entry) {
            nsPtr = (Namespace*)Tcl_GetHashValue(entry);
            name = Itcl_GetNamespPath((Itcl_Namespace)nsPtr);
            if (!pattern || Tcl_StringMatch(name,pattern)) {
                Tcl_AppendElement(interp, name);
            }
            entry = Tcl_NextHashEntry(&search);
        }
        elem = Itcl_NextListElem(elem);
    }
    Itcl_DeleteList(&imports);

    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoNamespQualCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info namespace qualifiers"
 *  command to get the namespace qualifiers contained in a particular
 *  string.  The "qualifiers" are the "namesp::namesp::namesp" references
 *  leading up to some final element name.  Handles the following syntax:
 *
 *    info namespace qualifiers <string>
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoNamespQualCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    char *head, *tail;

    Itcl_ParseNamespPath(argv[1], &head, &tail);
    if (head) {
        Tcl_SetResult(interp, head, TCL_VOLATILE);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoNamespTailCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info namespace tail"
 *  command to get the namespace qualifiers contained in a particular
 *  string.  The "tail" is the simple name at the end of a path like
 *  "namesp::namesp::namesp::element".  Handles the following syntax:
 *
 *    info namespace tail <string>
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoNamespTailCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    char *head, *tail;

    Itcl_ParseNamespPath(argv[1], &head, &tail);
    if (tail) {
        Tcl_SetResult(interp, tail, TCL_VOLATILE);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoNamespParentCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info namespace parent"
 *  command to get the parent namespace containing another namespace.
 *  Handles the following syntax:
 *
 *    info namespace parent ?<name>?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoNamespParentCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Itcl_Namespace ns;
    Namespace* nsPtr;

    if (argc == 2) {
        if (Itcl_FindNamesp(interp, argv[1], 0, &ns) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (ns == NULL) {
            Tcl_AppendResult(interp, "namespace \"",
                argv[1], "\" not found in context \"",
                Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
                (char*)NULL);
            return TCL_ERROR;
        }
    }
    else {
        ns = Itcl_GetActiveNamesp(interp);
    }
    nsPtr = (Namespace*)ns;

    if (nsPtr->parent) {
        Tcl_SetResult(interp,
            Itcl_GetNamespPath((Itcl_Namespace)nsPtr->parent), TCL_VOLATILE);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoNamespChildCmd()
 *
 *  Invoked by Tcl whenever the user issues an "info namespace children"
 *  command to get the child namespaces within a namespace.
 *  Handles the following syntax:
 *
 *    info namespace children ?<name>?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoNamespChildCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Itcl_Namespace ns;
    Namespace* nsPtr;
    Tcl_HashEntry *entry;
    Tcl_HashSearch place;

    if (argc == 2) {
        if (Itcl_FindNamesp(interp, argv[1], 0, &ns) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (ns == NULL) {
            Tcl_AppendResult(interp, "namespace \"",
                argv[1], "\" not found in context \"",
                Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
                (char*)NULL);
            return TCL_ERROR;
        }
    }
    else {
        ns = Itcl_GetActiveNamesp(interp);
    }
    nsPtr = (Namespace*)ns;

    entry = Tcl_FirstHashEntry(&nsPtr->children, &place);
    while (entry) {
        ns = (Itcl_Namespace)Tcl_GetHashValue(entry);
        Tcl_AppendElement(interp, Itcl_GetNamespPath(ns));
        entry = Tcl_NextHashEntry(&place);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoOtherCmd()
 *
 *  Invoked to handle the usual options in the Tcl "info" command.
 *  Translates from an ensemble command like:
 *
 *      complete <command>
 *
 *  to the argument list expected by Tcl_InfoCmd():
 *
 *      info complete <command>
 *
 *  and then invokes Tcl_InfoCmd() to handle the option.
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoOtherCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    /*
     *  NOTE:  Since the usual "info" command will expect "info"
     *    as the first argument, we must back up the argv list
     *    to include this argument.  This procedure will always
     *    be called from the ItclHandleEnsemble() procedure,
     *    which passes in (argc-1,argv+1).  Because of this,
     *    it is safe to simply decrement the pointer below:
     */
    return Tcl_InfoCmd((ClientData)NULL, interp, argc+1, argv-1);
}
