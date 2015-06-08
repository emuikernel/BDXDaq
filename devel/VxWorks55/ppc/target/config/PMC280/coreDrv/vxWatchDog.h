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
* vxWatchDog.h - Header File for : GT Watchdog Timer Driver
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCvxWatchDogh
#define __INCvxWatchDogh

/* includes */
#include <vxWorks.h>
#include "gtCore.h"

/* defines  */
#define PRESET_VALUE_FIELD    0xFFFFFF     /* preset value field    */
#define CONTROL1_FIELD        0x3000000    /* CTL1 field bits 24:25 */
#define CONTROL2_FIELD        0xC000000    /* CTL1 field bits 26:27 */
#define WD_ENABLE_BIT         0x80000000   /* WD enable bit 31      */

#define ENB_DSB1    0x1000000   /* Enable / Disable sequance. first write 01*/
#define ENB_DSB2    0x2000000   /* Enable / Disable sequance. second write 10*/

#define SERVICE1    0x4000000   /* Service sequance. first write 01*/
#define SERVICE2    0x8000000   /* Service sequance. second write 10*/

#define MPP24                   0x0F000000
#define WD_SERVICE_PRIORITY     0

/* typedefs */
typedef unsigned int    WATCHDOG_CONFIG;

/* globals */

/* vxWatchDog.h API list */
void frcWatchdogInit   (void);
bool frcWatchdogLoad   (unsigned int value); 
bool frcWatchdogNMILoad(unsigned int value);  
void frcWatchdogService(void);  
void frcWatchdogEnable (void);   
void frcWatchdogDisable(void);  

#endif /* __INCvxWatchDogh */

