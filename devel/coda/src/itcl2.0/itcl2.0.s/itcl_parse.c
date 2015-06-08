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
 *  Procedures in this file support the new syntax for [incr Tcl]
 *  class definitions:
 *
 *    itcl_class <className> {
 *        inherit <base-class>...
 *
 *        constructor {<arglist>} ?{<init>}? {<body>}
 *        destructor {<body>}
 *
 *        method <name> {<arglist>} {<body>}
 *        proc <name> {<arglist>} {<body>}
 *        variable <name> ?<init>? ?<config>?
 *        common <name> ?<init>?
 *
 *        public <thing> ?<args>...?
 *        protected <thing> ?<args>...?
 *        private <thing> ?<args>...?
 *    }
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl_parse.c,v 1.1.1.1 1996/08/21 19:29:44 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include <assert.h>
#include "itclInt.h"

static char rcsid[] = "$Id: itcl_parse.c,v 1.1.1.1 1996/08/21 19:29:44 heyes Exp $";

/*
 *  Info needed for public/protected/private commands:
 */
typedef struct ProtectionCmdInfo {
    int pLevel;               /* protection level */
    ItclObjectInfo *info;     /* info regarding all known objects */
} ProtectionCmdInfo;


/*
 * ------------------------------------------------------------------------
 *  Itcl_ParseInit()
 *
 *  Invoked by Itcl_Init() whenever a new interpeter is created to add
 *  [incr Tcl] facilities.  Adds the commands needed to parse class
 *  definitions.
 * ------------------------------------------------------------------------
 */
