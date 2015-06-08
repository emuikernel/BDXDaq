/* 
 * tclCmdLv.c --
 *
 *	This file contains the top-level command routines for most of
 *	the Tcl built-in commands dealing with List variable facility
 *
 * Copyright (c) 1987-1993 The Regents of the University of California.
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 * Copyright (c) 1995      ICEM CFD, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef lint
static char sccsid[] = "@(#) tclCmdLv.c 1.108 95/01/10 09:26:55";
#endif

#include "tclInt.h"
#include "tclPort.h"

/*
 * The variables below are used to implement the "lsort" command.
 * Unfortunately, this use of static variables prevents "lsort"
 * from being thread-safe, but there's no alternative given the
 * current implementation of qsort.  In a threaded environment
 * these variables should be made thread-local if possible, or else
 * "lsort" needs internal mutual exclusion.
 */

static Tcl_Interp *sortInterp = NULL;	/* Interpreter for "lsort" command. */
static enum {ASCII, INTEGER, REAL, COMMAND} sortMode;
					/* Mode for sorting: compare as strings,
					 * compare as numbers, or call
					 * user-defined command for
					 * comparison. */
static Tcl_DString sortCmd;		/* Holds command if mode is COMMAND.
					 * pre-initialized to hold base of
					 * command. */
static int sortIncreasing;		/* 0 means sort in decreasing order,
					 * 1 means increasing order. */
static int sortCode;			/* Anything other than TCL_OK means a
					 * problem occurred while sorting; this
					 * executing a comparison command, so
					 * the sort was aborted. */

