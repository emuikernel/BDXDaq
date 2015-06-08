/* templateTimer.c - template timer library */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,15sep01,dat  removed inline assembly, used vxDecReload()
01c,08sep99,cmc  test
01b,15apr99,cmc  Fixed a bug in sysAuxClkRateSet
01a,15apr99,cmc   ppc555Timer.c created 
*/

/*
DESCRIPTION
This library provides PowerPC/555 system clock, auxiliary clock, and timestamp
support.

The PPC555 Decrementer Timer is used to implement the system clock and timestamp.
The PPC555 PIT (Periodic Interrupt Timer) is used to implement the auxiliary clock.

The macro DEC_CLOCK_FREQ (frequency of the decrementer input clock) should be
defined before using this module. The macro DEC_CLK_TO_INC (ratio between the
number of decrementer input clock cycles and one counter increment) can be 
redefined prior to #including this file into sysLib.c.
The macros SYS_CLK_RATE_MIN and SYS_CLK_RATE_MAX must be defined to provide
parameter checking for sysClkRateSet().

To include the timestamp timer facility, the macro INCLUDE_TIMESTAMP must be
defined. Note that changing the system clock rate will affect the timestamp
timer period, which can be read by calling sysTimestampPeriod().

If dynamic bus clock speed calculation is needed, the BSP can define the
macro PPC_TMR_RATE_SET_ADJUST to be a call to the needed routine.
This macro, if defined, will be executed during each call to sysClkRateSet().
PPC_TMR_RATE_SET_ADJUST is usually not defined.

    e.g. Assuming sysClkRateAdjust can compute a correct value
    for sysDecClkFrequency.

    #define PPC_TMR_RATE_SET_ADJUST \
	sysClkRateAdjust (&sysDecClkFrequency)

INCLUDE FILES: ppc555Timer.h
   
SEE ALSO:
.pG "Configuration"
*/

/* includes */

#include "vxWorks.h"
#include "config.h"
#include "arch/ppc/vxPpcLib.h"
#include "drv/timer/timerDev.h"
#include "drv/timer/timestampDev.h"

/* local defines */

#ifndef	DEC_CLK_TO_INC
#define	DEC_CLK_TO_INC		1		/* # bus clks per increment */
#endif

#define DEFAULT_DEC_CLK_FREQ	33333333	/* 33.33 Mhz default */

#define	DEC_SHIFT 0

#ifndef CPU_INT_UNLOCK
#    define  CPU_INT_UNLOCK(x) (intUnlock(x))
#endif

#ifndef CPU_INT_LOCK
#    define  CPU_INT_LOCK(x) (*x = intLock())
#endif

/* extern declarations */
IMPORT STATUS excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);
IMPORT UINT32 vxImemBaseGet();

/* Locals */
LOCAL int 	sysClkTicksPerSecond 	= 60;	  /* default 60 ticks/second */
LOCAL FUNCPTR	sysClkRoutine		= NULL;
LOCAL int	sysClkArg		= NULL;
LOCAL BOOL	sysClkConnectFirstTime	= TRUE;
LOCAL int	decCountVal		= 10000000;	/* default dec value */
LOCAL BOOL	sysClkRunning 		= FALSE;
LOCAL int 	sysDecClkFrequency	= DEFAULT_DEC_CLK_FREQ/DEC_CLK_TO_INC;

LOCAL FUNCPTR sysAuxClkRoutine	= NULL;
LOCAL int sysAuxClkArg		= NULL;
LOCAL BOOL sysAuxClkRunning	= FALSE;
LOCAL BOOL sysAuxClkIntConnected	= FALSE;
LOCAL int sysAuxClkTicksPerSecond = 60;

#ifdef	INCLUDE_TIMESTAMP
LOCAL BOOL	sysTimestampRunning  	= FALSE;   /* timestamp running flag */
#endif	/* INCLUDE_TIMESTAMP */

/*******************************************************************************
*
* sysClkInt - clock interrupt handler
*
* This routine handles the clock interrupt on the PowerPC architecture. It is
* attached to the decrementer vector by the routine sysClkConnect().
*
* RETURNS : N/A
*/

LOCAL void sysClkInt (void)
    {
    vxDecReload (decCountVal); /* reload decrementer */

    /* Unlock interrupts during decrementer processing */

    CPU_INT_UNLOCK (_PPC_MSR_EE);

    /* execute the system clock routine */

    if (sysClkRunning && (sysClkRoutine != NULL))
	(*(FUNCPTR) sysClkRoutine) (sysClkArg);

    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c to
* connect usrClock() to the system clock interrupt.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/
    
STATUS sysClkConnect
    (
    FUNCPTR 	routine,	/* routine to connect */
    int 	arg		/* argument for the routine */
    )
    {

    if (sysClkConnectFirstTime)
	{
	sysHwInit2();
	sysClkConnectFirstTime = FALSE;
	}

#ifdef	DEC_CLOCK_FREQ 
    sysDecClkFrequency = DEC_CLOCK_FREQ / DEC_CLK_TO_INC;
#endif	/* DEC_CLOCK_FREQ */

    /* connect the routine to the decrementer exception */

    excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_DECR, (VOIDFUNCPTR) sysClkInt);
    
    sysClkRoutine	= routine;
    sysClkArg		= arg;

    return (OK);
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
	sysClkRunning = TRUE;
	vxDecSet (decCountVal);
	}
    }

