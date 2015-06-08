/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* vxDbIntCtrl.h - Header File for : Doorbell interrupt driver
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCvxDbIntCtrlh
#define __INCvxDbIntCtrlh

/* includes */
#include "gtIntControl.h"


/* defines  */


/* typedefs */

/* DB_CAUSE is an enumerator that describes the Doorbell causes. 
 * The enumerator describes the cause bits positions in the Doorbell registers.
 */

typedef enum _dbCause
{
    DB_CAUSE_START = -1,  /* Cause Start boundry  */
    DB_0      , 	  /* Doorbell 0           */
    DB_1      , 	  /* Doorbell 1           */
    DB_2      , 	  /* Doorbell 2           */
    DB_3      , 	  /* Doorbell 3           */
    DB_4      , 	  /* Doorbell 4           */
    DB_5      , 	  /* Doorbell 5           */
    DB_6      , 	  /* Doorbell 6           */
    DB_7      , 	  /* Doorbell 7           */
    DB_CAUSE_END   	  /* Cause End boundry 	  */
} DB_CAUSE;

/* vxDmaIntCtrl.h API list */
void    frcDbIntCtrlInit (void); 
STATUS  frcDbIntConnect  (DB_CAUSE cause, 
                         VOIDFUNCPTR routine,
                         int parameter, 
                         int prio);
STATUS  frcDbIntEnable   (DB_CAUSE cause);
STATUS  frcDbIntDisable  (DB_CAUSE cause);
STATUS  frcDbIntClear    (DB_CAUSE cause);
STATUS  frcDbIntSend     (DB_CAUSE cause);

#endif /* __INCvxDbIntCtrlh */

