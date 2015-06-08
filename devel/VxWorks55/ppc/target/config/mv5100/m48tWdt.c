/* m48tWdt.c - M48T Watchdog Timer library */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 2000-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01d,07mar01,rhk  Corrected comment regarding IRQ value.
01c,16aug00,dmw  Wind River coding standards review.
01b,26may00,dmw  Made more generic to support the M4837V.
01a,21mar00,rhv  written - Motorola.
*/

/*
DESCRIPTION
This library provides support routines for the Watchdog Timer sub-function of
the M48T "Timekeeper" RTC chip.  The following functions are provided:

    STATUS sysM48tWdtSet (UCHAR seconds, BOOL reset);
    STATUS sysM48tWdtGet (UCHAR * seconds, BOOL * reset);
    STATUS sysM48tWdtShow (void);

The Watchdog sub-function is disabled at power-up and after a reset. The first
call to sysM48tWdtSet() arms the watchdog and subsequent calls to
sysM48tWdtSet() will extend the watchdog timer and prevent it from expiring.
If sysM48tWdtSet() is called with a timeout of zero, the watchdog will be
disabled.

Watchdog expiration can be annunicated as either a maskable interrupt (IRQ 3 on
the MVME5100) or as a board reset event. The selection is made by the "reset"
parameter passed to sysM48tWdtSet(). If "reset" is TRUE, a board reset
event occurs when the watchdog expires. If "reset" is FALSE, an interrupt is
generated.

When watchdog expiration is annunciated using an interrupt, an interrupt handler
must be attached to the proper vector using intConnect() and the interrupt must
be enabled using intEnable() before calling sysM48tWdtSet() to arm the
watchdog. If recovery from the interrupt is desired, the interrupt service
routine must call sysM48tWdtSet() with "seconds" set to 0. This operation
disables the watchdog timer and clears the watchdog interrupt. If desired, the
watchdog can be re-enabled by calling sysM48tWdtSet() with the "seconds"
parameter set non-zero.

A two-stage watchdog can be implemented by initially configuring the watchdog
for interrupt generation and re-configuring it for reset generation during the
watchdog service interrupt. The second occurance of the watchdog will trigger a
board reset.

The sysM48tWdtGet() function returns the current watchdog configuration (not
the amount of time remaining until timer expiration). Watchdog service code can
use this function to extend the watchdog timeout without knowing the initial
setting as follows:

    UCHAR seconds;
    BOOL  reset;

    sysM48tWdtGet (&seconds, &reset);
    sysM48tWdtSet (seconds,  reset);


If INCLUDE_SHOW_ROUTINES is defined, sysM48tWdtShow() can be invoked from the
command line to display the current configuration of the watchdog.
sysM48tWdtShow() simply displays the results of a call to sysM48tWdtGet().

INCLUDE FILES: m48tWdt.h
*/

#include "m48tWdt.h"

/*******************************************************************************
*
* sysM48tWdtSet - set watchdog timer parameters/feed watchdog timer
*
* This routine sets the watchdog timer configuration parameters. It is also
* used to "feed" the watchdog to prevent expiration of the timer.
*
* NOTE: If watchdog timer expiration is configured to generate an interrupt,
* the interrupt must be enabled and a handler attached before sysM48tWdtSet
* is called to arm the watchdog. When the watchdog interrupt occurs,
* sysM48tWdtSet must be called with "seconds" set to 0 to clear the
* interrupt before the handler returns.
*
* RETURNS: OK if successful or ERROR if parameter error.
*
* SEE ALSO: sysM48tWDTGet ()
*/

