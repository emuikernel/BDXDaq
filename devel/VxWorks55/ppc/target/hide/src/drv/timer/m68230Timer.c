/* m68230Timer.c - MC68230 Parallel Interface/Timer (PIT) timer driver */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01g,25oct96,wlf  doc: cleanup.
01f,22jul96,dat  merged in timestamp driver.
01e,16apr94,dzb  changed the file name from m68230Timer.c m68230TimerTS.c
01d,07apr94,dzb  added delay loop for initial timer enable.
01c,01feb94,dzb  added interrupt locks around timer reset.
                 added PIT_TS_RESET_DELAY delay loop (SPR #2997).
01b,10jan94,dzb  added timestamp driver support for Force boards (2 PI/T's).
01a,26jan93,caf  derived from version 01o of frc30/sysLib.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
MC68230 (PIT) chip with a board-independent interface.  This library handles
either the auxiliary clock facility or the timestamp timer facility.  To
support the timestamp timer facility, two PIT chip timers are required.

To support the auxiliary clock, the macro PIT_BASE_ADRS must be defined
to indicate the base address of the PIT control registers. Additionally,
the macro PIT_HZ must be defined to indicate the frequency at which the
PIT hardware operates.  The macros AUX_CLK_RATE_MIN and AUX_CLK_RATE_MAX
must be defined to provide parameter checking for sysAuxClkRateSet().

To include the timestamp timer facility, the macro INCLUDE_TIMESTAMP must be
defined, and the macros FRC_PIT1_BASE_ADRS and FRC_PIT2_BASE_ADRS must be
defined to indicate the base addresses of the two required PIT chips.
The PIT #2 timer is used as the timestamp counter, but must be stopped to be
read, thereby introducing time skew.  The PIT #1 timer periodically resets
the PIT #2 timer in an effort to alleviate cumulative time skew.  In addition
to resetting the PIT #2 timer, the PIT #1 timer interrupt is used to
alert the user to a timestamp timer reset (analogous to a timestamp rollover
event).

The PIT_TS_CORRECTION_PERIOD macro defines the period of the PIT
#1 timer, which translates to the period of the timestamp timer reset
(analogous to a timestamp rollover event).  The PIT_TS_SKEW macro
can be used to compensate for time skew incurred when the PIT #2 timer
is stopped in sysTimestamp() and sysTimestampLock().  The value
of PIT_TS_SKEW is added to the stopped timestamp counter in an attempt to 
make up for "lost" time.  The correct value to adjust the timestamp counter
is not only board-dependent, it is influenced by CPU speed, caching mode,
memory speed, etc.  The PIT_TS_RESET_DELAY macro defines the length of
a wait loop in sysTimestampEnable().  The wait loop should be used on
extremely fast target boards where a delay of execution is necessary for
the timestamp timer operation to take effect.

The timestamp timer and auxiliary clock facility must exist on separate PIT
chip timers if they are to run concurrently.  That is, a particular PIT timer
may not be used for both the auxiliary clock and the timestamp timer at the
same time.
*/

/* includes */

#include "drv/timer/timerDev.h"
#include "drv/timer/timestampDev.h"
#include "drv/multi/m68230.h"

#ifdef	INCLUDE_TIMESTAMP

/* defines */

#ifndef	PIT_TS_CORRECTION_PERIOD
#define PIT_TS_CORRECTION_PERIOD 0x00008fff /* timestamp skew correction per. */
#endif	/* PIT_TS_CORRECTION_PERIOD */

#ifndef	PIT_TS_SKEW
#define PIT_TS_SKEW 0			/* timestamp skew correction time */
#endif	/* PIT_TS_SKEW */

/* locals */

LOCAL BOOL    sysTimestampRunning = FALSE;	/* running flag */
LOCAL FUNCPTR sysTimestampRoutine = NULL;	/* user rollover routine */
LOCAL int     sysTimestampArg     = NULL;	/* arg to user routine */

#endif	/* INCLUDE_TIMESTAMP */

#ifdef	PIT_BASE_ADRS

/* locals */

LOCAL FUNCPTR sysAuxClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysAuxClkArg		= NULL; /* its argument */
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int auxClkConnected	= FALSE;
LOCAL int auxClkTicksPerSecond	= 60;

#endif	/* PIT_BASE_ADRS */


#ifdef	PIT_BASE_ADRS

/*******************************************************************************
*
* sysAuxClkInt - interrupt level processing for auxiliary clock
*
* This routine handles auxiliary clock interrupts.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void sysAuxClkInt (void)
    {
    *PIT_TSR (PIT_BASE_ADRS) = PIT_ACK_INTR;    /* ack timer interrupt */

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
    FUNCPTR  routine,	/* routine called at each aux. clock interrupt */
    int      arg	/* argument to auxiliary clock interrupt routine */
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
	* PIT_TCR (PIT_BASE_ADRS)   = TIMER_CTL_5;	/* disable timer */
	sysAuxClkRunning	    = FALSE;
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
* SEE ALSO: sysAuxClkConnect(),sysAuxClkDisable(), sysAuxClkRateSet()
*/

void sysAuxClkEnable (void)
    {
    unsigned int tc;

    if (!sysAuxClkRunning)
	{
	/* divide-by-32 prescaler is used on CLK input */

        tc = PIT_HZ / (auxClkTicksPerSecond * 32);

        /* write the timer value */

        * PIT_CPRL (PIT_BASE_ADRS) = LSB(tc);
        * PIT_CPRM (PIT_BASE_ADRS) = MSB(tc);
        * PIT_CPRH (PIT_BASE_ADRS) = (tc >> 16) & 0xff;

        /* enable PIT1 timer */

        * PIT_TCR (PIT_BASE_ADRS) = TIMER_CTL_5 | TIMER_ENABLE;

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
#endif /*PIT_BASE_ADRS*/

#ifdef	INCLUDE_TIMESTAMP

/*******************************************************************************
*
* sysTimestampInt - correction timer interrupt handler
*
* This routine handles the correction timer interrupt.  A user routine is
* called, if one was connected by sysTimestampConnect().
*
* RETURNS: N/A
*/

LOCAL void sysTimestampInt (void)
    {
    /* acknowledge the timer interrupt */

    *PIT_TSR (FRC_PIT1_BASE_ADRS) = PIT_ACK_INTR;

    sysTimestampEnable ();

    if (sysTimestampRoutine != NULL)	/* call user connected routine */
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
    FUNCPTR routine,    /* routine called at each timestamp timer interrupt */
    int arg             /* argument with which to call routine */
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
    int lockKey;
#ifdef	PIT_TS_RESET_DELAY
    int ix;
#endif	/* PIT_TS_RESET_DELAY */

    if (sysTimestampRunning)
        {
        lockKey = intLock ();			/* LOCK INTERRUPTS */

        *PIT_CPR (FRC_PIT2_BASE_ADRS) = 0x00ffffff;

        /* disable the timer */

        *PIT_TCR (FRC_PIT2_BASE_ADRS) = TIMER_CTL_4;

        /* enable the timer */

        *PIT_TCR (FRC_PIT2_BASE_ADRS) = TIMER_CTL_4 | TIMER_ENABLE;

#ifdef	PIT_TS_RESET_DELAY
        /* wait for reset operation to take effect on FAST boards */

	for (ix = 0; ix < PIT_TS_RESET_DELAY; ix++)
	    ;
#endif	/* PIT_TS_RESET_DELAY */

        intUnlock (lockKey);			/* UNLOCK INTERRUPTS */

        return (OK);
        }

    /* connect interrupt handler for correction timer */

    (void) intConnect (INUM_TO_IVEC (INT_VEC_PIT1), sysTimestampInt, NULL);

    *PIT_TVIR (FRC_PIT1_BASE_ADRS) = INT_VEC_PIT1;

    sysTimestampRunning = TRUE;

    /* set the timer periods */
 
    *PIT_CPR (FRC_PIT1_BASE_ADRS) = PIT_TS_CORRECTION_PERIOD;
    *PIT_CPR (FRC_PIT2_BASE_ADRS) = 0x00ffffff;

    /* reset counters */

    /* enable the timers */

    *PIT_TCR (FRC_PIT1_BASE_ADRS) = TIMER_CTL_5 | TIMER_ENABLE;
    *PIT_TCR (FRC_PIT2_BASE_ADRS) = TIMER_CTL_4 | TIMER_ENABLE;

#ifdef	PIT_TS_RESET_DELAY
    /* wait for reset operation to take effect on FAST boards */

    for (ix = 0; ix < PIT_TS_RESET_DELAY; ix++)
        ;
#endif	/* PIT_TS_RESET_DELAY */

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
    if (sysTimestampRunning)
	{
        sysTimestampRunning = FALSE;

        /* disable both timers */

        *PIT_TCR (FRC_PIT1_BASE_ADRS) = TIMER_CTL_5;
        *PIT_TCR (FRC_PIT2_BASE_ADRS) = TIMER_CTL_4;
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
    /* set to reasonable correction period */

    return (PIT_TS_CORRECTION_PERIOD);
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

    timerFreq = CLOCK_FREQ / 32;

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
    register UINT32 * pTick;
    register UINT32 * pPreload;

    if (sysTimestampRunning)
        {
        pTick = PIT_CNTR (FRC_PIT2_BASE_ADRS);
        pPreload = PIT_CPR (FRC_PIT2_BASE_ADRS);

        *PIT_TCR (FRC_PIT2_BASE_ADRS) = TIMER_CTL_4;

        tick = *pTick;
        *pPreload = tick - PIT_TS_SKEW;

        *PIT_TCR (FRC_PIT2_BASE_ADRS) = TIMER_CTL_4 | TIMER_ENABLE;

        tick = 0xffffff - tick;
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
    register UINT32 * pTick;
    register UINT32 * pPreload;
    register UINT8 * pControl;
    int lockKey;

    if (sysTimestampRunning)
	{
        pTick = PIT_CNTR (FRC_PIT2_BASE_ADRS);
        pPreload = PIT_CPR (FRC_PIT2_BASE_ADRS);
        pControl = (UINT8 *) PIT_TCR (FRC_PIT2_BASE_ADRS);

        lockKey = intLock ();			/* LOCK INTERRUPTS */

        *pControl = TIMER_CTL_4;

        tick = *pTick;
        *pPreload = tick - PIT_TS_SKEW;

        *pControl = TIMER_CTL_4 | TIMER_ENABLE;

        intUnlock (lockKey);			/* UNLOCK INTERRUPTS */

        tick = 0xffffff - tick;
	}

    return (tick);
    }

#endif	/* INCLUDE_TIMESTAMP */
