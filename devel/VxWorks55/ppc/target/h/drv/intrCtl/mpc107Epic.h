/* mpc107Epic.h - MPC107 EPIC register definitions  */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,11sep00,rcs  correct include path errors
01a,06jun00,bri written
*/

#ifndef	__INCmpc107Epich
#define	__INCmpc107Epich

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "drv/multi/mpc107.h"

/* defines */

/* MPC107 configuration registers for EPIC  && other definitions */

#define  INTERRUPT_TABLESIZE   256 /* Interrupt Table Size */

#define EPIC_EUMBBAR  		0x40000		  /* EUMBBAR of EPIC  */
#define MPC107_EPIC_FEATURES_REG  (EPIC_EUMBBAR + 0x01000)/* Feature report */
#define MPC107_EPIC_GLOBAL_REG	  (EPIC_EUMBBAR + 0x01020)/* Global config.  */
#define MPC107_EPIC_INT_CONF_REG  (EPIC_EUMBBAR + 0x01030)/* Interrupt config */
#define MPC107_EPIC_VENDOR_ID_REG (EPIC_EUMBBAR + 0x01080)/* Vendor id */
#define MPC107_EPIC_PROC_INIT_REG (EPIC_EUMBBAR + 0x01090)/* Processor init. */
#define MPC107_EPIC_SPUR_VEC_REG  (EPIC_EUMBBAR + 0x010e0)/* Spurious vector */
#define MPC107_EPIC_TM_FREQ_REG	  (EPIC_EUMBBAR + 0x010f0)/* Timer Frequency */


/*
 * Definitions of Global Timer Registers :
 * Global Timer Current Count Register   (GTCCR)
 * Global Timer Base Count Register      (GTBCR)
 * Global Timer Vector/Priority Register (GTVPR)
 * Global Timer  Destination Register    (GTDR)
 */

#define MPC107_EPIC_TM0_CUR_COUNT_REG	(EPIC_EUMBBAR + 0x01100) /* GTCCR0 */
#define MPC107_EPIC_TM0_BASE_COUNT_REG	(EPIC_EUMBBAR + 0x01110) /* GTBCRO */
#define MPC107_EPIC_TM0_VEC_REG	        (EPIC_EUMBBAR + 0x01120) /* GTVPR0 */
#define MPC107_EPIC_TM0_DES_REG	        (EPIC_EUMBBAR + 0x01130) /* GTDR0 */

#define MPC107_EPIC_TM1_CUR_COUNT_REG	(EPIC_EUMBBAR + 0x01140) /* GTCCR1 */
#define MPC107_EPIC_TM1_BASE_COUNT_REG	(EPIC_EUMBBAR + 0x01150) /* GTBCR1*/
#define MPC107_EPIC_TM1_VEC_REG	        (EPIC_EUMBBAR + 0x01160) /* GTVPR1*/
#define MPC107_EPIC_TM1_DES_REG	        (EPIC_EUMBBAR + 0x01170) /* GTDR1*/

#define MPC107_EPIC_TM2_CUR_COUNT_REG	(EPIC_EUMBBAR + 0x01180) /* GTCCR2 */
#define MPC107_EPIC_TM2_BASE_COUNT_REG	(EPIC_EUMBBAR + 0x01190) /* GTBCR2 */
#define MPC107_EPIC_TM2_VEC_REG	        (EPIC_EUMBBAR + 0x011a0) /* GTVPR2 */
#define MPC107_EPIC_TM2_DES_REG	        (EPIC_EUMBBAR + 0x011b0) /* GTDR2 */

#define MPC107_EPIC_TM3_CUR_COUNT_REG	(EPIC_EUMBBAR + 0x011c0) /* GTCCR3 */
#define MPC107_EPIC_TM3_BASE_COUNT_REG	(EPIC_EUMBBAR + 0x011d0) /* GTBCR3 */
#define MPC107_EPIC_TM3_VEC_REG	        (EPIC_EUMBBAR + 0x011e0) /* GTVPR3 */
#define MPC107_EPIC_TM3_DES_REG	        (EPIC_EUMBBAR + 0x011f0) /* GTDR3  */


