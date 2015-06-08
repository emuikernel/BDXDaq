/* sa1501Timer.c - Digital Semiconductor SA-1501 timer library */

/* Copyright 1996-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,16apr99,jpd  fix bugs in sysTimestampLock and sysAuxClkDisable (SPR #26727).
01a,11nov98,jpd  Written, based on ambaTimer.c, version 01i.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on a
SA-1501 chip with a mostly board-independent interface. This driver
provides 3 main functions, system clock support, auxiliary clock
support, and timestamp timer support.  The timestamp function is always
conditional upon the INCLUDE_TIMESTAMP macro.

The SA-1501 chip is a companion to the SA-1500 Mediaprocessor, and is
found within the Acorn EBSA-1500 board.  It contains four timers,
clocked at the same rate to provide software programmable interval
timers via interrupt generation.

The four timers are 25-bit incrementers which have an associated limit
value register and a comparator between the two.  They increment
continuously until the timer limit value is reached, at which time they
flag an interrupt, reset to zero and then continue incrementing.

Timer 3 has a watchdog option that, once enabled, can reset the entire
system when it triggers. This is not used by VxWorks.

The SA-1501 timer registers are described below under the symbolic
names used herein.

At any time, the current timer values may be read from the Value
registers.


REGISTERS:

LIMIT: (read/write) The limit register extend from bits 24 down to 0.
Writing the register will set the limit value for the appropriate
timer.  Note that setting the limit value to less than the current
timer count value will cause the timer to count up to its maximum value
and wrap around.  Reads return the value previously programmed.
Programming a limit register to 0 will eventually cause the counter to
stop incrementing.  The counter will count from its current value up to
its maximum value, wrap around and then be continuously reloaded with
0, as if it has stopped incrementing.

VALUE: (read only) The value register gives the current value of the
timer on bits 24 down to 0.  Writing to this register when not in test
mode will have unpredictable results.

RESET: (read/write) Reading or writing to the reset register has the
effect of resetting the appropriate timer value to 0.  Reading returns
the appropriate current timer value.


The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.  The following macros must also be defined:

SYS_TIMER_CLK			/@ frequency of clock feeding SYS_CLK timer @/
AUX_TIMER_CLK			/@ frequency of clock feeding AUX_CLK @/
SYS_TIMER_LIMIT			/@ addresses of timer registers @/
SYS_TIMER_VALUE			/@ "" @/
SYS_TIMER_RESET			/@ "" @/
AUX_TIMER_LIMIT			/@ "" @/
AUX_TIMER_VALUE			/@ "" @/
AUX_TIMER_RESET			/@ "" @/
SYS_TIMER_INT_LVL		/@ interrupt level for sys Clk @/
AUX_TIMER_INT_LVL		/@ interrupt level for aux Clk @/

The following may also be defined, if required:
SA1501_TIMER_READ (reg, result)	/@ read a timer register @/
SA1501_TIMER_WRITE (reg, data)	/@ write ... @/
SA1501_TIMER_INT_ENABLE (level)	/@ enable an interrupt @/
SA1501_TIMER_INT_DISABLE (level) /@ disable an interrpt @/

BSP
Apart from defining such macros described above as are needed, the BSP
will need to connect the interrupt handlers (typically in sysHwInit2()).
e.g.

.CS
    /@ connect sys clock interrupt and auxiliary clock interrupt @/

    intConnect (INUM_TO_IVEC (INT_VEC...), sysClkInt, 0);
    intConnect (INUM_TO_IVEC (INT_VEC...), sysAuxClkInt, 0);
.CE

INCLUDES:
sa1501Timer.h
timestampDev.h

SEE ALSO:
.I "Digital Semiconductor SA-1501 Microprocessor Technical Manual"
*/


/* includes */

#include "drv/timer/sa1501Timer.h"
#include "drv/timer/timestampDev.h"


/* defines */

#ifndef SA1501_TIMER_READ
#define SA1501_TIMER_READ(reg, result) \
	((result) = *((volatile UINT32 *)(reg)))
#endif /* SA1501_TIMER_READ */

#ifndef SA1501_TIMER_WRITE
#define SA1501_TIMER_WRITE(reg, data) \
	(*((volatile UINT32 *)(reg)) = (data))
#endif /* SA1501_TIMER_WRITE */

#ifndef SA1501_TIMER_INT_ENABLE
#define SA1501_TIMER_INT_ENABLE(level) intEnable (level)
#endif

