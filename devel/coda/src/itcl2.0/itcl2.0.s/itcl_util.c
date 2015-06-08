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
 *  This segment provides common utility functions used throughout
 *  the other [incr Tcl] source files.
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl_util.c,v 1.1.1.1 1996/08/21 19:29:44 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include <assert.h>
#include "itclInt.h"

static char rcsid[] = "$Id: itcl_util.c,v 1.1.1.1 1996/08/21 19:29:44 heyes Exp $";


/*
 * ------------------------------------------------------------------------
 *  Itcl_GetClassContext()
 *
 *  Convenience routine for looking up the current class context.
 *  Useful in implementing methods/procs to see what class, and perhaps
 *  what object, is active.
 *
 *  Returns TCL_OK if the current namespace is a class namespace.
 *  Also returns pointers to the class namespace, its associated
 *  class data, and any object-specific data that is active.  Returns
 *  TCL_ERROR (along with an error message in the interpreter) if a
 *  class namespace is not active.
 * ------------------------------------------------------------------------
 */
int
Itcl_GetClassContext(interp, classNsPtr, cdefnPtr, odefnPtr)
    Tcl_Interp *interp;           /* current interpreter */
    Itcl_Namespace *classNsPtr;   /* returns:  class namespace or NULL */
    Itcl_Class **cdefnPtr;        /* returns:  class definition or NULL */
    Itcl_Object **odefnPtr;       /* returns:  object data or NULL */
{
    Itcl_Namespace activeNs = Itcl_GetActiveNamesp(interp);

    /*
     *  Return NULL for anything that cannot be found.
     */
    *classNsPtr = NULL;
    *cdefnPtr   = NULL;
    *odefnPtr   = NULL;

    /*
     *  If the active namespace is a class namespace, then return
     *  all known info.
     */
    if (Itcl_IsClass(activeNs)) {
        *classNsPtr = activeNs;
        *cdefnPtr   = (Itcl_Class*)Itcl_GetNamespData(activeNs);
        *odefnPtr   = (Itcl_Object*)Itcl_GetActiveNamespData(interp);
        return TCL_OK;
    }

    /*
     *  Otherwise, return an error message.
     */
    Tcl_AppendResult(interp, "namespace \"", Itcl_GetNamespPath(activeNs),
        "\" is not a class namespace",
        (char*)NULL);

    return TCL_ERROR;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_InstallCallFrame()
 *
 *  Used to install a new call frame for local variables prior to a
 *  Tcl method/proc invocation.  This same operation is usually buried
 *  in the Tcl InterpProc() procedure.  It is defined here so that
 *  it can be reused more easily.
 * ------------------------------------------------------------------------
 */
void
Itcl_InstallCallFrame(interp, frame, argc, argv)
    Tcl_Interp* interp;     /* interpreter being modified */
    CallFrame* frame;       /* call frame to be initialized and installed */
    int argc;               /* number of command-line arguments */
    char **argv;            /* strings for command-line arguments */
{
    Interp* iPtr = (Interp*)interp;

    Tcl_InitHashTable(&frame->varTable, TCL_STRING_KEYS);
    if (iPtr->varFramePtr != NULL) {
        frame->level = iPtr->varFramePtr->level + 1;
    } else {
        frame->level = 1;
    }
    frame->argc = argc;
    frame->argv = argv;
    frame->callerPtr = iPtr->framePtr;
    frame->callerVarPtr = iPtr->varFramePtr;
    frame->activeNs = iPtr->activeNs;
    frame->activeData = iPtr->activeData;
    frame->flags = 0;

    iPtr->framePtr = frame;
    iPtr->varFramePtr = frame;
    iPtr->returnCode = TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 *  Itcl_RemoveCallFrame()
 *
 *  Used to remove the topmost call frame from the specified interpreter.
 *  Usually invoked at the end of a Tcl method/proc to discard the
 *  temporary variables and tear down the call frame.  This same operation
 *  is usually buried in the Tcl InterpProc() procedure.  It is defined
 *  here so that it can be reused more easily.
 * ------------------------------------------------------------------------
 */
void
Itcl_RemoveCallFrame(interp)
    Tcl_Interp* interp;     /* interpreter being modified */
{
    Interp* iPtr = (Interp*)interp;
    CallFrame* frame = iPtr->framePtr;

    /*
     * It is important to remove the call frame from the interpreter
     * before deleting it, so that traces invoked during the deletion
     * don't see the partially-deleted frame.
     */
    iPtr->framePtr = frame->callerPtr;
    iPtr->varFramePtr = frame->callerVarPtr;
    TclDeleteVars(iPtr, &frame->varTable);
}
