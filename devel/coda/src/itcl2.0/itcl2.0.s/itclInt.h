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
 *  This file includes all declarations used internally by this
 *  package.
 *  
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itclInt.h,v 1.1.1.1 1996/08/21 19:29:43 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#ifndef ITCLINT_H
#define ITCLINT_H

#ifndef ITCL_H
#include "itcl.h"
#endif

#include "tclInt.h"


/*
 *  Common info for managing all known objects...
 */
typedef struct ItclObjectInfo {
    Tcl_Interp *interp;         /* interpreter that manages this info */
    Tcl_HashTable objects;      /* list of all known objects */
    Itcl_Stack cdefnStack;      /* stack of class definitions being parsed */
} ItclObjectInfo;

/*
 *  Representation for each class...
 */
typedef struct ItclClass {
    char *name;                   /* class name */
    Tcl_Interp *interp;           /* interpreter that manages this info */
    Itcl_Namespace namesp;        /* namespace representing class scope */
    Tcl_Command accessCmd;        /* access command for creating instances */

    /* ---- private stuff below this line ---- */

    struct ItclObjectInfo *info;  /* info about all known objects */
    Itcl_List bases;              /* list of base classes */
    Itcl_List derived;            /* list of all derived classes */
    char *initCmd;                /* initialization command for new objs */
    Tcl_HashTable variables;      /* var definitions for all data members */
    int instanceVars;             /* number of instance vars in virtualData */
    Tcl_HashTable virtualData;    /* most-specific vars for this class */
    Tcl_HashTable virtualMethods; /* most-specific methods for this class */
    int unique;                   /* unique number for #auto generation */
    int flags;                    /* maintains class status */
} ItclClass;

/*
 *  Representation for each object...
 */
typedef struct ItclObject {
    ItclClass *cdefn;            /* most-specific class */
    Tcl_Command accessCmd;       /* object access command */
    ClientData ClientData;
    /* ---- private stuff below this line ---- */

    int dataSize;                /* number of elements in data array */
    Var** data;                  /* all object-specific data members */
    Tcl_HashTable* constructed;  /* temp storage used during construction */
    Tcl_HashTable* destructed;   /* temp storage used during destruction */
} ItclObject;

/*
 *  COMMAND MEMBER IMPLEMENTATION
 */
typedef struct ItclCmdImplement {
    int flags;                  /* flags describing implementation */
    Arg *arglist;               /* list of arg names and initial values */
    char *body;                 /* body of Tcl statements or "@proc" */
    Tcl_CmdProc *proc;          /* C procedure implementation or NULL */

} ItclCmdImplement;

/*
 *  Flag bits for ItclCmdImplement:
 */
#define ITCL_IMPLEMENT_NONE  0x001   /* no implementation */
#define ITCL_IMPLEMENT_TCL   0x002   /* Tcl implementation for body */
#define ITCL_IMPLEMENT_C     0x004   /* C implementation for body */

/*
 *  COMMAND MEMBERS
 */
typedef struct ItclCmdMember {
    Tcl_Interp* interp;         /* interpreter containing this command */
    ItclClass* cdefn;           /* class containing this command */
    char* name;                 /* member name */
    char* fullname;             /* member name with "class::" qualifier */
    int protection;             /* protection level */
    int flags;                  /* flags describing member (see below) */

    ItclCmdImplement* implement;  /* implementation */
    Arg *arglist;               /* list of arg names and initial values */

} ItclCmdMember;

/*
 *  Flag bits for ItclCmdMember:
 */
#define ITCL_CONSTRUCTOR    0x001  /* non-zero => is a constructor */
#define ITCL_DESTRUCTOR     0x002  /* non-zero => is a destructor */
#define ITCL_COMMON         0x004  /* non-zero => is a "proc" */
#define ITCL_ARG_SPEC       0x008  /* non-zero => has an argument spec */

#define ITCL_OLD_STYLE      0x010  /* non-zero => old-style method */
                                   /* (process "config" argument) */

/*
 *  VARIABLE DEFINITION
 */
