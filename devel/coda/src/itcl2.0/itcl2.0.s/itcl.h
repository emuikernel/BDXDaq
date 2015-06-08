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
 *  ADDING [incr Tcl] TO A Tcl-BASED APPLICATION:
 *
 *    To add [incr Tcl] facilities to a Tcl application, modify the
 *    Tcl_AppInit() routine as follows:
 *
 *    1) Include this header file near the top of the file containing
 *       Tcl_AppInit():
 *
 *         #include "itcl.h"
 *
 *    2) Within the body of Tcl_AppInit(), add the following lines:
 *
 *         if (Itcl_Init(interp) == TCL_ERROR) {
 *             return TCL_ERROR;
 *         }
 * 
 *    3) Link your application with libitcl.a
 *
 *    NOTE:  An example file "tclAppInit.c" containing the changes shown
 *           above is included in this distribution.
 *  
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl.h,v 1.1.1.1 1996/08/21 19:29:43 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#ifndef ITCL_H
#define ITCL_H

#include "tcl.h"

#define ITCL_VERSION "2.0"
#define ITCL_MAJOR_VERSION 2
#define ITCL_MINOR_VERSION 0

/*
 *  Data structures defined opaquely in this module.
 *
 *  NOTE:  Any change to the structures below must be mirrored
 *         in their "real" definition in itclInt.h.
 */
typedef struct Itcl_Class {
    char *name;                   /* class name */
    Tcl_Interp *interp;           /* interpreter that manages this info */
    Itcl_Namespace namesp;        /* namespace representing class scope */
    Tcl_Command accessCmd;        /* access command for creating instances */
} Itcl_Class;

typedef struct Itcl_Object {
    Itcl_Class *cdefn;            /* most-specific class */
    Tcl_Command accessCmd;        /* object access command */
    ClientData ClientData;        /* private storage for object 
				   passed to methods in C implementation.*/
} Itcl_Object;


typedef struct Itcl_HierIter {
    Itcl_Class *current;          /* current position in hierarchy */
    Itcl_Stack stack;             /* stack used for traversal */
} Itcl_HierIter;


/*
 *  Exported functions (public interface to this package)
 */
EXTERN int Itcl_Init _ANSI_ARGS_((Tcl_Interp* interp));

EXTERN int Itcl_RegisterC _ANSI_ARGS_((Tcl_Interp* interp,
    char *name, Tcl_CmdProc *proc));
EXTERN Tcl_CmdProc* Itcl_FindC _ANSI_ARGS_((char *name));

EXTERN int Itcl_IsClass _ANSI_ARGS_((Itcl_Namespace ns));
EXTERN int Itcl_FindClass _ANSI_ARGS_((Tcl_Interp* interp, char* path,
    Itcl_Namespace* nsPtr));

EXTERN int Itcl_FindObject _ANSI_ARGS_((Tcl_Interp* interp,
    char* name, Itcl_Object** objPtr));
EXTERN int Itcl_IsObject _ANSI_ARGS_((Tcl_Command cmd));
EXTERN int Itcl_ObjectIsa _ANSI_ARGS_((Itcl_Object* obj,
    Itcl_Class* cdefn));

EXTERN int Itcl_GetClassContext _ANSI_ARGS_((Tcl_Interp* interp,
    Itcl_Namespace* classNsPtr, Itcl_Class** cdefnPtr,
    Itcl_Object** odefnPtr));

EXTERN void Itcl_InitHierIter _ANSI_ARGS_((Itcl_HierIter *iter,
    Itcl_Class *cdefn));
EXTERN void Itcl_DeleteHierIter _ANSI_ARGS_((Itcl_HierIter *iter));
EXTERN Itcl_Class* Itcl_AdvanceHierIter _ANSI_ARGS_((Itcl_HierIter *iter));
EXTERN int Itcl_AtScopeCmd2 _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp* interp, int argc, char** argv));
#endif