static int SortListCompareProc _ANSI_ARGS_((CONST VOID *first,
					    CONST VOID *second));

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConcatCmd_v --
 *
 *	This procedure is invoked to process the "concat_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ConcatCmd_v(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int i;

    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    Var *varPtr, *arrayPtr;
    int status, flags, allList;
    int table;			/* upvar? */
    Var **allVars;
    Var *static_vars[20];
    Tcl_C_List *list;
    TypeAttribute type, newType;
    char *element;
    int length;

    allVars = static_vars;
    if (argc < 1) {
	
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
	" ?list ?list...\"", (char *) NULL);
	return TCL_ERROR;
    }
    
    status = TCL_OK;
    if (argc > 21) allVars = (Var**) ckalloc((argc-1)*sizeof(Var *));
    
    /* Loop over passed variables.  Pass 1, figure out if the final
       type will be a DSTRING, a LIST or both... */
    
    newType = TCL_TYPE_DSTRING | TCL_TYPE_LIST;
    allList = 1;
    for (i = 1; i < argc; i++) {
	part1 = argv[i];
	Tcl_SplitArray(part1, &openp, &closep);

	if (openp) part2 = openp+1;
	else      part2 = NULL;

	varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG,
			      "concat_v", CRT_PART2, &arrayPtr);

	if (varPtr == NULL) {

	    /* clean up.  Have to wait after trace call if we are ok ... */

	    if (openp) {
		*openp = '('; *closep = ')';
	    }
	
	    status = TCL_ERROR;
	    goto error_exit;
	}
    
	/* invoke traces, convert variable to a list ... */
    
	status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "join_v",
				  TCL_TRACE_READS);
	if (openp) {
	    *openp = '('; *closep = ')';
	}
	if (status != TCL_OK) goto error_exit;
	
	type = varPtr->varAttr.typeAttr;
	
	if (!(type & TCL_TYPE_LIST)) {
	    allList = 0;
	    if (!(type & TCL_TYPE_STRLIST))
		newType &= ~(TCL_TYPE_LIST | TCL_TYPE_STRLIST);
	}
	allVars[i-1] = varPtr;
    }
    if (allList) newType &= ~TCL_TYPE_DSTRING;

    /* Pass 2: Create concatenated object ... */
    
    TVAR_RESET(&iPtr->results);
    TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | newType);
    
    for (i = 1; i < argc; i++) {
	varPtr = allVars[i-1];
	type = varPtr->varAttr.typeAttr;
	
	if (newType&TCL_TYPE_DSTRING) {
	    if (!(type&TCL_TYPE_DSTRING)) {
		if (type & TCL_TYPE_STRING) {
		    status =
			Tcl_ConvertFromStringToDString
			    (interp,
			    &varPtr->value.allValue,
			    &varPtr->varAttr.typeAttr);
		}
		else status = Tcl_ConvertVar(interp, varPtr,
					     TCL_TYPE_DSTRING);
		if (status != TCL_OK) goto error_exit;
	    }
	    
	    /*
	     * Clip white space off the front and back of the string
	     * to generate a neater result, and ignore any empty
	     * elements.
	     */

	    element = varPtr->value.allValue.dString.string;
	    while (isspace(UCHAR(*element))) {
		element++;
	    }
	    for (length = strlen(element);
		(length > 0) && (isspace(UCHAR(element[length-1])));
		 length--) {
		/* Null loop body. */
	    }
	    if (length != 0) {
		Tcl_DStringAppend(&iPtr->results.allValue.dString,
				  element,
				  length);
		if (i != argc-1)
		    Tcl_DStringAppend(&iPtr->results.allValue.dString,
				      " ",
				      1);
	    }
	}
	if (newType&TCL_TYPE_LIST) {
	    if (!(varPtr->varAttr.typeAttr & TCL_TYPE_LIST)) {
		status = Tcl_ConvertToList(interp, varPtr);
		if (status != TCL_OK) goto error_exit;
	    }
	    Tcl_ConcatList(&iPtr->results.allValue.list,
			   &varPtr->value.allValue.list,
			   NULL);
	}
    }
  error_exit:
    if (allVars != static_vars) ckfree((void *) allVars);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_JoinCmd_v --
 *
 *	This procedure is invoked to process the "join_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_JoinCmd_v(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int i;

    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString, *baseStr;
    Var *varPtr, *arrayPtr;
    int status, flags, lenJoin;
    int table;			/* upvar? */
    Tcl_C_List *list;

    if (argc == 2) {
	joinString = " ";
	lenJoin = 1;
    } else if (argc == 3) {
	joinString = argv[2];
	lenJoin = strlen(joinString);
    }
    else {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
	" list ?join String?\"", (char *) NULL);
	return TCL_ERROR;
    }

    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "join_v",
			  CRT_PART2, &arrayPtr);

    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	return TCL_ERROR;
    }
    
    /* invoke traces, convert variable to a list ... */
    
    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "join_v",
			      TCL_TRACE_READS);
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (status != TCL_OK) goto error_exit;
    
    if (!(varPtr->varAttr.typeAttr & TCL_TYPE_LIST)) {
	status = Tcl_ConvertToList(interp, varPtr);
	if (status != TCL_OK) return TCL_ERROR;
    }
    
    /* Loop over elements in the list ... */
    
    list = &varPtr->value.allValue.list;
    baseStr = list->list.string;
    
    TVAR_RESET(&iPtr->results);
    TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | TCL_TYPE_DSTRING);
    
    for (i = 0; i < list->nElem; i++) {
	if (i == 0) {
	    Tcl_DStringAppend(&iPtr->results.allValue.dString,
			      baseStr+list->argv[0], -1);
	} else  {
	    Tcl_DStringAppend(&iPtr->results.allValue.dString,
			      joinString, lenJoin);
	    Tcl_DStringAppend(&iPtr->results.allValue.dString,
			      baseStr+list->argv[i], -1);
	}
    }
  error_exit:
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_Lappend_v --
 *
 *	This procedure is invoked to process the "lappend_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	NO TCL result.  Only the variable is updated
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_LappendCmd_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString;
    Var  *varPtr, *arrayPtr;
    int  i, flags, index, status;
    int  table;			/* upvar? */
    Tcl_C_List  *list;
    Tcl_DString *dStr;
    TypeAttribute type, newType;

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " list element ?element ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    
    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "list_v",
			  CRT_PART1 | CRT_PART2, &arrayPtr);

    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	return TCL_ERROR;
    }
    varPtr->flags &= ~VAR_UNDEFINED;

    /* If we are adding as a list element, make sure we are a list so
       that the element can be appended as both a string and a list
       ... */
    
    type = varPtr->varAttr.typeAttr;
    if (!(type & TCL_TYPE_CONVSTR)) {
	newType = TCL_TYPE_LIST;
	if (!(type & TCL_TYPE_LIST))
	    Tcl_ConvertVar(interp, varPtr, TCL_TYPE_LIST);
    }
    else {
	    
	/* Keep both the Dstring and an existing List representation
	   valid.  If this has the attribute STRLIST, then the
	   list and string reps are equivalent.  Drop the string
	   ... */

	newType = TCL_TYPE_DSTRING;
	if (type & TCL_TYPE_LIST) {
	    if (type & TCL_TYPE_STRLIST)
		 newType = TCL_TYPE_LIST;
	    else newType |= TCL_TYPE_LIST;
	}
	if ((newType&TCL_TYPE_DSTRING) && !(type&TCL_TYPE_DSTRING)) {
	    if (type & TCL_TYPE_STRING) {
		status =
		    Tcl_ConvertFromStringToDString
			(interp,
			&varPtr->value.allValue,
			&varPtr->varAttr.typeAttr);
	    }
	    else status = Tcl_ConvertVar(interp, varPtr,
					 TCL_TYPE_DSTRING);
	    if (status != TCL_OK) { 
		VarErrMsg(interp, part1, part2, "set",
			  "Illegal conversion to DString/String");
		return status;
	    }
	}
    }
    
    /* We are only a DString and/or a list ... */
    
    Tcl_OnlyValidVarMACRO(varPtr, newType);
    list = &varPtr->value.allValue.list;

    for (i = 2; i < argc; i++) {
	
	/* Variable is either a list, a Dstring or both.  If have one
	   or the other just keep them around and if we have both,
	   update them both ... */
	
	if (newType & TCL_TYPE_DSTRING) {
	    int  length, listFlags;
	    int  *len = &varPtr->value.allValue.dString.length;
	    char *dst = varPtr->value.allValue.dString.string + *len;
		
	    length = Tcl_ScanElement(argv[i], &listFlags) + 1;
	    if ((*len > 0) && ((dst[-1] != '{')
			       || ((*len > 1) && (dst[-2] == '\\')))) {
		*dst = ' ';
		dst++;
		*len++;
	    }
	    *len += Tcl_ConvertElement(argv[i], dst, listFlags);
	    status = TCL_OK;
	}
	
	/* Append the element if we have a list representation.
	   If we didn't produce an output string, make it here ... */
	
	if (newType & TCL_TYPE_LIST)
	    status = Tcl_ListAppendElement(interp,
					   &varPtr->value.allValue.list,
					   argv[i],
					   NULL);
	
	if (status != TCL_OK) {

	    /* clean up.  Have to wait after trace call if we are ok ... */

	    if (openp) {
		*openp = '('; *closep = ')';
	    }
	    return TCL_ERROR;
	}
    }

#ifdef __INTERNAL__
    TVAR_RESET(&iPtr->results);
    TVAR_CREATE(iPtr->results, TCL_TYPE_LIST | TCL_TYPE_RETOVR);
    Tcl_ConcatList(&iPtr->results.allValue.list, list, NULL);
    Tcl_OvrLoadResult(interp);
