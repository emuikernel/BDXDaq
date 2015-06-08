/* sun4Timer.c - sun1e timer library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01g,03dec96,wlf  doc: cleanup.
01f,11oct96,dat  merged sun4TimerTS.c into this code.
01e,11jun96,wlf  doc: cleanup.
01d,22mar96,dds  tranferred the intConnects to sysLib.c. 
01c,07dec94,rdc  tweeked sysTimestampEnable.
01b,10nov94,jds  added timestamp support
01a,10jun92,ccc  created by moving routines from sysLib.c of sun1e.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
sun1e with a board-independent interface.  This library handles both
the system clock and the auxiliary clock functions.

The BSP code is responsible for connecting appropriate interrupt service
routines to the interrupt hardware system.  The routines sysClkInt() and
sysAuxClkInt() are the routines that should be called as a result of
the appropriate interrupt request. Interrupt connect actions are normally
done as part of sysHwInit2().

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

If INCLUDE_TIMESTAMP is defined, then a timestamp interface for this
timer will be generated.
*/

#include "drv/timer/timerDev.h"
#include "drv/timer/timestampDev.h"

IMPORT	void	sysClkAck ();
IMPORT	void	sysAuxClkAck ();

/* Locals */ 

LOCAL int     sysClkTicksPerSecond = 60; 
LOCAL BOOL    sysClkRunning        = FALSE; 
LOCAL FUNCPTR sysClkRoutine        = NULL; 
LOCAL int     sysClkArg            = NULL; 

LOCAL int     auxClkTicksPerSecond = 60; 
LOCAL BOOL    sysAuxClkRunning     = FALSE; 
LOCAL FUNCPTR sysAuxClkRoutine     = NULL; 
LOCAL int     sysAuxClkArg         = NULL;

LOCAL BOOL    sysTimestampRunning  = FALSE;     /* if running - TRUE */
LOCAL FUNCPTR sysTimestampRoutine  = NULL;      /* user rollover routine */
LOCAL int     sysTimestampArg      = NULL;      /* arg to user routine */

/* globals */


/**************************************************************************
*
* sysClkInt - clock interrupt handler
*
* This routine handles the clock interrupt.  It is attached to the clock
* interrupt vector by the routine sysClkConnect().  The appropriate routine
* is called and the interrupts are acknowledged.
*/

LOCAL void sysClkInt (void)

    {
    if (sysClkRoutine != NULL)
        (*(FUNCPTR) sysClkRoutine) (sysClkArg);

    sysClkAck ();
    }

/***************************************************************************
*
* sysAuxClkInt - clock interrupt handler
*
* This routine handles the clock interrupt.  It is attached to the clock
* interrupt vector by the routine sysAuxClkConnect().  The appropriate
* routine is called and the interrupts are acknowledged.
*
* This routine supports both auxClock interrupts and timestamp interrupts.
* It selects the routine based upon the last successful 'connect' routine,
* either sysAuxClkConnect(), or sysTimestampConnect().  The appropriate
* timer must also be enabled before the connected user routine is called.
*/

LOCAL void sysAuxClkInt (void)

    {
    if (sysAuxClkRunning && sysAuxClkRoutine != NULL)
        (*(FUNCPTR) sysAuxClkRoutine) (sysAuxClkArg);

    else if (sysTimestampRunning && sysTimestampRoutine != NULL)
        (*(FUNCPTR) sysTimestampRoutine) (sysTimestampArg);

    sysAuxClkAck ();
    }

/***************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c to
* connect usrClock to the system clock interrupt.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,    /* routine called at each system clock interrupt */
    int     arg         /* argument with which to call routine           */
    )
    {
    sysClkRoutine     = NULL;
    sysClkArg         = arg;
    sysClkRoutine     = routine;

    sysHwInit2 ();

    return (OK);
    }
 
/***************************************************************************
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
        *SUN_IR         &= ~IR_ALL;     /* turn off interrupts */
        *SUN_IR         &= ~IR_CLOCK10; /* turn off lvl 10 */
        *SUN_IR         |= IR_ALL;      /* turn on interrupts */
        sysClkRunning = FALSE;
        }
    }

/***************************************************************************
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
    unsigned int intRegister;

    if (!sysClkRunning)
        {
        *SUN_IR         &= ~IR_ALL;     /* turn off interrupts */
        intRegister = *SUN_LIMIT_0;     /* Clear interrupt at clock */
        *SUN_IR         &= ~IR_CLOCK10; /* Clear interrupt pending */
        *SUN_IR         |= IR_CLOCK10;  /* turn on lvl 10 */
        *SUN_IR         |= IR_ALL;      /* turn on interrupts */
        sysClkRunning = TRUE;
        }
    }

/***************************************************************************
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

/***************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.  It is called
* by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond      /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
	return (ERROR);

    sysClkTicksPerSecond = ticksPerSecond;
    *SUN_COUNTER_0 = 0;
    *SUN_LIMIT_0 = (1000000 / ticksPerSecond) << 10;
    *SUN_COUNTER_0 = 0;
    return (OK);
    }

/***************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock
* interrupts.
* 
* The auxilliary clock and the timestamp clock use the same hardware, only
* one can be active at a time.  An error will result if an attempt is made
* to use both.
*
* RETURNS: OK, or ERROR if the timestamp clock is in use.
*
* SEE ALSO: sysAuxClkDisconnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,    /* routine called at each aux. clock interrupt */
    int     parm        /* argument to auxiliary clock interrupt routine */
    )
    {
    if (sysTimestampRunning)
	return ERROR;

    sysAuxClkRoutine   = routine;
    sysAuxClkArg       = parm;

    return (OK);
    }

