/* sa1100Timer.c - Digital Semiconductor SA-1100 timer library */

/* Copyright 1997-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01j,14oct01,dat  added #includes to stand alone
01i,22feb01,jpd  made auxClk support dependent on INCLUDE_AUX_CLK (SPR #64165).
01h,16nov00,sbs  corrected docs for vxWorks AE.
01g,17apr00,jpd  made sysClkInt, sysAuxClkInt functions global.
01f,29mar00,dat  removed sysHwInit2()
01e,05jan00,jpd  fixed warnings from new definition of NULL. Comments changes.
01d,23apr98,cdp  updated comment about reload correction value.
01c,22apr98,cdp  lock ints around reload of match register (preemptive ints);
		 add reload correction.
01b,19mar98,cdp  removed unused variable in sysTimestampInt; fix typo in docn.
01a,10dec97,cdp  created from 01f of ambaTimer.c.
*/

/*
DESCRIPTION
This library contains routines to manipulate the OS timer functions of
the SA-1100 with a mostly board-independent interface. This driver
provides 3 main functions, system clock support, auxiliary clock
support, and timestamp timer support.  The timestamp function is always
conditional upon the INCLUDE_TIMESTAMP component.

The SA-1100 provides a free-running 32-bit counter, which counts up
from zero, and four 32-bit match registers each of which can cause an
interrupt when its value matches the value of the counter.

The SA-1100 also provides a watchdog timer which is not used by VxWorks.

The SA-1100 timer registers are described below under the symbolic
names used herein.

SA1100_TIMER_CSR_OSCR (read/write): this is the "OS Timer Count
Register" (OSCR) described in the SA-1100 Data Sheet.  This register,
which can be read or written at any time, increments at 3.6864 MHz.

SA1100_TIMER_CSR_OSMR[0..3] (read/write): these are the "OS Timer Match
Registers" (OSMR[0..3]) described in the SA-1100 Data Sheet.  These
registers, which can be read or written at any time, are compared with
the counter every tick.  For each match register whose contents match
the counter, the corresponding event bit in the status register is
set.  Match registers 0, 1 and 2 are used for the system, auxiliary and
timestamp clocks respectively.

SA1100_TIMER_CSR_OSSR (read/write): this is the "OS Timer Status
Register" (OSSR) described in the SA-1100 Data Sheet.  When this
register is read, each data bit that is set (1) indicates a match
register that matches the counter register; unused bits read as 0.
When this register is written, each data bit that is set (1) clears the
corresponding timer match event;  bits that are clear (0) have no
effect.

SA1100_TIMER_CSR_OIER (read/write): this is the "OS Timer Interrupt
Enable Register" (OIER) described in the SA-1100 Data Sheet.  When this
register is written, each data bit that is set (1) enables the
interrupt when the corresponding match register matches the counter;
data bits that are clear (0) do not clear the corresponding interrupt
if the interrupt is already asserted.  When this register is read, each
data bit that is set (1) indicates that the corresponding timer match
interrupt is enabled; each data bit that is clear (0) indicates that
the corresponding timer match interrupt is disabled.

Note that the timestamp facility is provided by OSMR[2] and so is
independent of the system clock.

This driver assumes that the chip is memory-mapped and does direct
memory access to the registers which are assumed to be 32 bits wide.
If a different access method is needed, the BSP can redefine the parameters
SA1100_TIMER_REG_READ(addr,result) and SA1100_TIMER_REG_WRITE(addr,data).

The parameters SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines.  The following parameters must also be defined:

.CS
 SYS_TIMER_CLK			/@ frequency of clock feeding SYS_CLK timer @/
 AUX_TIMER_CLK			/@ frequency of clock feeding AUX_CLK @/
 TIMESTAMP_TIMER_CLK		/@ frequency of clock feeding TIMESTAMP_CLK @/
 SYS_TIMER_INT_LVL		/@ interrupt level for sys Clk @/
 AUX_TIMER_INT_LVL		/@ interrupt level for aux Clk @/
 TIMESTAMP_TIMER_INT_LVL	/@ interrupt level for timestamp Clk @/
 SA1100_TIMER_CSR_OSCR		/@ addresses of timer registers @/
 SA1100_TIMER_CSR_OSMR_0	/@ "" @/
 SA1100_TIMER_CSR_OSMR_1	/@ "" @/
 SA1100_TIMER_CSR_OSMR_2	/@ "" @/
 SA1100_TIMER_CSR_OSSR		/@ "" @/
 SA1100_TIMER_CSR_OIER		/@ "" @/
.CE

The following may also be defined, if required:
.CS
 SA1100_TIMER_REG_READ(reg, data)	/@ read a timer register @/
 SA1100_TIMER_REG_WRITE(reg, data)	/@ write ... @/
 SA1100_TIMER_INT_ENABLE(level)		/@ enable an interrupt @/
 SA1100_TIMER_INT_DISABLE(level)	/@ disable an interrpt @/
.CE

BSP
Apart from defining such parameters described above as are needed, the BSP
must connect the interrupt handlers (typically in sysHwInit2()).
e.g.

.CS
    /@ connect sys clock interrupt and auxiliary clock interrupt @/

    intConnect (INUM_TO_IVEC (INT_VEC...), sysClkInt, 0);
    @ifdef INCLUDE_AUX_CLK
    intConnect (INUM_TO_IVEC (INT_VEC...), sysAuxClkInt, 0);
    @endif
.CE

INCLUDES:
sa1100Timer.h
timestampDev.h

SEE ALSO:
.I "Digital StrongARM SA-1100 Portable Communications Microcontroller, Data
Sheet,"
*/


/* includes */

#include "vxWorks.h"
#include "sysLib.h"
#include "intLib.h"
#include "drv/timer/sa1100Timer.h"
#include "drv/timer/timestampDev.h"


/* defines */

#ifndef SA1100_TIMER_REG_READ
#define SA1100_TIMER_REG_READ(reg, result) \
	((result) = *(volatile UINT32 *)(reg))
#endif /* SA1100_TIMER_REG_READ */

#ifndef SA1100_TIMER_REG_WRITE
#define SA1100_TIMER_REG_WRITE(reg, data) \
	(*((volatile UINT32 *)(reg)) = (data))
#endif /* SA1100_TIMER_REG_WRITE */

#ifndef SA1100_TIMER_INT_ENABLE
#define SA1100_TIMER_INT_ENABLE(level) intEnable(level)
#endif

#ifndef SA1100_TIMER_INT_DISABLE
#define SA1100_TIMER_INT_DISABLE(level) intDisable(level)
#endif

/*
 * A correction to account for average reload delays. This is necessary
 * because the timer hardware does not reload itself and so any delays
 * caused by interrupt latency etc. will stretch the gap between ticks.
 * The value chosen ensures that the timer is accurate enough to pass the
 * BSP Validation Suite on a 200MHz Brutus board.  If required, the
 * reload code could be changed to read both counter and match registers
 * and work out a suitable match register reload value based on the
 * difference between the two.
 */

#ifndef SA1100_TIMER_RELOAD_CORRECTION
#define SA1100_TIMER_RELOAD_CORRECTION 6
#endif


/* locals */

LOCAL FUNCPTR sysClkRoutine	= NULL; /* routine to call on clock interrupt */
LOCAL int sysClkArg		= 0;	/* its argument */
LOCAL int sysClkRunning		= FALSE;
LOCAL int sysClkConnected	= FALSE;
LOCAL int sysClkTicksPerSecond	= 60;
LOCAL UINT32 sysClkTicks;		/* ticks of counter per interrupt */

#ifdef INCLUDE_AUX_CLK
LOCAL FUNCPTR sysAuxClkRoutine	= NULL;
LOCAL int sysAuxClkArg		= 0;
LOCAL int sysAuxClkRunning	= FALSE;
LOCAL int sysAuxClkTicksPerSecond = 100;
LOCAL UINT32 sysAuxClkTicks;		/* ticks of counter per interrupt */
#endif /* INCLUDE_AUX_CLK */

#ifdef	INCLUDE_TIMESTAMP
LOCAL BOOL	sysTimestampRunning 	= FALSE; /* timestamp running flag */
LOCAL FUNCPTR	sysTimestampRoutine	= NULL;	 /* routine to call on intr */
LOCAL int	sysTimestampArg		= 0;	 /* arg for routine */
#endif /* INCLUDE_TIMESTAMP */

