/* nvr4101RTCTimer.c - NEC Vr4101 (RTC) timer driver */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,23jun97,mem written.
*/

/*

DESCRIPTION

This library contains routines to manipulate the timer functions in
the Vr4101 RTC module.  This library handles auxiliary clock
functions.

The macros AUX_CLK_RATE_MIN, and AUX_CLK_RATE_MAX must be defined to
provide parameter checking for the sysAuxClkRateSet() routines.

The RTCLong timer is used for the auxiliary clock.

*/

/* defines */

#define RTC_LONG_RATE	32768	/* aux clock counter runs at 32kHz */

/* locals */

LOCAL int   sysAuxClkTicksPerSecond = 100;	/* default aux timer rate    */
LOCAL int   sysAuxClkArg	    = NULL;	/* aux clock int routine arg */
LOCAL BOOL  sysAuxClkRunning	    = FALSE;	/* sys aux clock enabled flag*/
LOCAL FUNCPTR	sysAuxClkRoutine    = NULL;	/* aux clock interpt routine */
LOCAL int   sysAuxClkTicks; 		        /* aux clk ticks */

/*******************************************************************************
*
* sysAuxClkInt - interrupt level processing for auxiliary clock
*
* This routine handles the auxiliary clock interrupt.  It is attached to the
* clock interrupt vector by the routine sysAuxClkConnect().
* The appropriate routine is called and the interrupt is acknowleged.
*/

LOCAL void sysAuxClkInt (void)
    {
    /* clear the RTCLong interrupt */
    
    *VR4101_RTCINTREG = VR4101_RTC_RTCINTR0;
    
    if (sysAuxClkRoutine != NULL)
	(*sysAuxClkRoutine) (sysAuxClkArg);	/* call system clock routine */
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
    sysAuxClkRoutine = routine;
    sysAuxClkArg = arg;

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
	int key;

	key = intLock ();
	*VR4101_ICU_MSYSINTREG &= ~VR4101_RTCLINTR;
	sysAuxClkRunning = FALSE;
	intUnlock (key);
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
	int i;
	int key;

	key = intLock ();
	sysAuxClkRunning = TRUE;
	sysAuxClkTicks = RTC_LONG_RATE / sysAuxClkTicksPerSecond;
	for (i = 0; i < 2; ++i)
	    {
	    /* set RTCLong register */

	    *VR4101_RTCLLREG = sysAuxClkTicks & 0xffff;
	    *VR4101_RTCLHREG = (sysAuxClkTicks >> 16) & 0xff;

	    /* set RTCLong counter register */
    
	    *VR4101_RTCLCNTLREG = sysAuxClkTicks & 0xffff;
	    *VR4101_RTCLCNTHREG = (sysAuxClkTicks >> 16) & 0xff;
	    }

	/* enable the RTCLong interrupt */

	*VR4101_ICU_MSYSINTREG |= VR4101_RTCLINTR;
	intUnlock (key);
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
    int ticksPerSecond     /* number of clock interrupts per second */
    )
    {
    if (ticksPerSecond < AUX_CLK_RATE_MIN || ticksPerSecond > AUX_CLK_RATE_MAX)
	return (ERROR);

    if (((RTC_LONG_RATE / ticksPerSecond) < 4)
	|| ((RTC_LONG_RATE / ticksPerSecond) > 0x00ffffff))
	return (ERROR);

    sysAuxClkTicksPerSecond = ticksPerSecond;

    if (sysAuxClkRunning)
	{
	sysAuxClkDisable ();
	sysAuxClkEnable ();
	}

    return (OK);
    }
