// VXWMemPart/vxwMemPartLib.h - memory partition classes

// Copyright 1995-1999 Wind River Systems, Inc.

// modification history
// --------------------
// 01c,08mar99,jdi  doc: fixed wrong cross-references.
// 01b,23feb99,fle  doc : made it refgen compliant
// 01c,21feb99,jdi  added library section, checked in documentation.
// 01b,03oct95,rhp  documented.
// 01a,15jun95,srh  written.

// DESCRIPTION
// The `VXWMemPart' class provides core facilities for managing the
// allocation of blocks of memory from ranges of memory called memory
// partitions.
//
// The allocation of memory, using routines such as
// VXWMemPart::alloc(), is done with a first-fit algorithm.  Adjacent
// blocks of memory are coalesced when they are freed with
// VXWMemPart::free().  There is also a routine provided for
// allocating memory aligned to a specified boundary from a specific
// memory partition, VXWMemPart::alignedAlloc().
//
// CAVEATS
// Architectures have various alignment constraints.  To provide
// optimal performance, VXWMemPart::alloc() returns a pointer to a
// buffer having the appropriate alignment for the architecture in
// use.  The portion of the allocated buffer reserved for system
// bookkeeping, known as the overhead, may vary depending on the
// architecture.
//
// .TS
// center,tab(|);
// lf3 cf3 cf3
// a n n .
// Architecture | Boundary | Overhead
// _
//  68K   | 4  | 8
//  SPARC | 8  | 12
//  MIPS  | 8  | 12
//  i960  | 16 | 16
// .TE
//
// INCLUDE FILES: vxwMemPartLib.h
//
// SEE ALSO: VXWSem
//
// SECTION: 1C
//

#ifndef vxwMemPartLib_h
#define vxwMemPartLib_h

#include "vxWorks.h"
#include "memLib.h"
#include "stdlib.h"
#include "private/memPartLibP.h"
#include "vxwObject.h"
#include "vxwErr.h"

