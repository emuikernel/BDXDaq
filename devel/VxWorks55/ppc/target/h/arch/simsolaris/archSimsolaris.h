/* archSimsparc.h - simsparc specific header */

/* Copyright 1993-95 Wind River Systems, Inc. */
/*
modification history
--------------------
01d,13mar01,sn   SPR 73723 - define supported toolchains
01c,29jul98,ms   added WV_INSTRUMENTATION
01b,16dec96,yp   va_start now passes LASTARG to builtin_next_arg
01a,07jun95,ism  derived from simsparc
*/

 /* XXX -- change all h files sparc to solaris */
#ifndef __INCarchSimsparch
#define __INCarchSimsparch

#ifdef __cplusplus
extern "C" {
#endif

#define _ARCH_SUPPORTS_GCC

#define	WV_INSTRUMENTATION
#define	_ALLOC_ALIGN_SIZE	8		/* 8-byte alignment */
#define _STACK_ALIGN_SIZE	8		/* 8-byte alignment */

#define _DYNAMIC_BUS_SIZING	FALSE		/* no dynamic bus sizing */

/* defines for stdarg.h */
/* Amount of space required in an argument list for an arg of type TYPE.
   TYPE may alternatively be an expression whose type is used.  */

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#if     CPU==SIMSPARCSOLARIS
#ifdef  __GNUC__
#ifndef __sparc__
#define __sparc__
#endif  /* __sparc__ */
#endif  /* __GNUC__ */
#endif  /* CPU==SIMSPARCSOLARIS */

#ifndef __sparc__
#define _ARCH_va_start(AP, LASTARG)                                     \
 (AP = ((char *) &(LASTARG) + __va_rounded_size (LASTARG)))
#else
#define _ARCH_va_start(AP, LASTARG)                                     \
  (__builtin_saveregs (), AP = ((char *) __builtin_next_arg (LASTARG)))
#endif

#define _ARCH_va_end(AP)

/* Avoid errors if compiling GCC v2 with GCC v1.  */
#if __GNUC__ == 1
#define __extension__
#endif

/* RECORD_TYPE args passed using the C calling convention are
   passed by invisible reference.  ??? RECORD_TYPE args passed
   in the stack are made to be word-aligned; for an aggregate that is
   not word-aligned, we advance the pointer to the first non-reg slot.  */
#define _ARCH_va_arg(pvar,TYPE)					\
__extension__							\
({ TYPE __va_temp;						\
   ((__builtin_classify_type (__va_temp) >= 12)			\
    ? ((pvar) += __va_rounded_size (TYPE *),			\
       **(TYPE **) ((pvar) - __va_rounded_size (TYPE *)))	\
    : __va_rounded_size (TYPE) == 8				\
    ? ({ union {TYPE d; int i[2];} u;				\
	 u.i[0] = ((int *) (pvar))[0];				\
	 u.i[1] = ((int *) (pvar))[1];				\
	 (pvar) += 8;						\
	 u.d; })						\
    : ((pvar) += __va_rounded_size (TYPE),			\
       *((TYPE *) ((pvar) - __va_rounded_size (TYPE)))));})

#ifdef __cplusplus
}
#endif

#endif /* __INCarchSimsparch */
