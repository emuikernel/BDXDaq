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
* vxDmaIntCtrl.h - Header File for : DMA interrupt controller driver
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCvxSdmaIntCtrlh
#define __INCvxSdmaIntCtrlh

/* includes */
#include "gtIntControl.h"


/* defines  */


/* typedefs */

/* SDMA_CAUSE is an enumerator that describes the SDMA causes. the enumerator */
/* describes the cause bits positions in the DMA cause  registers.            */

typedef enum _sdmaCause
{
    SDMA_CAUSE_START = -1,/* Cause Start boundry 							  */
    SDMA_RX_BUF      , 	  /* SDMA Channel Rx Buffer Return                    */
    SDMA_RX_ERR      , 	  /* SDMA Channel Rx Error. Rx resource error occurred*/
    SDMA_TX_BUF      , 	  /* SDMA Channel Tx Buffer Return                    */
    SDMA_TX_END      , 	  /* SDMA Channel Tx End  							  */
    SDMA_CAUSE_END   	  /* Cause End boundry 								  */
} SDMA_CAUSE;

/* vxDmaIntCtrl.h API list */
void    vxSdmaIntCtrlInit (void); 
STATUS  vxSdmaIntConnect  (SDMA_NUM sdmaNum, 
                           SDMA_CAUSE cause, 
                           VOIDFUNCPTR routine,
                           int parameter, 
                           int prio);
STATUS  vxSdmaIntEnable   (SDMA_NUM sdmaNum, SDMA_CAUSE cause);
STATUS  vxSdmaIntDisable  (SDMA_NUM sdmaNum, SDMA_CAUSE cause);
STATUS  vxSdmaIntClear    (SDMA_NUM sdmaNum, SDMA_CAUSE cause);

#endif /* __INCvxSdmaIntCtrlh */

