/* loadSomCoffLib.h - object module dependant loader library header */

/* Copyright 1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,13may98,ms   added prototype for loadSomCoffInit.
01a,12sep93,yao  written.
*/

#ifndef __INCloadSomCoffLibh
#define __INCloadSomCoffLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "vwModNum.h"
#include "symLib.h"

/* status codes */

#define S_loadSomCoffLib_HDR_READ		(M_loadSomCoffLib | 1)
#define S_loadSomCoffLib_AUXHDR_READ		(M_loadSomCoffLib | 2)
#define S_loadSomCoffLib_SYM_READ		(M_loadSomCoffLib | 3)
#define S_loadSomCoffLib_SYMSTR_READ		(M_loadSomCoffLib | 4)
#define S_loadSomCoffLib_OBJ_FMT		(M_loadSomCoffLib | 5)
#define S_loadSomCoffLib_SPHDR_ALLOC		(M_loadSomCoffLib | 6)
#define S_loadSomCoffLib_SPHDR_READ		(M_loadSomCoffLib | 7)
#define S_loadSomCoffLib_SUBSPHDR_ALLOC	(M_loadSomCoffLib | 8)
#define S_loadSomCoffLib_SUBSPHDR_READ		(M_loadSomCoffLib | 9)
#define S_loadSomCoffLib_SPSTRING_ALLOC	(M_loadSomCoffLib | 10)
#define S_loadSomCoffLib_SPSTRING_READ		(M_loadSomCoffLib | 11)
#define S_loadSomCoffLib_INFO_ALLOC		(M_loadSomCoffLib | 12)
#define S_loadSomCoffLib_LOAD_SPACE		(M_loadSomCoffLib | 13)
#define S_loadSomCoffLib_RELOC_ALLOC		(M_loadSomCoffLib | 14)
#define S_loadSomCoffLib_RELOC_READ		(M_loadSomCoffLib | 15)
#define S_loadSomCoffLib_RELOC_NEW		(M_loadSomCoffLib | 16)
#define S_loadSomCoffLib_RELOC_VERSION		(M_loadSomCoffLib | 17)

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS loadSomCoffInit (void);

#else	/* __STDC__ */

extern STATUS loadSomCoffInit ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCloadSomCoffLibh */
