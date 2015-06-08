/* mpc107Msg.h - MPC107 Message Unit  register numbers and values  */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,06jun00,bri written
*/

#ifndef	__INCmpc107Msgh
#define	__INCmpc107Msgh

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "mpc107.h"

/* defines */

/* MPC107 configuration registers for Message Unit  */

#define MPC107_MSG_IMR0     0x50  /* Inbound message register 0 */
#define MPC107_MSG_IMR1     0x54  /* Inbound message register 1 */
#define MPC107_MSG_OMR0     0x58  /* Outbound message register 0 */
#define MPC107_MSG_OMR1     0x5C  /* Outbound message register 1 */
#define MPC107_MSG_ODBR     0x60  /* Outbound doorbell register */
#define MPC107_MSG_IDBR     0x68  /* Inbound doorbell register */
#define MPC107_MSG_EDBMR    0x70  /* Extended doorbell mask register */
#define MPC107_MSG_EDBSR    0x78  /* Extended doorbell status register */
#define MPC107_MSG_EDBW1C   0x80  /* Extended doorbell write 1 clear register */
#define MPC107_MSG_EDBW1S   0x88  /* Extended doorbell write 1 set register */

/* MPC107 Configuration registers for I2O */

#define MPC107_MSG_I2O_IMISR    0x100 /* Inbound message interrupt status reg */
#define MPC107_MSG_I2O_IMIMR    0x104 /* Inbound message interrupt mask reg */
#define MPC107_MSG_I2O_IFHPR    0x120 /* Inbound free_FIFO head pointer reg */
#define MPC107_MSG_I2O_IFTPR    0x128 /* Inbound free_FIFO tail pointer reg */
#define MPC107_MSG_I2O_IPHPR    0x130 /* Inbound post_FIFO head pointer reg */
#define MPC107_MSG_I2O_IPTPR    0x138 /* Inbound post_FIFO tail pointer reg */
#define MPC107_MSG_I2O_OFHPR    0x140 /* Outbound free_FIFO head pointer reg */
#define MPC107_MSG_I2O_OFTPR    0x148 /* Outbound free_FIFO tail pointer reg */
#define MPC107_MSG_I2O_OPHPR    0x150 /* Outbound post_FIFO head pointer reg */
#define MPC107_MSG_I2O_OPTPR    0x158 /* Outbound post_FIFO tail pointer reg */
#define MPC107_MSG_I2O_MUCR     0x164 /* Messaging unit control reg */
#define MPC107_MSG_I2O_QBAR     0x170 /* Queue base address register */

#define MPC107_MSG_I2O_PCI_OMISR    0x30 /* Outbound message int status Reg */
#define MPC107_MSG_I2O_PCI_OMIMR    0x34 /* Outbound message int mask Reg */
#define MPC107_MSG_I2O_PCI_IFQPR    0x40 /* Inbound FIFO queue port reg */
#define MPC107_MSG_I2O_PCI_OFQPR    0x44 /* Outbound FIFO queue port reg */


/* Message Unit (I2O) Register Bit Definitions */

/* Outbound Message Interrupt Status Register */

#define MPC107_MSG_I2O_OPQI	0x00000020  /* Outbound Post Queue Interrupt */
#define MPC107_MSG_I2O_ODI      0x00000008  /* Outbound Doorbell Interrupt */
#define MPC107_MSG_I2O_OM1I     0x00000002  /* Outbound Message 1 Interrupt */
#define MPC107_MSG_I2O_OM0I     0x00000001  /* Outbound Message 0 Interrupt */

/* Outbound Message Interrupt Mask Register */

#define MPC107_MSG_I2O_OPQIM    0x00000020  /* Out Post Queue Interrupt Mask */
#define MPC107_MSG_I2O_ODIM     0x00000008  /* Out Doorbell Interrupt Mask */
#define MPC107_MSG_I2O_OM1IM    0x00000002  /* Outbound Msg 1 Interrupt Mask */
#define MPC107_MSG_I2O_OM0IM    0x00000001  /* Outbound Msg 0 Interrupt Mask */

/* Inbound Message Interrupt Status Register */