/*
 * External Interrupts  Vector /Priority Registers (IVPR) and
 * Destination Registers (IDR)
 */

#define MPC107_EPIC_EX_INT0_VEC_REG	(EPIC_EUMBBAR + 0x10200) /* IVPR0 */
#define MPC107_EPIC_EX_INT0_DES_REG	(EPIC_EUMBBAR + 0x10210) /* IDR0  */
#define MPC107_EPIC_EX_INT1_VEC_REG	(EPIC_EUMBBAR + 0x10220) /* IVPR1 */
#define MPC107_EPIC_EX_INT1_DES_REG	(EPIC_EUMBBAR + 0x10230) /* IDR1  */
#define MPC107_EPIC_EX_INT2_VEC_REG	(EPIC_EUMBBAR + 0x10240) /* IVPR2 */
#define MPC107_EPIC_EX_INT2_DES_REG	(EPIC_EUMBBAR + 0x10250) /* IDR2  */
#define MPC107_EPIC_EX_INT3_VEC_REG	(EPIC_EUMBBAR + 0x10260) /* IVPR3 */
#define MPC107_EPIC_EX_INT3_DES_REG	(EPIC_EUMBBAR + 0x10270) /* IDR3  */
#define MPC107_EPIC_EX_INT4_VEC_REG	(EPIC_EUMBBAR + 0x10280) /* IVPR4 */
#define MPC107_EPIC_EX_INT4_DES_REG	(EPIC_EUMBBAR + 0x10290) /* IDR4  */

/*
 * Serial Interrupts  Vector /Priority Registers (SVPR) and
 * Destination Registers (SDR)
 */

#define MPC107_EPIC_SR_INT0_VEC_REG	(EPIC_EUMBBAR + 0x10200) /*  SVPR0 */
#define MPC107_EPIC_SR_INT0_DES_REG	(EPIC_EUMBBAR + 0x10210) /*  SDR0  */
#define MPC107_EPIC_SR_INT1_VEC_REG	(EPIC_EUMBBAR + 0x10220) /*  SVPR1 */
#define MPC107_EPIC_SR_INT1_DES_REG	(EPIC_EUMBBAR + 0x10230) /*  SDR1  */
#define MPC107_EPIC_SR_INT2_VEC_REG	(EPIC_EUMBBAR + 0x10240) /*  SVPR2 */
#define MPC107_EPIC_SR_INT2_DES_REG	(EPIC_EUMBBAR + 0x10250) /*  SDR2  */
#define MPC107_EPIC_SR_INT3_VEC_REG	(EPIC_EUMBBAR + 0x10260) /*  SVPR3 */
#define MPC107_EPIC_SR_INT3_DES_REG	(EPIC_EUMBBAR + 0x10270) /*  SDR3  */
#define MPC107_EPIC_SR_INT4_VEC_REG	(EPIC_EUMBBAR + 0x10280) /*  SVPR4 */
#define MPC107_EPIC_SR_INT4_DES_REG	(EPIC_EUMBBAR + 0x10290) /*  SDR4  */

#define MPC107_EPIC_SR_INT5_VEC_REG	(EPIC_EUMBBAR + 0x102a0) /* SVPR5 */
#define MPC107_EPIC_SR_INT5_DES_REG	(EPIC_EUMBBAR + 0x102b0) /* SDR5  */
#define MPC107_EPIC_SR_INT6_VEC_REG	(EPIC_EUMBBAR + 0x102c0) /* SVPR6 */
#define MPC107_EPIC_SR_INT6_DES_REG	(EPIC_EUMBBAR + 0x102d0) /* SDR6  */
#define MPC107_EPIC_SR_INT7_VEC_REG	(EPIC_EUMBBAR + 0x102e0) /* SVPR7 */
#define MPC107_EPIC_SR_INT7_DES_REG	(EPIC_EUMBBAR + 0x102f0) /* SDR7  */
#define MPC107_EPIC_SR_INT8_VEC_REG	(EPIC_EUMBBAR + 0x10300) /* SVPR8 */
#define MPC107_EPIC_SR_INT8_DES_REG	(EPIC_EUMBBAR + 0x10310) /* SDR8  */
#define MPC107_EPIC_SR_INT9_VEC_REG	(EPIC_EUMBBAR + 0x10320) /* SVPR9 */
#define MPC107_EPIC_SR_INT9_DES_REG	(EPIC_EUMBBAR + 0x10330) /* SDR9  */

