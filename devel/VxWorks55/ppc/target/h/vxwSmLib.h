// VXWSm/vxwSmLib.h - support for shared memory objects

// Copyright 1995-1999 Wind River Systems, Inc.

// modification history
// --------------------
// 01d,09apr99,fle  added it to the doc build
// 01c,21feb99,jdi  added library section, checked in documentation.
// 01b,02oct95,rhp  documented.
// 01a,15jun95,srh  written.

// DESCRIPTION
// This library defines wrapper classes for VxWorks shared-memory
// objects: `VXWSmBSem' (shared-memory binary semaphores), `VXWSmCSem'
// (shared-memory counting semaphores), `VXWSmMsgQ' (shared-memory
// message queues), `VXWSmMemPart' (shared memory partitions), and
// `VXWSmMemBlock' (shared memory blocks).
//
// INHERITANCE
// All of the shared-memory object wrappers inherit the public members
// of `VXWSmNameLib'.
//
// The `VXWSmBSem' and `VXWSmCSem' classes also inherit from `VXWSem';
// `VXWSmMsgQ' also inherits from `VXWMsgQ'; `VXWSmMemPart' also
// inherits from `VXWMemPArt'.
//
// INCLUDE FILES: vxwSmLib.h
//
// SEE ALSO
// vxwSmNameLib, vxwMsgQLib, vxwMemPartLib, vxwSemLib,
// .pG "Shared-Memory Objects"
//
// SECTION: 1C
//

#ifndef vxwSmLib_h
#define vxwSmLib_h

#include "vxWorks.h"
#include "memLib.h"
#include "smMemLib.h"
#include "msgQSmLib.h"
#include "semLib.h"
#include "semSmLib.h"
#include "smNameLib.h"
#include "vxwObject.h"
#include "vxwMemPartLib.h"
#include "vxwMsgQLib.h"
#include "vxwSemLib.h"
#include "vxwSmNameLib.h"
#include "vxwErr.h"
#include "private/semLibP.h"

class VXWSmSem : public VXWSem, public VXWSmName
{
protected:
    VXWSmSem (SEM_ID aSemId)
	: VXWSem (aSemId)
    {
    }
    VXWSmSem ()
    {
    }
    VXWSmSem (const VXWSmSem & )
    {
    }
    const VXWSmSem & operator = (const VXWSmSem &)
    {
    return *this;
    }
};

class VXWSmBSem : public VXWSmSem
{
public:

//_ VXWSmBSem Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmBSem::VXWSmBSem - create and initialize binary shared-memory semaphore (VxMP Option)
//
// This routine allocates and initializes a shared memory binary semaphore.
// The semaphore is initialized to an initial state <istate> of either
// SEM_FULL (available) or SEM_EMPTY (not available).  The shared semaphore
// structure is allocated from the shared semaphore dedicated memory
// partition.  Use the optional <name> argument to identify the new
// semaphore by name.
//
// The queuing style for blocked tasks is set by <opts>; the only
// supported queuing style for shared memory semaphores is first-in-first-out,
// selected by SEM_Q_FIFO.
//
// The maximum number of shared memory semaphores (binary plus counting) that
// can be created is SM_OBJ_MAX_SEM, defined in configAll.h.
//
// AVAILABILITY
// This routine depends on code distributed as a component of the unbundled
// shared memory support option, VxMP.
// 
// RETURNS: N/A.
// 
// ERRNO:
//  S_smMemLib_NOT_ENOUGH_MEMORY
//  S_semLib_INVALID_QUEUE_TYPE
//  S_semLib_INVALID_STATE
//  S_smObjLib_LOCK_TIMEOUT
//
// SEE ALSO: vxwSemLib, vxwSmNameLib

    VXWSmBSem (int opts, SEM_B_STATE istate, char * name = 0)
	: VXWSmSem (semBSmCreate (opts, istate))
    {
    if (sem_ == 0)
	vxwThrowErrno ();
    if (name != 0 && nameSet (name) != OK)
	vxwThrowErrno ();
    }

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmBSem::VXWSmBSem - build a binary shared-memory semaphore object (VxMP Option)
// 
// This routine builds a shared-memory binary semaphore object around
// an existing named shared-memory semaphore.  The <name> argument
// identifies the existing semaphore; <waitType> specifies whether to
// wait if the desired name is not found in the shared-memory name
// database; see VXWSmName::nameGet().
// 
// Use this routine to take advantage of the VXWSmBSem class while
// working with semaphores created by some other means (for example,
// previously existing C code).
// 
// RETURNS: N/A.
//
// SEE ALSO: VXWSmName::nameGet()

    VXWSmBSem (char * name, int waitType)
    {
    int    type;

    if (smNameFind (name, (void **)(&sem_), &type, waitType) != OK)
	vxwThrowErrno ();
    }
    virtual STATUS nameSet (char * name);
protected:
    VXWSmBSem ()
    {
    }
    VXWSmBSem (const VXWSmBSem &)
    {
    }
    VXWSmBSem & operator = (const VXWSmBSem &)
    {
    return *this;
    }
};

