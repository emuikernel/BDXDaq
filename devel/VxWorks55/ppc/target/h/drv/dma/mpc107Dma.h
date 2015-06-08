/* mpc107Dma.h - MPC107 DMA registers definitions  */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,11sep00,rcs  fix include path for mpc107.h
01a,06jun00,bri written
*/

#ifndef	__INCmpc107Dmah
#define	__INCmpc107Dmah

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include "drv/multi/mpc107.h"

/* defines */

/* MPC107 configuration registers for DMA interface  */

#define MPC107_DMA_EUMBBAR	0x1000  /* Offset of DMA registers */
                                        /* in EUMBBAR Space */

/* DMA Channel Zero Register Definitions */

#define MPC107_DMA_DMR0  (MPC107_DMA_EUMBBAR + 0x100)/* Mode Register */
#define MPC107_DMA_DSR0  (MPC107_DMA_EUMBBAR + 0x104)/* Status Register */
#define MPC107_DMA_CDAR0 (MPC107_DMA_EUMBBAR + 0x108)/* Current Descriptor */
#define MPC107_DMA_SAR0  (MPC107_DMA_EUMBBAR + 0x110)/* Source Address */
#define MPC107_DMA_DAR0  (MPC107_DMA_EUMBBAR + 0x118)/* Destination Address */
#define MPC107_DMA_BCR0  (MPC107_DMA_EUMBBAR + 0x120)/* Byte Count */
#define MPC107_DMA_NDAR0 (MPC107_DMA_EUMBBAR + 0x124)/* Next Descriptor */

/* DMA Channel One  Register Definitions */

#define MPC107_DMA_DMR1   (MPC107_DMA_EUMBBAR + 0x200) /* Mode Register */
#define MPC107_DMA_DSR1   (MPC107_DMA_EUMBBAR + 0x204) /* Status Register */
#define MPC107_DMA_CDAR1  (MPC107_DMA_EUMBBAR + 0x208) /* Current Descriptor */
#define MPC107_DMA_SAR1   (MPC107_DMA_EUMBBAR + 0x210) /* Source Address */
#define MPC107_DMA_DAR1   (MPC107_DMA_EUMBBAR + 0x218) /* Destination Address */
#define MPC107_DMA_BCR1   (MPC107_DMA_EUMBBAR + 0x220) /* Byte Count */
#define MPC107_DMA_NDAR1  (MPC107_DMA_EUMBBAR + 0x224) /* Next Descriptor */

/* Bit Definitions */

/* Mode Register Bit Definitons */

#define MPC107_DMA_DMR_CS        0x00000001   /* Channel Start */
#define MPC107_DMA_DMR_CC        0x00000002   /* Channel Continue */
#define MPC107_DMA_DMR_CTM       0x00000004   /* Direct DMA mode */
                                              /* Channel Transfer */
#define MPC107_DMA_DMR_DL        0x00000008   /* Descriptor Location */
#define MPC107_DMA_DMR_EOTIE     0x00000080   /* End of Transfer */
                                              /* Interrupt Enable */
#define MPC107_DMA_DMR_EIE       0x00000100   /* Error Interrupt Enable */
#define MPC107_DMA_DMR_PDE       0x00040000   /* Periodic DMA Enable */

/*  Status Register Bit Definitions */

#define MPC107_DMA_DSR_EOCAI       0x00000001   /* End of chain or */
                                                /* direct Interrupt */
#define MPC107_DMA_DSR_EOSI        0x00000002   /* End of Segment Interrupt */
#define MPC107_DMA_DSR_CB          0x00000004   /* Channel Busy */
#define MPC107_DMA_DSR_PE          0x00000010   /* PCI Error */
#define MPC107_DMA_DSR_LME         0x00000080   /* Local Memory Error  */

/* Current Descriptor Address Register  Bit definitions */

#define MPC107_DMA_CDAR_CTT_SHIFT  0x2          /* Transfer Type Shift */
#define MPC107_DMA_CDAR_ADDR_MASK  0xffffffc0   /* Address Mask  */
#define MPC107_DMA_CDAR_EOSIE      0x00000008   /* End of Segment Interrupt */
#define MPC107_DMA_CDAR_SNEN       0x00000010   /* Snoop Enable */

/* Next Descriptor Address Register Bit definitions */

#define MPC107_DMA_NDAR_EOTD      0x00000001   /* End of transfer descriptor */
#define MPC107_DMA_NDAR_NDEOSIE   0x00000008   /* Next descriptor */
                                               /* end-of-segment interrupt */
                                               /* enable */
