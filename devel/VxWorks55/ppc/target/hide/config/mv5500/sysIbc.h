/* sysIbc.h - IBC register definitions */

/* Copyright 2000-2002 Wind River Systems, Inc. */
/* Copyright 2000-2003 Motorola, Inc. */

/*
modification history
--------------------
01a,11mar03,cak  Ported (from ver 01a), mv6100/sysIbc.h.
*/

/*
Description: Contains structure typedefs and device-specific 
defines for Programable Interrupt Bridge (8259 type) chips.
*/

#ifndef	INCsysIbch
#define	INCsysIbch

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* includes */

/* structures */

/* macros */

/* default definitions */

#define PIC_REG_ADDR_INTERVAL   1       /* address diff of adjacent regs. */
#define PIC_ADRS(base,reg)   (base+(reg*PIC_REG_ADDR_INTERVAL))

/* register definitions */

#define PIC_port1(base)	PIC_ADRS(base,0x00)	/* port 1. */
#define PIC_port2(base)	PIC_ADRS(base,0x01)	/* port 2. */

/* alias */

#define PIC_IMASK(base)	PIC_port2(base)		/* Interrupt mask. */
#define PIC_IACK(base)	PIC_port1(base)		/* Interrupt acknowledge. */

#define PIC_ISR_MASK(base) PIC_port1(base)	/* in-service register mask */
#define PIC_IRR_MASK(base) PIC_port1(base) 	/* interrupt request reg */

/* 
 * PCI Config Header register definitions
 */

#define SL82565_PCI_CFG_PCI_CTL 	0x40	/* Offset to PCI control reg */
#define SL82565_PCI_CTL_RETRYE		0x02	/* PCI control RETRYE bit */

/*
 *	Non-PCI Config Header register definitions
 *
 *	These are mapped to ISA I/O Space
 */

#define SL82565_INT1_CTRL        (ISA_MSTR_IO_LOCAL + 0x0020)	/* PIC1 */
#define SL82565_INT1_MASK        (ISA_MSTR_IO_LOCAL + 0x0021)
#define SL82565_INT1_ELC         (ISA_MSTR_IO_LOCAL + 0x04d0)
#define SL82565_INT2_CTRL        (ISA_MSTR_IO_LOCAL + 0x00a0)	/* PIC2 */
#define SL82565_INT2_MASK        (ISA_MSTR_IO_LOCAL + 0x00a1)
#define SL82565_INT2_ELC         (ISA_MSTR_IO_LOCAL + 0x04d1)
#define SL82565_TMR1_CNT0        (ISA_MSTR_IO_LOCAL + 0x0040)
#define SL82565_TMR1_CNT1        (ISA_MSTR_IO_LOCAL + 0x0041)
#define SL82565_TMR1_CNT2        (ISA_MSTR_IO_LOCAL + 0x0042)
#define SL82565_TMR1_CMOD        (ISA_MSTR_IO_LOCAL + 0x0043)
#define SL82565_RST_IRQ12        (ISA_MSTR_IO_LOCAL + 0x0060)
#define SL82565_NMI_SCTRL        (ISA_MSTR_IO_LOCAL + 0x0061)
#define SL82565_PCOP             (ISA_MSTR_IO_LOCAL + 0x0c04)
#define SL82565_TMCP             (ISA_MSTR_IO_LOCAL + 0x0c01)

/* programmable interrupt controller (PIC) */

#define PIC1_BASE_ADR           SL82565_INT1_CTRL
#define PIC2_BASE_ADR           SL82565_INT2_CTRL

#define PIC_OCW3_SEL         0x08      /* Select OCW3 */
#define PIC_IRR_READ         0x02      /* Read IRR (Int. req. reg.) */
#define PIC_ISR_READ         0x03      /* Read ISR (In service reg.) */

/*
 *      IBC Extensions to Standard PCI Config Header register offsets
 */
#define PCI_CFG_IBC_INTR_ROUTE  0x44
#define PCI_CFG_IBC_ARB_CTL     0x83

/* PCI Arbiter Control Register bit definitions */

#define ARB_CTL_GAT             (1 << 7)
#define ARB_CTL_TIMEOUT_TIMER   (1 << 2)
#define ARB_CTL_BUS_LOCK        (1 << 0)

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif	/* INCsysIbch */
