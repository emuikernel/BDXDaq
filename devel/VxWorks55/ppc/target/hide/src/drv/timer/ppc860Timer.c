/* ppc860Timer.c - PowerPC/860 timer library */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,22dec01,g_h  Remove compilation warning
01c,09oct01,dtr  Removing unwanted NULLs(funcptrs) when int required.
		 Also put in fix for SPR65678.
01b,15sep01,dat  Use new vxDecReload function
01a,11nov97,map  written from ppcDecTimer.c (v 01n), and m68360Timer.c (v 01f).
*/

/*
DESCRIPTION

This library provides PowerPC/860 system clock, auxiliary clock, and timestamp
support.

The PowerPC decrementer timer, is used to implement a system clock, and CPM
TIMER2 for a 16-bit auxiliary clock. CPM timers TIMER3, and TIMER4
are cascaded into a free-running 32-bit timer for timestamp support.

The macro, DEC_CLOCK_FREQ, the frequency of the decrementer input clock, must
be defined before using this module. The macro, DEC_CLK_TO_INC, the ratio
between the number of decrementer input clock cycles and one counter
increment, may be redefined prior to including this file in sysLib.c.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for
sysClkRateSet(), and sysAuxClkRateSet() routines.

To include the timestamp timer facility, the macro INCLUDE_TIMESTAMP must be
defined. 

To enable dynamic bus clock rates, the BSP should set the macro,
PPC_TMR_RATE_SET_ADJUST, to call an apropriate routine. This call, will be
made each time sysClkRateSet() is called. E.g. To use sysClkRateAdjust to
compute a correct value for sysDecClkFrequency.

  #define PPC_TMR_RATE_SET_ADJUST sysClkRateAdjust (&sysDecClkFrequency)

The macro, PPC_TMR_RATE_SET_ADJUST, is by default not defined.

INCLUDE FILES: timerDev.h, vxPpcLib.h
   
SEE ALSO:
.pG "Configuration"
*/

/* includes */

#include "arch/ppc/vxPpcLib.h"
#include "drv/timer/timerDev.h"

/* local defines */

#ifndef	DEC_CLK_TO_INC
#define	DEC_CLK_TO_INC		4		/* # bus clks per increment */
#endif

#ifndef DEC_CLOCK_FREQ
#define DEC_CLOCK_FREQ		33333333	/* 33.33 Mhz default */
#endif /* DEC_CLOCK_FREQ */

#define	CPM_MEM_BASE		INTERNAL_MEM_MAP_ADDR

/* extern declarations */

IMPORT STATUS	excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);

/* locals */

LOCAL int 	sysClkTicksPerSecond 	= 60;	  /* default 60 ticks/second */
LOCAL FUNCPTR	sysClkRoutine		= NULL;
LOCAL int	sysClkArg		= 0;
LOCAL BOOL	sysClkConnectFirstTime	= TRUE;
LOCAL int       decCountVal		= 10000000;	/* default dec value */
LOCAL BOOL	sysClkRunning 		= FALSE;
LOCAL int 	sysDecClkFrequency	= DEC_CLOCK_FREQ/DEC_CLK_TO_INC;

LOCAL FUNCPTR	sysAuxClkRoutine	= NULL;
LOCAL int	sysAuxClkArg		= 0;
LOCAL BOOL	sysAuxClkRunning	= FALSE;
LOCAL BOOL	sysAuxClkIntConnected	= FALSE;
LOCAL int	sysAuxClkTicksPerSecond	= 60;

#ifdef	INCLUDE_TIMESTAMP
LOCAL BOOL	sysTimestampRunning  	= FALSE;   /* timestamp running flag */
LOCAL FUNCPTR	sysTimestampRoutine	= NULL;
LOCAL int	sysTimestampArg		= 0;
LOCAL BOOL	sysTimestampIntConnected = FALSE;
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

#ifdef USE_KEYED_REGS
    vxKeyedDecReload(decCountVal);
#else
    vxDecReload (decCountVal); /* reload decrementer */
#endif /*USE_KEYED_REGS*/

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

    /* 
     * compute the value to load in the decrementer. The new value will be
     * load in the decrementer after the end of the current period
     */

    decCountVal = sysDecClkFrequency / ticksPerSecond;

    return (OK);
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
    *TER2(CPM_MEM_BASE) |= TER_REF;     /* clear event register */
    *CISR(CPM_MEM_BASE) = CISR_TIMER2;  /* clear in-service bit */

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
    if (sysAuxClkRunning)
	{
	*CIMR(CPM_MEM_BASE) &= ~CISR_TIMER2;	/* disable interrupt */
	*TGCR(CPM_MEM_BASE) |= TGCR_STP2;	/* stop timer */

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
    UINT32  tempDiv = SYS_CPU_FREQ / (sysAuxClkTicksPerSecond << 8);

    if ((!sysAuxClkRunning) && (tempDiv <= USHRT_MAX * 16))
	{
        /* start, reset, but disable timer2 */
        
        *TGCR(CPM_MEM_BASE) &= ~(TGCR_RST2 | TGCR_STP2);
        *TCN2(CPM_MEM_BASE) = 0x0;

	if (tempDiv <= USHRT_MAX)
	    {
            *TRR2(CPM_MEM_BASE) = (UINT16) tempDiv;
            *TMR2(CPM_MEM_BASE) = (TMR_ICLK_IN_GEN | TMR_ORI |
                                   TMR_FRR | (TMR_PS_MSK & 0xff00));
	    }
	else
	    {
            *TRR2(CPM_MEM_BASE) = (UINT16) (tempDiv / 16);
            *TMR2(CPM_MEM_BASE) = (TMR_ICLK_IN_GEN_DIV16 | TMR_ORI |
                                   TMR_FRR | (TMR_PS_MSK & 0xff00));
	    }
 
        *TER2(CPM_MEM_BASE) = 0xffff;		/* clear event */
        *CIMR(CPM_MEM_BASE) |= CISR_TIMER2;	/* unmask interupt */

        if (! sysAuxClkIntConnected)
            {
            (void) intConnect (IV_TIMER2, (VOIDFUNCPTR) sysAuxClkInt, 0);
            sysAuxClkIntConnected = TRUE;
            }
        
        *TGCR(CPM_MEM_BASE) |= TGCR_RST2;	/* enable timer2 */

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
    *TER4(CPM_MEM_BASE) |= TER_REF;	/* clear TMR4 reference bit */
    *CISR(CPM_MEM_BASE) = CISR_TIMER4;	/* clear TMR4 interrupt 
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
* This routine connects interrupts, and enables the timer device
*
* RETURNS: TRUE always
*/
STATUS sysTimestampEnable (void)
    {
    if (sysTimestampRunning)
        {
        *TCN3(CPM_MEM_BASE) = (UINT32) 0;	/* clear the counter */
        return (OK);
        }

    if (! sysTimestampIntConnected)
        {
        (void) intConnect (IV_TIMER4, (VOIDFUNCPTR) sysTimestampInt, 0);
        sysTimestampIntConnected = FALSE;
        }

    sysTimestampRunning = TRUE;

    /* cascade timer 3 and 4 in order to get a 32 bit timer */

    *TGCR(CPM_MEM_BASE) |= TGCR_CAS4;

    /* start, reset, but disable timers 3 and 4 */
    
    *TGCR(CPM_MEM_BASE) &= ~(TGCR_RST4 | TGCR_RST3 | TGCR_STP4 | TGCR_STP3 );

    /*
     * When timers 3 and 4 are cascaded, TMR4 is the mode register, while
     * TMR3 is ignored.
     * Set prescaler to 0, to divide the clock (SYS_CPU_FREQ) by 1.
     */

    *TMR4(CPM_MEM_BASE) = 0x0000 | TMR_ICLK_IN_GEN | TMR_ORI | TMR_FRR;
    *TMR3(CPM_MEM_BASE) = TMR_ICLK_IN_CAS;

    *((UINT32 *)TRR3(CPM_MEM_BASE)) = (UINT32) sysTimestampPeriod ();
    *((UINT32 *)TCN3(CPM_MEM_BASE)) = (UINT32) 0;
    *TER4(CPM_MEM_BASE) = (UINT16) 0xffff;
    *CIMR(CPM_MEM_BASE) |= CISR_TIMER4;
    *TGCR(CPM_MEM_BASE) |= TGCR_RST4 | TGCR_RST3;	/* enable */

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
    if (sysTimestampRunning)
        {
        *CIMR(CPM_MEM_BASE) &= ~CISR_TIMER4;	/* disable interrupt */
        *CIMR(CPM_MEM_BASE) &= ~CISR_TIMER3;	/* disable interrupt */
        *TGCR(CPM_MEM_BASE) |= TGCR_STP4;	/* stop timer */
        *TGCR(CPM_MEM_BASE) |= TGCR_STP3;	/* stop timer */

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
* NOTE: Because the PowerPC decrementer clock serves as the timestamp timer,
* the decrementer clock frequency is also the timestamp timer frequency.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    /* Get the prescaler value from TMR4. TMR3 is ignored in cascade mode. */

    return (SYS_CPU_FREQ / ((*TMR4(CPM_MEM_BASE) >> 8) + 1));
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
    return (*((UINT32 *) TCN3(CPM_MEM_BASE)));
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
    return (*((UINT32 *) TCN3(CPM_MEM_BASE)));
    }

#endif  /* INCLUDE_TIMESTAMP */

