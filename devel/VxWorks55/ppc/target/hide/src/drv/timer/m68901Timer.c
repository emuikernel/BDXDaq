/* m68901Timer.c - MC68901 Multi-Function Peripheral (MFP) timer driver */

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,30jan93,dzb  separated sys,aux clocks from particular timers.
		 added support for user-defined prescale values.
		 icreased auxClkTicksPerSecond to fit prescale=4 (worst case).
01a,04jan93,caf  derived from version 02a of config/mv133/sysLib.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
MFP chip with a board-independent interface.  This library handles both
the system clock and the auxiliary clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.  Additionally, the system and auxiliary
clocks must each be assigned to one of the four MFP timers.  The 
tyCoDv.sys and tyCoDv.aux structures must be intialized to point to
the appropriate registers and constants.
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
LOCAL int auxClkTicksPerSecond	= 2400; /* min freq if prescale=4,hz=2.4Mhz */

/* forward declarations */

LOCAL void	sysClkInt ();
LOCAL void	sysAuxClkInt ();

/*******************************************************************************
*
* sysClkInt - interrupt level processing for system clock
*
* This routine handles the system clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysClkConnect().
*
* RETURNS: N/A
*/

LOCAL void sysClkInt (void)
    {
    if (sysClkRoutine != NULL)
        (*sysClkRoutine) (sysClkArg);
    }

/*******************************************************************************
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
    FUNCPTR routine,	/* routine called at each clock interrupt */
    int     arg		/* argument with which to call routine    */
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
    if (sysClkRunning)
	{
	*tyCoDv.sys.ier &= ~tyCoDv.sys.iMask;	/* disable interrupt */

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
    if (!sysClkRunning)
	{
	*tyCoDv.sys.cr &= ~tyCoDv.sys.cMask;	/* clear previous mode value */

	if (tyCoDv.sys.prescale == 100)
	    *tyCoDv.sys.cr |= (MFP_TCR_DELAY_100 & tyCoDv.sys.cMask);
        else
	    {
	    *tyCoDv.sys.cr |= (MFP_TCR_DELAY_200 & tyCoDv.sys.cMask);
	    tyCoDv.sys.prescale = 200;
	    }

	/* write the timer value (with prescale) */

	*tyCoDv.sys.dr = tyCoDv.baudFreq / (tyCoDv.sys.prescale *
					   sysClkTicksPerSecond);

	*tyCoDv.sys.imr |= tyCoDv.sys.iMask;	/* set mask */
	*tyCoDv.sys.ier |= tyCoDv.sys.iMask;	/* enable interrupt */

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
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX
	|| ticksPerSecond < (tyCoDv.baudFreq/(256*tyCoDv.sys.prescale))
	|| ticksPerSecond > (tyCoDv.baudFreq/tyCoDv.sys.prescale))
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
* sysAuxClkInt - interrupt level processing for auxiliary clock
*
* This routine handles the auxiliary clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysAuxClkConnect().
*/

LOCAL void sysAuxClkInt (void)
    {
    if (sysAuxClkRoutine != NULL)
        (*sysAuxClkRoutine) (sysAuxClkArg);
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
    FUNCPTR  routine,	/* routine called at each aux. clock interrupt */
    int      arg	/* argument with which to call routine         */
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
    if (sysAuxClkRunning)
	{
	*tyCoDv.aux.ier &= ~tyCoDv.aux.iMask;	/* disable interrupt */

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
* SEE ALSO: sysAuxClkConnect(),sysAuxClkDisable(), sysAuxClkRateSet()
*/

void sysAuxClkEnable (void)
    {
    if (!sysAuxClkRunning)
	{
	*tyCoDv.aux.cr &= ~tyCoDv.aux.cMask;	/* clear previous mode value */

	switch (tyCoDv.aux.prescale)
	    {
	    case 4:
		*tyCoDv.aux.cr |= (MFP_TCR_DELAY_4 & tyCoDv.aux.cMask);
		break;
	    case 10:
		*tyCoDv.aux.cr |= (MFP_TCR_DELAY_10 & tyCoDv.aux.cMask);
		break;
	    case 16:
		*tyCoDv.aux.cr |= (MFP_TCR_DELAY_16 & tyCoDv.aux.cMask);
		break;
	    case 50:
		*tyCoDv.aux.cr |= (MFP_TCR_DELAY_50 & tyCoDv.aux.cMask);
		break;
	    case 64:
		*tyCoDv.aux.cr |= (MFP_TCR_DELAY_64 & tyCoDv.aux.cMask);
		break;
	    case 100:
		*tyCoDv.aux.cr |= (MFP_TCR_DELAY_100 & tyCoDv.aux.cMask);
		break;
	    default:
	        *tyCoDv.aux.cr |= (MFP_TCR_DELAY_200 & tyCoDv.aux.cMask);
	        tyCoDv.aux.prescale = 200;
	    }

	/* write the timer value (with prescale) */

	*tyCoDv.aux.dr = tyCoDv.baudFreq / (tyCoDv.aux.prescale *
					   auxClkTicksPerSecond);

	*tyCoDv.aux.imr |= tyCoDv.aux.iMask;	/* set mask */
	*tyCoDv.aux.ier |= tyCoDv.aux.iMask;	/* enable interrupt */

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
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX
	|| ticksPerSecond < (tyCoDv.baudFreq/(256*tyCoDv.aux.prescale))
	|| ticksPerSecond > (tyCoDv.baudFreq/tyCoDv.aux.prescale))
	return (ERROR);

    auxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }
