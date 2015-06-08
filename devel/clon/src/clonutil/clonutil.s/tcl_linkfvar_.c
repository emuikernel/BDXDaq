/* 
 * tcl_linkfvar_.c 
 *
 *	This file implements linked variables (a fortran variable that is
 *	tied to a Tcl variable).  The idea of linked variables was
 *	first suggested by Andreas Stolcke and this implementation is
 *	based heavily on a prototype implementation provided by
 *	him.
 *
 * Copyright (c) 1993 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 *
 *  Adapted for fortran by EJW, 21-oct-96
 *
 */

#define TCL_LINK_LOGICAL   5
#define TCL_LINK_REAL      6
#define TCL_LINK_CHARACTER 7



#ifndef lint
static char sccsid[] = "@(#) tclLink.c 1.9 94/12/17 16:14:21";
#endif /* not lint */

#include "tclInt.h"

/*
 * For each linked variable there is a data structure of the following
 * type, which describes the link and is the clientData for the trace
 * set on the Tcl variable.
 */

typedef struct Link {
    Tcl_Interp *interp;		/* Interpreter containing Tcl variable. */
    char *varName;		/* Name of variable (must be global).  This
				 * is needed during trace callbacks, since
				 * the actual variable may be aliased at
				 * that time via upvar. */
    char *addr;			/* Location of fortran variable. */
    int type;			/* Type of link (TCL_LINK_INT, etc.). */
    int length;			/* length of fortran character variable */
    int writable;		/* Zero means Tcl variable is read-only. */
    union {
	int i;
	float f;
	double d;
        char *c;
    } lastValue;		/* Last known value of fortran variable;  used to
				 * avoid string conversions and to hold c-compatible
				 * copy of fortran character variables. */
} Link;

/*
 * Forward references to procedures defined later in this file:
 */

static char *		LinkTraceProc _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, char *name1, char *name2,
			    int flags));
static char *		StringValue _ANSI_ARGS_((Link *linkPtr,
			    char *buffer));
char *strdupf(char *fname, int flen);
void strc2f(char *cstring, char *fstring, int flen);


/*
 *----------------------------------------------------------------------
 *
 * tcl_linkfvar_ --
 *
 *	Link a fortran variable to a Tcl variable so that changes to either
 *	one causes the other to change.
 *
 * Results:
 *	The return value is TCL_OK if everything went well or TCL_ERROR
 *	if an error occurred (interp->result is also set after errors).
 *
 * Side effects:
 *	The value at *addr is linked to the Tcl variable fname (varName, to c),
 *	using "type" to convert between string values for Tcl and
 *	binary values for *addr.
 *
 *----------------------------------------------------------------------
 */

int
tcl_linkfvar_(finterp, fname, addr, type, namelen, charlen)
    int *finterp;               /* interpreter stored in fortran integer */
    char *fname; 		/* Fortran name of a global variable in interp. */
    char *addr;			/* Address of a fortran variable to be linked
				 * to varName. */
    int *type;	        	/* Type of fortran  variable: TCL_LINK_INT, etc. 
				 * Also may have TCL_LINK_READ_ONLY
				 * OR'ed in. */
    int namelen;                /* length of fortran fname */
    int charlen;                /* length of fortran character variable */

{
    Tcl_Interp *interp = (Tcl_Interp *) *finterp;
    Link *linkPtr;
    char buffer[TCL_DOUBLE_SPACE];
    int code;
    char *varName = strdupf(fname,namelen);


    linkPtr = (Link *) ckalloc(sizeof(Link));
    linkPtr->interp = interp;
    linkPtr->varName = ckalloc((unsigned) (strlen(varName) + 1));
    strcpy(linkPtr->varName, varName);
    linkPtr->addr = addr;
    linkPtr->type = *type & ~TCL_LINK_READ_ONLY;
    linkPtr->writable = (*type & TCL_LINK_READ_ONLY) == 0;
    if(linkPtr->type==TCL_LINK_CHARACTER){
      linkPtr->length = charlen;
      linkPtr->lastValue.c=NULL;
    } else {
      linkPtr->length = 0;
    }
    if (Tcl_SetVar(interp, varName, StringValue(linkPtr, buffer),
	    TCL_LEAVE_ERR_MSG) == NULL) {
	if(linkPtr->type==TCL_LINK_CHARACTER)free(linkPtr->lastValue.c);
	ckfree(linkPtr->varName);
	ckfree((char *) linkPtr);
	free(varName);
	return TCL_ERROR;
    }
    code = Tcl_TraceVar(interp, varName, TCL_TRACE_READS
	    |TCL_TRACE_WRITES|TCL_TRACE_UNSETS, LinkTraceProc,
	    (ClientData) linkPtr);
    if (code != TCL_OK) {
	if(linkPtr->type==TCL_LINK_CHARACTER)free(linkPtr->lastValue.c);
	ckfree(linkPtr->varName);
	ckfree((char *) linkPtr);
    }
    free(varName);
    return code;
}