#if !defined(SYS_CLK_RATE_MIN)		|| \
    !defined(SYS_CLK_RATE_MAX)		|| \
    !defined(SYS_TIMER_CLK)		|| \
    !defined(TIMESTAMP_TIMER_CLK)	|| \
    !defined(SYS_TIMER_INT_LVL)		|| \
    !defined(TIMESTAMP_TIMER_INT_LVL)	|| \
    !defined(SA1100_TIMER_CSR_OSCR)	|| \
    !defined(SA1100_TIMER_CSR_OSMR_0)	|| \
    !defined(SA1100_TIMER_CSR_OSMR_1)	|| \
    !defined(SA1100_TIMER_CSR_OSMR_2)	|| \
    !defined(SA1100_TIMER_CSR_OSSR)	|| \
    !defined(SA1100_TIMER_CSR_OIER)
#   error missing SA-1100 timer definitions
#endif

#ifdef INCLUDE_AUX_CLK
#if !defined(AUX_CLK_RATE_MIN)		|| \
    !defined(AUX_CLK_RATE_MAX)		|| \
    !defined(AUX_TIMER_CLK)		|| \
    !defined(AUX_TIMER_INT_LVL)
#   error missing SA-1100 Aux timer definitions
#endif
#endif /* INCLUDE_AUX_CLK */

/*******************************************************************************
*
* sysClkInt - interrupt level processing for system clock
*
* This routine handles the system clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysClkConnect().
*
* RETURNS: N/A.
*/

void sysClkInt (void)
    {
    UINT32 tc;
    int key;


    /* clear interrupt */

    SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSSR, 1);


    /*
     * Load the match register with a new value calculated by
     * adding the ticks per interrupt to the current value of the
     * counter register.  Note that this may wraparound to a value
     * less than the current counter value but that's OK.
     */

    key = intLock ();

    SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OSCR, tc);
    tc += sysClkTicks;
    SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSMR_0, tc);


    /* read the match register to work around the bug in pre 2.1 silicon */

    SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OSMR_0, tc);

    intUnlock (key);

    /* If any routine attached via sysClkConnect(), call it */

    if (sysClkRoutine != NULL)
	(* sysClkRoutine) (sysClkArg);
    }

/*******************************************************************************
*
* sysClkConnect - connect a routine to the system clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* clock interrupt.  It does not enable system clock interrupts.
* Normally it is called from usrRoot() in usrConfig.c to connect
* usrClock() to the system clock interrupt.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), usrClock(), sysClkEnable()
*/

STATUS sysClkConnect
    (
    FUNCPTR routine,	/* routine to be called at each clock interrupt */
    int arg		/* argument with which to call routine */
    )
    {
#ifdef _WRS_VXWORKS_5_X
    if (sysClkConnected == FALSE)
    	{
    	sysHwInit2 ();	/* VxAE does this in prjConfig */
    	}
#endif

    sysClkConnected = TRUE;
    sysClkRoutine = NULL; /* ensure routine not called with wrong arg */
    sysClkArg	  = arg;
    sysClkRoutine = routine;

    return OK;
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
    UINT32 oier;
    int key;

    if (sysClkRunning)
	{
	/* disable timer interrupt in the timer */

	key = intLock ();

	SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OIER, oier);
	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OIER, oier & ~(1 << 0));

	intUnlock (key);


	/* clear any pending interrupt */

	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSSR, 1 << 0);


	/* disable timer interrupt in the interrupt controller */

	SA1100_TIMER_INT_DISABLE (SYS_TIMER_INT_LVL);

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
    UINT32 tc, oier;
    int key;

    if (!sysClkRunning)
	{
	/*
	 * Calculate the number of ticks of the timer clock that this
	 * period requires.  Do this once, here, so that the timer interrupt
	 * routine does not need to perform a division.
	 */
	sysClkTicks = (SYS_TIMER_CLK / sysClkTicksPerSecond) -
						SA1100_TIMER_RELOAD_CORRECTION;


	/* clear any pending interrupt */

	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSSR, 1 << 0);


	/*
	 * Load the match register with a new value calculated by
	 * adding the ticks per interrupt to the current value of the
	 * counter register.  Note that this may wraparound to a value
	 * less than the current counter value but that's OK.
	 */

	key = intLock ();

	SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OSCR, tc);
	tc += sysClkTicks;
	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSMR_0, tc);


	/* read the match register to work around the bug in per 2.1 silicon */

	SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OSMR_0, tc);


	/* enable interrupt in timer */

	SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OIER, oier);
	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OIER, oier | (1 << 0));

	intUnlock (key);


	/* enable clock interrupt in interrupt controller */

	SA1100_TIMER_INT_ENABLE (SYS_TIMER_INT_LVL);

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
* SEE ALSO: sysClkRateSet(), sysClkEnable()
*/

