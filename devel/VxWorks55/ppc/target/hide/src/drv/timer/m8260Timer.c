/* m8260Timer.c - Motorola MPC8260 Timer Library */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01j,25oct01,g_h  Move the initialization of sysDecClkFrequency to the function 
		 sysClkRateSet, remove #include "ads8260.h", part of SPR 71123.
01i,24oct01,g_h  Add #ifdef INCLUDE_AUX_CLK around AUX clock routines 
		 and variables to prevent compilation warnings, part of SPR 71123.
01h,15sep01,dat  Using new vxDecReload function
01g,16sep99,ms_  m8260Timer.h comes from drv/timer
01f,09sep99,ms_  change bsp name from vads8260 to ads8260
01e,15jul99,ms_  make compliant with our coding standards
01d,02may99,ms_  add aux clock and timestamp facility
01c,17apr99,ms_  final EAR cleanup
01b,14apr99,ms_  use interrupt numbers instead of vectors
01a,28jan99,ms_  adapted from ppc860Timer.c version 01a
*/

/*
DESCRIPTION

This library provides MPC8260 system clock, auxiliary clock, and timestamp
support.

The PowerPC decrementer timer is used to implement the system clock, the CPM
TIMER2 is used for a 16-bit auxiliary clock, and CPM timers TIMER3 and TIMER4
are cascaded into a free-running 32-bit timer for timestamp support.

The macro DEC_CLOCK_FREQ, the frequency of the decrementer input clock, must
be defined before using this module. The macro DEC_CLK_TO_INC, the ratio
between the number of decrementer input clock cycles and one counter
increment, may be redefined prior to including this file in sysLib.c.

The macros SYS_CLK_RATE_MIN and SYS_CLK_RATE_MAX must be defined to 
provide parameter checking for sysClkRateSet() the routine.

The auxiliary clock period is modified by programming the Timer Reference
Register. The input is the general system clock, which can be optionally
divided by 16. The prescaler is always used to divide by 256. The Timer
Reference Register is 16 bits. This information dictates the minimum and 
maximum auxiliarly clock rates.

The slowest rate is when the general system clock is divided by 16 and the 
value for the Reference register is the highest i.e. 65535. This gives us 
a minimum auxiliarly clock rate of 

.CS
	SYS_CPU_FREQ / (256 * 65535 * 16) 
		= 40000000 / 268431360
		= .15
.CE

We define AUX_CLK_RATE_MIN as 1

The highest rate is when the general system clock is not divided by
16, and the reference register is set to its minimum value i.e. 1.
This tells us the maximum auxiliary clock rate:

.CS
	SYS_CPU_FREQ / 256
		= 40000000 / 256
		= 156250
.CE

We define AUX_CLK_RATE_MAX as 8000

INCLUDE FILES: timerDev.h, vxPpcLib.h
   
SEE ALSO:
.pG "Configuration"
*/

/* includes */

#include "arch/ppc/vxPpcLib.h"
#include "drv/timer/timerDev.h"
#include "drv/timer/m8260Timer.h"

/* local defines */

#ifndef	DEC_CLK_TO_INC
#define	DEC_CLK_TO_INC		4		/* # bus clks per increment */
#endif

#ifndef DEC_CLOCK_FREQ
#define DEC_CLOCK_FREQ		OSCILLATOR_FREQ	/* Set to system default */
#endif /* DEC_CLOCK_FREQ */

#ifndef SYS_CPU_FREQ
#define SYS_CPU_FREQ		OSCILLATOR_FREQ	/* Set to system default */
#endif /* SYS_CPU_FREQ */

/* extern declarations */

IMPORT STATUS	excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);

/* locals */

LOCAL int 	sysClkTicksPerSecond 	= 60;	  /* default 60 ticks/second */
LOCAL FUNCPTR	sysClkRoutine		= NULL;
LOCAL int	sysClkArg		= 0;
LOCAL BOOL	sysClkConnectFirstTime	= TRUE;
LOCAL int       decCountVal		= 10000000;	/* default dec value */
LOCAL BOOL	sysClkRunning 		= FALSE;
LOCAL int 	sysDecClkFrequency	= 0;

#ifdef INCLUDE_AUX_CLK
LOCAL FUNCPTR sysAuxClkRoutine        = NULL;
LOCAL int     sysAuxClkArg            = 0;
LOCAL BOOL    sysAuxClkRunning        = FALSE;
LOCAL BOOL    sysAuxClkIntConnected   = FALSE;
LOCAL int     sysAuxClkTicksPerSecond = 60;
#endif /* INCLUDE_AUX_CLK */

