
/* sysMv64360TmrCntr.c - MV64360 Timer/Counter Library */

/* Copyright 2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01c,16sep04,cak  Replaced INCLUDE_AUXCLK with INCLUDE_AUX_CLK to match the
		 Tornado Project facility.
01b,21jun04,cak  Fixed a problem with the intConnect'ion support.
01a,14jun04,cak  Separated the timer/counter support from the auxClk 
		 support, and extended it to support all 4 timer/counters.
*/

/*
DESCRIPTION
There are a total of four identical timer/counters on the MV64360,
each 32-bits wide.  Each timer/counter can be selected to operate as
a timer or a counter.  Each timer/counter increments with every Tclk
rising edge.  The MV64360 timer/counters are driven by the CPU bus clock.

In Counter mode, the counter counts down to zero, stops, and issues
an interrupt.

In Timer mode, the timer counts down, issues an interrupt when it
reaches zero, reloads itself to the programmed value, and continues
to count.

Besides four timer/counter registers, there are three other timer
related registers:

Timer/Counter Control Register -
Bit 0 is the Timer/Counter Enable bit of timer/counter 0.  Writing
a value of "0" disables timer/counter 0, while writing a value of
"1" enables timer/counter 0.  When in timer mode, the count continues
as long as this bit is set to '1'.  Bit 1 is the Timer/Counter Mode bit,
the timer/counter 0 is in timer mode if this bit is set to '1'. Bits 8
and 9 are the enable and mode bits for timer/counter 1, bits 16 and 17
are the enable and mode bits for timer/counter 2, and bits 24 and 25 are
the enable and mode bits for timer/counter 3.

Timer/Counter Interrupt Cause Register -
Bit 0 is set to 1 upon timer 0 terminal count.  It's cleared by writing
a value of "0".  Writing a value of "1" has no effect.  Bit 1 is the 
terminal count for timer/counter 1, bit 2 is the terminal count for
timer/counter 2, and bit 3 is the terminal count for timer/counter 3.

Timer/Counter Interrupt Mask Register -
Writing a value of "0" to bit 0 disables timer/counter 0 interrupt.
Writing a value of "1" to bit 0 enables timer/counter 0 interrupt.
Bit 1 is the interrupt mask for timer/counter 1, bit 2 is the interrupt
mask for timer/counter 2, and bit 3 is the interrupt mask for 
timer/counter 3.

Keep in mind that one of the timer/counters is used for the VxWorks
auxClk, so only 3 are available for use.  In config.h, the definition
MV64360_AUXCLK_TMR is used to define which of the timer/counters is used
to implement the AuxClk, and is user-configurable.

The following routines are provided for the timer/counters:

STATUS sysMv64360TmrCntrStart (timer, seconds, mode)

This routine has three input parameters: the first is the number of the
timer, the second is the number of seconds until timer/counter expiration, 
and the third is the timer/counter mode.  The mode can be either timer (0) 
or counter (1).

STATUS sysMv64360TmrCntrDisable (timer)

This routine will disable the given timer/counter.  The only way to
restart it is to call sysMv64360TmrCntrStart() again.

Two routines are also provided for connecting and disconnecting user-defined
interrupt handlers to the timer/counter interrupts:

STATUS sysMv64360TmrCntrConnectRoutine (routine, arg, timer)
STATUS sysMv64360TmrCntrDisconnectRoutine (timer)

Before connecting a new routine be sure to disconnect the old routine by
first calling sysMv64360TmrCntrDisconnectRoutine().

The current (real-time) value of a given timer/counter can be retrieved using
the following routine:

UINT32 sysMv64360GetTmrCntrValue (timer)

If INCLUDE_SHOW_ROUTINES is defined then the following routine can be used
to display the current state of all 4 timer/counters:

STATUS sysMv64360TmrCntrShow (void)
*/

/* includes */

#include "mv64360.h"
#include "mv6100A.h"

/* defines */

#undef	MV64360_TMR_DEBUG
#define	MV64360_TMRCNTR_MODE_TMR	0
#define	MV64360_TMRCNTR_MODE_CNTR	1
#define	MV64360_MAX_TIMERS		4

/* typedefs */

/* globals */

/* locals */

