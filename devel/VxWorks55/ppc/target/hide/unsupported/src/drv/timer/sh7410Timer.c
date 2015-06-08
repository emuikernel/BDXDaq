/* sh7410Timer.c - SH7410 on-chip Timer library */

/* Copyright 1994-1998 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,19oct98,jmb   fixed bug in auxclock and timestamp clock rate selects.
01b,13oct98,jmb   fixed clock divisor problem, allow alternates, and external
                  clock, fix intlevel for auxclock and timestamp.
01a,19may98,jmc   derived from sh7604Timer.c 01q, with revisions by jb&es.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
SH7410 on-chip timer units (FRTs) with a board-independant interface.
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
#include "arch/sh/sh7410.h"

#if (SYS_CPU_FREQ==15000000)
#define FRT_SYS_TICK_FREQ	(15000000 / SYS_CLK_RES)
#define FRT_AUX_TICK_FREQ	(15000000 / AUX_CLK_RES)	
#define FRT_TIMESTAMP_TICK_FREQ	(15000000 / TIMESTAMP_CLK_RES)	
#else /* (SYS_CPU_FREQ==15000000) */
#error SYS_CPU_FREQ is not properly defined
#endif /* (SYS_CPU_FREQ==15000000) */

/* Locals */ 

UINT16		sysClkPreload;
LOCAL int	sysClkTicksPerSecond	= 60; 
LOCAL BOOL	sysClkRunning		= FALSE; 
LOCAL FUNCPTR	sysClkRoutine		= NULL; 
LOCAL int	sysClkArg		= NULL; 
LOCAL BOOL	sysClkConnected		= FALSE; 

LOCAL int	sysAuxClkTicksPerSecond	= 60; 
LOCAL BOOL	sysAuxClkRunning	= FALSE; 
LOCAL FUNCPTR	sysAuxClkRoutine	= NULL; 
LOCAL int	sysAuxClkArg		= NULL;
LOCAL BOOL	sysAuxClkConnected	= FALSE; 

