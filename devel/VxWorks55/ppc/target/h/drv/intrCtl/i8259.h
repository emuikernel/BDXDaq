/* i8259.h - Intel 8259a PIC (Programmable Interrupt Controller) */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,23apr02,hdn	 added prototypes for new BOI/EOI rtn (spr 76411)
01g,08mar02,hdn	 added prototype i8259Init() for HTT (spr 73738)
01f,28may97,hdn	 copied i8259a.h to i8259.h for APIC support.
01e,17mar97,hdn	 added function declarations.
01d,03jan95,vin	 added PIC_ISR_MASK, PIC_IRR_MASK, OCW3
01c,16aug93,hdn	 deleted PIC_BASE_ADRS macros.
01b,16jun93,hdn	 updated to 5.1.
01a,15may92,hdn	 written.
*/

#ifndef	__INCi8259h
#define	__INCi8259h

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

#define I8259_EOI	0x20			/* EOI bit in OCW2 */

#ifndef _ASMLANGUAGE

#if defined(__STDC__) || defined(__cplusplus)

IMPORT VOID	i8259Init	(void);
IMPORT int	sysIntLevel	(int arg);
IMPORT VOID	sysIntLock	(void);
IMPORT VOID	sysIntUnlock	(void);
IMPORT VOID 	i8259IntBoi 	(int irqNo);
IMPORT VOID 	i8259IntBoiEem	(int irqNo);
IMPORT VOID 	i8259IntBoiSmm	(int irqNo);
IMPORT VOID 	i8259IntEoiSmm	(int irqNo);
IMPORT VOID 	i8259IntEoiMaster (int irqNo);
IMPORT VOID 	i8259IntEoiSlave (int irqNo);
IMPORT VOID 	i8259IntEoiSlaveNfnm (int irqNo);
IMPORT VOID 	i8259IntEoiSlaveSfnm (int irqNo);

#else

IMPORT VOID	i8259Init	();
IMPORT int	sysIntLevel	();
IMPORT VOID	sysIntLock	();
IMPORT VOID	sysIntUnlock	();
IMPORT VOID 	i8259IntBoi 	();
IMPORT VOID 	i8259IntBoiEem	();
IMPORT VOID 	i8259IntBoiSmm	();
IMPORT VOID 	i8259IntEoiSmm	();
IMPORT VOID 	i8259IntEoiMaster ();
IMPORT VOID 	i8259IntEoiSlave ();
IMPORT VOID 	i8259IntEoiSlaveNfnm ();
IMPORT VOID 	i8259IntEoiSlaveSfnm ();

#endif  /* __STDC__ */

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* __INCi8259h */
