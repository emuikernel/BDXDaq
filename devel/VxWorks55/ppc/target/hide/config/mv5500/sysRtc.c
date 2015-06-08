/* sysRtc.c - Interface routines for real-time clock and alarm clock support */

/* Copyright 2002 Motorola, Inc.  All Rights Reserved */

/*
modification history
--------------------
01a,14oct02,yyz   Adapted from HXEB100 base.
*/

/*
DESCRIPTION

This code provides the real-time clock and alarm clock interfaces.

The routines defined in this file are intended as user-callable routines.
The routines themselves call device-specific routines as defined by the
following in the board header file:

.IP "RTC_SET(x)"
Sets the RTC date and time.

.IP "RTC_SHOW()"
Displays the current RTC time in a user-readable form on the user's display. 

.IP "RTC_GET(x)"
Gets the current RTC date and time.

.IP "RTC_DATE_TIME_HOOK(x)"
Hook to connect the RTC_DATE_GET routine to the dosFsLib to provide 
date/time values for file timestamps.

.IP "ALARM_SET(x,y)"
Set an alarm.

.IP "ALARM_GET(x,y)"
Gets the current alarm clock settings.

.IP "ALARM_SHOW()"
Displays the current alarm clock settings in a user-readable form on the 
user's display.

.IP "ALARM_CANCEL()"
Cancel an alarm.

.LP

INCLUDED FILES: m48t37.h, dosFsLib.h
*/

/* included files */

#include "m48t37.h"
#include "dosFsLib.h"

/* forward declarations */

STATUS sysAlarmSet (UCHAR method, ALARM_DATE_TIME * alarm_time);
STATUS sysAlarmGet (UCHAR * method, ALARM_DATE_TIME * alarm_time);
STATUS sysAlarmCancel (void);
STATUS sysAlarmShow (void);
void   sysAlarmInt (void);
STATUS sysRtcSet (RTC_DATE_TIME * rtc_time);
STATUS sysRtcShow (void);
STATUS sysRtcGet (RTC_DATE_TIME * rtc_time);
void   sysRtcDateTimeHook (DOS_DATE_TIME * pDateTime);

/*******************************************************************************
* 
* sysAlarmSet - This routine sets an alarm clock.
*
* The alarm can be programmed to go off once a month at a predetermined day,
* hour, minute, and second, to go off once a day at a predetermined hour, 
* minute, and second, to go off once an hour at a predetermined minute, and
* second, to go off once a minute at a predetermined second, or to go off once
* a second.
*
* method can have the following values:
* 
* .CS
*
* ALARM_EVERY_MONTH  = once a month
* ALARM_EVERY_DAY    = once a day
* ALARM_EVERY_HOUR   = once an hour
* ALARM_EVERY_MINUTE = once a minute
* ALARM_EVERY_SECOND = once a second
*
* .CE
*
* The alarm values for second, minute, hour, and day_of_month must be
* passed to the function in an ALARM_DATE_TIME structure.  The hour must be
* specified using a 24-hour clock.
*
* Valid parameters:
*
* .CS
* 
*  day of month = 01-31
*          hour = 00-23
*        minute = 00-59
*        second = 00-59
*
* .CE
*
* For example:
*  If you wanted to set an alarm to repeat once a month, on the 3rd day, at 
*  2:45 PM, you would create an ALARM_DATE_TIME structure with the following
*  values:
*
* .CS
*
*	ALARM_DATE_TIME * a_time;
*	a_time->day_of_month = 3;
*  	a_time->hour = 14;
*	a_time->minute = 45;
*	a_time->second = 0; 
*
* .CE
*
* You would then pass in a value of ALARM_EVERY_MONTH for the method, and 
* the a_time structure you created, to the function.
*  
* RETURNS: OK, or ERROR if the settings are invalid.
*/

STATUS sysAlarmSet
    (
    UCHAR method, 		 /* method = ALARM_EVERY_XXXX */
    ALARM_DATE_TIME * alarm_time /* alarm parameters */ 
    )
    {
    if (ALARM_SET(method, alarm_time) == ERROR)
       {
       return (ERROR);
       }
    return (OK);
    }

/*******************************************************************************
*
* sysAlarmCancel - This routine disables the alarm clock.
*
* A call to this routine will disable the alarm clock.
*
* RETURNS: OK, or ERROR if alarm cannot be canceled.
*/

STATUS sysAlarmCancel (void)
     {
     if (ALARM_CANCEL() == ERROR)
        {
        return (ERROR);
        }
     return (OK); 
     }