LOCAL UINT32 timerRate = 0;
LOCAL FUNCPTR sysMv64360TmrCntrRoutines [4] = {NULL};
LOCAL int sysMv64360TmrCntrRoutineArgs [4] = {0x0};
LOCAL BOOL isTimerEnabled [4] = {FALSE};
LOCAL BOOL isTimerRateInitialized = FALSE;
LOCAL BOOL isIntConnected [4] = {FALSE};
#ifdef INCLUDE_SHOW_ROUTINES
    static char timerShowMsgHeader[] = { "\
    Timer/Counter   Enabled   Mode   Interrupt-Enabled   Value\r\n"};
    static char timerShowMsgEntry[] = { "         %-14d%-8s %-13s%-11s%08X \r\n"};
#endif /* INCLUDE_SHOW_ROUTINES */

/* forward declarations */

#ifdef INCLUDE_SHOW_ROUTINES
    STATUS sysMv64360TmrCntrShow (void);
#endif /* INCLUDE_SHOW_ROUTINES */
UINT32 sysMv64360GetTmrCntrValue (UINT32);
STATUS sysMv64360TmrCntrConnectRoutine (FUNCPTR, int, UINT32);
STATUS sysMv64360TmrCntrDisconnectRoutine (UINT32);
LOCAL void sysMv64360TmrCntrInt (UINT32);
LOCAL void sysMv64360TmrCntrIntConnect (UINT32);
LOCAL void sysMv64360TmrCntrIntEnable (UINT32);
LOCAL void sysMv64360TmrCntrIntDisable (UINT32);
LOCAL void sysMv64360TmrCntrInit (UINT32);
void sysMv64360AuxClkStart (UINT32, UINT32);
STATUS sysMv64360TmrCntrStart (UINT32, UINT32, UINT32);
STATUS sysMv64360TmrCntrDisable (UINT32);

#ifdef INCLUDE_SHOW_ROUTINES
/*******************************************************************************
*
* sysMv64360TmrCntrShow - display current state of all timer/counters
*
* This routine displays the current state of all of the MV64360's 
* timer/counters.
*
* RETURNS: Always OK. 
*/

STATUS sysMv64360TmrCntrShow (void)
    {
    int x;
    BOOL isEnabled;
    BOOL mode;
    BOOL intEnabled;
    UINT32 intMaskMask;
    UINT32 tmrCtrlEnableBit;
    UINT32 tmrCtrlEnableMask;
    UINT32 tmrCtrlModeBit;
    UINT32 tmrCtrlModeMask;

    printf(timerShowMsgHeader);

    for (x=0; x < MV64360_MAX_TIMERS; x++)
	{
	intMaskMask = (1 << x);
	intEnabled = (MV64360_READ32 (MV64360_REG_BASE, TMR_CNTR_INT_MASK)
		      & intMaskMask);

	tmrCtrlEnableBit = x * 8;
	tmrCtrlEnableMask = (1 << tmrCtrlEnableBit);
	isEnabled = (MV64360_READ32 (MV64360_REG_BASE, TMR_CNTR_CTRL)
		     & tmrCtrlEnableMask);

	tmrCtrlModeBit = (1 + (x * 8));
	tmrCtrlModeMask = (1 << tmrCtrlModeBit);
	mode = (MV64360_READ32 (MV64360_REG_BASE, TMR_CNTR_CTRL)
		& tmrCtrlModeMask);

        printf(
            timerShowMsgEntry,
            x,
            isEnabled ? "Y" : "N",
            mode ? "T" : "C",
            intEnabled ? "Y" : "N",
            sysMv64360GetTmrCntrValue(x)
            );
	}
    return (OK);
    }
#endif /* INCLUDE_SHOW_ROUTINES */

/*******************************************************************************
*
* sysMv64360GetTmrCntrValue - get the current value of the given timer/counter
*
* This routine will return the current value of the specified timer/counter.
*
* RETURNS: value of timer/counter.
*/

UINT32 sysMv64360GetTmrCntrValue
    (
    UINT32 timer	/* number of timer */
    )
    {
    UINT32 tmrCntrReg;

    if (timer >= MV64360_MAX_TIMERS)
	{
	printf("Timer number must be between 0 - %d\n\r", 
	       (MV64360_MAX_TIMERS - 1));
	return (ERROR);
	}

    tmrCntrReg = TMR_CNTR0 + (timer * 0x4);

    return (MV64360_READ32 (MV64360_REG_BASE, tmrCntrReg));
    }

