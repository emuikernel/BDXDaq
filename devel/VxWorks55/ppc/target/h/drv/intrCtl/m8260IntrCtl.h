/* m8260IntrCtl.h - MPC 8260 interrupt controller header file */

/* Copyright 1991-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,17sep99,ms_  remove period in __INCm8260Intr
01f,17sep99,ms_  try removing config.h
01e,15jul99,ms_  make compliant with our coding standards
01d,17apr99,ms_  final EAR cleanup
01c,15apr99,ms_  add default interrupt priority ordering
01b,13apr99,ms_  redefine IVEC_TO_INUM and INUM_TO_IVEC to use the numbers as
                 shown in the 
MPC8260 user's manaul
01a,21jan99,ms_  adapted from ppc860Intr.h
*/

#ifndef __INCm8260IntrCtlh
#define __INCm8260IntrCtlh

#include "vxWorks.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  _ASMLANGUAGE
#define CAST(x)
#else /* _ASMLANGUAGE */
#define CAST(x) (x)
#endif  /* _ASMLANGUAGE */

#ifdef IVEC_TO_INUM
# undef IVEC_TO_INUM
#endif /* IVEC_TO_INUM */

#ifdef INUM_TO_IVEC
# undef INUM_TO_IVEC
#endif /* INUM_TO_IVEC */

#define IVEC_TO_INUM(intVec)    (m8260IvecToInum(intVec))
#define INUM_TO_IVEC(intNum)    (m8260InumToIvec(intNum))


/* Interrupt Controller Registers */

#define	M8260_SICR(base)	((VINT32 *) ((base) + 0x010C00))
#define	M8260_SIVEC(base)	((VINT32 *) ((base) + 0x010C04))
#define	M8260_SIPNR_H(base)	((VINT32 *) ((base) + 0x010C08))
#define	M8260_SIPNR_L(base)	((VINT32 *) ((base) + 0x010C0C))
#define	M8260_SIPRR(base)	((VINT32 *) ((base) + 0x010C10))
#define	M8260_SCPRR_H(base)	((VINT32 *) ((base) + 0x010C14))
#define	M8260_SCPRR_L(base)	((VINT32 *) ((base) + 0x010C18))
#define	M8260_SIMR_H(base)	((VINT32 *) ((base) + 0x010C1C))
#define	M8260_SIMR_L(base)	((VINT32 *) ((base) + 0x010C20))
#define	M8260_SIEXR(base)	((VINT32 *) ((base) + 0x010C24))


/* interrupt numbers  */

#define INUM_ERROR		0	/* Error */
#define INUM_I2C		1	/* I2C */
#define INUM_SPI		2	/* SPI */
#define INUM_RISC_TIMER		3	/* Risc Timer Table */
#define INUM_SMC1		4	/* SMC 1 */
#define INUM_SMC2		5	/* SMC 2 */
#define INUM_IDMA1		6	/* IDMA1 */
#define INUM_IDMA2		7	/* IDMA2 */
#define INUM_IDMA3		8	/* IDMA3 */
#define INUM_IDMA4		9	/* IDMA4 */
#define INUM_SDMA		10	/* SDMA */
#define INUM_RESERVED11		11	/* reserved */
#define INUM_TIMER1		12	/* Timer 1 */
#define INUM_TIMER2		13	/* Timer 2 */
#define INUM_TIMER3		14	/* Timer 3 */
#define INUM_TIMER4		15	/* Timer 4 */
#define INUM_TMCNT		16	/* TMCNT */
#define INUM_PIT		17	/* PIT */
#define	INUM_RESERVED18		18       /* reserved */
#define INUM_IRQ1		19	/* IRQ 1 interrupt */
#define INUM_IRQ2		20	/* IRQ 2 interrupt */
#define INUM_IRQ3		21	/* IRQ 3 interrupt */
#define INUM_IRQ4		22	/* IRQ 4 interrupt */
#define INUM_IRQ5		23	/* IRQ 5 interrupt */
#define INUM_IRQ6		24	/* IRQ 6 interrupt */
#define INUM_IRQ7		25	/* IRQ 7 interrupt */
#define INUM_RESERVED26		26	/* reserved */
#define INUM_FCC1		27	/* FCC1 */
#define INUM_FCC2		28	/* FCC2 */
#define INUM_FCC3		29	/* FCC3 */
#define INUM_RESERVED30		30	/* reserved */
#define INUM_MCC1		31	/* MCC1 */
#define INUM_MCC2		32	/* MCC2 */
#define INUM_RESERVED33		33	/* reserved */
#define INUM_RESERVED34		34	/* reserved */
#define INUM_SCC1		35	/* SCC1 */
#define INUM_SCC2		36	/* SCC2 */
#define INUM_SCC3		37	/* SCC3 */
#define INUM_SCC4		38	/* SCC4 */
#define INUM_RESERVED39		39	/* reserved */
#define INUM_PC15		40	/* Parallel I/O [15] */
#define INUM_PC14		41	/* Parallel I/O [14] */
#define INUM_PC13		42	/* Parallel I/O [13] */
#define INUM_PC12		43	/* Parallel I/O [12] */
#define INUM_PC11		44	/* Parallel I/O [11] */
#define INUM_PC10		45	/* Parallel I/O [10] */
#define INUM_PC9		46	/* Parallel I/O [9] */
#define INUM_PC8		47	/* Parallel I/O [8] */
#define INUM_PC7		48	/* Parallel I/O [7] */
#define INUM_PC6		49	/* Parallel I/O [6] */
#define INUM_PC5		50	/* Parallel I/O [5] */
#define INUM_PC4		51	/* Parallel I/O [4] */
#define INUM_PC3		52	/* Parallel I/O [3] */
#define INUM_PC2		53	/* Parallel I/O [2] */
#define INUM_PC1		54	/* Parallel I/O [1] */
#define INUM_PC0		55	/* Parallel I/O [0] */

