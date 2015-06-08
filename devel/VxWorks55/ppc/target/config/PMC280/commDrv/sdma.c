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
* sdma.c
*
* DESCRIPTION:
*	This file introduce the SDMA channel library. The library API refer to 
*	the SDMA channel struct to extract information on the channel.
*	The module API refers to the SDMA channel operation and data flow.
*
*	Supported features:
*	- The driver practice zero copy where no data copying is done during 
*	  receive operation.
*	- The driver supports Tx Scatter-Gather where no copying done when 
*	  transmitting a packet that is spanned over multiple descriptors.
*       - Self data structure built and managment. The driver carves the Rx 
*         and Tx descriptor linked list (in a form of a ring) according to 
*         parameters passed by the SDMA struct.
*	- The driver is protocol oriented. It considers the protocols 
*	  restrictions for Tx buffer alignments and size.
*
*	The API for SDMA channel operation are:
*	sdmaChanInit 	 - SDMA channel initialize.
*	sdmaChanStart 	 - Start SDMA activity.
*	sdmaChanStopRx 	 - Stop SDMA Rx activity.
*	sdmaChanStopTx 	 - Stop SDMA Tx activity.
*	sdmaChanStopTxRx - Stop both Rx and Tx simultaneously.
*
*	The API for SDMA data flow:
*	sdmaChanSend 	 - Send a packet using the SDMA channel.
*	sdmaTxReturnDesc - Return Tx descriptor back to Tx ring.
*	sdmaChanReceive  - Get received information from Rx ring.
*	sdmaRxReturnBuff - Return Rx buffer back to Rx descriptors ring.
*
*	All data from and to the data flow API are done using the packet info 
*	struct which is a unified struct for the Tx and Rx descriptors.
*
*	The driver curves the Tx/Rx descriptor linked list in a form of a ring 
*	using the parameters in the SDMA channel struct. Those parameters are 
*	filled by the user prior to calling the sdmaChanInit() routine which 
*	initiates the drivers descriptors rings in memory. 
*	The driver's descriptors rings are management by indexes. Those indexes
*	controls the ring resources and used to indicate a SW resource error:
*	'current' 
*	This index points to the current available resource for use. For 
*	example in Rx process this will be the descriptor passed to the 
*	sdmaChanReceive caller containing received information from the 
*	MPSC channel. In Tx process, this will be the descriptor
*	that will be assigned with the user packet info and transmitted.
*	'used'    
*	This index points to the descriptor that need to restore its 
*	resources. For example in Rx process, using sdmaRxReturnBuff will
*	attach the buffer returned in packet info to the descriptor 
*	pointed by 'used'. In Tx process, using sdmaTxReturnDesc will 
*	merely return the user packet info with the command status of the 
*	transmitted buffer pointed by the 'used' index. Nevertheless, it 
*	is essential to use this routine to update the 'used' index.
*	'first'
*	This index supports Tx Scatter-Gather. It points to the first 
*	descriptor of a packet assembled of multiple buffers. For example 
*	when in middle of such packet we have a Tx resource error the 
*	'curr' index get the value of 'first' to indicate that the ring 
*	returned to its state before trying to transmit this packet.
*		
*	Receive operation:
*	The sdmaChanReceive API return the caller the packet information struct 
*	pointer describing the current SDMA descriptor containing the received 
*	information. It is the user responsibility to return this resource back 
*	to the Rx descriptor ring to enable the reuse of this source. Return 
*	Rx resource is done using the sdmaRxReturnBuff API.
*
*	Transmit operation:
*	The sdmaChanSend API supports Scatter-Gather which enables to send a 
*	packet spanned over multiple buffers. This means that for each 
*	packet info structure given by the user and put into the Tx descriptors 
*	ring, will be transmitted only if the 'LAST' bit will be set in the 
*	packet info command status field. This API also consider the protocol 
*	used and its restriction regarding buffer alignments and sizes.
*	The user must return a Tx resource after ensuring the buffer has been 
*	transmitted to enable the Tx ring indexes to update.
*
* DEPENDENCIES:
*		MV General driver.
*		MPSC header
*
******************************************************************************/

/* includes */
#include "gtCore.h"
#include "sdma.h"   
#include "mpsc.h"   

#include "logLib.h"

/* defines */
/* SDMA command macros */
#define SDMA_ENABLE_TX(sdmaChan)                     		\
    GT_REG_WRITE(SDMA_COMMAND_REG(sdmaChan), SDCM_TXD_TX_DEMAND) 	
	
