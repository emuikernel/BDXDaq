/* tgtmem.h - target memory manager */

/* Copyright 1993-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
02i,18sep01,c_c  Added options to memory partitions (SPR #26645).
02h,17sep98,c_c  Fixed target server -m option.
02g,02mar98,pcn  WTX 2: added tgtMemWidthRead, tgtMemWidthWrite.
02f,21jan98,c_c  DLLized target server implementation.
		 Protected Globals with routines.
02e,29jan96,pad  added tgtMemAlignGet() interface.
02d,20oct95,jcf  added tgtMemCacheMax to cap memory cache usage.
02c,10aug95,p_m  added tgtMemMove() prototype (SPR# 4532).
02b,24mar95,p_m  fixed tgtMemAlign() prototype.
02a,22mar95,jcf  rewritten.  Block headers on host.
01f,01mar95,pad  removed old style error codes.
01e,03feb95,p_m  replaced tgtMemZero() with tgtMemSet(). Removed non ANSI 
		 prototypes.
01d,20jan95,jcf  made more portable.
01c,14dec94,p_m  added tgtMemPartInfoGet() prototype and MEM_INFO structure.
01b,13nov94,pad  added READ_ONLY_NOSYNC and NO_ATTRIB_NOSYNC attributes.
01a,24sep93,jcf  written.
*/

#ifndef __INCtgtMemLibh
#define __INCtgtMemLibh

#include "objLib.h"
#include "dllLib.h"
#include "memdesc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* macros for getting to next and previous block headers and
 * macros for converting between the "node" and the header in front of it */

#define NEXT_HDR(pHdr)  	((BLOCK_HDR *) (DLL_NEXT (&pHdr->hdrNode)))
#define PREV_HDR(pHdr)  	((BLOCK_HDR *) (DLL_PREVIOUS (&pHdr->hdrNode)))
#define HDR_TO_HNODE(pHdr)	(&pHdr->hdrNode)
#define HNODE_TO_HDR(pNode)	((BLOCK_HDR *) \
				 ((int) pNode - OFFSET (BLOCK_HDR, hdrNode)))

/* macro for converting between the header and the "block" to which it refers */

#define HDR_TO_BLOCK(pHdr)	((char *) pHdr->base)

/* macros for converting between the "node" that is strung on the freelist
 * and the block header in front of it */

#define HDR_TO_FNODE(pHdr)	(&pHdr->freeNode)
#define FNODE_TO_HDR(pNode)	((BLOCK_HDR *) \
				 ((int) pNode - OFFSET (BLOCK_HDR, freeNode)))

/* Memory partition options */

#define TGT_MEM_NO_GROWTH	1	/* No automatic growth allowed        */

/* typedefs */

typedef enum			/* MEM_ATTRIB */
    {
    MEM_NONE,				/* not cached */
    MEM_NO_WRITE,			/* region not writable */
    MEM_HOST,				/* reads/writes cached in host copy */
    MEM_TEXT				/* reads cached from host copy */
    } MEM_ATTRIB;

typedef void *REMPTR;		/* REMPTR */

typedef struct mem_part *PART_ID;

typedef struct mem_part
    {
    OBJ_CORE		objCore;	/* object management */
    DL_LIST		hdrList;	/* list of all headers */
    DL_LIST		freeList;	/* list of free blocks */
    MEM_DESC_HEAD	allocList;	/* list of free blocks */
    unsigned		totalWords;	/* total number of words in pool */
    unsigned		minBlockWords;	/* min blk size in words includes hdr */
    unsigned		options;	/* options */

    /* allocation statistics */

    unsigned 	curBlocksAllocated;	/* current # of blocks allocated */
    unsigned 	curWordsAllocated;	/* current # of words allocated */
    unsigned 	cumBlocksAllocated;	/* cumulative # of blocks allocated */
    unsigned 	cumWordsAllocated;	/* cumulative # of words allocated */
    } PARTITION;

typedef struct mem_info		/* MEM_INFO */
    {
    u_long 	curBytesFree;		/* number of free bytes */
    u_long 	curBytesAllocated;	/* current # of bytes allocated */
    u_long 	cumBytesAllocated;	/* cumulative # of bytes allocated */

    u_long 	curBlocksFree;		/* current # of free blocks */
    u_long 	curBlocksAlloc;		/* current # of blocks allocated */
    u_long 	cumBlocksAlloc;		/* cumulative # of blocks allocated */

    u_long 	avgFreeBlockSize;	/* average size of free blocks */
    u_long 	avgAllocBlockSize;	/* average size of allocated blocks */
    u_long 	cumAvgAllocBlockSize;	/* cum. avg size of alloc. blocks */

    u_long 	biggestBlockSize;	/* size in bytes of biggest block */

    u_long 	reserved1;		/* reserved */
    u_long 	reserved2;		/* reserved */
    u_long 	reserved3;		/* reserved */
    } MEM_INFO;

typedef struct blockHdr		/* BLOCK_HDR */
    {
    DL_NODE		hdrNode;	/* header list links */
    DL_NODE		freeNode;	/* free list links */
    MEM_DESC		allocNode;	/* alloc b-tree links */
    REMPTR		base;		/* remote base of block */
    unsigned		nWords : 31;	/* size in words of this block */
    unsigned		free   : 1;	/* TRUE = this block is free */
    } BLOCK_HDR;

