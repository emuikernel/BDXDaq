/*
 *	tclToC.h
 *
 *	List of general definitions for the tcl -> C parser
 *
 */

/*
	Name:	Forest Rouse
	Date:	5/6/94
	Copyright PMAC 1994/ICEM CFD 1995
*/

#ifndef  _TCLTOC_H
#define _TCLTOC_H

#include "tcl.h"
#ifdef NO_LIMITS_H
#   include "compat/limits.h"
#else
#   include <limits.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Constants
 */

#define TCL_TOC_OK   1
#define TCL_TOC_FAIL 0
#define MAX_TCL_SCRIPT 4196
#define TCL_TEMP_BUF 1024

#define TCL_STR_BUF  128	/* alloc strings in block of this */

/*
 * defines of TypeAttribute as integers
 */

#define TCL_TYPE_STRING  1		/* string result */
#define TCL_TYPE_INTEGER 2		/* integer */
#define TCL_TYPE_DOUBLE  4		/* double */
#define TCL_TYPE_LIST    8		/* a list */
#define TCL_TYPE_DSTRING 0x10		/* dynamic string */
    
    /* Reserve for further types FLAGS 0x20 - 0x1000 */
    			
#define TCL_TYPE_STRLIST 0x02000	/* List rep == String rep */
#define TCL_TYPE_FSTRING 0x04000	/* free "constant" string */
#define TCL_TYPE_NONE    0x08000	/* Unknown type */
#define TCL_TYPE_TEMP    0x10000	/* Temporary variable */
#define TCL_TYPE_RETSTR  0x20000	/* value in return string */
#define TCL_TYPE_RETOVR  0x40000 	/* value in overloaded return */

#define TCL_TYPE_BASIC (TCL_TYPE_STRING | TCL_TYPE_INTEGER | TCL_TYPE_DOUBLE\
			| TCL_TYPE_LIST | TCL_TYPE_DSTRING)/* basic types */
#define TCL_TYPE_NUMERIC (TCL_TYPE_INTEGER | TCL_TYPE_DOUBLE)
#define TCL_TYPE_CONVSTR (TCL_TYPE_STRING  | TCL_TYPE_DSTRING)
#define TCL_TYPE_STRINGY (TCL_TYPE_STRING  | TCL_TYPE_DSTRING | TCL_TYPE_LIST)

#define TCL_BASIC_MASK (TCL_TYPE_BASIC | TCL_TYPE_FSTRING)
/*
 * The structures below defines the type attributes and all possible TCL
 * values of a variable as a C structure.
 */

/*
 * Tcl to C list structure.  Maintain this structure when required to
 * coerce a string into a list
 */

#define LIST_STATIC_SPACE 10
#define LIST_BLOCK_ALLOC  10

typedef struct Tcl_C_List {
	int         nAlloc;	/* number of allocated strings */
	int         nElem;	/* number of list elements */
	int         *argv;	/* offsets to list elements */
	int         *flags;	/* ScanElement flag for this element */
	int	    *len;	/* length of this element */
	Tcl_DString list;	/* encoded list as a string */
	
	int         static_argv_space[LIST_STATIC_SPACE];
	int	    static_flags_space[LIST_STATIC_SPACE];
	int	    static_len_space  [LIST_STATIC_SPACE];
} Tcl_C_List;

/*
 * All valid values for a variable.  All valid values are noted in the
 * TypeAttribute structure.  Native type is preserved when variable is
 * an lvalue.  All other types made invalid and will be deallocated.
 */

typedef struct AllValue {	/* all possible values in parallel */
	char        *string;	/* String value of variable, used for scalar
				 * variables and array elements.  Malloc-ed. */
	int         integer;	/* value as integer */
	double      doub;	/* as double */
	Tcl_C_List  list;	/* as Tcl C representation of a list */
	Tcl_DString dString;	/* Dynamic string */
} AllValue;

/*
 * Conversion Attributes.  Address of functions required to convert
 * from given type to another
 */

typedef int (Tcl_ConvFunction) _ANSI_ARGS_((Tcl_Interp    *interp,
					    AllValue      *allValue,
					    TypeAttribute *typeAttr));
typedef struct ConvAttribute {
	Tcl_ConvFunction *string;	/* from given type to string */
	Tcl_ConvFunction *integer;	/* from given type to integer */
	Tcl_ConvFunction *doub;		/* from given type to double */
	Tcl_ConvFunction *list;		/* from given type to list */
	Tcl_ConvFunction *dString;	/* from type to dynamic string */
} ConvAttribute;

typedef void (Tcl_HashTableFunc) _ANSI_ARGS_((Tcl_HashEntry *h));
typedef void (Tcl_HashTableFunc_1Arg) _ANSI_ARGS_((Tcl_HashEntry *h, void *a));

