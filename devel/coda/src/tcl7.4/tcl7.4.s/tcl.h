/*
 * tcl.h --
 *
 *	This header file describes the externally-visible facilities
 *	of the Tcl interpreter.
 *
 * Copyright (c) 1987-1994 The Regents of the University of California.
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) tcl.h 1.153 95/06/27 15:42:31
 *
 * ========================================================================
 * >>>>>>>>>>>>>>>> INCLUDES MODIFICATIONS FOR [incr Tcl] <<<<<<<<<<<<<<<<<
 *
 *  AUTHOR:  Michael J. McLennan       Phone: (610)712-2842
 *           AT&T Bell Laboratories   E-mail: michael.mclennan@att.com
 *     RCS:  $Id: tcl.h,v 1.1.1.1 1996/08/21 19:30:14 heyes Exp $
 * ========================================================================
 *             Copyright (c) 1993-1995  AT&T Bell Laboratories
 * ------------------------------------------------------------------------
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef _TCL
#define _TCL

#ifndef BUFSIZ
#  ifdef VXWORKS
#    define __INCstdargh
#  endif

#ifdef USEVARARGS
#include <varargs.h>
#else
#include <stdarg.h>
#endif

#  include <stdio.h>
#endif

#define TCL_VERSION "7.4"
#define TCL_MAJOR_VERSION 7
#define TCL_MINOR_VERSION 4

/*
 * Definitions that allow this header file to be used either with or
 * without ANSI C features like function prototypes.
 */

#undef _ANSI_ARGS_
#undef CONST
#if ((defined(__STDC__) || defined(SABER)) && !defined(NO_PROTOTYPE)) || defined(__cplusplus)
#   define _USING_PROTOTYPES_ 1
#   define _ANSI_ARGS_(x)	x
#   define CONST const
#   ifdef __cplusplus
#       define VARARGS(first) (first, ...)
#   else
#       define VARARGS(first) ()
#   endif
#else
#   define _ANSI_ARGS_(x)	()
#   define CONST
#endif

#define EXTERN extern

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Macro to use instead of "void" for arguments that must have
 * type "void *" in ANSI C;  maps them to type "char *" in
 * non-ANSI systems.
 */

#ifndef VOID
#   ifdef __STDC__
#       define VOID void
#   else
#       define VOID char
#   endif
#endif

/*
 * Miscellaneous declarations (to allow Tcl to be used stand-alone,
 * without the rest of Sprite).
 */

#ifndef NULL
#define NULL 0
#endif

#ifndef _CLIENTDATA
#   if defined(__STDC__) || defined(__cplusplus)
    typedef void *ClientData;
#   else
    typedef int *ClientData;
#   endif /* __STDC__ */
#define _CLIENTDATA
#endif

/*
 * Data structures defined opaquely in this module.  The definitions
 * below just provide dummy types.  A few fields are made visible in
 * Tcl_Interp structures, namely those for returning string values.
 * Note:  any change to the Tcl_Interp definition below must be mirrored
 * in the "real" definition in tclInt.h.
 */

typedef struct Tcl_Interp
{
    char *result;		/* Points to result string returned by last
				 * command. */
    void (*freeProc) _ANSI_ARGS_((char *blockPtr));
				/* Zero means result is statically allocated.
				 * If non-zero, gives address of procedure
				 * to invoke to free the result.  Must be
				 * freed by Tcl_Eval before executing next
				 * command. */
    int errorLine;		/* When TCL_ERROR is returned, this gives
				 * the line number within the command where
				 * the error occurred (1 means first line). */
} Tcl_Interp;

typedef int *Tcl_Trace;
typedef int *Tcl_Command;
typedef struct Tcl_AsyncHandler_ *Tcl_AsyncHandler;
typedef struct Tcl_RegExp_ *Tcl_RegExp;

/*
 * When a TCL command returns, the string pointer interp->result points to
 * a string containing return information from the command.  In addition,
 * the command procedure returns an integer value, which is one of the
 * following:
 *
 * TCL_OK		Command completed normally;  interp->result contains
 *			the command's result.
 * TCL_ERROR		The command couldn't be completed successfully;
 *			interp->result describes what went wrong.
 * TCL_RETURN		The command requests that the current procedure
 *			return;  interp->result contains the procedure's
 *			return value.
 * TCL_BREAK		The command requests that the innermost loop
 *			be exited;  interp->result is meaningless.
 * TCL_CONTINUE		Go on to the next iteration of the current loop;
 *			interp->result is meaningless.
 */

#define TCL_OK		0
#define TCL_ERROR	1
#define TCL_RETURN	2
#define TCL_BREAK	3
#define TCL_CONTINUE	4
#define TCL_RETURN_OK   5

#define TCL_RESULT_SIZE 200

/*
 * Argument descriptors for math function callbacks in expressions:
 */

typedef enum {TCL_INT, TCL_DOUBLE, TCL_EITHER} Tcl_ValueType;

typedef struct Tcl_Value {
    Tcl_ValueType type;		/* Indicates intValue or doubleValue is
				 * valid, or both. */
    long intValue;		/* Integer value. */
    double doubleValue;		/* Double-precision floating value. */
} Tcl_Value;

/*
 * Procedure types defined by Tcl:
 */

typedef int (Tcl_AppInitProc) _ANSI_ARGS_((Tcl_Interp *interp));
typedef int (Tcl_AsyncProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int code));
typedef void (Tcl_CmdDeleteProc) _ANSI_ARGS_((ClientData clientData));
typedef int (Tcl_CmdProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));
typedef void (Tcl_CmdTraceProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int level, char *command, Tcl_CmdProc *proc,
	ClientData cmdClientData, int argc, char *argv[]));
