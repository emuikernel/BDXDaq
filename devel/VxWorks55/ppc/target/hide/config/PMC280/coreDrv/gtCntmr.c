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
* gtCntmr.c - GT counters/timers functions
*
* DESCRIPTION                                                                 
*       This file contains function which serves the user with a complete
*       interface to the counters and timers of the GT, please advise: 
*       Each counter/timer unit can function only as a counter or a timer in a
*       current time. Counters/timers are 32 bit wide.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include "gtCntmr.h"

#ifdef PMC280_DUAL_CPU
/*
 * It is assumed that the application that calls these functions use the
 * correct value for CntmrNum. It is inherently assumed that Counter/Timer 
 * 0 and 1 are assigned to CPU0 while Counter/Timer 2 and 3 are assigned to
 * CPU1.
 */
#include "gtSmp.h"
#endif 
                                      
/*******************************************************************************
* gtCntmrStart - Start a counter/timer with a given value and operation mode.
*
* DESCRIPTION:
*       This function load ,enable and set the desired mode to the desired 
*       counter/timer.If the timer/counter is working with external trigger 
*       mode ,it starts counting as soon as TCEn is set to 1 (can be set by the 
*       function ‘cntTmrEnable’ ) and the external TCEn input is asserted. 
*
* INPUT:
*       countNum - One of the possible Counter/Timer values as definded in 
*                  gtCntmr.h
*                  
*       countValue - Initial value for the counter/timer.
*       opMode     - Operation mode as defined in gtCntmr.h:
*
* OUTPUT:
*       None.
*
* RETURN:
*       false if one of the parameters is erroneous, true otherwise.
*
*******************************************************************************/
bool gtCntmrStart(CNTMR_NUM countNum,unsigned int countValue,
                  CNTMR_OP_MODES opMode)
{
    unsigned int cntmrReg = GT_CNTMR_GET_BASE_REG(countNum); 

    if(IS_INVALID_CNTMR(countNum))
        return false;
    gtCntmrDisable(countNum); /* Disable the cntmr if enabled */
    if(!gtCntmrSetMode(countNum,opMode))
        return false;
    GT_REG_WRITE((cntmrReg + (4*(countNum%4))),countValue); 
    gtCntmrEnable(countNum);
    return true;
}

/*******************************************************************************
* gtCntmrDisable - Disables the timer/counter operation and return its 
*                  value.
* DESCRIPTION:
*      This function disables a counter/timer and returns its current value.
*
* INPUT:
*       countNum - One of the possible Counter/Timer values as definded in 
*                  gtCntmr.h
*
* OUTPUT:
*      None.
*
* RETURN:
*      The counter/timer value (unsigned int). if one of the arguments is 
*      erroneous 0 is returned .
*
*******************************************************************************/
unsigned int gtCntmrDisable(CNTMR_NUM countNum)
{
    unsigned int command  = 1;
    unsigned int value    = 0;
    unsigned int regValue = 0;
    unsigned int cntmrControlReg = GT_CNTMR_GET_BASE_CONTROL_REG(countNum);
    unsigned int cntmrReg        = GT_CNTMR_GET_BASE_REG(countNum);    

    if(IS_INVALID_CNTMR(countNum))
        return 0;   /* counter/timer number out of range. */

#ifdef PMC280_DUAL_CPU
    /* Lock */
    frcMv64360semTake(CNTMR_SEM, MV64360_SMP_WAIT_FOREVER);

    GT_REG_READ(cntmrControlReg,&value);
    GT_REG_READ(cntmrReg + (4*(countNum%4)),&regValue);
    /* Disable the timer/counter */
    command = command << (countNum%4)*8; 
    value = value & (~command);  /* Enable-bit, set to logic '0' ==> Disable */
    GT_REG_WRITE(cntmrControlReg,value);

    /* Unlock */
    frcMv64360semGive(CNTMR_SEM);
#else
    GT_REG_READ(cntmrControlReg,&value);
    GT_REG_READ(cntmrReg + (4*(countNum%4)),&regValue);
    /* Disable the timer/counter */
    command = command << (countNum%4)*8; 
    value = value & (~command);  /* Enable-bit, set to logic '0' ==> Disable */
    GT_REG_WRITE(cntmrControlReg,value);
#endif
    return regValue;
}