/*
 * Macros that set types structure
 */

#define TCL_SETONLYVALID(types, type) *(types) = (type)
#define TCL_SETASVALID(types, type)   *(types) |= (type)
#define TCL_ERASEVALID(types, type)   *(types) &= ~(type)

/*
 * The following structure defines a temporary variable used in 
 * the TCL->C translator.  It is a simpiler version of the VAR
 * structure.  Also, two useful creation/deletion macros
 * are defined here.
 */

typedef struct tVar {
    TypeAttribute  typeAttr;	/* type of variable */
    AllValue       allValue;	/* all possible valid TCL types */
} tVar;

typedef int address_nat;
typedef unsigned char FLAG;

#define TVAR_CREATE(aVar, type) \
(aVar).typeAttr = (type) | TCL_TYPE_TEMP

#ifdef __NOT_COMPRESS__
#define TCL_ONLYVALIDVALUE Tcl_OnlyValidValueMACRO
#define TCL_RESETVALUE     Tcl_ResetValueMACRO
#define TCL_DELETEVALUE    Tcl_DeleteValueMACRO

#define TCL_ONLYVALIDVAR   Tcl_OnlyValidVarMACRO
#define TCL_ASVALIDVAR     Tcl_AsValidVarMACRO
#define TCL_DELETEVALUEVAR Tcl_DeleteValueVarMACRO
#define TVAR_INIT          Tcl_tVarInitMACRO
#else
#define TCL_ONLYVALIDVALUE Tcl_OnlyValidValue
#define TCL_RESETVALUE     Tcl_ResetValue
#define TCL_DELETEVALUE    Tcl_DeleteValue

#define TCL_ONLYVALIDVAR   Tcl_OnlyValidVar
#define TCL_ASVALIDVAR     Tcl_AsValidVar
#define TCL_DELETEVALUEVAR Tcl_DeleteValueVar
#define TVAR_INIT          Tcl_tVarInit
#endif

/*	Define the validation/deletion routines in terms of the
	above macro.  This allows the user to compress the size of
	the compiled code easily ... */

#define Tcl_tVarInitMACRO(aVar) \
(aVar)->typeAttr = 0; \
Tcl_initAllValueMACRO((&(aVar)->allValue))

#define Tcl_initAllValueMACRO(av) \
(av)->string = (char *) NULL; (av)->integer = 0; (av)->doub = 0.0; \
Tcl_ListInit(&((av)->list)); \
Tcl_DStringInit(&((av)->dString))

/*	Operations on temporary and Tcl variables ... */

#define TCL_VARSTRINGVALUE(aVar, str) \
if ((aVar)->varAttr.typeAttr & TCL_TYPE_DSTRING) \
(str) = Tcl_DStringValue(&((aVar)->value.allValue.dString)); \
else if ((aVar)->varAttr.typeAttr & TCL_TYPE_STRING) \
(str) = (aVar)->value.allValue.string

#define Tcl_OnlyValidVarMACRO(aVar, aType) \
Tcl_OnlyValidValueMACRO(&((aVar)->value.allValue), \
			&((aVar)->varAttr.typeAttr), (aType)); \
(aVar)->varAttr.convAttr = (ConvAttribute *) NULL; \
Tcl_AsValidVarMACRO((aVar), (aType)); \

#define Tcl_AsValidVarMACRO(aVar, aType) \
TCL_SETASVALID(&((aVar)->varAttr.typeAttr), (aType)); \
if ((aType) & TCL_TYPE_INTEGER || (aType) & TCL_TYPE_DOUBLE \
    || !(aVar)->varAttr.convAttr)\
(aVar)->varAttr.convAttr = Tcl_ReturnConversion((aType))

#define VAR_RESET(aVar) \
TCL_RESETVALUE(&((aVar)->value.allValue), &((aVar)->varAttr.typeAttr)); \
aVar->varAttr.convAttr = NULL;
	       
#define VAR_DELETENOTTYPE(aVar, aType) \
TCL_ONLYVALIDVALUE(&((aVar)->value.allValue),\
		   &((aVar)->varAttr.typeAttr),\
		   (((aVar)->varAttr.typeAttr)&(aType)))

#define Tcl_DeleteValueVarMACRO(aVar) \
Tcl_DeleteValueMACRO(&((aVar)->value.allValue), \
		     &((aVar)->varAttr.typeAttr), 0); \
(aVar)->varAttr.convAttr = NULL

/*	Operations on temporary variables ... */

#define TVAR_RESET(aTVar) \
TCL_RESETVALUE(&((aTVar)->allValue), &((aTVar)->typeAttr))

#define TVAR_DELETENOTTYPE(aTVar, aType) \
TCL_ONLYVALIDVALUE(&((aTVar)->allValue), &((aTVar)->typeAttr), \
		   TCL_TYPE_TEMP|(((aTVar)->typeAttr)&(aType)))