typedef void (Tcl_FreeProc) _ANSI_ARGS_((char *blockPtr));
typedef void (Tcl_InterpDeleteProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp));
typedef int (Tcl_MathProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, Tcl_Value *args, Tcl_Value *resultPtr));
typedef char *(Tcl_VarTraceProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, char *part1, char *part2, int flags));

/*
 * The structure returned by Tcl_GetCmdInfo and passed into
 * Tcl_SetCmdInfo:
 */

typedef struct Tcl_CmdInfo {
    Tcl_CmdProc *proc;			/* Procedure that implements command. */
    ClientData clientData;		/* ClientData passed to proc. */
    Tcl_CmdDeleteProc *deleteProc;	/* Procedure to call when command
					 * is deleted. */
    ClientData deleteData;		/* Value to pass to deleteProc (usually
					 * the same as clientData). */
} Tcl_CmdInfo;


/*
 * The structure defined below is used to hold dynamic strings.  The only
 * field that clients should use is the string field, and they should
 * never modify it.
 */

#define TCL_DSTRING_STATIC_SIZE 200
typedef struct Tcl_DString {
    char *string;		/* Points to beginning of string:  either
				 * staticSpace below or a malloc'ed array. */
    int length;			/* Number of non-NULL characters in the
				 * string. */
    int spaceAvl;		/* Total number of bytes available for the
				 * string and its terminating NULL char. */
    char staticSpace[TCL_DSTRING_STATIC_SIZE];
				/* Space to use in common case where string
				 * is small. */
} Tcl_DString;

#define Tcl_DStringLength(dsPtr) ((dsPtr)->length)
#define Tcl_DStringValue(dsPtr) ((dsPtr)->string)
#define Tcl_DStringTrunc Tcl_DStringSetLength

/*
 * Definitions for the maximum number of digits of precision that may
 * be specified in the "tcl_precision" variable, and the number of
 * characters of buffer space required by Tcl_PrintDouble.
 */

#define TCL_MAX_PREC 17
#define TCL_DOUBLE_SPACE (TCL_MAX_PREC+10)

/*
 * Flag that may be passed to Tcl_ConvertElement to force it not to
 * output braces (careful!  if you change this flag be sure to change
 * the definitions at the front of tclUtil.c).
 */

#define TCL_DONT_USE_BRACES	1

/*
 * Flag values passed to Tcl_RecordAndEval.
 * WARNING: these bit choices must not conflict with the bit choices
 * for evalFlag bits in tclInt.h!!
 */

#define TCL_NO_EVAL		0x10000
#define TCL_EVAL_GLOBAL		0x20000

/*
 * Special freeProc values that may be passed to Tcl_SetResult (see
 * the man page for details):
 */

#define TCL_VOLATILE	((Tcl_FreeProc *) 1)
#define TCL_STATIC	((Tcl_FreeProc *) 0)
#define TCL_DYNAMIC	((Tcl_FreeProc *) 3)

/*
 * Flag values passed to variable-related procedures.
 */

#define TCL_GLOBAL_ONLY		1
#define TCL_APPEND_VALUE	2
#define TCL_LIST_ELEMENT	4
#define TCL_TRACE_READS		0x10
#define TCL_TRACE_WRITES	0x20
#define TCL_TRACE_UNSETS	0x40
#define TCL_TRACE_DESTROYED	0x80
#define TCL_INTERP_DESTROYED	0x100
#define TCL_LEAVE_ERR_MSG	0x200

/*
 * Types for linked variables:
 */

#define TCL_LINK_INT		1
#define TCL_LINK_DOUBLE		2
#define TCL_LINK_BOOLEAN	3
#define TCL_LINK_STRING		4
#define TCL_LINK_READ_ONLY	0x80

/*
 * Permission flags for files:
 */

#define TCL_FILE_READABLE	1
#define TCL_FILE_WRITABLE	2

/*
 * Creation flag values passed in to TclLookupVar:
 *
 * CRT_PART1 -		1 means create hash table entry for part 1 of
 *			name, if it doesn't already exist.  0 means
 *			return an error if it doesn't exist.
 * CRT_PART2 -		1 means create hash table entry for part 2 of
 *			name, if it doesn't already exist.  0 means
 *			return an error if it doesn't exist.
 */

#define CRT_PART1	1
#define CRT_PART2	2

/*
 * The following additional flag is used internally and passed through
 * to LookupVar to indicate that a procedure like Tcl_GetVar was called
 * instead of Tcl_GetVar2 and the single name value hasn't yet been
 * parsed into an array name and index (if any).
 */

#define PART1_NOT_PARSED	0x10000

/*
 * Flag bits for variables:
 *
 * VAR_ARRAY	-		1 means this is an array variable rather
 *				than a scalar variable.
 * VAR_UPVAR - 			1 means this variable just contains a
 *				pointer to another variable that has the
 *				real value.  Variables like this come
 *				about through the "upvar" and "global"
 *				commands.
 * VAR_UNDEFINED -		1 means that the variable is currently
 *				undefined.  Undefined variables usually
 *				go away completely, but if an undefined
 *				variable has a trace on it, or if it is
 *				a global variable being used by a procedure,
 *				then it stays around even when undefined.
 * VAR_TRACE_ACTIVE -		1 means that trace processing is currently
 *				underway for a read or write access, so
 *				new read or write accesses should not cause
 *				trace procedures to be called and the
 *				variable can't be deleted.
 */

#define VAR_ARRAY		1
#define VAR_UPVAR		2
#define VAR_UNDEFINED		4
#define VAR_TRACE_ACTIVE	0x10

/*
 * The following declarations either map ckalloc and ckfree to
 * malloc and free, or they map them to procedures with all sorts
 * of debugging hooks defined in tclCkalloc.c.
 */

#ifdef TCL_MEM_DEBUG

EXTERN char *		Tcl_DbCkalloc _ANSI_ARGS_((unsigned int size,
			    char *file, int line));