/*******************************************************************************
*
* sysAlarmInt - Interrupt handler for the alarm clock.
*
* When the alarm clock expires, an interrupt is generated. This is an example 
* interrupt handler routine.  This routine prints a "@" every time the alarm
* clock interrupt is fired.
*
* To define your own alarm clock interrupt handler, simply edit this routine.
*
* RETURNS: void.
*/

void sysAlarmInt (void) 
     {
     logMsg("@",0,0,0,0,0,0);

     return;
     }

/*******************************************************************************
*
* sysAlarmGet - Get the current alarm clock parameters.
*
* This routine gets the current alarm clock parameters.  It populates an
* ALARM_DATE_TIME structure with the current settings.
*
* RETURNS: OK, or ERROR if unable to get the current settings.
*/

STATUS sysAlarmGet 
     (
     UCHAR * method,		   /* method = ALARM_EVERY_XXXX */
     ALARM_DATE_TIME * alarm_time  /* current alarm parameters */
     )
     {
     if (ALARM_GET(method, alarm_time) == ERROR)
        {
        return (ERROR);
        }
     return (OK);
     }

/*******************************************************************************
*
* sysAlarmShow - Display current alarm clock settings on the user's display.
*
* This routine will display the current alarm clock settings on the user's
* display in a user-readable fashion.
*
* RETURNS: OK, or ERROR if unable to get or display the current settings.
*/

STATUS sysAlarmShow (void)
     {
     if (ALARM_SHOW() == ERROR)
        {
        return (ERROR);
        }
     return (OK);
     }
 
/*******************************************************************************
*
* sysRtcSet - Sets the real-time clock date and time.
*
* This routine sets the real-time clock date and time as per the user's
* specification.
*
* The routine expects as input, an RTC_DATE_TIME structure configured with
* valid date and time values.
*
* Valid parameters:
*
* .CS
*
*        century = 00-99
*           year = 00-99
*          month = 01-12
*   day of month = 01-31
*    day of week = 01-07
*           hour = 00-23
*         minute = 00-59
*         second = 00-59
*
* .CE
*
* For example, to set the date and time to January 1, 2001 at 4:40 PM, you
* would populate the structure with the following values:
*
* .CS
*
* 	RTC_DATE_TIME *rtc_time;
*	rtc_time->century = 20;
*	rtc_time->year = 01;
*	rtc_time->month = 1;
*	rtc_time->day_of_month = 1;
*	rtc_time->day_of_week = 2;
*	rtc_time->hour = 16;
*	rtc_time->minute = 40;
*	rtc_time->second = 0;
*
* .CE
*
* RETURNS: OK, or ERROR if the date and time values were invalid.
*/

STATUS sysRtcSet 
    ( 
    RTC_DATE_TIME * rtc_time	/* Real-time clock parameters */
    )
    {
    if (RTC_SET(rtc_time) == ERROR)
        {
        return (ERROR);
        }
    return (OK);
    }

/*******************************************************************************
*
* sysRtcShow - Display current RTC time and date on the user's display.
*
* This routine displays the current RTC time and date on the user's display
* in a user-readable fashion.
*
*  For example:
*
* .CS
* 
*	Time: 18:34:12  Date: 2/20/2001
*
* .CE
*
* RETURNS: OK, or ERROR if unable to get or display the current RTC time
* and date.
*/

STATUS sysRtcShow (void) 
     {
     if (RTC_SHOW() == ERROR)
        {
        return (ERROR);
        }
     return (OK);
     }

/*******************************************************************************
*
* sysRtcGet - Populate RTC_DATE_TIME structure with current RTC date and time.
*
* This routine expects an RTC_DATE_TIME structure as input, which it will 
* populate with the RTC's current date and time.
*
* RETURNS: OK, or ERROR if unable to get the current RTC date and time.
*/

STATUS sysRtcGet
     (
     RTC_DATE_TIME * rtc_time	/* Current real-time clock parameters */
     )
     {
     if (RTC_GET(rtc_time) == ERROR)
        {
        return (ERROR);
        }
     return (OK);
     }

/*******************************************************************************
*
* sysRtcDateTimeHook - Hook real-time clock to the dosFsLib.
*
* This routine expects a DOS_DATE_TIME structure as input, which it will 
* populate with the current date and time.  It is then used by the standard
* VxWorks dosFsLib as a way to update the date/time values used for file
* timestamps.
*
* RETURNS: void.
*/

void sysRtcDateTimeHook
     (
     DOS_DATE_TIME * pDateTime	/* Current date and time */
     )
     {
     RTC_DATE_TIME_HOOK(pDateTime);
     }

       
