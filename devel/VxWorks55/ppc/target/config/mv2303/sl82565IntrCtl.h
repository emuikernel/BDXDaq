/* sl82565IntrCtl.h - sl82565 IBC (ISA Bridge Controller) driver */

/* Copyright 1984-1998 Wind River Systems, Inc. */
/* Copyright 1996, 1998 Motorola, Inc. */

/*
modification history
--------------------
01e,14apr98,ms_  merged Motorola mv2700 support
01d,11feb98,tb   Added TAS support which uses VMEbus RMW
01c,29apr97,dat  replaced pci.h with pciIomapLib.h
01b,02jan97,dat  mod history fix
01a,01sep96,mot	 written. (i82378Ibc.h, ver 01a)
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
#define SL82565_INT1_CTRL        (CPU_PCI_ISA_IO_ADRS + 0x0020)	/* PIC1 */
#define SL82565_INT1_MASK        (CPU_PCI_ISA_IO_ADRS + 0x0021)
#define SL82565_INT1_ELC         (CPU_PCI_ISA_IO_ADRS + 0x04d0)
#define SL82565_INT2_CTRL        (CPU_PCI_ISA_IO_ADRS + 0x00a0)	/* PIC2 */
#define SL82565_INT2_MASK        (CPU_PCI_ISA_IO_ADRS + 0x00a1)
#define SL82565_INT2_ELC         (CPU_PCI_ISA_IO_ADRS + 0x04d1)
#define SL82565_TMR1_CNT0        (CPU_PCI_ISA_IO_ADRS + 0x0040)
#define SL82565_TMR1_CNT1        (CPU_PCI_ISA_IO_ADRS + 0x0041)
#define SL82565_TMR1_CNT2        (CPU_PCI_ISA_IO_ADRS + 0x0042)
#define SL82565_TMR1_CMOD        (CPU_PCI_ISA_IO_ADRS + 0x0043)
#define SL82565_RST_IRQ12        (CPU_PCI_ISA_IO_ADRS + 0x0060)
#define SL82565_NMI_SCTRL        (CPU_PCI_ISA_IO_ADRS + 0x0061)
#define SL82565_PCOP             (CPU_PCI_ISA_IO_ADRS + 0x0c04)
#define SL82565_TMCP             (CPU_PCI_ISA_IO_ADRS + 0x0c01)

/* programmable interrupt controller (PIC) */

#define PIC1_BASE_ADR           SL82565_INT1_CTRL
#define PIC2_BASE_ADR           SL82565_INT2_CTRL


#ifdef __cplusplus
}
#endif

#endif	/* __INCsl82565IntrCtlh */