/*******************************************************************************
* gtCntmrRead - Reads a timer or a counter value.
*
* DESCRIPTION:
*       This function reads and return the given counter/timer (countNum) 
*       value (This operation can be perform while the counter/timer is 
*       active) .
*
* INPUT:
*       countNum - One of the possible Counter/Timer values as definded in 
*                  gtCntmr.h
*
* OUTPUT:
*       None.
*
* RETURN:
*       The 32 bit counter/timer value. 
*
*******************************************************************************/
unsigned int gtCntmrRead(CNTMR_NUM countNum)
{   
    unsigned int value    = 0;
    unsigned int cntmrReg = GT_CNTMR_GET_BASE_REG(countNum);    
    
    if(IS_INVALID_CNTMR(countNum))
        return 0;
    GT_REG_READ((cntmrReg + (4*(countNum%4))),&value); 
    return value;
} 

/*******************************************************************************
* gtCntmrEnable - Set the enable-bit of a given timer/counter.
*
* DESCRIPTION:
*  This function enables the given counter/timer 'countNum' by seting the
*  TCEn bit to ‘1’ . When working as a Timer , the count continues as long
*  as this bit is set to ‘1’ .When working as a Counter , new count starts
*  only with new write of  ‘1’ to the TCEn bit.If the counter/timer is 
*  active, this function will terminate with false.
* INPUT:
*  countNum - One of the possible Counter/Timer values as definded in 
*             gtCntmr.h
*
* OUTPUT:
*  None.
*
* RETURN:
*  false if one of the parameters is erroneous or if the counter/timer is 
*  active, true otherwise.
*
*******************************************************************************/
bool gtCntmrEnable(CNTMR_NUM countNum)
{
    unsigned int command = 1; 
    unsigned int value   = 0;
    unsigned int cntmrControlReg = GT_CNTMR_GET_BASE_CONTROL_REG(countNum);

    if(IS_INVALID_CNTMR(countNum))
        return false;

#ifdef PMC280_DUAL_CPU
    /* Lock */
    frcMv64360semTake(CNTMR_SEM, MV64360_SMP_WAIT_FOREVER);

    GT_REG_READ(cntmrControlReg,&value);
    command = command << (countNum%4)*8;
    if ( (command & value) != 0 )       /* ==> The counter/timer is enabled */
        return false;  /* doesn't make sense to Enable an "enabled" counter */
    value = value | command;           /* set the enable/disable bit to '1' */
    GT_REG_WRITE(cntmrControlReg,value);

    /* Unlock */
    frcMv64360semGive(CNTMR_SEM);
#else
    GT_REG_READ(cntmrControlReg,&value);
    command = command << (countNum%4)*8;
    if ( (command & value) != 0 )       /* ==> The counter/timer is enabled */
        return false;  /* doesn't make sense to Enable an "enabled" counter */
    value = value | command;           /* set the enable/disable bit to '1' */
    GT_REG_WRITE(cntmrControlReg,value);
#endif
    return true;
}

/*******************************************************************************
* gtCntmrLoad - Loads a value to the given counter/timer .
*
* DESCRIPTION:
*       This function loads an initial value to one of the given counter/timer 
*       (countNum) .This value will be decremented  with every Tclk rising edge
*       until it reaches zero.
*
* INPUT:
*       countNum - One of the possible Counter/Timer values as definded in 
*                  gtCntmr.h
*       value    - Initial 32 bit value to be loaded.
* 
* OUTPUT:
*       None.
*
* RETURN:
*       false if one of the parameters is erroneous, true otherwise.
*
*******************************************************************************/
bool gtCntmrLoad(CNTMR_NUM countNum, unsigned int countValue) 
{
    unsigned int command = 1;
    unsigned int value   = 0;
    unsigned int cntmrControlReg = GT_CNTMR_GET_BASE_CONTROL_REG(countNum);
    unsigned int cntmrReg        = GT_CNTMR_GET_BASE_REG(countNum);    
    
    if(IS_INVALID_CNTMR(countNum))
        return false;

#ifdef PMC280_DUAL_CPU
    /* Lock */
    frcMv64360semTake(CNTMR_SEM, MV64360_SMP_WAIT_FOREVER);

    GT_REG_READ(cntmrControlReg,&value);
    command = command << (countNum%4)*8;
    if ( (command & value) != 0 )       /* ==> The counter/timer is enabled */
        return false;  /* can't reload value when counter/timer is enabled  */
    GT_REG_WRITE((cntmrReg + (4*(countNum%4))),countValue);

    /* Unlock */
    frcMv64360semGive(CNTMR_SEM);
#else
    GT_REG_READ(cntmrControlReg,&value);
    command = command << (countNum%4)*8;
    if ( (command & value) != 0 )       /* ==> The counter/timer is enabled */
        return false;  /* can't reload value when counter/timer is enabled  */
    GT_REG_WRITE((cntmrReg + (4*(countNum%4))),countValue);
#endif /* PMC280_DUAL_CPU */

    return true;
}       

