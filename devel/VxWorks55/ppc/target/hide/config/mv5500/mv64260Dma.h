/* mv64260Dma.h - MV64260 DMA Controller Header File */

/* Copyright 2002 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01b,05jun03,yyz  Added define for IDMA chain-mode enable, and fixed indentation.
01a,08oct02,efb  New. Base on harrierDma.c features (01b,21jan02,mcp820).
*/

/*
DESCRIPTION
This file contains the MV64260 DMA register bit definitions, masks, and 
typical values. Register offsets are defined in the main MV64260 header file.
*/

#ifndef	INCmv64260Dmah
#define	INCmv64260Dmah

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

/* includes */

#include "sysDma.h"

/* Maximum counts */

#define IDMA_CHAN_CNT              8
#define IDMA_ARB_CNT               2
#define IDMA_ARB_SLICE_CNT         16

#define IDMA_CHAN_GRP_INTERVAL     0x100
#define IDMA_CHAN_GRP_SZ           (IDMA_CHAN_CNT / 2)

/* Main Interrupt Cause definitions for DMA */

#define IDMA_ERR_INT                ICI_MICL_INT_NUM_2

#define IDMA_CHAN0_1_COMPLETION_INT ICI_MICL_INT_NUM_4
#define IDMA_CHAN2_3_COMPLETION_INT ICI_MICL_INT_NUM_5
#define IDMA_CHAN4_5_COMPLETION_INT ICI_MICL_INT_NUM_6
#define IDMA_CHAN6_7_COMPLETION_INT ICI_MICL_INT_NUM_7

/* IDMA Channel Control Register (lower) */

#define IDMA_CTRL_DBURST_LIMIT_MASK   0x7
 
#define IDMA_CTRL_DBURST_LIMIT_8B     0x0
#define IDMA_CTRL_DBURST_LIMIT_16B    0x1
#define IDMA_CTRL_DBURST_LIMIT_32B    0x3
#define IDMA_CTRL_DBURST_LIMIT_64B    0x7
#define IDMA_CTRL_DBURST_LIMIT_128B   0x4

#define IDMA_CTRL_HOLD_SRC_ADDR_BIT   3
#define IDMA_CTRL_HOLD_SRC_ADDR       (0x01 << IDMA_CTRL_HOLD_SRC_ADDR_BIT)

#define IDMA_CTRL_DMA_ACK_WIDTH_BIT   4
#define IDMA_CTRL_DMA_ACK_DBL_WIDTH   (0x01 << IDMA_CTRL_DMA_ACK_WIDTH_BIT)

#define IDMA_CTRL_HOLD_DEST_ADDR_BIT  5
#define IDMA_CTRL_HOLD_DEST_ADDR      (0x01 << IDMA_CTRL_HOLD_DEST_ADDR_BIT)

#define IDMA_CTRL_SBURST_LIMIT_BIT    6
#define IDMA_CTRL_SBURST_LIMIT_MASK   (0x7 << IDMA_CTRL_SBURST_LIMIT_BIT)

#define IDMA_CTRL_SBURST_LIMIT_8B     (~IDMA_CTRL_SBURST_LIMIT_MASK)
#define IDMA_CTRL_SBURST_LIMIT_16B    (0x1 << IDMA_CTRL_SBURST_LIMIT_BIT)
#define IDMA_CTRL_SBURST_LIMIT_32B    (0x3 << IDMA_CTRL_SBURST_LIMIT_BIT)
#define IDMA_CTRL_SBURST_LIMIT_64B    (0x7 << IDMA_CTRL_SBURST_LIMIT_BIT)
#define IDMA_CTRL_SBURST_LIMIT_128B   (0x4 << IDMA_CTRL_SBURST_LIMIT_BIT)

#define IDMA_CTRL_CHAIN_MODE_BIT      9
#define IDMA_CTRL_NON_CHAIN_MODE      (0x01 << IDMA_CTRL_CHAIN_MODE_BIT)
#define IDMA_CTRL_CHAIN_MODE          ~IDMA_CTRL_NON_CHAIN_MODE

