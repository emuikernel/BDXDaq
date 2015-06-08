/* i8259a.h - Intel 8259a PIC (Programmable Interrupt Controller) */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,17mar97,hdn	 added function declarations.
01d,03jan95,vin	 added PIC_ISR_MASK, PIC_IRR_MASK, OCW3
01c,16aug93,hdn	 deleted PIC_BASE_ADRS macros.
01b,16jun93,hdn	 updated to 5.1.
01a,15may92,hdn	 written.
*/

#ifndef	__INCi8259ah
#define	__INCi8259ah

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The macro PIC_REG_ADDR_INTERVAL must be defined
 * when including this header.
 */

/* default definitions */

#define	CAST

#define PIC_ADRS(base,reg)   (CAST (base+(reg*PIC_REG_ADDR_INTERVAL)))

/* register definitions */

#define PIC_port1(base)	PIC_ADRS(base,0x00)	/* port 1. */
#define PIC_port2(base)	PIC_ADRS(base,0x01)	/* port 2. */

/* alias */

#define PIC_IMASK(base)	PIC_port2(base)		/* Interrupt mask. */
#define PIC_IACK(base)	PIC_port1(base)		/* Interrupt acknowledge. */

#define PIC_ISR_MASK(base) PIC_port1(base)	/* in-service register mask */
#define PIC_IRR_MASK(base) PIC_port1(base) 	/* interrupt request reg */

#define OCW3_DEF	0x08			/* 3rd default control word */
#define OCW3_PCB	0x04			/* Polling Control Bit */
#define OCW3_ISR	0x03			/* Read in-service reg */
#define OCW3_IRR	0x02			/* Read inter request reg */

#ifndef _ASMLANGUAGE

#if defined(__STDC__) || defined(__cplusplus)

IMPORT int	sysIntLevel	(void);
IMPORT VOID	sysIntLock	(void);
IMPORT VOID	sysIntUnlock	(void);

#else

IMPORT int	sysIntLevel	();
IMPORT VOID	sysIntLock	();
IMPORT VOID	sysIntUnlock	();

#endif  /* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* __INCi8259ah */
