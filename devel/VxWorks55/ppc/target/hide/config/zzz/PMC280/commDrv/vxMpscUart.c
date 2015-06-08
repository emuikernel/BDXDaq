/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* vxMpscUart.c
*
* DESCRIPTION:
*		This file introduce UART library over the MPSC serial channel.
*
* DEPENDENCIES:
*		Cache library.
*		VxWorks SIO library.
*		MV serial channel library.
*
******************************************************************************/

/* includes */
#include "vxWorks.h"
#include "errno.h"
#include "sioLib.h"
#include "userRsrvMem.h"
#include "vxMpscUart.h"
#ifdef PMC280_DEBUG_UART_VX
#include "dbgprints.h"
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
#include "gtSmp.h"
#endif /* PMC280_DUAL_CPU */

#include "config.h"

#ifdef  INCLUDE_WINDVIEW
#include "private/funcBindP.h"
#include "private/eventP.h"
#include "wvLib.h"
#endif


/* defines */

/* locals   */
LOCAL STATUS vxMpscUartIoctl        (MV_SIO_CHAN *pChan,int request,int arg);
LOCAL void   vxMpscUartStartChannel (MV_SIO_CHAN *pChan);
LOCAL int    vxMpscUartPollOutput   (SIO_CHAN *,char);
LOCAL int    vxMpscUartPollInput    (SIO_CHAN *,char *);
LOCAL void   vxMpscUartStartup 	    (MV_SIO_CHAN *);
LOCAL int    vxMpscUartCallbackInstall (SIO_CHAN *, int, STATUS (*)(),void *);

/* local driver function table */

LOCAL SIO_DRV_FUNCS vxMpscUartSioDrvFuncs =
    {
    (int (*)()) vxMpscUartIoctl,
    (int (*)())	vxMpscUartStartup,
    (int (*)()) vxMpscUartCallbackInstall,
    (int (*)())	vxMpscUartPollInput,
    (int (*)(SIO_CHAN *,char)) vxMpscUartPollOutput
    };

/*******************************************************************************
* vxMpscUartDevInit - Initialize the UART channel.
*
* DESCRIPTION:
*	This routine fills most of the channel's data structure in order for
*	each of the channel components to be initiated properly. It also 
*	allocates memory areas for the driver operation in case the caller 
*	defined the Tx number of descriptors as zero.
*	This function initialize the various interrupt controllers.
*	NOTE: The driver allocates memory from the USER_RESERVED_MEM area in 
*		order to be able to operate the serial channel prior to VxWorks 
*		memory initialization.
*
* INPUT:
*       MV_SIO_CHAN     *pChan      MV serial channel struct.
*
* OUTPUT:
*	See description.
*
* RETURN:
*	None.
*
*******************************************************************************/
void vxMpscUartDevInit(MV_SIO_CHAN *pChan)
{
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering vxMpscUartDevInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering vxMpscUartDevInit\n");
    }
#else
    dbg_puts0("Entering vxMpscUartDevInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Set MPSC channel registers for UART operation */
    pChan->serialChan.mpscChan.mmcrl = MPSC_MMCRL_UART_VALUE;
    pChan->serialChan.mpscChan.mmcrh = MPSC_MMCRH_UART_VALUE;
    pChan->serialChan.mpscChan.mpcr  = MPSC_MPCR_UART_VALUE;
    pChan->serialChan.mpscChan.char1 = MPSC_CHAR1_UART_VALUE;
    pChan->serialChan.mpscChan.char2 = MPSC_CHAR2_UART_VALUE;
    pChan->serialChan.mpscChan.char3 = 0;
    pChan->serialChan.mpscChan.char4 = 0;
    pChan->serialChan.mpscChan.char5 = 0;
    pChan->serialChan.mpscChan.char6 = 0;
    pChan->serialChan.mpscChan.char7 = 0;
    pChan->serialChan.mpscChan.char8 = 0;
    pChan->serialChan.mpscChan.char9 = 0;
    	
    /* Set SDMA channel registers for UART operation*/
    pChan->serialChan.sdmaChan.sdmaConfig = SDMA_CONFIG_UART_VALUE;
    
    /* The driver uses the User Reserved Memory space for Tx buffer */
    userRsrvMemInit();

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed userRsrvMemInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed userRsrvMemInit\n");
    }