#define IDMA_CTRL_INT_MODE_BIT 	      10
#define IDMA_CTRL_INT_MODE            (0x01 << IDMA_CTRL_INT_MODE_BIT)

#define IDMA_CTRL_BLOCK_MODE_BIT      11
#define IDMA_CTRL_BLOCK_MODE          (0x01 << IDMA_CTRL_BLOCK_MODE_BIT)

#define IDMA_CTRL_CHAN_ENB_BIT        12
#define IDMA_CTRL_CHAN_ENB            (0x01 << IDMA_CTRL_CHAN_ENB_BIT)
 
#define IDMA_CTRL_FETCH_NXT_DESC_BIT  13
#define IDMA_CTRL_FETCH_NXT_DESC      (0x01 << IDMA_CTRL_FETCH_NXT_DESC_BIT)

#define IDMA_CTRL_CHAN_ACTIVE_BIT     14
#define IDMA_CTRL_CHAN_ACTIVE_MASK    (0x01 << IDMA_CTRL_CHAN_ACTIVE_BIT)

#define IDMA_CTRL_DMA_REQ_DIR_BIT     15
#define IDMA_CTRL_DMA_REQ_DIR_DEST    (0x01 << IDMA_CTRL_DMA_REQ_DIR_BIT)

#define IDMA_CTRL_DMA_REQ_MODE_BIT    16
#define IDMA_CTRL_DMA_REQ_MODE_EDGE   (0x01 << IDMA_CTRL_DMA_REQ_MODE_BIT)

#define IDMA_CTRL_CLOSE_DESC_ENB_BIT  17
#define IDMA_CTRL_CLOSE_DESC_ENB      (0x01 << IDMA_CTRL_CLOSE_DESC_ENB_BIT)

#define IDMA_CTRL_END_XFER_ENB_BIT    18
#define IDMA_CTRL_END_OF_XFER_ENB     (0x01 << IDMA_CTRL_END_XFER_ENB_BIT)

#define IDMA_CTRL_END_XFER_MODE_BIT   19
#define IDMA_CTRL_END_OF_XFER_HALT    (0x01 << IDMA_CTRL_END_XFER_MODE_BIT)

#define IDMA_CTRL_CHAN_ABORT_BIT      20
#define IDMA_CTRL_CHAN_ABORT          (0x01 << IDMA_CTRL_CHAN_ABORT_BIT)

#define IDMA_CTRL_SADDR_OVERRIDE_BIT  21
#define IDMA_CTRL_SADDR_OVERRIDE_MASK (0x3 << IDMA_CTRL_SADDR_OVERRIDE_BIT)

#define IDMA_CTRL_SADDR_OVERRIDE_BAR1 (0x1 << IDMA_CTRL_SADDR_OVERRIDE_BIT)
#define IDMA_CTRL_SADDR_OVERRIDE_BAR2 (0x2 << IDMA_CTRL_SADDR_OVERRIDE_BIT)

#define IDMA_CTRL_DADDR_OVERRIDE_BIT  22
#define IDMA_CTRL_DADDR_OVERRIDE_MASK (0x3 << IDMA_CTRL_DADDR_OVERRIDE_BIT)

#define IDMA_CTRL_DADDR_OVERRIDE_BAR1 (0x1 << IDMA_CTRL_DADDR_OVERRIDE_BIT)
#define IDMA_CTRL_DADDR_OVERRIDE_BAR2 (0x2 << IDMA_CTRL_DADDR_OVERRIDE_BIT)

#define IDMA_CTRL_DESC_OVERRIDE_BIT   25
#define IDMA_CTRL_DESC_OVERRIDE_MASK  (0x3 << IDMA_CTRL_DESC_OVERRIDE_BIT)

#define IDMA_CTRL_DESC_OVERRIDE_BAR1  (0x1 << IDMA_CTRL_DESC_OVERRIDE_BIT)
#define IDMA_CTRL_DESC_OVERRIDE_BAR2  (0x2 << IDMA_CTRL_DESC_OVERRIDE_BIT)

