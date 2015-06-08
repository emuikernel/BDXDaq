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
* gtSRAM.h - The internal SRAM memory management header file.
*
* DESCRIPTION                                                                 
*       This file contain the declarations and data structure (if any) for 
*       SRAM's simple memory management. 
*
* DEPENDENCIES:
*       None
*
******************************************************************************/

#ifndef __INCgtSramh
#define __INCgtSramh

/* includes */


/* defines  */

#define INTERNAL_SRAM_SIZE	        _256K	/* Internal SRAM size */
#define INTERNAL_SRAM_TOP_ADDRESS	internalSramTopAddr	

/* typedefs */


/* gtSram.h API list */
void          gtSramInit(void);
unsigned int  gtSramMalloc(unsigned int nBytes);
unsigned int  gtSramMallocAligned(unsigned int nBytes, int aligment);

#endif /* __INCgtSramh */
