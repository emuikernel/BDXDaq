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
* gtCore.c - GT Internal Register Base Address
*
* DESCRIPTION                                                                 
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include "gtCore.h"

/* defines  */

/* typedefs */

/* locals   */
unsigned int gtInternalRegBaseAddr = 0xf1000000;

/*******************************************************************************
* __eabi - Basic HW initialization according the EABI standard.
*
* DESCRIPTION:
*       This function is invoked by 'main' (inserted by the GNU EABI 
*       compiler automaticly) and provides the basic initialization to the 
*       system HW (CPU, memory etc.) according to the EABI standard. Since 
*       the monitor (DINK/PMON) already provide these initializations at 
*       boot-time, this function has an empty implementation to satisfy the
*       GNU EABI compiler/linker. 
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
int __eabi(void)
{ 
    /* No further initialization is required.
       For any additional HW initialization that 
       is not accomplished by the monitor, simply
       enter the code here.*/
    return 0; 
}

