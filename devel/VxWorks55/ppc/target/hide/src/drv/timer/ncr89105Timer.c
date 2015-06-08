/* ncr89105Timer.c - NCR89C105 slave I/O controller library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
03d,15sep96,dat  merged with timestamp driver.
		 Fixed SPR 6133. (Warning: unused variable).
03c,08dec94,rdc  fixed sysTimestampLock and comments in sysTimestamp.
03b,07dec94,jds  fixed timestamp to use 21 bit timer and generate interrupts
03a,10nov94,kvk  provided timestamp functionality.
02c,19sep94,vin  fixed a bug in the sysAuxClkEnable.
		 removed the SUN_COUNTER_1 reset (SPR #3426)
02b,19sep94,vin  fixed a bug in the sysClkEnable,
		 changed the SUN_IR_PENDING to SUN_IR_MASK_SET (SPR #3400)
02a,13apr94,jkw  created from version 01c of sunec
01c,11jun93,ccc  moved intConnect() to sysHwInit2().
01b,13may93,ccc  moved to final directory.
01a,17mar93,ccc  created.
    26apr93,ccc  changed path to build in sunec directory.

*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
sun1e with a board-independant interface.  This library handles both
the system clock and the auxiliary clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

The macro INCLUDE_TIMESTAMP must be defined to generate the timestamp
driver code.  If not defined, only the system and auxiliary clock
interfaces will be created.
*/

#include "drv/timer/timerDev.h"
#include "drv/timer/timestampDev.h"

#ifdef INCLUDE_TIMESTAMP

#define USER_TIMER_MSW_MASK	0x000001FF /* Mask for user timer register */
#define USER_TIMER_MSW_SHIFT	23	   /* Adjust for user timer register */
#define USER_TIMER_LSW_SHIFT	9	   /* Adjust for user timer register */
#define USER_TIMER_FREQ		2000000	   /* (1000000/0.5) as timer count
						increments every 500ns */

/* Locals */

LOCAL BOOL    sysTimestampConnected      = FALSE;
LOCAL BOOL    sysTimestampRunning        = FALSE;
LOCAL FUNCPTR sysTimestampRoutine        = NULL;
LOCAL int     sysTimestampArg            = NULL;

#endif /* INCLUDE_TIMESTAMP */

IMPORT	void	sysClkAck ();
IMPORT	void	sysAuxClkAck ();

/* Locals */

LOCAL int     sysClkTicksPerSecond = 60;
LOCAL BOOL    sysClkRunning        = FALSE;
LOCAL FUNCPTR sysClkRoutine        = NULL;
LOCAL int     sysClkArg            = NULL;
LOCAL BOOL    sysClkConnected      = FALSE;

LOCAL int     auxClkTicksPerSecond = 60;
LOCAL BOOL    sysAuxClkRunning     = FALSE;
LOCAL FUNCPTR sysAuxClkRoutine     = NULL;
LOCAL int     sysAuxClkArg         = NULL;
LOCAL BOOL    sysAuxClkConnected   = FALSE;

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
*/

LOCAL void sysAuxClkInt (void)
    {
    if (sysAuxClkRoutine != NULL)
        (*(FUNCPTR) sysAuxClkRoutine) (sysAuxClkArg);

    sysAuxClkAck ();
    }

/***************************************************************************
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
    FUNCPTR routine,    /* routine called at each system clock interrupt */
    int     arg         /* argument with which to call routine           */
    )
    {
    sysHwInit2 ();

    sysClkConnected   = TRUE;
    sysClkRoutine     = routine;
    sysClkArg         = arg;

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
        *NCR89C105_IR_MASK_SET = IR_ALL;     /* turn off interrupts */
        *NCR89C105_IR_MASK_SET = IR_CLOCK10; /* turn off lvl 10 */
        *NCR89C105_IR_MASK_CLR = IR_ALL;     /* turn on interrupts */
        sysClkRunning    = FALSE;
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
        *NCR89C105_IR_MASK_SET = IR_ALL;       /* turn off interrupts */
        intRegister      = *NCR89C105_LIMIT_0; /* Clear interrupt at clock */
        *NCR89C105_IR_MASK_SET  = IR_CLOCK10;   /* Clear interrupt pending */
        *NCR89C105_IR_MASK_CLR = IR_CLOCK10;   /* turn on lvl 10 */
	*NCR89C105_LIMIT_0     = (1000000 / sysClkTicksPerSecond) << 10;
        *NCR89C105_IR_MASK_CLR = IR_ALL;       /* turn on interrupts */
        sysClkRunning    = TRUE;
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
    int ticksPerSecond      /* number of clock interrupts per second */
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

/***************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock
* interrupts.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysAuxClkDisconnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,    /* routine called at each aux. clock interrupt */
    int     arg         /* argument with which to call routine         */
    )
    {
    sysAuxClkConnected = TRUE;
    sysAuxClkRoutine   = routine;
    sysAuxClkArg       = arg;

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
        *NCR89C105_COUNTER_1_CR = 1;      /* disable timer */
        sysAuxClkRunning = FALSE;
        }
    }