#define SDMA_DISABLE_TX(sdmaChan)                     		\
    GT_REG_WRITE(SDMA_COMMAND_REG(sdmaChan), SDCM_STD_STOP_TX_DMA) 

#define SDMA_ENABLE_RX(sdmaChan)                     		\
    GT_REG_WRITE(SDMA_COMMAND_REG(sdmaChan), SDCM_ERD_ENABLE_RX_DMA) 	

#define SDMA_DISABLE_RX(sdmaChan)                     		\
    GT_REG_WRITE(SDMA_COMMAND_REG(sdmaChan), SDCM_AR_ABORT_RX) 


#define CURR_RFD_GET(pCurrDesc)									\
    ((pCurrDesc) = sdmaChan->pRxCurrDesc)

#define CURR_RFD_SET(pCurrDesc)									\
    (sdmaChan->pRxCurrDesc = (pCurrDesc))

#define USED_RFD_GET(pUsedDesc)									\
    ((pUsedDesc) = sdmaChan->pRxUsedDesc)

#define USED_RFD_SET(pUsedDesc)									\
    (sdmaChan->pRxUsedDesc = (pUsedDesc))


#define CURR_TFD_GET(pCurrDesc)									\
    ((pCurrDesc) = sdmaChan->pTxCurrDesc)

#define CURR_TFD_SET(pCurrDesc)									\
    (sdmaChan->pTxCurrDesc = (pCurrDesc))
	
#define USED_TFD_GET(pUsedDesc)									\
    ((pUsedDesc) = sdmaChan->pTxUsedDesc)
	
#define USED_TFD_SET(pUsedDesc)									\
    (sdmaChan->pTxUsedDesc = (pUsedDesc))

#define FIRST_TFD_GET(pFirstDesc)								\
    ((pFirstDesc) = sdmaChan->pTxFirstDesc)
	
#define FIRST_TFD_SET(pFirstDesc)								\
    (sdmaChan->pTxFirstDesc = (pFirstDesc))

/* Macros that save access to desc in order to find next desc pointer  */
#define RX_NEXT_DESC_PTR(pRxDesc)											   \
	(SDMA_RX_DESC*)(((((unsigned int)pRxDesc - 								   \
			(unsigned int)sdmaChan->pRxDescAreaBase) + RX_DESC_ALIGNED_SIZE) % \
			sdmaChan->rxDescAreaSize) + (unsigned int)sdmaChan->pRxDescAreaBase)

#define TX_NEXT_DESC_PTR(pTxDesc)											   \
	(SDMA_TX_DESC*)(((((unsigned int)pTxDesc - 								   \
			(unsigned int)sdmaChan->pTxDescAreaBase) + TX_DESC_ALIGNED_SIZE) % \
			sdmaChan->txDescAreaSize) + (unsigned int)sdmaChan->pTxDescAreaBase)

/* locals */
static bool sdmaInitRxDescRing(SDMA_CHANNEL *sdmaChan, 
			 				   int 			rxDescNum,
							   int 			rxBuffSize,
							   unsigned int rxDescBaseAddr,
							   unsigned int rxBuffBaseAddr);

static bool sdmaInitTxDescRing(SDMA_CHANNEL *sdmaChan,
							   int 			txDescNum,
							   int 			txBuffSize,
							   unsigned int txDescBaseAddr,
							   unsigned int txBuffBaseAddr);

void sdmaDbg(SDMA_CHANNEL *sdmaChan);

/*******************************************************************************
* sdmaChanInit - SDMA channel initialize
*
* DESCRIPTION:
*	This function completes the SDMA channel struct initialization and 
*	carves the Rx and Tx descriptor/buffers data structures in memory 
*	according to user parameters passed by the SDMA channel struct.
*
* INPUT:
*       SDMA_CHANNEL 	*sdmaChan       SDMA channel struct
*
* OUTPUT:
*	SDMA channel struct initialized and SDMA channel descriptors are ready.
*
* RETURN:
*	None.
*
*******************************************************************************/
void sdmaChanInit(SDMA_CHANNEL *sdmaChan)
{
    sdmaChan->rxResourceErr = false;
    sdmaChan->txResourceErr = false;

    /* Zero out Rx ring SW structs */
    CURR_RFD_SET (NULL);
    USED_RFD_SET (NULL);
	
    /* Zero out Tx ring SW structs */
    CURR_TFD_SET (NULL);
    USED_TFD_SET (NULL);
    FIRST_TFD_SET(NULL);

    /* Init Tx descriptors ring */
    sdmaInitTxDescRing(sdmaChan, sdmaChan->txDescNum,  sdmaChan->txBufSize,
                       sdmaChan->txDescBase, sdmaChan->txBufBase);
    
    /* Init Rx descriptors ring */
    sdmaInitRxDescRing(sdmaChan, sdmaChan->rxDescNum,  sdmaChan->rxBufSize,
                       sdmaChan->rxDescBase, sdmaChan->rxBufBase);
        
    return;
} 