EXTERN int		Tcl_DbCkfree _ANSI_ARGS_((char *ptr,
			    char *file, int line));
EXTERN char *		Tcl_DbCkrealloc _ANSI_ARGS_((char *ptr,
			    unsigned int size, char *file, int line));
EXTERN int		Tcl_DumpActiveMemory _ANSI_ARGS_((char *fileName));
EXTERN void		Tcl_ValidateAllMemory _ANSI_ARGS_((char *file,
			    int line));
#  define ckalloc(x) Tcl_DbCkalloc(x, __FILE__, __LINE__)
#  define ckfree(x)  Tcl_DbCkfree(x, __FILE__, __LINE__)
#  define ckrealloc(x,y) Tcl_DbCkrealloc((x), (y),__FILE__, __LINE__)

#else

#  define ckalloc(x) malloc(x)
#  define ckfree(x)  free(x)
#  define ckrealloc(x,y) realloc(x,y)
#  define Tcl_DumpActiveMemory(x)
#  define Tcl_ValidateAllMemory(x,y)

#endif /* TCL_MEM_DEBUG */

/*
 * Macro to free up result of interpreter.
 */

#define Tcl_FreeResult(interp)					\
    if ((interp)->freeProc != 0) {				\
	if ((interp)->freeProc == (Tcl_FreeProc *) free) {	\
	    ckfree((interp)->result);				\
	} else {						\
	    (*(interp)->freeProc)((interp)->result);		\
	}							\
	(interp)->freeProc = 0;					\
    } \
    Tcl_ResetValueInterp((Tcl_Interp *) interp)

/*
 * Forward declaration of Tcl_HashTable.  Needed by some C++ compilers
 * to prevent errors when the forward reference to Tcl_HashTable is
 * encountered in the Tcl_HashEntry structure.
 */

#ifdef __cplusplus
struct Tcl_HashTable;
#endif

/*
 * Structure definition for an entry in a hash table.  No-one outside
 * Tcl should access any of these fields directly;  use the macros
 * defined below.
 */

typedef struct Tcl_HashEntry {
    struct Tcl_HashEntry *nextPtr;	/* Pointer to next entry in this
					 * hash bucket, or NULL for end of
					 * chain. */
    struct Tcl_HashTable *tablePtr;	/* Pointer to table containing entry. */
    struct Tcl_HashEntry **bucketPtr;	/* Pointer to bucket that points to
					 * first entry in this entry's chain:
					 * used for deleting the entry. */
    ClientData clientData;		/* Application stores something here
					 * with Tcl_SetHashValue. */
    union {				/* Key has one of these forms: */
	char *oneWordValue;		/* One-word value for key. */
	int words[1];			/* Multiple integer words for key.
					 * The actual size will be as large
					 * as necessary for this table's
					 * keys. */
	char string[4];			/* String for key.  The actual size
					 * will be as large as needed to hold
					 * the key. */
    } key;				/* MUST BE LAST FIELD IN RECORD!! */
} Tcl_HashEntry;

/*
 * Structure definition for a hash table.  Must be in tcl.h so clients
 * can allocate space for these structures, but clients should never
 * access any fields in this structure.
 */

#define TCL_SMALL_HASH_TABLE 4
typedef struct Tcl_HashTable {
    Tcl_HashEntry **buckets;		/* Pointer to bucket array.  Each
					 * element points to first entry in
					 * bucket's hash chain, or NULL. */
    Tcl_HashEntry *staticBuckets[TCL_SMALL_HASH_TABLE];
					/* Bucket array used for small tables
					 * (to avoid mallocs and frees). */
    int numBuckets;			/* Total number of buckets allocated
					 * at **bucketPtr. */
    int numEntries;			/* Total number of entries present
					 * in table. */
    int rebuildSize;			/* Enlarge table when numEntries gets
					 * to be this large. */
    int downShift;			/* Shift count used in hashing
					 * function.  Designed to use high-
					 * order bits of randomized keys. */
    int mask;				/* Mask value used in hashing
					 * function. */
    int keyType;			/* Type of keys used in this table. 
					 * It's either TCL_STRING_KEYS,
					 * TCL_ONE_WORD_KEYS, or an integer
					 * giving the number of ints in a
					 */
    Tcl_HashEntry *(*findProc) _ANSI_ARGS_((struct Tcl_HashTable *tablePtr,
	    char *key));
    Tcl_HashEntry *(*createProc) _ANSI_ARGS_((struct Tcl_HashTable *tablePtr,
	    char *key, int *newPtr));
} Tcl_HashTable;

/*
 * Structure definition for information used to keep track of searches
 * through hash tables:
 */

typedef struct Tcl_HashSearch {
    Tcl_HashTable *tablePtr;		/* Table being searched. */
    Tcl_HashEntry *nextEntryPtr;	/* Next entry to be enumerated in the
					 * the current bucket. */
    int nextIndex;			/* Index of next bucket to be
					 * enumerated after present one. */
} Tcl_HashSearch;

/*
 * Acceptable key types for hash tables:
 */

#define TCL_STRING_KEYS		0
#define TCL_ONE_WORD_KEYS	1

/*
 * Macros for clients to use to access fields of hash entries:
 */

#define Tcl_GetHashValue(h) ((h)->clientData)
#define Tcl_SetHashValue(h, value) ((h)->clientData = (ClientData) (value))

/*
 * Macros to use for clients to use to invoke find and create procedures
 * for hash tables:
 */

#define Tcl_FindHashEntry(tablePtr, key) \
	(*((tablePtr)->findProc))(tablePtr, key)
#define Tcl_CreateHashEntry(tablePtr, key, newPtr) \
	(*((tablePtr)->createProc))(tablePtr, key, newPtr)

/*
 * Exported Tcl variables:
 */

