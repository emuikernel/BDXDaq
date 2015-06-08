/* m48t37.c - M48T37 library */

/* Copyright 2000-2004 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01c,06dec04,cak  Ported modifications from MCPM905 BSP (Fixed improper 
		 pointer usage to RTC structs).
01b,11may04,cak  Hard-coded the century to 20.
01a,17dec03,cak  Ported from ver 01a, mcpm905/m48t37.c.
*/

/*
DESCRIPTION

This library provides support routines for the real-time clock, alarm clock,
and Watchdog Timer sub-functions of the M48T37 "Timekeeper" RTC chip.  

.IP "Alarm Clock"

The alarm clock sub-function of the M48T37 can be configured to go off once
a month on a predetermined day, hour, minute, and second, to go off once a 
day at a predetermined hour, minute, and second, to go off once an hour at a
predetermined minute and second, to go off once a minute at a predetermined
second, or to go off once a second. 

When the clock information matches the alarm clock settings an interrupt is
generated.

To disable the alarm clock, you write "0" to the alarm date registers.  An
alarm clock interrupt is cleared by a read to the FLAGS register.

A call to m48t37AlarmShow() will display the current alarm clock 
configuration values.

.IP "Real-Time Clock"

The real-time clock sub-function of the M48T37 consists of 8 registers to
store the date and time.  The 8 values are as follows:

.CS

	Year			00-99
	Month			01-12
	Date ( day of month )	01-31
	Day ( day of week )	01-07
	Hour			00-23
	Min			00-59
	Sec 			00-59
	Century			00-99

.CE

All values in the RTC registers are stored in BCD ( binary coded decimal ).
Since the RTC_DATE_TIME and ALARM_DATE_TIME structures contain integers two
macros have been defined in m48t37.h which convert from binary to BCD and
vice-versa.  The macros are BIN_TO_BCD (bin) and BCD_TO_BIN (bcd).
	
.IP "Failsafe Timer"

The Failsafe sub-function is disabled at power-up and after a reset. The 
first call to m48t37FailsafeSet() arms the failsafe and subsequent calls to
m48t37FailsafeSet() will extend the failsafe timer and prevent it from 
expiring.

If m48t37FailsafeSet() is called with a timeout of zero, the failsafe timer
will be disabled.  The timer can also be disabled with a call to 
m48t37FailsafeCancel(). 

Failsafe expiration can be annunciated as either a maskable interrupt 
(IRQ 15) or as a board reset event. The selection is made by the "reset" 
parameter passed to m48t37FailsafeSet(). If "reset" is TRUE, a board reset 
event occurs when the failsafe expires. If "reset" is FALSE, an interrupt is 
generated.

A call to m48t37FailsafeCausedReset() will return a boolean indicating
whether the failsafe timer caused the last board reset.  To determine
if the failsafe timer caused the last reset you must call this routine
before setting an alarm, since setting the alarm will clear the failsafe 
timer flag.

When failsafe expiration is annunciated using an interrupt, an interrupt 
handler must be attached to the proper vector using intConnect() and the 
interrupt must be enabled using intEnable() before calling 
m48t37FailsafeSet() to arm the failsafe timer. If recovery from the interrupt
is desired, the interrupt service routine must call m48t37FailsafeSet() 
with "seconds" set to 0, or m48t37FailsafeCancel(). This operation disables 
the failsafe timer and clears the timer interrupt. If desired, the failsafe 
timer can be re-enabled by calling m48t37FailsafeSet() with the "seconds" 
parameter set non-zero.

A two-stage failsafe can be implemented by initially configuring the 
failsafe timer for interrupt generation and re-configuring it for reset 
generation during the failsafe service interrupt. The second occurrence of 
the failsafe will trigger a board reset.

The m48t37FailsafeGet() function returns the current failsafe configuration 
(not the amount of time remaining until timer expiration). Failsafe service 
code can use this function to extend the failsafe timeout without knowing 
the initial setting as follows:

.CS

 UCHAR seconds;
 BOOL  reset;

 m48t37FailsafeGet (&seconds, &reset);
 m48t37FailsafeSet (seconds,  reset);

.CE

A call to m48t37FailsafeShow() will display the current configuration of 
the failsafe timer.  m48t37FailsafeShow() simply displays the results of 
a call to m48t37FailsafeGet().

INCLUDE FILES: m48t37.h
*/

/* includes */

#include "m48t37.h"