/*******************************************************************************
* sdmaChanStart - Start SDMA activity.
*
* DESCRIPTION:
*       This function start the SDMA activity. The routine assigns the SDMA 
*	struct values to the SDMA channel registers and enables the Rx.
*
* INPUT:
*       SDMA_CHANNEL 	*sdmaChan       SDMA channel struct
*
* OUTPUT:
*       SDMA channel register are initiated and Rx is enabled.
*
* RETURN:
*       None.
*
*******************************************************************************/
void sdmaChanStart(SDMA_CHANNEL *sdmaChan)
{
    volatile SDMA_TX_DESC *pTxCurrDesc;
    volatile SDMA_RX_DESC *pRxCurrDesc;
	
    /* Assignment of Tx CTDP and FTDP */
    CURR_TFD_GET(pTxCurrDesc);

    GT_REG_WRITE(SDMA_CURRENT_TX_DESCRIPTOR_POINTER(sdmaChan->sdmaNum), 
                                                    (unsigned int)pTxCurrDesc);
    GT_REG_WRITE(SDMA_FIRST_TX_DESCRIPTOR_POINTER(sdmaChan->sdmaNum), 
                                                    (unsigned int)pTxCurrDesc);
									
    /* Assignment of Rx CRDP of given queue */
    CURR_RFD_GET(pRxCurrDesc);

    GT_REG_WRITE(SDMA_CURRENT_RX_DESCRIPTOR_POINTER(sdmaChan->sdmaNum), 
                                                    (unsigned int)pRxCurrDesc);

    /* Assign SDMA channel configuration */
    GT_REG_WRITE(SDMA_CONFIG_REG(sdmaChan->sdmaNum), sdmaChan->sdmaConfig);

    /* Enable SDMA channel Rx */
    GT_REG_WRITE(SDMA_COMMAND_REG(sdmaChan->sdmaNum), SDCM_ERD_ENABLE_RX_DMA);
    
    return;
}

/*******************************************************************************
* sdmaDbg - Display SDMA struct.
*
* DESCRIPTION:
*       None.
*
* INPUT:
*       SDMA_CHANNEL 	*sdmaChan       SDMA channel struct
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
void sdmaDbg(SDMA_CHANNEL *sdmaChan)
{
    volatile SDMA_TX_DESC *pTxCurrDesc;
    volatile SDMA_RX_DESC *pRxCurrDesc;

    logMsg("SDMA channel info:\n",0,0,0,0,0,0);
    logMsg("sdmaNum = 0x%x\n",	(unsigned int)sdmaChan->sdmaNum,0,0,0,0,0);
	
    USED_RFD_GET (pRxCurrDesc);
    logMsg("pRxQueue.usedDescPtr = 0x%x\n",(unsigned int)pRxCurrDesc,0,0,0,0,0);
        
    CURR_RFD_GET (pRxCurrDesc);
    logMsg("pRxQueue.currDescPtr = 0x%x\n",(unsigned int)pRxCurrDesc,0,0,0,0,0);

    USED_TFD_GET (pTxCurrDesc);
    logMsg("pTxQueue.usedDescPtr = 0x%x\n",(unsigned int)pTxCurrDesc,0,0,0,0,0);
        
    CURR_TFD_GET (pTxCurrDesc);
    logMsg("pTxQueue.currDescPtr = 0x%x\n",(unsigned int)pTxCurrDesc,0,0,0,0,0);   

    logMsg("rxBufBase = 0x%x\n",sdmaChan->rxBufBase,0,0,0,0,0); 
    logMsg("txBufBase = 0x%x\n",sdmaChan->txBufBase,0,0,0,0,0); 
	
    logMsg("sdmaChan->rxResourceErr= 0x%x\n",sdmaChan->rxResourceErr,0,0,0,0,0); 
    logMsg("sdmaChan->txResourceErr= 0x%x\n",sdmaChan->txResourceErr,0,0,0,0,0); 
}


/*******************************************************************************
* sdmaChanStopRx - Stop SDMA Rx activity.
*
* DESCRIPTION:
*       This function halts SDMA Rx activity.
*
* INPUT:
*       SDMA_CHANNEL 	*sdmaChan       SDMA channel struct
*
* OUTPUT:
*       If Rx enabled is set in SDMA command register, an abort Rx command is 
*       issued and register is polled until the abort command is complete.
*
* RETURN:
*       None.
*
*******************************************************************************/
void sdmaChanStopRx(SDMA_CHANNEL *sdmaChan)
{
    unsigned int  	regData;
	
    /* Is Rx SDMA enabled ?? */
    GT_REG_READ(SDMA_COMMAND_REG(sdmaChan->sdmaNum), &regData);

    /* check that the Rx enabled bit is set */
    if(regData & SDCM_ERD_ENABLE_RX_DMA)
    {
	/* Abort SDMA Rx machine */
	GT_REG_WRITE(SDMA_COMMAND_REG(sdmaChan->sdmaNum), SDCM_AR_ABORT_RX);

        do
        {
	    GT_REG_READ(SDMA_COMMAND_REG(sdmaChan->sdmaNum), &regData);
	}
	while(regData & SDCM_AR_ABORT_RX);
     }
		
     return;
}