/*
 *----------------------------------------------------------------------
 *
 * tcl_unlinkvar_
 *
 *	Destroy the link between a Tcl variable and a fortran variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If "varName" was previously linked to a fortran variable, the link
 *	is broken to make the variable independent.  If there was no
 *	previous link for "varName" then nothing happens.
 *
 *----------------------------------------------------------------------
 */

void
tcl_unlinkfvar_(finterp, fname, flen)
    int *finterp;               /* interp stored in fortran variable */
    char *fname;		/* Global variable in interp to unlink. */
    int flen;                   /* length of variable fname */
{
    Tcl_Interp *interp = (Tcl_Interp *) *finterp;
    Link *linkPtr;
    char *varName = strdupf(fname,flen);

    linkPtr = (Link *) Tcl_VarTraceInfo(interp, varName, ITCL_GLOBAL_VAR,
	    LinkTraceProc, (ClientData) NULL);
    if (linkPtr == NULL) {
        free(varName);
	return;
    }
    Tcl_UntraceVar(interp, varName,
	    TCL_TRACE_READS|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
	    LinkTraceProc, (ClientData) linkPtr);
    if(linkPtr->type==TCL_LINK_CHARACTER)free(linkPtr->lastValue.c);
    ckfree(linkPtr->varName);
    ckfree((char *) linkPtr);

    free(varName);
}

/*
 *----------------------------------------------------------------------
 *
 * LinkTraceProc --
 *
 *	This procedure is invoked when a linked Tcl variable is read,
 *	written, or unset from Tcl.  It's responsible for keeping the
 *	fortran variable in sync with the Tcl variable.
 *
 * Results:
 *	If all goes well, NULL is returned; otherwise an error message
 *	is returned.
 *
 * Side effects:
 *	The fortran variable may be updated to make it consistent with the
 *	Tcl variable, or the Tcl variable may be overwritten to reject
 *	a modification.
 *
 *----------------------------------------------------------------------
 */