/* forward declarations */

IMPORT void sysAlarmInt (void);
IMPORT void sysFailsafeInt (void);
STATUS m48t37RtcSet (RTC_DATE_TIME *rtc_time);
STATUS m48t37RtcShow (void);
STATUS m48t37RtcGet (RTC_DATE_TIME *rtc_time);
void   m48t37DateTimeHook (DOS_DATE_TIME *pDateTime);
STATUS m48t37AlarmSet (UCHAR method, ALARM_DATE_TIME *alarm_time);
STATUS m48t37AlarmCancel (void);
STATUS m48t37AlarmGet (UCHAR *method, ALARM_DATE_TIME *alarm_time);
STATUS m48t37AlarmShow (void);
STATUS m48t37FailsafeSet (UCHAR seconds, BOOL reset);
STATUS m48t37FailsafeGet (UCHAR *seconds, BOOL *reset);
STATUS m48t37FailsafeCancel (void);
STATUS m48t37Init (void);
STATUS m48t37FailsafeShow (void);
BOOL m48t37FailsafeCausedReset (void);

#if  ((defined INCLUDE_RTC) || (defined INCLUDE_FAILSAFE))

LOCAL STATUS m48t37RtcDateTest (RTC_DATE_TIME *rtc_time);
LOCAL STATUS m48t37AlarmDateTest (ALARM_DATE_TIME *alarm_time);
LOCAL void m48t37Int (void);

/*******************************************************************************
*
* m48t37RtcDateTest - test whether date/time values are valid
* 
* This routine tests the validity of the values in the RTC_DATE_TIME
* structure.
*
* RETURNS: OK, or ERROR if any values are invalid
*/

LOCAL STATUS m48t37RtcDateTest
    (
    RTC_DATE_TIME * rtc_time    /* pointer to time keeping structure */
    )
    {

    /* Check validity of seconds value */

    if (rtc_time->second < 0 || rtc_time->second > 59)
        {
	return (ERROR);
        }
    
    /* Check validity of minutes value */

    if (rtc_time->minute < 0 || rtc_time->minute > 59)
        {
	return (ERROR);
        }

    /* Check validity of hours value */

    if (rtc_time->hour < 0 || rtc_time->hour > 23)
        {
	return (ERROR);
        }

    /* Check validity of day of week value */

    if (rtc_time->day_of_week < 1 || rtc_time->day_of_week > 7)
        {
	return (ERROR);
        }

    /* Check validity of day of month value */

    if (rtc_time->day_of_month < 1 || rtc_time->day_of_month > 31)
        {
	return (ERROR);
        }

    /* Check validity of month value */

    if (rtc_time->month < 1 || rtc_time->month > 12)
        {
	return (ERROR);
        }

    /* Check validity of year value */

    if (rtc_time->year < 0 || rtc_time->year > 99)
        {
	return (ERROR);
        }

    /* Check validity of century value */

    if (rtc_time->century < 0 || rtc_time->century > 99)
        {
	return (ERROR);
        }
   
    return (OK);
 
    }

/*******************************************************************************
*
* m48t37AlarmDateTest - test whether alarm date/time values are valid
* 
* This routine tests the validity of the values in the ALARM_DATE_TIME
* structure.
*
* RETURNS: OK, or ERROR if any values are invalid.
*/

LOCAL STATUS m48t37AlarmDateTest
    (
    ALARM_DATE_TIME * alarm_time    /* pointer to time keeping structure */
    )
    {

    /* Check validity of seconds value */

    if (alarm_time->second < 0 || alarm_time->second > 59)
        {
	return (ERROR);
        }
    
    /* Check validity of minutes value */

    if (alarm_time->minute < 0 || alarm_time->minute > 59)
        {
	return (ERROR);
        }

    /* Check validity of hours value */

    if (alarm_time->hour < 0 || alarm_time->hour > 23)
        {
	return (ERROR);
        }

    /* Check validity of day of month value */

    if (alarm_time->day_of_month < 1 || alarm_time->day_of_month > 31)
        {
	return (ERROR);
        }

    return (OK);
 
    }

/*******************************************************************************
*
* m48t37RtcSet - set the RTC's date/time per caller's values.
*
* This routine allows the caller to set the RTC time and date.  The caller
* must allocate space for an RTC_DATE_TIME structure, fill the structure
* with the desired time and date, and call this routine.
*
* RETURNS: OK, or ERROR if date/time values are invalid.
*/