#endif
    
    /* Invoke the trace on the variable ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "lappend_v",
			      TCL_TRACE_WRITES);
    if (openp) {
	*openp = '('; *closep = ')';
    }
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LindexCmd_v --
 *
 *	This procedure is invoked to process the "lindex_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_LindexCmd_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString;
    Var *varPtr, *arrayPtr;
    int status, flags, index, firstIsEnd;
    int table;			/* upvar? */
    Tcl_C_List *list;

    if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " list index\"", (char *) NULL);
	return TCL_ERROR;
    }
    if ((*argv[2] == 'e') && (strncmp(argv[2], "end", strlen(argv[2])) == 0)) {
	firstIsEnd = 1;
	index = INT_MAX;
    } else {
	firstIsEnd = 0;
	if (Tcl_GetInt(interp, argv[2], &index) != TCL_OK) {
	    return TCL_ERROR;
	}
    }
    if (index < 0) {
	return TCL_OK;
    }

    /* Get the variable ... */

    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "lindex_v",
			  CRT_PART2, &arrayPtr);
    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	return TCL_ERROR;
    }

    /* invoke traces, convert variable to a list ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "lindex_v",
			      TCL_TRACE_READS);
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (status != TCL_OK) goto error_exit;
    
    if (!(varPtr->varAttr.typeAttr & TCL_TYPE_LIST)) {
	status = Tcl_ConvertToList(interp, varPtr);
	if (status != TCL_OK) goto error_exit;
    }
    list = &varPtr->value.allValue.list;
    if (firstIsEnd) index = list->nElem-1;
    if (index < list->nElem && index >= 0) {
	
	TVAR_RESET(&iPtr->results);
	TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | TCL_TYPE_DSTRING);
	Tcl_DStringAppend(&iPtr->results.allValue.dString,
			  list->list.string+list->argv[index], -1);
    }
  error_exit:
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LinsertCmd_v --
 *
 *	This procedure is invoked to process the "linsert_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_LinsertCmd_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString;
    Var  *varPtr, *arrayPtr;
    int  i, flags, index, status;
    TypeAttribute type;
    int  table;			/* upvar? */
    Tcl_C_List  *list, *listNew;

    if (argc < 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " list index element ?element ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    
    if ((*argv[2] == 'e') && (strncmp(argv[2], "end", strlen(argv[2])) == 0)) {
	index = INT_MAX;
    } else if (Tcl_GetInt(interp, argv[2], &index) != TCL_OK) {
	return TCL_ERROR;
    }

    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "linsert_v",
			  CRT_PART2, &arrayPtr);

    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	status = TCL_ERROR;
	goto error_exit;
    }

    /* invoke traces ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "linsert_v",
			      TCL_TRACE_READS);
	
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (status != TCL_OK) goto error_exit;
    
    type = varPtr->varAttr.typeAttr;

    /* If and only if a list, do insert as a list ... */

    if (type == TCL_TYPE_LIST) {
    
	list = &varPtr->value.allValue.list;
	if (index < 0) index = 0;
	if (index > list->nElem) index = list->nElem;

	/* Delete whatever is in the result buffer now ... */
    
	TVAR_RESET(&iPtr->results);
	TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | TCL_TYPE_LIST);
	listNew = &(iPtr->results.allValue.list);
   
	/* Copy list so that can insert elements in list.  Put directly
	   into the results area.  This will be forward compatible with
	   Version 2.0 API. ... */
    
	Tcl_ListAllocSize(listNew, list->nElem + argc-3);
	if (index > 0) Tcl_AppendList(listNew, list, 0, index);


	/*
	 * Add the new list elements.
	 */

	for (i = 3; i < argc; i++) {
	    status = Tcl_ListAppendElement(interp, listNew, argv[i], NULL);
	    if (status != TCL_OK) goto error_exit;
	}

	/*
	 * Append the remainder of the original list.
	 */

	Tcl_AppendList(listNew, list, index, list->nElem-index);
    }
    else {
	
	/* Use string representation as the preferred method ... */
	
	char *arg0 = argv[0];
	char *arg1 = argv[1];

	/* Just use the linsert with the string value of the variable ... */
	    
	argv[0] = "linsert";
	if (type & TCL_TYPE_STRING)
	    argv[1] = varPtr->value.allValue.string;
	else if (type & TCL_TYPE_DSTRING)
	    argv[1] = varPtr->value.allValue.dString.string;
	else {
	    Tcl_ConvertVar(interp, varPtr, TCL_TYPE_DSTRING);
	    argv[1] = varPtr->value.allValue.dString.string;
	}

	status = Tcl_LinsertCmd(dummy, interp, argc, argv);

	/* Restore the arguments to be safe and return ... */

	argv[0] = arg0;
	argv[1] = arg1;
    }
  error_exit:
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LinsertCmdl_v --
 *
 *	This procedure is invoked to process the "linsert_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_LinsertCmdl_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString;
    Var  *varPtr, *arrayPtr;
    int  i, flags, index, status;
    TypeAttribute type;
    int  table;			/* upvar? */
    Tcl_C_List  *list, *listNew;

    if (argc < 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " list index element ?element ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    
    if ((*argv[2] == 'e') && (strncmp(argv[2], "end", strlen(argv[2])) == 0)) {
	index = INT_MAX;
    } else if (Tcl_GetInt(interp, argv[2], &index) != TCL_OK) {
	return TCL_ERROR;
    }

    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "linsert_v",
			  CRT_PART2, &arrayPtr);

    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	status = TCL_ERROR;
	goto error_exit;
    }

    /* invoke traces ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "linsert_v",
			      TCL_TRACE_READS);
	
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (status != TCL_OK) goto error_exit;
    
    type = varPtr->varAttr.typeAttr;

    /* If a list, do insert as a list ... */

    if ((type & TCL_TYPE_LIST) || !(type & TCL_TYPE_STRINGY)) {
	if (!(type & TCL_TYPE_LIST))
	    Tcl_ConvertVar(interp, varPtr, TCL_TYPE_LIST);
    
	/* We are only a DString and/or a list ... */
    
	list = &varPtr->value.allValue.list;
	if (index < 0) index = 0;
	if (index > list->nElem) index = list->nElem;

	/* Delete whatever is in the result buffer now ... */
    
	TVAR_RESET(&iPtr->results);
	TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | TCL_TYPE_LIST);
	listNew = &(iPtr->results.allValue.list);
   
	/* Copy list so that can insert elements in list.  Put directly
	   into the results area.  This will be forward compatible with
	   Version 2.0 API. ... */
    
	Tcl_ListAllocSize(listNew, list->nElem + argc-3);
	if (index > 0) Tcl_AppendList(listNew, list, 0, index);


	/*
	 * Add the new list elements.
	 */

	for (i = 3; i < argc; i++) {
	    status = Tcl_ListAppendElement(interp, listNew, argv[i], NULL);
	    if (status != TCL_OK) goto error_exit;
	}

	/*
	 * Append the remainder of the original list.
	 */

	Tcl_AppendList(listNew, list, index, list->nElem-index);
    }
    else {
	char *arg0 = argv[0];
	char *arg1 = argv[1];

	/* Just use the linsert with the string value of the variable ... */
	    
	argv[0] = "linsert";
	if (type & TCL_TYPE_STRING)
	    argv[1] = varPtr->value.allValue.string;
	else if (type & TCL_TYPE_DSTRING)
	    argv[1] = varPtr->value.allValue.dString.string;

	status = Tcl_LinsertCmd(dummy, interp, argc, argv);

	/* Restore the arguments to be safe and return ... */

	argv[0] = arg0;
	argv[1] = arg1;
    }
  error_exit:
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ListCmd_v --
 *
 *	This procedure is invoked to process the "list_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_ListCmd_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString;
    Var  *varPtr, *arrayPtr;
    int  i, flags, index, status;
    int  table;			/* upvar? */
    Tcl_C_List  *list;
    Tcl_DString *dStr;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " variable ?element ?element ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    
    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "list_v",
			  CRT_PART1 | CRT_PART2, &arrayPtr);

    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	return TCL_ERROR;
    }
    varPtr->flags &= ~VAR_UNDEFINED;

    /* variable is a list and a DString ... */

    Tcl_ResetValueMACRO(&varPtr->value.allValue, &varPtr->varAttr.typeAttr);
    Tcl_OnlyValidVarMACRO(varPtr, TCL_TYPE_LIST);
    
    list = &varPtr->value.allValue.list;

    for (i = 2; i < argc; i++) {
	status = Tcl_ListAppendElement(interp, list, argv[i], NULL);
	if (status != TCL_OK) {

	    /* clean up.  Have to wait after trace call if we are ok ... */

	    if (openp) {
	        *openp = '('; *closep = ')';
	    }
	    return TCL_ERROR;
	}
    }

    /* Invoke the trace on the variable ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "list_v",
		     TCL_TRACE_WRITES);
    if (openp) {
	*openp = '('; *closep = ')';
    }
    
#ifdef __INTERNAL__
    if (status == TCL_OK) {
	TVAR_RESET(&iPtr->results);
	TVAR_CREATE(iPtr->results, TCL_TYPE_LIST | TCL_TYPE_RETOVR);
	Tcl_ConcatList(&iPtr->results.allValue.list, list, NULL);
	Tcl_OvrLoadResult(interp);
    }
#endif
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LlengthCmd_v --
 *
 *	This procedure is invoked to process the "llength_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_LlengthCmd_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString;
    Var *varPtr, *arrayPtr;
    int status, flags, index;
    int table;			/* upvar? */
    Tcl_C_List *list;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " list\"", (char *) NULL);
	return TCL_ERROR;
    }

    /* Get the variable ... */

    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "llength_v",
			  CRT_PART2, &arrayPtr);
    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	return TCL_ERROR;
    }

    /* invoke traces, convert variable to a list ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "llength_v",
			      TCL_TRACE_READS);
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (status != TCL_OK) goto error_exit;
    
    if (!(varPtr->varAttr.typeAttr & TCL_TYPE_LIST)) {
	status = Tcl_ConvertToList(interp, varPtr);
	if (status != TCL_OK) return TCL_ERROR;
    }
    list = &varPtr->value.allValue.list;
    TVAR_RESET(&iPtr->results);
    TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | TCL_TYPE_INTEGER);
    iPtr->results.allValue.integer = list->nElem;
  error_exit:
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LrangeCmd_v --
 *
 *	This procedure is invoked to process the "lrange_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_LrangeCmd_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString;
    Var  *varPtr, *arrayPtr;
    int  i, flags, first, last, index, status, firstIsEnd;
    int  table;			/* upvar? */
    TypeAttribute type;
    Tcl_C_List  *list, *listNew;

    if (argc != 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " list first last\"", (char *) NULL);
	return TCL_ERROR;
    }
    if ((*argv[2] == 'e') && (strncmp(argv[2], "end", strlen(argv[2])) == 0)) {
	firstIsEnd = 1;
	first = INT_MAX;
    } else {
	firstIsEnd = 0;
	if (Tcl_GetInt(interp, argv[2], &first) != TCL_OK) {
	    return TCL_ERROR;
	}
    }
    if ((*argv[3] == 'e') && (strncmp(argv[3], "end", strlen(argv[3])) == 0)) {
	last = INT_MAX;
    } else {
	if (Tcl_GetInt(interp, argv[3], &last) != TCL_OK) {
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, "expected integer or \"end\" but got \"",
			     argv[3], "\"", (char *) NULL);
	    return TCL_ERROR;
	}
    }
    
    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);
    
    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "linsert_v",
			  CRT_PART2, &arrayPtr);

    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	status = TCL_ERROR;
	goto error_exit;
    }

    /* invoke traces ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "linsert_v",
			      TCL_TRACE_READS);
	
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (status != TCL_OK) goto error_exit;
    
    /* convert variable to a list ... */

    type = varPtr->varAttr.typeAttr;
    
    /* iff type is a list, process as a list, otherwise as a string ... */
    
    if (type == TCL_TYPE_LIST) {

	list = &varPtr->value.allValue.list;
    
	if (first < 0) first = 0;
	if (firstIsEnd) first = list->nElem-1;
    
	if (first > last || first >= list->nElem) return TCL_OK;
	if (last >= list->nElem) last = list->nElem-1;
    
	/* Delete whatever is in the result buffer now ... */
    
	TVAR_RESET(&iPtr->results);
	TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | TCL_TYPE_LIST);
	listNew = &(iPtr->results.allValue.list);
 
	/* Make a new list from the first to last indices ... */
    
	Tcl_AppendList(listNew, list, first, last-first+1);
