/* ultrasparcTimer.c - ultrasparc timer library */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,02dec99,dra  update to new toolchain.
01b,16feb99,dra  minor change to prevent round-off error when calculating
		 sysClkProcessorTicks.
01a,19jan98,mem  written, based on mipsR4kTimer.c.
*/

/*
DESCRIPTION
This library provides timer routines for the on-chip timer on
UltraSPARC processors.  This library handles system clock and
timestamp functions.  This library does not provide auxillary clock
functions.

The macros SYS_CLK_RATE_MIN and SYS_CLK_RATE_MAX must be defined to
provide parameter checking for the sysClkRateSet() routines.

To include the timestamp timer facility, the macro INCLUDE_TIMESTAMP
must be defined. Note that changing the system clock rate will affect
the timestamp timer period, which can be read by calling
sysTimestampPeriod().

This driver requires the external routines sysTickCmprSet(),
sysTickCmprGet(), sysTickGet() to be defined.  Also the value
CPU_CLOCK_RATE must be defined in the BSP.

The variable sysSoftCompare needs to be defined as global, because
this variable gets used by the sysCompareSet() routine in sysALib.s of
the BSP.

INCLUDE FILES: timestampDev.h

SEE ALSO:
.pG "Configuration" */

/* defines */

#define TICK_INT	(1 << 0)		/* TICK interrupt in SOFTINT */
#define TICK_DIS	0x8000000000000000LL	/* disables TICK interrupt */
#define TICK_CNT_MASK	0x7fffffffffffffffLL	/* mask of valid TICK bits */

/* forwards */

IMPORT	ULLONG sysTickGet ();
IMPORT	ULLONG sysTickCmprGet ();
IMPORT	void sysTickCmprSet (ULLONG);

/* locals */

LOCAL FUNCPTR sysClkRoutine	= NULL;		/* routine for clock tick */
LOCAL int sysClkArg		= 0;		/* its argument */
LOCAL BOOL sysClkConnected	= FALSE;	/* sys clock connect flag */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkTicksPerSecond	= 60;
LOCAL ULLONG sysSoftCompare	= 0;
LOCAL ULLONG sysClkProcessorTicks;		/* UltraSPARC cpu ticks */

/*****************************************************************************
*
* sysClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkDisable(), sysClkRateSet()
*/

void sysClkEnable (void)
    {
    int key;

    /* start the timer */

    sysClkProcessorTicks = ((CPU_CLOCK_RATE + (sysClkTicksPerSecond/2))
			    / sysClkTicksPerSecond);
    key = intLock ();
    sysSoftCompare = (sysTickGet() & TICK_CNT_MASK) + sysClkProcessorTicks;
    sysTickCmprSet (sysSoftCompare & ~TICK_DIS);
    sysClkRunning = TRUE;
    intUnlock (key);
    }

/*****************************************************************************
*
* sysClkRateGet - get the system clock rate
*
* This routine returns the interrupt rate of the system clock.
*
* RETURNS: The number of ticks per second of the system clock.
*
* SEE ALSO: sysClkRateSet()
*/

int sysClkRateGet (void)
    {
    return (sysClkTicksPerSecond);
    }

/*****************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.  It does not
* enable system clock interrupts.  Normally, it is called by usrRoot() in
* usrConfig.c.
*
* NOTE: The UltraSPARC internal timer is used to provide the system clock.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be
* set.
*
* SEE ALSO: sysClkDisable(), sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond  /* number of clock interrupts per second */
    )
    {
    /* parameter checking */

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

/*****************************************************************************
*
* sysClkInt - handle a system clock interrupt
*
* This routine handles a system clock interrupt.  It increments the value
* on the front panel display and calls the routine installed by
* sysClkConnect().
*
* RETURNS: N/A
*/

LOCAL void sysClkInt (void)
    {
    int key;
    ULLONG now;
    ULLONG delta;	/* time to when next interrupt should be */

    /* see sysClkDisable(). Clear interrupt and return. */
    if (!sysClkRunning)
        {
	sysSoftIntClear (TICK_INT);
        return;
        }

    key = intLock();

    /* use unsigned arithmetic to compensate for wrap-around */

    now = (sysTickGet() & TICK_CNT_MASK);
    /* delta is how long ago the interrupt occured. */
    delta = now - sysSoftCompare;
    if (delta < (sysClkProcessorTicks - 0xf000))
        {
        /*
         * case 1 (normal): we haven't missed the next interrupt.
         *  The 0xf000 above is an estimate of how far count will advance
         *  between reading it above and setting the compare register
         *  below (count should not pass the new compare value before
         *  we've set the register).  sysClkProcessorTicks is on
         *  the order of 500,000.
         */
        sysSoftCompare += sysClkProcessorTicks;
        sysTickCmprSet (sysSoftCompare);
	sysSoftIntClear (TICK_INT);
        }
    else if (delta < (16 * sysClkProcessorTicks))
        {
        /*
         * case 2 (missed a couple): we've missed some interrupts,
         *   don't set the compare register (so we'll take another
         *   interrupt as soon as we return).
         */
        sysSoftCompare += sysClkProcessorTicks;
	}
    else
        {
        /*
         * case 3 (missed a lot): set the next interrupt at
         *   sysClkProcessorTicks past the current time.
         */
        sysSoftCompare = (sysTickGet() & TICK_CNT_MASK) + sysClkProcessorTicks;
        sysTickCmprSet (sysSoftCompare);
	sysSoftIntClear (TICK_INT);
        }
    intUnlock (key);

    if (sysClkRoutine != NULL)
        (*sysClkRoutine) (sysClkArg);
    }


/*****************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt handler to be called at each system
* clock interrupt.  It does not enable system clock interrupts.  Normally,
* it is called from usrRoot() in usrConfig.c to connect usrClock() to the
* system clock interrupt.  Other standard interrupt handlers are also set up
* at this time.
*
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,    /* routine called at each system clock interrupt */
    int arg             /* argument with which to call routine           */
    )
    {
    if (sysClkConnected == FALSE)
        {
        sysHwInit2 ();
        sysClkConnected = TRUE;
        }

    sysClkRoutine   = routine;
    sysClkArg       = arg;

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
* SEE ALSO: sysClkEnable(), sysClkRateSet()
*/

void sysClkDisable (void)
    {
    int key;

    key = intLock ();
    sysClkRunning = FALSE;
    sysTickCmprSet (TICK_DIS);
    intUnlock (key);
    }

#ifdef  INCLUDE_TIMESTAMP

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
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampDisable()
*/

STATUS sysTimestampEnable (void)
   {
   return (OK);
   }

/******************************************************************************
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
    return (OK);
    }

/******************************************************************************
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
    return (CPU_CLOCK_RATE/sysClkTicksPerSecond);
    }

/******************************************************************************
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
    return (CPU_CLOCK_RATE);
    }

/******************************************************************************
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
    return (sysTickGet() - (sysSoftCompare - sysClkProcessorTicks) + 1);
    }

/******************************************************************************
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
    UINT32 ts;
    int oldLevel;

    oldLevel = intLock ();
    ts = (sysTickGet() - (sysSoftCompare - sysClkProcessorTicks) + 1);
    intUnlock (oldLevel);
    return (ts);
    }

#endif  /* INCLUDE_TIMESTAMP */
