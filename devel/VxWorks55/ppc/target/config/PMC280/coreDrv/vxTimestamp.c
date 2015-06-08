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
* vxTimestamp.c - GT timer/counter time stamp driver
*
* DESCRIPTION:
*       Detailed monitoring of real time application performance requires timing 
*       information based on high resolution timers. Although a timestamp driver 
*       is implemented in the system clock driver, using it is not recommended. 
*       The GT counter provides a low roll-over interrupt timer that can be
*       attached to a user defined ISR.
*       The timer is a hardware facility; a timestamp driver is a software
*       interface to that facility. This driver complies with the standard
*       VxWorks timestamp driver interface. 
*
*   Interface:
*       vxTimestampConnect() - connect a C routine to the driver's interrupt
*                              handler.
*       vxTimestampEnable()  - initialize and enable the GT timestamp timer.
*       vxTimestampDisable() - disable the GT timestamp timer.
*       vxTimestampPeriod()  - get the GT timestamp timer period.
*       vxTimestampFreq()    - get the GT timestamp timer clock frequency.
*       vxTimestamp()        - get the GT timestamp timer tick count.
*       vxTimestampLock()    - get the GT timestamp tick count of a timer that
*                              can't be read while enabled.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include "gtCore.h"    
#include "gtCntmr.h"
#include "vxCntmrIntCtrl.h"
#include "vxTimestamp.h"


/* defines */
#define TIMESTAMP_COUT_NUM  0             /* Use Counter cause. */  

#if (TIMESTAMP_COUT_NUM < 4) 
    #define TIMESTAMP_COUT_CAUSE  TIMESTAMP_COUT_NUM      /*Cause 0,1,2,3 */
#else
    #define TIMESTAMP_COUT_CAUSE  TIMESTAMP_COUT_NUM + 27 /*Cause 31.32,33,34 */
#endif

#define INIT_VALUE          0xFFFFFFFE    /* Initial value of the counter  */
#define TMSTMP_INT_PRIO     10            /* Timestamp interrupt priority */

/* typedefs */


/* locals */
LOCAL bool vxTimestampRunning = false;  /* running flag */

/*******************************************************************************
*
* vxTimestampConnect - connect a user routine to the gtInt who handle 
*                       the interrupt.
*
* This routine connects a given routine to the Timer/Counter interrupt 
* controller. It specifies the interrupt cause bit of the GT Timer/Counter
* cause register representing counter expiration and connects the user routine 
* to the interrupt controller.
* It does not enable the GT timestamp timer itself.
*
* Inputs:
* VOIDFUNCPTR routine - routine called at each timestamp timer interrupt.
* int arg             - argument with which to call routine.
*
* RETURNS: OK, or ERROR if vxCntmrIntConnect() didn't succeed.
*/

STATUS vxTimestampConnect (VOIDFUNCPTR routine, int arg)
{
    vxCntmrIntCtrlInit();

    if(vxCntmrIntConnect(TIMESTAMP_COUT_CAUSE, routine, arg, TMSTMP_INT_PRIO) == OK)
        return OK;
    else
        return ERROR;
}

/*******************************************************************************
*
* vxTimestampEnable - initialize and enable the timestamp timer
*
* This routine enables the GT Timer/Counter  interrupt and initializes the
* counter registers.  If the GT timestamp timer is already running, this routine
* merely resets the GT timer counter.
*
* Output:
* Manipulates the GT internal register in order to start timestamp counting.
* 
* RETURNS: OK if output succeeded
*          ERROR if failed to enable GT timer 0 expiration interrupt.
*/

STATUS vxTimestampEnable (void)
{

    /* Enable GT Timer/Counter expiration interrupt */
    vxCntmrIntEnable(TIMESTAMP_COUT_CAUSE);

    /* first - disable the timer (for further counter reload) */
    gtCntmrDisable(TIMESTAMP_COUT_NUM);              

    if(!vxTimestampRunning)
        /* configure the GT counter control register to timestamp mode (TIMER MODE). */
        gtCntmrSetMode(TIMESTAMP_COUT_NUM, TIMER);

    /* set the counter register to a full count down*/
    gtCntmrLoad(TIMESTAMP_COUT_NUM, INIT_VALUE);  

    /* enable counter */
    gtCntmrEnable(TIMESTAMP_COUT_NUM);                

    vxTimestampRunning = true;
    return(OK);
} 


/*******************************************************************************
*
* vxTimestampDisable - disable the GT timestamp counter.
*
* This routine disables the GT timestamp timer.  Interrupts are not disabled,
* although the counter will not increment after the GT timestamp timer
* is disabled, thus interrupts will no longer be generated.
*
* Output:
* Reset the appropriate counter bit in the GT counter control register.
*
* RETURNS: OK.
*/

STATUS vxTimestampDisable (void)
{
    if(vxTimestampRunning)
    {
        gtCntmrDisable(TIMESTAMP_COUT_NUM);              
        vxTimestampRunning = false;
    }
    return(OK);  
}

/*******************************************************************************
*
* vxTimestampPeriod - get the GT timestamp timer period
*
* This routine returns the period of the GT timestamp timer in ticks.
* The period, or terminal count, is the number of ticks to which the GT 
* timestamp timer will count before rolling over and restarting the 
* counting process.
*
* RETURNS: The period of the GT timestamp timer in counter ticks.
*/

UINT32 vxTimestampPeriod (void)
{
    return(INIT_VALUE);    /* 32 bit length of counter */
}

/*******************************************************************************
*
* vxTimestampFreq - get the GT timestamp timer clock frequency
*
* This routine returns the frequency of the GT timer clock, in ticks per second.
*
* RETURNS: The GT timestamp timer clock frequency, in ticks per second.
*/

UINT32 vxTimestampFreq (void)
{
    return(TCLK_RATE);   /* GT clock is the TCLK on the evaluation board. */
}

/*******************************************************************************
*
* vxTimestamp - get the GT timestamp timer tick count
*
* This routine returns the current value of the GT timestamp timer tick counter.
* The tick count can be converted to seconds by dividing by the return of
* auxTimestampFreq().
*
* RETURNS: The current GT timestamp timer tick count.
*
* SEE ALSO: vxTimestampLock()
*/

UINT32 vxTimestamp (void)
{
    return ~(GTREGREAD(TIMER_COUNTER0));  /* up counting */       
}

/*******************************************************************************
*
* vxTimestampLock - get the GT timestamp tick count of a timer that can't 
*                    be read while enabled
*
* This routine returns the current value of the GT timestamp timer tick counter.
* The tick count can be converted to seconds by dividing by the return of
* vxTimestampFreq(). Because the GT timers can be read while counting this
* function does the same as vxTimestamp().
*
* 
* RETURNS: The current GT timestamp timer tick count.
*
* SEE ALSO: vxTimestamp()
*/

UINT32 vxTimestampLock (void)
{
    return(vxTimestamp());
}
