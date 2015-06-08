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
* serialChan.c
*
* DESCRIPTION:
*		This file introduce a virtual MV device serial channel. The channel 
*		is consist of three sub modules:
*		1) MPSC Channel driver.
*		2) SDMA Channel driver.
*		3) BRG engine driver.
*		This driver API only introduce API that controls the MV serial channel 
*		operation:
*		- Channel initiate. This API activates each sub module init routine
*			which merely initiate its SW structures.  
*		- Channel start. This API prepare the channel to start processing its 
*			data. The API activates each of the modules start routine in the 
*			proper order.
*		- Channel reset. This API stop the MPSC and the SDMA machines thus 
*			achieve a complete halt of the channel.
*		- Channel baud rate settings. This API sets the proper clock rate 
*			divisions in both MPSC and BRG modules to get the desired baud rate.
*		
*		This driver does not include API that controls the flow of data in the 
*		serial channel. Those APIs are to be found in the SDMA driver module.
*		
*
* DEPENDENCIES:
*		CUNIT driver
*		MPSC Channel driver.
*		SDMA Channel driver.
*		BRG engine driver.  
*
******************************************************************************/

/* includes */
#include "serialChan.h"   
#include "cunit.h"   
#include "gtMemory.h"   
#ifdef PMC280_DEBUG_UART_VX
#include "dbgprints.h"
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
#include "gtSmp.h"
#endif /* PMC280_DUAL_CPU */

/* defines  */


/* typedefs */


/* locals   */

/*******************************************************************************
* serialChanInit - Initialize serial channel 
*
* DESCRIPTION:
*	This routine initiates the serial channels sub modules (BRG, SDMA and 
*	MPSC)as well as the CUNIT driver. It checks that the SDMA restriction 
*	over Tx/Rx buffers are met. It also uses the CUNIT driver to enable 
*	access to the main memory where driver descriptors and buffers are 
*	assumed to be allocated.
*	RESTRICTION: This driver assumes that the descriptors and buffers 
*	memory area are located in SDRAM, thus the driver open all four access 
*	control windows to enable access to all SDRAM banks. 
*
* INPUT:
*       SERIAL_CHANNEL *serialChanCtrl  Serial channel struct pointer.
*
* OUTPUT:
*	See description.
*
* RETURN:
*	false if the SDMA memory area for Rx/Tx buffers is not under the 
*	protocol restriction or fail to enable access to main memory banks.
*	true otherwise.
*
*******************************************************************************/
bool serialChanInit(SERIAL_CHANNEL *serialChan)
{
    CUNIT_WIN_PARAM winParam;

#ifdef PMC280_UART_DEBUG_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering serialChanInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering serialChanInit\n");
    }
#else
    dbg_puts0("Entering serialChanInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_UART_DEBUG_VX */

#ifdef PMC280_DUAL_CPU
    /* Let only CPU0 call cunitInit () */ 
    if(!frcWhoAmI())
        cunitInit();
#else
    cunitInit();
#endif /* PMC280_DUAL_CPU */

    brgInit(&serialChan->brg);

    /* Assaf, add transparent restrictions */
    if((serialChan->mpscChan.mmcrl & MMCRL_MODE_MASK) == MMCRL_MODE_HDLC)
    {
        /* Check SDMA descriptor aligment restrictions */		
	/* Rx buffer Must be 64bit aligned (i.e. Buffer_Address[2:0]=0b000). */
	if (serialChan->sdmaChan.rxBufBase & 0x7) 
		return false; 

	/* Rx buffers are limited to 64K bytes and Minimum size is 8 bytes  */
	if ((serialChan->sdmaChan.rxBufSize < 8) || 
		(serialChan->sdmaChan.rxBufSize > SDMA_RX_BUFFER_MAX_SIZE)) 
		return false; 
    }
    
    /* Tx buffers are limited to 64K bytes and Minimum size is 8 bytes  */
    if (serialChan->sdmaChan.txBufSize > SDMA_TX_BUFFER_MAX_SIZE)
         return false; 

	/* Get the MPSC protocol type	*/
    serialChan->sdmaChan.protType = (serialChan->mpscChan.mmcrl & MMCRL_MODE_MASK);

    sdmaChanInit(&serialChan->sdmaChan);
	
    serialChan->mpscChan.rcrr = serialChan->brg.brgNum << (serialChan->mpscChan.mpscNum * CRR_BIT_GAP);
    serialChan->mpscChan.tcrr = serialChan->mpscChan.rcrr;
	
    mpscChanInit(&serialChan->mpscChan);

#ifdef PMC280_UART_DEBUG_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed mpscChanInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed mpscChanInit\n");
    }
