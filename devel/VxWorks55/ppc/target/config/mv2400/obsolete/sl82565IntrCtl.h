/* sl82565IntrCtl.h - sl82565 IBC (ISA Bridge Controller) driver */

/* Copyright 1984-1999 Wind River Systems, Inc. */
/* Copyright 1996, 1998, 1999 Motorola, Inc. */

/*
modification history
--------------------
01b,12may99,rhv  Modified to use new WRS PCI symbols.
01a,15dec98,mdp	 Written (from version 01e of mv2304/sl82565IntrCtl.h)
*/

#ifndef	__INCsl82565IntrCtlh
#define	__INCsl82565IntrCtlh

#ifdef __cplusplus
extern "C" {
#endif

#include "drv/pci/pciConfigLib.h"


/*
 *	IBC Extensions to Standard PCI Config Header register offsets
 */
#define PCI_CFG_IBC_INTR_ROUTE	0x44
#define PCI_CFG_IBC_ARB_CTL	0x83

/* PCI Arbiter Control Register bit definitions */

#define ARB_CTL_GAT		(1 << 7)
#define ARB_CTL_TIMEOUT_TIMER	(1 << 2)
#define ARB_CTL_BUS_LOCK	(1 << 0)


/* structure */

typedef struct intHandlerDesc		/* interrupt handler desciption */
    {
    VOIDFUNCPTR			vec;	/* interrupt vector */
    int				arg;	/* interrupt handler argument */
    struct intHandlerDesc *	next;	/* next interrupt handler & argument */
    } INT_HANDLER_DESC;


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


#ifdef __cplusplus
}
#endif

#endif	/* __INCsl82565IntrCtlh */
