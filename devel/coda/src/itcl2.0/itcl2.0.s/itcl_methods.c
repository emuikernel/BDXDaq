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
 * These procedures handle commands available within a class scope. In [incr
 * Tcl], the term "method" is used for a procedure that has access to
 * object-specific data, while the term "proc" is used for a procedure that
 * has access only to common class data.
 * 
 * ========================================================================
 * AUTHOR:  Michael J. McLennan       Phone: (610)712-2842 AT&T Bell
 * Laboratories   E-mail: michael.mclennan@att.com RCS:  $Id:
 * itcl_methods.c,v 1.1.1.1 1996/08/21 19:29:43 heyes Exp $
 * ========================================================================
 * Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
#include "itclInt.h"

#ifdef VXWORKS
#include <loadLib.h>
#include <sysSymTbl.h>
#endif

#if defined __sun||LINUX
#include <dlfcn.h>
#endif

static char     rcsid[] = "$Id";

/*
 * FORWARD DECLARATIONS
 */
static int ItclParseConfig 
_ANSI_ARGS_((Tcl_Interp * interp,
	     int argc, char **argv, Itcl_Object * obj,
	     int *rargc, ItclVarDefn *** rvars, char ***rvals));

	static int ItclHandleConfig _ANSI_ARGS_((Tcl_Interp * interp,
						                 int argc, ItclVarDefn ** vars, char **vals, Itcl_Object * obj));

	static int ItclReportProcErrors _ANSI_ARGS_((Tcl_Interp * interp,
                     ItclCmdMember * mdefn, Itcl_Object * obj, int status));


