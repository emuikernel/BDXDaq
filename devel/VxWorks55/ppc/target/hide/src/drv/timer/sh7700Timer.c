/* sh7700Timer.c - SH77xx on-chip Timer library */

/* Copyright 1996-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01y,25feb02,zl   fixed connect routines' memory leak, SPR #73803.
01x,02nov01,zl   corrected NULL usage.
01w,22jan01,zl   allow BSP to redefine interrupt enable.
01v,20nov00,zl   unified with sh7729Timer.c; generalized SYS_PCLK_FREQ use.
01u,02aug00,zl   connect timestamp driver interrupt only once. Simplifyed
                 SYS_PCLK_FREQ dependencies. Silicon bug fix from SH7729 code.
01t,31may00,frf  SH7751 timer for T2
01s,18sep98,hk   merged sh7750Timer.c. supported SYS_PCLK_FREQ of 33.33MHz.
01r,31aug98,st   (sh7750Timer.c) written based on sh7700 01q.
01s,28oct98,jmb  added missing sysClkRunning state flag.
01r,20jul98,jmb  added support for SYS_PCLK_FREQ of 40 and 14.75MHz.
01q,20jan98,jmc  added support for SYS_PCLK_FREQ of 20/10/5MHz.
01p,12dec97,hk   changed INUM_SYS_CLOCK to INUM_TMU0_UNDERFLOW, INUM_AUX_CLOCK
                 to INUM_TMU1_UNDERFLOW. controled INTC in sys{Aux}ClkConnect().
01o,15jul97,hk   reviewed timestamp driver code.
01n,12jul97,hk   changed inclusion of drv/multi/sh7700.h to arch/sh/sh7700.h.
01m,08may97,hk   added default TIMESTAMP_LEVEL.
01l,03may97,hk   merged in timestamp driver.
01b,17apr97,st   moved initialization of Timerstamp-Timer in
                 sysTimestampConnect() to sysTimestampEnable().
01a,26mar97,st   written sh7700TimerTS.c based on sh77700Timer.c 
01k,22mar97,hk   added support for various SYS_PCLK_FREQ values.
01j,07mar97,hk   deleted unused variable in sysClkEnable().
01i,27nov96,hk   deleted intBlock() for mmu support.
01h,17sep96,hk   added intBlock() call to SH7700 register access codes.
01g,26aug96,hk   changed to use INUM_TO_IVEC (INUM_xxx) for intConnect.
01f,21jul96,hk   changed SYS_CPU_FREQ to SYS_PCLK_FREQ (fixed to 15MHz).
01e,21jul96,hk   took ja's working file, modified.
01d,09jun96,hk   fixed sysClkEnable() and sysAuxClkEnable().
01c,07jun96,hk   changed for new macro header.
01b,22may96,ja   modified parts related with TMU (sysClkxxx).
01a,15may96,ja   written based on sh7604 01l.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
SH77xx on-chip timer unit (TMU) with a board-independant interface.
This library handles the system clock, auxiliary clock, and timestamp
timer facilities.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.
 
To include the timestamp timer facility, the macro INCLUDE_TIMESTAMP must be
defined.  The macro TIMESTAMP_LEVEL must be defined to provide the timestamp
timer's interrupt level.
*/

#include "drv/timer/timerDev.h"
#include "drv/timer/timestampDev.h"
#include "config.h"

#define TMU_MAX_FREQ		2000000			/* 2MHz max. */

#if   (SYS_PCLK_FREQ > 64 * TMU_MAX_FREQ)		/* PCLK: 128-512 MHz */
#define TICK_FREQ		(SYS_PCLK_FREQ / 256)	/* 0.5-2.0 MHz */
#define TCR_TPSC_VALUE		(TCR_TPSC_P256)
#elif (SYS_PCLK_FREQ > 16 * TMU_MAX_FREQ)		/* PCLK: 32-128 MHz */
#define TICK_FREQ		(SYS_PCLK_FREQ / 64)	/* 0.5-2.0 MHz */
#define TCR_TPSC_VALUE		(TCR_TPSC_P64)
#elif (SYS_PCLK_FREQ >  4 * TMU_MAX_FREQ)		/* PCLK: 8-32 MHz */
#define TICK_FREQ		(SYS_PCLK_FREQ / 16)	/* 0.5-2.0 MHz */
#define TCR_TPSC_VALUE		(TCR_TPSC_P16)
#else							/* PCLK: 0-8 MHz */
#define TICK_FREQ		(SYS_PCLK_FREQ / 4)	/* 0-2 MHz */
#define TCR_TPSC_VALUE		(TCR_TPSC_P4)
#endif

