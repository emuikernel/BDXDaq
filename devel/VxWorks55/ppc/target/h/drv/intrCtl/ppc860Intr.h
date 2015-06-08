/* ppc860Intr.h - PowerPC 860 interrupt header file */

/* Copyright 1991-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,09nov98,cn   added IVEC_TO_IRQ, IVEC_TO_ILEVEL macros.
01c,09mar98,map  added IV_RESERVED59, 60 for compatiblity [SPR# 20114]
01b,10jun97,map  added IV_SCC[34] definitions [SPR# 8450]
01a,17apr96,tpr  written.
*/

#ifndef __INCppc860Intrh
#define __INCppc860Intrh

#ifdef __cplusplus
extern "C" {
#endif

/* interrupt number definitions */

#define IV_IRQ0			INUM_TO_IVEC (0)	/* IRQ 0 interrupt */
#define IV_LEVEL0		INUM_TO_IVEC (1)	/* level 0 interrupt */
#define IV_IRQ1			INUM_TO_IVEC (2)	/* IRQ 1 interrupt */
#define IV_LEVEL1		INUM_TO_IVEC (3)	/* level 1 interrupt */
#define IV_IRQ2			INUM_TO_IVEC (4)	/* IRQ 2 interrupt */
#define IV_LEVEL2		INUM_TO_IVEC (5)	/* level 2 interrupt */
#define IV_IRQ3			INUM_TO_IVEC (6)	/* IRQ 3 interrupt */
#define IV_LEVEL3		INUM_TO_IVEC (7)	/* level 3 interrupt */
#define IV_IRQ4			INUM_TO_IVEC (8)	/* IRQ 4 interrupt */
#define IV_LEVEL4		INUM_TO_IVEC (9)	/* level 4 interrupt */
#define IV_IRQ5			INUM_TO_IVEC (10)	/* IRQ 5 interrupt */
#define IV_LEVEL5		INUM_TO_IVEC (11)	/* level 5 interrupt */
#define IV_IRQ6			INUM_TO_IVEC (12)	/* IRQ 6 interrupt */
#define IV_LEVEL6		INUM_TO_IVEC (13)	/* level 6 interrupt */
#define IV_IRQ7			INUM_TO_IVEC (14)	/* IRQ 7 interrupt */
#define IV_LEVEL7		INUM_TO_IVEC (15)	/* level 7 interrupt */

#define IV_RESERVE0		INUM_TO_IVEC (16)	/* reserved */
#define IV_RESERVE1		INUM_TO_IVEC (17)	/* reserved */
#define IV_RESERVE2		INUM_TO_IVEC (18)	/* reserved */
#define IV_RESERVE3		INUM_TO_IVEC (19)	/* reserved */
#define IV_RESERVE4		INUM_TO_IVEC (20)	/* reserved */
#define IV_RESERVE5		INUM_TO_IVEC (21)	/* reserved */
#define IV_RESERVE6		INUM_TO_IVEC (22)	/* reserved */
#define IV_RESERVE7		INUM_TO_IVEC (23)	/* reserved */
#define IV_RESERVE8		INUM_TO_IVEC (24)	/* reserved */
#define IV_RESERVE9		INUM_TO_IVEC (25)	/* reserved */
#define IV_RESERVE10		INUM_TO_IVEC (26)	/* reserved */
#define IV_RESERVE11		INUM_TO_IVEC (27)	/* reserved */
#define IV_RESERVE12		INUM_TO_IVEC (28)	/* reserved */
#define IV_RESERVE13		INUM_TO_IVEC (29)	/* reserved */
#define IV_RESERVE14		INUM_TO_IVEC (30)	/* reserved */
#define IV_RESERVE15		INUM_TO_IVEC (31)	/* reserved */

#define IV_ERROR		INUM_TO_IVEC (32)	/* Error */
#define IV_PC4			INUM_TO_IVEC (33)	/* Parallel I/O [4] */
#define IV_PC5			INUM_TO_IVEC (34)	/* Parallel I/O [5] */
#define IV_SMC2_PIP		INUM_TO_IVEC (35)	/* SMC 2/PIP */
#define IV_SMC1			INUM_TO_IVEC (36)	/* SMC 1 */
#define IV_SPI			INUM_TO_IVEC (37)	/* SPI */
#define IV_PC6			INUM_TO_IVEC (38)	/* Parallel I/O [6] */
#define IV_TIMER4		INUM_TO_IVEC (39)	/* Timer 4 */
#define IV_RESERVED40		INUM_TO_IVEC (40)	/* reserved */
#define IV_PC7			INUM_TO_IVEC (41)	/* Parallel I/O [7] */
#define IV_PC8			INUM_TO_IVEC (42)	/* Parallel I/O [8] */
#define IV_PC9			INUM_TO_IVEC (43)	/* Parallel I/O [9] */
#define IV_TIMER3		INUM_TO_IVEC (44)	/* Timer 3 */
#define IV_RESERVED45		INUM_TO_IVEC (45)	/* reserved */
#define IV_PC10			INUM_TO_IVEC (46)	/* Parallel I/O [10] */
#define IV_PC11			INUM_TO_IVEC (47)	/* Parallel I/O [11] */
#define IV_I2C			INUM_TO_IVEC (48)	/* I2C */
#define IV_RISC_TIMER_TABLE	INUM_TO_IVEC (49)	/* Risc Timer Table */
#define IV_TIMER2		INUM_TO_IVEC (50)	/* Timer 2 */
#define IV_RESERVED51		INUM_TO_IVEC (51)	/* reserved */
#define IV_IDMA2		INUM_TO_IVEC (52)	/* IDMA2 */
#define IV_IDMA1		INUM_TO_IVEC (53)	/* IDMA1 */
#define IV_SDMA			INUM_TO_IVEC (54)	/* SDMA bus error */
#define IV_PC12			INUM_TO_IVEC (55)	/* Parallel I/O [12] */
#define IV_PC13			INUM_TO_IVEC (56)	/* Parallel I/O [13] */
#define IV_TIMER1		INUM_TO_IVEC (57)	/* Timer 1 */
#define IV_PC14			INUM_TO_IVEC (58)	/* Parallel I/O [14] */
#define IV_SCC4			INUM_TO_IVEC (59)	/* SCC4 */
#define	IV_RESERVED59		IV_SCC4			
#define IV_SCC3			INUM_TO_IVEC (60)	/* SCC3 */
#define	IV_RESERVED60		IV_SCC3	
#define IV_SCC2			INUM_TO_IVEC (61)	/* SCC2 */
#define IV_SCC1			INUM_TO_IVEC (62)	/* SCC1 */
#define IV_PC15			INUM_TO_IVEC (63)	/* Parallel I/O [15] */

#define NUM_VEC_MAX		64

/* macro to get to the IRQ/ILEVEL from the vector number */

#define IVEC_TO_IRQ(x)		(int) (x >> 1)		/* interrupt request*/
#define IVEC_TO_ILEVEL(x)	(int) (x >> 1)		/* interrupt level */

/* typedefs */

/* interrupt handler */

#ifndef _ASMLANGUAGE
typedef struct intr_handler
    {
    VOIDFUNCPTR vec;    /* interrupt vector */
    int         arg;    /* interrupt handler argument */
    } INTR_HANDLER;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

IMPORT STATUS ppc860IntrInit (VOIDFUNCPTR *);

#else

IMPORT STATUS ppc860IntrInit ();

#endif	/* __STDC__ */
#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCppc860Intrh */
