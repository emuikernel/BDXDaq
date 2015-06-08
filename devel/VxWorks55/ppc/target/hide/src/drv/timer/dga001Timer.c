/* dga001Timer.c - DGA-001 Timer library */

/* Copyright 1994-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,23apr98,hk   added tkt's counter glitch filter to sysTimestamp{Lock}.
01n,09dec97,hk   changed TICK_FREQ to DGA_TICK_FREQ. added sys{Aux}ClkDisable()
                 in sys{Aux}ClkConnect(). reviewed timestamp driver code.
01m,12jul97,hk   changed inclusion of multi/dga001.h to timer/dga001Timer.h.
01l,06may97,hk   added TSTAMP_USE_DGA001_TT1. added default TIMESTAMP_LEVEL.
01k,04may97,hk   merged in timestamp driver.
01f,18apr97,st   moved initialization of Timerstamp-Timer in
                 sysTimestampConnect() to sysTimestampEnable().
01i,06mar97,st   added support for timestamp function for windview
01h,10nov95,hk   followed dga001.h-01j macro name changes.
01g,09nov95,hk   added more type casting. followed dga001.h 01i.
01f,08nov95,hk   changed dga001.h directory to drv/multi/.
01e,08jun95,hk   changed sysClkInt/sysAuxClkInt for CSR20 header mod.
01d,06jun95,hk   renamed timer select macros.
01c,28feb95,hk   changed sysClkConnect() and sysAuxClkConnect() to conform the
		 corrected IV_ defines in ivSh.h-01e. copyright 1995.
01b,09nov94,sa   fixed.
01a,08nov94,sa   derived from 01f of sh7604Timer.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
DGA-001 chip with a board-independant interface.  This library handles both
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
#include "drv/timer/dga001Timer.h"
#include "config.h"

/* Locals */ 

#define DGA_TICK_FREQ	1000000			/* 1MHz */

#ifdef	SYSCLK_USE_DGA001_TT0
LOCAL int     sysClkTicksPerSecond = 60; 
LOCAL BOOL    sysClkRunning        = FALSE; 
LOCAL FUNCPTR sysClkRoutine        = NULL; 
LOCAL int     sysClkArg            = NULL; 
LOCAL BOOL    sysClkConnected      = FALSE; 
#endif

#ifdef	AUXCLK_USE_DGA001_TT1
LOCAL int     auxClkTicksPerSecond = 60; 
LOCAL BOOL    sysAuxClkRunning     = FALSE; 
LOCAL FUNCPTR sysAuxClkRoutine     = NULL; 
LOCAL int     sysAuxClkArg         = NULL;
LOCAL BOOL    auxClkConnected      = FALSE; 
#endif

#if defined(INCLUDE_TIMESTAMP) && defined(TSTAMP_USE_DGA001_TT1)
LOCAL BOOL    sysTimestampRunning  = FALSE;	/* running flag */
LOCAL FUNCPTR sysTimestampRoutine  = NULL;	/* user rollover routine */
LOCAL int     sysTimestampArg      = NULL;	/* arg to user routine */
#ifndef	TIMESTAMP_LEVEL
#error TIMESTAMP_LEVEL is not defined
#endif /* ! TIMESTAMP_LEVEL */
#endif /* INCLUDE_TIMESTAMP && TSTAMP_USE_DGA001_TT1 */

#ifdef	SYSCLK_USE_DGA001_TT0
/*******************************************************************************
*
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
*/

LOCAL void sysClkInt (void)
    {
    if ( !(*DGA_CSR20 & CSR20_TT0IRQ))
	return;

    if (sysClkRoutine != NULL)
	(*sysClkRoutine) (sysClkArg);

    *DGA_CSR23 = (UINT32)CSR23_TT0ICL;		/* clear interrupt */
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
    sysHwInit2 ();      /* XXX for now -- needs to be in usrConfig.c */

    sysClkDisable ();

    if (intConnect (INT_VEC_TT0, sysClkInt, NULL) != OK)
	return (ERROR);

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
	*DGA_CSR21 &= (UINT32) ~CSR21_TT0IEN;	/* disable interrupt */
	*DGA_CSR12 &= (UINT32) ~CSR12_TT0CEN;	/* stop timer */

	sysClkRunning = FALSE;
	}
    }

/****************************************************************************** 
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
	*DGA_CSR12  = (UINT32) NULL;		/* stop timer */
	*DGA_CSR13  = (UINT32) NULL;		/* clear count */
	*DGA_CSR23  = (UINT32) CSR23_TT0ICL;	/* clear interrupt */
	*DGA_CSR21 |= (UINT32) CSR21_TT0IEN;	/* enable interrupt */

	/* load compare register with the number of micro seconds and 
         * start the counter.
	 */
	*DGA_CSR12 = (UINT32)((DGA_TICK_FREQ / sysClkTicksPerSecond - 1) | 
				CSR12_TT0CEN);
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
* SEE ALSO: sysClkEnable, sysClkRateSet()
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
* SEE ALSO: sysClkEnable, sysClkRateGet()
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
#endif	/* SYSCLK_USE_DGA001_TT0 */

