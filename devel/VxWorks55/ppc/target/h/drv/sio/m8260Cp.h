/* m8260Cp.h - Motorola MPC8260 Communications Processor header file */

/* Copyright 1984-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,12sep99,ms_	 created from m8260Cpm.h, 01d.
*/

/*
 * This file contains constants for the Communication Processor in
 * the Motorola MPC8260 PowerQUICC II integrated Communications Processor
 */

#ifndef __INCm8260Cph
#define __INCm8260Cph

#ifdef __cplusplus
extern "C" {
#endif

#ifndef M8260ABBREVIATIONS
#define M8260ABBREVIATIONS
    
#ifdef  _ASMLANGUAGE
#define CAST(x)
#else /* _ASMLANGUAGE */
typedef volatile UCHAR VCHAR;   /* shorthand for volatile UCHAR */
typedef volatile INT32 VINT32; /* volatile unsigned word */
typedef volatile INT16 VINT16; /* volatile unsigned halfword */
typedef volatile INT8 VINT8;   /* volatile unsigned byte */
typedef volatile UINT32 VUINT32; /* volatile unsigned word */
typedef volatile UINT16 VUINT16; /* volatile unsigned halfword */
typedef volatile UINT8 VUINT8;   /* volatile unsigned byte */
#define CAST(x) (x)
#endif  /* _ASMLANGUAGE */

#endif /* M8260ABBREVIATIONS */

/*
 * MPC8260 internal register/memory map (section 17 of prelim. spec)
 * note that these are offsets from the value stored in the IMMR
 * register. Also note that in the MPC8260, the IMMR is not a special
 * purpose register, but it is memory mapped.
 */
 
/* Command Register definitions (CPCR - 0x119C0) */    
 
#define M8260_CPCR(base)   (CAST(VUINT32 *)((base) + 0x119C0)) /* CPCR */
#define M8260_CPCR_LATENCY (65536 * 400)	/* worst case exec latency */

#define	M8260_CPCR_RESET	0x80000000	/* software reset command */
#define M8260_CPCR_PAGE_MSK	0x7c000000	/* RAM page number */
#define M8260_CPCR_SBC_MSK     	0x03e00000	/* sub-block code */
#define M8260_CPCR_RES1     	0x001e0000	/* reserved */
#define M8260_CPCR_FLG		0x00010000	/* flag - command executing */ 
#define M8260_CPCR_RES2     	0x0000c000	/* reserved */
#define M8260_CPCR_MCN_MSK     	0x00003fc0	/* MCC channel number */
#define M8260_CPCR_RES3     	0x00000030	/* reserved */
#define M8260_CPCR_OP_MSK     	0x0000000f	/* command opcode */
#define M8260_CPCR_PAGE_SHIFT   0x1a		/* get to the page field */
#define M8260_CPCR_SBC_SHIFT   	0x15		/* get to the SBC field */
#define M8260_CPCR_MCN_SHIFT   	0x6		/* get to the MCC field */
#define M8260_CPCR_OP_SHIFT 	0x0		/* get to the opcode field */
#define M8260_CPCR_MCN_HDLC   	0x0		/* protocol code: HDLC */
#define M8260_CPCR_MCN_ATM   	0xa		/* protocol code: ATM */
#define M8260_CPCR_MCN_ETH   	0xc		/* protocol code: ETH */
#define M8260_CPCR_MCN_TRANS   	0xf		/* protocol code: TRANS */

#define	M8260_CPCR_OP(x)						\
    (((x) << M8260_CPCR_OP_SHIFT) & M8260_CPCR_OP_MSK)
    
#define	M8260_CPCR_SBC(x)						\
    (((x) << M8260_CPCR_SBC_SHIFT) & M8260_CPCR_SBC_MSK)

#define	M8260_CPCR_PAGE(x)						\
    (((x) << M8260_CPCR_PAGE_SHIFT) & M8260_CPCR_PAGE_MSK)

#define	M8260_CPCR_MCN(x)						\
    (((x) << M8260_CPCR_MCN_SHIFT) & M8260_CPCR_MCN_MSK)

/* CPCR - Sub-block code */

#define M8260_CPCR_SBC_FCC1     0x10			/* FCC1 channel */ 
#define M8260_CPCR_SBC_ATM_FCC1 0x0e			/* ATM on FCC1 */ 
#define M8260_CPCR_SBC_FCC2     0x11			/* FCC2 channel */ 
#define M8260_CPCR_SBC_ATM_FCC2 0x0e			/* ATM on FCC2 */ 
#define M8260_CPCR_SBC_FCC3     0x12			/* FCC3 channel */ 
#define M8260_CPCR_SBC_SCC1     0x04			/* SCC1 channel */ 
#define M8260_CPCR_SBC_SCC2     0x05			/* SCC2 channel */ 
#define M8260_CPCR_SBC_SCC3     0x06			/* SCC3 channel */ 
#define M8260_CPCR_SBC_SCC4     0x07			/* SCC4 channel */ 
#define M8260_CPCR_SBC_SMC1     0x08			/* SMC1 channel */ 
#define M8260_CPCR_SBC_SMC2     0x09			/* SMC2 channel */ 
#define M8260_CPCR_SBC_RAND     0x0e			/* RAND channel */ 
#define M8260_CPCR_SBC_SPI     	0x0a			/* SPI */ 
#define M8260_CPCR_SBC_I2C     	0x0b			/* I2C */ 
#define M8260_CPCR_SBC_TIMER    0x0f			/* TIMER */ 
#define M8260_CPCR_SBC_MCC1     0x1c			/* MCC1 channel */ 
#define M8260_CPCR_SBC_MCC2     0x1d			/* MCC2 channel */ 
#define M8260_CPCR_SBC_IDMA1    0x14			/* IDMA1 channel */ 
#define M8260_CPCR_SBC_IDMA2    0x15			/* IDMA2 channel */ 
#define M8260_CPCR_SBC_IDMA3    0x16			/* IDMA3 channel */ 
#define M8260_CPCR_SBC_IDMA4    0x17			/* IDMA4 channel */ 

/* CPCR - Page code */

#define M8260_CPCR_PAGE_FCC1    0x04			/* FCC1 channel */ 
#define M8260_CPCR_PAGE_FCC2    0x05			/* FCC2 channel */ 
#define M8260_CPCR_PAGE_FCC3    0x06			/* FCC3 channel */ 
#define M8260_CPCR_PAGE_SCC1    0x00			/* SCC1 channel */ 
#define M8260_CPCR_PAGE_SCC2    0x01			/* SCC2 channel */ 
#define M8260_CPCR_PAGE_SCC3    0x02			/* SCC3 channel */ 
#define M8260_CPCR_PAGE_SCC4    0x03			/* SCC4 channel */ 
#define M8260_CPCR_PAGE_SMC1    0x07			/* SMC1 channel */ 
#define M8260_CPCR_PAGE_SMC2    0x08			/* SMC2 channel */ 
#define M8260_CPCR_PAGE_RAND    0x0a			/* RAND channel */ 
#define M8260_CPCR_PAGE_SPI     0x09			/* SPI */ 
#define M8260_CPCR_PAGE_I2C     0x0a			/* I2C */ 
#define M8260_CPCR_PAGE_TIMER   0x0a			/* TIMER */ 
#define M8260_CPCR_PAGE_MCC1    0x07			/* MCC1 channel */ 
#define M8260_CPCR_PAGE_MCC2    0x08			/* MCC2 channel */ 
#define M8260_CPCR_PAGE_IDMA1   0x07			/* IDMA1 channel */ 
#define M8260_CPCR_PAGE_IDMA2   0x08			/* IDMA2 channel */ 
#define M8260_CPCR_PAGE_IDMA3   0x09			/* IDMA3 channel */ 
#define M8260_CPCR_PAGE_IDMA4   0x0a			/* IDMA4 channel */ 

/* CPCR - opcodes */
 
#define M8260_CPCR_RT_INIT	0x0		/* Init rx and tx */
#define M8260_CPCR_RX_INIT   	0x1		/* init rx only */
#define M8260_CPCR_TX_INIT   	0x2		/* init tx only */
#define M8260_CPCR_HUNT      	0x3		/* rx frame hunt mode */
#define M8260_CPCR_TX_STOP      0x4		/* stop tx */
#define M8260_CPCR_TX_GRSTOP   	0x5		/* gracefully stop tx */
#define M8260_CPCR_TX_RESTART   0x6		/* restart tx */
#define M8260_CPCR_RX_CLOSE     0x7		/* close rx buffer */
#define M8260_CPCR_SET_GROUP 	0x8		/* set group address */
#define	M8260_CPCR_SET_TMR	0x8		/* set timer */
#define	M8260_CPCR_GCI_TMO	0x9		/* gci timeout */
#define	M8260_CPCR_IDMA_START	0x9		/* start idma */
#define	M8260_CPCR_MCC_RX_STOP	0x9		/* stop rx on MCC */
#define	M8260_CPCR_ATM_TX	0xa		/* ATM transmit */
#define M8260_CPCR_BCS_RESET 	0xa		/* blk chk seq reset */
#define	M8260_CPCR_GCI_ABRT	0xa		/* gci abort request */
#define	M8260_CPCR_IDMA_STOP	0xb		/* stop idma */
#define	M8260_CPCR_RANDOM	0xc		/* random number */

#ifdef __cplusplus
}
#endif

#endif /* __INCm8260Cph */