STATUS m48t37RtcSet
    (
    RTC_DATE_TIME * rtc_time   /* pointer to time keeping structure */
    )
    {
    UCHAR m48t37_control_values;	/* M48T37_CONTROL reg. */

        /* Determine whether date/time values are valid */
   
        if (m48t37RtcDateTest(rtc_time) == ERROR)
            {
            return (ERROR);
            }

        /* Set the write bit in the control register */

        m48t37_control_values = NV_RAM_READ((ULONG)M48T37_CONTROL);
        NV_RAM_WRITE((ULONG)M48T37_CONTROL, m48t37_control_values | M48T37_WRITE);

        /* Set the date/time in the chip's registers */

        NV_RAM_WRITE((ULONG)M48T37_SECOND, BIN_TO_BCD (rtc_time->second));
        NV_RAM_WRITE((ULONG)M48T37_MINUTE, BIN_TO_BCD (rtc_time->minute));
        NV_RAM_WRITE((ULONG)M48T37_HOUR, BIN_TO_BCD (rtc_time->hour));
        NV_RAM_WRITE((ULONG)M48T37_DAY_OF_WEEK, BIN_TO_BCD (rtc_time->day_of_week));
        NV_RAM_WRITE((ULONG)M48T37_DAY_OF_MONTH, BIN_TO_BCD (rtc_time->day_of_month));
        NV_RAM_WRITE((ULONG)M48T37_MONTH, BIN_TO_BCD (rtc_time->month));
        NV_RAM_WRITE((ULONG)M48T37_YEAR, BIN_TO_BCD (rtc_time->year));
        NV_RAM_WRITE((ULONG)M48T37_CENTURY, BIN_TO_BCD (rtc_time->century));

        /* Turn write bit in control register off */
  
        NV_RAM_WRITE((ULONG)M48T37_CONTROL, m48t37_control_values);

        return (OK);
    }

/*******************************************************************************
*
* m48t37RtcShow - Show the date/time on the user's display.
*
* This routine retrieves the current RTC date and time and sends it in a 
* user-readable fashion to the user's display.
*
* RETURNS: OK, or ERROR if unable to retrieve or print the current RTC
* date and time. 
*/

STATUS m48t37RtcShow (void)
    {
    RTC_DATE_TIME rtc_time;	/* RTC date and time */

    /* Retrieve the current RTC date and time */

    if (m48t37RtcGet(&rtc_time) == ERROR)
       {
       return (ERROR);
       }

    /* Send the date and time to the user's display */

    if (rtc_time.year < 10)
       {
       printf ("\n\r            Time: %d:%d:%d  Date: %d/%d/%d0%d\n\r",
               rtc_time.hour, rtc_time.minute, rtc_time.second,
               rtc_time.month, rtc_time.day_of_month, rtc_time.century,
               rtc_time.year);
       }
    else
       {
       printf ("\n\r            Time: %d:%d:%d  Date: %d/%d/%d%d\n\r",
               rtc_time.hour, rtc_time.minute, rtc_time.second,
               rtc_time.month, rtc_time.day_of_month, rtc_time.century,
               rtc_time.year);
       }

    return (OK);
    }

/***************************************************************************
*
* m48t37RtcGet - Get current RTC date/time. 
*
* This routine allows the caller to obtain the current RTC time and date.
* The caller must allocate space for an RTC_DATE_TIME structure, then call
* this routine.  
*
* RETURNS: OK, or ERROR if unable to retrieve the current RTC date and time. 
*/

STATUS m48t37RtcGet
    (
    RTC_DATE_TIME * rtc_time      /* pointer to time keeping structure */
    )
    {
    UCHAR m48t37_control_values; 	/* M48T37_CONTROL reg. */

    /* Set read bit in the control register */

    m48t37_control_values = NV_RAM_READ((ULONG)M48T37_CONTROL);
    NV_RAM_WRITE((ULONG)M48T37_CONTROL, m48t37_control_values | M48T37_READ);

    /* Read the values from the RTC into the rtc_time structure */

    rtc_time->second = BCD_TO_BIN (NV_RAM_READ((ULONG)M48T37_SECOND)); 
    rtc_time->minute = BCD_TO_BIN (NV_RAM_READ((ULONG)M48T37_MINUTE)); 
    rtc_time->hour = BCD_TO_BIN (NV_RAM_READ((ULONG)M48T37_HOUR)); 
    rtc_time->day_of_week = BCD_TO_BIN (NV_RAM_READ((ULONG)M48T37_DAY_OF_WEEK));
    rtc_time->day_of_month = BCD_TO_BIN (NV_RAM_READ((ULONG)M48T37_DAY_OF_MONTH));
    rtc_time->month = BCD_TO_BIN (NV_RAM_READ((ULONG)M48T37_MONTH)); 
    rtc_time->year = BCD_TO_BIN (NV_RAM_READ((ULONG)M48T37_YEAR));
#if 0
    rtc_time->century = BCD_TO_BIN (NV_RAM_READ((ULONG)M48T37_CENTURY));
#else
    rtc_time->century = (20);
#endif

    /* Turn off the read bit in the control register */

    NV_RAM_WRITE((ULONG)M48T37_CONTROL, m48t37_control_values);

    return (OK);

    }

