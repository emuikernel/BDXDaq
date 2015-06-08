/* loApicTimer.c - Intel Pentium[234] local APIC timer library */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01f,30nov01,hdn  doc update for 5.5
01e,24oct01,hdn  merged with VxAE 1.1
01d,12sep01,hdn  renamed TIMER_INT_VEC to INT_NUM_LOAPIC_TIMER
		 renamed TIMER_CLOCK_HZ to APIC_TIMER_CLOCK_HZ
01c,05sep01,hdn  shutoff the warning by changing NULL to 0
01b,21may98,hdn  re-written
01a,13jun97,sub  written
*/

/*
DESCRIPTION
This library contains routines for the timer in the Intel local APIC/xAPIC 
(Advanced Programmable Interrupt Controller) in P6 (PentiumPro, II, III)
and P7 (Pentium4) family processor.
The local APIC contains a 32-bit programmable timer for use by the local
processor.  This timer is configured through the timer register in the local
vector table.  The time base is derived from the processor's bus clock, 
divided by a value specified in the divide configuration register. 
After reset, the timer is initialized to zero.  The timer supports one-shot
and periodic modes.  The timer can be configured to interrupt the local
processor with an arbitrary vector.
This library gets the system clock from the local APIC timer and auxClock
from either RTC or PIT channel 0 (define PIT0_FOR_AUX in the BSP).
The macro APIC_TIMER_CLOCK_HZ must also be defined to indicate the clock 
frequency of the local APIC Timer.
The parameters SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.

This driver uses processor's on-chip TSC (Time Stamp Counter) for the
time stamp driver.  The TSC is a 64-bit time-stamp counter that is
incremented every processor clock cycle.  The counter is incremented even
when the processor is halted by the HLT instruction or the external STPCLK#
pin.  The time-stamp counter is set to 0 following a hardware reset of the
processor.  The RDTSC instruction reads the time stamp counter and is
guaranteed to return a monotonically increasing unique value whenever
executed, except for 64-bit counter wraparound.  Intel guarantees,
architecturally, that the time-stamp counter frequency and configuration will
be such that it will not wraparound within 10 years after being reset to 0.
The period for counter wrap is several thousands of years in the P6 (PentiumPro,
II, III) and P7 (Pentium4) family processors.
*/

/* includes */

#include "vxWorks.h"
#include "sysLib.h"
#include "intLib.h"
#include "taskLib.h"
#include "arch/i86/pentiumLib.h"
#include "drv/intrCtl/loApic.h"
#include "drv/timer/mc146818.h"


/* locals */

LOCAL FUNCPTR sysClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysClkArg		= 0;	/* its argument */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkConnected	= FALSE;
LOCAL int sysClkTicksPerSecond	= 60;

LOCAL FUNCPTR sysAuxClkRoutine	= NULL;
LOCAL int sysAuxClkArg		= 0;
LOCAL int sysAuxClkRunning	= FALSE;

#ifdef	PIT0_FOR_AUX
LOCAL int sysAuxClkTicksPerSecond = 60;
#else
LOCAL int sysAuxClkTicksPerSecond = 64;
LOCAL CLK_RATE auxTable[] =
    {
    {   2, 0x0f}, 
    {   4, 0x0e}, 
    {   8, 0x0d}, 
    {  16, 0x0c}, 
    {  32, 0x0b}, 
    {  64, 0x0a}, 
    { 128, 0x09}, 
    { 256, 0x08}, 
    { 512, 0x07}, 
    {1024, 0x06}, 
    {2048, 0x05}, 
    {4096, 0x04}, 
    {8192, 0x03} 
    };
#endif	/* PIT0_FOR_AUX */

#ifdef	INCLUDE_TIMESTAMP_TSC 
LOCAL BOOL	sysTimestampRunning	= FALSE; /* running flag */
LOCAL int	sysTimestampPeriodValue	= 0;	 /* Max counter value */
LOCAL FUNCPTR	sysTimestampRoutine	= NULL;	 /* user rollover routine */
LOCAL int   	sysTimestampTickCount	= 0;	 /* system ticks counter */
LOCAL UINT32	sysTimestampFreqValue	= PENTIUMPRO_TSC_FREQ; /* TSC clock */
LOCAL void	sysTimestampFreqGet (void);
#endif	/* INCLUDE_TIMESTAMP_TSC */


