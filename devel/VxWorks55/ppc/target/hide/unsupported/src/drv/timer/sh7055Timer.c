/* sh7055Timer.c - SH7055 on-chip Timer library */

/* Copyright 1999 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,23sep99,rip  CMT_TICK_FREQ computation wrong, clocks ran at half-speed
01a,25aug99,gwn  written based on sh7040Timer.c 01f.
*/

/*
DESCRIPTION
This library contains routines to manipulate the timer functions on the
evb7055f Compare Match Timer (CMT) and Advanced Timer Unit (ATU II) to
implement the system, auxiliary and timestamp clocks used by vxWorks.

The system and auxiliary clocks require periodic interval interrupts
to be generated with minimum and maximum rates less than and greater
than 60 Hz, respectively. This function is most easily implemented with
the Compare Match Timer. The input clock frequency can be prescaled
by 8, 32, 128 or 512. The higher the prescale factor the lower the
minimum frequency, although this is achieved at the expense of accuracy.
The CMT counts clock transitions, comparing the running total against a
16-bit pre-loaded register.

The system clock uses CMT0 and the auxiliary clock CMT1.

The macros SYS_CLK_RATE_MIN, SYS_CLK_RATE_MAX, AUX_CLK_RATE_MIN, and
AUX_CLK_RATE_MAX must be defined to provide parameter checking for the
sys[Aux]ClkRateSet() routines. Note that the prescale factor for the
input clocks to the CMT affects the minimum and maximum rates that can
be set.

The timestamp is implemented with the SH7055 ATU-II, using the 32-bit free
running counter TCNT0. An interrupt is generated on rollover. The timestamp
clock is prescaled by 32, giving a resolution of 0.8us for a 40 MHz system
clock, and a rollover every 57 minutes. Note that this prescale factor is
shared by some of the other on-chip counters, so exercise caution when
changing this value.

To include the timestamp timer facility, the macro INCLUDE_TIMESTAMP must be
defined.  The macro TIMESTAMP_LEVEL must be defined to provide the timestamp
timer's interrupt level.
*/

#include "drv/timer/timerDev.h"
#include "arch/sh/sh7055.h"

/* Locals */

/* 01b:  CMT runs off peripheral clock, which runs at SYS_CPU_FREQ / 2 
 * not SYS_CPU_FREQ.
 */ 
#define CMT_TICK_FREQ           ((SYS_CPU_FREQ / 2) / 32)

LOCAL int       sysClkTicksPerSecond    = 60;
LOCAL BOOL      sysClkRunning           = FALSE;
LOCAL FUNCPTR   sysClkRoutine           = NULL;
LOCAL int       sysClkArg               = NULL;
LOCAL BOOL      sysClkConnected         = FALSE;

LOCAL int       sysAuxClkTicksPerSecond = 60;
LOCAL BOOL      sysAuxClkRunning        = FALSE;
LOCAL FUNCPTR   sysAuxClkRoutine        = NULL;
LOCAL int       sysAuxClkArg            = NULL;
LOCAL BOOL      sysAuxClkConnected      = FALSE;

#ifdef  INCLUDE_TIMESTAMP
LOCAL BOOL      sysTimestampRunning     = FALSE;  /* running flag */
LOCAL FUNCPTR   sysTimestampRoutine     = NULL;   /* user rollover routine */
LOCAL int       sysTimestampArg         = NULL;   /* arg to user routine */
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
    UINT16 stat;

    if ((stat = *CMT_CMCSR_0) & CMT_CMCSR_CMF)
        {
        *CMT_CMCSR_0 = (UINT16)(stat & ~CMT_CMCSR_CMF); /* reset status */

        if (sysClkRoutine != NULL)
            (*sysClkRoutine)(sysClkArg);
        }
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
    FUNCPTR routine,    /* routine called at each system clock interrupt */
    int arg             /* argument with which to call routine           */
    )
    {
    sysHwInit2 ();      /* XXX for now -- needs to be in usrConfig.c */

    sysClkDisable ();

    *CMT_CMCSR_0 = CMT_CMCSR_CKS_32;

    if (intConnect (IV_CMT0_CMTI, sysClkInt, NULL) != OK)
        return (ERROR);

    *INTC_IPRJ = (UINT16)((*INTC_IPRJ & 0xf0ff) | ((INT_LVL_SYSCLK & 0xf) << 8));

    sysClkConnected = TRUE;
    sysClkArg       = arg;
    sysClkRoutine   = routine;

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
        *CMT_CMCSR_0 = (UINT16) 0x0000;                 /* disable interrupt */
        *CMT_CMSTR  &= (UINT16) ~CMT_CMSTR_STR0_START;  /* stop counter */

        sysClkRunning = FALSE;
        }
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
        *CMT_CMCSR_0 = (UINT16) 0x0000;                 /* disable interrupt */
        *CMT_CMSTR  &= (UINT16) ~CMT_CMSTR_STR0_START;  /* stop counter */
        *CMT_CMCNT_0 = (UINT16) 0;                      /* clear counter */

        /* load compare register with the number of micro seconds */

        *CMT_CMCOR_0 = (UINT16)(CMT_TICK_FREQ / sysClkTicksPerSecond - 1);

        *CMT_CMCSR_0 = (UINT16)(CMT_CMCSR_CMIE | CMT_CMCSR_CKS_32);
        *CMT_CMSTR  |= (UINT16) CMT_CMSTR_STR0_START;   /* start count */

        sysClkRunning = TRUE;
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
* SEE ALSO: sysClkEnable, sysClkRateSet()
*/