/*******************************************************************************
*
* m48t37DateTimeHook - This is a hook routine for the dosFsLib.
*
* This routine fills in a DOS_DATE_TIME structure with the current date
* and time.  This is used by the standard VxWorks dosFsLib as a way to update 
* the date/time values used for file timestamps.  This is "hooked" into the
* dosFsLib during board initialization if both the dosFsLib and the RTC support
* is included in the BSP.
*
* RETURNS: void.
*
* SEE ALSO: dosFsDateTimeInstall(), and the dosFsLib documentation.
*/

void m48t37DateTimeHook
    (
    DOS_DATE_TIME * pDateTime    /* pointer to time keeping structure */
    )
    {
    RTC_DATE_TIME rtc_time;	/* RTC date and time */
    int dos_year = 0;			/* Current year */
 
    m48t37RtcGet(&rtc_time);

    /* Convert century and year to a form suitable for DOS_DATE_TIME */

    dos_year = ((rtc_time.century * 100) + rtc_time.year);

    /* Populate DOS_DATE_TIME structure within current RTC time and date */
    
    pDateTime->dosdt_year = dos_year;
    pDateTime->dosdt_month = rtc_time.month;
    pDateTime->dosdt_day = rtc_time.day_of_month;
    pDateTime->dosdt_hour = rtc_time.hour;
    pDateTime->dosdt_minute = rtc_time.minute;
    pDateTime->dosdt_second = rtc_time.second;
 
    }

/*******************************************************************************
*
* m48t37AlarmSet - Set an alarm clock per the caller's settings.
*
* This routine sets an alarm clock. The alarm can be programmed to go off
* once a month at a predetermined day, hour, minute, and second, to go off
* once a day at a predetermined hour, minute, and second, to go off once an
* hour at a predetermined minute and second, to go off once a minute at a
* predetermined second, or to go off once a second. 
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
* passed to the function in an ALARM_DATE_TIME structure. 
*
* RETURNS: OK, or ERROR if the settings are invalid.
*/