#define MPC107_EPIC_SR_INT10_VEC_REG	(EPIC_EUMBBAR + 0x10340) /* SVPR10 */
#define MPC107_EPIC_SR_INT10_DES_REG	(EPIC_EUMBBAR + 0x10350) /* SDR10  */
#define MPC107_EPIC_SR_INT11_VEC_REG	(EPIC_EUMBBAR + 0x10360) /* SVPR11 */
#define MPC107_EPIC_SR_INT11_DES_REG	(EPIC_EUMBBAR + 0x10370) /* SDR11  */
#define MPC107_EPIC_SR_INT12_VEC_REG	(EPIC_EUMBBAR + 0x10380) /* SVPR12 */
#define MPC107_EPIC_SR_INT12_DES_REG	(EPIC_EUMBBAR + 0x10390) /* SDR12  */
#define MPC107_EPIC_SR_INT13_VEC_REG	(EPIC_EUMBBAR + 0x103a0) /* SVPR13 */
#define MPC107_EPIC_SR_INT13_DES_REG	(EPIC_EUMBBAR + 0x103b0) /* SDR13  */
#define MPC107_EPIC_SR_INT14_VEC_REG	(EPIC_EUMBBAR + 0x103c0) /* SVPR14 */
#define MPC107_EPIC_SR_INT14_DES_REG	(EPIC_EUMBBAR + 0x103d0) /* SDR14  */
#define MPC107_EPIC_SR_INT15_VEC_REG	(EPIC_EUMBBAR + 0x103e0) /* SVPR15 */
#define MPC107_EPIC_SR_INT15_DES_REG	(EPIC_EUMBBAR + 0x103f0) /* SDR15 */

/*
 * IIC Interrupts  Vector /Priority Registers (IIVPR) and
 * Destination Registers (IIDR)
 */

#define MPC107_EPIC_I2C_INT_VEC_REG	(EPIC_EUMBBAR + 0x11020)/* IIVPR0 */
#define MPC107_EPIC_I2C_INT_DES_REG	(EPIC_EUMBBAR + 0x11030)/* IIDR0 */

/*
 * DMA Interrupts  Vector /Priority Registers (IIVPR) and
 * Destination Registers (IIDR) - Channel Zero
 */

#define MPC107_EPIC_DMA0_INT_VEC_REG	(EPIC_EUMBBAR + 0x11040) /* IIVPR1 */
#define MPC107_EPIC_DMA0_INT_DES_REG	(EPIC_EUMBBAR + 0x11050) /* IIDR1  */

/*
 * DMA Interrupts  Vector /Priority Registers (IIVPR) and
 * Destination Registers (IIDR) -  Channel One
 */

#define MPC107_EPIC_DMA1_INT_VEC_REG	(EPIC_EUMBBAR + 0x11060) /* IIVPR2 */
#define MPC107_EPIC_DMA1_INT_DES_REG	(EPIC_EUMBBAR + 0x11070) /* IIDR2 */


/*
 * Message Unit  Interrupts  Vector /Priority Registers (IIVPR) and
 * Destination Registers (IIDR)
 */

#define MPC107_EPIC_MSG_INT_VEC_REG	(EPIC_EUMBBAR + 0x110c0)/* IIVPR3 */
#define MPC107_EPIC_MSG_INT_DES_REG	(EPIC_EUMBBAR + 0x110d0)/* IIDR3  */


/* General  Register Definitions */

#define MPC107_EPIC_PROC_CTASK_PRI_REG \
              (EPIC_EUMBBAR + 0x20080)  /* Proccesor current task Register */
#define MPC107_EPIC_PROC_INT_ACK_REG \
              (EPIC_EUMBBAR + 0x200a0) /* Interrupt  acknowledge  Register */
#define MPC107_EPIC_PROC_EOI_REG \
              (EPIC_EUMBBAR + 0x200b0) /* End of interrupt Register */

