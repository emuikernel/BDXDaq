/* m5204Timer.c - Timer driver for Motorola MCF5204 */

/* Copyright 1984-2000 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,06jul00,dh   changed IMR_Tn to IMR_TIMERn throughout.
01b,10jul97,mem  added timestamp driver support.
01a,09dec96,mem  written.
*/

/*
DESCRIPTION

This is the timer for the MCF5204 ColdFire CPU.  It provides system clock,
auxillary clock and timestamp functionality.

If INCLUDE_M5204_SYS_CLK is defined, the system clock is provided.
The macro SYS_CLK_TIMER must be defined (as 1 or 2) to select which
on-chip timer to use for the system clock. The macros SYS_CLK_RATE_MIN
and SYS_CLK_RATE_MAX must be defined to provide parameter checking for
sysClkRateSet().

If INCLUDE_M5204_AUX_CLK is defined, the auxillary clock is provided.
The macro AUX_CLK_TIMER must be defined (as 1 or 2) to select which
on-chip timer to use for the auxillary clock. The macros AUX_CLK_RATE_MIN
and AUX_CLK_RATE_MAX must be defined to provide parameter checking for
auxClkRateSet().

If INCLUDE_M5204_TIMESTAMP is defined, a timestamp driver based on the
system clock is provided.  If this option is selected,
INCLUDE_M5204_SYS_CLK must also be defined.
*/

#include "drv/multi/m5204.h"

/* defines */

#ifdef INCLUDE_M5204_SYS_CLK
#ifndef SYS_CLK_TIMER
#error "INCLUDE_M5204_SYS_CLK requires SYS_CLK_TIMER be defined"
#endif
#if ((SYS_CLK_TIMER != 1) && (SYS_CLK_TIMER != 2))
#error "Bad value for SYS_CLK_TIMER, must be 1 or 2"
#endif
#endif

#ifdef INCLUDE_M5204_AUX_CLK
#ifndef AUX_CLK_TIMER
#error "INCLUDE_M5204_AUX_CLK requires AUX_CLK_TIMER be defined"
#endif
#if ((AUX_CLK_TIMER != 1) && (AUX_CLK_TIMER != 2))
#error "Bad value for AUX_CLK_TIMER, must be 1 or 2"
#endif
#endif

#if (defined(INCLUDE_M5204_SYS_CLK) && defined(INCLUDE_M5204_AUX_CLK) \
    && (SYS_CLK_TIMER == AUX_CLK_TIMER))
#error "SYS_CLK_TIMER and AUX_CLK_TIMER must be different"
#endif

#if defined(INCLUDE_M5204_TIMESTAMP) && !defined(INCLUDE_M5204_SYS_CLK)
#error "INCLUDE_M5204_TIMESTAMP requires INCLUDE_M5204_SYS_CLK"
#endif

/* locals */

#ifdef INCLUDE_M5204_SYS_CLK
LOCAL FUNCPTR sysClkRoutine	= NULL; /* routine to call on clock int */
LOCAL int sysClkArg		= NULL; /* its argument */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkConnected	= FALSE;
LOCAL int sysClkTicksPerSecond	= 60;
LOCAL int sysClkPrescale	= 0;
LOCAL int sysClkCount		= 0;
#endif /* INCLUDE_M5204_SYS_CLK */

#ifdef INCLUDE_M5204_AUX_CLK
LOCAL FUNCPTR sysAuxClkRoutine	= NULL; /* routine to call on clock int */
LOCAL int sysAuxClkArg		= NULL; /* its argument */
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int sysAuxClkConnected	= FALSE;
LOCAL int sysAuxClkTicksPerSecond	= 60;
LOCAL int sysAuxClkPrescale		= 0;
LOCAL int sysAuxClkCount		= 0;
#endif /* INCLUDE_M5204_AUX_CLK */

#ifdef	INCLUDE_M5204_TIMESTAMP
#include "drv/timer/timestampDev.h"

LOCAL BOOL	sysTimestampRunning	= FALSE; 

