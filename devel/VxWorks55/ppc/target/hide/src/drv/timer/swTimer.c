/* swTimer.c - software timer library */

/* Copyright 1984-1994 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,02feb94,dzb
*/

/*
DESCRIPTION
*/

/* includes */

#include "semLib.h"
#include "taskLib.h"

/* defines */

#ifndef	SYS_SW_CLK_PRIORITY
#define	SYS_SW_CLK_PRIORITY	255
#endif	/* SYS_SW_CLK_PRIORITY */

/* locals */

LOCAL FUNCPTR sysClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysClkArg		= NULL; /* its argument */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkTicksPerSecond	= 1;
LOCAL SEM_ID semDId;

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
    sysClkRunning = FALSE;

    semTake (semDId, WAIT_FOREVER);
    }

/*******************************************************************************
*
* sysSwClk - simulate a system clock
*
* RETURNS: N/A.
*
* SEE ALSO: sysClkEnable(), sysClkDisable()
*/

void sysSwClk (void)
    {
    int ix;

    while (sysClkRunning)
	{
	for (ix = 0; ix < sysClkTicksPerSecond; ix++)
	    ;

        if (sysClkRoutine != NULL)
           (*sysClkRoutine) (sysClkArg);
	}

    semFlush (semDId);
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
    static BOOL initialized = FALSE;

    if (!sysClkRunning)
        {
	if (!initialized)
	    {
            semDId = semBCreate (SEM_Q_FIFO, SEM_EMPTY);
	    initialized = TRUE;
	    }

	sysClkRunning = TRUE;

	taskSpawn ("sysSwClk", SYS_SW_CLK_PRIORITY, VX_NO_STACK_FILL, 2000,
	    (FUNCPTR) sysSwClk, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
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