typedef struct ItclVarDefn {
    char* name;                 /* variable name */
    char* fullname;             /* variable name with "class::" qualifier */
    int flags;                  /* ITCL_COMMON => common variable */
                                /* ITCL_THIS_VAR => built-in "this" variable */
    char* init;                 /* initial value */
    ItclCmdImplement* config;   /* code invoked when value is configured */
    ItclClass* cdefn;           /* class containing this variable */
    int protection;             /* protect level for variable */
} ItclVarDefn;

#define ITCL_THIS_VAR  0x010  /* non-zero => built-in "this" variable */

/*
 *  VARIABLE LOOKUP ENTRY
 */
typedef struct ItclVarLookup {
    int usage;                /* number of uses for this record */
    ItclVarDefn* vdefn;       /* variable definition */
    int accessible;           /* non-zero => accessible from class with
                               * this lookup record in its virtualData */

    int vindex;               /* index into virtual table (instance data) */
    Tcl_Var var;              /* variable (common data) */
} ItclVarLookup;


/*
 *  Functions used within the package, but not considered "public"...
 */
EXTERN int Itcl_InfoClassesCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp* interp, int argc, char** argv));
EXTERN int Itcl_InfoObjectsCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp* interp, int argc, char** argv));

EXTERN int Itcl_DelClassCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp* interp, int argc, char** argv));
EXTERN int Itcl_DelObjectCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp* interp, int argc, char** argv));


/*
 *  Functions for manipulating classes...
 */
EXTERN int Itcl_CreateClass _ANSI_ARGS_((Tcl_Interp* interp, char* name,
    ItclObjectInfo *info, Itcl_Namespace* nsPtr));
EXTERN int Itcl_DeleteClass _ANSI_ARGS_((Tcl_Interp* interp,
    Itcl_Namespace ns));

EXTERN int Itcl_HandleClass _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_ClassVarEnforcer _ANSI_ARGS_((Tcl_Interp* interp,
    char* name, Tcl_Var* varPtr, int flags));
EXTERN int Itcl_ClassUnknownCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN void Itcl_BuildVirtualTables _ANSI_ARGS_((ItclClass* cdefnPtr));

EXTERN int Itcl_CreateVarDefn _ANSI_ARGS_((Tcl_Interp *interp,
    Itcl_Class* cdefn, char* name, char* init, char* config,
    ItclVarDefn** vdefn));
EXTERN void Itcl_DeleteVarDefn _ANSI_ARGS_((ItclVarDefn *vdefn));

EXTERN int Itcl_FindVarDefn _ANSI_ARGS_((Tcl_Interp *interp,
    Itcl_Class* cdefn, char* name, ItclVarDefn **vdefnPtr));
EXTERN int Itcl_FindCmdDefn _ANSI_ARGS_((Tcl_Interp *interp,
    Itcl_Class* cdefn, char* name, ItclCmdMember **mdefnPtr));


/*
 *  Functions for manipulating objects...
 */
EXTERN int Itcl_CreateObject _ANSI_ARGS_((Tcl_Interp *interp,
    char* name, Itcl_Class *cdefn, int argc, char** argv,
    Itcl_Object **objPtr));

EXTERN int Itcl_DeleteObject _ANSI_ARGS_((Tcl_Interp *interp,
    Itcl_Object* obj));
EXTERN int Itcl_DestructObject _ANSI_ARGS_((Tcl_Interp *interp,
    Itcl_Object* obj));

EXTERN int Itcl_HandleInstance _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));

EXTERN char* Itcl_GetInstanceVar _ANSI_ARGS_((Tcl_Interp *interp,
    char* name, Itcl_Object* obj, Itcl_Class* scope));


/*
 *  Functions for manipulating methods and procs...
 */
EXTERN int Itcl_BodyCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_ConfigBodyCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));

EXTERN int Itcl_CreateMethod _ANSI_ARGS_((Tcl_Interp* interp,
    Itcl_Class *cdefn, char* name, char* arglist, char* body));

EXTERN int Itcl_CreateProc _ANSI_ARGS_((Tcl_Interp* interp,
    Itcl_Class *cdefn, char* name, char* arglist, char* body));

EXTERN int Itcl_CreateCmdMember _ANSI_ARGS_((Tcl_Interp* interp,
    Itcl_Class *cdefn, char* name, char* arglist, char* body,
    ItclCmdMember** mdefnPtr));
EXTERN int Itcl_ChangeCmdMember _ANSI_ARGS_((Tcl_Interp* interp,
    ItclCmdMember* mdefn, char* arglist, char* body));

