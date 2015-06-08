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
