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
 *  Procedures in this file support the old-style syntax for [incr Tcl]
 *  class definitions:
 *
 *    itcl_class <className> {
 *        inherit <base-class>...
 *
 *        constructor {<arglist>} { <body> }
 *        destructor { <body> }
 *
 *        method <name> {<arglist>} { <body> }
 *        proc <name> {<arglist>} { <body> }
 *
 *        public <varname> ?<init>? ?<config>?
 *        protected <varname> ?<init>?
 *        common <varname> ?<init>?
 *    }
 *
 *  This capability will be removed in a future release, after users
 *  have had a chance to switch over to the new syntax.
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl_obsolete.c,v 1.1.1.1 1996/08/21 19:29:43 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include "itclInt.h"

static char rcsid[] = "$Id: itcl_obsolete.c,v 1.1.1.1 1996/08/21 19:29:43 heyes Exp $";

/*
 *  FORWARD DECLARATIONS
 */
static int Itcl_OldClassCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));

static int Itcl_OldMethodCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
static int Itcl_OldPublicCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
static int Itcl_OldProtectedCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
static int Itcl_OldCommonCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));

static int Itcl_OldItclInfoCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));

static int Itcl_OldBiDeleteCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
static int Itcl_OldBiVirtualCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
static int Itcl_OldBiPreviousCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));

static int Itcl_OldBiInfoMethodsCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
static int Itcl_OldBiInfoProcsCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
static int Itcl_OldBiInfoPublicsCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
static int Itcl_OldBiInfoProtectedsCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
static int Itcl_OldBiInfoCommonsCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));


/*
 *  Standard list of built-in methods for old-style objects.
 */
typedef struct BiMethod {
    char* name;            /* method name */
    char* usage;           /* string describing usage */
    char* registration;    /* registration name for C proc */
    Tcl_CmdProc *proc;     /* implementation C proc */
} BiMethod;

static BiMethod BiMethodList[] = {
    { "cget",      "-option",
                   "@itcl-oldstyle-cget",  Itcl_BiCgetCmd },
    { "configure", "?-option? ?value -option value...?",
                   "@itcl-oldstyle-configure",  Itcl_BiConfigureCmd },
    { "delete",    "",
                   "@itcl-oldstyle-delete",  Itcl_OldBiDeleteCmd },
    { "isa",       "className",
                   "@itcl-oldstyle-isa",  Itcl_BiIsaCmd },
};
static int BiMethodListLen = sizeof(BiMethodList)/sizeof(BiMethod);


/*
 * ------------------------------------------------------------------------
 *  Itcl_OldInit()
 *
 *  Invoked by Itcl_Init() whenever a new interpeter is created to add
 *  [incr Tcl] facilities.  Adds the commands needed for backward
 *  compatibility with previous releases of [incr Tcl].
 * ------------------------------------------------------------------------
 */
