/* 
 * tclVarUtil.c --
 *
 *	This file contains routines that implement Tcl variables
 *	(both scalars and arrays).
 *
 *	The implementation of arrays is modelled after an initial
 *	implementation by Mark Diekhans and Karl Lehenbauer.
 *
 * Copyright (c) 1987-1993 The Regents of the University of California.
 * Copyright (c) 1994-1995 ICEM CFD Inc.
 *
 */

#ifndef lint
#endif

#include "tclInt.h"

#ifdef NO_FLOAT_H
#   include "compat/float.h"
#else
#   include <float.h>
#endif
#ifndef TCL_NO_MATH
#include <math.h>
#endif

/*
 * The stuff below is a bit of a hack so that this file can be used
 * in environments that include no UNIX, i.e. no errno.  Just define
 * errno here.
 */

#ifndef TCL_GENERIC_ONLY
#include "tclPort.h"
extern int errno;
#else
#define NO_ERRNO_H
#endif

#ifdef NO_ERRNO_H
int errno;
#define EDOM 33
#define ERANGE 34
#endif

/* int __NO_CFOLD_WARNING; */

/*
 * Conversion procedure attribute structures
 */

static ConvAttribute TCL_FROMSTRING = {
(Tcl_ConvFunction *) NULL,
(Tcl_ConvFunction *) Tcl_ConvertFromStringToInteger,
(Tcl_ConvFunction *) Tcl_ConvertFromStringToDouble,
(Tcl_ConvFunction *) Tcl_ConvertFromStringToList,
(Tcl_ConvFunction *) Tcl_ConvertFromStringToDString
};

static ConvAttribute TCL_FROMINTEGER = {
(Tcl_ConvFunction *) Tcl_ConvertFromIntegerToString,
(Tcl_ConvFunction *) NULL,
(Tcl_ConvFunction *) Tcl_ConvertFromIntegerToDouble,
(Tcl_ConvFunction *) Tcl_ConvertFromIntegerToList,
(Tcl_ConvFunction *) Tcl_ConvertFromIntegerToDString
};

static ConvAttribute TCL_FROMDOUBLE = {
(Tcl_ConvFunction *) Tcl_ConvertFromDoubleToString,
(Tcl_ConvFunction *) Tcl_ConvertFromDoubleToInteger,
(Tcl_ConvFunction *) NULL,
(Tcl_ConvFunction *) Tcl_ConvertFromDoubleToList,
(Tcl_ConvFunction *) Tcl_ConvertFromDoubleToDString
};

static ConvAttribute TCL_FROMLIST = {
(Tcl_ConvFunction *) Tcl_ConvertFromListToString,
(Tcl_ConvFunction *) Tcl_ConvertFromListToInteger,
(Tcl_ConvFunction *) Tcl_ConvertFromListToDouble,
(Tcl_ConvFunction *) NULL,
(Tcl_ConvFunction *) Tcl_ConvertFromListToDString
};

static ConvAttribute TCL_FROMDSTRING = {
(Tcl_ConvFunction *) Tcl_ConvertFromDStringToString,
(Tcl_ConvFunction *) Tcl_ConvertFromDStringToInteger,
(Tcl_ConvFunction *) Tcl_ConvertFromDStringToDouble,
(Tcl_ConvFunction *) Tcl_ConvertFromDStringToList,
(Tcl_ConvFunction *) NULL
};

/*
 *	G L O B A L  U T I L I T Y  F U N C T I O N S  F O R
 *	V A R
 */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_MaintainValid
 *
 *	Maintain all valid fields.  Needed when refer to variable via
 *	upvar
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Writes updated value back into all currently valid fields
 *
 *----------------------------------------------------------------------
 */

