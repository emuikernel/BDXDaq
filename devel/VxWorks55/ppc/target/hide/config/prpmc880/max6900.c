/* max6900.c - Motorola PRPMC880 board series system-dependent library */

/* Copyright 1996-2003 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01b,02Jun03,simon  updated based on peer review results
01a,04apr03,simon  Ported. from ver 01a, mcp815/max6900.c.
*/

/*
DESCRIPTION
This file contains the following MAX6900 I2C RTC utility routines:

.CS
  PLDI2CWrite     - Write one byte to MAX6900 I2C RTC eeprom
  PLDI2CRead      - Reads one byte from MAX6900 I2C RTC eeprom
  PLDI2CBurstWrite     - Burst mode write to MAX6900 I2C RTC eeprom
  PLDI2CBurstRead      - Burst mode read  from MAX6900 I2C RTC eeprom
.CE


*/

#include "vxWorks.h"
#include "config.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "PLDI2CDrv.h"

/* board dependant I2C defines */

/* The RTC code expects binary values in this structure, not BCD. */

typedef struct rtcdt         /* RTC_DATE_TIME */
    {
	int         year;             /* year */
	int         month;            /* month */
	int         day;              /* day */
	int         hour;             /* hour */
	int         minute;           /* minute */
	int         second;           /* second */
    } RTC_DATE_TIME;

typedef struct max6900time {
   unsigned char sec1;
   unsigned char sec10;
   unsigned char min1;
   unsigned char min10;
   unsigned char hr1;
   unsigned char hr10;
   unsigned char date1;
   unsigned char date10;
   unsigned char month1;
   unsigned char month10;
   unsigned char weekday;
   unsigned char year1;
   unsigned char year10;
   unsigned char year100;
   unsigned char year1000;
   unsigned char ampm;
   unsigned char hr12or24;
}max6900time_t;

STATUS sysRtcDateTimeTest(RTC_DATE_TIME *timecal);
STATUS sysRtcDateTimeSet(RTC_DATE_TIME *timecal);
STATUS sysRtcDateTimePrint(RTC_DATE_TIME *timecal);
STATUS sysRtcDateTimeGet(RTC_DATE_TIME *timecal);

IMPORT void   sysMotMsDelay(UINT32);
LOCAL void str2time(unsigned char* src, max6900time_t* dest);
LOCAL void time2str( max6900time_t* src, unsigned char* dest);

/***************************************************************************    
*
* sysRtcDateTimeGet - get current date and time from max6900.
*
* This routine read the register in max6900 first then translate it into date and time 
*
* RETURNS: timecal .
* ERROR if do not find max6900 chip or max6900 report error.
*/

STATUS sysRtcDateTimeGet(RTC_DATE_TIME *timecal)
{
	unsigned char pBuf[16]={0,};
	max6900time_t info;

	if( PLDI2CBurstRead(MAX6900__RTC_ADDR, pBuf) != OK ){
		printf("PLDI2CBurstRead error.\r\n");
		return (ERROR);
		}
	if( PLDI2CRead(MAX6900__RTC_ADDR, 0x93,&pBuf[9]) !=OK ){
		printf("PLDI2CRead error.\r\n");
		return (ERROR);
		} 
	str2time(pBuf, &info);
	timecal->year = info.year1000 * 1000 + info.year100 * 100 + \
					info.year10 * 10 + info.year1;
	timecal->month = info.month10 * 10 + info.month1;
	timecal->day = info.date10 * 10 + info.date1;
	timecal->hour = info.hr10 * 10 + info.hr1;
	timecal->minute = info.min10 * 10 + info.min1;
	timecal->second = info.sec10 * 10 + info.sec1;

	return OK;
}

/***************************************************************************    
*
* sysRtcDateTimeSet - set current date and time into max6900.
*
* This routine translate input date and time then set it into max6900 register 
*
* RETURNS: none .
* ERROR if input date is not right or do not find max6900 chip or max6900 report error.
*/

STATUS sysRtcDateTimeSet(RTC_DATE_TIME *timecal)
{
	unsigned char pBuffer[16]={0,};			/* RAM "download" buffer */
	max6900time_t info;

        /* Determine whether date/time values are valid */
   
        if (sysRtcDateTimeTest(timecal) == ERROR)
            {
            return (ERROR);
            }

	info.date1 = (timecal->day) % 10;
	info.date10 = (timecal->day) / 10;
	info.hr1 = (timecal->hour) % 10;
	info.hr10 = (timecal->hour) / 10;
	info.min1 = (timecal->minute) % 10;
	info.min10 = (timecal->minute) / 10;
	info.month1 = (timecal->month) % 10;
	info.month10 = (timecal->month) / 10;
	info.sec1 = (timecal->second) % 10;
	info.sec10 = (timecal->second) / 10;
	info.year1000 = (timecal->year) / 1000;
	info.year100 = (timecal->year - info.year1000 * 1000) / 100;
	info.year10 = (timecal->year - info.year1000 * 1000 - info.year100 * 100) / 10;
	info.year1 = (timecal->year) % 10;
	info.hr12or24 = 0;   /* 24 */
	
	time2str(&info, pBuffer);

	if( PLDI2CBurstWrite(MAX6900__RTC_ADDR, pBuffer) != OK )
		return (ERROR);
	if( PLDI2CWrite(MAX6900__RTC_ADDR, 0x92, &pBuffer[9]) !=OK )
		return (ERROR); 
	
	return OK;
}