/***************************************************************************
*
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkDisable(), sysAuxClkRateSet()
*/

void sysAuxClkEnable (void)
    {
    if (!sysAuxClkRunning)
        {
	*NCR89C105_COUNTER_1_CR = 0;		/* enable timer */
	*NCR89C105_LIMIT_1      = (1000000 / auxClkTicksPerSecond) << 10;

        sysAuxClkRunning  = TRUE;
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

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }


#ifdef INCLUDE_TIMESTAMP

/**************************************************************************
*
* sysTimestampInt - timestamp timer interrupt handler
*
* This routine handles the timestamp timer interrupt.   A user routine is
* called, if one was connected by sysTimestampConnect().
*
* RETURNS: N/A
*
* SEE ALSO: sysTimestampConnect()
*/

LOCAL void sysTimestampInt (void)
    {
    /* acknowledge the timer rollover interrupt here */

    if (sysTimestampRoutine != NULL)     /* call user-connected routine */
        (*sysTimestampRoutine) (sysTimestampArg);

    sysAuxClkAck ();
    }

/**************************************************************************
*
* sysTimestampConnect - connect a user routine to a timestamp timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt.  
*
* RETURNS: OK, or ???? ERROR if sysTimestampInt() interrupt service routine is 
* not used.
*/

STATUS sysTimestampConnect
    (
    FUNCPTR routine, /* routine called at each timestamp timer interrupt */
    int arg          /* argument with which to call routine */
    )
    {
    sysTimestampConnected = TRUE;
    sysTimestampRoutine = routine;
    sysTimestampArg = arg;
    return (OK);
    }

/**************************************************************************
*
* sysTimestampEnable - enable a timestamp timer interrupt
*
* This routine enables timestamp timer interrupts and resets the counter.
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be enabled.
*/

STATUS sysTimestampEnable (void)
    {
    if (!sysTimestampRunning)
	{
        (void) intConnect ((VOIDFUNCPTR *) INUM_TO_IVEC  (INT_VEC_AUX_CLOCK),
		       (VOIDFUNCPTR) sysTimestampInt, NULL);

        /* Put the processor timer in timer mode */

        *NCR89C105_COUNTER_1_CR = 0;

        /* Set the limit register (not really used though, just for safety */

        *NCR89C105_LIMIT_1 = 0x7ffffe00;

        sysTimestampRunning = TRUE;
	}

    return (OK);
    }


/**************************************************************************
*
* sysTimestampDisable - disable a timestamp timer interrupt
*
* This routine disables the timestamp timer.  It does not directly disable
* interrupts.  However, the tick counter does not increment once the 
* timestamp timer is disabled, thus, interrupts are no longer generated.
* This routine merely resets the timer counter.
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be disabled.
*/

STATUS sysTimestampDisable (void)
    {
    if (sysTimestampRunning)
	{

	/* Stop the timer to count */

        *NCR89C105_COUNTER_1_CR = 1;

	sysTimestampRunning = FALSE;
	}

    return (OK);
    }


/**************************************************************************
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
     * Return the timestamp timer period here.
     * The highest period (maximum terminal count) should be used so
     * that rollover interrupts are kept to a minimum.
     *
     */

    return (0x1fffff);
    }


/**************************************************************************
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
    UINT32 timerFreq;

    /*
     * Return the timestamp tick output frequency here.
     * This value can be determined from the following equation:
     *     timerFreq = clock input frequency / prescaler
     *
     * When possible, read the clock input frequency and prescaler values
     * directly from chip registers.
     */

    /* (1000000/0.5) = 2000000 counter increments every 500ns */

    timerFreq = USER_TIMER_FREQ;

    return (timerFreq);
    }


/**************************************************************************
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
    unsigned long countValueLsw;

    countValueLsw = (*NCR89C105_COUNTER_1);

    countValueLsw = countValueLsw >> USER_TIMER_LSW_SHIFT;

    /* return the timestamp timer tick count here */

    return (countValueLsw);
    }


/**************************************************************************
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
    unsigned long countValueLsw;

    countValueLsw = (*NCR89C105_COUNTER_1);

    countValueLsw = countValueLsw >> USER_TIMER_LSW_SHIFT;

    /* return the timestamp timer tick count here */

    return (countValueLsw);
    }

#endif   /* INCLUDE_TIMESTAMP */
