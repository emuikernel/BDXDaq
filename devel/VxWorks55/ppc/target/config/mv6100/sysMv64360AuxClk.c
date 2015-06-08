/* sysMv64360AuxClk.c - Motorola MVME6100 AuxClock library */

/* Copyright 2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01g,14jun04,cak   Moved the MV64360 timer/counter support to a separate file.
01f,25feb04,cak   Moved the initialization of timerRate from the int()
		  routine - where it was mistakenly placed - to the
		  init() routine.
01e,17dec03,cak   BSP update.
01d,28jan03,cak   Redefined MV64360_TMR_RATE as CPU_BUS_SPD.
01c,24jan03,cak   Replaced DEFAULT_BUS_CLOCK with DFLT_BUS_CLK.
01b,14oct02,yyz	  Fixed "&" to "|" in setting timer mode in sysAuxClkInit().
01a,18jun02,yyz   Initial writing.
*/

/*
DESCRIPTION
There are a total of four identical timer/counters on the MV64360,
each 32-bits wide. This file contains routines to use one of the
timer/counters as the auxiliary clock.  
*/

/* includes */

#include "config.h"
#include "mv64360.h"
#include "mv6100A.h"

/* defines */

/* typedefs */

/* globals */

/* locals */

LOCAL BOOL	sysAuxClkRunning	= FALSE;
LOCAL int	sysAuxClkTicksPerSecond = 60;

/* forward declarations */

STATUS 		sysAuxClkConnect (FUNCPTR routine, int arg);
void 		sysAuxClkDisable (void);
void 		sysAuxClkEnable  (void);
int 		sysAuxClkRateGet (void);
STATUS 		sysAuxClkRateSet (int ticksPerSecond);

/********************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.
*
* RETURN: OK, always.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,	/* routine called at each aux clock interrupt */
    int arg		/* argument with which to call routine */
    )
    {

    /* first disconnect any existing routine */

    sysMv64360TmrCntrDisconnectRoutine(MV64360_AUXCLK_TMR);

    if (sysMv64360TmrCntrConnectRoutine(routine, arg, MV64360_AUXCLK_TMR) == OK)
	{
	return (OK);
	}
    else
	return (ERROR);
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
	sysMv64360TmrCntrDisable(MV64360_AUXCLK_TMR);
	sysAuxClkRunning = FALSE;
	}
    }

/********************************************************************************
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* RETURN: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable()
*/

void sysAuxClkEnable (void)
    {

    if (!sysAuxClkRunning)
        {
	sysMv64360AuxClkStart(MV64360_AUXCLK_TMR, sysAuxClkTicksPerSecond);
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
    return (sysAuxClkTicksPerSecond);
    }

/*******************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock. It does not
* enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond	/* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || 
        ticksPerSecond > AUX_CLK_RATE_MAX)
        return (ERROR);

    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
        {
        sysAuxClkDisable ();
        sysAuxClkEnable ();
        }
    return (OK);
    }