/*******************************************************************************
*
* sysClkInt - interrupt level processing for system clock
*
* This routine handles the system clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysClkConnect().
*/

void sysClkInt (void)
    {

    /* reset TSC */

#ifdef	INCLUDE_TIMESTAMP_TSC 
    if (sysTimestampRoutine != NULL)
	(* sysTimestampRoutine) ();
#endif	/* INCLUDE_TIMESTAMP_TSC */

    /* acknowledge interrupt */

    if (sysClkRoutine != NULL)
	(* sysClkRoutine) (sysClkArg);

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
#ifdef	_WRS_VXWORKS_5_X
    if (sysClkConnected == FALSE)
	sysHwInit2 ();	/* VxAE does this in the prjConfig */
#endif	/* _WRS_VXWORKS_5_X */

    sysClkRoutine   = routine;
    sysClkArg	    = arg;
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
    int oldLevel;

    if (sysClkRunning)
	{
        oldLevel = intLock ();	/* LOCK INTERRUPT */

	/* mask interrupt and reset the Initial Counter */

	*(int *)(loApicBase + LOAPIC_TIMER) |= LOAPIC_MASK;
	*(int *)(loApicBase + LOAPIC_TIMER_ICR) = 0;

        intUnlock (oldLevel);	/* UNLOCK INTERRUPT */
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
    int oldLevel;
    int count;

    if (!sysClkRunning)
	{

	count = ((APIC_TIMER_CLOCK_HZ) / sysClkTicksPerSecond) - 1;

        oldLevel = intLock ();	/* LOCK INTERRUPT */

	/* set a divide value in the Configuration Register */

	*(int *)(loApicBase + LOAPIC_TIMER_CONFIG) = 
	(*(int *)(loApicBase + LOAPIC_TIMER_CONFIG) & ~0xf) | 
				          LOAPIC_TIMER_DIVBY_1;

	/* set a initial count in the Initial Count Register */

	*(int *)(loApicBase + LOAPIC_TIMER_ICR) = count ;

	/* set the vector in the local vector table (Timer) */

	*(int *)(loApicBase + LOAPIC_TIMER) = 
	(*(int *)(loApicBase + LOAPIC_TIMER) & 0xfffcff00) |
			 INT_NUM_LOAPIC_TIMER | LOAPIC_TIMER_PERIODIC;

#ifdef	INCLUDE_TIMESTAMP_TSC 
	sysTimestampRoutine = (FUNCPTR)pentiumTscReset;
#endif	/* INCLUDE_TIMESTAMP_TSC */

        intUnlock (oldLevel);	/* UNLOCK INTERRUPT */	

	sysClkRunning = TRUE;

#ifdef	INCLUDE_TIMESTAMP_TSC 
	if (sysTimestampFreqValue == 0)			/* get TSC freq */
	    {
	    FUNCPTR oldFunc = sysTimestampRoutine;
	    sysTimestampRoutine = (FUNCPTR)sysTimestampFreqGet;
	    taskDelay (sysClkTicksPerSecond + 5);	/* wait 1 sec */
	    sysTimestampRoutine = oldFunc;
	    }
#endif	/* INCLUDE_TIMESTAMP_TSC */
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


#ifdef	PIT0_FOR_AUX

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

    /* call auxiliary clock service routine */

    if (sysAuxClkRoutine != NULL)
	(*sysAuxClkRoutine) (sysAuxClkArg);
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
    FUNCPTR routine,    /* routine called at each aux clock interrupt    */
    int arg             /* argument to auxiliary clock interrupt routine */
    )
    {
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
	sysOutByte (PIT_CMD (PIT_BASE_ADR), 0x38);
	sysOutByte (PIT_CNT0 (PIT_BASE_ADR), LSB(0));
	sysOutByte (PIT_CNT0 (PIT_BASE_ADR), MSB(0));

	sysIntDisablePIC (PIT0_INT_LVL);
	
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
    UINT tc;

    if (!sysAuxClkRunning)
	{
	tc = PIT_CLOCK / sysAuxClkTicksPerSecond;

	sysOutByte (PIT_CMD (PIT_BASE_ADR), 0x36);
	sysOutByte (PIT_CNT0 (PIT_BASE_ADR), LSB(tc));
	sysOutByte (PIT_CNT0 (PIT_BASE_ADR), MSB(tc));

	/* enable clock interrupt */

	sysIntEnablePIC (PIT0_INT_LVL);
	
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

#else	/* PIT0_FOR_AUX */

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
    int oldLevel;

    /* acknowledge the interrupt */

    oldLevel = intLock ();				/* LOCK INTERRUPT */
    sysOutByte (RTC_INDEX, MC146818_STATUS_C);
    sysInByte (RTC_DATA);
    intUnlock (oldLevel);				/* UNLOCK INTERRUPT */

    /* call auxiliary clock service routine */

    if (sysAuxClkRoutine != NULL)
	(*sysAuxClkRoutine) (sysAuxClkArg);
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

	sysOutByte (RTC_INDEX, MC146818_STATUS_B);
	sysOutByte (RTC_DATA, MC146818_24);

	sysIntDisablePIC (RTC_INT_LVL);

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
    int ix;
    char statusA;
    int oldLevel;

    if (!sysAuxClkRunning)
        {

	/* set an interrupt rate */

	for (ix = 0; ix < NELEMENTS (auxTable); ix++)
	    {
	    if (auxTable [ix].rate == sysAuxClkTicksPerSecond)
		{
	        sysOutByte (RTC_INDEX, MC146818_STATUS_A);
	        statusA = sysInByte (RTC_DATA) & ~MC146818_RS_BITS;
	        sysOutByte (RTC_INDEX, MC146818_STATUS_A);
	        sysOutByte (RTC_DATA, statusA | auxTable [ix].bits);
		break;
		}
	    }

	/* start the timer */

        oldLevel = intLock ();				/* LOCK INTERRUPT */

        sysOutByte (RTC_INDEX, MC146818_STATUS_C);	/* clear Int Flags */
        sysInByte (RTC_DATA);

	sysOutByte (RTC_INDEX, MC146818_STATUS_B);	/* set PIE */
	sysOutByte (RTC_DATA, MC146818_PIE | MC146818_24);

	sysIntEnablePIC (RTC_INT_LVL);

	sysAuxClkRunning = TRUE;

        intUnlock (oldLevel);				/* UNLOCK INTERRUPT */
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
    int		ix;	/* hold temporary variable */
    BOOL	match;	/* hold the match status */

    match = FALSE; 	/* initialize to false */

    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
        return (ERROR);

    for (ix = 0; ix < NELEMENTS (auxTable); ix++)
	{
	if (auxTable [ix].rate == ticksPerSecond)
	    {
	    sysAuxClkTicksPerSecond = ticksPerSecond;
	    match = TRUE;
	    break;
	    }
	}

    if (!match)		/* ticksPerSecond not matching the values in table */
       return (ERROR);

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }
#endif	/* PIT0_FOR_AUX */


#ifdef	INCLUDE_TIMESTAMP_TSC

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
    return (ERROR);
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
	return (OK);
    
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
        sysTimestampRunning = FALSE;
	}

    return (ERROR);
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
    sysTimestampPeriodValue = sysTimestampFreq () / sysClkTicksPerSecond;

    return (sysTimestampPeriodValue);
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
    return (sysTimestampFreqValue);
    }
 
LOCAL void sysTimestampFreqGet (void)
    {
    if (sysTimestampTickCount == sysClkTicksPerSecond)
	{
	sysTimestampFreqValue = pentiumTscGet32 ();
	sysTimestampTickCount = 0;
	}
    if (sysTimestampTickCount == 0)
	pentiumTscReset ();
    sysTimestampTickCount++;
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
    return (pentiumTscGet32 ());
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
    /* 
     * pentiumTscGet32() has just one instruction "rdtsc", so locking
     * the interrupt is not necessary. 
     */

    return (pentiumTscGet32 ());
    }

#endif	/* INCLUDE_TIMESTAMP_TSC */

