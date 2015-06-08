/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************


*******************************************************************************
* sdma.h - Header File for : SDMA channel
*
* DESCRIPTION:
*		This file is the header file for SDMA channel. It includes definitions,
*		typedefs and function declarations for the SDMA module.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/

#ifndef __INCsdmah
#define __INCsdmah

/* includes */
#include "gtCore.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */


/* defines  */

/* Set default SDMA Configuration Register (SDCx) */
#define SDMA_CONFIG_UART_VALUE              \
            SDC_RFT_HALF_FIFO           |   \
            SDC_SFM_SINGLE_FRAME_MODE   |   \
            SDC_BSZ_LIMIT_64BIT_X8      


#define SDMA_RX_BUFFER_MAX_SIZE 0x10000
#define SDMA_TX_BUFFER_MAX_SIZE 0x10000

#define SDMA_TX_BUFFER_MIN_SIZE	0x8

/* driver definition */

#define RX_DESC_ALIGNED_SIZE		0x20
#define TX_DESC_ALIGNED_SIZE		0x20

#define TX_BUF_OFFSET_IN_DESC       0x18	/* For buffers less than 8 Bytes */


/* SDMA Configuration Register (SDCx) */
#define SDC_RFT_8_BYTES             0x00000000
#define SDC_RFT_HALF_FIFO           0x00000001
#define SDC_SFM_MULTI_FRAME_MODE    0x00000000 
#define SDC_SFM_SINGLE_FRAME_MODE   0x00000002
#define SDC_RC_MASK                 0x0000003c
#define SDC_BLMR_RX_BIG_ENDIAN      0x00000000
#define SDC_BLMR_RX_LITTLE_ENDIAN   0x00000040
#define SDC_BLMT_TX_BIG_ENDIAN      0x00000000
#define SDC_BLMT_TX_LITTLE_ENDIAN   0x00000080
#define SDC_PVOR_PCI_OVERRIDE       0x00000100
#define SDC_RIFB_ENABLED            0x00000200
#define SDC_BSZ_LIMIT_64BIT_X1      0x00000000
#define SDC_BSZ_LIMIT_64BIT_X2      0x00001000
#define SDC_BSZ_LIMIT_64BIT_X4      0x00002000
#define SDC_BSZ_LIMIT_64BIT_X8      0x00003000


/* SDMA Command Register (SDCMx) */
#define SDCM_ERD_ENABLE_RX_DMA      0x00000080
#define SDCM_AR_ABORT_RX	        0x00008000
#define SDCM_STD_STOP_TX_DMA        0x00010000
#define SDCM_TXD_TX_DEMAND          0x00800000
#define SDCM_AT_ABORT_TX            0x80000000


/* SDMA Descriptor - Command/Status Word (DCSW) */
#define DCSW_ERROR_SUMMARY                BIT15
#define DCSW_LAST_DESC                    BIT16
#define DCSW_FIRST_DESC                   BIT17
#define DCSW_ENABLE_INTERRUPT             BIT23
#define DCSW_AUTO_MODE                    BIT30
#define DCSW_BUFFER_OWNED_BY_DMA          BIT31

/* SDMA Tx Descriptor - Command/Status Word (DCSW) for UART protocol */
#define DCSW_UART_CTS_LOSS                BIT1
#define DCSW_UART_PREAMBLE                BIT18
#define DCSW_UART_TX_ADDRESS              BIT19
#define DCSW_UART_NO_STOP_BIT             BIT20

/* SDMA Rx Descriptor - Command/Status Word (DCSW) for UART protocol */
#define DCSW_UART_PARITY_ERROR			  BIT0
#define DCSW_UART_CD_LOSS                 BIT1
#define DCSW_UART_FRAMING_ERROR           BIT3
#define DCSW_UART_DATA_OVERRUN            BIT6
#define DCSW_UART_BREAK_RECEIVED          BIT9
#define DCSW_UART_MAX_IDEL                BIT10
#define DCSW_UART_RX_ADDRESS              BIT11
#define DCSW_UART_ADDRESS_MATCH           BIT12
#define DCSW_UART_CT			          BIT13
#define DCSW_UART_USER_CTRL_CHAR          BIT14

/* SDMA Tx Descriptor - Command/Status Word (DCSW) for HDLC protocol */
#define DCSW_HDLC_CTS_LOSS                BIT1
#define DCSW_HDLC_DEFERRED                BIT3
#define DCSW_HDLC_DATA_UNDERRUN           BIT6
#define DCSW_HDLC_RETRANSMIT_LIM_ERR      BIT8
#define DCSW_HDLC_COLLISION      	      BIT9
#define DCSW_HDLC_RETRANSMIT_COUNT        (BIT10| BIT11| BIT12| BIT13)
#define DCSW_HDLC_GENERATE_CRC		      BIT22

/* SDMA Rx Descriptor - Command/Status Word (DCSW) for HDLC protocol */
#define DCSW_HDLC_CRC_ERROR			      BIT0
#define DCSW_HDLC_CD_LOSS				  BIT1
#define DCSW_HDLC_DECODING_ERROR		  BIT2
#define DCSW_HDLC_NON_OCTET_FRAME		  BIT3
#define DCSW_HDLC_ABORT_SEQ_RESIDUE0	  BIT4
#define DCSW_HDLC_RESIDUE1                BIT5
#define DCSW_HDLC_DATA_OVERRUN_RESIDUE2   BIT6
#define DCSW_HDLC_MFL_ERROR               BIT7
#define DCSW_HDLC_SHORT_FRAME             BIT8

    
/* typedefs */