#ifdef __INTERNAL__
	Tcl_OvrLoadResult(interp);
#endif
    }
    else {
	
	/* String is preferred method of handling ... */
	
	char *arg0 = argv[0];
	char *arg1 = argv[1];

	/* Just use the linsert with the string value of the variable ... */
	    
	argv[0] = "lrange";
	if (type & TCL_TYPE_STRING)
	    argv[1] = varPtr->value.allValue.string;
	else if (type & TCL_TYPE_DSTRING)
	    argv[1] = varPtr->value.allValue.dString.string;
	else {
	    Tcl_ConvertVar(interp, varPtr, TCL_TYPE_DSTRING);
	    argv[1] = varPtr->value.allValue.dString.string;
	}

	status = Tcl_LrangeCmd(dummy, interp, argc, argv);

	/* Restore the arguments to be safe and return ... */

	argv[0] = arg0;
	argv[1] = arg1;
    }
  error_exit:
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LrangeCmdl_v --
 *
 *	This procedure is invoked to process the "lrange_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_LrangeCmdl_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString;
    Var  *varPtr, *arrayPtr;
    int  i, flags, first, last, index, status, firstIsEnd;
    int  table;			/* upvar? */
    TypeAttribute type;
    Tcl_C_List  *list, *listNew;

    if (argc != 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " list first last\"", (char *) NULL);
	return TCL_ERROR;
    }
    if ((*argv[2] == 'e') && (strncmp(argv[2], "end", strlen(argv[2])) == 0)) {
	firstIsEnd = 1;
	first = INT_MAX;
    } else {
	firstIsEnd = 0;
	if (Tcl_GetInt(interp, argv[2], &first) != TCL_OK) {
	    return TCL_ERROR;
	}
    }
    if ((*argv[3] == 'e') && (strncmp(argv[3], "end", strlen(argv[3])) == 0)) {
	last = INT_MAX;
    } else {
	if (Tcl_GetInt(interp, argv[3], &last) != TCL_OK) {
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, "expected integer or \"end\" but got \"",
			     argv[3], "\"", (char *) NULL);
	    return TCL_ERROR;
	}
    }
    
    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);
    
    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "linsert_v",
			  CRT_PART2, &arrayPtr);

    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	status = TCL_ERROR;
	goto error_exit;
    }

    /* invoke traces ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "linsert_v",
			      TCL_TRACE_READS);
	
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (status != TCL_OK) goto error_exit;
    
    /* convert variable to a list ... */

    type = varPtr->varAttr.typeAttr;
    if ((type & TCL_TYPE_LIST) || !(type & TCL_TYPE_STRINGY)) {

	if (!(type & TCL_TYPE_LIST))
	    Tcl_ConvertVar(interp, varPtr, TCL_TYPE_LIST);
	    
	list = &varPtr->value.allValue.list;
    
	if (first < 0) first = 0;
	if (firstIsEnd) first = list->nElem-1;
    
	if (first > last || first >= list->nElem) return TCL_OK;
	if (last >= list->nElem) last = list->nElem-1;
    
	/* Delete whatever is in the result buffer now ... */
    
	TVAR_RESET(&iPtr->results);
	TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | TCL_TYPE_LIST);
	listNew = &(iPtr->results.allValue.list);
 
	/* Make a new list from the first to last indices ... */
    
	Tcl_AppendList(listNew, list, first, last-first+1);
    }
    else {
	char *arg0 = argv[0];
	char *arg1 = argv[1];

	/* Just use the linsert with the string value of the variable ... */
	    
	argv[0] = "lrange";
	if (type & TCL_TYPE_STRING)
	    argv[1] = varPtr->value.allValue.string;
	else if (type & TCL_TYPE_DSTRING)
	    argv[1] = varPtr->value.allValue.dString.string;

	status = Tcl_LrangeCmd(dummy, interp, argc, argv);

	/* Restore the arguments to be safe and return ... */

	argv[0] = arg0;
	argv[1] = arg1;
    }
  error_exit:
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LreplaceCmd_v --
 *
 *	This procedure is invoked to process the "lreplace_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_LreplaceCmd_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString;
    Var  *varPtr, *arrayPtr;
    int  i, flags, first, last, index, status, firstIsEnd;
    int  table;			/* upvar? */
    Tcl_C_List  *list, *listNew;
    TypeAttribute type;

    if (argc < 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " list first last ?element element ...?\"",
			 (char *) NULL);
	return TCL_ERROR;
    }
    if ((*argv[2] == 'e') && (strncmp(argv[2], "end", strlen(argv[2])) == 0)) {
	firstIsEnd = 1;
	first = INT_MAX;
    } else {
	firstIsEnd = 0;
	if (Tcl_GetInt(interp, argv[2], &first) != TCL_OK) {
	    return TCL_ERROR;
	}
    }
    if ((*argv[3] == 'e') && (strncmp(argv[3], "end", strlen(argv[3])) == 0)) {
	last = INT_MAX;
    } else {
	if (TclGetListIndex(interp, argv[3], &last) != TCL_OK) {
	    return TCL_ERROR;
	}
    }
    
    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "linsert_v",
			  CRT_PART2, &arrayPtr);
    
    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	status = TCL_ERROR;
	goto error_exit;
    }
    status = TCL_OK;

    /* invoke traces ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "linsert_v",
			      TCL_TRACE_READS);
	
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (status != TCL_OK) goto error_exit;
    
    /* convert variable to a list ... */

    type = varPtr->varAttr.typeAttr;
    
    /* iff type is a list, process as a list, otherwise as a string ... */
    
    if (type == TCL_TYPE_LIST) {

	list = &varPtr->value.allValue.list;
    
	if (first < 0) first = 0;
	if (last < 0)  last  = 0;
    
	if (firstIsEnd)           first = list->nElem-1;
	if (last >= list->nElem)  last  = list->nElem-1;

	if (first >= list->nElem) {
	    Tcl_AppendResult(interp, "list doesn't contain element ",
			     argv[2], (char *) NULL);
	    return TCL_ERROR;
	}
	if (first > last) {
	    Tcl_AppendResult(interp,
			     "first index must not be greater than second",
			     (char *) NULL);
	    return TCL_ERROR;
	}
    
	/* Because first <= last, if first >=0, last must be too.  Notice
	   that if the list if empty, then first and/or last could be < 0
	   ... */
    
	/* Delete whatever is in the result buffer now ... */
    
	TVAR_RESET(&iPtr->results);
	TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | TCL_TYPE_LIST);
	listNew = &(iPtr->results.allValue.list);
	
	Tcl_AppendList(listNew, list, 0, first);

	for (i = 4; i < argc; i++) {
	    status = Tcl_ListAppendElement(interp, listNew, argv[i], NULL);
	    if (status != TCL_OK) goto error_exit;
	}

	/*
	 * Append the remainder of the original list.
	 */

	Tcl_AppendList(listNew, list, last+1, list->nElem-last-1);
    }
    else {
	char *arg0 = argv[0];
	char *arg1 = argv[1];

	/* Just use the linsert with the string value of the variable ... */
	    
	argv[0] = "lreplace";
	if (type & TCL_TYPE_STRING)
	    argv[1] = varPtr->value.allValue.string;
	else if (type & TCL_TYPE_DSTRING)
	    argv[1] = varPtr->value.allValue.dString.string;
	else {
	    Tcl_ConvertVar(interp, varPtr, TCL_TYPE_DSTRING);
	    argv[1] = varPtr->value.allValue.dString.string;
	}

	status = Tcl_LreplaceCmd(dummy, interp, argc, argv);

	/* Restore the arguments to be safe and return ... */

	argv[0] = arg0;
	argv[1] = arg1;
    }
  error_exit:
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LreplaceCmdl_v --
 *
 *	This procedure is invoked to process the "lreplace_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_LreplaceCmdl_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *joinString;
    Var  *varPtr, *arrayPtr;
    int  i, flags, first, last, index, status, firstIsEnd;
    int  table;			/* upvar? */
    Tcl_C_List  *list, *listNew;
    TypeAttribute type;

    if (argc < 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " list first last ?element element ...?\"",
			 (char *) NULL);
	return TCL_ERROR;
    }
    if ((*argv[2] == 'e') && (strncmp(argv[2], "end", strlen(argv[2])) == 0)) {
	firstIsEnd = 1;
	first = INT_MAX;
    } else {
	firstIsEnd = 0;
	if (Tcl_GetInt(interp, argv[2], &first) != TCL_OK) {
	    return TCL_ERROR;
	}
    }
    if ((*argv[3] == 'e') && (strncmp(argv[3], "end", strlen(argv[3])) == 0)) {
	last = INT_MAX;
    } else {
	if (TclGetListIndex(interp, argv[3], &last) != TCL_OK) {
	    return TCL_ERROR;
	}
    }
    
    part1 = argv[1];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "linsert_v",
			  CRT_PART2, &arrayPtr);

    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	status = TCL_ERROR;
	goto error_exit;
    }
    status = TCL_OK;

    /* invoke traces ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "linsert_v",
			      TCL_TRACE_READS);
	
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (status != TCL_OK) goto error_exit;
    
    /* convert variable to a list ... */

    type = varPtr->varAttr.typeAttr;
    if ((type & TCL_TYPE_LIST) || !(type & TCL_TYPE_STRINGY)) {
	if (!(type & TCL_TYPE_LIST))
	    Tcl_ConvertVar(interp, varPtr, TCL_TYPE_LIST);

	list = &varPtr->value.allValue.list;
    
	if (first < 0) first = 0;
	if (last < 0)  last  = 0;
    
	if (firstIsEnd)           first = list->nElem-1;
	if (last >= list->nElem)  last  = list->nElem-1;

	if (first >= list->nElem) {
	    Tcl_AppendResult(interp, "list doesn't contain element ",
			     argv[2], (char *) NULL);
	    return TCL_ERROR;
	}
	if (first > last) {
	    Tcl_AppendResult(interp,
			     "first index must not be greater than second",
			     (char *) NULL);
	    return TCL_ERROR;
	}
    
	/* Because first <= last, if first >=0, last must be too.  Notice
	   that if the list if empty, then first and/or last could be < 0
	   ... */
    
	/* Delete whatever is in the result buffer now ... */
    
	TVAR_RESET(&iPtr->results);
	TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | TCL_TYPE_LIST);
	listNew = &(iPtr->results.allValue.list);
	
	Tcl_AppendList(listNew, list, 0, first);

	for (i = 4; i < argc; i++) {
	    status = Tcl_ListAppendElement(interp, listNew, argv[i], NULL);
	    if (status != TCL_OK) goto error_exit;
	}

	/*
	 * Append the remainder of the original list.
	 */

	Tcl_AppendList(listNew, list, last+1, list->nElem-last-1);
    }
    else {
	char *arg0 = argv[0];
	char *arg1 = argv[1];

	/* Just use the linsert with the string value of the variable ... */
	    
	argv[0] = "lreplace";
	if (type & TCL_TYPE_STRING)
	    argv[1] = varPtr->value.allValue.string;
	else if (type & TCL_TYPE_DSTRING)
	    argv[1] = varPtr->value.allValue.dString.string;

	status = Tcl_LreplaceCmd(dummy, interp, argc, argv);

	/* Restore the arguments to be safe and return ... */

	argv[0] = arg0;
	argv[1] = arg1;
    }
  error_exit:
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LsearchCmd_v --
 *
 *	This procedure is invoked to process the "lsearch_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcl_LsearchCmd_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