#ifdef	AUXCLK_USE_DGA001_TT1
/*******************************************************************************
*
* sysAuxClkInt - handle auxiliary clock interrupts
*/
 
LOCAL void sysAuxClkInt (void)
    {
    if ( !(*DGA_CSR20 & CSR20_TT1IRQ))
	return;

    if (sysAuxClkRoutine != NULL)
	(*sysAuxClkRoutine) (sysAuxClkArg);

    *DGA_CSR23 = (UINT32)CSR23_TT1ICL;		/* reset clock interrupt */
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
    sysAuxClkDisable ();

    if (intConnect (INT_VEC_TT1, sysAuxClkInt, NULL) != OK)
	return (ERROR);

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
	*DGA_CSR21 &= (UINT32) ~CSR21_TT1IEN;	/* disable interrupt */
	*DGA_CSR14 &= (UINT32) ~CSR14_TT1CEN;	/* stop timer */

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
	*DGA_CSR14  = (UINT32) NULL;		/* stop timer */
	*DGA_CSR15  = (UINT32) NULL;		/* clear count */
	*DGA_CSR23  = (UINT32) CSR23_TT1ICL;	/* clear interrupt */
	*DGA_CSR21 |= (UINT32) CSR21_TT1IEN;	/* enable interrupt */

	/* load compare register with the number of micro seconds and 
	 * start the counter.
	 */
	*DGA_CSR14 = (UINT32)((DGA_TICK_FREQ / auxClkTicksPerSecond - 1) | 
				CSR14_TT1CEN);
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
#endif	/* AUXCLK_USE_DGA001_TT1 */

#if defined(INCLUDE_TIMESTAMP) && defined(TSTAMP_USE_DGA001_TT1)
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
    if (*DGA_CSR20 & CSR20_TT1IRQ)
	{
	*DGA_CSR23 = (UINT32)CSR23_TT1ICL;		/* clear interrupt */

	if (sysTimestampRoutine != NULL)
	    (*sysTimestampRoutine) (sysTimestampArg);	/* call user routine */
	}
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
    sysTimestampArg     = arg;
 
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
    if (sysTimestampRunning)
	{
	*DGA_CSR14 &= (UINT32) ~CSR14_TT1CEN;	/* stop timer */
	*DGA_CSR15  = (UINT32) NULL; 		/* clear count */
	*DGA_CSR14 |= (UINT32) CSR14_TT1CEN;	/* start timer */
	return (OK);
	}
    
    *DGA_CSR14  = (UINT32) NULL;		/* stop timer */
    *DGA_CSR15  = (UINT32) NULL;		/* clear count */
    *DGA_CSR23  = (UINT32) CSR23_TT1ICL;	/* clear interrupt */

    if (intConnect (INT_VEC_TT1, sysTimestampInt, NULL) != OK)
	return (ERROR);

    *DGA_CSR25 = (UINT32)((*DGA_CSR25 & 0xff0fffff)
	       | (((TIMESTAMP_LEVEL >> 1) & 0x7) << 20));
						/* set interrupt level */
    *DGA_CSR21 |= (UINT32) CSR21_TT1IEN;	/* enable interrupt */

    /* load compare register with the number of micro seconds and
     * start the counter.
     */
    *DGA_CSR14 = (UINT32)(sysTimestampPeriod() | CSR14_TT1CEN);

    sysTimestampRunning = TRUE;
    return (OK);
    }

/********************************************************************************
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
	*DGA_CSR21 &= (UINT32) ~CSR21_TT1IEN;	/* disable interrupt */
	*DGA_CSR14 &= (UINT32) ~CSR14_TT1CEN;	/* stop timer */
	*DGA_CSR23 = (UINT32) CSR23_TT1ICL;	/* clear pending interrupt */

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
    return ((UINT32)0x00ffffff);	/* highest period -> freerunning */
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
    return ((UINT32)DGA_TICK_FREQ);
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
* NOTE DGA-001:
* The following code workarounds the DGA-001 up-counter glitch as shown below:
*
* .CS
*     1111                                .
*     1110                                :.
*     1101                                ::
*     1100                                ::.
*     1011                             -->: :<-- (few nano seconds)
*     1010                                : :
*     1001                                : :            _______________
*     1000                                : :____________
*     0111                 _______________:
*     0110  _______________
*
*          0              +1             +2             +3 [micro sec]
* .CE
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampLock()
*/
 
UINT32 sysTimestamp (void)
    {
    UINT32 cnt1 = *DGA_CSR15 & 0x00ffffff;
    UINT32 cnt2 = *DGA_CSR15 & 0x00ffffff;

    if (cnt1 <= cnt2) return (cnt1);

    return (cnt2);
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
    int lockKey = intLock ();

    UINT32 cnt1 = *DGA_CSR15 & 0x00ffffff;
    UINT32 cnt2 = *DGA_CSR15 & 0x00ffffff;

    intUnlock (lockKey);

    if (cnt1 <= cnt2) return (cnt1);

    return (cnt2);
    }

#endif	/* INCLUDE_TIMESTAMP */
