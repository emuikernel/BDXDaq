/******************************************************************************
*              (c), Copyright, MOTOROLA INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
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
