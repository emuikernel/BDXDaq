/* pccTimer.c - PCC Timer library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01i,22jul96,dat  merged with timestamp driver.
01h,06jun96,wlf  doc: cleanup.
01g,16apr94,dzb  changed the file name from pccTimer.c to pccTimerTS.c
01f,07apr94,dzb  optimized timestamp routines.
01e,10jan94,dzb  added conditional compilation for INCLUDE_TIMESTAMP macro.
01d,17sep93,dzb  added timestamp support.
01c,07jul92,ccc  changed genericTimer.h to timerDev.h.
01b,26jun92,ccc  changed include to genericTimer.h.
01a,10jun92,ccc  created by moving routines from sysLib.c of mv147, ansified.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
PCC chip with a board-independent interface.  This library handles both
the system clock and the auxiliary clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

To include the timestamp timer facility, the macro INCLUDE_TIMESTAMP must be
defined.  When the timestamp timer is running, tick timer #2 is reset every
system clock interrupt in an effort to control time skew.  Note that changing
the system clock rate will affect the timestamp timer period, which can be
read by calling sysTimestampPeriod().

NOTE: the auxiliary clock and timestamp timer share the same physical timer
(tick timer #2).  Only one of these facilities may used at a time.  The
applicable disable routine (sysAuxClkDisable() or sysTimestampDisable())
can be used to relinquish control of tick timer #2, and the other facility
may then be enabled.  If either clock is active, attempting to activate
the other clock will fail.
*/

#include "drv/timer/timerDev.h"
#include "drv/timer/timestampDev.h"

/* Locals */

#define TICK_FREQ         160000

LOCAL int     sysClkTicksPerSecond  = 60;
LOCAL BOOL    sysClkRunning         = FALSE;
LOCAL FUNCPTR sysClkRoutine         = NULL;
LOCAL int     sysClkArg             = NULL;
LOCAL BOOL    sysClkConnected       = FALSE;

LOCAL int     auxClkTicksPerSecond  = 60;
LOCAL BOOL    sysAuxClkRunning      = FALSE;
LOCAL FUNCPTR sysAuxClkRoutine      = NULL;
LOCAL int     sysAuxClkArg          = NULL;
LOCAL BOOL    auxClkConnected       = FALSE;

LOCAL BOOL    sysTimestampRunning   = FALSE;		/* running flag */

/*******************************************************************************
*
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
*/