#define TVAR_DELETE(aTVar) \
TCL_DELETEVALUE(&((aTVar)->allValue), &((aTVar)->typeAttr), TCL_TYPE_TEMP)

/*	Value Macros ... */

#define Tcl_ResetValueMACRO(values, types) \
if (*(types)&TCL_TYPE_STRING && *(types)&TCL_TYPE_FSTRING) { \
    ckfree((values)->string); (values)->string = (char *) NULL; \
} \
(values)->list.nElem=(values)->list.list.length=(values)->dString.length=0; \
(values)->list.list.string[0] = (values)->dString.string[0] = '\0'; \
*(types) = 0

#define Tcl_DeleteValueMACRO(values, types, aType) \
if (*(types)&TCL_TYPE_STRING&&*(types)&TCL_TYPE_FSTRING) {\
    ckfree((values)->string); (values)->string = (char *) NULL;\
}\
if (*(types)&TCL_TYPE_LIST) Tcl_ListDelete(&((values)->list));\
if (*(types)&TCL_TYPE_DSTRING)Tcl_DStringFree(&((values)->dString));\
TCL_SETONLYVALID((types), (aType))

/*	Setting variables/temporaries as valid ... */

#define Tcl_OnlyValidValueMACRO(values, types, aType) \
if (*(types)&TCL_TYPE_STRING && *(types)&TCL_TYPE_FSTRING && \
!((aType) & TCL_TYPE_STRING)) { \
    ckfree((values)->string); (values)->string = (char *) NULL; \
} \
if (*(types)&TCL_TYPE_LIST && !((aType) & TCL_TYPE_LIST)) { \
    (values)->list.nElem = (values)->list.list.length = 0; \
    (values)->list.list.string[0] = '\0'; \
} \
if (*(types)&TCL_TYPE_DSTRING && !((aType)&TCL_TYPE_DSTRING)) { \
    (values)->dString.length = 0; (values)->dString.string[0] = '\0'; \
} \
TCL_SETONLYVALID((types), (aType))

/*
 * Prototypes are in tclToCProto.h.  Needs to included at end of
 * tclInt.h because of Var, and CallFrame structure definitions
 */

      /* Public functions ... */
      
extern void		Tcl_ConvertDoubleToBuffer _ANSI_ARGS_((char *fmt,
							       double doub,
							       char *buf));

extern int		Tcl_ParseNumeric _ANSI_ARGS_((Tcl_Interp *interp,
						      AllValue   *aValue,
		 				      TypeAttribute *typeAttr,
						      TypeAttribute *type));

extern int		Tcl_ParseString _ANSI_ARGS_((Tcl_Interp    *interp,
						     char          *p,
						     int           *i,
						     double        *d,
						     TypeAttribute *t));

extern void		Tcl_ListInit         _ANSI_ARGS_((Tcl_C_List *l));

extern void		Tcl_ListDelete       _ANSI_ARGS_((Tcl_C_List *aList));

extern ConvAttribute *	Tcl_ReturnConversion _ANSI_ARGS_((TypeAttribute t));

extern char *		Tcl_SplitArray _ANSI_ARGS_((char *varName,
						    char **open,
						    char **close));

extern int		Tcl_SplitToList      _ANSI_ARGS_((Tcl_Interp *interp,
							  Tcl_C_List *l,
							  char *str));


extern int      	Tcl_TestInit _ANSI_ARGS_((Tcl_Interp *interp));

extern void             Tcl_LoopOverEntries _ANSI_ARGS_((Tcl_HashTable *t,
			    Tcl_HashTableFunc *tf));

extern void             Tcl_LoopOverEntries_1Arg _ANSI_ARGS_((
			    Tcl_HashTable *t, Tcl_HashTableFunc_1Arg *f,
			    void *arg1));

extern char *		Tcl_ReturnHashKey _ANSI_ARGS_((Tcl_HashTable *t,
						       Tcl_HashEntry *hPtr));

extern int 		Tclc_ErrorCProc _ANSI_ARGS_((Tcl_Interp *interp,
						     char       *procName,
						     char       *errFile,
						     int        errLine,
						     int        curResult));

extern void		Tclc_ErrReadVar _ANSI_ARGS_((Tcl_Interp *interp,
						     char       *part1,
						     char       *part2));
extern void		Tclc_ErrArray _ANSI_ARGS_((Tcl_Interp *interp,
						     char       *part1));
extern void		Tclc_ErrCatch _ANSI_ARGS_((Tcl_Interp *interp));
extern void		Tclc_ErrUplevel _ANSI_ARGS_((Tcl_Interp *interp));
#ifdef __cplusplus
}
#endif

#endif
