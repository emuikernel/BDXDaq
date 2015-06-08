/* mb96840Timer.c - SPARClite Companion Chip Timer library */
   
/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01g,14dec96,dat  fixed SPR 2702, documenting HZ and HZprescale.
01f,30may96,wlf  doc: cleanup.
01e,27apr94,jkw  Added macro INT_AUXCLOCK, IRC_REQUEST_AUXCLK. and
                 IRC_REQUEST_SYSCLK for Auxiliary and System clock
                 to make the sysClkEnable and sysAuxClkEnable 
                 Interrupt level independent of the board
01d,04feb94,jkw  changed the Auxiliary clock level to 1  
01c,07jul93,vin  removed intConnect calls from sysClkConnect and 
                 sysAuxClkConnect and placed them in sysHwInit2
01b,13nov92,jwt  clean up; auxClkTicksPerSecond from 60 to 310 per ccc.
01a,20aug92,ccc  created by moving routines from sysLib.c of mb930.
		 fixed sysAuxClkRateSet() to set aux ticks properly.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions for
the SPARClite with a board-independent interface.  This library
handles both the system clock and the auxiliary clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

The global values HZ and HZprescale are  expected to contain the clock
rate and the prescale divider value.
*/

#include "drv/timer/timerDev.h"

/* globals */

IMPORT int	HZ;		/* clock rate */
IMPORT int	HZprescale;	/* clock prescale value, normally 200 */

/* Locals */ 

LOCAL int     sysClkTicksPerSecond = 60; 
LOCAL BOOL    sysClkRunning        = FALSE; 
LOCAL FUNCPTR sysClkRoutine        = NULL; 
LOCAL int     sysClkArg            = NULL; 
LOCAL BOOL    sysClkConnected      = FALSE; 

LOCAL int     auxClkTicksPerSecond = 310; 
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
    }

/***************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c to 
* connect usrClock() to the system clock interrupt.
*
* INTERNAL
* Note that the system clock uses timer 1 interrupts.  This allows the
* use of the pre-scale and a wider range of values.  Otherwise, the
* slowest rate would be 310 ticks per second (the usual 60 or 100 ticks
* per second would not work).
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
        sys940AsiSeth ((void *) (TIMER1_CONTROL_ADDR), SPARC_TIMER_SETUP);

        sysClkRunning = FALSE;
        }
    }

/***************************************************************************
*
* sysClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.  The system clock uses Timer 1
* interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkConnect(), sysClkDisable(), sysClkRateSet()
*/

void sysClkEnable (void)

    {
    int clockrate;

    if (!sysClkRunning)
        {
	clockrate = (HZ / HZprescale) / sysClkTicksPerSecond;

        sys940AsiSeth ((void *) (TIMER1_CONTROL_ADDR),
                    SPARC_TIMER_SETUP | TIMER_CONT_PRESCALE); /* Enable */
        sys940AsiSeth ((void *) (TIMER1_PRESCALE_ADDR),
                   HZprescale);    /* Set prescale */

        /* clear any pending IRQ before we start timer */

        sys940AsiSeth ((void *) (IRC_REQ_CLEAR_ADDR), IRC_REQUEST_SYSCLK);
        sys940AsiSeth ((void *) (TIMER1_RELOAD_ADDR),
                   clockrate); /* Set reload->GO */

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
* This routine sets the interrupt rate of the system clock.  It does not
* enable system clock interrupts.  It is called by usrRoot() in
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
	sysClkEnable ();	/* new count takes after mode set */
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
    FUNCPTR routine,    /* routine called at each aux. clock interrupt   */
    int     parm        /* argument to auxiliary clock interrupt routine */
    )
    {

    sysAuxClkConnected = TRUE;
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

        /* STOP TIMER */

        sys940AsiSeth ((void *) (TIMER2_CONTROL_ADDR),
                   SPARC_TIMER_SETUP); /* Enable */

        /* clear any pending interrupts after we unmask */

        sysIntDisable (INT_AUXCLOCK);   /* mask interrupt */
        sys940AsiSeth ((void *) (IRC_REQ_CLEAR_ADDR), IRC_REQUEST_AUXCLK);

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
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable(), sysAuxClkRateSet()
*/

void sysAuxClkEnable (void)

    {
    if (!sysAuxClkRunning)
        {
        sys940AsiSeth ((void *) (TIMER2_CONTROL_ADDR),
                   SPARC_TIMER_SETUP); /* Enable */

        /* clear any pending IRQ before we start timer */

        sys940AsiSeth ((void *) (IRC_REQ_CLEAR_ADDR), IRC_REQUEST_AUXCLK);
        sysIntEnable (INT_AUXCLOCK);              /* unmask interrupt */
        sys940AsiSeth ((void *) (TIMER2_RELOAD_ADDR),
                    HZ / auxClkTicksPerSecond); /* Set reload->GO */

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

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();		/* clock restarts upon new value */
	}

    return (OK);
    }
