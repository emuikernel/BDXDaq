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
 *  These procedures handle built-in class methods, including the
 *  "isa" method (to query hierarchy info) and the "info" method
 *  (to query class/object data).
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl_bicmds.c,v 1.2 1997/10/21 18:19:04 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include <assert.h>
#include "itclInt.h"

static char rcsid[] = "$Id: itcl_bicmds.c,v 1.2 1997/10/21 18:19:04 heyes Exp $";

/*
 *  Standard list of built-in methods for all objects.
 */
typedef struct BiMethod {
    char* name;            /* method name */
    char* usage;           /* string describing usage */
    char* registration;    /* registration name for C proc */
    Tcl_CmdProc *proc;     /* implementation C proc */
} BiMethod;

static BiMethod BiMethodList[] = {
    { "cget",      "-option",
                   "@itcl-builtin-cget",  Itcl_BiCgetCmd },
    { "configure", "?-option? ?value -option value...?",
                   "@itcl-builtin-configure",  Itcl_BiConfigureCmd },
    { "conf", "?-option? ?value -option value...?",
                   "@itcl-builtin-conf",  Itcl_BiConfigureCmd },
    { "isa",       "className",
                   "@itcl-builtin-isa",  Itcl_BiIsaCmd },
};
static int BiMethodListLen = sizeof(BiMethodList)/sizeof(BiMethod);


/*
 *  FORWARD DECLARATIONS
 */
static char* ItclReportPublicOpt _ANSI_ARGS_((Tcl_Interp *interp,
    ItclVarDefn *vdefn, Itcl_Object *obj));


