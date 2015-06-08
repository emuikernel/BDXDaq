/*
 * ------------------------------------------------------------------------
 * PACKAGE:  [incr Tcl] DESCRIPTION:  Object-Oriented Extensions to Tcl
 * 
 * [incr Tcl] provides object-oriented extensions to Tcl, much as C++ provides
 * object-oriented extensions to C.  It provides a means of encapsulating
 * related procedures together with their shared data in a local namespace
 * that is hidden from the outside world.  It promotes code re-use through
 * inheritance.  More than anything else, it encourages better organization
 * of Tcl applications through the object-oriented paradigm, leading to code
 * that is easier to understand and maintain.
 * 
 * This segment handles "objects" which are instantiated from class definitions.
 * Objects contain public/protected/private data members from all classes in
 * a derivation hierarchy.
 * 
 * ========================================================================
 * AUTHOR:  Michael J. McLennan       Phone: (610)712-2842 AT&T Bell
 * Laboratories   E-mail: michael.mclennan@att.com RCS:  $Id:
 * itcl_objects.c,v 1.1.1.1 1996/08/21 19:29:45 heyes Exp $
 * ========================================================================
 * Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include <assert.h>
#include "itclInt.h"

#ifdef VXWORKS
#include <loadLib.h>
#include <sysSymTbl.h>
#endif

static char     rcsid[] = "$Id";

/*
 * FORWARD DECLARATIONS
 */
static void ItclReportObjectUsage 
_ANSI_ARGS_((Tcl_Interp * interp,
	     Itcl_Object * obj));

	static char    *ItclTraceThisVar _ANSI_ARGS_((ClientData cdata,
                 Tcl_Interp * interp, char *name1, char *name2, int flags));

	static void ItclDestroyObject _ANSI_ARGS_((ClientData cdata));
	static void ItclFreeObject _ANSI_ARGS_((ClientData cdata));

	static int ItclDestructBase _ANSI_ARGS_((Tcl_Interp * interp,
		                    Itcl_Object * obj, Itcl_Class * cdefn));

	static void ItclCreateObjVar _ANSI_ARGS_((Tcl_Interp * interp,
		                   ItclVarDefn * vdefn, Itcl_Object * obj));


