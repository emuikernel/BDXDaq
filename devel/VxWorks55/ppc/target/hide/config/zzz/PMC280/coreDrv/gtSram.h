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