#define IDMA_CTRL_DMA_ACK_MODE_BIT    27
#define IDMA_CTRL_DMA_ACK_XACTN       (0x1 << IDMA_CTRL_DMA_ACK_MODE_BIT)

#define IDMA_CTRL_DMA_TMR_REQ_BIT     28
#define IDMA_CTRL_DMA_TMR_REQ         (0x1 << IDMA_CTRL_DMA_TMR_REQ_BIT)

#define IDMA_CTRL_DMA_ACK_DIR_BIT     29
#define IDMA_CTRL_DMA_ACK_DIR_MASK    (0x3 << IDMA_CTRL_DMA_ACK_DIR_BIT)

#define IDMA_CTRL_DMA_ACK_DIR_DEST    (0x1 << IDMA_CTRL_DMA_ACK_DIR_BIT)
#define IDMA_CTRL_DMA_ACK_DIR_SRC     (0x2 << IDMA_CTRL_DMA_ACK_DIR_BIT)

#define IDMA_CTRL_DESC_MODE_BIT       31
#define IDMA_CTRL_DESC_16MB           (0x1 << IDMA_CTRL_DESC_MODE_BIT)
#define IDMA_CTRL_DESC_64KB           ~IDMA_CTRL_DESC_16MB

/* IDMA Channel Control Register (high) */

/* Bit 3:0 are reserved */

#define IDMA_CTRL_SRC_PCI_SWAP_BIT       4
#define IDMA_CTRL_SRC_PCI_SWAP_MASK      (0x3 << IDMA_CTRL_SRC_PCI_SWAP_BIT)

#define IDMA_CTRL_SRC_PCI_BYTE_SWAP      0x0
#define IDMA_CTRL_SRC_PCI_NO_SWAP        0x1
#define IDMA_CTRL_SRC_PCI_BYTE_WORD_SWAP 0x2
#define IDMA_CTRL_SRC_PCI_WORD_SWAP      0x3

#define IDMA_CTRL_SRC_SNOOP_ENB_BIT      6
#define IDMA_CTRL_SRC_SNOOP_ENB_MASK     (0x1 << IDMA_CTRL_SRC_SNOOP_ENB_BIT)

#define IDMA_CTRL_SRC_PCI_REQ64_BIT      7
#define IDMA_CTRL_SRC_PCI_REQ64_MASK     (0x1 << IDMA_CTRL_SRC_PCI_REQ64_BIT)

#define IDMA_CTRL_DEST_PCI_SWAP_BIT       12
#define IDMA_CTRL_DEST_PCI_SWAP_MASK      (0x3 << IDMA_CTRL_DEST_PCI_SWAP_BIT)

#define IDMA_CTRL_DEST_PCI_BYTE_SWAP      0x0
#define IDMA_CTRL_DEST_PCI_NO_SWAP        0x1
#define IDMA_CTRL_DEST_PCI_BYTE_WORD_SWAP 0x2
#define IDMA_CTRL_DEST_PCI_WORD_SWAP      0x3

#define IDMA_CTRL_DEST_SNOOP_ENB_BIT      14
#define IDMA_CTRL_DEST_SNOOP_ENB_MASK     (0x1 << IDMA_CTRL_DEST_SNOOP_ENB_BIT)

#define IDMA_CTRL_DEST_PCI_REQ64_BIT      15
#define IDMA_CTRL_DEST_PCI_REQ64_MASK     (0x1 << IDMA_CTRL_DEST_PCI_REQ64_BIT)

#define IDMA_CTRL_NXT_PCI_SWAP_BIT       20
#define IDMA_CTRL_NXT_PCI_SWAP_MASK      (0x3 << IDMA_CTRL_NXT_PCI_SWAP_BIT)

#define IDMA_CTRL_NXT_PCI_BYTE_SWAP      0x0
#define IDMA_CTRL_NXT_PCI_NO_SWAP        0x1
#define IDMA_CTRL_NXT_PCI_BYTE_WORD_SWAP 0x2
#define IDMA_CTRL_NXT_PCI_WORD_SWAP      0x3