/*******************************************************************************
* sdmaChanStopTx - Stop SDMA Tx activity.
*
* DESCRIPTION:
*       This function halts SDMA Tx activity.
*
* INPUT:
*       SDMA_CHANNEL 	*sdmaChan       SDMA channel struct
*
* OUTPUT:
*       If Tx demand is set in SDMA command register, a stop Tx command is 
*       issued and register is polled until the stop command is complete.
*
* RETURN:
*       None.
*
*******************************************************************************/
void sdmaChanStopTx(SDMA_CHANNEL *sdmaChan)
{
    unsigned int regData;
	
    GT_REG_READ(SDMA_COMMAND_REG(sdmaChan->sdmaNum), &regData);

    /* check that the Tx demand bit is set */
    if(regData & SDCM_TXD_TX_DEMAND)
    {
        /* Stop SDMA Tx machine */
        GT_REG_WRITE(SDMA_COMMAND_REG(sdmaChan->sdmaNum), SDCM_STD_STOP_TX_DMA);
	
        do
        {
	    GT_REG_READ(SDMA_COMMAND_REG(sdmaChan->sdmaNum), &regData);
        }
	while(regData & SDCM_STD_STOP_TX_DMA);
    }
    return;
}

/*******************************************************************************
* sdmaChanStopTxRx - Stop both Rx and Tx simultaneously
*
* DESCRIPTION:
*       This routine halts both Rx and Tx activities of a given SDMA channel.
*       Note: This code supports stoping machine as mentioned in the 
*	workaround for the Errata "Address decoding error in Communication
*	Unit".
*
* INPUT:
*       SDMA_CHANNEL 	*sdmaChan       SDMA channel struct
*
* OUTPUT:
*       Rx and Tx activity are stopped.
*
* RETURN:
*       None.
*
*******************************************************************************/
void sdmaChanStopTxRx(SDMA_CHANNEL *sdmaChan)
{
    unsigned int regData;
	
    /* Abort SDMA Rx and Tx machine in the SDMA command register */
    GT_REG_WRITE(SDMA_COMMAND_REG(sdmaChan->sdmaNum), (SDCM_AR_ABORT_RX | SDCM_AT_ABORT_TX));

    do
    {
        GT_REG_READ(SDMA_COMMAND_REG(sdmaChan->sdmaNum),&regData);
    }
    while(regData & (SDCM_ERD_ENABLE_RX_DMA | SDCM_TXD_TX_DEMAND));

    return;
}