int sysClkRateGet (void)
    {
    return (sysClkTicksPerSecond);
    }

/*******************************************************************************
* sysClkRateSet - set the system clock rate
*
* This routine sets the interrupt rate of the system clock.
* It is called by usrRoot() in usrConfig.c.
*
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be
* set.
*
* SEE ALSO: sysClkEnable, sysClkRateGet()
*/

STATUS sysClkRateSet
    (
    int ticksPerSecond          /* number of clock interrupts per second */
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
*/

LOCAL void sysAuxClkInt (void)
    {
    UINT16 stat;

    if ((stat = *CMT_CMCSR_1) & CMT_CMCSR_CMF)
        {
        *CMT_CMCSR_1 = (UINT16)(stat & ~CMT_CMCSR_CMF); /* reset status */

        if (sysAuxClkRoutine != NULL)
            (*sysAuxClkRoutine)(sysAuxClkArg);
        }
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
    FUNCPTR routine,    /* routine called at each aux clock interrupt    */
    int arg             /* argument to auxiliary clock interrupt routine */
    )
    {
    sysAuxClkDisable ();

    if (intConnect (IV_CMT1_CMTI, sysAuxClkInt, NULL) != OK)
        return (ERROR);

    *INTC_IPRJ = (UINT16)((*INTC_IPRJ & 0xff0f) | ((INT_LVL_AUXCLK & 0xf) << 4));

    /* Set flags */
    sysAuxClkConnected = TRUE;
    sysAuxClkArg       = arg;
    sysAuxClkRoutine   = routine;

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
        *CMT_CMCSR_1 = (UINT16) 0x0000;                 /* disable interrupt */
        *CMT_CMSTR  &= (UINT16) ~CMT_CMSTR_STR1_START;  /* stop count */

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
    if (!sysAuxClkRunning)
        {
        *CMT_CMCSR_1 = (UINT16) 0x0000;                 /* disable interrupt */
        *CMT_CMSTR  &= (UINT16) ~CMT_CMSTR_STR1_START;  /* stop counter */
        *CMT_CMCNT_1 = (UINT16) 0;                      /* clear counter */

        /* load compare register with the number of micro seconds */

        *CMT_CMCOR_1 = (UINT16)(CMT_TICK_FREQ / sysAuxClkTicksPerSecond - 1);

        *CMT_CMCSR_1 = (UINT16)(CMT_CMCSR_CMIE | CMT_CMCSR_CKS_32);
        *CMT_CMSTR  |= (UINT16) CMT_CMSTR_STR1_START;   /* start count */

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
* RETURNS: OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* SEE ALSO: sysAuxClkEnable(), sysAuxClkRateGet()
*/

STATUS sysAuxClkRateSet
    (
    int ticksPerSecond          /* number of clock interrupts per second */
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
    if (*ATU_TSR0 & 0x10)       /* Bit 4: overflow. Read clears */
        {
        if (sysTimestampRoutine != NULL)
            (*sysTimestampRoutine) (sysTimestampArg);
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
    int arg             /* argument with which to call routine           */
    )
    {
    sysTimestampRoutine = routine;
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
    if (sysTimestampRunning)
        {
        *ATU_TCNT0 = (UINT32)0x00000000;        /* simply clear */
        return (OK);
        }

    *ATU_ITER0 = (UINT16)0x0000;        /* Disable interrupts */
    *ATU_TSTR1 &= (UINT8) ~0x01;        /* Stop counter */
    *ATU_TCNT0 = (UINT32)0x00000000;    /* Clear counter */

    /* Connect handler */
    if (intConnect (IV_ATU0_ATU04_OVI0, sysTimestampInt, NULL) != OK)
        return (ERROR);

    /* Set interrupt level */
    *INTC_IPRD  = (UINT16)((*INTC_IPRD & 0xf0ff) | ((TIMESTAMP_LEVEL & 0xf) << 8));

    *ATU_PSCR1 = (UINT8) 0x1f;          /* prescale by 32 */

    *ATU_TIER0 = (UINT16)0x0010;        /* enable interrupt */

    *ATU_TSTR1 |= (UINT8) 0x01;         /* start counter */

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
        *ATU_ITER0 = (UINT16)0x0000;            /* Disable interrupts */
        *ATU_TSTR1 &= (UINT8) ~0x01;            /* Stop counter */

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
    return (UINT32)(0xffffffff);        /* highest period -> freerunning */
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
    return ( CMT_TICK_FREQ );
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
    return ( *ATU_TCNT0 );      /* 32bit free-running counter */
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
    int lockKey = intLock ();

    UINT32 counter = *ATU_TCNT0;

    intUnlock (lockKey);

    return (counter);
    }

#endif  /* INCLUDE_TIMESTAMP */