#define EXACT	0
#define GLOB	1
#define REGEXP	2
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *baseStr;
    Var  *varPtr, *arrayPtr;
    int  i, flags, status, index, mode, match;
    int  table;			/* upvar? */
    Tcl_C_List  *list;

    mode = GLOB;
    
    index = 1;
    if (argc == 4) {
	index = 2;
	if (strcmp(argv[1], "-exact") == 0) {
	    mode = EXACT;
	} else if (strcmp(argv[1], "-glob") == 0) {
	    mode = GLOB;
	} else if (strcmp(argv[1], "-regexp") == 0) {
	    mode = REGEXP;
	} else {
	    Tcl_AppendResult(interp, "bad search mode \"", argv[1],
			     "\": must be -exact, -glob, or -regexp",
			     (char *) NULL);
	    return TCL_ERROR;
	}
    } else if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " ?mode? list pattern\"", (char *) NULL);
	return TCL_ERROR;
    }
    
    part1 = argv[index];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "linsert_v",
			  CRT_PART2, &arrayPtr);

    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	status = TCL_ERROR;
	goto error_exit;
    }

    /* invoke traces ... */

    status = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "linsert_v",
			      TCL_TRACE_READS);
	
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (status != TCL_OK) goto error_exit;
    
    /* convert variable to a list ... */

    if (!(varPtr->varAttr.typeAttr & TCL_TYPE_LIST)) {
	status = Tcl_ConvertToList(interp, varPtr);
	if (status != TCL_OK) goto error_exit;
    }
    list = &varPtr->value.allValue.list;
    baseStr = list->list.string;
    
    index = -1;
    for (i = 0; i < list->nElem; i++) {
	match = 0;
	switch (mode) {
	  case EXACT:
	    match = (strcmp(baseStr+list->argv[i], argv[argc-1]) == 0);
	    break;
	  case GLOB:
	    match = Tcl_StringMatch(baseStr+list->argv[i], argv[argc-1]);
	    break;
	  case REGEXP:
	    match = Tcl_RegExpMatch(interp, baseStr+list->argv[i],
				    argv[argc-1]);
	    if (match < 0) {
		status = TCL_ERROR;
		goto error_exit;
	    }
	    break;
	}
	if (match) {
	    index = i;
	    break;
	}
    }
    
    TVAR_RESET(&iPtr->results);
    TVAR_CREATE(iPtr->results, TCL_TYPE_RETOVR | TCL_TYPE_INTEGER);
    iPtr->results.allValue.integer = index;
  error_exit:
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LsortCmd_v --
 *
 *	This procedure is invoked to process the "lsort_v" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

