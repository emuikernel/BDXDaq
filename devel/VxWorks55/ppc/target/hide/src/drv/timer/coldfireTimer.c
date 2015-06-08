/* coldfireTimer.c - Timer driver for Motorola ColdFire processors */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,29jan02,dee  fixed typecasting to clean up compiler warnings int<->uint
                 merge in T2.1.0 changes SET_IMR_BIT, CLEAR_IMR_BIT
01b,09jul98,gls  Adapted to WRS coding conventions
01a,19apr98,mem  written, based on m5204Timer and m5206Timer.
*/

/*
DESCRIPTION

This is the timer for the ColdFire CPUs.  It provides system clock,
auxillary clock and timestamp functionality.

If INCLUDE_COLDFIRE_SYS_CLK is defined, the system clock is provided.
The macros SYS_CLK_RATE_MIN and SYS_CLK_RATE_MAX must be defined to
provide parameter checking for sysClkRateSet().

If INCLUDE_COLDFIRE_AUX_CLK is defined, the auxillary clock is provided.
The macros AUX_CLK_RATE_MIN
and AUX_CLK_RATE_MAX must be defined to provide parameter checking for
auxClkRateSet().

If INCLUDE_COLDFIRE_TIMESTAMP is defined, a timestamp driver based on the
system clock is provided.  If this option is selected,
INCLUDE_COLDFIRE_SYS_CLK must also be defined.

*/

#include "drv/timer/coldfireTimer.h"

/* defines */

#ifndef SET_IMR_BIT	/* allows BSP to define this macro */
#define SET_IMR_BIT(pTimer) \
	do \
	    { \
	    if ((pTimer)->imrSize == 1) \
		*(volatile UINT8  *) (pTimer)->imr |= (pTimer)->imrMask; \
	    else if ((pTimer)->imrSize == 2) \
		*(volatile UINT16 *) (pTimer)->imr |= (pTimer)->imrMask; \
	    else \
		*(volatile UINT32 *) (pTimer)->imr |= (pTimer)->imrMask; \
	    } while (0)
#endif /* SET_IMR_BIT */

#ifndef CLEAR_IMR_BIT	/* allows BSP to define this macro */
#define CLEAR_IMR_BIT(pTimer) \
	do \
	    { \
	    if ((pTimer)->imrSize == 1) \
		*(volatile UINT8  *) (pTimer)->imr &= ~(pTimer)->imrMask; \
	    else if ((pTimer)->imrSize == 2) \
		*(volatile UINT16 *) (pTimer)->imr &= ~(pTimer)->imrMask; \
	    else \
		*(volatile UINT32 *) (pTimer)->imr &= ~(pTimer)->imrMask; \
	    } while (0)
#endif /* CLEAR_IMR_BIT */

/* locals */

#ifdef INCLUDE_COLDFIRE_SYS_CLK
LOCAL COLDFIRE_TIMER		sysClkTimer;
LOCAL FUNCPTR sysClkRoutine	= NULL; /* routine to call on clock int */
LOCAL int sysClkArg		= NULL; /* its argument */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkConnected	= FALSE;
LOCAL int sysClkTicksPerSecond	= 60;
LOCAL int sysClkPrescale	= 0;
LOCAL int sysClkCount		= 0;
#endif /* INCLUDE_COLDFIRE_SYS_CLK */

#ifdef INCLUDE_COLDFIRE_AUX_CLK
LOCAL COLDFIRE_TIMER		auxClkTimer;
LOCAL FUNCPTR sysAuxClkRoutine	= NULL; /* routine to call on clock int */
LOCAL int sysAuxClkArg		= NULL; /* its argument */
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int sysAuxClkConnected	= FALSE;
LOCAL int sysAuxClkTicksPerSecond	= 60;
LOCAL int sysAuxClkPrescale		= 0;
LOCAL int sysAuxClkCount		= 0;
#endif /* INCLUDE_COLDFIRE_AUX_CLK */

#ifdef	INCLUDE_COLDFIRE_TIMESTAMP
#include "drv/timer/timestampDev.h"

LOCAL BOOL	sysTimestampRunning	= FALSE; 

#endif	/* INCLUDE_COLDFIRE_TIMESTAMP */

#ifdef INCLUDE_COLDFIRE_SYS_CLK
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

    *sysClkTimer.ter = COLDFIRE_TER_REF;
    
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

    /* XXX for now -- needs to be in usrConfig.c */

    if (!sysClkConnected)
	sysHwInit2 ();

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
	SET_IMR_BIT(&sysClkTimer);

	/* Clear CLK1-CLK0 to stop timer. */

	*sysClkTimer.tmr = 0x00;
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

	*sysClkTimer.tmr = 0x00;

	/* Reset timer count */

	*sysClkTimer.tcn = 0x00;

	sysClkPrescale = (MASTER_CLOCK / sysClkTicksPerSecond) >> 16;
	sysClkCount = ((MASTER_CLOCK / (sysClkPrescale + 1))
		       / sysClkTicksPerSecond);

	/* Set reference register */

	*sysClkTimer.trr = (UINT16)sysClkCount;

	/* Start timer */

	*sysClkTimer.tmr = (UINT16)(COLDFIRE_TMR_ORI | COLDFIRE_TMR_FRR
			    | COLDFIRE_TMR_CLK_BUS | COLDFIRE_TMR_EN
			    | (sysClkPrescale << 8));
	CLEAR_IMR_BIT(&sysClkTimer);

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
#endif /* INCLUDE_COLDFIRE_SYS_CLK */


#ifdef INCLUDE_COLDFIRE_AUX_CLK
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

    *auxClkTimer.ter = COLDFIRE_TER_REF;
    
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
	SET_IMR_BIT(&auxClkTimer);

	/* Clear CLK1-CLK0 to stop timer. */

	*auxClkTimer.tmr = 0x00;
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

	*auxClkTimer.tmr = 0x00;

	/* Reset timer count */

	*auxClkTimer.tcn = 0x00;

	sysAuxClkPrescale = (MASTER_CLOCK / sysAuxClkTicksPerSecond) >> 16;
	sysAuxClkCount = ((MASTER_CLOCK / (sysAuxClkPrescale + 1))
			  / sysAuxClkTicksPerSecond);

	/* Set reference register */

	*auxClkTimer.trr = (UINT16)sysAuxClkCount;

	/* Start timer */

	*auxClkTimer.tmr = (UINT16)(COLDFIRE_TMR_ORI | COLDFIRE_TMR_FRR
			    | COLDFIRE_TMR_CLK_BUS | COLDFIRE_TMR_EN
			    | (sysAuxClkPrescale << 8));
	CLEAR_IMR_BIT(&auxClkTimer);

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
#endif /* INCLUDE_COLDFIRE_AUX_CLK */

#ifdef	INCLUDE_COLDFIRE_TIMESTAMP

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

    prescale = ((*sysClkTimer.tmr >> 8) + 1);
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
    return (*sysClkTimer.tcn);
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

    lockKey = intLock ();
    value = sysTimestamp ();
    intUnlock (lockKey);
    
    return (value);
    }

#endif	/* INCLUDE_COLDFIRE_TIMESTAMP */