/*******************************************************************************
*
* sysMv64360TmrCntrConnectRoutine - connect user specified interrupt handler
*
* This routine connects a user specifed interrupt handler routine to be
* called by the timer/counter's interrupt handler.
*
* RETURNS: OK if routine not already connected, ERROR otherwise.
*/

STATUS sysMv64360TmrCntrConnectRoutine
    (
    FUNCPTR routine,	/* user-defined routine */
    int arg,		/* argument */
    UINT32 timer	/* number of timer */
    )
    {
    if (timer >= MV64360_MAX_TIMERS)
	{
	printf("Timer number must be between 0 - %d\n\r", 
	       (MV64360_MAX_TIMERS - 1));
	return (ERROR);
	}

    if (sysMv64360TmrCntrRoutines[timer] == NULL)
        {
        sysMv64360TmrCntrRoutines[timer] = routine;
	sysMv64360TmrCntrRoutineArgs[timer] = arg;
        return (OK);
        }
    else
        {
        printf("Routine already connected\n");
        return (ERROR);
        }
    }

/*******************************************************************************
*
* sysMv64360TmrCntrDisconnectRoutine - disconnect user specified interrupt 
* handler
*
* This routine disconnects a user specifed interrupt handler routine 
* from the timer/counter's interrupt handler.
*
* RETURNS: OK if successfully disconnected, ERROR otherwise.
*/

STATUS sysMv64360TmrCntrDisconnectRoutine
    (
    UINT32 timer	/* number of timer */
    )
    {
    if (timer >= MV64360_MAX_TIMERS)
	{
	printf("Timer number must be between 0 - %d\n\r", 
	       (MV64360_MAX_TIMERS - 1));
	return (ERROR);
	}

    sysMv64360TmrCntrRoutines[timer] = NULL;
    sysMv64360TmrCntrRoutineArgs[timer] = 0x0;
    return (OK);
    }

/*******************************************************************************
*
* sysMv64360TmrCntrInt - timer/counter interrupt handler
*
* This routine is the interrupt handler for the mv64360 timer/counters.
*
* RETURNS: N/A
*/

LOCAL void sysMv64360TmrCntrInt
    (
    UINT32 timer	/* number of timer */
    ) 
    {
    UINT32 intCauseMask;

    intCauseMask = (1 << timer);

    /* clear timer/counter cause bit */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_INT_CAUSE,
                          ((MV64360_READ32
                          (MV64360_REG_BASE, TMR_CNTR_INT_CAUSE))
                          & ~intCauseMask));

#ifdef MV64360_TMR_DEBUG
    printf ("DEBUG: Received timer/counter interrupt for timer/counter %d\n",
	    timer);
#endif /* MV64360_TMR_DEBUG */

    /* call user-specified routine */

    if (sysMv64360TmrCntrRoutines[timer] != NULL)
        {
        (*sysMv64360TmrCntrRoutines[timer])(sysMv64360TmrCntrRoutineArgs[timer]);
        }
    }

/*******************************************************************************
*
* sysMv64360TmrCntrIntConnect - connect timer/counter interrupt
*
* This routine connects a timer/counter interrupt handler to the
* system interrupt vector table.
*
* RETURNS: N/A
*/

LOCAL void sysMv64360TmrCntrIntConnect
    (
    UINT32 timer	/* number of timer */
    )
    {
    intConnect (INUM_TO_IVEC(TMR_CNTR0_INT_VEC + timer), 
		(VOIDFUNCPTR)sysMv64360TmrCntrInt, timer);
    }

/*******************************************************************************
*
* sysMv64360TmrCntrIntEnable - enable timer/counter interrupt
*
* This routine enables a timer/counter interrupt.
*
* RETURNS: N/A
*/

LOCAL void sysMv64360TmrCntrIntEnable
    (
    UINT32 timer	/* number of timer */
    )
    {
    UINT32 intMaskMask;

    intMaskMask = (1 << timer);

    /* enable timer/counter interrupt */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_INT_MASK,
                          ((MV64360_READ32 (MV64360_REG_BASE,
                            TMR_CNTR_INT_MASK)) | intMaskMask));

    intEnable (TMR_CNTR0_INT_LVL + timer);
    }

/*******************************************************************************
*
* sysMv64360TmrCntrIntDisable - disable timer/counter interrupt
*
* This routine disables a timer/counter interrupt.
*
* RETURNS: N/A
*/