/*
 * ------------------------------------------------------------------------
 * Itcl_BodyCmd()
 * 
 * Invoked by Tcl whenever the user issues an "itcl::body" command to define or
 * redefine the implementation for a class method/proc. Handles the following
 * syntax:
 * 
 * itcl::body <class>::<func> <arglist> <body>
 * 
 * Looks for an existing class member function with the name <func>, and if
 * found, tries to assign the implementation.  If an argument list was
 * specified in the original declaration, it must match <arglist> or an error
 * is flagged.  If <body> has the form "@name" then it is treated as a
 * reference to a C handling procedure; otherwise, it is taken as a body of
 * Tcl statements.
 * 
 * Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
	int
	                Itcl_BodyCmd(dummy, interp, argc, argv)
	ClientData      dummy;	/* unused */
	Tcl_Interp     *interp;	/* current interpreter */
	int             argc;	/* number of arguments */
	char          **argv;	/* argument strings */
{
	static Tcl_DString *buffer = NULL;

	char           *head, *tail;
	Itcl_Namespace  ns;
	Itcl_Class     *cdefn;
	ItclCmdMember  *mdefn;

	if (argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0], " class::func arglist body\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	/*
	 * If the name buffer has not yet been created, do it now.
	 */
	if (buffer == NULL) {
		buffer = (Tcl_DString *) ckalloc(sizeof(Tcl_DString));
		Tcl_DStringInit(buffer);
	}
	Tcl_DStringTrunc(buffer, 0);

	/*
	 * Parse the member name "namesp::namesp::class::func". Make sure
	 * that a class name was specified, and that the class exists.
	 */
	Itcl_ParseNamespPath(argv[1], &head, &tail);
	Tcl_DStringAppend(buffer, tail, -1);
	tail = Tcl_DStringValue(buffer);

	if (!head || *head == '\0') {
		Tcl_AppendResult(interp, "missing class specifier ",
				 "for body declaration \"", argv[1], "\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	if (Itcl_FindClass(interp, head, &ns) != TCL_OK) {
		return TCL_ERROR;
	}
	if (ns == NULL) {
		Tcl_AppendResult(interp, "class \"",
				 head, "\" not found in context \"",
		     Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	cdefn = (Itcl_Class *) Itcl_GetNamespData(ns);

	/*
	 * Find the function and try to change its implementation. Note that
	 * Itcl_FindCmdDefn() will find any implementation, even in a base
	 * class.  Make sure that the class containing the method definition
	 * is the requested class.
	 */
	if (argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0], " class::func arglist body\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	if (Itcl_FindCmdDefn(interp, cdefn, tail, &mdefn) != TCL_OK) {
		return TCL_ERROR;
	}
	if (mdefn && mdefn->cdefn != (ItclClass *) cdefn) {
		mdefn = NULL;
	}
	if (mdefn == NULL) {
		Tcl_AppendResult(interp, "function \"", tail,
				 "\" is not defined in class \"", Itcl_GetNamespPath(ns), "\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	if (Itcl_ChangeCmdMember(interp, mdefn, argv[2], argv[3]) != TCL_OK) {
		return TCL_ERROR;
	}
	return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_ConfigBodyCmd()
 * 
 * Invoked by Tcl whenever the user issues an "itcl::configbody" command to
 * define or redefine the configuration code associated with a public
 * variable.  Handles the following syntax:
 * 
 * itcl::configbody <class>::<publicVar> <body>
 * 
 * Looks for an existing public variable with the name <publicVar>, and if
 * found, tries to assign the implementation.  If <body> has the form "@name"
 * then it is treated as a reference to a C handling procedure; otherwise, it
 * is taken as a body of Tcl statements.
 * 
 * Returns TCL_OK/TCL_ERROR to indicate success/failure.
 * ------------------------------------------------------------------------
 */
/* ARGSUSED */
int
Itcl_ConfigBodyCmd(dummy, interp, argc, argv)
	ClientData      dummy;	/* unused */
	Tcl_Interp     *interp;	/* current interpreter */
	int             argc;	/* number of arguments */
	char          **argv;	/* argument strings */
{
	char           *head, *tail;
	Itcl_Namespace  ns;
	Itcl_Class     *cdefn;
	ItclVarDefn    *vdefn;
	ItclCmdImplement *mimpl;

	if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
				 argv[0], " class::option body\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	/*
	 * Parse the member name "namesp::namesp::class::option". Make sure
	 * that a class name was specified, and that the class exists.
	 */
	Itcl_ParseNamespPath(argv[1], &head, &tail);

	if (!head || *head == '\0') {
		Tcl_AppendResult(interp, "missing class specifier ",
				 "for body declaration \"", argv[1], "\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	if (Itcl_FindClass(interp, head, &ns) != TCL_OK) {
		return TCL_ERROR;
	}
	if (ns == NULL) {
		Tcl_AppendResult(interp, "class \"",
				 head, "\" not found in context \"",
		     Itcl_GetNamespPath(Itcl_GetActiveNamesp(interp)), "\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	cdefn = (Itcl_Class *) Itcl_GetNamespData(ns);

	/*
	 * Find the variable and change its implementation. Note that
	 * Itcl_FindVarDefn() will find any implementation, even in a base
	 * class.  Make sure that the class containing the variable
	 * definition is the requested class.
	 */
	if (Itcl_FindVarDefn(interp, cdefn, tail, &vdefn) != TCL_OK) {
		return TCL_ERROR;
	}
	if (vdefn && vdefn->cdefn != (ItclClass *) cdefn) {
		vdefn = NULL;
	}
	if (vdefn == NULL) {
		Tcl_AppendResult(interp, "option \"", tail,
				 "\" is not defined in class \"", Itcl_GetNamespPath(ns), "\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	if (vdefn->protection != ITCL_PUBLIC) {
		Tcl_AppendResult(interp, "option \"", vdefn->fullname,
				 "\" is not a public configuration option",
				 (char *) NULL);
		return TCL_ERROR;
	}
	if (Itcl_CreateCmdImplement(interp, (char *) NULL, argv[2], &mimpl)
	    != TCL_OK) {
		return TCL_ERROR;
	}
	Itcl_PreserveData((ClientData) mimpl);
	Itcl_EventuallyFree((ClientData) mimpl, Itcl_DeleteCmdImplement);

	if (vdefn->config) {
		Itcl_ReleaseData((ClientData) vdefn->config);
	}
	vdefn->config = mimpl;

	return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_CreateMethod()
 * 
 * Installs a method (implemented with Tcl code) into the namespace associated
 * with a class.  If another command with the same name is already installed,
 * then it is overwritten.  Returns TCL_OK on success, or TCL_ERROR (along
 * with an error message in the specified interp) if anything goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_CreateMethod(interp, cdefn, name, arglist, body)
	Tcl_Interp     *interp;	/* interpreter managing this action */
	Itcl_Class     *cdefn;	/* class definition */
	char           *name;	/* name of new method */
	char           *arglist;/* space-separated list of arg names */
	char           *body;	/* body of commands for the method */
{
	ItclCmdMember  *mdefn;
	Itcl_ActiveNamespace nsToken;

	/*
	 * Make sure that the method name does not contain anything goofy
	 * like a "::" scope qualifier.
	 */
	if (strstr(name, "::")) {
		Tcl_AppendResult(interp, "bad method name \"", name, "\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	/*
	 * Create the method definition.
	 */
	if (Itcl_CreateCmdMember(interp, cdefn, name, arglist, body, &mdefn) != TCL_OK) {
		return TCL_ERROR;
	}
	/*
	 * Activate the class namespace, and install the method command.
	 */
	nsToken = Itcl_ActivateNamesp(interp, cdefn->namesp);

	Tcl_CreateCommand(interp, name, Itcl_ExecMethod,
		   Itcl_PreserveData((ClientData) mdefn), Itcl_ReleaseData);

	Itcl_DeactivateNamesp(interp, nsToken);

	Itcl_EventuallyFree((ClientData) mdefn, Itcl_DeleteCmdMember);
	return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 * Itcl_CreateProc()
 * 
 * Installs a class proc into the namespace associated with a class. If another
 * command with the same name is already installed, then it is overwritten.
 * Returns TCL_OK on success, or TCL_ERROR (along with an error message in
 * the specified interp) if anything goes wrong.
 * ------------------------------------------------------------------------
 */
int
Itcl_CreateProc(interp, cdefn, name, arglist, body)
	Tcl_Interp     *interp;	/* interpreter managing this action */
	Itcl_Class     *cdefn;	/* class definition */
	char           *name;	/* name of new proc */
	char           *arglist;/* space-separated list of arg names */
	char           *body;	/* body of commands for the proc */
{
	ItclCmdMember  *mdefn;
	Itcl_ActiveNamespace nsToken;

	/*
	 * Make sure that the proc name does not contain anything goofy like
	 * a "::" scope qualifier.
	 */
	if (strstr(name, "::")) {
		Tcl_AppendResult(interp, "bad proc name \"", name, "\"",
				 (char *) NULL);
		return TCL_ERROR;
	}
	/*
	 * Create the proc definition.
	 */
	if (Itcl_CreateCmdMember(interp, cdefn, name, arglist, body, &mdefn) != TCL_OK) {
		return TCL_ERROR;
	}
	/*
	 * Mark procs as "common".  This distinguishes them from methods.
	 */
	mdefn->flags |= ITCL_COMMON;

	/*
	 * Activate the class namespace, and install the proc command.
	 */
	nsToken = Itcl_ActivateNamesp(interp, cdefn->namesp);

	Tcl_CreateCommand(interp, name, Itcl_ExecProc,
		   Itcl_PreserveData((ClientData) mdefn), Itcl_ReleaseData);

	Itcl_DeactivateNamesp(interp, nsToken);

	Itcl_EventuallyFree((ClientData) mdefn, Itcl_DeleteCmdMember);
	return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_CreateCmdMember()
 * 
 * Creates the data record representing a member function.  This includes the
 * argument list and the body of the function.  If the body is of the form
 * "@name", then it is treated as a label for a C procedure registered by
 * Itcl_RegisterC().
 * 
 * If any errors are encountered, this procedure returns TCL_ERROR along with an
 * error message in the interpreter.  Otherwise, it returns TCL_OK, and
 * "mdefnPtr" returns a pointer to the new member function.
 * ------------------------------------------------------------------------
 */
int
Itcl_CreateCmdMember(interp, cdefn, name, arglist, body, mdefnPtr)
	Tcl_Interp     *interp;	/* interpreter managing this action */
	Itcl_Class     *cdefn;	/* class definition */
	char           *name;	/* name of new member */
	char           *arglist;/* space-separated list of arg names */
	char           *body;	/* body of commands for the method */
	ItclCmdMember **mdefnPtr;	/* returns: pointer to new method
					 * defn */
{
	ItclCmdMember  *mdefn;
	ItclCmdImplement *mimpl;
	int             fullsize;

	/*
	 * Try to create the implementation for this command member.
	 */
	if (Itcl_CreateCmdImplement(interp, arglist, body, &mimpl) != TCL_OK) {
		return TCL_ERROR;
	}
	/*
	 * Allocate a member function definition and return.
	 */
	mdefn = (ItclCmdMember *) ckalloc(sizeof(ItclCmdMember));
	mdefn->interp = interp;
	mdefn->cdefn = (ItclClass *) cdefn;
	mdefn->flags = 0;
	mdefn->arglist = NULL;

	mdefn->name = (char *) ckalloc((unsigned) (strlen(name) + 1));
	strcpy(mdefn->name, name);

	fullsize = strlen(cdefn->name) + 2 + strlen(name);
	mdefn->fullname = (char *) ckalloc((unsigned) (fullsize + 1));
	strcpy(mdefn->fullname, cdefn->name);
	strcat(mdefn->fullname, "::");
	strcat(mdefn->fullname, name);

	mdefn->protection = Itcl_CmdProtection(interp, 0);

	if (arglist) {
		mdefn->flags |= ITCL_ARG_SPEC;
	}
	if (mimpl->arglist) {
		Itcl_CreateArgList(interp, arglist, &mdefn->arglist);
	}
	if (strcmp(name, "constructor") == 0) {
		mdefn->flags |= ITCL_CONSTRUCTOR;
	}
	if (strcmp(name, "destructor") == 0) {
		mdefn->flags |= ITCL_DESTRUCTOR;
	}
	mdefn->implement = mimpl;
	Itcl_PreserveData((ClientData) mimpl);
	Itcl_EventuallyFree((ClientData) mimpl, Itcl_DeleteCmdImplement);

	*mdefnPtr = mdefn;
	return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_ChangeCmdMember()
 * 
 * Modifies the data record representing a member function.  This is usually the
 * body of the function, but can include the argument list if it was not
 * defined when the member was first created. If the body is of the form
 * "@name", then it is treated as a label for a C procedure registered by
 * Itcl_RegisterC().
 * 
 * If any errors are encountered, this procedure returns TCL_ERROR along with an
 * error message in the interpreter.  Otherwise, it returns TCL_OK, and
 * "mdefnPtr" returns a pointer to the new member function.
 * ------------------------------------------------------------------------
 */
int
Itcl_ChangeCmdMember(interp, mdefn, arglist, body)
	Tcl_Interp     *interp;	/* interpreter managing this action */
	ItclCmdMember  *mdefn;	/* command member being changed */
	char           *arglist;/* space-separated list of arg names */
	char           *body;	/* body of commands for the method */
{
	ItclCmdImplement *mimpl = NULL;

	/*
	 * Try to create the implementation for this command member.
	 */
	if (Itcl_CreateCmdImplement(interp, arglist, body, &mimpl) != TCL_OK) {
		return TCL_ERROR;
	}
	/*
	 * If the argument list was defined when the function was created,
	 * compare the arg lists or usage strings to make sure that the
	 * interface is not being redefined.
	 */
	if ((mdefn->flags & ITCL_ARG_SPEC) != 0 &&
	    !Itcl_EquivArgLists(mdefn->arglist, mimpl->arglist)) {
		Tcl_AppendResult(interp,
				 "argument list changed for function \"",
				 mdefn->fullname, "\": should be \"",
				 Itcl_ArgList(mdefn->arglist), "\"",
				 (char *) NULL);
		goto changeCmdFail;
	}
	/*
	 * Free up the old implementation and install the new one.
	 */
	Itcl_PreserveData((ClientData) mimpl);
	Itcl_EventuallyFree((ClientData) mimpl, Itcl_DeleteCmdImplement);

	Itcl_ReleaseData((ClientData) mdefn->implement);
	mdefn->implement = mimpl;

	return TCL_OK;

changeCmdFail:
	if (mimpl) {
		Itcl_DeleteCmdImplement(mimpl);
	}
	return TCL_ERROR;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_DeleteCmdMember()
 * 
 * Destroys all data associated with the given member function definition.
 * Usually invoked by the interpreter when a member function is deleted.
 * ------------------------------------------------------------------------
 */
void
Itcl_DeleteCmdMember(cdata)
	ClientData      cdata;	/* pointer to member function definition */
{
	ItclCmdMember  *mdefn = (ItclCmdMember *) cdata;

	if (mdefn) {
		if (mdefn->arglist) {
			Itcl_DeleteArgList(mdefn->arglist);
		}
		ckfree(mdefn->name);
		ckfree(mdefn->fullname);

		Itcl_ReleaseData((ClientData) mdefn->implement);
		mdefn->implement = NULL;

		ckfree((char *) mdefn);
	}
}


/*
 * ------------------------------------------------------------------------
 * Itcl_CreateCmdImplement()
 * 
 * Creates the data record representing the implementation behind a class member
 * function.  This includes the argument list and the body of the function.
 * If the body is of the form "@name", then it is treated as a label for a C
 * procedure registered by Itcl_RegisterC().
 * 
 * The implementation is kept by the member function definition, and controlled
 * by a preserve/release paradigm.  That way, if it is in use while it is
 * being redefined, it will stay around long enough to avoid a core dump.
 * 
 * If any errors are encountered, this procedure returns TCL_ERROR along with an
 * error message in the interpreter.  Otherwise, it returns TCL_OK, and
 * "mimplPtr" returns a pointer to the new implementation.
 * ------------------------------------------------------------------------
 */
int
Itcl_CreateCmdImplement(interp, arglist, body, mimplPtr)
	Tcl_Interp     *interp;	/* interpreter managing this action */
	char           *arglist;/* space-separated list of arg names */
	char           *body;	/* body of commands for the method */
	ItclCmdImplement **mimplPtr;	/* returns: pointer to new
					 * implementation */
{
	Arg            *args;
	ItclCmdImplement *mimpl;
	Tcl_CmdProc    *proc;

	/*
printf("Itcl_CreateCmdImplement: body >%s<\n",body);fflush(stdout);
	*/
	/*
	 * Allocate some space to hold the implementation.
	 */
	mimpl = (ItclCmdImplement *) ckalloc(sizeof(ItclCmdImplement));
	mimpl->flags = 0;
	mimpl->arglist = NULL;
	mimpl->body = NULL;
	mimpl->proc = NULL;

	if (arglist) {
		if (Itcl_CreateArgList(interp, arglist, &args) != TCL_OK) {
			Itcl_DeleteCmdImplement((ClientData) mimpl);
			return TCL_ERROR;
		}
		mimpl->arglist = args;
		mimpl->flags |= ITCL_ARG_SPEC;
	}
	/*
	 * If the body definition starts with '@', then treat the value as a
	 * symbolic name for a C procedure.  Otherwise, treat it as a Tcl
	 * command string.
	 */
	if(body == NULL)
    {
	  mimpl->flags |= ITCL_IMPLEMENT_NONE;
	}
    else if(*body == '@')
    {
	  proc = Itcl_FindC(body + 1);
/*printf("Itcl_CreateCmdImplement: proc1=0x%08x\n",proc);fflush(stdout);*/
	  if(proc == NULL)
      {
#if defined __sun||LINUX
		void *handle;

		handle = (void *) dlopen(0, RTLD_NOW);
/*printf("Itcl_CreateCmdImplement: handle=0x%08x\n",handle);fflush(stdout);*/
		proc = (Tcl_CmdProc *) dlsym(handle, (char *)(body+1));
/*printf("Itcl_CreateCmdImplement: proc2=0x%08x (body >%s<)\n",proc,(char *)(body+1));*/
		dlclose(handle);
#endif
#ifdef __hpux
			shl_findsym(PROG_HANDLE, body + 1, (short) TYPE_UNDEFINED, (void *) &proc);
#endif
#ifdef VXWORKS
		SYM_TYPE        psymType;	/* symbol type */
		Tcl_DString     newName;	/* add "_" in front of
							 * name */

		Tcl_DStringInit(&newName);
#ifndef VXWORKSPPC
		Tcl_DStringAppend(&newName, "_", 1);
#endif
		Tcl_DStringAppend(&newName, body + 1, -1);

		symFindByName(sysSymTbl, Tcl_DStringValue(&newName), (char *) &proc, &psymType);
		Tcl_DStringFree(&newName);
#endif

	  }
	  /*
printf("Itcl_CreateCmdImplement: proc9=0x%08x\n",proc);fflush(stdout);
	  */
	  if(proc == NULL)
      {
		Tcl_AppendResult(interp, "no registered C procedure with name \"",
					 body + 1, "\"",
					 (char *) NULL);
		Itcl_DeleteCmdImplement((ClientData) mimpl);
		return TCL_ERROR;
	  }
	  mimpl->flags |= ITCL_IMPLEMENT_C;
	  mimpl->body = (char *) ckalloc((unsigned) (strlen(body) + 1));
	  strcpy(mimpl->body, body);
	  mimpl->proc = proc;
	}
    else
    {
	  mimpl->flags |= ITCL_IMPLEMENT_TCL;
	  mimpl->body = (char *) ckalloc((unsigned) (strlen(body) + 1));
	  strcpy(mimpl->body, body);
	}

	*mimplPtr = mimpl;
	return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_DeleteCmdImplement()
 * 
 * Destroys all data associated with the given command implementation. Invoked
 * automatically by Itcl_ReleaseData() when the implementation is no longer
 * being used.
 * ------------------------------------------------------------------------
 */
void
Itcl_DeleteCmdImplement(cdata)
	ClientData      cdata;	/* pointer to member function definition */
{
	ItclCmdImplement *mimpl = (ItclCmdImplement *) cdata;
	if (mimpl->arglist) {
		Itcl_DeleteArgList(mimpl->arglist);
	}
	if (mimpl->body) {
		ckfree(mimpl->body);
	}
	ckfree((char *) mimpl);
}


/*
 * ------------------------------------------------------------------------
 * Itcl_GetCmdImplement()
 * 
 * Finds and returns the implementation for a class member function. Note that a
 * member function can be declared without being defined. The class
 * definition may contain a declaration of the member function, but its body
 * may be defined in a separate file.  If an undefined function is
 * encountered, this routine automatically attempts to autoload it.
 * 
 * Returns TCL_ERROR (along with an error message in the interpreter) if an
 * error is encountered, or if the implementation is not defined and cannot
 * be autoloaded.  Returns TCL_OK, along with a pointer to the implementation
 * in "mimplPtr", if successful.
 * ------------------------------------------------------------------------
 */
int
Itcl_GetCmdImplement(interp, mdefn, mimplPtr)
	Tcl_Interp     *interp;	/* interpreter managing this action */
	ItclCmdMember  *mdefn;	/* command member definition */
	ItclCmdImplement **mimplPtr;	/* returns: pointer to implementation */
{
	int             status;

	/*
	 * If the implementation has not yet been defined, try to autoload it
	 * now.
	 */
	if ((mdefn->implement->flags & ITCL_IMPLEMENT_NONE) != 0) {
		status = Tcl_VarEval(interp, "::auto_load ", mdefn->fullname,
				     (char *) NULL);

		if (status != TCL_OK) {
			char            msg[256];
			sprintf(msg, "\n    (while autoloading member function \"%.100s\")",
				mdefn->fullname);
			Tcl_AddErrorInfo(interp, msg);
			return status;
		}
		Tcl_ResetResult(interp);	/* get rid of 1/0 status */
	}
	/*
	 * If the implementation is still not available, then autoloading
	 * must have failed.
	 */
	if ((mdefn->implement->flags & ITCL_IMPLEMENT_NONE) != 0) {
		Tcl_AppendResult(interp, "member function \"", mdefn->fullname,
			       "\" is not defined and cannot be autoloaded",
				 (char *) NULL);
		return TCL_ERROR;
	}
	*mimplPtr = mdefn->implement;
	return TCL_OK;
}

/*
 * ------------------------------------------------------------------------
 * Itcl_EvalCmdImplement()
 * 
 * Used to execute an ItclCmdImplement representation of a code fragment.  This
 * code may be a body of Tcl commands, or a C handler procedure.
 * 
 * Executes the command with the given arguments (argc,argv) and returns an
 * integer status code (TCL_OK/TCL_ERROR).  Returns the result string or an
 * error message in the interpreter.
 * ------------------------------------------------------------------------
 */
int
Itcl_EvalCmdImplement(interp, mdefn, mimpl, argc, argv)
	Tcl_Interp     *interp;	/* current interpreter */
	ItclCmdMember  *mdefn;	/* command member info (for error messages) */
	ItclCmdImplement *mimpl;/* member implementation */
	int             argc;	/* number of arguments */
	char          **argv;	/* argument strings */
{
	int             result = TCL_OK;
	CallFrame       frame;

	/*
	 * Install a call frame for local Tcl variables.
	 */
	Itcl_InstallCallFrame(interp, &frame, argc, argv);
	Itcl_PreserveData((ClientData) mimpl);

	/*
	 * Execute the body of the proc...
	 */
	if ((mimpl->flags & ITCL_IMPLEMENT_C) != 0) {
		result = (*mimpl->proc) ((ClientData) NULL, interp, argc, argv);
	} else if ((mimpl->flags & ITCL_IMPLEMENT_TCL) != 0) {
		if (argc > 0) {
			if (Itcl_ParseArgs(interp, argc, argv, mdefn, mimpl, 0) != TCL_OK) {
				result = TCL_ERROR;
			}
		}
		if (result == TCL_OK) {
			result = Tcl_Eval(interp, mimpl->body);
		}
	} else {
		panic("bad implementation flag found in command implementation");
	}

	/*
	 * Deactivate the local call frame and return.
	 */
	Itcl_ReleaseData((ClientData) mimpl);
	Itcl_RemoveCallFrame(interp);

	return result;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_CreateArgList()
 * 
 * Parses a Tcl list representing an argument declaration and returns a linked
 * list of Arg values.  Usually invoked as part of Itcl_CreateCmdMember()
 * when a new method or procedure is being defined.
 * ------------------------------------------------------------------------
 */
int
Itcl_CreateArgList(interp, decl, argPtr)
	Tcl_Interp     *interp;	/* interpreter managing this function */
	char           *decl;	/* string representing argument list */
	Arg           **argPtr;	/* returns pointer to parsed argument list */
{
	int             status = TCL_OK;	/* assume that this will
						 * succeed */

	int             i, argc, fargc;
	char          **argv, **fargv;
	Arg            *newarg, *next;

	*argPtr = NULL;

	if (decl) {
		if (Tcl_SplitList(interp, decl, &argc, &argv) != TCL_OK) {
			return TCL_ERROR;
		}
		for (i = argc - 1; i >= 0 && status == TCL_OK; i--) {
			if (Tcl_SplitList(interp, argv[i], &fargc, &fargv) != TCL_OK) {
				status = TCL_ERROR;
			} else {
				newarg = NULL;
				if (fargc == 0 || *fargv[0] == '\0') {
					char            mesg[100];
					sprintf(mesg, "argument #%d has no name", i);
					Tcl_SetResult(interp, mesg, TCL_VOLATILE);
					status = TCL_ERROR;
				} else if (fargc > 2) {
					Tcl_AppendResult(interp,
							 "too many fields in argument specifier \"",
							 argv[i], "\"",
							 (char *) NULL);
					status = TCL_ERROR;
				} else if (strstr(fargv[0], "::")) {
					Tcl_AppendResult(interp, "bad argument name \"",
							 fargv[0], "\"",
							 (char *) NULL);
					status = TCL_ERROR;
				} else if (fargc == 1) {
					newarg = Itcl_CreateArg(fargv[0], (char *) NULL);
				} else if (fargc == 2) {
					newarg = Itcl_CreateArg(fargv[0], fargv[1]);
				}
				if (newarg) {	/* attach new arg to arglist */
					newarg->nextPtr = *argPtr;
					*argPtr = newarg;
				}
			}
			ckfree((char *) fargv);
		}
		ckfree((char *) argv);
	}
	/*
	 * If anything went wrong, destroy whatever arguments were created
	 * and return an error.
	 */
	if (status != TCL_OK) {
		for (newarg = *argPtr; newarg; newarg = next) {
			next = newarg->nextPtr;
			ckfree((char *) newarg);
		}
		*argPtr = NULL;
	}
	return status;
}

/*
 * ------------------------------------------------------------------------
 * Itcl_CreateArg()
 * 
 * Creates a new Tcl Arg structure and fills it with the given information.
 * Returns a pointer to the new Arg structure.
 * ------------------------------------------------------------------------
 */
Arg *
Itcl_CreateArg(name, init)
	char           *name;	/* name of new argument */
	char           *init;	/* initial value */
{
	Arg            *newarg = NULL;
	int             nameLen, valueLen;
	unsigned        len;

	nameLen = (name) ? strlen(name) + 1 : 0;
	valueLen = (init) ? strlen(init) + 1 : 0;
	len = sizeof(Arg) - sizeof(newarg->name) + nameLen + valueLen;
	newarg = (Arg *) ckalloc(len);

	strcpy(newarg->name, name);

	if (init) {
		newarg->defValue = newarg->name + nameLen;
		strcpy(newarg->defValue, init);
	} else
		newarg->defValue = NULL;

	return newarg;
}

/*
 * ------------------------------------------------------------------------
 * Itcl_DeleteArgList()
 * 
 * Destroys a chain of arguments acting as an argument list.  Usually invoked
 * when a method/proc is being destroyed, to discard its argument list.
 * ------------------------------------------------------------------------
 */
void
Itcl_DeleteArgList(arglist)
	Arg            *arglist;/* first argument in arg list chain */
{
	Arg            *arg, *next;

	for (arg = arglist; arg; arg = next) {
		next = arg->nextPtr;
		ckfree((char *) arg);
	}
}

/*
 * ------------------------------------------------------------------------
 * Itcl_ArgList()
 * 
 * Returns a string representation for the given argument list.  This string
 * remains valid until the next call to Itcl_ArgList().
 * ------------------------------------------------------------------------
 */
char           *
Itcl_ArgList(arg)
	Arg            *arg;	/* first argument in arglist */
{
	static Tcl_DString *buffer = NULL;

	/*
	 * If the arglist buffer has not yet been created, do it now.
	 */
	if (buffer == NULL) {
		buffer = (Tcl_DString *) ckalloc(sizeof(Tcl_DString));
		Tcl_DStringInit(buffer);
	}
	Tcl_DStringTrunc(buffer, 0);

	while (arg) {
		if (arg->defValue) {
			Tcl_DStringStartSublist(buffer);
			Tcl_DStringAppendElement(buffer, arg->name);
			Tcl_DStringAppendElement(buffer, arg->defValue);
			Tcl_DStringEndSublist(buffer);
		} else {
			Tcl_DStringAppendElement(buffer, arg->name);
		}
		arg = arg->nextPtr;
	}
	return Tcl_DStringValue(buffer);
}

/*
 * ------------------------------------------------------------------------
 * Itcl_EquivArgLists()
 * 
 * Compares two argument lists to see if they are equivalent.  The first list is
 * treated as a prototype, and the second list must match it.  Argument names
 * may be different, but they must match in meaning.  If one argument is
 * optional, the corresponding argument must also be optional.  If the
 * prototype list ends with the magic "args" argument, then it matches
 * everything in the other list.
 * 
 * Returns non-zero if the argument lists are equivalent.
 * ------------------------------------------------------------------------
 */
int
Itcl_EquivArgLists(arg1, arg2)
	Arg            *arg1;	/* prototype argument list */
	Arg            *arg2;	/* another argument list to match against */
{
	while (arg1 && arg2) {
		/*
		 * If the prototype argument list ends with the magic "args"
		 * argument, then it matches everything in the other list.
		 */
		if (arg1->nextPtr == NULL && strcmp(arg1->name, "args") == 0) {
			return 1;
		}
		/*
		 * If one has a default value, then the other must have the
		 * same default value.
		 */
		if (arg1->defValue) {
			if (arg2->defValue == NULL) {
				return 0;
			}
			if (strcmp(arg1->defValue, arg2->defValue) != 0) {
				return 0;
			}
		}
		arg1 = arg1->nextPtr;
		arg2 = arg2->nextPtr;
	}
	if (arg1 && arg1->nextPtr == NULL && strcmp(arg1->name, "args") == 0) {
		return 1;
	}
	return (arg1 == NULL && arg2 == NULL);
}


/*
 * ------------------------------------------------------------------------
 * Itcl_GetCmdMemberUsage()
 * 
 * Returns a string showing how a command member should be invoked. If the
 * command member is a method, then the specified object name is reported as
 * part of the invocation path:
 * 
 * obj method arg ?arg arg ...?
 * 
 * Otherwise, the "obj" pointer is ignored, and the class name is used as the
 * invocation path:
 * 
 * class::proc arg ?arg arg ...?
 * 
 * Returns a string that remains valid until the next call to this procedure.
 * ------------------------------------------------------------------------
 */
char           *
Itcl_GetCmdMemberUsage(mdefn, obj)
	ItclCmdMember  *mdefn;	/* command member being examined */
	Itcl_Object    *obj;	/* invoked with respect to this object */
{
	static Tcl_DString *buffer = NULL;
	Arg            *arglist, *argPtr;
	char           *name;

	/*
	 * If a buffer has not yet been created, do it now.
	 */
	if (buffer == NULL) {
		buffer = (Tcl_DString *) ckalloc(sizeof(Tcl_DString));
		Tcl_DStringInit(buffer);
	}
	Tcl_DStringTrunc(buffer, 0);

	/*
	 * If the command is a method and an object context was specified,
	 * then add the object context.  If the method was a constructor, and
	 * if the object is being created, then report the invocation via the
	 * class creation command.
	 */
	if ((mdefn->flags & ITCL_COMMON) == 0) {
		if ((mdefn->flags & ITCL_CONSTRUCTOR) != 0 &&
		    ((ItclObject *) obj)->constructed) {
			name = Tcl_GetCommandName(obj->cdefn->interp,
						  obj->cdefn->accessCmd);
			Tcl_DStringAppend(buffer, name, -1);
			Tcl_DStringAppend(buffer, " ", -1);
			name = Tcl_GetCommandName(obj->cdefn->interp, obj->accessCmd);
			Tcl_DStringAppend(buffer, name, -1);
		} else if (obj && obj->accessCmd) {
			name = Tcl_GetCommandName(obj->cdefn->interp, obj->accessCmd);
			Tcl_DStringAppend(buffer, name, -1);
			Tcl_DStringAppend(buffer, " ", -1);
			Tcl_DStringAppend(buffer, mdefn->name, -1);
		} else {
			Tcl_DStringAppend(buffer, "<object> ", -1);
			Tcl_DStringAppend(buffer, mdefn->name, -1);
		}
	} else {
		Tcl_DStringAppend(buffer,
			      Itcl_GetNamespName(mdefn->cdefn->namesp), -1);
		Tcl_DStringAppend(buffer, "::", -1);
		Tcl_DStringAppend(buffer, mdefn->name, -1);
	}

	/*
	 * Add the argument usage info.
	 */
	if (mdefn->implement) {
		arglist = mdefn->implement->arglist;
	} else if (mdefn->arglist) {
		arglist = mdefn->arglist;
	} else {
		arglist = NULL;
	}

	if (arglist) {
		for (argPtr = arglist; argPtr != NULL; argPtr = argPtr->nextPtr) {
			if (argPtr->nextPtr == NULL && strcmp(argPtr->name, "args") == 0) {
				Tcl_DStringAppend(buffer, " ?arg arg ...?", -1);
			} else if (argPtr->defValue) {
				Tcl_DStringAppend(buffer, " ?", -1);
				Tcl_DStringAppend(buffer, argPtr->name, -1);
				Tcl_DStringAppend(buffer, "?", -1);
			} else {
				Tcl_DStringAppend(buffer, " ", -1);
				Tcl_DStringAppend(buffer, argPtr->name, -1);
			}
		}
	}
	return Tcl_DStringValue(buffer);
}


/*
 * ------------------------------------------------------------------------
 * Itcl_ExecMethod()
 * 
 * Invoked by Tcl to handle the execution of a user-defined method. A method is
 * similar to the usual Tcl proc, but has access to object-specific data.
 * The object context is installed as ClientData for the active namespace.
 * If for some reason this is NULL, then a method is inappropriate in this
 * context, and an error is returned.
 * 
 * Methods are implemented either as Tcl code fragments, or as C-coded
 * procedures.  For Tcl code fragments, command arguments are parsed
 * according to the argument list, and the body is executed in the scope of
 * the class where it was defined.  For C procedures, the arguments are
 * passed in "as-is", and the procedure is executed in the most-specific
 * class scope.
 * ------------------------------------------------------------------------
 */
int
Itcl_ExecMethod(clientData, interp, argc, argv)
	ClientData      clientData;	/* method definition */
	Tcl_Interp     *interp;	/* current interpreter */
	int             argc;	/* number of arguments */
	char          **argv;	/* argument strings */
{
	ItclCmdMember  *mdefn = (ItclCmdMember *) clientData;
	ItclObject     *objPtr = (ItclObject *) Itcl_GetActiveNamespData(interp);
	int             result = TCL_OK;

	int             newEntry;
	Tcl_HashEntry  *entry;
	ItclCmdImplement *mimpl;
	Itcl_ActiveNamespace nsToken;
	CallFrame       frame;

	/*
	 * Make sure that the current namespace context includes an object
	 * that is being manipulated.
	 */
	if (objPtr == NULL) {
		Tcl_AppendResult(interp, "cannot access object-specific info ",
				 "without an object context",
				 (char *) NULL);
		return TCL_ERROR;
	}
	/*
	 * All methods should be "virtual" unless they are invoked with a
	 * "::" scope qualifier.
	 * 
	 * To implement the "virtual" behavior, find the most-specific
	 * implementation for the method by looking in the "virtualMethods"
	 * table for this class.
	 */
	if (strstr(argv[0], "::") == NULL) {
		entry = Tcl_FindHashEntry(&objPtr->cdefn->virtualMethods, mdefn->name);
		mdefn = (ItclCmdMember *) Tcl_GetHashValue(entry);
	}
	/*
	 * If this method does not have an implementation defined, then try
	 * to autoload one.
	 */
	if (Itcl_GetCmdImplement(interp, mdefn, &mimpl) != TCL_OK) {
		return TCL_ERROR;
	}
	/*
	 * Activate the namespace where the method was defined and install a
	 * call frame for local Tcl variables.  Obviously, Tcl methods need
	 * the call frame to represent the local scope.  But C
	 * implementations need a call frame as well.  The call frame acts as
	 * a cache for object-specific data members, since these cannot be
	 * cached at the class-generic level.
	 */
	nsToken = Itcl_ActivateNamesp2(interp, mdefn->cdefn->namesp,
				       (ClientData) objPtr);
	Itcl_InstallCallFrame(interp, &frame, argc, argv);
	Itcl_PreserveData((ClientData) mdefn);
	Itcl_PreserveData((ClientData) mimpl);


	/*
	 * Execute the body of the method...
	 */
	if ((mimpl->flags & ITCL_IMPLEMENT_C) != 0) {

		if ((mdefn->flags & ITCL_CONSTRUCTOR) && objPtr->constructed) {
			if (Itcl_ParseArgs(interp, argc, argv, mdefn,
					   mimpl, mdefn->flags) != TCL_OK) {
				result = TCL_ERROR;
				goto methodDone;
			}
			if (Itcl_ConstructBase(interp, (Itcl_Object *) objPtr,
				   (Itcl_Class *) mdefn->cdefn) != TCL_OK) {
				result = TCL_ERROR;
				goto methodDone;
			}
		}
		result = (*mimpl->proc) ((ClientData) objPtr->ClientData, interp, argc, argv);
	} else if ((mimpl->flags & ITCL_IMPLEMENT_TCL) != 0) {
		if (Itcl_ParseArgs(interp, argc, argv, mdefn,
				   mimpl, mdefn->flags) != TCL_OK) {
			result = TCL_ERROR;
			goto methodDone;
		}
		if ((mdefn->flags & ITCL_CONSTRUCTOR) && objPtr->constructed) {
			if (Itcl_ConstructBase(interp, (Itcl_Object *) objPtr,
				   (Itcl_Class *) mdefn->cdefn) != TCL_OK) {
				result = TCL_ERROR;
				goto methodDone;
			}
		}
		result = Tcl_Eval(interp, mimpl->body);
	} else {
		panic("bad implementation flag found in method definition");
	}

	/*
	 * If this method is a constructor or destructor, and if it is being
	 * invoked at the appropriate time, keep track of which methods have
	 * been called.  This information is used to implicitly invoke
	 * constructors/destructors as needed.
	 */
	if ((mdefn->flags & ITCL_DESTRUCTOR) && objPtr->destructed) {
		Tcl_CreateHashEntry(objPtr->destructed, mdefn->cdefn->name, &newEntry);
	}
	if ((mdefn->flags & ITCL_CONSTRUCTOR) && objPtr->constructed) {
		Tcl_CreateHashEntry(objPtr->constructed, mdefn->cdefn->name, &newEntry);
	}
	/*
	 * Interpret the result code and handle errors.
	 */
	result = ItclReportProcErrors(interp, mdefn, (Itcl_Object *) objPtr, result);

methodDone:
	/*
	 * Deactivate the method scope and return.
	 */
	Itcl_ReleaseData((ClientData) mimpl);
	Itcl_ReleaseData((ClientData) mdefn);
	Itcl_RemoveCallFrame(interp);
	Itcl_DeactivateNamesp(interp, nsToken);

	return result;
}

/*
 * ------------------------------------------------------------------------
 * Itcl_ExecProc()
 * 
 * Invoked by Tcl to handle the execution of a user-defined proc.
 * 
 * Procs are implemented either as Tcl code fragments, or as C-coded procedures.
 * For Tcl code fragments, command arguments are parsed according to the
 * argument list, and the body is executed in the scope of the class where it
 * was defined.  For C procedures, the arguments are passed in "as-is", and
 * the procedure is executed in the most-specific class scope.
 * ------------------------------------------------------------------------
 */
int
Itcl_ExecProc(clientData, interp, argc, argv)
	ClientData      clientData;	/* proc definition */
	Tcl_Interp     *interp;	/* current interpreter */
	int             argc;	/* number of arguments */
	char          **argv;	/* argument strings */
{
	ItclCmdMember  *mdefn = (ItclCmdMember *) clientData;

	int             result;
	ItclCmdImplement *mimpl;
	Itcl_ActiveNamespace nsToken;

	/*
	 * If this method does not have an implementation defined, then try
	 * to autoload one.
	 */
	if (Itcl_GetCmdImplement(interp, mdefn, &mimpl) != TCL_OK) {
		return TCL_ERROR;
	}
	/*
	 * Activate the namespace where the proc was defined and install a
	 * call frame for local Tcl variables.
	 * 
	 * Nullify the clientData for this context.  This blocks any
	 * object-specific data from the current context.
	 */
	nsToken = Itcl_ActivateNamesp2(interp, mdefn->cdefn->namesp,
				       (ClientData) NULL);
	Itcl_PreserveData((ClientData) mdefn);

	/*
	 * Execute the body of the proc. Don't trust the "mimpl" pointer
	 * after this call--a procedure can redefine itself, making its old
	 * implementation void.
	 */
	result = Itcl_EvalCmdImplement(interp, mdefn, mimpl, argc, argv);

	/*
	 * Interpret the result code and handle errors.
	 */
	result = ItclReportProcErrors(interp, mdefn, (Itcl_Object *) NULL, result);

	/*
	 * Deactivate the proc scope and return.
	 */
	Itcl_ReleaseData((ClientData) mdefn);
	Itcl_DeactivateNamesp(interp, nsToken);

	return result;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_ConstructBase()
 * 
 * Usually invoked just before executing the body of a constructor when an
 * object is first created.  Makes sure that all base classes are properly
 * constructed.  If an "initCmd" was defined with the constructor for the
 * class, then it is invoked.  After that, the list of base classes is
 * checked for constructors that are defined but have not yet been invoked.
 * Each of these is invoked implicitly with no arguments.
 * 
 * Assumes that a local call frame is already installed, and that constructor
 * arguments have already been matched and are sitting in this frame.
 * Returns TCL_OK on success, or TCL_ERROR (along with an error message in
 * interp->result) on error.
 * ------------------------------------------------------------------------
 */
int
Itcl_ConstructBase(interp, obj, cdefn)
	Tcl_Interp     *interp;	/* interpreter */
	Itcl_Object    *obj;	/* object being constructed */
	Itcl_Class     *cdefn;	/* current class being constructed */
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
	 * If the class has an "initCmd", invoke it in the current context.
	 * 
	 * TRICKY NOTE: This context is the call frame containing the arguments
	 * for the constructor.  The "initCmd" only makes sense right
	 * now--just before the body of the constructor is executed.
	 */
	if (cdefnPtr->initCmd) {
		if (Tcl_Eval(interp, cdefnPtr->initCmd) != TCL_OK) {
			char            msg[256];
			sprintf(msg, "\n    (while constructing base class \"%.100s\")",
				cdefnPtr->name);
			Tcl_AddErrorInfo(interp, msg);
			return TCL_ERROR;
		}
	}
	/*
	 * Scan through the list of base classes and see if any of these have
	 * not been constructed.  Invoke base class constructors implicitly,
	 * as needed.  Go through the list of base classes in reverse order,
	 * so that least-specific classes are constructed first.
	 */
	elem = Itcl_LastListElem(&cdefnPtr->bases);
	while (elem) {
		cd = (Itcl_Class *) Itcl_GetListValue(elem);
		nsPtr = (Namespace *) cd->namesp;

		if (!Tcl_FindHashEntry(objPtr->constructed, cd->name)) {
			entry = Tcl_FindHashEntry(&nsPtr->commands, "constructor");

			if (entry) {
				cmdPtr = (Command *) Tcl_GetHashValue(entry);
				if (cmdPtr->proc == Itcl_ExecMethod) {
					mdefn = (ItclCmdMember *) cmdPtr->clientData;

					/*
					 * NOTE:  Avoid the usual "virtual"
					 * behavior of the constructor by
					 * passing its full name as the
					 * command argument.
					 */
					result = (cmdPtr->proc) (cmdPtr->clientData, interp,
						       1, &mdefn->fullname);
				} else {
					Tcl_AppendResult(interp,
							 "command \"constructor\" in class \"",
							 cd->name, "\" is not a proper constructor method",
							 (char *) NULL);
					result = TCL_ERROR;
				}

				if (result != TCL_OK) {
					char            msg[256];
					sprintf(msg,
						"\n    (while constructing base class \"%.100s\")",
						cd->name);
					Tcl_AddErrorInfo(interp, msg);
					return TCL_ERROR;
				}
			}
		}
		elem = Itcl_PrevListElem(elem);
	}
	return TCL_OK;
}


/*
 * ------------------------------------------------------------------------
 * Itcl_ParseArgs()
 * 
 * Matches a list of arguments against a Tcl argument specification. Supports
 * all of the rules regarding arguments for Tcl procs, including default
 * arguments and variable-length argument lists.
 * 
 * Assumes that a local call frame is already installed.  As variables are
 * successfully matched, they are stored as variables in the call frame.
 * Returns TCL_OK on success, or TCL_ERROR (along with an error message in
 * interp->result) on error.
 * ------------------------------------------------------------------------
 */
int
Itcl_ParseArgs(interp, argc, argv, mdefn, mimpl, flags)
	Tcl_Interp     *interp;	/* interpreter */
	int             argc;	/* number of arguments */
	char          **argv;	/* argument strings */
	ItclCmdMember  *mdefn;	/* command member info (for error messages) */
	ItclCmdImplement *mimpl;/* command implementation */
	int             flags;	/* flags:  ITCL_OLD_STYLE => old-style args */
{
	Itcl_Namespace  activeNs = Itcl_GetActiveNamesp(interp);
	Itcl_Object    *obj = (Itcl_Object *) Itcl_GetActiveNamespData(interp);
	Itcl_VarEnforcerProc *varEnforcer = NULL;

	int             result = TCL_OK;

	int             vi;
	char          **args, *value;
	Arg            *argm;
	Tcl_DString     buffer;

	int             defargc;
	char          **defargv = NULL;
	int             configc = 0;
	ItclVarDefn   **configVars = NULL;
	char          **configVals = NULL;

	/*
	 * If the active namespace is a class namespace, then suppress its
	 * variable enforcer while the argument list is being processed.
	 * Otherwise, arguments that have the same names as class data
	 * members would overwrite them.
	 */
	if (Itcl_IsClass(activeNs)) {
		varEnforcer = Itcl_SetVarEnforcer(activeNs, (Itcl_VarEnforcerProc *) NULL);
	}
	/*
	 * Match the actual arguments against the procedure's formal
	 * parameters to compute local variables.
	 */
	for (argm = mimpl->arglist, args = argv + 1, argc--;
	     argm != NULL;
	     argm = argm->nextPtr, args++, argc--) {
		/*
		 * Handle the special case of the last formal being "args".
		 * When it occurs, assign it a list consisting of all the
		 * remaining actual arguments.
		 */
		if ((argm->nextPtr == NULL) &&
		    (strcmp(argm->name, "args") == 0)) {
			if (argc < 0)
				argc = 0;
			value = Tcl_Merge(argc, args);
			Tcl_SetVar(interp, argm->name, value, 0);
			ckfree(value);
			argc = 0;
			break;
		}
		/*
		 * Handle the special case of the last formal being "config".
		 * When it occurs, treat all remaining arguments as public
		 * variable assignments.  Set the local "config" variable to
		 * the list of public variables assigned.
		 */
		else if ((argm->nextPtr == NULL) &&
			 (strcmp(argm->name, "config") == 0) &&
			 Itcl_IsClass(activeNs)) {
			/*
			 * If this is not an old-style method, discourage
			 * against the use of the "config" argument.
			 */
			if ((flags & ITCL_OLD_STYLE) == 0) {
				Tcl_AppendResult(interp,
				  "\"config\" argument is an anachronism\n",
						 "[incr Tcl] no longer supports the \"config\" argument.\n",
						 "Instead, use the \"args\" argument and then use the\n",
						 "built-in configure method to handle args like this:\n",
						 "  eval configure $args",
						 (char *) NULL);
				result = TCL_ERROR;
				goto argErrors;
			}
			/*
			 * Otherwise, handle the "config" argument in the
			 * usual way... - parse all "-name value" assignments
			 * - set "config" argument to the list of variable
			 * names
			 */
			if (argc > 0) {	/* still have some arguments left? */

				result = ItclParseConfig(interp, argc, args,
				   obj, &configc, &configVars, &configVals);

				if (result != TCL_OK) {
					goto argErrors;
				}
				Tcl_DStringInit(&buffer);
				for (vi = 0; vi < configc; vi++) {
					Tcl_DStringAppendElement(&buffer, configVars[vi]->fullname);
				}

				Tcl_SetVar(interp, "config", Tcl_DStringValue(&buffer), 0);
				Tcl_DStringFree(&buffer);

				argc = 0;	/* all remaining args handled */
			} else if (argm->defValue) {	/* otherwise, handle
							 * default values */
				Tcl_SplitList(interp, argm->defValue, &defargc, &defargv);

				result = ItclParseConfig(interp, defargc, defargv,
				   obj, &configc, &configVars, &configVals);

				if (result != TCL_OK) {
					goto argErrors;
				}
				Tcl_DStringInit(&buffer);
				for (vi = 0; vi < configc; vi++) {
					Tcl_DStringAppendElement(&buffer, configVars[vi]->fullname);
				}

				Tcl_SetVar(interp, "config", Tcl_DStringValue(&buffer), 0);
				Tcl_DStringFree(&buffer);
			} else
				Tcl_SetVar(interp, "config", "", 0);
		}
		/*
		 * Resume the usual processing of arguments...
		 */
		else if (argc > 0) {	/* take next arg as value */
			value = *args;
			Tcl_SetVar(interp, argm->name, value, 0);
		} else if (argm->defValue) {	/* ...or use default value */
			value = argm->defValue;
			Tcl_SetVar(interp, argm->name, value, 0);
		} else {
			if (mdefn) {
				Tcl_AppendResult(interp, "wrong # args: should be \"",
				   Itcl_GetCmdMemberUsage(mdefn, obj), "\"",
						 (char *) NULL);
			} else {
				Tcl_AppendResult(interp, "no value given for parameter \"",
						 argm->name, "\"",
						 (char *) NULL);
			}
			result = TCL_ERROR;
			goto argErrors;
		}
	}

	if (argc > 0) {
		if (mdefn) {
			Tcl_AppendResult(interp, "wrong # args: should be \"",
				   Itcl_GetCmdMemberUsage(mdefn, obj), "\"",
					 (char *) NULL);
		} else {
			Tcl_AppendResult(interp, "too many arguments",
					 (char *) NULL);
		}
		result = TCL_ERROR;
		goto argErrors;
	}
	/*
	 * Handle any "config" assignments.  Make sure to put the class
	 * variable enforcer back in place, so that public variables can be
	 * accessed.
	 */
	if (configc > 0) {
		Itcl_SetVarEnforcer(activeNs, varEnforcer);

		if (ItclHandleConfig(interp, configc, configVars, configVals, obj)
		    != TCL_OK) {
			result = TCL_ERROR;
			goto argErrors;
		}
	}
	/*
	 * All arguments were successfully matched.
	 */
	result = TCL_OK;

	/*
	 * If any errors were found, clean up and return error status.
	 */
argErrors:
	if (defargv)
		ckfree((char *) defargv);
	if (configVars)
		ckfree((char *) configVars);
	if (configVals)
		ckfree((char *) configVals);

	/*
	 * If the active namespace is a class namespace, then put its
	 * variable enforcer back to normal.
	 */
	if (Itcl_IsClass(activeNs)) {
		Itcl_SetVarEnforcer(activeNs, varEnforcer);
	}
	return result;
}

/*
 * ------------------------------------------------------------------------
 * ItclParseConfig()
 * 
 * Parses a set of arguments as "-variable value" assignments. Interprets all
 * variable names in the most-specific class scope, so that an inherited
 * method with a "config" parameter will work correctly.  Returns a list of
 * public variable names and their corresponding values; both lists should
 * passed to ItclHandleConfig() to perform assignments, and freed when no
 * longer in use.  Returns a status TCL_OK/TCL_ERROR and returns error
 * messages in the interpreter.
 * ------------------------------------------------------------------------
 */
static int
ItclParseConfig(interp, argc, argv, obj, rargc, rvars, rvals)
	Tcl_Interp     *interp;	/* interpreter */
	int             argc;	/* number of arguments */
	char          **argv;	/* argument strings */
	Itcl_Object    *obj;	/* object whose public vars are being
				 * config'd */
	int            *rargc;	/* return: number of variables accessed */
	ItclVarDefn  ***rvars;	/* return: list of variables */
	char         ***rvals;	/* return: list of values */
{
	int             result = TCL_OK;
	ItclVarDefn    *vdefn;

	if (argc < 0)
		argc = 0;
	*rargc = 0;
	*rvars = (ItclVarDefn **) ckalloc((unsigned) (argc * sizeof(ItclVarDefn *)));
	*rvals = (char **) ckalloc((unsigned) (argc * sizeof(char *)));

	while (argc-- > 0) {
		/*
		 * Next argument should be "-variable"
		 */
		if (**argv != '-') {
			Tcl_AppendResult(interp, "syntax error in config assignment \"",
				 *argv, "\": should be \"-variable value\"",
					 (char *) NULL);
			result = TCL_ERROR;
			break;
		} else if (argc-- <= 0) {
			Tcl_AppendResult(interp, "syntax error in config assignment \"",
					 *argv, "\": should be \"-variable value\" (missing value)",
					 (char *) NULL);
			result = TCL_ERROR;
			break;
		}
		if (Itcl_FindVarDefn(interp, obj->cdefn, (*argv) + 1, &vdefn) != TCL_OK) {
			result = TCL_ERROR;
			break;
		} else if (vdefn == NULL) {
			Tcl_AppendResult(interp,
				     "syntax error in config assignment \"",
					 *argv, "\": unrecognized variable",
					 (char *) NULL);
			result = TCL_ERROR;
			break;
		} else {
			(*rvars)[*rargc] = vdefn;	/* variable definition */
			(*rvals)[*rargc] = *(argv + 1);	/* config value */
			(*rargc)++;
			argv += 2;
		}
	}
	return result;
}

/*
 * ------------------------------------------------------------------------
 * ItclHandleConfig()
 * 
 * Handles the assignment of "config" values to public variables. The list of
 * assignments is parsed in ItclParseConfig(), but the actual assignments are
 * performed here.  If the variables have any associated "config" code, it is
 * invoked here as well.  If errors are detected during assignment or
 * "config" code execution, the variable is set back to its previous value
 * and an error is returned.
 * 
 * Returns a status TCL_OK/TCL_ERROR, and returns any error messages in the
 * given interpreter.
 * ------------------------------------------------------------------------
 */
static int
ItclHandleConfig(interp, argc, vars, vals, obj)
	Tcl_Interp     *interp;	/* interpreter currently in control */
	int             argc;	/* number of assignments */
	ItclVarDefn   **vars;	/* list of public variable definitions */
	char          **vals;	/* list of public variable values */
	Itcl_Object    *obj;	/* object whose public vars are being
				 * config'd */
{
	int             result = TCL_OK;

	int             i;
	char           *val;
	Itcl_ActiveNamespace nsToken, configToken;
	Tcl_DString     lastval;

	Tcl_DStringInit(&lastval);

	/*
	 * All "config" assignments are performed in the most-specific class
	 * scope, so that inherited methods with "config" arguments will work
	 * correctly.
	 */
	nsToken = Itcl_ActivateNamesp2(interp, obj->cdefn->namesp,
				       (ClientData) obj);

	/*
	 * Perform each assignment and executed the "config" code associated
	 * with each variable.  If any errors are encountered, set the
	 * variable back to its previous value, and return an error.
	 */
	for (i = 0; i < argc; i++) {
		val = Tcl_GetVar2(interp, vars[i]->fullname, (char *) NULL, 0);
		if (!val)
			val = "";
		Tcl_DStringTrunc(&lastval, 0);
		Tcl_DStringAppend(&lastval, val, -1);

		/*
		 * Set the variable to the specified value.
		 */
		if (!Tcl_SetVar2(interp, vars[i]->fullname, (char *) NULL, vals[i], 0)) {
			char            msg[256];
			sprintf(msg, "\n    (error in configuration of public variable \"%.100s\")", vars[i]->fullname);
			Tcl_AddErrorInfo(interp, msg);
			result = TCL_ERROR;
			break;
		}
		/*
		 * If the variable has a "config" condition, then execute it
		 * in the scope of its associated class. If it fails, put the
		 * variable back the way it was and return an error.
		 */
		if (vars[i]->config) {
			configToken = Itcl_ActivateNamesp2(interp,
				  vars[i]->cdefn->namesp, (ClientData) obj);

			result = Itcl_EvalCmdImplement(interp, (ItclCmdMember *) NULL,
					vars[i]->config, 0, (char **) NULL);

			Itcl_DeactivateNamesp(interp, configToken);

			if (result != TCL_OK) {
				char            msg[256];
				sprintf(msg, "\n    (error in configuration of public variable \"%.100s\")", vars[i]->fullname);
				Tcl_AddErrorInfo(interp, msg);
				Tcl_SetVar2(interp, vars[i]->fullname, (char *) NULL,
					    Tcl_DStringValue(&lastval), 0);
				break;
			}
		}
	}

	/*
	 * Clean up and return.
	 */
	Tcl_DStringFree(&lastval);
	Itcl_DeactivateNamesp(interp, nsToken);

	return result;
}


/*
 * ------------------------------------------------------------------------
 * ItclReportProcErrors()
 * 
 * Used to interpret the status code returned when the body of a Tcl-style proc
 * is executed.  Handles the "errorInfo" and "errorCode" variables properly,
 * and adds error information into the interpreter if anything went wrong.
 * Returns a new status code that should be treated as the return status code
 * for the command.
 * 
 * This same operation is usually buried in the Tcl InterpProc() procedure.  It
 * is defined here so that it can be reused more easily.
 * ------------------------------------------------------------------------
 */
static int
ItclReportProcErrors(interp, mdefn, obj, status)
	Tcl_Interp     *interp;	/* interpreter being modified */
	ItclCmdMember  *mdefn;	/* command member that was invoked */
	Itcl_Object    *obj;	/* object context for this command */
	int             status;	/* integer status code from proc body */
{
	Interp         *iPtr = (Interp *) interp;
	Tcl_DString     buffer;
	char            num[20];

	if (status == TCL_RETURN) {
		status = iPtr->returnCode;
		iPtr->returnCode = TCL_OK;

		if (status == TCL_ERROR) {
			Tcl_SetVar2(interp, "errorCode", (char *) NULL,
			(iPtr->errorCode != NULL) ? iPtr->errorCode : "NONE",
				    TCL_GLOBAL_ONLY);
			iPtr->flags |= ERROR_CODE_SET;
			if (iPtr->errorInfo != NULL) {
				Tcl_SetVar2(interp, "errorInfo", (char *) NULL,
					  iPtr->errorInfo, TCL_GLOBAL_ONLY);
				iPtr->flags |= ERR_IN_PROGRESS;
			}
		}
	} else if (status == TCL_ERROR) {
		Tcl_DStringInit(&buffer);
		Tcl_DStringAppend(&buffer, "\n    (", -1);

		if (obj && obj->accessCmd) {
			char           *objName;
			objName = Tcl_GetCommandPath(obj->cdefn->interp, obj->accessCmd);
			Tcl_DStringAppend(&buffer, "object \"", -1);
			Tcl_DStringAppend(&buffer, objName, -1);
			Tcl_DStringAppend(&buffer, "\" ", -1);
		}
		if ((mdefn->flags & ITCL_COMMON) != 0) {
			Tcl_DStringAppend(&buffer, "procedure", -1);
		} else {
			Tcl_DStringAppend(&buffer, "method", -1);
		}
		Tcl_DStringAppend(&buffer, " \"", -1);

		Tcl_DStringAppend(&buffer,
			      Itcl_GetNamespPath(mdefn->cdefn->namesp), -1);
		Tcl_DStringAppend(&buffer, "::", -1);
		Tcl_DStringAppend(&buffer, mdefn->name, -1);

		if ((mdefn->implement->flags & ITCL_IMPLEMENT_TCL) != 0) {
			Tcl_DStringAppend(&buffer, "\" body line ", -1);
			sprintf(num, "%d", iPtr->errorLine);
			Tcl_DStringAppend(&buffer, num, -1);
			Tcl_DStringAppend(&buffer, ")", -1);
		} else {
			Tcl_DStringAppend(&buffer, "\")", -1);
		}
		Tcl_AddErrorInfo(interp, Tcl_DStringValue(&buffer));
		Tcl_DStringFree(&buffer);
	} else if (status == TCL_BREAK) {
		iPtr->result = "invoked \"break\" outside of a loop";
		status = TCL_ERROR;
	} else if (status == TCL_CONTINUE) {
		iPtr->result = "invoked \"continue\" outside of a loop";
		status = TCL_ERROR;
	}
	return status;
}