/*******************************************************************************
* sdmaInitRxDescRing - Curve a Rx chain desc list and buffer in memory.
*
* DESCRIPTION:
*       This function prepares a Rx chained list of descriptors and packet 
*       buffers in a form of a ring.
* 
* INPUT:
*       SDMA_CHANNEL 	*sdmaChan       SDMA channel struct
*       int 			rxDescNum       Number of Rx descriptors
*       int 			rxBuffSize      Size of Rx buffer
*       unsigned int    rxDescBaseAddr  Rx descriptors memory area base addr.
*       unsigned int    rxBuffBaseAddr  Rx buffer memory area base addr.
*
* OUTPUT:
*       The routine updates the SDMA channel struct with the information 
*       regarding the Rx descriptors and buffers.
*
* RETURN:
*       false if the given descriptors memory area is not aligned according to
*       SDMA specifications.
*       true otherwise.
*
*******************************************************************************/
bool sdmaInitRxDescRing(SDMA_CHANNEL 	*sdmaChan, 
                        int rxDescNum,
                        int rxBuffSize,
                        unsigned int rxDescBaseAddr,
                        unsigned int rxBuffBaseAddr)
{
    SDMA_RX_DESC  *pRxDesc;
    SDMA_RX_DESC  *pRxPrevDesc; /* pointer to link with the last descriptor */
    unsigned int bufferAddr; 
    int	ix;				/* a counter */

    /* Rx desc Must be 4LW aligned (i.e. Descriptor_Address[3:0]=0000). */
    if (rxDescBaseAddr & 0xF) 
	return false; 

    pRxDesc		 = (SDMA_RX_DESC*)rxDescBaseAddr;
    pRxPrevDesc	 = pRxDesc;
    bufferAddr   = rxBuffBaseAddr;

    /* initialize the Rx Desc ring */
    for (ix = 0; ix < rxDescNum; ix++)
    {
        pRxDesc->bufSize     = rxBuffSize;
        pRxDesc->byteCnt     = 0x0000;
        pRxDesc->cmdSts      = DCSW_BUFFER_OWNED_BY_DMA | DCSW_ENABLE_INTERRUPT;
        pRxDesc->nextDescPtr = (unsigned int) (pRxDesc) + RX_DESC_ALIGNED_SIZE;
        pRxDesc->bufPtr      = bufferAddr;
        pRxDesc->returnInfo  = 0x00000000;
	bufferAddr += rxBuffSize;
        pRxPrevDesc = pRxDesc;
	pRxDesc=(SDMA_RX_DESC*)((unsigned int)(pRxDesc) + RX_DESC_ALIGNED_SIZE);
    }
     
    /* Closing Rx descriptors ring */
    pRxPrevDesc->nextDescPtr = rxDescBaseAddr;

    /* Save Rx desc pointer to driver struct. */
    CURR_RFD_SET ((SDMA_RX_DESC*)rxDescBaseAddr);
    USED_RFD_SET ((SDMA_RX_DESC*)rxDescBaseAddr);
	
    sdmaChan->pRxDescAreaBase = (SDMA_RX_DESC*)rxDescBaseAddr;
    sdmaChan->rxDescAreaSize  = rxDescNum * RX_DESC_ALIGNED_SIZE;

    return true;
}
/*******************************************************************************
* sdmaInitTxDescRing - Curve a Tx chain desc list and buffer in memory.
*
* DESCRIPTION:
*       This function prepares a Tx chained list of descriptors and packet 
*       buffers in a form of a ring.
*
* INPUT:
*       SDMA_CHANNEL 	*sdmaChan       SDMA channel struct
*       int 			txDescNum       Number of Tx descriptors
*       int 			txBuffSize      Size of Tx buffer
*       unsigned int    txDescBaseAddr  Tx descriptors memory area base addr.
*       unsigned int    txBuffBaseAddr  Tx buffer memory area base addr.
*
* OUTPUT:
*       The routine updates the SDMA channel struct with the information 
*       regarding the Tx descriptors and buffers.
*
* RETURN:
*       false if the given descriptors memory area is not aligned according to
*       SDMA specifications.
*       true otherwise.
*
*******************************************************************************/
bool sdmaInitTxDescRing(SDMA_CHANNEL	*sdmaChan,
                        int txDescNum,
		        int txBuffSize,
		        unsigned int txDescBaseAddr,
		        unsigned int txBuffBaseAddr)
{

    SDMA_TX_DESC  *pTxDesc;
    SDMA_TX_DESC   *pTxPrevDesc;
    unsigned int  bufferAddr; 
    int	 ix;				/* a counter */

    /* Tx desc Must be 4LW aligned (i.e. Descriptor_Address[3:0]=0000). */
    if (txDescBaseAddr & 0xF) 
	return false; 

    /* save the first desc pointer to link with the last descriptor */
    pTxDesc	 = (SDMA_TX_DESC*)txDescBaseAddr;
    pTxPrevDesc	 = pTxDesc;
    bufferAddr   = txBuffBaseAddr;

    /* Initialize the Tx descriptors ring */
    for (ix = 0; ix < txDescNum; ix++)
    {
        pTxDesc->byteCnt     = 0x0000;
        pTxDesc->shadowBc    = 0x0000;
        pTxDesc->cmdSts      = 0x00000000;
        pTxDesc->nextDescPtr = (unsigned int)(pTxDesc) + TX_DESC_ALIGNED_SIZE;
        pTxDesc->bufPtr      = bufferAddr;
        pTxDesc->returnInfo  = 0x00000000;
	bufferAddr += txBuffSize;
        pTxPrevDesc = pTxDesc;
	pTxDesc=(SDMA_TX_DESC*)((unsigned int)(pTxDesc) + TX_DESC_ALIGNED_SIZE);
    }    

    /* Closing Tx descriptors ring */
    pTxPrevDesc->nextDescPtr = txDescBaseAddr;

    /* Set Tx desc pointer in driver struct. */
    CURR_TFD_SET ((SDMA_TX_DESC*)txDescBaseAddr);
    USED_TFD_SET ((SDMA_TX_DESC*)txDescBaseAddr);
    
    sdmaChan->pTxDescAreaBase = (SDMA_TX_DESC*)txDescBaseAddr;
    sdmaChan->txDescAreaSize  = (txDescNum * TX_DESC_ALIGNED_SIZE);

    return true;
}

