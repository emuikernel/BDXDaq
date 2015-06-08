/* symbolP.h - private symbol structure header */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,29oct01,jn   created from HIDDEN part of symbol.h
*/

#ifndef __INCsymbolPh
#define __INCsymbolPh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "sllLib.h"

typedef unsigned char SYM_TYPE;		/* SYM_TYPE */

typedef struct			/* SYMBOL - entry in symbol table */
    {
    SL_NODE	nameHNode;	/* hash node (must come first) */
    char	*name;		/* pointer to symbol name */
    void	*value;		/* symbol value */
    UINT16	group;		/* symbol group */
    SYM_TYPE	type;		/* symbol type */
    } SYMBOL;

typedef SYMBOL * SYMBOL_ID;

#ifdef __cplusplus
}
#endif

#endif /* __INCsymbolPh */

