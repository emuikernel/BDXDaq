/* symLibP.h - private symbol library header file */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,15oct01,jn   created.
*/

#ifndef __INCsymLibPh
#define __INCsymLibPh

#ifdef __cplusplus
extern "C" {
#endif

#include "private/symbolP.h"

/* structure definitions */

typedef struct symtab	/* SYMTAB - symbol table */
    {
    OBJ_CORE	objCore;	/* object maintanance */
    HASH_ID	nameHashId;	/* hash table for names */
    SEMAPHORE	symMutex;	/* symbol table mutual exclusion sem */
    PART_ID	symPartId;	/* memory partition id for symbols */
    BOOL	sameNameOk;	/* symbol table name clash policy */
    int		nsymbols;	/* current number of symbols in table */
    } SYMTAB;

typedef SYMTAB *SYMTAB_ID;

/* function declarations */

extern STATUS symFindSymbol
    (
    SYMTAB_ID   symTblId,       /* symbol table ID */
    char *      name,           /* name to search for */
    void *	value,		/* value of symbol to search for */
    SYM_TYPE    type,           /* symbol type */
    SYM_TYPE    mask,           /* type bits that matter */
    SYMBOL_ID * pSymbolId       /* where to return pointer to matching symbol */
    );

extern STATUS symByCNameFind
    (
    SYMTAB_ID   symTblId,	/* ID of symbol table to look in */
    char *	name,		/* symbol name to look for   */
    char **	pValue,		/* where to put symbol value */
    SYM_TYPE *	pType		/* where to put symbol type  */
    );

extern STATUS symNameGet
    (
    SYMBOL_ID  symbolId,
    char **    pName
    );

extern STATUS symValueGet 
    (
    SYMBOL_ID  symbolId,
    void **    pValue
    );

extern STATUS symTypeGet 
    (
    SYMBOL_ID  symbolId,
    SYM_TYPE * pType
    );

#ifdef __cplusplus
}
#endif

#endif /* __INCsymLibPh */