/******************************************************************************
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
	sysClkRunning = FALSE;
    }

/******************************************************************************
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

/******************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.  It is called
* by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkEnable(), sysClkRateGet()
*/
   
STATUS sysClkRateSet
    (
    int 	ticksPerSecond  /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
        return (ERROR);

    /* save the clock speed */

    sysClkTicksPerSecond = ticksPerSecond;

    /* Calibrate the clock, if needed. */

#ifdef PPC_TMR_RATE_SET_ADJUST
    PPC_TMR_RATE_SET_ADJUST;
#endif

    /* 
     * compute the value to load in the decrementer. The new value will be
     * load in the decrementer after the end of the current period
     */

    decCountVal = sysDecClkFrequency / ticksPerSecond;

    return (OK);
    }


#ifdef  INCLUDE_TIMESTAMP

/*******************************************************************************
*
* sysTimestampConnect - connect a user routine to a timestamp timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt.  
*
* NOTE: This routine has no effect, since the CPU decrementer has no
* timestamp timer interrupt.
*
* RETURNS: ERROR, always.
*/

STATUS sysTimestampConnect
    (
    FUNCPTR routine,    /* routine called at each timestamp timer interrupt */
    int arg             /* argument with which to call routine */
    )
    {
    return (ERROR);
    }

/*******************************************************************************
*
* sysTimestampEnable - enable a timestamp timer interrupt
*
* This routine enables timestamp timer interrupts and resets the counter.
*
* NOTE: This routine has no effect, since the CPU decrementer has no
* timestamp timer interrupt.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampDisable()
*/

STATUS sysTimestampEnable (void)
   {
   if (sysTimestampRunning)
      {
      return (OK);
      }

   if (!sysClkRunning)          /* don't have any auxiliary clock ! */
      return (ERROR);

   sysTimestampRunning = TRUE;

   return (OK);
   }

/*******************************************************************************
*
* sysTimestampDisable - disable a timestamp timer interrupt
*
* This routine disables timestamp timer interrupts.
*
* NOTE: This routine has no effect, since the CPU decrementer has no
* timestamp timer interrupt.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampDisable (void)
    {
    if (sysTimestampRunning)
        sysTimestampRunning = FALSE;

    return (OK);
    }

/*******************************************************************************
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
     * The period of the timestamp depends on the clock rate of the on-chip
     * timer (ie the Decrementer reload value).
     */
    
    return (decCountVal);
    }

/*******************************************************************************
*
* sysTimestampFreq - get a timestamp timer clock frequency
*
* This routine gets the frequency of the timer clock, in ticks per 
* second.  The rate of the timestamp timer is set explicitly by the 
* hardware and typically cannot be altered.
*
* NOTE: Because the PowerPC decrementer clock serves as the timestamp timer,
* the decrementer clock frequency is also the timestamp timer frequency.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    return (sysDecClkFrequency);
    }

/*******************************************************************************
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
    return (decCountVal - (INT32) vxDecGet());
    }

/*******************************************************************************
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
    UINT32 currentDecValue;
    int oldLevel;

    oldLevel = intLock ();                              /* LOCK INTERRUPT */
    currentDecValue = vxDecGet();
    intUnlock (oldLevel);                               /* UNLOCK INTERRUPT */

    return (decCountVal - currentDecValue);
    }

#endif  /* INCLUDE_TIMESTAMP */

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
    UINT32 imemBase = vxImemBaseGet();  /* internal memory Base Address */

    /* read PS bit in PISCR, and then reset it to terminate int request */
    if(*PISCR(imemBase) & PISCR_PS)
            *PISCR(imemBase) |= PISCR_PS;

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

    /* connect the ISR to PIT exception */
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
    UINT32 imemBase = vxImemBaseGet();  /* internal memory Base Address */

    if (sysAuxClkRunning)
        {
        *PISCR(imemBase) &= ~PISCR_PIE;      /* disable PIT interrupts */
        *PISCR(imemBase) &= ~PISCR_PTE;       /* stop PIT */

	intDisable (PIT_INT_LVL); /* disable int controller PIT level */

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
    UINT32 imemBase = vxImemBaseGet();  /* internal memory Base Address */

    if (!sysAuxClkIntConnected)
        {
	*PISCR(imemBase) |= PISCR_PIRQ;	/* set PIT int level */

        /* connect sysAuxClkInt to PIT interrupt */
        (void) intConnect (INUM_TO_IVEC(PIT_INT_LVL), (VOIDFUNCPTR) sysAuxClkInt, NULL);
        
        sysAuxClkIntConnected = TRUE;
        }

    if (!sysAuxClkRunning)
        {
        /* clear any pendding PIT int request */
        if(*PISCR(imemBase) & PISCR_PS)
            *PISCR(imemBase) |= PISCR_PS;

        /* set clock divider */
	*SCCR(imemBase) &= ~SCCR_RTDIV_MSK;
	*SCCR(imemBase) |= SCCR_RTDIV;

        /* set counter preload value */
        *PITC(imemBase) = (PIT_CLOCK_FREQ / sysAuxClkTicksPerSecond) << 16; 

        *PISCR(imemBase) |= PISCR_PTE;      /* start PIT */
        *PISCR(imemBase) |= PISCR_PIE;      /* enable PIT interrupts */

	intEnable (PIT_INT_LVL); /* enable int controller PIT level */

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
    /* return ERROR if rate is not supported */
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
