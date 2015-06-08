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
* gtSysGal.h - header file for: specific BSP and DB-64360 board use
*
* DESCRIPTION:
*       This file contain:
*       - specific includes heder files for the DB-64360 board
*       - defines used by the BSP
*       - functions used by the BSP
*
* DEPENDENCIES:
*       the API functions sould be in sysLib.c.
*
******************************************************************************/

#ifndef __INCgtSysGalh
#define __INCgtSysGalh

/* includes */
#include "./coreDrv/gt64360r.h"
#include "pmc280.h"

/* defines  */
#ifndef _ASMLANGUAGE

#define NONE_CACHEABLE(address)	(address)  
#define CACHEABLE(address)		(address)  

#define REG_ADDR(offset)      (NONE_CACHEABLE(gtInternalRegBaseAddr | (offset)))
#define REG_CONTENT(offset)   ((volatile unsigned int)                         \
                                               *(unsigned int*)REG_ADDR(offset))

#define VIRTUAL_TO_PHY(address) ((unsigned int)address)
#define PHY_TO_VIRTUAL(address) ((unsigned int)address)

#define D_CACHE_FLUSH_LINE(addr, offset)							\
{  																	\
  __asm__ __volatile__ ("dcbf %0,%1" : : "r" (addr), "r" (offset));	\
}

#define CPU_PIPE_FLUSH 												\
{																	\
  __asm__ __volatile__ ("eieio");									\
}

static __inline__ __const__ unsigned int WORD_SWAP_FAST(unsigned int value)
{
  unsigned int result;
  
  __asm__("rlwimi %0,%1,24,16,23\n\t"
          "rlwimi %0,%1,8,8,15\n\t"
          "rlwimi %0,%1,24,0,7"
          : "=r" (result)
          : "r" (value), "0" ((value) >> 24));
  return result;
}

/* GT load short */
static __inline__ unsigned short gt_ld_le16(const volatile unsigned short *addr)
{
	unsigned short val;

	__asm__ __volatile__ ("lhbrx %0,0,%1" : "=r"(val) : "r"(addr), "m"(*addr));
	return val;
}  
 
/* GT store short */
static __inline__ void gt_st_le16(volatile unsigned short *addr, 
								  const unsigned short val)
{
    __asm__ __volatile__ ("sthbrx %1,0,%2" : "=m"(*addr) : "r"(val), "r"(addr));
}

/* GT load unsigned int */
static __inline__ unsigned int gt_ld_le32(const volatile unsigned int *addr)
{
	unsigned int val;

    __asm__ __volatile__ ("lwbrx %0,0,%1" : "=r"(val) : "r"(addr), "m"(*addr));
    return val;
}

/* GT store unsigned int */
static __inline__ void gt_st_le32(volatile unsigned int *addr, 
								  const unsigned int val)
{
    __asm__ __volatile__ ("stwbrx %1,0,%2" : "=m"(*addr) : "r"(val), "r"(addr));
}

#ifndef LE /* Little Endian */          	
	
	/* Read/Write to/from GT`s internal registers */
	#define GT_REG_READ(offset, pData)                                         \
		    (*(pData) = gt_ld_le32((volatile unsigned int *)(REG_ADDR(offset))))

	#define GTREGREAD(offset)                                                  \
            gt_ld_le32( (const volatile unsigned int *)(REG_ADDR(offset)) )

	#define GT_REG_WRITE(offset, data)                                         \
		    gt_st_le32 ( ((volatile unsigned int *)(REG_ADDR(offset))), (data) )

#endif /* LE */

/* typedefs */

/* API list for function in syslib.c */
void gtRegReadBlock  (unsigned int offset, unsigned int *block, 
                      unsigned int length);

void gtRegWriteBlock (unsigned int offset, unsigned int *block, 
					  unsigned int length);

void memoryRead      (unsigned int memOffset, unsigned int *data,
				      unsigned int memSize);

unsigned int sysVirtToPhys(unsigned int addr);
unsigned int sysPhysToVirt(unsigned int addr);

#endif /* _ASMLANGUAGE */

#endif  /* __INCgtSysGalh */