/*******************************************************************************
* gtCntmrSetMode - Configure the operation mode of the given counetr/timer.
*
* DESCRIPTION:
*       This function sets the operation mode for a channel tp work as a counter
*       or as a timer.When working in Timer mode ,the timer counts down , issues
*       an interrupt on terminal count, reloads itself to the programmed value,
*       and continues to count.When working in Counter mode ,the counter counts
*       down to terminal count, stops, and issues an interrupt.Note that this 
*       function sets only the working mode and do not enable the channel. 
* INPUT:
*       countNum - One of the possible Counter/Timer values as definded in 
*                  gtCntmr.h
*       opMode   - Operation mode as defined in gtCntmr.h:
*
* OUTPUT:
*       None.
*
* RETURN:
*       false if one of the parameters is erroneous true otherwise .
*
*******************************************************************************/
bool gtCntmrSetMode(CNTMR_NUM countNum, CNTMR_OP_MODES opMode)
{
    unsigned int command = 1;
    unsigned int value   = 0;
    unsigned int cntmrControlReg = GT_CNTMR_GET_BASE_CONTROL_REG(countNum);

    if(IS_INVALID_CNTMR(countNum))
        return false;

#ifdef PMC280_DUAL_CPU
    /* Lock */
    frcMv64360semTake(CNTMR_SEM, MV64360_SMP_WAIT_FOREVER);

    GT_REG_READ(cntmrControlReg,&value);
    command = command << (countNum%4)*8;
    if ( (command & value) != 0 )       /* ==> The counter/timer is enabled */
        return false;  /* can't set the Mode when counter/timer is enabled  */
    command = command << 1;
    switch(opMode)
    {
        /* Timer mode with EXT TRIG-disable */
        case TIMER: 
            value = value | command; /* Enable Timer Mode */
            break;
        /* Timer mode with EXT TRIG-enable */
        case TIMER_EXT_TRIG: 
            value = value | command; /* Enable Timer Mode */
            command = command << 1;
            value = value | command; /* Enable EXT TRIG */
            break;
        /* Counter mode with EXT TRIG-enable */
        case COUNTER_EXT_TRIG:
            value = value & ~command; /* Enable Counter Mode */
            command = command << 1;
            value = value | command;  /* Enable EXT TRIG */
            break;
        /* Counter mode with EXT TRIG-disable */
        case COUNTER:
            value = value & ~command; /* Enable Counter Mode */
            break;
        default:
            return false;
    } 
    GT_REG_WRITE(cntmrControlReg,value);

    /* Unlock */
    frcMv64360semGive(CNTMR_SEM);
#else
    GT_REG_READ(cntmrControlReg,&value);
    command = command << (countNum%4)*8;
    if ( (command & value) != 0 )       /* ==> The counter/timer is enabled */
        return false;  /* can't set the Mode when counter/timer is enabled  */
    command = command << 1;
    switch(opMode)
    {
        /* Timer mode with EXT TRIG-disable */
        case TIMER: 
            value = value | command; /* Enable Timer Mode */
            break;
        /* Timer mode with EXT TRIG-enable */
        case TIMER_EXT_TRIG: 
            value = value | command; /* Enable Timer Mode */
            command = command << 1;
            value = value | command; /* Enable EXT TRIG */
            break;
        /* Counter mode with EXT TRIG-enable */
        case COUNTER_EXT_TRIG:
            value = value & ~command; /* Enable Counter Mode */
            command = command << 1;
            value = value | command;  /* Enable EXT TRIG */
            break;
        /* Counter mode with EXT TRIG-disable */
        case COUNTER:
            value = value & ~command; /* Enable Counter Mode */
            break;
        default:
            return false;
    } 
    GT_REG_WRITE(cntmrControlReg,value);
#endif /* PMC280_DUAL_CPU */

    return true;
}

