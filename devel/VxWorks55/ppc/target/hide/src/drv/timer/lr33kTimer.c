/* lr33kTimer.c - LSI Logic LR33xx0 timer library */

/* Copyright 1984-1994 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,21mar95,kvk  integrated into 5.2 vxWorks.
01a,10sep94,xxx  derived from version 02p of racerx/sysLib.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions of the
LSI Logic LR33xx0.  This library handles both the system clock and the
auxiliary clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.
*/

/* locals */

LOCAL FUNCPTR sysClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysClkArg		= NULL; /* its argument */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkConnected	= FALSE;
LOCAL int sysClkTicksPerSecond	= 60;

LOCAL FUNCPTR sysAuxClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysAuxClkArg		= NULL; /* its argument */
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int auxClkConnected	= FALSE;
LOCAL int auxClkTicksPerSecond	= 60;

/*******************************************************************************
*
* sysClkInt - system clock interrupt handler
*
* This routine handles the system clock interrupt.  It calls a user routine
* if one was specified by the routine sysClkConnect().
*/

LOCAL void sysClkInt (void)
    {
    FAST volatile int *pTimer1Cntl = (volatile int *) M_TC1;

    *pTimer1Cntl &= ~TC_INT;		/* acknowledge timer int */
    sysWbFlush ();

    if (sysClkRoutine != NULL)
        (* sysClkRoutine) (sysClkArg);
    }

/*******************************************************************************
*
* sysAuxClkInt - auxiliary clock interrupt handler
*
* This routine handles the auxiliary clock interrupt.  It calls a user routine
* if one was specified by the routine sysAuxClkConnect().
*/

LOCAL void sysAuxClkInt (void)
    {
    FAST volatile int *pTimer2Cntl = (volatile int *) M_TC2;

    *pTimer2Cntl &= ~TC_INT;		/* acknowledge timer int */
    sysWbFlush ();

    if (sysAuxClkRoutine != NULL)
        (* sysAuxClkRoutine) (sysAuxClkArg);
    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* system clock interrupt.  It does not enable system clock interrupts.
* Normally, it is called from usrRoot() in usrConfig.c to connect usrClock()
* to the system clock interrupt.
*
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,	/* routine to be called at each clock interrupt */
    int     arg		/* argument with which to call routine          */
    )
    {
    sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */

    sysClkRoutine   = routine;
    sysClkArg       = arg;
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
    FAST volatile int *pTimer1Cntl = (volatile int *) M_TC1;

    if (sysClkRunning)
	{
        *pTimer1Cntl = 0;
        sysWbFlush ();
 
	sysClkRunning = FALSE;		/* clock is no longer running */
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
    FAST volatile int *pTimer1Cntl  = (volatile int *) M_TC1;
    FAST volatile int *pTimer1Count = (volatile int *) M_TIC1;

    if (!sysClkRunning)
	{
        *pTimer1Count = TIMER_HZ / sysClkTicksPerSecond;

        *pTimer1Cntl = TC_IE | TC_CE;
        sysWbFlush ();
 
        intEnable (INT_LVL_TIMER1);	/* enable interrupts at the CPU */

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
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be
* set.
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
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,	/* routine called at each aux. clock interrupt */
    int     arg		/* argument with which to call routine         */
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
    FAST volatile int *pTimer2Cntl = (volatile int *) M_TC2;

    if (sysAuxClkRunning)
	{
        *pTimer2Cntl = 0;
        sysWbFlush ();
 
	sysAuxClkRunning = FALSE;	/* clock is no longer running */
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
    FAST volatile int *pTimer2Cntl  = (volatile int *) M_TC2;
    FAST volatile int *pTimer2Count = (volatile int *) M_TIC2;

    if (!sysAuxClkRunning)
	{
        *pTimer2Count = TIMER_HZ / auxClkTicksPerSecond;

        *pTimer2Cntl = TC_IE | TC_CE;
        sysWbFlush ();
 
        intEnable (INT_LVL_TIMER2);	/* enable interrupts at the CPU */

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