#define MPC107_DMA_NDAR_NDSNEN    0x00000010   /* Snoop Enable */
#define MPC107_DMA_NDAR_ADDR_MASK 0xffffffc0   /* Address Mask  */

/*
 * DMA 0 Mode register default value
 * Error Interrupt Disabled ,End of Transfer
 * Interrupt Disabled and Direct Mode DMA transfer
 */

#define MPC107_DMA_DMR0_DEFAULT   0x04

/*
 * DMA 1 Mode register default value
 * Error Interrupt Disabled ,End of Transfer
 * Interrupt Disabled and Direct Mode DMA transfer
 */

#define MPC107_DMA_DMR1_DEFAULT   0x04


/* General Definitions */

#define MPC107_DMA_CHANNEL0          0x0  /* Channel Zero */
#define MPC107_DMA_CHANNEL1          0x1  /* Channel One */
#define MPC107_DMA_MEM_ALIGN         0x3  /* For aligining to 8 words */
                                          /* 2exp3 = 8 */

#define MPC107_DMA_CH_BUSY_MASK     0x00000001 /* Channel busy mask */

#define MPC107_DMA_TIMEOUT_BUSY     0x10000

/* Error Numbers and Return Values */

#define MPC107_DMA_PERIODIC_CH       0x01  /* Channel configured for */
                                           /* Periodic DMA */
#define MPC107_DMA_CHAIN_CH          0x02  /* Channel configured for */
                                           /* Chained DMA */
#define MPC107_DMA_DIRECT_CH         0x03  /* Channel configured for */
                                           /* Direct DMA */
#define MPC107_DMA_PERIODIC_CH_FREE  0x04  /* periodic DMA is configured */
                                           /* and channel is free */
#define MPC107_DMA_PERIODIC_CH_BUSY  0x05  /* "periodic" DMA is configured */
                                           /* and channel is Busy */
#define MPC107_DMA_CHAIN_CH_FREE     0x06  /* chained  DMA is configured */
                                           /* and channel is free */
#define MPC107_DMA_CHAIN_CH_BUSY     0x07  /* chained  DMA is configured */
                                           /* and channel is busy */
#define MPC107_DMA_DIRECT_CH_FREE    0x08  /* direct  DMA is configured */
                                           /* and channel is free */
#define MPC107_DMA_DIRECT_CH_BUSY    0x09  /* direct  DMA is configured */
                                           /* and channel is busy */
#define  MPC107_DMA_NUM_BYTES        0x0a  /* Number of bytes is zero  */
#define  MPC107_DMA_UNDEF_CH         0x0b  /* Undefined Channel Number */


/* typedefs */

/*
 * Descriptors  for  Chained /Periodic DMA transfers .
 * Descriptors should be alligned on a 8 word byte boundary
 */

typedef struct mpc107DmaDescriptors
    {
     UCHAR *pSourceAddress;        /* Pointer to Source address  */
     ULONG reserved1;              /* Reserved 1 */
     UCHAR *pDestinationAddress;   /* Pointer to Destination address  */
     ULONG reserved2;              /* Reserved 2 */
     struct mpc107DmaDescriptors *pNextDescriptorAddress; /* Pointer to next */
                                                          /* descriptor */
     ULONG reserved3;              /* Reserved 3 */
     ULONG numberOfBytes;          /* Number of bytes */
     ULONG reserved4;              /* Reserved 4 */
    } MPC107_DMA_DESCRIPTOR;

/* function declrations */

IMPORT void 	mpc107DmaInit (void);
IMPORT STATUS 	mpc107DmaStartDirect (UINT32 channelNumber, UINT32 transferType,
                                     UINT32 sourceAddress ,
                                     UINT32 destinationAddress,
                                     UINT32 numberOfBytes);
IMPORT STATUS 	mpc107DmaChainedStart (UINT32 channelNumber,UINT32 timePeriod);
IMPORT STATUS 	mpc107DmaPeriodicStop (UINT32 channelNumber );
IMPORT STATUS 	mpc107DmaBuildDecsriptor (UINT32 channelNumber,
                                         UCHAR *pSourceAddress,
                                         UCHAR *pDestinationAddress,
                                         UINT32 transferType,
                                         UINT32 numberOfBytes);
IMPORT INT32 mpc107DmaStatus (ULONG channel);

#ifdef __cplusplus
}
#endif

#endif	/* __INCmpc107Dmah */

