/* sh7615Timer.c - template timer library */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,21feb02,h_k  fixed timestamp interrupt enable (SPR #73506).
                 removed sysTimestampPeriodValue to avoid compile warning.
01f,02nov01,zl   corrected NULL usage.
01e,06oct00,zl   corrected vectors for AUX and timestamp drivers.
01d,16sep00,zl   timestamp to use maximum period.
01c,30aug00,zl   fixed INTC_IPRD modification.
01b,20aug00,csi  added AUX clock and timestamp.
01a,11aug00,zl   written.
*/


/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
SH7615 on-chip timer unit (TPU) with a board-independant interface.
This library handles the system clock, auxiliary clock, and timestamp
timer facilities.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

To include the timestamp timer facility, the macro INCLUDE_TIMESTAMP must be
defined.  The macro TIMESTAMP_LEVEL must be defined to provide the timestamp
timer's interrupt level.

INCLUDES:
timestampDev.h
*/

/* includes */

#include "vxWorks.h"
#include "config.h"
#include "drv/timer/timerDev.h"
#include "drv/timer/timestampDev.h"


/* select tick frequency */

#define TICK_FREQ	(SYS_PCLK_FREQ / 64)
#define TCR_DIVIDE	TCR_DIV64

/* The default is to assume memory mapped I/O */

#ifndef SH7615TPU_READ
#define SH7615TPU_READ(reg, result) \
	((result) = *(reg))
#endif /* SH7615TPU_READ */

#ifndef SH7615TPU_WRITE
#define SH7615TPU_WRITE(reg, data) \
	(*(reg) = (data))
#endif /* SH7615TPU_WRITE */

#ifndef SH7615TPU_SET
#define SH7615TPU_SET(reg, data) \
	(*(reg) |= (data))
#endif /* SH7615TPU_SET */

#ifndef SH7615TPU_CLEAR
#define SH7615TPU_CLEAR(reg, data) \
	(*(reg) &= ~(data))
#endif /* SH7615TPU_CLEAR */


/* locals */

LOCAL FUNCPTR sysClkRoutine	= NULL; /* routine to call on clock tick */
LOCAL int sysClkArg		= 0; /* its argument */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkTicksPerSecond	= 60;

LOCAL FUNCPTR sysAuxClkRoutine	= NULL;
LOCAL int sysAuxClkArg		= 0;
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int sysAuxClkTicksPerSecond = 100;

#ifdef INCLUDE_TIMESTAMP

LOCAL BOOL	sysTimestampRunning	= FALSE; /* running flag */
LOCAL FUNCPTR	sysTimestampRoutine	= NULL;  /* routine to call on intr */
LOCAL int	sysTimestampArg		= 0;     /* arg for routine */
      void	sysTimestampInt (void);		 /* forward declaration */

#endif  /* INCLUDE_TIMESTAMP */


/*******************************************************************************
*
* sysClkInt - interrupt level processing for system clock
*
* This routine handles an auxiliary clock interrupt.  It acknowledges the
* interrupt and calls the routine installed by sysClkConnect().
*/

void sysClkInt (void)
    {
    UINT8 value;

    if (SH7615TPU_READ (TPU_TSR0, value), (value & TSR_TGFA) == TSR_TGFA)
	{
	/* clear the interrupt */

	SH7615TPU_WRITE (TPU_TSR0, (value & ~TSR_TGFA));
	
	/* call system clock service routine */

	if (sysClkRoutine != NULL)
	    (* sysClkRoutine) (sysClkArg);
	}
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
    FUNCPTR routine,	/* routine to be called at each clock interrupt */
    int arg		/* argument with which to call routine */
    )
    {
    static BOOL beenHere = FALSE;

    if (!beenHere)
	{
	beenHere = TRUE;
	sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */
	}

    sysClkRoutine   = NULL;
    sysClkArg	    = arg;
    sysClkRoutine   = routine;

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
	/* disable system timer interrupts */

	SH7615TPU_WRITE (TPU_TIER0, 0);

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
    static BOOL connected = FALSE;

    if (!connected)
	{
	UINT16 tmp;

	/* Disable interrupt and clear the status register */

	SH7615TPU_WRITE (TPU_TIER0, 0);
	SH7615TPU_READ  (TPU_TSR0, tmp);
	SH7615TPU_WRITE (TPU_TSR0, 0);

	/* connect handler */

	intConnect(IV_TPU_TGI0A, sysClkInt, 0);

	/* Set system clock interrupt priority */
    
	SH7615TPU_READ  (INTC_IPRD, tmp);
	SH7615TPU_WRITE (INTC_IPRD, (tmp & 0x0fff) | (INT_LVL_SYSCLK << 12));

	sysClkRunning = FALSE;
	connected = TRUE;
	}

    if (!sysClkRunning)
	{
	/* select counter at F/64 , clear with TGRA */

	SH7615TPU_WRITE (TPU_TCR0, TCR_DIVIDE | TCR_TGRA);

	/* set period */

	SH7615TPU_WRITE (TPU_TGR0A, (TICK_FREQ / sysClkTicksPerSecond));

	/* Reset counter */

	SH7615TPU_WRITE (TPU_TCNT0, 0);

	/* enable interrupt */

	SH7615TPU_WRITE (TPU_TIER0, TIER_TGIEA);
	
	/* start counter */

	SH7615TPU_SET (TPU_TSTR, TSTR_CST0);

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
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
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
* sysAuxClkInt - handle an auxiliary clock interrupt
*
* This routine handles an auxiliary clock interrupt.  It acknowledges the
* interrupt and calls the routine installed by sysAuxClkConnect().
*
* RETURNS: N/A
*/

void sysAuxClkInt (void)
    {
    UINT8 value;

    /* acknowledge the interrupt if needed */

    if (SH7615TPU_READ (TPU_TSR1, value), (value & TSR_TGFA) == TSR_TGFA)
        {
        /* clear the interrupt */

        SH7615TPU_WRITE (TPU_TSR1, (value & ~TSR_TGFA));

        /* call auxiliary clock service routine */

        if (sysAuxClkRoutine != NULL)
	    (*sysAuxClkRoutine) (sysAuxClkArg);
        }
    }

/*******************************************************************************
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
    FUNCPTR routine,    /* routine called at each aux clock interrupt */
    int arg             /* argument to auxiliary clock interrupt routine */
    )
    {
    sysAuxClkRoutine	= NULL;
    sysAuxClkArg	= arg;
    sysAuxClkRoutine	= routine;

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
        /* disable system timer interrupts */

	SH7615TPU_WRITE (TPU_TIER1, 0);

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
    static BOOL connected = FALSE;

    if (!connected)
	{
	UINT16 tmp;

	/* Disable interrupts until it's safe */

	SH7615TPU_WRITE (TPU_TIER1, 0);
	SH7615TPU_READ  (TPU_TSR1, tmp);
	SH7615TPU_WRITE (TPU_TSR1, 0);

	/* Connect handler */

	intConnect (IV_TPU_TGI1A, sysAuxClkInt, 0);

	/* Set AUX clock interrupt priority */
    
	SH7615TPU_READ  (INTC_IPRD, tmp);
	SH7615TPU_WRITE (INTC_IPRD, (tmp & 0xf0ff) | (INT_LVL_AUXCLK << 8));

	connected = TRUE;
	sysAuxClkRunning = FALSE;
	}

    if (!sysAuxClkRunning)
        {
        /* select counter at F/64 , clear with TGRA */

	SH7615TPU_WRITE (TPU_TCR1, TCR_DIVIDE | TCR_TGRA);

	/* set period */

	SH7615TPU_WRITE (TPU_TGR1A, (TICK_FREQ / sysAuxClkTicksPerSecond));

	/* Reset counter */

	SH7615TPU_WRITE (TPU_TCNT1, 0);

	/* enable interrupt */

	SH7615TPU_WRITE (TPU_TIER1, TIER_TGIEA);

	/* start counter */

	SH7615TPU_SET (TPU_TSTR, TSTR_CST1);

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
* This routine sets the interrupt rate of the auxiliary clock.  It does not
* enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond  /* number of clock interrupts per second */
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

#ifdef  INCLUDE_TIMESTAMP

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

void sysTimestampInt (void)
    {
    UINT8 value;

    if (SH7615TPU_READ (TPU_TSR2, value), (value & TSR_TGFA) == TSR_TGFA)
	{
	/* clear the interrupt */

        SH7615TPU_WRITE (TPU_TSR2, (value & ~TSR_TGFA));

        /* call system clock service routine */

        if (sysTimestampRunning && sysTimestampRoutine != NULL)
	    (*sysTimestampRoutine)(sysTimestampArg);
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
    sysTimestampRoutine = NULL;		/* set to NULL to avoid calling old */
					/* routine with new argument */

    sysTimestampArg = arg;
    sysTimestampRoutine = routine;

    return OK;
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
* RETURNS: OK, or ERROR if hardware cannot be enabled.
*/

STATUS sysTimestampEnable (void)
    {
    static BOOL connected = FALSE;

    if (!connected)
	{
	UINT16 tmp;

	/* Disable interrupts until it's safe */

	SH7615TPU_WRITE (TPU_TIER2, 0);
	SH7615TPU_READ  (TPU_TSR2, tmp);
	SH7615TPU_WRITE (TPU_TSR2, 0);

	/* connect sysTimestamp to the appropriate interrupt */

	if (intConnect(IV_TPU_TGI2A, sysTimestampInt, 0) != OK)
	    return (ERROR);

	/* Set timestamp interrupt priority */
    
	SH7615TPU_READ  (INTC_IPRD, tmp);
	SH7615TPU_WRITE (INTC_IPRD, (tmp & 0xff0f) | (INT_LVL_TSTAMP << 4));

	connected = TRUE;
	}

    if (!sysTimestampRunning)
	{
        /* select counter at F/64 , clear with TGRA */

	SH7615TPU_WRITE (TPU_TCR2, TCR_DIVIDE | TCR_TGRA);

	/* set rate */

	SH7615TPU_WRITE (TPU_TGR2A, 0xffff);

	/* Reset counter */

	SH7615TPU_WRITE (TPU_TCNT2, 0);

        /* enable interrupt */

        SH7615TPU_WRITE (TPU_TIER2, TIER_TGIEA);

        /* start counter */
	
	SH7615TPU_SET (TPU_TSTR, TSTR_CST2);

	sysTimestampRunning = TRUE;
	}

    return (OK);
    }

/*******************************************************************************
*
* sysTimestampDisable - disable the timestamp timer
*
* This routine disables the timestamp timer.  
*
* RETURNS: OK, or ERROR if timer cannot be disabled.
*/

STATUS sysTimestampDisable (void)
    {
    if (sysTimestampRunning)
        {
	/* Stop timer */

	SH7615TPU_CLEAR (TPU_TSTR, TSTR_CST2);

        /* disable system timer interrupts */

        SH7615TPU_WRITE (TPU_TIER2, 0);

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
    return (UINT32)(0xffff);        /* highest period -> freerunning */
    }

/*******************************************************************************
*
* sysTimestampFreq - get the timestamp timer clock frequency
*
* This routine returns the frequency of the timer clock, in ticks per second.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    return (UINT32)(TICK_FREQ);
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
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampLock()
*/

UINT32 sysTimestamp (void)
    {
    UINT16 result;
    SH7615TPU_READ (TPU_TCNT2, result);
    return (UINT32) result;
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
    UINT16 result;
    SH7615TPU_READ (TPU_TCNT2, result);
    return (UINT32) result;
    }

#endif  /* INCLUDE_TIMESTAMP */
