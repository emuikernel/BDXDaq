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
* vxCunitIntCtrl.h - Header File for : C-Unit interrupt controller driver
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCvxCunitIntCtrlh
#define __INCvxCunitIntCtrlh

/* includes */
#include "gtIntControl.h"
#include "mpsc.h"


/* defines  */
#define CAUSE_OFFSET	16

/* typedefs */

/* CUNIT_CAUSE is an enumerator that describes the cause bits positions 	*/
/* in the C-Unit Error Report cause registers.						        */

typedef enum _cunitCause
{
    CUNIT_CAUSE_START = -1			, /* Cause Start boundary 				*/
    CUNIT_ADDR_MISS      			, /* Failed address decoding.           */
    CUNIT_ACCESS_PROTECT_VIOLATION 	, /* Access Protect Violation.			*/
    CUNIT_WRITE_PROTECT_VIOLATION  	, /* Write Protect Violation.			*/
    CUNIT_CAUSE_END   				  /* Cause End boundary 				*/
} CUNIT_CAUSE;

/* vxCunitIntCtrl.h API list */
void    vxCunitIntCtrlInit (void); 
STATUS  vxCunitIntConnect  (MPSC_NUM mpscNum, 
							CUNIT_CAUSE cause, 
							VOIDFUNCPTR routine,
							int parameter, 
							int prio);
STATUS  vxCunitIntEnable   (MPSC_NUM mpscNum, CUNIT_CAUSE cause);
STATUS  vxCunitIntDisable  (MPSC_NUM mpscNum, CUNIT_CAUSE cause);
STATUS  vxCunitIntClear    (MPSC_NUM mpscNum, CUNIT_CAUSE cause);

#endif /* __INCvxCunitIntCtrlh */

