/*
 *	tclToC.h
 *
 *	List of general definitions for the tcl -> C parser.  Intended to be
 *	included in tclInt.h
 *
 */

/*
	Name:	Forest Rouse
	Date:	5/6/94
	Copyright PMAC 1994/ICEM CFD 1995
*/

#ifndef  _TCLTOCPROTO_H
#define _TCLTOCPROTO_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Prototypes
 */

/*
 *----------------------------------------------------------------
 * Tcl To C Utilities.  Used by TCL --> C translated code.
 *----------------------------------------------------------------
 */

extern void		Tcl_AppendDStrings _ANSI_ARGS_((Tcl_DString *d1,
							Tcl_DString *d2));

extern int		Itcl_InitProc   _ANSI_ARGS_((Tcl_Interp *interp,
						     ClientData clientData,
						     CallFrame  *frame,
						     int        argc,
						     char       **argv,
						     Itcl_ActiveNamespace *n));
						   
extern int		Itcl_InitUplevel
     				_ANSI_ARGS_((Tcl_Interp *interp,
					      CallFrame *framePtr,
					      CallFrame **svfPtrPtr,
					      Itcl_ActiveNamespace *n));

extern int		Tcl_IntegerDiv _ANSI_ARGS_((Tcl_Interp *interp,
						    int v1, int tok, int v2,
						    int *result));

extern int		Tcl_InvokeTraces _ANSI_ARGS_((Tcl_Interp *i, Var *v,
						      char *p1, char *p2,
						      int f, char *e, int t));

extern void             Itcl_FinishProc _ANSI_ARGS_((Tcl_Interp *interp,
						     ClientData clientData,
						     CallFrame  *frame,
						     Itcl_ActiveNamespace n));

extern void             Itcl_FinishUplevel
    				_ANSI_ARGS_((Tcl_Interp *interp,
					     CallFrame  *savedVarFramePtr,
					     Itcl_ActiveNamespace n));

extern int              Tcl_OvrLoadResult _ANSI_ARGS_((Tcl_Interp *interp));
extern void             Tcl_DStringResOvr _ANSI_ARGS_((Tcl_Interp *interp));

extern int		Tcl_CheckEquivalence _ANSI_ARGS_((Var *v1, Var *v2));

/*
 *----------------------------------------------------------------
 * Tcl Variable Utilities.  Used by TCL --> C translated code along with
 *			    internal tclVar routines.
 *----------------------------------------------------------------
 */

extern void		Tcl_tVarInit       _ANSI_ARGS_((tVar *aVar));
extern int 		Tcl_MaintainValid  _ANSI_ARGS_((Tcl_Interp *interp,
							Var *v));

extern void		Tcl_OnlyValidVar   _ANSI_ARGS_((Var *aVar,
							TypeAttribute type));

extern void		Tcl_OnlyValidValue _ANSI_ARGS_((AllValue      *vals,
							TypeAttribute *attr,
							TypeAttribute type));

extern void		Tcl_AsValidVar   _ANSI_ARGS_((Var *aVar,
						      TypeAttribute type));

extern void		Tcl_ResetValue   _ANSI_ARGS_((AllValue      *vals,
						      TypeAttribute *attr));
/*
 * Conversion/value return routines
 */

extern char *		Tcl_StringValue      _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar,
							  int *status));
extern int		Tcl_NumericValue     _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar));
extern int		Tcl_IntegerValue     _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar,
							  int *status));
extern double		Tcl_DoubleValue      _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar,
							  int *status));
extern Tcl_C_List *	Tcl_ListValue        _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar,
							  int *status));
extern Tcl_DString *	Tcl_rDStringValue    _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar,
							  int *status));

extern int		Tcl_ConvertToString  _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar));
extern int		Tcl_ConvertToInteger _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar));
extern int   		Tcl_ConvertToDouble  _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar));
extern int		Tcl_ConvertToList    _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar));
extern int		Tcl_ConvertToDString _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar));
extern int 		Tcl_ConvertVar       _ANSI_ARGS_((Tcl_Interp *interp,
							  Var *aVar,
							  TypeAttribute tt));