LOCAL void sysMv64360TmrCntrIntDisable
    (
    UINT32 timer	/* number of timer */
    )
    {
    UINT32 intMaskMask;

    intMaskMask = (1 << timer);

    /* disable timer/counter interrupt */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_INT_MASK,
                          ((MV64360_READ32 (MV64360_REG_BASE,
                            TMR_CNTR_INT_MASK)) & ~intMaskMask));

    intDisable (TMR_CNTR0_INT_LVL + timer);
    }

/*******************************************************************************
*
* sysMv64360TmrCntrInit - timer/counter initialization routine
*
* This routine initializes an mv64360 timer/counter.
*
* RETURNS: N/A.
*/

LOCAL void sysMv64360TmrCntrInit
    (
    UINT32 timer	/* number of timer */
    )
    {
    UINT32 tmrCtrlEnableBit;
    UINT32 tmrCtrlEnableMask;

    tmrCtrlEnableBit = timer * 8;
    tmrCtrlEnableMask = (1 << tmrCtrlEnableBit);

    if (!isTimerRateInitialized)
	{
	timerRate = CPU_BUS_SPD;
	isTimerRateInitialized = TRUE;
	}

    /* disable timer/counter */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL,
                          ((MV64360_READ32 (MV64360_REG_BASE, TMR_CNTR_CTRL)) &
                          ~tmrCtrlEnableMask));

    /* connect timer/counter interrupt */

    if (!isIntConnected[timer])
	{
	sysMv64360TmrCntrIntConnect(timer);
	isIntConnected[timer] = TRUE;
	}

#if (defined MV64360_TMR_DEBUG) && (defined INCLUDE_SHOW_ROUTINES)
    sysMv64360TmrCntrConnectRoutine((FUNCPTR)sysMv64360TmrCntrShow, 0x0, timer);
#endif /* MV64360_TMR_DEBUG, INCLUDE_SHOW_ROUTINES */

    /* enabled timer/counter interrupt */

    sysMv64360TmrCntrIntEnable(timer);

    /* flag the timer as enabled */

    isTimerEnabled[timer] = TRUE;
    }

/*******************************************************************************
*
* sysMv64360AuxClkStart - start an mv64360 timer/counter for use as the auxClk
*
* This routine configures, and starts, an mv64360 timer/counter to be used 
* as the VxWorks auxClk.
*
* RETURNS: N/A 
*/

void sysMv64360AuxClkStart
    (
    UINT32 timer,		/* number of timer */
    UINT32 ticksPerSecond	/* number of ticks per second for auxClk */
    )
    {
    UINT32 numCycles = 0x0;
    UINT32 tmrCtrlModeBit;
    UINT32 tmrCtrlModeMask;
    UINT32 tmrCntrReg;
    UINT32 tmrCtrlEnableBit;
    UINT32 tmrCtrlEnableMask;

    /* initialize the timer */

    if (!isTimerEnabled[timer])
	{
	sysMv64360TmrCntrInit(timer);
	}

    tmrCtrlModeBit = (1 + (timer * 8));
    tmrCtrlModeMask = (1 << tmrCtrlModeBit);
    tmrCntrReg = TMR_CNTR0 + (timer * 0x4);
    tmrCtrlEnableBit = timer * 8;
    tmrCtrlEnableMask = (1 << tmrCtrlEnableBit);

    /* set timer/counter in timer mode */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL,
	((MV64360_READ32 (MV64360_REG_BASE, TMR_CNTR_CTRL)) |
	tmrCtrlModeMask));

    numCycles = (timerRate/ticksPerSecond);

    /* write value to count from */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, tmrCntrReg, numCycles);

    /* enable timer/counter */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL,
                          ((MV64360_READ32 (MV64360_REG_BASE,
                            TMR_CNTR_CTRL)) | tmrCtrlEnableMask));
    }

/*******************************************************************************
*
* sysMv64360TmrCntrStart - start an mv64360 timer/counter
*
* This routine configures (based on user input), and starts, an mv64360
* timer/counter.
*
* RETURNS: OK if timer/counter successfully started, ERROR otherwise.
*/

