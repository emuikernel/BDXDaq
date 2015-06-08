/* sh7032Timer.c - SH7032 on-chip Timer library */

/* Copyright 1994-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01n,13dec97,hk   changed TICK_FREQ to ITU_TICK_FREQ, INT_VEC_CLOCK to IV_ITU0_
                 IMIA0, INT_VEC_AUX_CLOCK to IV_ITU1_IMIA1. added INTC control
                 to sys{Aux}ClkConnect(). reviewed timestamp driver code.
01m,12jul97,hk   changed inclusion of drv/multi/sh7032.h to arch/sh/sh7000.h.
01l,16may97,st   added default TIMESTAMP_LEVEL.
01k,03may97,hk   merged in timestamp driver.
01d,18apr97,st   moved initialization of Timerstamp-Timer in
                 sysTimestampConnect() to sysTimestampEnable().
01j,06mar97,st   added support for timestamp function for windview
01i,14nov95,hk   code review. added more type casting to raw digits and macros.
01a,27jul95,sa   written sh7032TimerTS.c.
01h,19jun95,hk   code review.
01g,08jun95,hk   deleted SH7032_ITU_SYSCLK_INCLUDE/SH7032_ITU_SYSAUXCLK_INCLUDE.
01f,28feb95,hk   changed sysClkConnect() and sysAuxClkConnect() to follow the
		 corrected IV_ define in ivSh.h-01e. copyright 1995.
01e,26dec94,sa   fixed sysClkInt(), sysAuxClkInt().
		 removed SH7032_SYSCLK_INT_DISABLE,SH7032_SYSAUXCLK_INT_DISABLE.
01d,13oct94,sa   fixed sysAuxClkInt().
01c,05oct94,sa   added sysHwInit2() in sysClkConnect().
01b,05oct94,sa   debug.
01a,27sep94,sa   derived from sh7604Timer.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
SH7032 on-chip timer unit (ITU) with a board-independant interface.
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
#include "arch/sh/sh7000.h"
#include "config.h"

/* Locals */ 

#define ITU_TICK_FREQ		(SYS_CPU_FREQ / 8)

LOCAL int	sysClkTicksPerSecond	= 60; 
LOCAL BOOL	sysClkRunning		= FALSE; 
LOCAL FUNCPTR	sysClkRoutine		= NULL; 
LOCAL int	sysClkArg		= NULL; 
LOCAL BOOL	sysClkConnected		= FALSE; 

LOCAL int	auxClkTicksPerSecond	= 60; 
LOCAL BOOL	sysAuxClkRunning	= FALSE; 
LOCAL FUNCPTR	sysAuxClkRoutine	= NULL; 
LOCAL int	sysAuxClkArg		= NULL;
LOCAL BOOL	auxClkConnected		= FALSE; 

#ifdef	INCLUDE_TIMESTAMP
LOCAL BOOL	sysTimestampRunning	= FALSE;  /* running flag */
LOCAL FUNCPTR	sysTimestampRoutine	= NULL;   /* user rollover routine */
LOCAL int	sysTimestampArg		= NULL;   /* arg to user routine */
#ifndef TIMESTAMP_LEVEL
#error TIMESTAMP_LEVEL is not defined
#endif /* ! TIMESTAMP_LEVEL */
#endif /* INCLUDE_TIMESTAMP */

/*******************************************************************************
*
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
*/