/* General   Definitions */

#define MPC107_EPIC_VEC_REG_INTERVAL	0x20 /* Distance between interrupt */
                                             /* vector registers */

#define MPC107_EPIC_VEC_REG(irq)       (MPC107_EPIC_EX_INT0_VEC_REG + \
                                 (irq * MPC107_EPIC_VEC_REG_INTERVAL))


#define MPC107_EPIC_DIRECT_IRQ		0 /* Direct IRQ */
#define MPC107_INT_VEC_IRQ0             5 /* Maximum Ext Interrupts */
#define MPC107_EPIC_INTER_IN_SERVICE	2 /* Interrupt in service */
#define MPC107_EPIC_INTERNAL_INTERRUPT 20 /* Internal Interrupts */
#define MPC107_EPIC_EXTERNAL_INTERRUPT 21 /* External Interrupts */
#define MPC107_EPIC_INV_INTER_SOURCE   22 /* Invalid Interrupt Source */
#define MPC107_EPIC_MAX_EXT_IRQS        5 /* Maximum No. of ext.Interrupts */
#define MPC107_EPIC_PRIORITY_MIN	0 /* minimum level of priority */
#define MPC107_EPIC_PRIORITY_MAX       15 /* maximum level of priority */
#define MPC107_EPIC_PRIORITY_GENERAL   12 /* General priority */
#define MPC107_EPIC_INV_PRIO_ERROR ((ULONG)(-1)) /* Invalid Priority */

#define MPC107_EPIC_IVPRO_INT_NUM       0  /* External Interrupt 0 */
#define MPC107_EPIC_IVPR1_INT_NUM	1  /* External Interrupt 1 */
#define MPC107_EPIC_IVPR2_INT_NUM	2  /* External Interrupt 2 */
#define MPC107_EPIC_IVPR3_INT_NUM	3  /* External Interrupt 3 */
#define MPC107_EPIC_IVPR4_INT_NUM	4  /* External Interrupt 4 */

#define MPC107_EPIC_SVPRO_INT_NUM	0  /* Serial Interrupt 0   */
#define MPC107_EPIC_SVPR1_INT_NUM	1  /* Serial Interrupt 1   */
#define MPC107_EPIC_SVPR2_INT_NUM	2  /* Serial Interrupt 2   */
#define MPC107_EPIC_SVPR3_INT_NUM	3  /* Serial Interrupt 3   */
#define MPC107_EPIC_SVPR4_INT_NUM	4  /* Serial Interrupt 4   */
#define MPC107_EPIC_SVPR5_INT_NUM	5  /* Serial Interrupt 5   */
#define MPC107_EPIC_SVPR6_INT_NUM	6  /* Serial Interrupt 6   */
#define MPC107_EPIC_SVPR7_INT_NUM	7  /* Serial Interrupt 7   */
#define MPC107_EPIC_SVPR8_INT_NUM	8  /* Serial Interrupt 8   */
#define MPC107_EPIC_SVPR9_INT_NUM	9  /* Serial Interrupt 9   */
#define MPC107_EPIC_SVPR1O_INT_NUM	10 /* Serial Interrupt 10  */
#define MPC107_EPIC_SVPR11_INT_NUM	11 /* Serial Interrupt 11  */
#define MPC107_EPIC_SVPR12_INT_NUM	12 /* Serial Interrupt 12  */
#define MPC107_EPIC_SVPR13_INT_NUM	13 /* Serial Interrupt 13  */
#define MPC107_EPIC_SVPR14_INT_NUM	14 /* Serial Interrupt 14  */
#define MPC107_EPIC_SVPR15_INT_NUM	15 /* Serial Interrupt 15  */


#define MPC107_EPIC_IIC_INT_VECT         0x71  /* IIC  Interrupt  */
#define MPC107_EPIC_DMA0_INT_VECT	 0x72  /* DMA Channel 0 Interrupt */
#define MPC107_EPIC_DMA1_INT_VECT	 0x73  /* DMA Channel 1 Interrupt */
#define MPC107_EPIC_MU_INT_VECT	         0x76  /* Message Unit Interrupt  */

/* Bit definitions */

/* Global Configuration Register Bit Definitions  */