#define Tcl_GetHashKey(tablePtr, h) \
    ((char *) (((tablePtr)->keyType == TCL_ONE_WORD_KEYS) ? (h)->key.oneWordValue \
						: (h)->key.string))
EXTERN int		tcl_AsyncReady;
EXTERN void		(*tcl_FileCloseProc) _ANSI_ARGS_((FILE *f));
EXTERN char *		tcl_RcFileName;

/*
 * Exported Tcl procedures:
 */

EXTERN void		Tcl_AddErrorInfo _ANSI_ARGS_((Tcl_Interp *interp,
			    char *message));
EXTERN void		Tcl_AllowExceptions _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN void		Tcl_AppendElement _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string));

#ifdef USEVARARGS
EXTERN void		Tcl_AppendResult _ANSI_ARGS_(VARARGS(Tcl_Interp *i));
#else
EXTERN void		Tcl_AppendResult (Tcl_Interp *first, ...);
#endif

EXTERN int		Tcl_AppInit _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN void		Tcl_AsyncMark _ANSI_ARGS_((Tcl_AsyncHandler async));
EXTERN Tcl_AsyncHandler	Tcl_AsyncCreate _ANSI_ARGS_((Tcl_AsyncProc *proc,
			    ClientData clientData));
EXTERN void		Tcl_AsyncDelete _ANSI_ARGS_((Tcl_AsyncHandler async));
EXTERN int		Tcl_AsyncInvoke _ANSI_ARGS_((Tcl_Interp *interp,
			    int code));
EXTERN char		Tcl_Backslash _ANSI_ARGS_((char *src,
			    int *readPtr));
EXTERN void		Tcl_CallWhenDeleted _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_InterpDeleteProc *proc,
			    ClientData clientData));
EXTERN int		Tcl_CommandComplete _ANSI_ARGS_((char *cmd));
EXTERN char *		Tcl_Concat _ANSI_ARGS_((int argc, char **argv));
EXTERN int		Tcl_ConvertElement _ANSI_ARGS_((char *src,
			    char *dst, int flags));
EXTERN Tcl_Command	Tcl_CreateCommand _ANSI_ARGS_((Tcl_Interp *interp,
			    char *cmdName, Tcl_CmdProc *proc,
			    ClientData clientData, Tcl_CmdDeleteProc *dProc));
EXTERN Tcl_Command	Tcl_CreateCommandNs _ANSI_ARGS_((Tcl_Interp *interp,
			    char *cmdName, Tcl_CmdProc *proc,
			    ClientData clientData, Tcl_CmdDeleteProc *dProc));
EXTERN Tcl_Command	Tcl_CreateCommandFull _ANSI_ARGS_((Tcl_Interp *interp,
			    char *cmdName, Tcl_CmdProc *proc,
			    ClientData clientData, Tcl_CmdDeleteProc *dProc,
			    int procNameSpace));
EXTERN Tcl_Interp *	Tcl_CreateInterp _ANSI_ARGS_((void));
EXTERN void		Tcl_CreateMathFunc _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name, int numArgs, Tcl_ValueType *argTypes,
			    Tcl_MathProc *proc, ClientData clientData));
EXTERN int		Tcl_CreatePipeline _ANSI_ARGS_((Tcl_Interp *interp,
			    int argc, char **argv, int **pidArrayPtr,
			    int *inPipePtr, int *outPipePtr,
			    int *errFilePtr));
EXTERN Tcl_Trace	Tcl_CreateTrace _ANSI_ARGS_((Tcl_Interp *interp,
			    int level, Tcl_CmdTraceProc *proc,
			    ClientData clientData));
EXTERN int		Tcl_DeleteCommand _ANSI_ARGS_((Tcl_Interp *interp,
			    char *cmdName));
EXTERN int		Tcl_DeleteCommand2 _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Command cmd));
EXTERN void		Tcl_DeleteHashEntry _ANSI_ARGS_((
			    Tcl_HashEntry *entryPtr));
EXTERN void		Tcl_DeleteHashTable _ANSI_ARGS_((
			    Tcl_HashTable *tablePtr));
EXTERN void		Tcl_DeleteInterp _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN void		Tcl_DeleteTrace _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Trace trace));
EXTERN void		Tcl_DetachPids _ANSI_ARGS_((int numPids, int *pidPtr));
EXTERN void		Tcl_DontCallWhenDeleted _ANSI_ARGS_((
			    Tcl_Interp *interp, Tcl_InterpDeleteProc *proc,
			    ClientData clientData));
EXTERN char *		Tcl_DStringAppend _ANSI_ARGS_((Tcl_DString *dsPtr,
			    char *string, int length));
EXTERN char *		Tcl_DStringAppendElement _ANSI_ARGS_((
			    Tcl_DString *dsPtr, char *string));
EXTERN void		Tcl_DStringEndSublist _ANSI_ARGS_((Tcl_DString *dsPtr));
EXTERN void		Tcl_DStringFree _ANSI_ARGS_((Tcl_DString *dsPtr));
EXTERN void		Tcl_DStringGetResult _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_DString *dsPtr));
EXTERN void		Tcl_DStringInit _ANSI_ARGS_((Tcl_DString *dsPtr));
EXTERN void		Tcl_DStringResult _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_DString *dsPtr));
EXTERN void		Tcl_DStringSetLength _ANSI_ARGS_((Tcl_DString *dsPtr,
			    int length));
EXTERN void		Tcl_DStringStartSublist _ANSI_ARGS_((
			    Tcl_DString *dsPtr));
EXTERN void		Tcl_EnterFile _ANSI_ARGS_((Tcl_Interp *interp,
			    FILE *file, int permissions));
EXTERN void             Tcl_EnvFinish _ANSI_ARGS_(());
EXTERN char *		Tcl_ErrnoId _ANSI_ARGS_((void));
EXTERN int		Tcl_Eval _ANSI_ARGS_((Tcl_Interp *interp, char *cmd));
EXTERN int		Tcl_EvalFile _ANSI_ARGS_((Tcl_Interp *interp,
			    char *fileName));