int
Itcl_ParseInit(interp,info)
    Tcl_Interp *interp;     /* interpreter to be updated */
    ItclObjectInfo *info;   /* info regarding all known objects */
{
    Itcl_Namespace parserNs;
    ProtectionCmdInfo *pInfo;

    /*
     *  Create the "itcl::parser" namespace used to parse class
     *  definitions.
     */
    if (Itcl_CreateNamesp(interp, "::itcl::parser",
        Itcl_PreserveData((ClientData)info), Itcl_ReleaseData,
        &parserNs) != TCL_OK) {
        Tcl_AppendResult(interp, " (cannot initialize itcl parser)",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Add commands for parsing class definitions.
     */
    Tcl_CreateCommand(interp, "::itcl::parser::inherit",
        Itcl_ClassInheritCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::constructor",
        Itcl_ClassConstructorCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::destructor",
        Itcl_ClassDestructorCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::method",
        Itcl_ClassMethodCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::proc",
        Itcl_ClassProcCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::common",
        Itcl_ClassCommonCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::variable",
        Itcl_ClassVariableCmd, (ClientData)info, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::set",
        Tcl_SetCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::array",
        Tcl_ArrayCmd, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::scope", Itcl_ScopeCmd,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::code", Itcl_CodeCmd,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::parser::@scope", Itcl_AtScopeCmd,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    pInfo = (ProtectionCmdInfo*)ckalloc(sizeof(ProtectionCmdInfo));
    pInfo->pLevel = ITCL_PUBLIC;
    pInfo->info = info;

    Tcl_CreateCommand(interp, "::itcl::parser::public",
        Itcl_ClassProtectionCmd, (ClientData)pInfo, (Tcl_CmdDeleteProc*)free);

    pInfo = (ProtectionCmdInfo*)ckalloc(sizeof(ProtectionCmdInfo));
    pInfo->pLevel = ITCL_PROTECTED;
    pInfo->info = info;

    Tcl_CreateCommand(interp, "::itcl::parser::protected",
        Itcl_ClassProtectionCmd, (ClientData)pInfo, (Tcl_CmdDeleteProc*)free);

    pInfo = (ProtectionCmdInfo*)ckalloc(sizeof(ProtectionCmdInfo));
    pInfo->pLevel = ITCL_PRIVATE;
    pInfo->info = info;

    Tcl_CreateCommand(interp, "::itcl::parser::private",
        Itcl_ClassProtectionCmd, (ClientData)pInfo, (Tcl_CmdDeleteProc*)free);

    /*
     *  Clear the import list for the parser namespace, so that
     *  only the parser commands are allowed in the context of a
     *  class definition.  Set a "variable enforcer" procedure
     *  to control access to "common" data members.
     */
    Itcl_ClearImportNamesp(parserNs);
    Itcl_SetVarEnforcer(parserNs, Itcl_ParseVarEnforcer);

    /*
     *  Install the "class" command for defining new classes.
     */
    Tcl_CreateCommand(interp, "::itcl::class", Itcl_ClassCmd,
        Itcl_PreserveData((ClientData)info), Itcl_ReleaseData);

    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ClassCmd()
 *
 *  Invoked by Tcl whenever the user issues an "itcl::class" command to
 *  specify a class definition.  Handles the following syntax:
 *
 *    itcl::class <className> {
 *        inherit <base-class>...
 *
 *        constructor {<arglist>} ?{<init>}? {<body>}
 *        destructor {<body>}
 *
 *        method <name> {<arglist>} {<body>}
 *        proc <name> {<arglist>} {<body>}
 *        variable <varname> ?<init>? ?<config>?
 *        common <varname> ?<init>?
 *
 *        public <args>...
 *        protected <args>...
 *        private <args>...
 *    }
 *
 * ------------------------------------------------------------------------
 */
int
Itcl_ClassCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo* info = (ItclObjectInfo*)clientData;

    int status, cmdProt, varProt;
    Itcl_Namespace classNs;
    ItclClass *cdefnPtr, *basePtr;
    Itcl_ListElem *elem;
    Itcl_Namespace parserNs, itclBiNs;
    Itcl_ActiveNamespace nsToken;

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
    if (Itcl_FindNamesp(interp, "::itcl::parser", 0, &parserNs) != TCL_OK) {
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
    if (Itcl_InstallBiMethods(interp, (Itcl_Class*)cdefnPtr) != TCL_OK) {
        Itcl_DeleteNamesp(classNs);
        return TCL_ERROR;
    }

    /*
     *  Set up the import list to bring in methods/procs from base
     *  classes.  Add itcl::builtin for built-in commands, and the
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

    if (Itcl_FindNamesp(interp, "::itcl::builtin", 0, &itclBiNs) == TCL_OK &&
        itclBiNs != NULL) {
        Itcl_AppendImportNamesp(cdefnPtr->namesp, itclBiNs, ITCL_PUBLIC);
    }

    Itcl_AppendImportNamesp(cdefnPtr->namesp,
        Itcl_GetNamespParent(cdefnPtr->namesp), ITCL_PUBLIC);

    /*
     *  Build the virtual table for methods in this class.
     */
    Itcl_BuildVirtualTables(cdefnPtr);

    Tcl_SetResult(interp, "", TCL_STATIC);
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ClassInheritCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "inherit" command is invoked to define one or more base classes.
 *  Handles the following syntax:
 *
 *      inherit <baseclass> ?<baseclass>...?
 *
 * ------------------------------------------------------------------------
 */
int
Itcl_ClassInheritCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    ItclClass *cdefnPtr = (ItclClass*)Itcl_PeekStack(&info->cdefnStack);

    int i, j;
    Itcl_ListElem *elem, *elem2;
    ItclClass *cdPtr, *badcdPtr;
    Itcl_HierIter hier;
    Itcl_Stack classes, stack;
    Itcl_Namespace baseNs;
    Itcl_ActiveNamespace nsToken;

    if (argc < 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " class ?class...?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  In "inherit" statement can only be included once in a
     *  class definition.
     */
    elem = Itcl_FirstListElem(&cdefnPtr->bases);
    if (elem != NULL) {
        Tcl_AppendResult(interp, "inheritance \"", (char*)NULL);
        while (elem) {
            cdPtr = (ItclClass*)Itcl_GetListValue(elem);
            Tcl_AppendResult(interp, cdPtr->name, " ", (char*)NULL);
            elem = Itcl_NextListElem(elem);
        }

        Tcl_AppendResult(interp, "\" already defined for class \"",
            Itcl_GetNamespPath(cdefnPtr->namesp), "\"", (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Validate each base class and add it to the "bases" list.
     */
    nsToken = Itcl_ActivateNamesp(interp,
        Itcl_GetNamespParent(cdefnPtr->namesp));

    for (argc--,argv++; argc > 0; argc--,argv++) {
        /*
         *  Invoke the class name as a command in the parent
         *  namespace, giving the autoloader a chance to load
         *  the class definition.
         */
        if (Tcl_Eval(interp, *argv) != TCL_OK) {
            char msg[256];
            sprintf(msg, "\n    (while inheriting from class \"%.200s\")",
                *argv);
            Tcl_AddErrorInfo(interp, msg);
            goto inheritError;
        }

        /*
         *  Make sure that the base class name is known in the
         *  parent namespace (currently active).
         */
        if (Itcl_FindClass(interp, *argv, &baseNs) != TCL_OK) {
            Tcl_AppendResult(interp, "cannot inherit from unknown class \"",
                *argv, "\"", (char*)NULL);
            goto inheritError;
        }

        /*
         *  Make sure that the base class is not the same as the
         *  class that is being built.
         */
        if (baseNs == cdefnPtr->namesp) {
            Tcl_AppendResult(interp, "class \"", cdefnPtr->name,
                "\" cannot inherit from itself", (char*)NULL);
            goto inheritError;
        }

        cdPtr = (ItclClass*)Itcl_GetNamespData(baseNs);
        Itcl_AppendList(&cdefnPtr->bases, Itcl_PreserveData((ClientData)cdPtr));
    }
    Itcl_DeactivateNamesp(interp, nsToken);
    nsToken = NULL;

    /*
     *  Scan through the inheritance list to make sure that no
     *  class appears twice.
     */
    elem = Itcl_FirstListElem(&cdefnPtr->bases);
    while (elem) {
        elem2 = Itcl_NextListElem(elem);
        while (elem2) {
            if (Itcl_GetListValue(elem) == Itcl_GetListValue(elem2)) {
                cdPtr = (ItclClass*)Itcl_GetListValue(elem);
                Tcl_AppendResult(interp, "class \"",
                    Itcl_GetNamespPath(cdefnPtr->namesp),
                    (char*)NULL);
                Tcl_AppendResult(interp, "\" cannot inherit base class \"",
                    Itcl_GetNamespPath(cdPtr->namesp),
                    "\" more than once", (char*)NULL);
                goto inheritError;
            }
            elem2 = Itcl_NextListElem(elem2);
        }
        elem = Itcl_NextListElem(elem);
    }

    /*
     *  Scan through the entire class hierarchy and make sure that
     *  no class appears twice.
     */
    Itcl_InitStack(&classes);
    Itcl_InitHierIter(&hier, (Itcl_Class*)cdefnPtr);

    cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
    while (cdPtr != NULL) {
        Itcl_PushStack((ClientData)cdPtr, &classes);
        cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
    }

    for (i=0; i < Itcl_GetStackSize(&classes); i++) {
        for (j=i+1; j < Itcl_GetStackSize(&classes); j++)
            if (Itcl_GetStackValue(&classes,i)
                == Itcl_GetStackValue(&classes,j))
                break;

        if (j < Itcl_GetStackSize(&classes))
            break;
    }

    if (i < Itcl_GetStackSize(&classes)) {
        badcdPtr = (ItclClass*)Itcl_GetStackValue(&classes,i);
    } else {
        badcdPtr = NULL;
    }

    Itcl_DeleteStack(&classes);
    Itcl_DeleteHierIter(&hier);

    /*
     *  Same base class found twice in the hierarchy?
     *  Then flag error.  Show the list of multiple paths
     *  leading to the same base class.
     */
    if (badcdPtr) {
        Tcl_AppendResult(interp, "class \"",
            Itcl_GetNamespPath(cdefnPtr->namesp),
            (char*)NULL);
        Tcl_AppendResult(interp, "\" inherits base class \"",
            Itcl_GetNamespPath(badcdPtr->namesp),
            "\" more than once:",
            (char*)NULL);

        cdPtr = cdefnPtr;
        Itcl_InitStack(&stack);
        Itcl_PushStack((ClientData)cdPtr, &stack);

        /*
         *  Show paths leading to bad base class
         */
        while (Itcl_GetStackSize(&stack) > 0) {
            cdPtr = (ItclClass*)Itcl_PopStack(&stack);

            if (cdPtr == badcdPtr) {
                Tcl_AppendResult(interp, "\n  ", (char*)NULL);
                for (i=0; i < Itcl_GetStackSize(&stack); i++)
                    if (Itcl_GetStackValue(&stack,i) == NULL)
                        Tcl_AppendResult(interp,
                            ((Itcl_Class*)Itcl_GetStackValue(&stack,i-1))->name,
                            "->",
                            (char*)NULL);

                Tcl_AppendResult(interp, badcdPtr->name, (char*)NULL);
            }
            else if (!cdPtr)
                (void)Itcl_PopStack(&stack);

            else {
                elem = Itcl_LastListElem(&cdPtr->bases);
                if (elem) {
                    Itcl_PushStack((ClientData)cdPtr, &stack);
                    Itcl_PushStack((ClientData)NULL, &stack);
                    while (elem) {
                        Itcl_PushStack(Itcl_GetListValue(elem), &stack);
                        elem = Itcl_PrevListElem(elem);
                    }
                }
            }
        }
        Itcl_DeleteStack(&stack);
        goto inheritError;
    }

    return TCL_OK;


    /*
     *  If the "inherit" list cannot be built properly, tear it
     *  down and return an error.
     */
inheritError:
    if (nsToken != NULL) {
        Itcl_DeactivateNamesp(interp, nsToken);
    }

    elem = Itcl_FirstListElem(&cdefnPtr->bases);
    while (elem) {
        Itcl_ReleaseData( Itcl_GetListValue(elem) );
        elem = Itcl_DeleteListElem(elem);
    }
    return TCL_ERROR;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ClassProtectionCmd()
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
Itcl_ClassProtectionCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* protection level (public/protected/private) */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ProtectionCmdInfo *pInfo = (ProtectionCmdInfo*)clientData;

    int status;
    int oldCmdLevel, oldVarLevel;
    char *cmdStr;

    if (argc < 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " command ?arg arg...?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    oldCmdLevel = Itcl_CmdProtection(interp, pInfo->pLevel);
    oldVarLevel = Itcl_VarProtection(interp, pInfo->pLevel);

    if (argc == 2) {
        status = Tcl_Eval(interp, argv[1]);
    } else {
        cmdStr = Tcl_Merge(argc-1,argv+1);
        status = Itcl_EvalArgs(interp, cmdStr, (char*)NULL, argc-1, argv+1);
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
 *  Itcl_ClassConstructorCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "constructor" command is invoked to define the constructor
 *  for an object.  Handles the following syntax:
 *
 *      constructor <arglist> ?<init>? <body>
 *
 * ------------------------------------------------------------------------
 */
int
Itcl_ClassConstructorCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    ItclClass *cdefnPtr = (ItclClass*)Itcl_PeekStack(&info->cdefnStack);
    Namespace *nsPtr = (Namespace*)cdefnPtr->namesp;

    char *init, *body;

    if (argc < 3 || argc > 4) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " args ?init? body\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    if (Tcl_FindHashEntry(&nsPtr->commands, argv[0])) {
        Tcl_AppendResult(interp, "\"", argv[0],
            "\" already defined in class \"",
            Itcl_GetNamespPath(cdefnPtr->namesp), "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  If there is an object initialization statement, pick this
     *  out and take the last argument as the constructor body.
     */
    init = (argc == 4) ? argv[2] : NULL;
    body = (argc == 4) ? argv[3] : argv[2];

    if (init) {
        cdefnPtr->initCmd = (char*)ckalloc((unsigned)(strlen(init)+1));
        strcpy(cdefnPtr->initCmd, init);
    }

    if (Itcl_CreateMethod(interp, (Itcl_Class*)cdefnPtr,
        argv[0], argv[1], body) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_ClassDestructorCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "destructor" command is invoked to define the destructor
 *  for an object.  Handles the following syntax:
 *
 *      destructor <body>
 *
 * ------------------------------------------------------------------------
 */
int
Itcl_ClassDestructorCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    Itcl_Class *cdefn = (Itcl_Class*)Itcl_PeekStack(&info->cdefnStack);
    Namespace *nsPtr = (Namespace*)cdefn->namesp;

    if (argc != 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " body\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    if (Tcl_FindHashEntry(&nsPtr->commands, argv[0])) {
        Tcl_AppendResult(interp, "\"", argv[0],
            "\" already defined in class \"",
            Itcl_GetNamespPath(cdefn->namesp), "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    if (Itcl_CreateMethod(interp,cdefn,argv[0],(char*)NULL,argv[1]) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_ClassMethodCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "method" command is invoked to define an object method.
 *  Handles the following syntax:
 *
 *      method <name> ?<arglist>? ?<body>?
 *
 * ------------------------------------------------------------------------
 */
int
Itcl_ClassMethodCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    Itcl_Class *cdefn = (Itcl_Class*)Itcl_PeekStack(&info->cdefnStack);
    Namespace *nsPtr = (Namespace*)cdefn->namesp;
    char *args, *body;

    if (argc < 2 || argc > 4) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name ?args? ?body?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    if (Tcl_FindHashEntry(&nsPtr->commands, argv[1])) {
        Tcl_AppendResult(interp, "\"", argv[1],
            "\" already defined in class \"",
            Itcl_GetNamespPath(cdefn->namesp), "\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    args = (argc >= 3) ? argv[2] : NULL;
    body = (argc >= 4) ? argv[3] : NULL;

    if (Itcl_CreateMethod(interp,cdefn,argv[1],args,body) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_ClassProcCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "proc" command is invoked to define a common class proc.
 *  A "proc" is like a "method", but only has access to "common"
 *  class variables.  Handles the following syntax:
 *
 *      proc <name> ?<arglist>? ?<body>?
 *
 * ------------------------------------------------------------------------
 */
int
Itcl_ClassProcCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    Itcl_Class *cdefn = (Itcl_Class*)Itcl_PeekStack(&info->cdefnStack);
    Namespace *nsPtr = (Namespace*)cdefn->namesp;
    char *args, *body;

    if (argc < 2 || argc > 4) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name ?args? ?body?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    if (Tcl_FindHashEntry(&nsPtr->commands, argv[1])) {
        Tcl_AppendResult(interp, "\"", argv[1],
            "\" already defined in class \"",
            Itcl_GetNamespPath(cdefn->namesp), "\"",
            (char*)NULL);
        return TCL_ERROR;
    }
    args = (argc >= 3) ? argv[2] : NULL;
    body = (argc >= 4) ? argv[3] : NULL;

    if (Itcl_CreateProc(interp,cdefn,argv[1],args,body) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ClassVariableCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "variable" command is invoked to define an instance variable.
 *  Handles the following syntax:
 *
 *      variable <varname> ?<init>? ?<config>?
 *
 * ------------------------------------------------------------------------
 */
int
Itcl_ClassVariableCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* info for all known objects */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    ItclClass *cdefnPtr = (ItclClass*)Itcl_PeekStack(&info->cdefnStack);

    int pLevel, newEntry;
    Tcl_HashEntry *entry;
    ItclVarDefn *vdefn;
    char *init, *config;

    pLevel = Itcl_VarProtection(interp, 0);

    if (pLevel == ITCL_PUBLIC) {
        if (argc < 2 || argc > 4) {
            Tcl_AppendResult(interp, "wrong # args: should be \"",
                argv[0], " name ?init? ?config?\"",
                (char*)NULL);
            return TCL_ERROR;
        }
    }
    else if ((argc < 2) || (argc > 3)) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            argv[0], " name ?init?\"",
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

    vdefn->protection = pLevel;
    Tcl_SetHashValue(entry, (ClientData)vdefn);

    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_ClassCommonCmd()
 *
 *  Invoked by Tcl during the parsing of a class definition whenever
 *  the "common" command is invoked to define a variable that is
 *  common to all objects in the class.  Handles the following syntax:
 *
 *      common <varname> ?<init>?
 *
 * ------------------------------------------------------------------------
 */
int
Itcl_ClassCommonCmd(clientData, interp, argc, argv)
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

    vdefn->protection = Itcl_VarProtection(interp, 0);
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
 *  Itcl_ParseVarEnforcer()
 *
 *  Used by the "parser" namespace as an enforcer for variable accesses.
 *  An "enforcer" is a procedure which is consulted whenever a variable
 *  cannot be found in the local cache.  It can deny access to certain
 *  variables, or perform special lookups itself.
 *
 *  This procedure is used whenever a class definition is parsed.  It
 *  only allows access to "common" class variables that have been
 *  declared within the class or inherited from another class.  A "set"
 *  command can be used to initialized common data members within the
 *  body of the class definition itself:
 *
 *    itcl::class Foo {
 *        common colors
 *        set colors(red)   #ff0000
 *        set colors(green) #00ff00
 *        set colors(blue)  #0000ff
 *        ...
 *    }
 *
 *    itcl::class Bar {
 *        inherit Foo
 *        set colors(gray)  #a0a0a0
 *        set colors(white) #ffffff
 *
 *        common numbers
 *        set numbers(0) zero
 *        set numbers(1) one
 *    }
 *
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_ParseVarEnforcer(interp, name, varPtr, flags)
    Tcl_Interp *interp;    /* current interpreter */
    char* name;            /* name of the variable being accessed */
    Tcl_Var* varPtr;       /* returns Tcl_Var for desired variable or NULL */
    int flags;             /* ITCL_GLOBAL_VAR => global variable desired */
{
    Itcl_Namespace parserNs = Itcl_GetActiveNamesp(interp);
    ItclObjectInfo *info = (ItclObjectInfo*)Itcl_GetNamespData(parserNs);
    Itcl_Class *cdefn = (Itcl_Class*)Itcl_PeekStack(&info->cdefnStack);

    Namespace *nsPtr;
    Tcl_HashEntry *hPtr;
    ItclVarDefn *vdefn;

    /*
     *  See if the requested variable is a recognized "common" member.
     */
    if (Itcl_FindVarDefn(interp, cdefn, name, &vdefn) != TCL_OK) {
        return TCL_ERROR;
    }
    if (vdefn && (vdefn->flags & ITCL_COMMON) != 0) {
        nsPtr = (Namespace*)vdefn->cdefn->namesp;
        hPtr = Tcl_FindHashEntry(&nsPtr->variables, vdefn->name);
        assert(hPtr != NULL);

        *varPtr = (Tcl_Var)Tcl_GetHashValue(hPtr);
        return TCL_OK;
    }

    /*
     *  If it is not, return NULL and let lookup continue.
     *  This is important for variables like "errorInfo"
     *  that might get set while the parser namespace is active.
     */
    *varPtr = NULL;
    return TCL_OK;
}