#else
    dbg_puts0("Passed userRsrvMemInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */


    /* Set channels memory parameters if no Tx descriptor assigned by user */
    if (pChan->serialChan.sdmaChan.txDescNum == 0)
    {
        /* The driver allocates the memory necessary itself */
        unsigned int rxBufBase, txDescBase, rxDescBase;
        unsigned int rxBufAreaSize, rxDescAreaSize, txDescAreaSize;
        
        pChan->serialChan.sdmaChan.txDescNum = DEFAULT_TX_DESC_NUM;
        pChan->serialChan.sdmaChan.rxDescNum = DEFAULT_RX_DESC_NUM;
        
        pChan->serialChan.sdmaChan.txBufSize = DEFAULT_TX_BUF_SIZE;
        pChan->serialChan.sdmaChan.rxBufSize = DEFAULT_RX_BUF_SIZE;

	rxBufAreaSize  = DEFAULT_RX_DESC_NUM * DEFAULT_RX_BUF_SIZE;
	rxDescAreaSize = (DEFAULT_RX_DESC_NUM + 1) * RX_DESC_ALIGNED_SIZE;
	txDescAreaSize = (DEFAULT_TX_DESC_NUM + 1) * TX_DESC_ALIGNED_SIZE;

        /* Use User Reserved Mem for Rx buffers and Tx/Rx descriptors */
	rxDescBase = userRsrvMemMallocAligned(rxDescAreaSize, 4);
        txDescBase = userRsrvMemMallocAligned(txDescAreaSize, 4);
        rxBufBase  = userRsrvMemMalloc(rxBufAreaSize);	
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
        /*
         * Note: 
         *     rxDescBase, txDescBase and rxBufBase are virtual addresses.
         */
        if(frcWhoAmI())
        {
            dbg_printf1("rx desc = 0x%08x tx desc = 0x%08x rx buf = 0x%08x\n", 
                         rxDescBase, txDescBase, rxBufBase);
        }
        else
        {
            dbg_printf0("rx desc = 0x%08x tx desc = 0x%08x rx buf = 0x%08x\n", 
                         rxDescBase, txDescBase, rxBufBase);
        }
#else
        dbg_printf0("rx desc = 0x%08x tx desc = 0x%08x rx buf = 0x%08x\n", 
                     rxDescBase, txDescBase, rxBufBase);

#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
		
	pChan->serialChan.sdmaChan.txDescBase = txDescBase;
        pChan->serialChan.sdmaChan.rxDescBase = rxDescBase;
        
        /* Do not allocate SDMA driver Tx buffers. Use UART driver Tx buffer */
        pChan->serialChan.sdmaChan.txBufBase = 0;
        pChan->serialChan.sdmaChan.rxBufBase = rxBufBase;
    }
	
    pChan->serialChan.baudRate  = pChan->baudRate;

    /* Each of the serial channel elements completes its SW struct init */
    serialChanInit(&pChan->serialChan);

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed serialChanInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed serialChanInit\n");
    }
#else
    dbg_puts0("Passed serialChanInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
	
    /* Initiate the Tx buffer memory area. Must maintain cache coherency */
    pChan->txBufBase = (char*)userRsrvMemMalloc(DEFAULT_TX_BUF_SIZE);

    pChan->channelMode = 0;
    pChan->pDrvFuncs = &vxMpscUartSioDrvFuncs;
	
    /* Clear and disable mpsc interrupts */
    vxMpscIntCntlInit();

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed vxMpscIntCntlInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed vxMpscIntCntlInit\n");
    }
#else
    dbg_puts0("Passed vxMpscIntCntlInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Clear and disable port interrupts */
    vxSdmaIntCtrlInit();

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed vxSdmaIntCntlInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed vxSdmaIntCntlInit\n");
    }
#else
    dbg_puts0("Passed vxSdmaIntCntlInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Clear and disable C-Unit interrupts */
    vxCunitIntCtrlInit();

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed vxCunitIntCntlInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed vxCunitIntCntlInit\n");
    }
#else
    dbg_puts0("Passed vxCunitIntCntlInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Reset the UART channel */
    vxMpscUartDevReset(pChan);

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting vxMpscUartDevInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting vxMpscUartDevInit\n");
    }
#else
    dbg_puts0("Exiting vxMpscUartDevInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */        
}

