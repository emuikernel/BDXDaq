/* ioApic.h - Intel IO APIC/xAPIC (Advanced PIC) */

/* Copyright 1984-2002 Wind River Systems, Inc. */ 

/*
modification history
--------------------
01e,15may02,hdn  removed ifdef for IOAPIC_DATA
01d,08mar02,hdn  added prototypes for HTT (spr 73738)
01c,26jun01,hdn  added support for Pentium4 IO xAPIC
01b,25may98,hdn  re-written
01a,30jun97,sub  written
*/

#ifndef __INCioApich
#define __INCioApich

#ifdef __cplusplus
extern "C" {
#endif


/* IO APIC direct register offset */

#define	IOAPIC_DATA		0x10	/* IO window (data) - pc.h */ 
#define IOAPIC_IRQPA		0x20	/* IRQ Pin Assertion Register */ 
#define IOAPIC_EOI		0x40	/* EOI Register */ 

/* IO APIC indirect register offset */

#define IOAPIC_ID		0x00 	/* IOAPIC ID */
#define IOAPIC_VERS		0x01 	/* IOAPIC Version */
#define IOAPIC_ARB		0x02	/* IOAPIC Arbitration ID */
#define IOAPIC_BOOT		0x03	/* IOAPIC Boot Configuration */
#define IOAPIC_REDTBL		0x10 	/* Redirection Table (24 * 64bit) */

/* Interrupt delivery type */

#define IOAPIC_DT_APIC		0x0	/* APIC serial bus */
#define IOAPIC_DT_FS		0x1	/* Front side bus message*/

/* Version register bits */

#define IOAPIC_MRE_MASK		0x00ff0000	/* Max Red. entry mask */
#define IOAPIC_PRQ		0x00008000	/* this has IRQ reg */
#define IOAPIC_VERSION		0x000000ff	/* version number */

/* Redirection table entry number */

#define MAX_REDTABLE_ENTRIES	32

/* Redirection table entry bits: upper 32 bit */

#define IOAPIC_DESTINATION	0xff000000

/* Redirection table entry bits: lower 32 bit */

#define IOAPIC_INT_MASK		0x00010000
#define IOAPIC_LEVEL		0x00008000
#define IOAPIC_EDGE		0x00000000
#define IOAPIC_REMOTE		0x00004000
#define IOAPIC_LOW		0x00002000
#define IOAPIC_HIGH		0x00000000
#define IOAPIC_LOGICAL		0x00000800
#define IOAPIC_PHYSICAL		0x00000000
#define IOAPIC_FIXED		0x00000000
#define IOAPIC_LOWEST		0x00000100
#define IOAPIC_SMI		0x00000200
#define IOAPIC_NMI		0x00000400
#define IOAPIC_INIT		0x00000500
#define IOAPIC_EXTINT		0x00000700
#define IOAPIC_VEC_MASK		0x000000ff


#ifndef	_ASMLANGUAGE


/* variable declarations */

IMPORT UINT32	ioApicVersion;
IMPORT UINT32	ioApicRedEntries;


#if	defined(__STDC__) || defined(__cplusplus)

IMPORT STATUS   ioApicInit       (void);
IMPORT VOID     ioApicEnable     (BOOL enable);
IMPORT STATUS	ioApicIrqSet	 (INT32 irq, INT32 apicId);
IMPORT STATUS   ioApicRedGet     (INT32 irq, INT32 * pUpper32, INT32 * pLower32);
IMPORT STATUS   ioApicRedSet     (INT32 irq, INT32 upper32, INT32 lower32);
IMPORT INT32    ioApicGet        (UINT32 index, UINT32 data, INT32 offset);
IMPORT void     ioApicSet        (UINT32 index, UINT32 data, INT32 offset, INT32 value);

#else

IMPORT STATUS   ioApicInit       ();
IMPORT VOID     ioApicEnable     ();
IMPORT STATUS	ioApicIrqSet	 ();
IMPORT STATUS   ioApicRedGet     ();
IMPORT STATUS   ioApicRedSet     ();
IMPORT INT32    ioApicGet        ();
IMPORT void     ioApicSet        ();

#endif  /* __STDC__ */


#endif	/* _ASMLANGUAGE */


#ifdef __cplusplus
}
#endif

#endif /* __INCioApich */