/*
 *----------------------------------------------------------------
 *	Result conversion routines
 *----------------------------------------------------------------
 */

extern char *		Tcl_rStringResult  _ANSI_ARGS_((Interp *iPtr));
extern int		Tcl_rIntegerResult _ANSI_ARGS_((Interp *iPtr));
extern double		Tcl_rDoubleResult  _ANSI_ARGS_((Interp *iPtr));
extern Tcl_DString *	Tcl_rDStringResult _ANSI_ARGS_((Interp *iPtr));
extern Tcl_C_List *	Tcl_rListResult    _ANSI_ARGS_((Interp *iPtr));

/*
 *----------------------------------------------------------------
 *	Variable conversion routines
 *----------------------------------------------------------------
 */

extern int 		Tcl_ConvertFromStringToInteger _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromStringToDouble  _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromStringToList    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromStringToDString    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));

extern int 		Tcl_ConvertFromDStringToString    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromDStringToInteger _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromDStringToDouble  _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromDStringToList    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));

extern int 		Tcl_ConvertFromIntegerToString _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromIntegerToDouble _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromIntegerToList   _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromIntegerToDString    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));

extern int 		Tcl_ConvertFromDoubleToString  _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromDoubleToInteger _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromDoubleToList    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromDoubleToDString    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));

extern int 		Tcl_ConvertFromListToString    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromListToInteger   _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromListToDouble    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromListToDString    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));

extern int 		Tcl_ConvertFromUnknownToString  _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromUnknownToInteger _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromUnknownToDouble  _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromUnknownToList    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int 		Tcl_ConvertFromUnknownToDString    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));
extern int		Tcl_ConvertUnknownToNumeric    _ANSI_ARGS_((
				Tcl_Interp *i, AllValue *v, TypeAttribute *t));

/*
 *----------------------------------------------------------------
 *	List processing
 *----------------------------------------------------------------
 */

extern void		Tcl_DStringSetAlloc  _ANSI_ARGS_((Tcl_DString *dsPtr,
							  int length));

extern int		Tcl_ListAppendElement
    					     _ANSI_ARGS_((Tcl_Interp  *interp,
							  Tcl_C_List  *list,
							  char        *string,
							  Tcl_DString *sL));

extern int		Tcl_MergeList        _ANSI_ARGS_((Tcl_Interp  *interp,
							  Tcl_C_List  *list,
							  const char  *js,
							  Tcl_DString *sl));

#ifdef USEVARARGS
extern void		Tcl_ConcatList _ANSI_ARGS_(VARARGS(Tcl_C_List *l));
#else
extern void		Tcl_ConcatList (Tcl_C_List *first, ...);
#endif


extern void		Tcl_AppendList       _ANSI_ARGS_((Tcl_C_List *aList1,
							  Tcl_C_List *aList2,
							  int        bIndex,
							  int        nAppElm));

extern void		Tcl_ListSetSize      _ANSI_ARGS_((Tcl_C_List *list,
							  int nElements));

extern void		Tcl_ListAllocSize    _ANSI_ARGS_((Tcl_C_List *list,
							  int nElements));

/*
 *----------------------------------------------------------------
 *	Creation/Deletion/Initialization routines
 *----------------------------------------------------------------
 */

extern void		Tcl_VarValueCreate _ANSI_ARGS_((Var *aVar));
extern void		Tcl_AllValueCreate _ANSI_ARGS_((
				AllValue *aValue, TypeAttribute *typeAttr));

extern void		Tcl_DeleteValueVar _ANSI_ARGS_((Var *aVar));
extern void             Tcl_DeleteValue    _ANSI_ARGS_((AllValue      *vals,
							TypeAttribute *attr,
							TypeAttribute type));

#ifdef __cplusplus
}
#endif

#endif
