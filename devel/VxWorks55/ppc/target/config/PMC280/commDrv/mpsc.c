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
* mpsc.c
*
* DESCRIPTION:
*		This file introduce a simple API which controls the MPSC operations.
*		This module operations are based on the MPSC struct defined in the 
*		header file.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/

/* includes */
#include "mpsc.h"   
#include "logLib.h"


/* defines  */
#define MPSC_ENTER_HUNT_TIMEOUT 0x10000


/* typedefs */


/* locals   */
void mpscDbg(MPSC_CHANNEL *mpscChan);

/*******************************************************************************
* mpscChanInit - Initialize the MPSC channel driver.
*
* DESCRIPTION:
*       This function completes the MPSC SW struct initialization towards 
*       the mpscChanStart() phase. It also initiates the MPSC interrupt 
*       controller.
*
* INPUT:
*       MPSC_CHANNEL *mpscChan  MPSC channel struct.
*
* OUTPUT:
*       MPSC Interrupt controller initiated.
*
* RETURN:
*       N/A.
*
*******************************************************************************/
void mpscChanInit(MPSC_CHANNEL *mpscChan)
{    	      
	mpscChan->mrr  = MRR_MR_SERIAL_PORT;
	
	return;    
} 

/*******************************************************************************
* mpscChanStart - Start the MPSC channel
*
* DESCRIPTION:
*       This function starts the MPSC channel activity by writing to the MPSC 
*       registers the values needed for its operation. Some of the values are 
*       given by the user and some are calculate in the mpscChanInit() phase.
*
* INPUT:
*       MPSC_CHANNEL *mpscChan  MPSC channel struct.
*
* OUTPUT:
*       All MPSC register are assigned. The channel enters hunt state.
*
* RETURN:
*       false if the MPSC channel fails to enter hunt state within a given
*       timeout. Otherwise, it return true.
*
*******************************************************************************/
bool mpscChanStart(MPSC_CHANNEL	*mpscChan)
{
	unsigned int regValue;
    int huntTimeout = MPSC_ENTER_HUNT_TIMEOUT;
    
    /* Assign the MRR */
	GT_REG_READ(MPSC_ROUTING_REG, &regValue);	
	regValue &= ~(MRR_MR_MASK << (mpscChan->mpscNum * MRR_MR_BIT_GAP));
	regValue |= mpscChan->mrr;
	GT_REG_WRITE(MPSC_ROUTING_REG, regValue);

    /* Assign the RCRR */
	GT_REG_READ(MPSC_RX_CLOCK_ROUTING_REG, &regValue);
	regValue &= ~(CRR_MASK << (mpscChan->mpscNum * CRR_BIT_GAP));
	regValue |= mpscChan->rcrr;
	GT_REG_WRITE(MPSC_RX_CLOCK_ROUTING_REG, regValue);
	
    /* Assign the TCRR */
	GT_REG_READ(MPSC_TX_CLOCK_ROUTING_REG, &regValue);
	regValue &= ~(CRR_MASK << (mpscChan->mpscNum * CRR_BIT_GAP));
	regValue |= mpscChan->tcrr;
	GT_REG_WRITE(MPSC_TX_CLOCK_ROUTING_REG, regValue);

    /* Make sure the channel enters hunt state */
    mpscChan->char2 |= CHAR2_EH_ENABLED;
	
	/* Assign the MPSC configuration and channel registers */
	GT_REG_WRITE((MPSC_MAIN_CONFIG_LOW(mpscChan->mpscNum)), mpscChan->mmcrl);
	GT_REG_WRITE((MPSC_MAIN_CONFIG_HIGH(mpscChan->mpscNum)), mpscChan->mmcrh);
    GT_REG_WRITE((MPSC_PROTOCOL_CONFIG(mpscChan->mpscNum)), mpscChan->mpcr );
    GT_REG_WRITE((MPSC_CHANNEL_REG1(mpscChan->mpscNum)), mpscChan->char1);
    GT_REG_WRITE((MPSC_CHANNEL_REG2(mpscChan->mpscNum)), mpscChan->char2);
	GT_REG_WRITE((MPSC_CHANNEL_REG3(mpscChan->mpscNum)), mpscChan->char3);
	GT_REG_WRITE((MPSC_CHANNEL_REG4(mpscChan->mpscNum)), mpscChan->char4);
	GT_REG_WRITE((MPSC_CHANNEL_REG5(mpscChan->mpscNum)), mpscChan->char5);
	GT_REG_WRITE((MPSC_CHANNEL_REG6(mpscChan->mpscNum)), mpscChan->char6);
	GT_REG_WRITE((MPSC_CHANNEL_REG7(mpscChan->mpscNum)), mpscChan->char7);
	GT_REG_WRITE((MPSC_CHANNEL_REG8(mpscChan->mpscNum)), mpscChan->char8);
	GT_REG_WRITE((MPSC_CHANNEL_REG9(mpscChan->mpscNum)), mpscChan->char9);
    
    /* Make sure MPSC entered hunt state */
    do
    {
        GT_REG_READ(MPSC_CHANNEL_REG2(mpscChan->mpscNum), &regValue);
        if(huntTimeout-- == 0)
        {
            logMsg("MPSC %d can not enter hunt state\n",mpscChan->mpscNum,
                                                                    0,0,0,0,0);
            return false;
        }
    }
    while(regValue & CHAR2_EH_ENABLED);

	
	return true;
}

