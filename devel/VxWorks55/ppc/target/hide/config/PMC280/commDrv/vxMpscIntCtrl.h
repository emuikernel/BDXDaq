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
* vxMpscIntCtrl.h - Header File for : MPSC Interrupt Controller driver
*
* DESCRIPTION:
*		None.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/

#ifndef __INCvxMpscIntCtrlh
#define __INCvxMpscIntCtrlh

/* includes */
#include "gtCore.h"


/* defines  */


/* typedefs */

  /*
  * MPSC_CAUSE is an enumerator that describes the cause bits positions 
  * in the MPSC cause register
  */             

typedef enum _mpscCause{
    /* Mpsc Interrupt cause register bit description */
    
	MPSC_CAUSE_START = -1, /* Cause low boundry								  */
    MPSC_RX_NORMAL_SUM   , /* MPSC Normal Rx Interrupt Summary 				  */
    MPSC_RX_ERROR_SUM    , /* MPSC Rx Error Interrupt Summary 				  */
    MPSC_RSRVD_02		 ,
	MPSC_TX_ERROR_SUM    , /* MPSC Tx Error Interrupt Summary 				  */
	MPSC_RX_LINE_STATUS	 , /* MPSC Rx Line Status Change (from to IDLE)		  */
	MPSC_RX_ENTER_HUNT	 , /* MPSC Rx Entered HUNT State 					  */
	MPSC_RX_RFSC_RCC 	 , /* MPSC Rx Flag Status Change (HDLC mode) or		  */
						   /* MPSC Received Control Character (Bisync, UART)  */
	MPSC_RX_RCSC   	 	 , /* MPSC Rx Carrier Sense Change 					  */
	MPSC_RX_OVERRUN   	 , /* MPSC Rx Overrun 								  */
	MPSC_RX_CARRIER_LOSS , /* MPSC Rx Carrier Detect Loss 					  */
	MPSC_RX_CLOCK_GLITCH , /* MPSC Rx Clock Glitch 							  */
	MPSC_RX_BISYNC_ERROR , /* MPSC Bisync Protocol Error 					  */
	MPSC_TX_IN_IDLE   	 , /* MPSC Tx Entered IDLE State 					  */
	MPSC_TX_UNDERRUN   	 , /* MPSC Tx Underrun 								  */
	MPSC_TX_CTS_LOSS     , /* MPSC Tx Clear To Send Loss 					  */
	MPSC_TX_CLOCK_GLITCH , /* MPSC Tx Clock Glitch							  */
    MPSC_CAUSE_END		   /* Cause high boundry							  */
    }MPSC_CAUSE;

#define MPSC0_ISR_PRIO	9	/* Mpsc port 0 ISR priority (max is 0 ) */
#define MPSC1_ISR_PRIO	9	/* Mpsc port 1 ISR priority (max is 0 ) */


/* vxMpscIntCntl.h API list */
void vxMpscIntCntlInit (); 
bool vxMpscIntConnect  (MPSC_NUM mpscNum,
						MPSC_CAUSE cause, 
						VOIDFUNCPTR routine, 
						int parameter, 
						int prio);
bool vxMpscIntDisable  (MPSC_NUM mpscNum, MPSC_CAUSE cause);
bool vxMpscIntEnable   (MPSC_NUM mpscNum, MPSC_CAUSE cause);
bool vxMpscIntClear    (MPSC_NUM mpscNum, MPSC_CAUSE cause);

#endif /* __INCvxMpscIntCtrlh */