/*
 * ------------------------------------------------------------------------
 *  Itcl_BiInit()
 *
 *  Creates a namespace full of built-in methods/procs for [incr Tcl]
 *  classes.  This includes things like the "isa" method and "info"
 *  for querying class info.  Usually invoked by Itcl_Init() when
 *  [incr Tcl] is first installed into an interpreter.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
int
Itcl_BiInit(interp)
    Tcl_Interp *interp;      /* current interpreter */
{
    int i;
    Itcl_Namespace ns;

    /*
     *  Declare all of the built-in methods as C procedures.
     */
    for (i=0; i < BiMethodListLen; i++) {
        if (Itcl_RegisterC(interp, BiMethodList[i].registration+1,
            BiMethodList[i].proc) != TCL_OK) {
            return TCL_ERROR;
        }
    }

    /*
     *  Create the "::itcl::builtin" namespace for built-in class
     *  commands.  Make this an isolated namespace.  It is simply
     *  an extra bag of commands imported into each class.
     */
    if (Itcl_CreateNamesp(interp, "::itcl::builtin",
        (ClientData)NULL, (Itcl_DeleteProc*)NULL, &ns) != TCL_OK) {
        Tcl_AppendResult(interp, " (cannot initialize itcl built-in commands)",
            (char*)NULL);
        return TCL_ERROR;
    }
    Itcl_ClearImportNamesp(ns);

    /*
     *  Create an "info" ensemble in the global namespace, and
     *  install subcommands needed for [incr Tcl].
     */
    if (Itcl_CreateEnsemble(interp, "::itcl::builtin::info") != TCL_OK) {
        return TCL_ERROR;
    }
    if (Itcl_AddEnsembleOption(interp, "::itcl::builtin::info", "class",
            0, 0, (char*)NULL,
            Itcl_BiInfoClassCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::itcl::builtin::info", "inherit",
            0, 0, (char*)NULL,
            Itcl_BiInfoInheritCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::itcl::builtin::info", "heritage",
            0, 0, (char*)NULL,
            Itcl_BiInfoHeritageCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::itcl::builtin::info", "function",
            0, ITCL_VAR_ARGS,
            "?name? ?-protection? ?-type? ?-name? ?-args? ?-body?",
            Itcl_BiInfoFunctionCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::itcl::builtin::info", "variable",
            0, ITCL_VAR_ARGS,
            "?name? ?-protection? ?-type? ?-name? ?-init? ?-value? ?-config?",
            Itcl_BiInfoVariableCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::itcl::builtin::info", "args",
            1, 1, "procname",
            Itcl_BiInfoArgsCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||
        Itcl_AddEnsembleOption(interp, "::itcl::builtin::info", "body",
            1, 1, "procname",
            Itcl_BiInfoBodyCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK
    ) {
        return TCL_ERROR;
    }

    /*
     *  Create an "info" ensemble in the global namespace, and
     *  install subcommands needed for [incr Tcl].
     */
    if (Itcl_AddEnsembleOption(interp, "::itcl::builtin::info", "@error",
            0, ITCL_VAR_ARGS, (char*)NULL,
            Itcl_DefaultInfoCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK
    ) {
        return TCL_ERROR;
    }

    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InstallBiMethods()
 *
 *  Invoked when a class is first created, just after the class
 *  definition has been parsed, to add definitions for built-in
 *  methods to the class.  If a method already exists in the class
 *  with the same name as the built-in, then the built-in is skipped.
 *  Otherwise, a method definition for the built-in method is added.
 *
 *  Returns TCL_OK if successful, or TCL_ERROR (along with an error
 *  message in the interpreter) if anything goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_InstallBiMethods(interp,cdefn)
    Tcl_Interp *interp;      /* current interpreter */
    Itcl_Class *cdefn;       /* class definition to be updated */
{
    int status = TCL_OK;

    int i;
    ItclCmdMember *mdefn;

    /*
     *  Scan through all of the built-in methods and see if
     *  that method already exists in the class.  If not, add
     *  it in.
     */
    for (i=0; i < BiMethodListLen; i++) {
        status = Itcl_FindCmdDefn(interp, cdefn, BiMethodList[i].name, &mdefn);
        if (status != TCL_OK) {
            break;
        }
        if (!mdefn) {
            status = Itcl_CreateMethod(interp, cdefn, BiMethodList[i].name,
                BiMethodList[i].usage, BiMethodList[i].registration);

            if (status != TCL_OK) {
                break;
            }
        }
    }
    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_BiIsaCmd()
 *
 *  Invoked whenever the user issues the "isa" method for an object.
 *  Handles the following syntax:
 *
 *    <objName> isa <className>
 *
 *  Checks to see if the object has the given <className> anywhere
 *  in its heritage.  Returns 1 if so, and 0 otherwise.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_BiIsaCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* class definition */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;

    /*
     *  Make sure that this command is being invoked in the proper
     *  context.
     */
    if ((Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK)
        || !obj) {
        Tcl_AppendResult(interp, "improper usage: should be ",
            "\"object isa className\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    if (argc != 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            "object ", argv[0], " className\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Find the requested class.
     */
    if (Itcl_FindClass(interp, argv[1], &classNs) != TCL_OK) {
        return TCL_ERROR;
    }
    if (classNs == NULL) {
        Tcl_AppendResult(interp, "class \"", argv[1],
            "\" not found in context \"",
            Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    cdefn = (Itcl_Class*)Itcl_GetNamespData(classNs);

    if (Itcl_ObjectIsa(obj,cdefn))
        Tcl_SetResult(interp, "1", TCL_STATIC);
    else
        Tcl_SetResult(interp, "0", TCL_STATIC);

    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_BiConfigureCmd()
 *
 *  Invoked whenever the user issues the "configure" method for an object.
 *  Handles the following syntax:
 *
 *    <objName> configure ?-<option>? ?<value> -<option> <value>...?
 *
 *  Allows access to public variables as if they were configuration
 *  options.  With no arguments, this command returns the current
 *  list of public variable options.  If -<option> is specified,
 *  this returns the information for just one option:
 *
 *    -<optionName> <initVal> <currentVal>
 *
 *  Otherwise, the list of arguments is parsed, and values are
 *  assigned to the various public variable options.  When each
 *  option changes, a big of "config" code associated with the option
 *  is executed, to bring the object up to date.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_BiConfigureCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* class definition */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    static Tcl_DString *buffer = NULL;

    Itcl_Namespace classNs;
    Itcl_Class *cdefn, *cd;
    Itcl_Object *obj;

    int i, status;
    char *lastval;
    ItclClass *cdPtr;
    Tcl_HashSearch place;
    Tcl_HashEntry *entry;
    ItclVarDefn *vdefn;
    Itcl_HierIter hier;
    Itcl_ActiveNamespace vscope, nsToken;

    /*
     *  If the utility buffer has not yet been created, do it now.
     */
    if (buffer == NULL) {
        buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
        Tcl_DStringInit(buffer);
    }

    /*
     *  Make sure that this command is being invoked in the proper
     *  context.
     */
    if ((Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK)
        || !obj) {
        Tcl_AppendResult(interp, "improper usage: should be ",
            "\"object configure ?-option? ?value -option value...?\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    cdefn = obj->cdefn;   /* BE CAREFUL:  work in the virtual scope! */

    /*
     *  HANDLE:  configure
     */
    if (argc == 1) {
        Itcl_InitHierIter(&hier, cdefn);
        while ((cd=Itcl_AdvanceHierIter(&hier)) != NULL) {
            cdPtr = (ItclClass*)cd;
            entry = Tcl_FirstHashEntry(&cdPtr->variables, &place);
            while (entry) {
                vdefn = (ItclVarDefn*)Tcl_GetHashValue(entry);
                if (vdefn->protection == ITCL_PUBLIC) {
                    Tcl_AppendElement(interp,
                        ItclReportPublicOpt(interp, vdefn, obj));
                }
                entry = Tcl_NextHashEntry(&place);
            }
        }
        Itcl_DeleteHierIter(&hier);
        return TCL_OK;
    }

    /*
     *  HANDLE:  configure -option
     */
    else if (argc == 2) {
        if (*argv[1] != '-') {
            Tcl_AppendResult(interp, "improper usage: should be ",
                "\"object configure ?-option? ?value -option value...?\"",
                (char*)NULL);
            return TCL_ERROR;
        }
        if (Itcl_FindVarDefn(interp, cdefn, argv[1]+1, &vdefn) != TCL_OK) {
            return TCL_ERROR;
        }
        if (vdefn == NULL || vdefn->protection != ITCL_PUBLIC) {
            Tcl_AppendResult(interp, "unknown option \"", argv[1], "\"",
                (char*)NULL);
            return TCL_ERROR;
        }

        Tcl_SetResult(interp, ItclReportPublicOpt(interp, vdefn, obj),
            TCL_VOLATILE);
        return TCL_OK;
    }

    /*
     *  HANDLE:  configure -option value -option value...
     *
     *  Be careful to work in the virtual scope.  If this "configure"
     *  method was defined in a base class, the current namespace
     *  (from Itcl_ExecMethod()) will be that base class.  Activate
     *  the derived class namespace here, so that instance variables
     *  are accessed properly.
     */
    status = TCL_OK;

    vscope = Itcl_ActivateNamesp2(interp, cdefn->namesp,
        (ClientData)obj);

    for (i=1; i < argc; i+=2) {
        vdefn = NULL;
        if (*argv[i] == '-') {
            if (Itcl_FindVarDefn(interp, cdefn, argv[i]+1, &vdefn) != TCL_OK) {
                status = TCL_ERROR;
                break;
            }
        }
        if (vdefn == NULL || vdefn->protection != ITCL_PUBLIC) {
            Tcl_AppendResult(interp, "unknown option \"", argv[i], "\"",
                (char*)NULL);
            status = TCL_ERROR;
            break;
        }
        if (i == argc-1) {
            Tcl_AppendResult(interp, "value for \"", argv[i], "\" missing",
                (char*)NULL);
            status = TCL_ERROR;
            break;
        }

        lastval = Tcl_GetVar2(interp, vdefn->fullname,(char*)NULL, 0);
        Tcl_DStringTrunc(buffer, 0);
        Tcl_DStringAppend(buffer, (lastval) ? lastval : "", -1);

        if (Tcl_SetVar2(interp, vdefn->fullname,(char*)NULL, argv[i+1],
            TCL_LEAVE_ERR_MSG) == NULL) {
            char msg[256];
            sprintf(msg, "\n    (error in configuration of public variable \"%.100s\")", vdefn->fullname);
            Tcl_AddErrorInfo(interp, msg);
            status = TCL_ERROR;
            break;
        }

        /*
         *  If this variable has some "config" code, invoke it now.
         *
         *  NOTE:  Invoke the "config" code in the class scope
         *    containing the data member.
         */
        if (vdefn->config) {
            nsToken = Itcl_ActivateNamesp2(interp, vdefn->cdefn->namesp,
                (ClientData)obj);

            status = Itcl_EvalCmdImplement(interp, (ItclCmdMember*)NULL,
                vdefn->config, 0, (char**)NULL);

            Itcl_DeactivateNamesp(interp, nsToken);

            if (status == TCL_OK) {
                Tcl_ResetResult(interp);
            } else {
                char msg[256];
                sprintf(msg, "\n    (error in configuration of public variable \"%.100s\")", vdefn->fullname);
                Tcl_AddErrorInfo(interp, msg);
                Tcl_SetVar2(interp, vdefn->fullname,(char*)NULL,
                    Tcl_DStringValue(buffer), 0);
                break;
            }
        }
    }

    Itcl_DeactivateNamesp(interp, vscope);
    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_BiCgetCmd()
 *
 *  Invoked whenever the user issues the "cget" method for an object.
 *  Handles the following syntax:
 *
 *    <objName> cget -<option>
 *
 *  Allows access to public variables as if they were configuration
 *  options.  Mimics the behavior of the usual "cget" method for
 *  Tk widgets.  Returns the current value of the public variable
 *  with name <option>.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_BiCgetCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* class definition */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;

    char *val;
    ItclVarDefn *vdefn;

    /*
     *  Make sure that this command is being invoked in the proper
     *  context.
     */
    if ((Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK)
        || !obj || argc != 2 || *argv[1] != '-') {
        Tcl_AppendResult(interp, "improper usage: should be ",
            "\"object cget -option\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    cdefn = obj->cdefn;   /* BE CAREFUL:  work in the virtual scope! */

    if (Itcl_FindVarDefn(interp, cdefn, argv[1]+1, &vdefn) != TCL_OK) {
        return TCL_ERROR;
    }
    if (vdefn == NULL || vdefn->protection != ITCL_PUBLIC) {
        Tcl_AppendResult(interp, "unknown option \"", argv[1], "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    val = Itcl_GetInstanceVar(interp, vdefn->fullname, obj, obj->cdefn);
    Tcl_SetResult(interp, val, TCL_VOLATILE);

    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  ItclReportPublicOpt()
 *
 *  Returns a string characterizing a public variable as a configuration
 *  option:
 *
 *    -<varName> <initVal> <currentVal>
 *
 *  Used by Itcl_BiConfigureCmd() to report configuration options.
 *  Returns a string that remains valid until the next call to this
 *  procedure.
 * ------------------------------------------------------------------------
 */
static char*
ItclReportPublicOpt(interp,vdefn,obj)
    Tcl_Interp *interp;    /* interpreter containing the object */
    ItclVarDefn *vdefn;    /* public variable to be reported */
    Itcl_Object *obj;      /* object containing this variable */
{
    static Tcl_DString *buffer = NULL;

    ItclClass *cdefnPtr;
    Tcl_DString optName;
    Tcl_HashEntry *entry;
    ItclVarLookup *vlookup;
    char *val;

    /*
     *  If the result buffer has not yet been created, do it now.
     */
    if (buffer == NULL) {
        buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
        Tcl_DStringInit(buffer);
    }
    Tcl_DStringTrunc(buffer, 0);

    /*
     *  Determine how the option name should be reported.
     *  If the simple name can be used to find it in the virtual
     *  data table, then use the simple name.  Otherwise, this
     *  is a shadowed variable; use the full name.
     */
    Tcl_DStringInit(&optName);
    Tcl_DStringAppend(&optName, "-", -1);

    cdefnPtr = (ItclClass*)obj->cdefn;
    entry = Tcl_FindHashEntry(&cdefnPtr->virtualData, vdefn->name);
    assert(entry != NULL);

    vlookup = (ItclVarLookup*)Tcl_GetHashValue(entry);
    if (vlookup->vdefn == vdefn) {
        Tcl_DStringAppend(&optName, vdefn->name, -1);
    } else {
        Tcl_DStringAppend(&optName, vdefn->fullname, -1);
    }
    Tcl_DStringAppendElement(buffer, Tcl_DStringValue(&optName));
    Tcl_DStringFree(&optName);

    if (vdefn->init) {
        Tcl_DStringAppendElement(buffer, vdefn->init);
    } else {
        Tcl_DStringAppendElement(buffer, "<undefined>");
    }

    val = Itcl_GetInstanceVar(interp, vdefn->fullname, obj, obj->cdefn);
    if (val) {
        Tcl_DStringAppendElement(buffer, val);
    } else {
        Tcl_DStringAppendElement(buffer, "<undefined>");
    }

    return Tcl_DStringValue(buffer);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_BiInfoClassCmd()
 *
 *  Returns information regarding the class for an object.  This command
 *  can be invoked with or without an object context:
 *
 *    <objName> info class   <= returns most-specific class name
 *    info class             <= returns active namespace name
 *
 *  Returns a status TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_BiInfoClassCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    Itcl_Namespace activeNs = Itcl_GetCallingNamesp(interp,1);

    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;
    char *name;

    /*
     *  If this command is not invoked within a class namespace,
     *  signal an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        Tcl_AppendResult(interp, "\nget info like this instead: ",
            "\n  namespace className { info ", argv[0], "... }",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  If there is an object context, then return the most-specific
     *  class for the object.  Otherwise, return the class namespace
     *  name.  Use normal class names when possible.
     */
    if (obj) {
        classNs = obj->cdefn->namesp;
    }

    if (Itcl_GetNamespParent(classNs) == activeNs) {
        name = Itcl_GetNamespName(classNs);
    } else {
        name = Itcl_GetNamespPath(classNs);
    }

    Tcl_SetResult(interp, name, TCL_VOLATILE);
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_BiInfoInheritCmd()
 *
 *  Returns the list of base classes for the current class context.
 *  Returns a status TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_BiInfoInheritCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    Itcl_Namespace activeNs = Itcl_GetCallingNamesp(interp,1);

    Itcl_Namespace classNs;
    Itcl_Class *cdefn, *cd;
    Itcl_Object *obj;

    ItclClass *cdefnPtr;
    Itcl_ListElem *elem;
    char *name;

    /*
     *  If this command is not invoked within a class namespace,
     *  signal an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        Tcl_AppendResult(interp, "\nget info like this instead: ",
            "\n  namespace className { info ", argv[0], "... }",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Return the list of base classes.
     */
    cdefnPtr = (ItclClass*)cdefn;
    elem = Itcl_FirstListElem(&cdefnPtr->bases);
    while (elem) {
        cd = (Itcl_Class*)Itcl_GetListValue(elem);
        if (Itcl_GetNamespParent(cd->namesp) == activeNs) {
            name = Itcl_GetNamespName(cd->namesp);
        } else {
            name = Itcl_GetNamespPath(cd->namesp);
        }
        Tcl_AppendElement(interp, name);
        elem = Itcl_NextListElem(elem);
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_BiInfoHeritageCmd()
 *
 *  Returns the entire derivation hierarchy for this class, presented
 *  in the order that classes are traversed for finding data members
 *  and member functions.
 *
 *  Returns a status TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_BiInfoHeritageCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    Itcl_Namespace activeNs = Itcl_GetCallingNamesp(interp,1);

    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;
    Itcl_HierIter hier;
    char *name;

    /*
     *  If this command is not invoked within a class namespace,
     *  signal an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        Tcl_AppendResult(interp, "\nget info like this instead: ",
            "\n  namespace className { info ", argv[0], "... }",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Traverse through the derivation hierarchy and return
     *  base class names.
     */
    Itcl_InitHierIter(&hier,cdefn);
    while ((cdefn=Itcl_AdvanceHierIter(&hier)) != NULL) {
        if (Itcl_GetNamespParent(cdefn->namesp) == activeNs) {
            name = Itcl_GetNamespName(cdefn->namesp);
        } else {
            name = Itcl_GetNamespPath(cdefn->namesp);
        }
        Tcl_AppendElement(interp, name);
    }
    Itcl_DeleteHierIter(&hier);
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_BiInfoFunctionCmd()
 *
 *  Returns information regarding class member functions (methods/procs).
 *  Handles the following syntax:
 *
 *    info function ?cmdName? ?-protection? ?-type? ?-name? ?-args? ?-body?
 *
 *  If the ?cmdName? is not specified, then a list of all known
 *  command members is returned.  Otherwise, the information for
 *  a specific command is returned.  Returns a status TCL_OK/TCL_ERROR
 *  to indicate success/failure.
 * ------------------------------------------------------------------------
 */
static char* DefInfoFunction[5] = {
    "-protection",
    "-type",
    "-name",
    "-args",
    "-body",
};

/* ARGSUSED */
int
Itcl_BiInfoFunctionCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    char *cmdName = NULL;

    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;

    Tcl_HashSearch place;
    Tcl_HashEntry *entry;
    ItclCmdMember *mdefn;
    ItclCmdImplement *mimpl;
    Command *cmdPtr;
    Namespace *nsPtr;
    Itcl_HierIter hier;
    int oneElement;
    char *val;

    /*
     *  If this command is not invoked within a class namespace,
     *  signal an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        Tcl_AppendResult(interp, "\nget info like this instead: ",
            "\n  namespace className { info ", argv[0], "... }",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Process args:
     *  ?cmdName? ?-protection? ?-type? ?-name? ?-args? ?-body?
     */
    argv++;  /* skip over command name */
    argc--;

    if (argc > 0) {
        cmdName = *argv;
        argc--; argv++;
    }

    /*
     *  Return info for a specific command.
     */
    if (cmdName) {
        if (Itcl_FindCmdDefn(interp, cdefn, cmdName, &mdefn) != TCL_OK) {
            return TCL_ERROR;
        }
        if (!mdefn) {
            Tcl_AppendResult(interp, "\"", cmdName,
                "\" isn't a member function in class \"",
                Itcl_GetNamespPath(cdefn->namesp), "\"",
                (char*)NULL);
            return TCL_ERROR;
        }
        mimpl = mdefn->implement;

        /*
         *  By default, return everything.
         */
        if (argc == 0) {
            argc = 5;
            argv = DefInfoFunction;
        }
        oneElement = (argc == 1);

        /*
         *  Scan through all remaining flags and return the
         *  requested info.
         */
        for ( ; argc > 0; argc--, argv++) {
            if (strcmp(*argv,"-args") == 0) {
                if (mimpl->arglist != NULL) {
                    val = Itcl_ArgList(mimpl->arglist);
                }
                else if ((mdefn->flags & ITCL_ARG_SPEC) != 0) {
                    val = Itcl_ArgList(mdefn->arglist);
                }
                else {
                    val = "<undefined>";
                }
            }
            else if (strcmp(*argv,"-body") == 0) {
                if (mimpl->body) {
                    val = mimpl->body;
                } else {
                    val = "<undefined>";
                }
            }
            else if (strcmp(*argv,"-name") == 0) {
                val = mdefn->fullname;
            }
            else if (strcmp(*argv,"-protection") == 0) {
                val = Itcl_ProtectionStr(mdefn->protection);
            }
            else if (strcmp(*argv,"-type") == 0) {
                val = ((mdefn->flags & ITCL_COMMON) != 0)
                    ? "proc" : "method";
            }
            else {
                Tcl_AppendResult(interp, "bad option \"", *argv,
                    "\": should be -args, -body, -name, -protection ",
                    "or -type",
                    (char*)NULL);
                return TCL_ERROR;
            }

            if (oneElement) {
                Tcl_AppendResult(interp, val, (char*)NULL);
            } else {
                Tcl_AppendElement(interp, val);
            }
        }
    }

    /*
     *  Return the list of available commands.
     */
    else {
        Itcl_InitHierIter(&hier, cdefn);
        while ((cdefn=Itcl_AdvanceHierIter(&hier)) != NULL) {
            nsPtr = (Namespace*)cdefn->namesp;
            entry = Tcl_FirstHashEntry(&nsPtr->commands, &place);
            while (entry) {
                cmdPtr = (Command*)Tcl_GetHashValue(entry);
                if (cmdPtr->proc == Itcl_ExecMethod ||
                    cmdPtr->proc == Itcl_ExecProc) {
                    mdefn = (ItclCmdMember*)cmdPtr->clientData;
                    Tcl_AppendElement(interp, mdefn->fullname);
                }
                entry = Tcl_NextHashEntry(&place);
            }
        }
        Itcl_DeleteHierIter(&hier);
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_BiInfoVariableCmd()
 *
 *  Returns information regarding class data members (variables and
 *  commons).  Handles the following syntax:
 *
 *    info variable ?varName? ?-protection? ?-type? ?-name?
 *        ?-init? ?-config? ?-value?
 *
 *  If the ?varName? is not specified, then a list of all known
 *  data members is returned.  Otherwise, the information for a
 *  specific member is returned.  Returns a status TCL_OK/TCL_ERROR
 *  to indicate success/failure.
 * ------------------------------------------------------------------------
 */
static char* DefInfoVariable[5] = {
    "-protection",
    "-type",
    "-name",
    "-init",
    "-value",
};
static char* DefInfoPubVariable[6] = {
    "-protection",
    "-type",
    "-name",
    "-init",
    "-config",
    "-value",
};

/* ARGSUSED */
int
Itcl_BiInfoVariableCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    char *varName = NULL;

    Itcl_Namespace classNs;
    Itcl_Class *cdefn, *cd;
    Itcl_Object *obj;

    ItclClass *cdPtr;
    Tcl_HashSearch place;
    Tcl_HashEntry *entry;
    ItclVarDefn *vdefn;
    Itcl_HierIter hier;
    char *val;
    int oneElement;

    /*
     *  If this command is not invoked within a class namespace,
     *  signal an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        Tcl_AppendResult(interp, "\nget info like this instead: ",
            "\n  namespace className { info ", argv[0], "... }",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Process args:
     *  ?varName? ?-protection? ?-type? ?-name? ?-init? ?-config? ?-value?
     */
    argv++;  /* skip over command name */
    argc--;

    if (argc > 0) {
        varName = *argv;
        argc--; argv++;
    }

    /*
     *  Return info for a specific command.
     */
    if (varName) {
        if (Itcl_FindVarDefn(interp, cdefn, varName,&vdefn) != TCL_OK) {
            return TCL_ERROR;
        }
        if (vdefn) {
            /*
             *  By default, return everything.
             */
            if (argc == 0) {
                if (vdefn->protection == ITCL_PUBLIC &&
                    ((vdefn->flags & ITCL_COMMON) == 0)) {
                    argv = DefInfoPubVariable;
                    argc = 6;
                } else {
                    argv = DefInfoVariable;
                    argc = 5;
                }
            }
            oneElement = (argc == 1);

            /*
             *  Scan through all remaining flags and return the
             *  requested info.
             */
            for ( ; argc > 0; argc--, argv++) {
                if (strcmp(*argv,"-config") == 0) {
                    val = (vdefn->config) ? vdefn->config->body : "";
                }
                else if (strcmp(*argv,"-init") == 0) {
                    /*
                     *  If this is the built-in "this" variable, then
                     *  report the object name as its initialization string.
                     */
                    if ((vdefn->flags & ITCL_THIS_VAR) != 0) {
                        if (obj && obj->accessCmd) {
                            val = Tcl_GetCommandPath(interp, obj->accessCmd);
                        } else {
                            val = "<objectName>";
                        }
                    }
                    else if (vdefn->init) {
                        val = vdefn->init;
                    }
                    else {
                        val = "<undefined>";
                    }
                }
                else if (strcmp(*argv,"-name") == 0) {
                    val = vdefn->fullname;
                }
                else if (strcmp(*argv,"-protection") == 0) {
                    val = Itcl_ProtectionStr(vdefn->protection);
                }
                else if (strcmp(*argv,"-type") == 0) {
                    val = ((vdefn->flags & ITCL_COMMON) != 0)
                        ? "common" : "variable";
                }
                else if (strcmp(*argv,"-value") == 0) {
                    if ((vdefn->flags & ITCL_COMMON) != 0) {
                        val = Tcl_GetVar(interp, vdefn->fullname, 0);
                    }
                    else if (obj == NULL) {
                        Tcl_AppendResult(interp,
                            "cannot access object-specific info ",
                            "without an object context",
                            (char*)NULL);
                        return TCL_ERROR;
                    }
                    else {
                        val = Itcl_GetInstanceVar(interp, vdefn->fullname,
                            obj, obj->cdefn);
                    }

                    if (val == NULL) {
                        val = "<undefined>";
                    }
                }
                else {
                    Tcl_AppendResult(interp, "bad option \"", *argv,
                        "\": should be -config, -init, -name, -protection, ",
                        "-type or -value",
                        (char*)NULL);
                    return TCL_ERROR;
                }

                if (oneElement) {
                    Tcl_AppendResult(interp, val, (char*)NULL);
                } else {
                    Tcl_AppendElement(interp, val);
                }
            }
        }
    }

    /*
     *  Return the list of available variables.  Report the built-in
     *  "this" variable only once, for the most-specific class.
     */
    else {
        Itcl_InitHierIter(&hier, cdefn);
        while ((cd=Itcl_AdvanceHierIter(&hier)) != NULL) {
            cdPtr = (ItclClass*)cd;
            entry = Tcl_FirstHashEntry(&cdPtr->variables, &place);
            while (entry) {
                vdefn = (ItclVarDefn*)Tcl_GetHashValue(entry);
                if ((vdefn->flags & ITCL_THIS_VAR) != 0) {
                    if (cd == cdefn) {
                        Tcl_AppendElement(interp, vdefn->fullname);
                    }
                }
                else {
                    Tcl_AppendElement(interp, vdefn->fullname);
                }
                entry = Tcl_NextHashEntry(&place);
            }
        }
        Itcl_DeleteHierIter(&hier);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_BiInfoBodyCmd()
 *
 *  Handles the usual "info body" request, returning the body for a
 *  specific proc.  Included here for backward compatibility, since
 *  otherwise Tcl would complain that class procs are not real "procs".
 *  Returns a status TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_BiInfoBodyCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;
    ItclCmdMember *mdefn;
    char *val;

    /*
     *  If this command is not invoked within a class namespace,
     *  then use the normal implementation for "info body".
     *  Note that this procedure will always be called from the
     *  ItclHandleEnsemble() procedure, which passes in (argc-1,argv+1).
     *  Because of this, it is safe to simply decrement the pointer below:
     */
    if (!Itcl_IsClass(Itcl_GetActiveNamesp(interp))) {
        return Tcl_InfoCmd((ClientData)NULL, interp, argc+1, argv-1);
    }

    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        Tcl_AppendResult(interp, "\nget info like this instead: ",
            "\n  namespace className { info ", argv[0], "... }",
            (char*)NULL);
        return TCL_ERROR;
    }
    if (Itcl_FindCmdDefn(interp, cdefn, argv[1], &mdefn) != TCL_OK) {
        return TCL_ERROR;
    }
    if (!mdefn) {
        Tcl_AppendResult(interp, "\"", argv[1], "\" isn't a procedure",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Return a string describing the implementation.
     */
    if (mdefn->implement->body) {
        val = mdefn->implement->body;
    } else {
        val = "<undefined>";
    }
    Tcl_SetResult(interp, val, TCL_VOLATILE);
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_BiInfoArgsCmd()
 *
 *  Handles the usual "info args" request, returning the argument list
 *  for a specific proc.  Included here for backward compatibility, since
 *  otherwise Tcl would complain that class procs are not real "procs".
 *  Returns a status TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_BiInfoArgsCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;
    ItclCmdMember *mdefn;
    char *val;

    /*
     *  If this command is not invoked within a class namespace,
     *  then use the normal implementation for "info body".
     *  Note that this procedure will always be called from the
     *  ItclHandleEnsemble() procedure, which passes in (argc-1,argv+1).
     *  Because of this, it is safe to simply decrement the pointer below:
     */
    if (!Itcl_IsClass(Itcl_GetActiveNamesp(interp))) {
        return Tcl_InfoCmd((ClientData)NULL, interp, argc+1, argv-1);
    }

    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        Tcl_AppendResult(interp, "\nget info like this instead: ",
            "\n  namespace className { info ", argv[0], "... }",
            (char*)NULL);
        return TCL_ERROR;
    }
    if (Itcl_FindCmdDefn(interp, cdefn, argv[1], &mdefn) != TCL_OK) {
        return TCL_ERROR;
    }
    if (!mdefn) {
        Tcl_AppendResult(interp, "\"", argv[1], "\" isn't a procedure",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Return a string describing the implementation.
     */
    if (mdefn->implement->arglist != NULL) {
        val = Itcl_ArgList(mdefn->implement->arglist);
    }
    else if ((mdefn->flags & ITCL_ARG_SPEC) != 0) {
        val = Itcl_ArgList(mdefn->arglist);
    }
    else {
        val = "<undefined>";
    }
    Tcl_SetResult(interp, val, TCL_VOLATILE);
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_DefaultInfoCmd()
 *
 *  Handles any unknown options for the "itcl::builtin::info" command
 *  by passing requests on to the usual "::info" command.  If the
 *  option is recognized, then it is handled.  Otherwise, if it is
 *  still unknown, then an error message is returned with the list
 *  of possible options.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_DefaultInfoCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    int status;
    Tcl_Command cmd;
    Command *cmdPtr;

    /*
     *  Look for the usual "::info" command, and use it to
     *  evaluate the unknown option.
     */
    if (Itcl_FindCommand(interp, "::info", 0, &cmd) != TCL_OK || cmd == NULL) {
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, "bad option \"", argv[0],
            "\" should be one of...\n", (char*)NULL);
        Tcl_AppendResult(interp, Itcl_EnsembleUsage(), (char*)NULL);
        return TCL_ERROR;
    }
    cmdPtr = (Command*)cmd;

    status = (*cmdPtr->proc)(cmdPtr->clientData, interp, argc, argv);

    /*
     *  If the option was not recognized by the usual "info" command,
     *  then we got a "bad option" error message.  Add the options
     *  for the current ensemble to the error message.
     */
    if (status != TCL_OK && strncmp(interp->result,"bad option",10) == 0) {
        Tcl_AppendResult(interp, "\nor", Itcl_EnsembleUsage(), (char*)NULL);
    }
    return status;
}
