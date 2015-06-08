/* ev960jxTimer.c - i960 processor timer library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01f,21may97,db   moved intConnect from sysTimestampConnect to 
		 sysTimestampEnable.(SPR #8400)
01e,03jan97,wlf  doc: more cleanup.
01d,17jul96,tmk  fixed casting of TIMER_ROLL_OVER - made arithmetic work.
01c,23may96,wlf  doc: cleanup.
01b,05jan96,myz  removed intConnect in xxxClkEnable routine.
01a,20mar95,kvk  Created.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
ep960jx and hx with a board-independent interface.  This library handles both
the system clock and the auxiliary clock functions.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.
*/

/* Move the following into /ev960jxTimer.h when created */

/* Timer Reload Register 0 */
#define EP960_TRR0 ((unsigned long *) TRR0_ADDR)

/* Timer Count Register 0 */
#define EP960_TCR0 ((unsigned long *) TCR0_ADDR)

/* Timer Mode Register 0 */
#define EP960_TMR0 ((unsigned long *) TMR0_ADDR)

/* Timer Reload Register 1 */
#define EP960_TRR1 ((unsigned long *) TRR1_ADDR)

/* Timer Count Register 1 */
#define EP960_TCR1 ((unsigned long *) TCR1_ADDR)

/* Timer Mode Register 1 */
#define EP960_TMR1 ((unsigned long *) TMR1_ADDR)

/* IMSK Register */
#define EP960_IMSK ((unsigned long *) IMSK_ADDR)


#define CPU_SPEED_MHZ            33
#define TIMER_ROLL_OVER		 ((unsigned long) 0xffffffff)



#include "drv/timer/timerDev.h"

/* Locals */ 

LOCAL int     sysClkTicksPerSecond = 60; 
LOCAL BOOL    sysClkRunning        = FALSE; 
LOCAL FUNCPTR sysClkRoutine        = NULL;
LOCAL int     sysClkArg            = NULL;
LOCAL BOOL    sysClkConnected      = FALSE; 

LOCAL int     auxClkTicksPerSecond = 60; 
LOCAL BOOL    sysAuxClkRunning     = FALSE; 
LOCAL FUNCPTR sysAuxClkRoutine     = NULL;
LOCAL int     sysAuxClkArg         = NULL;
LOCAL BOOL    sysAuxClkConnected   = FALSE; 

/**************************************************************************
*
* sysClkInt - clock interrupt handler
*
* This routine handles the clock interrupt.  It is attached to the clock
* interrupt vector by the routine sysClkConnect().  The appropriate routine
* is called and the interrupts are acknowledged.
*/

LOCAL void sysClkInt (void)
    {
    if (sysClkRoutine != NULL)
        (*(FUNCPTR) sysClkRoutine) (sysClkArg);

    /* processor clears the IPEND bit upon entry into the ISR */
    }

/***************************************************************************
*
* sysAuxClkInt - clock interrupt handler
*
* This routine handles the clock interrupt.  It is attached to the clock
* interrupt vector by the routine sysAuxClkConnect().  The appropriate
* routine is called and the interrupts are acknowledged.
*/

LOCAL void sysAuxClkInt (void)
    {
    if (sysAuxClkRoutine != NULL)
        (*(FUNCPTR) sysAuxClkRoutine) (sysAuxClkArg);
    }

/***************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally it is called from usrRoot() in usrConfig.c to 
* connect usrClock() to the system clock interrupt.
*
* RETURN: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,    /* routine called at each system clock interrupt */
    int     arg         /* argument with which to call routine           */
    )
    {
    (void)intConnect ((void *)VECTOR_TMR0, sysClkInt, 0);

    sysClkConnected   = TRUE;
    sysClkRoutine     = routine;
    sysClkArg         = arg;

    sysHwInit2 ();

    return (OK);
    }
 
/***************************************************************************
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
	/* Mask the timer interrupt bits in the IMSK register */
	*EP960_IMSK &= 0x20ff;
	
	/* Set the Timer mode Register */
	*EP960_TMR0 = 0;
	
	/* Clear the timer interrupts in IPND register, if any */
	vxIPNDClear(0x1000);
	
        sysClkRunning    = FALSE;
        }
    }

/***************************************************************************
*
* sysClkEnable - turn on system clock interrupts
*
* This routine enables system clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysClkDisable(), sysClkRateSet()
*/

