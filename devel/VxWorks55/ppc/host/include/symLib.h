/* symLib.h - symbol table subroutine library header */

/* Copyright 1995-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,11dec01,jn   put symFindByValue and symFindByValueAndType back - 
                 removing them breaks the DLL interface
01f,29nov01,jn   comment out symFindByValue and symFindByValueAndType - 
                 these routines should no longer be used
01e,15may98,pcn  Changed symFindByValue in symByValueFind and
                 symFindByValueAndType in symByValueAndTypeFind.
01d,27mar98,pcn  Changed symFindByValue and symFindByValueAndType symbol name
                 parameter.
01c,22jan98,c_c  Changed VxWorks Mutex APIs for WIN32 APIs
01b,01mar95,p_m  removed status codes.
01a,20jan95,jcf  based on version 04c of VxWorks.
*/

#ifndef __INCsymLibh
#define __INCsymLibh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
#include "win32ThreadLib.h"
#endif
#include "symbol.h"
#include "hashLib.h"
#include "objLib.h"

/* HIDDEN */

typedef struct symtab	/* SYMTAB - symbol table */
    {
    OBJ_CORE	objCore;	/* object maintanance */
    HASH_ID	nameHashId;	/* hash table for names */
    HANDLE      symMutex;	/* symbol table mutual exclusion sem */
    BOOL	sameNameOk;	/* symbol table name clash policy */
    int		nsymbols;	/* current number of symbols in table */
    } SYMTAB;

/* END_HIDDEN */

typedef SYMTAB *SYMTAB_ID;

/* default group number */

IMPORT UINT16 symGroupDefault;

/* function declarations */

extern STATUS 	symLibInit (void);

extern SYMBOL *	symEach
    (
    SYMTAB_ID	symTblId,
    FUNCPTR	routine,
    int		routineArg
    );

extern char *	symName
    (
    SYMTAB_ID	symTbl,
    char *	value
    );

extern void 	symShowInit (void);

extern STATUS 	symShow
    (
     SYMTAB_ID	pSymTbl,
     char *	substr
    );

extern STATUS 	symTblDelete
    (
    SYMTAB_ID	symTblId
    );

extern STATUS 	symTblTerminate 
    (
    SYMTAB_ID	symTblId
    );

extern STATUS 	symTblDestroy 
    (
    SYMTAB_ID	symTblId, 
    BOOL	dealloc
    );

extern STATUS 	symFree
    (
    SYMTAB_ID	symTblId, 
    SYMBOL *	pSymbol
    );

extern STATUS   symAdd
    (
    SYMTAB_ID	symTblId,
    char *	name,
    char *	value,
    SYM_TYPE	type,
    UINT16	group
    );

extern STATUS 	symTblAdd
    (
    SYMTAB_ID	symTblId,
    SYMBOL *	pSymbol
    );

extern STATUS 	symRemove
    (
    SYMTAB_ID	symTblId,
    char *	name,
    SYM_TYPE	type
    );

extern STATUS 	symTblRemove 
    (
    SYMTAB_ID	symTblId,
    SYMBOL *	pSymbol
    );

extern STATUS 	symInit
    (
    SYMBOL *	pSymbol,
    char *	name,
    char *	value,
    SYM_TYPE	type,
    UINT16	group
    );

extern SYMBOL *	symAlloc 
    (
    SYMTAB_ID	symTblId,
    char *	name,
    char *	value,
    SYM_TYPE	type,
    UINT16	group
    );

extern STATUS 	symTblInit
    (
    SYMTAB *	pSymTbl, 
    BOOL	sameNameOk,
    HASH_ID	symHashTblId
    );

extern SYMTAB_ID symTblCreate 
    (
    int		hashSizeLog2,
    BOOL	sameNameOk
    );

extern STATUS 	symFindSymbol
    (
    SYMTAB_ID	symTblId,
    char *	name, 
    SYM_TYPE	type,
    SYM_TYPE	mask,
    SYMBOL **	ppSymbol
    );

extern STATUS 	symFindByName 
    (
    SYMTAB_ID	symTblId,
    char *	name,
    char **	pValue,
    SYM_TYPE *	pType
    );

extern STATUS 	symFindByValue
    (
    SYMTAB_ID	symTblId,
    UINT	value,
    char * 	name,
    int *	pValue,
    SYM_TYPE *	pType
    );

extern STATUS 	symFindByCName 
    (
    SYMTAB_ID	symTblId,
    char *	name,
    char **	pValue,
    SYM_TYPE *	pType
    );

extern STATUS 	symFindByNameAndType 
    (
    SYMTAB_ID	symTblId,
    char *	name,
    char **	pValue,
    SYM_TYPE *	pType,
    SYM_TYPE	sType,
    SYM_TYPE	mask
    );

extern STATUS 	symFindByValueAndType
    (
    SYMTAB_ID	symTblId,
    UINT	value,
    char * 	name,
    int *	pValue,
    SYM_TYPE *	pType,
    SYM_TYPE	sType,
    SYM_TYPE	mask
    );

extern STATUS 	symByValueAndTypeFind
    (
    SYMTAB_ID	symTblId,
    UINT	value,
    char **	name,
    int *	pValue,
    SYM_TYPE *	pType,
    SYM_TYPE	sType,
    SYM_TYPE	mask
    );

extern STATUS 	symByValueFind
    (
    SYMTAB_ID	symTblId,
    UINT	value,
    char **	name,
    int *	pValue,
    SYM_TYPE *	pType
    );


#ifdef __cplusplus
}
#endif

#endif /* __INCsymLibh */
