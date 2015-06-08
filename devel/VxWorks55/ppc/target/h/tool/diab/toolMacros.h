/* toolMacros.h - compile time macros for Diab C compiler */

/* Copyright 2001 Wind River Systems, Alameda, CA */

/*
modification history
--------------------
01d,26nov01,dat  New macros for alignment checking and unaligned copies
01c,14nov01,dat  Adding underscores to macro names
01b,18oct01,jab  fixed WRS_ASM macro
01a,04sep01,dat  written (assumes Diab 4.4b or later)
*/

#ifndef __INCtoolMacrosh
#define __INCtoolMacrosh

#define _WRS_PACK_ALIGN(x) __attribute__((packed, aligned(x)))

#define _WRS_ASM(x) __asm volatile (x)

#define _WRS_DATA_ALIGN_BYTES(x) __attribute__((aligned(x)))

/* This tool uses the ANSI variadic macro style */

#undef _WRS_GNU_VAR_MACROS

/* New macros for alignment issues */

#define _WRS_ALIGNOF(x) 	sizeof(x,1)

#define _WRS_UNALIGNED_COPY(pSrc, pDest, size) \
	(memcpy ((pDest), (pSrc), (size)))

#define _WRS_ALIGN_CHECK(ptr, type) \
	(((int)(ptr) & ( _WRS_ALIGNOF(type) - 1)) == 0 ? TRUE : FALSE)

/*
 * For backward compatibility with v5 of portable C coding guide.
 *
 * These are now obsolete, please don't use them.
 */

#define WRS_PACK_ALIGN(x)	_WRS_PACK_ALIGN(x)
#define WRS_ASM(x) 		_WRS_ASM(x)
#define WRS_DATA_ALIGN_BYTES(x)	_WRS_DATA_ALIGN_BYTES(x)
#undef WRS_GNU_VAR_MACROS

#endif /* __INCtoolMacrosh */