int
Itcl_OldInit(interp,info)
    Tcl_Interp *interp;     /* interpreter to be updated */
    ItclObjectInfo *info;   /* info regarding all known objects */
{
    int i;
    Itcl_Namespace parserNs, oldBiNs, ns;

    /*
     *  Declare all of the old-style built-in methods as C procedures.
     */
    for (i=0; i < BiMethodListLen; i++) {
        if (Itcl_RegisterC(interp, BiMethodList[i].registration+1,
            BiMethodList[i].proc) != TCL_OK) {
            return TCL_ERROR;
        }
    }

    /*
     *  Create the "itcl::old-parser" namespace for backward
     *  compatibility, to handle the old-style class definitions.
     */
    if (Itcl_CreateNamesp(interp, "::itcl::old-parser",
        Itcl_PreserveData((ClientData)info), Itcl_ReleaseData,
        &parserNs) != TCL_OK) {
        Tcl_AppendResult(interp, " (cannot initialize itcl old-style parser)",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Add commands for parsing old-style class definitions.
     */
    Tcl_CreateCommand(interp, "::itcl::old-parser::inherit",
        Itcl_ClassInheritCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::old-parser::constructor",
        Itcl_ClassConstructorCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::old-parser::destructor",
        Itcl_ClassDestructorCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::old-parser::method",
        Itcl_OldMethodCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::old-parser::proc",
        Itcl_ClassProcCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::old-parser::public",
        Itcl_OldPublicCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::old-parser::protected",
        Itcl_OldProtectedCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::old-parser::common",
        Itcl_OldCommonCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::old-parser::set",
        Tcl_SetCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    /*
     *  Clear the import list for the parser namespace, so that
     *  only the parser commands are allowed in the context of a
     *  class definition.  Set a "variable enforcer" procedure
     *  to control access to "common" data members.
     */
    Itcl_ClearImportNamesp(parserNs);
    Itcl_SetVarEnforcer(parserNs, Itcl_ParseVarEnforcer);

    /*
     *  Create the "itcl::old-builtin" namespace for backward
     *  compatibility with the old-style built-in commands.
     *  Make this import from the newer "itcl::builtin::" namespace,
     *  but nothing else.  This is simply an extra bag of commands
     *  imported into each old-style class.
     */
    if (Itcl_CreateNamesp(interp, "::itcl::old-builtin",
        (ClientData)NULL, (Itcl_DeleteProc*)NULL, &oldBiNs) != TCL_OK) {
        Tcl_AppendResult(interp, " (cannot initialize itcl built-in commands)",
            (char*)NULL);
        return TCL_ERROR;
    }

    Itcl_ClearImportNamesp(oldBiNs);
    if (Itcl_FindNamesp(interp,"::itcl::builtin", 0, &ns) == TCL_OK) {
        Itcl_AppendImportNamesp(oldBiNs, ns, ITCL_PUBLIC);
    }

    Tcl_CreateCommand(interp, "::itcl::old-builtin::virtual",
        Itcl_OldBiVirtualCmd, (ClientData)NULL, (Itcl_DeleteProc*)NULL);
    Tcl_CreateCommand(interp, "::itcl::old-builtin::previous",
        Itcl_OldBiPreviousCmd, (ClientData)NULL, (Itcl_DeleteProc*)NULL);

    if (Itcl_CreateEnsemble(interp, "::itcl::old-builtin::info") != TCL_OK) {
        return TCL_ERROR;
    }

    if (Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "class",
            0, 0, (char*)NULL,
            Itcl_BiInfoClassCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "inherit",
            0, 0, (char*)NULL,
            Itcl_BiInfoInheritCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "heritage",
            0, 0, (char*)NULL,
            Itcl_BiInfoHeritageCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "method",
            0, ITCL_VAR_ARGS, "?methodName? ?-args? ?-body?",
            Itcl_OldBiInfoMethodsCmd,
            (ClientData)NULL,(Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "proc",
            0, ITCL_VAR_ARGS, "?procName? ?-args? ?-body?",
            Itcl_OldBiInfoProcsCmd,
            (ClientData)NULL,(Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "public",
            0, ITCL_VAR_ARGS, "?varName? ?-init? ?-value? ?-config?",
            Itcl_OldBiInfoPublicsCmd,
            (ClientData)NULL,(Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "protected",
            0, ITCL_VAR_ARGS, "?varName? ?-init? ?-value?",
            Itcl_OldBiInfoProtectedsCmd,
            (ClientData)NULL,(Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "common",
            0, ITCL_VAR_ARGS, "?varName? ?-init? ?-value?",
            Itcl_OldBiInfoCommonsCmd,
            (ClientData)NULL,(Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "args",
            1, 1, "procname",
            Itcl_BiInfoArgsCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "body",
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
    if (Itcl_AddEnsembleOption(interp, "::itcl::old-builtin::info", "@error",
            0, ITCL_VAR_ARGS, (char*)NULL,
            Itcl_DefaultInfoCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL)
            != TCL_OK
    ) {
        return TCL_ERROR;
    }

    /*
     *  Install the "itcl_class" and "itcl_info" commands into
     *  the global scope.  This supports the old syntax for
     *  backward compatibility.
     */
    Tcl_CreateCommand(interp, "::itcl_class", Itcl_OldClassCmd,
        Itcl_PreserveData((ClientData)info), Itcl_ReleaseData);

    Tcl_CreateCommand(interp, "::itcl_info", Itcl_OldItclInfoCmd,
        Itcl_PreserveData((ClientData)info), Itcl_ReleaseData);

    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InstallOldBiMethods()
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
Itcl_InstallOldBiMethods(interp,cdefn)
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
 *  Itcl_OldClassCmd()
 *
 *  Invoked by Tcl whenever the user issues a "itcl_class" command to
 *  specify a class definition.  Handles the following syntax:
 *
 *    itcl_class <className> {
 *        inherit <base-class>...
 *
 *        constructor {<arglist>} { <body> }
 *        destructor { <body> }
 *
 *        method <name> {<arglist>} { <body> }
 *        proc <name> {<arglist>} { <body> }
 *
 *        public <varname> ?<init>? ?<config>?
 *        protected <varname> ?<init>?
 *        common <varname> ?<init>?
 *    }
 *
 *  NOTE:  This command is will only be provided for a limited time,
 *         to support backward compatibility with the old-style
 *         [incr Tcl] syntax.  Users should convert their scripts
 *         to use the newer syntax (Itcl_ClassCmd()) as soon as possible.
 *
 * ------------------------------------------------------------------------
 */
static int
Itcl_OldClassCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo* info = (ItclObjectInfo*)clientData;

    int status, cmdProt, varProt;
    Itcl_Namespace classNs, parserNs, itclBiNs;
    ItclClass *cdefnPtr, *basePtr;
    Itcl_ListElem *elem;
    Tcl_HashEntry* entry;
    ItclCmdMember *mdefn;
    Itcl_ActiveNamespace nsToken;
    Command *cmdPtr;
    Namespace *nsPtr;

    if (argc != 3) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name { definition }\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Find the namespace to use as a parser for the class definition.
     *  If for some reason it is destroyed, bail out here.
     */
    if (Itcl_FindNamesp(interp, "::itcl::old-parser", 0, &parserNs) != TCL_OK) {
        return TCL_ERROR;
    }
    if (parserNs == NULL) {
        Tcl_AppendResult(interp,
            "cannot parse class definition (internal error)",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Try to create the specified class and its namespace.
     */
    if (Itcl_CreateClass(interp, argv[1], info, &classNs) != TCL_OK) {
        return TCL_ERROR;
    }
    cdefnPtr = (ItclClass*)Itcl_GetNamespData(classNs);
    cdefnPtr->flags |= ITCL_OLD_STYLE;

    /*
     *  Push this class onto the class definition stack so that it
     *  becomes the current context for all commands in the parser.
     *  Activate the parser and evaluate the class definition.
     *  By default, make class data members "protected" and
     *  member functions "public".
     */
    Itcl_PushStack((ClientData)cdefnPtr, &info->cdefnStack);
    nsToken = Itcl_ActivateNamesp(interp, parserNs);

    cmdProt = Itcl_CmdProtection(interp, ITCL_PUBLIC);
    varProt = Itcl_VarProtection(interp, ITCL_PROTECTED);

    status = Tcl_Eval(interp, argv[2]);

    Itcl_CmdProtection(interp, cmdProt);
    Itcl_VarProtection(interp, varProt);

    Itcl_DeactivateNamesp(interp, nsToken);
    Itcl_PopStack(&info->cdefnStack);

    if (status != TCL_OK) {
        char msg[256];
        sprintf(msg, "\n    (class \"%.200s\" body line %d)",
            argv[1], interp->errorLine);
        Tcl_AddErrorInfo(interp, msg);

        Itcl_DeleteNamesp(classNs);
        return TCL_ERROR;
    }

    /*
     *  At this point, parsing of the class definition has succeeded.
     *  Add built-in methods which do not conflict with those defined
     *  in the class.
     */
    if (Itcl_InstallOldBiMethods(interp, (Itcl_Class*)cdefnPtr) != TCL_OK) {
        Itcl_DeleteNamesp(classNs);
        return TCL_ERROR;
    }

    /*
     *  Set up the import list to bring in methods/procs from base
     *  classes.  Add itcl::old-builtin for built-in commands, and the
     *  parent namespace as a last resort.
     */
    Itcl_ClearImportNamesp(cdefnPtr->namesp);

    elem = Itcl_FirstListElem(&cdefnPtr->bases);
    while (elem) {
        basePtr = (ItclClass*)Itcl_GetListValue(elem);

        Itcl_AppendImportNamesp(cdefnPtr->namesp, basePtr->namesp,
            ITCL_PROTECTED);
        Itcl_AppendList(&basePtr->derived,
            Itcl_PreserveData((ClientData)cdefnPtr));

        elem = Itcl_NextListElem(elem);
    }

    if (Itcl_FindNamesp(interp,"::itcl::old-builtin",0,&itclBiNs) == TCL_OK &&
        itclBiNs != NULL) {
        Itcl_AppendImportNamesp(cdefnPtr->namesp, itclBiNs, ITCL_PUBLIC);
    }

    Itcl_AppendImportNamesp(cdefnPtr->namesp,
        Itcl_GetNamespParent(cdefnPtr->namesp), ITCL_PUBLIC);

    /*
     *  See if this class has a "constructor", and if it does, mark
     *  it as "old-style".  This will allow the "config" argument
     *  to work.
     */
    nsPtr = (Namespace*)cdefnPtr->namesp;
    entry = Tcl_FindHashEntry(&nsPtr->commands, "constructor");
    if (entry) {
        cmdPtr = (Command*)Tcl_GetHashValue(entry);
        if (cmdPtr->proc == Itcl_ExecMethod) {
            mdefn = (ItclCmdMember*)cmdPtr->clientData;
            mdefn->flags |= ITCL_OLD_STYLE;
        }
    }

    /*
     *  Build the virtual tables for this class.
     */
    Itcl_BuildVirtualTables(cdefnPtr);

    Tcl_SetResult(interp, "", TCL_STATIC);
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_OldItclInfoCmd()
 *
 *  Invoked by Tcl whenever the user issues an "itcl_info" command to
 *  query high-level class/object info.  Handles the following syntax:
 *
 *    itcl_info classes ?<pattern>?
 *    itcl_info objects ?<pattern>? ?-class <className>? ?-isa <className>?
 *
 *  NOTE:  This command is will only be provided for a limited time,
 *         to support backward compatibility with the old-style
 *         [incr Tcl] syntax.  Users should convert their scripts
 *         to use the newer syntax as soon as possible.
 *
 * ------------------------------------------------------------------------
 */
static int
Itcl_OldItclInfoCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    char c, *argv0, *cmdStr;
    int length, status;

    if (argc < 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " option ?args...?\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);

    /*
     *  HANDLE:  itcl_info classes ?<pattern>?
     */
    if ((c == 'c') && strncmp(argv[1], "classes", length) == 0) {
        argv0 = argv[0];
        argv[0] = "::info";
        cmdStr = Tcl_Merge(argc,argv);

        status = Itcl_EvalArgs(interp, cmdStr, (char*)NULL, argc, argv);

        ckfree(cmdStr);
        argv[0] = argv0;

        return status;
    }

    /*
     *  HANDLE:  itcl_info objects ?<pattern>? ?-class <className>?
     *               ?-isa <className>?
     */
    else if ((c == 'o') && strncmp(argv[1], "objects", length) == 0) {
        argv0 = argv[0];
        argv[0] = "::info";
        cmdStr = Tcl_Merge(argc,argv);

        status = Itcl_EvalArgs(interp, cmdStr, (char*)NULL, argc, argv);

        ckfree(cmdStr);
        argv[0] = argv0;

        return status;
    }

    /*
     *  Flag improper usage.
     */
    Tcl_AppendResult(interp, "bad option \"", argv[1],
        "\": must be classes or objects",
        (char*)NULL);
    return TCL_ERROR;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_OldMethodCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "method" command is invoked to define an object method.
 *  Handles the following syntax:
 *
 *      method <name> {<arglist>} {<body>}
 *
 * ------------------------------------------------------------------------
 */
static int
Itcl_OldMethodCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    Itcl_Class *cdefn = (Itcl_Class*)Itcl_PeekStack(&info->cdefnStack);
    Namespace *nsPtr = (Namespace*)cdefn->namesp;

    Tcl_HashEntry *entry;
    ItclCmdMember *mdefn;
    Command *cmdPtr;

    if (argc != 4) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name args body\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    if (Tcl_FindHashEntry(&nsPtr->commands, argv[1])) {
        Tcl_AppendResult(interp, "\"", argv[1],
            "\" already defined in class \"", cdefn->name, "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    if (Itcl_CreateMethod(interp,cdefn,argv[1],argv[2],argv[3]) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  Find the method that was just created and mark it as an
     *  "old-style" method, so that the magic "config" argument
     *  will be allowed to work.  This is done for backward-
     *  compatibility with earlier releases.  In the latest version,
     *  use of the "config" argument is discouraged.
     */
    entry = Tcl_FindHashEntry(&nsPtr->commands, argv[1]);
    cmdPtr = (Command*)Tcl_GetHashValue(entry);
    if (cmdPtr->proc == Itcl_ExecMethod) {
        mdefn = (ItclCmdMember*)cmdPtr->clientData;
        mdefn->flags |= ITCL_OLD_STYLE;
    }

    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_OldPublicCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "public" command is invoked to define a public variable.
 *  Handles the following syntax:
 *
 *      public <varname> ?<init>? ?<config>?
 *
 * ------------------------------------------------------------------------
 */
static int
Itcl_OldPublicCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    ItclClass *cdefnPtr = (ItclClass*)Itcl_PeekStack(&info->cdefnStack);

    int newEntry;
    Tcl_HashEntry *entry;
    ItclVarDefn *vdefn;
    char *init, *config;

    if ((argc < 2) || (argc > 4)) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " varname ?init? ?config?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Make sure that the variable name does not contain anything
     *  goofy like a "::" scope qualifier.
     */
    if (strstr(argv[1],"::")) {
        Tcl_AppendResult(interp, "bad variable name \"", argv[1], "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Make sure that the variable name does not already exist.
     */
    entry = Tcl_CreateHashEntry(&cdefnPtr->variables, argv[1], &newEntry);
    if (!newEntry) {
        Tcl_AppendResult(interp, "variable name \"",
            argv[1], "\" already defined in class \"", cdefnPtr->name, "\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    init   = (argc >= 3) ? argv[2] : NULL;
    config = (argc >= 4) ? argv[3] : NULL;

    if (Itcl_CreateVarDefn(interp, (Itcl_Class*)cdefnPtr, argv[1],
        init, config, &vdefn) != TCL_OK) {
        Tcl_DeleteHashEntry(entry);
        return TCL_ERROR;
    }

    vdefn->protection = ITCL_PUBLIC;
    Tcl_SetHashValue(entry, (ClientData)vdefn);

    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_OldProtectedCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "protected" command is invoked to define a protected variable.
 *  Handles the following syntax:
 *
 *      protected <varname> ?<init>?
 *
 * ------------------------------------------------------------------------
 */
static int
Itcl_OldProtectedCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    ItclClass *cdefnPtr = (ItclClass*)Itcl_PeekStack(&info->cdefnStack);

    int newEntry;
    Tcl_HashEntry *entry;
    ItclVarDefn *vdefn;
    char *init;

    if ((argc < 2) || (argc > 3)) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " varname ?init?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Make sure that the variable name does not contain anything
     *  goofy like a "::" scope qualifier.
     */
    if (strstr(argv[1],"::")) {
        Tcl_AppendResult(interp, "bad variable name \"", argv[1], "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Make sure that the variable name does not already exist.
     */
    entry = Tcl_CreateHashEntry(&cdefnPtr->variables, argv[1], &newEntry);
    if (!newEntry) {
        Tcl_AppendResult(interp, "variable name \"",
            argv[1], "\" already defined in class \"", cdefnPtr->name, "\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    init = (argc >= 3) ? argv[2] : NULL;

    if (Itcl_CreateVarDefn(interp, (Itcl_Class*)cdefnPtr, argv[1],
        init, (char*)NULL, &vdefn) != TCL_OK) {
        Tcl_DeleteHashEntry(entry);
        return TCL_ERROR;
    }

    vdefn->protection = ITCL_PROTECTED;
    Tcl_SetHashValue(entry, (ClientData)vdefn);

    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_OldCommonCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "common" command is invoked to define a variable that is
 *  common to all objects in the class.  Handles the following syntax:
 *
 *      common <varname> ?<init>?
 *
 * ------------------------------------------------------------------------
 */
static int
Itcl_OldCommonCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    ItclClass *cdefnPtr = (ItclClass*)Itcl_PeekStack(&info->cdefnStack);

    int newEntry;
    Tcl_HashEntry *entry;
    Tcl_HashEntry *hPtr;
    ItclVarDefn *vdefn;
    char *init;
    Itcl_ActiveNamespace nsToken;
    Namespace *nsPtr;
    Var *varPtr;

    if ((argc < 2) || (argc > 3)) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " varname ?init?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Make sure that the variable name does not contain anything
     *  goofy like a "::" scope qualifier.
     */
    if (strstr(argv[1],"::")) {
        Tcl_AppendResult(interp, "bad variable name \"", argv[1], "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Make sure that the variable name does not already exist.
     */
    entry = Tcl_CreateHashEntry(&cdefnPtr->variables, argv[1], &newEntry);
    if (!newEntry) {
        Tcl_AppendResult(interp, "variable name \"",
            argv[1], "\" already defined in class \"", cdefnPtr->name, "\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    init = (argc >= 3) ? argv[2] : NULL;

    if (Itcl_CreateVarDefn(interp, (Itcl_Class*)cdefnPtr, argv[1],
        init, (char*)NULL, &vdefn) != TCL_OK) {
        Tcl_DeleteHashEntry(entry);
        return TCL_ERROR;
    }

    vdefn->protection = ITCL_PROTECTED;
    vdefn->flags |= ITCL_COMMON;
    Tcl_SetHashValue(entry, (ClientData)vdefn);

    /*
     *  Create the variable in the namespace associated with the
     *  class, so it will be immediately available for initialization.
     *
     *  Note that we cannot use Tcl_SetVar() for this, since we want
     *  to create a variable with an "undefined" value.
     */
    nsPtr  = (Namespace*)cdefnPtr->namesp;
    hPtr   = Tcl_CreateHashEntry(&nsPtr->variables, vdefn->name, &newEntry);
    varPtr = TclNewVar(interp);
    Tcl_SetHashValue(hPtr, (ClientData)varPtr);

    varPtr->name = Tcl_GetHashKey(&nsPtr->variables, hPtr);
    varPtr->namesp  = (Namespace*)cdefnPtr->namesp;
    varPtr->protection = vdefn->protection;
    Itcl_NsCacheVarChanged(cdefnPtr->namesp, vdefn->name);

    /*
     *  NOTE:  Tcl reports a "dangling upvar" error for variables
     *         with a null "hPtr" field.  Put something non-zero
     *         in here to keep Tcl_SetVar2() happy.  The only time
     *         this field is really used is it remove a variable
     *         from the hash table that contains it in CleanupVar,
     *         but since these variables are protected by their
     *         higher refCount, they will not be deleted by CleanupVar
     *         anyway.  These variables are unset and removed in
     *         ItclFreeClass().
     */
    varPtr->hPtr = (Tcl_HashEntry*)0x1;
    varPtr->refCount = 1;  /* protect from being deleted */

    /*
     *  If an initialization value was specified, then initialize
     *  the variable now.  Update the virtual data table, so the
     *  "set" command will work properly.
     */
    if (init) {
        Itcl_BuildVirtualTables(cdefnPtr);

        nsToken = Itcl_ActivateNamesp(interp, cdefnPtr->namesp);
        init = Tcl_SetVar(interp, vdefn->name, init, ITCL_GLOBAL_VAR);
        Itcl_DeactivateNamesp(interp, nsToken);

        if (init == NULL) {
            Tcl_AppendResult(interp, "cannot initialize common variable \"",
                vdefn->name, "\"",
                (char*)NULL);
            return TCL_ERROR;
        }
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_OldDeleteCmd()
 *
 *  Invokes the destructors, and deletes the object that invoked this
 *  operation.  If an error is encountered during destruction, the
 *  delete operation is aborted.  Handles the following syntax:
 *
 *     <objName> delete
 *
 *  When an object is successfully deleted, it is removed from the
 *  list of known objects, and its access command is deleted.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
static int
Itcl_OldBiDeleteCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;

    if (argc != 1) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  If there is an object context, then destruct the object
     *  and delete it.
     */
    if ((Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK)
        || !obj) {
        Tcl_SetResult(interp, "improper usage: should be \"object delete\"",
            TCL_STATIC);
        return TCL_ERROR;
    }

    if (Itcl_DeleteObject(interp,obj) != TCL_OK) {
        return TCL_ERROR;
    }

    Tcl_SetResult(interp, "", TCL_STATIC);
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_OldVirtualCmd()
 *
 *  Executes the remainder of its command line arguments in the
 *  most-specific class scope for the current object.  If there is
 *  no object context, this fails.
 *
 *  NOTE:  All methods are now implicitly virtual, and there are
 *    much better ways to manipulate scope.  This command is only
 *    provided for backward-compatibility, and should be avoided.
 *
 *  Returns a status TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
static int
Itcl_OldBiVirtualCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    int status;
    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;
    Itcl_ActiveNamespace nsToken;
    char* cmd;

    if (argc == 1) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " command ?args...?\"\n",
            "  This command will be removed soon.\n",
            "  Commands are now virtual by default.",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  If there is no object context, then return an error.
     */
    if ((Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK)
        || !obj) {
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp,
            "cannot use \"virtual\" without an object context\n",
            "  This command will be removed soon.\n",
            "  Commands are now virtual by default.",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Install the most-specific namespace for this object, with
     *  the object context as clientData.  Invoke the rest of the
     *  args as a command in that namespace.
     */
    nsToken = Itcl_ActivateNamesp2(interp, obj->cdefn->namesp,
        (ClientData)obj);

    cmd = Tcl_Merge(argc-1,argv+1);
    status = Itcl_EvalArgs(interp, cmd, (char*)NULL, argc-1, argv+1);
    ckfree(cmd);

    Itcl_DeactivateNamesp(interp, nsToken);

    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_OldPreviousCmd()
 *
 *  Executes the remainder of its command line arguments in the
 *  previous class scope (i.e., the next scope up in the heritage
 *  list).
 *
 *  NOTE:  There are much better ways to manipulate scope.  This
 *    command is only provided for backward-compatibility, and should
 *    be avoided.
 *
 *  Returns a status TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
static int
Itcl_OldBiPreviousCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    int status;
    Itcl_Namespace classNs;
    Itcl_Class *cdefn, *base;
    ItclClass *cdefnPtr;
    Itcl_Object *obj;
    Itcl_ActiveNamespace nsToken;
    Itcl_ListElem *elem;
    char* cmd;

    if (argc == 1) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " command ?args...?\"\n",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  If the current context is not a class namespace,
     *  return an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        return TCL_ERROR;
    }
    cdefnPtr = (ItclClass*)cdefn;

    /*
     *  Get the heritage information for this class and move one
     *  level up in the hierarchy.  If there is no base class,
     *  return an error.
     */
    elem = Itcl_FirstListElem(&cdefnPtr->bases);
    if (!elem) {
        Tcl_AppendResult(interp,
            "no previous class in inheritance hierarchy for \"",
            Itcl_GetNamespPath(cdefnPtr->namesp), "\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    base = (Itcl_Class*)Itcl_GetListValue(elem);

    /*
     *  Invoke the rest of the args as a command in that namespace.
     */
    nsToken = Itcl_ActivateNamesp2(interp, base->namesp, (ClientData)obj);

    cmd = Tcl_Merge(argc-1,argv+1);
    status = Itcl_EvalArgs(interp, cmd, (char*)NULL, argc-1, argv+1);
    ckfree(cmd);

    Itcl_DeactivateNamesp(interp, nsToken);

    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_OldBiInfoMethodsCmd()
 *
 *  Returns information regarding methods for an object.  This command
 *  can be invoked with or without an object context:
 *
 *    <objName> info...   <= returns info for most-specific class
 *    info...             <= returns info for active namespace
 *
 *  Handles the following syntax:
 *
 *    info method ?methodName? ?-args? ?-body?
 *
 *  If the ?methodName? is not specified, then a list of all known
 *  methods is returned.  Otherwise, the information (args/body) for
 *  a specific method is returned.  Returns a status TCL_OK/TCL_ERROR
 *  to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
static int
Itcl_OldBiInfoMethodsCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    char *methodName = NULL;
    int methodArgs = 0;
    int methodBody = 0;

    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;
    Itcl_HierIter hier;
    Tcl_HashSearch place;
    Tcl_HashEntry *entry;
    ItclCmdMember *mdefn;
    ItclCmdImplement *mimpl;
    Namespace *nsPtr;
    Command *cmdPtr;

    /*
     *  If this command is not invoked within a class namespace,
     *  signal an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  If there is an object context, then use the most-specific
     *  class for the object.  Otherwise, use the current class
     *  namespace.
     */
    if (obj) {
        cdefn = obj->cdefn;
    }

    /*
     *  Process args:  ?methodName? ?-args? ?-body?
     */
    argv++;  /* skip over command name */
    argc--;

    if (argc > 0) {
        methodName = *argv;
        argc--; argv++;
    }
    for ( ; argc > 0; argc--, argv++) {
        if (strcmp(*argv,"-args") == 0)
            methodArgs = ~0;
        else if (strcmp(*argv,"-body") == 0)
            methodBody = ~0;
        else {
            Tcl_AppendResult(interp, "bad option \"", *argv,
                "\": should be -args or -body",
                (char*)NULL);
            return TCL_ERROR;
        }
    }

    /*
     *  Return info for a specific method.
     */
    if (methodName) {
        if (Itcl_FindCmdDefn(interp,cdefn,methodName,&mdefn) != TCL_OK) {
            return TCL_ERROR;
        }
        if (mdefn) {
            int i, valc = 0;
            char *valv[5];

            /*
             *  If the implementation has not yet been defined,
             *  autoload it now.
             */
            if (Itcl_GetCmdImplement(interp, mdefn, &mimpl) != TCL_OK) {
                return TCL_ERROR;
            }

            if (!methodArgs && !methodBody) {
                valv[valc++] = mdefn->fullname;
                methodArgs = methodBody = ~0;
            }
            if (methodArgs) {
                if (mimpl->arglist) {
                    valv[valc++] = Itcl_ArgList(mimpl->arglist);
                }
                else {
                    valv[valc++] = "";
                }
            }
            if (methodBody) {
                valv[valc++] = mimpl->body;
            }

            /*
             *  If the result list has a single element, then
             *  return it using Tcl_SetResult() so that it will
             *  look like a string and not a list with one element.
             */
            if (valc == 1) {
                Tcl_SetResult(interp, valv[0], TCL_VOLATILE);
            }
            else {
                for (i=0; i < valc; i++) {
                    Tcl_AppendElement(interp,valv[i]);
                }
            }
        }
    }

    /*
     *  Return the list of available methods.
     */
    else {
        Itcl_InitHierIter(&hier, cdefn);
        while ((cdefn=Itcl_AdvanceHierIter(&hier)) != NULL) {
            nsPtr = (Namespace*)cdefn->namesp;

            entry = Tcl_FirstHashEntry(&nsPtr->commands,&place);
            while (entry) {
                cmdPtr = (Command*)Tcl_GetHashValue(entry);
                if (cmdPtr->proc == Itcl_ExecMethod) {
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
 *  Itcl_OldBiInfoProcsCmd()
 *
 *  Returns information regarding procs for a class.  This command
 *  can be invoked with or without an object context:
 *
 *    <objName> info...   <= returns info for most-specific class
 *    info...             <= returns info for active namespace
 *
 *  Handles the following syntax:
 *
 *    info proc ?procName? ?-args? ?-body?
 *
 *  If the ?procName? is not specified, then a list of all known
 *  procs is returned.  Otherwise, the information (args/body) for
 *  a specific proc is returned.  Returns a status TCL_OK/TCL_ERROR
 *  to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
static int
Itcl_OldBiInfoProcsCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    Itcl_HierIter hier;
    Itcl_Namespace classNs;
    Itcl_Object *obj;
    Itcl_Class *cdefn;

    char *procName = NULL;
    int procArgs = 0;
    int procBody = 0;

    Tcl_HashSearch place;
    Tcl_HashEntry *entry;
    ItclCmdMember *mdefn;
    ItclCmdImplement *mimpl;
    Namespace *nsPtr;
    Command *cmdPtr;

    /*
     *  If this command is not invoked within a class namespace,
     *  signal an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  If there is an object context, then use the most-specific
     *  class for the object.  Otherwise, use the current class
     *  namespace.
     */
    if (obj) {
        cdefn = obj->cdefn;
    }

    /*
     *  Process args:  ?procName? ?-args? ?-body?
     */
    argv++;  /* skip over command name */
    argc--;

    if (argc > 0) {
        procName = *argv;
        argc--; argv++;
    }
    for ( ; argc > 0; argc--, argv++) {
        if (strcmp(*argv,"-args") == 0)
            procArgs = ~0;
        else if (strcmp(*argv,"-body") == 0)
            procBody = ~0;
        else {
            Tcl_AppendResult(interp, "bad option \"", *argv,
                "\": should be -args or -body",
                (char*)NULL);
            return TCL_ERROR;
        }
    }

    /*
     *  Return info for a specific proc.
     */
    if (procName) {
        if (Itcl_FindCmdDefn(interp,cdefn,procName,&mdefn) != TCL_OK) {
            return TCL_ERROR;
        }
        if (mdefn) {
            int i, valc = 0;
            char *valv[5];

            /*
             *  If the implementation has not yet been defined,
             *  autoload it now.
             */
            if (Itcl_GetCmdImplement(interp, mdefn, &mimpl) != TCL_OK) {
                return TCL_ERROR;
            }

            if (!procArgs && !procBody) {
                valv[valc++] = mdefn->fullname;
                procArgs = procBody = ~0;
            }
            if (procArgs) {
                if (mimpl->arglist) {
                    valv[valc++] = Itcl_ArgList(mimpl->arglist);
                }
                else {
                    valv[valc++] = "";
                }
            }
            if (procBody) {
                valv[valc++] = mimpl->body;
            }

            /*
             *  If the result list has a single element, then
             *  return it using Tcl_SetResult() so that it will
             *  look like a string and not a list with one element.
             */
            if (valc == 1) {
                Tcl_SetResult(interp, valv[0], TCL_VOLATILE);
            }
            else {
                for (i=0; i < valc; i++) {
                    Tcl_AppendElement(interp,valv[i]);
                }
            }
        }
    }

    /*
     *  Return the list of available procs.
     */
    else {
        Itcl_InitHierIter(&hier, cdefn);
        while ((cdefn=Itcl_AdvanceHierIter(&hier)) != NULL) {
            nsPtr = (Namespace*)cdefn->namesp;

            entry = Tcl_FirstHashEntry(&nsPtr->commands, &place);
            while (entry) {
                cmdPtr = (Command*)Tcl_GetHashValue(entry);
                if (cmdPtr->proc == Itcl_ExecProc) {
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
 *  Itcl_OldBiInfoPublicsCmd()
 *
 *  Sets the interpreter result to contain information for public
 *  variables in the class.  Handles the following syntax:
 *
 *     info public ?varName? ?-init? ?-value? ?-config?
 *
 *  If the ?varName? is not specified, then a list of all known public
 *  variables is returned.  Otherwise, the information (init/value/config)
 *  for a specific variable is returned.  Returns a status
 *  TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
static int
Itcl_OldBiInfoPublicsCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    char *varName = NULL;
    int varInit = 0;
    int varCheck = 0;
    int varValue = 0;

    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;

    ItclClass *cdPtr;
    ItclVarDefn *vdefn;
    Itcl_HierIter hier;
    Tcl_HashEntry *entry;
    Tcl_HashSearch place;

    /*
     *  If this command is not invoked within a class namespace,
     *  signal an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  Process args:  ?varName? ?-init? ?-value? ?-config?
     */
    argv++;  /* skip over command name */
    argc--;

    if (argc > 0) {
        varName = *argv;
        argc--; argv++;
    }
    for ( ; argc > 0; argc--, argv++) {
        if (strcmp(*argv,"-init") == 0)
            varInit = ~0;
        else if (strcmp(*argv,"-value") == 0)
            varValue = ~0;
        else if (strcmp(*argv,"-config") == 0)
            varCheck = ~0;
        else {
            Tcl_AppendResult(interp, "bad option \"", *argv,
                "\": should be -init, -value or -config",
                (char*)NULL);
            return TCL_ERROR;
        }
    }

    /*
     *  Return info for a specific variable.
     */
    if (varName) {
        int i, valc = 0;
        char *valv[5];

        if (Itcl_FindVarDefn(interp, cdefn, varName, &vdefn) != TCL_OK) {
            return TCL_ERROR;
        }
        if (vdefn && (vdefn->protection == ITCL_PUBLIC)) {

            if (!varInit && !varCheck && !varValue) {
                valv[valc++] = vdefn->fullname;
                varInit = varCheck = varValue = ~0;
            }
            if (varInit) {
                valv[valc++] = (vdefn->init) ? vdefn->init : "";
            }
            if (varValue) {
                char *val;
                val = Itcl_GetInstanceVar(interp, vdefn->fullname,
                    obj, obj->cdefn);

                valv[valc++] = (val) ? val : "<undefined>";
            }

            if (varCheck) {
                valv[valc++] = (vdefn->config) ? vdefn->config->body : "";
            }

            /*
             *  If the result list has a single element, then
             *  return it using Tcl_SetResult() so that it will
             *  look like a string and not a list with one element.
             */
            if (valc == 1) {
                Tcl_SetResult(interp, valv[0], TCL_VOLATILE);
            }
            else {
                for (i=0; i < valc; i++) {
                    Tcl_AppendElement(interp,valv[i]);
                }
            }
        }
    }

    /*
     *  Return the list of public variables.
     */
    else {
        Itcl_InitHierIter(&hier, cdefn);
        cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
        while (cdPtr != NULL) {
            entry = Tcl_FirstHashEntry(&cdPtr->variables, &place);
            while (entry) {
                vdefn = (ItclVarDefn*)Tcl_GetHashValue(entry);
                if ((vdefn->flags & ITCL_COMMON) == 0 &&
                     vdefn->protection == ITCL_PUBLIC) {
                    Tcl_AppendElement(interp, vdefn->fullname);
                }
                entry = Tcl_NextHashEntry(&place);
            }
            cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
        }
        Itcl_DeleteHierIter(&hier);
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_OldBiInfoProtectedsCmd()
 *
 *  Sets the interpreter result to contain information for protected
 *  variables in the class.  Handles the following syntax:
 *
 *     info protected ?varName? ?-init? ?-value?
 *
 *  If the ?varName? is not specified, then a list of all known public
 *  variables is returned.  Otherwise, the information (init/value)
 *  for a specific variable is returned.  Returns a status
 *  TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
static int
Itcl_OldBiInfoProtectedsCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    char *varName = NULL;
    int varInit = 0;
    int varValue = 0;

    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;

    ItclClass *cdPtr;
    ItclVarDefn *vdefn;
    Itcl_HierIter hier;
    Tcl_HashEntry *entry;
    Tcl_HashSearch place;

    /*
     *  If this command is not invoked within a class namespace,
     *  signal an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  Process args:  ?varName? ?-init? ?-value?
     */
    argv++;  /* skip over command name */
    argc--;

    if (argc > 0) {
        varName = *argv;
        argc--; argv++;
    }
    for ( ; argc > 0; argc--, argv++) {
        if (strcmp(*argv,"-init") == 0)
            varInit = ~0;
        else if (strcmp(*argv,"-value") == 0)
            varValue = ~0;
        else {
            Tcl_AppendResult(interp, "bad option \"", *argv,
                "\": should be -init or -value",
                (char*)NULL);
            return TCL_ERROR;
        }
    }

    /*
     *  Return info for a specific variable.
     */
    if (varName) {
        int i, valc = 0;
        char *valv[5];

        if (Itcl_FindVarDefn(interp, cdefn, varName, &vdefn) != TCL_OK) {
            return TCL_ERROR;
        }
        if (vdefn && (vdefn->protection == ITCL_PROTECTED)) {

            if (!varInit && !varValue) {
                valv[valc++] = vdefn->fullname;
                varInit = varValue = ~0;
            }

            /*
             *  If this is the built-in "this" variable, then
             *  report the object name as its initialization string.
             */
            if (varInit) {
                if ((vdefn->flags & ITCL_THIS_VAR) != 0) {
                    if (obj && obj->accessCmd)
                        valv[valc++] = Tcl_GetCommandPath(interp, obj->accessCmd);
                    else
                        valv[valc++] = "<objectName>";
                }
                else {
                    valv[valc++] = (vdefn->init) ? vdefn->init : "";
                }
            }

            if (varValue) {
                char *val;
                val = Itcl_GetInstanceVar(interp, vdefn->fullname,
                    obj, obj->cdefn);

                valv[valc++] = (val) ? val : "<undefined>";
            }

            /*
             *  If the result list has a single element, then
             *  return it using Tcl_SetResult() so that it will
             *  look like a string and not a list with one element.
             */
            if (valc == 1) {
                Tcl_SetResult(interp, valv[0], TCL_VOLATILE);
            }
            else {
                for (i=0; i < valc; i++) {
                    Tcl_AppendElement(interp,valv[i]);
                }
            }
        }
    }

    /*
     *  Return the list of public variables.
     */
    else {
        Itcl_InitHierIter(&hier, cdefn);
        cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
        while (cdPtr != NULL) {
            entry = Tcl_FirstHashEntry(&cdPtr->variables, &place);
            while (entry) {
                vdefn = (ItclVarDefn*)Tcl_GetHashValue(entry);
                if ((vdefn->flags & ITCL_COMMON) == 0 &&
                     vdefn->protection == ITCL_PROTECTED) {
                    Tcl_AppendElement(interp, vdefn->fullname);
                }
                entry = Tcl_NextHashEntry(&place);
            }
            cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
        }
        Itcl_DeleteHierIter(&hier);
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_OldBiInfoCommonsCmd()
 *
 *  Sets the interpreter result to contain information for common
 *  variables in the class.  Handles the following syntax:
 *
 *     info common ?varName? ?-init? ?-value?
 *
 *  If the ?varName? is not specified, then a list of all known common
 *  variables is returned.  Otherwise, the information (init/value)
 *  for a specific variable is returned.  Returns a status
 *  TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
static int
Itcl_OldBiInfoCommonsCmd(dummy, interp, argc, argv)
    ClientData dummy;     /* not used */
    Tcl_Interp *interp;   /* current interpreter */
    int argc;             /* number of arguments */
    char **argv;          /* argument strings */
{
    char *varName = NULL;
    int varInit = 0;
    int varValue = 0;

    Itcl_Namespace classNs;
    Itcl_Class *cdefn;
    Itcl_Object *obj;

    ItclClass *cdPtr;
    ItclVarDefn *vdefn;
    Itcl_HierIter hier;
    Tcl_HashEntry *entry;
    Tcl_HashSearch place;
    Itcl_ActiveNamespace nsToken;

    /*
     *  If this command is not invoked within a class namespace,
     *  signal an error.
     */
    if (Itcl_GetClassContext(interp, &classNs, &cdefn, &obj) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  Process args:  ?varName? ?-init? ?-value?
     */
    argv++;  /* skip over command name */
    argc--;

    if (argc > 0) {
        varName = *argv;
        argc--; argv++;
    }
    for ( ; argc > 0; argc--, argv++) {
        if (strcmp(*argv,"-init") == 0)
            varInit = ~0;
        else if (strcmp(*argv,"-value") == 0)
            varValue = ~0;
        else {
            Tcl_AppendResult(interp, "bad option \"", *argv,
                "\": should be -init or -value",
                (char*)NULL);
            return TCL_ERROR;
        }
    }

    /*
     *  Return info for a specific variable.
     */
    if (varName) {
        int i, valc = 0;
        char *valv[5];

        if (Itcl_FindVarDefn(interp, cdefn, varName, &vdefn) != TCL_OK) {
            return TCL_ERROR;
        }
        if (vdefn && (vdefn->protection == ITCL_PROTECTED)) {

            if (!varInit && !varValue) {
                valv[valc++] = vdefn->fullname;
                varInit = varValue = ~0;
            }
            if (varInit)
                valv[valc++] = (vdefn->init) ? vdefn->init : "";

            if (varValue) {
                char *val;
                nsToken = Itcl_ActivateNamesp(interp, cdefn->namesp);
                val = Tcl_GetVar(interp, vdefn->fullname, 0);
                Itcl_DeactivateNamesp(interp, nsToken);

                valv[valc++] = (val) ? val : "<undefined>";
            }

            /*
             *  If the result list has a single element, then
             *  return it using Tcl_SetResult() so that it will
             *  look like a string and not a list with one element.
             */
            if (valc == 1) {
                Tcl_SetResult(interp, valv[0], TCL_VOLATILE);
            }
            else {
                for (i=0; i < valc; i++) {
                    Tcl_AppendElement(interp,valv[i]);
                }
            }
        }
    }

    /*
     *  Return the list of public variables.
     */
    else {
        Itcl_InitHierIter(&hier, cdefn);
        cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
        while (cdPtr != NULL) {
            entry = Tcl_FirstHashEntry(&cdPtr->variables, &place);
            while (entry) {
                vdefn = (ItclVarDefn*)Tcl_GetHashValue(entry);
                if ((vdefn->flags & ITCL_COMMON) &&
                     vdefn->protection == ITCL_PROTECTED) {
                    Tcl_AppendElement(interp, vdefn->fullname);
                }
                entry = Tcl_NextHashEntry(&place);
            }
            cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
        }
        Itcl_DeleteHierIter(&hier);
    }
    return TCL_OK;
}