int sysClkRateGet (void)
    {
    return sysClkTicksPerSecond;
    }

/*******************************************************************************
*
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.  It does not
* enable system clock interrupts unilaterally, but if the system clock is
* currently enabled, the clock is disabled and then re-enabled with the new
* rate.  Normally it is called by usrRoot() in usrConfig.c.
*
* RETURNS:
* OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysClkRateGet(), sysClkEnable()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < SYS_CLK_RATE_MIN || ticksPerSecond > SYS_CLK_RATE_MAX)
	return ERROR;

    sysClkTicksPerSecond = ticksPerSecond;

    if (sysClkRunning)
	{
	sysClkDisable ();
	sysClkEnable ();
	}

    return OK;
    }

#ifdef INCLUDE_AUX_CLK
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
    UINT32 tc;
    int key;

    /* clear interrupt */

    SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSSR, 1 << 1);


    /*
     * Load the match register with a new value calculated by
     * adding the ticks per interrupt to the current value of the
     * counter register.  Note that this may wraparound to a value
     * less than the current counter value but that's OK.
     */

    key = intLock ();

    SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OSCR, tc);
    tc += sysAuxClkTicks;
    SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSMR_1, tc);

    intUnlock (key);


    /* If any routine attached via sysAuxClkConnect(), call it */

    if (sysAuxClkRoutine != NULL)
	(*sysAuxClkRoutine) (sysAuxClkArg);
    }

/*******************************************************************************
*
* sysAuxClkConnect - connect a routine to the auxiliary clock interrupt
*
* This routine specifies the interrupt service routine to be called at each
* auxiliary clock interrupt.  It also connects the clock error interrupt
* service routine.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysAuxClkEnable()
*/

STATUS sysAuxClkConnect
    (
    FUNCPTR routine,    /* routine called at each aux clock interrupt */
    int arg             /* argument with which to call routine        */
    )
    {
    sysAuxClkRoutine = NULL;	/* ensure routine not called with wrong arg */
    sysAuxClkArg	= arg;
    sysAuxClkRoutine	= routine;

    return OK;
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
    UINT32 oier;
    int key;

    if (sysAuxClkRunning)
        {
	/* disable timer interrupt in the timer */

	key = intLock ();

	SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OIER, oier);
	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OIER, oier & ~(1 << 1));

	intUnlock (key);


	/* clear any pending interrupt */

	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSSR, 1 << 1);


	/* disable timer interrupt in the interrupt controller */

	SA1100_TIMER_INT_DISABLE (AUX_TIMER_INT_LVL);

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
* SEE ALSO: sysAuxClkDisable()
*/

void sysAuxClkEnable (void)
    {
    UINT32 tc, oier;
    int key;

    if (!sysAuxClkRunning)
	{
	/*
	 * Calculate the number of ticks of the timer clock that this
	 * period requires.  Do this once, here, so that the timer interrupt
	 * routine does not need to perform a division.
	 */
	sysAuxClkTicks = (AUX_TIMER_CLK / sysAuxClkTicksPerSecond) -
					    SA1100_TIMER_RELOAD_CORRECTION;


	/* clear any pending interrupt */

	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSSR, 1 << 1);


	/*
	 * Load the match register with a new value calculated by
	 * adding the ticks per interrupt to the current value of the
	 * counter register.  Note that this may wraparound to a value
	 * less than the current counter value but that's OK.
	 */

	key = intLock ();

	SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OSCR, tc);
	tc += sysAuxClkTicks;
	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSMR_1, tc);


	/* enable interrupt in timer */

	SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OIER, oier);
	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OIER, oier | (1 << 1));

	intUnlock (key);


	/* enable clock interrupt in interrupt controller */

	SA1100_TIMER_INT_ENABLE (AUX_TIMER_INT_LVL);

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
    return sysAuxClkTicksPerSecond;
    }

/*******************************************************************************
*
* sysAuxClkRateSet - set the auxiliary clock rate
*
* This routine sets the interrupt rate of the auxiliary clock.  It does
* not enable auxiliary clock interrupts unilaterally, but if the
* auxiliary clock is currently enabled, the clock is disabled and then
* re-enabled with the new rate.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond	    /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
	return ERROR;

    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return OK;
    }

#endif /* INCLUDE_AUX_CLK */

#ifdef  INCLUDE_TIMESTAMP