EXTERN int		Tcl_ExprBoolean _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, int *ptr));
EXTERN int		Tcl_ExprDouble _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, double *ptr));
EXTERN int		Tcl_ExprLong _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, long *ptr));
EXTERN int		Tcl_ExprString _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string));
EXTERN int		Tcl_FilePermissions _ANSI_ARGS_((FILE *file));
EXTERN Tcl_HashEntry *	Tcl_FirstHashEntry _ANSI_ARGS_((
			    Tcl_HashTable *tablePtr,
			    Tcl_HashSearch *searchPtr));
EXTERN int		Tcl_GetBoolean _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, int *boolPtr));
EXTERN int		Tcl_GetCommandInfo _ANSI_ARGS_((Tcl_Interp *interp,
			    char *cmdName, Tcl_CmdInfo *infoPtr));
EXTERN char *		Tcl_GetCommandName _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Command command));
EXTERN char *		Tcl_GetCommandPath _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Command command));
EXTERN int		Tcl_GetDouble _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, double *doublePtr));
EXTERN int		Tcl_GetInt _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, int *intPtr));
EXTERN int		Tcl_GetOpenFile _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, int write, int checkUsage,
			    FILE **filePtr));
EXTERN char *		Tcl_GetVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, int flags));
EXTERN char *		Tcl_GetVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *part1, char *part2, int flags));
EXTERN int		Tcl_GlobalEval _ANSI_ARGS_((Tcl_Interp *interp,
			    char *command));
EXTERN char *		Tcl_HashStats _ANSI_ARGS_((Tcl_HashTable *tablePtr));
EXTERN int		Tcl_Init _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN void		Tcl_InitHashTable _ANSI_ARGS_((Tcl_HashTable *tablePtr,
			    int keyType));
EXTERN void		Tcl_InitMemory _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN int		Tcl_LinkVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, char *addr, int type));
EXTERN void		Tcl_Main _ANSI_ARGS_((int argc, char **argv,
			    Tcl_AppInitProc *appInitProc));
EXTERN char *		Tcl_Merge _ANSI_ARGS_((int argc, char **argv));
EXTERN Tcl_HashEntry *	Tcl_NextHashEntry _ANSI_ARGS_((
			    Tcl_HashSearch *searchPtr));
EXTERN char *		Tcl_ParseVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, char **termPtr));
EXTERN char *		Tcl_PosixError _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN void		Tcl_FreePosixError _ANSI_ARGS_(());
EXTERN void		Tcl_PrintDouble _ANSI_ARGS_((Tcl_Interp *interp,
			    double value, char *dst));
EXTERN void		Tcl_ReapDetachedProcs _ANSI_ARGS_((void));
EXTERN int		Tcl_RecordAndEval _ANSI_ARGS_((Tcl_Interp *interp,
			    char *cmd, int flags));
EXTERN Tcl_RegExp	Tcl_RegExpCompile _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string));
EXTERN int		Tcl_RegExpExec _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_RegExp regexp, char *string, char *start));
EXTERN int		Tcl_RegExpMatch _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, char *pattern));
EXTERN void		Tcl_RegExpRange _ANSI_ARGS_((Tcl_RegExp regexp,
			    int index, char **startPtr, char **endPtr));
EXTERN void		Tcl_ResetResult _ANSI_ARGS_((Tcl_Interp *interp));
#define Tcl_Return Tcl_SetResult
EXTERN int		Tcl_ScanElement _ANSI_ARGS_((char *string,
			    int *flagPtr));
EXTERN int		Tcl_SetCommandInfo _ANSI_ARGS_((Tcl_Interp *interp,
			    char *cmdName, Tcl_CmdInfo *infoPtr));

#ifdef USEVARARGS
EXTERN void		Tcl_SetErrorCode _ANSI_ARGS_(VARARGS(Tcl_Interp *i));
#else
EXTERN void		Tcl_SetErrorCode (Tcl_Interp *first, ...);
#endif

EXTERN int		Tcl_SetRecursionLimit _ANSI_ARGS_((Tcl_Interp *interp,
			    int depth));
EXTERN void		Tcl_SetResult _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, Tcl_FreeProc *freeProc));
EXTERN char *		Tcl_SetVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, char *newValue, int flags));
EXTERN char *		Tcl_SetVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *part1, char *part2, char *newValue,
			    int flags));
EXTERN char *		Tcl_SignalId _ANSI_ARGS_((int sig));
EXTERN char *		Tcl_SignalMsg _ANSI_ARGS_((int sig));
EXTERN int		Tcl_SplitList _ANSI_ARGS_((Tcl_Interp *interp,
			    char *list, int *argcPtr, char ***argvPtr));
EXTERN int		Tcl_StringMatch _ANSI_ARGS_((char *string,
			    char *pattern));
EXTERN char *		Tcl_TildeSubst _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name, Tcl_DString *bufferPtr));
EXTERN int		Tcl_TraceVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, int flags, Tcl_VarTraceProc *proc,
			    ClientData clientData));
EXTERN int		Tcl_TraceVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *part1, char *part2, int flags,
			    Tcl_VarTraceProc *proc, ClientData clientData));
EXTERN void		Tcl_UnlinkVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName));
EXTERN int		Tcl_UnsetVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, int flags));
EXTERN int		Tcl_UnsetVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *part1, char *part2, int flags));
EXTERN void		Tcl_UntraceVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, int flags, Tcl_VarTraceProc *proc,
			    ClientData clientData));
EXTERN void		Tcl_UntraceVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *part1, char *part2, int flags,
			    Tcl_VarTraceProc *proc, ClientData clientData));