#ifdef        INCLUDE_TIMESTAMP
LOCAL BOOL    sysTimestampRunning     = FALSE;   /* timestamp running flag */
LOCAL FUNCPTR sysTimestampRoutine     = NULL;
LOCAL int     sysTimestampArg         = 0;
LOCAL BOOL    sysTimestampIntConnected = FALSE;
#endif        /* INCLUDE_TIMESTAMP */

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
    vxDecReload (decCountVal); /* reload the decrementer */

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
        excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_DECR, (VOIDFUNCPTR) sysClkInt);
	}

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

    sysDecClkFrequency = DEC_CLOCK_FREQ / DEC_CLK_TO_INC;

    /* 
     * compute the value to load in the decrementer. The new value will be
     * loaded into the decrementer after the current period
     */

    decCountVal = sysDecClkFrequency / ticksPerSecond;

    return (OK);
    }

#ifdef INCLUDE_AUX_CLK

/*******************************************************************************
*
* sysAuxClkInt - auxiliary clock interrupt handler
*
* This routine handles the auxiliary clock interrupt.  It calls a user routine
* if one was specified by the routine sysAuxClkConnect().
*/

LOCAL void sysAuxClkInt (void)
    {
    UINT32 immrVal = vxImmrGet();

    *M8260_TER2(immrVal) = M8260_TER_REF | M8260_TER_CAP; /* clear all events */

    if (sysAuxClkRoutine != NULL)
        (*sysAuxClkRoutine) (sysAuxClkArg);
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
    /* connect the ISR to the TIMER2 exception */

    sysAuxClkRoutine   = routine;
    sysAuxClkArg       = arg;

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
    UINT32 immrVal = vxImmrGet();

    if (sysAuxClkRunning)
	{
	m8260IntDisable(INUM_TIMER2);
	*M8260_TGCR1(immrVal) |= M8260_TGCR_STP2;	/* stop timer */

	sysAuxClkRunning = FALSE;		/* clock is no longer running */
	}
    }

/*******************************************************************************
*
* sysAuxClkEnable - turn on auxiliary clock interrupts
*
* This routine enables auxiliary clock interrupts.
* The timer is used in "reference mode" i.e. a value is programmed into 
* the reference register and an interrupt occurs when the timer reaches
* that value. 
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkConnect(), sysAuxClkDisable(), sysAuxClkRateSet()
*/