class VXWMemPart : virtual public VXWIdObject
    {
  public:

//_ VXWMemPart Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWMemPart::VXWMemPart - create a memory partition
//
// This constructor creates a new memory partition containing a specified
// memory pool.  Partitions can be created to manage
// any number of separate memory pools.
//
// NOTE
// The descriptor for the new partition is allocated out of the system memory
// partition (i.e., with malloc()).
//
// RETURNS: N/A.

    VXWMemPart (char *pool, unsigned poolSize)
	: partid_ (memPartCreate (pool, poolSize))
	{
	if (partid_ == NULL)
	    vxwThrowErrno ();
	}

//_ VXWMemPart Public Member Functions

///////////////////////////////////////////////////////////////////////////////
//
// VXWMemPart::addToPool - add memory to a memory partition
//
// This routine adds memory to its memory partition.
// The new memory added need not be contiguous with
// memory previously assigned to the partition.
//
// RETURNS: OK or ERROR.

    STATUS addToPool (char *pool, unsigned poolSize)
	{
	return memPartAddToPool (partid_, pool, poolSize);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMemPart::alignedAlloc - allocate aligned memory from partition
//
// This routine allocates a buffer of size <nBytes> from its
// partition.  Additionally, it ensures that the allocated buffer begins on a
// memory address evenly divisible by <alignment>.  The <alignment> parameter
// must be a power of 2.
//
// RETURNS:
// A pointer to the newly allocated block, or NULL if the buffer cannot be
// allocated.

    void * alignedAlloc (unsigned nBytes, unsigned alignment)
	{
	return memPartAlignedAlloc (partid_, nBytes, alignment);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMemPart::alloc - allocate a block of memory from partition
//
// This routine allocates a block of memory from its partition. 
// The size of the block allocated is equal to or greater than <nBytes>.  
//
// RETURNS:
// A pointer to a block, or NULL if the call fails.
//
// SEE ALSO: VXWMemPart::free()

    void * alloc (unsigned nBytes)
	{
	return memPartAlloc (partid_, nBytes);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMemPart::findMax - find the size of the largest available free block
//
// This routine searches for the largest block in the memory partition free
// list and returns its size.
//
// RETURNS: The size, in bytes, of the largest available block.

    int findMax () const
	{
	return memPartFindMax (partid_);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMemPart::free - free a block of memory in partition
//
// This routine returns to the partition's free memory list a block of 
// memory previously allocated with VXWMemPart::alloc().
//
// RETURNS: OK, or ERROR if the block is invalid.
//
// SEE ALSO: VXWMemPart::alloc()

    STATUS free (char *pBlock)
	{
	return memPartFree (partid_, pBlock);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMemPart::info - get partition information
//
// This routine takes a pointer to a MEM_PART_STATS structure.
// All the parameters of the structure are filled in with the current partition
// information.
//
// RETURNS: OK if the structure has valid data, otherwise ERROR.
//
// SEE ALSO: VXWMemPart::show()

    STATUS info (MEM_PART_STATS *pPartStats) const
	{
	return memPartInfoGet (partid_, pPartStats);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMemPart::options - set the debug options for memory partition
//
// This routine sets the debug options for its memory partition.
// Two kinds of errors are detected: attempts to allocate more memory
// than is available, and bad blocks found when memory is freed.  In both
// cases, the error status is returned.  There are four error-handling
// options that can be individually selected:
// 
// .iP "MEM_ALLOC_ERROR_LOG_FLAG" 8
// Log a message when there is an error in allocating memory.
// .iP "MEM_ALLOC_ERROR_SUSPEND_FLAG"
// Suspend the task when there is an error in allocating memory (unless
// the task was spawned with the VX_UNBREAKABLE option, in which case it
// cannot be suspended).
// .iP "MEM_BLOCK_ERROR_LOG_FLAG"
// Log a message when there is an error in freeing memory.
// .iP "MEM_BLOCK_ERROR_SUSPEND_FLAG"
// Suspend the task when there is an error in freeing memory (unless
// the task was spawned with the VX_UNBREAKABLE option, in which case it
// cannot be suspended).
// .LP
// 
// These options are discussed in detail in the library manual entry for
// memLib.
// 
// RETURNS: OK or ERROR.

    STATUS options (unsigned options)
	{
	return memPartOptionsSet (partid_, options);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMemPart::realloc - reallocate a block of memory in partition
//
// This routine changes the size of a specified block of memory and returns a
// pointer to the new block.  The contents that fit inside the new size (or
// old size if smaller) remain unchanged.  The memory alignment of the new
// block is not guaranteed to be the same as the original block.
//
// If <pBlock> is NULL, this call is equivalent to VXWMemPart::alloc().
//
// RETURNS:
// A pointer to the new block of memory, or NULL if the call fails.

    void * realloc (char *pBlock, int nBytes)
	{
	return memPartRealloc (partid_, pBlock, nBytes);
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWMemPart::show - show partition blocks and statistics
//
// This routine displays statistics about the available and allocated memory
// in its memory partition.  It shows the number of bytes, the number
// of blocks, and the average block size in both free and allocated memory,
// and also the maximum block size of free memory.  It also shows the number
// of blocks currently allocated and the average allocated block size.
//
// In addition, if <type> is 1, the routine displays a list of all the blocks
// in the free list of the specified partition.
//
// RETURNS: OK or ERROR.

    STATUS show (int type = 0) const
	{
	return memPartShow (partid_, type);
	}

  protected:
    VXWMemPart ()
	{
	}
    VXWMemPart (PART_ID aPartId)
	: partid_ (aPartId)
	{
	}
    VXWMemPart (const VXWMemPart & aMemPart)
	: partid_ (aMemPart.partid_)
	{
	}
    VXWMemPart & operator = (const VXWMemPart & aMemPart)
	{
	partid_ = aMemPart.partid_;
	return *this;
	}
    ~VXWMemPart ()
	{
	}
    virtual void * myValue ();

    PART_ID partid_;
    };

#endif /* ifndef vxwMemPartLib_h */
