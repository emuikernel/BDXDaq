/* archSimhppa.h - simhppa specific header */

/* Copyright 1993 Wind River Systems, Inc. */
/*
modification history
--------------------
01d,29jul98,ms   added WV_INSTRUMENTATION
01c,20nov95,mem  added decl of _ARCH_va_list for c++, fixes for gcc.
01b,20jul94,ms   added _ARCH_INT_MIN, changed _ARCH_MULTIPLE_CACHELIB.
01a,11aug93,gae  from rrr.
*/

#ifndef __INCarchSimhppah
#define __INCarchSimhppah

#ifdef __cplusplus
extern "C" {
#endif

#define	WV_INSTRUMENTATION
#define	_STACK_DIR	_STACK_GROWS_UP


#define	_ALLOC_ALIGN_SIZE	8		/* 8-byte alignment */
#define _STACK_ALIGN_SIZE	8		/* 8-byte alignment */

#define	_ARCH_MULTIPLE_CACHELIB	FALSE		/* multiple cache libraries */

#define _DYNAMIC_BUS_SIZING	FALSE		/* no dynamic bus sizing */

#define _ARCH_INT_MIN		(-2147483647 - 1)

/* defines for stdarg.h */
/* Amount of space required in an argument list for an arg of type TYPE.
   TYPE may alternatively be an expression whose type is used.  */

#ifndef	_ARCH_va_list
#define	_ARCH_va_list	typedef double *va_list
#endif	/* _ARCH_va_list */

#define __WORD_MASK 0xFFFFFFFC
#define __DW_MASK   0xFFFFFFF8

/* Args > 8 bytes are passed by reference.  Args > 4 and <= 8 are
 * right-justified in 8 bytes.  Args <= 4 are right-justified in
 * 4 bytes.
 */
/*
 * __list is the word-aligned address of the previous argument.
 */
/* If sizeof __mode > 8, address of arg is at __list - 4.  We need to do
 * two indirections:  1 to fetch the address, and 1 to fetch the value.
 * If sizeof __mode <= 8, the word-aligned address of arg is 
 * __list - sizeof __mode, masked to requred alignment (4 or 8 byte).  
 * The real address is the word-aligned address + extra byte offset.
 */

#ifdef __GNUC__
#define __gnuc_va_start(AP) \
	((AP) = (va_list)__builtin_saveregs())
#define _ARCH_va_start(__list,__parmN) \
	(__builtin_next_arg (__parmN), __gnuc_va_start (__list))
#else /* not __GNUC__ */
#ifdef __cplusplus
_ARCH_va_list;
extern "C" {
	void __builtin_va_start(va_list, ...);
}
#define _ARCH_va_start(__list,__parmN) \
	(__list=0,__builtin_va_start(__list,&__parmN))
#else /* not __cplusplus */
#define _ARCH_va_start(__list,__parmN) \
	__builtin_va_start (__list, &__parmN)
#endif /* __cplusplus */
#endif /* __GNUC__ */

#define _ARCH_va_arg(__list,__mode)					\
	(sizeof(__mode) > 8 ?						\
	  ((__list = (va_list) ((char *)__list - sizeof (int))),	\
	   (*((__mode *) (*((int *) (__list)))))) :			\
	  ((__list =							\
	      (va_list) ((long)((char *)__list - sizeof (__mode))	\
	      & (sizeof(__mode) > 4 ? __DW_MASK : __WORD_MASK))),	\
	   (*((__mode *) ((char *)__list +				\
		((8 - sizeof(__mode)) % 4))))))

#define _ARCH_va_end(__list)

#ifdef __cplusplus
}
#endif

#endif /* __INCarchSimhppah */