STATUS sysM48tWdtSet
    (
    UCHAR seconds,	/* timer interval, 0 = disarm, 1-31 = arm, else ERROR */
    BOOL  reset		/* board reset or interrupt on WDT expiration */
    )
    {
    UCHAR temp = 0;	/* 0 = disarm watchdog timer */

    if (seconds > M48TWDT_MULTIPLIER_MAX)
         return (ERROR);

    /* if seconds is non-zero, buildup the WDT control register image */

    if (seconds != 0)
        {

        /* configure the WDT "steering" bit */

        temp = (reset ? M48TWDT_WDS_RESET : M48TWDT_WDS_INTERRUPT) <<
                M48TWDT_WDS_SHIFT;

        /* configure the multiplier field */

        temp |= seconds << M48TWDT_MULTIPLIER_SHIFT;

        /* set the resolution to seconds */

        temp |= M48TWDT_RESOLUTION_SECS;
        }

    /* write the watchdog timer control register */

    NV_RAM_WRITE((ULONG)WD_TIMER, temp);

    return (OK);

    }

/*******************************************************************************
*
* sysM48tWdtGet - get watchdog timer parameters
*
* This routine gets the watchdog timer configuration parameters. Note that it
* returns the current configuration of the timer, not the amount of time
* remaining until timer expiration.
*
*
* RETURNS: OK always
*
* SEE ALSO: sysM48tWDTSet ()
*/

STATUS sysM48tWdtGet
    (
    UCHAR * seconds,	/* address for returning seconds (or NULL) */
    BOOL  * reset	/* address for returning expiration action (or NULL) */
    )
    {
    UCHAR temp;

    /* read the watchdog timer control register */

    temp = NV_RAM_READ((ULONG)WD_TIMER);

    /* return configured timeout if requested */

    if (seconds != NULL)
        *seconds = (temp & M48TWDT_MULTIPLIER_MASK)
                    >> M48TWDT_MULTIPLIER_SHIFT;

    /* return configured timeout action if requested */

    if (reset != NULL)
        *reset = (temp & M48TWDT_WDS_MASK) >> M48TWDT_WDS_SHIFT;

    return (OK);

    }

/*******************************************************************************
*
* sysM48tWdtInt - Example Watchdog interrupt handler.
*
* This example routine is the watchdog interrupt service routine.  This 
* routine prints a "*" every time the watchdog interrupt is fired.  This
* routine then restarts the watchdog for a frequency of 1 second.
*
* RETURNS: none
*
*/

LOCAL void sysM48tWdtInt (void)
    {
    logMsg("*",0,0,0,0,0,0);

    sysM48tWdtSet (1, 0);

    return;
    }

/*******************************************************************************
*
* sysM48tWdtInit - Initialize watchdog timer
*
* This routine initializes the watchdog timer by connecting an example interrupt
* handler function pointer to the RTC interrupt line.  This routine enables the
* RTC interrupt and calls sysM48tWDTSet() with an example frequency of 1 second.
*
* RETURNS: OK always
*
*/

STATUS sysM48tWdtInit (void)
    {
    intConnect (INUM_TO_IVEC(RTC_ALARM_VEC), sysM48tWdtInt, 0);

    intEnable (RTC_ALARM_LVL);

    sysM48tWdtSet (1, 0);

    return (OK);
    }

#ifdef INCLUDE_SHOW_ROUTINES
/*******************************************************************************
*
* sysM48tWdtShow - show the current watchdog timer parameters
*
* This routine displays the current watchdog timer configuration parameters.
*
* RETURNS: OK always
*
* SEE ALSO:sysM48tWDTSet (),  sysM48tWDTGet () 
*/

STATUS sysM48tWdtShow (void)
    {
    UCHAR seconds;	/* timer interval, 0 = disarmed, 1-31 = arm */
    BOOL  reset;	/* board reset or interrupt on WDT expiration */

    /* get current watchdog configuration */

    if (sysM48tWdtGet (&seconds, &reset) != OK)
        return (ERROR);

    if (seconds == 0)
        printf ("Watchdog is disabled.\r\n");
    else
        printf ("Watchdog timeout set at %d second%s and generates a%s.\r\n",
                seconds, (seconds == 1) ? "" : "s",
                reset ? " board reset" : "n interrupt");

    return (OK);

    }

#endif /* INCLUDE_SHOW_ROUTINES */