class VXWSmCSem : public VXWSmSem
{
public:

//_ VXWSmCSem Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmCSem::VXWSmCSem - create and initialize a shared memory counting semaphore (VxMP Option)
//
// This routine allocates and initializes a shared memory counting
// semaphore.  The initial count value of the semaphore (the number of
// times the semaphore must be taken before it can be given) is specified
// by <icount>.
//
// The queuing style for blocked tasks is set by <opts>; the only
// supported queuing style for shared memory semaphores is first-in-first-out,
// selected by SEM_Q_FIFO.
//
// The maximum number of shared memory semaphores (binary plus counting) that
// can be created is SM_OBJ_MAX_SEM, defined in configAll.h.
//
// AVAILABILITY
// This routine depends on code distributed as a component of the unbundled
// shared memory support option, VxMP.
// 
// RETURNS: N/A.
//
// ERRNO:
//  S_smMemLib_NOT_ENOUGH_MEMORY
//  S_semLib_INVALID_QUEUE_TYPE
//  S_smObjLib_LOCK_TIMEOUT
//
// SEE ALSO: vxwSemLib, vxwSmNameLib

    VXWSmCSem (int opts, int icount, char * name = 0)
	: VXWSmSem (semCSmCreate (opts, icount))
    {
    if (sem_ == 0)
	vxwThrowErrno ();
    if (name != 0 && nameSet (name) != OK)
	vxwThrowErrno ();
    }

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmCSem::VXWSmCSem - build a shared-memory counting semaphore object (VxMP Option)
// 
// This routine builds a shared-memory semaphore object around
// an existing named shared-memory counting semaphore.  The <name> argument
// identifies the existing semaphore, and <waitType> specifies whether to
// wait if the desired name is not found in the shared-memory name
// database; see VXWSmName::nameGet().
// 
// Use this routine to take advantage of the VXWSmBSem class while
// working with semaphores created by some other means (for example,
// previously existing C code).
// 
// RETURNS: N/A.
//
// SEE ALSO: VXWSmName::nameGet()

    VXWSmCSem (char * name, int waitType)
    {
    int    type;

    if (smNameFind (name, (void **)(&sem_), &type, waitType) != OK)
	vxwThrowErrno ();
    }
    virtual STATUS nameSet (char * name);
protected:
    VXWSmCSem ()
    {
    }
    VXWSmCSem (const VXWSmCSem &)
    {
    }
    VXWSmCSem & operator = (const VXWSmCSem &)
    {
    return *this;
    }
};    

class VXWSmMsgQ : public VXWMsgQ, public  VXWSmName
{
public:
//_ VXWSmMsgQ Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmMsgQ::VXWSmMsgQ - create and initialize a shared-memory message queue (VxMP Option)
//
// This routine creates a shared memory message queue capable of holding up
// to <maxMsgs> messages, each up to <maxMsgLength> bytes long.  The queue
// can only be created with the option MSG_Q_FIFO (0), thus queuing pended
// tasks in FIFO order.
//
// If there is insufficient memory to store the message queue structure
// in the shared memory message queue partition or if the shared memory system
// pool cannot handle the requested message queue size, shared memory message 
// queue creation fails with `errno' set to S_smMemLib_NOT_ENOUGH_MEMORY.
// This problem can be solved by incrementing the SM_OBJ_MAX_MSG_Q value
// in configAll.h and/or the size of memory dedicated to shared-memory objects
// SM_OBJ_MEM_SIZE in config.h.
//
// AVAILABILITY:
// This routine depends on code distributed as a component of the unbundled
// shared memory objects support option, VxMP.
//
// RETURNS: N/A.
//
// ERRNO:
//  S_smMemLib_NOT_ENOUGH_MEMORY
//  S_intLib_NOT_ISR_CALLABLE
//  S_msgQLib_INVALID_QUEUE_TYPE
//  S_smObjLib_LOCK_TIMEOUT
//
// SEE ALSO: vxwMsgQLib, vxwSmNameLib

    VXWSmMsgQ (int maxMsgs, int maxMsgLength, int options)
	: VXWMsgQ (msgQSmCreate (maxMsgs, maxMsgLength, options))
    {
    if (msgq_ == 0)
	vxwThrowErrno ();
    }
    virtual STATUS nameSet (char *name);
};

