/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                 *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA OR ANY THIRD PARTY. MOTOROLA RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************


*******************************************************************************
* brg.c
*
* DESCRIPTION:
*		This file implements the BRG engine low level driver. It is based on 
*		the BRG struct defined in the brg.h header file.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/

/* includes */
#include "brg.h"   
#include "logLib.h"   

/* defines  */


/* typedefs */


/* locals   */

void brgDbg(BRG_ENGINE *pBrg);

/*******************************************************************************
* brgInit - Initiate the BRG engine SW struct.
*
* DESCRIPTION:
*	Initiate the BRG engine SW struct.
*
* INPUT:
*	BRG_ENGINE 		*pBrg     Pointer to BRG struct.
*
* OUTPUT:
*	None.
*
* RETURN:
*	N/A.
*
*******************************************************************************/
void brgInit(BRG_ENGINE *pBrg)
{
	switch(pBrg->brgSrcClk)
	{
		case(BCLKIN):
			pBrg->brgConfigReg = BRG_CLK_SRC_BCLKIN;
			break;
		
		case(SCLK0):
			pBrg->brgConfigReg = BRG_CLK_SRC_SCLK0;
			break;
		
		case(TSCLK0):
			pBrg->brgConfigReg = BRG_CLK_SRC_TSCLK0;
			break;

		case(SCLK1):
			pBrg->brgConfigReg = BRG_CLK_SRC_SCLK1;
			break;

		case(TSCLK1):
			pBrg->brgConfigReg = BRG_CLK_SRC_TSCLK1;
			break;

		case(SYSCLK):
			pBrg->brgConfigReg = BRG_CLK_SRC_SYSCLK;
			break;
		default:
			pBrg->brgConfigReg = BRG_CLK_SRC_BCLKIN;
	}
	return;
} 

/*******************************************************************************
* brgStart - Enable the BRG engine operation
*
* DESCRIPTION:
*		This routine starts the BRG engine.
*
* INPUT:
*		BRG_ENGINE 		*pBrg     Pointer to BRG struct.
*
* OUTPUT:
*		None.
*
* RETURN:
*		N/A.
*
*******************************************************************************/
void brgStart(BRG_ENGINE *pBrg)
{
    unsigned int    regValue;

	/* Disable BRG engine */
	GT_REG_READ(BRG_CONFIG_REG(pBrg->brgNum), &regValue);
	regValue &= ~BCR_EN_ENABLE_BRG;
	GT_REG_WRITE(BRG_CONFIG_REG(pBrg->brgNum), regValue);
	
	/* Enable BRG */
	pBrg->brgConfigReg |= BCR_EN_ENABLE_BRG;
	
	GT_REG_WRITE(BRG_CONFIG_REG(pBrg->brgNum), pBrg->brgConfigReg);

	return;
}
/*******************************************************************************
* brgDbg - display the BRG engine struct.
*
* DESCRIPTION:
*		display the BRG engine struct.
*
* INPUT:
*		BRG_ENGINE 		*pBrg     Pointer to BRG struct.
*
* OUTPUT:
*		None.
*
* RETURN:
*		N/A.
*
*******************************************************************************/
void brgDbg(BRG_ENGINE *pBrg)
{
	logMsg("BRG info:\n",0,0,0,0,0,0);
	logMsg("brgNum = 0x%x\n",	   (unsigned int)pBrg->brgNum,0,0,0,0,0);
	logMsg("brgSrcClk = 0x%x\n",   (unsigned int)pBrg->brgSrcClk,0,0,0,0,0);
	logMsg("brgSrcClkRate = %d\n", (unsigned int)pBrg->brgSrcClkRate,0,0,0,0,0);
	logMsg("brgConfigReg = 0x%x\n",(unsigned int)pBrg->brgConfigReg,0,0,0,0,0);
}

/*******************************************************************************
* brgSetCdv - Set the CDV.
*
* DESCRIPTION:
*		This function Set CDV field in the BRG Control register.
*
* INPUT:
*		BRG_ENGINE 		*pBrg     Pointer to BRG struct.
*		unsigned short 	brgCdv	  New CDV value.
*
* OUTPUT:
*		Set Only CDV value in BRG config register.
*
* RETURN:
*		N/A.
*
*******************************************************************************/
void brgSetCdv(BRG_ENGINE *pBrg, unsigned short brgCdv)
{
    unsigned int    regValue;

	/* Disable BRG engine */
	GT_REG_READ(BRG_CONFIG_REG(pBrg->brgNum), &regValue);
	regValue &= ~BCR_EN_ENABLE_BRG;
	GT_REG_WRITE(BRG_CONFIG_REG(pBrg->brgNum), regValue);
	
	/* Set CDV with new value */
    GT_REG_READ(BRG_CONFIG_REG(pBrg->brgNum), &regValue);
	regValue &= ~BCR_CDV_MASK;
	regValue |= (brgCdv | BCR_EN_ENABLE_BRG);
    GT_REG_WRITE(BRG_CONFIG_REG(pBrg->brgNum), regValue);

	return;
}