/*******************************************************************************
* mpscDbg - Display the MPSC struct.
*
* DESCRIPTION:
*       None.
*
* INPUT:
*       MPSC_CHANNEL *mpscChan  MPSC channel struct.
*
* OUTPUT:
*       None.
*
* RETURN:
*       N/A.
*
*******************************************************************************/
void mpscDbg(MPSC_CHANNEL *mpscChan)
{
	logMsg("SDMA channel info:\n",0,0,0,0,0,0);
	logMsg("mpscNum = 0x%x\n",	(unsigned int)mpscChan->mpscNum,0,0,0,0,0);

	logMsg("MRR = 0x%x\n",  (unsigned int)mpscChan->mrr,0,0,0,0,0);
	logMsg("RCRR = 0x%x\n", (unsigned int)mpscChan->rcrr,0,0,0,0,0);
	logMsg("TCRR = 0x%x\n", (unsigned int)mpscChan->tcrr,0,0,0,0,0);
	logMsg("MMCRL = 0x%x\n",(unsigned int)mpscChan->mmcrl,0,0,0,0,0);
	logMsg("MMERH = 0x%x\n",(unsigned int)mpscChan->mmcrh,0,0,0,0,0);
	logMsg("CHAR1 = 0x%x\n",(unsigned int)mpscChan->char1,0,0,0,0,0);
	logMsg("CHAR2 = 0x%x\n",(unsigned int)mpscChan->char2,0,0,0,0,0);
	logMsg("CHAR3 = 0x%x\n",(unsigned int)mpscChan->char3,0,0,0,0,0);
	logMsg("CHAR4 = 0x%x\n",(unsigned int)mpscChan->char4,0,0,0,0,0);
	logMsg("CHAR5 = 0x%x\n",(unsigned int)mpscChan->char5,0,0,0,0,0);
	logMsg("CHAR6 = 0x%x\n",(unsigned int)mpscChan->char6,0,0,0,0,0);
	logMsg("CHAR7 = 0x%x\n",(unsigned int)mpscChan->char7,0,0,0,0,0);
	logMsg("CHAR8 = 0x%x\n",(unsigned int)mpscChan->char8,0,0,0,0,0);
	logMsg("CHAR9 = 0x%x\n",(unsigned int)mpscChan->char9,0,0,0,0,0);

}

/*******************************************************************************
* mpscChanStopTx - Stop MPSC channel Tx activity
*
* DESCRIPTION:
*       This function stops any Tx activity of a given MPSC channel.
*       Note: In order to stop a complete serial channel Tx, the MPSC channel 
*       Tx should be stopped only after the SDMA channel Tx already stopped.
*
* INPUT:
*       MPSC_CHANNEL *mpscChan  MPSC channel struct.
*
* OUTPUT:
*       MPSC Tx activity is stopped. MPSC channel register 10 (Event Status 
*       Register) should mark that the Tx is in idle state.
*
* RETURN:
*       N/A.
*
*******************************************************************************/
void mpscChanStopTx(MPSC_CHANNEL *mpscChan)
{
	unsigned int regValue;

	/* Is MPSC Tx enabled ?? */
	GT_REG_READ(MPSC_MAIN_CONFIG_LOW(mpscChan->mpscNum), &regValue);
	
	if(regValue & MMCRL_ET_ENABLED)
    {
		/* stop the MPSC Tx machines */
		GT_REG_WRITE(MPSC_CHANNEL_REG2(mpscChan->mpscNum), CHAR2_AT_TX_ABORT);		

        /* Check that the Tx aborted in CHAR10 status register */
		do
		{
			GT_REG_READ(MPSC_CHANNEL_REG2(mpscChan->mpscNum) , &regValue);
		}
        while(regValue & CHAR2_AT_TX_ABORT);
    }
	}

