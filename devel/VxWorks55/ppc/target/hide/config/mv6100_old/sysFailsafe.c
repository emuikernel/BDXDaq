/* sysFailsafe.c - Interface routines for Failsafe Timer support */

/* Copyright 2003 Motorola, Inc.  All Rights Reserved */

/*
modification history
--------------------
01b,17dec03,cak  BSP update.
01a,23oct02,cak  Ported from ver 01a, hxeb100/sysFailsafe.c
*/

/*
DESCRIPTION

This code provides the failsafe timer interface.

The routines defined in this file are intended as user-callable routines.
The routines themselves call device-specific routines as defined by the
following in the board header file:

.IP "FAILSAFE_SET(x,y)"
Sets the failsafe timer.

.IP "FAILSAFE_GET(x,y)"
Gets the current failsafe timer configuration parameters.

.IP "FAILSAFE_CANCEL()"
Cancels the failsafe timer.

.IP "FAILSAFE_SHOW()"
Shows the current failsafe timer configuration parameters.

.IP "FAILSAFE_CAUSED_RESET()"
Determines whether the failsafe timer caused the last board reset.
*/

/* included files */

#include "m48t37.h"

/* forward declarations */

STATUS sysFailsafeSet (UCHAR seconds, BOOL reset);
STATUS sysFailsafeGet (UCHAR * seconds, BOOL * reset);
STATUS sysFailsafeShow (void);
void sysFailsafeInt (void);
void sysFailsafeCancel (void);
BOOL sysFailsafeCausedReset (void);

/*******************************************************************************
*
* sysFailsafeSet - Set the failsafe timer.
*
* This routine sets the failsafe timer.  The timer can be configured to either
* generate an interrupt or reset the board upon expiration.  The routine
* expects the timer interval "seconds" and an expiration parameter "reset".
* Seconds can be set anywhere from 0 to 31.  If set to 0 the timer is disabled.
* Reset is a boolean.  If set to FALSE, an interrupt will be generated.  If set
* to TRUE, the board will be reset.
*
* RETURNS: OK, or error if one of the parameters is invalid.
*/

STATUS sysFailsafeSet
     (
     UCHAR seconds,	/* Time until timer expiration */
     BOOL reset		/* reset=FALSE, interrupt, reset=TRUE, board reset */
     )
     {
     if (FAILSAFE_SET(seconds, reset) == ERROR)
        {
        return (ERROR);
        }
     return (OK);
     }

/*******************************************************************************
*
* sysFailsafeGet - Get the failsafe timer configuration parameters.
*
* This routine gets the current failsafe timer configuration parameters NOT
* the amount of time remaining until timer expiration.
*
* RETURNS: OK always.
*/

STATUS sysFailsafeGet
     (
     UCHAR * seconds,	/* Time until timer expiration */
     BOOL * reset	/* reset=FALSE, interrupt, reset=TRUE, board reset */
     )
     {
     FAILSAFE_GET(seconds, reset);

     return (OK);
     }

/*******************************************************************************
*
* sysFailsafeCancel - This routine disables the failsafe timer.
*
* A call to this routine will disable the failsafe timer.
*
* RETURNS: void.
*/

void sysFailsafeCancel (void)
     {
     FAILSAFE_CANCEL();
     }

/*******************************************************************************
*
* sysFailsafeInt - Interrupt handler for the Failsafe timer.
*
* When the failsafe timer expires, either an interrupt is generated or a board
* reset. This is an example interrupt handler routine.  This routine prints a
* "*" every time the failsafe timer interrupt is fired.
*
* To define your own failsafe timer interrupt handler, simply edit this
* routine.
*
* RETURNS: void.
*/

void sysFailsafeInt (void)
     {
     UCHAR seconds = 30;	/* Time until timer expiration */
     BOOL reset = 0;		/* reset = FALSE, interrupt */

     logMsg("*",0,0,0,0,0,0);

     /* Clear the interrupt - this code must remain. */

     FAILSAFE_CANCEL();

     /* Reset the failsafe timer for 30 seconds. */

     FAILSAFE_SET(seconds, reset);

     return;
     }

/*******************************************************************************
*
* sysFailsafeCausedReset - Determine if failsafe timer caused last board reset.
*
* This routine determines whether the failsafe timer caused the last board
* reset.  To determine whether or not the failsafe timer caused the last
* board reset you must call this routine before setting an alarm.  When an
* alarm is set, all flags including the failsafe timer flag are cleared.
*
* RETURNS: True, if the failsafe timer caused the last board reset, false
* otherwise.
*/

BOOL sysFailsafeCausedReset (void)
    {
    if (FAILSAFE_CAUSED_RESET() == TRUE)
        {
        return (TRUE);
        }
    else
        return (FALSE);
    }

/*******************************************************************************
*
* sysFailsafeShow - Display current failsafe timer configuration parameters.
*
* This routine displays the current failsafe timer configuration parameters.
*
* RETURNS: OK always.
*/

STATUS sysFailsafeShow (void)
     {
     FAILSAFE_SHOW();

     return (OK);
     }