/*******************************************************************************
* gtCntmrIsTerminate - Check whether a counter/timer has terminated its count or
*                      not.
*
* DESCRIPTION:
*  When the counter/timer terminates its count ,a corresponding bit to the
*  Timer/Counter number ( CNTMR_0 --> BIT0 , CNTMR_1 --> BIT1 .... ) ,is 
*  set to logic ‘1’ to indicate the termination. This function returns 
*  true if this bit is set and resets that bit, it return false otherwise . 
*
*  Note:
*  When operating in timer mode the counter/timer will reach terminal 
*  count,generate an interrupt and starts counting again with the initial 
*  value provided the first time.
*
* INPUT:
*   countNum - One of the possible Counter/Timer values as definded in 
*              gtCntmr.h
*
* OUTPUT:
*       Reset the bit indicating the count termination.
*
* RETURN:
*       true if the bit is set to logic '1', false if there was an erronious
*       input or the bit is in logic '0'.
*
*******************************************************************************/
bool gtCntmrIsTerminate(CNTMR_NUM countNum)
{
    unsigned int group4_7    = 0;
    unsigned int termination = 0;
    unsigned int regData     = 0;

    if(IS_INVALID_CNTMR(countNum))
        return false; /* incorrect input. */
    if(countNum > CNTMR_3) /* the requested counter is in the second group*/
         group4_7 = 1;      /* if the GT supports more than 4 counter/timers*/

#ifdef PMC280_DUAL_CPU
    /* Lock */
    frcMv64360semTake(CNTMR_SEM, MV64360_SMP_WAIT_FOREVER);

    GT_REG_READ(TIMER_COUNTER_0_3_INTERRUPT_CAUSE + 0x100 * group4_7,&regData);
    termination = regData & ( BIT0 << (countNum%4) );
    if(termination) /* the bit was in logic '1' */
    {
        /* clear the interrupt bit and return true. */
        GT_RESET_REG_BITS(TIMER_COUNTER_0_3_INTERRUPT_CAUSE + 0x100*group4_7,
                          BIT0 << (countNum%4) ); 
        return true;
    }
    else
        return false; /* there was no interrupt. */

    /* Unlock */
    frcMv64360semGive(CNTMR_SEM);
#else
    GT_REG_READ(TIMER_COUNTER_0_3_INTERRUPT_CAUSE + 0x100 * group4_7,&regData);
    termination = regData & ( BIT0 << (countNum%4) );
    if(termination) /* the bit was in logic '1' */
    {
        /* clear the interrupt bit and return true. */
        GT_RESET_REG_BITS(TIMER_COUNTER_0_3_INTERRUPT_CAUSE + 0x100*group4_7,
                          BIT0 << (countNum%4) ); 
        return true;
    }
    else
        return false; /* there was no interrupt. */
#endif /* PMC280_DUAL_CPU */
}

/*******************************************************************************
* gtCntmrSetTCTclocks - Set the number of clocks the TcTcnt is asserted.
*
* DESCRIPTION:
*       Each timer/counter has a TCTcnt output pin. This pin is asserted when 
*       the counter reaches zero. This function determine if TcTcnt is asserted
*       for one or two Tclk cycles.
*
* INPUT:
*       countNum    - One of the possible Counter/Timer values as definded in 
*                     gtCntmr.h
*       numOfCycles - The desired number of clocks the TcTcnt is asserted.
*
* OUTPUT:
*       None.
*
* RETURN:
*       True for success , flase if one of the paramters is erroneous.
*
*******************************************************************************/
bool gtCntmrSetTCTclocks(CNTMR_NUM countNum, CNTMR_TCT_CLOCKS numOfCycles)
{
    unsigned int value;

    if(IS_INVALID_CNTMR(countNum))
        return false;  /* # out of range.*/
    if((numOfCycles < ONE_TCLK_CYC) || (TWO_TCLK_CYC < numOfCycles))
        return false; /* # out of range.*/              

#ifdef PMC280_DUAL_CPU
    /* Lock */
    frcMv64360semTake(CNTMR_SEM, MV64360_SMP_WAIT_FOREVER);

    GT_REG_READ(TIMER_COUNTER_0_3_CONTROL,&value);
    value = value | ((numOfCycles<<3) << 8*countNum); 
    GT_REG_WRITE(TIMER_COUNTER_0_3_CONTROL,value);

    /* Unlock */
    frcMv64360semGive(CNTMR_SEM);
#else
    GT_REG_READ(TIMER_COUNTER_0_3_CONTROL,&value);
    value = value | ((numOfCycles<<3) << 8*countNum); 
    GT_REG_WRITE(TIMER_COUNTER_0_3_CONTROL,value);
#endif /* PMC280_DUAL_CPU */
    return true;
}
