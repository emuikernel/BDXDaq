/* ravenMpic.h - Raven MPIC register definitions */

/* Copyright 1984-1998 Wind River Systems, Inc. */
/* Copyright 1996, 1998 Motorola, Inc. */

/*
modification history
--------------------
01e,14apr98,ms_  merged Motorola mv2700 support
01d,11feb98,tb   Added TAS support which uses VMEbus RMW
01c,29apr97,dat  replaced pci.h with pciIomapLib.h
01b,23apr97,mas  merged with sl82565IntrCtl.h (SPR 8170).
01a,30oct96,rhk  created by Motorola.
*/

/*
Description:

Contains structure typedefs and device-specific defines for the Motorola MPIC
and Winbond PIB chips.
*/


#ifndef	INCravenMpich
#define	INCravenMpich

#ifdef __cplusplus
extern "C" {
#endif

/* includes */


/* structures */
typedef struct intHandlerDesc           /* interrupt handler desciption */
    {
    VOIDFUNCPTR                 vec;    /* interrupt vector */
    int                         arg;    /* interrupt handler argument */
    struct intHandlerDesc *     next;   /* next interrupt handler & argument */
    } INT_HANDLER_DESC;

typedef struct localIntData
    {
    UCHAR		mpicVec;
    UCHAR		ibcVec;
    INT8		cpuCtrlReg;
    int			temp;
    UINT32		regAddr;
    UINT32		regVal;
    } LOCAL_INT_DATA;

/* macros */

#define MPIC_ADDR(reg) (MPIC_BASE_ADRS + reg)

/*
 *	Raven MPIC bit masks and settings for the registers
 */

#define SET_MIXED_MODE		0x20000000
#define TIMER_INHIBIT		0x80000000
#define IPI_INHIBIT		0x80000000
#define INT_MASK_BIT		0x80000000
#define RESET_CNTRLR		0x80000000
#define HIGH_POLARITY		0x00800000
#define LEVEL_SENSE		0x00400000
#define VECTOR_MASK		0x000000ff
#define DESTINATION_CPU0	0x00000001

#define PRIORITY_MASK		0x000f0000
#define PRIORITY_LVL0		0x00000000
#define PRIORITY_LVL1		0x00010000
#define PRIORITY_LVL2           0x00020000
#define PRIORITY_LVL3           0x00030000
#define PRIORITY_LVL4           0x00040000
#define PRIORITY_LVL5           0x00050000
#define PRIORITY_LVL6           0x00060000
#define PRIORITY_LVL7           0x00070000
#define PRIORITY_LVL8           0x00080000
#define PRIORITY_LVL9           0x00090000
#define PRIORITY_LVL10          0x000a0000
#define PRIORITY_LVL11          0x000b0000
#define PRIORITY_LVL12          0x000c0000
#define PRIORITY_LVL13          0x000d0000
#define PRIORITY_LVL14          0x000e0000
#define PRIORITY_LVL15          0x000f0000

/* miscellaneous defines */

#define REG_OFFSET		0x10

/*
 *	Raven MPIC register definitions 
 */

/* feature reporting register */
#define MPIC_FEATURE_REG	0x01000

/* global configuration register */
#define MPIC_GLOBAL_CONFIG_REG	0x01020

/* vendor identification register */
#define MPIC_VENDOR_ID_REG	0x01080

/* processor init register */
#define MPIC_PROCESSOR_INIT_REG 0x01090

/* IPI vector/priority registers 0-3 */
#define MPIC_IPI0_VEC_PRI_REG	0x010a0
#define MPIC_IPI1_VEC_PRI_REG   0x010b0
#define MPIC_IPI2_VEC_PRI_REG   0x010c0
#define MPIC_IPI3_VEC_PRI_REG   0x010d0
 
/* spurious vector register */
#define MPIC_SPUR_VEC_REG		0x010e0

/* timer frequency register */
#define MPIC_TIMER_FREQ_REG	0x010f0

/* 
 * timer registers 0-3
 * - current count register
 * - base count register
 * - vector/priority register
 * - destination register
 */
#define MPIC_TIMER0_CUR_CNT_REG 0x01100
#define MPIC_TIMER0_BASE_CT_REG 0x01110
#define MPIC_TIMER0_VEC_PRI_REG 0x01120
#define MPIC_TIMER0_DEST_REG	0x01130

#define MPIC_TIMER1_CUR_CNT_REG 0x01140
#define MPIC_TIMER1_BASE_CT_REG 0x01150
#define MPIC_TIMER1_VEC_PRI_REG 0x01160
#define MPIC_TIMER1_DEST_REG    0x01170

#define MPIC_TIMER2_CUR_CNT_REG 0x01180
#define MPIC_TIMER2_BASE_CT_REG 0x01190
#define MPIC_TIMER2_VEC_PRI_REG 0x011a0
#define MPIC_TIMER2_DEST_REG    0x011b0

#define MPIC_TIMER3_CUR_CNT_REG 0x011c0
#define MPIC_TIMER3_BASE_CT_REG 0x011d0
#define MPIC_TIMER3_VEC_PRI_REG 0x011e0
#define MPIC_TIMER3_DEST_REG    0x011f0

/* 
 * external source registers 0-15
 * - vector/priority register
 * - destination register
 */
#define MPIC_EXT_SRC0_VEC_PRI_REG 0x10000
#define MPIC_EXT_SRC0_DEST_REG	  0x10010

#define MPIC_EXT_SRC1_VEC_PRI_REG 0x10020
#define MPIC_EXT_SRC1_DEST_REG    0x10030

#define MPIC_EXT_SRC2_VEC_PRI_REG 0x10040
#define MPIC_EXT_SRC2_DEST_REG    0x10050
 
#define MPIC_EXT_SRC3_VEC_PRI_REG 0x10060
#define MPIC_EXT_SRC3_DEST_REG    0x10070
 
#define MPIC_EXT_SRC4_VEC_PRI_REG 0x10080
#define MPIC_EXT_SRC4_DEST_REG    0x10090
 
#define MPIC_EXT_SRC5_VEC_PRI_REG 0x100a0
#define MPIC_EXT_SRC5_DEST_REG    0x100b0
 
#define MPIC_EXT_SRC6_VEC_PRI_REG 0x100c0
#define MPIC_EXT_SRC6_DEST_REG    0x100d0
 
#define MPIC_EXT_SRC7_VEC_PRI_REG 0x100e0
#define MPIC_EXT_SRC7_DEST_REG    0x100f0
 
#define MPIC_EXT_SRC8_VEC_PRI_REG 0x10100
#define MPIC_EXT_SRC8_DEST_REG    0x10110
 
#define MPIC_EXT_SRC9_VEC_PRI_REG 0x10120
#define MPIC_EXT_SRC9_DEST_REG    0x10130
 
#define MPIC_EXT_SRC10_VEC_PRI_REG 0x10140
#define MPIC_EXT_SRC10_DEST_REG    0x10150
 
#define MPIC_EXT_SRC11_VEC_PRI_REG 0x10160
#define MPIC_EXT_SRC11_DEST_REG    0x10170
 
#define MPIC_EXT_SRC12_VEC_PRI_REG 0x10180
#define MPIC_EXT_SRC12_DEST_REG    0x10190
 
#define MPIC_EXT_SRC13_VEC_PRI_REG 0x101a0
#define MPIC_EXT_SRC13_DEST_REG    0x101b0
 
#define MPIC_EXT_SRC14_VEC_PRI_REG 0x101c0
#define MPIC_EXT_SRC14_DEST_REG    0x101d0
 
#define MPIC_EXT_SRC15_VEC_PRI_REG 0x101e0
#define MPIC_EXT_SRC15_DEST_REG    0x101f0
 
/* raven detected error registers */
#define MPIC_ERR_VEC_PRI_REG	0x10200
#define MPIC_ERR_DEST_REG	0x10210

/* interprocessor interrupt dispatch registers for cpu 0 */
#define MPIC_CPU0_IPI0_DISP_REG 0x20040
#define MPIC_CPU0_IPI1_DISP_REG 0x20050
#define MPIC_CPU0_IPI2_DISP_REG 0x20060
#define MPIC_CPU0_IPI3_DISP_REG 0x20070
/* interrupt task priority register for cpu 0 */
#define MPIC_CPU0_CUR_TASK_PRI_REG 0x20080

/* cpu 0 interrupt acknowledge register */
#define MPIC_CPU0_IACK_REG	0x200a0
/* cpu 0 end-of-interrupt register */
#define MPIC_CPU0_EOI_REG	0x200b0 

/* interprocessor interrupt dispatch registers for cpu 1 */
#define MPIC_CPU1_IPI0_DISP_REG 0x21040
#define MPIC_CPU1_IPI1_DISP_REG 0x21050
#define MPIC_CPU1_IPI2_DISP_REG 0x21060
#define MPIC_CPU1_IPI3_DISP_REG 0x21070
/* interrupt task priority register for cpu 1 */
#define MPIC_CPU1_CUR_TASK_PRI_REG 0x21080
 
/* cpu 1 interrupt acknowledge register */
#define MPIC_CPU1_IACK_REG      0x210a0
/* cpu 1 end-of-interrupt register */
#define MPIC_CPU1_EOI_REG       0x210b0



/*
 *	IBC Extensions to Standard PCI Config Header register offsets
 */
#define PCI_CFG_IBC_INTR_ROUTE	0x44
#define PCI_CFG_IBC_ARB_CTL	0x83

/* PCI Arbiter Control Register bit definitions */

#define ARB_CTL_GAT		(1 << 7)
#define ARB_CTL_TIMEOUT_TIMER	(1 << 2)
#define ARB_CTL_BUS_LOCK	(1 << 0)

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

#define PIC_OCW3_SEL         0x08      /* Select OCW3 */
#define PIC_IRR_READ         0x02      /* Read IRR (Int. req. reg.) */
#define PIC_ISR_READ         0x03      /* Read ISR (In service reg.) */


#ifdef __cplusplus
}
#endif

#endif	/* INCravenMpich */

