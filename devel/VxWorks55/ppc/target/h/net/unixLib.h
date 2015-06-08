/* unixLib.h - UNIX kernel compatability library header file */

/* Copyright 1984-1992 Wind River Systems, Inc. */
/*
modification history
--------------------
01f,05oct97,vin  added DATA_TO_MBLK macro
01e,31mar97,vin  added LIST macros for protocol control blocks.
01d,05dec96,vin  redefined MALLOC and FREE so that they use network buffers.
		 removed unnecessary macros M_WAITOK, M_NOWAIT, M_....
01c,22sep92,rrr  added support for c++
01b,26may92,rrr  the tree shuffle
01a,01apr92,elh  written.
*/

#ifndef __INCunixLibh
#define __INCunixLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "semLib.h"

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/* linked list macros */

#define LIST_HEAD(name, type)						\
	struct name							\
            {								\
            struct type *lh_first;	/* first element */		\
            }

#define LIST_ENTRY(type)						\
struct {								\
	struct type *le_next;	/* next element */			\
	struct type **le_prev;	/* address of previous next element */	\
}

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/*
 * List functions.
 */
#define	LIST_INIT(head) {						\
	(head)->lh_first = NULL;					\
}

#define LIST_INSERT_HEAD(head, elm, field) {				\
	if (((elm)->field.le_next = (head)->lh_first) != NULL)		\
		(head)->lh_first->field.le_prev = &(elm)->field.le_next;\
	(head)->lh_first = (elm);					\
	(elm)->field.le_prev = &(head)->lh_first;			\
}

#define LIST_REMOVE(elm, field) {					\
	if ((elm)->field.le_next != NULL)				\
		(elm)->field.le_next->field.le_prev = 			\
		    (elm)->field.le_prev;				\
	*(elm)->field.le_prev = (elm)->field.le_next;			\
}


#define MALLOC(space, cast, size, type, flags) { \
		(space) = (cast)_netMalloc((size), (type), (flags)); \
}						 

#define FREE(addr, type) { \
            _netFree((caddr_t)(addr)); \
}			   

#define DATA_TO_MBLK(pBuf)	\
     (*((struct mbuf **)((char *)(pBuf) - sizeof(struct mbuf **))))
            
/* Function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern int 	splnet (void);
extern int 	splimp (void);
extern void 	splx (int x);
extern void 	panic (char *msg);
extern void 	wakeup (SEM_ID semId);
extern void 	ksleep (SEM_ID semId);
extern char *	_netMalloc (int bufSize, UCHAR type, int canWait); 
extern void	_netFree (char * pBuf); 
extern void * 	hashinit (int 	elements, int type, u_long * hashmask);

#else	/* __STDC__ */

extern int	splnet ();
extern int	splimp ();
extern void	splx ();
extern void 	panic ();
extern void 	wakeup ();
extern void 	ksleep();
extern char *	_netMalloc ();
extern void	_netFree ();
extern void * 	hashinit ();

#endif	/* __STDC__ */


#ifdef __cplusplus
}
#endif

#endif /* __INCunixLibh */
