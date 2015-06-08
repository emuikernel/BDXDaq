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
 *  This part adds a mechanism for integrating C procedures into
 *  [incr Tcl] classes as methods and procs.  Each C procedure must
 *  either be declared via Itcl_RegisterC() or dynamically loaded.
 *
 * ========================================================================
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: itcl_linkage.c,v 1.1.1.1 1996/08/21 19:29:43 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include "itclInt.h"

static char rcsid[] = "$Id: itcl_linkage.c,v 1.1.1.1 1996/08/21 19:29:43 heyes Exp $";


/* lookup table for all registered C routines */
static Tcl_HashTable* RegisteredProcs = NULL;


/*
 * ------------------------------------------------------------------------
 *  Itcl_RegisterC()
 *
 *  Used to associate a symbolic name with the C procedure that handles
 *  a Tcl command.  Procedures that are registered in this manner can
 *  be referenced in the body of an [incr Tcl] class definition to
 *  specify C procedures to acting as methods/procs.  Usually invoked
 *  in an initialization routine for an extension, called out in
 *  Tcl_AppInit() at the start of an application.  A symbolic procedure
 *  name can only be used once.  If the same name is used twice, this
 *  procedure returns an error.
 *
 *  Returns TCL_OK on success, or TCL_ERROR (along with an error message
 *  in interp->result) if anything goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_RegisterC(interp,name,proc)
    Tcl_Interp *interp;  /* interpreter handling this registration */
    char *name;          /* symbolic name for procedure */
    Tcl_CmdProc *proc;   /* procedure handling Tcl command */
{
    int newEntry;
    Tcl_HashEntry *entry;
	/*
printf("Itcl_RegisterC: name >%s<\n",name);fflush(stdout);
	*/
    /*
     *  If the registration table does not yet exist, then create it.
     */
    if (!RegisteredProcs) {
        RegisteredProcs = (Tcl_HashTable*)ckalloc(sizeof(Tcl_HashTable));
        Tcl_InitHashTable(RegisteredProcs, TCL_STRING_KEYS);
    }

    /*
     *  Make sure that a proc was specified.
     */
    if (!proc) {
        Tcl_AppendResult(interp, "initialization error: null pointer for ",
            "C procedure \"", name, "\"",
            (char*)NULL);
        return TCL_ERROR;
    }

    /*
     *  Add a new entry for the given procedure.  If an entry with
     *  this name already exists, return an error.
     */
    entry = Tcl_CreateHashEntry(RegisteredProcs, name, &newEntry);
    if (!newEntry) {
        Tcl_AppendResult(interp, "initialization error: C procedure ",
            "with name \"", name, "\" already exists",
            (char*)NULL);
        return TCL_ERROR;
    }

    Tcl_SetHashValue(entry, (ClientData)proc);
    return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 *  Itcl_FindC()
 *
 *  Used to query a C procedure via its symbolic name.  Looks at the list
 *  of procedures registered previously by Itcl_RegisterC() and returns
 *  a pointer to the appropriate entry.  Returns NULL if a procedure is
 *  not found.
 * ------------------------------------------------------------------------
 */
Tcl_CmdProc*
Itcl_FindC(name)
    char *name;          /* symbolic name for procedure */
{
    Tcl_HashEntry *entry;

    if (RegisteredProcs) {
        entry = Tcl_FindHashEntry(RegisteredProcs, name);
        if (entry) {
            return (Tcl_CmdProc*)Tcl_GetHashValue(entry);
        }
    }
    return NULL;
}