#define MPC107_MSG_I2O_OFOI	0x00000100  /* Outbound Free Overflow Int */
#define MPC107_MSG_I2O_IPOI	0x00000080  /* Inbound Post Overflow Int */
#define MPC107_MSG_I2O_IPQI     0x00000020  /* Inbound Post Queue Interrupt */
#define MPC107_MSG_I2O_MCI	0x00000010  /* Machine Check Interrupt */
#define MPC107_MSG_I2O_IDI      0x00000008  /* Inbound Doorbell Interrupt */
#define MPC107_MSG_I2O_IM1I     0x00000002  /* Inbound Message 1 Interrupt */
#define MPC107_MSG_I2O_IM0I     0x00000001  /* Inbound Message 0 Interrupt */

/* Inbound Message Interrupt Mask Register */

#define MPC107_MSG_I2O_OFOIM    0x00000100  /* Outbound Free Ovrflow Mask */
#define MPC107_MSG_I2O_IPOIM    0x00000080  /* Inbound Post Overflow Mask */
#define MPC107_MSG_I2O_IPQIM    0x00000020  /* Inbound Post Queue Mask */
#define MPC107_MSG_I2O_MCIM     0x00000010  /* Machine Check Mask */
#define MPC107_MSG_I2O_IDIM     0x00000008  /* Inbound Doorbell Mask */
#define MPC107_MSG_I2O_IM1IM    0x00000002  /* Inbound Message 1 Mask */
#define MPC107_MSG_I2O_IM0IM    0x00000001  /* Inbound Message 0 Mask */

/* Inbound FIFO Pointer Registers */

#define MPC107_MSG_I2O_IN_QBA	   0xfff00000	/* Inbound Queue Base Addr */
#define MPC107_MSG_I2O_IN_FIFOPTR  0x000ffffc	/* In FIFO pointer offset */

/* Outbound FIFO Pointer Registers */

#define MPC107_MSG_I2O_OUT_QBA      0xfff00000   /* Outbound Queue Base Addr */
#define MPC107_MSG_I2O_OUT_FIFOPTR  0x000ffffc   /* Out FIFO pointer offset */


/* I2O Messaging Unit Control Register */

#define MPC107_MSG_I2O_CQS 	    0x0000003e  /* circular queue size mask */
#define MPC107_MSG_I2O_CQS_16K	    0x00000002  /* 4K entries, 16K bytes */
#define MPC107_MSG_I2O_CQS_32K	    0x00000004  /* 8K entries, 32K bytes */
#define MPC107_MSG_I2O_CQS_64K	    0x00000008  /* 16K entries, 64K bytes */
#define MPC107_MSG_I2O_CQS_128K	    0x00000010  /* 32K entries, 128K bytes */
#define MPC107_MSG_I2O_CQS_256K	    0x00000020  /* 64K entries, 256K bytes */
#define MPC107_MSG_I2O_CQE	    0x00000001  /* circular queue enable */
#define MPC107_MSG_I2O_MUCR_DEFAULT MPC107_MSG_I2O_CQS_16K /* Default CQ Size */
#define MPC107_MSG_I2O_CQS_ALIGN    20          /* 1 Mb Memory alignment */

#define MPC107_MSG_16KSIZE 	        0x1000	                /* 4K Words */
#define MPC107_MSG_I2O_CQS_16K_SIZE    (MPC107_MSG_16KSIZE * 1) /* 4K Words */
#define MPC107_MSG_I2O_CQS_32K_SIZE    (MPC107_MSG_16KSIZE * 2) /* 8K Words */
#define MPC107_MSG_I2O_CQS_64K_SIZE    (MPC107_MSG_16KSIZE * 4) /* 16K Words */
#define MPC107_MSG_I2O_CQS_128K_SIZE   (MPC107_MSG_16KSIZE * 8) /* 32K Words */
#define MPC107_MSG_I2O_CQS_256K_SIZE   (MPC107_MSG_16KSIZE * 16)/* 64K Words */

#define MPC107_MSG_I2O_MFRAME_SIZE 	0x20 /* Message Frame size 128 bytes */
                                             /* 32 words  */
#define MPC107_MSG_I2O_MUCR_QBAR_ALIGN    0x100000 /* 1 Mbytes */


/* Inbound/Outbound Door Bell Register Definitions */

#define MPC107_MSG_IDBR_DATA_MASK   0xefffffff /* IDBR Data Mask */
#define MPC107_MSG_IDBR_MC_SHIFT    31         /* IDBReg Machine Check Mask */
#define MPC107_MSG_IDBR_MC_BIT	    0x80000000    /* IDBR Machine Check Bit */
#define MPC107_MSG_ODBR_DATA_MASK   0x1fffffff /* ODBR Data Mask */