void sysClkEnable (void)
    {
    if (!sysClkRunning)
        {
	/* Set the Timer Count register to 0 */
	*EP960_TCR0 = (CPU_SPEED_MHZ * 1000000 / sysClkTicksPerSecond);
	
	/* Load the Timer Reload register */
	*EP960_TRR0 = (CPU_SPEED_MHZ * 1000000 / sysClkTicksPerSecond);
	
	/* Set the Timer mode Register */
	*EP960_TMR0 = 7; /* enable timer, auto reload */
	
	/* Enable the timer interrupt bits in the IMSK register */
	*EP960_IMSK |= 0x1000;
	
        sysClkRunning    = TRUE;
        }
    }
    
/***************************************************************************
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
    
/***************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.  It is called by 
* usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond      /* number of clock interrupts per second */
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

/***************************************************************************
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
    FUNCPTR routine,    /* routine called at each aux. clock interrupt */
    int     arg         /* argument with which to call routine         */
    )
    {
#ifdef INCLUDE_TIMESTAMP
    return (ERROR);
#endif

    (void) intConnect ((void *)VECTOR_TMR1, sysAuxClkInt, 0);

    sysAuxClkConnected = TRUE;
    sysAuxClkRoutine   = routine;
    sysAuxClkArg       = arg;
    
    return (OK);
    }

/***************************************************************************
*
* sysAuxClkDisconnect - clear the auxiliary clock routine
*
* This routine disables the auxiliary clock interrupt, stops the timer,
* and disconnects the routine currently connected to the auxiliary clock
* interrupt.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkEnable()
*/

void sysAuxClkDisconnect (void)
    {
    /* disable the auxiliary clock interrupt */
    sysAuxClkDisable ();

    }

/***************************************************************************
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
	/* Mask the timer interrupt bits in the IMSK register */
	*EP960_IMSK &= 0x10ff;
	
	/* Set the Timer mode Register */
	*EP960_TMR1 = 0;

	/* Clear the timer interrupts in IPND register, if any */
	vxIPNDClear(0x2000);
	
	
        sysAuxClkRunning = FALSE;
        }
    }

/***************************************************************************
*
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkDisable()
*/

void sysAuxClkEnable (void)
    {
    if (!sysAuxClkRunning)
        {
	/* Set the Timer Count register to 0 */
	*EP960_TCR1 = (CPU_SPEED_MHZ * 1000000 / auxClkTicksPerSecond);
	
	/* Load the Timer Reload register */
	*EP960_TRR1 = (CPU_SPEED_MHZ * 1000000 / auxClkTicksPerSecond);
	
	/* Set the Timer mode Register */
	*EP960_TMR1 = 7;
	
	/* Enable the timer interrupt bits in the IMSK register */
	*EP960_IMSK |= 0x2000;

        sysAuxClkRunning  = TRUE;
        }
    }

/***************************************************************************
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

/***************************************************************************
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
    int ticksPerSecond      /* number of clock interrupts per second */
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

#ifdef INCLUDE_TIMESTAMP

#include "drv/timer/timestampDev.h"
	

/* Locals */ 

LOCAL int     sysTimestampTicksPerSecond = 60; 
LOCAL BOOL    sysTimestampConnected      = FALSE; 
LOCAL BOOL    sysTimestampRunning        = FALSE; 
LOCAL FUNCPTR sysTimestampRoutine        = NULL; 
LOCAL int     sysTimestampArg            = NULL;


/**************************************************************************
*
* sysTimestampInt - timestamp timer interrupt handler
*
* This routine handles the timestamp timer interrupt.   A user routine is
* called, if one was connected by sysTimestampConnect().
*
* RETURNS: N/A
*
* SEE ALSO: sysTimestampConnect()
*/

LOCAL void sysTimestampInt (void)
    {
    if (sysTimestampRoutine != NULL)     /* call user-connected routine */
        (*sysTimestampRoutine) (sysTimestampArg);
    }

/**************************************************************************
*
* sysTimestampConnect - connect a user routine to a timestamp timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt.  
*
* RETURNS: OK, or ERROR if sysTimestampInt() has not been used.
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampConnect
    (
    FUNCPTR routine, /* routine called at each timestamp timer interrupt */
    int arg          /* argument with which to call routine */
    )
    {

    sysTimestampConnected = TRUE; 
    sysTimestampRoutine = routine;
    sysTimestampArg = arg;
    return (OK);

    }