typedef struct			/* BLOCK_DESC */
    {
    MEM_DESC		memDesc;	/* block descriptor */
    REMPTR		base;		/* block pointer */
    void *		pLocal;		/* local cached copy */
    int			nBytes;		/* block size */
    MEM_ATTRIB		attrib;		/* memory attribute */
    } BLOCK_DESC;


/* function declarations */

extern STATUS 	tgtMemLibInit (void);

extern void	tgtMemCacheMaxSet
    (
    int		value
    );

extern STATUS 	tgtMemInit 
    (
    REMPTR	pPool, 
    unsigned	poolSize,
    UINT32	options
    );

extern PART_ID 	tgtMemPartCreate 
    (
    REMPTR	pPool, 
    unsigned	poolSize
    );

extern STATUS   tgtMemPartInit
    (
    PART_ID	partId,
    REMPTR	pPool,
    unsigned	poolSize,
    UINT32	options
    );

extern STATUS   tgtMemPartOptionSet
    (
    PART_ID	partId,
    UINT32	options
    );

extern STATUS	tgtMemPartOptionGet
    (
    PART_ID	partId,
    UINT32 *	pOptions
    );

extern STATUS   tgtMemPartAddToPool
    (
    PART_ID	partId,
    REMPTR	pPool,
    unsigned	poolSize
    );
 
extern REMPTR   tgtMemPartAlloc
    (
    PART_ID	partId,
    unsigned	nBytes
    );
 
extern STATUS   tgtMemPartFree
    (
    PART_ID	partId,
    REMPTR	pBlock
    );
 
extern REMPTR   tgtMemPartRealloc
    (
    PART_ID	partId,
    REMPTR	ptr,
    size_t	nBytes
    );
 
extern int  tgtMemPartFindMax
    (
    PART_ID	partId
    );
 
extern STATUS   tgtMemPartDump
    (
    PART_ID	partId
    );
 
extern STATUS   tgtMemPartShow
    (
    PART_ID	partId,
    int		type
    );
 
extern STATUS   tgtMemPartInfoGet
    (
    PART_ID	 partId,
    MEM_INFO *	 pMemInfo
    );
 
extern size_t   tgtMemPartAvailable
    (
    PART_ID	 partId,
    size_t *	largestBlock,
    BOOL	printEach
    );
 
extern REMPTR   tgtMemPartAlignedAlloc
    (
    PART_ID	partId,
    unsigned	nBytes,
    unsigned	alignment
    );
 
extern void     tgtMemAddToPool
    (
    REMPTR	pPool,
    unsigned	poolSize
    );
 
extern REMPTR   tgtMemMalloc
    (
    size_t	nBytes
    );
 
extern void tgtMemFree
    (
    REMPTR	ptr
    );
 
extern REMPTR   tgtMemCalloc
    (
    size_t	elemNum,
    size_t	elemSize
    );
 
extern STATUS   tgtMemCfree
    (
    REMPTR	pBlock
    );
 
extern REMPTR   tgtMemRealloc
    (
    REMPTR	pBlock,
    size_t	nBytes
    );
 
extern REMPTR   tgtMemAlign
    (
    unsigned	alignment,
    unsigned	size
    );
 
extern REMPTR   tgtMemValloc
    (
    unsigned	size
    );
 
extern int  tgtMemFindMax (void);
 
extern void     tgtMemShow
    (
    int		type
    );
 
extern STATUS   tgtMemMove
    (
    REMPTR	source,
    REMPTR	destination,
    int		 nBytes
    );
 
extern STATUS   tgtMemChecksum
    (
    REMPTR	source,
    int		nBytes,
    u_int *	pChecksum
    );
 
extern STATUS   tgtMemRead
    (
    REMPTR	source,
    void *	destination,
    int		nBytes
    );

extern STATUS   tgtMemWidthRead
    (
    REMPTR 	source,
    void *	destination,
    int 	nBytes,
    int	 	width
    );
 
extern STATUS   tgtMemSet
    (
    REMPTR	buffer,
    int		nBytes,
    int		value
    );
 
extern STATUS   tgtMemWrite
    (
    void *	source,
    REMPTR	destination,
    int		nBytes
    );

extern STATUS   tgtMemWidthWrite
    (
    void *	source,
    REMPTR 	destination,
    int 	nBytes,
    int 	width
    );
 
extern STATUS   tgtMemWriteByte
    (
    REMPTR	destination,
    char	value
    );
 
extern STATUS   tgtMemWriteShort
    (
    REMPTR	destination,
    UINT16	value
    );
 
extern STATUS   tgtMemWriteInt
    (
    REMPTR	destination,
    int		value
    );
 
extern STATUS   tgtMemWriteMany
    (
    void *	source,
    REMPTR	destination,
    int		nBytes
    );
 
extern STATUS   tgtMemWriteManyInts
    (
    REMPTR	destination,
    int		value
    );
 
extern STATUS   tgtMemCacheSet
    (
    REMPTR	pRem,
    int		nBytes,
    MEM_ATTRIB	attribute,
    BOOL	pushBlock
    );
 
extern STATUS   tgtMemCacheGet
    (
    REMPTR	pRem,
    BLOCK_DESC *pBlockDesc
    );
 
extern UINT tgtMemAlignGet (void);
 
extern PART_ID tgtMemPartIdGet (void);
 
#ifdef __cplusplus
}
#endif

#endif /* __INCtgtMemLibh */
