/* sh7604Timer.c - SH7604 on-chip Timer library */

/* Copyright 1994-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01q,09dec97,hk   changed INT_VEC_CLOCK to IV_WDT_ITI, INT_VEC_AUX_CLOCK to
                 IV_FRT_OCI. added INTC control to sys{Aux}ClkConnect().
                 reviewed timestamp driver code.
01p,12jul97,hk   changed inclusion of drv/multi/sh7604.h to arch/sh/sh7600.h.
01o,15may97,st   modified sysTimestampConnect(), sysTimestampEnable(). and
                 added default TIMESTAMP_LEVEL.
01n,04may97,hk   merged in timestamp driver. swapped sys/aux clock sources.
01m,06mar97,st   added support for timestamp function for windview
01l,14jun95,hk   deleted SH7604_EXT_FRT_CLOCK conditional. code review.
01k,06jun95,hk   renamed timer select macros.
01j,28feb95,hk   changed sysClkConnect() and sysAuxConnect() to conform the defs
		 in ivSh.h-01e rule. copyright year 1995.
01i,10nov94,sa   delete SH7604_SYSCLK_INT_DISABLE,SH7604_SYSAUXCLK_INT_DISABLE.
		 added '#ifdef SH7604_FRT_SYSCLK_INCLUDE'
01h,21oct94,hk   modified int ack sequence in sysClkInt, sysAuxClkInt.
		 fixed FRT_FTCSR clear code in sysClkEnable.
01g,21oct94,hk   fixed TICK_FREQ for SH_EMULATOR (6144000/32 = 192000).
		 substituted macros for some magic numbers in SH_EMULATOR code.
01f,19oct94,sa   added TICK_AUX_FREQ.
                 fixed sysAuxClkXXX().
01e,13oct94,sa   modified sysClkInt(),
                 TICK_FREQ for SH_EMULATOR
                 Output Compare A.
01d,07oct94,sa   modified sysClkInt(), syAuxClkInt().
01c,05oct94,sa   added sysHwInit2() in sysClkConnect().
01b,26sep94,sa   modified.
01a,18jul94,hk   derived from 01c of pccTimer.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
SH7604 on-chip timer units (WDT and FRT) with a board-independant interface.
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
#include "arch/sh/sh7600.h"

#if (SYS_CPU_FREQ==28700000)
#define	WDT_WTCSR_CLKSEL	WDT_WTCSR_CLK8192
#define WDT_TICK_FREQ		(SYS_CPU_FREQ / 8192)	/*   3503.418 */
#define FRT_TICK_FREQ		(SYS_CPU_FREQ / 32)	/* 896875     */
#elif (SYS_CPU_FREQ==6144000)
#define	WDT_WTCSR_CLKSEL	WDT_WTCSR_CLK512
#define WDT_TICK_FREQ		(SYS_CPU_FREQ / 512)	/*  12000    */
#define FRT_TICK_FREQ		(SYS_CPU_FREQ / 32)	/* 192000    */
#else
#error SYS_CPU_FREQ is not properly defined
#endif

#undef	MONITOR_SH7604_FTOA	/* probe SH7604 FTOA pin by oscilloscope */

/* Locals */ 

#ifdef	SYSCLK_USE_SH7604_WDT
UINT16		sysClkPreload;
LOCAL int	sysClkTicksPerSecond	= 60; 
LOCAL BOOL	sysClkRunning		= FALSE; 
LOCAL FUNCPTR	sysClkRoutine		= NULL; 
LOCAL int	sysClkArg		= NULL; 
LOCAL BOOL	sysClkConnected		= FALSE; 
#endif

#ifdef	AUXCLK_USE_SH7604_FRT
LOCAL int	sysAuxClkTicksPerSecond	= 60; 
LOCAL BOOL	sysAuxClkRunning	= FALSE; 
LOCAL FUNCPTR	sysAuxClkRoutine	= NULL; 
LOCAL int	sysAuxClkArg		= NULL;
LOCAL BOOL	sysAuxClkConnected	= FALSE; 
#endif

