/* i82378.h - i82378 IBC (ISA Bridge Controller) driver */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,30oct96,tam	 added IBC_READ_BYTE and IBC_WRITE_BYTE definitions (spr #7415).
01b,11oct96,tam	 added _ASMLANGUAGE conditional.
01a,29jan96,tam	 written.
*/

#ifndef	__INCi82378Ibch
#define	__INCi82378Ibch

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The macro IBC_BASE_ADRS, PIC_REG_ADDR_INTERVAL  & PIT_REG_ADDR_INTERVAL
 * must be defined when including this header.
 */

/* default definitions */

#define	CAST

#define PIC_ADRS(base,reg)   (CAST (base+(reg*PIC_REG_ADDR_INTERVAL)))

/* register definitions */

#define PIC_port1(base)	PIC_ADRS(base,0x00)	/* port 1. */
#define PIC_port2(base)	PIC_ADRS(base,0x01)	/* port 2. */

/* hardware register access macros */

#ifndef IBC_BYTE_READ				/* reads IBC register */
#    define IBC_BYTE_READ(addr,pData)	(*pData = *(UCHAR *)(addr))
#endif

#ifndef IBC_BYTE_WRITE				/* write to IBC register */
#    define IBC_BYTE_WRITE(addr,data) 	*(UCHAR *)(addr) = (data); EIEIO
#endif

/* alias */

#define PIC_IMASK(base)	PIC_port2(base)		/* Interrupt mask. */
#define PIC_IACK(base)	PIC_port1(base)		/* Interrupt acknowledge. */

#define PIC_ISR_MASK(base) PIC_port1(base)	/* in-service register mask */
#define PIC_IRR_MASK(base) PIC_port1(base) 	/* interrupt request reg */

#define OCW3_DEF	0x08			/* 3rd default control word */
#define OCW3_PCB	0x04			/* Polling Control Bit */
#define OCW3_ISR	0x03			/* Read in-service reg */
#define OCW3_IRR	0x02			/* Read inter request reg */


#define ISA_INTR_ACK_REG        (CPU_PCI_IACK_ADRS + 0x1ff0)

#define i82378_INT1_CTRL        (IBC_BASE_ADRS + 0x0020)        /* pic1 82378 */
#define i82378_INT1_MASK        (IBC_BASE_ADRS + 0x0021)
#define i82378_INT1_ELC         (IBC_BASE_ADRS + 0x04d0)
#define i82378_INT2_CTRL        (IBC_BASE_ADRS + 0x00a0)        /* pic2 82378 */
#define i82378_INT2_MASK        (IBC_BASE_ADRS + 0x00a1)
#define i82378_INT2_ELC         (IBC_BASE_ADRS + 0x04d1)
#define i82378_TMR1_CNT0        (IBC_BASE_ADRS + 0x0040)        /* timer */
#define i82378_TMR1_CNT1        (IBC_BASE_ADRS + 0x0041)
#define i82378_TMR1_CNT2        (IBC_BASE_ADRS + 0x0042)
#define i82378_TMR1_CMOD        (IBC_BASE_ADRS + 0x0043)
#define i82378_RST_IRQ12        (IBC_BASE_ADRS + 0x0060)
#define i82378_NMI_SCTRL        (IBC_BASE_ADRS + 0x0061)
#define i82378_PCOP             (IBC_BASE_ADRS + 0x0c04)
#define i82378_TMCP             (IBC_BASE_ADRS + 0x0c01)

/* programmable interrupt controller (PIC) */

#define PIC1_BASE_ADR           i82378_INT1_CTRL
#define PIC2_BASE_ADR           i82378_INT2_CTRL

/* programmable interrupt timers */

#define PIT_BASE_ADR            i82378_TMR1_CNT0        /* timeraddrs */
#define PIT_CLOCK               1193180


/* typedefs */

#ifndef _ASMLANGUAGE

typedef struct intHandlerDesc		/* interrupt handler desciption */
    {
    VOIDFUNCPTR			vec;	/* interrupt vector */
    int				arg;	/* interrupt handler argument */
    struct intHandlerDesc *	next;	/* next interrupt handler & argument */
    } INT_HANDLER_DESC;

#endif	/* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif	/* __INCi82378Ibch */