STATUS m48t37AlarmSet
    (
    UCHAR method,               	/* ALARM_EVERY_XXXX */
    ALARM_DATE_TIME * alarm_time     	/* pointer to time keeping structure */
    )
    {
    UCHAR alarm_clock_rpt1;		/* M48T37_ALARM_SECOND reg. */
    UCHAR alarm_clock_rpt2;		/* M48T37_ALARM_MINUTE reg. */
    UCHAR alarm_clock_rpt3;		/* M48T37_ALARM_HOUR reg. */
    UCHAR alarm_clock_rpt4;		/* M48T37_ALARM_DATE reg. */
    UCHAR m48t37_interrupts;		/* M48T37_INTERRUPTS reg. */

    /* Determine whether date/time values are valid */
   
    if (m48t37AlarmDateTest(alarm_time) == ERROR)
        {
        return (ERROR);
        }

    /* Clear the Flags register */

    m48t37FailsafeCausedReset();

    /* Set the Alarm Flag Enable Flag */

    m48t37_interrupts = NV_RAM_READ((ULONG)M48T37_INTERRUPTS);
    NV_RAM_WRITE((ULONG)M48T37_INTERRUPTS, m48t37_interrupts | M48T37_ALARM_ENABLE);

    /* Set the values for the alarm clock */

    NV_RAM_WRITE((ULONG)M48T37_ALARM_SECOND, BIN_TO_BCD (alarm_time->second));
    NV_RAM_WRITE((ULONG)M48T37_ALARM_MINUTE, BIN_TO_BCD (alarm_time->minute));
    NV_RAM_WRITE((ULONG)M48T37_ALARM_HOUR, BIN_TO_BCD (alarm_time->hour));
    NV_RAM_WRITE((ULONG)M48T37_ALARM_DATE, BIN_TO_BCD (alarm_time->day_of_month));

    /* If repeatable(day, hour, minute, second)then set the repeat bits */

    alarm_clock_rpt1 = NV_RAM_READ((ULONG)M48T37_ALARM_SECOND);
    alarm_clock_rpt2 = NV_RAM_READ((ULONG)M48T37_ALARM_MINUTE);
    alarm_clock_rpt3 = NV_RAM_READ((ULONG)M48T37_ALARM_HOUR);
    alarm_clock_rpt4 = NV_RAM_READ((ULONG)M48T37_ALARM_DATE);

    if (method == ALARM_EVERY_DAY)
        {
 NV_RAM_WRITE((ULONG)M48T37_ALARM_DATE, alarm_clock_rpt4 | M48T37_REPEAT);
        }
    else if (method == ALARM_EVERY_HOUR)
        {
        NV_RAM_WRITE((ULONG)M48T37_ALARM_DATE, alarm_clock_rpt4 | M48T37_REPEAT);
        NV_RAM_WRITE((ULONG)M48T37_ALARM_HOUR, alarm_clock_rpt3 | M48T37_REPEAT);
        }
    else if (method == ALARM_EVERY_MINUTE)
        {
        NV_RAM_WRITE((ULONG)M48T37_ALARM_DATE, alarm_clock_rpt4 | M48T37_REPEAT);
        NV_RAM_WRITE((ULONG)M48T37_ALARM_HOUR, alarm_clock_rpt3 | M48T37_REPEAT);
        NV_RAM_WRITE((ULONG)M48T37_ALARM_MINUTE, alarm_clock_rpt2 | M48T37_REPEAT);
        }
    else if (method == ALARM_EVERY_SECOND) 
        {
        NV_RAM_WRITE((ULONG)M48T37_ALARM_DATE, alarm_clock_rpt4 | M48T37_REPEAT);
        NV_RAM_WRITE((ULONG)M48T37_ALARM_HOUR, alarm_clock_rpt3 | M48T37_REPEAT);
        NV_RAM_WRITE((ULONG)M48T37_ALARM_MINUTE, alarm_clock_rpt2 | M48T37_REPEAT);
        NV_RAM_WRITE((ULONG)M48T37_ALARM_SECOND, alarm_clock_rpt1 | M48T37_REPEAT);
        }
    else if (method != ALARM_EVERY_MONTH)
        {
        return (ERROR);
        }

    return (OK);

    }

/*******************************************************************************
*
* m48t37AlarmCancel - Disable the alarm clock.
*
* This routine disables the alarm clock.
*
* RETURNS: OK always. 
*/

STATUS m48t37AlarmCancel (void)
     {

     /* To disable the alarm clock we must write 0 to alarm registers */
    
     NV_RAM_WRITE((ULONG)M48T37_ALARM_SECOND, 0);
     NV_RAM_WRITE((ULONG)M48T37_ALARM_MINUTE, 0);
     NV_RAM_WRITE((ULONG)M48T37_ALARM_HOUR, 0);
     NV_RAM_WRITE((ULONG)M48T37_ALARM_DATE, 0);

     return (OK);

     }

/*******************************************************************************
*
* m48t37AlarmGet -  Populate an ALARM_DATE_TIME structure with current alarm clock settings. 
*
* This routine will retrieve the current alarm clock settings and place them
* in an ALARM_DATE_TIME structure and method which the user must pass in.
*
* RETURNS: OK, or error if the alarm settings are not valid.
*/

