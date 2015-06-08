/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* vxTimestamp.h - Header File for : user-defined timestamp header
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCvxTimestamph
#define __INCvxTimestamph

/* includes */
#include <vxWorks.h>


/* defines  */


/* typedefs */


/* user definition timestamp routine */
#ifdef  INCLUDE_USER_TIMESTAMP
   #define  USER_TIMESTAMP     vxTimestamp
   #define  USER_TIMESTAMPLOCK vxTimestampLock
   #define  USER_TIMEENABLE    vxTimestampEnable
   #define  USER_TIMEDISABLE   vxTimestampDisable
   #define  USER_TIMECONNECT   vxTimestampConnect
   #define  USER_TIMEPERIOD    vxTimestampPeriod
   #define  USER_TIMEFREQ      vxTimestampFreq
#endif

/* vxTimestamp.h - user-defined timestamp header */
#ifndef _ASMLANGUAGE

STATUS vxTimestampConnect (VOIDFUNCPTR, int);
STATUS vxTimestampEnable  (void);
STATUS vxTimestampDisable (void);
UINT32 vxTimestampPeriod  (void);
UINT32 vxTimestampFreq    (void);
UINT32 vxTimestamp        (void);
UINT32 vxTimestampLock    (void);

#endif /* _ASMLANGUAGE */

#endif /* __INCvxTimestamph */
