/* m68340Timer.c - MC68340 timer library */

/* Copyright 1984-1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,07aug92,caf  moved clock routines from ver 01i of evs340/sysLib.c, ansified.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
Motorola MC68340.  This library handles both the system clock and the auxiliary
clock functions.

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
LOCAL int sysClkPITR		= 0x182;/* 512 prescale... 60.09 Hz */

LOCAL FUNCPTR sysAuxClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysAuxClkArg		= NULL; /* its argument */
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int sysAuxClkConnected	= FALSE;
LOCAL int auxClkTicksPerSecond	= 60;

/*******************************************************************************
*
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
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
    FUNCPTR routine,    /* routine called at each system clock interrupt */
    int arg             /* argument with which to call routine           */
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
	*M340_SIM_PICR &= ~(SIM_PICR_PIRQL_MASK);	/* disable interrupts */
	sysClkRunning	= FALSE;			/* clock is stopped */
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
	*M340_SIM_PITR	= sysClkPITR;			/* set clock rate */
	*M340_SIM_PICR |= INT_LVL_SIM << 8;		/* enable interrupts */
	sysClkRunning	= TRUE;
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
    int ticksPerSecond     /* number of clock interrupts per second */
    )
    {
    FAST int count;

    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
        return (ERROR);

    /* round to best PITR count value using integer arithmetic */

    count = ((((10 * SYS_EXTAL_FREQ) / (4 * ticksPerSecond)) + 5) / 10);

    if (count < 256)
        sysClkPITR = count;
    else
	{
	count = ((((10 * SYS_EXTAL_FREQ) / (2048 * ticksPerSecond)) + 5) / 10);
        sysClkPITR = SIM_PITR_PTP | count;
	}

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
*
* RETURNS: N/A
*/

LOCAL void sysAuxClkInt (void)

    {
    /* write 1's to clear these bits. only using the TO interrupt */

    *M340_TMR1_SR    |= (TMR_SR_TC | TMR_SR_TG | TMR_SR_TO);

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
    FUNCPTR routine,    /* routine called at each aux clock interrupt    */
    int arg             /* argument to auxiliary clock interrupt routine */
    )
    {
    if (!sysAuxClkConnected &&
        (intConnect (INUM_TO_IVEC (INT_VEC_TMR1), sysAuxClkInt, 0) == ERROR))
        {
        return (ERROR);
        }

    sysAuxClkConnected = TRUE;

    sysAuxClkRoutine   = routine;
    sysAuxClkArg       = arg;

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
    *M340_TMR1_CR  = 0;		/* reset timer 1 */

    /* write 1's to clear these bits. only using the TO interrupt */

    *M340_TMR1_SR    |= (TMR_SR_TC | TMR_SR_TG | TMR_SR_TO);

    sysAuxClkRunning = FALSE;
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
    /* initiate software reset of timer 1 */

    *M340_TMR1_CR    &= ~TMR_CR_SWR;
    *M340_TMR1_SR    |= (TMR_SR_TC | TMR_SR_TG | TMR_SR_TO);

    /* fill in the preload value of timer 1 */

    *M340_TMR1_PREL1 = ((10 * SYS_CLOCK_FREQ) /
                        (512 * auxClkTicksPerSecond) + 5) / 10 - 1;

    /* configure timer 1 with x256 prescale and timeout interrupt */

    *M340_TMR1_CR = TMR_CR_PSE | TMR_CR_CPE | TMR_CR_X256 | TMR_CR_TO;

    /* release software reset of timer 1 */

    *M340_TMR1_CR |= TMR_CR_SWR;

    sysAuxClkRunning = TRUE;
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
* sysAuxClkRateSet - set auxiliary clock rate
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