/* 
 * define TMU_BUG_FIXED as FALSE in the BSPs config.h file if the TSTR
 * register can't be read.
 */

#ifndef TMU_BUG_FIXED
#define TMU_BUG_FIXED		TRUE
#endif

/* Locals */ 

LOCAL int	sysClkTicksPerSecond	= 60; 
LOCAL BOOL	sysClkRunning		= FALSE; 
LOCAL FUNCPTR	sysClkRoutine		= NULL; 
LOCAL int	sysClkArg		= 0; 

LOCAL int	sysAuxClkTicksPerSecond	= 60; 
LOCAL BOOL	sysAuxClkRunning	= FALSE; 
LOCAL FUNCPTR	sysAuxClkRoutine	= NULL; 
LOCAL int	sysAuxClkArg		= 0;

#if (TMU_BUG_FIXED == FALSE)
LOCAL UINT8     sysTstrReg    	        = 0;
#endif

#ifdef	INCLUDE_TIMESTAMP

LOCAL BOOL	sysTimestampRunning = FALSE;	/* running flag */
LOCAL FUNCPTR	sysTimestampRoutine = NULL;	/* user rollover routine */
LOCAL int	sysTimestampArg     = 0;	/* arg to user routine */

#ifndef INT_LVL_TSTAMP
#define INT_LVL_TSTAMP	TIMESTAMP_LEVEL		/* for older BSPs */
#endif  /*!TIMESTAMP_LEVEL*/
#endif	/* INCLUDE_TIMESTAMP */

/*******************************************************************************
*
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
*/

LOCAL void sysClkInt (void)
    {
    volatile UINT16 stat;

    if ((stat = *TMU_TCR0) & TCR_UNF)
	{
	*TMU_TCR0 = (UINT16)(stat & (~TCR_UNF));  /* clear underflow flg */

	if (sysClkRoutine != NULL)
	    (*sysClkRoutine)(sysClkArg);
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
    FUNCPTR routine,	/* routine called at each system clock interrupt */
    int arg		/* argument with which to call routine           */
    )
    {
    static BOOL beenHere = FALSE;

    if (!beenHere)
    	{
	beenHere = TRUE;
    	sysHwInit2 ();

	if (intConnect (INUM_TO_IVEC (INUM_TMU0_UNDERFLOW), sysClkInt, 0) != OK)
	    return (ERROR);

	*TMU_TCR0 = TCR_TPSC_VALUE;

	/* set timer interrupt priority */

#ifdef SYS_TMU0_INT_ENABLE
	SYS_TMU0_INT_ENABLE (INT_LVL_SYSCLK);
#else
	/* for backward compatible BSPs */
	*INTC_IPRA = (UINT16)((*INTC_IPRA & 0x0fff) | 
			      ((INT_LVL_SYSCLK & 0xf)<<12));
#endif
	}


    sysClkRoutine = NULL; /* ensure routine not called with wrong arg */
    sysClkArg	  = arg;
    sysClkRoutine = routine;

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
	/* stop timer */
#if (TMU_BUG_FIXED == TRUE)
	*TMU_TSTR  &= ~TSTR_STR0;
#else
	sysTstrReg &= ~TSTR_STR0;
	*TMU_TSTR   = sysTstrReg;
#endif

	/* disable interrupts */
	*TMU_TCR0  &= ~(TCR_UNF | TCR_UNIE);

	sysClkRunning = FALSE;
	}
    }

/****************************************************************************** 
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
    UINT32 tmp32;

    if (!sysClkRunning)
	{
	/* stop timer */
#if (TMU_BUG_FIXED == TRUE)
	*TMU_TSTR  &= ~TSTR_STR0;
#else
	sysTstrReg &= ~TSTR_STR0;
	*TMU_TSTR   = sysTstrReg;
#endif

	/* disable interrupts */
	*TMU_TCR0  &= ~(TCR_UNF | TCR_UNIE);

	/* load const register with the number of micro seconds */
	tmp32 = (UINT32)(TICK_FREQ / sysClkTicksPerSecond - 1);
	*TMU_TCOR0  = tmp32;
	*TMU_TCNT0  = tmp32;

	/* enable clock interrupt */

	*TMU_TCR0  |= TCR_UNIE;

	/* start timer */
