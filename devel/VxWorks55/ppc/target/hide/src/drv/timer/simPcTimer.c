/* simPcTimer.c - Windows simulator timer library */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,13sep01,hbh  Fixed SPR 63532 to be able to modify the system clock rate.
		 Code cleanup.
01a,07jul98,cym  written.
*/

/*
DESCRIPTION
This driver exposes windows timer objects in a vxWorks way, allowing
them to be used as the system and auxiliary clocks.  Windows timers are cyclic,
and can be set to the millisecond.  All timers send the WM_TIMER message/
interrupt, which also passes in a timer ID.  This driver's multiplexer then
calls the appropriate driver function based on the ID.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

INCLUDES:
win_Lib.h
intLib.h
*/


/* includes */

#include "win_Lib.h"
#include "intLib.h"

/* defines */

#define TIMER_VEC 0x0113 /* from windows headers */

/* locals */

LOCAL FUNCPTR	clockRoutines[2];		/* Handler for SYS/AUX clock */
LOCAL int 	clockArgs[2];			/* Args for SYS/AUX handler */
LOCAL int 	clockRate[2] = {60,60};		/* Rate for SYS/AUX clock */
LOCAL int 	clockEnable[2];			/* Enable flag for SYS/AUX */

/***************************************************************************
*
* sysClkInt - interrupt level processing for system clock
*
* This routine handles a clock interrupt.  It demultiplexes the interrupt,
* and calls the routine installed by sysClkConnect() for that timer.
*/

LOCAL void sysClkInt
    (
    int zero,		/* ignore the parameter */
    int timerId		/* 1 = sysClk 2 = sysAuxClk */
    )
    {
    if (timerId == 1) clockRoutines[0](clockArgs[0]);
    if (timerId == 2) clockRoutines[1](clockArgs[1]);
    }

/***************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt from the system timer.  Normally, it is called from usrRoot()
* in usrConfig.c to connect usrClock() to the system clock interrupt.
*
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR	routine,
    int		arg
    )
    {
    int key = intLock ();		/* INTERRUPTS LOCKED */
 
    clockRoutines[0] = routine;
    clockArgs[0] = arg;
    intUnlock (key);

    intConnect ((VOIDFUNCPTR *)TIMER_VEC, (VOIDFUNCPTR)sysClkInt, 0);

    return(0);
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

void sysClkDisable(void)
    {
    int key = intLock ();		/* INTERRUPTS LOCKED */
    clockEnable[0] = 0;
    win_KillTimer (1);
    intUnlock (key);
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

void sysClkEnable(void)
    {
    int key = intLock();		/* INTERRUPTS LOCKED */
    clockEnable[0] = 1;
    win_SetTimer (1, 1000 / clockRate[0]);
    intUnlock (key);
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

int sysClkRateGet(void)
    {
    return (clockRate[0]);
    }

/***************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int rate
    )
    {
    if ( (rate < SYS_CLK_RATE_MIN) || (rate > SYS_CLK_RATE_MAX) )
        return (ERROR);
    clockRate[0] = rate;
    if (clockEnable[0])
        {
        win_KillTimer (1);
        win_SetTimer (1, 1000 / clockRate[0]);
        }
    return (OK);
    }

/***************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR	routine,
    int 	arg
    )
    {
    int key = intLock();		/* INTERRUPTS LOCKED */

    clockRoutines[1] = routine;
    clockArgs[1] = arg;
    intUnlock (key);

    intConnect ((VOIDFUNCPTR *)TIMER_VEC, (VOIDFUNCPTR)sysClkInt, 0);

    return (0);
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
    int key = intLock();		/* INTERRUPTS LOCKED */
    clockEnable[1] = 0;
    win_KillTimer (2);
    intUnlock (key);
    }

/***************************************************************************
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
    int key = intLock();		/* INTERRUPTS LOCKED */
    clockEnable[1] = 1;
    win_SetTimer (2, 1000 / clockRate[1]);
    intUnlock (key);
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
    return (clockRate[1]);
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
    int rate
    )
    {
    if ( (rate < AUX_CLK_RATE_MIN) || (rate > AUX_CLK_RATE_MAX) )
        return(ERROR);
    clockRate[1] = rate;
    if (clockEnable[1])
        {
        win_KillTimer (2);
        win_SetTimer (2, 1000 / clockRate[1]);
        }

    return (OK);
    }