/*******************************************************************************
* sdmaChanSend - Send a packet using the SDMA channel
*
* DESCRIPTION:
*	This routine send a given packet described by pPktinfo parameter. It 
*       support a packet span over multi buffers. The routine updates 'curr'
*       and 'first' indexes according to the descriptor given. In case the 
*       descriptor is "first", the 'first' index is update. In any case, the 
*       'curr' index is updated. If the routine get into Tx resource error it
*       assigns 'curr' index as 'first'. This way the function can abort Tx 
*       process of multiple descriptors per packet.
*		
*
* INPUT:
*	SDMA_CHANNEL    *sdmaChan   SDMA channel struct pointer
*    PKT_INFO        *pPktInfo   Packet struct pointer.
*
* OUTPUT:
*	Tx ring 'curr' and 'first' indexes are updated. 
*
* RETURN:
*       SDMA_QUEUE_FULL in case of Tx resource error.
*	SDMA_ERROR in case the routine can not access Tx desc ring.
*	SDMA_QUEUE_LAST_RESOURCE if the routine uses the last Tx resource.
*       SDMA_OK otherwise.
*
*******************************************************************************/
SDMA_STATUS sdmaChanSend(SDMA_CHANNEL *sdmaChan, PKT_INFO *pPktInfo)
{                                                                                       
    volatile SDMA_TX_DESC *pTxDescCurr;
    volatile SDMA_TX_DESC *pTxDescFirst;
    volatile SDMA_TX_DESC *pTxNextDescCurr;
    volatile SDMA_TX_DESC *pTxDescUsed;
    unsigned int pBuff;
    unsigned int commandStatus = 0;
     
    if(sdmaChan->txResourceErr == true)
	return SDMA_QUEUE_FULL;

    /* Get the Tx Desc ring indexes */
    CURR_TFD_GET (pTxDescCurr);
    USED_TFD_GET (pTxDescUsed);

    /* Sanity check */
    if(pTxDescCurr == NULL)
        return SDMA_ERROR;
	
    /* The following parameters are used to save readings from memory */
    pTxNextDescCurr = TX_NEXT_DESC_PTR(pTxDescCurr);
    commandStatus   = pPktInfo->cmdSts;

    if (pPktInfo->cmdSts & (DCSW_FIRST_DESC))
    {
	FIRST_TFD_SET(pTxDescCurr);
	pTxDescFirst = pTxDescCurr;
    }
    else
    {
        FIRST_TFD_GET(pTxDescFirst);
        commandStatus |= DCSW_BUFFER_OWNED_BY_DMA;
    }

    pTxDescCurr->byteCnt    = pPktInfo->byteCnt;
    pTxDescCurr->returnInfo = pPktInfo->returnInfo; /* Release information */
    pBuff  = pPktInfo->bufPtr;
    
    if(sdmaChan->protType == MMCRL_MODE_HDLC)
    {
	/* Buffers with a payload smaller than 8 bytes must be aligned    */
        /* to 64-bit boundary for HDLC and Transparent. We use the memory */
	/* allocated for the Tx descriptor. This memory located in 	  */
	/* TX_BUF_OFFSET_IN_DESC offset within the Tx descriptor.  	  */
		
	if(pPktInfo->byteCnt <= SDMA_TX_BUFFER_MIN_SIZE)
	{
	unsigned int destBufPtr;
	unsigned int zeroPadCnt=SDMA_TX_BUFFER_MIN_SIZE - pPktInfo->byteCnt;
	
	destBufPtr 	= (unsigned int)pTxDescCurr + TX_BUF_OFFSET_IN_DESC;
	pBuff 		= destBufPtr;
		
	while(pPktInfo->byteCnt != 0)
	{
		*(char*)destBufPtr = *(char*)pPktInfo->bufPtr;
		destBufPtr++;
		pPktInfo->bufPtr++;
		pPktInfo->byteCnt--;
	}
	while(zeroPadCnt != 0)
	{
		*(char*)destBufPtr = 0;
		destBufPtr++;
		zeroPadCnt--;
	}
        }
    }
    pTxDescCurr->bufPtr = pBuff;

    if (pPktInfo->cmdSts & (DCSW_LAST_DESC))
    {        
        pTxDescCurr->cmdSts = commandStatus             | 
                              DCSW_BUFFER_OWNED_BY_DMA  | 
                              DCSW_ENABLE_INTERRUPT;

	if(pTxDescCurr != pTxDescFirst)
		pTxDescFirst->cmdSts |= DCSW_BUFFER_OWNED_BY_DMA;

	/* Flush CPU pipe */
	__asm__ __volatile__("eieio");
	    
        /* Give send command */
        SDMA_ENABLE_TX(sdmaChan->sdmaNum);
    }
    else
        pTxDescCurr->cmdSts	= commandStatus;
    
    /* Update the current descriptor */
    CURR_TFD_SET(pTxNextDescCurr);
	
    /* Check for Tx resource error */
    if(pTxNextDescCurr == pTxDescUsed)
    {
	/* Update the current descriptor */
	CURR_TFD_SET(pTxDescFirst);
		
	sdmaChan->txResourceErr = true;

	return SDMA_QUEUE_LAST_RESOURCE; 
    }
    else
	return SDMA_OK;
}