static char *
LinkTraceProc(clientData, interp, name1, name2, flags)
    ClientData clientData;	/* Contains information about the link. */
    Tcl_Interp *interp;		/* Interpreter containing Tcl variable. */
    char *name1;		/* First part of variable name. */
    char *name2;		/* Second part of variable name. */
    int flags;			/* Miscellaneous additional information. */
{
    Link *linkPtr = (Link *) clientData;
    int changed;
    char buffer[TCL_DOUBLE_SPACE];
    char *value, **pp;
    Tcl_DString savedResult;

    /*
     * If the variable is being unset, then just re-create it (with a
     * trace) unless the whole interpreter is going away.
     */

    if (flags & TCL_TRACE_UNSETS) {
	if (flags & TCL_INTERP_DESTROYED) {
	    if(linkPtr->type==TCL_LINK_CHARACTER)free(linkPtr->lastValue.c);
	    ckfree(linkPtr->varName);
	    ckfree((char *) linkPtr);
	} else if (flags & TCL_TRACE_DESTROYED) {
	    Tcl_SetVar(interp, linkPtr->varName, StringValue(linkPtr, buffer),
		    0);
	    Tcl_TraceVar(interp, linkPtr->varName, 
			 TCL_TRACE_READS|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		    LinkTraceProc, (ClientData) linkPtr);
	}
	return NULL;
    }

    /*
     * For read accesses, update the Tcl variable if the fortran variable
     * has changed since the last time we updated the Tcl variable.
     */

    if (flags & TCL_TRACE_READS) {
	switch (linkPtr->type) {
	    case TCL_LINK_INT:
	    case TCL_LINK_LOGICAL:
	    case TCL_LINK_BOOLEAN:
		changed = *(int *)(linkPtr->addr) != linkPtr->lastValue.i;
		break;
	    case TCL_LINK_REAL:
		changed = *(float *)(linkPtr->addr) != linkPtr->lastValue.f;
		break;
	    case TCL_LINK_DOUBLE:
		changed = *(double *)(linkPtr->addr) != linkPtr->lastValue.d;
		break;
	    case TCL_LINK_STRING:
	    case TCL_LINK_CHARACTER:
		changed = 1;
		break;
	    default:
		return "internal error: bad linked variable type";
	}
	if (changed) {
	    Tcl_SetVar(interp, linkPtr->varName, StringValue(linkPtr, buffer),
		    0);
	}
	return NULL;
    }

    /*
     * For writes, first make sure that the variable is writable.  Then
     * convert the Tcl value to fortran if possible.  If the variable isn't
     * writable or can't be converted, then restore the varaible's old
     * value and return an error.  Another tricky thing: we have to save
     * and restore the interpreter's result, since the variable access
     * could occur when the result has been partially set.
     */

    if (!linkPtr->writable) {
	Tcl_SetVar(interp, linkPtr->varName, StringValue(linkPtr, buffer),
		0);
	return "linked variable is read-only";
    }
    value = Tcl_GetVar(interp, linkPtr->varName, 0);
    if (value == NULL) {
	/*
	 * This shouldn't ever happen.
	 */
	return "internal error: linked variable couldn't be read";
    }
    Tcl_DStringInit(&savedResult);
    Tcl_DStringAppend(&savedResult, interp->result, -1);
    Tcl_ResetResult(interp);

    switch (linkPtr->type) {
	case TCL_LINK_INT:
	    if (Tcl_GetInt(interp, value, &linkPtr->lastValue.i) != TCL_OK) {
		Tcl_DStringResult(interp, &savedResult);
		Tcl_SetVar(interp, linkPtr->varName,
			StringValue(linkPtr, buffer), 0);
		return "variable must have integer value";
	    }
	    *(int *)(linkPtr->addr) = linkPtr->lastValue.i;
	    break;
	case TCL_LINK_REAL:
	    if (Tcl_GetDouble(interp, value, &linkPtr->lastValue.d)
		    != TCL_OK) {
	        linkPtr->lastValue.f=(float)linkPtr->lastValue.d;
		Tcl_DStringResult(interp, &savedResult);
		Tcl_SetVar(interp, linkPtr->varName,
			StringValue(linkPtr, buffer), 0);
		return "variable must have real value";
	    }
	    *(float *)(linkPtr->addr) = linkPtr->lastValue.f;
	    break;
	case TCL_LINK_DOUBLE:
	    if (Tcl_GetDouble(interp, value, &linkPtr->lastValue.d)
		    != TCL_OK) {
		Tcl_DStringResult(interp, &savedResult);
		Tcl_SetVar(interp, linkPtr->varName,
			StringValue(linkPtr, buffer), 0);
		return "variable must have real value";
	    }
	    *(double *)(linkPtr->addr) = linkPtr->lastValue.d;
	    break;
	case TCL_LINK_BOOLEAN:
	    if (Tcl_GetBoolean(interp, value, &linkPtr->lastValue.i)
		    != TCL_OK) {
		Tcl_DStringResult(interp, &savedResult);
		Tcl_SetVar(interp, linkPtr->varName,
			StringValue(linkPtr, buffer), 0);
		return "variable must have boolean value";
	    }
	    *(int *)(linkPtr->addr) = linkPtr->lastValue.i;
	    break;
	case TCL_LINK_LOGICAL:
	    if (Tcl_GetLogical(interp, value, &linkPtr->lastValue.i)
		    != TCL_OK) {
		Tcl_DStringResult(interp, &savedResult);
		Tcl_SetVar(interp, linkPtr->varName,
			StringValue(linkPtr, buffer), 0);
		return "variable must have logical value";
	    }
	    *(int *)(linkPtr->addr) = linkPtr->lastValue.i;
	    break;
	case TCL_LINK_STRING:
	    pp = (char **)(linkPtr->addr);
	    if (*pp != NULL) {
		ckfree(*pp);
	    }
	    *pp = ckalloc((unsigned) (strlen(value) + 1));
	    strcpy(*pp, value);
	    break;
	case TCL_LINK_CHARACTER:
	    strc2f(linkPtr->addr,value,linkPtr->length);
	    break;
	default:
	    return "internal error: bad linked variable type";
    }
    Tcl_DStringResult(interp, &savedResult);
    return NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * StringValue --
 *
 *	Converts the value of a fortran variable to a string for use in a
 *	Tcl variable to which it is linked.
 *
 * Results:
 *	The return value is a pointer
 to a string that represents
 *	the value of the fortran variable given by linkPtr.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
StringValue(linkPtr, buffer)
    Link *linkPtr;		/* Structure describing linked variable. */
    char *buffer;		/* Small buffer to use for converting
				 * values.  Must have TCL_DOUBLE_SPACE
				 * bytes or more. */
{
    char *p;

    switch (linkPtr->type) {
	case TCL_LINK_INT:
	    linkPtr->lastValue.i = *(int *)(linkPtr->addr);
	    sprintf(buffer, "%d", linkPtr->lastValue.i);
	    return buffer;
	case TCL_LINK_REAL:
	    linkPtr->lastValue.f = *(float *)(linkPtr->addr);
	    Tcl_PrintDouble(linkPtr->interp, (double)linkPtr->lastValue.f, buffer);
	    return buffer;
	case TCL_LINK_DOUBLE:
	    linkPtr->lastValue.d = *(double *)(linkPtr->addr);
	    Tcl_PrintDouble(linkPtr->interp, linkPtr->lastValue.d, buffer);
	    return buffer;
	case TCL_LINK_BOOLEAN:
	    linkPtr->lastValue.i = *(int *)(linkPtr->addr);
	    if (linkPtr->lastValue.i != 0) {
		return "1";
	    }
	    return "0";
	case TCL_LINK_LOGICAL:
	    linkPtr->lastValue.i = *(int *)(linkPtr->addr);
	    if (linkPtr->lastValue.i != 0) {
		return "-1";
	    }
	    return "0";
	case TCL_LINK_STRING:
	    p = *(char **)(linkPtr->addr);
	    if (p == NULL) {
		return "NULL";
	    }
	    return p;
	case TCL_LINK_CHARACTER:
	    if(linkPtr->lastValue.c!=NULL)free(linkPtr->lastValue.c);
	    linkPtr->lastValue.c=strdupf(linkPtr->addr,linkPtr->length);
	    return linkPtr->lastValue.c;
    }

    /*
     * This code only gets executed if the link type is unknown
     * (shouldn't ever happen).
     */

    return "??";
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetLogical --
 *
 *	Given a string, return a 0/-1 logicalvalue corresponding
 *	to the string.
 *
 * Results:
 *	The return value is normally TCL_OK;  in this case *logiPtr
 *	will be set to the 0/-1 value equivalent to string.  If
 *	string is improperly formed then TCL_ERROR is returned and
 *	an error message will be left in interp->result.
 *
 * Side effects:
 *	None.
 *
 *  modified vsn of Tcl_GetBoolean for fortran logicals...ejw
 *
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetLogical(interp, string, logiPtr)
    Tcl_Interp *interp;		/* Interpreter to use for error reporting. */
    char *string;		/* String containing a logical number
				 * specified either as -1/0 or true/false or
				 * yes/no or on/off */
    int *logiPtr;		/* Place to store converted result, which
				 * will be 0 or -1 */
{
    int i, c;
    char lowerCase[10];
    size_t length;

    /*
     * Convert the input string to all lower-case.
     */

    for (i = 0; i < 9; i++) {
	c = string[i];
	if (c == 0) {
	    break;
	}
	if ((c >= 'A') && (c <= 'Z')) {
	    c += 'a' - 'A';
	}
	lowerCase[i] = c;
    }
    lowerCase[i] = 0;

    length = strlen(lowerCase);
    c = lowerCase[0];
    if ((c == '0') && (lowerCase[1] == '\0')) {
	*logiPtr = 0;
    } else if ((c == '-') && (lowerCase[1]  == '1') && (lowerCase[2] == '\0')) {
	*logiPtr = -1;
    } else if ((c == 'y') && (strncmp(lowerCase, "yes", length) == 0)) {
	*logiPtr = -1;
    } else if ((c == 'n') && (strncmp(lowerCase, "no", length) == 0)) {
	*logiPtr = 0;
    } else if ((c == 't') && (strncmp(lowerCase, "true", length) == 0)) {
	*logiPtr = -1;
    } else if ((c == 'f') && (strncmp(lowerCase, "false", length) == 0)) {
	*logiPtr = 0;
    } else if ((c == 'o') && (length >= 2)) {
	if (strncmp(lowerCase, "on", length) == 0) {
	    *logiPtr = -1;
	} else if (strncmp(lowerCase, "off", length) == 0) {
	    *logiPtr = 0;
	} else {
	    goto badLogical;
	}
    } else {
	badLogical:
	Tcl_AppendResult(interp, "expected logical value but got \"",
		string, "\"", (char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}
