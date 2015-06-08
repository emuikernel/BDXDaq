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
 *  This file defines information that tracks classes and objects
 *  at a global level for a given interpreter.
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl_cmds.c,v 1.1.1.1 1996/08/21 19:29:42 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include <assert.h>
#include "itclInt.h"
#include "patchlevel.h"

/*
 *  FORWARD DECLARATIONS
 */
static void ItclDelObjectInfo _ANSI_ARGS_((ClientData cdata));

static char rcsid[] = "$Id: itcl_cmds.c,v 1.1.1.1 1996/08/21 19:29:42 heyes Exp $";


/*
 * ------------------------------------------------------------------------
 *  Itcl_Init()
 *
 *  Invoked whenever a new interpeter is created to install the
 *  [incr Tcl] package.  Usually invoked within Tcl_AppInit() at
 *  the start of execution.
 *
 *  Creates the "::itcl" namespace and installs access commands for
 *  creating classes and querying info.
 *
 *  Returns TCL_OK on success, or TCL_ERROR (along with an error
 *  message in the interpreter) if anything goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_Init(interp)
    Tcl_Interp *interp;  /* interpreter to be updated */
{
    static char initCmd[] =
    "if [file exists ${itcl::library}/init.itcl] {\n\
        source ${itcl::library}/init.itcl\n\
    } else {\n\
        set msg \"can't find ${itcl::library}/init.itcl\\n\"\n\
        append msg \"Perhaps you need to install \\[incr Tcl\\] \\n\"\n\
        append msg \"or set your ITCL_LIBRARY environment variable?\"\n\
        error $msg\n\
    }";

    Tcl_CmdInfo cmdInfo;
    Itcl_Namespace ns;
    ItclObjectInfo *info;
    char *libDir;

    /*
     *  See if [incr Tcl] is already installed.
     */
    if (Tcl_GetCommandInfo(interp, "::itcl::class", &cmdInfo)) {
        Tcl_SetResult(interp, "already installed: [incr Tcl]", TCL_STATIC);
        return TCL_ERROR;
    }

    /*
     *  Find or create the "::itcl" namespace.
     */
    if (Itcl_FindNamesp(interp, "::itcl", 0, &ns) != TCL_OK) {
        return TCL_ERROR;
    }
    if (ns == NULL && Itcl_CreateNamesp(interp, "::itcl",
        (ClientData)NULL, (Itcl_DeleteProc*)NULL, &ns) != TCL_OK) {
        Tcl_AppendResult(interp, " (cannot initialize itcl namespace)",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Create the top-level data structure for tracking objects.
     */
    info = (ItclObjectInfo*)ckalloc(sizeof(ItclObjectInfo));
    info->interp = interp;
    Tcl_InitHashTable(&info->objects, TCL_ONE_WORD_KEYS);
    Itcl_InitStack(&info->cdefnStack);

    /*
     *  Install commands into the "::itcl" namespace.
     */
    Tcl_CreateCommand(interp, "::itcl::class", Itcl_ClassCmd,
        Itcl_PreserveData((ClientData)info), Itcl_ReleaseData);

    Tcl_CreateCommand(interp, "::itcl::body", Itcl_BodyCmd,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl::configbody", Itcl_ConfigBodyCmd,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Tcl_CreateCommand(interp, "::itcl_context", Itcl_AtScopeCmd2,
        (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);

    Itcl_EventuallyFree((ClientData)info, ItclDelObjectInfo);

    /*
     *  Add high-level queries into the "::info" ensemble.
     */
    if (Itcl_AddEnsembleOption(interp, "::info", "classes",
            0, 1, "?pattern?",
            Itcl_InfoClassesCmd,
            Itcl_PreserveData((ClientData)info), Itcl_ReleaseData)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::info", "objects",
            0, ITCL_VAR_ARGS, "?-class className? ?-isa className? ?pattern?",
            Itcl_InfoObjectsCmd,
            Itcl_PreserveData((ClientData)info), Itcl_ReleaseData)
            != TCL_OK
    ) {
        return TCL_ERROR;
    }


    /*
     *  Add high-level delete operations into the "::delete" ensemble.
     */
    if (Itcl_AddEnsembleOption(interp, "::delete", "class",
            1, ITCL_VAR_ARGS, "name ?name...?",
            Itcl_DelClassCmd,
            Itcl_PreserveData((ClientData)info), Itcl_ReleaseData)
            != TCL_OK ||

        Itcl_AddEnsembleOption(interp, "::delete", "object",
            1, ITCL_VAR_ARGS, "name ?name...?",
            Itcl_DelObjectCmd,
            Itcl_PreserveData((ClientData)info), Itcl_ReleaseData)
            != TCL_OK
    ) {
        return TCL_ERROR;
    }

    /*
     *  Install the "::itcl::parser" namespace used to parse the
     *  class definitions.
     */
    if (Itcl_ParseInit(interp,info) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  Create "itcl::builtin" namespace for commands that
     *  are automatically built into class definitions.
     */
    if (Itcl_BiInit(interp) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  Install stuff needed for backward compatibility with previous
     *  version of [incr Tcl].
     */
    if (Itcl_OldInit(interp,info) != TCL_OK) {
        return TCL_ERROR;
    }

    /*
     *  Set up the library and load the "init.itcl" file.
     */

	/* Sergey: use built-in path */
    /*libDir = Tcl_GetVar2(interp, "env", "ITCL_LIBRARY", TCL_GLOBAL_ONLY);
	if (libDir == NULL) {*/
        libDir = ITCL_LIBRARY;
	/*}*/
    Tcl_SetVar(interp, "itcl::library", libDir, ITCL_GLOBAL_VAR);
    Tcl_SetVar(interp, "itcl::patchLevel", ITCL_PATCH_LEVEL, ITCL_GLOBAL_VAR);
    Tcl_SetVar(interp, "itcl::version", ITCL_VERSION, ITCL_GLOBAL_VAR);

    return Tcl_Eval(interp, initCmd);
}


/*
 * ------------------------------------------------------------------------
 *  ItclDelObjectInfo()
 *
 *  Invoked when the management info for [incr Tcl] is no longer being
 *  used in an interpreter.  This will only occur when all class
 *  manipulation commands are removed from the interpreter.
 * ------------------------------------------------------------------------
 */
static void
ItclDelObjectInfo(cdata)
    ClientData cdata;    /* client data for class command */
{
    ItclObjectInfo *info = (ItclObjectInfo*)cdata;

    Itcl_Object *obj;
    Tcl_HashSearch place;
    Tcl_HashEntry *entry;

    /*
     *  Destroy all known objects by deleting their access
     *  commands.
     */
    entry = Tcl_FirstHashEntry(&info->objects,&place);
    while (entry) {
        obj = (Itcl_Object*)Tcl_GetHashValue(entry);
        Tcl_DeleteCommand2(info->interp, obj->accessCmd);
        Tcl_DeleteHashEntry(entry);
        entry = Tcl_NextHashEntry(&place);
    }
    Tcl_DeleteHashTable(&info->objects);

    Itcl_DeleteStack(&info->cdefnStack);
    ckfree((char*)info);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoClassesCmd()
 *
 *  Part of the "::info" ensemble.  Invoked by Tcl whenever the user
 *  issues an "info classes" command to query the list of classes
 *  in the current namespace.  Handles the following syntax:
 *
 *    info classes ?<pattern>?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_InfoClassesCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* class/object info */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Itcl_Namespace activeNs = Itcl_GetActiveNamesp(interp);

    char *pattern;
    char *name;
    Tcl_HashEntry *entry;
    Tcl_HashSearch place;
    Itcl_List imports;
    Itcl_ListElem *elem;
    NamespImportRef *nsref;
    Itcl_Namespace ns;

    pattern = (argc == 2) ? argv[1] : NULL;

    /*
     *  Search all namespaces in the "import" list for child
     *  namespaces that represent classes.
     */
    Itcl_GetAllImportNamesp(activeNs, &imports);
    elem = Itcl_FirstListElem(&imports);

    while (elem) {
        nsref = (NamespImportRef*)Itcl_GetListValue(elem);

        entry = Tcl_FirstHashEntry(&nsref->namesp->children, &place);
        while (entry) {
            ns = (Itcl_Namespace)Tcl_GetHashValue(entry);
            if (Itcl_IsClass(ns)) {
                if (nsref->namesp == (Namespace*)activeNs) {
                    name = Itcl_GetNamespName(ns);
                } else {
                    name = Itcl_GetNamespPath(ns);
                }
                if (!pattern || Tcl_StringMatch(name,pattern)) {
                    Tcl_AppendElement(interp, name);
                }
            }
            entry = Tcl_NextHashEntry(&place);
        }
        elem = Itcl_NextListElem(elem);
    }
    Itcl_DeleteList(&imports);

    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InfoObjectsCmd()
 *
 *  Part of the "::info" ensemble.  Invoked by Tcl whenever the user
 *  issues an "info objects" command to query the list of known objects.
 *  Handles the following syntax:
 *
 *    info objects ?-class <className>? ?-isa <className>? ?<pattern>?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
int
Itcl_InfoObjectsCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* class/object info */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclObjectInfo *info = (ItclObjectInfo*)clientData;
    Itcl_Namespace activeNs = Itcl_GetActiveNamesp(interp);

    char *pattern = NULL;
    Itcl_Class *classDefn = NULL;
    Itcl_Class *isaDefn = NULL;

    int pos;
    Itcl_Namespace ns;
    Tcl_HashEntry *entry;
    Tcl_HashSearch place;
    char *name;
    Itcl_Object *obj;
    Itcl_Class *cdefn;
    Itcl_HierIter hier;

    /*
     *  Parse arguments:
     *  objects ?-class <className>? ?-isa <className>? ?<pattern>?
     */
    pos = 0;
    while (++pos < argc) {
        if (*argv[pos] != '-') {
            if (!pattern) {
                pattern = argv[pos];
            } else {
                break;
            }
        }
        else if ((pos+1 < argc) && (strcmp(argv[pos],"-class") == 0)) {
            if (Itcl_FindClass(interp, argv[pos+1], &ns) != TCL_OK) {
                return TCL_ERROR;
            }
            if (ns == NULL) {
                Tcl_AppendResult(interp, "class \"", argv[pos+1],
                    "\" not found in context \"",
                    Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
                    (char*)NULL);
                return TCL_ERROR;
            }
            classDefn = (Itcl_Class*)Itcl_GetNamespData(ns);
            pos++;
        }
        else if ((pos+1 < argc) && (strcmp(argv[pos],"-isa") == 0)) {
            if (Itcl_FindClass(interp, argv[pos+1], &ns) != TCL_OK) {
                return TCL_ERROR;
            }
            if (ns == NULL) {
                Tcl_AppendResult(interp, "class \"", argv[pos+1],
                    "\" not found in context \"",
                    Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
                    (char*)NULL);
                return TCL_ERROR;
            }
            isaDefn = (Itcl_Class*)Itcl_GetNamespData(ns);
            pos++;
        }
        else {
            break;
        }
    }

    if (pos < argc) {
        Tcl_AppendResult(interp, "wrong # args: should be \"",
            Itcl_EnsembleInvoc(),
            " ?-class className? ?-isa className? ?pattern?\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Scan through the list of all known objects and search for
     *  those whose name matches the given pattern, and who class
     *  info matches the rest of the specification.
     */
    entry = Tcl_FirstHashEntry(&info->objects,&place);
    while (entry) {
        obj = (Itcl_Object*)Tcl_GetHashValue(entry);

        if (obj && obj->accessCmd) {
            if (Itcl_GetCommandNamesp(obj->accessCmd) == activeNs) {
                name = Tcl_GetCommandName(interp, obj->accessCmd);
            } else {
                name = Tcl_GetCommandPath(interp, obj->accessCmd);
            }

            if (!pattern || Tcl_StringMatch(name,pattern)) {
                if (!classDefn || (obj->cdefn == classDefn)) {
                    if (!isaDefn) {
                        Tcl_AppendElement(interp, name);
                    }
                    else {
                        Itcl_InitHierIter(&hier,obj->cdefn);
                        while ((cdefn=Itcl_AdvanceHierIter(&hier)) != NULL) {
                            if (isaDefn == cdefn)
                                break;
                        }
                        Itcl_DeleteHierIter(&hier);

                        if (cdefn) {
                            Tcl_AppendElement(interp, name);
                        }
                    }
                }
            }
        }
        entry = Tcl_NextHashEntry(&place);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_DelClassCmd()
 *
 *  Part of the "delete" ensemble.  Invoked by Tcl whenever the
 *  user issues a "delete class" command to delete classes.
 *  Handles the following syntax:
 *
 *    delete class <name> ?<name>...?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_DelClassCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    int i;
    Itcl_Namespace ns;

    /*
     *  Since destroying a base class will destroy all derived
     *  classes, calls like "destroy class Base Derived" could
     *  fail.  Break this into two passes:  first check to make
     *  sure that all classes on the command line are valid,
     *  then delete them.
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
            if (Itcl_DeleteClass(interp, ns) != TCL_OK) {
                return TCL_ERROR;
            }
        }
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_DelObjectCmd()
 *
 *  Part of the "delete" ensemble.  Invoked by Tcl whenever the user
 *  issues a "delete object" command to delete [incr Tcl] objects.
 *  Handles the following syntax:
 *
 *    delete object <name> ?<name>...?
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
int
Itcl_DelObjectCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* object management info */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    int i;
    Itcl_Object *obj;

    /*
     *  Scan through the list of objects and attempt to delete them.
     *  If anything goes wrong (i.e., destructors fail), then
     *  abort with an error.
     */
    for (i=1; i < argc; i++) {
        if (Itcl_FindObject(interp, argv[i], &obj) != TCL_OK) {
            return TCL_ERROR;
        }
        if (obj == NULL) {
            Tcl_AppendResult(interp, "object \"", argv[i], "\" not found",
                (char*)NULL);
            return TCL_ERROR;
        }

        if (Itcl_DeleteObject(interp, obj) != TCL_OK) {
            return TCL_ERROR;
        }
    }
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
Itcl_AtScopeCmd2(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
  Interp* iPtr = (Interp*)interp;
  Itcl_ActiveNamespace nsToken, csToken;
  int i, status;
  Itcl_Namespace ns;
  Tcl_DString buffer;
  char *cmdstr;
  Itcl_Object*objPtr;
  CallFrame frame;
  ItclClass *cdPtr;
  Itcl_HierIter hier;
  
  if (argc < 3) {
    Tcl_AppendResult(interp, "wrong # args: should be \"",
		     argv[0], " name name command ?arg arg ...?\"",
		     (char*)NULL);
    return TCL_ERROR;
  }
  
  /*
   *  Look for the specified namespace, and if it is not found,
   *  return an error.
   */
  if (Itcl_FindObject(interp, argv[1], &objPtr) != TCL_OK) {
    return TCL_ERROR;
  }
  
  if (objPtr == NULL) {
    Tcl_AppendResult(interp,"object \"",
		     argv[1], "\" not found in context \"",
		     Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
		     (char*)NULL);
    return TCL_ERROR;
  }
  
  if (Itcl_FindNamesp(interp, argv[2], 0, &ns) != TCL_OK) {
    return TCL_ERROR;
  }
  if (ns == NULL) {
        Tcl_AppendResult(interp, "namespace \"",
            argv[2], "\" not found in context \"",
            argv[1], "\"",
            (char*)NULL);
        return TCL_ERROR;
    }
  
  /*
   *  Load the command string into the buffer, and if there
   *  are any extra arguments, append them onto the command
   *  string as list elements.
   */
  if (argc > 4) {
    Tcl_DStringInit(&buffer);
    Tcl_DStringAppend(&buffer, argv[3], -1);
    for (i=4; i < argc; i++) {
      Tcl_DStringAppendElement(&buffer, argv[i]);
        }
    cmdstr = Tcl_DStringValue(&buffer);
  }
  else {
    cmdstr = argv[3];
  }
  
  /*
   *  Activate the desired namespace and evaluate the commands
   *  within it.
   */
    nsToken = Itcl_ActivateNamesp2(interp, ns, (ClientData) objPtr);
  if (nsToken == NULL) {
    return TCL_ERROR;
  }
  
  if (iPtr->varFramePtr != NULL) {
        frame.level = iPtr->varFramePtr->level + 1;
  } else {
    frame.level = 1;
  }
  frame.argc = 0;
  frame.argv = NULL;
  frame.callerPtr    = iPtr->framePtr;
  frame.callerVarPtr = iPtr->varFramePtr;
  frame.activeNs     = iPtr->activeNs;
  frame.activeData   = iPtr->activeData;
  frame.flags        = ITCL_GLOBAL_VAR;
  iPtr->framePtr     = &frame;
  iPtr->varFramePtr  = &frame;
  iPtr->returnCode   = TCL_OK;
  
  status = Tcl_Eval(interp, cmdstr);
  
  Itcl_DeactivateNamesp(interp,nsToken);
  iPtr->framePtr = frame.callerPtr;
  iPtr->varFramePtr = frame.callerVarPtr;
  
  if (status != TCL_OK) {
    char mesg[512];
        sprintf(mesg, "\n    (in namespace \"%.400s\" body line %d)",
		Itcl_GetNamespPath(ns), interp->errorLine);
    Tcl_AddErrorInfo(interp, mesg);
  }
  
  if (argc > 4) {
    Tcl_DStringFree(&buffer);
  }
  return status;
}