#endif	/* INCLUDE_M5204_TIMESTAMP */

#ifdef INCLUDE_M5204_SYS_CLK
/*****************************************************************************
*
* sysClkInt - interrupt level processing for system clock
*
* This routine handles the system clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysClkConnect().
*/

LOCAL void sysClkInt (void)
    {
    /* Clear interrupt condition */
    *M5204_TIMER_TER(SIM_BASE, SYS_CLK_TIMER) = 0x02;
    
    if (sysClkRoutine != NULL)
	(* sysClkRoutine) (sysClkArg);
    }

/*****************************************************************************
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
    int arg		/* argument with which to call routine */
    )
    {
    sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */

    sysClkRoutine   = routine;
    sysClkArg	    = arg;
    sysClkConnected = TRUE;

    return (OK);
    }

/*****************************************************************************
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
    if (sysClkRunning && sysClkConnected)
	{
#if (SYS_CLK_TIMER == 1)
	*M5204_SIM_IMR(SIM_BASE) |= M5204_IMR_TIMER1;
#else /* (SYS_CLK_TIMER == 1) */
	*M5204_SIM_IMR(SIM_BASE) |= M5204_IMR_TIMER2;
#endif /* (SYS_CLK_TIMER == 1) */

	/* Clear CLK1-CLK0 to stop timer. */
	*M5204_TIMER_TMR(SIM_BASE, SYS_CLK_TIMER) = 0x00;
	sysClkRunning = FALSE;
	}
    }

/*****************************************************************************
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
    if (!sysClkRunning && sysClkConnected)
	{
	/* Disable timer */
	*M5204_TIMER_TMR(SIM_BASE, SYS_CLK_TIMER) = 0x00;

	/* Reset timer count */
	*M5204_TIMER_TCN(SIM_BASE, SYS_CLK_TIMER) = 0x00;

	sysClkPrescale = (MASTER_CLOCK / sysClkTicksPerSecond) >> 16;
	sysClkCount = ((MASTER_CLOCK / (sysClkPrescale + 1))
		       / sysClkTicksPerSecond);

	/* Set reference register */
	*M5204_TIMER_TRR(SIM_BASE, SYS_CLK_TIMER) = sysClkCount;

	/* Start timer */
	*M5204_TIMER_TMR(SIM_BASE, SYS_CLK_TIMER) = 0x1b | (sysClkPrescale<<8);
	
#if (SYS_CLK_TIMER == 1)
	*M5204_SIM_IMR(SIM_BASE) &= ~M5204_IMR_TIMER1;
#else /* (SYS_CLK_TIMER == 1) */
	*M5204_SIM_IMR(SIM_BASE) &= ~M5204_IMR_TIMER2;
#endif /* (SYS_CLK_TIMER == 1) */

	sysClkRunning = TRUE;
	}
    }

/*****************************************************************************
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

/*****************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    UINT count, prescale;

    if (ticksPerSecond > SYS_CLK_RATE_MAX || ticksPerSecond < SYS_CLK_RATE_MIN)
	return (ERROR);

    prescale = (MASTER_CLOCK / ticksPerSecond) >> 16;
    if (prescale & ~0x00ff)
	return (ERROR);

    count = (MASTER_CLOCK / (prescale + 1)) / ticksPerSecond;
    if (count & 0xffff0000)
	return (ERROR);

    sysClkTicksPerSecond = ticksPerSecond;

    if (sysClkRunning)
	{
	sysClkDisable ();
	sysClkEnable ();
	}

    return (OK);
    }
#endif /* INCLUDE_M5204_SYS_CLK */


#ifdef INCLUDE_M5204_AUX_CLK
/*****************************************************************************
*
* sysAuxClkInt - interrupt level processing for system clock
*
* This routine handles the aux clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysAuxClkConnect().
*/

LOCAL void sysAuxClkInt (void)
    {
    /* Clear interrupt condition */
    *M5204_TIMER_TER(SIM_BASE, AUX_CLK_TIMER) = 0x02;
    
    if (sysAuxClkRoutine != NULL)
	(* sysAuxClkRoutine) (sysAuxClkArg);
    }

