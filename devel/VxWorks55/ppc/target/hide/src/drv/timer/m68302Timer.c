/* m68302Timer.c - MC68302 timer library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,10mar97,map  added timestamp support [SPR# 7943]
01d,31oct96,wlf  doc: cleanup.
01c,04feb93,caf  fixed modification history, updated copyright notice.
01b,04feb93,caf  modified sysClkEnable() and sysAuxClkEnable() to use
		 divide-by-16 prescaler when necessary (SPR #1756).
01a,10aug92,caf  moved clock routines from ver 01h of ads302/sysLib.c, ansified.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
Motorola MC68302.  This library handles system clock, auxiliary clock,
and timestamp functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

To include timestamp facility, define a macro INCLUDE TIMESTAMP. Note that the
system clock is used by the timestamp facility; and changing the system clock
rate will affect the timestamp period. Current timestamp period can be read by
calling sysTimestampPeriod().

INCLUDE_FILES: timestampDev.h
*/

/*includes */

#include "config.h"
#include "drv/timer/timestampDev.h"

/* locals */

LOCAL FUNCPTR sysClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysClkArg		= NULL; /* its argument */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkConnected	= FALSE;
LOCAL int sysClkTicksPerSecond	= 60;

LOCAL FUNCPTR sysAuxClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysAuxClkArg		= NULL; /* its argument */
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int auxClkConnected	= FALSE;
LOCAL int auxClkTicksPerSecond	= 60;

#ifdef	INCLUDE_TIMESTAMP
LOCAL BOOL sysTimestampRunning 	= FALSE; /* timestamp running flag */
#endif	/* INCLUDE_TIMESTAMP */

/*******************************************************************************
*
* sysClkInt - interrupt level processing for system clock
*
* This routine handles the system clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysClkConnect().
*/

LOCAL void sysClkInt (void)
    {
    *IMP_TER1 = TER_REF_CNT;		/* clear event register */
    *IMP_ISR  = INT_TMR1;		/* clear in-service bit */

    if (sysClkRoutine != NULL)
        (*sysClkRoutine) (sysClkArg);
    }

/*******************************************************************************
*
* sysAuxClkInt - interrupt level processing for auxiliary clock
*
* This routine handles the auxiliary clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysAuxClkConnect().
*/

LOCAL void sysAuxClkInt (void)
    {
    *IMP_TER2 = TER_REF_CNT;		/* clear event register */
    *IMP_ISR  = INT_TMR2;		/* clear in-service bit */

    if (sysAuxClkRoutine != NULL)
        (*sysAuxClkRoutine) (sysAuxClkArg);
    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c to
* connect usrClock() to the system clock interrupt.
*
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,	/* routine to be called at each clock interrupt */
    int     arg		/* argument with which to call routine          */
    )
    {
    sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */

    sysClkRoutine   = routine;
    sysClkArg       = arg;
    sysClkConnected = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* sysClkDisable - turn off system clock interrupts
*
* This routine disables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkEnable()
*/

void sysClkDisable (void)
    {
    if (sysClkRunning)
	{
	*IMP_TMR1     = TMR_STOP;	/* turn off timer */
	*IMP_IMR     &= ~INT_TMR1;	/* disable interrupt */
	sysClkRunning = FALSE;		/* clock is no longer running */
	}
    }

/*******************************************************************************
*
* sysClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkConnect(), sysClkDisable(), sysClkRateSet()
*/

void sysClkEnable (void)
    {
    UINT32  tempDiv = SYS_CPU_FREQ / (sysClkTicksPerSecond << 8);

    if ((!sysClkRunning) && (tempDiv <= USHRT_MAX * 16))
	{
	if (tempDiv <= USHRT_MAX)
	    {
	    *IMP_TRR1 = (UINT16) tempDiv;
	    *IMP_TMR1 = 0xff00 | TMR_ENABLE | TMR_CLK |	    /* mpu clk/256   */
			TMR_RESTART | TMR_INT | TMR_CAP_DIS;/* no capture    */
	    }
	else
	    {
	    *IMP_TRR1 = (UINT16) (tempDiv / 16);
	    *IMP_TMR1 = 0xff00 | TMR_ENABLE | TMR_CLK16 |   /* mpu clk/4096  */
			TMR_RESTART | TMR_INT | TMR_CAP_DIS;/* no capture    */
	    }

	*IMP_TER1 = TER_REF_CNT;			    /* clear event   */
	*IMP_IMR |= INT_TMR1;				    /* enable intrpt */

	sysClkRunning = TRUE;
	}
    }

/*******************************************************************************
*
* sysClkRateGet - get the system clock rate
*
* This routine returns the system clock rate.
*
* RETURNS: The number of ticks per second of the system clock.
*
* SEE ALSO: sysClkEnable(), sysClkRateSet()
*/

int sysClkRateGet (void)
    {
    return (sysClkTicksPerSecond);
    }

/*******************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.  It does not
* enable system clock interrupts.  It is called by usrRoot() in
* usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be
* set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
	return (ERROR);

    sysClkTicksPerSecond = ticksPerSecond;

    if (sysClkRunning)
	{
	sysClkDisable ();
	sysClkEnable ();
	}

    return (OK);
    }

/*******************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock
* interrupts.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,	/* routine called at each aux. clock interrupt */
    int     arg		/* argument to auxiliary clock interrupt routine */
    )
    {
    sysAuxClkRoutine   = routine;
    sysAuxClkArg       = arg;
    auxClkConnected    = TRUE;

    return (OK);
    }