#define IDMA_CTRL_NXT_SNOOP_ENB_BIT      22
#define IDMA_CTRL_NXT_SNOOP_ENB_MASK     (0x1 << IDMA_CTRL_NXT_SNOOP_ENB_BIT)

#define IDMA_CTRL_NXT_PCI_REQ64_BIT      23
#define IDMA_CTRL_NXT_PCI_REQ64_MASK     (0x1 << IDMA_CTRL_NXT_PCI_REQ64_BIT)

#define IDMA_CTRL_SRC_HALF_WORD_SWAP_BIT  24
#define IDMA_CTRL_SRC_HALF_WORD_SWAP_MASK          \
        (0x1 << IDMA_CTRL_SRC_HALF_WORD_SWAP_BIT)

#define IDMA_CTRL_DEST_HALF_WORD_SWAP_BIT 25
#define IDMA_CTRL_DEST_HALF_WORD_SWAP_MASK         \
        (0x1 << IDMA_CTRL_DEST_HALF_WORD_SWAP_BIT)

#define IDMA_CTRL_BURST_LIMIT_MODE_BIT    31
#define IDMA_CTRL_BURST_LIMIT_MODE_MASK            \
        (0x1 << IDMA_CTRL_BURST_LIMIT_MODE_BIT)

/* IDMA Descriptors */

#define IDMA_DESC_BYTE_CNT_BIT         0

#define IDMA_DESC_REMAIN_BYTE_CNT_MASK 0xFFFF0000
#define IDMA_DESC_64KB_BYTE_CNT_MASK   0x0000FFFF
#define IDMA_DESC_16MB_BYTE_CNT_MASK   0x00FFFFFF

#define IDMA_DESC_BYTE_CNT_LEFT_BIT    30
#define IDMA_DESC_BYTE_CNT_LEFT_MASK   (0x1 << IDMA_DESC_BYTE_CNT_LEFT_BIT)

#define IDMA_DESC_OWNERSHIP_BIT        31
#define IDMA_DESC_OWNERSHIP_MASK       (0x1 << IDMA_DESC_OWNERSHIP_BIT)

#define IDMA_DESC_CPU_OWNERSHIP        IDMA_DESC_OWNERSHIP_MASK
#define IDMA_DESC_DMA_OWNERSHIP        ~IDMA_DESC_OWNERSHIP_MASK

/* IDMA Arbiter Control Register */

#define IDMA_ARB_0_BIT               0
#define IDMA_ARB_1_BIT               2
#define IDMA_ARB_2_BIT               4
#define IDMA_ARB_3_BIT               6
#define IDMA_ARB_4_BIT               8
#define IDMA_ARB_5_BIT               10
#define IDMA_ARB_6_BIT               12
#define IDMA_ARB_7_BIT               14
#define IDMA_ARB_8_BIT               16
#define IDMA_ARB_9_BIT               18
#define IDMA_ARB_10_BIT              20
#define IDMA_ARB_11_BIT              22
#define IDMA_ARB_12_BIT              24
#define IDMA_ARB_13_BIT              26
#define IDMA_ARB_14_BIT              28
#define IDMA_ARB_15_BIT              30
#define IDMA_ARB_SLICE_MASK          0x3

#define IDMA_ARB_CHAN0_SEL        0x0
#define IDMA_ARB_CHAN1_SEL        0x1
#define IDMA_ARB_CHAN2_SEL        0x2
#define IDMA_ARB_CHAN3_SEL        0x3

/* IDMA Crossbar Timeout Register */

#define IDMA_XBAR_TMOUT_BIT       0
#define IDMA_XBAR_TMOUT_MASK      0xFF
#define IDMA_XBAR_TMOUT_PRESET    0xFF

#define IDMA_XBAR_TMOUT_ENB_BIT   16
#define IDMA_XBAR_TMOUT_DISABLE   (0x1 << IDMA_XBAR_TMOUT_ENB_BIT)