EXTERN int		Tcl_UpVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *frameName, char *varName,
			    char *localName, int flags));
EXTERN int		Tcl_UpVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *frameName, char *part1, char *part2,
			    char *localName, int flags));

#ifdef USEVARARGS
EXTERN int		Tcl_VarEval _ANSI_ARGS_(VARARGS(Tcl_Interp *interp));
#else
EXTERN int		Tcl_VarEval(Tcl_Interp *first, ...);
#endif

EXTERN ClientData	Tcl_VarTraceInfo _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, int flags,
			    Tcl_VarTraceProc *procPtr,
			    ClientData prevClientData));
EXTERN ClientData	Tcl_VarTraceInfo2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *part1, char *part2, int flags,
			    Tcl_VarTraceProc *procPtr,
			    ClientData prevClientData));
    
/*
 * ------------------------------------------------------------------------
 * >>>>>>>>>>>>>>>>>>> stuff for [incr Tcl] namespaces <<<<<<<<<<<<<<<<<<<
 * ------------------------------------------------------------------------
 *  UTILITIES
 * ========================================================================
 */
#define ITCL_NAMESPACES

typedef int* Itcl_InterpState;
typedef void (Itcl_FreeProc) _ANSI_ARGS_((ClientData cdata));

/*
 *  STACK INFO
 */
typedef struct Itcl_Stack {
    ClientData *values;          /* values on stack */
    int len;                     /* number of values on stack */
    int max;                     /* maximum size of stack */
    ClientData space[5];         /* initial space for stack data */
} Itcl_Stack;

#define Itcl_GetStackSize(stackPtr) ((stackPtr)->len)

/*
 *  LIST INFO
 */
struct Itcl_List;
typedef struct Itcl_ListElem {
    struct Itcl_List* owner;     /* list containing this element */
    ClientData value;            /* value associated with this element */
    struct Itcl_ListElem *prev;  /* previous element in linked list */
    struct Itcl_ListElem *next;  /* next element in linked list */
} Itcl_ListElem;

typedef struct Itcl_List {
    int validate;                /* validation stamp */
    int num;                     /* number of elements */
    struct Itcl_ListElem *head;  /* previous element in linked list */
    struct Itcl_ListElem *tail;  /* next element in linked list */
} Itcl_List;

#define Itcl_FirstListElem(listPtr) ((listPtr)->head)
#define Itcl_LastListElem(listPtr)  ((listPtr)->tail)
#define Itcl_NextListElem(elemPtr)  ((elemPtr)->next)
#define Itcl_PrevListElem(elemPtr)  ((elemPtr)->prev)
#define Itcl_GetListLength(listPtr) ((listPtr)->num)
#define Itcl_GetListValue(elemPtr)  ((elemPtr)->value)

/*
 *  UTILITIES:  exported procedures
 */
EXTERN void Itcl_InitStack _ANSI_ARGS_((Itcl_Stack *stack));
EXTERN void Itcl_DeleteStack _ANSI_ARGS_((Itcl_Stack *stack));
EXTERN void Itcl_PushStack _ANSI_ARGS_((ClientData cdata, Itcl_Stack *stack));
EXTERN ClientData Itcl_PopStack _ANSI_ARGS_((Itcl_Stack *stack));
EXTERN ClientData Itcl_PeekStack _ANSI_ARGS_((Itcl_Stack *stack));
EXTERN ClientData Itcl_GetStackValue _ANSI_ARGS_((Itcl_Stack *stack, int i));

EXTERN void Itcl_InitList _ANSI_ARGS_((Itcl_List *listPtr));
EXTERN void Itcl_DeleteList _ANSI_ARGS_((Itcl_List *listPtr));
EXTERN Itcl_ListElem* Itcl_CreateListElem _ANSI_ARGS_((Itcl_List *listPtr));
EXTERN Itcl_ListElem* Itcl_DeleteListElem _ANSI_ARGS_((Itcl_ListElem *elemPtr));
EXTERN Itcl_ListElem* Itcl_InsertList _ANSI_ARGS_((Itcl_List *listPtr,
    ClientData val));
EXTERN Itcl_ListElem* Itcl_InsertListElem _ANSI_ARGS_((Itcl_ListElem *elemPtr,
    ClientData val));
EXTERN Itcl_ListElem* Itcl_AppendList _ANSI_ARGS_((Itcl_List *listPtr,
    ClientData val));
EXTERN Itcl_ListElem* Itcl_AppendListElem _ANSI_ARGS_((Itcl_ListElem *elemPtr,
    ClientData val));
EXTERN void Itcl_SetListValue _ANSI_ARGS_((Itcl_ListElem *elemPtr,
    ClientData val));

EXTERN void Itcl_EventuallyFree _ANSI_ARGS_((ClientData cdata,
    Itcl_FreeProc *fproc));
EXTERN ClientData Itcl_PreserveData _ANSI_ARGS_((ClientData cdata));
EXTERN void Itcl_ReleaseData _ANSI_ARGS_((ClientData cdata));

EXTERN Itcl_InterpState Itcl_SaveInterpState _ANSI_ARGS_((Tcl_Interp *interp,
    int status));
EXTERN int Itcl_RestoreInterpState _ANSI_ARGS_((Tcl_Interp *interp,
    Itcl_InterpState state));
EXTERN void Itcl_DiscardInterpState _ANSI_ARGS_((Itcl_InterpState state));

EXTERN char* Itcl_ProtectionStr _ANSI_ARGS_((int pLevel));
EXTERN void Itcl_ParseNamespPath _ANSI_ARGS_((char *name, char **head,
    char **tail));

/*
 *  NAMESPACES
 * ========================================================================
 */
typedef int* Tcl_Var;
typedef int* Itcl_Namespace;
typedef int* Itcl_ActiveNamespace;

typedef void (Itcl_DeleteProc) _ANSI_ARGS_((ClientData cdata));