/*******************************************************************************
* vxMpscUartDevReset - reset the UART channel.
*
* DESCRIPTION:
*	This function resets the UART channel. It halts any of the channel 
*	Tx/Rx operation, clears and mask any pending interrupts.
*
* INPUT:
*       MV_SIO_CHAN     *pChan      MV serial channel struct.
*
* OUTPUT:
*	See description.
*
* RETURN:
*	None.
*
*******************************************************************************/
void vxMpscUartDevReset(MV_SIO_CHAN *pChan)
{
    int oldlevel = intLock ();	/* lock interrupts */ 

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering vxMpscUartDevReset\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering vxMpscUartDevReset\n");
    }
#else
    dbg_puts0("Entering vxMpscUartDevRest\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */


    /* Stop channel MPSC and SDMA machines */
    serialChanStop(&pChan->serialChan);

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed serialChanStop\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed serialChanStop\n");
    }
#else
    dbg_puts0("Passed serialChanStop\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Clear SDMA interrupts */
    vxSdmaIntClear(pChan->serialChan.sdmaChan.sdmaNum, SDMA_RX_BUF);
    vxSdmaIntClear(pChan->serialChan.sdmaChan.sdmaNum, SDMA_TX_BUF);

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed vxSdmaIntClear\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed vxSdmaIntClear\n");
    }
#else
    dbg_puts0("Passed vxSdmaIntClear\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Disable SDMA interrupts */
    vxSdmaIntDisable(pChan->serialChan.sdmaChan.sdmaNum, SDMA_RX_BUF);
    vxSdmaIntDisable(pChan->serialChan.sdmaChan.sdmaNum, SDMA_TX_BUF);

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed vxSdmaIntDisable\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed vxSdmaIntDisable\n");
    }
#else
    dbg_puts0("Passed vxSdmaIntDisable\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting vxMpscUartDevReset\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting vxMpscUartDevReset\n");
    }
#else
    dbg_puts0("Exiting vxMpscUartDevRest\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
}

/*******************************************************************************
* vxMpscUartStartChannel - Start the UART channel.
*
* DESCRIPTION:
*		This routine resets the serial channel and start it.
*
* INPUT:
*       MV_SIO_CHAN     *pChan      MV serial channel struct.
*
* OUTPUT:
*		The serial channel is ready to operate.
*
* RETURN:
*		None.
*
*******************************************************************************/
LOCAL void  vxMpscUartStartChannel(MV_SIO_CHAN *pChan)
{
    int oldlevel = intLock ();	/* lock interrupts */ 

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering vxMpscUartStartChannel\n");
        dbg_printf1("MPSC = %d\n", pChan->serialChan.mpscChan.mpscNum);
    }
    else
    {
        dbg_puts0("CPU0: Entering vxMpscUartStartChannel\n");
        dbg_printf0("MPSC = %d\n", pChan->serialChan.mpscChan.mpscNum);
    }
#else
    dbg_puts0("Entering vxMpscUartStartChannel\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    serialChanStop(&pChan->serialChan);

    serialChanStart(&pChan->serialChan);
    
    intUnlock (oldlevel);			/* UNLOCK INTERRUPTS */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting vxMpscUartStartChannel\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting vxMpscUartStartChannel\n");
    }
#else
    dbg_puts0("Exiting vxMpscUartStartChannel\n");



#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
}