/**************************************************************************
*
* sysTimestampEnable - enable a timestamp timer interrupt
*
* This routine enables timestamp timer interrupts and resets the counter.
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be enabled.
*
* SEE ALSO: sysTimestampDisable()
*/

STATUS sysTimestampEnable(void)
    {

    if (!sysTimestampRunning)
	{

        (void)intConnect ((void *)VECTOR_TMR1, sysTimestampInt, 0);

	/* Set the Timer Count register to 0 */
	*EP960_TCR1 = TIMER_ROLL_OVER;
	
	/* Load the Timer Reload register */
	*EP960_TRR1 = TIMER_ROLL_OVER;
	
	/* Set the Timer mode Register */
	*EP960_TMR1 = 0x37; /* timer clock = cpu clock / 8 */
	
	/* Enable the timer interrupt bits in the IMSK register */
	*EP960_IMSK |= 0x2000;

        sysTimestampRunning = TRUE;

	}

    /* Aux clock and Timestamp timer both are on the same vector */
    
    return (OK);
    }


/**************************************************************************
*
* sysTimestampDisable - disable a timestamp timer interrupt
*
* This routine disables the timestamp timer.  It does not directly disable
* interrupts.  However, the tick counter does not increment once the 
* timestamp timer is disabled, thus, interrupts are no longer generated.
* This routine merely resets the timer counter.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampDisable (void)
    {
    if (sysTimestampRunning)
	{

	/* Mask the timer interrupt bits in the IMSK register */
	*EP960_IMSK &= 0x10ff;

	/* Set the Timer mode Register */
	*EP960_TMR1 = 0;

	/* Clear the timer interrupts in IPND register, if any */
	vxIPNDClear(0x2000);


	sysTimestampRunning = FALSE;
	}

    return (OK);
    }


/**************************************************************************
*
* sysTimestampPeriod - get the period of a timestamp timer 
*
* This routine gets the period of the timestamp timer, in ticks.  The
* period, or terminal count, is the number of ticks to which the timestamp
* timer counts before rolling over and restarting the counting process.
*
* RETURNS: The period of the timestamp timer in counter ticks.
*/

UINT32 sysTimestampPeriod (void)
    {
    /* 
     * Return the timestamp timer period here.
     * The highest period (maximum terminal count) should be used so
     * that rollover interrupts are kept to a minimum.
     *
     */
    return (TIMER_ROLL_OVER);

    }


/**************************************************************************
*
* sysTimestampFreq - get a timestamp timer clock frequency
*
* This routine gets the frequency of the timer clock, in ticks per 
* second.  The rate of the timestamp timer is set explicitly by the 
* hardware and typically cannot be altered.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {

    /*
     * Return the timestamp tick output frequency here.
     * This value can be determined from the following equation:
     *     timerFreq = clock input frequency / prescaler
     *
     * When possible, read the clock input frequency and prescaler values
     * directly from chip registers.
     */

    return ((1000000 * CPU_SPEED_MHZ) / 8);
    }


/**************************************************************************
*
* sysTimestamp - get a timestamp timer tick count
*
* This routine returns the current value of the timestamp timer tick counter.
* The tick count can be converted to seconds by dividing it by the return of
* sysTimestampFreq().
*
* This routine should be called with interrupts locked.  If interrupts are
* not locked, sysTimestampLock() should be used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampFreq(), sysTimestampLock()
*/

UINT32 sysTimestamp (void)
    {
    unsigned long countValue;

    /* Read the timer counter register */

    countValue = ((TIMER_ROLL_OVER - *EP960_TCR1));

    /* return the timestamp timer tick count here */

    return (countValue);
    }


/**************************************************************************
*
* sysTimestampLock - lock interrupts and get the timestamp timer tick count
*
* This routine locks interrupts when the tick counter must be stopped 
* in order to read it or when two independent counters must be read.  
* It then returns the current value of the timestamp timer tick
* counter.
* 
* The tick count can be converted to seconds by dividing it by the return of
* sysTimestampFreq().
*
* If interrupts are already locked, sysTimestamp() should be
* used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestampFreq(), sysTimestamp()
*/

UINT32 sysTimestampLock (void)
    {
    unsigned long countValue;

    /* Read the timer counter register */

    countValue = (TIMER_ROLL_OVER - *EP960_TCR1);

    /* return the timestamp timer tick count here */

    return (countValue);
    }

#endif   /* INCLUDE_TIMESTAMP */