#if (TMU_BUG_FIXED == TRUE)
	*TMU_TSTR  |= TSTR_STR0;
#else
	sysTstrReg |= TSTR_STR0;
	*TMU_TSTR   = sysTstrReg;
#endif

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
* SEE ALSO: sysClkEnable, sysClkRateSet()
*/
 
int sysClkRateGet (void)
    {
    return (sysClkTicksPerSecond);
    }

/********************************************************************************
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be
* set.
*
* SEE ALSO: sysClkEnable, sysClkRateGet()
*/
 
STATUS sysClkRateSet
    (
    int ticksPerSecond		/* number of clock interrupts per second */
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
* sysAuxClkInt - handle auxiliary clock interrupts
*/
 
LOCAL void sysAuxClkInt (void)
    {
    volatile UINT16 stat;

    if ((stat = *TMU_TCR1) & TCR_UNF)
	{
	*TMU_TCR1 = (UINT16)(stat & (~TCR_UNF)); /* clear underflow flg */

	if (sysAuxClkRoutine != NULL)
	    (*sysAuxClkRoutine)(sysAuxClkArg);
	}
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
    FUNCPTR routine,	/* routine called at each aux clock interrupt    */
    int arg		/* argument to auxiliary clock interrupt routine */
    )
    {
    static BOOL connected = FALSE;

    if (!connected)
	{
	if (intConnect (INUM_TO_IVEC(INUM_TMU1_UNDERFLOW), sysAuxClkInt, 0) 
	    != OK)
	    return (ERROR);

	*TMU_TCR1 = TCR_TPSC_VALUE;

	/* set timer interrupt priority */

#ifdef SYS_TMU1_INT_ENABLE
	SYS_TMU1_INT_ENABLE (INT_LVL_AUXCLK);
#else
	/* for backward compatible BSPs */
	*INTC_IPRA = (UINT16)((*INTC_IPRA & 0xf0ff) | 
			      ((INT_LVL_AUXCLK & 0xf)<< 8));
#endif

	connected = TRUE;
	}

    sysAuxClkRoutine	= NULL;
    sysAuxClkRoutine	= routine;
    sysAuxClkArg	= arg;
 
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
	/* stop timer */
#if (TMU_BUG_FIXED == TRUE)
	*TMU_TSTR  &= ~TSTR_STR1;
#else
	sysTstrReg &= ~TSTR_STR1;
	*TMU_TSTR   = sysTstrReg;
#endif

	/* disable interrupts */
	*TMU_TCR1  &= ~(TCR_UNF | TCR_UNIE);

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
    UINT32 tmp32;

    if (!sysAuxClkRunning)
	{
	/* stop timer */
#if (TMU_BUG_FIXED == TRUE)
	*TMU_TSTR  &= ~TSTR_STR1;
#else
	sysTstrReg &= ~TSTR_STR1;
	*TMU_TSTR   = sysTstrReg;
#endif

	/* disable interrupts */
	*TMU_TCR1  &= ~(TCR_UNF | TCR_UNIE);

	/* load const register with the number of micro seconds */
	tmp32 = (UINT32)(TICK_FREQ / sysAuxClkTicksPerSecond - 1);
	*TMU_TCOR1  = tmp32;
	*TMU_TCNT1  = tmp32;

	/* enable clock interrupt */
	*TMU_TCR1  |= TCR_UNIE;

	/* start timer */
#if (TMU_BUG_FIXED == TRUE)
	*TMU_TSTR  |= TSTR_STR1;
#else
	sysTstrReg |= TSTR_STR1;
	*TMU_TSTR   = sysTstrReg;
#endif

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
 
    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}
    return (OK);
    } 

#ifdef	INCLUDE_TIMESTAMP
/********************************************************************************
* sysTimestampInt - timestamp timer interrupt handler
*
* This rountine handles the timestamp timer underflow interrupt.  A user
* routine is called, if one was connected by sysTimestampConnect().
*
* RETURNS: N/A
*
* SEE ALSO: sysTimestampConnect()
*/

LOCAL void sysTimestampInt (void)
    {
    volatile UINT16 stat;

    if ((stat = *TMU_TCR2) & TCR_UNF)
	{
	*TMU_TCR2 = (UINT16)(stat & (~TCR_UNF));	/* acknowledge int */

	if (sysTimestampRoutine != NULL)
	    (*sysTimestampRoutine) (sysTimestampArg);	/* call user routine */
	}
    }

/********************************************************************************
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
    static BOOL connected = FALSE;

    if (!connected)
	{
	/* connect sysTimestampInt to the appropriate interrupt vector */

	if (intConnect (INUM_TO_IVEC(INUM_TMU2_UNDERFLOW), sysTimestampInt, 0)
	    != OK)
	    return (ERROR);

	/* set timer interrupt priority */

#ifdef SYS_TMU2_INT_ENABLE
	SYS_TMU2_INT_ENABLE (INT_LVL_TSTAMP);
#else
	/* for backward compatible BSPs */
	*INTC_IPRA = (UINT16)((*INTC_IPRA & 0xff0f) | 
			      ((INT_LVL_TSTAMP & 0xf)<<4));
#endif

	connected = TRUE;
	}

    sysTimestampRoutine = NULL;
    sysTimestampRoutine	= routine;
    sysTimestampArg	= arg;

    return (OK);
    }

