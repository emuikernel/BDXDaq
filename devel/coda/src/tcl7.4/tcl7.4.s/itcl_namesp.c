/*		
*------------------------------------------------------------------------
*      PACKAGE:		  [incr Tcl]
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
*  This part adds namespaces to ordinary Tcl.  Namespaces provide
*  a local context of commands and variables that are kept separate
*  from the usual Tcl facilities at the global scope.  In effect,
*  the global scope is simply a master namespace filled with the
*  usual Tcl/Tk stuff.  Other namespaces are created as children
*  of the global namespace, containing special-purpose commands and
*  variables.
*
* ========================================================================
*  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
*           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
*     RCS:  $Id: itcl_namesp.c,v 1.4 1998/09/21 18:36:23 abbottd Exp $
* ========================================================================
*             Copyright (c) 1993-1995  AT&T Bell Laboratories
* ------------------------------------------------------------------------
* See the file "license.terms" for information on usage and redistribution
* of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include <stdio.h>
#include <string.h>

/* Sergey: dlopen etc */
#ifndef VXWORKS
#include <dlfcn.h>
#endif

#include <assert.h>
#include "tclInt.h"

static char rcsid[] = "$Id: itcl_namesp.c,v 1.4 1998/09/21 18:36:23 abbottd Exp $";


/*
 *  FORWARD DECLARATIONS
 */
static void ItclNsFree _ANSI_ARGS_((ClientData cdata));

static NamespImportRef* ItclCreateNamespImportRef _ANSI_ARGS_((
    Namespace* nsPtr, int pLevel));
static void ItclDeleteNamespImportRef _ANSI_ARGS_((
    NamespImportRef* nsref));


/*
 * ------------------------------------------------------------------------
 *  ItclNsInterpInit()
 *
 *  Invoked within Tcl_CreateInterp() as soon as a new Tcl interpreter
 *  is created to initialize the namespace part of the Interp data
 *  structure.
 *
 *  This should really be integrated directly into Tcl_CreateInterp(),
 *  but having this as a separate procedure allows the changes to
 *  tclBasic.c be more surgical.
 * ------------------------------------------------------------------------
 */
void
ItclNsInterpInit(iPtr)
    Interp *iPtr;  /* interpreter to be updated */
{
    int status;
    Itcl_Namespace ns;

    /*
     *  Initialize data members which keep track of namespace info.
     */
    iPtr->globalNs = NULL;
    iPtr->activeNs = NULL;
    iPtr->activeData = NULL;
    Itcl_InitStack(&iPtr->nsStack);
    Itcl_InitStack(&iPtr->nsDataStack);
    iPtr->cmdProtection = ITCL_PUBLIC;
    iPtr->varProtection = ITCL_PUBLIC;

    /*
     *  Create the global namespace and make it active by default.
     */
    status = Itcl_CreateNamesp((Tcl_Interp*)iPtr, "",
        (ClientData)NULL, (Itcl_DeleteProc*)NULL, &ns);

    assert(status == TCL_OK);

    iPtr->globalNs = (Namespace*)ns;
    iPtr->gNsToken = Itcl_ActivateNamesp((Tcl_Interp*)iPtr, ns);
}

/*
 * ------------------------------------------------------------------------
 *  ItclNsInterpDelete()
 *
 *  Invoked within Tcl_DeleteInterp() when an existing Tcl interpreter
 *  is being destroyed.  Frees all resources used to manage namespaces
 *  in the Interp data structure.
 *
 *  This should really be integrated directly into Tcl_DeleteInterp(),
 *  but having this as a separate procedure allows the changes to
 *  tclBasic.c be more surgical.
 * ------------------------------------------------------------------------
 */
