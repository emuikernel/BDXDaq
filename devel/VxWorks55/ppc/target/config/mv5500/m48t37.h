/* m48t37.h - STMicroelectronics Timekeeper SRAM driver header */

/* Copyright 2000-2002 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,14oct02,yyz   Adapted from HXEB100 base.
*/

/*
This file contains the configuration parameters and control functions for 
the SGS-Thomson M48T37V and M48T37Y Real-time clock, alarm clock, and 
Watchdog (failsafe) timer functions.  
*/

#ifndef	INCm48t37h
#define	INCm48t37h

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

/* includes */

#include "dosFsLib.h"

/* This structure holds the Real-Time Clock configuration values */

typedef struct rtcdt 
    {
    int  century;		/* century */
    int  year;			/* year */
    int  month;			/* month */
    int  day_of_month; 		/* day of month */
    int  day_of_week;		/* day of week */
    int  hour;			/* hour */
    int  minute;		/* minute */
    int  second;		/* second */
    } RTC_DATE_TIME; 

/* This structure holds the Alarm Clock configuration values */

typedef struct alarmdt
    {
    int  day_of_month;		/* day of month */
    int  hour;			/* hour */
    int  minute;		/* minute */
    int  second;		/* second */
    } ALARM_DATE_TIME;

/* defines */

/* M48T37 register bit settings */

#define M48T37_WRITE		0x80	/* Write bit */
#define M48T37_READ		0x40	/* Read bit */
#define M48T37_ALARM_ENABLE	0x80	/* Alarm flag enable flag */
#define M48T37_REPEAT		0x80	/* Repeat bit for register 7ff2-7ff5 */
#define M48T37_FAILSAFE_FLAG	0x80	/* Failsafe flag */
#define M48T37_ALARM_FLAG	0x40	/* Alarm flag */

/* Control values for the m48t37AlarmSet routine */

#define ALARM_EVERY_MONTH	1	/* Alarm to recur once a month */
#define ALARM_EVERY_DAY		2	/* Alarm to recur once a day */
#define ALARM_EVERY_HOUR	3	/* Alarm to recur once an hour */
#define ALARM_EVERY_MINUTE	4	/* Alarm to recur once a minute */
#define ALARM_EVERY_SECOND	5	/* Alarm to recur every second */

/* Alarm clock masks */

#define ALARM_DATE_MASK			0x3F	/* Alarm date mask */
#define ALARM_HOUR_MASK			0x3F	/* Alarm hour mask */
#define ALARM_MINUTE_MASK		0x7F	/* Alarm minute mask */
#define ALARM_SECOND_MASK		0x7F	/* Alarm second mask */
#define ALARM_REPEAT_SHIFT_VALUE	7	/* Repeat bit shift value */

/* Failsafe(Watchdog) timer resolutions */

#define M48T37FAILSAFE_RESOLUTION_16THS    0	/* 16 ticks/Sec */
#define M48T37FAILSAFE_RESOLUTION_4THS     1	/* 4  ticks/Sec */
#define M48T37FAILSAFE_RESOLUTION_SECS     2	/* 1  tick /Sec */
#define M48T37FAILSAFE_RESOLUTION_4_SECS   3	/* 1  tick/4 secs */
#define M48T37FAILSAFE_RESOLUTION_MASK     3   /* resolution mask */

/* Failsafe(Watchdog) timer multiplier (timer "ticks" until expiration) */

#define M48T37FAILSAFE_MULTIPLIER_MAX      31  /* maximum multiplier */
#define M48T37FAILSAFE_MULTIPLIER_SHIFT    2   /* multiplier shift value */
#define M48T37FAILSAFE_MULTIPLIER_MASK     (M48T37FAILSAFE_MULTIPLIER_MAX \
                                         << M48T37FAILSAFE_MULTIPLIER_SHIFT)

/* Failsafe(Watchdog) timer expiration routing */

#define M48T37FAILSAFE_WDS_RESET           1	/* generate board reset */
#define M48T37FAILSAFE_WDS_INTERRUPT       0	/* generate interrupt */
#define M48T37FAILSAFE_WDS_SHIFT           7   /* shift value */
#define M48T37FAILSAFE_WDS_MASK            (1 << M48T37FAILSAFE_WDS_SHIFT)

/* M48T37 Register Offsets */

#define M48T37_YEAR		0x7FFF
#define M48T37_MONTH		0x7FFE
#define M48T37_DAY_OF_MONTH	0x7FFD
#define M48T37_DAY_OF_WEEK	0x7FFC
#define M48T37_HOUR		0x7FFB
#define M48T37_MINUTE		0x7FFA
#define M48T37_SECOND		0x7FF9
#define M48T37_CONTROL		0x7FF8
#define M48T37_FAILSAFE		0x7FF7
#define M48T37_INTERRUPTS	0x7FF6
#define M48T37_ALARM_DATE	0x7FF5
#define M48T37_ALARM_HOUR	0x7FF4
#define M48T37_ALARM_MINUTE	0x7FF3
#define M48T37_ALARM_SECOND	0x7FF2
#define M48T37_CENTURY		0x7FF1
#define M48T37_FLAGS		0x7FF0

/* global functions */

STATUS m48t37Init (void);
STATUS m48t37RtcSet (RTC_DATE_TIME * rtc_time);
STATUS m48t37RtcShow (void);
STATUS m48t37RtcGet (RTC_DATE_TIME * rtc_time);
void   m48t37DateTimeHook (DOS_DATE_TIME * pDateTime);
STATUS m48t37AlarmSet (UCHAR method, ALARM_DATE_TIME * alarm_time);
STATUS m48t37AlarmCancel ( void );
STATUS m48t37AlarmGet (UCHAR * method, ALARM_DATE_TIME * alarm_time);
STATUS m48t37AlarmShow (void);
STATUS m48t37FailsafeSet (UCHAR seconds, BOOL reset);
STATUS m48t37FailsafeGet (UCHAR * seconds, BOOL * reset);
STATUS m48t37FailsafeCancel (void);
STATUS m48t37FailsafeShow (void);
BOOL   m48t37FailsafeCausedReset (void);

/*
 * the following macros convert from BCD to binary and back.
 * Be careful that the arguments are chars, only char width returned.
 */

#define BCD_TO_BIN(bcd) (( ((((bcd)&0xf0)>>4)*10) + ((bcd)&0xf) ) & 0xff)
#define BIN_TO_BCD(bin) (( (((bin)/10)<<4) + ((bin)%10) ) & 0xff)

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif	/* INCm48t37h */
