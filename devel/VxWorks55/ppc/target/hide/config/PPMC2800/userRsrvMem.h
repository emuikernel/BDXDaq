/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* userRsrvMem.h - User Reserved Memory management header file.
*
* DESCRIPTION                                                                 
*       This file contain the declarations and data structure (if any) for 
*       User Reserved Memory simple memory management. 
*
* DEPENDENCIES:
*       None
*
******************************************************************************/

#ifndef __INCuserRsrvMemh
#define __INCuserRsrvMemh

/* includes */
#include "gtCore.h"

/* defines  */


/* typedefs */


/* gtSram.h API list */
bool          userRsrvMemInit(void);
unsigned int  userRsrvMemMalloc(unsigned int nBytes);
unsigned int  userRsrvMemMallocAligned(unsigned int nBytes, int aligment);

#endif /* __INCuserRsrvMemh */