STATUS m48t37AlarmGet
     (
     UCHAR * method,			/* method = ALARM_EVERY_XXXX */
     ALARM_DATE_TIME * alarm_time	/* pointer to alarm parameters */
     )
     {
     UCHAR alarm_date_reg;		/* M48T37_ALARM_DATE reg. */
     UCHAR alarm_hour_reg;		/* M48T37_ALARM_HOUR reg. */
     UCHAR alarm_minute_reg;		/* M48T37_ALARM_MINUTE reg. */
     UCHAR alarm_second_reg;		/* M48T37_ALARM_SECOND reg. */

     UCHAR rpt4 = 0;			/* RPT4 bit */
     UCHAR rpt3 = 0;			/* RPT3 bit */
     UCHAR rpt2 = 0; 			/* RPT3 bit */
     UCHAR rpt1 = 0;			/* RPT1 bit */

     /* Read the values from the alarm clock */

     alarm_date_reg = NV_RAM_READ((ULONG)M48T37_ALARM_DATE);
     alarm_hour_reg = NV_RAM_READ((ULONG)M48T37_ALARM_HOUR);
     alarm_minute_reg = NV_RAM_READ((ULONG)M48T37_ALARM_MINUTE);
     alarm_second_reg = NV_RAM_READ((ULONG)M48T37_ALARM_SECOND);

     /* Determine if the alarm is set for a repeat mode */
     
     rpt4 = (alarm_date_reg >> ALARM_REPEAT_SHIFT_VALUE);
     rpt3 = (alarm_hour_reg >> ALARM_REPEAT_SHIFT_VALUE);
     rpt2 = (alarm_minute_reg >> ALARM_REPEAT_SHIFT_VALUE);
     rpt1 = (alarm_second_reg >> ALARM_REPEAT_SHIFT_VALUE);
 
     if ((rpt4 == 0x01) && (rpt3 == 0x01) && (rpt2 == 0x01) && (rpt1 == 0x01))
        {

        /* ALARM_EVERY_SECOND */

        *method = ALARM_EVERY_SECOND; 
        }
     else if ((rpt4 == 0x01) && (rpt3 == 0x01) && (rpt2 == 0x01) && 
              (rpt1 == 0x00))
        {

        /* ALARM_EVERY_MINUTE */

        *method = ALARM_EVERY_MINUTE;
        }
     else if ((rpt4 == 0x01) && (rpt3 == 0x01) && (rpt2 == 0x00) && 
              (rpt1 == 0x00))
        {

        /* ALARM_EVERY_HOUR */

        *method = ALARM_EVERY_HOUR; 
        }
     else if ((rpt4 == 0x01) && (rpt3 == 0x00) && (rpt2 == 0x00) && 
              (rpt1 == 0x00))
        {

        /* ALARM_EVERY_DAY */

        *method = ALARM_EVERY_DAY;
        }
     else if ((rpt4 == 0x00) && (rpt3 == 0x00) && (rpt2 == 0x00) && 
              (rpt1 == 0x00))
        {

        /* ALARM_EVERY_MONTH */

        *method = ALARM_EVERY_MONTH;
        }
     else
        {

        /* Unknown alarm configuration */

        return (ERROR);
        }

     /* populate alarm_time with alarm clock settings */

     alarm_time->day_of_month = BCD_TO_BIN ( alarm_date_reg & ALARM_DATE_MASK );
     alarm_time->hour = BCD_TO_BIN ( alarm_hour_reg & ALARM_HOUR_MASK ); 
     alarm_time->minute = BCD_TO_BIN ( alarm_minute_reg & ALARM_MINUTE_MASK ); 
     alarm_time->second = BCD_TO_BIN ( alarm_second_reg & ALARM_SECOND_MASK ); 

     return (OK);
     
     }

/*******************************************************************************
*
* m48t37AlarmShow - Display current alarm clock settings on the user's display.
*
* This routine retrieves the current alarm clock settings and sends them to 
* the user's display.
*
* RETURN: OK, or ERROR if unable to retrieve current settings.
*/

STATUS m48t37AlarmShow (void)
     {
     UCHAR method = 0;			/* method = ALARM_EVERY_XXXX */
     ALARM_DATE_TIME alarm_time;	/* pointer to alarm parameters */

     /* Retrieve the current settings */

     if (m48t37AlarmGet(&method, &alarm_time) == ERROR)
        {
        return (ERROR);
        }
 
     /* Send the alarm settings to the user's display */

     if ((alarm_time.day_of_month == 0) && (alarm_time.hour == 0) && 
         (alarm_time.minute == 0) && (alarm_time.second == 0))
        {
        printf("Alarm is disabled.\n");
        return (OK);
        }

     if (method == ALARM_EVERY_MONTH)
        {
        printf("Alarm is set to go off once a month at:\n");
        printf("  Date: %d\n",alarm_time.day_of_month);
        printf("  Hour: %d\n",alarm_time.hour);
        printf("Minute: %d\n",alarm_time.minute);
        printf("Second: %d\n",alarm_time.second);
        }
     else if (method == ALARM_EVERY_DAY)
        {
        printf("Alarm is set to go off once a day at:\n");
        printf("  Hour: %d\n",alarm_time.hour);
        printf("Minute: %d\n",alarm_time.minute);
        printf("Second: %d\n",alarm_time.second);
        }
     else if (method == ALARM_EVERY_HOUR)
        {
        printf("Alarm is set to go off once an hour at:\n");
        printf("Minute: %d\n",alarm_time.minute);
        printf("Second: %d\n",alarm_time.second);
        }
     else if (method == ALARM_EVERY_MINUTE)
        {
        printf("Alarm is set to go off once a minute at:\n");
        printf("Second: %d\n",alarm_time.second);
        }
     else if (method == ALARM_EVERY_SECOND)
        {
        printf("Alarm is set to go off once a second.\n");
        }
     else
        {

        /* Unknown method */

        return (ERROR);
        }

     return (OK);

     }

