/* m68360Timer.c - Motorola MC68360 timer library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01f,13may97,map  updated *Enable(), and *Disable() to use STP bits [SPR# 8473]
                 fixed prescaler use in sysTimestamp*() [SPR# 8433]
01e,31oct96,wlf  doc: cleanup.
01d,28may96,dat  fixed SPR #5430, fixed clearing of status flags
01c,12nov94,kdl	 made include of timestampDev.h conditional on INCLUDE_TIMESTAMP
01b,10jun94,jds	 included timestamp driver functionality
01a,01aug93,dzb  derived from version 01c of timer/m68302Timer.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
Motorola MC68360.  This library handles both the system clock and the auxiliary
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

LOCAL FUNCPTR sysAuxClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysAuxClkArg		= NULL; /* its argument */
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int auxClkConnected	= FALSE;
LOCAL int auxClkTicksPerSecond	= 60;

#ifdef  INCLUDE_TIMESTAMP

#include "drv/timer/timestampDev.h"

LOCAL BOOL    sysTimestampRunning  = FALSE;     /* if running - TRUE */
LOCAL FUNCPTR sysTimestampRoutine  = NULL;      /* user rollover routine */
LOCAL int     sysTimestampArg      = NULL;      /* arg to user routine */

#endif  /* INCLUDE_TIMESTAMP */

/*******************************************************************************
*
* sysClkInt - system clock interrupt handler
*
* This routine handles the system clock interrupt.  It calls a user routine
* if one was specified by the routine sysClkConnect().
*/