class VXWSmMemPart : public VXWMemPart, public VXWSmName
{
public:
//_ VXWSmMemPart Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmMemPart::VXWSmMemPart - create a shared memory partition (VxMP Option)
//
// This routine creates a shared memory partition that can be used by tasks
// on all CPUs in the system to manage memory blocks .  Because the
// `VXWSmMemPart' class inherits from `VXWMemPart', you can use the
// general-purpose methods in that class to manage the partition (that
// is, to allocate and free memory blocks in the partition).
//
// <pool> is a pointer to the global address of shared memory dedicated to the
// partition.  The memory area where <pool> points must be in the same
// address space as the shared memory anchor and shared memory pool.
//
// <poolSize> is the size in bytes of shared memory dedicated to the partition.
//
// NOTE
// The descriptor for the new partition is allocated out of an internal
// dedicated shared memory partition.  The maximum number of partitions that can
// be created is SM_OBJ_MAX_MEM_PART, defined in configAll.h.
//
// Memory pool size is rounded down to a 16-byte boundary.
//
// AVAILABILITY
// This routine depends on code distributed as a component of the unbundled
// shared memory objects support option, VxMP.
// 
// RETURNS: N/A.
//
// ERRNO:
//  S_memLib_NOT_ENOUGH_MEMORY
//  S_smObjLib_LOCK_TIMEOUT 
//
// SEE ALSO: vxwMemPartLib, vxwSmNameLib

    VXWSmMemPart (char *pool, unsigned poolSize)
	: VXWMemPart (memPartSmCreate (pool, poolSize))
    {
    if (partid_ == 0)
	vxwThrowErrno ();
    }
    virtual STATUS nameSet (char *name);
};

class VXWSmMemBlock : public VXWSmName
{
public:
//_ VXWSmMemBlock Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmMemBlock::VXWSmMemBlock - allocate a block of memory from the shared memory system partition (VxMP Option)
// 
// This routine allocates, from the shared memory system partition, a
// block of memory whose size is equal to or greater than <nBytes>.
// The local address of the allocated shared memory block can be
// obtained from VXWSmMemBlock::baseAddress().
//
// AVAILABILITY
// This routine depends on code distributed as a component of the unbundled
// shared memory objects support option, VxMP.
// 
// RETURNS: N/A.
//
// ERRNO:
//  S_memLib_NOT_ENOUGH_MEMORY
//  S_smObjLib_LOCK_TIMEOUT
//

    VXWSmMemBlock (int nBytes)
	: pBlock_ (smMemMalloc (nBytes)), nBytes_ (nBytes)
    {
    if (pBlock_ == 0)
	vxwThrowErrno ();
    }

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmMemBlock::VXWSmMemBlock - allocate memory for an array from the shared memory system partition (VxMP Option)
//
// This routine allocates a block of memory for an array that contains
// <nElems> elements of size <sizeOfElem> from the shared memory system 
// partition.
// The local address of the allocated shared memory block can be
// obtained from VXWSmMemBlock::baseAddress().
//
// AVAILABILITY
// This routine depends on code distributed as a component of the unbundled
// shared memory objects support option, VxMP.
// 
// RETURNS:
// A pointer to the block, or NULL if the memory cannot be allocated.
//
// ERRNO:
//  S_memLib_NOT_ENOUGH_MEMORY
//  S_smObjLib_LOCK_TIMEOUT

    VXWSmMemBlock (int nElems, int sizeOfElem)
	: pBlock_ (smMemCalloc (nElems, sizeOfElem)),
	  nBytes_ (nElems * sizeOfElem)
    {
    if (pBlock_ == 0)
	vxwThrowErrno ();
    }

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmMemBlock::~VXWSmMemBlock - free a shared memory system partition block of memory (VxMP Option)
//
// This routine returns a `VXWSmMemBlock' shared-memory block to the
// free-memory pool in the shared-memory system partition.
//
// AVAILABILITY
// This routine depends on code distributed as a component of the unbundled shared memory
// objects support option, VxMP.
// 
// RETURNS: N/A.
//
// ERRNO:
//  S_smObjLib_LOCK_TIMEOUT
//
// SEE ALSO: VXWSmMemBlock::VXWSmMemBlock()

    virtual ~VXWSmMemBlock ()
    {
    if (smMemFree (pBlock_) != OK)
	vxwThrowErrno ();
    }

//_ VXWSmMemBlock Public Member Functions

///////////////////////////////////////////////////////////////////////////////
//
// VXWSmMemBlock::baseAddress - address of shared-memory block
//
// This routine reports the local address of a block of shared memory
// managed as a `VXWSmMemBlock' object.
//
// RETURNS: Local address of memory block in shared-memory system partition.

    void * baseAddress () const
    {
    return pBlock_;
    }
    virtual STATUS nameSet (char * name);
    virtual void * myValue ();
protected:
    VXWSmMemBlock ()
    {
    }
    VXWSmMemBlock (const VXWSmMemBlock &)
    {
    }
    VXWSmMemBlock & operator = (const VXWSmMemBlock &)
    {
    return *this;
    }
    void     * pBlock_;
    unsigned   nBytes_;
};

#endif /* ifndef vxwSmLib_h */