/*******************************************************************************
*
* m48t37FailsafeSet - Set failsafe timer parameters/feed failsafe timer.
*
* This routine sets the failsafe timer configuration parameters. It is also
* used to "feed" the failsafe to prevent expiration of the timer.
*
* The reset parameter determines whether timer expiration will generate
* an interrupt or a board reset.  If set to FALSE an interrupt will be 
* generated.  If set to TRUE a board reset will occur.
*
* If failsafe timer expiration is configured to generate an interrupt,
* the interrupt must be enabled and a handler attached before m48t37FailsafeSet
* is called to arm the failsafe timer. When the failsafe interrupt occurs,
* m48t37FailsafeSet must be called with "seconds" set to 0 to clear the
* interrupt, or m48t37FailsafeCancel must be called before the handler returns.
*
* RETURNS: OK if successful or ERROR if parameter error.
*
* SEE ALSO: m48t37FailsafeGet ()
*/

STATUS m48t37FailsafeSet
    (
    UCHAR seconds,	/* timer interval, 0 = disarm, 1-31 = arm, else ERROR */
    BOOL  reset		/* board reset or interrupt on timer expiration */
    )
    {
    UCHAR temp = 0;	/* 0 = disarm failsafe timer */

    if (seconds > M48T37FAILSAFE_MULTIPLIER_MAX)
         return (ERROR);

    /* if seconds is non-zero, buildup the failsafe control register image */

    if (seconds != 0)
        {

        /* configure the failsafe "steering" bit */

        temp = (reset ? M48T37FAILSAFE_WDS_RESET : 
                 M48T37FAILSAFE_WDS_INTERRUPT) << M48T37FAILSAFE_WDS_SHIFT;

        /* configure the multiplier field */

        temp |= seconds << M48T37FAILSAFE_MULTIPLIER_SHIFT;

        /* set the resolution to seconds */

        temp |= M48T37FAILSAFE_RESOLUTION_SECS;
        }

    /* write the failsafe ( watchdog ) timer control register */

    NV_RAM_WRITE((ULONG)M48T37_FAILSAFE, temp);

    return (OK);

    }

/*******************************************************************************
*
* m48t37FailsafeGet - Get failsafe timer parameters.
*
* This routine gets the failsafe timer configuration parameters. Note that it
* returns the current configuration of the timer, not the amount of time
* remaining until timer expiration.
*
* RETURNS: OK always.
*
* SEE ALSO: m48t37FailsafeSet ()
*/

STATUS m48t37FailsafeGet
    (
    UCHAR * seconds,	/* address for returning seconds (or NULL) */
    BOOL  * reset	/* address for returning expiration action (or NULL) */
    )
    {
    UCHAR temp;

    /* read the failsafe timer control register */

    temp = NV_RAM_READ((ULONG)M48T37_FAILSAFE);

    /* return configured timeout if requested */

    if (seconds != NULL)
        *seconds = (temp & M48T37FAILSAFE_MULTIPLIER_MASK)
                    >> M48T37FAILSAFE_MULTIPLIER_SHIFT;

    /* return configured timeout action if requested */

    if (reset != NULL)
        *reset = (temp & M48T37FAILSAFE_WDS_MASK) >> M48T37FAILSAFE_WDS_SHIFT;

    return (OK);

    }

/*******************************************************************************
*
* m48t37FailsafeCausedReset - Determine if failsafe timer caused last board reset.
*
* This routine determines whether the failsafe timer caused the last board
* reset.
*
* RETURNS: TRUE if failsafe timer caused last board reset, FALSE if not.
*/

