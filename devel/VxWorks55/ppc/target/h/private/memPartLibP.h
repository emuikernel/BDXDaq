/* memPartLibP.h - private memory management library header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */
/*
modification history
--------------------
01h,23oct01,tam  re-organized FREE_BLOCK
01g,19oct01,tam  added definition for KMEM_XXX macros
01f,25sep01,tam  added definition of the KHEAP_XXX macros
01e,19aug96,dbt  removed memPartAlignedAlloc prototype (fixed SPR #6898).
01e,05aug96,kkk  made padding in BLOCK_HDR generic to all arch with alignment
		 at 16.
01d,22sep92,rrr  added support for c++
01c,28jul92,jcf  added external declaration for memPartOptionsDefault.
01b,19jul92,pme  added external declarations for sm partition functions.
01a,01jul92,jcf  extracted from memLib v3r.
*/

#ifndef __INCmemPartLibPh
#define __INCmemPartLibPh

#ifdef __cplusplus
extern "C" {
#endif

#include "vwModNum.h"
#include "memLib.h"
#include "classLib.h"
#include "dllLib.h"
#include "private/semLibP.h"
#include "private/objLibP.h"


/* macros for getting to next and previous blocks */

#define NEXT_HDR(pHdr)  ((BLOCK_HDR *) ((char *) (pHdr) + (2 * (pHdr)->nWords)))
#define PREV_HDR(pHdr)	((pHdr)->pPrevHdr)


/* macros for converting between the "block" that caller knows
 * (actual available data area) and the block header in front of it */

#define HDR_TO_BLOCK(pHdr)	((char *) ((int) pHdr + sizeof (BLOCK_HDR)))
#define BLOCK_TO_HDR(pBlock)	((BLOCK_HDR *) ((int) pBlock - \
						sizeof(BLOCK_HDR)))


/* macros for converting between the "node" that is strung on the freelist
 * and the block header in front of it */

#define HDR_TO_NODE(pHdr)	(& ((FREE_BLOCK *) pHdr)->node)
#define NODE_TO_HDR(pNode)	((BLOCK_HDR *) ((int) pNode - \
						OFFSET (FREE_BLOCK, node)))

/*
 * Kernel memory allocation macros (KHEAP and KMEM macros)
 *
 * NOTE: They should not be used by any application code.
 * It is also needed by code being back-ported from VxWorks AE 1.x. to VxWorks
 * 5.5.x.
 */

/*
 * KHEAP macros:
 * These macros are defined simply for cosmetic reasons, to make it more 
 * obvious that buffers are allocated or freed from the kernel heap, also 
 * called the system memory partition.
 * No guaranty is given that the buffer allocated from the Kernel
 * Heap via KHEAP_ALLOC has a known and constant virtual to physical mapping
 * (1 to 1 mapping for instance) if virtual memory support is included (i.e.
 * INCLUDE_MMU_BASIC or INCLUDE_MMU_FULL components).
 *
 * NOTE: KHEAP_ALIGNED_ALLOC() uses memPartAlignedAlloc() instead of memalign()
 *       to prevent any requirements on INCLUDE_MEM_MGR_FULL when using the 
 *	 macro KHEAP_ALIGNED_ALLOC.
 */

#define KHEAP_ALLOC(nBytes)                                             \
        malloc((nBytes))
#define KHEAP_FREE(pChar)                                               \
        free((pChar))
#define KHEAP_ALIGNED_ALLOC(nBytes, alignment)                          \
        memPartAlignedAlloc(memSysPartId, (nBytes), (alignment))
#define KHEAP_REALLOC(pChar, newSize)                                    \
        memPartRealloc(memSysPartId, (pChar), (newSize))