typedef int (Itcl_CmdEnforcerProc) _ANSI_ARGS_((Tcl_Interp* interp,
    char* name, Tcl_Command* cmdPtr));

typedef int (Itcl_VarEnforcerProc) _ANSI_ARGS_((Tcl_Interp* interp,
    char* name, Tcl_Var* varPtr, int flags));

typedef struct Itcl_ScopedVal {
    Itcl_Namespace namesp;        /* namespace context for value */
    char* value;                  /* string value (script or variable) */
} Itcl_ScopedVal;

/*
 * With namespaces, the usual TCL_GLOBAL_ONLY flag means "variable
 * at the global scope".  The following flag means "variable is
 * global within the current namespace".
 *
 * BE CAREFUL - the value for this flag must not conflict with
 * the regular flags for Tcl variables.
 */
#define ITCL_GLOBAL_VAR   0x1000

/*
 * Protection levels:
 *
 * ITCL_PUBLIC    - accessible from any namespace
 * ITCL_PROTECTED - accessible from namespace that imports in "protected" mode
 * ITCL_PRIVATE   - accessible only within the namespace that contains it
 */
#define ITCL_PUBLIC        1
#define ITCL_PROTECTED     2
#define ITCL_PRIVATE       3

/*
 * Flags for namespace/command/variable lookup:
 *
 * ITCL_FIND_LOCAL_ONLY
 *   avoid imported namespaces during name search
 *
 * ITCL_FIND_NO_PRIVACY
 *   bypass "private" and "protected" designations when performing
 *   command/variable lookup
 *
 * ITCL_FIND_AUTO_CREATE
 *   create namespaces in path during lookup (needed to support
 *   operations like Tcl_CreateCommand() that cannot fail)
 *
 * ITCL_FIND_NAMESP
 *   resolve a namespace path like "foo::bar::baz" all the way
 *   down the path, treating the last name as another namespace.
 *   Used to tell ItclFollowNamespPath() to follow the entire
 *   path, instead of stopping short with the last name.
 */
#define ITCL_FIND_LOCAL_ONLY    0x10
#define ITCL_FIND_NO_PRIVACY    0x20
#define ITCL_FIND_AUTO_CREATE   0x40
#define ITCL_FIND_NAMESP        0x80

/*
 * Flags for Itcl_AddEnsembleOption():
 *   ITCL_VAR_ARGS - disable error checking on ensemble options
 */
#define ITCL_VAR_ARGS     -1

/*
 *  NAMESPACES:  exported procedures
 */

EXTERN char* Tcl_GetVariableName _ANSI_ARGS_((Tcl_Interp *interp,
    Tcl_Var var));

EXTERN Itcl_Namespace Itcl_GetGlobalNamesp _ANSI_ARGS_((Tcl_Interp* interp));
EXTERN Itcl_Namespace Itcl_GetCallingNamesp _ANSI_ARGS_((Tcl_Interp* interp,
    int level));
EXTERN Itcl_Namespace Itcl_GetActiveNamesp _ANSI_ARGS_((Tcl_Interp* interp));
EXTERN ClientData Itcl_GetActiveNamespData _ANSI_ARGS_((Tcl_Interp* interp));
EXTERN Itcl_Namespace Itcl_GetCommandNamesp _ANSI_ARGS_((Tcl_Command command));
EXTERN int Itcl_ValidNamesp _ANSI_ARGS_((Itcl_Namespace ns));
EXTERN int Itcl_CmdProtection _ANSI_ARGS_((Tcl_Interp* interp, int newLevel));
EXTERN int Itcl_ChangeCmdProtection _ANSI_ARGS_((Tcl_Interp* interp,
    Tcl_Command cmd, int newLevel));
EXTERN int Itcl_VarProtection _ANSI_ARGS_((Tcl_Interp* interp, int newLevel));
EXTERN int Itcl_ChangeVarProtection _ANSI_ARGS_((Tcl_Interp* interp,
    Tcl_Var var, int newLevel));
EXTERN Itcl_CmdEnforcerProc* Itcl_SetCmdEnforcer _ANSI_ARGS_((
    Itcl_Namespace ns, Itcl_CmdEnforcerProc* proc));
EXTERN Itcl_VarEnforcerProc* Itcl_SetVarEnforcer _ANSI_ARGS_((
    Itcl_Namespace ns, Itcl_VarEnforcerProc* proc));

#define Itcl_ActivateNamesp(interp,ns)  \
    Itcl_ActivateNamesp2(interp, ns, (ClientData)NULL)

EXTERN Itcl_ActiveNamespace Itcl_ActivateNamesp2 _ANSI_ARGS_((
    Tcl_Interp* interp, Itcl_Namespace ns, ClientData cd));
EXTERN void Itcl_DeactivateNamesp _ANSI_ARGS_((Tcl_Interp* interp,
    Itcl_ActiveNamespace token));

EXTERN int Itcl_CreateNamesp _ANSI_ARGS_((Tcl_Interp* interp, char* name,
    ClientData cdata, Itcl_DeleteProc* dproc, Itcl_Namespace* nsPtr));
EXTERN void Itcl_DeleteNamesp _ANSI_ARGS_((Itcl_Namespace ns));

EXTERN void Itcl_ClearImportNamesp _ANSI_ARGS_((Itcl_Namespace ns));
EXTERN void Itcl_InsertImportNamesp2 _ANSI_ARGS_((Itcl_Namespace ns,
    Itcl_Namespace import, int pLevel, Itcl_Namespace pos));
EXTERN void Itcl_AppendImportNamesp2 _ANSI_ARGS_((Itcl_Namespace ns,
    Itcl_Namespace import, int pLevel, Itcl_Namespace pos));
EXTERN void Itcl_RemoveImportNamesp _ANSI_ARGS_((Itcl_Namespace ns,
    Itcl_Namespace import));