/*****************************************************************************
*
* sysAuxClkConnect - connect a routine to the aux clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* aux clock interrupt.
*
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,	/* routine to be called at each clock interrupt */
    int arg		/* argument with which to call routine */
    )
    {
    sysAuxClkRoutine   = routine;
    sysAuxClkArg       = arg;
    sysAuxClkConnected = TRUE;

    return (OK);
    }

/*****************************************************************************
*
* sysAuxClkDisable - turn off aux clock interrupts
*
* This routine disables auxilliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkEnable()
*/

void sysAuxClkDisable (void)
    {
    if (sysAuxClkRunning && sysAuxClkConnected)
	{
#if (AUX_CLK_TIMER == 1)
	*M5204_SIM_IMR(SIM_BASE) |= M5204_IMR_TIMER1;
#else /* (AUX_CLK_TIMER == 1) */
	*M5204_SIM_IMR(SIM_BASE) |= M5204_IMR_TIMER2;
#endif /* (AUX_CLK_TIMER == 1) */

	/* Clear CLK1-CLK0 to stop timer. */
	*M5204_TIMER_TMR(SIM_BASE, AUX_CLK_TIMER) = 0x00;
	sysAuxClkRunning = FALSE;
	}
    }

/*****************************************************************************
*
* sysAuxClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable(), sysAuxClkRateSet()
*/

void sysAuxClkEnable (void)
    {
    if (!sysAuxClkRunning && sysAuxClkConnected)
	{
	/* Disable timer */
	*M5204_TIMER_TMR(SIM_BASE, AUX_CLK_TIMER) = 0x00;

	/* Reset timer count */
	*M5204_TIMER_TCN(SIM_BASE, AUX_CLK_TIMER) = 0x00;

	sysAuxClkPrescale = (MASTER_CLOCK / sysAuxClkTicksPerSecond) >> 16;
	sysAuxClkCount = ((MASTER_CLOCK / (sysAuxClkPrescale + 1))
			  / sysAuxClkTicksPerSecond);

	/* Set reference register */
	*M5204_TIMER_TRR(SIM_BASE, AUX_CLK_TIMER) = sysAuxClkCount;

	/* Start timer */
	*M5204_TIMER_TMR(SIM_BASE, AUX_CLK_TIMER) =
	  0x1b | (sysAuxClkPrescale<<8);
	
#if (AUX_CLK_TIMER == 1)
	*M5204_SIM_IMR(SIM_BASE) &= ~M5204_IMR_TIMER1;
#else /* (AUX_CLK_TIMER == 1) */
	*M5204_SIM_IMR(SIM_BASE) &= ~M5204_IMR_TIMER2;
#endif /* (AUX_CLK_TIMER == 1) */

	sysAuxClkRunning = TRUE;
	}
    }

/*****************************************************************************
*
* sysAuxClkRateGet - get the system clock rate
*
* This routine returns the system clock rate.
*
* RETURNS: The number of ticks per second of the system clock.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateSet()
*/

int sysAuxClkRateGet (void)
    
    {
    return (sysAuxClkTicksPerSecond);
    }

/*****************************************************************************
*
* sysAuxClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
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
    UINT count, prescale;

    if (ticksPerSecond > AUX_CLK_RATE_MAX ||
	ticksPerSecond < AUX_CLK_RATE_MIN)
	return (ERROR);

    prescale = (MASTER_CLOCK / ticksPerSecond) >> 16;
    if (prescale & ~0x00ff)
	return (ERROR);

    count = (MASTER_CLOCK / (prescale + 1)) / ticksPerSecond;
    if (count & 0xffff0000)
	return (ERROR);

    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }
#endif /* INCLUDE_M5204_AUX_CLK */

#ifdef	INCLUDE_M5204_TIMESTAMP

/**************************************************************************
*
* sysTimestampConnect - connect a user routine to the timestamp timer 
*                       interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt. It does not enable the timestamp timer itself.
*
* RETURNS: OK, or ERROR if sysTimestampInt() interrupt handler is not used.
*/

