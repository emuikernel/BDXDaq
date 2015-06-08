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
* vxCntmrIntCtrl.h - Header File for : GT Counter/Timer interrupt controller
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCvxCntmrIntCtrlh
#define __INCvxCntmrIntCtrlh

/* includes */
#include "gtIntControl.h"


/* defines  */


/* typedefs */

 
/* CNTMR_CAUSE is an enumerator that moves between 0 and 31/63. It summarize */
/* both Counter/Timer cause register into one virtual cause register sized   */
/* 64. This enumerator describes the cause bits positions in the             */
/* Counter/Timer cause registers.                                            */
typedef enum _cntmrCause
{
    /* Counter/Timer 0-3 Interrupt cause register - offset 0x868 */
    CNTMR_CAUSE_START = -1, /* Cause Start boundry */
    CNTMR0_EXP      , /* Counter/Timer 0 expiration interrupt event */
    CNTMR1_EXP      , /* Counter/Timer 1 expiration interrupt event */
    CNTMR2_EXP      , /* Counter/Timer 2 expiration interrupt event */
    CNTMR3_EXP      , /* Counter/Timer 3 expiration interrupt event */
    CNTMR_RSRVD_L04 ,
    CNTMR_RSRVD_L05 ,
    CNTMR_RSRVD_L06 ,
    CNTMR_RSRVD_L07 ,
    CNTMR_RSRVD_L08 ,
    CNTMR_RSRVD_L09 ,
    CNTMR_RSRVD_L10 ,
    CNTMR_RSRVD_L11 ,
    CNTMR_RSRVD_L12 ,
    CNTMR_RSRVD_L13 ,
    CNTMR_RSRVD_L14 ,
    CNTMR_RSRVD_L15 ,
    CNTMR_RSRVD_L16 ,
    CNTMR_RSRVD_L17 ,
    CNTMR_RSRVD_L18 ,
    CNTMR_RSRVD_L19 ,
    CNTMR_RSRVD_L20 ,
    CNTMR_RSRVD_L21 ,
    CNTMR_RSRVD_L22 ,
    CNTMR_RSRVD_L23 ,
    CNTMR_RSRVD_L24 ,
    CNTMR_RSRVD_L25 ,
    CNTMR_RSRVD_L26 ,
    CNTMR_RSRVD_L27 ,
    CNTMR_RSRVD_L28 ,
    CNTMR_RSRVD_L29 ,
    CNTMR_RSRVD_L30 ,
    CNTMR_SUM       , /* Summary of all cause bits. - Read Only */
    CNTMR_CAUSE_END   /* Cause End boundry */
} CNTMR_CAUSE;

        
/* vxCntmrIntCtrl.h API list */
void    vxCntmrIntCtrlInit(void); 
STATUS  vxCntmrIntConnect (CNTMR_CAUSE cause, VOIDFUNCPTR routine,
                           int parameter, int prio);
STATUS  vxCntmrIntEnable  (CNTMR_CAUSE cause);
STATUS  vxCntmrIntDisable (CNTMR_CAUSE cause);

#endif /* __INCvxCntmrIntCtrlh */
