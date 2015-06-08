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
* cunit.c
*
* DESCRIPTION:
*		This file introduce simple C-Unit driver. Its API merely initialize the 
*		C-Unit interrupt controller and configures the access control.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/

/* includes */
#include "cunit.h"   

/* defines  */


/* typedefs */


/* locals */
static bool cunitInitialized = false;

/*******************************************************************************
* cunitInit - Init the C-Unit
*
* DESCRIPTION:
*	This function initialize the C-Unit by setting the GPP interrupt mode
*	to be level sensitive.
*
* INPUT:
*	None.
*
* OUTPUT:
*	See description.
*
* RETURN:
*	None.
*
*******************************************************************************/
void cunitInit(void)
{
    unsigned int regValue;

    if(cunitInitialized == true)
        return;

    /* Set the GPP cause register to be level sensitive */
    GT_REG_READ(CUNIT_ARBITER_CONTROL_REG, &regValue);
    regValue |= CACR_GPP_INT_LEVEL;
    GT_REG_WRITE(CUNIT_ARBITER_CONTROL_REG, regValue);

    cunitInitialized = true;

    return;
} 

/*******************************************************************************
* gtConfAddrWin - Config address decode parameters for C-Unit 
*
* DESCRIPTION:
*       This function configures the address decode parameters for the C-Unit 
*		according the given parameters struct. This routine does not consider 
*		the case where the second parameters struct given to this routine has 
*		the same address space and the following window will be used instead of 
*		using the same window.
*
* INPUT:                           
*       CUNIT_WIN_PARAM *param - Address decode parameter struct.
*                                               
* OUTPUT:                                       
*       An access window is opened using the given access parameters.
*                                               
* RETURN:                                                                 
*       None.
*
*******************************************************************************/
void cunitSetAccessControl(MPSC_NUM mpscNum, CUNIT_WIN_PARAM *param)
{
	unsigned int regValue;
	unsigned int baseAddrValue, highAddrValue, sizeValue, accessProtVal;

	baseAddrValue =	(param->target) | (param->attributes) | (param->baseAddr);

	highAddrValue = param->highAddr;

	/* divide the size to get the size by 64K terms */
	sizeValue     = (((param->size / 0x10000) -1) << 16);

	accessProtVal = param->accessCtrl << (param->win * 2);

	switch(param->win)
	{
		case CUNIT_WIN0:
			/* Base address remap reg (BARR) */
			GT_REG_WRITE(CUNIT_BASE_ADDR_REG0, baseAddrValue);
			
			/* High address remap reg (HARR) */
			GT_REG_WRITE(CUNIT_HIGH_ADDR_REMAP_REG0, highAddrValue);
			
			/* Set window Size reg (SR) */ 
			GT_REG_WRITE(CUNIT_SIZE0, sizeValue);
			
			/* Set access control register */
			GT_REG_READ((MPSC0_ACCESS_PROTECTION_REG + (mpscNum*4)), &regValue);
			regValue &= ~WIN0_ACCESS_MASK; /* clear window permission */
			regValue |= accessProtVal;
			GT_REG_WRITE((MPSC0_ACCESS_PROTECTION_REG + (mpscNum*4)), regValue);
			
			break;
				
		case CUNIT_WIN1:
			/* Base address remap reg (BARR) */
			GT_REG_WRITE(CUNIT_BASE_ADDR_REG1, baseAddrValue);
			
			/* High address remap reg (HARR) */
			GT_REG_WRITE(CUNIT_HIGH_ADDR_REMAP_REG1, highAddrValue);
			
			/* Set window Size reg (SR) */ 
			GT_REG_WRITE(CUNIT_SIZE1, sizeValue);
			
			/* Set access control register */
			GT_REG_READ((MPSC0_ACCESS_PROTECTION_REG + (mpscNum*4)), &regValue);
			regValue &= ~WIN1_ACCESS_MASK; /* clear window permission */
			regValue |= accessProtVal;
			GT_REG_WRITE((MPSC0_ACCESS_PROTECTION_REG + (mpscNum*4)), regValue);
			
			break;

		case CUNIT_WIN2:
			/* Base address remap reg (BARR) */
			GT_REG_WRITE(CUNIT_BASE_ADDR_REG2, baseAddrValue);
			
			/* Set window Size reg (SR) */ 
			GT_REG_WRITE(CUNIT_SIZE2, sizeValue);
			
			/* Set access control register */
			GT_REG_READ((MPSC0_ACCESS_PROTECTION_REG + (mpscNum*4)), &regValue);
			regValue &= ~WIN2_ACCESS_MASK; /* clear window permission */
			regValue |= accessProtVal;
			GT_REG_WRITE((MPSC0_ACCESS_PROTECTION_REG + (mpscNum*4)), regValue);
			
			break;

		case CUNIT_WIN3:
			/* Base address remap reg (BARR) */
			GT_REG_WRITE(CUNIT_BASE_ADDR_REG3, baseAddrValue);
			
			/* Set window Size reg (SR) */ 
			GT_REG_WRITE(CUNIT_SIZE3, sizeValue);
			
			/* Set access control register */
			GT_REG_READ((MPSC0_ACCESS_PROTECTION_REG + (mpscNum*4)), &regValue);
			regValue &= ~WIN3_ACCESS_MASK; /* clear window permission */
			regValue |= accessProtVal;
			GT_REG_WRITE((MPSC0_ACCESS_PROTECTION_REG + (mpscNum*4)), regValue);

			break;
		
		default:
			return;
	}

	/* Base address enable reg (BARER) */
	if (param->enable == 1)
		GT_RESET_REG_BITS(CUNIT_BASE_ADDR_ENABLE_REG, (1 << param->win));
	else
		GT_SET_REG_BITS(CUNIT_BASE_ADDR_ENABLE_REG, (1 << param->win));
        
    return;
}
