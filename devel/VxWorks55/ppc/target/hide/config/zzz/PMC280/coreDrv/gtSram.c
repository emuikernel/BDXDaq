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
* gtSRAM.c - Simple memory management for the internal SRAM. 
*
* DESCRIPTION:
*		This file introduce a simple memory management for the internal SRAM.
*     	Any interface that uses the SRAM must get its memory allocation
*     	using the API supplied in this file, to avoid overriding data of other 
*     	applications that might uses the SRAM as well.  
*
* DEPENDENCIES:
*       gtMemory module.
*
******************************************************************************/

/* includes */
#include "gtMemory.h"
#include "gtSram.h"

/* globals */

unsigned int nextFreeBlkPtr = 0;		/* next available memory allocation */
unsigned int internalSramTopAddr = 0;   /* Internal SRAM top address */

/*******************************************************************************
* gtSramInit - Initializes the SRAM's simple memory management.
*
* DESCRIPTION:
*       The SRAM's memory management is handled by a global pointer 
*       (nextFreeBlkPtr) that allways points to the next availiable 
*       space within the internal SRAM. The pointer is incremented by the size 
*       of the memory allocation requested by the user. This function 
*       initializes the SRAM's simple memory management by simply setting the 
*       next availiable space pointer to the internal SRAM's base address.
*
* NOTE: 
*       This function must be called once BEFORE using the SRAM malloc service.
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
void gtSramInit(void)
{
    nextFreeBlkPtr = gtMemoryGetInternalSramBaseAddr();
    internalSramTopAddr = nextFreeBlkPtr + INTERNAL_SRAM_SIZE;
}

/*******************************************************************************
* gtSramMalloc - Allocate's memory within the internal SRAM.
*
* DESCRIPTION:
* 		This routine allocates a block of memory from the SRAM. The size of 		
* 		the block will be equal to or greater than nBytes. 						
*  
* INPUT:
		unsigned int nBytes 	number of bytes to allocate
*
* OUTPUT:
*       None.
*
* RETURN:
*       unsigned int base address of the allocated memory space within SRAM.
*
*******************************************************************************/
unsigned int  gtSramMalloc(unsigned int nBytes)
{
    unsigned int returnPtr;

	if(nextFreeBlkPtr + nBytes > INTERNAL_SRAM_TOP_ADDRESS)
		return 0;
	returnPtr = nextFreeBlkPtr;
	nextFreeBlkPtr += nBytes;
	return returnPtr;
}

/*******************************************************************************
* gtSramMallocAligned - Allocate's aligned memory within the internal SRAM.
*
* DESCRIPTION:
* 		This routine allocates a buffer of size nBytes from the SRAM memory
* 		partition. Additionally, it insures that the allocated buffer begins
* 		on a memory address evenly divisible by the specified alignment
* 		parameter. The alignment parameter must be a power of 2.
*		Note: The routine will add aligment size of bytes to the block for 
*			aligment purposes.
*
* INPUT:
*		nBytes 	 - number of bytes to allocate.
*		aligment - boundary to align to (power of 2). 
*
* OUTPUT:
*       None.
*
* RETURN:
*       The base address of the allocated memory space within SRAM.
*
*******************************************************************************/
unsigned int  gtSramMallocAligned(unsigned int nBytes, int aligment)
{
    unsigned int blkBase, aligmentMask = 0;
	
	/* Add aligment bytes to the block size for aligment space */
	blkBase = gtSramMalloc(nBytes + (1 << aligment));
	/* If malloc is already aligned */
	if(blkBase % (1 << aligment) != 0)
	{
		/* Mask for getting the address low bits for checking the aligment */
		aligmentMask = ~(0xFFFFFFFF << aligment);
		/* Add offset to the block base to complete the aligment */
		blkBase += (1 << aligment) - (aligmentMask & blkBase);
	}
	return blkBase;
}