LOCAL void sysClkInt (void)
    {
    *M360_CPM_TER1(SYS_TMR_BASE) |= TMR_TER_REF;     /* clear event register */
    *M360_CPM_CISR(SYS_TMR_BASE) = CPIC_CIXR_TMR1;  /* clear in-service bit */

    if (sysClkRoutine != NULL)
        (*sysClkRoutine) (sysClkArg);
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
    *M360_CPM_TER2(AUX_TMR_BASE) |= TMR_TER_REF;     /* clear event register */
    *M360_CPM_CISR(AUX_TMR_BASE) = CPIC_CIXR_TMR2;  /* clear in-service bit */

    if (sysAuxClkRoutine != NULL)
        (*sysAuxClkRoutine) (sysAuxClkArg);
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
    if (sysClkRunning)
	{
	*M360_CPM_CIMR(SYS_TMR_BASE) &= ~CPIC_CIXR_TMR1; /* disable interrupt */
	*M360_CPM_TGCR(SYS_TMR_BASE) |= TMR_TGCR_STP1;	 /* stop the timer */

	sysClkRunning = FALSE;			/* clock is no longer running */
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
    UINT32  tempDiv = SYS_CPU_FREQ / (sysClkTicksPerSecond << 8);

    if ((!sysClkRunning) && (tempDiv <= USHRT_MAX * 16))
	{
        /* start, reset, but disable timer 1 */
        
        *M360_CPM_TGCR(SYS_TMR_BASE) &= ~(TMR_TGCR_RST1 | TMR_TGCR_STP1);

        *M360_CPM_TCN1(SYS_TMR_BASE) = 0x0;		/* reset timer count */

	if (tempDiv <= USHRT_MAX)
	    {
            *M360_CPM_TRR1(SYS_TMR_BASE) = (UINT16) tempDiv;
            *M360_CPM_TMR1(SYS_TMR_BASE) = TMR_TMR_ICLK_CLK | TMR_TMR_ORI |
                                           TMR_TMR_FRR | (TMR_TMR_PS & 0xff00);
	    }
	else
	    {
            *M360_CPM_TRR1(SYS_TMR_BASE) = (UINT16) (tempDiv / 16);
            *M360_CPM_TMR1(SYS_TMR_BASE) = TMR_TMR_ICLK_CLK16 | TMR_TMR_ORI |
                                           TMR_TMR_FRR | (TMR_TMR_PS & 0xff00);
	    }
 
        *M360_CPM_TER1(SYS_TMR_BASE) = 0xffff;		/* clear event */
        *M360_CPM_CIMR(SYS_TMR_BASE) |= CPIC_CIXR_TMR1; /* unmask interupt */

        *M360_CPM_TGCR(SYS_TMR_BASE) |= TMR_TGCR_RST1;	/* enable timer 1 */

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
    int     arg		/* argument to auxiliary clock interrupt routine */
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
	*M360_CPM_CIMR(AUX_TMR_BASE) &= ~CPIC_CIXR_TMR2; /* disable interrupt */
	*M360_CPM_TGCR(AUX_TMR_BASE) |= TMR_TGCR_STP2;	 /* stop the timer */

	sysAuxClkRunning = FALSE;		/* clock is no longer running */
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
    UINT32  tempDiv = SYS_CPU_FREQ / (auxClkTicksPerSecond << 8);

    if ((!sysAuxClkRunning) && (tempDiv <= USHRT_MAX * 16))
	{
        /* start, reset, but disable timer 2 */
        *M360_CPM_TGCR(AUX_TMR_BASE) &= ~(TMR_TGCR_RST2 | TMR_TGCR_STP2);

        *M360_CPM_TCN2(AUX_TMR_BASE) = 0x0;		/* reset timer count */

	if (tempDiv <= USHRT_MAX)
	    {
            *M360_CPM_TRR2(AUX_TMR_BASE) = (UINT16) tempDiv;
            *M360_CPM_TMR2(AUX_TMR_BASE) = TMR_TMR_ICLK_CLK | TMR_TMR_ORI |
                                           TMR_TMR_FRR | (TMR_TMR_PS & 0xff00);
	    }
	else
	    {
            *M360_CPM_TRR2(AUX_TMR_BASE) = (UINT16) (tempDiv / 16);
            *M360_CPM_TMR2(AUX_TMR_BASE) = TMR_TMR_ICLK_CLK16 | TMR_TMR_ORI |
                                           TMR_TMR_FRR | (TMR_TMR_PS & 0xff00);
	    }
 
        *M360_CPM_TER2(AUX_TMR_BASE) = 0xffff;		/* clear event */
        *M360_CPM_CIMR(AUX_TMR_BASE) |= CPIC_CIXR_TMR2; /* unmask interupt */

        *M360_CPM_TGCR(AUX_TMR_BASE) |= TMR_TGCR_RST2;	/* enable timer 2 */

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

LOCAL void sysTimestampInt (void)
    {
    *M360_CPM_TER4(SYS_TMR_BASE) |= TMR_TER_REF; /* clear TMR4 reference bit */
    *M360_CPM_CISR(SYS_TMR_BASE) = CPIC_CIXR_TMR4; /* clear TMR4 interrupt 
								register bit */
    if (sysTimestampRoutine != NULL)    /* call user routine */
        (*sysTimestampRoutine) (sysTimestampArg);
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
    FUNCPTR routine,    /* routine called at each timestamp timer interrupt */
    int arg             /* argument with which to call routine */
    )
    {
    sysTimestampRoutine = routine;
    sysTimestampArg = arg;

    return (OK);
    }


/*******************************************************************************
*
* sysTimestampEnable - initialize and enable the timestamp timer
*
*/
STATUS sysTimestampEnable (void)
    {
    if (sysTimestampRunning)
        {
        *M360_CPM_TCN3(SYS_TMR_BASE) = (UINT32) 0;	/* clear the counter */
        return (OK);
        }

    (void) intConnect (INUM_TO_IVEC (INT_VEC_TMR4(SYS_TMR_BASE)),
                           (VOIDFUNCPTR) sysTimestampInt, NULL);

    sysTimestampRunning = TRUE;

    /* cascade timer 3 and 4 in order to get a 32 bit timer */

    *M360_CPM_TGCR(SYS_TMR_BASE) |= TMR_TGCR_CAS4;

    /* start, reset, but disable timers 3 and 4 */
    
    *M360_CPM_TGCR(SYS_TMR_BASE) &= ~(TMR_TGCR_RST4 | TMR_TGCR_RST3 |
                                      TMR_TGCR_STP4 | TMR_TGCR_STP3 );

    /* When timers 3 and 4 are in cascade mode, TMR4 is the mode register for
     * the cascaded 32 bit timer, while TMR3 is ignored.
     * Use prescaler of zero to divide the clock (SYS_CPU_FREQ) by 1.
     */
    *M360_CPM_TMR4(SYS_TMR_BASE) = 0x0000 | TMR_TMR_ICLK_CLK | TMR_TMR_ORI |
								 TMR_TMR_FRR;
    *M360_CPM_TMR3(SYS_TMR_BASE) = 0x0;

    *((UINT32 *)M360_CPM_TRR3(SYS_TMR_BASE)) = (UINT32) sysTimestampPeriod ();
    *((UINT32 *)M360_CPM_TCN3(SYS_TMR_BASE)) = (UINT32) 0;
    *M360_CPM_TER4(SYS_TMR_BASE) = (UINT16) 0xffff;
    *M360_CPM_CIMR(SYS_TMR_BASE) |= CPIC_CIXR_TMR4;
    *M360_CPM_TGCR(SYS_TMR_BASE) |= TMR_TGCR_RST4 | TMR_TGCR_RST3; /* enable */
    return (OK);
    }


/*******************************************************************************
*
* sysTimestampDisable - turn off auxiliary clock interrupts
*
* This routine disables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampDisable (void)
    {
    if (sysTimestampRunning)
        {
        *M360_CPM_CIMR(SYS_TMR_BASE) &= ~CPIC_CIXR_TMR4; /* disable interrupt */
        *M360_CPM_CIMR(SYS_TMR_BASE) &= ~CPIC_CIXR_TMR3; /* disable interrupt */
        *M360_CPM_TGCR(SYS_TMR_BASE) |= TMR_TGCR_STP4;	/* stop timer */
        *M360_CPM_TGCR(SYS_TMR_BASE) |= TMR_TGCR_STP3;	/* stop timer */

        sysTimestampRunning = FALSE;            /* clock is no longer running */
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
    return (0xffffffff);        /* highest period -> freerunning */
    }

/*******************************************************************************
*
* sysTimestampFreq - get the timestamp timer clock frequency
*
* This routine returns the frequency of the timer clock, in ticks per second.
* The rate of the timestamp timer should be set explicitly within the BSP,
* in the sysHwInit() routine.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    UINT32 divider;

    /*
     * Get the prescaler value from TMR4. TMR3 is ignored in cascade mode.
     */
    divider = (*M360_CPM_TMR4(SYS_TMR_BASE) >> 8) + 1;
    return (SYS_CPU_FREQ / divider);
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
    return (*((UINT32 *) M360_CPM_TCN3(SYS_TMR_BASE)));
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
    return (*((UINT32 *) M360_CPM_TCN3(SYS_TMR_BASE)));
    }

#endif  /* INCLUDE_TIMESTAMP */