EXTERN void Itcl_DeleteCmdMember _ANSI_ARGS_((ClientData cdata));

EXTERN int Itcl_CreateCmdImplement _ANSI_ARGS_((Tcl_Interp* interp,
    char* arglist, char* body, ItclCmdImplement** mimplPtr));
EXTERN void Itcl_DeleteCmdImplement _ANSI_ARGS_((ClientData cdata));
EXTERN int Itcl_GetCmdImplement _ANSI_ARGS_((Tcl_Interp* interp,
    ItclCmdMember* mdefn, ItclCmdImplement** mimplPtr));
EXTERN int Itcl_EvalCmdImplement _ANSI_ARGS_((Tcl_Interp* interp,
    ItclCmdMember* mdefn, ItclCmdImplement* mimpl, int argc, char** argv));

EXTERN int Itcl_CreateArgList _ANSI_ARGS_((Tcl_Interp* interp,
    char* decl, Arg** argPtr));
EXTERN Arg* Itcl_CreateArg _ANSI_ARGS_((char* name, char* init));
EXTERN void Itcl_DeleteArgList _ANSI_ARGS_((Arg* arg));
EXTERN char* Itcl_ArgList _ANSI_ARGS_((Arg* arg));
EXTERN int Itcl_EquivArgLists _ANSI_ARGS_((Arg* arg1, Arg* arg2));
EXTERN char* Itcl_GetCmdMemberUsage _ANSI_ARGS_((ItclCmdMember* mdefn,
    Itcl_Object* obj));

EXTERN int Itcl_ExecMethod _ANSI_ARGS_((ClientData cdata, Tcl_Interp *interp,
    int argc, char **argv));
EXTERN int Itcl_ExecProc _ANSI_ARGS_((ClientData cdata, Tcl_Interp *interp,
    int argc, char **argv));

EXTERN int Itcl_ConstructBase _ANSI_ARGS_((Tcl_Interp *interp,
    struct Itcl_Object* odefn, struct Itcl_Class* cdefn));
EXTERN int Itcl_ParseArgs _ANSI_ARGS_((Tcl_Interp* interp, int argc,
    char** argv, ItclCmdMember *mdefn, ItclCmdImplement* mimpl, int flags));


/*
 *  Commands for parsing class definitions...
 */
EXTERN int Itcl_ParseInit _ANSI_ARGS_((Tcl_Interp* interp,
    ItclObjectInfo* info));
EXTERN int Itcl_ClassCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));

EXTERN int Itcl_ClassInheritCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_ClassConstructorCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_ClassDestructorCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_ClassMethodCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_ClassProcCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_ClassVariableCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_ClassCommonCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_ClassProtectionCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));

EXTERN int Itcl_ParseVarEnforcer _ANSI_ARGS_((Tcl_Interp* interp,
    char* name1, Tcl_Var* varPtr, int flags));


/*
 *  Commands in the "builtin" namespace...
 */
EXTERN int Itcl_BiInit _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN int Itcl_InstallBiMethods _ANSI_ARGS_((Tcl_Interp *interp,
    Itcl_Class *cdefn));

EXTERN int Itcl_BiIsaCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_BiConfigureCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_BiCgetCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));

EXTERN int Itcl_BiInfoClassCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_BiInfoInheritCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_BiInfoHeritageCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_BiInfoFunctionCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_BiInfoVariableCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_BiInfoArgsCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));
EXTERN int Itcl_BiInfoBodyCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char **argv));

EXTERN int Itcl_DefaultInfoCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp *interp, int argc, char** argv));


/*
 *  Utility functions...
 */
EXTERN void Itcl_InstallCallFrame _ANSI_ARGS_((Tcl_Interp *interp,
    CallFrame* frame, int argc, char** argv));
EXTERN void Itcl_RemoveCallFrame _ANSI_ARGS_((Tcl_Interp *interp));


/*
 *  Commands provided for backward compatibility...
 */
EXTERN int Itcl_OldInit _ANSI_ARGS_((Tcl_Interp* interp,
    ItclObjectInfo* info));
EXTERN int Itcl_InstallOldBiMethods _ANSI_ARGS_((Tcl_Interp *interp,
    Itcl_Class *cdefn));

#endif