STATUS sysMv64360TmrCntrStart
    (
    UINT32 timer,	/* number of timer */
    UINT32 seconds,     /* number of seconds to timer/counter expiration */
    UINT32 mode         /* timer/counter mode, either timer or counter */
    )
    {
    int maxSeconds = 0x0;
    UINT32 numCycles = 0x0;
    UINT32 tmrCtrlModeBit;
    UINT32 tmrCtrlModeMask;
    UINT32 tmrCntrReg;
    UINT32 tmrCtrlEnableBit;
    UINT32 tmrCtrlEnableMask;

    if (timer >= MV64360_MAX_TIMERS)
	{
	printf("Timer number must be between 0 - %d\n\r", 
	       (MV64360_MAX_TIMERS - 1));
	return (ERROR);
	}

#ifdef INCLUDE_AUX_CLK
    if (timer == MV64360_AUXCLK_TMR)
	{
	printf("Timer %d is used for the AuxClock\n\r", timer);
	return (ERROR);
	}
#endif /* INCLUDE_AUX_CLK */

    /* initialize the timer */

    if (!isTimerEnabled[timer])
	{
	sysMv64360TmrCntrInit(timer);
	}

    if (seconds == 0x0)
        {
        printf("Timer/counter duration must be greater than 0\n");
        return (ERROR);
        }

    tmrCtrlModeBit = (1 + (timer * 8));
    tmrCtrlModeMask = (1 << tmrCtrlModeBit);
    tmrCntrReg = TMR_CNTR0 + (timer * 0x4);
    tmrCtrlEnableBit = timer * 8;
    tmrCtrlEnableMask = (1 << tmrCtrlEnableBit);

    /*
     * Calculate the maximum duration of the timer/counter (in seconds)
     * based on the current timer frequency, which happens to
     * be the same as the CPU bus frequency.  Use this to validate the
     * duration requested by the user.
     */

    maxSeconds = 0xFFFFFFFF/timerRate;

    if (seconds > maxSeconds)
        {
        printf("Timer/counter cannot be configured for %d seconds - duration too long for the current bus frequency of %d!\n", seconds, timerRate);
        return (ERROR);
        }
    else
        {

	if (mode == MV64360_TMRCNTR_MODE_TMR)
	    {

	    /* set timer/counter in timer mode */

	    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL,
		((MV64360_READ32 (MV64360_REG_BASE, TMR_CNTR_CTRL)) |
		tmrCtrlModeMask));
	    }
	else if (mode == MV64360_TMRCNTR_MODE_CNTR)
	    {

	    /* set timer/counter in counter mode */

	    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL,
		((MV64360_READ32 (MV64360_REG_BASE, TMR_CNTR_CTRL)) &
		~tmrCtrlModeMask));

	    }
	else
	    {
	    printf("Valid modes are:\n");
	    printf(" 0 - timer mode\n");
	    printf(" 1 - counter mode\n");
	    printf("sysMv64360TmrCntrStart(<timer>,<seconds>,<mode>)\n");
	    return (ERROR);
	    }

        numCycles = (seconds * timerRate);

        /* write value to count from */

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, tmrCntrReg, numCycles);

        /* enable timer/counter */

        MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL,
                              ((MV64360_READ32 (MV64360_REG_BASE,
                                TMR_CNTR_CTRL)) | tmrCtrlEnableMask));
	}
    return (OK);
    }

/*******************************************************************************
*
* sysMv64360TmrCntrDisable - Disable an mv64360 Timer/counter
*
* This routine will disable an mv64360 timer/counter.
*
* RETURNS: OK if successfully disabled, ERROR otherwise.
*/

STATUS sysMv64360TmrCntrDisable
    (
    UINT32 timer	/* number of timer */
    )
    {
    UINT32 tmrCtrlEnableBit;
    UINT32 tmrCtrlEnableMask;
    UINT32 tmrCntrReg;

    tmrCntrReg = TMR_CNTR0 + (timer * 0x4);

    if (timer >= MV64360_MAX_TIMERS)
        {
        printf("Timer number must be between 0 - %d\n\r", 
	       (MV64360_MAX_TIMERS - 1));
        return (ERROR);
        }

    tmrCtrlEnableBit = timer * 8;
    tmrCtrlEnableMask = (1 << tmrCtrlEnableBit);

    /* disable timer/counter */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, TMR_CNTR_CTRL,
	((MV64360_READ32 (MV64360_REG_BASE, TMR_CNTR_CTRL)) &
	~tmrCtrlEnableMask));

    /* disable timer/counter interrupt */

    sysMv64360TmrCntrIntDisable(timer);

    /* clear timer/counter */

    MV64360_WRITE32_PUSH (MV64360_REG_BASE, tmrCntrReg, 0x00000000);

    /* flag the timer as disabled */

    isTimerEnabled[timer] = FALSE;

    return (OK);
    }