/********************************************************************************
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
* RETURNS: OK, or ERROR if the timestamp timer cannot be enabled.
*/
 
STATUS sysTimestampEnable (void)
    {
    UINT32 tmp32 = sysTimestampPeriod ();

    if (sysTimestampRunning)
	{
#if (TMU_BUG_FIXED == TRUE)
	*TMU_TSTR  &= ~TSTR_STR2;	/* stop timer */
#else
	sysTstrReg &= ~TSTR_STR2;
	*TMU_TSTR   = sysTstrReg;
#endif
	*TMU_TCOR2  = tmp32;		/* reset auto-reload count */
	*TMU_TCNT2  = tmp32;		/* reset counter */
#if (TMU_BUG_FIXED == TRUE)
	*TMU_TSTR  |= TSTR_STR2;	/* restart timer */
#else
	sysTstrReg |= TSTR_STR2;
	*TMU_TSTR   = sysTstrReg;
#endif
	return (OK);
	}

#if (TMU_BUG_FIXED == TRUE)
    *TMU_TSTR  &= ~TSTR_STR2;		/* stop timer */
#else
    sysTstrReg &= ~TSTR_STR2;
    *TMU_TSTR	= sysTstrReg;
#endif
    *TMU_TCR2	= TCR_TPSC_VALUE;
    *TMU_TCOR2	= tmp32;		/* reset auto-reload count */
    *TMU_TCNT2	= tmp32;		/* reset counter */

    *TMU_TCR2      |= TCR_UNIE;		/* enable underflow interrupt */
#if (TMU_BUG_FIXED == TRUE)
    *TMU_TSTR  |= TSTR_STR2;		/* start timer */
#else
    sysTstrReg |= TSTR_STR2;
    *TMU_TSTR	= sysTstrReg;
#endif

    sysTimestampRunning = TRUE;
    return (OK);
    }

/********************************************************************************
* sysTimestampDisable - disable the timestamp timer
*
* This routine disables the timestamp timer.  Interrupts are not disabled,
* although the tick counter will not increment after the timestamp timer
* is disabled, thus interrupts will no longer be generated.
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be disabled.
*/
 
STATUS sysTimestampDisable (void)
    {
    if (sysTimestampRunning)
	{
	*TMU_TCR2  &= ~(TCR_ICPE1 | TCR_ICPE0 | TCR_UNIE);
						/* disable ints */
#if (TMU_BUG_FIXED == TRUE)
	*TMU_TSTR  &= ~TSTR_STR2;		/* stop timer */
#else
	sysTstrReg &= ~TSTR_STR2;
	*TMU_TSTR   = sysTstrReg;
#endif
	*TMU_TCR2  &= ~(TCR_ICPF | TCR_UNF);	/* clear pending ints */

	sysTimestampRunning = FALSE;
	}
    return (OK);
    }

/********************************************************************************
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
    return (UINT32)(0xffffffff);	/* highest period -> freerunning */
    }

/********************************************************************************
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
    return (UINT32) TICK_FREQ;
    }

/********************************************************************************       
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
    return (sysTimestampPeriod() - *TMU_TCNT2);	/* 32bit down counter */
    }

/********************************************************************************
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
    return (sysTimestampPeriod() - *TMU_TCNT2);	/* 32bit down counter */
    }

#endif	/* INCLUDE_TIMESTAMP */