#define STATIC_LIST_INDEX 20
typedef struct ListIndex {
    char *string;
    int  oldIndex;
    int  alloc;
} ListIndex_ts;

/* ARGSUSED */
int
Tcl_LsortCmd_v(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    char *openp, *closep, *part1, *part2;
    char *baseStr;
    Var  *varPtr, *arrayPtr;
    int  i, flags, index, status, c;
    int  table, deleteIndices;	/* upvar? */
    Tcl_C_List  *list, *listNew;
    Tcl_C_List  listLocal;
    
    ListIndex_ts *static_list_index[STATIC_LIST_INDEX];
    ListIndex_ts listIndices[STATIC_LIST_INDEX];
    
    ListIndex_ts **indexTable;

    size_t length;
    char *command = NULL;	/* Initialization needed only to
				 * prevent compiler warning. */

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			 " ?-ascii? ?-integer? ?-real? ?-increasing? ",
			 "?-decreasing? ?-command string? list\"",
			 (char *) NULL);
	return TCL_ERROR;
    }

    if (sortInterp != NULL) {
	interp->result = "can't invoke \"lsort\" recursively";
	return TCL_ERROR;
    }

    /*
     * Parse arguments to set up the mode for the sort.
     */

    sortInterp = interp;
    sortMode = ASCII;
    sortIncreasing = 1;
    sortCode = TCL_OK;
    
    for (i = 1; i < argc-1; i++) {
	length = strlen(argv[i]);
	if (length < 2) {
	  badSwitch:
	    Tcl_AppendResult(interp, "bad switch \"", argv[i],
			     "\": must be -ascii, -integer, -real, ",
			     "-increasing -decreasing, or -command",
			     (char *) NULL);
	    sortCode = TCL_ERROR;
	    goto error_exit;
	}
	c = argv[i][1];
	if ((c == 'a') && (strncmp(argv[i], "-ascii", length) == 0)) {
	    sortMode = ASCII;
	} else if ((c == 'c') && (strncmp(argv[i], "-command", length) == 0)) {
	    if (i == argc-2) {
		Tcl_AppendResult(interp, "\"-command\" must be",
				 " followed by comparison command",
				 (char *) NULL);
		sortCode = TCL_ERROR;
		goto error_exit;
	    }
	    sortMode = COMMAND;
	    command = argv[i+1];
	    i++;
	} else if ((c == 'd')
		   && (strncmp(argv[i], "-decreasing", length) == 0)) {
	    sortIncreasing = 0;
	} else if ((c == 'i') && (length >= 4)
		   && (strncmp(argv[i], "-increasing", length) == 0)) {
	    sortIncreasing = 1;
	} else if ((c == 'i') && (length >= 4)
		   && (strncmp(argv[i], "-integer", length) == 0)) {
	    sortMode = INTEGER;
	} else if ((c == 'r')
		   && (strncmp(argv[i], "-real", length) == 0)) {
	    sortMode = REAL;
	} else {
	    goto badSwitch;
	}
    }
    if (sortMode == COMMAND) {
	Tcl_DStringInit(&sortCmd);
	Tcl_DStringAppend(&sortCmd, command, -1);
    }

    index = argc-1;
    part1 = argv[index];
    Tcl_SplitArray(part1, &openp, &closep);

    if (openp) part2 = openp+1;
    else      part2 = NULL;

    varPtr = TclLookupVar(interp, part1, part2, TCL_LEAVE_ERR_MSG, "linsert_v",
			  CRT_PART2, &arrayPtr);

    if (varPtr == NULL) {

	/* clean up.  Have to wait after trace call if we are ok ... */

	if (openp) {
	    *openp = '('; *closep = ')';
	}
	sortCode = TCL_ERROR;
	goto error_exit;
    }

    /* invoke traces ... */

    sortCode = Tcl_InvokeTraces(interp, varPtr, part1, part2, 0, "linsert_v",
				TCL_TRACE_READS);
	
    if (openp) {
	*openp = '('; *closep = ')';
    }
    if (sortCode != TCL_OK) goto error_exit;
    
    /* convert variable to a list ... */

    if (!(varPtr->varAttr.typeAttr & TCL_TYPE_LIST)) {
	status = Tcl_ConvertToList(interp, varPtr);
	if (status != TCL_OK) {
	  sortCode = TCL_ERROR;
	  goto error_exit;
	}
    }
    list    = &varPtr->value.allValue.list;
    baseStr = list->list.string;

    /* Delete whatever is in the result buffer now ... */
    
    listNew = &listLocal;
    Tcl_ListInit(listNew);
   
    /* Make table of indices to sort ... */
    
    if (list->nElem > STATIC_LIST_INDEX) {
	indexTable = (ListIndex_ts **)
	    ckalloc(list->nElem*sizeof(ListIndex_ts *));
	
	for (i = STATIC_LIST_INDEX; i < list->nElem; i++) {
	    indexTable[i] = (ListIndex_ts *) ckalloc(sizeof(ListIndex_ts));
	    indexTable[i]->alloc = 1;
	}
	deleteIndices = 1;
    }
    else {
	indexTable = static_list_index;
	deleteIndices = 0;
    }
    
    /* Always use static listIndices for the first STATIC_LIST_INDEX
       entries ... */
    
    for (i = 0; i < ((list->nElem < STATIC_LIST_INDEX) ? list->nElem :
		     STATIC_LIST_INDEX); i++) {
	indexTable[i] = &listIndices[i];
	indexTable[i]->alloc = 0;
    }
    
    /* Make the list index fields ... */
    
    for (i = 0; i < list->nElem; i++) {
	indexTable[i]->string = baseStr+list->argv[i];
	indexTable[i]->oldIndex = i;
    }
    
    /* Sort the string then put in order of the new indices ... */
    
    qsort((VOID *) indexTable, (size_t) list->nElem, sizeof (char *),
	  SortListCompareProc);
  
    if (sortCode == TCL_OK) {
	int nextIndex;
	Tcl_ListAllocSize(listNew, list->nElem);
	listNew->list.length = 0;
	listNew->nElem = list->nElem;
	
	for (i = 0; i < list->nElem; i++) {
	    nextIndex = indexTable[i]->oldIndex;
	    
	    /* Update the pointer to the element ... */
	    
	    listNew->argv[i] = listNew->list.length;
	    Tcl_DStringAppend(&listNew->list, baseStr+list->argv[nextIndex],
			      -1);
	    Tcl_DStringAppend(&listNew->list, "\0", 1);
	    
	    /* Update the length and flag fields ... */
	    
	    listNew->len[i]   = list->len[nextIndex];
	    listNew->flags[i] = list->flags[nextIndex];
	}
    }
    if (deleteIndices) {
	for (i = 0; i < list->nElem; i++) {
	    if (indexTable[i]->alloc) ckfree((void *) indexTable[i]);
	}
	ckfree((void *) indexTable);
    }
	    
    if (sortCode == TCL_OK) {
	TVAR_RESET(&iPtr->results);
	TVAR_CREATE(iPtr->results, TCL_TYPE_LIST | TCL_TYPE_RETOVR);
	Tcl_ConcatList(&iPtr->results.allValue.list, listNew, NULL);
	Tcl_ListDelete(listNew);
    }
    
    if (sortMode == COMMAND) {
	Tcl_DStringFree(&sortCmd);
    }

  error_exit:
    sortInterp = NULL;
    return sortCode;
}

