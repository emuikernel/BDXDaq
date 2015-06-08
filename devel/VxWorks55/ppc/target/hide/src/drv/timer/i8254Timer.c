/* i8254Timer.c - Intel 8254 timer library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,13mar97,kkk  fixed warnings. 
01c,19apr95,ism  added WindView timestamp support
01b,14mar94,caf  fixed sysClkDisable() to clear counter 0 (SPR #2859).
01b,14mar94,caf  changed intDisable() to intLock() (SPR #2858).
01a,28sep92,caf  created by moving routines from version 02e of star/sysLib.c,
		 ansified.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
Intel 8254 chip with a board-independent interface.  This library handles
both the system clock and the auxiliary clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

If the board includes an external write buffer, define SYS_WB_FLUSH when
including this driver and implement sysWbFlush() in the BSP.

INTERNAL
This driver is not completely board-independent.  It relies on several
macros found in the star BSP.  The way interrupts are controlled may also
reflect the organization of MIPS R3000 targets such as the star.
*/

/* defines */

#define PERIOD(x)	(((10 * TIMER2_HZ / (x)) + 5) / 10)	/* rounded */

/* locals */

LOCAL FUNCPTR sysClkRoutine	= NULL;	/* routine to call on clock interrupt */
LOCAL int sysClkArg		= NULL;	/* its argument */
LOCAL int sysClkTicksPerSecond	= 60;	/* system clock rate */
LOCAL BOOL sysClkConnected	= FALSE;/* hooked up to interrupt yet? */
LOCAL BOOL sysClkRunning	= FALSE;/* system clock enabled? */

LOCAL FUNCPTR sysAuxClkRoutine	= NULL;	/* routine to call on clock interrupt */
LOCAL int sysAuxClkArg		= NULL;	/* its argument */
LOCAL int auxClkTicksPerSecond	= 60;	/* auxiliary clock rate */
LOCAL BOOL auxClkConnected	= FALSE;/* hooked up to interrupt yet? */
LOCAL BOOL auxClkRunning	= FALSE;/* auxiliary clock enabled? */

#ifdef INCLUDE_TIMESTAMP

LOCAL BOOL    sysTimestampRunning	= FALSE; /* running flag */
LOCAL int     sysTimestampPeriodValue	= NULL;  /* Max counter value */

UINT32        sysTimestamp (void);

#endif  /* INCLUDE_TIMESTAMP */

/*******************************************************************************
*
* sysClkInt - interrupt level processing for timer 0.
*
* This routine handles the interrupts associated with the Intel 8254 chip.
* It is attached to the interrupt vector by sysClkConnect().
*/

LOCAL void sysClkInt (void)
    {
    FAST char staleValue;		/* dummy read character */

    if (sysClkRoutine != NULL)
        (* sysClkRoutine) (sysClkArg);	/* call system clock routine */

    staleValue = *(volatile char *) TIM0_ACK_ADDR;	/* acknowledge int */
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
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,	/* routine called at each system clock interrupt */
    int     arg		/* argument with which to call with routine      */
    )
    {
    sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */

    sysClkRoutine = routine;
    sysClkArg     = arg;
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
    FAST volatile TIMER *pTimer = (volatile TIMER *) TIMER_BASE_ADRS;
    FAST ULONG statusReg;

    statusReg = intLock ();	/* LOCK INTERRUPTS */

    if (sysClkRunning)
	{
	/* stop timer 0, actually we get one more interrupt after this */

        pTimer->cntrl_word = CW_SELECT (0) | CW_BOTHBYTE | 
	CW_MODE (MD_SWTRIGSB);
        pTimer->counter0 = LSB (0);
        pTimer->counter0 = MSB (0);

#ifdef	SYS_WB_FLUSH
	sysWbFlush ();
#endif	/* SYS_WB_FLUSH */

#ifdef  INCLUDE_TIMESTAMP
	sysTimestampRunning = FALSE;
#endif  /* INCLUDE_TIMESTAMP */

	sysClkRunning = FALSE;
	}

    intUnlock (statusReg);	/* UNLOCK INTERRUPTS */
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
    FAST volatile TIMER *pTimer = (volatile TIMER *) TIMER_BASE_ADRS;
    /* Initialize timer 0, primary clock */

    if (!sysClkRunning)
	{
	/* setup device */

        pTimer->cntrl_word = CW_SELECT (0) | CW_BOTHBYTE | CW_MODE (MD_RATEGEN);
        pTimer->counter0 = LSB (PERIOD (sysClkTicksPerSecond));
        pTimer->counter0 = MSB (PERIOD (sysClkTicksPerSecond));

#ifdef	SYS_WB_FLUSH
	sysWbFlush ();
#endif	/* SYS_WB_FLUSH */

        sysClkRunning = TRUE;
        intEnable (INT_LVL_TIMER0);	/* turn on timer 0 ints just in case */
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
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
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
* SEE ALSO: intConnect(), sysClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,	/* routine called at each aux. clock interrupt */
    int     arg		/* argument with which to call routine         */
    )
    {
    sysAuxClkRoutine   = routine;
    sysAuxClkArg       = arg;
    auxClkConnected  = TRUE;

    return (OK);
    }

/********************************************************************************
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
    FAST volatile TIMER *pTimer = (volatile TIMER *) TIMER_BASE_ADRS;
    FAST ULONG statusReg;

    statusReg = intLock ();	/* turn of timer 1 ints just in case */

    if (auxClkRunning)
	{
	/* stop timer 1, actually we get one more interrupt after this */
     
        pTimer->cntrl_word = CW_SELECT (1) | CW_BOTHBYTE | 
	    CW_MODE (MD_SWTRIGSB);
        pTimer->counter1 = LSB (0);
        pTimer->counter1 = MSB (0);

#ifdef	SYS_WB_FLUSH
	sysWbFlush ();
#endif	/* SYS_WB_FLUSH */

	auxClkRunning = FALSE;
	}

    intUnlock (statusReg);
    }

