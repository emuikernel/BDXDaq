/* ppc509Intr.h - PowerPC 509 interrupt header file */

/* Copyright 1991-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,16mar99,zl   written.
*/

#ifndef __INCppc509Intrh
#define __INCppc509Intrh

#ifdef __cplusplus
extern "C" {
#endif

/* interrupt number definitions */

#define LEVELN_PRIORITY_LVL(n)	(2*n + 1)
#define IRQN_PRIORITY_LVL(n)	(2*n)

#define INT_LEVEL0		0	/* level 0 interrupt */
#define INT_LEVEL1		1	/* level 1 interrupt */
#define INT_LEVEL2		2	/* level 2 interrupt */
#define INT_LEVEL3		3	/* level 3 interrupt */
#define INT_LEVEL4		4	/* level 4 interrupt */
#define INT_LEVEL5		5	/* level 5 interrupt */
#define INT_LEVEL6		6	/* level 6 interrupt */
#define INT_LEVEL7		7	/* level 7 interrupt */

#define INT_IRQ0		0	/* IRQ 0 interrupt */
#define INT_IRQ1		1	/* IRQ 1 interrupt */
#define INT_IRQ2		2	/* IRQ 2 interrupt */
#define INT_IRQ3		3	/* IRQ 3 interrupt */
#define INT_IRQ4		4	/* IRQ 4 interrupt */
#define INT_IRQ5		5	/* IRQ 5 interrupt */
#define INT_IRQ6		6	/* IRQ 6 interrupt */
#define INT_IRQ7		7	/* IRQ 7 interrupt */


#define NUM_VEC_MAX	64

/* typedefs */

/* interrupt handler */

#ifndef _ASMLANGUAGE
typedef struct intr_handler
    {
    VOIDFUNCPTR vec;    		/* interrupt vector */
    int         arg;    		/* interrupt handler argument */
    } INTR_HANDLER;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

IMPORT STATUS ppc509IntrInit (void);

#else

IMPORT STATUS ppc509IntrInit ();

#endif	/* __STDC__ */
#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCppc509Intrh */