#define INUM_MAX		INUM_PC0
#define IVEC_MAX		(INUM_PC0 + 8)

/* SICR Masks*/

#define MPC8260_CICR_SCCDP_MSK	0xc00000	/* SCCd Priority Order Mask */
#define MPC8260_CICR_SCCDP_SCC1	0x000000	/* SCC1 asserts the SCCd */ 
#define MPC8260_CICR_SCCDP_SCC2	0x400000	/* SCC2 asserts the SCCd */ 
#define MPC8260_CICR_SCCDP_SCC3	0x800000	/* SCC3 asserts the SCCd */ 
#define MPC8260_CICR_SCCDP_SCC4	0xc00000	/* SCC4 asserts the SCCd */ 
#define MPC8260_CICR_SCCCP_MSK	0x300000	/* SCCc Priority Order Mask */
#define MPC8260_CICR_SCCCP_SCC1	0x000000	/* SCC1 asserts the SCCc */ 
#define MPC8260_CICR_SCCCP_SCC2	0x100000	/* SCC2 asserts the SCCc */ 
#define MPC8260_CICR_SCCCP_SCC3	0x200000	/* SCC3 asserts the SCCc */ 
#define MPC8260_CICR_SCCCP_SCC4	0x300000	/* SCC4 asserts the SCCc */ 
#define MPC8260_CICR_SCCBP_MSK	0x0c0000	/* SCCb Priority Order Mask */
#define MPC8260_CICR_SCCBP_SCC1	0x000000	/* SCC1 asserts the SCCb */ 
#define MPC8260_CICR_SCCBP_SCC2	0x040000	/* SCC2 asserts the SCCb */ 
#define MPC8260_CICR_SCCBP_SCC3	0x080000	/* SCC3 asserts the SCCb */ 
#define MPC8260_CICR_SCCBP_SCC4	0x0c0000	/* SCC4 asserts the SCCb */ 
#define MPC8260_CICR_SCCAP_MSK	0x030000	/* SCCa Priority Order Mask */
#define MPC8260_CICR_SCCAP_SCC1	0x000000	/* SCC1 asserts the SCCa */ 
#define MPC8260_CICR_SCCAP_SCC2	0x010000	/* SCC2 asserts the SCCa */ 
#define MPC8260_CICR_SCCAP_SCC3	0x020000	/* SCC3 asserts the SCCa */ 
#define MPC8260_CICR_SCCAP_SCC4	0x030000	/* SCC4 asserts the SCCa */ 
#define MPC8260_CICR_IRL_MSK	0x00e000	/* Interrupt Request Level */
#define MPC8260_CICR_IRL_LVL0	0x000000	/* Interrupt Request Level 0 */
#define MPC8260_CICR_IRL_LVL1	0x002000	/* Interrupt Request Level 1 */
#define MPC8260_CICR_IRL_LVL2	0x004000	/* Interrupt Request Level 2 */
#define MPC8260_CICR_IRL_LVL3	0x006000	/* Interrupt Request Level 3 */
#define MPC8260_CICR_IRL_LVL4	0x008000	/* Interrupt Request Level 4 */
#define MPC8260_CICR_IRL_LVL5	0x00a000	/* Interrupt Request Level 5 */
#define MPC8260_CICR_IRL_LVL6	0x00c000	/* Interrupt Request Level 6 */
#define MPC8260_CICR_IRL_LVL7	0x00e000	/* Interrupt Request Level 7 */
#define MPC8260_CICR_HP_MSK	0x001f00	/* Highest Priority */
#define MPC8260_CICR_IEN	0x000080	/* Interrupt Enable */
#define MPC8260_CICR_SPS	0x000001	/* Spread Priority Scheme */

/* interrupt handler */

#ifndef _ASMLANGUAGE
typedef struct intr_handler
    {
    VOIDFUNCPTR vec;    /* interrupt vector */
    int         arg;    /* interrupt handler argument */
    } INTR_HANDLER;

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

IMPORT STATUS m8260IntrInit ();
extern STATUS m8260IntDisable (int );
IMPORT STATUS m8260IntEnable (int );
extern int m8260IvecToInum(VOIDFUNCPTR *);
extern VOIDFUNCPTR * m8260InumToIvec(int);

#else

IMPORT STATUS m8260IntrInit ();
extern STATUS m8260IntDisable ();
IMPORT STATUS m8260IntEnable ();
extern int m8260IvecToInum();
extern VOIDFUNCPTR * m8260InumToIvec();

#endif	/* __STDC__ */
#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCm8260IntrCtlh */