#ifndef SA1501_TIMER_INT_DISABLE
#define SA1501_TIMER_INT_DISABLE(level) intDisable (level)
#endif


/* locals */

LOCAL FUNCPTR sysClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysClkArg		= NULL; /* its argument */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkConnected	= FALSE;
LOCAL int sysClkTicksPerSecond	= 60;

LOCAL FUNCPTR sysAuxClkRoutine	= NULL;
LOCAL int sysAuxClkArg		= NULL;
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int sysAuxClkTicksPerSecond = 100;

#ifdef	INCLUDE_TIMESTAMP
LOCAL BOOL	sysTimestampRunning  	= FALSE;   /* timestamp running flag */
#endif /* INCLUDE_TIMESTAMP */

#if !defined (SYS_CLK_RATE_MIN) || !defined (SYS_CLK_RATE_MAX) || \
    !defined (AUX_CLK_RATE_MIN) || !defined (AUX_CLK_RATE_MAX) || \
    !defined (SYS_TIMER_CLK) || !defined (AUX_TIMER_CLK) || \
    !defined (SYS_TIMER_LIMIT) || !defined (AUX_TIMER_LIMIT) || \
    !defined (SYS_TIMER_VALUE) || !defined (SYS_TIMER_RESET) || \
    !defined (AUX_TIMER_VALUE) || !defined (AUX_TIMER_RESET) || \
    !defined (SYS_TIMER_INT_LVL) || \
    !defined (AUX_TIMER_INT_LVL)
#error missing #defines in sa1501Timer.c.
#endif

/*******************************************************************************
*
* sysClkInt - interrupt level processing for system clock
*
* This routine handles the system clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysClkConnect().
*
* RETURNS: N/A.
*/