/********************************************************************************
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable(), susAuxClkRateSet()
*/
 
void sysAuxClkEnable (void)
    {
    FAST volatile TIMER *pTimer = (volatile TIMER *) TIMER_BASE_ADRS;

    if (!auxClkRunning)
	{

        /* Initialize timer 1, aux clock */
        pTimer->cntrl_word = CW_SELECT (1) | CW_BOTHBYTE | CW_MODE (MD_RATEGEN);
        pTimer->counter1 = LSB (PERIOD (auxClkTicksPerSecond));
        pTimer->counter1 = MSB (PERIOD (auxClkTicksPerSecond));

#ifdef	SYS_WB_FLUSH
	sysWbFlush ();
#endif	/* SYS_WB_FLUSH */
 
        auxClkRunning = TRUE;
        intEnable (INT_LVL_TIMER1);	/* turn on timer 1 ints just in case */
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
    FAST int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
        return (ERROR);

    auxClkTicksPerSecond = ticksPerSecond;
	     
    if (auxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }

#ifdef  INCLUDE_TIMESTAMP

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
    int arg     /* argument with which to call routine */
    )
    {
	return (ERROR);
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
    sysTimestampPeriodValue = TIMER2_HZ / sysClkTicksPerSecond;

    if (sysTimestampRunning)
    return (OK);

    sysTimestampRunning = TRUE;

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
        sysTimestampRunning = FALSE;

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
    sysTimestampPeriodValue = TIMER2_HZ / sysClkTicksPerSecond;

    return (sysTimestampPeriodValue);
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
    return (TIMER2_HZ);
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
    unsigned int count;
    FAST volatile TIMER *pTimer = (volatile TIMER *) TIMER_BASE_ADRS;

    /* select timer 0, and latch current count */
    pTimer->cntrl_word = CW_SELECT (0) | CW_COUNTLCH |
	CW_MODE (MD_RATEGEN);
    count = (unsigned int) pTimer->counter0;
    count += (((unsigned int) pTimer->counter0) << 8);

    count = (sysTimestampPeriodValue-1) - count;

    return (count);
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
    FAST ULONG statusReg;
    unsigned int count;
    FAST volatile TIMER *pTimer = (volatile TIMER *) TIMER_BASE_ADRS;

    statusReg = intLock ();	/* turn of timer 1 ints just in case */

    /* select timer 0, and latch current count */
    pTimer->cntrl_word = CW_SELECT (0) | CW_COUNTLCH |
        CW_MODE (MD_RATEGEN);
    count = (unsigned int) pTimer->counter0;
    count += (((unsigned int) pTimer->counter0) << 8);

    intUnlock (statusReg);

    count = (sysTimestampPeriodValue-1) - count;

    return (count);
    }

#endif  /* INCLUDE_TIMESTAMP */