void sysAuxClkEnable (void)
    {
    UINT32 immrVal = vxImmrGet();

    /* 
     * Calculate the preliminary value for the Reference register. This 
     * does not yet take into account whether we will divide the general 
     * system clock by 16 or not. The 8 bit left shift accounts for 
     * the prescaler which will always be set to 0xFF i.e. the clock 
     * will further be divided by 256.
     *
     */

    UINT32  tempDiv = SYS_CPU_FREQ / (sysAuxClkTicksPerSecond << 8);

    /* 
     * Enable the auxiliary clock only if it is not already running 
     * and if the required reference value will fit in the reference register
     * if the general system clock is divided by 16.
     */

    if ((!sysAuxClkRunning) && (tempDiv < ((1 << (M8260_TRR_SIZE-1)) * 16)))
	{

        /* 
	 * start and hold in reset (i.e. disable) timer2 
	 * Note that RST is active low while STP is active high
	 */

	*M8260_TGCR1(immrVal) &= ~M8260_TGCR_STP2;	/* clear stop bit */
	*M8260_TGCR1(immrVal) &= ~M8260_TGCR_RST2;	/* clear reset bit */

	*M8260_TCN2(immrVal) = 0x0;	/* clear the timer counter */

	/* 
	 * If the preliminary value for the Reference register is small
	 * enough, we don't need to divide the general system clock by 16,
	 * and the preliminary value for the Reference register is the
	 * value used 
	 */
        
	if (tempDiv < (1 << (M8260_TRR_SIZE-1)))
	    {
	    *M8260_TRR2(immrVal) = (UINT16) tempDiv;
            *M8260_TMR2(immrVal) = (
		(M8260_TMR_ICLK_IN_GEN  & M8260_TMR_ICLK_MSK) |    /* int clk */
		M8260_TMR_ORI |                                 /* int on ref */
	        M8260_TMR_FRR |                                 /* free run */
	        (M8260_TMR_PS_MSK & M8260_TMR_PS_MAX));		/* max prscl  */
	    }
	else
	    {
            *M8260_TRR2(immrVal) = (UINT16) (tempDiv / 16);
            *M8260_TMR2(immrVal) = (M8260_TMR_ICLK_IN_GEN_DIV16 | 
			           M8260_TMR_ORI |
                                   M8260_TMR_FRR | 
				   (M8260_TMR_PS_MSK & M8260_TMR_PS_MAX));
	    }
 
	/* clear all timer events */

	*M8260_TER2(immrVal) = M8260_TER_REF | M8260_TER_CAP;

	/* enable timer interrupt */

	m8260IntEnable(INUM_TIMER2);

        if (! sysAuxClkIntConnected)
            {
            (void) intConnect (INUM_TO_IVEC(INUM_TIMER2), 
			       (VOIDFUNCPTR) sysAuxClkInt, 0);
            sysAuxClkIntConnected = TRUE;
            }
        
	/* Enable timer by removing reset, which is active low */

	*M8260_TGCR1(immrVal) |= M8260_TGCR_RST2;	/* set reset bit */

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
    int ticksPerSecond	    /* number of clock interrupts per second */
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

#endif /* INCLUDE_AUX_CLK */

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
    UINT32 immrVal = vxImmrGet();

    *M8260_TER4(immrVal) |= M8260_TER_REF;	/* clear event register */

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
* This routine connects interrupts, and enables the timer device
*
* RETURNS: TRUE always
*/
STATUS sysTimestampEnable (void)
    {
    UINT32 immrVal = vxImmrGet();

    if (sysTimestampRunning)
        {
        *M8260_TCN3(immrVal) = (UINT32) 0;	/* clear the counter */
        return (OK);
        }

    if (! sysTimestampIntConnected)
        {
        (void) intConnect (INUM_TO_IVEC(INUM_TIMER4), 
	                                (VOIDFUNCPTR) sysTimestampInt, 
					0);
        sysTimestampIntConnected = FALSE;
        }

    sysTimestampRunning = TRUE;

    /* cascade timer 3 and 4 in order to get a 32 bit timer */

    *M8260_TGCR2(immrVal) |= M8260_TGCR_CAS4;

    /* start and hold in reset (i.e. disable) timers 3 and 4 */
    
    *M8260_TGCR2(immrVal) &= ~(M8260_TGCR_RST4 | M8260_TGCR_RST3 | 
                              M8260_TGCR_STP4 | M8260_TGCR_STP3 );

    /*
     * When timers 3 and 4 are cascaded, TMR4 is the mode register, 
     * while TMR3 is ignored.  Set prescaler to 0 to divide the clock 
     * (SYS_CPU_FREQ) by 1.
     */

    *M8260_TMR4(immrVal) = M8260_TMR_PS_MIN | M8260_TMR_ICLK_IN_GEN | 
			   M8260_TMR_ORI | M8260_TMR_FRR;

    *M8260_TMR3(immrVal) = M8260_TMR_ICLK_IN_CAS;

    /* 
     * when cascaded, the combined TRR, TCR, and TCN must be referenced
     * with 32-bit bus cycles 
     */

    *((UINT32 *)M8260_TRR3(immrVal)) = (UINT32) sysTimestampPeriod ();
    *((UINT32 *)M8260_TCN3(immrVal)) = (UINT32) 0;

    /* clear all timer events */

    *M8260_TER4(immrVal) = M8260_TER_REF | M8260_TER_CAP;

    /* enable timer interrupt */

    m8260IntEnable(INUM_TIMER4);

   /* enable timer 3 and 4 */

   *M8260_TGCR2(immrVal) |= (M8260_TGCR_RST3 | M8260_TGCR_RST4);	

    return (OK);
    }

/*******************************************************************************
*
* sysTimestampDisable - turn off auxiliary clock interrupts
*
* This routine disables auxiliary clock interrupts.
*
* RETURNS: OK, always
*
* SEE ALSO: sysTimestampEnable()
*/

STATUS sysTimestampDisable (void)
    {
    UINT32 immrVal = vxImmrGet();
    if (sysTimestampRunning)
        {
	m8260IntDisable(INUM_TIMER3);	/* disable interrupt */
	m8260IntDisable(INUM_TIMER4);	/* disable interrupt */

	*M8260_TGCR2(immrVal) |= M8260_TGCR_STP3;	/* stop timer */
	*M8260_TGCR2(immrVal) |= M8260_TGCR_STP4;	/* stop timer */

        sysTimestampRunning = FALSE;            /* clock is no longer running */
        }

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
    
    return (0xffffffff);	/* highest period -> freerunning */
    }

/*******************************************************************************
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
    UINT32 immrVal = vxImmrGet();

    /* Get the prescaler value from TMR4. TMR3 is ignored in cascade mode. */

    return (SYS_CPU_FREQ / ((*M8260_TMR4(immrVal) >> 8) + 1));
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
    UINT32 immrVal = vxImmrGet();
    
    return (*((UINT32 *) M8260_TCN3(immrVal)));
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
    UINT32 immrVal = vxImmrGet();

    return (*((UINT32 *) M8260_TCN3(immrVal)));
    }

#endif  /* INCLUDE_TIMESTAMP */