#if defined(INCLUDE_TIMESTAMP)
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

    if ((stat = *FRT_FTCSR0) & FRT_FTCSR_OCFA)
      {
	*FRT_FTCSR0 = (UINT8)FRT_FTCSR_CCLRA;	/* reset clock interrupt */

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
    FUNCPTR routine,	/* routine called at each aux clock interrupt    */
    int arg		/* argument to auxiliary clock interrupt routine */
    )
    {
    sysHwInit2 ();	/* XXX for now -- needs to be in usrConfig.c */

    /* initialize */

    *FRT_FTCR0	= (UINT8) 0; 	/* default state, interrupts disabled */
    *FRT_FTCSR0	= (UINT8) FRT_FTCSR_CCLRA;

    /* Select clock divisor or external clock */

#if (SYS_CLK_RES==CLK_RES_HIGH)
    *FRT_FTCR0	= (UINT8) FRT_FTCR_CLK_DIV8; 
#elif (SYS_CLK_RES==CLK_RES_MED)
    *FRT_FTCR0	= (UINT8) FRT_FTCR_CLK_DIV32; 
#elif (SYS_CLK_RES==CLK_RES_LOW)
    *FRT_FTCR0	= (UINT8) FRT_FTCR_CLK_DIV128; 
#elif (SYS_CLK_RES==CLK_EXT)
    *FRT_FTCR0	= (UINT8) FRT_FTCR_CLK_EXT; 
#endif
    *FRT_FRC0H	= (UINT8)0;
    *FRT_FRC0L	= (UINT8)0;
    *FRT_FOCRA0H= (UINT8)0xff;
    *FRT_FOCRA0L= (UINT8)0xff;

    /* connect sys clock interrupts */

    if (intConnect (IV_FRT_OCIA0, sysClkInt, NULL) != OK)
	return (ERROR);

    /* set FRT interrupt level to INTC */

    *INTC_IPRC = (UINT16)((*INTC_IPRC & 0x0fff) | ((INT_LVL_SYSCLK & 0xf)<<12));

    sysClkConnected	= TRUE;
    sysClkRoutine	= routine;
    sysClkArg	= arg;

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
	/* disable interrupts */

	*FRT_FTCR0 &= ~FRT_INT_MASK;

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
    UINT8 tmp;

    if (!sysClkRunning)
	{
	/* disable the clock interrupt. */

	*FRT_FTCR0 &= ~FRT_INT_MASK;

	/* clear */
	tmp = *FRT_FTCSR0;
	*FRT_FTCSR0 = (UINT8)FRT_FTCSR_CCLRA;

	/* load compare register with the number of micro seconds */

	*FRT_FOCRA0H = 
            (UINT8)((FRT_SYS_TICK_FREQ / sysClkTicksPerSecond - 1) >> 8);
	*FRT_FOCRA0L = 
            (UINT8)((FRT_SYS_TICK_FREQ / sysClkTicksPerSecond - 1) & 0xff);
	*FRT_FRC0H  = (UINT8)0;
	*FRT_FRC0L  = (UINT8)0;

	/* enable the clock interrupt. */

	*FRT_FTCR0  |= (UINT8) FRT_FTCR_OCFA_INT_ENABLE;

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
*
* This routine handles auxiliary clock interrupts.
*/

LOCAL void sysAuxClkInt (void)
    {
    UINT8 stat;

    if ((stat = *FRT_FTCSR1) & FRT_FTCSR_OCFA)
	{
	*FRT_FTCSR1 = (UINT8)FRT_FTCSR_CCLRA;	/* reset clock interrupt */

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

    *FRT_FTCR1	= (UINT8) 0;	/* default state, interrupts disabled */

    *FRT_FTCSR1	= (UINT8)FRT_FTCSR_CCLRA;

    /* Select clock divisor or external clock */

#if (AUX_CLK_RES==CLK_RES_HIGH)
    *FRT_FTCR1	= (UINT8) FRT_FTCR_CLK_DIV8; 
#elif (AUX_CLK_RES==CLK_RES_MED)
    *FRT_FTCR1	= (UINT8) FRT_FTCR_CLK_DIV32; 
#elif (AUX_CLK_RES==CLK_RES_LOW)
    *FRT_FTCR1	= (UINT8) FRT_FTCR_CLK_DIV128; 
#elif (AUX_CLK_RES==CLK_EXT)
    *FRT_FTCR1	= (UINT8) FRT_FTCR_CLK_EXT; 
#endif

    *FRT_FRC1H	= (UINT8)0;
    *FRT_FRC1L	= (UINT8)0;
    *FRT_FOCRA1H= (UINT8)0xff;
    *FRT_FOCRA1L= (UINT8)0xff;

    /* connect aux clock interrupts */

    if (intConnect (IV_FRT_OCIA1, sysAuxClkInt, NULL) != OK)
	return (ERROR);

    /* set FRT interrupt level to INTC */

    *INTC_IPRC = (UINT16)((*INTC_IPRC & 0xf0ff) | ((INT_LVL_AUXCLK & 0xf)<<8));

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

	*FRT_FTCR1 &= (UINT8) ~FRT_INT_MASK;  

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

	*FRT_FTCR1 &= (UINT8) ~FRT_INT_MASK;

	/* clear */
	tmp = *FRT_FTCSR1;
	*FRT_FTCSR1 = (UINT8)FRT_FTCSR_CCLRA;

	/* load compare register with the number of micro seconds */

	*FRT_FOCRA1H = 
            (UINT8)((FRT_AUX_TICK_FREQ / sysAuxClkTicksPerSecond - 1) >> 8);
	*FRT_FOCRA1L = 
            (UINT8)((FRT_AUX_TICK_FREQ / sysAuxClkTicksPerSecond - 1) & 0xff);
	*FRT_FRC1H  = (UINT8)0;
	*FRT_FRC1L  = (UINT8)0;

	/* enable the clock interrupt. */

	*FRT_FTCR1 |= (UINT8) FRT_FTCR_OCFA_INT_ENABLE;

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

#if defined(INCLUDE_TIMESTAMP)
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
    UINT8 stat;

    if ((stat = *FRT_FTCSR2) & FRT_FTCSR_OCFA)
	{
	*FRT_FTCSR2 = (UINT8)FRT_FTCSR_CCLRA;	/* reset clock interrupt */

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
	*FRT_FRC2H = (UINT8)0x00;		/* set upper byte in TEMP */
	*FRT_FRC2L = (UINT8)0x00;		/* clear 16bit up-counter */
	return (OK);
	}

    /* Initialize here, since the connect routine doesn't do it */

    *FRT_FTCR2 = (UINT8) 0;	/* disable interrupts, take defaults */

    /* Select clock source */

#if (TIMESTAMP_CLK_RES==CLK_RES_HIGH)
    *FRT_FTCR2	= (UINT8) FRT_FTCR_CLK_DIV8; 
#elif (TIMESTAMP_CLK_RES==CLK_RES_MED)
    *FRT_FTCR2	= (UINT8) FRT_FTCR_CLK_DIV32; 
#elif (TIMESTAMP_CLK_RES==CLK_RES_LOW)
    *FRT_FTCR2	= (UINT8) FRT_FTCR_CLK_DIV128; 
#elif (TIMESTAMP_CLK_RES==CLK_EXT)
    *FRT_FTCR2	= (UINT8) FRT_FTCR_CLK_EXT; 
#endif

    *FRT_FTCSR2	= (UINT8)FRT_FTCSR_CCLRA;
						/* select compare match reg A */
    *FRT_FOCRA2H = (UINT8)((sysTimestampPeriod() & 0xff00) >> 8);
    *FRT_FOCRA2L = (UINT8) (sysTimestampPeriod() & 0x00ff);
						/* load compare match reg A */
    if (intConnect (IV_FRT_OCIA2, sysTimestampInt, NULL) != OK)
	return (ERROR);				/* set interrupt handler */

    *INTC_IPRC = (UINT16)((*INTC_IPRC & 0xff0f) | ((TIMESTAMP_LEVEL & 0xf)<<4));
						/* set interrupt level */
    *FRT_FRC2H	= (UINT8)0x00;			/* set upper byte in TEMP */
    *FRT_FRC2L	= (UINT8)0x00;			/* clear 16bit up-counter */
    tmp8 = *FRT_FTCSR2;
    *FRT_FTCSR2	= (UINT8)FRT_FTCSR_CCLRA;	/* clear misc. status */

    /* enable the clock interrupt. */

    *FRT_FTCR2  |= (UINT8) FRT_FTCR_OCFA_INT_ENABLE;

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
	/* disable interrupts */

	*FRT_FTCR2 &= (UINT8) ~FRT_INT_MASK;

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
    return (UINT32)(FRT_TIMESTAMP_TICK_FREQ);
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
    return (UINT32)((*FRT_FRC2H << 8) | *FRT_FRC2L);
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
    return (UINT32)((*FRT_FRC2H << 8) | *FRT_FRC2L);
    }

#endif	/* INCLUDE_TIMESTAMP */