/*
 * ------------------------------------------------------------------------
 * Itcl_CreateObject()
 * 
 * Creates a new object instance belonging to the given class. Supports complex
 * object names like "namesp::namesp::name" by following the namespace path
 * and creating the object in the desired namespace.
 * 
 * Automatically creates and initializes data members, including the built-in
 * protected "this" variable containing the object name. Installs an access
 * command in the current namespace, and invokes the constructor to
 * initialize the object.
 * 
 * If any errors are encountered, the object is destroyed and this procedure
 * returns TCL_ERROR (along with an error message in the interpreter).
 * Otherwise, it returns TCL_OK, along with a pointer to the new object data
 * in roPtr.
 * ------------------------------------------------------------------------
 */
	int
	                Itcl_CreateObject(interp, name, cdefn, argc, argv, roPtr)
	Tcl_Interp     *interp;	/* interpreter mananging new object */
	char           *name;	/* name of new object */
	Itcl_Class     *cdefn;	/* class for new object */
	int             argc;	/* number of args in object creation cmd */
	char          **argv;	/* list of args in object creation cmd */
	Itcl_Object   **roPtr;	/* returns: pointer to object data */
{
	Itcl_ActiveNamespace context = NULL;
	ItclClass      *cdefnPtr = (ItclClass *) cdefn;

	int             status;
	char           *className, *objName, *argv1, *cmdStr;
	Itcl_Namespace  ns;
	Itcl_ActiveNamespace nsToken;
	Tcl_Command     cmd;
	ItclObject     *objPtr;
	ItclClass      *cdPtr;
	ItclVarDefn    *vdefn;
	ItclCmdMember  *mdefn;
	Itcl_HierIter   hier;
	Tcl_HashEntry  *entry;
	Tcl_HashSearch  place;
	int             newEntry;
	CallFrame       frame;
	Itcl_InterpState istate;

	/*
	 * Parse the object name according to "namesp::namesp::name". If it
	 * has any scope qualifiers, activate the parent namespace before
	 * creating the object.
	 */
	Itcl_ParseNamespPath(name, &className, &objName);
	if (className) {
		if (Itcl_FindNamesp(interp, className, 0, &ns) != TCL_OK) {
			goto objCreateFail;
		}
		if (ns == NULL) {
			ns = Itcl_GetActiveNamesp(interp);
			Tcl_AppendResult(interp, "namespace \"",
				    className, "\" not found in context \"",
					 Itcl_GetNamespPath(ns), "\"",
					 (char *) NULL);
			goto objCreateFail;
		}
		context = Itcl_ActivateNamesp(interp, ns);
		if (context == NULL) {
			goto objCreateFail;
		}
	}
	if (Itcl_FindCommand(interp, objName, ITCL_FIND_LOCAL_ONLY, &cmd) != TCL_OK) {
		goto objCreateFail;
	}
	if (cmd != NULL) {
		Tcl_AppendResult(interp, "command \"", objName,
				 "\" already exists in namespace \"",
		     Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
				 (char *) NULL);
		goto objCreateFail;
	}
	/*
	 * Create a new object and initialize it.
	 */
	objPtr = (ItclObject *) ckalloc(sizeof(ItclObject));
	bzero((char *) objPtr, sizeof(ItclObject));
	objPtr->cdefn = cdefnPtr;
	Itcl_PreserveData((ClientData) cdefnPtr);

	objPtr->dataSize = cdefnPtr->instanceVars;
	objPtr->data = (Var **) ckalloc((unsigned) (objPtr->dataSize * sizeof(Var *)));

	objPtr->constructed = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
	Tcl_InitHashTable(objPtr->constructed, TCL_STRING_KEYS);
	objPtr->destructed = NULL;

	/*
	 * Add a command to the current namespace with the object name. This
	 * is done before invoking the constructors so that the command can
	 * be used during construction to query info.
	 */
	objPtr->accessCmd = Tcl_CreateCommand(interp, objName,
					      Itcl_HandleInstance,
		 Itcl_PreserveData((ClientData) objPtr), ItclDestroyObject);

	Itcl_PreserveData((ClientData) objPtr);	/* while we're using this... */

	Itcl_EventuallyFree((ClientData) objPtr, ItclFreeObject);

	/*
	 * Install the class namespace and object context so that the
	 * object's data members can be initialized via simple "set"
	 * commands.
	 * 
	 * NOTE:  A local call frame MUST be installed to gain proper access to
	 * object-specific data members.  Object-specific variables cannot be
	 * cached at the class-namespace level, and therefore must be
	 * accessed via a local call frame.
	 * 
	 * Scan through all object-specific data members and create the actual
	 * variables that maintain the object state.
	 */
	nsToken = Itcl_ActivateNamesp2(interp, cdefn->namesp,
				       (ClientData) objPtr);

	if (nsToken == NULL) {
		goto objCreateFail;
	}
	Itcl_InstallCallFrame(interp, &frame, 0, (char **) NULL);

	Itcl_InitHierIter(&hier, cdefn);

	cdPtr = (ItclClass *) Itcl_AdvanceHierIter(&hier);
	while (cdPtr != NULL) {
		entry = Tcl_FirstHashEntry(&cdPtr->variables, &place);
		while (entry) {
			vdefn = (ItclVarDefn *) Tcl_GetHashValue(entry);
			if ((vdefn->flags & ITCL_THIS_VAR) != 0) {
				if (cdPtr == cdefnPtr) {
					ItclCreateObjVar(interp, vdefn, (Itcl_Object *) objPtr);
					Tcl_SetVar2(interp, "this", (char *) NULL, "", 0);
					Tcl_TraceVar2(interp, "this", (char *) NULL,
						      TCL_TRACE_READS | TCL_TRACE_WRITES, ItclTraceThisVar,
						      (ClientData) objPtr);
				}
			} else if ((vdefn->flags & ITCL_COMMON) == 0) {
				ItclCreateObjVar(interp, vdefn, (Itcl_Object *) objPtr);
			}
			entry = Tcl_NextHashEntry(&place);
		}
		cdPtr = (ItclClass *) Itcl_AdvanceHierIter(&hier);
	}
	Itcl_DeleteHierIter(&hier);

	Itcl_RemoveCallFrame(interp);
	Itcl_DeactivateNamesp(interp, nsToken);

	/*
	 * Now construct the object by invoking the most-specific
	 * constructor.  If we use the generic name "constructor", the
	 * virtual behavior of methods will make sure that we get the
	 * most-specific one.
	 * 
	 * This will cause a chain reaction, making sure that all base classes
	 * constructors are invoked as well, in order from least- to
	 * most-specific.  Any constructors that are not called out
	 * explicitly in "initCmd" code fragments are invoked implicitly
	 * without arguments.
	 */
	status = Itcl_FindCmdDefn(interp, cdefn, "constructor", &mdefn);

	if (status == TCL_OK && mdefn != NULL) {
		nsToken = Itcl_ActivateNamesp2(interp, cdefn->namesp,
					       (ClientData) objPtr);

		argv1 = argv[1];
		argv[1] = "constructor";
		cmdStr = Tcl_Merge(argc - 1, argv + 1);

		status = Itcl_EvalArgs(interp, cmdStr, (char *) NULL, argc - 1, argv + 1);

		ckfree(cmdStr);
		argv[1] = argv1;

		Itcl_DeactivateNamesp(interp, nsToken);
	}
	/*
	 * If construction failed, then delete the object access command.
	 * This will destruct the object and delete the object data.  Be
	 * careful to save and restore the interpreter state, since the
	 * destructors may generate errors of their own.
	 */
	if (status != TCL_OK) {
		Tcl_DString     buffer;

		objName = Tcl_GetCommandName(objPtr->cdefn->interp, objPtr->accessCmd);
		Tcl_DStringInit(&buffer);
		Tcl_DStringAppend(&buffer, "\n    (while constructing object \"", -1);
		Tcl_DStringAppend(&buffer, objName, -1);
		Tcl_DStringAppend(&buffer, "\")", -1);
		Tcl_AddErrorInfo(interp, Tcl_DStringValue(&buffer));
		Tcl_DStringFree(&buffer);
		printf("constructor failed : %s\n", interp->result);
		istate = Itcl_SaveInterpState(interp, status);

		Tcl_DeleteCommand2(interp, objPtr->accessCmd);

		status = Itcl_RestoreInterpState(interp, istate);
	}
	/*
	 * At this point, the object is fully constructed. Destroy the
	 * "constructed" table in the object data, since it is no longer
	 * needed.
	 */
	Tcl_DeleteHashTable(objPtr->constructed);
	ckfree((char *) objPtr->constructed);
	objPtr->constructed = NULL;

	/*
	 * Add it to the list of all known objects.
	 */
	if (status == TCL_OK) {
		entry = Tcl_CreateHashEntry(&cdefnPtr->info->objects,
				     (char *) objPtr->accessCmd, &newEntry);

		Tcl_SetHashValue(entry, (ClientData) objPtr);
	}
	if (context != NULL) {
		Itcl_DeactivateNamesp(interp, context);
	}
	Itcl_ReleaseData((ClientData) objPtr);

	*roPtr = (Itcl_Object *) objPtr;
	return status;

objCreateFail:
	if (context != NULL) {
		Itcl_DeactivateNamesp(interp, context);
	}
	return TCL_ERROR;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_DeleteObject()
 * 
 * Attempts to delete an object by invoking its destructor.
 * 
 * If the destructor is successful, then the object is deleted by removing its
 * access command, and this procedure returns TCL_OK. Otherwise, the object
 * will remain alive, and this procedure returns TCL_ERROR (along with an
 * error message in the interpreter).
 * ------------------------------------------------------------------------
 */
int
Itcl_DeleteObject(interp, obj)
	Tcl_Interp     *interp;	/* interpreter mananging object */
	Itcl_Object    *obj;	/* object to be deleted */
{
	ItclClass      *cdefnPtr = (ItclClass *) obj->cdefn;

	Tcl_HashEntry  *entry;
	Command        *cmdPtr;

	Itcl_PreserveData((ClientData) obj);

	/*
	 * Invoke the object's destructors.
	 */
	if (Itcl_DestructObject(interp, obj) != TCL_OK) {
		return TCL_ERROR;
	}
	/*
	 * Remove the object from the global list.
	 */
	entry = Tcl_FindHashEntry(&cdefnPtr->info->objects,
				  (char *) obj->accessCmd);

	if (entry) {
		Tcl_DeleteHashEntry(entry);
	}
	/*
	 * Change the object's access command so that it can be safely
	 * deleted without attempting to destruct the object again.  Then
	 * delete the access command.  If this is the last use of the object
	 * data, the object will die here.
	 */
	cmdPtr = (Command *) obj->accessCmd;
	cmdPtr->deleteProc = Itcl_ReleaseData;

	Tcl_DeleteCommand2(interp, obj->accessCmd);

	Itcl_ReleaseData((ClientData) obj);	/* object should die here */

	return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_DestructObject()
 * 
 * Invokes the destructor for a particular object.  Usually invoked by
 * Itcl_DeleteObject() or Itcl_DestroyObject() as a part of the object
 * destruction process.
 * 
 * Returns TCL_OK on success, or TCL_ERROR (along with an error message in the
 * interpreter) if anything goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_DestructObject(interp, obj)
	Tcl_Interp     *interp;	/* interpreter mananging new object */
	Itcl_Object    *obj;	/* object to be destructed */
{
	ItclObject     *objPtr = (ItclObject *) obj;

	int             status;
	Tcl_DString     buffer;
	Itcl_ActiveNamespace nsToken;

	/*
	 * Create a "destructed" table to keep track of which destructors
	 * have been invoked.  This is used in ItclDestructBase to make sure
	 * that all base class destructors have been called, explicitly or
	 * implicitly.
	 */
	objPtr->destructed = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
	Tcl_InitHashTable(objPtr->destructed, TCL_STRING_KEYS);

	/*
	 * Destruct the object starting from the most-specific class.
	 * Activate the object context so that Itcl_ExecMethod is happy.
	 */
	nsToken = Itcl_ActivateNamesp2(interp, obj->cdefn->namesp,
				       (ClientData) obj);

	if (nsToken) {
		status = ItclDestructBase(interp, obj, obj->cdefn);
		Itcl_DeactivateNamesp(interp, nsToken);
	} else {
		status = TCL_ERROR;
	}

	if (status == TCL_OK) {
		Tcl_ResetResult(interp);
	} else if (obj->accessCmd != NULL) {
		char           *objName;
		objName = Tcl_GetCommandName(obj->cdefn->interp, obj->accessCmd);
		Tcl_DStringInit(&buffer);
		Tcl_DStringAppend(&buffer, "\n    (while destructing object \"", -1);
		Tcl_DStringAppend(&buffer, objName, -1);
		Tcl_DStringAppend(&buffer, "\")", -1);
		Tcl_AddErrorInfo(interp, Tcl_DStringValue(&buffer));
		Tcl_DStringFree(&buffer);
	}
	Tcl_DeleteHashTable(objPtr->destructed);
	ckfree((char *) objPtr->destructed);
	objPtr->destructed = NULL;

	return status;
}

/*
 * ------------------------------------------------------------------------
 * ItclDestructBase()
 * 
 * Invoked by Itcl_DestructObject() to recursively destruct an object from the
 * specified class level.  Finds and invokes the destructor for the specified
 * class, and then recursively destructs all base classes.
 * 
 * Returns TCL_OK on success, or TCL_ERROR (along with an error message in
 * interp->result) on error.
 * ------------------------------------------------------------------------
 */
static int
ItclDestructBase(interp, obj, cdefn)
	Tcl_Interp     *interp;	/* interpreter */
	Itcl_Object    *obj;	/* object being destructed */
	Itcl_Class     *cdefn;	/* current class being destructed */
{
	ItclObject     *objPtr = (ItclObject *) obj;
	ItclClass      *cdefnPtr = (ItclClass *) cdefn;

	int             result;
	Itcl_ListElem  *elem;
	Itcl_Class     *cd;
	ItclCmdMember  *mdefn;
	Tcl_HashEntry  *entry;
	Command        *cmdPtr;
	Namespace      *nsPtr;

	/*
	 * Look for a destructor in this class, and if found, invoke it.
	 */
	nsPtr = (Namespace *) cdefnPtr->namesp;

	if (!Tcl_FindHashEntry(objPtr->destructed, cdefnPtr->name)) {
		entry = Tcl_FindHashEntry(&nsPtr->commands, "destructor");

		if (entry) {
			cmdPtr = (Command *) Tcl_GetHashValue(entry);
			if (cmdPtr->proc == Itcl_ExecMethod) {
				mdefn = (ItclCmdMember *) cmdPtr->clientData;

				/*
				 * NOTE:  Avoid the usual "virtual" behavior
				 * of the destructor by passing its full name
				 * as the command argument.
				 */
				result = (cmdPtr->proc) (cmdPtr->clientData, interp,
						       1, &mdefn->fullname);
			} else {
				Tcl_AppendResult(interp,
				       "command \"destructor\" in class \"",
						 cdefnPtr->name, "\" is not a proper destructor method",
						 (char *) NULL);
				result = TCL_ERROR;
			}

			if (result != TCL_OK) {
				char            msg[256];
				sprintf(msg,
				"\n    (while destructing class \"%.100s\")",
					cdefnPtr->name);
				Tcl_AddErrorInfo(interp, msg);
				return TCL_ERROR;
			}
		}
	}
	/*
	 * Scan through the list of base classes recursively and destruct
	 * them.  Traverse the list in normal order, so that we destruct from
	 * most- to least-specific.
	 */
	elem = Itcl_FirstListElem(&cdefnPtr->bases);
	while (elem) {
		cd = (Itcl_Class *) Itcl_GetListValue(elem);

		if (ItclDestructBase(interp, obj, cd) != TCL_OK) {
			return TCL_ERROR;
		}
		elem = Itcl_NextListElem(elem);
	}
	return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_FindObject()
 * 
 * Searches for an object with the specified name, which may include namespace
 * scope qualifiers like "namesp::namesp::name".  If an object exists, it
 * will be found regardless of the protection level for its access command.
 * In other words, the object may exist, but may not be accessible from the
 * current namespace.
 * 
 * If an error is encountered, this procedure returns TCL_ERROR along with an
 * error message in the interpreter.  Otherwise, it returns TCL_OK.  If an
 * object was found, "roPtr" returns a pointer to the object data.
 * Otherwise, it returns NULL.
 * ------------------------------------------------------------------------
 */
int
Itcl_FindObject(interp, name, roPtr)
	Tcl_Interp     *interp;	/* interpreter containing this object */
	char           *name;	/* name of the object */
	Itcl_Object   **roPtr;	/* returns: object data or NULL */
{
	Tcl_Command     cmd;
	Command        *cmdPtr;

	if (Itcl_FindCommand(interp, name, ITCL_FIND_NO_PRIVACY, &cmd) != TCL_OK) {
		return TCL_ERROR;
	}
	if (cmd != NULL && Itcl_IsObject(cmd)) {
		cmdPtr = (Command *) cmd;
		*roPtr = (Itcl_Object *) cmdPtr->clientData;
	} else {
		*roPtr = NULL;
	}
	return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_IsObject()
 * 
 * Checks the given Tcl command to see if it represents an itcl object. Returns
 * non-zero if the command is associated with an object.
 * ------------------------------------------------------------------------
 */
int
Itcl_IsObject(cmd)
	Tcl_Command     cmd;	/* command being tested */
{
	Command        *cmdPtr = (Command *) cmd;

	if (cmdPtr->deleteProc == ItclDestroyObject) {
		return 1;
	}
	return 0;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_ObjectIsa()
 * 
 * Checks to see if an object belongs to the given class.  An object "is-a"
 * member of the class if the class appears anywhere in its inheritance
 * hierarchy.  Returns non-zero if the object belongs to the class, and zero
 * otherwise.
 * ------------------------------------------------------------------------
 */
int
Itcl_ObjectIsa(obj, cdefn)
	Itcl_Object    *obj;	/* object being tested */
	Itcl_Class     *cdefn;	/* class to test for "is-a" relationship */
{
	Itcl_HierIter   hier;
	Itcl_Class     *cd;

	Itcl_InitHierIter(&hier, obj->cdefn);
	while ((cd = Itcl_AdvanceHierIter(&hier)) != NULL) {
		if (cd == cdefn) {
			break;
		}
	}
	Itcl_DeleteHierIter(&hier);

	return (cd != NULL);
}


/*
 * ------------------------------------------------------------------------
 * Itcl_HandleInstance()
 * 
 * Invoked by Tcl whenever the user issues a command associated with an object
 * instance.  Handles the following syntax:
 * 
 * <objName> <method> <args>...
 * 
 * ------------------------------------------------------------------------
 */
int
Itcl_HandleInstance(clientData, interp, argc, argv)
	ClientData      clientData;	/* object definition */
	Tcl_Interp     *interp;	/* current interpreter */
	int             argc;	/* number of arguments */
	char          **argv;	/* argument strings */
{
	ItclObject     *objPtr = (ItclObject *) clientData;

	int             status;
	Tcl_HashEntry  *entry;
	ItclCmdMember  *mdefn;
	Itcl_ActiveNamespace nsToken;
	char           *cmd;

	if (argc < 2) {
		Tcl_AppendResult(interp, "wrong # args: should be one of...",
				 (char *) NULL);
		ItclReportObjectUsage(interp, (Itcl_Object *) objPtr);
		return TCL_ERROR;
	}
	/*
	 * Make sure that the specified operation is really an object method.
	 * If not, sort the list of methods in alphabetical order and return
	 * usage information.
	 */
	mdefn = NULL;

	entry = Tcl_FindHashEntry(&objPtr->cdefn->virtualMethods, argv[1]);
	if (entry) {
		mdefn = (ItclCmdMember *) Tcl_GetHashValue(entry);
	} else {
		Itcl_FindCmdDefn(interp, (Itcl_Class *) objPtr->cdefn, argv[1], &mdefn);
		Tcl_ResetResult(interp);
	}

	if (mdefn) {
		if ((mdefn->flags & ITCL_COMMON) != 0) {
			mdefn = NULL;
		} else if (mdefn->protection != ITCL_PUBLIC) {
			if (!ItclCanAccessNamesp((Namespace *) mdefn->cdefn->namesp,
						 (Namespace *) Itcl_GetActiveNamesp(interp), mdefn->protection)) {
				mdefn = NULL;
			}
		}
	}
	if (!mdefn && (*argv[1] != 'i' || strcmp(argv[1], "info") != 0)) {
		Tcl_AppendResult(interp, "bad option \"", argv[1],
				 "\": should be one of...",
				 (char *) NULL);
		ItclReportObjectUsage(interp, (Itcl_Object *) objPtr);
		return TCL_ERROR;
	}
	/*
	 * Install the most-specific namespace for this object, with the
	 * object context as clientData.  Invoke the rest of the args as a
	 * command in that namespace.
	 */
	nsToken = Itcl_ActivateNamesp2(interp, objPtr->cdefn->namesp,
				       (ClientData) objPtr);

	if (nsToken) {
		cmd = Tcl_Merge(argc - 1, argv + 1);
		status = Itcl_EvalArgs(interp, cmd, (char *) NULL, argc - 1, argv + 1);
		ckfree(cmd);

		Itcl_DeactivateNamesp(interp, nsToken);
	} else {
		status = TCL_ERROR;
	}
	return status;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_GetInstanceVar()
 * 
 * Returns the current value for an object data member.  The member name is
 * interpreted with respect to the given class scope, which is usually the
 * most-specific class for the object.
 * 
 * If successful, this procedure returns a pointer to a string value which
 * remains alive until the variable changes it value.  If anything goes
 * wrong, this returns NULL.
 * ------------------------------------------------------------------------
 */
char           *
Itcl_GetInstanceVar(interp, name, obj, scope)
	Tcl_Interp     *interp;	/* current interpreter */
	char           *name;	/* name of desired instance variable */
	Itcl_Object    *obj;	/* current object */
	Itcl_Class     *scope;	/* name is interpreted in this scope */
{
	CallFrame       frame;
	Itcl_ActiveNamespace nsToken;
	char           *val;

	/*
	 * Make sure that the current namespace context includes an object
	 * that is being manipulated.
	 */
	if (obj == NULL) {
		Tcl_AppendResult(interp, "cannot access object-specific info ",
				 "without an object context",
				 (char *) NULL);
		return NULL;
	}
	/*
	 * NOTE:  A local call frame MUST be installed to gain proper access
	 * to object-specific data members. Object-specific variables cannot
	 * be cached at the class-namespace level, and therefore must be
	 * accessed via a local call frame.
	 */
	nsToken = Itcl_ActivateNamesp2(interp, scope->namesp,
				       (ClientData) obj);

	if (nsToken) {
		Itcl_InstallCallFrame(interp, &frame, 0, (char **) NULL);

		val = Tcl_GetVar2(interp, name, (char *) NULL, 0);

		Itcl_RemoveCallFrame(interp);
		Itcl_DeactivateNamesp(interp, nsToken);
	} else {
		val = NULL;
	}
	return val;
}


/*
 * ------------------------------------------------------------------------
 * ItclReportObjectUsage()
 * 
 * Appends information to the given interp summarizing the usage for all of the
 * methods available for this object.  Useful when reporting errors in
 * Itcl_HandleInstance().
 * ------------------------------------------------------------------------
 */
static void
ItclReportObjectUsage(interp, obj)
	Tcl_Interp     *interp;	/* current interpreter */
	Itcl_Object    *obj;	/* current object */
{
	ItclClass      *cdefnPtr = (ItclClass *) obj->cdefn;

	Itcl_List       cmdList;
	Itcl_ListElem  *elem;
	Tcl_HashEntry  *entry;
	Tcl_HashSearch  place;
	ItclCmdMember  *mdefn, *cmpDefn;
	int             cmp;

	/*
	 * Scan through all methods in the virtual table and sort them in
	 * alphabetical order.
	 */
	Itcl_InitList(&cmdList);
	entry = Tcl_FirstHashEntry(&cdefnPtr->virtualMethods, &place);
	while (entry) {
		mdefn = (ItclCmdMember *) Tcl_GetHashValue(entry);

		if ((mdefn->flags & ITCL_CONSTRUCTOR) != 0 ||
		    (mdefn->flags & ITCL_DESTRUCTOR) != 0) {
			mdefn = NULL;
		}
		if (mdefn && mdefn->protection != ITCL_PUBLIC) {
			if (!ItclCanAccessNamesp((Namespace *) mdefn->cdefn->namesp,
						 (Namespace *) Itcl_GetActiveNamesp(interp), mdefn->protection)) {
				mdefn = NULL;
			}
		}
		if (mdefn) {
			elem = Itcl_FirstListElem(&cmdList);
			while (elem) {
				cmpDefn = (ItclCmdMember *) Itcl_GetListValue(elem);
				cmp = strcmp(mdefn->name, cmpDefn->name);
				if (cmp < 0) {
					Itcl_InsertListElem(elem, (ClientData) mdefn);
					mdefn = NULL;
					break;
				} else if (cmp == 0) {
					mdefn = NULL;
					break;
				}
				elem = Itcl_NextListElem(elem);
			}
			if (mdefn) {
				Itcl_AppendList(&cmdList, (ClientData) mdefn);
			}
		}
		entry = Tcl_NextHashEntry(&place);
	}

	/*
	 * Add a series of statements showing usage info.
	 */
	elem = Itcl_FirstListElem(&cmdList);
	while (elem) {
		mdefn = (ItclCmdMember *) Itcl_GetListValue(elem);
		Tcl_AppendResult(interp, "\n  ",
			 Itcl_GetCmdMemberUsage(mdefn, obj), (char *) NULL);
		elem = Itcl_NextListElem(elem);
	}
	Itcl_DeleteList(&cmdList);
}


/*
 * ------------------------------------------------------------------------
 * ItclTraceThisVar()
 * 
 * Invoked to handle read/write traces on the "this" variable built into each
 * object.
 * 
 * On read, this procedure updates the "this" variable to contain the current
 * object name.  This is done dynamically, since an object's identity can
 * change if its access command is renamed.
 * 
 * On write, this procedure returns an error string, warning that the "this"
 * variable cannot be set.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
static char    *
ItclTraceThisVar(cdata, interp, name1, name2, flags)
	ClientData      cdata;	/* object instance data */
	Tcl_Interp     *interp;	/* interpreter managing this variable */
	char           *name1;	/* variable name */
	char           *name2;	/* unused */
	int             flags;	/* flags indicating read/write */
{
	Itcl_Object    *obj = (Itcl_Object *) cdata;
	char           *objName;

	/*
	 * Handle read traces on "this"
	 */
	if ((flags & TCL_TRACE_READS) != 0) {
		if (obj->accessCmd) {
			objName = Tcl_GetCommandPath(obj->cdefn->interp, obj->accessCmd);
		} else {
			objName = "";
		}
		Tcl_SetVar(interp, name1, objName, 0);
		return NULL;
	}
	/*
	 * Handle write traces on "this"
	 */
	if ((flags & TCL_TRACE_WRITES) != 0) {
		return "variable \"this\" cannot be modified";
	}
	return NULL;
}


/*
 * ------------------------------------------------------------------------
 * ItclDestroyObject()
 * 
 * Invoked when the object access command is deleted to implicitly destroy the
 * object.  Invokes the object's destructors, ignoring any errors encountered
 * along the way.  Removes the object from the list of all known objects and
 * releases the access command's claim to the object data.
 * 
 * Note that the usual way to delete an object is via Itcl_DeleteObject(). This
 * procedure is provided as a back-up, to handle the case when an object is
 * deleted by removing its access command.
 * ------------------------------------------------------------------------
 */
static void
ItclDestroyObject(cdata)
	ClientData      cdata;	/* object instance data */
{
	Itcl_Object    *obj = (Itcl_Object *) cdata;
	ItclClass      *cdefnPtr = (ItclClass *) obj->cdefn;
	Tcl_HashEntry  *entry;
	Itcl_InterpState istate;

	/*
	 * Remove the object from the global list.
	 */
	entry = Tcl_FindHashEntry(&cdefnPtr->info->objects, (char *) obj->accessCmd);
	if (entry) {
		Tcl_DeleteHashEntry(entry);
	}
	/*
	 * The object access command is already dead, so null it out to avoid
	 * access.
	 */
	obj->accessCmd = NULL;

	/*
	 * Attempt to destruct the object, but ignore any errors.
	 */
	istate = Itcl_SaveInterpState(cdefnPtr->interp, 0);
	Itcl_DestructObject(cdefnPtr->interp, obj);
	Itcl_RestoreInterpState(cdefnPtr->interp, istate);

	Itcl_ReleaseData((ClientData) obj);
}


/*
 * ------------------------------------------------------------------------
 * ItclFreeObject()
 * 
 * Deletes all instance variables and frees all memory associated with the given
 * object instance.  This is usually invoked automatically by
 * Itcl_ReleaseData(), when an object's data is no longer being used.
 * ------------------------------------------------------------------------
 */
static void
ItclFreeObject(cdata)
	ClientData      cdata;	/* object instance data */
{
	ItclObject     *objPtr = (ItclObject *) cdata;
	Tcl_Interp     *interp = objPtr->cdefn->interp;

	int             i;
	ItclClass      *cdPtr;
	Itcl_HierIter   hier;
	Tcl_HashSearch  place;
	Tcl_HashEntry  *entry;
	ItclVarDefn    *vdefn;
	Itcl_ActiveNamespace nsToken, csToken;
	CallFrame       frame;
	Itcl_InterpState istate;

	/*
	 * Install the class namespace and object context so that the
	 * object's data members can be destroyed via simple "unset"
	 * commands.  This makes sure that traces work properly and all
	 * memory gets cleaned up.
	 * 
	 * NOTE:  A local call frame MUST be installed to gain proper access to
	 * object-specific data members.  Object-specific variables cannot be
	 * cached at the class-namespace level, and therefore must be
	 * accessed via a local call frame.
	 * 
	 * NOTE:  Be careful to save and restore the interpreter state. Data can
	 * get freed in the middle of any operation, and we can't affort to
	 * clobber the interpreter with any errors from below.
	 */
	istate = Itcl_SaveInterpState(interp, 0);

	nsToken = Itcl_ActivateNamesp2(interp, objPtr->cdefn->namesp,
				       (ClientData) objPtr);

	if (nsToken) {
		Itcl_InstallCallFrame(interp, &frame, 0, (char **) NULL);

		/*
		 * Scan through all object-specific data members and destroy
		 * the actual variables that maintain the object state.  Do
		 * this by unsetting each variable, so that traces are fired
		 * off correctly.  Make sure that the built-in "this"
		 * variable is only destroyed once.  Also, be careful to
		 * activate the namespace for each class, so that private
		 * variables can be accessed.
		 */
		Itcl_InitHierIter(&hier, (Itcl_Class *) objPtr->cdefn);
		cdPtr = (ItclClass *) Itcl_AdvanceHierIter(&hier);
		while (cdPtr != NULL) {
			csToken = Itcl_ActivateNamesp2(interp, cdPtr->namesp,
						       (ClientData) objPtr);

			entry = Tcl_FirstHashEntry(&cdPtr->variables, &place);
			while (entry) {
				vdefn = (ItclVarDefn *) Tcl_GetHashValue(entry);
				if ((vdefn->flags & ITCL_THIS_VAR) != 0) {
					if (cdPtr == objPtr->cdefn) {
						Tcl_UnsetVar2(interp, vdefn->fullname,
							  (char *) NULL, 0);
					}
				} else if ((vdefn->flags & ITCL_COMMON) == 0) {
					Tcl_UnsetVar2(interp, vdefn->fullname,
						      (char *) NULL, 0);
				}
				entry = Tcl_NextHashEntry(&place);
			}
			if (csToken) {
				Itcl_DeactivateNamesp(interp, csToken);
			}
			cdPtr = (ItclClass *) Itcl_AdvanceHierIter(&hier);
		}
		Itcl_DeleteHierIter(&hier);

		Itcl_RemoveCallFrame(interp);
		Itcl_DeactivateNamesp(interp, nsToken);

		/*
		 * Free the memory associated with object-specific variables.
		 * For normal variables this would be done automatically by
		 * CleanupVar() when the variable is unset.  But
		 * object-specific variables are protected by an extra
		 * reference count, and they must be deleted explicitly here.
		 */
		for (i = 0; i < objPtr->dataSize; i++) {
			if (objPtr->data[i]) {
				ckfree((char *) objPtr->data[i]);
			}
		}
	}
	/*
	 * If the object data members cannot be deleted "nicely" with
	 * Tcl_UnsetVar(), then do it the hard way.  Dummy up a variable
	 * table, and delete the variables as is they were going out of
	 * scope.
	 */
	else {
		Tcl_HashTable   objvars;
		ItclVarLookup  *vlookup;
		Var            *varPtr;
		int             newEntry;

		Tcl_InitHashTable(&objvars, TCL_STRING_KEYS);

		entry = Tcl_FirstHashEntry(&objPtr->cdefn->virtualData, &place);
		while (entry) {
			vlookup = (ItclVarLookup *) Tcl_GetHashValue(entry);
			if ((vlookup->vdefn->flags & ITCL_COMMON) == 0) {
				varPtr = objPtr->data[vlookup->vindex];
				if (varPtr) {
					varPtr->hPtr = entry;
					varPtr->refCount--;

					entry = Tcl_CreateHashEntry(&objvars,
					vlookup->vdefn->fullname, &newEntry);
					Tcl_SetHashValue(entry, (ClientData) varPtr);
				}
				objPtr->data[vlookup->vindex] = NULL;
			}
			entry = Tcl_NextHashEntry(&place);
		}
		TclDeleteVars((Interp *) interp, &objvars);
	}
	Itcl_RestoreInterpState(interp, istate);

	/*
	 * Free any remaining memory associated with the object.
	 */
	ckfree((char *) objPtr->data);

	if (objPtr->constructed) {
		Tcl_DeleteHashTable(objPtr->constructed);
		ckfree((char *) objPtr->constructed);
	}
	if (objPtr->destructed) {
		Tcl_DeleteHashTable(objPtr->destructed);
		ckfree((char *) objPtr->destructed);
	}
	Itcl_ReleaseData((ClientData) objPtr->cdefn);

	ckfree((char *) objPtr);
}


/*
 * ------------------------------------------------------------------------
 * ItclCreateObjVar()
 * 
 * Creates one variable acting as a data member for a specific object.
 * Initializes the variable according to its definition, and sets up its
 * reference count so that it cannot be deleted by ordinary means.  Installs
 * the new variable directly into the data array for the specified object.
 * 
 * NOTE:  This procedure initializes the variable using a "set" command, and
 * therefore assumes that the proper namespace and object context are already
 * installed.
 * ------------------------------------------------------------------------
 */
static void
ItclCreateObjVar(interp, vdefn, obj)
	Tcl_Interp     *interp;	/* interpreter managing this object */
	ItclVarDefn    *vdefn;	/* variable definition */
	Itcl_Object    *obj;	/* object being updated */
{
	ItclObject     *objPtr = (ItclObject *) obj;

	Var            *varPtr;
	Tcl_HashEntry  *entry;
	ItclVarLookup  *vlookup;
	Itcl_ActiveNamespace nsToken;

	varPtr = TclNewVar(objPtr->cdefn->interp);
	varPtr->name = vdefn->name;
	varPtr->namesp = (Namespace *) vdefn->cdefn->namesp;
	varPtr->protection = vdefn->protection;

	/*
	 * NOTE:  Tcl reports a "dangling upvar" error for variables with a
	 * null "hPtr" field.  Put something non-zero in here to keep
	 * Tcl_SetVar2() happy.  The only time this field is really used is
	 * it remove a variable from the hash table that contains it in
	 * CleanupVar, but since these variables are protected by their
	 * higher refCount, they will not be deleted by CleanupVar anyway.
	 * These variables are unset and removed in ItclFreeObject().
	 */
	varPtr->hPtr = (Tcl_HashEntry *) 0x1;
	varPtr->refCount = 1;	/* protect from being deleted */

	/*
	 * Install the new variable in the object's data array. Look up the
	 * appropriate index for the object using the data table in the class
	 * definition.
	 */
	entry = Tcl_FindHashEntry(&objPtr->cdefn->virtualData, vdefn->fullname);

	if (entry) {
		vlookup = (ItclVarLookup *) Tcl_GetHashValue(entry);
		objPtr->data[vlookup->vindex] = varPtr;
	}
	/*
	 * If this variable has an initial value, initialize it here using a
	 * "set" command.
	 */
	if (vdefn->init) {
		nsToken = Itcl_ActivateNamesp2(interp, vdefn->cdefn->namesp,
					       (ClientData) obj);

		if (nsToken != NULL) {
			char           *token = vdefn->init;
			char           *var;

			if (*token == '@') {
				var = (char *) Itcl_FindC(token + 1);
				if (var == NULL) {
#if defined __sun||LINUX
					void           *handle;

					handle = (void *) dlopen(0, 2);
					var = (char *) dlsym(handle, token + 1);
					dlclose(handle);
#endif
#ifdef __hpux
					shl_findsym(PROG_HANDLE, token + 1, (short) TYPE_UNDEFINED, (void *) &var);
#endif
#ifdef VXWORKS
					SYM_TYPE        psymType;	/* symbol type */
					Tcl_DString     newName;	/* add "_" in front of
									 * name */

					Tcl_DStringInit(&newName);
#ifndef VXWORKSPPC
					Tcl_DStringAppend(&newName, "_", 1);
#endif
					Tcl_DStringAppend(&newName, token + 1, -1);

					symFindByName(sysSymTbl, Tcl_DStringValue(&newName), (char *) &var, &psymType);
					Tcl_DStringFree(&newName);
#endif
				}
				if (var == NULL) {
					printf("no registered C variable with name \"%s\"\n", token + 1);
				} else {
					Tcl_LinkVar(interp, vdefn->fullname, (char *) var, TCL_LINK_INT);
				}
			} else {
				Tcl_SetVar2(objPtr->cdefn->interp, vdefn->fullname, (char *) NULL,
					    vdefn->init, 0);
			}
			Itcl_DeactivateNamesp(interp, nsToken);
		}
	}
}