STATUS sysTimestampConnect
    (
    FUNCPTR routine, /* routine called at each timestamp timer interrupt */
    int arg          /* argument with which to call routine */
    )
    {
    return (ERROR);
    }


/**************************************************************************
*
* sysTimestampEnable - initialize and enable the timestamp timer
*
* This routine connects the timestamp timer interrupt and initializes the
* counter registers. If the timestamp timer is already running, this 
* routine merely resets the timer counter.
*
* Set the rate of the timestamp timer input clock explicitly within the 
* BSP, in the sysHwInit() routine. This routine does not initialize 
* the timer clock rate.
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be enabled.
*/

STATUS sysTimestampEnable(void)
    {
    if (sysTimestampRunning)
	return (OK);

    sysTimestampRunning = TRUE;

    sysClkEnable ();	/* ensure the system clock is running */

    return (OK);
    }


/**************************************************************************
*
* sysTimestampDisable - disable the timestamp timer
*
* This routine disables the timestamp timer. Interrupts are not disabled.
* However, the tick counter will not increment after the timestamp timer
* is disabled, ensuring that interrupts are no longer generated.
* routine merely resets the timer counter.
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be disabled.
*/

STATUS sysTimestampDisable (void)
    {
    sysTimestampRunning = FALSE;

    return (ERROR);
    }


/**************************************************************************
*
* sysTimestampPeriod - get the timestamp timer period
*
* This routine returns the period of the timer in timestamp ticks.
* The period, or terminal count, is the number of ticks to which the 
* timestamp timer counts before rolling over and restarting the counting
* process.
*
* RETURNS: The period of the timer in timestamp ticks.
*/

UINT32 sysTimestampPeriod (void)
    {
    /* Return the system clock period in timestamp ticks */

    return (sysTimestampFreq () / sysClkRateGet ());
    }


/**************************************************************************
*
* sysTimestampFreq - get the timestamp timer clock frequency
*
* This routine returns the frequency of the timer clock, in ticks per 
* second. The rate of the timestamp timer should be set explicitly in the 
* BSP, in the sysHwInit() routine.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    UINT32 prescale;

    /*
     * Return the timestamp tick output frequency here.
     * This value can be determined from the following equation:
     *     timerFreq = clock input frequency / prescaler
     *
     * When possible, read the clock input frequency and prescaler values
     * directly from chip registers.
     */

    prescale = ((*M5204_TIMER_TMR(SIM_BASE, SYS_CLK_TIMER) >> 8) + 1);
    return (MASTER_CLOCK / prescale);
    }


/**************************************************************************
*
* sysTimestamp - get the timestamp timer tick count
*
* This routine returns the current value of the timestamp timer tick counter.
* The tick count can be converted to seconds by dividing by the return of
* sysTimestampFreq().
*
* Call this routine with interrupts locked. If interrupts are 
* not already locked, use sysTimestampLock() instead.
*
* RETURNS: The current timestamp timer tick count.
* SEE ALSO: sysTimestampLock()
*/

UINT32 sysTimestamp (void)
    {
    return (*M5204_TIMER_TCN(SIM_BASE, SYS_CLK_TIMER));
    }


/**************************************************************************
*
* sysTimestampLock - lock interrupts for reading the timer
*
* This routine returns the current value of the timestamp timer tick counter.
* The tick count can be converted to seconds by dividing by the return of
* sysTimestampFreq().
*
* Call this routine with interrupts locked. If interrupts are 
* not already locked, use sysTimestampLock() instead.
*
* RETURNS: The current timestamp timer tick count.
* SEE ALSO: sysTimestampLock()
*/

UINT32 sysTimestampLock (void)
    {
    int lockKey;
    UINT32 value;

    lockKey = intLock();
    value = sysTimestamp ();
    intUnlock (lockKey);
    
    return (value);
    }

#endif	/* INCLUDE_M5204_TIMESTAMP */