typedef enum _sdmaChan 
{	
	SDMA_0,
    SDMA_1
}SDMA_NUM;
	
typedef enum _sdmaStatus
{
    SDMA_OK,                    /* Returned as expected                 */
    SDMA_ERROR,                 /* Fundemental error                    */
    SDMA_END_OF_JOB,            /* Ring has nothing to process          */
    SDMA_QUEUE_FULL,            /* Ring resource error                  */
    SDMA_QUEUE_LAST_RESOURCE,   /* Ring resources about to exhaust      */
	SDMA_RETRY                  /* Could not process request. Try later */
}SDMA_STATUS;
    
    
typedef struct _sdmaRxDesc
{
	unsigned short	bufSize	   ;	
	unsigned short	byteCnt	   ; 
	unsigned int	cmdSts	   ; 
	unsigned int	nextDescPtr; 
	unsigned int	bufPtr	   ;
    unsigned int    returnInfo ;    /* Resource return information */

} SDMA_RX_DESC;


typedef struct _sdmaTxDesc
{    
	unsigned short  byteCnt	   ;
	unsigned short  shadowBc   ;
	unsigned int    cmdSts	   ;
    unsigned int    nextDescPtr;    
	unsigned int    bufPtr	   ;
    unsigned int    returnInfo ;    /* Resource return information */
} SDMA_TX_DESC;


typedef struct _pktInfo		/* Unified structure for Rx and Tx descriptors */
{    
	unsigned int    cmdSts;
	unsigned short  byteCnt;
	unsigned int    bufPtr;
    unsigned int    returnInfo;
} PKT_INFO;


typedef struct _sdmaChannel
{
    SDMA_NUM	  sdmaNum;          /* SDMA channel number 					 */
	unsigned int  sdmaConfig;		/* SDMA config register value 			 */
	unsigned int  protType;			/* Protocol type (UART, HDLC, BISYNC.. ) */

	/* Tx/Rx rings attributes parameters. User defined */
	int           txDescNum;    	/* Number of Transmit Buffer Descriptors */
	int           rxDescNum;    	/* Number of Receive Buffer Descriptors  */

    unsigned int  txDescBase;   	/* Transmit Buffer Descriptors base adrs */
    unsigned int  rxDescBase;   	/* Receive Buffer Descriptors base adrs  */

	unsigned int  txBufBase;  		/* Tx buffer base adrs 					 */
	unsigned int  rxBufBase;  		/* Rx buffer base adrs 					 */
	
	int           txBufSize;  		/* Transmit buffer size 				 */
	int           rxBufSize;  		/* Receive buffer size  				 */


    /* Tx/Rx rings managment variables. For driver use */
	volatile SDMA_RX_DESC   *pRxCurrDesc;  /* Next available resource 		 */
	volatile SDMA_RX_DESC	*pRxUsedDesc;  /* Returning resource      		 */

	volatile SDMA_TX_DESC   *pTxCurrDesc;  /* Next available resource 		 */		
	volatile SDMA_TX_DESC   *pTxUsedDesc;  /* Returning resource      		 */
	volatile SDMA_TX_DESC 	*pTxFirstDesc; /* An extra Tx index to support 	 */
										   /* Transmit of multiple buffers	 */
										   /* per packet 					 */ 
	/* Parameters for calculatung the next descriptors */
	volatile SDMA_RX_DESC	*pRxDescAreaBase;
	unsigned int  			rxDescAreaSize;

	volatile SDMA_TX_DESC	*pTxDescAreaBase;
	unsigned int  			txDescAreaSize;

    bool		  rxResourceErr;	/* Resource error flag - Rx queue full 	 */
    bool		  txResourceErr;	/* Resource error flag - Tx queue full   */
    

} SDMA_CHANNEL; 



/* sdma.h API list */
void sdmaChanInit  (SDMA_CHANNEL *sdmaChan);
void sdmaChanStart (SDMA_CHANNEL *sdmaChan);
void sdmaChanStopRx(SDMA_CHANNEL *sdmaChan);
void sdmaChanStopTx(SDMA_CHANNEL *sdmaChan);
void sdmaChanStopTxRx(SDMA_CHANNEL *sdmaChan);

SDMA_STATUS sdmaChanSend    (SDMA_CHANNEL *pSdmaChan, PKT_INFO *pPktInfo);
SDMA_STATUS sdmaTxReturnDesc(SDMA_CHANNEL *pSdmaChan, PKT_INFO *pPktInfo);
SDMA_STATUS sdmaChanReceive (SDMA_CHANNEL *pSdmaChan, PKT_INFO *pPktInfo);
SDMA_STATUS sdmaRxReturnBuff(SDMA_CHANNEL *pSdmaChan, PKT_INFO *pPktInfo);


#ifdef __cplusplus
}
#endif

#endif /* __INCsdmah */
