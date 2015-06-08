/* mccTimer.c - MCC Timer library */

/* Copyright 1984-1996 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,22jul96,dat  merged in timestamp functions, from mccTimerTS.c.
01d,16apr94,dzb  changed the file name from mccTimer.c to mccTimerTS.c
01c,10jan94,dzb  added conditional compilation for INCLUDE_TIMESTAMP macro
		 (SPR #2773).
01b,17sep93,dzb  added timestamp timer support.
01a,07jan93,ccc  created.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
MCchip with a board-independent interface.  This library handles
the system clock, auxiliary clock, and timestamp timer facilities.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

To include the timestamp timer facility, the macro INCLUDE_TIMESTAMP must be
defined.  The macro TIMESTAMP_LEVEL must be defined to provide the timestamp
timer's interrupt level.
*/

#include "drv/timer/timerDev.h"
#include "drv/timer/timestampDev.h"

/* Locals */

LOCAL int     sysClkTicksPerSecond = 60;
LOCAL BOOL    sysClkRunning        = FALSE;
LOCAL FUNCPTR sysClkRoutine        = NULL;
LOCAL int     sysClkArg            = NULL;
LOCAL BOOL    sysClkConnected      = FALSE;

LOCAL int     auxClkTicksPerSecond = 100;
LOCAL BOOL    sysAuxClkRunning     = FALSE;
LOCAL FUNCPTR sysAuxClkRoutine     = NULL;
LOCAL int     sysAuxClkArg         = NULL;
LOCAL BOOL    auxClkConnected      = FALSE;

#ifdef	INCLUDE_TIMESTAMP

LOCAL BOOL    sysTimestampRunning  = FALSE;	/* running flag */
LOCAL FUNCPTR sysTimestampRoutine  = NULL;	/* user rollover routine */
LOCAL int     sysTimestampArg	   = NULL;	/* arg to user routine */

#ifndef TIMESTAMP_LEVEL
#   define TIMESTAMP_LEVEL 1
#endif /*!TIMESTAMP_LEVEL*/
#endif	/* INCLUDE_TIMESTAMP */

/*******************************************************************************
*
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
*/

LOCAL void sysClkInt (void)

    {
    /* reset clock interrupt */
    *MCC_T1_IRQ_CR |=  T1_IRQ_CR_ICLR;

    if (sysClkRoutine != NULL)
        (*sysClkRoutine) (sysClkArg);
    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  It does not enable system clock interrupts.  Normally,
* it is called from usrRoot() in usrConfig.c to connect usrClock() to the
* system clock interrupt.
*
* RETURN: OK, always.
*
* SEE ALSO: usrClock(), sysClkEnable()
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

        *MCC_T1_IRQ_CR = 0;
        *MCC_TIMER1_CR = 0;    /* and disable counter */

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
        /* load compare register with the number of micro seconds */

        *MCC_TIMER1_CMP = 1000000 / sysClkTicksPerSecond;

        /* enable the clock interrupt. */

        *MCC_TIMER1_CNT = 0;           /* clear counter */

        /* enable interrupt, clear any pending, and set IRQ level */

        *MCC_T1_IRQ_CR = T1_IRQ_CR_IEN         |
                         T1_IRQ_CR_ICLR        |
                         SYS_CLK_LEVEL;

        /* now enable timer */

        *MCC_TIMER1_CR = TIMER1_CR_CEN         |
                         TIMER1_CR_COC;

        sysClkRunning = TRUE;
        }
    }

/*******************************************************************************
*
* sysClkRateGet - get the system clock rate
*
* This routine returns the interrupt rate of the system clock.
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
* enable system clock interrupts.  Normally, it is called by usrRoot() in
* usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid.
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
*
* This is the interrupt service routine for auxilliary clock interrupts.
*
* NOMANUAL
*/

LOCAL void sysAuxClkInt (void)

    {
    /* reset clock interrupt */
    *MCC_T2_IRQ_CR |=  T2_IRQ_CR_ICLR;

    if (sysAuxClkRoutine != NULL)
        (*sysAuxClkRoutine) (sysAuxClkArg);
    }

/*******************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock interrupts.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysAuxClkEnable()
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
    if (sysAuxClkRunning)
        {
        /* disable interrupts */

        *MCC_T2_IRQ_CR = 0;
        *MCC_TIMER2_CR = 0;

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
    if (!sysAuxClkRunning)
        {
        /* load compare register with the number of micro seconds per tick */

        *MCC_TIMER2_CMP = 1000000 / auxClkTicksPerSecond;

        /* enable the clock interrupt */

        *MCC_TIMER2_CNT = 0;   /* clear counter */

        /* enable interrupt, clear any pending, and set IRQ level */

        *MCC_T2_IRQ_CR = T2_IRQ_CR_IEN         |
                         T2_IRQ_CR_ICLR        |
                         AUX_CLK_LEVEL;

        /* now enable timer */

        *MCC_TIMER2_CR = TIMER2_CR_CEN         |
                         TIMER2_CR_COC;

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
* RETURNS: OK, or ERROR if the tick rate is invalid.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond     /* number of clock interrupts per second */
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

#ifdef	INCLUDE_TIMESTAMP

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
    *MCC_T3_IRQ_CR |= T3_IRQ_CR_ICLR;	/* acknowledge timer interrupt */

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
    static BOOL beenHere = FALSE;

    if (sysTimestampRunning)
	{
	*MCC_TIMER3_CNT = 0;			/* clear the counter */
	return (OK);
	}

    if (!beenHere)
	{
	intConnect (INUM_TO_IVEC (MCC_INT_VEC_BASE + MCC_INT_TT3),
			   sysTimestampInt, NULL);
	beenHere = TRUE;
	}

    sysTimestampRunning = TRUE;

    /* reset & enable the timestamp timer interrupt */

    *MCC_T3_IRQ_CR = T3_IRQ_CR_IEN | T3_IRQ_CR_ICLR | TIMESTAMP_LEVEL;

    *MCC_TIMER3_CMP = sysTimestampPeriod ();	/* set the timer period */

    *MCC_TIMER3_CNT = 0;			/* clear the counter */
    *MCC_TIMER3_CR = TIMER3_CR_CEN;		/* enable the timer */

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
        *MCC_TIMER3_CR = TIMER3_CR_DIS;		/* disable the timer */

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

    if (*MCC_VERSION_REG & VERSION_REG_SPEED)	/* check clock speed */
        Bclk = 33000000;
    else
	Bclk = 25000000;

    timerFreq = Bclk/(256 - (*MCC_PRESCALE_CLK_ADJ & 0xff));

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
    return (*MCC_TIMER3_CNT);
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
    return (*MCC_TIMER3_CNT);
    }

#endif	/* INCLUDE_TIMESTAMP */