LOCAL void sysClkInt (void)
    {
    /* reset clock interrupt */

    /* clear overflow counter, enable & start counter */
    *TIC_1_CSR = TIC_1_CSR_CLR_OVF |
                 TIC_1_CSR_ENABLE;

    *TIC_1_INT_CTL = INT_LVL_TIC_1       |
                     TIC_1_INT_CTL_CLEAR |
                     TIC_1_INT_CTL_ENABLE; /* reset & enable int, set level */

#ifdef	INCLUDE_TIMESTAMP
    if (sysTimestampRunning)		/* sync up timestamp timer */
	sysTimestampEnable ();
#endif	/* INCLUDE_TIMESTAMP */

    if (sysClkRoutine != NULL)
        (*sysClkRoutine) (sysClkArg);
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
    FUNCPTR routine,    /* routine called at each system clock interrupt */
    int arg             /* argument with which to call routine           */
    )
    {
    sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */

    sysClkConnected = TRUE;

    sysClkRoutine   = routine;
    sysClkArg       = arg;

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
        /* disable interrupts */

        *TIC_1_INT_CTL = TIC_1_INT_CTL_DISABLE;
        sysClkRunning = FALSE;
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
    if (!sysClkRunning)
        {
        /* preload the preload register */
        *TIC_1_PRELOAD = (short) (65536 - (TICK_FREQ / sysClkTicksPerSecond));

        /* enable the clock interrupt. */

        *TIC_1_CSR = TIC_1_CSR_STOP;   /* load counter from preload register */

        /* clear overflow counter, enable & start counter */
        *TIC_1_CSR = TIC_1_CSR_CLR_OVF |
                     TIC_1_CSR_ENABLE;

        *TIC_1_INT_CTL = TIC_1_INT_CTL_CLEAR  |
                         TIC_1_INT_CTL_ENABLE |
                         INT_LVL_TIC_1;  /* reset&enable int, set level */

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

/********************************************************************************
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be
* set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond     /* number of clock interrupts per second */
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
* sysAuxClkInt - handle auxiliary clock interrupts
*/

LOCAL void sysAuxClkInt (void)
    {
    /* reset clock interrupt */

    /* clear overflow counter, enable & start counter */
    *TIC_2_CSR = TIC_2_CSR_CLR_OVF |
                 TIC_2_CSR_ENABLE;

    *TIC_2_INT_CTL = TIC_2_INT_CTL_CLEAR  |
                     TIC_2_INT_CTL_ENABLE |
                     INT_LVL_TIC_2;     /* reset & enable int, set level */

    if (sysAuxClkRoutine != NULL)
        (*sysAuxClkRoutine) (sysAuxClkArg);
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
    FUNCPTR routine,    /* routine called at each aux clock interrupt    */
    int arg             /* argument to auxiliary clock interrupt routine */
    )
    {
    auxClkConnected  = TRUE;

    sysAuxClkRoutine = routine;
    sysAuxClkArg     = arg;

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
    if ((sysAuxClkRunning) && (!sysTimestampRunning))
        {

        /* clear overflow counter & stop counter */

        *TIC_2_CSR     = TIC_2_CSR_CLR_OVF |
                         TIC_2_CSR_STOP;

        /* reset & disable int, set level */

        *TIC_2_INT_CTL = TIC_2_INT_CTL_CLEAR |
                         TIC_2_INT_CTL_DISABLE;

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
    if ((!sysAuxClkRunning) && (!sysTimestampRunning))
        {
        /* preload the preload register */
        *TIC_2_PRELOAD = (short) (65536 - (TICK_FREQ / auxClkTicksPerSecond));

        /* enable the clock interrupt */

        *TIC_2_CSR     = TIC_2_CSR_STOP; /* load counter from preload reg */

        /* clear overflow counter, enable & start counter */
        *TIC_2_CSR     = TIC_2_CSR_CLR_OVF |
                         TIC_2_CSR_ENABLE;

        *TIC_2_INT_CTL = TIC_2_INT_CTL_CLEAR  |
                         TIC_2_INT_CTL_ENABLE |
                         INT_LVL_TIC_2;   /* reset&enable int, set level */

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
    int ticksPerSecond     /* number of clock interrupts per second */
    )
    {
    if (sysTimestampRunning)
	return (ERROR);

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

#ifdef	INCLUDE_TIMESTAMP

/*******************************************************************************
*
* sysTimestampConnect - connect a user routine to the timestamp timer interrupt
*
* This routine usually specifies the user interrupt routine to be called at
* each timestamp timer interrupt.  In this case, however, the timestamp timer
* interrupt is not used since the timer is reset by the system clock before
* an interrupt can be generated.
*
* RETURNS: OK, or ERROR if sysTimestampInt() interrupt handler is not used.
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
* sysTimestampEnable - initialize and enable the timestamp timer
*
* This routine disables the timestamp timer interrupt and initializes the
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
        *TIC_2_CSR = TIC_2_CSR_STOP;		/* clear the counter */
        *TIC_2_CSR = TIC_2_CSR_ENABLE;		/* enable the timer */
        return (OK);
        }

    if ((sysAuxClkRunning) || (!sysClkRunning))
	return (ERROR);

    sysTimestampRunning = TRUE;

    /*
     * Disable the timer interrupt.  The timestamp timer is reset by the
     * system clock before rollover occurs.
     */

    *TIC_2_INT_CTL = TIC_2_INT_CTL_DISABLE;

    *TIC_2_PRELOAD = 0x0;		/* highest period -> freerunning */

    *TIC_2_CSR = TIC_2_CSR_STOP;	/* clear the counter */
    *TIC_2_CSR = TIC_2_CSR_ENABLE;	/* enable the timer */

    return (OK);
    }

/*******************************************************************************
*
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
    if (sysAuxClkRunning)
        return (ERROR);

    if (sysTimestampRunning)
	{
        sysTimestampRunning = FALSE;

        *TIC_2_CSR = TIC_2_CSR_DISABLE;		/* disable the timer */
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
    return (TICK_FREQ / sysClkTicksPerSecond);
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

    timerFreq = TICK_FREQ;

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
    UINT32 tick = 0;
    register short * pCounter;

    if (sysTimestampRunning)
	{
	pCounter = TIC_2_COUNTER;

        *TIC_2_CSR = TIC_2_CSR_DISABLE;		/* disable the timer */

        tick = *pCounter;			/* read tick counter value */

        *TIC_2_CSR = TIC_2_CSR_ENABLE;		/* enable the timer */

	tick &= 0x0000ffff;
	}

    return (tick);
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
    UINT32 tick = 0;
    register short * pCounter;
    int lockKey;

    if (sysTimestampRunning)
	{
	pCounter = TIC_2_COUNTER;

        lockKey = intLock ();			/* LOCK INTERRUPTS */

        *TIC_2_CSR = TIC_2_CSR_DISABLE;		/* disable the timer */

        tick = *pCounter;			/* read tick counter value */

        *TIC_2_CSR = TIC_2_CSR_ENABLE;		/* enable the timer */

        intUnlock (lockKey);			/* UNLOCK INTERRUPTS */

	tick &= 0x0000ffff;
	}

    return (tick);
    }

#endif	/* INCLUDE_TIMESTAMP */