/*******************************************************************************
* vxMpscUartIoctl - special device control

*
* DESCRIPTION:
*	This routine controls the device basic functionality like baud rate
*	and mode (polling or interrupt).
*
* INPUT:
*       *pChan  - MV serial channel struct, device to control.
*       request - request code      
*       arg     - some argument     
*
* OUTPUT:
*	See description.
*
* RETURN:
* 	OK on success, EIO on device error, ENOSYS on unsupported request.
*
*******************************************************************************/
LOCAL STATUS vxMpscUartIoctl(MV_SIO_CHAN *pChan, int request, int arg)
{
    int 	oldlevel;
    STATUS 	status = OK;

    switch (request)
	{
	    case SIO_BAUD_SET:
			if((serialChanSetBaudRate(&pChan->serialChan, arg) == false))
            {
                status = EIO;
                break;
            }
            pChan->baudRate = arg;
            
			break;
    
		case SIO_BAUD_GET:
			* (int *) arg = pChan->baudRate;
			break;

		case SIO_MODE_SET:
            
			if (!((int) arg == SIO_MODE_POLL || (int) arg == SIO_MODE_INT))
            {
                status = EIO;
                break;
            }

            /* Lock interrupt  */

            oldlevel = intLock();

            /* Initialize channel on first MODE_SET */

            if (!pChan->channelMode)
				vxMpscUartStartChannel(pChan);
			
            /*
             * If switching from POLL to INT mode, wait for all characters to
             * clear the output pins
             */


            if ((pChan->channelMode ==SIO_MODE_POLL)&&((int)arg ==SIO_MODE_INT))
				serialChanStop(&pChan->serialChan);
	  
            if ((int)arg == SIO_MODE_INT)
    		{                
                /* Clear SDMA interrupts */
                vxSdmaIntClear(pChan->serialChan.sdmaChan.sdmaNum, SDMA_RX_BUF);
                vxSdmaIntClear(pChan->serialChan.sdmaChan.sdmaNum, SDMA_TX_BUF);
                
                /* Enable interrupts */
                vxSdmaIntEnable(pChan->serialChan.sdmaChan.sdmaNum,SDMA_RX_BUF);
                vxSdmaIntEnable(pChan->serialChan.sdmaChan.sdmaNum,SDMA_TX_BUF);
    	    }
            else
		    {
                vxSdmaIntDisable(pChan->serialChan.sdmaChan.sdmaNum,SDMA_RX_BUF);
                vxSdmaIntDisable(pChan->serialChan.sdmaChan.sdmaNum,SDMA_TX_BUF);
            }

            pChan->channelMode = (int)arg;

            intUnlock(oldlevel);

            break;

        case SIO_MODE_GET:
            * (int *) arg = pChan->channelMode;
			break;

        case SIO_AVAIL_MODES_GET:
            *(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
			break;

		default:
			status = ENOSYS;
	}

    return (status);
}

/*******************************************************************************
* vxMpscUartRxInt - Receiver interrupt routine.
*
* DESCRIPTION:
*		This routine is called when there is an Rx packet ready to be 
*		processed. The data with in the Rx packet is then passed to the 
*		VxWorks IO buffers using the Rx callback routine.
*
* INPUT:
*       *pChan  - MV serial channel struct.
*
* OUTPUT:
*		Received data is transferred to the IO layer.
*
* RETURN:
*		None.
*
*******************************************************************************/
void vxMpscUartRxInt(MV_SIO_CHAN *pChan)
{
    char *pChar;
    PKT_INFO    pktInfo;
    
    /* process all filled receive buffers */
    while(sdmaChanReceive(&pChan->serialChan.sdmaChan, &pktInfo) == SDMA_OK)
    {
        /* Pass the packet up only if reception was Ok */
	if (pktInfo.cmdSts & (DCSW_ERROR_SUMMARY))
	{
		/* Assaf deal with error packet... */
		return;
	}                            
	pChar = (char*)pktInfo.bufPtr;

        while(pktInfo.byteCnt--)
           (*pChan->putRcvChar) (pChan->putRcvArg, *pChar++);
        
	/* Release Rx resources */
        sdmaRxReturnBuff(&pChan->serialChan.sdmaChan, &pktInfo);

        if (pChan->channelMode == SIO_MODE_POLL)
            break;

    }
}

/*******************************************************************************
* vxMpscUartTxInt - transmitter interrupt routine
*
* DESCRIPTION:
*       This routine transmits characters taken from the VxWorks IO buffer by  
*       Tx callback function. The characters are copied into the driver Tx 
*       buffer. This buffer and other information creates the packet 
*       information passed to the channel transmitting routine.
*       This routine continues the Tx process which started by  
*       vxMpscUartStartup () routine and ends when there are no more 
*		characters to transmit.
*
* INPUT:
*       *pChan  - MV serial channel struct.
*
* OUTPUT:
*       Calling the channel transmit routine with the proper packet information.
*
* RETURN:
*       None.
*
*******************************************************************************/
void vxMpscUartTxInt(MV_SIO_CHAN *pChan)
{
    char		    outChar;
	PKT_INFO 	    pktInfo;
    FAST UINT16 	dataLen = 0;
	
    if (pChan->channelMode == SIO_MODE_POLL)
        return;

	if(sdmaTxReturnDesc(&pChan->serialChan.sdmaChan, &pktInfo) != SDMA_OK)
		return;
    
    /* Check if a character needs to be output */

	if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK)	
	{
		do
	    {
			pChan->txBufBase[dataLen++] = outChar;
	    }
		while ((dataLen < pChan->serialChan.sdmaChan.txBufSize) &&
               ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK));

		/* Send transmit buffer */
		pktInfo.byteCnt = dataLen;
		pktInfo.cmdSts  = DCSW_FIRST_DESC    | 
                          DCSW_LAST_DESC     | 
                          DCSW_UART_PREAMBLE | 
                          DCSW_ENABLE_INTERRUPT;

		pktInfo.bufPtr  = (unsigned int)pChan->txBufBase;

		/* Send the packet using the SDMA channel */
		sdmaChanSend(&pChan->serialChan.sdmaChan, &pktInfo);
	}
}