int
Tcl_MaintainValid(interp, aVar)
    Tcl_Interp    *interp;	/* the current interpreter */
    Var           *aVar;	/* variable to return value for */
{
    TypeAttribute typeAttr   = aVar->varAttr.typeAttr;
    int           ret;

/*  ... Convert to all valid types ... */

    if (typeAttr & TCL_TYPE_STRING)  ret = Tcl_ConvertVar(interp, aVar,
							 TCL_TYPE_STRING);
    if (typeAttr & TCL_TYPE_INTEGER) ret = Tcl_ConvertVar(interp, aVar,
							 TCL_TYPE_INTEGER);
    if (typeAttr & TCL_TYPE_DOUBLE)  ret = Tcl_ConvertVar(interp, aVar,
							 TCL_TYPE_DOUBLE);
    if (typeAttr & TCL_TYPE_LIST)    ret = Tcl_ConvertVar(interp, aVar,
							 TCL_TYPE_LIST);
    if (typeAttr & TCL_TYPE_DSTRING) ret = Tcl_ConvertVar(interp, aVar,
							 TCL_TYPE_DSTRING);
    return ret;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_OnlyValidVar
 *
 *	Set that given type is only valid type.  Deallocate all
 *	invalid fields (if applicable)
 *
 * Tcl_AsValidVar
 *
 *	Set that given type is also now valid.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Writes updated value back into all currently valid fields
 *
 *----------------------------------------------------------------------
 */

void
Tcl_OnlyValidVar(aVar, curType)
    Var           *aVar;	/* variable pointer */
    TypeAttribute curType;	/* current valid type */
{
    Tcl_OnlyValidValueMACRO(&(aVar->value.allValue),
		            &(aVar->varAttr.typeAttr), curType);

    aVar->varAttr.convAttr = (ConvAttribute *) NULL;
    Tcl_AsValidVarMACRO(aVar, curType);
}

void
Tcl_OnlyValidValue(values, attrs, curType)
    AllValue      *values;	/* Current values */
    TypeAttribute *attrs;	/* type Attributes */
    TypeAttribute curType;	/* current valid type */
{
    Tcl_OnlyValidValueMACRO(values, attrs, curType);
}

void
Tcl_AsValidVar(aVar, curType)
    Var           *aVar;
    TypeAttribute curType;
{
    Tcl_AsValidVarMACRO(aVar, curType);
    return;
}

void
Tcl_ResetValue(values, attrs)
    AllValue      *values;	/* Current values */
    TypeAttribute *attrs;	/* type Attributes */
{
    Tcl_ResetValueMACRO(values, attrs);
}

/*
 * Conversion/value return routines
 */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ReturnConversion
 *
 *	Return the address to the conversion routines for this type
 *
 * Results:
 *	pointer to Conversion routines
 *
 * Side effects:
 *	None
 *
 *----------------------------------------------------------------------
 */

ConvAttribute *
Tcl_ReturnConversion(curType)
    TypeAttribute curType;	/* current valid type */
{
    ConvAttribute *ret;

/*  ... We come up with proper translator from valid TCL atomic type
	to all other TCL atomic types.  Note that if we have the
	allVal type, we still have a valid TCL atomic type.  This
	conversion block is valid for allVal types. ... */

    if      (curType & TCL_TYPE_INTEGER)	ret = &TCL_FROMINTEGER;
    else if (curType & TCL_TYPE_DOUBLE)		ret = &TCL_FROMDOUBLE;
    else if (curType & TCL_TYPE_LIST)		ret = &TCL_FROMLIST;
    else if (curType & TCL_TYPE_DSTRING)	ret = &TCL_FROMDSTRING;
    else if (curType & TCL_TYPE_STRING)		ret = &TCL_FROMSTRING;
    else					ret = NULL;

    return ret;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_StringValue
 *
 *	Return the string value of the variable.  If the string value
 *	is not valid, return NULL
 *
 * Tcl_NumericValue
 *
 *	Try and parse an integer first, then a double number
 *
 * Tcl_IntegerValue
 *
 *	Return the integer value of variable.  If the integer value
 *	is not valid, return 0
 *
 * Tcl_DoubleValue
 *
 *	Return the double value of variable.  If the double value
 *	is not valid, return 0.
 *
 * Tcl_ListValue
 *
 *	Return the list value of variable.  If the list value
 *	is not valid, return NULL
 *
 * Results:
 *	Value of field (as string, integer, double or list)
 *
 * Side effects:
 *	sets *valid to 0 (not valid) or 1 (valid)
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_StringValue(interp, aVar, status)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
    int        *status;	/* status of conversion (if applicable) */
{
    if (aVar->varAttr.typeAttr & TCL_TYPE_STRING)
	 *status = TCL_OK;
    else *status = Tcl_ConvertToString(interp, aVar);

    return aVar->value.allValue.string;
}

int
Tcl_NumericValue(interp, aVar)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
{
    TypeAttribute type;			/* type it converts to */

    return Tcl_ParseNumeric(interp, &aVar->value.allValue,
			    &aVar->varAttr.typeAttr, &type);
}

int
Tcl_IntegerValue(interp, aVar, status)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
    int        *status;	/* status of conversion */
{
    if (aVar->varAttr.typeAttr & TCL_TYPE_INTEGER)
	 *status = TCL_OK;
    else *status = Tcl_ConvertToInteger(interp, aVar);

    return aVar->value.allValue.integer;
}

double
Tcl_DoubleValue(interp, aVar, status)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
    int        *status;	/* status of conversion */
{
    if (aVar->varAttr.typeAttr & TCL_TYPE_DOUBLE)
	 *status = TCL_OK;
    else *status = Tcl_ConvertToDouble(interp, aVar);

    return aVar->value.allValue.doub;
}

Tcl_C_List *
Tcl_ListValue(interp, aVar, status)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
    int        *status;	/* status of conversion */
{
    if (aVar->varAttr.typeAttr & TCL_TYPE_LIST)
	 *status = TCL_OK;
    else *status = Tcl_ConvertToList(interp, aVar);

    return &aVar->value.allValue.list;
}

Tcl_DString *
Tcl_rDStringValue(interp, aVar, status)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
    int        *status;	/* status of conversion */
{
    if (aVar->varAttr.typeAttr & TCL_TYPE_DSTRING)
	 *status = TCL_OK;
    else *status = Tcl_ConvertToDString(interp, aVar);

    return &aVar->value.allValue.dString;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_StringResult
 *
 *	Return the string result of proc.  If the string value
 *	is not valid, return NULL
 *
 * Tcl_IntegerResult
 *
 *	Return the integer result of proc.  If the integer value
 *	is not valid, return 0
 *
 * Tcl_DoubleResult
 *
 *	Return the double result of proc.  If the double value
 *	is not valid, return 0.
 *
 * Tcl_ListResult
 *
 *	Return the list result of proc.  If the list value
 *	is not valid, return NULL
 *
 * Results:
 *	Result of field (as string, integer, double or list)
 *
 * Side effects:
 *	sets *valid to 0 (not valid) or 1 (valid)
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_rStringResult(iPtr)
    Interp        *iPtr;	/* variable to return value for */
{
    return iPtr->result;
/*
    return iPtr->results.string;
*/
}

int
Tcl_rIntegerResult(iPtr)
    Interp        *iPtr;	/* variable to return value for */
{
    return iPtr->results.allValue.integer;
}

double
Tcl_rDoubleResult(iPtr)
    Interp        *iPtr;	/* variable to return value for */
{
    return iPtr->results.allValue.doub;
}

Tcl_DString *
Tcl_rDStringResult(iPtr)
    Interp        *iPtr;	/* variable to return value for */
{
    return &iPtr->results.allValue.dString;
}

Tcl_C_List *
Tcl_rListResult(iPtr)
    Interp        *iPtr;	/* variable to return value for */
{
    return &iPtr->results.allValue.list;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConvertToString
 *
 *	Convert variable to string and mark as valid.  Return string as
 *	value.
 *
 * Tcl_ConvertToInteger
 *
 *	Convert variable to integer and mark as valid.  Return integer
 *	as value.
 *
 * Tcl_ConvertToDouble
 *
 *	Convert variable to double and mark as valid.  Return double as
 *	value.
 *
 * Tcl_ConvertToList
 *
 *	Convert variable to list and mark as valid.  Return list as
 *	value.
 *
 * Results:
 *	Value of field (as string, integer, double or list)
 *
 * Side effects:
 *	Sets the field in the AllValue structure to valid value
 *
 *----------------------------------------------------------------------
 */

int
Tcl_ConvertToString(interp, aVar)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
{
    return Tcl_ConvertVar(interp, aVar, TCL_TYPE_STRING);
}

int
Tcl_ConvertToInteger(interp, aVar)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
{
    return Tcl_ConvertVar(interp, aVar, TCL_TYPE_INTEGER);
}

int
Tcl_ConvertToDouble(interp, aVar)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
{
    return Tcl_ConvertVar(interp, aVar, TCL_TYPE_DOUBLE);
}

int
Tcl_ConvertToList(interp, aVar)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
{
    return Tcl_ConvertVar(interp, aVar, TCL_TYPE_LIST);
}

int
Tcl_ConvertToDString(interp, aVar)
    Tcl_Interp *interp; /* the current interpreter */
    Var        *aVar;	/* variable to return value for */
{
    return Tcl_ConvertVar(interp, aVar, TCL_TYPE_DSTRING);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConvertVar
 *
 *	Convert from a valid variable type to another
 *
 * Results:
 *	None
 *
 * Side effects:
 *	Sets affected field in AllValue structure
 *
 *----------------------------------------------------------------------
 */

int
Tcl_ConvertVar(interp, aVar, toType)
    Tcl_Interp    *interp;	/* the current interpreter */
    Var           *aVar;	/* variable to return value for */
    TypeAttribute toType;	/* type to convert to */
{
    Tcl_ConvFunction *convProc;
    int              oldType, ret = TCL_OK;

    if (!aVar) goto exit_p;

    oldType = aVar->varAttr.typeAttr & TCL_TYPE_BASIC;

    /* If we are converting to a currently valid type, we needn't
       bother ... */

    if ((oldType & toType)) goto exit_p;
    
    /* if we don't have a conversion routine, bug out ... */
    
    if (!aVar->varAttr.convAttr) {
	ret = TCL_ERROR;
	Tcl_AddErrorInfo(interp, "No Runtime type for variable!");
	goto exit_p;
    }

    switch (toType & TCL_TYPE_BASIC) {
	
	case TCL_TYPE_STRING:
	    convProc = aVar->varAttr.convAttr->string; break;
	case TCL_TYPE_INTEGER:
	    convProc = aVar->varAttr.convAttr->integer; break;
	case TCL_TYPE_DOUBLE:
	    convProc = aVar->varAttr.convAttr->doub; break;
	case TCL_TYPE_LIST:
	    convProc = aVar->varAttr.convAttr->list; break;
	case TCL_TYPE_DSTRING:
	    convProc = aVar->varAttr.convAttr->dString; break;

	default: convProc = (Tcl_ConvFunction *) NULL; break;
    }
    if (convProc) {
	ret = (*convProc)(interp,
		    &aVar->value.allValue,
		    &aVar->varAttr.typeAttr);
    }
    else ret = TCL_OK;

exit_p:
    return ret;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConvertFromStringToInteger
 *
 *	Convert variable from string to int.
 *
 * Tcl_ConvertFromStringToDouble
 *
 *	Convert variable from string to double
 *
 * Tcl_ConvertFromStringToList
 *
 *	Convert variable from string to list.
 *
 * Results:
 *	None
 *
 * Side effects:
 *	Sets the field in the AllValue structure to valid value
 *
 *----------------------------------------------------------------------
 */

int
Tcl_ConvertFromStringToInteger(Tcl_Interp *interp, AllValue *aValue,
			       TypeAttribute *typeAttr)
{
    char *p, *start, *term;
    int  status = TCL_ERROR;
    
    p = aValue->string;
    if (p) {
	if (*p == '-') {
	    start = p+1;
	    aValue->integer = -strtoul(start, &term, 0);
	}
	else if (*p == '+') {
	    start = p+1;
	    aValue->integer = strtoul(start, &term, 0);
	}
	else {
	    start = p;
	    aValue->integer = strtoul(p, &term, 0);
	}
	if ((term == start) || (*term == '.') || (*term == 'e') ||
		(*term == 'E') || (*term == '8') || (*term == '9')) {
	    status = Tcl_ConvertFromStringToDouble(interp, aValue, typeAttr);
	    if (status == TCL_OK) aValue->integer = aValue->doub;
	    else if (interp)
		Tcl_AppendResult(interp, "Illegal integer value ", p, NULL);
	}
	else status = TCL_OK;
    }
    else aValue->integer = 0;
    TCL_SETASVALID(typeAttr, TCL_TYPE_INTEGER);
    return status;
}

int
Tcl_ConvertFromStringToDouble(Tcl_Interp *interp, AllValue *aValue,
			      TypeAttribute *typeAttr)
{
    char *term;
    int  status = TCL_ERROR;

    if (aValue->string) {
	errno = 0;
	aValue->doub = strtod(aValue->string, &term);
        if ((term != aValue->string) && (*term == '\0')) {
	    if (errno == 0) status = TCL_OK;
	}
    }
    else aValue->doub = 0.;
    TCL_SETASVALID(typeAttr, TCL_TYPE_DOUBLE);
    return status;
}

int
Tcl_ConvertFromStringToList(Tcl_Interp *interp, AllValue *aValue,
			    TypeAttribute *typeAttr)
{
    int status = TCL_OK;

    if (!(*typeAttr & TCL_TYPE_LIST)) {
	status = Tcl_SplitToList(interp, &aValue->list,
				 aValue->string);
    	if (status == TCL_OK) *typeAttr |= TCL_TYPE_LIST;
    }
    return status;
}

int
Tcl_ConvertFromStringToDString(Tcl_Interp *interp, AllValue *aValue,
			       TypeAttribute *typeAttr)
{
    if (!(*typeAttr & TCL_TYPE_DSTRING)) {
    	aValue->dString.length = 0;
    	aValue->dString.string[0] = '\0';
    	if (aValue->string)
		Tcl_DStringAppend(&aValue->dString,
			  	  aValue->string,
			  	  strlen(aValue->string));
    
    	*typeAttr |= TCL_TYPE_DSTRING;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConvertFromDStringToString
 *
 *	Convert from a dynamic string to string
 *
 * Tcl_ConvertFromDStringToInteger
 *
 *	Convert variable from string to int.
 *
 * Tcl_ConvertFromDStringToDouble
 *
 *	Convert variable from string to double
 *
 * Tcl_ConvertFromDStringToList
 *
 *	Convert variable from string to list.
 *
 * Results:
 *	None
 *
 * Side effects:
 *	Sets the field in the AllValue structure to valid value
 *
 *----------------------------------------------------------------------
 */

int
Tcl_ConvertFromDStringToString(Tcl_Interp *interp, AllValue *aValue,
			       TypeAttribute *typeAttr)
{
    if (!(*typeAttr & TCL_TYPE_STRING)) {
    	int length = aValue->dString.length+1;
    	length = (length > 0) ? length : 1;
    
    	aValue->string = (char *) ckalloc(length*sizeof(char));
    	strncpy(aValue->string, aValue->dString.string, length-1);
    	aValue->string[length] = '\0';
    
    	*typeAttr |= TCL_TYPE_STRING | TCL_TYPE_FSTRING;
    }
    return TCL_OK;
}

int
Tcl_ConvertFromDStringToInteger(Tcl_Interp *interp, AllValue *aValue,
				TypeAttribute *typeAttr)
{
    char *p, *start, *term;
    int  status = TCL_ERROR;
    
    p = aValue->dString.string;
    if (p) {
	if (*p == '-') {
	    start = p+1;
	    aValue->integer = -strtoul(start, &term, 0);
	}
	else if (*p == '+') {
	    start = p+1;
	    aValue->integer = strtoul(start, &term, 0);
	}
	else {
	    start = p;
	    aValue->integer = strtoul(p, &term, 0);
	}
	if ((term == start) || (*term == '.') || (*term == 'e') ||
		(*term == 'E') || (*term == '8') || (*term == '9')) {
	    status = Tcl_ConvertFromDStringToDouble(interp, aValue,
						    typeAttr);
	    if (status == TCL_OK) aValue->integer = aValue->doub;
	    else if (interp)
		Tcl_AppendResult(interp, "Illegal integer value ", p, NULL);
	}
	else status = TCL_OK;
    }
    else aValue->integer = 0;

    TCL_SETASVALID(typeAttr, TCL_TYPE_INTEGER);
    return status;
}

int
Tcl_ConvertFromDStringToDouble(Tcl_Interp *interp, AllValue *aValue,
			       TypeAttribute *typeAttr)
{
    char *term;
    int  status = TCL_ERROR;
    
    if (aValue->dString.string) {
	errno = 0;
	aValue->doub = strtod(aValue->dString.string, &term);
        if ((term != aValue->dString.string) && (*term == '\0')) {
	    if (errno == 0) status = TCL_OK;
	}
    }
    else aValue->doub = 0.;
    TCL_SETASVALID(typeAttr, TCL_TYPE_DOUBLE);
    return status;
}

int
Tcl_ConvertFromDStringToList(Tcl_Interp *interp, AllValue *aValue,
			     TypeAttribute *typeAttr)
{
    int status = TCL_OK;

    if (!(*typeAttr & TCL_TYPE_LIST)) {
	status = Tcl_SplitToList(interp, &aValue->list,
				 aValue->dString.string);
    	if (status == TCL_OK) *typeAttr |= TCL_TYPE_LIST;
    }
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ParseNumeric
 *
 *	Parse a number.  Try first an integer, then a float.  If neither
 *	succeeds, we have an error.
 *
 * Results:
 *	TCL_OK - it is a number, or TCL_ERROR - not a number
 *
 * Side effects:
 *	Sets the field in the AllValue structure to valid value
 *
 *----------------------------------------------------------------------
 */

int
Tcl_ParseNumeric(interp, aValue, typeAttr, type)
Tcl_Interp    *interp;
AllValue      *aValue;
TypeAttribute *typeAttr;
TypeAttribute *type;
{
    char *p;
    if (*typeAttr & (TCL_TYPE_INTEGER | TCL_TYPE_DOUBLE)) {
	*type = 0;
	return TCL_OK;
    } else {
	if (!(*typeAttr & TCL_TYPE_STRING) && !(*typeAttr&TCL_TYPE_DSTRING)) {
	    Tcl_ConvertFromUnknownToDString(interp, aValue, typeAttr);
	    p = aValue->dString.string;
	}
	else {
	    if (*typeAttr & TCL_TYPE_DSTRING) p = aValue->dString.string;
	    else p = aValue->string;
	}
	return Tcl_ParseString(interp,
			       p,
			       &aValue->integer,
			       &aValue->doub,
			       type);
    }
}

int Tcl_ParseString(interp, p, iv, dv, type)
    Tcl_Interp    *interp;
    char          *p;
    int           *iv;
    double        *dv;
    TypeAttribute *type;
{
    char *start, *term;	/* local string pointers */

    if (p) while (*p == ' ' && *p != '\0') p++;

    if (!p || strlen(p) == 0) {
	Tcl_SetResult(interp, "No Number", TCL_STATIC);
	return TCL_ERROR;
    }
	
    *type = TCL_TYPE_STRING;
    errno = 0;

    if (ExprLooksLikeInt(p)) {
	if (*p == '-') {
	    start = p+1;
	    *iv = -strtoul(start, &term, 0);
	}
	else if (*p == '+') {
	    start = p+1;
	    *iv = strtoul(start, &term, 0);
	}
	else {
	    start = p;
	    *iv = strtoul(p, &term, 0);
	}
	if (*term == 0) {
	    if (errno == ERANGE) {
		    
		/*
		 * This procedure is sometimes called with string in
		 * interp->result, so we have to clear the result before
		 * logging an error message.
		 */

		Tcl_ResetResult(interp);
		interp->result = "integer value too large to represent";
		Tcl_SetErrorCode(interp, "ARITH", "IOVERFLOW",
				 interp->result, (char *) NULL);
		return TCL_ERROR;
	    } else {
		*type = TCL_TYPE_INTEGER;
		return TCL_OK;
	    }
	}
    } else {
	    
	errno = 0;
	*dv = strtod(p, &term);
	if (term != p && *term == 0) {
	    if (errno != 0) {
		Tcl_ResetResult(interp);
		if (interp) TclExprFloatError(interp, *dv);
		return TCL_ERROR;
	    }
	    *type = TCL_TYPE_DOUBLE;
	    return TCL_OK;
	}
    }
    return TCL_ERROR;
}


/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConvertFromIntegerToString
 *
 *	Convert variable from integer to string.
 *
 * Tcl_ConvertFromIntegerToDouble
 *
 *	Convert variable from string to double
 *
 * Tcl_ConvertFromIntegerToList
 *
 *	Convert variable from string to list.
 *
 * Results:
 *	None
 *
 * Side effects:
 *	Sets the field in the AllValue structure to valid value
 *
 *----------------------------------------------------------------------
 */


int
Tcl_ConvertFromIntegerToString(Tcl_Interp *interp, AllValue *aValue,
			       TypeAttribute *typeAttr)
{
    char buffer[100];
    int  len;

    sprintf(buffer, "%d", aValue->integer);
    if (aValue->string && *typeAttr & TCL_TYPE_STRING &&
			  *typeAttr & TCL_TYPE_FSTRING)
	ckfree((char *) aValue->string);

    len = strlen(buffer);
    aValue->string = ckalloc(len+1);
    strcpy(aValue->string, buffer);
    TCL_SETASVALID(typeAttr, TCL_TYPE_STRING|TCL_TYPE_FSTRING);
    return TCL_OK;
}

int
Tcl_ConvertFromIntegerToDouble(Tcl_Interp *interp, AllValue *aValue,
			       TypeAttribute *typeAttr)
{
    aValue->doub = aValue->integer;
    TCL_SETASVALID(typeAttr, TCL_TYPE_DOUBLE);
    return TCL_OK;
}

int
Tcl_ConvertFromIntegerToList(Tcl_Interp *interp, AllValue *aValue,
			     TypeAttribute *typeAttr)
{
    int status = Tcl_ConvertFromIntegerToDString(interp, aValue, typeAttr);
    if (status == TCL_OK)
	status = Tcl_ConvertFromDStringToList(interp, aValue, typeAttr);

    return status;
}

int
Tcl_ConvertFromIntegerToDString(Tcl_Interp *interp, AllValue *aValue,
				TypeAttribute *typeAttr)
{
    char buffer[100];

    sprintf(buffer, "%d", aValue->integer);
    aValue->dString.length = 0;
    aValue->dString.string[0] = '\0';

    Tcl_DStringAppend(&aValue->dString,
		      buffer,
		      strlen(buffer));
    
    *typeAttr |= TCL_TYPE_DSTRING;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConvertFromDoubleToString
 *
 *	Convert variable from integer to string.
 *
 * Tcl_ConvertFromDoubleToDouble
 *
 *	Convert variable from string to double
 *
 * Tcl_ConvertFromDoubleToList
 *
 *	Convert variable from string to list.
 *
 * Results:
 *	None
 *
 * Side effects:
 *	Sets the field in the AllValue structure to valid value
 *
 *----------------------------------------------------------------------
 */

int
Tcl_ConvertFromDoubleToString(Tcl_Interp *interp, AllValue *aValue,
			      TypeAttribute *typeAttr)
{
    char buffer[100];
    int  len;

    Tcl_ConvertDoubleToBuffer(((Interp*) interp)->pdFormat, aValue->doub,
			      buffer);
    
    if (aValue->string && *typeAttr & TCL_TYPE_STRING &&
			  *typeAttr & TCL_TYPE_FSTRING)
	ckfree((char *) aValue->string);

    len = strlen(buffer);
    aValue->string = ckalloc(len+1);
    strcpy(aValue->string, buffer);
    TCL_SETASVALID(typeAttr, TCL_TYPE_STRING|TCL_TYPE_FSTRING);
    
    return TCL_OK;
}

int
Tcl_ConvertFromDoubleToInteger(Tcl_Interp *interp, AllValue *aValue,
			       TypeAttribute *typeAttr)
{
    aValue->integer = aValue->doub;
    TCL_SETASVALID(typeAttr, TCL_TYPE_INTEGER);
    return TCL_OK;
}

int
Tcl_ConvertFromDoubleToList(Tcl_Interp *interp, AllValue *aValue,
			    TypeAttribute *typeAttr)
{
    int status = Tcl_ConvertFromDoubleToDString(interp, aValue, typeAttr);
    if (status == TCL_OK)
	status = Tcl_ConvertFromDStringToList(interp, aValue, typeAttr);

    return status;
}

int
Tcl_ConvertFromDoubleToDString(Tcl_Interp *interp, AllValue *aValue,
			       TypeAttribute *typeAttr)
{
    char buffer[100];
    char* fmt = (interp ? ((Interp*) interp)->pdFormat : "%g");
    
    Tcl_ConvertDoubleToBuffer(fmt, aValue->doub, buffer);
    
    aValue->dString.length = 0;
    aValue->dString.string[0] = '\0';

    Tcl_DStringAppend(&aValue->dString,
		      buffer,
		      strlen(buffer));
    
    *typeAttr |= TCL_TYPE_DSTRING;
    return TCL_OK;
}

void Tcl_ConvertDoubleToBuffer(char *fmt, double doub, char *buffer)
{
    register FLAG cont = 1;
    register char *p;
    
    sprintf(buffer, fmt, doub);
    for (p = buffer; *p != 0 && cont; p++) {
	if ((*p == '.') || (isalpha(UCHAR(*p)))) {
	    cont = 0;
	}
    }

    /* Append a 0. at the end to ensure that we don't look like an
       integer ... */
    
    if (cont) {
	
	p[0] = '.';
	p[1] = '0';
	p[2] = 0;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConvertFromListToString
 *
 *	Convert variable from integer to string.
 *
 * Tcl_ConvertFromListToInteger
 *
 *	Convert variable from string to integer.
 *
 * Tcl_ConvertFromListToDouble
 *
 *	Convert variable from string to double
 *
 * Results:
 *	None
 *
 * Side effects:
 *	Sets the field in the AllValue structure to valid value
 *
 *----------------------------------------------------------------------
 */

int
Tcl_ConvertFromListToString(Tcl_Interp *interp, AllValue *aValue,
			    TypeAttribute *typeAttr)
{
    int status = Tcl_ConvertFromListToDString(interp, aValue, typeAttr);
    if (status == TCL_OK)
	status = Tcl_ConvertFromDStringToString(interp, aValue, typeAttr);

    return status;
}

int
Tcl_ConvertFromListToInteger(Tcl_Interp *interp, AllValue *aValue,
			     TypeAttribute *typeAttr)
{
    int status = Tcl_ConvertFromListToDString(interp, aValue, typeAttr);
    if (status == TCL_OK)
	status = Tcl_ConvertFromDStringToInteger(interp, aValue, typeAttr);
    
    return status;
}

int
Tcl_ConvertFromListToDouble(Tcl_Interp *interp, AllValue *aValue,
			    TypeAttribute *typeAttr)
{
    int status = Tcl_ConvertFromListToDString(interp, aValue, typeAttr);
    if (status == TCL_OK)
	status = Tcl_ConvertFromDStringToDouble(interp, aValue, typeAttr);

    return status;
}

int
Tcl_ConvertFromListToDString(Tcl_Interp *interp, AllValue *aValue,
			     TypeAttribute *typeAttr)
{
    int status;

    if (!(*typeAttr & TCL_TYPE_DSTRING)) {
        status = Tcl_MergeList(interp, &aValue->list, " ", &aValue->dString);
	if (status == TCL_OK) *typeAttr |= TCL_TYPE_STRLIST | TCL_TYPE_DSTRING;
    }
    else status = TCL_OK;

    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConvertFromUnknownToString
 *
 *	Convert unknown variable type to string.
 *
 * Tcl_ConvertFromUnknownToInteger
 *
 *	Convert unknown variable type to integer
 *
 * Tcl_ConvertFromUnknownToDouble
 *
 *	Convert unknown variable type to double
 *
 * Tcl_ConvertFromUnknownToList
 *
 *	Convert unknown variable type to list.
 *
 * Results:
 *	None
 *
 * Side effects:
 *	Sets the field in the AllValue structure to valid value
 *
 *----------------------------------------------------------------------
 */

int
Tcl_ConvertFromUnknownToString(Tcl_Interp *interp, AllValue *aValue,
			       TypeAttribute *typeAttr)
{
    ConvAttribute    *block;
    Tcl_ConvFunction *convProc;
    int              status = TCL_OK;

    if (!(*typeAttr & TCL_TYPE_STRING)) {

	if ((*typeAttr & TCL_TYPE_DSTRING)) {
	    status = Tcl_ConvertFromDStringToString(interp, aValue, typeAttr);
	}
	else {
	    block = Tcl_ReturnConversion(*typeAttr);
	    if (block) {
		convProc = block->string;
		if (convProc) status = (*convProc)(interp, aValue, typeAttr);
		TCL_SETASVALID(typeAttr, TCL_TYPE_STRING);
	    }
	    else {
		if (interp)
		    Tcl_AppendResult(interp,
				     "internal error - null type!", NULL);
		status = TCL_ERROR;
	    }
	}
    }
    return status;
}

int
Tcl_ConvertFromUnknownToInteger(Tcl_Interp *interp, AllValue *aValue,
				TypeAttribute *typeAttr)
{
    ConvAttribute    *block;
    Tcl_ConvFunction *convProc;
    int              status = TCL_OK;

    if (!(*typeAttr & TCL_TYPE_INTEGER)) {
	
        block = Tcl_ReturnConversion(*typeAttr);
	if (block) {
	    convProc = block->integer;
	    if (convProc) status = (*convProc)(interp, aValue, typeAttr);
	    TCL_SETASVALID(typeAttr, TCL_TYPE_INTEGER);
	}
 	else {
	    if (interp)
		Tcl_AppendResult(interp, "internal error - null type!", NULL);
	    status = TCL_ERROR;
	}
   }
    return status;
}

int
Tcl_ConvertFromUnknownToDouble(Tcl_Interp *interp, AllValue *aValue,
			       TypeAttribute *typeAttr)
{
    ConvAttribute    *block;
    Tcl_ConvFunction *convProc;
    int              status = TCL_OK;

    if (!(*typeAttr & TCL_TYPE_DOUBLE)) {
	
        block = Tcl_ReturnConversion(*typeAttr);
 	if (block) {
	    convProc = block->doub;
	    if (convProc) status = (*convProc)(interp, aValue, typeAttr);
	    TCL_SETASVALID(typeAttr, TCL_TYPE_DOUBLE);
	}
	else {
	    if (interp)
		Tcl_AppendResult(interp, "internal error - null type!", NULL);
	    status = TCL_ERROR;
	}
    }
    return status;
}

int
Tcl_ConvertFromUnknownToList(Tcl_Interp *interp, AllValue *aValue,
			     TypeAttribute *typeAttr)
{
    ConvAttribute    *block;
    Tcl_ConvFunction *convProc;
    int              status = TCL_OK;

    if (!(*typeAttr & TCL_TYPE_LIST))
    {
        block = Tcl_ReturnConversion(*typeAttr);
	if (block) {
	    convProc = block->list;
	    if (convProc) status = (*convProc)(interp, aValue, typeAttr);
	    TCL_SETASVALID(typeAttr, TCL_TYPE_LIST);
	}
	else {
	    if (interp)
		Tcl_AppendResult(interp, "internal error - null type!", NULL);
	    status = TCL_ERROR;
	}
    }
    return status;
}

int
Tcl_ConvertFromUnknownToDString(Tcl_Interp *interp, AllValue *aValue,
				TypeAttribute *typeAttr)
{
    ConvAttribute    *block;
    Tcl_ConvFunction *convProc;
    int              status = TCL_OK;

    if (!(*typeAttr & TCL_TYPE_DSTRING)) {
	
	if ((*typeAttr & TCL_TYPE_STRING)) {
	    status = Tcl_ConvertFromStringToDString(interp, aValue, typeAttr);
	}
	else {
	    block = Tcl_ReturnConversion(*typeAttr);
	    if (block) {
		convProc = block->dString;
		if (convProc) status = (*convProc)(interp, aValue, typeAttr);
		TCL_SETASVALID(typeAttr, TCL_TYPE_DSTRING);
	    }
	    else {
		if (interp)
		    Tcl_AppendResult(interp,
				     "internal error - null type!", NULL);
		status = TCL_ERROR;
	    }
	}
    }
    return status;
}

int
Tcl_ConvertUnknownToNumeric(Tcl_Interp *interp, AllValue *aValue,
			    TypeAttribute *typeAttr)
{
    int           status;
    TypeAttribute retType;

    if (!(*typeAttr & (TCL_TYPE_INTEGER | TCL_TYPE_DOUBLE))) {
	status = Tcl_ParseNumeric(interp, aValue, typeAttr, &retType);
	if (status == TCL_OK && (retType == TCL_TYPE_INTEGER ||
				 retType == TCL_TYPE_DOUBLE)) {
	    TCL_SETASVALID(typeAttr, retType);
	}
    }
    else status = TCL_OK;
    
    return status;
}

/*
 *	List Processing
 */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DStringSetAlloc --
 *
 *	Change the allocation of a dynamic string.  This can cause the
 *	string to either grow or shrink, depending on the value of
 *	length.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The length of dsPtr is changed to length and a null byte is
 *	stored at that position in the string.  If length is larger
 *	than the space allocated for dsPtr, then a panic occurs.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DStringSetAlloc(dsPtr, length)
    register Tcl_DString *dsPtr;	/* Structure describing dynamic
					 * string. */
    int length;				/* New length for dynamic string. */
{
    if (length < 0) {
	length = 0;
    }
    if (length >= dsPtr->spaceAvl) {
	char *newString;

	dsPtr->spaceAvl = length+1;
	newString = (char *) ckalloc((unsigned) dsPtr->spaceAvl);

	/*
	 * SPECIAL NOTE: must use memcpy, not strcpy, to copy the string
	 * to a larger buffer, since there may be embedded NULLs in the
	 * string in some cases.
	 */

	memcpy((VOID *) newString, (VOID *) dsPtr->string,
		(size_t) dsPtr->length);
	if (dsPtr->string != dsPtr->staticSpace) {
	    ckfree(dsPtr->string);
	}
	dsPtr->string = newString;
    }
    else if (length < dsPtr->length) {
	dsPtr->length = length;
	dsPtr->string[length] = '\0';
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ListInit
 *
 *	Create a representation of a TCL list that C can get at
 *
 * Results:
 *	Tcl_C_List *aList
 *
 * Side effects:
 *	Allocate storage to hold the rep of a list
 *
 *----------------------------------------------------------------------
 */

void Tcl_ListInit(Tcl_C_List *list)
{
    list->nAlloc = LIST_STATIC_SPACE;
    list->nElem  = 0;
    list->argv   = list->static_argv_space;
    list->flags  = list->static_flags_space;
    list->len    = list->static_len_space;
    Tcl_DStringInit(&list->list);
    return;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SplitToList --
 *
 *	Splits a list up into its constituent fields.
 *
 * Results
 *	The return value is normally the list itself, which means that
 *	the list was successfully split up.  If NULL is
 *	returned, it means that "list" didn't have proper list
 *	structure;  interp->result will contain a more detailed
 *	error message.
 *
 *	list is an internal list structure where argv is an array
 *	whose elements point to the elements of list, in order.
 *	nElem will get filled in with the number of valid elements
 *	in the array.  The list field is a copy of the list (with
 *	backslashes and braces removed in the standard way).
 *	The caller must eventually free this memory by calling
 *	Tcl_ListDelete(list)
 *
 * Side effects:
 *	Memory is allocated.
 *
 *----------------------------------------------------------------------
 */

#define USE_BRACES		2
#define BRACES_UNMATCHED	4

int
Tcl_SplitToList(interp, list, string)
Tcl_Interp  *interp;	/* Interpreter to use for error reporting. */
Tcl_C_List  *list;	/* list to split the string to */
char        *string;	/* Pointer to string with list structure. */
{
    register char *p;		/* destination pointers for strings */
    char          *element;	/* next list element in the string */
    register int  strLen;	/* length of list rep of string */
    int           size, i, result, elSize, brace;
    char          *baseStr, *beginEle;

    Tcl_ListInit(list);
    if (!string) list->nElem = 0;
    else {
	
	/*
	 * Figure out how much space to allocate.  There must be enough
	 * space for both the array of pointers and also for a copy of
	 * the list.  To estimate the number of pointers needed, count
	 * the number of space characters in the list.
	 */

	for (size = 1, p = (char *) string; *p != 0; p++) {
	    if (isspace(UCHAR(*p))) {
		size++;
	    }
	}
	strLen = p - string + 1;
    
	/* Initialize the list.  This is maximum # of elements possible.
	   It might be less ... */
    
	Tcl_ListAllocSize(list, size);
	list->nElem = 0;
	
	Tcl_DStringSetAlloc(&list->list, strLen);
    
	/* point the initial destination pointer to the beginning of
	   the string ... */
    
	baseStr = p = list->list.string;
    
	/* Get all of the elements ... */
	    
	for (i = 0; *string != 0; i++) {
	    result = TclFindElement(interp, string, &element,
				    &string, &elSize, &brace);
	    if (result != TCL_OK) {
		Tcl_ListDelete(list);
		return TCL_ERROR;
	    }
	    if (*element == 0) {
		break;
	    }
	    if (i > size) {
		Tcl_SetResult(interp, "internal error in Tcl_SplitToList",
			      TCL_STATIC);
		Tcl_ListDelete(list);
		return TCL_ERROR;
	    }
	
	    /* New Element ... */
	
	    list->argv[i] = p-baseStr;
	    list->nElem++;
	    
	    /* Figure out what the proper element is and while we
	       are at it, figure out the disassembler flag ... */
	    
	    beginEle = p;
	    if (brace) {
		strncpy(p, element, (size_t) elSize);
		p += elSize;
		*p = 0;
		p++;
	    } else {
		TclCopyAndCollapse(elSize, element, p);
		p += elSize+1;
	    }
	    list->len[i] = Tcl_ScanElement(beginEle, &list->flags[i]);
	    if (brace) list->flags[i] |= USE_BRACES;
	    list->list.length += elSize+1;
	}
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ListAppendElement
 *
 *	Add a list element to a TCL list.  Convert the string to
 *	a list representation then append the rep to current list.
 *
 * Return: None
 *
 * Side effects:
 *	Expanded list + appended string
 *
 *----------------------------------------------------------------------
 */

int Tcl_ListAppendElement(Tcl_Interp  *interp,
			  Tcl_C_List  *list,
			  char        *element,
			  Tcl_DString *strList)
{
    int  nChars, length, newSize;
    int  oldLstr, oldLlength, newLlength;
    int  listFlags;
    char *dst, *p;
    
    if (element) {
	
	/*
	 * Figure out how much space to allocate.  There must be enough
	 * space for both the array of pointers and also for a copy of
	 * the list.  To estimate the number of pointers needed, count
	 * the number of space characters in the list.
	 */

	nChars = strlen(element);
	
	/* Store old length and old string structure length of the list ... */
	
	oldLlength = list->nElem;
	oldLstr    = list->list.length;
	length     = nChars+oldLstr+1;
	newLlength = oldLlength+1;
	
	if (newLlength > list->nAlloc) {
	    newSize = 2*list->nAlloc+1;
	    if (newLlength > newSize) newSize = newLlength+1;
	    Tcl_ListAllocSize(list, newSize);
	}
	list->nElem += 1;
	
	if (length > list->list.spaceAvl) {
	    newSize = 2*list->list.spaceAvl + 1;
	    if (newSize < length) newSize = length+1;
	    Tcl_DStringSetAlloc(&list->list, newSize);
	}
	
	/* point the initial destination pointer to the beginning of
	   the string and copy the element to there. ... */
    
	p                      = list->list.string + oldLstr;
	list->argv[oldLlength] = oldLstr;
	strcpy(p, element);
	
	length = list->len[oldLlength] =
	    Tcl_ScanElement(element, &list->flags[oldLlength]);
	
	/* Include null termination in the DString length ... */
	
	list->list.length += nChars + 1;
    
	/* Update the optional string as a list ... */
	
	if (strList) {
	    listFlags = list->flags[oldLlength];
	    nChars    = strList->length;
	    
	    /* Next call sets length of the DString ... */

	    length += nChars+1;
	    if (length > strList->spaceAvl) {
		newSize = 2*strList->spaceAvl + 1;
		if (newSize < length) newSize = length+1;
		Tcl_DStringSetAlloc(strList, newSize);
	    }
	    
	    /* Determine where to put the element and fill in DString ... */
	    
	    dst = strList->string + nChars;
	    if (TclNeedSpace(strList->string, dst)) {
		*dst = ' ';
		dst++;
		strList->length++;
	    }
	    strList->length += Tcl_ConvertElement((char *) element,
						  dst, listFlags);
	}
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_MergeList --
 *
 *	Merges together list elements into a string
 *
 * Results
 *	The return value is TCL_OK if it was successful, TCL_ERROR if not
 *
 * Side effects:
 *	Memory is allocated.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_MergeList(interp, list, joinString, sList)
Tcl_Interp  *interp;	/* Interpreter to use for error reporting. */
Tcl_C_List  *list;	/* Pointer to string with list structure. */
const char  *joinString;/* separator between list elements */
Tcl_DString *sList;	/* String that is result of merge ... */
{
    int numChars;
    register char *dst, *baseStr;
    int i, lenj;

    Tcl_DStringSetLength(sList, 0);
    if (list && list->nElem > 0) {
	
	/*
	 * Pass 1: estimate space, gather flags.
	 */
	
	numChars = 1;
	for (i = 0; i < list->nElem; i++) {
	    numChars += list->len[i] + 1;
	}
	
	/*
	 * Pass two: copy into the result area.
	 */

	Tcl_DStringSetAlloc(sList, numChars);
	dst = sList->string;
	lenj = strlen(joinString);
	baseStr = list->list.string;
	
	for (i = 0; i < list->nElem; i++) {
	    numChars = Tcl_ConvertElement(baseStr+list->argv[i], dst,
					  list->flags[i]);
	    dst += numChars;
	    sList->length += numChars;
	    
	    /* Add join string except the last element ... */
	    
	    if (i != list->nElem-1) {
		Tcl_DStringAppend(sList, (char *) joinString, lenj);
		dst += lenj;
	    }
	}
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConcatList
 *
 *	concatentate two or more lists together
 *
 * Results:
 *	New list
 *
 * Side effects:
 *	Storage expanded to get the two lists.
 *
 *----------------------------------------------------------------------
 */

	/* VARARGS2 */
void
#ifdef USEVARARGS
Tcl_ConcatList(va_alist)
    va_dcl
#else
Tcl_ConcatList(Tcl_C_List *first, ...)
#endif
{
    va_list    ap;			/* variable # of lists */
    Tcl_C_List *toList;			/* list to write to */
    Tcl_C_List *next;			/* next one to concatenate */
    
#ifdef USEVARARGS
    va_start(ap);
    toList = va_arg(ap, Tcl_C_List *);
#else
    va_start(ap, first);
    toList = first;
#endif
    next   = va_arg(ap, Tcl_C_List *);
    
    /* Create the return list.  Then append list by list ... */

    while (next) {
	Tcl_AppendList(toList, next, 0, -1);
	next = va_arg(ap, Tcl_C_List *);
    }
    va_end(ap);
    return;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppendList
 *
 *	concatentate two lists together
 *
 * Return:
 *	None
 *
 * Side effects:
 *	Storage expanded to get the two lists.  aList1 the concatenated
 *	list
 *
 *----------------------------------------------------------------------
 */

void Tcl_AppendList(Tcl_C_List *aList1, Tcl_C_List *aList2,
		    int baseIndex, int nAppElem)
{
    int i;		/* loop variable */
    int offset;		/* offset to next storage block */
    int stringOffset;	/* offset to pointers to list elements */

    if (aList1 && aList2) {

	int  nEleFromBase, copyLength;
	char *baseStr, *finalStr;
	
	baseIndex = (baseIndex >= 0) ? baseIndex : 0;
	nEleFromBase = aList2->nElem - baseIndex;
	nEleFromBase = (nEleFromBase >= 0) ? nEleFromBase : 0;
	
	if (nAppElem < 0)
	     nAppElem = nEleFromBase;
	else nAppElem = (nAppElem <= nEleFromBase) ? nAppElem : nEleFromBase;

	if (nAppElem) {
	    offset       = aList1->nElem;
	    stringOffset = aList1->list.length;
	    
	    /* Figure out exactly how much to append.  Start from the base
	       of the first string, go to the end of the last string and
	       include terminating 0. ... */
	    
	    baseStr    = aList2->list.string + aList2->argv[baseIndex];
	    finalStr   = aList2->list.string +
		aList2->argv[baseIndex+nAppElem-1];
	    
	    copyLength = finalStr - baseStr + strlen(finalStr) + 1;
	    Tcl_DStringAppend(&aList1->list, baseStr, copyLength);
	    
	    Tcl_ListAllocSize(aList1, aList1->nElem + nAppElem);
	    aList1->nElem += nAppElem;
	    
	    /* Fill in descriptive data starting after the old data ... */
	    
	    memcpy(aList1->flags+offset, aList2->flags+baseIndex,
		    nAppElem*sizeof(int));
	    
	    memcpy(aList1->len+offset,   aList2->len+baseIndex,
		    nAppElem*sizeof(int));
	    
	    /* Update the offsets to the pointers of appended list
	       elements ... */
	    
	    for (i = 0; i < nAppElem; i++)
		aList1->argv[offset+i] = stringOffset +
		    aList2->argv[i+baseIndex] - aList2->argv[baseIndex];
	}
    }
    return;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ListSetSize
 *
 *	Set the size of the list to nElements
 *
 * Return:
 *	None
 *
 * Side effects:
 *	Storage expanded to accomodate nElements in list
 *
 *----------------------------------------------------------------------
 */

void Tcl_ListSetSize(Tcl_C_List *list, int nElements)
{
    if (nElements < 0) nElements = 0;
    Tcl_ListAllocSize(list, nElements);
    list->nElem = nElements;
}

void Tcl_ListAllocSize(Tcl_C_List *list, int nElements)
{
    int nextAlloc, offset;
    
    offset = list->nElem;
    if (nElements > list->nAlloc) {
	nextAlloc = (1+nElements/LIST_BLOCK_ALLOC)*LIST_BLOCK_ALLOC;
	list->nAlloc = nextAlloc;
	if (list->argv != list->static_argv_space) {
	    list->argv  = (int *) realloc(list->argv,
					  nextAlloc*sizeof(char *));
	    list->flags = (int *) realloc(list->flags,
					  nextAlloc*sizeof(int));
	    list->len   = (int *) realloc(list->len,
					  nextAlloc*sizeof(int));
	}
	else {
	    int *atemp = (int *) ckalloc(nextAlloc*sizeof(char *));
	    int *ftemp = (int *) ckalloc(nextAlloc*sizeof(int));
	    int *ltemp = (int *) ckalloc(nextAlloc*sizeof(int));
	    
	    memcpy(atemp, list->argv,  offset*sizeof(int));
	    memcpy(ftemp, list->flags, offset*sizeof(int));
	    memcpy(ltemp, list->len,   offset*sizeof(int));
	    
	    list->argv  = atemp;
	    list->flags = ftemp;
	    list->len   = ltemp;
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ListDelete
 *
 *	Free a representation of a list
 *
 * Results:
 *	NULL
 *
 * Side effects:
 *	All associated list storage is freed
 *
 *----------------------------------------------------------------------
 */

void
Tcl_ListDelete(aList)
    Tcl_C_List *aList;
{
    if (aList) {
	Tcl_DStringFree(&aList->list);
	if (aList->argv != aList->static_argv_space) {
	    ckfree((void *) aList->argv);
	    ckfree((void *) aList->flags);
	    ckfree((void *) aList->len);
	}
	Tcl_ListInit(aList);
    }
    return;
}

/*
 *	C R E A T E / F R E E  V A L U E S
 */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_VarValueCreate/Tcl_AllValueCreate
 *
 *	Create an allValue field
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The allValue structure is empty
 *
 *----------------------------------------------------------------------
 */

void
Tcl_VarValueCreate(aVar)
    Var *aVar;		/* variable */
{
    Tcl_AllValueCreate(&aVar->value.allValue, &aVar->varAttr.typeAttr);

/*  ... Create conversion attribute structure ... */

    Tcl_OnlyValidVarMACRO(aVar, TCL_TYPE_STRING);
}

void
Tcl_AllValueCreate(allValue, typeAttr)
    AllValue      *allValue;
    TypeAttribute *typeAttr;
{
    Tcl_initAllValueMACRO(allValue);
    
    /* Initialize the valid type to only string ... */

    *typeAttr = TCL_TYPE_INTEGER;
    return;
}

void Tcl_tVarInit(tVar *aVar)
{
    Tcl_tVarInitMACRO(aVar);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteValueVar/Tcl_DeleteValue
 *
 *	Delete an allValue field
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The allValue structure is empty and all allocated space freed
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DeleteValueVar(aVar)
    Var *aVar;		/* variable */
{
    Tcl_DeleteValueVarMACRO(aVar);
    return;
}

void
Tcl_DeleteValue(allValue, typeAttr, curType)
    AllValue      *allValue;
    TypeAttribute *typeAttr;
    TypeAttribute curType;
{
    Tcl_DeleteValueMACRO(allValue, typeAttr, curType);
    return;
}