#define MPC107_EPIC_GCR_RESET		0x80000000 /* EPIC reset bit */
#define MPC107_EPIC_GCR_MODE_MIXED	0x20000000 /* EPIC Mixed Mode bit */

/* Interrupt Configuration Register Bit Definitions  */

#define MPC107_EPIC_ICR_SEI		0x08000000 /* Serial Interrupt Enable */
#define MPC107_EPIC_ICR_CLK_RATIO_MSK	\
        (0x40000000 | 0x20000000 | 0x10000000) /* Clock  Ratio Mask  Bits */
#define MPC107_EPIC_ICR_CLK_RATIO(val) \
        (((val) << 28) & MPC107_EPIC_ICR_CLK_RATIO_MSK) /* Clock Ratio Shift */


/* Interrupt Vector/Priority  Register Bit Definitions */

#define MPC107_EPIC_IVPR_INTR_MSK	   0x80000000   /* Interrupt Mask bit */
#define MPC107_EPIC_IVPR_INTR_ACTIVE	0x40000000   /* Interrupt Activity Bit */
#define MPC107_EPIC_IVPR_INTR_POLARITY 0x00800000   /* Interrupt Polarity Bit */
#define MPC107_EPIC_IVPR_INTR_SENSE	   0x00400000   /* Interrupt Sense Bit */
#define MPC107_EPIC_IVPR_POLARITY(p)	((p) << 23) /* Int Polarity Shift */
#define MPC107_EPIC_IVPR_SENS(s)	      ((s) << 22) /* Int Sense Shift */
#define MPC107_EPIC_IVPR_PRIORITY_MSK	\
        (0x00080000 | 0x00040000 | 0x00020000 | 0x00010000) /* Priority Mask */
#define MPC107_EPIC_IVPR_PRIORITY(p) \
        (((p) << 16) & MPC107_EPIC_IVPR_PRIORITY_MSK)/* Priority Shift */
#define MPC107_EPIC_IVPR_VECTOR_MSK	   (0xff)       /* Vector Mask  */
#define MPC107_EPIC_IVPR_VECTOR(vec) \
               ((vec) & MPC107_EPIC_IVPR_VECTOR_MSK) /* Vector Mask */
#define MPC107_EPIC_INT_ACT_LOW	   0                 /* Active low Interrupt */
#define MPC107_EPIC_INT_ACT_HIGH   1                 /* Active high Interrupt */
#define MPC107_EPIC_SENSE_LVL	   1                 /* Level Sensitive */
#define MPC107_EPIC_SENSE_EDG	   0                 /* Edge Sensitive  */
#define MPC107_EPIC_INT_POLARITY   0x0               /* Interrupt polarity */

/* Global Timer Vector/Priority register Bit Definitions */

#define MPC107_EPIC_TM_VECREG_INTMASK   0x80000000 /* Interrupt Mask Bit */


/* Global Timer Base Count Register  Bit Definitions */

#define MPC107_EPIC_TM_BASE_COUNT_CI   0x80000000 /* Count Inhibit Bit */


#define MPC107_EPIC_TM_CUR_COUNT_TB   0x80000000 /* toggle bit */

/* typedefs */

/*  interrupt handler description  */

typedef struct intHandlerDesc
    {
    VOIDFUNCPTR			vec;	/* interrupt vector */
    INT32			arg;	/* interrupt handler argument */
    struct  intHandlerDesc *	next;	/* pointer to the next handler */
    } INT_HANDLER_DESC;


/* function declarations */

IMPORT VOID   	mpc107EpicInit (ULONG IRQType, ULONG clkRatio);
IMPORT VOID	mpc107EpicIntrInit (VOID);
IMPORT STATUS   mpc107EpicIntConnect (VOIDFUNCPTR *vector,
                                      VOIDFUNCPTR routine,
                                      INT32 parameter );
IMPORT STATUS	mpc107EpicIntDisconnect (VOIDFUNCPTR * vector,
                                        VOIDFUNCPTR routine);
IMPORT VOID   	mpc107EpicIntHandler (VOID);

#ifdef __cplusplus
}
#endif

#endif	/* __INCmpc107Epich */