LOCAL void sysClkInt (void)
    {
    UINT8 stat;

    if ((stat = *ITU_TSR0) & ITU_TSR_IMFA)
	{
	*ITU_TSR0 = (UINT8)(stat & ~ITU_TSR_IMFA);	/* reset status */

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
    sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */

    *ITU_TSNC  &= (UINT8) ~ITU_TSNC_SYNC0;
    *ITU_TMDR  &= (UINT8) ~ITU_TMDR_PWM0_PWM;
    *ITU_TCNT0	= (UINT16) 0;
    *ITU_GRA0	= (UINT16) 0xffff;
    *ITU_GRB0	= (UINT16) 0xffff;
    *ITU_TCR0	= (UINT8)(ITU_TCR_CCLR_GRA | ITU_TCR_TPSC_CLK8);
    *ITU_TIOR0	= (UINT8) ITU_TIOR_RESERVED;
    *ITU_TIER0	= (UINT8) ITU_TIER_RESERVED;

    if (intConnect (IV_ITU0_IMIA0, sysClkInt, NULL) != OK)
	return (ERROR);

    *INTC_IPRC = (UINT16)((*INTC_IPRC & 0xff0f) | ((INT_LVL_SYSCLK & 0xf) <<4));

    sysClkConnected = TRUE;
    sysClkRoutine   = routine;
    sysClkArg       = arg;

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
	*ITU_TIER0 = (UINT8)  ITU_TIER_RESERVED;	/* disable interrupt */
	*ITU_TSTR &= (UINT8) ~ITU_TSTR_STR0_START;	/* stop counter */

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
    if (!sysClkRunning)
	{
	*ITU_TIER0 = (UINT8)  ITU_TIER_RESERVED;	/* disable interrupt */
	*ITU_TSTR &= (UINT8) ~ITU_TSTR_STR0_START;	/* stop counter */
	*ITU_TCNT0 = (UINT16) 0;			/* clear counter */

	/* load compare register with the number of micro seconds */

	*ITU_GRA0  = (UINT16)(ITU_TICK_FREQ / sysClkTicksPerSecond - 1);

	*ITU_TIER0 = (UINT8)(ITU_TIER_IMIEA | ITU_TIER_RESERVED); /*enable int*/
	*ITU_TSTR |= (UINT8) ITU_TSTR_STR0_START;	/* start count */

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
    UINT8 stat;

    if ((stat = *ITU_TSR1) & ITU_TSR_IMFA)
	{
	*ITU_TSR1 = (UINT8)(stat & ~ITU_TSR_IMFA);	/* reset status */

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
    *ITU_TSNC  &= (UINT8) ~ITU_TSNC_SYNC1;
    *ITU_TMDR  &= (UINT8) ~ITU_TMDR_PWM1_PWM;
    *ITU_TCNT1	= (UINT16) 0;
    *ITU_GRA1	= (UINT16) 0xffff;
    *ITU_GRB1	= (UINT16) 0xffff;
    *ITU_TCR1	= (UINT8)(ITU_TCR_CCLR_GRA | ITU_TCR_TPSC_CLK8);
    *ITU_TIOR1	= (UINT8) ITU_TIOR_RESERVED;
    *ITU_TIER1	= (UINT8) ITU_TIER_RESERVED;

    if (intConnect (IV_ITU1_IMIA1, sysAuxClkInt, NULL) != OK)
	return (ERROR);

    *INTC_IPRC = (UINT16)((*INTC_IPRC & 0xfff0) | (INT_LVL_AUXCLK & 0xf));

    auxClkConnected	= TRUE;
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
	*ITU_TIER1 = (UINT8)  ITU_TIER_RESERVED;	/* disable interrupt */
	*ITU_TSTR &= (UINT8) ~ITU_TSTR_STR1_START;	/* stop count */

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
    if (!sysAuxClkRunning)
	{
	*ITU_TIER1 = (UINT8)  ITU_TIER_RESERVED;	/* disable interrupt */
	*ITU_TSTR &= (UINT8) ~ITU_TSTR_STR1_START;	/* stop  count */
	*ITU_TCNT1 = (UINT16) 0;			/* clear count */

	/* load compare register with the number of micro seconds */

	*ITU_GRA1  = (UINT16)(ITU_TICK_FREQ / auxClkTicksPerSecond - 1);

	*ITU_TIER1 = (UINT8)(ITU_TIER_IMIEA | ITU_TIER_RESERVED); /*enable int*/
	*ITU_TSTR |= (UINT8) ITU_TSTR_STR1_START;	/* start count */

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
    int ticksPerSecond		/* number of clock interrupts per second */
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

#ifdef	INCLUDE_TIMESTAMP
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
    volatile UINT8 stat;

    if ((stat = *ITU_TSR2) & ITU_TSR_IMFA)
	{
	*ITU_TSR2 = (UINT8)(stat & ~ITU_TSR_IMFA);	/* reset status */

	if (sysTimestampRoutine != NULL)		/* call user routine */
	    (*sysTimestampRoutine) (sysTimestampArg);
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
    sysTimestampRoutine = routine;
    sysTimestampArg     = arg;
 
    return (OK);
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
* RETURNS: OK, or ERROR if the timestamp timer cannot be enabled.
*/
 
STATUS sysTimestampEnable (void)
    {
    if (sysTimestampRunning)
	{
	*ITU_TCNT2 = (UINT16)0x0000; 		/* reset count */
	return (OK);
	}

    *ITU_TIER2  = (UINT8) ITU_TIER_RESERVED;	/* disable interrupt */
    *ITU_TSTR  &= (UINT8)~ITU_TSTR_STR2_START;	/* stop counter */
    *ITU_TCNT2  = (UINT16)0x0000;		/* reset count */

    *ITU_TSNC  &= (UINT8)~ITU_TSNC_SYNC2;
    *ITU_TMDR  &= (UINT8)~ITU_TMDR_PWM2_PWM;	/* set to up-count mode */
    *ITU_TCR2	= (UINT8)(ITU_TCR_CCLR_GRA | ITU_TCR_TPSC_CLK8);
    *ITU_TIOR2	= (UINT8) ITU_TIOR_RESERVED;	/* set GRA/GRB to compare match
						 * mode, also disable outputs */
    if (intConnect (IV_ITU2_IMIA2, sysTimestampInt, NULL) != OK)
	return (ERROR);				/* set interrupt handler */

    *INTC_IPRD = (UINT16)((*INTC_IPRD & 0x0fff) |((TIMESTAMP_LEVEL & 0xf)<<12));
						/* set interrupt level */
    *ITU_GRA2	= (UINT16)sysTimestampPeriod ();/* load compare match count */
    *ITU_GRB2	= (UINT16)0xffff;		/* reset to default */

    *ITU_TIER2	= (UINT8)(ITU_TIER_IMIEA | ITU_TIER_RESERVED);
						/* enable interrupt */
    *ITU_TSTR  |= (UINT8) ITU_TSTR_STR2_START;	/* start counter */

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
	*ITU_TIER2 = (UINT8) ITU_TIER_RESERVED;		/* disable interrupt */
	*ITU_TSTR &= (UINT8)~ITU_TSTR_STR2_START;	/* stop counter */

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
    return (UINT32)(0xffff);		/* highest period -> freerunning */
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
    return (UINT32)(ITU_TICK_FREQ);
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
    return (UINT32)(*ITU_TCNT2 & 0xffff);
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
    return (UINT32)(*ITU_TCNT2 & 0xffff);
    }

#endif	/* INCLUDE_TIMESTAMP */