#define Itcl_InsertImportNamesp(ns,import,plevel)  \
    Itcl_InsertImportNamesp2(ns, import, plevel, (Itcl_Namespace)NULL)
#define Itcl_AppendImportNamesp(ns,import,plevel)  \
    Itcl_AppendImportNamesp2(ns, import, plevel, (Itcl_Namespace)NULL)

EXTERN void Itcl_GetImportNamesp _ANSI_ARGS_((Itcl_Namespace ns,
    Itcl_List* listPtr));
EXTERN void Itcl_GetAllImportNamesp _ANSI_ARGS_((Itcl_Namespace ns,
    Itcl_List* listPtr));
EXTERN void Itcl_GetAllImportedByNamesp _ANSI_ARGS_((Itcl_Namespace ns,
    Itcl_List* listPtr));

EXTERN int Itcl_UplevelEval _ANSI_ARGS_((Tcl_Interp* interp, char* cmd));
EXTERN int Itcl_NamespEval _ANSI_ARGS_((Tcl_Interp* interp,
    Itcl_Namespace ns, char* cmd));

#ifdef USEVARARGS
EXTERN int Itcl_NamespVarEval _ANSI_ARGS_(VARARGS(Tcl_Interp *interp));
#else
EXTERN int Itcl_NamespVarEval (Tcl_Interp *interp, ...);
#endif

EXTERN void Itcl_NsCacheClear _ANSI_ARGS_((Itcl_Namespace ns));
EXTERN void Itcl_NsCacheNamespChanged _ANSI_ARGS_((Itcl_Namespace ns));
EXTERN void Itcl_NsCacheCmdChanged _ANSI_ARGS_((Itcl_Namespace ns,
    char* name));
EXTERN void Itcl_NsCacheVarChanged _ANSI_ARGS_((Itcl_Namespace ns,
    char* name));

EXTERN ClientData Itcl_GetNamespData _ANSI_ARGS_((Itcl_Namespace ns));
EXTERN char* Itcl_GetNamespName _ANSI_ARGS_((Itcl_Namespace ns));
EXTERN char* Itcl_GetNamespPath _ANSI_ARGS_((Itcl_Namespace ns));
EXTERN Itcl_Namespace Itcl_GetNamespParent _ANSI_ARGS_((Itcl_Namespace ns));

EXTERN int Itcl_FindNamesp _ANSI_ARGS_((Tcl_Interp* interp,
    char* path, int flags, Itcl_Namespace* rnsPtr));
EXTERN int Itcl_FindCommand _ANSI_ARGS_((Tcl_Interp* interp,
    char* path, int flags, Tcl_Command *rcmd));
EXTERN int Itcl_FindVariable _ANSI_ARGS_((Tcl_Interp* interp,
    char* path, int flags, Tcl_Var *rvar));

EXTERN void Itcl_ScopedValInit _ANSI_ARGS_((Itcl_ScopedVal* sval));
EXTERN void Itcl_ScopedValSet _ANSI_ARGS_((Itcl_ScopedVal* sval,
    char *valStr, Itcl_Namespace valNs));
EXTERN void Itcl_ScopedValFree _ANSI_ARGS_((Itcl_ScopedVal* sval));
EXTERN char* Itcl_ScopedValEncode _ANSI_ARGS_((Itcl_ScopedVal* sval));
EXTERN int Itcl_ScopedValDecode _ANSI_ARGS_((Tcl_Interp* interp,
    char* mesg, Itcl_ScopedVal* sval));
EXTERN int Itcl_ScopedValDecodeList _ANSI_ARGS_((Tcl_Interp* interp,
    char* mesg, Itcl_List* svlist));

EXTERN int Itcl_CreateEnsemble _ANSI_ARGS_((Tcl_Interp* interp, char* name));
EXTERN int Itcl_AddEnsembleOption _ANSI_ARGS_((Tcl_Interp* interp,
    char* ensName, char* optName, int minArgs, int maxArgs, char* usage,
    Tcl_CmdProc* proc, ClientData cdata, Tcl_CmdDeleteProc* dproc));
EXTERN int Itcl_GetEnsembleOption _ANSI_ARGS_((Tcl_Interp* interp,
    char* ensName, char* optName, Tcl_CmdInfo* cinfo));
EXTERN int Itcl_IsEnsemble _ANSI_ARGS_((Tcl_CmdInfo* cinfo));
EXTERN char* Itcl_EnsembleInvoc _ANSI_ARGS_((void));
EXTERN char* Itcl_EnsembleUsage _ANSI_ARGS_((void));
EXTERN int Itcl_EnsembleErrorCmd _ANSI_ARGS_((ClientData cdata,
    Tcl_Interp* interp, int argc, char** argv));

/* *********
   
   C H A N G E S  F O R  T H E  T C L -> C  C O M P I L E R
   
   *********
 */

/*
 * Type Attributes.  All basic types we will translate the tcl code
 * into.
 */

typedef unsigned int TypeAttribute;

/*
 * Valid values for type (Why they need to be different than the above
 * Don't ask me...
 */

#define TYPE_INT	0
#define TYPE_DOUBLE	1
#define TYPE_STRING	2

EXTERN int		quitCode;
EXTERN int		forceExit;


EXTERN int		Tcl_Eval_Args _ANSI_ARGS_((Tcl_Interp *interp,
						   char *cmd, int pargc,
						   char **pargv,
						   ClientData clientD,
						   Tcl_CmdProc *procEntry));

EXTERN void		Tcl_EnterErrorInfo _ANSI_ARGS_((Tcl_Interp *interp,
							char *cmd, int n,
							char *e));

EXTERN void		Tcl_ResetValueInterp _ANSI_ARGS_((Tcl_Interp *interp));

#ifdef __cplusplus
}
#endif

#endif /* _TCL */
