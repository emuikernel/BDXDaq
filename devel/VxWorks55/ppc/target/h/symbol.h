/* symbol.h - symbol structure header */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
02f,29oct01,jn   create symbolP.h
02e,25jul97,cdp  added SYM_THUMB.
02d,26mai96,dbt  added support for PowerPC SDA-related symbols.
02c,22sep92,rrr  added support for c++
02b,14jul92,jmm  added define of SYM_MASK_EXACT for symTblRemove
02a,04jul92,jcf  cleaned up.
01f,26may92,rrr  the tree shuffle
01e,30apr92,jmm  Added support for group numbers
01d,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed TINY and UTINY to INT8 and UINT8
		  -changed copyright notice
01c,19may91,gae  changed UINT8 to unsigned char.
01b,05oct90,shl  added copyright notice.
01a,10dec89,jcf  written by pulling out of symLib.h.
*/

#ifndef __INCsymbolh
#define __INCsymbolh

#ifdef __cplusplus
extern "C" {
#endif

#include "private/symbolP.h"  /* Definition of SYM_TYPE, SYMBOL_ID, etc. */

/* symbol types */

#define SYM_UNDF        0x0     /* undefined */
#define SYM_LOCAL       0x0     /* local */
#define SYM_GLOBAL      0x1     /* global (external) (ORed) */
#define SYM_ABS         0x2     /* absolute */
#define SYM_TEXT        0x4     /* text */
#define SYM_DATA        0x6     /* data */
#define SYM_BSS         0x8     /* bss */
#define SYM_COMM        0x12    /* common symbol */
#define SYM_SDA         0x40    /* symbols related to a PowerPC SDA section */
#define SYM_SDA2        0x80    /* symbols related to a PowerPC SDA2 section */

#define SYM_THUMB	0x40	/* Thumb function */

/* symbol masks */

#define SYM_MASK_ALL	0xff            /* all bits of symbol type valid */
#define SYM_MASK_NONE	0x00		/* no bits of symbol type valid */
#define SYM_MASK_EXACT	0x1ff		/* match symbol pointer exactly */
#define SYM_SDA_MASK    0xc0            /* for SDA and SDA2 symbols */

#define SYM_MASK_ANY_TYPE    SYM_MASK_NONE  /* ignore type in searches */
#define SYM_MASK_EXACT_TYPE  SYM_MASK_ALL   /* match type exactly in searches */

#ifdef __cplusplus
}
#endif

#endif /* __INCsymbolh */
