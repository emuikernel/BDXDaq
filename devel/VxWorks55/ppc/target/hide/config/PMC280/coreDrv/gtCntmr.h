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
* gtCntmr.h - GT counters/timers functions header
*
* DESCRIPTION                                                                 
* This file contains function which serves the user with a complete interface 
* to the counters and timers of the GT, please advise: Each counter/timer unit
* can function only as a counter or a timer in a current time.                 
* Counters/timers 0-7 are 32 bit wide.                                        
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCgtCntmrh 
#define __INCgtCntmrh

/* includes */           
#include "gtCore.h"

/* defines  */
#define CNTMR_FIRST       CNTMR_0

#ifdef INCLUDE_CNTMR_4_7
    #define CNTMR_LAST    CNTMR_7
#else    
    #define CNTMR_LAST    CNTMR_3 
#endif

#define GT_CNTMR0_READ(pData) GT_REG_READ(TIMER_COUNTER0, (pData))
#define GT_CNTMR1_READ(pData) GT_REG_READ(TIMER_COUNTER1, (pData))
#define GT_CNTMR2_READ(pData) GT_REG_READ(TIMER_COUNTER2, (pData))
#define GT_CNTMR3_READ(pData) GT_REG_READ(TIMER_COUNTER3, (pData))
       
#define IS_INVALID_CNTMR(countNum)                                             \
                      (((countNum) < CNTMR_FIRST) || (CNTMR_LAST < (countNum)))
#ifdef INCLUDE_CNTMR_4_7
    #define GT_CNTMR4_READ(pData) GT_REG_READ(TIMER_COUNTER4, (pData))
    #define GT_CNTMR5_READ(pData) GT_REG_READ(TIMER_COUNTER5, (pData))
    #define GT_CNTMR6_READ(pData) GT_REG_READ(TIMER_COUNTER6, (pData))
    #define GT_CNTMR7_READ(pData) GT_REG_READ(TIMER_COUNTER7, (pData))
    #define GT_CNTMR_GET_BASE_CONTROL_REG(countNum) ((countNum) <= CNTMR_3)?   \
                                                    TIMER_COUNTER_0_3_CONTROL :\
                                                    TIMER_COUNTER_4_7_CONTROL   
    
    #define GT_CNTMR_GET_BASE_REG(countNum)         ((countNum) <= CNTMR_3)?   \
                                                    TIMER_COUNTER0 :           \
                                                    TIMER_COUNTER4              
#else
    #define GT_CNTMR_GET_BASE_CONTROL_REG(countNum)  TIMER_COUNTER_0_3_CONTROL
    #define GT_CNTMR_GET_BASE_REG(countNum)     TIMER_COUNTER0
#endif /* INCLUDE_CNTMR_4_7 */

/* typedefs */
typedef enum _cntmrNum      {CNTMR_0, CNTMR_1, CNTMR_2, CNTMR_3
#ifdef INCLUDE_CNTMR_4_7
                            ,CNTMR_4, CNTMR_5, CNTMR_6, CNTMR_7
#endif /* INCLUDE_CNTMR_4_7 */
                            } CNTMR_NUM;

typedef enum _cntmrOpModes  {COUNTER, TIMER, COUNTER_EXT_TRIG, TIMER_EXT_TRIG
                            } CNTMR_OP_MODES;

typedef enum _cntmrTctClocks{ONE_TCLK_CYC, TWO_TCLK_CYC} CNTMR_TCT_CLOCKS;

/* Functions */

/* Load an init Value to a given counter/timer */
bool gtCntmrLoad(CNTMR_NUM countNum, unsigned int value);

/* Set the Mode COUNTER/TIMER/EXTRIG to a given counter/timer */
bool gtCntmrSetMode(CNTMR_NUM countNum, CNTMR_OP_MODES opMode);

/* Set the Enable-Bit to logic '1' ==> starting the counter. */
bool gtCntmrEnable(CNTMR_NUM countNum);	            

/* Stop the counter/timer running, and returns its Value. */
unsigned int gtCntmrDisable(CNTMR_NUM countNum);              

/* Returns the value of the given Counter/Timer */
unsigned int gtCntmrRead(CNTMR_NUM countNum);

/* Combined all the sub-operations above to one function: Load,setMode,Enable */
bool gtCntmrStart(CNTMR_NUM countNum, unsigned int countValue,
                  CNTMR_OP_MODES opMode);

bool gtCntmrIsTerminate (CNTMR_NUM countNum);

/* Set the TCT pin clock cycles */
bool gtCntmrSetTCTclocks(CNTMR_NUM countNum, CNTMR_TCT_CLOCKS numOfCycles);

#endif /* __INCgtCntmrh */