/* IDMA Interrupt Cause Register */

#define IDMA_INT_CAUSE_CHAN0_4_BIT       0
#define IDMA_INT_CAUSE_CHAN1_5_BIT       8
#define IDMA_INT_CAUSE_CHAN2_6_BIT       16
#define IDMA_INT_CAUSE_CHAN3_7_BIT       24
#define IDMA_INT_CAUSE_CHAN_MASK         0x1F
#define IDMA_INT_CAUSE_CHAN_ERR_MASK     0x1E

#define IDMA_INT_CAUSE_DMA_COMPL_BIT     0
#define IDMA_INT_CAUSE_DMA_COMPL_MASK    (0x1 << IDMA_INT_CAUSE_DMA_COMPL_BIT)

#define IDMA_INT_CAUSE_ADDR_MISS_BIT     1
#define IDMA_INT_CAUSE_ADDR_MISS_MASK    (0x1 << IDMA_INT_CAUSE_ADDR_MISS_BIT)

#define IDMA_INT_CAUSE_ACC_PTCT_BIT      2
#define IDMA_INT_CAUSE_ACC_PTCT_MASK     (0x1 << IDMA_INT_CAUSE_ACC_PTCT_BIT)

#define IDMA_INT_CAUSE_WRT_PTCT_BIT      3
#define IDMA_INT_CAUSE_WRT_PTCT_MASK     (0x1 << IDMA_INT_CAUSE_WRT_PTCT_BIT)

#define IDMA_INT_CAUSE_OWN_VIOL_BIT      4
#define IDMA_INT_CAUSE_OWN_VIOL_MASK     (0x1 << IDMA_INT_CAUSE_OWN_VIOL_BIT)

/* IDMA Interrupt Mask Register */

#define IDMA_INT_MASK_CHAN0_4_BIT       0
#define IDMA_INT_MASK_CHAN1_5_BIT       8
#define IDMA_INT_MASK_CHAN2_6_BIT       16
#define IDMA_INT_MASK_CHAN3_7_BIT       24
#define IDMA_INT_MASK_CHAN_MASK         0x1F

#define IDMA_INT_MASK_CHAN0_4_MASK      (IDMA_INT_MASK_CHAN_MASK << \
                                        IDMA_INT_MASK_CHAN0_4_BIT)

#define IDMA_INT_MASK_CHAN1_5_MASK      (IDMA_INT_MASK_CHAN_MASK << \
                                        IDMA_INT_MASK_CHAN1_5_BIT)

#define IDMA_INT_MASK_CHAN2_6_MASK      (IDMA_INT_MASK_CHAN_MASK << \
                                        IDMA_INT_MASK_CHAN2_6_BIT)

#define IDMA_INT_MASK_CHAN3_7_MASK      (IDMA_INT_MASK_CHAN_MASK << \
                                        IDMA_INT_MASK_CHAN3_7_BIT)

#define IDMA_INT_MASK_DMA_COMPL_BIT     0
#define IDMA_INT_MASK_DMA_COMPL_MASK    (0x1 << IDMA_INT_MASK_DMA_COMPL_BIT)

#define IDMA_INT_MASK_ADDR_MISS_BIT     1
#define IDMA_INT_MASK_ADDR_MISS_MASK    (0x1 << IDMA_INT_MASK_ADDR_MISS_BIT)

#define IDMA_INT_MASK_ACC_PTCT_BIT      2
#define IDMA_INT_MASK_ACC_PTCT_MASK     (0x1 << IDMA_INT_MASK_ACC_PTCT_BIT)

#define IDMA_INT_MASK_WRT_PTCT_BIT      3
#define IDMA_INT_MASK_WRT_PTCT_MASK     (0x1 << IDMA_INT_MASK_WRT_PTCT_BIT)

#define IDMA_INT_MASK_OWN_VIOL_BIT      4
#define IDMA_INT_MASK_OWN_VIOL_MASK     (0x1 << IDMA_INT_MASK_OWN_VIOL_BIT)

/* IDMA Error Select Register */

