/* z8036Timer.c - Z8036 timer library */

/* Copyright 1984-1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,15dec92,caf  moved clock routines from ver 01y of mv135/sysLib.c, ansified.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
Zilog Z8036.  This library handles both the system clock and the auxiliary
clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

The macro ZCIO_HZ must also be defined to indicate the clock frequency
of the Z8036.

The system clock is the ZCIO counter/timer 1.
The auxiliary clock is the ZCIO counter/timer 2.

SEE ALSO
.I "ZCIO Counter/Timer and Parallel I/O Unit Technical Manual"
*/

/* locals */

LOCAL FUNCPTR sysClkRoutine           = NULL;
LOCAL int     sysClkArg               = NULL;
LOCAL int     sysClkTicksPerSecond    = 60;
LOCAL BOOL    sysClkConnected         = FALSE;
LOCAL int     sysClkRunning           = FALSE;
LOCAL FUNCPTR sysAuxClkRoutine        = NULL;
LOCAL int     sysAuxClkArg            = NULL;
LOCAL int     sysAuxClkTicksPerSecond = 60;
LOCAL BOOL    sysAuxClkConnected      = FALSE;
LOCAL int     sysAuxClkRunning        = FALSE;

/*******************************************************************************
*
* sysClkInt - clock interrupt handler
*/

LOCAL void sysClkInt (void)
    {
    char mask = *Z8036_CTIV(CIO_ADRS) & 0x06;

    if (mask & 0x04)			/* main system clock */
	{
	/* invoke user system clock routine */

	if (sysClkRoutine != NULL)
	    (*(FUNCPTR) sysClkRoutine) (sysClkArg);

	/* clear pending interupt and under service flags */

	*Z8036_CT1CS(CIO_ADRS) = ZCIO_CS_CLIPIUS;

	/* restart timer */

	*Z8036_CT1CS(CIO_ADRS) = ZCIO_CS_SIE | ZCIO_CS_GCB | ZCIO_CS_TCB;
	}
    else
	{
	/* invoke auxiliary clock routine */

	if (sysAuxClkRoutine != NULL)
	    (*(FUNCPTR) sysAuxClkRoutine) (sysAuxClkArg);

	/* clear pending interupt and under service flags */

	*Z8036_CT2CS(CIO_ADRS) = ZCIO_CS_CLIPIUS;

	/* restart timer */

	*Z8036_CT2CS(CIO_ADRS) = ZCIO_CS_SIE | ZCIO_CS_GCB | ZCIO_CS_TCB;
	}
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
* NOTE:
* The abort switch is also set up at this time.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,
    int     arg
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
	*Z8036_MCC(CIO_ADRS) &= (~ZCIO_MCC_CT1E);	/* stop interrupts */
	*Z8036_CT1CS(CIO_ADRS) = ZCIO_CS_CLIE;		/* clear intr enable */

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
    unsigned int tc;		/* time constant */

    if (!sysClkRunning)
	{
	/* initialize timer A; the timer chip has been prepared in sysHwInit */

	tc = ZCIO_HZ / sysClkTicksPerSecond;

	*Z8036_CT1CS(CIO_ADRS) = ZCIO_CS_CLIE;	/* disable interrupts */

	/* set time constant */

	*Z8036_CT1TCMSB(CIO_ADRS) = MSB(tc);
	*Z8036_CT1TCLSB(CIO_ADRS) = LSB(tc);

	*Z8036_MCC(CIO_ADRS) |= ZCIO_MCC_CT1E;	/* enable interrupts */


	/* start timer */

	*Z8036_CT1CS(CIO_ADRS)  = ZCIO_CS_SIE | ZCIO_CS_GCB | ZCIO_CS_TCB;

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
    int ticksPerSecond
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
    FUNCPTR routine,
    int     arg
    )
    {
    sysAuxClkRoutine    = routine;
    sysAuxClkArg        = arg;
    sysAuxClkConnected  = TRUE;

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
	*Z8036_MCC(CIO_ADRS) &= ~(ZCIO_MCC_CT2E);	/* stop timer */
	*Z8036_CT2CS(CIO_ADRS)= ZCIO_CS_CLIE;		/* clear intr enable */

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
    unsigned int tc;		/* time constant */

    if (!sysAuxClkRunning)
	{
	/* initialize timer B; the timer chip has been prepared in sysHwInit */

	tc = ZCIO_HZ / sysAuxClkTicksPerSecond;

	*Z8036_CT2CS(CIO_ADRS) = ZCIO_CS_CLIE;	/* disable interrupts */

	/* set time constant */

	*Z8036_CT2TCMSB(CIO_ADRS) = MSB(tc);
	*Z8036_CT2TCLSB(CIO_ADRS) = LSB(tc);

	/* enable timer B interrupts */

	*Z8036_MCC(CIO_ADRS) |= ZCIO_MCC_CT2E;

	/* Start timer B */

	*Z8036_CT2CS(CIO_ADRS) = ZCIO_CS_SIE | ZCIO_CS_GCB | ZCIO_CS_TCB;

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
* This routine sets the interrupt rate of the auxiliary clock.
* It does not enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be
* set.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
        return (ERROR);

    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }
