/* ppc403Timer.c - PowerPC 403 timer library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,19sep01,pch  SPR 20698, 69142, 69143:  TSR is write-to-clear,
		 not read/modify/writeback
01n,25sep97,ms2  corrected sysAuxClkTicksPerSecond (spr 9348)
01m,16jul97,tam  add code to enable change of sysClk rate on the fly for GCX. 
01l,07may97,tam  added use of IS_CPU_403GCX macro. 
01k,17apr97,dat  rework on SYS_CLK_FREQ, fixed SPR 8372 - extra calls to
		 excIntConnect.
01j,09apr97,mkw  add support for 403GCX
01i,24mar97,tam  replace excCrtConnect by excIntCrtConnect in sysWdtConnect().
01h,08nov96,wlf  doc: cleanup.
01g,07oct96,tam  fixed spr 7242: declared sysBusClkFrequency as a LOCAL.
01f,02jul96,tam  added code to make system clock more accurate. Added
		 timestamp support.
01e,19mar96,tam  fixed sysAuxClkEnable() & sysWdtClkEnable().
01d,12mar96,tam  changed excIntConnect to excIntConnectTimer for FIT & PIT.
01c,11mar96,tam  added functions for the watchdog timer (WDT).
01b,07mar96,tam  added functions for the auxiliary clock (FIT).
01a,20feb96,tpr  written, inspired from ppcDecTimer.c version 01g.
*/

/*
DESCRIPTION
This library provides PowerPC 403 Timer routines.  This library handles
the system clock, the auxiliary clock and timestamp functions. To
include the timestamp timer facility, the macro INCLUDE_TIMESTAMP must
be defined.

The macro SYS_CLK_FREQ should be defined before using
this module.  This macro is the rate at which clock ticks occur.  For
double clocked busses, this value should be half the actual clock speed.

The macro PIT_CORRECTION is needed to adjust the reload value to account
for the latency delay between generation of the interrupt and the time
to actually enter the service routine.  If not defined, an approximate
value is calculated from the SYS_CLK_FREQ value.  The calculated value
for a 25Mhz clock is a 9 microsecond correction.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN,
and AUX_CLK_RATE_MAX must be defined to provide
parameter checking for sysClkRateSet().

The BSP is responsible for connecting the interrupt handlers, sysClkInt(),
and sysAuxClkInt(), to the appropriate vectors.

INCLUDE FILES: ppc403Timer.h

SEE ALSO:
.pG "Configuration"
*/

/* includes */

#include "vxLib.h"
#include "intLib.h"
#include "drv/timer/ppc403Timer.h"
#include "arch/ppc/ppc403.h"
#include "drv/timer/timestampDev.h"

/* local defines */

/* System clock speed, default is 25 MHz */

#ifndef	SYS_CLK_FREQ
#   define SYS_CLK_FREQ		25000000
#endif

/*
 * The PIT_CORRECTION is expressed in nanoseconds.  It is used
 * to correct the reload value for the 403GA and 403GC cpus
 * which don't have an autoreload feature on the PIT clock.  It accounts
 * for the latency time between when the interrupt was generated and when
 * the interrupt handler was entered.
 */

#ifndef PIT_CORRECTION
#   define PIT_CORRECTION	(225000000 / (SYS_CLK_FREQ / 1000))
#endif

/* IS_CPU_403GCX returns TRUE if the CPU is a 403GCX, FALSE otherwise */

#ifndef IS_CPU_403GCX
#   define IS_CPU_403GCX      ((vxPvrGet() & (_PVR_CCF_MSK | _PVR_PCF_MSK)) \
                              == (_PVR_CONF_403GCX << 8) ? 1:0)
#endif

/* Auxiliary clock default rate */
#ifndef AUX_CLK_RATE_DEFAULT
#   define AUX_CLK_RATE_DEFAULT 10
#endif

/* extern declarations */

/* Locals */

LOCAL FIT_PERIOD fitTable[] =                   /* available FIT periods */
    {
    {   (1 << 9),  0x00000000},
    {   (1 << 13), 0x01000000},
    {   (1 << 17), 0x02000000},
    {   (1 << 21), 0x03000000},
    };

LOCAL WDT_PERIOD wdtTable[] =                   /* available WDT periods */
    {
    {   (1 << 17), 0x00000000},
    {   (1 << 21), 0x40000000},
    {   (1 << 25), 0x80000000},
    {   (1 << 29), 0xC0000000},
    };

LOCAL int 	sysClkTicksPerSecond 	= 60;	/* default 60 ticks/second */
LOCAL FUNCPTR	sysClkRoutine		= NULL;
LOCAL int	sysClkArg		= NULL;
LOCAL BOOL	sysClkConnectFirstTime	= TRUE;
LOCAL BOOL	sysClkRunning 		= FALSE;
LOCAL BOOL	sysClkReload		= TRUE;
LOCAL BOOL	sysClkReloadOnce	= FALSE;

LOCAL int 	sysAuxClkTicksPerSecond = AUX_CLK_RATE_DEFAULT;
LOCAL FUNCPTR	sysAuxClkRoutine	= NULL;
LOCAL int	sysAuxClkArg		= NULL;
LOCAL BOOL	sysAuxClkRunning 	= FALSE;

LOCAL int 	sysWdtTicksPerSecond	= 1;
LOCAL FUNCPTR	sysWdtRoutine		= NULL;
LOCAL int	sysWdtArg		= NULL;
LOCAL BOOL	sysWdtRunning 		= FALSE;

LOCAL UINT32	sysBusClkFrequency 	= TIMER_CLK_FREQ;
LOCAL UINT32	pitCountVal;			/* PIT counter value */
LOCAL UINT32 	fitPeriodMask = 0x01000000;	/* default FIT period: 2^21 */
LOCAL UINT32	wdtPeriodMask = 0xC0000000;	/* default WDT period: 2^29 */

LOCAL UINT32	pitCorrection;

#ifdef	INCLUDE_TIMESTAMP
LOCAL BOOL      sysTimestampRunning     = FALSE;   /* timestamp running flag */
#endif 	/* INCLUDE_TIMESTAMP */

/*******************************************************************************
*
* sysClkInt - clock interrupt handler
*
* This routine handles the clock interrupt on the PowerPC 403 architecture. It
* is attached to the Programmable Interval Timer vector by the routine
* sysClkConnect().
*
* RETURNS : N/A
*/

LOCAL void sysClkInt (void)
    {

    vxPitIntAck ();            	/* acknowledge PIT interrupt */

    /*
     * Since the auto-reload feature of the PIT seems not to work properly
     * for non-GCX it is necessary to reload the Programmable Interrupt Timer.
     * The reload value should be adjusted each time because the time spent
     * between the exception generation and the moment the register is reloaded
     * changes.
     * Since the PIT does not keep decrementing after it reaches 0 we cannot
     * read it again and adjust pitCountVal. The current correction corresponds
     * to the average time spend between the exception generation and the
     * moment the register is reloaded. A better solution would be to use
     * the time base to calculate that value dynamically but that requires
     * adding code to the PPC interrupt stub routine.
     */

    if (sysClkReload)
	vxPitSet(pitCountVal - pitCorrection);

    if (sysClkReloadOnce)
	{
	vxPitSet(pitCountVal);		/* reload the PIT once after a call */
	sysClkReloadOnce = FALSE;	/* to sysClkRateSet() for GCX cpu   */
	}

    /* execute the system clock routine */

    if (sysClkRoutine != NULL)
	(*(FUNCPTR) sysClkRoutine) (sysClkArg);

    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  Normally, it is called from usrRoot() in usrConfig.c to
* connect usrClock() to the system clock interrupt.  It also connects the
* clock error interrupt service routine.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR 	routine,	/* routine to connect */
    int 	arg		/* argument with which to call the routine */
    )
    {

    if (sysClkConnectFirstTime)
	{
	sysHwInit2();
	sysClkConnectFirstTime = FALSE;
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
	/* clear the pending PIT interrupt */

	vxTsrSet (_PPC403_TSR_PIS);

	/* load the PIT counter and the hidden register with interval value */

	vxPitSet (pitCountVal);

	/* Enable the PIT interrupt & enable autoreload for 403GCX */

	if (IS_CPU_403GCX)
	    {
	    vxTcrSet (vxTcrGet() | _PPC403_TCR_PIE | _PPC403_TCR_ARE);
	    sysClkReload = FALSE;
	    }
	else
	    vxTcrSet (vxTcrGet() | _PPC403_TCR_PIE);

	/* set the running flag */

	sysClkRunning = TRUE;
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
	{
	/* disable the PIT interrupt and auto-reload capability */

	vxTcrSet (vxTcrGet() & ~ (_PPC403_TCR_PIE | _PPC403_TCR_ARE));

	/* clear the PIT counter */

	vxPitSet (0);

	/* clear the pending PIT interrupt */

	vxTsrSet (_PPC403_TSR_PIS);

	/* reset the running flag */

	sysClkRunning = FALSE;
	}
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
* This routine sets the interrupt rate of the system clock.  It does not
* enable system clock interrupts.  It is called by usrRoot() in
* usrConfig.c.
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

    /*
     * compute the value to load in the decrementor. The new value will be
     * load in the decrementor after the end of the current period
     */

    if (vxIocrGet () & _PPC403_IOCR_TCSM)
	sysBusClkFrequency = TIMER_CLK_FREQ;
    else
	sysBusClkFrequency = SYS_CLK_FREQ;

    pitCountVal = sysBusClkFrequency / ticksPerSecond;

    pitCorrection = (((sysBusClkFrequency / 1000000) *
		    PIT_CORRECTION) / 1000);

    if (IS_CPU_403GCX)
	sysClkReloadOnce = TRUE;	/* reload the PIT with new value */

    return (OK);
    }

/*******************************************************************************
*
* sysAuxClkInt - auxilary clock interrupt handler
*
* This routine handles the auxilary clock interrupt on the PowerPC 403
* architecture. It is attached to the Fix Interval Timer vector by the routine
* sysAuxClkConnect().
*
* RETURNS : N/A
*/

LOCAL void sysAuxClkInt (void)
    {

    vxFitIntAck ();             /* acknowledge FIT interrupt */

    /* program TCR with the FIT period */

    vxTcrSet ((vxTcrGet() & ~_PPC403_TCR_FP) | fitPeriodMask);

    /* execute the system clock routine */

    if (sysAuxClkRoutine != NULL)
	(*(FUNCPTR) sysAuxClkRoutine) (sysAuxClkArg);

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
* SEE ALSO: excIntConnectTimer(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,	/* routine called at each aux. clock interrupt */
    int     arg		/* argument to auxiliary clock interrupt */
    )
    {

    sysAuxClkRoutine	= routine;
    sysAuxClkArg	= arg;

    return (OK);
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
	/* clear the pending FIT interrupt */

	vxTsrSet (_PPC403_TSR_FIS);

	/* program TCR with the FIT period */

	vxTcrSet ((vxTcrGet() & ~_PPC403_TCR_FP) | fitPeriodMask);

	/* Enable the FIT interrupt */

	vxTcrSet (vxTcrGet() | _PPC403_TCR_FIE);

	/* set the running flag */

	sysAuxClkRunning = TRUE;
	}
    }

/*******************************************************************************
*
* sysAuxClkDisable - turn off auxiliary clock interrupts
*
* This routine disables auxiliary clock interrupts.
*
*
* RETURNS: N/A
*
* SEE ALSO: sysAuxClkEnable()
*/

void sysAuxClkDisable (void)
    {
    if (sysAuxClkRunning)
	{
	/* disable the FIT interrupt */

	vxTcrSet (vxTcrGet() & ~ (_PPC403_TCR_FIE));

	/* clear the pending FIT interrupt */

	vxTsrSet (_PPC403_TSR_FIS);

	/* reset the running flag */

	sysAuxClkRunning = FALSE;
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
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    int ix;
    int jx = 0;
    UINT32 fitPeriod;

    /*
     * compute the FIT period.
     * only 4 values are possible (cf fitTable[]). The closest value to
     * <ticksPerSecond> is being used.
     */

    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
        return (ERROR);

    if (vxIocrGet () & _PPC403_IOCR_TCSM)
        {       /* service clock */
        sysBusClkFrequency = TIMER_CLK_FREQ;
        }
    else sysBusClkFrequency = SYS_CLK_FREQ;

    fitPeriod = sysBusClkFrequency / ticksPerSecond;

    /* get the closest value to ticksPerSecond supported by the FIT */

    for (ix = 0; ix < NELEMENTS (fitTable); ix++)
        {
        if (fitPeriod <= fitTable [ix].fitPeriod)
            {
	    if (ix != 0)
		if ( fitPeriod <
		     ((fitTable [ix].fitPeriod + fitTable [ix-1].fitPeriod)/2))
		    jx = ix-1;
                else
		    jx = ix;
	    else
		jx = ix;
            break;
            }
	if (ix == NELEMENTS (fitTable) - 1)
	    jx = ix;
        }
    fitPeriod = fitTable [jx].fitPeriod;	/* actual period of the FIT */
    fitPeriodMask = fitTable [jx].fpMask;	/* Mask to program TCR with */

    /* save the clock speed */

    sysAuxClkTicksPerSecond = sysBusClkFrequency / fitPeriod;

    return (OK);
    }

/*******************************************************************************
*
* sysWdtInt - watchdog interrupt handler.
*
* This routine handles the watchdog interrupt on the PowerPC 403
* architecture. It is attached to the watchdog timer vector by the routine
* sysWdtConnect().
*
* RETURNS : N/A
*/

LOCAL void sysWdtInt (void)
    {
    /* acknowledge WDT interrupt */

    vxTsrSet (_PPC403_TSR_WIS);

    /* execute the watchdog  clock routine */

    if (sysWdtRoutine != NULL)
        (*(FUNCPTR) sysWdtRoutine) (sysWdtArg);

    }

/*******************************************************************************
*
* sysWdtConnect - connect a routine to the watchdog interrupt
*
* This routine specifies the interrupt service routine to be called at each
* watchdog interrupt.  It does not enable watchdog interrupts.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the watchdog
* interrupt.
*
* SEE ALSO: excIntCrtConnect(), sysWdtEnable()
*/

STATUS sysWdtConnect
    (
    FUNCPTR routine,    /* routine called at each watchdog interrupt */
    int     arg         /* argument with which to call routine         */
    )
    {
    /* connect the routine to the WDT vector */

    excIntCrtConnect ((VOIDFUNCPTR *) _EXC_OFF_WD, (VOIDFUNCPTR) sysWdtInt);

    sysWdtRoutine    = routine;
    sysWdtArg        = arg;

    return (OK);
    }

/*******************************************************************************
*
* sysWdtEnable - turn on watchdog interrupts
*
* This routine enables watchdog interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysWdtConnect(), sysWdtDisable(), sysWdtRateSet()
*/

void sysWdtEnable (void)
    {
    if (!sysWdtRunning)
        {
        /* clear the pending WDT interrupt */

        vxTsrSet (_PPC403_TSR_WIS);

        /* program TCR with the WDT period */

        vxTcrSet ((vxTcrGet() & ~_PPC403_TCR_WP) | wdtPeriodMask);

        /* Enable the WDT interrupt */

        vxTcrSet (vxTcrGet() | _PPC403_TCR_WIE);

	/* Enable critical interrupt */

	vxMsrSet (vxMsrGet() | _PPC_MSR_CE);

        /* set the running flag */

        sysWdtRunning = TRUE;
        }
    }

/*******************************************************************************
*
* sysWdtDisable - turn off watchdog interrupts
*
* This routine disables watchdog interrupts.
* This routine does not disable critical interrupts.
*
* RETURNS: N/A
*
* SEE ALSO: sysWdtEnable()
*/

void sysWdtDisable (void)
    {
    if (sysWdtRunning)
        {
        /* disable the WDT interrupt */

        vxTcrSet (vxTcrGet() & ~ (_PPC403_TCR_WIE));

        /* clear the pending WDT interrupt */

        vxTsrSet (_PPC403_TSR_WIS);

        /* reset the running flag */

        sysWdtRunning = FALSE;
        }
    }

/*******************************************************************************
*
* sysWdtRateGet - get the watchdog timer rate
*
* This routine returns the interrupt rate of the watchdog timer.
*
* RETURNS: The number of watchdog interrupts per second.
*
* SEE ALSO: sysWdtEnable(), sysWdtRateSet()
*/

int sysWdtRateGet (void)
    {
    return (sysWdtTicksPerSecond);
    }

/*******************************************************************************
*
* sysWdtRateSet - set the watchdog timer rate
*
* This routine sets the interrupt rate of the watchdog timer.  It does not
* enable watchdog interrupts.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysWdtEnable(), sysWdtRateGet()
*/

STATUS sysWdtRateSet
    (
    int ticksPerSecond      /* number of clock interrupts per second */
    )
    {
    int ix;
    int jx = 0;
    UINT32 wdtPeriod;

    /*
     * compute the WDT period.
     * only 4 values are possible (cf wdtTable[]). The closest value to
     * <ticksPerSecond> is being used.
     */

    if (ticksPerSecond < WDT_RATE_MIN || ticksPerSecond > WDT_RATE_MAX)
        return (ERROR);

    if (vxIocrGet () & _PPC403_IOCR_TCSM)
        {       /* service clock */
        sysBusClkFrequency = TIMER_CLK_FREQ;
        }
    else sysBusClkFrequency = SYS_CLK_FREQ;

    wdtPeriod = sysBusClkFrequency / ticksPerSecond;

    /* get the closest value to ticksPerSecond supported by the WDT */

    for (ix = 0; ix < NELEMENTS (wdtTable); ix++)
        {
        if (wdtPeriod <= wdtTable [ix].wdtPeriod)
            {
            if (ix != 0)
                if ( wdtPeriod <
                     ((wdtTable [ix].wdtPeriod + wdtTable [ix-1].wdtPeriod)/2))
                    jx = ix-1;
                else
                    jx = ix;
            else
                jx = ix;
            break;
            }
        if (ix == NELEMENTS (wdtTable) - 1)
            jx = ix;
        }
    wdtPeriod = wdtTable [jx].wdtPeriod;        /* actual period of the WDT */
    wdtPeriodMask = wdtTable [jx].fpMask;	/* Mask to program TCR with */

    /* save the clock speed */

    sysWdtTicksPerSecond = sysBusClkFrequency / wdtPeriod;

    return (OK);
    }


#ifdef  INCLUDE_TIMESTAMP

/*******************************************************************************
*
* sysTimestampConnect - connect a user routine to a timestamp timer interrupt
*
* This routine specifies the user interrupt routine to be called at each
* timestamp timer interrupt. In this case, however, the timestamp timer
* interrupt is not used since the on-chip timer is also used by the system
* clock.
*
* RETURNS: OK, or ERROR if sysTimestampInt() interrupt handler is not used.
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
* sysTimestampEnable - initialize and enable a timestamp timer
*
* This routine disables the timestamp timer interrupt and initializes the
* counter registers.  If the timestamp timer is already running, this routine
* merely resets the timer counter.
*
* This routine does not intialize the timer rate.  The rate of the timestamp
* timer should be set explicitly in the BSP by sysHwInit().
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be enabled.
*/

STATUS sysTimestampEnable (void)
   {
   if (sysTimestampRunning)
      {
      return (OK);
      }

   if (!sysClkRunning)
      return (ERROR);

   sysTimestampRunning = TRUE;

   return (OK);
   }

/*******************************************************************************
*
* sysTimestampDisable - disable a timestamp timer
*
* This routine disables the timestamp timer.  Interrupts are not disabled;
* however, because the tick counter does not increment after the timestamp
* timer is disabled, interrupts no longer are generated.
*
* RETURNS: OK, or ERROR if the timestamp timer cannot be disabled.
*/

STATUS sysTimestampDisable (void)
    {
    if (sysTimestampRunning)
        sysTimestampRunning = FALSE;

    return (OK);
    }

/*******************************************************************************
*
* sysTimestampPeriod - get a timestamp timer period
*
* This routine specifies the period of the timestamp timer, in ticks.
* The period, or terminal count, is the number of ticks to which the timestamp
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

    return (pitCountVal);
    }

/*******************************************************************************
*
* sysTimestampFreq - get a timestamp timer clock frequency
*
* This routine specifies the frequency of the timer clock, in ticks per second.
* The rate of the timestamp timer should be set explicitly in the BSP
* by sysHwInit().
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    return (sysBusClkFrequency);
    }

/*******************************************************************************
*
* sysTimestamp - get a timestamp timer tick count
*
* This routine returns the current value of the timestamp timer tick counter.
* The tick count can be converted to seconds by dividing by the return of
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
    return (pitCountVal - (UINT32) vxPitGet());
    }

/*******************************************************************************
*
* sysTimestampLock - lock an interrupt and get a timestamp timer tick count
*
* This routine locks interrupts when stop the tick counter must be stopped
* in order to read it or when two independent counters must be read.
* It then returns the current value of the timestamp timer tick counter.
*
* The tick count can be converted to seconds by dividing by the return of
* sysTimestampFreq().
*
* If interrupts are already locked, sysTimestamp() should be
* used instead.
*
* RETURNS: The current timestamp timer tick count.
*
* SEE ALSO: sysTimestamp()
*/

UINT32 sysTimestampLock (void)
    {
    UINT32 currentPitValue;
    int oldLevel;

    oldLevel = intLock ();                              /* LOCK INTERRUPT */
    currentPitValue = vxPitGet();
    intUnlock (oldLevel);                               /* UNLOCK INTERRUPT */

    return (pitCountVal - currentPitValue);
    }

#endif  /* INCLUDE_TIMESTAMP */