#else
    dbg_puts0("Passed mpscChanInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_UART_DEBUG_VX */

	/* Set access parameters for SDRAM bank 0 */
    winParam.win 	= CUNIT_WIN0;	/* Use CUNIT window 0		*/
    winParam.target	= TARGET_DRAM;	/* Window target - SDRAM	*/
    winParam.attributes = CBAR_ATTR_DRAM_CS0; /* Enable SDRAM bank 	*/
    winParam.highAddr = 0;
    winParam.baseAddr = gtMemoryGetBankBaseAddress(BANK0);/* Get bank base*/
    winParam.size 	= gtMemoryGetBankSize(BANK0);	/* Get bank size*/
    winParam.enable = 1;   	/* Enable the access		*/
    winParam.accessCtrl = WIN_ACCESS_FULL;	/* Enable full access	*/

    cunitSetAccessControl(serialChan->mpscChan.mpscNum, &winParam);

    if(gtMemoryGetBankSize(BANK1) != 0)
    {
            /* Set access parameters for SDRAM bank 1 */
            winParam.win 	= CUNIT_WIN1;	/* Use CUNIT window 1	*/
            winParam.target	= TARGET_DRAM;	/* Window target - SDRAM*/
            winParam.attributes = CBAR_ATTR_DRAM_CS1; /* Enable SDRAM bank */
            winParam.baseAddr 	= gtMemoryGetBankBaseAddress(BANK1);
            winParam.highAddr 	= 0;
            winParam.size 	= gtMemoryGetBankSize(BANK1);  		
            winParam.enable 	= 1;	/* Enable the access	*/
            winParam.accessCtrl = WIN_ACCESS_FULL;/* Enable full access	*/
            cunitSetAccessControl(serialChan->mpscChan.mpscNum, &winParam);
    }
	
    if(gtMemoryGetBankSize(BANK2) != 0)
    {
           /* Set access parameters for SDRAM bank 2 */
            winParam.win = CUNIT_WIN2;	/* Use CUNIT window 2	*/
            winParam.target = TARGET_DRAM;/* Window target - SDRAM*/
            winParam.attributes = CBAR_ATTR_DRAM_CS2;/* Enable SDRAM bank */
            winParam.baseAddr = gtMemoryGetBankBaseAddress(BANK2);
            winParam.highAddr = 0;
            winParam.size = gtMemoryGetBankSize(BANK2);  		
            winParam.enable = 1;	/* Enable the access	*/
            winParam.accessCtrl = WIN_ACCESS_FULL;/* Enable full access	*/
            cunitSetAccessControl(serialChan->mpscChan.mpscNum, &winParam);
    }
	
    if(gtMemoryGetBankSize(BANK3) != 0)
    {	
            /* Set access parameters for SDRAM bank 3 */
            winParam.win = CUNIT_WIN3;	/* Use CUNIT window 3	*/
            winParam.target = TARGET_DRAM;	/* Window target - SDRAM*/
            winParam.attributes = CBAR_ATTR_DRAM_CS3;/* Enable SDRAM bank */
            winParam.baseAddr = gtMemoryGetBankBaseAddress(BANK3);
            winParam.highAddr = 0;
            winParam.size = gtMemoryGetBankSize(BANK3);  		
            winParam.enable = 1; /* Enable the access	*/
            winParam.accessCtrl = WIN_ACCESS_FULL;/* Enable full access	*/
            cunitSetAccessControl(serialChan->mpscChan.mpscNum, &winParam);
    }

#ifdef PMC280_UART_DEBUG_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting serialChanInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting serialChanInit\n");
    }
#else
    dbg_puts0("Exiting serialChanInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_UART_DEBUG_VX */

	return true;
} 

/*******************************************************************************
* serialChanStart - Start serial channel activity 
*
* DESCRIPTION:
*       This function starts a serial channel activity. The initialization 
*       sequence is restricted to have the BRG first followed by the SDMA and 
*       finally MPSC. The function also sets the serial channel baud rate 
*       according to the serial channel struct parameter.
*
* INPUT:
*       SERIAL_CHANNEL *serialChanCtrl  Serial channel struct pointer.
*
* OUTPUT:
*       The Serial channel is ready to operate at the preset baud rate.
*
* RETURN:
*       false if baud rate setting failed.
*       true otherwise.
*
*******************************************************************************/
bool serialChanStart(SERIAL_CHANNEL *serialChan)
{
	brgStart(&serialChan->brg);
	sdmaChanStart(&serialChan->sdmaChan);
	mpscChanStart(&serialChan->mpscChan);

	/* Set the default baud rate */
	return (serialChanSetBaudRate(serialChan, serialChan->baudRate));
}

