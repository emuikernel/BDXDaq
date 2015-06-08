/* symLib.h - symbol table subroutine library header */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
04g,15oct01,jn   move private funcs into symLibP.h
04f,04sep98,cdp  make Thumb-specific changes dependent on ARM_THUMB.
04e,02dec97,cdp  added thumbSymTblAdd().
04d,30oct96,elp  Added syncSymAddRtn, symSymRemoveRtn function pointers and
		 symSAdd().
04c,22sep92,rrr  added support for c++
04b,28jul92,jmm  added prototype for symAdd
04a,04jul92,jcf  cleaned up.
03t,22jun92,jmm  removed symFooWithGroup, added parameter to routines instead
03s,26may92,rrr  the tree shuffle
03r,15may92,jmm  Changed "symFooGroup" to "symFooWithGroup"
03i,30apr92,jmm  Added support for goup numbers
03h,02jan92,gae  used UINT's for value in symFindByValue{AndType}.
03g,13dec91,gae  made symFindByCName() consistent with other symFind* routines.
03f,26nov91,llk  added symName().
03e,04oct91,rrr  passed through the ansification filter
		  -changed copyright notice
03d,05oct90,dnw  deleted private functions.
03c,05oct90,shl  added ANSI function prototypes.
                 added copyright notice.
03b,26jun90,jcf  removed symbol table id error.
03a,22nov89,jcf  changed for hasing/multiple access implementation
02d,30may88,dnw  changed to v4 names.
02c,24dec86,gae  changed stsLib.h to vwModNum.h.
02b,05jun86,dnw  changed SYMTAB structure to have ptr to symbol array
		   instead of imbedded array.
02a,11oct85,dnw  changed SYMTAB structure to have maxSymbols instead of
		   string table pool pointer.
		 deleted obsolete definition of MAX_SYM_LENGTH.
01g,11jun85,rdc  added maxSymLength and free to the SYMTAB structure.
		 These were necessary
		 for variable symbol length.
01f,13aug84,ecs  deleted status code S_sysLib_EMPTY_SYMBOL_TABLE.
01e,08aug84,ecs  added include of stsLib.h, symLib status codes.
01d,27jun84,ecs  changed length of name in SYMBOL to allow room for EOS.
01c,15jun84,dnw  moved typedefs for SYMBOL and SYMTAB here from symLib.c.
		 added typedef for SYMTAB_ID.
01b,27jan84,ecs  added inclusion test.
01a,03aug83,dnw  written
*/

#ifndef __INCsymLibh
#define __INCsymLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vwModNum.h"
#include "symbol.h"
#include "classLib.h"
#include "memLib.h"
#include "hashLib.h"
#include "private/semLibP.h"
#include "private/objLibP.h"
#include "private/symLibP.h"

/* status codes */

#define S_symLib_SYMBOL_NOT_FOUND	(M_symLib | 1)
#define S_symLib_NAME_CLASH		(M_symLib | 2)
#define S_symLib_TABLE_NOT_EMPTY	(M_symLib | 3)
#define S_symLib_SYMBOL_STILL_IN_TABLE	(M_symLib | 4)

/* 
 * XXX do not use errno's 5-11, inclusive, in symLib - 
 * these are used by vxWorks AE. 
 */

#define S_symLib_INVALID_SYMTAB_ID	(M_symLib | 12)
#define S_symLib_INVALID_SYM_ID_PTR     (M_symLib | 13)

/* class definition */

IMPORT CLASS_ID symTblClassId;		/* symbol table class id */

/* default group number */

IMPORT UINT16	symGroupDefault;
IMPORT FUNCPTR	syncSymAddRtn;
IMPORT FUNCPTR	syncSymRemoveRtn;

/* function declarations */

extern STATUS 	symLibInit (void);
extern SYMBOL *	symEach (SYMTAB_ID symTblId, FUNCPTR routine, int routineArg);
extern char *	symName (SYMTAB_ID symTbl, char *value);
extern void 	symShowInit (void);
extern STATUS 	symShow (SYMTAB_ID pSymTbl, char *substr);
extern STATUS 	symTblDelete (SYMTAB_ID symTblId);
extern STATUS 	symTblTerminate (SYMTAB_ID symTblId);
extern STATUS 	symTblDestroy (SYMTAB_ID symTblId, BOOL dealloc);
extern STATUS 	symFree (SYMTAB_ID symTblId, SYMBOL *pSymbol);
extern STATUS   symSAdd (SYMTAB_ID symTblId, char *name, char *value,
			SYM_TYPE type, UINT16 group);
extern STATUS   symAdd (SYMTAB_ID symTblId, char *name, char *value,
			SYM_TYPE type, UINT16 group);
extern STATUS 	symTblAdd (SYMTAB_ID symTblId, SYMBOL *pSymbol);
extern STATUS 	symRemove (SYMTAB_ID symTblId, char *name, SYM_TYPE type);
extern STATUS 	symTblRemove (SYMTAB_ID symTblId, SYMBOL *pSymbol);
extern STATUS 	symInit (SYMBOL *pSymbol, char *name, char *value,
		         SYM_TYPE type, UINT16 group);
extern SYMBOL *	symAlloc (SYMTAB_ID symTblId, char *name, char *value,
			  SYM_TYPE type, UINT16 group);
extern STATUS 	symTblInit (SYMTAB *pSymTbl, BOOL sameNameOk,
			    PART_ID symPartId, HASH_ID symHashTblId);
extern SYMTAB_ID symTblCreate (int hashSizeLog2, BOOL sameNameOk,
			       PART_ID symPartId);
extern STATUS 	symFindByName (SYMTAB_ID symTblId, char *name, char **pValue,
			       SYM_TYPE *pType);
extern STATUS 	symFindByValue (SYMTAB_ID symTblId, UINT value, char *name,
				int *pValue, SYM_TYPE *pType);
extern STATUS 	symFindByCName (SYMTAB_ID symTblId, char *name, char **pValue,
				SYM_TYPE *pType);
extern STATUS 	symFindByNameAndType (SYMTAB_ID symTblId, char *name,
				      char **pValue, SYM_TYPE *pType,
				      SYM_TYPE sType, SYM_TYPE mask);
extern STATUS 	symFindByValueAndType (SYMTAB_ID symTblId, UINT value,
				       char *name, int *pValue, SYM_TYPE *pType,
				       SYM_TYPE sType, SYM_TYPE mask);
extern STATUS 	symByValueFind (SYMTAB_ID symTblId, UINT value, char **pName,
				int *pValue, SYM_TYPE *pType);
extern STATUS 	symByValueAndTypeFind (SYMTAB_ID symTblId, UINT value,
				       char **pName, int *pValue, 
				       SYM_TYPE *pType,SYM_TYPE sType, 
				       SYM_TYPE mask);

#if	((CPU_FAMILY == ARM) && ARM_THUMB)
extern STATUS	thumbSymTblAdd (SYMTAB_ID symTblId, SYMBOL *pSymbol);
#endif	/* CPU_FAMILY == ARM */

#ifdef __cplusplus
}
#endif

#endif /* __INCsymLibh */
