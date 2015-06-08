/* vme2Timer.c - vmechip2 Timestamp Timer driver library */

/* Copyright 1994-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,06aug96,dat  merged into main 5_3_x tree.
01d,28nov94,rdc  fixed spr 3023.
01c,16apr94,dzb  changed file name from vme2Timer.c to vme2TS.c
01b,10jan94,dzb  added conditional compilation for INCLUDE_TIMESTAMP macro.
01a,17sep93,dzb  written.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
VMEchip2 with a board-independent interface.  This library handles
the timestamp timer facility.

To include the timestamp timer facility, the macro INCLUDE_TIMESTAMP must be
defined.  The macro TIMESTAMP_LEVEL must be defined to provide the timestamp
timer's interrupt level.

For proper operation the macro CPU_SPEED is presumed to be the correct
speed, in hertz, for the CPU clock.

Other macros used by this driver:

    VMECHIP2_TTCOUNT1  - timer counter 1, 32 bits
    VMECHIP2_TIMEOUTCR - Prescaler count in byte 0
    VMECHIP2_TTCOMP1   - timer 1 compare reg, 32 bits
    VMECHIP2_ILR1      - interrupt level register 1
    VMECHIP2_LBIER     - local bus int enable 1
    VMECHIP2_ICLR      - interrupt clear register
*/

#ifdef	INCLUDE_TIMESTAMP

#include "drv/timer/timestampDev.h"
#include "drv/vme/vmechip2.h"

/* Locals */ 

LOCAL BOOL    sysTimestampRunning = FALSE;	/* running flag */
LOCAL FUNCPTR sysTimestampRoutine = NULL;	/* user rollover routine */
LOCAL int     sysTimestampArg     = NULL;	/* arg to user routine */

/*******************************************************************************
*
* sysTimestampInt - timestamp timer interrupt handler
*
* This rountine handles the timestamp timer interrupt.  A user routine is
* called, if one was connected by sysTimestampConnect().
*
* RETURNS: N/A   
*
* SEE ALSO: sysTimestampConnect()
*/

LOCAL void sysTimestampInt (void)
    {
    *VMECHIP2_ICLR = ICLR_CTIC1;	/* acknowledge timer interrupt */

    if (sysTimestampRoutine != NULL)	/* call user routine */
        (*sysTimestampRoutine) (sysTimestampArg);
    }

/*******************************************************************************
*
* sysTimestampConnect - connect a user routine to the timestamp timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt.  It does not enable the timestamp timer itself.
*
* RETURNS: OK, or ERROR if sysTimestampInt() interrupt handler is not used.
*/
 
STATUS sysTimestampConnect
    (
    FUNCPTR routine,	/* routine called at each timestamp timer interrupt */
    int arg		/* argument with which to call routine */
    )
    {
    sysTimestampRoutine = routine;
    sysTimestampArg = arg;
 
    return (OK);
    }

/*******************************************************************************
*
* sysTimestampEnable - initialize and enable the timestamp timer
*
* This routine connects the timestamp timer interrupt and initializes the
* counter registers.  If the timestamp timer is already running, this routine
* merely resets the timer counter.
*
* The rate of the timestamp timer should be set explicitly within the BSP,
* in the sysHwInit() routine.  This routine does not intialize the timer
* rate.  
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be enabled.
*/
 
STATUS sysTimestampEnable (void)
    {
    if (sysTimestampRunning)
	{
        *VMECHIP2_TTCOUNT1 = 0;			/* clear the counter */
	return (OK);
	}
    
    (void) intConnect (INUM_TO_IVEC (UTIL_INT_VEC_BASE0 + LBIV_TT1),
                       sysTimestampInt, NULL);

    sysTimestampRunning = TRUE;

    /* reset & enable the timestamp timer interrupt */

    *VMECHIP2_ICLR  = ICLR_CTIC1;
    *VMECHIP2_LBIER |= LBIER_ETIC1;
    *VMECHIP2_ILR1  |= TIMESTAMP_LEVEL;

    *VMECHIP2_TTCOMP1 = sysTimestampPeriod();	/* set the timer period */

    *VMECHIP2_TTCOUNT1 = 0;			/* clear the counter */
    *VMECHIP2_TIMERCR |= TIMERCR_TT1_EN;	/* enable the timer */

    return (OK);
    }

/********************************************************************************
* sysTimestampDisable - disable the timestamp timer
*
* This routine disables the timestamp timer.  Interrupts are not disabled,
* although the tick counter will not increment after the timestamp timer
* is disabled, thus interrupts will no longer be generated.
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be disabled.
*/
 
STATUS sysTimestampDisable (void)
    {
    if (sysTimestampRunning)
	{
        *VMECHIP2_TIMERCR &= ~TIMERCR_TT1_EN;	/* disable the timer */

        sysTimestampRunning = FALSE;
	}

    return (OK);
    }

/*******************************************************************************
*
* sysTimestampPeriod - get the timestamp timer period
*
* This routine returns the period of the timestamp timer in ticks.
* The period, or terminal count, is the number of ticks to which the timestamp
* timer will count before rolling over and restarting the counting process.
*
* RETURNS: The period of the timestamp timer in counter ticks.
*/
 
UINT32 sysTimestampPeriod (void)
    {
    return (0xffffffff);	/* highest period -> freerunning */
    }

/*******************************************************************************
*
* sysTimestampFreq - get the timestamp timer clock frequency
*
* This routine returns the frequency of the timer clock, in ticks per second.
* The rate of the timestamp timer should be set explicitly within the BSP,
* in the sysHwInit() routine.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/
 
UINT32 sysTimestampFreq (void)
    {
    UINT32 timerFreq;
    UINT32 Bclk;

#ifdef CPU_SPEED
    Bclk = CPU_SPEED;
#else
    Bclk = 25000000;          	/* use 25MHz if not valid speed */
#endif

    timerFreq = Bclk/(256 - (*VMECHIP2_TIMEOUTCR & 0xff));

    return (timerFreq);
    }
 
/*******************************************************************************
*
* sysTimestamp - get the timestamp timer tick count
*
* This routine returns the current value of the timestamp timer tick counter.
* The tick count can be converted to seconds by dividing by the return of
* sysTimestampFreq().
*
* This routine should be called with interrupts locked.  If interrupts are
* not already locked, sysTimestampLock() should be used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampLock()
*/
 
UINT32 sysTimestamp (void)
    {
    return (*VMECHIP2_TTCOUNT1);
    }

/*******************************************************************************
*
* sysTimestampLock - get the timestamp timer tick count
*
* This routine returns the current value of the timestamp timer tick counter.
* The tick count can be converted to seconds by dividing by the return of
* sysTimestampFreq().
*
* This routine locks interrupts for cases where it is necessary to stop the
* tick counter in order to read it, or when two independent counters must
* be read.  If interrupts are already locked, sysTimestamp() should be
* used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestamp()
*/
 
UINT32 sysTimestampLock (void)
    {
    return (*VMECHIP2_TTCOUNT1);
    }

#endif	/* INCLUDE_TIMESTAMP */