/*******************************************************************************
* sdmaTxReturnDesc - Return Tx descriptor back to Tx ring.
*
* DESCRIPTION:
*	This routine returns the transmitted packet information to the caller.
*       It updates the 'used' Tx ring index.
*       In case the Tx queue was in "resource error" condition, where there are 
*       no available Tx resources, the function resets the resource error flag.
* 
* INPUT:
*	SDMA_CHANNEL    *sdmaChan   SDMA channel struct pointer
*    PKT_INFO        *pPktInfo   Packet struct pointer.
*
* OUTPUT:
*	Tx ring current and used indexes are updated. 
* 
* RETURN:
*	SDMA_ERROR in case the routine can not access Tx desc ring.
*       SDMA_RETRY in case the routine could not release descriptor.
*	SDMA_END_OF_JOB if the routine has nothing to release.
*       SDMA_OK otherwise.
*
*******************************************************************************/
SDMA_STATUS sdmaTxReturnDesc(SDMA_CHANNEL *sdmaChan, PKT_INFO *pPktInfo)
{
    volatile SDMA_TX_DESC*	pTxDescUsed = NULL;
    volatile SDMA_TX_DESC*  pTxDescCurr = NULL;
    unsigned int commandStatus;

    /* Get the Tx Desc ring indexes */
    USED_TFD_GET (pTxDescUsed);
    CURR_TFD_GET (pTxDescCurr);

    /* Sanity check */
    if(pTxDescUsed == NULL)
	return SDMA_ERROR;
	
    commandStatus = pTxDescUsed->cmdSts;
	
    /* Still transmitting... */
    if (commandStatus & (DCSW_BUFFER_OWNED_BY_DMA))
	return SDMA_RETRY;
    
    /* Stop release. About to overlap the current available Tx descriptor */		
    if((pTxDescUsed == pTxDescCurr) && (sdmaChan->txResourceErr == false))
		return SDMA_OK;
	
    /* Move out of resource error after resource return */
    if(sdmaChan->txResourceErr == true)
	sdmaChan->txResourceErr = false;
	
    /* Pass the packet status to the caller */
    pPktInfo->cmdSts      = commandStatus;
    pPktInfo->returnInfo  = pTxDescUsed->returnInfo;

    /* Update the next descriptor to release. */
    USED_TFD_SET(TX_NEXT_DESC_PTR(pTxDescUsed));
    
    return SDMA_OK;
}

