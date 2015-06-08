/* 
 * tclToCUtil.c --
 *
 *	This file contains routines that are utilities for the TCL-->
 *	C translator.
 *
 */

#ifndef lint
static char rcsid[] = "$Header: /net/mizar/usr/local/source/coda_source/Tcl/src7.4/tcl7.4/tclToCUtil.c,v 1.1.1.1 1996/08/21 19:30:13 heyes Exp $ SPRITE (Berkeley)";
#endif

#include "tclInt.h"

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SplitArray(char *varName, char **open, char **close)
 *
 *	Split a variable name into the array name + index
 *	Needed for the TCL->C translator.  The call to this routine will
 *	be inserted directly into the code.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Modify the string varName and note the parenthesis
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_SplitArray(varName, open, close)
    char *varName;	/* variable to split */
    char **open;	/* location of ( */
    char **close;	/* location of ) */
{
    for (*close = varName; **close != '\0'; (*close)++) {
	if (**close == '(') {
	    *open = *close;

	    do {
		(*close)++;
	    } while (**close != '\0');
	    (*close)--;
	    if (**close != ')') {
		goto scalar;
	    }
	    **open = '\0';
	    **close= '\0';
	    return *close;
	}
    }

scalar: *close = *open = (char *) NULL;
    return *close;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_Append_DStrings --
 *
 *	Append d2 to the end of d1
 *
 * Results:
 *	Updated d1
 *
 * Side effects:
 *	See Tcl_DStringAppend for side effects on d1
 *
 *----------------------------------------------------------------------
 */

void Tcl_AppendDStrings(d1, d2)
Tcl_DString *d1;
Tcl_DString *d2;
{
    Tcl_DStringAppend(d1, d2->string, d2->length);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_OvrLoadResult --
 *
 *	Makes the string result valid if we have overloaded result
 *
 * Results:
 *	Updated result structure
 *
 * Side effects:
 *	See Tcl_DStringAppend for side effects on d1
 *
 *----------------------------------------------------------------------
 */

int Tcl_OvrLoadResult(Tcl_Interp *interp)
{
    Interp *iPtr = (Interp *) interp;
    TypeAttribute type = iPtr->results.typeAttr;
    int status;
    
    if (type & TCL_TYPE_RETOVR) {
	if (type & TCL_TYPE_DSTRING)
	    Tcl_DStringResOvr(interp);
	else if (type & TCL_TYPE_STRING) {
	    Tcl_FreeResult(interp);
	    interp->result = iPtr->results.allValue.string;
	    if (type & TCL_TYPE_FSTRING) {
		interp->freeProc = TCL_VOLATILE;
		iPtr->results.typeAttr &= ~TCL_TYPE_FSTRING;
	    }
	    else interp->freeProc = TCL_STATIC;
	}
	else if (type & TCL_TYPE_LIST) {
	    status =
		Tcl_ConvertFromListToDString(interp,
					     &iPtr->results.allValue,
					     &iPtr->results.typeAttr);
	    Tcl_DStringResOvr(interp);
	}
	else if (type & TCL_TYPE_INTEGER) {
	    status =
		Tcl_ConvertFromIntegerToDString(interp,
						&iPtr->results.allValue,
						&iPtr->results.typeAttr);
	    Tcl_DStringResOvr(interp);
	}
	else if (type & TCL_TYPE_DOUBLE) {
	    status =
		Tcl_ConvertFromDoubleToDString(interp,
					       &iPtr->results.allValue,
					       &iPtr->results.typeAttr);
	    Tcl_DStringResOvr(interp);
	}
	Tcl_ResetValueMACRO(&iPtr->results.allValue,
			    &iPtr->results.typeAttr);
    }
    else status = TCL_OK;
    
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CheckEquivalence --
 *
 *	Check the equivalence of 2 variables.  They could either be the
 *	same address, the same string address or the string address could be
 *	the dstring storage address.  For use with arguments and global
 *	or uplevel, upvar variables where the compiler cannot statically
 *	determine the place.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None
 *
 *----------------------------------------------------------------------
 */

int Tcl_CheckEquivalence(Var *v1, Var *v2)
{
    int ret = 0;
    ret = v1 == v2;
    if (!ret && v1 && v2) {
	ret = (v1->value.allValue.string == v2->value.allValue.string) ||
	    (v1->value.allValue.dString.string==v2->value.allValue.string) ||
	    (v2->value.allValue.dString.string==v1->value.allValue.string);
    }
    return ret;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DStringOvr --
 *
 *	This procedure moves the value of the overloaded dynamic string into an
 *	interpreter as its result.  The overloaded result dstring  itself is
 *	reinitialized to an empty string.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The string is "moved" to interp's result, and any existing
 *	result for interp is freed up.  DsPtr is reinitialized to
 *	an empty string.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DStringResOvr(interp)
    Tcl_Interp *interp;		/* Interpreter whose result is to be
				 * reset. */
{
    Tcl_DString *dsPtr;		/* Dynamic string that is to become
				 * the result of interp. */
    Interp *iPtr = (Interp *) interp;
    
    /* Don't reset/free the result, because that wipes out the overloaded
       result structure ... */
    
    if ((interp)->freeProc != 0) {
	if ((interp)->freeProc == (Tcl_FreeProc *) free) {
	    ckfree((interp)->result);
	} else {
	    (*(interp)->freeProc)((interp)->result);
	}
	(interp)->freeProc = 0;
    }
    iPtr->result = iPtr->resultSpace;
    iPtr->resultSpace[0] = 0;
    
    iPtr->flags &=
	    ~(ERR_ALREADY_LOGGED | ERR_IN_PROGRESS | ERROR_CODE_SET);
    
    dsPtr = &iPtr->results.allValue.dString;
    if (dsPtr->string != dsPtr->staticSpace) {
	interp->result = dsPtr->string;
	interp->freeProc = (Tcl_FreeProc *) free;
    } else if (dsPtr->length < TCL_RESULT_SIZE) {
	interp->result = ((Interp *) interp)->resultSpace;
	strcpy(interp->result, dsPtr->string);
    } else {
	Tcl_SetResult(interp, dsPtr->string, TCL_VOLATILE);
    }
    dsPtr->string = dsPtr->staticSpace;
    dsPtr->length = 0;
    dsPtr->spaceAvl = TCL_DSTRING_STATIC_SIZE;
    dsPtr->staticSpace[0] = 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_IntegerDiv(interp, v1, tok, v2, result)
 *
 *	*result = v1 (operator) v2
 *
 * Inputs:
 *	interp - current interpreter
 *	v1     - long operand 1
 *      tok    - DIVIDE or MOD
 *      v2     - long operand 2
 *
 * Outputs:
 *      result - return value.  Note that v1, tok, v2 and result form a
 *	quad.
 *
 * Results:
 *	TCL_OK or TCL_ERROR.
 *
 * Side effects:
 *	Possible setting of result error code
 *
 *----------------------------------------------------------------------
 */

int Tcl_IntegerDiv(Tcl_Interp *interp, int v1, int tok, int v2, int *result)
{
    int ret;
    long negative, quot, rem;
    
/*
 * The code below is tricky because C doesn't guarantee
 * much about the properties of the quotient or
 * remainder, but Tcl does:  the remainder always has
 * the same sign as the divisor and a smaller absolute
 * value.
 */

    if (v2 == 0) {
      divideByZero:
	interp->result = "divide by zero";
	Tcl_SetErrorCode(interp, "ARITH", "DIVZERO",
			 interp->result, (char *) NULL);
	ret = TCL_ERROR;
	goto done;
    }
    
    /*
     * The code below is tricky because C doesn't guarantee
     * much about the properties of the quotient or
     * remainder, but Tcl does:  the remainder always has
     * the same sign as the divisor and a smaller absolute
     * value.
     */
    
    negative = 0;
    if (v2 < 0) {
	v2 = -v2;
	v1 = -v1;
	negative = 1;
    }
    quot = v1 / v2;
    rem  = v1 % v2;
    if (rem < 0) {
	rem += v2;
	quot -= 1;
    }
    if (negative) {
	rem = -rem;
    }
    *result = (tok == DIVIDE) ? quot : rem;
    ret = TCL_OK;
  done:
    return ret;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_InvokeTraces(Tcl_Interp *interp, Var *varPtr, char *part1,
 *                  char *part2, int flags, char *err, int trace)
 *
 *	Split a variable name into the array name + index
 *	Needed for the TCL->C translator.  The call to this routine will
 *	be inserted directly into the code.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Modify the string varName and note the parenthesis
 *
 *----------------------------------------------------------------------
 */

int
Tcl_InvokeTraces(Tcl_Interp *interp, Var *varPtr, char *part1,
		 char *part2, int flags, char *errpre,
		 int traceType)
{
    Interp *iPtr = (Interp *) interp;
    int status = TCL_OK;
    Var *arrayPtr;
    
    arrayPtr = varPtr->arrayPtr;
    if ((varPtr->tracePtr != NULL)
	    || ((arrayPtr != NULL) && (arrayPtr->tracePtr != NULL))) {
	char *msg;

	msg = CallTraces(iPtr, arrayPtr, varPtr, part1, part2,
			 (flags & (ITCL_GLOBAL_MASK | PART1_NOT_PARSED)) |
			 traceType);
	if (msg != NULL) {
	    VarErrMsg(interp, part1, part2, errpre, msg);
	    status = TCL_ERROR;
	}
    }
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Itcl_InitProc(Tcl_Interp *interp, CallFrame *frame, int argc, char **argv)
 * Itcl_FinishProc(Tcl_Interp *interp, CallFrame *frame)
 *
 *	Initialize/Finish Tcl procs.
 *	Needed for the TCL->C translator.  The call to this routine will
 *	be inserted directly into the code.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Modify the string varName and note the parenthesis
 *
 *----------------------------------------------------------------------
 */

int Itcl_InitProc(Tcl_Interp           *interp,
		  ClientData           clientData,
		  CallFrame            *frame,
		  int                  argc,
		  char                 **argv,
		  Itcl_ActiveNamespace *nsToken)
{
    int             dummy_i;
    register Interp *iPtr;
    register Proc   *procPtr;
   
    /*
     * Set up a call frame for the new procedure invocation.
     */

    procPtr = (Proc *) clientData;
    *nsToken = Itcl_ActivateNamesp(interp, (Itcl_Namespace)procPtr->namesp);
    if (*nsToken == NULL) {
        return TCL_ERROR;
    }
    iPtr = procPtr->iPtr;
    
    Tcl_InitHashTable(&frame->varTable, TCL_STRING_KEYS);
    if (iPtr->varFramePtr != NULL) {
        frame->level = iPtr->varFramePtr->level+1;
    } else {
        frame->level = 1;
    }
    frame->argc         = argc;
    frame->argv         = argv;
    frame->callerPtr    = iPtr->framePtr;
    frame->callerVarPtr = iPtr->varFramePtr;
    frame->activeNs     = iPtr->activeNs;
    frame->activeData   = iPtr->activeData;
    frame->flags        = 0;
    
    iPtr->framePtr      = frame;
    iPtr->varFramePtr   = frame;
    iPtr->returnCode    = TCL_OK;
   
    return TCL_OK;
}

void Itcl_FinishProc(Tcl_Interp           *interp,
		     ClientData           clientData,
		     CallFrame            *frame,
		     Itcl_ActiveNamespace nsToken)
{
    register Interp *iPtr = (Interp*) interp;
    register Proc   *procPtr;
    
    iPtr->framePtr    = frame->callerPtr;
    iPtr->varFramePtr = frame->callerVarPtr;
    
    /*
     * The check below is a hack.  The problem is that there could be
     * unset traces on the variables, which cause scripts to be evaluated.
     * This will clear the ERR_IN_PROGRESS flag, losing stack trace
     * information if the procedure was exiting with an error.  The
     * code below preserves the flag.  Unfortunately, that isn't
     * really enough:  we really should preserve the errorInfo variable
     * too (otherwise a nested error in the trace script will trash
     * errorInfo).  What's really needed is a general-purpose
     * mechanism for saving and restoring interpreter state.
     */

    if (iPtr->flags & ERR_IN_PROGRESS) {
        TclDeleteVars(iPtr, &frame->varTable);
        iPtr->flags |= ERR_IN_PROGRESS;
    } else {
        TclDeleteVars(iPtr, &frame->varTable);
    }
    Itcl_DeactivateNamesp(interp,nsToken);
    return;
}

int Tclc_ErrorCProc(Tcl_Interp *interp,
		    char       *procName,
		    char       *errFile,
		    int        errLine,
		    int        curResult)
{
    int    result;
    Interp *iPtr = (Interp *) interp;
    
    if (curResult == TCL_RETURN)
	result = TclUpdateReturnInfo(iPtr);
    else if (curResult == TCL_ERROR) {
        char msg[100];

        /*
         * Record information telling where the error occurred.
         */

	if (errLine > 0) 
	    sprintf(msg, "tcl error in compiled file \"%.50s\" at line %d",
		    errFile, errLine);
	else 
	    sprintf(msg, "tcl error in compiled file \"%.50s\" <no line info>",
		    errFile);

	Tcl_AddErrorInfo(interp, msg);
        sprintf(msg, "\n    (procedure \"%.50s\")", procName);
        Tcl_AddErrorInfo(interp, msg);
	result = TCL_ERROR;
    } else if (curResult == TCL_BREAK) {
	Tcl_ResetResult(interp);
	iPtr->result = "invoked \"break\" outside of a loop";
        result = TCL_ERROR;
    } else if (curResult == TCL_CONTINUE) {
	Tcl_ResetResult(interp);
        iPtr->result = "invoked \"continue\" outside of a loop";
        result = TCL_ERROR;
    } else result = TCL_OK;
    return result;
}

void Tclc_ErrReadVar(Tcl_Interp *interp, char *part1, char *part2)
{
    char msg[1024];
    if (!part2)
	 sprintf(msg, "can't read \"%s\": no such variable", part1);
    else sprintf(msg, "can't read \"%s(%s)\": no such variable", part1, part2);
    Tcl_SetResult(interp, msg, TCL_STATIC);
}

void Tclc_ErrArray(Tcl_Interp *interp, char *part1)
{
    char msg[1024];
    sprintf(msg, "can't set \"%s\": variable is an array", part1);
    Tcl_SetResult(interp, msg, TCL_STATIC);
}

void Tclc_ErrCatch(Tcl_Interp *interp)
{
    Tcl_SetResult(interp, "can't evaluate catch_c code because \
-nocatcheval flag was given during compilation. Report error to appropriate \
party.", TCL_STATIC);
}

void Tclc_ErrUplevel(Tcl_Interp *interp)
{
    Tcl_SetResult(interp, "can't evaluate uplevel_c code because \
-noupleveleval flag was given during compilation. Report error to \
appropriate party.", TCL_STATIC);
}

/*
 *----------------------------------------------------------------------
 *
 * Itcl_InitUplevel(Tcl_Interp *interp, CallFrame *frame, CallFrame *sFrame,
 *                  Itcl_ActiveNamespace *nsToken)
 * Itcl_FinishUplevel(Tcl_Interp *interp, CallFrame *sFrame,
 *                    Itcl_ActiveNamespace *nsToken)
 *
 *	Initialize/Finish the uplevel command.
 *	Needed for the TCL->C translator.  The call to this routine will
 *	be inserted directly into the code.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Modify the string varName and note the parenthesis
 *
 *----------------------------------------------------------------------
 */

int Itcl_InitUplevel(Tcl_Interp           *interp,
		     CallFrame            *framePtr,
		     CallFrame            **savedVarFramePtrPtr,
		     Itcl_ActiveNamespace *nsToken)
{
    Itcl_Namespace  ns;
    ClientData      cdata;
    register Interp *iPtr = (Interp *) interp;
   
    /*
     * Set up upleveled name space
     */

    /*itcl*/
    if (framePtr == NULL) {
        ns = (Itcl_Namespace)iPtr->globalNs;
        cdata = NULL;
    } else {
        ns = (Itcl_Namespace)framePtr->activeNs;
        cdata = framePtr->activeData;
    }
  
    *savedVarFramePtrPtr = iPtr->varFramePtr;
    iPtr->varFramePtr = framePtr;

    /*itcl*/
    *nsToken = Itcl_ActivateNamesp2(interp, ns, cdata);
    if (*nsToken == NULL) return TCL_ERROR;
    
    return TCL_OK;
}

void Itcl_FinishUplevel(Tcl_Interp           *interp,
			CallFrame            *savedVarFramePtr,
			Itcl_ActiveNamespace nsToken)
{
    register Interp *iPtr = (Interp*) interp;
    
    /*
     * Restore the variable frame, and return.
     */
    
    iPtr->varFramePtr = savedVarFramePtr;
    Itcl_DeactivateNamesp(interp,nsToken);
    return;
}

/*
 *----------------------------------------------------------------------
 *
 * int Tcl_GetFlags(Tcl_Interp *interp)
 * int Tcl_GetEvalFlags(Tcl_Interp *interp)
 *
 *	Get fields of Interp.  This is so we do not need to include
 *	tclInt.h in the compiler.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Modify the string varName and note the parenthesis
 *
 *----------------------------------------------------------------------
 */

int Tcl_GetFlags(Tcl_Interp *interp)
{
    Interp *iPtr = (Interp *) interp;
    return iPtr->evalFlags;
}
/*
 */

void
Tcl_LoopOverEntries(Tcl_HashTable *table, Tcl_HashTableFunc *tf)
{
  Tcl_HashSearch search;
  Tcl_HashEntry  *hPtr;

  if (table) {
    for (hPtr = Tcl_FirstHashEntry(table, &search); hPtr != NULL;
	 hPtr = Tcl_NextHashEntry(&search)) {

      if (hPtr) (*tf)(hPtr);
    }
  }
}

void
Tcl_LoopOverEntries_1Arg(Tcl_HashTable *table, Tcl_HashTableFunc_1Arg *tf,
			 void *arg_1)
{
  Tcl_HashSearch search;
  Tcl_HashEntry  *hPtr;

  if (table) {
    for (hPtr = Tcl_FirstHashEntry(table, &search); hPtr != NULL;
         hPtr = Tcl_NextHashEntry(&search)) {

      if (hPtr) (*tf)(hPtr, arg_1);
    }
  }
}

char *Tcl_ReturnHashKey(Tcl_HashTable *tablePtr, Tcl_HashEntry *hPtr)
{
	return Tcl_GetHashKey(tablePtr, hPtr);
}
