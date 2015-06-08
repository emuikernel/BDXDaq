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
* vxEthernetIntCtrl.h - Header File for : Ethernet Interrupt Controller driver
*
* DESCRIPTION:
*		This file includes definitions, typedefs and function declarations 
*		for the Ethernet Interrupt Controller module.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/

#ifndef __INCvxEthernetIntCntlh
#define __INCvxEthernetIntCntlh

/* includes */
#include "gtCore.h"


/* defines  */


/* typedefs */

  /*
  * ETH_CAUSE is an enumerator that moves between 0 and 63. This 
  * enumerator describes the cause bits positions in the Etherent interrupt 
  * cause register as well as ethernet interrupt extended cause register.
  * Use those macros as cause parameter in the various driver API.
  */             

typedef enum ethCause
{
    /* Ethernet Interrupt cause register bit description */
    
	ETH_CAUSE_START = -1   	, /* Cause low boundary				*/
    ETH_RX_BUFFER_RETURN    , /* Rx Buffer Return 				*/
	ETH_INT_CAUSE_EXTEND	, /* Interrupt Extend register		*/
	ETH_RX_BUFFER_QUEUE_0	, /* Rx Buffer Return in Queue[0] 	*/
	ETH_RX_BUFFER_QUEUE_1 	, /* Rx Buffer Return in Queue[1] 	*/
	ETH_RX_BUFFER_QUEUE_2   , /* Rx Buffer Return in Queue[2] 	*/
	ETH_RX_BUFFER_QUEUE_3   , /* Rx Buffer Return in Queue[3] 	*/
	ETH_RX_BUFFER_QUEUE_4	, /* Rx Buffer Return in Queue[4] 	*/
	ETH_RX_BUFFER_QUEUE_5 	, /* Rx Buffer Return in Queue[5] 	*/
	ETH_RX_BUFFER_QUEUE_6   , /* Rx Buffer Return in Queue[6] 	*/
	ETH_RX_BUFFER_QUEUE_7   , /* Rx Buffer Return in Queue[7] 	*/
	ETH_RX_ERROR           	, /* Rx Resource Error Summary		*/	
	ETH_RX_ERROR_QUEUE_0    , /* Rx Resource Error in Queue[0] 	*/
	ETH_RX_ERROR_QUEUE_1    , /* Rx Resource Error in Queue[1] 	*/
	ETH_RX_ERROR_QUEUE_2    , /* Rx Resource Error in Queue[2] 	*/
	ETH_RX_ERROR_QUEUE_3    , /* Rx Resource Error in Queue[3] 	*/
	ETH_RX_ERROR_QUEUE_4    , /* Rx Resource Error in Queue[4] 	*/
	ETH_RX_ERROR_QUEUE_5    , /* Rx Resource Error in Queue[1] 	*/
	ETH_RX_ERROR_QUEUE_6    , /* Rx Resource Error in Queue[6] 	*/
	ETH_RX_ERROR_QUEUE_7    , /* Rx Resource Error in Queue[7] 	*/
	ETH_TX_END_QUEUE_0		, /* Tx Buffer end in Queue[0] 		*/
	ETH_TX_END_QUEUE_1		, /* Tx Buffer end in Queue[1] 		*/
	ETH_TX_END_QUEUE_2		, /* Tx Buffer end in Queue[2] 		*/
	ETH_TX_END_QUEUE_3		, /* Tx Buffer end in Queue[3] 		*/
	ETH_TX_END_QUEUE_4		, /* Tx Buffer end in Queue[4] 		*/
	ETH_TX_END_QUEUE_5		, /* Tx Buffer end in Queue[5] 		*/
	ETH_TX_END_QUEUE_6		, /* Tx Buffer end in Queue[6] 		*/
	ETH_TX_END_QUEUE_7		, /* Tx Buffer end in Queue[7] 		*/
	ETH_RESERVED_1			,
	ETH_RESERVED_2			,
	ETH_RESERVED_3			,
	ETH_RESERVED_4			,
	ETH_INT_SUM             , /* Ethernet Interrupt Summary */
	
	/* Ethernet Extend Interrupt cause register */
	ETH_TX_BUFFER_QUEUE_0	, /* Tx Buffer Return in Priority Queue[0] 	*/
	ETH_TX_BUFFER_QUEUE_1 	, /* Tx Buffer Return in Priority Queue[1] 	*/
	ETH_TX_BUFFER_QUEUE_2   , /* Tx Buffer Return in Priority Queue[2] 	*/
	ETH_TX_BUFFER_QUEUE_3   , /* Tx Buffer Return in Priority Queue[3] 	*/
	ETH_TX_BUFFER_QUEUE_4	, /* Tx Buffer Return in Priority Queue[4] 	*/
	ETH_TX_BUFFER_QUEUE_5 	, /* Tx Buffer Return in Priority Queue[5] 	*/
	ETH_TX_BUFFER_QUEUE_6   , /* Tx Buffer Return in Priority Queue[6] 	*/
	ETH_TX_BUFFER_QUEUE_7   , /* Tx Buffer Return in Priority Queue[7] 	*/
	ETH_TX_ERROR_QUEUE_0    , /* Tx Resource Error in Priority Queue[0]	*/
	ETH_TX_ERROR_QUEUE_1    , /* Tx Resource Error in Priority Queue[1] */
	ETH_TX_ERROR_QUEUE_2    , /* Tx Resource Error in Priority Queue[2] */
	ETH_TX_ERROR_QUEUE_3    , /* Tx Resource Error in Priority Queue[3] */
	ETH_TX_ERROR_QUEUE_4    , /* Tx Resource Error in Priority Queue[4] */
	ETH_TX_ERROR_QUEUE_5    , /* Tx Resource Error in Priority Queue[5] */
	ETH_TX_ERROR_QUEUE_6    , /* Tx Resource Error in Priority Queue[6] */
	ETH_TX_ERROR_QUEUE_7    , /* Tx Resource Error in Priority Queue[7] */
	ETH_PHY_STATUS_CHANGE	, /* Status change reported by the PHY 		*/
	ETH_RESERVED_5			,
	ETH_RX_OVERRUN			,
	ETH_TX_UNDERRUN			,
	ETH_LINK_STATE_CHANGE	,
	ETH_PARTITION			,
	ETH_AUTO_NEG_DONE		,
	ETH_INTERNAL_ADDR_ERR	,
	ETH_RESERVED_6			,
	ETH_RESERVED_7			,
	ETH_RESERVED_8			,
	ETH_RESERVED_9  		,
	ETH_RESERVED_10  		,
	ETH_RESERVED_11  		,
	ETH_RESERVED_12			,
	ETH_INT_EXTEND_SUM      , /* Ethernet Extended Interrupt Summary 	*/
    ETH_CAUSE_END		 	
    }ETH_CAUSE;

#define ETH_PORT_0_ISR_PRIO	9	/* Ethernet port 0 ISR priority (max is 0 ) */
#define ETH_PORT_1_ISR_PRIO	9	/* Ethernet port 1 ISR priority (max is 0 ) */


/* ethernetIntCntl.h API list */
void vxEthernetIntCntlInit (); 
bool vxEthernetIntConnect  (ETH_PORT ethPortNum,
						    ETH_CAUSE cause, 
						    VOIDFUNCPTR routine, 
						    int parameter, 
						    int prio);
bool vxEthernetIntDisable  (ETH_PORT ethPortNum, ETH_CAUSE cause);
bool vxEthernetIntEnable   (ETH_PORT ethPortNum, ETH_CAUSE cause);

#endif /* __INCvxEthernetIntCntlh */