/*******************************************************************************
* sdmaChanReceive - Get received information from Rx ring.
* 
* DESCRIPTION:
* 	This routine returns the received data to the caller. There is no 
*	data copying during routine operation. All information is returned 
*	using pointer to packet information struct. If the routine exhausts
*	the Rx ring resources the resource error flag is set.  
*
* INPUT:
*    SDMA_CHANNEL    *sdmaChan   SDMA channel struct pointer
*    PKT_INFO        *pPktInfo   Packet struct pointer.
*
* OUTPUT:
*	Rx ring current and used indexes are updated. 
*
* RETURN:
*	SDMA_ERROR in case the routine can not access Rx desc ring.
*	SDMA_QUEUE_FULL if Rx ring resources are exhausted.
*	SDMA_END_OF_JOB if there is no received data.
*       SDMA_OK otherwise.
*
*******************************************************************************/
SDMA_STATUS sdmaChanReceive(SDMA_CHANNEL *sdmaChan, PKT_INFO *pPktInfo)
{
    volatile SDMA_RX_DESC *pRxCurrDesc;
    volatile SDMA_RX_DESC *pRxNextCurrDesc;
    volatile SDMA_RX_DESC *pRxUsedDesc;
    unsigned int commandStatus;

    /* Is the Rx ring out of resources */
    if(sdmaChan->rxResourceErr == true)
	return SDMA_QUEUE_FULL;

    /* Get the Rx Desc ring indexes */
    CURR_RFD_GET (pRxCurrDesc);	
    USED_RFD_GET (pRxUsedDesc);

    /* Sanity check */
    if (pRxCurrDesc == NULL)
	return SDMA_ERROR;
	
    /* The following parameters are used to save readings from memory */
    pRxNextCurrDesc = RX_NEXT_DESC_PTR(pRxCurrDesc);
    commandStatus	= pRxCurrDesc->cmdSts;

    /* Nothing to receive... */
    if (commandStatus & (DCSW_BUFFER_OWNED_BY_DMA))
	return SDMA_END_OF_JOB;

    pPktInfo->byteCnt     = pRxCurrDesc->byteCnt;
    pPktInfo->cmdSts      = commandStatus;
    pPktInfo->bufPtr 	  = pRxCurrDesc->bufPtr;

    /* Update new available Rx descriptor in data structure */
    CURR_RFD_SET(pRxNextCurrDesc);

    /* Rx resource exhaust. The current Rx desc about to overlap used desc. */
    if (pRxNextCurrDesc == pRxUsedDesc)
	sdmaChan->rxResourceErr = true;

    return SDMA_OK;
}

/*******************************************************************************
* sdmaRxReturnBuff - Return Rx buffer back to Rx descriptors ring.
*
* DESCRIPTION:
*	This routine returns a Rx buffer back to the Rx ring. It retrieves the 
*       next 'used' descriptor and attached the returned buffer to it.
*       In case the Rx queue was in "resource error" condition, where there are 
*       no available Rx resources, the function resets the resource error flag.
*
* INPUT:
*	SDMA_CHANNEL    *sdmaChan   SDMA channel struct pointer
*    PKT_INFO        *pPktInfo   Packet struct pointer.
*
* OUTPUT:
*	New available Rx resource in Rx descriptor ring.
*
* RETURN:
*	SDMA_ERROR in case the routine can not access Rx desc ring.
*       SDMA_OK otherwise.
*
*******************************************************************************/
SDMA_STATUS sdmaRxReturnBuff(SDMA_CHANNEL *sdmaChan, PKT_INFO *pPktInfo)
{
    volatile SDMA_RX_DESC *pUsedRxDesc;	/* Pointer to a Rx descriptor */
            
    /* Get the Rx desc from which to start to return buffers */
    USED_RFD_GET(pUsedRxDesc);

    /* Sanity check */
    if (pUsedRxDesc == NULL)
	return SDMA_ERROR;

    /* Link the returned buffer with the available Rx descriptor*/
    pUsedRxDesc->bufPtr = pPktInfo->bufPtr;
    
    /* Flush CPU pipe */
    __asm__ __volatile__ ("eieio");
	
    /* Return the descriptor ownership to the MV device */
    pUsedRxDesc->cmdSts = DCSW_BUFFER_OWNED_BY_DMA | DCSW_ENABLE_INTERRUPT;
	
    /* Move the used descriptor pointer to the next descriptor */
    USED_RFD_SET(RX_NEXT_DESC_PTR(pUsedRxDesc));
    
    /* Move out of resource error after resource return */
    if(sdmaChan->rxResourceErr == true)
	sdmaChan->rxResourceErr = false;

    return SDMA_OK;
}