#define IDMA_ERR_SEL_MASK             0x1E
#define IDMA_ERR_SEL_CHAN_MASK        0x18
#define IDMA_ERR_SEL_CHAN_BIT         3

#define IDMA_ERR_ADDR_MISS_CHAN0_4    0x1
#define IDMA_ERR_ADDR_MISS_CHAN1_5    0x9
#define IDMA_ERR_ADDR_MISS_CHAN2_6    0x10
#define IDMA_ERR_ADDR_MISS_CHAN3_7    0x19

#define IDMA_ERR_ACC_PTCT_CHAN0_4     0x2
#define IDMA_ERR_ACC_PTCT_CHAN1_5     0xA
#define IDMA_ERR_ACC_PTCT_CHAN2_6     0x12
#define IDMA_ERR_ACC_PTCT_CHAN3_7     0x1A

#define IDMA_ERR_WRT_PTCT_CHAN0_4     0x3
#define IDMA_ERR_WRT_PTCT_CHAN1_5     0xB
#define IDMA_ERR_WRT_PTCT_CHAN2_6     0x13
#define IDMA_ERR_WRT_PTCT_CHAN3_7     0x1B

#define IDMA_ERR_OWN_VIOL_CHAN0_4     0x4
#define IDMA_ERR_OWN_VIOL_CHAN1_5     0xC
#define IDMA_ERR_OWN_VIOL_CHAN2_6     0x14
#define IDMA_ERR_OWN_VIOL_CHAN3_7     0x1C

/*
 * The following structure defines the basic DMA transfer parameters.
 */

typedef DMA_DESCRIPTOR IDMA_DESCRIPTOR; /* Generic byte count and addresses */

/*
 * The following structure defines user controlled attributes for a
 * given DMA transfer. The userHandler routine provides an optional
 * hook for notification upon completion interrupt occurrence. The
 * value of this field provides three notification options based on
 * its value: Wait in the DMA start routine until the completion 
 * interrupt occurs (userHandler == WAIT_FOREVER), call a user routine 
 * from the interrupt handler upon DMA completion that conforms to ISR rules 
 * (userHandler == user routine), return immediately and check for 
 * completion later (userHandler == 0). Chained mode is not currently
 * supported. 
 */

typedef DMA_ATTRIBUTES IDMA_ATTRIBUTES; /* Generic DMA transfer options */

/*
 * The following structure defines DMA result information for the last
 * successful DMA transfer or the last error for a particular channel. 
 * The dmaErrorCode and dmaErrorAddr fields are only meaningful if 
 * dmaError is ERROR. No error code (0) with an ERROR status indicates DMA is 
 * still in progress.
 */
 
typedef struct mv64260DmaStatus
    {
    STATUS dmaStatus;       /* OK when DMA completes successfully */
    UINT32 dmaErrorCode;    /* Error code when dmaStatus is ERROR */
    UINT32 dmaErrorAddr;    /* Address causing the error code */
    UINT32 curSourceAddr;   /* Current address of source data */
    UINT32 curDestAddr;     /* Current address of destination */
    UINT32 curNextDesc;     /* Address of next descriptor */
    UINT32 chanCtrlLow;     /* Current channel control settings */
    UINT32 chanCtrlHigh;    /* Current channel control settings */
    } IDMA_STATUS;


STATUS sysMv64260DmaInit (void);

STATUS sysMv64260DmaArbiterSet (UINT32 arbId, UINT32 arbSlice[]);

STATUS sysMv64260DmaStart (UINT32 chan, IDMA_DESCRIPTOR *dmaDesc, 
                           IDMA_ATTRIBUTES *dmaAttrib, IDMA_STATUS *dmaStatus);

STATUS sysMv64260DmaStatus (UINT32 chan, IDMA_STATUS *dmaStatus);

STATUS sysMv64260DmaAbort (UINT32 chan);

STATUS sysMv64260DmaPause (UINT32 chan);

STATUS sysMv64260DmaResume (UINT32 chan);


#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* INCmv64260Dmah */