/*******************************************************************************
* vxMpscUartStartup - transmitter startup routine
*
* DESCRIPTION:
*       This routine transmits characters taken from the VxWorks IO buffer by  
*       Tx callback function. The characters are copied into the driver Tx 
*       buffer. This buffer and other information creates the packet 
*       information passed to the channel transmitting routine.
*       This routine invokes Tx process which continues in vxMpscUartTxInt() 
*       and ends when there are no more characters to transmit.
*
* INPUT:
*       *pChan  - MV serial channel struct.
*
* OUTPUT:
*       Calling the channel transmit routine with the proper packet information.
*
* RETURN:
*       None.
*
*******************************************************************************/
LOCAL void vxMpscUartStartup(MV_SIO_CHAN *pChan)
{
    char		    outChar;
    PKT_INFO 	    pktInfo;
    FAST UINT16 	dataLen = 0;

    if (pChan->channelMode == SIO_MODE_POLL)
        return;

    /* Check if a character needs to be output */

	if ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK)	
	{
		do
	    {
			pChan->txBufBase[dataLen++] = outChar;
	    }
		while ((dataLen < pChan->serialChan.sdmaChan.txBufSize) &&
               ((*pChan->getTxChar) (pChan->getTxArg, &outChar) == OK));

		/* Send transmit buffer */
		pktInfo.byteCnt = dataLen;
		pktInfo.cmdSts  = DCSW_FIRST_DESC    | 
                          DCSW_LAST_DESC     | 
                          DCSW_UART_PREAMBLE | 
                          DCSW_ENABLE_INTERRUPT;

		pktInfo.bufPtr  = (unsigned int)pChan->txBufBase;

		/* Send the packet using the SDMA channel */
		sdmaChanSend(&pChan->serialChan.sdmaChan, &pktInfo);
	}
}

/*******************************************************************************
* vxMpscUartPollInput - input a character in polled mode.
*
* DESCRIPTION:
*       This function receives a character using the given serial channel.
*       The routine fills the given char buffer with the received char.
*
* INPUT:
*       *pSioChan - SIO channel struct.
*       *thisChar - Rx character buffer.
*
* OUTPUT:
*       Calling the channel receive routine with the proper packet information.
*
* RETURN:
*       OK if a character arrived, ERROR on device error, EAGAIN
*       if the output buffer if full.
*
*******************************************************************************/
LOCAL int vxMpscUartPollInput(SIO_CHAN *pSioChan, char *thisChar)
{

    PKT_INFO pktInfo;

    MV_SIO_CHAN * pChan = (MV_SIO_CHAN *) pSioChan;
    
    if(sdmaChanReceive(&pChan->serialChan.sdmaChan, &pktInfo) != SDMA_OK)
        return (EAGAIN);
    
    /* Pass the packet up only if reception was Ok */
    if (pktInfo.cmdSts & (DCSW_ERROR_SUMMARY))
    {
        return (EAGAIN);
    }                            

    *thisChar = *(char*)pktInfo.bufPtr;
    
    /* Release Rx resources */
    sdmaRxReturnBuff(&pChan->serialChan.sdmaChan, &pktInfo);
    return (OK);
}   

