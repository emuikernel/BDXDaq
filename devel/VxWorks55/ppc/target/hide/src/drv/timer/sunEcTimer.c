/* sunEcTimer.c - Sun EC Timer library */

/* Copyright 1984-1994 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02c,19sep94,vin  fixed a bug in the sysAuxClkEnable, changed copyright.
		 removed the SUN_COUNTER_1 reset (SPR #3426)
02b,19sep94,vin  fixed a bug in the sysClkEnable,
		 changed the SUN_IR_PENDING to SUN_IR_MASK_SET (SPR #3400)
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
*/

#include "drv/timer/timerDev.h"

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
* is called and the interrupts are acknowleged.
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
* routine is called and the interrupts are acknowleged.
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
* This routine specifies the interrupt handler to be called at each clock
* interrupt.  It does not enable system clock interrupts.  Normally, it is
* called from usrRoot() in usrConfig.c to connect usrClock() to the system
* clock interrupt.
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
        *SUN_IR_MASK_SET = IR_ALL;     /* turn off interrupts */
        *SUN_IR_MASK_SET = IR_CLOCK10; /* turn off lvl 10 */
        *SUN_IR_MASK_CLR = IR_ALL;     /* turn on interrupts */
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
* SEE ALSO: sysClkDisable(), sysClkRateSet()
*/

void sysClkEnable (void)

    {
    unsigned int intRegister;

    if (!sysClkRunning)
        {
        *SUN_IR_MASK_SET = IR_ALL;       /* turn off interrupts */
        intRegister      = *SUN_LIMIT_0; /* Clear interrupt at clock */
        *SUN_IR_MASK_SET = IR_CLOCK10;   /* Clear interrupt pending */
        *SUN_IR_MASK_CLR = IR_CLOCK10;   /* turn on lvl 10 */
	*SUN_LIMIT_0     = (1000000 / sysClkTicksPerSecond) << 10;
        *SUN_IR_MASK_CLR = IR_ALL;       /* turn on interrupts */
        sysClkRunning    = TRUE;
        }
    }

/***************************************************************************
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
* This routine specifies the interrupt handler to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock
* interrupts.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: sysAuxClkDisconnect(), sysAuxClkEnable()
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
        *SUN_COUNTER_1_CR = 1;      /* disable timer */
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
	*SUN_COUNTER_1_CR = 0;		/* enable timer */
	*SUN_LIMIT_1      = (1000000 / auxClkTicksPerSecond) << 10;

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