void
ItclNsInterpDelete(iPtr)
    Interp *iPtr;  /* interpreter being destroyed */
{
    Itcl_Namespace ns;
    ClientData nsd;

    /*
     *  Release all namespaces including the global namespace,
     *  which should be the last one left on the stack.
     */
    while (Itcl_GetStackSize(&iPtr->nsStack) > 0) {
        ns = (Itcl_Namespace)Itcl_PopStack(&iPtr->nsStack);
        Itcl_DeleteNamesp(ns);
        nsd = (ClientData)Itcl_PopStack(&iPtr->nsDataStack);
        if (nsd) {
            Itcl_ReleaseData(nsd);
        }
    }
    Itcl_DeleteStack(&iPtr->nsStack);
    Itcl_DeleteStack(&iPtr->nsDataStack);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_GetGlobalNamesp()
 *
 *  Used to query the global namespace context for a particular Tcl
 *  interpreter.  The global namespace is a collection of commands
 *  and variables, just like an ordinary Tcl interpreter.
 *
 *  This procedure is part of the public API for namespaces.  Procedures
 *  within the Tcl core (having access to "tclInt.h") can simply look at
 *  the "globalNs" field directly.
 *
 *  Returns a token for the global namespace.
 * ------------------------------------------------------------------------
 */
Itcl_Namespace
Itcl_GetGlobalNamesp(interp)
    Tcl_Interp *interp;  /* interpreter being queried */
{
    Interp *iPtr = (Interp*)interp;
    return (Itcl_Namespace)iPtr->globalNs;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_GetCallingNamesp()
 *
 *  Checks the call stack and returns the namespace context some
 *  number of levels up.  It is often useful to know the invocation
 *  context for a method or proc, to check member access.
 *
 *  Returns a token for the namespace in the calling context.
 * ------------------------------------------------------------------------
 */
Itcl_Namespace
Itcl_GetCallingNamesp(interp,level)
    Tcl_Interp *interp;  /* interpreter being queried */
    int level;           /* number of levels up in the call stack (>= 1) */
{
    Interp *iPtr = (Interp*)interp;
    CallFrame *framePtr;
    Namespace *nsPtr;

    assert(level >= 1);

    framePtr = iPtr->varFramePtr;
    while (framePtr && level > 0) {
        framePtr = framePtr->callerVarPtr;
        level--;
    }
    if (framePtr) {
        nsPtr = framePtr->activeNs;
    } else {
        nsPtr = iPtr->globalNs;
    }
    return (Itcl_Namespace)nsPtr;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_GetActiveNamesp()
 *
 *  Used to query the active namespace context for a particular Tcl
 *  interpreter.  When a namespace is "active", command/variable names
 *  are interpreted first and foremost with respect to this namespace,
 *  and then with respect to namespaces that it uses.  Furthermore,
 *  all commands and (global) variables created when a namespace is
 *  active remain local to it.
 *
 *  This procedure is part of the public API for namespaces.  Procedures
 *  within the Tcl core (having access to "tclInt.h") can simply look at
 *  the "activeNs" field directly.
 *
 *  Returns a token for the global namespace.
 * ------------------------------------------------------------------------
 */
Itcl_Namespace
Itcl_GetActiveNamesp(interp)
    Tcl_Interp *interp;  /* interpreter being queried */
{
    Interp *iPtr = (Interp*)interp;
    return (Itcl_Namespace)iPtr->activeNs;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_GetActiveNamespData()
 *
 *  Used to query the clientData associated with the active namespace
 *  context for a particular Tcl interpreter.  ClientData is specified
 *  when the namespace is first activated via Itcl_ActivateNamesp2().
 *
 *  This procedure is part of the public API for namespaces.  Procedures
 *  within the Tcl core (having access to "tclInt.h") can simply look at
 *  the "activeData" field directly.
 *
 *  Returns the ClientData value, or NULL if the active namespace has
 *  no extra data.
 * ------------------------------------------------------------------------
 */
ClientData
Itcl_GetActiveNamespData(interp)
    Tcl_Interp *interp;  /* interpreter being queried */
{
    Interp *iPtr = (Interp*)interp;
    return iPtr->activeData;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_GetCommandNamesp()
 *
 *  Used to query the namespace context associated with a Tcl command.
 *  Returns a token for the namespace containing the specified command.
 * ------------------------------------------------------------------------
 */
Itcl_Namespace
Itcl_GetCommandNamesp(command)
    Tcl_Command command;  /* command being queried */
{
    Command *cmdPtr = (Command*)command;

    if (cmdPtr == NULL) {
        return (Itcl_Namespace)NULL;
    }
    return (Itcl_Namespace)cmdPtr->namesp;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ValidNamesp()
 *
 *  Used to determine whether a namespace context is alive and well.
 *  A namespace token returned from Itcl_GetActiveNamesp() is guaranteed
 *  to be recognized if it is preserved like this:
 *
 *      Itcl_Namespace ns = Itcl_GetActiveNamesp(interp);
 *      Itcl_PreserveData((ClientData)ns);
 *
 *  However, just because it is preserved does not mean that the
 *  namespace context will remain valid.  If the namespace is deleted,
 *  it becomes a "dead" namespace.
 *
 *  This procedure examines a namespace token, and returns non-zero
 *  if it is still an active namespace.  Returns zero if the namespace
 *  has been deleted.
 * ------------------------------------------------------------------------
 */
int
Itcl_ValidNamesp(ns)
    Itcl_Namespace ns;  /* namespace context being checked */
{
    Namespace *nsPtr = (Namespace*)ns;
    return (nsPtr && (nsPtr->flags & ITCL_NS_ALIVE) != 0);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_CmdProtection()
 *
 *  Used to query/set the protection level used when new commands are
 *  created within an interpreter.  The default protection level (set
 *  when the interpreter is created) is PUBLIC.
 *
 *  If the specified level is 0, then this procedure returns the
 *  current value without changing it.  Otherwise, it sets the current
 *  value to the specified protection level, and returns the previous
 *  value.
 * ------------------------------------------------------------------------
 */
int
Itcl_CmdProtection(interp,newLevel)
    Tcl_Interp *interp;  /* interpreter being queried */
    int newLevel;        /* new protection level or 0 */
{
    Interp *iPtr = (Interp*)interp;
    int oldVal;

    /*
     *  If a new level was specified, then set the protection level.
     *  In any case, return the protection level as it stands right now.
     */
    oldVal = iPtr->cmdProtection;

    if (newLevel != 0) {
        assert(newLevel == ITCL_PUBLIC ||
            newLevel == ITCL_PROTECTED ||
            newLevel == ITCL_PRIVATE);
        iPtr->cmdProtection = newLevel;
    }
    return oldVal;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_ChangeCmdProtection()
 *
 *  Changes the protection level for the specified command.  By default,
 *  commands have the protection level that was in effect when the
 *  command was created.  The protection level is usually set by
 *  commands like "public" or "private" that call Itcl_CmdProtection()
 *  to install a protection level.
 *
 *  If the specified level is 0, then this procedure returns the
 *  current protection for the command without changing it.  Otherwise,
 *  it changes the command to the specified protection level, and
 *  returns its previous level.
 * ------------------------------------------------------------------------
 */
int
Itcl_ChangeCmdProtection(interp,cmd,newLevel)
    Tcl_Interp *interp;  /* interpreter being queried */
    Tcl_Command cmd;     /* command being modified */
    int newLevel;        /* new protection level or 0 */
{
    Command *cmdPtr = (Command*)cmd;
    int oldLevel;
    char *cmdName;

    assert(cmdPtr);

    oldLevel = cmdPtr->protection;
    if (newLevel > 0) {
        assert(newLevel == ITCL_PUBLIC ||
            newLevel == ITCL_PROTECTED ||
            newLevel == ITCL_PRIVATE);
        cmdPtr->protection = newLevel;

        cmdName = Tcl_GetHashKey(&cmdPtr->namesp->commands, cmdPtr->hPtr);
        Itcl_NsCacheCmdChanged((Itcl_Namespace)cmdPtr->namesp, cmdName);
    }
    return oldLevel;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_VarProtection()
 *
 *  Used to query/set the protection level used when new variables are
 *  created within an interpreter.  The default protection level (set
 *  when the interpreter is created) is PROTECTED.
 *
 *  If the specified level is 0, then this procedure returns the
 *  current value without changing it.  Otherwise, it sets the current
 *  value to the specified protection level, and returns the new value.
 * ------------------------------------------------------------------------
 */
int
Itcl_VarProtection(interp,newLevel)
    Tcl_Interp *interp;  /* interpreter being queried */
    int newLevel;        /* new protection level or 0 */
{
    Interp *iPtr = (Interp*)interp;
    int oldVal;

    /*
     *  If a new level was specified, then set the protection level.
     *  In any case, return the protection level as it stands right now.
     */
    oldVal = iPtr->varProtection;

    if (newLevel != 0) {
        assert(newLevel == ITCL_PUBLIC ||
            newLevel == ITCL_PROTECTED ||
            newLevel == ITCL_PRIVATE);
        iPtr->varProtection = newLevel;
    }
    return oldVal;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_ChangeVarProtection()
 *
 *  Changes the protection level for the specified variable.  By default,
 *  variables have the protection level that was in effect when the
 *  variable was created.  The protection level is usually set by
 *  commands like "public" or "private" that call Itcl_VarProtection()
 *  to install a protection level.
 *
 *  If the specified level is 0, then this procedure returns the
 *  current protection for the variable without changing it.  Otherwise,
 *  it changes the variable to the specified protection level, and
 *  returns its previous level.
 * ------------------------------------------------------------------------
 */
int
Itcl_ChangeVarProtection(interp,var,newLevel)
    Tcl_Interp *interp;  /* interpreter being queried */
    Tcl_Var var;         /* variable being modified */
    int newLevel;        /* new protection level or 0 */
{
    Var *varPtr = (Var*)var;
    int oldLevel;

    assert(varPtr);

    oldLevel = varPtr->protection;
    if (newLevel > 0) {
        assert(newLevel == ITCL_PUBLIC ||
            newLevel == ITCL_PROTECTED ||
            newLevel == ITCL_PRIVATE);
        varPtr->protection = newLevel;

        Itcl_NsCacheVarChanged((Itcl_Namespace)varPtr->namesp, varPtr->name);
    }
    return oldLevel;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_SetCmdEnforcer()
 *
 *  Used to query/register a special "enforcer" procedure that is
 *  invoked whenever a command is first used within a namespace.
 *  This procedure takes the following arguments:
 *
 *     typedef int Itcl_CmdEnforcerProc(Tcl_Interp* interp,
 *        char* name, Tcl_Command* cmdPtr);
 *
 *  and can be used to enforce special rules concerning command names, or
 *  to add substitute commands to the command cache.  Here, "interp" is
 *  the interpreter processing the command lookup, "name" is the command
 *  name, and "cmdPtr" returns a pointer to a substitute command record.
 *
 *  To disallow access to a command, simply return an error message in
 *  the interpreter result and return the status TCL_ERROR.  To provide
 *  a substitute command, return TCL_OK along with a pointer to a
 *  Tcl_Command record to use instead.  The storage for the Tcl_Command
 *  should be managed by Tcl_CreateCommand(); when the cache is destroyed,
 *  this token is simply forgotten.  To allow command lookup to continue,
 *  return TCL_OK, along with a NULL token in "cmdPtr".
 *
 *  To create a "safe" namespace, for example, an enforcer procedure
 *  could be registered which simply disallows access to destructive
 *  commands by returning TCL_ERROR.
 *
 *  Sets the current enforcer to the specified procedure, and returns
 *  a pointer to the previous enforcer.  This makes it easy to capture
 *  the old value and later set it back.
 * ------------------------------------------------------------------------
 */
Itcl_CmdEnforcerProc*
Itcl_SetCmdEnforcer(ns,proc)
    Itcl_Namespace ns;           /* namespace being modified */
    Itcl_CmdEnforcerProc* proc;  /* proc enforcing namespace rules */
{
    Namespace *nsPtr = (Namespace*)ns;
    Itcl_CmdEnforcerProc *oldProc;

    oldProc = nsPtr->cmdEnforcer;
    nsPtr->cmdEnforcer = proc;

    return oldProc;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_SetVarEnforcer()
 *
 *  Used to query/register a special "enforcer" procedure that is
 *  invoked whenever a variable is first accessed within a namespace.
 *  This procedure takes the following arguments:
 *
 *     typedef int Itcl_VarEnforcerProc(Tcl_Interp* interp,
 *        char* name, Tcl_Var* varPtr, int flags);
 *
 *  and can be used to enforce special rules concerning variable names, or
 *  to add substitute variables to the variable cache.  Here, "interp" is
 *  the interpreter processing the variable lookup, "name" is the variable
 *  name, and "varPtr" returns a pointer to a substitute variable record.
 *  If "flags" has ITCL_GLOBAL_VAR set, then this variable is being
 *  requested in a "global" context; otherwise, it is being used as a
 *  local variable within a procedure.
 *
 *  To disallow access to a variable, simply return an error message in
 *  the interpreter result and return the status TCL_ERROR.  To provide
 *  a substitute variable, return TCL_OK along with a Tcl_Var token to
 *  use instead.  The storage for the Tcl_Var should be managed by
 *  TclNewVar(); when the cache is destroyed, this token is simply
 *  forgotten.  To allow variable lookup to continue, return TCL_OK,
 *  along with a NULL token in "varPtr".
 *
 *  Sets the current enforcer to the specified procedure, and returns
 *  a pointer to the previous enforcer.  This makes it easy to capture
 *  the old value and later set it back.
 * ------------------------------------------------------------------------
 */
Itcl_VarEnforcerProc*
Itcl_SetVarEnforcer(ns,proc)
    Itcl_Namespace ns;           /* namespace being modified */
    Itcl_VarEnforcerProc* proc;  /* proc enforcing namespace rules */
{
    Namespace *nsPtr = (Namespace*)ns;
    Itcl_VarEnforcerProc *oldProc;

    oldProc = nsPtr->varEnforcer;
    nsPtr->varEnforcer = proc;

    return oldProc;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ActivateNamesp2()
 *
 *  Installs the specified namespace as the "active" namespace for an
 *  interpreter.  All previously active namespaces are kept in a stack,
 *  so they can be restored later.  Any commands or global variables that
 *  are created while a namespace is active remain local to it.
 *
 *  Returns a token that must be passed to Itcl_DeactiveNamesp(), to
 *  deactivate the namespace.  Any mismatch between namespace
 *  activation/deactivation that would upset the stack is trapped
 *  and treated as a programming error.  Returns a NULL token if
 *  anything goes wrong (e.g., the namespace is no longer valid).
 * ------------------------------------------------------------------------
 */
Itcl_ActiveNamespace
Itcl_ActivateNamesp2(interp,ns,cd)
    Tcl_Interp *interp;  /* interpreter being modified */
    Itcl_Namespace ns;   /* namespace becoming active */
    ClientData cd;       /* client data associated with this activation */
{
    Interp *iPtr = (Interp*)interp;
    Namespace *nsPtr = (Namespace*)ns;
    Itcl_ActiveNamespace token;

    /*
     *  If the context namespace is "dead", return an error.
     */
    if ((nsPtr->flags&ITCL_NS_ALIVE) == 0) {
        Tcl_AppendResult(interp, "context namespace \"",
            Itcl_GetNamespPath(ns), "\" no longer exists",
            (char*)NULL);
        return NULL;
    }

    /*
     *  Activate the namespace by pushing it on top of the stack.
     *  Keep track of the number of activations for this namespace,
     *  to keep it alive if someone tries to activate it.
     */
    nsPtr->activations++;
    Itcl_PushStack((ClientData)nsPtr, &iPtr->nsStack);
    iPtr->activeNs = nsPtr;

    Itcl_PushStack(Itcl_PreserveData(cd), &iPtr->nsDataStack);
    iPtr->activeData = cd;

    /*
     *  Form a token that characterizes the state of the interpreter,
     *  so we can check later to make sure that this namespace is
     *  deactivated properly.
     */
    token = (Itcl_ActiveNamespace)iPtr;
    token += Itcl_GetStackSize(&iPtr->nsStack);
    return token;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_DeactivateNamesp()
 *
 *  Deactivates the namespace put in force by the most recent call
 *  to Itcl_ActivateNamesp2().  Active namespaces are kept in a stack,
 *  and the activate/deactivate operations should be called in pairs to
 *  push/pop namespaces from the "active" list.
 *
 *  The active namespace token is used to check for mismatch errors
 *  between activate/deactivate calls.  If an error is found, it is
 *  treated as a programming error, and execution is aborted.
 * ------------------------------------------------------------------------
 */
void
Itcl_DeactivateNamesp(interp,token)
    Tcl_Interp *interp;           /* interpreter being updated */
    Itcl_ActiveNamespace token;   /* token representing active namespace */
{
    Interp *iPtr = (Interp*)interp;

    Itcl_ActiveNamespace cmp;
    Namespace* nsPtr;
    ClientData cd;

    /*
     *  Make sure that the token accurately reflects the state of
     *  the interpreter.  If there is a mismatch between activate
     *  and deactivate calls, the token will be wrong.
     */
    cmp = (Itcl_ActiveNamespace)iPtr;
    cmp += Itcl_GetStackSize(&iPtr->nsStack);

    if (cmp != token) {
        panic("mismatch in namespace activate/deactivate operations");
    }

    /*
     *  Deactivate the current namespace by popping it from the stack.
     *  Move the next namespace into place as the "active" namespace.
     */
    nsPtr = (Namespace*)Itcl_PopStack(&iPtr->nsStack);
    nsPtr->activations--;

    if ((nsPtr->flags&ITCL_NS_DYING) && (nsPtr->activations == 0)) {
        Itcl_DeleteNamesp((Itcl_Namespace)nsPtr);
    }

    cd = Itcl_PopStack(&iPtr->nsDataStack);
    Itcl_ReleaseData(cd);

    iPtr->activeNs = (Namespace*)Itcl_PeekStack(&iPtr->nsStack);
    iPtr->activeData = Itcl_PeekStack(&iPtr->nsDataStack);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_CreateNamesp()
 *
 *  Creates a new namespace with the given name as a child of the
 *  currently active namespace.  If there is no active namespace
 *  (i.e., the interpreter is being initialized), then a global
 *  namespace is created.
 *
 *  For ordinary namespaces, if the namespace name contains "::"
 *  qualifiers, then parent namespaces are traversed and created if
 *  necessary.
 *
 *  Returns TCL_OK and a pointer to the new namespace if successful.
 *  If the namespace already exists, if the name has invalid characters
 *  like ":::", or if any other error occurs, this routine returns
 *  TCL_ERROR, along with an error message in the given interp.
 * ------------------------------------------------------------------------
 */
int
Itcl_CreateNamesp(interp,path,cdata,dproc,nsPtr)
    Tcl_Interp* interp;        /* interpreter being modified */
    char* path;                /* name for new namespace */
    ClientData cdata;          /* extra data associated with namespace */
    Itcl_DeleteProc* dproc;    /* proc to delete client data */
    Itcl_Namespace* nsPtr;     /* returns the new namespace */
{
    Interp *iPtr = (Interp*)interp;

    int newEntry, specificRef;
    char* name;
    Namespace *parentNs, *newNs;
    Tcl_HashEntry *entry;

    /*
     *  If there is no active namespace, then the interpreter is
     *  probably being initialized.  Treat this namespace as the
     *  global namespace, and avoid looking for a parent.
     */
    if (iPtr->activeNs == NULL) {
        parentNs = NULL;
        name = "";
    }

    /*
     *  Find the namespace that will contain the new namespace.
     *  If the namespace path cannot be followed, return an error.
     */
    else {
        if (ItclFollowNamespPath(interp, iPtr->activeNs, path,
            0, &parentNs,&name,&specificRef) != TCL_OK) {
            return TCL_ERROR;
        }
        if (parentNs == NULL) {
            Tcl_AppendResult(interp, "cannot create namespace \"", path,
                "\": parent namespace does not exist",
                (char*)NULL);
            return TCL_ERROR;
        }

        /*
         *  Avoid bogus namespace names.
         */
        if (!name || (*name == '\0')) {
            Tcl_AppendResult(interp, "cannot create namespace \"", path,
                "\": invalid name",
                (char*)NULL);
            return TCL_ERROR;
        }

        /*
         *  Make sure that the specified name does not already exist
         *  in the parent namespace.
         */
        if (Tcl_FindHashEntry(&parentNs->children,name)) {
            Tcl_AppendResult(interp, "cannot create namespace \"", name,
                "\": already exists",
                (char*)NULL);
            return TCL_ERROR;
        }
    }

    /*
     *  Create the new namespace and root it in the active namespace.
     */
    newNs = (Namespace*)ckalloc(sizeof(Namespace));
    newNs->name = (char*)ckalloc((unsigned)(strlen(name)+1));
    strcpy(newNs->name, name);

    newNs->clientData  = cdata;
    newNs->flags       = ITCL_NS_ALIVE;
    newNs->activations = 0;
    newNs->interp      = interp;
    newNs->parent      = parentNs;
    Tcl_InitHashTable(&newNs->children, TCL_STRING_KEYS);
    Tcl_InitHashTable(&newNs->commands, TCL_STRING_KEYS);
    Tcl_InitHashTable(&newNs->commandTable, TCL_STRING_KEYS);
    Tcl_InitHashTable(&newNs->variables, TCL_STRING_KEYS);
    Tcl_InitHashTable(&newNs->variableTable, TCL_STRING_KEYS);

    Itcl_InitList(&newNs->importList);
    Itcl_InitList(&newNs->importAllList);
    Itcl_InitList(&newNs->importedByList);
    newNs->deleteProc  = dproc;
    newNs->cmdEnforcer = NULL;
    newNs->varEnforcer = NULL;

    Itcl_PreserveData((ClientData)newNs);
    Itcl_EventuallyFree((ClientData)newNs, ItclNsFree);

    /*
     *  Root the new namespace in its parent, and set it up to import
     *  from the parent by default.  Since the parent has a new child
     *  its command/variable lookup cache must be reset.
     */
    if (parentNs) {
        entry = Tcl_CreateHashEntry(&parentNs->children, name, &newEntry);
        Tcl_SetHashValue(entry, Itcl_PreserveData((ClientData)newNs));

        Itcl_AppendImportNamesp((Itcl_Namespace)newNs,
            (Itcl_Namespace)parentNs, ITCL_PUBLIC);

        Itcl_NsCacheNamespChanged((Itcl_Namespace)parentNs);
    }

    *nsPtr = (Itcl_Namespace)newNs;
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_DeleteNamesp()
 *
 *  Deletes a namespace by removing it from its parent's list and
 *  from any "import" list that might be referencing it.
 *
 *  If the namespace is on the activation stack, it is marked as
 *  "dying".  When it has been completely removed from the stack,
 *  this procedure is called again to perform the actual "delete"
 *  operation.
 *
 *  If the namespace is not on the activation stack, then it is
 *  immediately deleted and marked as "dead".  The data structure
 *  representing the namespace remains around until there are no
 *  other uses of it (recorded via Itcl_PreserveData()), in which
 *  case it is automatically freed.
 * ------------------------------------------------------------------------
 */
void
Itcl_DeleteNamesp(ns)
    Itcl_Namespace ns;   /* namespace to be deleted */
{
    Namespace *nsPtr = (Namespace*)ns;

    Tcl_HashEntry *entry;
    Tcl_HashSearch search;
    Namespace* childNs;
    Command *cmdPtr;
    Itcl_ListElem *elem;
    Itcl_Namespace importNs;
    NamespCacheRef *cacheRef;

    /*
     *  If the namespace is on the current activation stack,
     *  simply mark it as "dying".
     */
    if (nsPtr->activations > 0) {
        nsPtr->flags |= ITCL_NS_DYING;
    }

    /*
     *  Otherwise, delete it by divorcing it from its parent and
     *  clearing out its import list.  Note that clearing the
     *  import list automatically removes the namespace from the
     *  "importByList" in other namespaces that might be referencing it.
     */
    else {
        /*
         *  Start by destroying variables, since they might fire
         *  off traces.
         */
        TclDeleteVars((Interp*)nsPtr->interp, &nsPtr->variables);

        entry = Tcl_FirstHashEntry(&nsPtr->variableTable, &search);
        while (entry != NULL) {
            cacheRef = (NamespCacheRef*)Tcl_GetHashValue(entry);
            if (--cacheRef->usage == 0 && cacheRef->element == NULL) {
                ckfree((char*)cacheRef);
            }
            Tcl_DeleteHashEntry(entry);
            entry = Tcl_NextHashEntry(&search);
        }

        /*
         *  Go to all namespaces on the "importedByList", and break
         *  their connection to this namespace.  Note that each entry
         *  on the "importedByList" is automatically deleted by
         *  Itcl_RemoveImportNamesp().  Also, clear the import list.
         *  This breaks all import and imported-by connections to
         *  other namespaces.
         */
        elem = Itcl_FirstListElem(&nsPtr->importedByList);
        while (elem) {
            importNs = (Itcl_Namespace)Itcl_GetListValue(elem);
            elem = Itcl_NextListElem(elem);

            Itcl_RemoveImportNamesp(importNs, ns);
        }
        Itcl_ClearImportNamesp(ns);

        /*
         *  Divorce the namespace from its parent.
         */
        if (nsPtr->parent) {
            entry = Tcl_FindHashEntry(&nsPtr->parent->children, nsPtr->name);
            if (entry) {
                Tcl_DeleteHashEntry(entry);
            }
            Itcl_NsCacheNamespChanged((Itcl_Namespace)nsPtr->parent);
        }
        nsPtr->parent = NULL;

        /*
         *  Delete all child namespaces.
         */
        entry = Tcl_FirstHashEntry(&nsPtr->children, &search);
        while (entry) {
            childNs = (Namespace*)Tcl_GetHashValue(entry);
            Itcl_DeleteNamesp((Itcl_Namespace)childNs);
            entry = Tcl_NextHashEntry(&search);
        }
        Tcl_DeleteHashTable(&nsPtr->children);

        /*
         *  Delete all commands and variables in this namespace.
         */
        entry = Tcl_FirstHashEntry(&nsPtr->commands, &search);
        while (entry != NULL) {
            cmdPtr = (Command*)Tcl_GetHashValue(entry);
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
            ckfree((char*)cmdPtr);
            entry = Tcl_NextHashEntry(&search);
        }
        Tcl_DeleteHashTable(&nsPtr->commands);

        entry = Tcl_FirstHashEntry(&nsPtr->commandTable, &search);
        while (entry != NULL) {
            cacheRef = (NamespCacheRef*)Tcl_GetHashValue(entry);
            if (--cacheRef->usage == 0 && cacheRef->element == NULL) {
                ckfree((char*)cacheRef);
            }
            Tcl_DeleteHashEntry(entry);
            entry = Tcl_NextHashEntry(&search);
        }
        Tcl_DeleteHashTable(&nsPtr->commandTable);
        Tcl_DeleteHashTable(&nsPtr->variableTable);

        Itcl_DeleteList(&nsPtr->importList);
        Itcl_DeleteList(&nsPtr->importAllList);
        Itcl_DeleteList(&nsPtr->importedByList);

        nsPtr->flags &= ~(ITCL_NS_ALIVE|ITCL_NS_DYING);

        /*
         *  Free any client data associated with the namespace
         */
        if (nsPtr->deleteProc) {
            (*nsPtr->deleteProc)(nsPtr->clientData);
        }
        nsPtr->clientData = NULL;

        /*
         *  The namespace record will die here if no one else is
         *  referencing it.
         */
        Itcl_ReleaseData((ClientData)ns);
    }
}


/*
 * ------------------------------------------------------------------------
 *  ItclNsFree()
 *
 *  Cleanup procedure for a Namespace data structure.  Invoked by
 *  Itcl_ReleaseData() when there are no more uses of a dead namespace,
 *  to free up the memory associated with it.
 * ------------------------------------------------------------------------
 */
static void
ItclNsFree(cdata)
    ClientData cdata;   /* namespace to be freed */
{
    Namespace* nsPtr = (Namespace*)cdata;

    ckfree(nsPtr->name);
    ckfree((char*)nsPtr);
}


/*
 * ------------------------------------------------------------------------
 *  ItclCreateNamespImportRef()
 *
 *  Allocates and initializes a reference on a namespace importList.
 *  Returns a pointer to the new reference.
 * ------------------------------------------------------------------------
 */
static NamespImportRef*
ItclCreateNamespImportRef(nsPtr,pLevel)
    Namespace* nsPtr;         /* namespace to be imported */
    int pLevel;               /* protection level for import */
{
    NamespImportRef* nsref;

    assert(pLevel == ITCL_PUBLIC || pLevel == ITCL_PROTECTED);

    nsref = (NamespImportRef*)ckalloc(sizeof(NamespImportRef));
    nsref->namesp  = nsPtr;
    nsref->protection = pLevel;

    return nsref;
}

/*
 * ------------------------------------------------------------------------
 *  ItclDeleteNamespImportRef()
 *
 *  Destroys a namespace importList reference created previously by
 *  ItclCreateNamespImportRef().
 * ------------------------------------------------------------------------
 */
static void
ItclDeleteNamespImportRef(nsref)
    NamespImportRef* nsref;   /* reference to be deleted */
{
    ckfree((char*)nsref);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ClearImportNamesp()
 *
 *  Removes all import references in the specified namespace.  This
 *  means that commands/variables will no longer be included from any
 *  other namespace.
 * ------------------------------------------------------------------------
 */
void
Itcl_ClearImportNamesp(ns)
    Itcl_Namespace ns;       /* namespace to be modified */
{
    Namespace *nsPtr = (Namespace*)ns;

    register Itcl_ListElem *elem, *import;
    NamespImportRef *nsref;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));

    /*
     *  Clear the command/variable caches for this namespace,
     *  and for any namespace using it.  Do this now, while we
     *  still know what the import and importedBy lists are.
     */
    Itcl_NsCacheClear(ns);

    /*
     *  Clear the import list by deleting each element.  Go to
     *  each namespace that is no longer imported and remove this
     *  namespace from the "importedByList".
     */
    elem = Itcl_FirstListElem(&nsPtr->importList);
    while (elem) {
        nsref = (NamespImportRef*)Itcl_GetListValue(elem);

        import = Itcl_FirstListElem(&nsref->namesp->importedByList);
        while (import) {
            if (Itcl_GetListValue(import) == (ClientData)ns) {
                import = Itcl_DeleteListElem(import);
            }
            else {
                import = Itcl_NextListElem(import);
            }
        }

        ItclDeleteNamespImportRef(nsref);
        elem = Itcl_DeleteListElem(elem);
    }
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_InsertImportNamesp2()
 *
 *  Finds the <pos> in the import list of the specified namespace, and
 *  then adds a new <import> reference just before the <pos>.  If the
 *  <pos> is NULL or cannot be found, then the <import> reference is
 *  inserted at the start of the list.
 *
 *  Importing a namespace effectively includes all of the commands and
 *  variables in the imported namespace to the given namespace.  If the
 *  protection level is ITCL_PUBLIC, then only public commands/variables
 *  are added; if it is ITCL_PROTECTED, then both public and protected
 *  commands/variables are added.  During command/variable lookup,
 *  if a reference cannot be resolved locally, the list of imported
 *  namespaces is searched in order before resorting to the "unknown"
 *  facility.  The imported namespace at the start of the list has
 *  highest search priority.
 * ------------------------------------------------------------------------
 */
void
Itcl_InsertImportNamesp2(ns,import,pLevel,pos)
    Itcl_Namespace ns;       /* namespace to be modified */
    Itcl_Namespace import;   /* namespace to be imported */
    int pLevel;              /* protection level for import */
    Itcl_Namespace pos;      /* namespace in import list for insert position */
{
    Namespace *nsPtr = (Namespace*)ns;
    Namespace *importPtr = (Namespace*)import;

    register Itcl_ListElem *elem;
    NamespImportRef* nsref;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));
    assert(importPtr != NULL && (importPtr->flags&ITCL_NS_ALIVE));

    /*
     *  If the import namespace is already on the list, then
     *  remove it.  Find the specified position and add the
     *  import namespace to the list.
     */
    Itcl_RemoveImportNamesp(ns, import);

    elem = Itcl_FirstListElem(&nsPtr->importList);
    while (elem) {
        nsref = (NamespImportRef*)Itcl_GetListValue(elem);
        if (nsref->namesp == (Namespace*)pos) {
            break;
        }
        elem = Itcl_NextListElem(elem);
    }

    nsref = ItclCreateNamespImportRef(importPtr,pLevel);
    if (elem == NULL) {
        Itcl_InsertList(&nsPtr->importList, (ClientData)nsref);
    }
    else {
        Itcl_InsertListElem(elem, (ClientData)nsref);
    }
    Itcl_AppendList(&importPtr->importedByList, (ClientData)ns);

    /*
     *  Clear the command/variable caches for this namespace,
     *  and for any namespace using it.
     */
    Itcl_NsCacheClear(ns);
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_AppendImportNamesp2()
 *
 *  Finds the <pos> in the import list of the specified namespace, and
 *  then adds a new <import> reference just after the <pos>.  If the
 *  <pos> is NULL or cannot be found, then the <import> reference is
 *  appended at the end of the list.
 *
 *  Importing a namespace effectively includes all of the commands and
 *  variables in the imported namespace to the given namespace.  If the
 *  protection level is ITCL_PUBLIC, then only public commands/variables
 *  are added; if it is ITCL_PROTECTED, then both public and protected
 *  commands/variables are added.  During command/variable lookup,
 *  if a reference cannot be resolved locally, the list of imported
 *  namespaces is searched in order before resorting to the "unknown"
 *  facility.  The imported namespace at the start of the list has
 *  highest search priority.
 * ------------------------------------------------------------------------
 */
void
Itcl_AppendImportNamesp2(ns,import,pLevel,pos)
    Itcl_Namespace ns;       /* namespace to be modified */
    Itcl_Namespace import;   /* namespace to be imported */
    int pLevel;              /* protection level for import */
    Itcl_Namespace pos;      /* namespace in import list for insert position */
{
    Namespace *nsPtr = (Namespace*)ns;
    Namespace *importPtr = (Namespace*)import;

    register Itcl_ListElem *elem;
    NamespImportRef* nsref;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));
    assert(importPtr != NULL && (importPtr->flags&ITCL_NS_ALIVE));

    /*
     *  If the import namespace is already on the list, then
     *  remove it.  Find the specified position and add the
     *  import namespace to the list.
     */
    Itcl_RemoveImportNamesp(ns, import);

    elem = Itcl_FirstListElem(&nsPtr->importList);
    while (elem) {
        nsref = (NamespImportRef*)Itcl_GetListValue(elem);
        if (nsref->namesp == (Namespace*)pos) {
            break;
        }
        elem = Itcl_NextListElem(elem);
    }

    nsref = ItclCreateNamespImportRef(importPtr,pLevel);
    if (elem == NULL) {
        Itcl_AppendList(&nsPtr->importList, (ClientData)nsref);
    }
    else {
        Itcl_AppendListElem(elem, (ClientData)nsref);
    }
    Itcl_AppendList(&importPtr->importedByList, (ClientData)ns);

    /*
     *  Clear the command/variable caches for this namespace,
     *  and for any namespace using it.
     */
    Itcl_NsCacheClear(ns);
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_RemoveImportNamesp()
 *
 *  Removes an import reference to the specified namespace.  Commands
 *  and variables in the imported namespace will no longer be included
 *  in the specified namespace.  If the "import" namespace is not on
 *  the import list of the other namespace, this procedure does nothing.
 * ------------------------------------------------------------------------
 */
void
Itcl_RemoveImportNamesp(ns,import)
    Itcl_Namespace ns;       /* namespace to be modified */
    Itcl_Namespace import;   /* import namespace to be dropped */
{
    Namespace *nsPtr = (Namespace*)ns;
    Namespace *importPtr = (Namespace*)import;

    register Itcl_ListElem *elem;
    NamespImportRef* nsref;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));
    assert(importPtr != NULL && (importPtr->flags&ITCL_NS_ALIVE));

    /*
     *  Remove the import from the importList.
     */
    elem = Itcl_FirstListElem(&nsPtr->importList);
    while (elem) {
        nsref = (NamespImportRef*)Itcl_GetListValue(elem);
        if (nsref->namesp == (Namespace*)import) {
            ItclDeleteNamespImportRef(nsref);
            elem = Itcl_DeleteListElem(elem);
        }
        else {
            elem = Itcl_NextListElem(elem);
        }
    }

    /*
     *  Remove the namespace from the importedByList of the import.
     */
    elem = Itcl_FirstListElem(&importPtr->importedByList);
    while (elem) {
        if (Itcl_GetListValue(elem) == (ClientData)ns) {
            elem = Itcl_DeleteListElem(elem);
        }
        else {
            elem = Itcl_NextListElem(elem);
        }
    }

    /*
     *  Clear the command/variable caches for this namespace,
     *  and for any namespace using it.
     */
    Itcl_NsCacheClear(ns);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_GetImportNamesp()
 *
 *  Builds a list of imported namespaces for the specified namespace,
 *  returned in the order that they were added to the import list.
 *  Each element in the list is really a pointer to a NamespImportRef
 *  record, giving both the imported namespace and its protection level.
 *  These records are valid as long as the import list for the namespace
 *  remains intact.
 *
 *  The listPtr is assumed to point to space allocated for a linked list,
 *  but the list itself should not yet be initialized.  Updates the
 *  linked list to contain a list of namespace pointers.  This list
 *  should be later freed by calling:
 *
 *     Itcl_DeleteList(listPtr);
 *
 * ------------------------------------------------------------------------
 */
void
Itcl_GetImportNamesp(ns,listPtr)
    Itcl_Namespace ns;        /* namespace to be checked */
    Itcl_List* listPtr;       /* returns a list of namespaces */
{
    Namespace *nsPtr = (Namespace*)ns;
    Itcl_ListElem *elem;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));

    Itcl_InitList(listPtr);

    elem = Itcl_FirstListElem(&nsPtr->importList);
    while (elem) {
        Itcl_AppendList(listPtr, Itcl_GetListValue(elem));
        elem = Itcl_NextListElem(elem);
    }
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_GetAllImportNamesp()
 *
 *  Builds a complete list of imported namespaces for the specified
 *  namespace, returned in the order that they should be searched for
 *  command/variable lookups.  Each namespace appears only once in this
 *  list, even though it may be imported from several different places
 *  in the general tree.  Each element in the list is really a pointer
 *  to a NamespImportRef record, giving both the imported namespace
 *  and its protection level.  These records are valid as long as the
 *  import list for the namespace remains intact.
 *
 *  The listPtr is assumed to point to space allocated for a linked list,
 *  but the list itself should not yet be initialized.  Updates the linked
 *  list to contain a list of namespace pointers.  This list should be
 *  later freed by calling:
 *
 *     Itcl_DeleteList(listPtr);
 *
 * ------------------------------------------------------------------------
 */
void
Itcl_GetAllImportNamesp(ns,listPtr)
    Itcl_Namespace ns;        /* namespace to be checked */
    Itcl_List* listPtr;       /* returns a list of namespaces */
{
    Namespace *nsPtr = (Namespace*)ns;

    Itcl_Stack nsStack;
    Itcl_Stack prStack;
    Itcl_ListElem *elem;
    Namespace* searchNs;
    NamespImportRef* nsref;
    Tcl_HashTable visited;
    int pLevel, nextLevel, newEntry;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));

    /*
     *  If the complete list of imports does not yet exist,
     *  then build it.
     */
    if (Itcl_GetListLength(&nsPtr->importAllList) == 0) {
        Tcl_InitHashTable(&visited, TCL_ONE_WORD_KEYS);

        Itcl_InitStack(&nsStack);
        Itcl_InitStack(&prStack);

        Itcl_PushStack((ClientData)nsPtr, &nsStack);
        Itcl_PushStack((ClientData)ITCL_PROTECTED, &prStack);

        while (Itcl_GetStackSize(&nsStack) > 0) {
            searchNs = (Namespace*)Itcl_PopStack(&nsStack);
            pLevel = (int)Itcl_PopStack(&prStack);

            Tcl_CreateHashEntry(&visited, (char*)searchNs, &newEntry);
            if (newEntry) {
                nsref = ItclCreateNamespImportRef(searchNs,pLevel);
                Itcl_AppendList(&nsPtr->importAllList, (ClientData)nsref);
            }

            /*
             *  Put other imported namespaces on the stack.  If a
             *  namespace is found that is already on the list,
             *  ignore it.  This avoids infinite loops in the search
             *  process.
             *
             *  Push other imported namespaces on the stack in
             *  reverse order, so they will be traversed in the proper
             *  order when they are popped off the stack.
             */
            elem = Itcl_LastListElem(&searchNs->importList);
            while (elem) {
                nsref = (NamespImportRef*)Itcl_GetListValue(elem);

                if (!Tcl_FindHashEntry(&visited, (char*)nsref->namesp)) {
                    nextLevel = (nsref->protection < pLevel)
                        ? nsref->protection : pLevel;

                    Itcl_PushStack((ClientData)nsref->namesp, &nsStack);
                    Itcl_PushStack((ClientData)nextLevel, &prStack);
                }
                elem = Itcl_PrevListElem(elem);
            }
        }
        Itcl_DeleteStack(&nsStack);
        Itcl_DeleteStack(&prStack);
        Tcl_DeleteHashTable(&visited);
    }

    /*
     *  Return a copy of this list.
     */
    Itcl_InitList(listPtr);

    elem = Itcl_FirstListElem(&nsPtr->importAllList);
    while (elem) {
        Itcl_AppendList(listPtr, Itcl_GetListValue(elem));
        elem = Itcl_NextListElem(elem);
    }
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_GetAllImportedByNamesp()
 *
 *  Builds a complete list of namespaces that import the specified
 *  namespace.  Each namespace appears only once in this list, even
 *  though it may be imported from several different places in the
 *  general tree.  The listPtr is assumed to point to space allocated
 *  for a linked list, but the list itself should not yet be initialized.
 *  Updates the linked list to contain a list of namespace pointers.
 *  This list should be later freed by calling:
 *
 *     Itcl_DeleteList(listPtr);
 *
 * ------------------------------------------------------------------------
 */
void
Itcl_GetAllImportedByNamesp(ns,listPtr)
    Itcl_Namespace ns;        /* namespace to be checked */
    Itcl_List* listPtr;       /* returns a list of namespaces */
{
    Namespace *nsPtr = (Namespace*)ns;

    ClientData cd;
    Itcl_Stack stack;
    Itcl_ListElem *elem;
    Itcl_ListElem *history;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));

    Itcl_InitList(listPtr);

    Itcl_InitStack(&stack);
    Itcl_PushStack((ClientData)nsPtr, &stack);

    while (Itcl_GetStackSize(&stack) > 0) {
        nsPtr = (Namespace*)Itcl_PopStack(&stack);

        Itcl_AppendList(listPtr, (ClientData)nsPtr);

        /*
         *  Put other imported namespaces on the stack.
         *  Keep a history of namespaces already in the list,
         *  to avoid infinite loops in the search process.
         */
        elem = Itcl_LastListElem(&nsPtr->importedByList);
        while (elem) {
            cd = Itcl_GetListValue(elem);

            history = Itcl_LastListElem(listPtr);
            while (history) {
                if (Itcl_GetListValue(history) == cd) {
                    break;
                }
                history = Itcl_PrevListElem(history);
            }
            if (!history) {
                Itcl_PushStack(cd, &stack);
            }
            elem = Itcl_PrevListElem(elem);
        }
    }
    Itcl_DeleteStack(&stack);
}

/*
 * ------------------------------------------------------------------------
 *  ItclCanAccessNamesp()
 *
 *  Checks to see if a command/variable with the specified protection
 *  level can be accessed from another namespace.  Public things
 *  can always be accessed.  Protected things can be accessed if the
 *  "from" namespace is imported in "protected" mode.  Private things
 *  can never be accessed.  Returns 1/0 indicating true/false.
 * ------------------------------------------------------------------------
 */
int
ItclCanAccessNamesp(nsPtr,fromPtr,pLevel)
    Namespace* nsPtr;      /* namespace to be checked */
    Namespace* fromPtr;    /* namespace requesting access */
    int pLevel;            /* protection level of thing being accessed */
{
    Itcl_List imports;
    Itcl_ListElem *elem;
    NamespImportRef* nsref;
    int access;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));
    assert(fromPtr != NULL && (fromPtr->flags&ITCL_NS_ALIVE));

    /*
     *  If the protection level is "public" or "private", then the
     *  answer is known immediately.
     */
    if (pLevel == ITCL_PUBLIC) {
        return 1;
    }
    else if (pLevel == ITCL_PRIVATE) {
        return (nsPtr == fromPtr);
    }

    /*
     *  If the protection level is "protected", then check the
     *  import list and see if the "from" namespace can be found
     *  along a path in "protected" mode.
     */
    assert (pLevel == ITCL_PROTECTED);

    Itcl_GetAllImportNamesp((Itcl_Namespace)fromPtr, &imports);
    access = 0;

    elem = Itcl_FirstListElem(&imports);
    while (elem) {
        nsref = (NamespImportRef*)Itcl_GetListValue(elem);
        if (nsref->namesp == nsPtr) {
            access = (nsref->protection >= ITCL_PROTECTED);
            break;
        }
        elem = Itcl_NextListElem(elem);
    }
    Itcl_DeleteList(&imports);
    return access;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_EvalArgs()
 *
 *  Performs an actual Tcl command evaluation given a list of command
 *  line arguments.  These arguments are parsed into the usual (argc,argv)
 *  list passed to command handling routines, and have all necessary
 *  command and variable substitutions.  This procedure treats the first
 *  argument as the command name.  It locates and executes the command
 *  in the following manner:
 *
 *    - look for the command in the active namespace
 *    - follow any "::" scope qualifiers to find the command
 *    - search for the command in the "import" list for the active namespace
 *
 *    - invoke the "unknown" command in the active namespace
 *
 * ------------------------------------------------------------------------
 */
int
Itcl_EvalArgs(interp,cmdStart,cmdEnd,argc,argv)
Tcl_Interp* interp;   /* interpreter performing evaluation */
char* cmdStart;       /* command string (for command trace) */
char* cmdEnd;         /* end of command string (for termination at ']') */
int argc;             /* number of command-line arguments */
char** argv;          /* command-line argument strings */
{
    return Itcl_EvalArgsFull(interp,cmdStart,cmdEnd,argc,argv,NULL,NULL);
}

int
Itcl_EvalArgsFull(interp,cmdStart,cmdEnd,argc,argv,clientData,procEntry)
Tcl_Interp *interp;   /* interpreter performing evaluation */
char* cmdStart;       /* command string (for command trace) */
char* cmdEnd;         /* end of command string (for termination at ']') */
int argc;             /* number of command-line arguments */
char** argv;          /* command-line argument strings */
ClientData clientData;	/* client data for proc */
Tcl_CmdProc *procEntry;	/* optional entry for tcl command */
{
  Interp* iPtr = (Interp*)interp;
  int result = TCL_OK;

  int i;
  char saved;
  Tcl_Command cmd;
  Command* cmdPtr;
  Trace *tracePtr;
  int splitArgc,scopedArgc;
  char **splitArgv,**scopedArgv;
  Tcl_DString buffer;

#define ITCL_SCOPED_CMD_LEN 100
  char *argvBuffer[ITCL_SCOPED_CMD_LEN];

  if(!procEntry)
  {
    /*
     *  Follow the lookup scheme for namespaces:
     *   - look for the command in the active namespace
     *   - follow any "::" scope qualifiers to find the command
     *   - search for the command in the "import" list for the active
     *       namespace
     */
    if(Itcl_FindCommand(interp, argv[0], 0, &cmd) != TCL_OK)
    {
	  return TCL_ERROR;
    }
      
    /* Maybe this is a pre-compiled Itcl class */
    if(cmd == NULL)
    {  
	  int *(*proc)();
	  char proc_name[256];
  	  sprintf(proc_name,"%s_class_Init", argv[0]);

	  proc = NULL; /* Itcl_FindC(proc_name);*/
	  if(proc == NULL)
      {

#if defined __sun
	    void *handle;
	    handle = (void *) dlopen(0,2);
	    /*if(handle==NULL) printf("ERROR: handle=0x%08x\n",handle);fflush(stdout);*/
	    proc = (int *(*)()) dlsym(handle,proc_name);
	    dlclose(handle);
#endif

#ifdef Linux
	    void *handle;

	    handle = (void *) dlopen(0,RTLD_NOW);
	    /*if(handle==NULL) printf("ERROR: handle=0x%08x\n",handle);fflush(stdout);*/
	    proc = (int *(*)()) dlsym(handle,proc_name);
	    /*if(proc==NULL) printf("ERROR: proc=0x%08x, name>%s<\n",proc,proc_name);fflush(stdout);*/
	    dlclose(handle);
#endif

#ifdef VXWORKS
	    SYM_TYPE    psymType;	/* symbol type */
	    long        sres = 0;
	    Tcl_DString newName;	/* add "_" in front of name for 68K not PPC*/
	    Tcl_DStringInit(&newName);
#ifndef VXWORKSPPC
	    Tcl_DStringAppend(&newName, "_", 1);
#endif
	    Tcl_DStringAppend(&newName, proc_name, -1);
	    sres = symFindByName (sysSymTbl, Tcl_DStringValue(&newName), (char *) &proc, &psymType);
	    Tcl_DStringFree(&newName);
	    if(sres == ERROR)
        {
	      proc = NULL;
	    };
#endif 
	  }
	  if(proc)
      {
	    if((*proc)(interp) != TCL_OK)
        {
	      fprintf (stderr, "%s\n", interp->result);
	      exit(3);
	    }
	  }

	  if(Itcl_FindCommand(interp, argv[0], 0, &cmd) != TCL_OK)
      {
	    return TCL_ERROR;
	  }
	}

    /*
     *  Unknown command:
     *  - look for {@scope namespace command arg arg...}
     *  - use "unknown" command to handle unknown commands
     */
      
    if(cmd == NULL)  /* use unknown? */
    {
	  if(*argv[0] == '@' && strncmp(argv[0],"@scope ",7) == 0)
      {
	    if(Tcl_SplitList(interp,argv[0],&splitArgc,&splitArgv) != TCL_OK)
        {
	      Tcl_DStringInit(&buffer);
	      Tcl_DStringAppend
	        (&buffer,"\n    (while parsing scoped command \"", -1);
	      Tcl_DStringAppend(&buffer, argv[0], -1);
	      Tcl_DStringAppend(&buffer, "\")", -1);
	      Tcl_AddErrorInfo(interp, Tcl_DStringValue(&buffer));
	      Tcl_DStringFree(&buffer);
	      return(TCL_ERROR);
	    }

	    /*
	     * Splice the arguments together and build a new command
	     */
	  
	    scopedArgc = splitArgc+argc-1;
	    if(scopedArgc > ITCL_SCOPED_CMD_LEN)
        {
	      scopedArgv =
	        (char**)ckalloc((unsigned)(scopedArgc*sizeof(char*)));
	    }
        else
        {
	      scopedArgv = argvBuffer;
	    }
	  
	    for(i=0; i < splitArgc ; i++)
        {
	      scopedArgv[i] = splitArgv[i];
	    }
	    for(i=1; i < argc; i++)
        {
	      scopedArgv[i+splitArgc-1] = argv[i];
	    }
	  
	    result = Itcl_EvalArgsFull(interp,cmdStart,cmdEnd,
				     scopedArgc,scopedArgv,NULL,NULL);
	  
	    if(scopedArgv != argvBuffer)
        {
	      ckfree((char*)scopedArgv);
	    }
	    ckfree((char*)splitArgv);
	    return result;
	  }
	
	  if((Itcl_FindCommand(interp, "unknown", 0, &cmd) != TCL_OK) ||
	      (cmd == NULL))
      {
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, "invalid command name \"",
			   argv[0], "\"", (char*)NULL);
	    return TCL_ERROR;
	  }
	  for (i=argc; i >= 0; i--)
      {
	    argv[i+1] = argv[i];
	  }
	  argv[0] = "unknown";
	  argc++;
    }
    cmdPtr = (Command*)cmd;
    procEntry = cmdPtr->proc;
    clientData = cmdPtr->clientData;
  }

  /*
   * Call trace procedures, if any.
   */
    
  for(tracePtr = iPtr->tracePtr; tracePtr != NULL;
        tracePtr = tracePtr->nextPtr)
  {
    if(tracePtr->level < iPtr->numLevels)
    {
      continue;
    }
    if(cmdEnd)
    {         /* terminate command string, if needed */
      saved = *cmdEnd;
      *cmdEnd = '\0';
    }

    (*tracePtr->proc)(tracePtr->clientData, interp, iPtr->numLevels,
            cmdStart, cmdPtr->proc, cmdPtr->clientData, argc, argv);

    if(cmdEnd)
    {         /* put command string back the way it was */
      *cmdEnd = saved;
    }
  }

  /*
   * At long last, invoke the command procedure.  Reset the
   * result to its default empty value first (it could have
   * gotten changed by earlier commands in the same command
   * string).
   */
    
  iPtr->cmdCount++;
  Tcl_FreeResult(iPtr);
  iPtr->result = iPtr->resultSpace;
  iPtr->resultSpace[0] = 0;

  result = (*procEntry)(clientData, interp, argc, argv);

  if(tcl_AsyncReady)
  {
    result = Tcl_AsyncInvoke(interp, result);
  }

  return(result);
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_UplevelEval()
 *
 *  Similar to Tcl_Eval(), but executes the command string one level up
 *  in the call stack.  This merely provides a convenient means up doing
 *  an "uplevel" call from C code.
 *
 *  Returns a Tcl status code (e.g., TCL_OK, TCL_ERROR, etc.) along
 *  with a result string in interp->result.
 * ------------------------------------------------------------------------
 */
int
Itcl_UplevelEval(interp,cmd)
    Tcl_Interp* interp;   /* interpreter performing evaluation */
    char* cmd;            /* command string to be executed */
{
    Interp* iPtr = (Interp*)interp;

    int status;
    Itcl_Namespace ns;
    ClientData cdata;
    Itcl_ActiveNamespace nsToken;
    CallFrame *savedVarFramePtr, *framePtr;

    /*
     *  Modify the call stack in the interpreter to point up
     *  one level.
     */
    if (iPtr->varFramePtr && iPtr->varFramePtr->callerVarPtr) {
        framePtr = iPtr->varFramePtr->callerVarPtr;
    } else {
        framePtr = NULL;
    }
    savedVarFramePtr = iPtr->varFramePtr;
    iPtr->varFramePtr = framePtr;

    if (framePtr == NULL) {
        ns    = (Itcl_Namespace)iPtr->globalNs;
        cdata = NULL;
    }
    else {
        ns    = (Itcl_Namespace)framePtr->activeNs;
        cdata = framePtr->activeData;
    }

    /*
     *  Set up the namespace context and execute the command string.
     */
    nsToken = Itcl_ActivateNamesp2(interp, ns, cdata);
    if (nsToken) {
        status = Tcl_Eval(interp, cmd);
        Itcl_DeactivateNamesp(interp,nsToken);
    }
    else {
        status = TCL_ERROR;
    }

    /*
     *  Restore the interpreter and return the result.
     */
    iPtr->varFramePtr = savedVarFramePtr;
    return status;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_NamespEval()
 *
 *  Similar to Tcl_Eval(), but executes the command string in the
 *  specified namespace.  If the namespace has been destroyed, then
 *  this call returns an error.
 *
 *  Returns a Tcl status code (e.g., TCL_OK, TCL_ERROR, etc.) along
 *  with a result string in interp->result.
 * ------------------------------------------------------------------------
 */
int
Itcl_NamespEval(interp,ns,cmd)
    Tcl_Interp* interp;       /* interpreter performing evaluation */
    Itcl_Namespace ns;        /* namespace to use for command evalution */
    char* cmd;                /* command string to be executed */
{
    Interp* iPtr = (Interp*)interp;

    Itcl_ActiveNamespace nsToken;
    int status;
    CallFrame frame;

    /*
     *  Set up the namespace context.  Work at the global scope
     *  within the namespace.
     */
    nsToken = Itcl_ActivateNamesp(interp, ns);
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

    status = Tcl_Eval(interp, cmd);

    Itcl_DeactivateNamesp(interp,nsToken);
    iPtr->framePtr = frame.callerPtr;
    iPtr->varFramePtr = frame.callerVarPtr;

    return status;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_NamespVarEval()
 *
 *  Similar to Tcl_VarEval(), but executes the command string in the
 *  specified namespace.  If the namespace has been destroyed, then
 *  this call returns an error.
 *
 *  Returns a Tcl status code (e.g., TCL_OK, TCL_ERROR, etc.) along
 *  with a result string in interp->result.
 * ------------------------------------------------------------------------
 */
/* VARARGS2 */ /* ARGSUSED */
int
#ifdef USEVARARGS
Itcl_NamespVarEval(va_alist)
    va_dcl
#else
Itcl_NamespVarEval(Tcl_Interp *first, ...)
#endif
{
    va_list argList;
    Tcl_DString buffer;
    char *string;
    Tcl_Interp *interp;
    Itcl_Namespace context;
    int result;

    /*
     *  Copy the strings one after the other into a single larger
     *  string, and then evaluate in the namespace context.
     */
#ifdef USEVARARGS
    va_start(argList);
    interp = va_arg(argList, Tcl_Interp*);
#else
    va_start(argList, first);
    interp = first;
#endif
    context = va_arg(argList, Itcl_Namespace);

    Tcl_DStringInit(&buffer);
    while (1) {
        string = va_arg(argList, char *);
        if (string == NULL) {
            break;
        }
        Tcl_DStringAppend(&buffer, string, -1);
    }
    va_end(argList);

    result = Itcl_NamespEval(interp, context, Tcl_DStringValue(&buffer));
    Tcl_DStringFree(&buffer);
    return result;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_NsCacheClear()
 *
 *  Clears the cache of command/variable names recognized in a particular
 *  namespace and in all namespaces using it (directly or indirectly)
 *  via the "import" list.
 *
 *  The cache is kept to avoid complex lookups involving the "import"
 *  list.  If a command/variable is not found within the cache, then all
 *  imported namespaces are searched in turn.  If the command is
 *  eventually found, it is automatically installed in the cache so that
 *  next time it can be found immediately.  This procedure is used to
 *  clear the cache whenever the import list changes.  Subsequent calls
 *  to Itcl_FindCommand() and Itcl_FindVariable() will begin rebuilding
 *  the cache from scratch.
 * ------------------------------------------------------------------------
 */
void
Itcl_NsCacheClear(ns)
    Itcl_Namespace ns;   /* namespace being modified */
{
    Namespace *nsPtr = (Namespace*)ns;

    Itcl_List import;
    Itcl_ListElem *nsElem;
    Itcl_ListElem *elem;
    register Tcl_HashEntry *entry;
    Tcl_HashSearch search;
    NamespImportRef *nsref;
    NamespCacheRef *cacheRef;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));

    /*
     *  Make a list of namespaces to clear.  Start with the list
     *  of all namespaces importing this namespace, then tack the
     *  specified namespace onto the end.
     */
    Itcl_GetAllImportedByNamesp(ns, &import);
    Itcl_AppendList(&import, (ClientData)nsPtr);

    nsElem = Itcl_FirstListElem(&import);
    while (nsElem) {
        nsPtr = (Namespace*)Itcl_GetListValue(nsElem);

        /*
         *  Clear the command cache.  Release the claim on each
         *  cache reference, and delete those that are no longer needed.
         */
        entry = Tcl_FirstHashEntry(&nsPtr->commandTable, &search);
        while (entry) {
            cacheRef = (NamespCacheRef*)Tcl_GetHashValue(entry);
            if (--cacheRef->usage == 0 && cacheRef->element == NULL) {
                ckfree((char*)cacheRef);
            }
            Tcl_DeleteHashEntry(entry);
            entry = Tcl_NextHashEntry(&search);
        }

        /*
         *  Clear the variable cache.  Variable records are not owned by
         *  the cache, so just delete the entries in the variable table.
         */
        entry = Tcl_FirstHashEntry(&nsPtr->variableTable, &search);
        while (entry) {
            cacheRef = (NamespCacheRef*)Tcl_GetHashValue(entry);
            if (--cacheRef->usage == 0 && cacheRef->element == NULL) {
                ckfree((char*)cacheRef);
            }
            Tcl_DeleteHashEntry(entry);
            entry = Tcl_NextHashEntry(&search);
        }

        /*
         *  Clear the importAllList cache as well.
         */
        elem = Itcl_FirstListElem(&nsPtr->importAllList);
        while (elem) {
            nsref = (NamespImportRef*)Itcl_GetListValue(elem);
            ItclDeleteNamespImportRef(nsref);
            elem = Itcl_DeleteListElem(elem);
        }
        nsElem = Itcl_NextListElem(nsElem);
    }
    Itcl_DeleteList(&import);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_NsCacheNamespChanged()
 *
 *  Clears the command/variable cache in the specified namespace and
 *  any other namespace that uses it.  Also clears the namespace
 *  cache for all parents and their users.  This is usually invoked
 *  when a namespace adds/deletes child namespaces, since relative
 *  paths like "foo::x" or "foo::bar::x" can change their meaning
 *  at this time.  Any such entries in the cache must be cleared, and
 *  rather than finding these individually, it is easier to clear the
 *  cache wholesale and start fresh.
 * ------------------------------------------------------------------------
 */
void
Itcl_NsCacheNamespChanged(ns)
    Itcl_Namespace ns;    /* namespace adding/deleting a child namespace */
{
    Namespace *nsPtr = (Namespace*)ns;

    Itcl_List import;
    Itcl_ListElem *elem;
    Namespace *userPtr;

    /*
     *  Start with the specified namespace, and work upward through
     *  all parent namespaces to the global scope.  Clear the cache
     *  in each parent, and in anyone using the parent.
     */
    while (nsPtr) {
        Itcl_NsCacheClear((Itcl_Namespace)nsPtr);

        Itcl_GetAllImportedByNamesp((Itcl_Namespace)nsPtr, &import);
        elem = Itcl_FirstListElem(&import);
        while (elem) {
            userPtr = (Namespace*)Itcl_GetListValue(elem);
            Itcl_NsCacheClear((Itcl_Namespace)userPtr);

            elem = Itcl_NextListElem(elem);
        }
        Itcl_DeleteList(&import);

        nsPtr = nsPtr->parent;
    }
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_NsCacheCmdChanged()
 *
 *  Clears a single entry in the cache of command names recognized in
 *  a particular namespace and in all namespaces using it (directly or
 *  indirectly) via the "import" list.  Invoked whenever a command is
 *  created or deleted in a namespace, since that command may have been
 *  stored in the cache, and should be looked up again when needed.
 * ------------------------------------------------------------------------
 */
void
Itcl_NsCacheCmdChanged(ns,name)
    Itcl_Namespace ns;    /* namespace being modified */
    char* name;           /* name of command that changed */
{
    Namespace *nsPtr = (Namespace*)ns;

    char *scopedName;
    Tcl_DString *path, *buffer, *tmp;
    Itcl_List import;
    Itcl_ListElem* usingElem;
    Namespace *userPtr;
    Tcl_HashEntry *cache;
    NamespCacheRef *cacheRef;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));

    path = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
    Tcl_DStringInit(path);

    buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
    Tcl_DStringInit(buffer);

    /*
     *  Scan through all namespaces starting from the one being
     *  changed, and working through all parents upward to the
     *  global scope.  For each parent, reset the cache of any
     *  namespace using the parent.  Use the name that the parent
     *  sees when looking down on the element that was deleted.
     */
    Tcl_DStringAppend(path, name, -1);
    scopedName = Tcl_DStringValue(path);

    while (nsPtr) {
        Itcl_GetAllImportedByNamesp((Itcl_Namespace)nsPtr, &import);

        usingElem = Itcl_FirstListElem(&import);
        while (usingElem) {
            userPtr = (Namespace*)Itcl_GetListValue(usingElem);
            cache = Tcl_FindHashEntry(&userPtr->commandTable, scopedName);
            if (cache) {
                cacheRef = (NamespCacheRef*)Tcl_GetHashValue(cache);
                if (--cacheRef->usage == 0 && cacheRef->element == NULL) {
                    ckfree((char*)cacheRef);
                }
                Tcl_DeleteHashEntry(cache);
            }
            usingElem = Itcl_NextListElem(usingElem);
        }
        Itcl_DeleteList(&import);

        /*
         * Build a new path name by prepending the current namespace context.
         */
        Tcl_DStringTrunc(buffer, 0);
        Tcl_DStringAppend(buffer, nsPtr->name, -1);
        Tcl_DStringAppend(buffer, "::", -1);
        Tcl_DStringAppend(buffer, scopedName, -1);

        tmp = buffer;
        buffer = path;
        path = tmp;
        scopedName = Tcl_DStringValue(path);

        nsPtr = nsPtr->parent;
    }

    Tcl_DStringFree(path);
    ckfree((char*)path);

    Tcl_DStringFree(buffer);
    ckfree((char*)buffer);
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_NsCacheVarChanged()
 *
 *  Clears a single entry in the cache of variable names recognized in
 *  a particular namespace and in all namespaces using it (directly or
 *  indirectly) via the "import" list.  Invoked whenever a variable is
 *  created or deleted in a namespace, since that variable may have been
 *  stored in the cache, and should be looked up again when needed.
 * ------------------------------------------------------------------------
 */
void
Itcl_NsCacheVarChanged(ns,name)
    Itcl_Namespace ns;    /* namespace being modified */
    char* name;           /* name of variable that changed */
{
    Namespace *nsPtr = (Namespace*)ns;

    char *scopedName;
    Tcl_DString *path, *buffer, *tmp;
    Itcl_List import;
    Itcl_ListElem* usingElem;
    Namespace *userPtr;
    Tcl_HashEntry *cache;
    NamespCacheRef *cacheRef;

    assert(nsPtr != NULL && (nsPtr->flags&ITCL_NS_ALIVE));

    path = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
    Tcl_DStringInit(path);

    buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
    Tcl_DStringInit(buffer);

    /*
     *  Scan through all namespaces starting from the one being
     *  changed, and working through all parents upward to the
     *  global scope.  For each parent, reset the cache of any
     *  namespace using the parent.  Use the name that the parent
     *  sees when looking down on the element that was deleted.
     */
    Tcl_DStringAppend(path, name, -1);
    scopedName = Tcl_DStringValue(path);

    while (nsPtr) {
        Itcl_GetAllImportedByNamesp((Itcl_Namespace)nsPtr, &import);

        usingElem = Itcl_FirstListElem(&import);
        while (usingElem) {
            userPtr = (Namespace*)Itcl_GetListValue(usingElem);
            cache = Tcl_FindHashEntry(&userPtr->variableTable, scopedName);
            if (cache) {
                cacheRef = (NamespCacheRef*)Tcl_GetHashValue(cache);
                if (--cacheRef->usage == 0 && cacheRef->element == NULL) {
                    ckfree((char*)cacheRef);
                }
                Tcl_DeleteHashEntry(cache);
            }
            usingElem = Itcl_NextListElem(usingElem);
        }
        Itcl_DeleteList(&import);

        /*
         * Build a new path name by prepending the current namespace context.
         */
        Tcl_DStringTrunc(buffer, 0);
        Tcl_DStringAppend(buffer, nsPtr->name, -1);
        Tcl_DStringAppend(buffer, "::", -1);
        Tcl_DStringAppend(buffer, scopedName, -1);

        tmp = buffer;
        buffer = path;
        path = tmp;
        scopedName = Tcl_DStringValue(path);

        nsPtr = nsPtr->parent;
    }

    Tcl_DStringFree(path);
    ckfree((char*)path);

    Tcl_DStringFree(buffer);
    ckfree((char*)buffer);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_GetNamespData()
 *
 *  Returns the client data associated with a namespace.
 * ------------------------------------------------------------------------
 */
ClientData
Itcl_GetNamespData(ns)
    Itcl_Namespace ns;   /* namespace being queried */
{
    Namespace *nsPtr = (Namespace*)ns;
    return nsPtr->clientData;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_GetNamespName()
 *
 *  Returns the simple name of a namespace, without any "::" scope
 *  qualifiers.  This string persists until the namespace is destroyed.
 * ------------------------------------------------------------------------
 */
char*
Itcl_GetNamespName(ns)
    Itcl_Namespace ns;   /* namespace being queried */
{
    Namespace *nsPtr = (Namespace*)ns;
    return nsPtr->name;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_GetNamespPath()
 *
 *  Returns the full name of a namespace, starting with the global
 *  "::" scope qualifier, and following the trail of all parent
 *  namespaces required to find it.  Useful for error information,
 *  or when reporting info about a namespace.
 *
 *  Returns a pointer to a string which remains valid until the
 *  next call to this routine.
 * ------------------------------------------------------------------------
 */
char*
Itcl_GetNamespPath(ns)
    Itcl_Namespace ns;   /* namespace being queried */
{
    static Tcl_DString *buffer = NULL;

    Namespace *nsPtr = (Namespace*)ns;
    Itcl_Stack stack;

    /*
     *  If the namespace buffer does not exist, then create
     *  one now.  Otherwise, clear the existing buffer.
     */
    if (buffer == NULL) {
        buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
        Tcl_DStringInit(buffer);
    }
    else {
        Tcl_DStringTrunc(buffer,0);
    }

    /*
     *  If there is no parent, then this is either the global
     *  namespace, or a namespace that is dead.  Return as much
     *  information as possible.
     */
    if (nsPtr->parent == NULL) {
        if (*nsPtr->name == '\0') {
            return "::";
        } else {
            return nsPtr->name;
        }
    }

    /*
     *  Otherwise, search upward from the given namespace and
     *  save the trail of parent namespaces.
     */
    Itcl_InitStack(&stack);
    while (nsPtr && nsPtr->parent) {
        Itcl_PushStack((ClientData)nsPtr, &stack);
        nsPtr = nsPtr->parent;
    }

    /*
     *  Create a string that describes the trail leading
     *  to the given namespace.
     */
    nsPtr = (Namespace*)Itcl_PopStack(&stack);
    while (nsPtr) {
        Tcl_DStringAppend(buffer, "::", -1);
        Tcl_DStringAppend(buffer, nsPtr->name, -1);
        nsPtr = (Namespace*)Itcl_PopStack(&stack);
    }
    Itcl_DeleteStack(&stack);

    return Tcl_DStringValue(buffer);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_GetNamespParent()
 *
 *  Returns the parent namespace of another namespace.  The parent of
 *  the global namespace is NULL.
 * ------------------------------------------------------------------------
 */
Itcl_Namespace
Itcl_GetNamespParent(ns)
    Itcl_Namespace ns;   /* namespace being queried */
{
    Namespace *nsPtr = (Namespace*)ns;
    return (Itcl_Namespace)nsPtr->parent;
}


/*
 * ------------------------------------------------------------------------
 *  ItclFollowNamespPath()
 *
 *  Treats the specified path as a command or variable name with an
 *  arbitrary number of "::" scope qualifiers.  If the path starts with
 *  "::", then it is interpreted absolutely from the global scope.
 *  Otherwise, it is taken relative to the specified namespace.
 *
 *  If "flags" contains ITCL_FIND_LOCAL_ONLY, then the namespace path
 *  is followed and the name is sought in that context only; otherwise,
 *  all namespaces on the "import" list are searched as well.
 *
 *  If "flags" contains ITCL_FIND_AUTO_CREATE, then all components of
 *  the namespace path that cannot be found are automatically created
 *  within their specified parent.  This makes sure that functions
 *  like Tcl_CreateCommand() can succeed no matter what.
 *
 *  If "flags" contains ITCL_FIND_NAMESP, then the path is treated as
 *  a reference to a namespace, and the entire path is followed; otherwise,
 *  only the leading components are treated as namespace names, and the
 *  final path component is returned in "rnamePtr".
 *
 *  If the path contains a syntax error (e.g., ":::") then this procedure
 *  returns TCL_ERROR along with an error message in the interpreter.
 *  Otherwise, it returns TCL_OK.  If the path could be followed, then
 *  "nsPtr" returns the last namespace in the trail, and "namePtr" returns
 *  a pointer (within "path") to the simple name at the end of the
 *  trail.  If the path is "::" or was treated as a namespace reference,
 *  this procedure returns the namespace in "nsPtr", and NULL in "namePtr".
 * ------------------------------------------------------------------------
 */
int
ItclFollowNamespPath(interp,nsPtr,path,flags,rnsPtr,rnamePtr,rspecRef)
    Tcl_Interp* interp;      /* interpreter containing namespaces */
    Namespace* nsPtr;        /* namespace to search */
    char* path;              /* string path for search */
    int flags;               /* flags controlling lookup */
    Namespace** rnsPtr;      /* returns: namespace at end of the path */
    char** rnamePtr;         /* returns: simple name at end of the path */
    int *rspecRef;           /* returns: non-zero => "::" in path name */
{
    static Tcl_DString *buffer = NULL;

    Interp *iPtr = (Interp*)interp;
    int status = TCL_OK;
    char *wend = NULL;

    char *wstart;
    Tcl_HashEntry *entry;
    Itcl_List imports, qualifiers;
    Itcl_ListElem *elem, *avoid;
    Itcl_Namespace ns;
    NamespImportRef* nsref;
    Itcl_ActiveNamespace nsToken;

    assert(nsPtr != NULL);
    Itcl_InitList(&qualifiers);

    *rspecRef = 0;  /* assume path name has no "::" qualifiers */

    /*
     *  If the path name is NULL and is being treated as a namespace
     *  reference, then return the global namespace.
     */
    wstart = path;
    if (*wstart == '\0' && (flags & ITCL_FIND_NAMESP) != 0) {
        *rnsPtr   = iPtr->globalNs;
        *rnamePtr = NULL;
        goto followPathDone;
    }

    /*
     *  Look for "::" scope qualifiers.  If any are found, then copy
     *  the namespace name to a buffer so it can be dissected.
     */
    wend = strstr(path,"::");
    if (strncmp(path, "tix", 3) == 0) {
	wend = NULL;
    }
    if (wend) {
        *rspecRef = 1;

        if (buffer == NULL) {
            buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
            Tcl_DStringInit(buffer);
        }
        else {
            Tcl_DStringTrunc(buffer,0);
        }
        Tcl_DStringAppend(buffer, path, -1);

        wend = (wend-path) + Tcl_DStringValue(buffer);
        wstart = Tcl_DStringValue(buffer);
    }

    /*
     *  If the first "::" scope qualifier is found right at the
     *  beginning of the path, then treat path as an absolute
     *  reference.  Otherwise, treat relative to the given namespace.
     */
    if (wend == wstart) {
        nsPtr = iPtr->globalNs;

        wstart = wend+2;
        if (*wstart == '\0') {
            *rnsPtr   = nsPtr;
            *rnamePtr = NULL;
            goto followPathDone;
        }
        wend = strstr(wstart,"::");
        Itcl_AppendList(&qualifiers, (ClientData)nsPtr);
    }

    /*
     *  Segment the path according to "::" scope qualifiers and
     *  look up each namespace along the way.  If a namespace cannot
     *  be found and "auto create" mode is set, then create it.
     *  Otherwise, bail out.
     */
    *rnsPtr = NULL;
    *rnamePtr = NULL;

    while (wstart) {
        if ((wend == NULL && (flags & ITCL_FIND_NAMESP) == 0)
            || *wstart == ':' || *wstart == '\0') {
            break;
        }
        if (wend) {
            *wend = '\0';
        }

        ns = NULL;    /* next namespace path component */

        /*
         *  First, look for the namespace locally...
         */
        entry = Tcl_FindHashEntry(&nsPtr->children, wstart);
        if (entry) {
            ns = (Itcl_Namespace)Tcl_GetHashValue(entry);
        }

        /*
         *  Next, if "auto create" is set, create it...
         *  This is needed for commands like Tcl_CreateCommand()
         *  that cannot fail.
         */
        if (ns == NULL && (flags & ITCL_FIND_AUTO_CREATE) != 0) {
            nsToken = Itcl_ActivateNamesp(interp, (Itcl_Namespace)nsPtr);
            if (nsToken == NULL) {
                status = TCL_ERROR;
            }
            else {
                status = Itcl_CreateNamesp(interp, wstart, (ClientData)NULL,
                    (Itcl_DeleteProc*)NULL, &ns);
                Itcl_DeactivateNamesp(interp,nsToken);
            }

            if (status != TCL_OK) {
                goto followPathDone;
            }
        }

        /*
         *  If all else fails, search for the next component in all
         *  other namespaces on the "import" list.
         *
         *  NOTE:  When searching imported namespaces, avoid doubling
         *    back on namespaces already found in the search path.
         *    For example "::foo::foo", will resolve to the namespace
         *    "::foo", and then search for an imported namespace named
         *    "foo".  If "::foo" imports from the global namespace,
         *    then it will see itself coming from the global scope.
         */
        if (ns == NULL && (flags & ITCL_FIND_LOCAL_ONLY) == 0) {
            Itcl_GetAllImportNamesp((Itcl_Namespace)nsPtr, &imports);

            elem = Itcl_FirstListElem(&imports);
            while (elem) {
                nsref = (NamespImportRef*)Itcl_GetListValue(elem);
                nsPtr = nsref->namesp;

                avoid = Itcl_FirstListElem(&qualifiers);
                while (avoid) {
                    if (Itcl_GetListValue(avoid) == (ClientData)nsPtr) {
                        break;
                    }
                    avoid = Itcl_NextListElem(avoid);
                }

                if (!avoid) {
                    entry = Tcl_FindHashEntry(&nsPtr->children, wstart);
                    if (entry) {
                        ns = (Itcl_Namespace)Tcl_GetHashValue(entry);
                        break;
                    }
                }
                elem = Itcl_NextListElem(elem);
            }
            Itcl_DeleteList(&imports);
        }

        /*
         *  At this point, we found a namespace, or we return having
         *  found nothing.
         */
        if (ns) {
            nsPtr = (Namespace*)ns;
            Itcl_AppendList(&qualifiers, (ClientData)nsPtr);
        } else {
            goto followPathDone;
        }

        if (wend == NULL) {
            wstart = NULL;
        } else {
            wstart = wend+2;
            wend   = strstr(wstart,"::");

            if (wend == NULL && (flags & ITCL_FIND_NAMESP) == 0) {
                break;
            }
        }
    }

    /*
     *  At this point, a parent namespace has been found and the
     *  last word is the name of the element within the namespace.
     *  Return the results and signal success.
     */
    if (wstart) {
        if (*wstart == ':') {
            Tcl_AppendResult(interp, "cannot resolve reference \"",
                path, "\": syntax error near \"", wstart, "\"",
                (char*)NULL);
            status = TCL_ERROR;
            goto followPathDone;
        }
        if (*wstart == '\0' && *path != '\0') {
            Tcl_AppendResult(interp, "cannot resolve reference \"",
                path, "\": missing name at end of path",
                (char*)NULL);
            status = TCL_ERROR;
            goto followPathDone;
        }
    }

    *rnsPtr = nsPtr;
    *rnamePtr = wstart;

followPathDone:
    Itcl_DeleteList(&qualifiers);
    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_FindNamesp()
 *
 *  Searches for a namespace along the specified path, which may contain
 *  a series of "::" scope qualifiers.  If the path starts with "::",
 *  then it is interpreted absolutely from the global scope.  Otherwise,
 *  it is taken relative to the currently active namespace.  If "flags"
 *  contains ITCL_FIND_LOCAL_ONLY, then the namespace path is followed
 *  and the name is sought in that context only.  Otherwise, all
 *  namespaces on the "import" list are searched as well.
 *
 *  Returns TCL_ERROR (along with an error message in the interpreter)
 *  if an error is found in the namespace path.  Otherwise, it returns
 *  TCL_OK.  If the namespace is found it is returned in rns; otherwise,
 *  rns will be NULL.
 * ------------------------------------------------------------------------
 */
int
Itcl_FindNamesp(interp,path,flags,rns)
    Tcl_Interp* interp;          /* interpreter containing the namespace */
    char* path;                  /* string path identifying namespace */
    int flags;                   /* flags controlling namespace lookup */
    Itcl_Namespace* rns;         /* returns: namespace */
{
    Interp *iPtr = (Interp*)interp;

    int specificRef;
    Namespace *nsPtr;
    char* name;

    /*
     *  Look up the name in the active namespace, or in the global
     *  scope for absolute path names.
     */
    *rns = NULL;

    if (ItclFollowNamespPath(interp, iPtr->activeNs, path,
        flags|ITCL_FIND_NAMESP, &nsPtr,&name,&specificRef) != TCL_OK) {
        return TCL_ERROR;
    }

    *rns = (Itcl_Namespace)nsPtr;
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_FindCommand()
 *
 *  Searches for a command along the specified path, which may contain
 *  a series of "::" scope qualifiers.  If the path starts with "::",
 *  then it is interpreted absolutely from the global scope.  Otherwise,
 *  it is taken relative to the currently active namespace.  If "flags"
 *  contains ITCL_FIND_LOCAL_ONLY, then the namespace path is followed
 *  and the name is sought in that context only.  Otherwise, all
 *  namespaces on the "import" list are searched as well.  If "flags"
 *  contains ITCL_FIND_NO_PRIVACY, then the command will be found
 *  whether or not it has any protection.  Otherwise, the command will
 *  be found only if access is allowed.
 *
 *  Returns TCL_ERROR, along with an error message in the specified
 *  interpreter, if an error is encountered.  Otherwise, this procedure
 *  returns TCL_OK.  If command lookup was successful, rcmd returns the
 *  command token; otherwise, rcmd will be NULL.
 * ------------------------------------------------------------------------
 */
int
Itcl_FindCommand(interp,path,flags,rcmd)
    Tcl_Interp* interp;        /* interpreter containing the command */
    char* path;                /* string path identifying command */
    int flags;                 /* flags controlling command lookup */
    Tcl_Command* rcmd;         /* returns: command token */
{
    Interp *iPtr = (Interp*)interp;

    int newEntry, specificRef;
    char *name, *tail;
    Tcl_Command cmd;
    Command* cmdPtr;
    Namespace* nsPtr;
    NamespImportRef* nsref;
    Itcl_List imports;
    Itcl_ListElem *elem;
    Tcl_HashEntry* entry;
    Tcl_HashEntry* cache;
    NamespCacheRef *cacheRef;

    /*
     *  See if the command is recognized in the command cache for
     *  the active namespace.  If it is, and if the cache record
     *  is still valid, then return immediately.  Delete any cache
     *  records that have expired.
     */
    cmdPtr = NULL;
    entry  = Tcl_FindHashEntry(&iPtr->activeNs->commandTable, path);

    if (entry != NULL) {
        cacheRef = (NamespCacheRef*)Tcl_GetHashValue(entry);
        if (cacheRef->element != NULL) {
            *rcmd  = (Tcl_Command)cacheRef->element;
            return TCL_OK;
        }
        if (--cacheRef->usage == 0) {
            ckfree((char*)cacheRef);
        }
        Tcl_DeleteHashEntry(entry);
    }

    /*
     *  If the command is not found immediately, the invoke the
     *  command "enforcer" procedure to enforce any special lookup
     *  rules for this namespace.
     */
    if (iPtr->activeNs->cmdEnforcer) {
        if ((*iPtr->activeNs->cmdEnforcer)(interp, path, &cmd) != TCL_OK) {
            return TCL_ERROR;
        }
        if (cmd != NULL) {
            cmdPtr = (Command*)cmd;
        }
    }

    /*
     *  If not, then look up the name in the active namespace, or
     *  in the global scope for absolute path names.
     */
    if (cmdPtr == NULL) {
        if (ItclFollowNamespPath(interp, iPtr->activeNs, path, 0,
            &nsPtr,&name,&specificRef) != TCL_OK) {
            return TCL_ERROR;
        }
        if ((nsPtr != NULL) && (name != NULL)) {
            entry = Tcl_FindHashEntry(&nsPtr->commands, name);
        }
        if (entry != NULL) {
            cmdPtr = (Command*)Tcl_GetHashValue(entry);

            if ((flags & ITCL_FIND_NO_PRIVACY) == 0 &&
                !ItclCanAccessNamesp(nsPtr, iPtr->activeNs,
                    cmdPtr->protection)) {
                cmdPtr = NULL;  /* disallow access */
                entry  = NULL;
            }
        }

        /*
         *  If it is still not found, and if it was not a specific
         *  reference with namespace qualifiers, then check all
         *  namespaces on the "import" list.
         */
        if ((entry == NULL) && !specificRef && (nsPtr != NULL) &&
            ((flags & ITCL_FIND_LOCAL_ONLY) == 0)) {

            for (tail=path; *tail != '\0'; tail++)
                ;
            while ((tail > path) && ((*tail != ':') || (*(tail-1) != ':')))
                tail--;
            if (*tail == ':')
                tail++;

            Itcl_GetAllImportNamesp((Itcl_Namespace)nsPtr, &imports);

            elem = Itcl_FirstListElem(&imports);
            elem = Itcl_NextListElem(elem);  /* skip over active namesp */

            while (elem) {
                nsref = (NamespImportRef*)Itcl_GetListValue(elem);
                entry = Tcl_FindHashEntry(&nsref->namesp->commands, tail);
                if (entry != NULL) {
                    cmdPtr = (Command*)Tcl_GetHashValue(entry);

                    if ((flags & ITCL_FIND_NO_PRIVACY) != 0 ||
                        ItclCanAccessNamesp(nsref->namesp,
                            iPtr->activeNs, cmdPtr->protection)) {
                        nsPtr = nsref->namesp;
                        break;
                    }
                    else {
                        cmdPtr = NULL;  /* disallow access */
                        entry  = NULL;
                    }
                }
                elem = Itcl_NextListElem(elem);
            }
            Itcl_DeleteList(&imports);
        }
        if (entry != NULL) {
            cmdPtr = (Command*)Tcl_GetHashValue(entry);
        }
    }

    /*
     *  If the command was found, then add it to the local cache
     *  to make lookup faster next time.
     */
    if (cmdPtr != NULL) {
        if (cmdPtr->cacheInfo == NULL) {
            cacheRef = (NamespCacheRef*)ckalloc(sizeof(NamespCacheRef));
            cacheRef->element = (ClientData)cmdPtr;
            cacheRef->usage   = 0;
            cmdPtr->cacheInfo = cacheRef;
        }
        cache = Tcl_CreateHashEntry(&iPtr->activeNs->commandTable,
            path, &newEntry);

        Tcl_SetHashValue(cache, (ClientData)cmdPtr->cacheInfo);
        cmdPtr->cacheInfo->usage++;

        *rcmd = (Tcl_Command)cmdPtr;
    }
    else {
        *rcmd = NULL;
    }
    return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_FindVariable()
 *
 *  Searches for a variable along the specified path, which may contain
 *  a series of "::" scope qualifiers.  If the path starts with "::",
 *  then it is interpreted absolutely from the global scope.  Otherwise,
 *  it is taken relative to the currently active namespace.  If "flags"
 *  contains ITCL_FIND_LOCAL_ONLY, then the namespace path is followed
 *  and the name is sought in that context only.  Otherwise, all
 *  namespaces on the "import" list are searched as well.  If "flags"
 *  contains ITCL_FIND_NO_PRIVACY, then the variable will be found
 *  whether or not it has any protection.  Otherwise, the variable will
 *  be found only if access is allowed.
 *
 *  Returns TCL_ERROR, along with an error message in the specified
 *  interpreter, if an error is encountered.  Otherwise, this procedure
 *  returns TCL_OK.  If variable lookup was successful, rvar returns a
 *  variable token; otherwise, rvar will be NULL.
 * ------------------------------------------------------------------------
 */
int
Itcl_FindVariable(interp,path,flags,rvar)
    Tcl_Interp* interp;        /* interpreter containing the variable */
    char* path;                /* string path identifying variable */
    int flags;                 /* flags controlling variable lookup */
    Tcl_Var* rvar;             /* returns: variable token */
{
    Interp *iPtr = (Interp*)interp;

    int newEntry, specificRef;
    char *name, *tail;
    Tcl_Var var;
    Var* varPtr;
    Namespace* nsPtr;
    NamespImportRef* nsref;
    Itcl_List imports;
    Itcl_ListElem *elem;
    Tcl_HashEntry* entry;
    Tcl_HashEntry* cache;
    NamespCacheRef *cacheRef;

    /*
     *  See if the variable is recognized in the variable cache for
     *  the active namespace.  If it is, and if the cache record
     *  is still valid, then return immediately.  Delete any cache
     *  records that have expired.
     */
    varPtr = NULL;
    entry  = Tcl_FindHashEntry(&iPtr->activeNs->variableTable, path);

    if (entry != NULL) {
        cacheRef = (NamespCacheRef*)Tcl_GetHashValue(entry);
        if (cacheRef->element != NULL) {
            *rvar  = (Tcl_Var)cacheRef->element;
            return TCL_OK;
        }
        if (--cacheRef->usage == 0) {
            ckfree((char*)cacheRef);
        }
        Tcl_DeleteHashEntry(entry);
    }

    /*
     *  If the variable is not found immediately, the invoke the
     *  variable "enforcer" procedure to enforce any special lookup
     *  rules for this namespace.
     */
    if (iPtr->activeNs->varEnforcer) {
        if ((*iPtr->activeNs->varEnforcer)(interp, path, &var,
            ITCL_GLOBAL_VAR) != TCL_OK) {
            return TCL_ERROR;
        }
        else if (var != NULL) {
            varPtr = (Var*)var;
        }
    }

    /*
     *  If not, then look up the name in the active namespace, or
     *  in the global scope for absolute path names.
     */
    if (varPtr == NULL) {
        if (ItclFollowNamespPath(interp, iPtr->activeNs, path, 0,
            &nsPtr,&name,&specificRef) != TCL_OK) {
            return TCL_ERROR;
        }
        if ((nsPtr != NULL) && (name != NULL)) {
            entry = Tcl_FindHashEntry(&nsPtr->variables, name);
        }
        if (entry != NULL) {
            varPtr = (Var*)Tcl_GetHashValue(entry);

            if ((flags & ITCL_FIND_NO_PRIVACY) == 0 &&
                !ItclCanAccessNamesp(nsPtr, iPtr->activeNs,
                    varPtr->protection)) {
                varPtr = NULL;  /* disallow access */
                entry  = NULL;
            }
        }

        /*
         *  If it is still not found, and if it was not a specific
         *  reference with namespace qualifiers, then check all
         *  namespaces on the "import" list.
         */
        if ((entry == NULL) && !specificRef && (nsPtr != NULL) &&
            ((flags & ITCL_FIND_LOCAL_ONLY) == 0)) {

            for (tail=path; *tail != '\0'; tail++)
                ;
            while ((tail > path) && ((*tail != ':') || (*(tail-1) != ':')))
                tail--;
            if (*tail == ':')
                tail++;

            Itcl_GetAllImportNamesp((Itcl_Namespace)nsPtr, &imports);

            elem = Itcl_FirstListElem(&imports);
            elem = Itcl_NextListElem(elem);  /* skip over active namesp */

            while (elem) {
                nsref = (NamespImportRef*)Itcl_GetListValue(elem);
                entry = Tcl_FindHashEntry(&nsref->namesp->variables, tail);
                if (entry != NULL) {
                    varPtr = (Var*)Tcl_GetHashValue(entry);

                    if ((flags & ITCL_FIND_NO_PRIVACY) != 0 ||
                        ItclCanAccessNamesp(nsref->namesp,
                            iPtr->activeNs, varPtr->protection)) {
                        nsPtr = nsref->namesp;
                        break;
                    }
                    else {
                        varPtr = NULL;  /* disallow access */
                        entry  = NULL;
                    }
                }
                elem = Itcl_NextListElem(elem);
            }
            Itcl_DeleteList(&imports);
        }
        if (entry != NULL) {
            varPtr = (Var*)Tcl_GetHashValue(entry);
        }
    }

    /*
     *  If the variable was found, then add it to the local cache
     *  to make lookup faster next time.
     */
    if (varPtr != NULL) {
        if (varPtr->cacheInfo == NULL) {
            cacheRef = (NamespCacheRef*)ckalloc(sizeof(NamespCacheRef));
            cacheRef->element = (ClientData)varPtr;
            cacheRef->usage   = 0;
            varPtr->cacheInfo = cacheRef;
        }
        cache = Tcl_CreateHashEntry(&iPtr->activeNs->variableTable,
            path, &newEntry);

        Tcl_SetHashValue(cache, (ClientData)varPtr->cacheInfo);
        varPtr->cacheInfo->usage++;

        *rvar = (Tcl_Var)varPtr;
    }
    else {
        *rvar = NULL;
    }
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ScopedValInit()
 *
 *  Initializes an Itcl_ScopedVal structure to an empty state.
 *  Should be invoked before the structure is used for any other
 *  operations.
 * ------------------------------------------------------------------------
 */
void
Itcl_ScopedValInit(sval)
    Itcl_ScopedVal *sval;       /* value to be initialized */
{
    sval->namesp = NULL;
    sval->value = NULL;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ScopedValSet()
 *
 *  Discards any existing information in an Itcl_ScopedVal structure,
 *  and sets a new value.  Saves both a string value and its namespace
 *  context.
 * ------------------------------------------------------------------------
 */
void
Itcl_ScopedValSet(sval,valStr,valNs)
    Itcl_ScopedVal *sval;       /* value to be set */
    char *valStr;               /* string part of value */
    Itcl_Namespace valNs;       /* namespace context of value */
{
    Itcl_ScopedValFree(sval);
    sval->namesp = valNs;
    Itcl_PreserveData((ClientData)valNs);

    sval->value = (char*)ckalloc((unsigned)(strlen(valStr)+1));
    strcpy(sval->value, valStr);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ScopedValFree()
 *
 *  Frees an Itcl_ScopedVal structure, releasing any claim to the
 *  information that it contains.  This should be done whenever a
 *  Itcl_ScopedVal structure is no longer needed, to properly discard
 *  its information.
 * ------------------------------------------------------------------------
 */
void
Itcl_ScopedValFree(sval)
    Itcl_ScopedVal *sval;       /* value to be freed */
{
    if (sval->namesp) {
        Itcl_ReleaseData((ClientData)sval->namesp);
        sval->namesp = NULL;
    }
    if (sval->value) {
        ckfree(sval->value);
        sval->value = NULL;
    }
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ScopedValEncode()
 *
 *  Returns a character string which encodes all of the information
 *  contained in an Itcl_ScopedVal.  A scoped value represents a
 *  value (like a command script or variable name) together with its
 *  namespace context.  Any widget or extension that wants to take
 *  full advantage of namespaces should rely on scoped values instead
 *  of plain old strings.
 *
 *  The scoped value information is encoded in a string like this:
 *
 *      @scope <namespace> <value>
 *
 *  so that it can be executed as a Tcl command, or picked apart
 *  as a simple list.
 *
 *  Returns a pointer to a character string representing the given
 *  value.  This string remains valid until the next call to this
 *  routine.
 * ------------------------------------------------------------------------
 */
char*
Itcl_ScopedValEncode(sval)
    Itcl_ScopedVal *sval;     /* value being encoded */
{
    static Tcl_DString *buffer = NULL;

    /*
     *  If a string buffer does not yet exist, build it now.
     */
    if (buffer == NULL) {
        buffer = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
        Tcl_DStringInit(buffer);
    }
    Tcl_DStringTrunc(buffer,0);

    /*
     *  If the scoped value is null, then just return the null string.
     *  Otherwise, build a representation for the scoped value.
     */
    if (*sval->value != '\0') {
        Tcl_DStringAppendElement(buffer, "@scope");
        Tcl_DStringAppendElement(buffer, Itcl_GetNamespPath(sval->namesp));
        Tcl_DStringAppendElement(buffer, sval->value);
    }
    return Tcl_DStringValue(buffer);
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ScopedValDecode()
 *
 *  Takes a character string produced by Itcl_ScopedValEncode()
 *  and decodes it into an internal Itcl_ScopedVal representation.
 *  A scoped value represents a value (like a command script or
 *  variable name) together with its namespace context.  Any widget
 *  or extension that wants to take full advantage of namespaces
 *  should rely on scoped values instead of plain old strings.
 *
 *  If successful, this routine frees any existing information in
 *  "sval", saves the decoded information in its place, and returns
 *  the status TCL_OK.  Otherwise, it returns TCL_ERROR along with
 *  an error message in the interpreter.
 * ------------------------------------------------------------------------
 */
int
Itcl_ScopedValDecode(interp,mesg,sval)
    Tcl_Interp *interp;         /* interpreter managing this value */
    char *mesg;                 /* string form of scoped value */
    Itcl_ScopedVal *sval;       /* returns: value decoded */
{
    int scopec;
    char **scopev;
    Itcl_Namespace ns;

    /*
     *  For the null string, return a trivial scoped value.
     */
    if (*mesg == '\0') {
        Itcl_ScopedValSet(sval, mesg, Itcl_GetActiveNamesp(interp));
        return TCL_OK;
    }

    /*
     *  Decode the string into its component parts.
     */
    if (Tcl_SplitList(interp, mesg, &scopec, &scopev) != TCL_OK) {
        return TCL_ERROR;
    }
    if (scopec != 3 || strcmp(scopev[0],"@scope") != 0) {
        Tcl_AppendResult(interp, "bad scope description \"", mesg,
            "\": should be \"@scope namespace value\"",
            (char*)NULL);
        ckfree((char*)scopev);
        return TCL_ERROR;
    }

    if (Itcl_FindNamesp(interp,scopev[1],0,&ns) != TCL_OK) {
        ckfree((char*)scopev);
        return TCL_ERROR;
    }
    if (ns == NULL) {
        Tcl_AppendResult(interp, "namespace \"",
            scopev[1], "\" not found in context \"",
            Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
            (char*)NULL);
        ckfree((char*)scopev);
        return TCL_ERROR;
    }

    /*
     *  Return the information in the Itcl_ScopedVal structure.
     */
    Itcl_ScopedValSet(sval,scopev[2],ns);

    ckfree((char*)scopev);
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_ScopedValDecodeList()
 *
 *  Similar to Itcl_ScopedValDecode(), but takes a series of scoped
 *  declarations separated by newlines:
 *
 *      @scope ::foo {puts "hello"}
 *      @scope ::bar {puts "world"}
 *      ...
 *
 *  Decodes each of these declarations into the Itcl_ScopedVal form
 *  and appends them into the given list.
 *
 *  If successful, this routine appends a series of Itcl_ScopedVal*
 *  elements into the given list and returns the status TCL_OK.
 *  Otherwise, it returns TCL_ERROR along with an error message
 *  in the interpreter.
 * ------------------------------------------------------------------------
 */
static Itcl_List *DecodeScopeList = NULL;  /* add scoped vals to this list */

int
Itcl_ScopedValDecodeList(interp,mesg,svlist)
    Tcl_Interp *interp;         /* interpreter managing this value */
    char *mesg;                 /* string form of scoped value */
    Itcl_List *svlist;          /* returns: list of values decoded */
{
    int status;
    Itcl_Namespace ns;
    Itcl_ActiveNamespace nsToken;
    Itcl_List tmplist;
    Itcl_ListElem *elem;
    Itcl_ScopedVal *sval;

    /*
     *  Find the namespace to handling the scope parsing.
     */
    if (Itcl_FindNamesp(interp,"::tcl::scope-parser",0,&ns) != TCL_OK) {
        return TCL_ERROR;
    }
    if (ns == NULL) {
        Tcl_AppendResult(interp, "internal error: ",
            "parser namespace \"::tcl::scope-parser\" not found",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Activate the parser namespace, and interpret the scope
     *  declarations in that namespace.
     */
    Itcl_InitList(&tmplist);
    DecodeScopeList = &tmplist;

    nsToken = Itcl_ActivateNamesp(interp, ns);
    if (nsToken == NULL) {
        status = TCL_ERROR;
    }
    else {
        status = Tcl_Eval(interp, mesg);
        Itcl_DeactivateNamesp(interp, nsToken);
    }

    /*
     *  If everything went well, transfer the results to the
     *  return list.  Otherwise, delete the partial results.
     */
    if (status == TCL_OK) {
        elem = Itcl_FirstListElem(&tmplist);
        while (elem) {
            Itcl_AppendList(svlist, Itcl_GetListValue(elem));
            elem = Itcl_NextListElem(elem);
        }
        Itcl_DeleteList(&tmplist);
    }
    else {
        char err[256];
        sprintf(err, "\n    (scope description \"%.50s\" line %d)",
            mesg, interp->errorLine);
        Tcl_AddErrorInfo(interp, err);

        elem = Itcl_FirstListElem(&tmplist);
        while (elem) {
            sval = (Itcl_ScopedVal*)Itcl_GetListValue(elem);
            Itcl_ScopedValFree(sval);
            ckfree((char*)sval);
            elem = Itcl_NextListElem(elem);
        }
        Itcl_DeleteList(&tmplist);
    }
    return status;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_DecodeScopeCmd()
 *
 *  Invoked by Tcl when a "@scope" declaration is being decoded in
 *  the "scope-parser" namespace.  This namespace is used by
 *  Itcl_ScopedValDecodeList() to decode a list of newline-separated
 *  scoped value declarations.  Each declaration is parsed by this
 *  command.  Handles the following syntax:
 *
 *    @scope <name> <value>
 *
 *  As a side-effect, this command creates a new Itcl_ScopedVal*
 *  structure to contain the <name> and <value> data, and adds this
 *  structure to an Itcl_List, assumed to be client data for the
 *  active namespace.
 *
 *  Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_DecodeScopeCmd(dummy, interp, argc, argv)
    ClientData dummy;        /* unused */
    Tcl_Interp *interp;      /* current interpreter */
    int argc;                /* number of arguments */
    char **argv;             /* argument strings */
{
    Itcl_Namespace ns;
    Itcl_ScopedVal *sval;

    if (argc != 3) {
        Tcl_AppendResult(interp, "bad scope description \"",
            argv[0], "\": should be \"@scope namespace value\"",
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
     *  Create a scoped value structure and add it to the return list.
     */
    sval = (Itcl_ScopedVal*)ckalloc(sizeof(Itcl_ScopedVal));
    Itcl_ScopedValInit(sval);
    Itcl_ScopedValSet(sval, argv[2], ns);
    Itcl_AppendList(DecodeScopeList, (ClientData)sval);

    return TCL_OK;
}