BOOL m48t37FailsafeCausedReset (void)
    {
    UCHAR m48t37_flags_value;		/* M48T37_FLAGS reg. */

    /* Read the Flags register */

    m48t37_flags_value = NV_RAM_READ((ULONG)M48T37_FLAGS);

    /* Determine if failsafe flag is set */

    if ((m48t37_flags_value & M48T37_FAILSAFE_FLAG) == 0x80)
        {
        return (TRUE);
        }
    else
        return (FALSE);
    }

/*******************************************************************************
*
* m48t37Int - Interrupt handler for the M48T37.
*
* This routine is a generic interrupt handler for the M48T37.  It determines
* whether the interrupt was triggered by the alarm clock or the failsafe 
* timer and calls the appropriate interrupt handler.  The interrupt handlers
* are defined in the real-time clock/alarm clock and failsafe timer "interface"
* files (ie. sysRtc.c and sysFailsafe.c).  It is in these files that the user
* can design their own interrupt handlers by adjusting the existing example
* interrupt handlers.
*
* RETURNS: void.
*/

LOCAL void m48t37Int (void)
    {
    UCHAR m48t37_flags_value;		/* M48T37_FLAGS reg. */

    m48t37_flags_value = NV_RAM_READ((ULONG)M48T37_FLAGS);

    #ifdef INCLUDE_RTC

    /* Alarm clock interrupt - alarm clock interrupt handler */

    if (m48t37_flags_value & M48T37_ALARM_FLAG)
       {

       /* 
        * Call the alarm clock "interface" interrupt handler.
        * If the user wishes to write their own handler routine, they
        * need only edit the interface handler routine. 
        */

       sysAlarmInt(); 
       }
    #endif /* INCLUDE_RTC */

    #ifdef INCLUDE_FAILSAFE

    /* Failsafe timer interrupt - failsafe timer interrupt handler */

    if (m48t37_flags_value & M48T37_FAILSAFE_FLAG)
       {

       /*
        * Call the failsafe timer "interface" interrupt handler.
        * If the user wishes to write their own handler routine, they
        * need only edit the interface handler routine.
        */

       sysFailsafeInt(); 
       }
    #endif /* INCLUDE_FAILSAFE */

    }

/*******************************************************************************
*
* m48t37FailsafeShow - Show the current failsafe timer parameters.
*
* This routine displays the current failsafe timer configuration parameters.
*
* RETURNS: OK always
*
* SEE ALSO:m48t37FailsafeSet (),  m48t37FailsafeGet () 
*/

STATUS m48t37FailsafeShow (void)
    {
    UCHAR seconds;	/* timer interval, 0 = disarmed, 1-31 = arm */
    BOOL  reset;	/* board reset or interrupt on timer expiration */

    /* get current watchdog configuration */

    if (m48t37FailsafeGet(&seconds, &reset) != OK)
        return (ERROR);

    if (seconds == 0)
        printf ("Failsafe is disabled.\r\n");
    else
        printf ("Failsafe timeout set at %d second%s and generates a%s.\r\n",
                seconds, (seconds == 1) ? "" : "s",
                reset ? " board reset" : "n interrupt");

    return (OK);

    }

/*******************************************************************************
*
* m48t37Init - Initialize the M48T37. 
*
* This routine initializes the M48T37 by connecting the interrupt
* handler function pointer to the RTC interrupt line.  This routine enables the
* RTC interrupt. 
*
* RETURNS: OK always
*
*/

STATUS m48t37Init (void)
    {
    intConnect (INUM_TO_IVEC(RTC_INT_VEC), m48t37Int, 0);

    intEnable (RTC_INT_LVL);

    return (OK);

    }
#endif /* INCLUDE_RTC, INCLUDE_FAILSAFE */

/*******************************************************************************
*
* m48t37FailsafeCancel - cancel the failsafe timer 
*
* This routine cancels the failsafe timer.
*
* RETURNS: OK always. 
*
* SEE ALSO: m48t37FailsafeSet ()
*/

STATUS  m48t37FailsafeCancel (void)
    {
    UCHAR temp = 0;	/* 0 = disarm failsafe timer */

    /* write the failsafe ( watchdog ) timer control register */

    NV_RAM_WRITE((ULONG)M48T37_FAILSAFE, temp);

    return (OK);

    }