/*
 *----------------------------------------------------------------------
 *
 * SortListCompareProc --
 *
 *	This procedure is invoked by qsort to determine the proper
 *	ordering between two elements.
 *
 * Results:
 *	< 0 means first is "smaller" than "second", > 0 means "first"
 *	is larger than "second", and 0 means they should be treated
 *	as equal.
 *
 * Side effects:
 *	None, unless a user-defined comparison command does something
 *	weird.
 *
 *----------------------------------------------------------------------
 */

static int
SortListCompareProc(first, second)
CONST VOID *first, *second;		/* Elements to be compared. */
{
    int order;
    
    ListIndex_ts *fi = *((ListIndex_ts **) first);
    ListIndex_ts *si = *((ListIndex_ts **) second);
    
    char *firstString  = fi->string;
    char *secondString = si->string;

    order = 0;
    if (sortCode != TCL_OK) {
	/*
	 * Once an error has occurred, skip any future comparisons
	 * so as to preserve the error message in sortInterp->result.
	 */

	return order;
    }
    if (sortMode == ASCII) {
	order = strcmp(firstString, secondString);
    } else if (sortMode == INTEGER) {
	int a, b;

	if ((Tcl_GetInt(sortInterp, firstString, &a) != TCL_OK)
	    || (Tcl_GetInt(sortInterp, secondString, &b) != TCL_OK)) {
	    Tcl_AddErrorInfo(sortInterp,
			     "\n    (converting list element from string to integer)");
	    sortCode = TCL_ERROR;
	    return order;
	}
	if (a > b) {
	    order = 1;
	} else if (b > a) {
	    order = -1;
	}
    } else if (sortMode == REAL) {
	double a, b;

	if ((Tcl_GetDouble(sortInterp, firstString, &a) != TCL_OK)
	    || (Tcl_GetDouble(sortInterp, secondString, &b) != TCL_OK)) {
	    Tcl_AddErrorInfo(sortInterp,
			     "\n    (converting list element from string to real)");
	    sortCode = TCL_ERROR;
	    return order;
	}
	if (a > b) {
	    order = 1;
	} else if (b > a) {
	    order = -1;
	}
    } else {
	int oldLength;
	char *end;

	/*
	 * Generate and evaluate a command to determine which string comes
	 * first.
	 */

	oldLength = Tcl_DStringLength(&sortCmd);
	Tcl_DStringAppendElement(&sortCmd, firstString);
	Tcl_DStringAppendElement(&sortCmd, secondString);
	sortCode = Tcl_Eval(sortInterp, Tcl_DStringValue(&sortCmd));
	Tcl_DStringTrunc(&sortCmd, oldLength);
	if (sortCode != TCL_OK) {
	    Tcl_AddErrorInfo(sortInterp,
			     "\n    (user-defined comparison command)");
	    return order;
	}

	/*
	 * Parse the result of the command.
	 */
	
	if (((Interp *) sortInterp)->results.typeAttr & TCL_TYPE_RETOVR)
	    Tcl_OvrLoadResult(sortInterp);
	
	order = strtol(sortInterp->result, &end, 0);
	if ((end == sortInterp->result) || (*end != 0)) {
	    Tcl_ResetResult(sortInterp);
	    Tcl_AppendResult(sortInterp,
			     "comparison command returned non-numeric result",
			     (char *) NULL);
	    sortCode = TCL_ERROR;
	    return order;
	}
    }
    if (!sortIncreasing) {
	order = -order;
    }
    return order;
}