/* Extended Doorbell register definitions */

#define MPC107_MSG_EDBMR_INTAIM_BIT   0x00000001 /* EDMBR INTA */
                                                 /* Interrupt mask bit */
#define MPC107_MSG_EDBMR_C0IM_BIT     0x00000008 /* EDMBR CPU */
                                                 /* Interrupt mask bit */
#define MPC107_MSG_EDBW1S_INTA_MSK    0xff000000 /* EDBW1S Reg INTA mask */
#define MPC107_MSG_EDBW1S_INTA_SHIFT  24         /* EDBW1S Reg INTA shift */
#define MPC107_MSG_EDBW1S_CP0_MSK     0x00ff0000 /* EDBW1S Reg CPU INT mask */
#define MPC107_MSG_EDBW1S_CP0_SHIFT   16         /* EDBW1S Reg CPU INT shift */

/* I2O Queue Base Address Register */

#define MPC107_MSG_I2O_QBA	0xfff00000  /* queue base address bits */

/* General definitions */

#define MPC107_MSG_WRITE            0x0  /* Read */
#define MPC107_MSG_READ             0x1  /* Write */
#define MPC107_MSG_READ_OR_WRITE    0x2  /* Read OR Write */
#define MPC107_MSG_READ_AND_WRITE   0x3  /* Read AND Write */
#define MPC107_MSG_READ_ANDOR_WRITE 0x4  /* Read ANDOR Write */
#define MPC107_MSG_INBOUND	    0x01 /* In Bound */
#define MPC107_MSG_OUTBOUND         0x02 /* Out Bound*/


#define MPC107_MSG_GENERAL_LSB_MSK 0xff /* LSB Mask */

#define MPC107_MSG_INTAGEN	0x0       /* Generate INTA Interrupt */
#define MPC107_MSG_CP0GEN	0x1       /* Generate CPU Interrupt */

#define MPC107_MSG_REGISTER0    0x0	  /* Message Regiter 0 */
#define MPC107_MSG_REGISTER1    0x1       /* Message Regiter 1 */

#define MPC107_MSG_I2O_NO_FREE_MFA   0xffffffff  /* No MFA's are available */
#define MPC107_MSG_I2O_FIFO_EMPTY    0xffffffff  /* FIFO is empty   */


/* error Values  */

#define MPC107_MSG_CHOUT_RANGE      0x10   /* Channel out of range */
#define MPC107_MSG_REGOUT_RANGE     0x11   /* Register out of range */
#define MPC107_MSG_UNDEF_REG        0x12   /* Undefined Register */
#define MPC107_MSG_UNDEF_INT_TYPE   0x13   /* Undefined Interrupt type */

/* function declarations */

IMPORT void 	mpc107MsgMessageHostInit(void);
IMPORT void 	mpc107MsgMessagePeripheralInit(void);
IMPORT STATUS 	mpc107MsgExtDoorbellInit (UINT32 cpuIntA);
IMPORT STATUS 	mpc107MsgMessageWrite (UINT32 inOutBound,
                                       UINT32 messageRegNum,
                                       UINT32 message);
IMPORT STATUS 	mpc107MsgMessageRead (UINT32 inOutBound, UINT32 messageRegNum,
                                      UINT32 *pMessageRead );
IMPORT STATUS 	mpc107MsgDoorbellWrite (UINT32 inOutBound, UINT32 message,
                                        UINT32 machineCheck);
IMPORT STATUS 	mpc107MsgDoorbellRead (UINT32 inOutBound,
                                       UINT32 *pMsgDoorbellRead);
IMPORT STATUS 	mpc107MsgExtDoorbellSetWrite (UINT32 message, UINT32 cpuIntA);
IMPORT ULONG 	mpc107MsgExtDoorbellClrRead (void);
IMPORT STATUS 	mpc107MsgI2oInit (void);
IMPORT void 	mpc107MsgI2oInboundRead (void);
IMPORT STATUS	mpc107MsgI2oInboundWrite (UINT32 *pBuf);
IMPORT void 	mpc107MsgI2oOutboundRead (void);
IMPORT void 	mpc107MsgI2oOutboundWrite (UINT32 *pBuf);

#ifdef __cplusplus
}
#endif

#endif	/* __INCmpc107Msgh */