/*******************************************************************************
*
* sysAuxClkDisable - turn off auxiliary clock interrupts
*
* This routine disables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkEnable()
*/

void sysAuxClkDisable (void)
    {
    if (sysAuxClkRunning)
	{
	*IMP_TMR2 = TMR_STOP;			/* turn off timer */
	*IMP_IMR &= ~INT_TMR2;			/* disable interrupt */
	sysAuxClkRunning = FALSE;
	}
    }

/*******************************************************************************
*
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable(), sysAuxClkRateSet()
*/

void sysAuxClkEnable (void)
    {
    UINT32  tempDiv = SYS_CPU_FREQ / (auxClkTicksPerSecond << 8);

    if ((!sysAuxClkRunning) && (tempDiv <= USHRT_MAX * 16))
	{
	if (tempDiv <= USHRT_MAX)
	    {
	    *IMP_TRR2 = (UINT16) tempDiv;
	    *IMP_TMR2 = 0xff00 | TMR_ENABLE | TMR_CLK |	    /* mpu clk/256   */
			TMR_RESTART | TMR_INT | TMR_CAP_DIS;/* no capture    */
	    }
	else
	    {
	    *IMP_TRR2 = (UINT16) (tempDiv / 16);
	    *IMP_TMR2 = 0xff00 | TMR_ENABLE | TMR_CLK16 |   /* mpu clk/4096  */
			TMR_RESTART | TMR_INT | TMR_CAP_DIS;/* no capture    */
	    }

	*IMP_TER1 = TER_REF_CNT;			    /* clear event   */
	*IMP_IMR |= INT_TMR2;				    /* enable intrpt */

	sysAuxClkRunning = TRUE;
	}
    }

/*******************************************************************************
*
* sysAuxClkRateGet - get the auxiliary clock rate
*
* This routine returns the interrupt rate of the auxiliary clock.
*
* RETURNS: The number of ticks per second of the auxiliary clock.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateSet()
*/

int sysAuxClkRateGet (void)
    {
    return (auxClkTicksPerSecond);
    }

/*******************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock.
* It does not enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
	return (ERROR);

    auxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }

#ifdef INCLUDE_TIMESTAMP

/*******************************************************************************
*
* sysTimestampConnect - connect a user routine to a timestamp timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt.
*
* RETURNS: ERROR, always.
*/

STATUS sysTimestampConnect
    (
    FUNCPTR routine,    /* routine called at each timestamp timer interrupt */
    int arg             /* argument with which to call routine */
    )
    {
    return (ERROR);
    }

/*******************************************************************************
*
* sysTimestampEnable - enable a timestamp timer interrupt
*
* This routine enables timestamp timer interrupts and resets the counter.
*
* RETURNS: OK, while sysClock is enabled.
*
* SEE ALSO: sysTimestampDisable()
*/

STATUS sysTimestampEnable (void)
   {
   if (!sysClkRunning)
       return (ERROR);

   sysTimestampRunning = TRUE;

   return (OK);
   }

/*******************************************************************************
*
* sysTimestampDisable - disable a timestamp timer interrupt
*
* This routine disables timestamp timer interrupts.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampDisable (void)
    {
    sysTimestampRunning = FALSE;
    return (OK);
    }

/*******************************************************************************
*
* sysTimestampPeriod - get the period of a timestamp timer
*
* This routine gets the period of the timestamp timer, in ticks.  The
* period, or terminal count, is the number of ticks to which the timestamp
* timer counts before rolling over and restarting the counting process.
*
* RETURNS: The period of the timestamp timer in counter ticks.
*/

UINT32 sysTimestampPeriod (void)
    {
    return (*IMP_TRR1);
    }

/*******************************************************************************
*
* sysTimestampFreq - get a timestamp timer clock frequency
*
* This routine gets the frequency of the timer clock, in ticks per
* second.  The rate of the timestamp timer is set explicitly by the
* hardware and typically cannot be altered.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
   {
   UINT32 prescale, clockDiv;
   
   prescale = (*IMP_TMR1 >> 8) + 1;
   clockDiv = ((*IMP_TMR1 & (TMR_CLK | TMR_CLK16)) == TMR_CLK16) ? 16 : 1;

   return (SYS_CPU_FREQ / (prescale * clockDiv));
   }

/*******************************************************************************
*
* sysTimestamp - get a timestamp timer tick count
*
* This routine returns the current value of the timestamp timer tick counter.
* The tick count can be converted to seconds by dividing it by the return of
* sysTimestampFreq().
*
* This routine should be called with interrupts locked.  If interrupts are
* not locked, sysTimestampLock() should be used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampFreq(), sysTimestampLock()
*/

UINT32 sysTimestamp (void)
    {
    return ((UINT32) *IMP_TCN1);
    }

/*******************************************************************************
*
* sysTimestampLock - lock interrupts and get the timestamp timer tick count
*
* This routine locks interrupts when the tick counter must be stopped
* in order to read it or when two independent counters must be read.
* It then returns the current value of the timestamp timer tick
* counter.
*
* The tick count can be converted to seconds by dividing it by the return of
* sysTimestampFreq().
*
* If interrupts are already locked, sysTimestamp() should be
* used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampFreq(), sysTimestamp()
*/

UINT32 sysTimestampLock (void)
    {
    /* same as sysTimestamp() */
    return ((UINT32) *IMP_TCN1);
    }
#endif  /* INCLUDE_TIMESTAMP */