/***************************************************************************
*
* sysAuxClkDisconnect - clear the auxiliary clock routine
*
* This routine disables the auxiliary clock interrupt, stops the timer,
* and disconnects the routine currently connected to the auxiliary clock
* interrupt.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkEnable()
*/

void sysAuxClkDisconnect (void)

    {
    /* disable the auxiliary clock interrupt */

    sysAuxClkDisable ();

    /* connect dummy routine, just in case */

    sysAuxClkConnect ((FUNCPTR) logMsg, (int) "auxiliary clock interrupt\n");
    }

/***************************************************************************
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
        *SUN_IR         &= ~IR_ALL;     /* turn off interrupts */
        *SUN_IR         &= ~IR_CLOCK14; /* turn off lvl 14 */
        *SUN_IR         |= IR_ALL;      /* turn on interrupts */
        sysAuxClkRunning = FALSE;
        }
    }

/***************************************************************************
*
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.  
*
* NOTE: The auxilliary clock and the timestamp clock are mutually
* exclusive.  The timestamp clock cannot be used if the auxilliary clock
* is running.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkDisable()
*/

void sysAuxClkEnable (void)

    {
    unsigned int intRegister;

    if (! sysAuxClkRunning && ! sysTimestampRunning)
        {
        *SUN_IR         &= ~IR_ALL;     /* turn off interrupts */
        intRegister = *SUN_LIMIT_1;     /* Clear interrupt at clock */
        *SUN_IR         &= ~IR_CLOCK14; /* Clear interrupt pending */
        *SUN_IR         |= IR_CLOCK14;  /* turn on lvl 14 */
        *SUN_IR         |= IR_ALL;      /* turn on interrupts */
        sysAuxClkRunning = TRUE;
        }
    }

/***************************************************************************
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

/***************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock.  It does not
* enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond      /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
	return (ERROR);

    auxClkTicksPerSecond = ticksPerSecond;
    *SUN_COUNTER_1 = 0;
    *SUN_LIMIT_1 = (1000000 / ticksPerSecond) << 10;
    *SUN_COUNTER_1 = 0;
    return (OK);
    }

#ifdef  INCLUDE_TIMESTAMP

/*******************************************************************************
*
* sysTimestampConnect - connect a user routine to a timestamp timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt.  
*
* NOTE: The auxilliary clock and the timestamp clock are mutually
* exclusive.  The timestamp clock cannot be used if the auxilliary clock
* is running.
*
* RETURNS: OK, or ERROR if the auxiliary clock is in use.
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampConnect
    (
    FUNCPTR routine,    /* routine called at each timestamp timer interrupt */
    int arg             /* argument with which to call routine */
    )
    {
    if (sysAuxClkRunning)
	return ERROR;

    sysTimestampRoutine = routine;
    sysTimestampArg = arg;

    return (OK);
    }

/*******************************************************************************
*
* sysTimestampEnable - enable a timestamp timer interrupt
*
* This routine enables timestamp timer interrupts and resets the counter.
*
* NOTE: The auxilliary clock and the timestamp clock are mutually
* exclusive.  The timestamp clock cannot be used if the auxilliary clock
* is running.
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be enabled.
*
* SEE ALSO: sysTimestampDisable()
*/

STATUS sysTimestampEnable (void)
    {
    unsigned int intRegister;

    if (sysAuxClkRunning)
        return (ERROR);

    if (! sysTimestampRunning)
        {
        *SUN_IR         &= ~IR_ALL;     /* turn off interrupts */

        *SUN_COUNTER_1 = 0;
        *SUN_LIMIT_1 = 0x1fffff << 10;  	/* 0 limit means free running */
        *SUN_COUNTER_1 = 0;

        intRegister = *SUN_LIMIT_1;     /* Clear interrupt at clock */
        *SUN_IR         &= ~IR_CLOCK14; /* Clear interrupt pending */
        *SUN_IR         |= IR_CLOCK14;  /* turn on lvl 14 */

        *SUN_IR         |= IR_ALL;      /* turn on interrupts */

        *SUN_COUNTER_1 = 0;
        *SUN_LIMIT_1 = 0x1fffff << 10;  	/* 0 limit means free running */
        *SUN_COUNTER_1 = 0;

        sysTimestampRunning = TRUE;
        }

    return (OK);
    }

/*******************************************************************************
*
* sysTimestampDisable - disable a timestamp timer interrupt
*
* This routine disables the timestamp timer.  It does not directly disable
* interrupts.  However, the tick counter does not increment once the 
* timestamp timer is disabled, thus, interrupts are no longer generated.
* This routine merely resets the timer counter.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampDisable (void)
    {
    if (sysTimestampRunning)
        {
	*SUN_LIMIT_1 = 0x80000000 ;          /* 0 limit means free running */

        *SUN_IR         &= ~IR_ALL;     /* turn off interrupts */
        *SUN_IR         &= ~IR_CLOCK14; /* turn off lvl 14 */
        *SUN_IR         |= IR_ALL;      /* turn on interrupts */
        sysTimestampRunning = FALSE;
        }

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
    return (0x1fffff);        /* highest period -> freerunning */
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
    return ((UINT32) 1000000);
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
    return (*((UINT32 *) SUN_COUNTER_1) >> 10);
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
    return (*((UINT32 *) SUN_COUNTER_1) >> 10);
    }

#endif /* INCLUDE_TIMESTAMP */ 