LOCAL void sysClkInt (void)
    {
    /* If any routine is attached via sysClkConnect(), call it */

    if (sysClkRoutine != NULL)
	(* sysClkRoutine) (sysClkArg);
    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  It does not enable system clock interrupts.
* Normally it is called from usrRoot() in usrConfig.c to connect
* usrClock() to the system clock interrupt.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,	/* routine to be called at each clock interrupt */
    int arg		/* argument with which to call routine */
    )
    {
    if (sysClkConnected == FALSE)
    	{
    	sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */
    	sysClkConnected = TRUE;
    	}

    sysClkRoutine = NULL; /* ensure routine not called with wrong arg */
    sysClkArg	  = arg;

#if ((CPU_FAMILY == ARM) && ARM_THUMB)
    /* set b0 so that sysClkConnect() can be used from shell */

    sysClkRoutine = (FUNCPTR)((UINT32)routine | 1);
#else
    sysClkRoutine = routine;
#endif /* CPU_FAMILY == ARM */

    return OK;
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
	/* Disable timer itself: set limit register to 0 */

	SA1501_TIMER_WRITE (SYS_TIMER_LIMIT (SA1501_TIMER_BASE), 0);


	/* Disable the timer interrupt in the Interrupt Controller */

	SA1501_TIMER_INT_DISABLE (SYS_TIMER_INT_LVL);
	
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
    UINT32 tl;

    if (!sysClkRunning)
	{
	/*
	 * Calculate the timer limit value:
	 * counter limit value = (clock rate / sysClkTicksPerSecond)
	 */
	
	tl = (SYS_TIMER_CLK / sysClkTicksPerSecond);


	/* Load Limit value into Timer registers */

	SA1501_TIMER_WRITE (SYS_TIMER_LIMIT (SA1501_TIMER_BASE), tl);


	/* reset timer value to zero */

	SA1501_TIMER_WRITE (SYS_TIMER_RESET (SA1501_TIMER_BASE), 0);


	/* enable clock interrupt in interrupt controller */

	SA1501_TIMER_INT_ENABLE (SYS_TIMER_INT_LVL);
	
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
* SEE ALSO: sysClkRateSet(), sysClkEnable()
*/

int sysClkRateGet (void)
    {
    return sysClkTicksPerSecond;
    }

/*******************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.
* It does not enable system clock interrupts.
* Normally it is called by usrRoot() in usrConfig.c.
*
* RETURNS:
* OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkRateGet(), sysClkEnable()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
	return ERROR;

    sysClkTicksPerSecond = ticksPerSecond;

    if (sysClkRunning)
	{
	sysClkDisable ();
	sysClkEnable ();
	}

    return OK;
    }

/*******************************************************************************
*
* sysAuxClkInt - handle an auxiliary clock interrupt
*
* This routine handles an auxiliary clock interrupt.  It acknowledges the
* interrupt and calls the routine installed by sysAuxClkConnect().
*
* RETURNS: N/A
*/

LOCAL void sysAuxClkInt (void)
    {
    /* If any routine is attached via sysAuxClkConnect(), call it */

    if (sysAuxClkRoutine != NULL)
	(*sysAuxClkRoutine) (sysAuxClkArg);
    }

/*******************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It also connects the clock error interrupt
* service routine.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,    /* routine called at each aux clock interrupt */
    int arg             /* argument with which to call routine        */
    )
    {
    sysAuxClkRoutine = NULL;	/* ensure routine not called with wrong arg */
    sysAuxClkArg	= arg;

#if ((CPU_FAMILY == ARM) && ARM_THUMB)
    /* set b0 so that sysAuxClkConnect() can be used from shell */

    sysAuxClkRoutine = (FUNCPTR)((UINT32)routine | 1);
#else
    sysAuxClkRoutine	= routine;
#endif /* CPU_FAMILY == ARM */

    return OK;
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
	/* Disable timer itself: set limit register to 0 */

	SA1501_TIMER_WRITE (AUX_TIMER_LIMIT (SA1501_TIMER_BASE), 0);


	/* Disable the timer interrupt in the Interrupt Controller */

	SA1501_TIMER_INT_DISABLE (AUX_TIMER_INT_LVL);

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
* SEE ALSO: sysAuxClkDisable()
*/

void sysAuxClkEnable (void)
    {
    UINT32 tl;

    if (!sysAuxClkRunning)
	{
	/*
	 * Calculate the timer limit value:
	 * counter limit value = (clock rate / sysAuxClkTicksPerSecond)
	 */
	
	tl = (AUX_TIMER_CLK / sysAuxClkTicksPerSecond);


	/* Load Limit value into Timer registers */

	SA1501_TIMER_WRITE (AUX_TIMER_LIMIT (SA1501_TIMER_BASE), tl);


	/* reset timer value to zero */

	SA1501_TIMER_WRITE (AUX_TIMER_RESET (SA1501_TIMER_BASE), 0);


	/* enable clock interrupt in interrupt controller */

	SA1501_TIMER_INT_ENABLE (AUX_TIMER_INT_LVL);
	
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
    return sysAuxClkTicksPerSecond;
    }

/*******************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock.  If the
* auxiliary clock is currently enabled, the clock is disabled and then
* re-enabled with the new rate.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
	return ERROR;

    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return OK;
    }

#ifdef	INCLUDE_TIMESTAMP

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
    return ERROR;
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
   if (sysTimestampRunning)
      {
      return OK;
      }

   if (!sysClkRunning)          /* timestamp timer is derived from the sysClk */
      return ERROR;

   sysTimestampRunning = TRUE;

   return OK;
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
    if (sysTimestampRunning)
        sysTimestampRunning = FALSE;

    return OK;
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
    /*
     * The period of the timestamp depends on the clock rate of the system
     * clock.
     */

    return ((UINT32)(SYS_TIMER_CLK / sysClkTicksPerSecond));
    }

/*******************************************************************************
*
* sysTimestampFreq - get a timestamp timer clock frequency
*
* This routine gets the frequency of the timer clock, in ticks per
* second.  The rate of the timestamp timer is set explicitly by the
* hardware and typically cannot be altered.
*
* NOTE: Because the system clock serves as the timestamp timer,
* the system clock frequency is also the timestamp timer frequency.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    return ((UINT32)SYS_TIMER_CLK);
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
    UINT32 t;

    SA1501_TIMER_READ (SYS_TIMER_VALUE (SA1501_TIMER_BASE), t);

#if defined (SA1501_TIMER_VALUE_MASK)
    t &= SA1501_TIMER_VALUE_MASK;
#endif

    return t;
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
    UINT32 t;

    SA1501_TIMER_READ (SYS_TIMER_VALUE (SA1501_TIMER_BASE), t);

#if defined (SA1501_TIMER_VALUE_MASK)
    t &= SA1501_TIMER_VALUE_MASK;
#endif

    return t;
    }

#endif  /* INCLUDE_TIMESTAMP */