/*
 * KMEM macros:
 * If a known and constant virtual to physical mapping is required for buffer
 * allocated, KMEM macros should be used instead of KHEAP macros.
 * For instance memory block dynamically allocated that may be accessed when
 * the processor MMU is turn off or on should always be allocated using 
 * KMEM_ALLOC() or KMEM_ALIGNED_ALLOC(). Good examples are memory blocks used 
 * to store MMU translation information on some processor, or memory blocks
 * accessed by DMA devices.
 * Good care should be taken before using KMEM macros. If no constant virtual 
 * to physical mapping is required, then KHEAP macros should always be used 
 * instead.
 *
 * NOTE: With VxWorks 5.5.x, there's no differences between KHEAP_XXX and 
 * KMEM_XXX macros. However this will change in future releases.
 */

#define KMEM_ALLOC(nBytes)      					\
	memPartAlloc(memSysPartId, (nBytes))
#define KMEM_FREE(pChar)						\
	memPartFree(memSysPartId, (pChar))
#define KMEM_ALIGNED_ALLOC(nBytes, alignment)				\
        memPartAlignedAlloc(memSysPartId, (nBytes), (alignment))


#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1			/* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/* typedefs */

typedef struct mem_part
    {
    OBJ_CORE	objCore;		/* object management */
    DL_LIST	freeList;		/* list of free blocks */
    SEMAPHORE	sem;			/* partition semaphore */
    unsigned	totalWords;		/* total number of words in pool */
    unsigned	minBlockWords;		/* min blk size in words includes hdr */
    unsigned	options;		/* options */

    /* allocation statistics */

    unsigned curBlocksAllocated;	/* current # of blocks allocated */
    unsigned curWordsAllocated;		/* current # of words allocated */
    unsigned cumBlocksAllocated;	/* cumulative # of blocks allocated */
    unsigned cumWordsAllocated;		/* cumulative # of words allocated */

    } PARTITION;

/* 
 * Allocated block header. 
 * NOTE: The size of this data structure must be aligned on _ALLOC_ALIGN_SIZE.
 */

typedef struct blockHdr		/* BLOCK_HDR */
    {
    struct blockHdr *	pPrevHdr;	/* pointer to previous block hdr */
    unsigned		nWords : 31;	/* size in words of this block */
    unsigned		free   : 1;	/* TRUE = this block is free */
#if (_ALLOC_ALIGN_SIZE == 16)
    UINT32		pad[2];		/* 8 byte pad for round up */
#endif	/* _ALLOC_ALIGN_SIZE == 16 */
    } BLOCK_HDR;

/* 
 * Free block header. 
 * NOTE: The size of this data structure is already aligned on 
 * _ALLOC_ALIGN_SIZE, for 4, 8, and 16 bytes alignments.
 */

typedef struct			/* FREE_BLOCK */
    {
    struct
        {
	struct blockHdr *   pPrevHdr;	/* pointer to previous block hdr */
	unsigned	    nWords : 31;/* size in words of this block */
	unsigned	    free   : 1;	/* TRUE = this block is free */
	} hdr;
    DL_NODE		node;		/* freelist links */
    } FREE_BLOCK;

/* variable declarations */

extern CLASS_ID memPartClassId;		/* memory partition class id */
extern FUNCPTR  memPartBlockErrorRtn;	/* block error method */
extern FUNCPTR  memPartAllocErrorRtn;	/* alloc error method */
extern FUNCPTR  memPartSemInitRtn;	/* partition semaphore init method */
extern unsigned	memPartOptionsDefault;	/* default partition options */
extern UINT	memDefaultAlignment;	/* default alignment */
extern int	mutexOptionsMemLib;	/* mutex options */

/* shared memory manager function pointers */

extern FUNCPTR  smMemPartAddToPoolRtn;
extern FUNCPTR  smMemPartFreeRtn;
extern FUNCPTR  smMemPartAllocRtn;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0			/* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern BOOL	memPartBlockIsValid (PART_ID partId, BLOCK_HDR *pHdr,
				     BOOL isFree);

#else	/* __STDC__ */

extern BOOL	memPartBlockIsValid ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCmemPartLibPh */
