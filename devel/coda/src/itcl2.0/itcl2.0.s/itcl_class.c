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
 *  These procedures handle class definitions.  Classes are composed of
 *  data members (public/protected/common) and the member functions
 *  (methods/procs) that operate on them.  Each class has its own
 *  namespace which manages the class scope.
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl_class.c,v 1.1.1.1 1996/08/21 19:29:43 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include <assert.h>
#include "itclInt.h"

static char rcsid[] = "$Id: itcl_class.c,v 1.1.1.1 1996/08/21 19:29:43 heyes Exp $";

/*
 *  FORWARD DECLARATIONS
 */
static void ItclDestroyClass _ANSI_ARGS_((ClientData cdata));
static void ItclDestroyClassNamesp _ANSI_ARGS_((ClientData cdata));
static void ItclFreeClass _ANSI_ARGS_((ClientData cdata));

static Tcl_Var ItclResolveVar _ANSI_ARGS_((ItclVarLookup *vlookup,
    Itcl_Class *cdefn, int flags));


/*
 * ------------------------------------------------------------------------
 *  Itcl_CreateClass()
 *
 *  Creates a namespace and its associated class definition data.
 *  If a namespace already exists with that name, then this routine
 *  returns TCL_ERROR, along with an error message in the interp.
 *  If successful, it returns TCL_OK and a pointer to the new class
 *  namespace.
 * ------------------------------------------------------------------------
 */
int
Itcl_CreateClass(interp,path,info,nsPtr)
    Tcl_Interp* interp;      /* interpreter that will contain new class */
    char* path;              /* name of new class */
    ItclObjectInfo *info;    /* info for all known objects */
    Itcl_Namespace* nsPtr;   /* returns: pointer to class namespace */
{
    int status;
    char *head, *tail;
    Tcl_Command cmd;
    Itcl_Namespace ns;
    Itcl_ActiveNamespace nsToken;
    ItclClass *cdefnPtr;
    ItclVarDefn *vdefn;
    Tcl_HashEntry *entry;
    int newEntry;

    /*
     *  Make sure that a namespace with the given name does not
     *  already exist.  Look only in the local namespace for
     *  the given path--avoid imported namespaces.
     */
    if (Itcl_FindNamesp(interp, path, ITCL_FIND_LOCAL_ONLY, &ns) != TCL_OK) {
        return TCL_ERROR;
    }
    if (ns != NULL) {
        Tcl_AppendResult(interp, (Itcl_IsClass(ns)) ? "class" : "namespace",
            " \"", path, "\" already exists",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Make sure that a command with the given class name does not already
     *  exist.  This prevents the usual Tcl commands from being clobbered
     *  when the user makes a bogus call like "class info".  Look only in
     *  the local namespace for the given path--avoid imported namespaces.
     */
    if (Itcl_FindCommand(interp,path,ITCL_FIND_LOCAL_ONLY,&cmd) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cmd != NULL) {
        Tcl_AppendResult(interp, "command \"", path,
            "\" already exists",
            (char*)NULL);
        if (strstr(path,"::") == NULL) {
            Tcl_AppendResult(interp, " in namespace \"",
                Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
                (char*)NULL);
        }
        return TCL_ERROR;
    }

    /*
     *  Make sure that the class name does not have any goofy
     *  characters:
     *
     *    .  =>  reserved for member access like:  class.publicVar
     */
    Itcl_ParseNamespPath(path, &head, &tail);
    if (strstr(tail,".")) {
        Tcl_AppendResult(interp, "bad class name \"", tail, "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Allocate class definition data.
     */
    cdefnPtr = (ItclClass*)ckalloc(sizeof(ItclClass));
    cdefnPtr->name = NULL;
    cdefnPtr->interp = interp;
    cdefnPtr->info = info;  Itcl_PreserveData((ClientData)info);
    cdefnPtr->namesp = NULL;
    cdefnPtr->accessCmd = NULL;

    Tcl_InitHashTable(&cdefnPtr->variables,TCL_STRING_KEYS);

    cdefnPtr->instanceVars = 0;
    Tcl_InitHashTable(&cdefnPtr->virtualData,TCL_STRING_KEYS);
    Tcl_InitHashTable(&cdefnPtr->virtualMethods,TCL_STRING_KEYS);

    Itcl_InitList(&cdefnPtr->bases);
    Itcl_InitList(&cdefnPtr->derived);

    cdefnPtr->initCmd = NULL;
    cdefnPtr->unique  = 0;
    cdefnPtr->flags   = 0;

    /*
     *  Create a namespace to represent the class.  Add the class
     *  definition info as extra data for the namespace.
     */
    status = Itcl_CreateNamesp(interp, path,
        Itcl_PreserveData((ClientData)cdefnPtr), ItclDestroyClassNamesp, &ns);

    Itcl_EventuallyFree((ClientData)cdefnPtr, ItclFreeClass);

    if (status != TCL_OK) {
        Itcl_ReleaseData((ClientData)cdefnPtr);
        return TCL_ERROR;
    }
    cdefnPtr->name = Itcl_GetNamespName(ns);
    cdefnPtr->namesp = ns;  Itcl_PreserveData((ClientData)ns);

    /*
     *  Add an "unknown" command to the class namespace, to handle
     *  access to objects in "foreign" namespaces.
     */
    nsToken = Itcl_ActivateNamesp(interp, ns);

    Tcl_CreateCommand(interp, "unknown", Itcl_ClassUnknownCmd,
        Itcl_PreserveData((ClientData)cdefnPtr), Itcl_ReleaseData);

    Itcl_DeactivateNamesp(interp, nsToken);

    /*
     *  Set up the variable enforcer for the namespace, to
     *  control access to class data.
     */
    Itcl_SetVarEnforcer(ns, Itcl_ClassVarEnforcer);

    /*
     *  Add the built-in "this" variable to the list of data members.
     */
    Itcl_CreateVarDefn(interp, (Itcl_Class*)cdefnPtr, "this",
        (char*)NULL,(char*)NULL, &vdefn);

    vdefn->protection = ITCL_PROTECTED;  /* always "protected" */
    vdefn->flags |= ITCL_THIS_VAR;       /* mark as "this" variable */

    entry = Tcl_CreateHashEntry(&cdefnPtr->variables, "this", &newEntry);
    Tcl_SetHashValue(entry, (ClientData)vdefn);

    /*
     *  Create a command in the active namespace to manage the class:
     *    <className>
     *    <className> <objName> ?<constructor-args>?
     */
    cdefnPtr->accessCmd = Tcl_CreateCommand(interp, path,
        Itcl_HandleClass,
        Itcl_PreserveData((ClientData)cdefnPtr), ItclDestroyClass);

    *nsPtr = ns;
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_DeleteClass()
 *
 *  Deletes a class by deleting all derived classes and all objects in
 *  that class, and finally, by destroying the class namespace.  This
 *  procedure provides a friendly way of doing this.  If any errors
 *  are detected along the way, the process is aborted.
 *
 *  Returns TCL_OK if successful, or TCL_ERROR (along with an error
 *  message in the interpreter) if anything goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_DeleteClass(interp, ns)
    Tcl_Interp *interp;   /* interpreter managing this class */
    Itcl_Namespace ns;    /* class namespace */
{
    ItclClass *cdefnPtr = (ItclClass*)Itcl_GetNamespData(ns);
    ItclClass *cdPtr = NULL;

    Itcl_ListElem *elem;
    ItclObject *objPtr;
    Tcl_HashEntry *entry;
    Tcl_HashSearch place;
    Tcl_DString buffer;

    /*
     *  If this is not a class namespace, return an error.
     */
    if (!Itcl_IsClass(ns)) {
        Tcl_AppendResult(interp, "can't delete class \"",
            Itcl_GetNamespPath(ns), "\": not a class namespace",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Destroy all derived classes, since these lose their meaning
     *  when the base class goes away.  If anything goes wrong,
     *  abort with an error.
     *
     *  TRICKY NOTE:  When a derived class is destroyed, it
     *    automatically deletes itself from the "derived" list.
     */
    elem = Itcl_FirstListElem(&cdefnPtr->derived);
    while (elem) {
        cdPtr = (ItclClass*)Itcl_GetListValue(elem);
        elem = Itcl_NextListElem(elem);  /* advance here--elem will go away */

        if (Itcl_DeleteClass(interp, cdPtr->namesp) != TCL_OK) {
            goto deleteClassFail;
        }
    }

    /*
     *  Scan through and find all objects that belong to this class.
     *  Note that more specialized objects have already been
     *  destroyed above, when derived classes were destroyed.
     *  Destroy objects and report any errors.
     */
    entry = Tcl_FirstHashEntry(&cdefnPtr->info->objects, &place);
    while (entry) {
        objPtr = (ItclObject*)Tcl_GetHashValue(entry);
        if (objPtr->cdefn == cdefnPtr) {
            if (Itcl_DeleteObject(interp, (Itcl_Object*)objPtr) != TCL_OK) {
                cdPtr = cdefnPtr;
                goto deleteClassFail;
            }
        }
        entry = Tcl_NextHashEntry(&place);
    }

    /*
     *  Destroy the namespace associated with this class.
     *
     *  TRICKY NOTE:
     *    The cleanup procedure associated with the namespace is
     *    invoked automatically.  It does all of the same things
     *    above, but it also disconnects this class from its
     *    base-class lists, and removes the class access command.
     */
    Itcl_DeleteNamesp(cdefnPtr->namesp);
    return TCL_OK;

deleteClassFail:
    Tcl_DStringInit(&buffer);
    Tcl_DStringAppend(&buffer, "\n    (while deleting class \"",-1);
    Tcl_DStringAppend(&buffer, Itcl_GetNamespPath(cdPtr->namesp),-1);
    Tcl_DStringAppend(&buffer, "\")", -1);
    Tcl_AddErrorInfo(interp, Tcl_DStringValue(&buffer));
    Tcl_DStringFree(&buffer);
    return TCL_ERROR;
}


/*
 * ------------------------------------------------------------------------
 *  ItclDestroyClass()
 *
 *  Invoked whenever the access command for a class is destroyed.
 *  Destroys the namespace associated with the class, which also
 *  destroys all objects in the class and all derived classes.
 *  Disconnects this class from the "derived" class lists of its
 *  base classes, and releases any claim to the class definition
 *  data.  If this is the last use of that data, the class will
 *  completely vanish at this point.
 * ------------------------------------------------------------------------
 */
static void
ItclDestroyClass(cdata)
    ClientData cdata;  /* class definition to be destroyed */
{
    ItclClass *cdefnPtr = (ItclClass*)cdata;
    cdefnPtr->accessCmd = NULL;

    Itcl_DeleteNamesp(cdefnPtr->namesp);
    Itcl_ReleaseData((ClientData)cdefnPtr);
}


/*
 * ------------------------------------------------------------------------
 *  ItclDestroyClassNamesp()
 *
 *  Invoked whenever the namespace associated with a class is destroyed.
 *  Destroys all objects associated with this class and all derived
 *  classes.  Disconnects this class from the "derived" class lists
 *  of its base classes, and removes the class access command.  Releases
 *  any claim to the class definition data.  If this is the last use
 *  of that data, the class will completely vanish at this point.
 * ------------------------------------------------------------------------
 */
static void
ItclDestroyClassNamesp(cdata)
    ClientData cdata;  /* class definition to be destroyed */
{
    ItclClass *cdefnPtr = (ItclClass*)cdata;
    ItclObject *objPtr;
    Itcl_ListElem *elem, *belem;
    ItclClass *cdPtr, *basePtr, *derivedPtr;
    Tcl_HashEntry *entry;
    Tcl_HashSearch place;

    /*
     *  Destroy all derived classes, since these lose their meaning
     *  when the base class goes away.
     *
     *  TRICKY NOTE:  When a derived class is destroyed, it
     *    automatically deletes itself from the "derived" list.
     */
    elem = Itcl_FirstListElem(&cdefnPtr->derived);
    while (elem) {
        cdPtr = (ItclClass*)Itcl_GetListValue(elem);
        elem = Itcl_NextListElem(elem);  /* advance here--elem will go away */

        Itcl_DeleteNamesp(cdPtr->namesp);
    }

    /*
     *  Scan through and find all objects that belong to this class.
     *  Destroy them quietly by deleting their access command.
     */
    entry = Tcl_FirstHashEntry(&cdefnPtr->info->objects, &place);
    while (entry) {
        objPtr = (ItclObject*)Tcl_GetHashValue(entry);
        if (objPtr->cdefn == cdefnPtr) {
            Tcl_DeleteCommand2(cdefnPtr->interp, objPtr->accessCmd);
        }
        entry = Tcl_NextHashEntry(&place);
    }

    /*
     *  Next, remove this class from the "derived" list in
     *  all base classes.
     */
    belem = Itcl_FirstListElem(&cdefnPtr->bases);
    while (belem) {
        basePtr = (ItclClass*)Itcl_GetListValue(belem);

        elem = Itcl_FirstListElem(&basePtr->derived);
        while (elem) {
            derivedPtr = (ItclClass*)Itcl_GetListValue(elem);
            if (derivedPtr == cdefnPtr) {
                Itcl_ReleaseData( Itcl_GetListValue(elem) );
                elem = Itcl_DeleteListElem(elem);
            } else {
                elem = Itcl_NextListElem(elem);
            }
        }
        belem = Itcl_NextListElem(belem);
    }

    /*
     *  Next, destroy the access command associated with the class.
     */
    if (cdefnPtr->accessCmd) {
        Command *cmdPtr = (Command*)cdefnPtr->accessCmd;

        cmdPtr->deleteProc = Itcl_ReleaseData;
        Tcl_DeleteCommand2(cdefnPtr->interp, cdefnPtr->accessCmd);
    }

    /*
     *  Release the namespace's claim on the class definition.
     */
    Itcl_ReleaseData((ClientData)cdefnPtr);
}


/*
 * ------------------------------------------------------------------------
 *  ItclFreeClass()
 *
 *  Frees all memory associated with a class definition.  This is
 *  usually invoked automatically by Itcl_ReleaseData(), when class
 *  data is no longer being used.
 * ------------------------------------------------------------------------
 */
static void
ItclFreeClass(cdata)
    ClientData cdata;  /* class definition to be destroyed */
{
    ItclClass *cdefnPtr = (ItclClass*)cdata;

    Itcl_ListElem *elem;
    Tcl_HashTable classvars;
    Tcl_HashSearch place;
    Tcl_HashEntry *entry;
    ItclVarDefn *vdefn;
    ItclVarLookup *vlookup;
    Var *varPtr;
    int newEntry;

    /*
     *  Tear down the list of derived classes.  This list should
     *  really be empty if everything is working properly, but
     *  release it here just in case.
     */
    elem = Itcl_FirstListElem(&cdefnPtr->derived);
    while (elem) {
        Itcl_ReleaseData( Itcl_GetListValue(elem) );
        elem = Itcl_NextListElem(elem);
    }
    Itcl_DeleteList(&cdefnPtr->derived);

    /*
     *  The global variable list for the class namespace has
     *  already been deleted, but the class variables remain,
     *  since they are protected by an extra reference count.
     *  Build a phony variable table here to clean up common
     *  class variables.  Be careful to avoid inherited data
     *  members that are also sitting in the virtualData table.
     */
    Tcl_InitHashTable(&classvars, TCL_STRING_KEYS);

    entry = Tcl_FirstHashEntry(&cdefnPtr->virtualData, &place);
    while (entry) {
        vlookup = (ItclVarLookup*)Tcl_GetHashValue(entry);

        if ((vlookup->vdefn->flags & ITCL_COMMON) != 0 &&
            vlookup->vdefn->cdefn == cdefnPtr) {

            varPtr = (Var*)vlookup->var;
            if (varPtr) {
                entry = Tcl_CreateHashEntry(&classvars,
                    vlookup->vdefn->name, &newEntry);
                Tcl_SetHashValue(entry, (ClientData)varPtr);

                varPtr->hPtr = entry;
                varPtr->refCount--;
            }
            vlookup->var = NULL;
        }
        entry = Tcl_NextHashEntry(&place);
    }
    TclDeleteVars((Interp*)cdefnPtr->interp, &classvars);

    /*
     *  Tear down the virtual data table...
     */
    entry = Tcl_FirstHashEntry(&cdefnPtr->virtualData, &place);
    while (entry) {
        vlookup = (ItclVarLookup*)Tcl_GetHashValue(entry);
        if (--vlookup->usage == 0) {
            ckfree((char*)vlookup);
        }
        entry = Tcl_NextHashEntry(&place);
    }
    Tcl_DeleteHashTable(&cdefnPtr->virtualData);

    /*
     *  Tear down the virtual method table...
     */
    entry = Tcl_FirstHashEntry(&cdefnPtr->virtualMethods, &place);
    while (entry) {
        Itcl_ReleaseData(Tcl_GetHashValue(entry));
        entry = Tcl_NextHashEntry(&place);
    }
    Tcl_DeleteHashTable(&cdefnPtr->virtualMethods);

    /*
     *  Delete all variable definitions...
     */
    entry = Tcl_FirstHashEntry(&cdefnPtr->variables, &place);
    while (entry) {
        vdefn = (ItclVarDefn*)Tcl_GetHashValue(entry);
        Itcl_DeleteVarDefn(vdefn);
        entry = Tcl_NextHashEntry(&place);
    }
    Tcl_DeleteHashTable(&cdefnPtr->variables);

    /*
     *  Release the claim on all base classes.
     */
    elem = Itcl_FirstListElem(&cdefnPtr->bases);
    while (elem) {
        Itcl_ReleaseData( Itcl_GetListValue(elem) );
        elem = Itcl_NextListElem(elem);
    }
    Itcl_DeleteList(&cdefnPtr->bases);

    /*
     *  Free up the object initialization code.
     */
    if (cdefnPtr->initCmd) {
        ckfree((char*)cdefnPtr->initCmd);
    }

    Itcl_ReleaseData((ClientData)cdefnPtr->info);
    Itcl_ReleaseData((ClientData)cdefnPtr->namesp);

    ckfree((char*)cdefnPtr);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_IsClass()
 *
 *  Checks to see whether or not the given namespace represents an
 *  [incr Tcl] class.  Returns non-zero if so, and zero otherwise.
 * ------------------------------------------------------------------------
 */
int
Itcl_IsClass(ns)
    Itcl_Namespace ns;  /* namespace being tested */
{
    Namespace *nsPtr = (Namespace*)ns;

    if (nsPtr != NULL) {
        return (nsPtr->deleteProc == ItclDestroyClassNamesp);
    }
    return 0;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_FindClass()
 *
 *  Searches for the specified class in the active namespace.  Returns
 *  TCL_ERROR (along with an error in the associated interp) if anything
 *  goes wrong; otherwise, returns TCL_OK.  If a class namespace with
 *  the specified name can be found, then a pointer to that namespace
 *  is returned in nsPtr.  Otherwise, nsPtr returns NULL.
 * ------------------------------------------------------------------------
 */
int
Itcl_FindClass(interp,path,nsPtr)
    Tcl_Interp* interp;      /* interpreter containing class */
    char* path;              /* path name for class */
    Itcl_Namespace* nsPtr;   /* returns: class namespace or NULL */
{
    Itcl_Namespace ns;

    /*
     *  Search for a namespace with the specified name, and if
     *  one is found, see if it is a class namespace.
     */
    if (Itcl_FindNamesp(interp, path, 0, &ns) != TCL_OK) {
        return TCL_ERROR;
    }

    if (ns && Itcl_IsClass(ns))
        *nsPtr = ns;
    else
        *nsPtr = NULL;

    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_HandleClass()
 *
 *  Invoked by Tcl whenever the user issues the command associated with
 *  a class name.  Handles the following syntax:
 *
 *    <className>
 *    <className> <objName> ?<args>...?
 *
 *  Without any arguments, the command does nothing; this allows the
 *  class name to be invoked by itself to prompt the autoloader to
 *  load the class definition.  Otherwise, it creates a new object
 *  named <objName> and returns its name.  Note that if <objName>
 *  contains "#auto", that part is automatically replaced by a unique
 *  string built from the class name.
 * ------------------------------------------------------------------------
 */
int
Itcl_HandleClass(clientData, interp, argc, argv)
    ClientData clientData;   /* class definition */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    ItclClass *cdefnPtr = (ItclClass*)clientData;
    int status = TCL_OK;

    char unique[256];    /* buffer used to unique part of object names */
    Tcl_DString buffer;  /* buffer used to build object names */
    char *objName, tmp, *start, *pos, *match;
    char *cmdStr;

    Itcl_Object *obj;
    Itcl_ActiveNamespace nsToken;

    /*
     *  If the command is invoked without an object name, then do nothing.
     *  This supports autoloading, so that the class name can be invoked
     *  as a command by itself, prompting the autoloader to load the
     *  class definition.
     */
    if (argc == 1) {
        return TCL_OK;
    }

    /*
     *  If the object name is "::", and if this is an old-style class
     *  definition, then treat the remaining arguments as a command
     *  in the class namespace.  This used to be the way of invoking
     *  a class proc, but the new syntax is "class::proc" (without
     *  spaces).
     */
    if ((*argv[1] == ':') && (strcmp(argv[1],"::") == 0) && (argc > 2)) {
        if ((cdefnPtr->flags & ITCL_OLD_STYLE) != 0) {
            nsToken = Itcl_ActivateNamesp(interp, cdefnPtr->namesp);
            cmdStr = Tcl_Merge(argc-2,argv+2);

            status = Itcl_EvalArgs(interp, cmdStr, (char*)NULL, argc-2,argv+2);

            ckfree(cmdStr);
            Itcl_DeactivateNamesp(interp, nsToken);

            return status;
        }

        /*
         *  If this is not an old-style class, then return an error
         *  describing the syntax change.
         */
        Tcl_AppendResult(interp,
            "syntax \"class :: proc\" is an anachronism\n",
            "[incr Tcl] no longer supports this syntax.\n",
            "Instead, remove the spaces from your procedure invocations:\n",
            "  ", argv[0], "::", argv[2], " ?args?",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Otherwise, create a new object instance.  The first argument
     *  is the object name.  If it contains "#auto", replace this with
     *  a uniquely generated string based on the class name.
     */
    Tcl_DStringInit(&buffer);
    objName = NULL;

    match = "#auto";
    start = argv[1];
    for (pos=start; *pos != '\0'; pos++) {
        if (*pos == *match) {
            if (*(++match) == '\0') {
                tmp = *start;
                *start = '\0';  /* null-terminate first part */

                /*
                 *  Substitute a unique part in for "#auto", and keep
                 *  incrementing a counter until a valid name is found.
                 */
                do {
		  char *p = unique;
		  sprintf(unique,"%.200s%d", cdefnPtr->name,
			  cdefnPtr->unique++);
		  
		  for (; *p != 0; p++) {
		    if (isupper(UCHAR(*p))) {
		      *p = tolower(UCHAR(*p));
		    }
		  }
		  
                    Tcl_DStringTrunc(&buffer, 0);
                    Tcl_DStringAppend(&buffer, argv[1], -1);
                    Tcl_DStringAppend(&buffer, unique, -1);
                    Tcl_DStringAppend(&buffer, start+5, -1);

                    objName = Tcl_DStringValue(&buffer);
                    if (Itcl_FindObject(interp, objName, &obj) != TCL_OK) {
                        break;  /* if an error is found, bail out! */
                    }
                } while (obj != NULL);

                *start = tmp;       /* undo null-termination */
                objName = Tcl_DStringValue(&buffer);
                break;              /* object name is ready to go! */
            }
        }
        else {
            match = "#auto";
            pos = start++;
        }
    }

    /*
     *  If "#auto" was not found, then just use object name as-is.
     */
    if (objName == NULL) {
        objName = argv[1];
    }

    /*
     *  Try to create a new object.  If successful, return the
     *  object name as the result of this command.
     */
    status = Itcl_CreateObject(interp, objName, (Itcl_Class*)cdefnPtr,
        argc, argv, &obj);

    if (status == TCL_OK) {
        Tcl_SetResult(interp, objName, TCL_VOLATILE);
    }

    Tcl_DStringFree(&buffer);
    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ClassVarEnforcer()
 *
 *  Used by the class namespaces as an enforcer for variable accesses.
 *  An "enforcer" is a procedure which is consulted whenever a variable
 *  cannot be found in the local cache.  It can deny access to certain
 *  variables, or perform special lookups itself.
 *
 *  This procedure is used whenever variables are accessed in the
 *  context of a class scope.  It looks for references to both common
 *  and object-specific variables, and when they are recognized,
 *  returns them.  If a variable is not recognized, the enforcer
 *  lets it pass through, so that lookup will continue in the local
 *  call frame or in the namespace scope.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_ClassVarEnforcer(interp, name, varPtr, flags)
    Tcl_Interp *interp;    /* current interpreter */
    char* name;            /* name of the variable being accessed */
    Tcl_Var* varPtr;       /* returns Tcl_Var for desired variable or NULL */
    int flags;             /* ITCL_GLOBAL_VAR => global variable desired */
{
    Itcl_Namespace ns = Itcl_GetActiveNamesp(interp);
    ItclClass *cdefnPtr = (ItclClass*)Itcl_GetNamespData(ns);

    Tcl_HashEntry *entry;
    ItclVarLookup *vlookup;
    char *className, *varName;
    Itcl_HierIter hier;
    Itcl_Class *cd, *varCd;

    /*
     *  Look for the requested variable in the virtual table.
     *  If the name is recognized, then find the associated
     *  instance variable or common variable.
     */
    entry = Tcl_FindHashEntry(&cdefnPtr->virtualData, name);
    if (entry) {
        vlookup = (ItclVarLookup*)Tcl_GetHashValue(entry);
        if (vlookup->accessible) {
            *varPtr = ItclResolveVar(vlookup, (Itcl_Class*)cdefnPtr, flags);
        } else {
            *varPtr = NULL;
        }
        return TCL_OK;
    }

    /*
     *  If the variable name was not recognized right away,
     *  then try to parse it as "namesp::class::slot".  Then
     *  look for the simple "slot" name in that class.
     */
    Itcl_ParseNamespPath(name, &className, &varName);
    if (className) {
        if (Itcl_FindNamesp(interp, className, 0, &ns) != TCL_OK) {
            return TCL_ERROR;
        }
        if (ns != NULL && Itcl_IsClass(ns)) {
            varCd = (Itcl_Class*)Itcl_GetNamespData(ns);

            /*
             *  Make sure that the class we found is in this
             *  inheritance hierarchy.
             */
            Itcl_InitHierIter(&hier, (Itcl_Class*)cdefnPtr);
            while ((cd=Itcl_AdvanceHierIter(&hier)) != NULL) {
                if (cd == varCd) {
                    break;
                }
            }
            Itcl_DeleteHierIter(&hier);
        }
    }

    /*
     *  Look for the slot in the class virtual table,
     *  and resolve the variable reference.
     */
    if (cdefnPtr != NULL) {
        entry = Tcl_FindHashEntry(&cdefnPtr->virtualData, varName);
        if (entry) {
            vlookup = (ItclVarLookup*)Tcl_GetHashValue(entry);
            if (vlookup->accessible) {
                *varPtr = ItclResolveVar(vlookup, (Itcl_Class*)cdefnPtr, flags);
            } else {
                *varPtr = NULL;
            }
            return TCL_OK;
        }
    }

    /*
     *  If this variable is not recognized as a data member,
     *  return a NULL pointer so that lookup will continue.
     */
    *varPtr = NULL;
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ClassUnknownCmd()
 *
 *  Invoked automatically whenever an unknown command is encountered
 *  within a class namespace.  Checks to see if it is an object
 *  access command for an object belonging to the class.  If so,
 *  then this procedure provides transparent access to the object.
 *  This is needed, since an object might be created as "private"
 *  or "protected" in another namespace, but the class should still
 *  be able to access it.
 *
 *  If the command is not a recognized object, then the unknown
 *  referenced is passed along to the parent namespace for further
 *  evaluation.
 *
 *  Evaluates the unknown command (if possible) and returns TCL_OK
 *  on success, or TCL_ERROR and an error message in the interpreter.
 * ------------------------------------------------------------------------
 */
int
Itcl_ClassUnknownCmd(clientData, interp, argc, argv)
    ClientData clientData;   /* class definition */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Itcl_Class *cdefn = (Itcl_Class*)clientData;

    int status;
    Itcl_Object *obj;
    Command *cmdPtr;
    Itcl_ActiveNamespace nsToken;
    char *cmdStr;

    /*
     *  See if the command is a recognized object belonging to
     *  this class.
     */
    if (Itcl_FindObject(interp, argv[1], &obj) != TCL_OK) {
        return TCL_ERROR;
    }
    if (obj && Itcl_ObjectIsa(obj,cdefn) && obj->accessCmd) {
        cmdPtr = (Command*)obj->accessCmd;
        return (*cmdPtr->proc)(cmdPtr->clientData, interp, argc-1, argv+1);
    }

    /*
     *  Otherwise, try using "unknown" in the parent namespace.
     */
    nsToken = Itcl_ActivateNamesp(interp,
        Itcl_GetNamespParent(cdefn->namesp));

    cmdStr = Tcl_Merge(argc,argv);
    status = Itcl_EvalArgs(interp, cmdStr, (char*)NULL, argc,argv);
    ckfree(cmdStr);

    Itcl_DeactivateNamesp(interp, nsToken);
    return status;
}


/*
 * ------------------------------------------------------------------------
 *  ItclResolveVar()
 *
 *  Used by Itcl_ClassVarEnforcer to resolve a variable reference in
 *  a class virtual table to a real class or instance variable.
 *
 *  This procedure is used whenever variables are accessed in the
 *  context of a class scope.  It looks for references to both common
 *  and object-specific variables, and when they are recognized,
 *  returns them.  If the "flags" argument contains ITCL_GLOBAL_VAR,
 *  then only common class variables are recognized.  If a variable
 *  is not recognized, the enforcer lets it pass through, so that
 *  lookup will continue in the local call frame or in the namespace
 *  scope.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
static Tcl_Var
ItclResolveVar(vlookup, cdefn, flags)
    ItclVarLookup *vlookup;   /* lookup reference from class virtual table */
    Itcl_Class *cdefn;        /* current class context */
    int flags;                /* ITCL_GLOBAL_VAR => global variable desired */
{
    ItclObject *objPtr;
    ItclVarDefn *vdefn;
    Tcl_HashEntry *entry;

    /*
     *  If it is a common data member, then the associated
     *  variable is known directly.
     */
    if (vlookup->var) {
        return vlookup->var;
    }

    /*
     *  Otherwise, if the ITCL_GLOBAL_VAR flag is not set, look
     *  for an object-specific variable in the virtual table.
     *  The variable must be found at the proper index into the
     *  object's data table.  Find the index in the virtual table
     *  of the MOST-SPECIFIC class.
     */
    else if ((flags & ITCL_GLOBAL_VAR) == 0) {
        objPtr = (ItclObject*)Itcl_GetActiveNamespData(cdefn->interp);
        if (objPtr != NULL) {
            if (cdefn != (Itcl_Class*)objPtr->cdefn) {
                vdefn = vlookup->vdefn;
                entry = Tcl_FindHashEntry(&objPtr->cdefn->virtualData,
                    vdefn->fullname);

                if (!entry) {
                    return NULL;
                }

                vlookup = (ItclVarLookup*)Tcl_GetHashValue(entry);
            }
            return (Tcl_Var)objPtr->data[vlookup->vindex];
        }
    }
    return NULL;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_BuildVirtualTables()
 *
 *  Invoked whenever the class heritage changes or members are added or
 *  removed from a class definition to rebuild the virtual lookup
 *  tables.  There are two virtual tables:
 *
 *  DATA MEMBERS:  virtualData
 *    Used primarily in Itcl_ClassVarEnforcer() to quickly resolve
 *    variable references in each class scope.  Stores the full
 *    name ("class::var") and the short name ("var") of each variable,
 *    according to the lookup rules for a particular class context.
 *
 *  METHODS:  virtualMethods
 *    Used primarily in Itcl_ExecMethod() to implement the "virtual"
 *    behavior associated with methods.  When a particular method is
 *    found, the most-specific implementation can be obtained by
 *    looking for the method in this table.
 * ------------------------------------------------------------------------
 */
void
Itcl_BuildVirtualTables(cdefnPtr)
    ItclClass* cdefnPtr;       /* class definition being updated */
{
    Tcl_HashEntry *entry, *hPtr;
    Tcl_HashSearch place;
    ItclVarLookup *vlookup;
    ItclVarDefn *vdefn;
    Command *cmdPtr;
    ItclCmdMember *mdefn;
    Itcl_HierIter hier;
    ItclClass *cdPtr;
    Namespace* nsPtr;
    int newEntry;

    /*
     *  Clear the data table.
     */
    entry = Tcl_FirstHashEntry(&cdefnPtr->virtualData, &place);
    while (entry) {
        vlookup = (ItclVarLookup*)Tcl_GetHashValue(entry);
        if (--vlookup->usage == 0) {
            ckfree((char*)vlookup);
        }
        entry = Tcl_NextHashEntry(&place);
    }
    Tcl_DeleteHashTable(&cdefnPtr->virtualData);
    Tcl_InitHashTable(&cdefnPtr->virtualData, TCL_STRING_KEYS);
    cdefnPtr->instanceVars = 0;

    /*
     *  Set aside the first object-specific slot for the built-in
     *  "this" variable.  Only allocate one of these, even though
     *  there is a definition for "this" in each class scope.
     */
    cdefnPtr->instanceVars++;

    /*
     *  Scan through all classes in the hierarchy, from most to
     *  least specific.  Add a lookup entry for each variable
     *  into the table.
     */
    Itcl_InitHierIter(&hier, (Itcl_Class*)cdefnPtr);
    cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
    while (cdPtr != NULL) {
        entry = Tcl_FirstHashEntry(&cdPtr->variables, &place);
        while (entry) {
            vdefn = (ItclVarDefn*)Tcl_GetHashValue(entry);

            vlookup = (ItclVarLookup*)ckalloc(sizeof(ItclVarLookup));
            vlookup->usage = 0;
            vlookup->vdefn = vdefn;
            vlookup->accessible = 0;

            /*
             *  If this variable is PRIVATE to another class scope,
             *  then mark it as "inaccessible".
             */
            vlookup->accessible = (vdefn->protection != ITCL_PRIVATE ||
                vdefn->cdefn == cdefnPtr);

            /*
             *  If this is a common variable, then keep a reference to
             *  the variable directly.  Otherwise, keep an index into
             *  the object's variable table.
             */
            if ((vdefn->flags & ITCL_COMMON) != 0) {
                nsPtr = (Namespace*)cdPtr->namesp;
                hPtr = Tcl_FindHashEntry(&nsPtr->variables, vdefn->name);
                assert(hPtr != NULL);

                vlookup->var = (Tcl_Var)Tcl_GetHashValue(hPtr);
                vlookup->vindex = 0;
            }
            else {
                /*
                 *  If this is a reference to the built-in "this"
                 *  variable, then its index is "0".  Otherwise,
                 *  add another slot to the end of the table.
                 */
                vlookup->var = NULL;
                if ((vdefn->flags & ITCL_THIS_VAR) != 0)
                    vlookup->vindex = 0;
                else
                    vlookup->vindex = cdefnPtr->instanceVars++;
            }

            /*
             *  Enter these data references into the table, if they
             *  do not already exist:
             *     var
             *     class::var
             */
            entry = Tcl_CreateHashEntry(&cdefnPtr->virtualData, vdefn->fullname,
                &newEntry);

            if (newEntry) {
                Tcl_SetHashValue(entry, (ClientData)vlookup);
                vlookup->usage++;
            }

            entry = Tcl_CreateHashEntry(&cdefnPtr->virtualData, vdefn->name,
                &newEntry);

            if (newEntry) {
                Tcl_SetHashValue(entry, (ClientData)vlookup);
                vlookup->usage++;
            }

            /*
             *  If this record is not needed, free it now.
             */
            if (vlookup->usage == 0) {
                ckfree((char*)vlookup);
            }
            entry = Tcl_NextHashEntry(&place);
        }
        cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
    }
    Itcl_DeleteHierIter(&hier);

    /*
     *  Clear the method table.
     */
    entry = Tcl_FirstHashEntry(&cdefnPtr->virtualMethods, &place);
    while (entry) {
        Itcl_ReleaseData(Tcl_GetHashValue(entry));
        entry = Tcl_NextHashEntry(&place);
    }
    Tcl_DeleteHashTable(&cdefnPtr->virtualMethods);
    Tcl_InitHashTable(&cdefnPtr->virtualMethods, TCL_STRING_KEYS);

    /*
     *  Scan through all classes in the hierarchy, from most to
     *  least specific.  Look for the first (most-specific) definition
     *  of each virtual method, and enter it into the virtual table.
     */
    Itcl_InitHierIter(&hier, (Itcl_Class*)cdefnPtr);
    cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
    while (cdPtr != NULL) {
        nsPtr = (Namespace*)cdPtr->namesp;
        entry = Tcl_FirstHashEntry(&nsPtr->commands, &place);
        while (entry) {
            /*
             *  Command is only a method if it uses the magic
             *  "method" implementation proc.
             */
            cmdPtr = (Command*)Tcl_GetHashValue(entry);
            if (cmdPtr->proc == Itcl_ExecMethod) {
                mdefn = (ItclCmdMember*)cmdPtr->clientData;
                entry = Tcl_CreateHashEntry(&cdefnPtr->virtualMethods,
                    mdefn->name, &newEntry);

                if (newEntry) {
                    Tcl_SetHashValue(entry,
                        Itcl_PreserveData(cmdPtr->clientData));
                }
            }
            entry = Tcl_NextHashEntry(&place);
        }
        cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
    }
    Itcl_DeleteHierIter(&hier);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_CreateVarDefn()
 *
 *  Creates a new class variable definition.  If this is a public
 *  variable, it may have a bit of "config" code that is used to
 *  update the object whenever the variable is modified via the
 *  built-in "configure" method.
 *
 *  Returns TCL_ERROR along with an error message in the specified
 *  interpreter if anything goes wrong.  Otherwise, this returns
 *  TCL_OK and a pointer to the new variable definition in "vdefnPtr".
 * ------------------------------------------------------------------------
 */
int
Itcl_CreateVarDefn(interp,cdefn,name,init,config,vdefnPtr)
    Tcl_Interp *interp;       /* interpreter managing this transaction */
    Itcl_Class* cdefn;        /* class containing this variable */
    char* name;               /* variable name */
    char* init;               /* initial value */
    char* config;             /* code invoked when variable is configured */
    ItclVarDefn** vdefnPtr;   /* returns: new variable definition */
{
    int fullsize;
    ItclVarDefn *vdefn;
    ItclCmdImplement *mimpl;

    /*
     *  If this variable has some "config" code, try to capture
     *  its implementation.
     */
    if (config) {
        if (Itcl_CreateCmdImplement(interp, (char*)NULL, config, &mimpl)
            != TCL_OK) {
            return TCL_ERROR;
        }
        Itcl_PreserveData((ClientData)mimpl);
        Itcl_EventuallyFree((ClientData)mimpl, Itcl_DeleteCmdImplement);
    }
    else {
        mimpl = NULL;
    }
        

    /*
     *  If everything looks good, create the variable definition.
     */
    vdefn = (ItclVarDefn*)ckalloc(sizeof(ItclVarDefn));

    vdefn->name = (char*)ckalloc((unsigned)(strlen(name)+1));
    strcpy(vdefn->name, name);

    fullsize = strlen(cdefn->name) + 2 + strlen(name);
    vdefn->fullname = (char*)ckalloc((unsigned)(fullsize+1));
    strcpy(vdefn->fullname, cdefn->name);
    strcat(vdefn->fullname, "::");
    strcat(vdefn->fullname, name);

    vdefn->flags = 0;

    if (init) {
        vdefn->init = (char*)ckalloc((unsigned)(strlen(init)+1));
        strcpy(vdefn->init, init);
    }
    else {
        vdefn->init = NULL;
    }

    vdefn->config = mimpl;
    vdefn->cdefn = (ItclClass*)cdefn;
    vdefn->protection = Itcl_VarProtection(cdefn->interp, 0);

    *vdefnPtr = vdefn;
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_DeleteVarDefn()
 *
 *  Destroys a variable definition created by Itcl_CreateVarDefn(),
 *  freeing all resources associated with it.
 * ------------------------------------------------------------------------
 */
void
Itcl_DeleteVarDefn(vdefn)
    ItclVarDefn *vdefn;   /* variable definition to be destroyed */
{
    ckfree(vdefn->name);
    ckfree(vdefn->fullname);

    if (vdefn->init) {
        ckfree(vdefn->init);
    }
    if (vdefn->config) {
        Itcl_ReleaseData((ClientData)vdefn->config);
    }

    ckfree((char*)vdefn);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_FindVarDefn()
 *
 *  Searches a class definition for a particular variable.  Recognizes
 *  simple variable names and the full-blown "namesp::class::var"
 *  notation.
 *
 *  If any errors are encountered, this procedure returns TCL_ERROR
 *  along with an error message in the interpreter.  Otherwise, it
 *  returns TCL_OK.  If the variable definition was found, then
 *  "vdefnPtr" returns a pointer to it.  Otherwise, "vdefnPtr" returns
 *  NULL.
 * ------------------------------------------------------------------------
 */
int
Itcl_FindVarDefn(interp,cdefn,name,vdefnPtr)
    Tcl_Interp *interp;       /* interpreter managing this class */
    Itcl_Class* cdefn;        /* class containing this variable */
    char* name;               /* variable name */
    ItclVarDefn** vdefnPtr;   /* returns:  variable definition */
{
    char *className, *varName;
    Itcl_Namespace ns;
    Itcl_Class *cd;
    ItclClass *cdPtr;
    Itcl_HierIter hier;
    Tcl_HashEntry *entry;

    /*
     *  Break the reference into a class name part and a variable
     *  name part.  If the class name part is non-null, then find
     *  that class in the hierarchy and look there.
     */
    Itcl_ParseNamespPath(name, &className, &varName);
    if (className) {
        if (Itcl_FindNamesp(interp, className, 0, &ns) != TCL_OK) {
            return TCL_ERROR;
        }
        if (ns == NULL || !Itcl_IsClass(ns)) {
            cdefn = NULL;
        }
        else {
            cd = (Itcl_Class*)Itcl_GetNamespData(ns);

            /*
             *  Make sure that the class we found is in this
             *  inheritance hierarchy.
             */
            Itcl_InitHierIter(&hier,cdefn);
            while ((cdefn=Itcl_AdvanceHierIter(&hier)) != NULL) {
                if (cdefn == cd) {
                    break;
                }
            }
            Itcl_DeleteHierIter(&hier);
        }
    }

    /*
     *  At this point, we either look for the name in the specified
     *  class scope, or in the scope determined from the scope
     *  qualifier prefix.  Search the hierarchy from most- to least-
     *  specific class.
     */
    *vdefnPtr = NULL;

    if (cdefn != NULL) {
        Itcl_InitHierIter(&hier,cdefn);
        cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
        while (cdPtr != NULL) {
            entry = Tcl_FindHashEntry(&cdPtr->variables, varName);
            if (entry) {
                *vdefnPtr = (ItclVarDefn*)Tcl_GetHashValue(entry);
                break;
            }
            cdPtr = (ItclClass*)Itcl_AdvanceHierIter(&hier);
        }
        Itcl_DeleteHierIter(&hier);
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_FindCmdDefn()
 *
 *  Searches a class definition for a particular method/proc.  Recognizes
 *  simple names and the full-blown "namesp::class::command" notation.
 *
 *  If any errors are encountered, this procedure returns TCL_ERROR
 *  along with an error message in the interpreter.  Otherwise, it
 *  returns TCL_OK.  If the command definition was found, then
 *  "mdefnPtr" returns a pointer to it.  Otherwise, "mdefnPtr" returns
 *  NULL.
 * ------------------------------------------------------------------------
 */
int
Itcl_FindCmdDefn(interp,cdefn,name,mdefnPtr)
    Tcl_Interp *interp;         /* interpreter managing this class */
    Itcl_Class* cdefn;          /* class containing this command member */
    char* name;                 /* member name */
    ItclCmdMember** mdefnPtr;   /* returns:  member definition */
{
    char *className, *cmdName;
    Itcl_Namespace ns;
    Itcl_Class *cd;
    Itcl_HierIter hier;
    Tcl_HashEntry *entry;
    Namespace *nsPtr;
    Command *cmdPtr;

    /*
     *  Break the reference into a class name part and a command
     *  name part.  If the class name part is non-null, then find
     *  that class in the hierarchy and look there.
     */
    Itcl_ParseNamespPath(name, &className, &cmdName);
    if (className) {
        if (Itcl_FindNamesp(interp, className, 0, &ns) != TCL_OK) {
            return TCL_ERROR;
        }
        if (ns == NULL || !Itcl_IsClass(ns)) {
            cdefn = NULL;
        }
        else {
            cd = (Itcl_Class*)Itcl_GetNamespData(ns);

            /*
             *  Make sure that the class we found is in this
             *  inheritance hierarchy.
             */
            Itcl_InitHierIter(&hier,cdefn);
            while ((cdefn=Itcl_AdvanceHierIter(&hier)) != NULL) {
                if (cdefn == cd) {
                    break;
                }
            }
            Itcl_DeleteHierIter(&hier);
        }
    }

    /*
     *  At this point, we either look for the name in the specified
     *  class scope, or in the scope determined from the scope
     *  qualifier prefix.  Search the hierarchy from most- to least-
     *  specific class.
     */
    *mdefnPtr = NULL;

    if (cdefn != NULL) {
        Itcl_InitHierIter(&hier,cdefn);
        while ((cd=Itcl_AdvanceHierIter(&hier)) != NULL) {
            nsPtr = (Namespace*)cd->namesp;
            entry = Tcl_FindHashEntry(&nsPtr->commands, cmdName);
            if (entry) {
                cmdPtr = (Command*)Tcl_GetHashValue(entry);
                if (cmdPtr->proc == Itcl_ExecMethod ||
                    cmdPtr->proc == Itcl_ExecProc) {
                    *mdefnPtr = (ItclCmdMember*)cmdPtr->clientData;
                    break;
                }
            }
        }
        Itcl_DeleteHierIter(&hier);
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InitHierIter()
 *
 *  Initializes an iterator for traversing the hierarchy of the given
 *  class.  Subsequent calls to Itcl_AdvanceHierIter() will return
 *  the base classes in order from most-to-least specific.
 * ------------------------------------------------------------------------
 */
void
Itcl_InitHierIter(iter,cdefn)
    Itcl_HierIter *iter;  /* iterator used for traversal */
    Itcl_Class *cdefn;    /* class definition for start of traversal */
{
    Itcl_InitStack(&iter->stack);
    Itcl_PushStack((ClientData)cdefn, &iter->stack);
    iter->current = cdefn;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_DeleteHierIter()
 *
 *  Destroys an iterator for traversing class hierarchies, freeing
 *  all memory associated with it.
 * ------------------------------------------------------------------------
 */
void
Itcl_DeleteHierIter(iter)
    Itcl_HierIter *iter;  /* iterator used for traversal */
{
    Itcl_DeleteStack(&iter->stack);
    iter->current = NULL;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_AdvanceHierIter()
 *
 *  Moves a class hierarchy iterator forward to the next base class.
 *  Returns a pointer to the current class definition, or NULL when
 *  the end of the hierarchy has been reached.
 * ------------------------------------------------------------------------
 */
Itcl_Class*
Itcl_AdvanceHierIter(iter)
    Itcl_HierIter *iter;  /* iterator used for traversal */
{
    register Itcl_ListElem *elem;
    ItclClass *cdPtr;

    iter->current = (Itcl_Class*)Itcl_PopStack(&iter->stack);

    /*
     *  Push classes onto the stack in reverse order, so that
     *  they will be popped off in the proper order.
     */
    if (iter->current) {
        cdPtr = (ItclClass*)iter->current;
        elem = Itcl_LastListElem(&cdPtr->bases);
        while (elem) {
            Itcl_PushStack(Itcl_GetListValue(elem), &iter->stack);
            elem = Itcl_PrevListElem(elem);
        }
    }
    return iter->current;
}