/*******************************************************************************
* serialChanStop - Serial channel reset
*
* DESCRIPTION:
*       This function merely calls the MPSC and SDMA Tx/Rx stop routines to 
*       halt any channel traffic.
*       Note: This code perform a stoping machine as mentioned in the 
*	workaround for the Errata "Address decoding error in Communication
*	Unit".
*
* INPUT:
*       SERIAL_CHANNEL *serialChanCtrl  Serial channel struct pointer.
*
* OUTPUT:
*       Tx and Rx activity of the given serial channel are stopped.
*
* RETURN:
*       N/A.
*
*******************************************************************************/
void serialChanStop(SERIAL_CHANNEL	*serialChan)
{
    int i = 0; 

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering serialChanStop\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering serialChanStop\n");
    }
#else
    dbg_puts0("Entering serialChanStop\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Abort SDMA Rx and Tx machine */
    sdmaChanStopTxRx(&serialChan->sdmaChan);

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed sdmaChanStopTxRx\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed sdmaChanStopTxRx\n");
    }
#else
    dbg_puts0("Passed sdmaChanStopTxRx\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Abort MPSC Rx and Tx machine */
    mpscChanStopTxRx(&serialChan->mpscChan);

    /* Put in a delay */
    for(i = 0; i < 0x10000; i++);

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Passed mpscChanStopTxRx\n");
    }
    else
    {
        dbg_puts0("CPU0: Passed mpscChanStopTxRx\n");
    }
#else
    dbg_puts0("Passed mpscChanStopTxRx\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    /* Abort SDMA Rx and Tx machiner */
    sdmaChanStopTxRx(&serialChan->sdmaChan);

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting serialChanStop\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting serialChanStop\n");
    }
#else
    dbg_puts0("Exiting serialChanStop\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
}

/*******************************************************************************
* serialChanSetBaudRate - Set a serial channel baud rate
*
* DESCRIPTION:
*       This routine calculates the MPSC TCDV and RCDV as well as BRG CDV that
*       will produce the required channel baud rate.
*       Note: This routine returns the actual baud rate the channel can produce 
*           using the serial channel input parameters (BRG source clock and 
*           BRG source clock rate). Make sure this baude rate meets your 
*           requirements.
*
* INPUT:
*       SERIAL_CHANNEL  *serialChan     Serial channel struct pointer
*       unsigned int    baudRate        Required channel baud rate
*
* OUTPUT:
*       Appalling MPSC and BRG API to set the CDV parameter to produce the
*       necessary baud rate.
*
* RETURN:
*       false if the baud rate is zero or the baud rate can not be created 
*           using the given clock rate input.
*       true otherwise.
*
*******************************************************************************/
unsigned int serialChanSetBaudRate	(SERIAL_CHANNEL	*serialChan, 
						 unsigned int 	baudRate)
{    
    int mpscCdv = 0;
    unsigned int clkRatio, brgCdv;

    if(baudRate == 0 || baudRate > serialChan->brg.brgSrcClkRate)
        return 0;
    
    clkRatio = serialChan->brg.brgSrcClkRate / baudRate;

    /* Start with the highest resolution divider */

    /* in case of UART, use Rx MPSC_CDV_X1 only when isochronous mode */ 
    if((serialChan->mpscChan.mmcrl & MMCRL_MODE_UART) &&
           (serialChan->mpscChan.mpcr  & MPCR_ISO_SYNCHRONOUS_MODE))
    {
        if(clkRatio / MPSC_CDV_X1 <= BRG_MAX_CDV)
            mpscCdv = MPSC_CDV_X1;
    }
    else if(clkRatio / MPSC_CDV_X8 <= BRG_MAX_CDV)
        mpscCdv = MPSC_CDV_X8;

    else if(clkRatio / MPSC_CDV_X16 <= BRG_MAX_CDV)
        mpscCdv = MPSC_CDV_X16;

    else if(clkRatio / MPSC_CDV_X32 <= BRG_MAX_CDV)
        mpscCdv = MPSC_CDV_X32;

    else /* (clkRatio / MPSC_CDV_X32 > BRG_MAX_CDV) */
        return 0;	/* Baud rate too slow to create */

	
    brgCdv  = serialChan->brg.brgSrcClkRate / (2 * mpscCdv * baudRate) - 1;
	
    mpscChanSetCdv (&serialChan->mpscChan, mpscCdv);
    brgSetCdv(&serialChan->brg, (unsigned short)brgCdv);

    if(brgCdv != 0)
        baudRate = serialChan->brg.brgSrcClkRate / ((brgCdv + 1) * 2) / mpscCdv;
    else
        baudRate = serialChan->brg.brgSrcClkRate / mpscCdv;
	
    return baudRate;
}