/*******************************************************************************
* mpscChanStopRx - Stop MPSC channel Rx activity
*
* DESCRIPTION:
*       This function stops any Rx activity of a given MPSC channel.
*       Note: In order to stop a complete serial channel Rx, the MPSC channel 
*       Rx should be stopped before the SDMA channel Rx.
*
* INPUT:
*       MPSC_CHANNEL *mpscChan  MPSC channel struct.
*
* OUTPUT:
*       MPSC Rx activity is stopped. MPSC channel register 2 (Command Register) 
*       should clear the Rx abort bit upon entering Rx IDLE state.
*
* RETURN:
*       N/A.
*
*******************************************************************************/
void mpscChanStopRx(MPSC_CHANNEL *mpscChan)
{
	unsigned int regValue;
	
	/* Is MPSC Rx enabled ?? */
	GT_REG_READ(MPSC_MAIN_CONFIG_LOW(mpscChan->mpscNum), &regValue);
	
	if(regValue & MMCRL_ER_ENABLED)
	{
		/* Stop the MPSC Rx machines */
		GT_REG_WRITE(MPSC_CHANNEL_REG2(mpscChan->mpscNum), CHAR2_AR_RX_ABORT);
		
		/* Check that the Rx aborted in the MPSC machine */
        do
        {
            GT_REG_READ(MPSC_CHANNEL_REG2(mpscChan->mpscNum) , &regValue);
        }
		while(regValue & CHAR2_AR_RX_ABORT);
	}

}

/*******************************************************************************
* mpscChanStopTxRx - Stop both Rx and Tx simultaneously
*
* DESCRIPTION:
*       This function stops Tx and Rx activity of a given MPSC channel 
*       simultaneously.
*       Note: This code support a stoping machine as mentioned in the 
*			workaround for the Errata "Address decoding error in Communication
*			Unit".
*
* INPUT:
*       MPSC_CHANNEL *mpscChan  MPSC channel struct.
*
* OUTPUT:
*       MPSC Tx and Rx activity are stopped. MPSC channel register 10 
8       (Event Status Register) should mark that the Tx and Rx are in 
*       idle state.
*
* RETURN:
*       N/A.
*
*******************************************************************************/
void mpscChanStopTxRx(MPSC_CHANNEL *mpscChan)
{
	unsigned int regValue;
	
	GT_REG_READ(MPSC_MAIN_CONFIG_LOW(mpscChan->mpscNum), &regValue);

	if(regValue & (MMCRL_ET_ENABLED | MMCRL_ER_ENABLED))
	{
            /* Stop the MPSC Rx and Tx machines */
            GT_REG_WRITE(MPSC_CHANNEL_REG2(mpscChan->mpscNum), (CHAR2_AT_TX_ABORT | CHAR2_AR_RX_ABORT));		

#if FALSE
#error This will not work!!
            do
	    {
                  /* Check that the Rx and Tx aborted in the MPSC machine */
		  GT_REG_READ(MPSC_CHANNEL_REG2(mpscChan->mpscNum) , &regValue);
            }
	    while(regValue & (CHAR2_AR_RX_ABORT | CHAR2_AT_TX_ABORT));
#endif /* FALSE */
    }
}

/*******************************************************************************
* mpscChanSetCdv - Set MPSC Tx and Rx CDV
*
* DESCRIPTION:
*       This function sets the MPSC Tx and Rx clock divider according to a 
*       given parameter. 
*       Note: This function sets TCDV and RCDV with the same value.
*
* INPUT:
*       MPSC_CHANNEL *mpscChan  MPSC channel struct.
*       int          mpscCdv    Clock divider. Can be 1, 8, 16, 32.
*
* OUTPUT:
*       Set MPSC Main configuration register fields TCDV and RCDV.
*
* RETURN:
*       false if the mpscCdv argument is nither of the following 1, 8, 16, 32
*       true otherwise.
*
*******************************************************************************/
bool mpscChanSetCdv(MPSC_CHANNEL *mpscChan, int mpscCdv)
{
	unsigned int regValue;

    GT_REG_READ(MPSC_MAIN_CONFIG_HIGH(mpscChan->mpscNum), &regValue);
	
	regValue &= ~(MMCRH_RCDV_MASK | MMCRH_TCDV_MASK);

	switch(mpscCdv)
	{
		case 1:
            regValue |= (MMCRH_RCDV_X1 | MMCRH_TCDV_X1);
			break;
		case 8:
            regValue |= (MMCRH_RCDV_X8 | MMCRH_TCDV_X8);
			break;
		case 16:
            regValue |= (MMCRH_RCDV_X16 | MMCRH_TCDV_X16);
			break;
		case 32:
            regValue |= (MMCRH_RCDV_X32 | MMCRH_TCDV_X32);
			break;
		default:
			return false;
	}

    GT_REG_WRITE(MPSC_MAIN_CONFIG_HIGH(mpscChan->mpscNum), regValue);

	return true;
}
