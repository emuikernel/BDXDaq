/* toolMacros.h - compile time macros for GNU C compiler */

/* Copyright 2001 Wind River Systems, Alameda, CA */

/*
modification history
--------------------
01c,26nov01,dat  New macros for alignment checking and unaligned copies
01b,14nov01,dat  Adding underscores to macro names
01a,18apr01,dat  written
*/

#ifndef __INCtoolMacrosh
#define __INCtoolMacrosh

#define _WRS_PACK_ALIGN(x) __attribute__((packed, aligned(x)))

#define _WRS_ASM(x) __asm__ volatile (x)

#define _WRS_DATA_ALIGN_BYTES(x) __attribute__((aligned(x)))

#define _WRS_GNU_VAR_MACROS

/* New macros for alignment issues */

#define _WRS_ALIGNOF(x) 	__alignof__(x)

#define _WRS_ALIGN_CHECK(ptr, type) \
	(((int)(ptr) & ( _WRS_ALIGNOF(type) - 1)) == 0 ? TRUE : FALSE)

#define _WRS_UNALIGNED_COPY(pSrc, pDest, size) \
	( __builtin_memcpy ((pDest), (void *)(pSrc), size))

/*
 * For compatibility with v5 of portable C coding guide
 *
 * These versions are obsolete, please don't use them
 */

#define WRS_PACK_ALIGN(x)	_WRS_PACK_ALIGN(x)
#define WRS_ASM(x)		_WRS_ASM(x)
#define WRS_DATA_ALIGN_BYTES(x)	_WRS_DATA_ALIGN_BYTES(x)
#define WRS_GNU_VAR_MACROS

#endif /* __INCtoolMacrosh */

