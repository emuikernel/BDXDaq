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
* userRsrvMem.c - Simple memory management for the user reserved memory. 
*
* DESCRIPTION:
*	This file introduce a simple memory management for the User Reserved 
*	Memory which size is define in config.h file by USER_RESERVED_MEM macro.
*       The memory management is handled by a global pointer userNextFreeBlkPtr
*	that always points to the next available space within the 
*	User Reserved Memory. The pointer is incremented by the size 
*       of the memory allocation requested by the user.
*
*     	Any interface that uses the user reserved memory must get its memory 
*	allocation using the API supplied in this file, to avoid overriding 
*	data of other applications that might uses the user reserved memory 
*	as well.  
*
* DEPENDENCIES:
*       gtMemory module.
*		
******************************************************************************/

/* includes */
#include "vxWorks.h"
#include "config.h"
#include "sysLib.h"
#include "userRsrvMem.h"

#ifdef PMC280_DUAL_CPU
#include "gtSmp.h"
#endif /* PMC280_DUAL_CPU */

/* defines */

/* globals */

static unsigned int userNextFreeBlkPtr;	/* Next available memory allocation  */
static unsigned int userRsrvMemTop;		/* Top address of User Reserved Mem  */
static bool	userRsrvMemInitFlag = false;

/*******************************************************************************
* userRsrvMemInit - Initializes the User Reserved Memory management.
*
* DESCRIPTION:
*       This function initialize the global internal free block pointer 
*	userNextFreeBlkPtr to the system highest memory.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Driver SW parameters initialize with system information on User 
*		Reserved Memory.
*
* RETURN:
*       None.
*
*******************************************************************************/
bool userRsrvMemInit(void)
{
    if(userRsrvMemInitFlag == true)
    {
	return userRsrvMemInitFlag;
    }

#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        if((userNextFreeBlkPtr = ((unsigned int)sysMemTop() + (SYS_DRV_SIZE/2))) == 0x0)
	    return false;

        userRsrvMemTop     = userNextFreeBlkPtr + (SYS_DRV_SIZE/2);
    }
    else
    {
        if((userNextFreeBlkPtr = (unsigned int)sysMemTop()) == 0x0)
	    return false;

        userRsrvMemTop     = userNextFreeBlkPtr + (SYS_DRV_SIZE/2);
    } 
#else
    if((userNextFreeBlkPtr = (unsigned int)sysMemTop()) == 0x0)
	return false;

    userRsrvMemTop     = userNextFreeBlkPtr + USER_RESERVED_MEM;
#endif /* PMC280_DUAL_CPU */

    userRsrvMemInitFlag = true;

    return userRsrvMemInitFlag;
}

/*******************************************************************************
* userRsrvMemMalloc - Allocates memory within User Reserved Memory.
*
* DESCRIPTION:
* 	This routine allocates a block of memory from User Reserved Memory. 
*	The size of the block will be equal to or greater than nBytes. 						
*  
* INPUT:
	unsigned int nBytes 	number of bytes to allocate
*
* OUTPUT:
*       None.
*
* RETURN:
*       unsigned int base address of the allocated memory within User 
*	Reserved Memory.
*	The function returns 0 if the allocation failed.
*
*******************************************************************************/
unsigned int  userRsrvMemMalloc(unsigned int nBytes)
{
    unsigned int returnPtr;
   
	/* Do we have enough room ?? */
	if(userNextFreeBlkPtr + nBytes > userRsrvMemTop)
		return 0;
	
	returnPtr = userNextFreeBlkPtr;
    
	userNextFreeBlkPtr += nBytes;

	return returnPtr;

}
/*******************************************************************************
* userRsrvMemMallocAligned - Allocate's aligned memory in User Reserved Memory.
*
* DESCRIPTION:
* 	This routine allocates a buffer of size nBytes from the User Reserved 
*	Memory. Additionally, it insures that the allocated buffer begins
* 	on a memory address evenly divisible by the specified alignment
* 	parameter. The alignment parameter must be a power of 2.
*	Note: The routine will add alignment size of bytes to the block for 
*		alignment purposes.
*
* INPUT:
*	unsigned int  nBytes 	number of bytes to allocate
*	int 		alignment 	boundary to align to (power of 2) 
*
* OUTPUT:
*       None.
*
* RETURN:
*       unsigned int base address of the allocated memory space within User 
*	Reserved Memory.
*	The function returns 0 if the allocation failed.
*
*******************************************************************************/
unsigned int  userRsrvMemMallocAligned(unsigned int nBytes, int aligment)
{
    unsigned int blkBase, aligmentMask = 0;
	
	/* Add alignment bytes to the block size for alignment space */
	blkBase = userRsrvMemMalloc(nBytes + (1 << aligment));

	/* If malloc is already aligned */
	if(blkBase % (1 << aligment) != 0)
	{
		/* Mask for getting the address low bits for checking the alignment */
		aligmentMask = ~(0xFFFFFFFF << aligment);
		
		/* Add offset to the block base to compete the alignment */
		blkBase += (1 << aligment) - (aligmentMask & blkBase);
	}

	return blkBase;
}

