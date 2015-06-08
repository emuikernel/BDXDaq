/* symbol.h - symbol structure header */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,09sep99,myz  added SYM_MIPS16.
01f,17apr98,c_c  ARM merge.
01e,16jul97,cdp  added SYM_THUMB.
01d,23jul96,pad  Value for SYM_COMM is now 0x10, instead of 0x12 (SPR #6934).
01c,23apr96,pad  added support for PowerPC SDA-related symbols. Updated
		 history structure.
01b,15mar95,p_m  changed #include "vxWorks.h" to #include "host.h".
01a,01dec94,pad  based on version 02c of VxWorks' symLib.h.
*/

#ifndef __INCsymbolh
#define __INCsymbolh

#ifdef __cplusplus
extern "C" {
#endif

#include "host.h"
#include "sllLib.h"

/* symbol types */

#define SYM_UNDF	0x0	/* undefined */
#define SYM_LOCAL	0x0	/* local */
#define SYM_GLOBAL	0x1	/* global (external) (ORed) */
#define SYM_ABS		0x2	/* absolute */
#define SYM_TEXT	0x4	/* text */
#define SYM_DATA	0x6	/* data */
#define SYM_BSS		0x8	/* bss */
#define SYM_COMM	0x10	/* common symbol */

#define SYM_BAL_ENTRY	0x20	/* I80960 BAL entry point to function */
#define SYM_SDA		0x40	/* symbols related to a PowerPC SDA section */
#define SYM_SDA2	0x80	/* symbols related to a PowerPC SDA2 section */

#define SYM_THUMB	0x40	/* Thumb function - unlikely clash with SYM_SDA */

#define SYM_MIPS16      0x80    /* mips16 function - unlikely clash with
				   SYM_SDA2, arch. specific */ 

/* symbol masks */

#define SYM_MASK_ALL	0xff		/* all bits of symbol type valid */
#define SYM_MASK_NONE	0x00		/* no bits of symbol type valid */
#define SYM_MASK_EXACT	0x1ff		/* match symbol pointer exactly */
#define SYM_BASIC_MASK	0x0f		/* only basic types are of interest */
#define SYM_SDA_MASK	0xc0		/* for SDA and SDA2 symbols */

/* HIDDEN */

typedef signed char SYM_TYPE;		/* SYM_TYPE */

typedef struct			/* SYMBOL - entry in symbol table */
    {
    SL_NODE	nameHNode;	/* hash node (must come first) */
    char	*name;		/* pointer to symbol name */
    char	*value;		/* symbol value */
    UINT16	group;		/* symbol group */
    SYM_TYPE	type;		/* symbol type */
    } SYMBOL;

/* END_HIDDEN */

#ifdef __cplusplus
}
#endif

#endif /* __INCsymbolh */