/***************************************************************************    
*
* sysRtcDateTimePrint - print input date and time .
*
* This routine print input date and time
*
* RETURNS: none .
*/

STATUS sysRtcDateTimePrint(RTC_DATE_TIME *timecal)
{
    RTC_DATE_TIME * rtc_time = timecal;	/* RTC date and time */

    /* Send the date and time to the user's display */

    if (rtc_time->year < 10)
       {
       printf ("\n\r            Time: %d:%d:%d  Date: %d/%d/%d\n\r",
               rtc_time->hour, rtc_time->minute, rtc_time->second,
               rtc_time->day, rtc_time->month, rtc_time->year);
       }
    else
       {
       printf ("\n\r            Time: %d:%d:%d  Date: %d/%d/%d\n\r",
               rtc_time->hour, rtc_time->minute, rtc_time->second,
               rtc_time->day, rtc_time->month, rtc_time->year);
       }

    return (OK);
    }

/***************************************************************************    
*
* sysRtcDateTimeTest - test the input data is good or not.
*
* This routine test the input data is good or not.
*
* RETURNS: none .
* ERROR if input date is not right.
*/

STATUS sysRtcDateTimeTest(RTC_DATE_TIME *timecal)
{

    /* Check validity of seconds value */

    if (timecal->second < 0 || timecal->second > 59)
        {
	return (ERROR);
        }
    
    /* Check validity of minutes value */

    if (timecal->minute < 0 || timecal->minute > 59)
        {
	return (ERROR);
        }

    /* Check validity of hours value */

    if (timecal->hour < 0 || timecal->hour > 23)
        {
	return (ERROR);
        }

    /* Check validity of day of month value */

    if (timecal->day < 1 || timecal->day > 31)
        {
	return (ERROR);
        }

    /* Check validity of month value */

    if (timecal->month < 1 || timecal->month > 12)
        {
	return (ERROR);
        }

    /* Check validity of year value */

    if (timecal->year < 0 || timecal->year > 2100)
        {
	return (ERROR);
        }

    return (OK);
 
    }

/***************************************************************************    
*
* str2time - translate string into time.
*
* This routine translate string into time.
*
* RETURNS: none .
*/

LOCAL void str2time(unsigned char* src, max6900time_t* dest)
{
	dest->sec1 = (*src)&0xf;
	dest->sec10 = ((*src)>>4)&0x7;
	dest->min1 = (*(src+1))&0xf;
	dest->min10 = ((*(src+1))>>4)&0x7;
	dest->hr1 = (*(src+2))&0xf;
	if(!(dest->hr12or24 = ((*(src+2))>>7)&0x1)){
		dest->hr10 = ((*(src+2))>>4)&0x3;
		dest->ampm = 0;
	}
	else{
		dest->hr10 = ((*(src+2))>>4)&0x1;
		dest->ampm = ((*(src+2))>>5)&0x1;
	}
	
	dest->date1 = (*(src+3))&0xf;
	dest->date10 = ((*(src+3))>>4)&0x3;
	dest->month1 = (*(src+4))&0xf;
	dest->month10 = ((*(src+4))>>4)&0x1;
	dest->weekday = (*(src+5))&0x7;
	
	dest->year1 = (*(src+6))&0xf;
	dest->year10 = ((*(src+6))>>4)&0xf;
	dest->year100 = (*(src+9))&0xf;
	dest->year1000 = ((*(src+9))>>4)&0x7;
	return;
}

/***************************************************************************    
*
* time2str - translate time into string.
*
* This routine translate time into string.
*
* RETURNS: none .
*/

LOCAL void time2str( max6900time_t* src, unsigned char* dest)
{
	*dest = (src->sec1) | (src->sec10 << 4);
	*(dest+1) = (src->min1) | (src->min10 << 4);
	if(src->hr12or24){
		*(dest+2) = (src->hr1) | (src->hr10 << 4) | (src->ampm <<5) | (src->hr12or24 <<7);
	}else{
		*(dest+2) = (src->hr1) | (src->hr10 << 4);
	}
	*(dest+3) = (src->date1) | (src->date10 << 4);
	*(dest+4) = (src->month1) | (src->month10 << 4);
	*(dest+5) = src->weekday;
	*(dest+6) = (src->year1) | (src->year10 << 4);
	*(dest+9) = (src->year100) | (src->year1000 << 4);
	
	return;
}