/*******************************************************************************
* vxMpscUartPollOutput - output a character in polled mode.
*
* DESCRIPTION:
*       This function transmits a character using the given serial channel.
*       The routine fills the packet information struct with the proper 
*		information for the given character.
*
* INPUT:
*       *pSioChan - SIO channel struct.
*       outChar   - The transmitted character.
*
* OUTPUT:
*       Calling the channel transmit routine with the proper packet information.
*
* RETURN:
*       OK if a character arrived, ERROR on device error, EAGAIN
*          if the output buffer if full.

*
*******************************************************************************/
LOCAL int  vxMpscUartPollOutput(SIO_CHAN *pSioChan, char outChar)
{
    PKT_INFO pktInfo;
    MV_SIO_CHAN *pChan = (MV_SIO_CHAN *) pSioChan;
	
	/* Is there a free buffer ?? */
	if(sdmaTxReturnDesc(&pChan->serialChan.sdmaChan, &pktInfo) != SDMA_OK)
        return (EAGAIN);
            
	*pChan->txBufBase = outChar;

    pktInfo.cmdSts  = DCSW_LAST_DESC | DCSW_FIRST_DESC | DCSW_UART_PREAMBLE;
    pktInfo.byteCnt = 0x1;
    pktInfo.bufPtr  = (unsigned int)pChan->txBufBase;
    
    /* Send the packet using the SDMA channel */
    sdmaChanSend(&pChan->serialChan.sdmaChan, &pktInfo);

    return (OK);
}

/*******************************************************************************
* vxMpscUartCallbackInstall - install ISR callbacks to get/put chars.
*
* DESCRIPTION:
*       This function installs ISR callbacks that transfers the data to and 
*       from the OS serial buffers.       
*
* INPUT:
*       *pSioChan      - SIO channel struct.
*       callbackType   - Rx or Tx type of callback routine.
*       (* callback)() - Callback routine.
*       *callbackArg   - Callback routine argument.
*
* OUTPUT:
*       Calling Rx callback will deliver the received data to the OS layer. 
*       Calling Tx callback will retrieve data from the OS layer.
*
* RETURN:
*       OK if callback installation succeeded, ENOSYS on wrong callbackType.
*
*******************************************************************************/
LOCAL int vxMpscUartCallbackInstall(SIO_CHAN *pSioChan, int	callbackType,
									STATUS (* callback)(), void *callbackArg)
{
    MV_SIO_CHAN * pChan = (MV_SIO_CHAN *) pSioChan;

    switch (callbackType)
    {
        case SIO_CALLBACK_GET_TX_CHAR:
            pChan->getTxChar    = callback;
            pChan->getTxArg     = callbackArg;
            return (OK);
	    break;

        case SIO_CALLBACK_PUT_RCV_CHAR:
            pChan->putRcvChar   = callback;
            pChan->putRcvArg    = callbackArg;
            return (OK);
	    break;

        default:
            return (ENOSYS);
    }
}
/******************************************************************************
* Baud Rate change
*******************************************************************************/

void gtUartBaudRateChange(int portNum,int baudRate) 
{
	
		
	switch (baudRate)
	{
	case 9600:
		if(portNum ==0)
			GT_REG_WRITE(BRG_CONFIG_REG(0),BAUD_RATE_9600);
		
		if(portNum ==1)
			GT_REG_WRITE(BRG_CONFIG_REG(1),BAUD_RATE_9600);
				 
		break;
	case 19200:
		if(portNum ==0)
			GT_REG_WRITE(BRG_CONFIG_REG(0),BAUD_RATE_19200);
		
		if(portNum ==1)
			GT_REG_WRITE(BRG_CONFIG_REG(1),BAUD_RATE_19200);
		
		break;
	case 38400:
		if(portNum ==0)
			GT_REG_WRITE(BRG_CONFIG_REG(0),BAUD_RATE_38400);
		
		if(portNum ==1)
			GT_REG_WRITE(BRG_CONFIG_REG(1),BAUD_RATE_38400);
		 
		break;

	case 57600:
		if(portNum ==0)
			GT_REG_WRITE(BRG_CONFIG_REG(0),BAUD_RATE_57600);
		
		if(portNum ==1)
			GT_REG_WRITE(BRG_CONFIG_REG(1),BAUD_RATE_57600);
		 
		break;
	case 115200:
                if(portNum ==0)
			GT_REG_WRITE(BRG_CONFIG_REG(0),BAUD_RATE_115200);
		
		if(portNum ==1)
			GT_REG_WRITE(BRG_CONFIG_REG(1),BAUD_RATE_115200);
		
		break;
	default:
		break;


	}


}