#if defined(INCLUDE_TIMESTAMP) && defined(TSTAMP_USE_SH7604_FRT)
LOCAL BOOL	sysTimestampRunning	= FALSE;  /* running flag */
LOCAL FUNCPTR	sysTimestampRoutine	= NULL;   /* user rollover routine */
LOCAL int	sysTimestampArg		= NULL;   /* arg to user routine */
#ifndef TIMESTAMP_LEVEL
#error TIMESTAMP_LEVEL is not defined
#endif /* ! TIMESTAMP_LEVEL */
#endif /* INCLUDE_TIMESTAMP && TSTAMP_USE_SH7604_FRT */

#ifdef	SYSCLK_USE_SH7604_WDT
/*******************************************************************************
*
* sysClkInt - handle system clock interrupts
*
* This routine handles system clock interrupts.
*/
 
LOCAL void sysClkInt (void)
    {
    UINT8 stat;

    if ((stat = *WDT_WTCSR_R) & WDT_WTCSR_OVF)
	{
	/* reset clock interrupt */

	*WDT_WTCSR_W = (UINT16)((stat & (~WDT_WTCSR_OVF)) | WDT_WTCSR_RESERVED);

	/* preload WTCNT register */

	*WDT_WTCNT_W = sysClkPreload;

	if (sysClkRoutine != NULL)
	    (*sysClkRoutine)(sysClkArg);
	}
    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* system clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c
* to connect usrClock() to the system clock interrupt.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
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

    /* initialize */

    *WDT_WTCSR_W = (UINT16)(WDT_WTCSR_IT | WDT_WTCSR_TME_DISABLE |
			WDT_WTCSR_CLKSEL | WDT_WTCSR_RESERVED);

    *WDT_WTCNT_W = (UINT16)(WDT_WTCNT_RESERVED | 0);

    /* connect system clock interrupt handler */

    if (intConnect (IV_WDT_ITI, sysClkInt, NULL) != OK)
	return (ERROR);

    /* set WDT interrupt level to INTC */

    *INTC_IPRA = (UINT16)((*INTC_IPRA & 0xff0f) | ((INT_LVL_SYSCLK & 0xf)<<4));

    sysClkConnected	= TRUE;
    sysClkRoutine	= routine;
    sysClkArg		= arg;
 
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
    UINT8 dummy;

    if (sysClkRunning)
	{
	/* clear overflow counter & stop counter */

	dummy = *WDT_WTCSR_R;

	*WDT_WTCSR_W = (UINT16)(WDT_WTCSR_IT | WDT_WTCSR_TME_DISABLE | 
				WDT_WTCSR_CLKSEL | WDT_WTCSR_RESERVED);

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
    UINT8 dummy;

    if (!sysClkRunning)
	{
	/* preload WTCNT register */
	*WDT_WTCNT_W = sysClkPreload;
 
	/* clear overflow counter, enable & start counter */
	dummy = *WDT_WTCSR_R;

	*WDT_WTCSR_W = (UINT16)(WDT_WTCSR_IT | WDT_WTCSR_TME_ENABLE |
				WDT_WTCSR_CLKSEL | WDT_WTCSR_RESERVED);

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
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/
 
STATUS sysClkRateSet
    (
    int ticksPerSecond		/* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
	return (ERROR);
 
    sysClkTicksPerSecond = ticksPerSecond;
    sysClkPreload
	= (UINT16)(((256 - (WDT_TICK_FREQ / sysClkTicksPerSecond)) & 0xff)
		   | WDT_WTCNT_RESERVED);

    if (sysClkRunning)
	{
	sysClkDisable ();
	sysClkEnable ();
	}
    return (OK);
    } 
#endif	/* SYSCLK_USE_SH7604_WDT */

#ifdef	AUXCLK_USE_SH7604_FRT
/*******************************************************************************
*
* sysAuxClkInt - handle auxiliary clock interrupts
*
* This routine handles auxiliary clock interrupts.
*/

LOCAL void sysAuxClkInt (void)
    {
    UINT8 stat;

    if ((stat = *FRT_FTCSR) & FRT_FTCSR_OCFA)
	{
	*FRT_FTCSR = (UINT8)FRT_FTCSR_CCLRA;	/* reset clock interrupt */

#ifdef	MONITOR_SH7604_FTOA
	if (*FRT_TOCR & FRT_TOCR_OLVLA_1)
	    *FRT_TOCR = (UINT8)(FRT_TOCR_OLVLA_0 | FRT_TOCR_RESERVED);
	else
	    *FRT_TOCR = (UINT8)(FRT_TOCR_OLVLA_1 | FRT_TOCR_RESERVED);
#endif	/* MONITOR_SH7604_FTOA */

	if (sysAuxClkRoutine != NULL)
	    (*sysAuxClkRoutine)(sysAuxClkArg);
	}
    }

/*******************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It does not enable auxiliary clock interrupts.
*
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/
 
STATUS sysAuxClkConnect
    (
    FUNCPTR routine,	/* routine called at each aux clock interrupt    */
    int arg		/* argument to auxiliary clock interrupt routine */
    )
    {
    /* initialize */

    *FRT_TIER	= (UINT8)(FRT_TIER_ICF_INT_DISABLE | 
			FRT_TIER_OCFA_INT_DISABLE | FRT_TIER_OCFB_INT_DISABLE | 
			FRT_TIER_OVF_INT_DISABLE  | FRT_TIER_RESERVED);
    *FRT_FTCSR	= (UINT8)FRT_FTCSR_CCLRA;
    *FRT_TCR	= (UINT8)FRT_TCR_CLK_DIV32;
#ifndef	MONITOR_SH7604_FTOA
    *FRT_TOCR	= (UINT8)(FRT_TOCR_OCRS_A | FRT_TOCR_RESERVED);
#else
    *FRT_TOCR = (UINT8)(FRT_TOCR_OCRS_A | FRT_TOCR_RESERVED | FRT_TOCR_OLVLA_1);
#endif	/* MONITOR_SH7604_FTOA */
    *FRT_FRCH	= (UINT8)0;
    *FRT_FRCL	= (UINT8)0;
    *FRT_OCRAH	= (UINT8)0xff;
    *FRT_OCRAL	= (UINT8)0xff;

    /* connect aux clock interrupts */

    if (intConnect (IV_FRT_OCI, sysAuxClkInt, NULL) != OK)
	return (ERROR);

    /* set FRT interrupt level to INTC */

    *INTC_IPRB = (UINT16)((*INTC_IPRB & 0xf0ff) | ((INT_LVL_AUXCLK & 0xf)<<8));

    sysAuxClkConnected	= TRUE;
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
	/* disable interrupts */

	*FRT_TIER = (UINT8)(FRT_TIER_ICF_INT_DISABLE | 
			FRT_TIER_OCFA_INT_DISABLE | FRT_TIER_OCFB_INT_DISABLE | 
			FRT_TIER_OVF_INT_DISABLE | FRT_TIER_RESERVED);

	sysAuxClkRunning = FALSE;
	}
    }

/****************************************************************************** 
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
    UINT8 tmp;

    if (!sysAuxClkRunning)
	{
	/* disable the clock interrupt. */

	*FRT_TIER = (UINT8)(FRT_TIER_ICF_INT_DISABLE | 
			FRT_TIER_OCFA_INT_DISABLE | FRT_TIER_OCFB_INT_DISABLE | 
			FRT_TIER_OVF_INT_DISABLE | FRT_TIER_RESERVED);
	/* clear */
	tmp = *FRT_FTCSR;
	*FRT_FTCSR = (UINT8)FRT_FTCSR_CCLRA;

	/* load compare register with the number of micro seconds */

#ifndef	MONITOR_SH7604_FTOA
	*FRT_TOCR = (UINT8)(FRT_TOCR_OCRS_A | FRT_TOCR_RESERVED);
#else
    *FRT_TOCR = (UINT8)(FRT_TOCR_OCRS_A | FRT_TOCR_RESERVED | FRT_TOCR_OLVLA_1);
#endif	/* MONITOR_SH7604_FTOA */
	*FRT_OCRAH = (UINT8)((FRT_TICK_FREQ / sysClkTicksPerSecond - 1) >> 8);
	*FRT_OCRAL = (UINT8)((FRT_TICK_FREQ / sysClkTicksPerSecond - 1) & 0xff);
	*FRT_FRCH  = (UINT8)0;
	*FRT_FRCL  = (UINT8)0;

	/* enable the clock interrupt. */

	*FRT_TIER  = (UINT8)(FRT_TIER_ICF_INT_DISABLE | 
			FRT_TIER_OCFA_INT_ENABLE | FRT_TIER_OCFB_INT_DISABLE | 
			FRT_TIER_OVF_INT_DISABLE | FRT_TIER_RESERVED);

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
* SEE ALSO: sysAuxClkEnable, sysAuxClkRateSet()
*/
 
int sysAuxClkRateGet (void)
    {
    return (sysAuxClkTicksPerSecond);
    }

/********************************************************************************
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock.
* It does not enable auxiliary clock interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysAuxClkEnable, sysAuxClkRateGet()
*/
 
STATUS sysAuxClkRateSet
    (
    int ticksPerSecond		/* number of clock interrupts per second */
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
#endif	/* AUXCLK_USE_SH7604_FRT */

#if defined(INCLUDE_TIMESTAMP) && defined(TSTAMP_USE_SH7604_FRT)
/********************************************************************************
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
    if (*FRT_FTCSR & FRT_FTCSR_OCFA)
	{
	*FRT_FTCSR = (UINT8)FRT_FTCSR_CCLRA;	/* reset clock interrupt */

	if (sysTimestampRoutine != NULL)
	    (*sysTimestampRoutine)(sysTimestampArg);
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
    sysTimestampRoutine	= routine;
    sysTimestampArg     = arg;

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
    volatile UINT8 tmp8;

    if (sysTimestampRunning)
	{
	*FRT_FRCH = (UINT8)0x00;		/* set upper byte in TEMP */
	*FRT_FRCL = (UINT8)0x00;		/* clear 16bit up-counter */
	return (OK);
	}

    *FRT_TIER	= (UINT8)FRT_TIER_RESERVED;	/* disable interrupts */
    *FRT_TCR	= (UINT8)FRT_TCR_CLK_DIV32;	/* select clock source */
    *FRT_TOCR	= (UINT8)(FRT_TOCR_OCRS_A | FRT_TOCR_RESERVED);
						/* select compare match reg A */
    *FRT_OCRAH	= (UINT8)((sysTimestampPeriod() & 0xff00) >> 8);
    *FRT_OCRAL	= (UINT8) (sysTimestampPeriod() & 0x00ff);
						/* load compare match reg A */
    if (intConnect (IV_FRT_OCI, sysTimestampInt, NULL) != OK)
	return (ERROR);				/* set interrupt handler */

    *INTC_IPRB = (UINT16)((*INTC_IPRB & 0xf0ff) | ((TIMESTAMP_LEVEL & 0xf)<<8));
						/* set interrupt level */
    *FRT_FRCH	= (UINT8)0x00;			/* set upper byte in TEMP */
    *FRT_FRCL	= (UINT8)0x00;			/* clear 16bit up-counter */
    tmp8 = *FRT_FTCSR;
    *FRT_FTCSR	= (UINT8)FRT_FTCSR_CCLRA;	/* clear misc. status */
    *FRT_TIER	= (UINT8)(FRT_TIER_OCFA_INT_ENABLE | FRT_TIER_RESERVED);
						/* enable interrupt */
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
	*FRT_TIER = (UINT8)FRT_TIER_RESERVED;	/* disable interrupts */

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
    return (UINT32)(0xffff);            /* highest period -> freerunning */
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
    return (UINT32)(FRT_TICK_FREQ);
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
    return (UINT32)((*FRT_FRCH << 8) | *FRT_FRCL);
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
    return (UINT32)((*FRT_FRCH << 8) | *FRT_FRCL);
    }

#endif	/* INCLUDE_TIMESTAMP && TSTAMP_USE_SH7604_FRT */