/*******************************************************************************
*
* sysTimestampInt - timestamp timer interrupt handler
*
* This routine handles the timestamp timer interrupt.  A user routine is
* called, if one was connected by sysTimestampConnect().
*
* RETURNS: N/A
*
* SEE ALSO: sysTimestampConnect()
*/

void sysTimestampInt (void)
    {
    /* clear interrupt */

    SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSSR, 1 << 2);


    /*
     * do not reload the match register - another interrupt will be
     * generated next time the counter hits the current value
     *
     * If any routine attached via sysTimestampConnect(), call it
     */

    if (sysTimestampRunning && sysTimestampRoutine != NULL)
	(*sysTimestampRoutine)(sysTimestampArg);
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
    int arg     	/* argument with which to call routine */
    )
    {
    sysTimestampRoutine = NULL;
    sysTimestampArg = arg;
    sysTimestampRoutine = routine;

    return OK;
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
* RETURNS: OK, or ERROR if hardware cannot be enabled.
*/

STATUS sysTimestampEnable (void)
    {
    static BOOL connected = FALSE;
    UINT32 tc, oier;
    int key;

    if (!connected)
	{
	/* connect sysTimestampInt to the appropriate interrupt */

	intConnect (INUM_TO_IVEC (INT_LVL_TIMER_2), sysTimestampInt, 0);
	connected = TRUE;
	}

    if (!sysTimestampRunning)
	{
	/*
	 * Set match register (OSMR) to (current counter value) (OSCR)
	 * minus one so that an interrupt will be generated when the
	 * counter wraps around to that value but there will not be an
	 * immediate interrupt. This means that the first interrupt will
	 * arrive one tick too soon but this inaccuracy should be
	 * tolerable.
	 */

	key = intLock();

	SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OSCR, tc);
	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSMR_2, tc - 1);


	/* clear any pending interrupt */

	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSSR, 1 << 2);


	/* enable interrupt in timer */

	SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OIER, oier);
	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OIER, oier | (1 << 2));

	intUnlock (key);


	/* enable clock interrupt in interrupt controller */

	SA1100_TIMER_INT_ENABLE (TIMESTAMP_TIMER_INT_LVL);

	sysTimestampRunning = TRUE;
	}

    return OK;
    }

/*******************************************************************************
*
* sysTimestampDisable - disable the timestamp timer
*
* This routine disables the timestamp timer.  Interrupts are not disabled,
* although the tick counter will not increment after the timestamp timer
* is disabled, thus interrupts will no longer be generated.
*
* RETURNS: OK, or ERROR if timer cannot be disabled.
*/

STATUS sysTimestampDisable (void)
    {
    UINT32 oier;
    int key;

    if (sysTimestampRunning)
	{
	/* disable timer interrupt in the timer */

	key = intLock ();

	SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OIER, oier);
	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OIER, oier & ~(1 << 2));

	intUnlock (key);


	/* clear any pending interrupt */

	SA1100_TIMER_REG_WRITE (SA1100_TIMER_CSR_OSSR, 1 << 2);


	/* disable timer interrupt in the interrupt controller */

	SA1100_TIMER_INT_DISABLE (TIMESTAMP_TIMER_INT_LVL);

        sysTimestampRunning = FALSE;
	}

    return OK;
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
    return 0xFFFFFFFF;		/* max value of 32-bit counter */
    }

/*******************************************************************************
*
* sysTimestampFreq - get the timestamp timer clock frequency
*
* This routine returns the frequency of the timer clock, in ticks per second.
*
* RETURNS: The timestamp timer clock frequency, in ticks per second.
*/

UINT32 sysTimestampFreq (void)
    {
    return TIMESTAMP_TIMER_CLK;
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
    UINT32 count, match;

    /*
     * read count and match registers - this is effectively atomic
     * (since the match register is not reloaded on interrupt) so there is
     * no need to disable interrupts around this.
     */

    SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OSCR, count);
    SA1100_TIMER_REG_READ (SA1100_TIMER_CSR_OSMR_2, match);


    /*
     * When the timer was enabled, the match register (OSMR) was
     * initialised to the current value of the count register (OSCR) and
     * then generates an interrupt every time the two registers match.
     * The ticks since the last interrupt are thus (OSCR - OSMR).
     */

    return count - match;
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
    /* no need to disable interrupts - see sysTimestamp() */

    return sysTimestamp ();
    }

#endif  /* INCLUDE_TIMESTAMP */
