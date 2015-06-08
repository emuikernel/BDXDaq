// VXWRingBuf/vxwRngLib.h - ring buffer class

// Copyright 1995-1999 Wind River Systems, Inc.

// modification history
// --------------------
// 01b,23feb99,fle  doc : made it refgen compliant
// 01d,21feb99,jdi  added library section, checked in documentation.
// 01c,03oct95,rhp  documented.
// 01b,05feb97,bss  fixed SPR #7821 by defining myValue() method.
// 01a,15jun95,srh  written.

// DESCRIPTION
// The `VXWRingBuf' class provides routines for creating and using
// ring buffers, which are first-in-first-out circular buffers.  The
// routines simply manipulate the ring buffer data structure; no
// kernel functions are invoked.  In particular, ring buffers by
// themselves provide no task synchronization or mutual exclusion.
//
// However, the ring buffer pointers are manipulated in such a way
// that a reader task (invoking VXWRingBuf::get()) and a writer task
// (invoking VXWRingBuf::put()) can access a ring simultaneously
// without requiring mutual exclusion.  This is because readers only
// affect a <read> pointer and writers only affect a <write>
// pointer in a ring buffer data structure.  However, access by
// multiple readers or writers <must> be interlocked through a
// mutual exclusion mechanism (for example, a mutual-exclusion
// semaphore guarding a ring buffer).
//
// INCLUDE FILES: vxwRngLib.h
//
// SECTION: 1C
//

#ifndef vxwRngLib_h
#define vxwRngLib_h

#include "vxWorks.h"
#include "rngLib.h"
#include "vxwObject.h"
#include "vxwErr.h"

class VXWRingBuf : virtual public VXWIdObject
    {
  public:
//_ VXWRingBuf Public Constructors

///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::VXWRingBuf - create an empty ring buffer
//
// This constructor creates a ring buffer of size <nbytes>, and initializes
// it.  Memory for the buffer is allocated from the system memory partition.
//
// RETURNS: N/A.

    VXWRingBuf (int nbytes)
	: rid_ (rngCreate (nbytes))
	{
	if (rid_ == 0)
	    vxwThrowErrno ();
	}

///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::VXWRingBuf - build ring-buffer object from existing ID
// 
// Use this constructor to build a ring-buffer object from an existing
// ring buffer.  This permits you to use the C++ ring-buffer
// interfaces even if the ring buffer itself was created by a routine
// written in C.
// 
// RETURNS: N/A.
// 
// SEE ALSO: rngLib

    VXWRingBuf (RING_ID aRingId)
	: rid_ (aRingId)
	{
	}
///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::~VXWRingBuf - delete ring buffer
//
// This destructor deletes a specified ring buffer.
// Any data in the buffer at the time it is deleted is lost.
//
// RETURNS: N/A

    ~VXWRingBuf ()
	{
	rngDelete (rid_);
	}

//_ VXWRingBuf Public Member Functions

///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::get - get characters from ring buffer
//
// This routine copies bytes from the ring buffer into <buffer>.
// It copies as many bytes as are available in the ring, up to <maxbytes>.
// The bytes copied are then removed from the ring.
//
// RETURNS:
// The number of bytes actually received from the ring buffer;
// it may be zero if the ring buffer is empty at the time of the call.

    int get (char * buffer, int maxbytes)
	{
	return rngBufGet (rid_, buffer, maxbytes);
	}
///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::put - put bytes into ring buffer
//
// This routine puts bytes from <buffer> into the ring buffer.  The
// specified number of bytes is put into the ring, up to the number of
// bytes available in the ring.
//
// RETURNS:
// The number of bytes actually put into the ring buffer;
// it may be less than number requested, even zero,
// if there is insufficient room in the ring buffer at the time of the call.

    int put (char * buffer, int nBytes)
	{
	return rngBufPut (rid_, buffer, nBytes);
	}
///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::flush - make ring buffer empty
//
// This routine initializes the ring buffer to be empty.
// Any data in the buffer is lost.
//
// RETURNS: N/A

    void flush ()
	{
	rngFlush (rid_);
	}
///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::freeBytes - determine the number of free bytes in ring buffer
//
// This routine determines the number of bytes currently unused in the
// ring buffer.
//
// RETURNS: The number of unused bytes in the ring buffer.

    int freeBytes () const
	{
	return rngFreeBytes (rid_);
	}
///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::isEmpty - test whether ring buffer is empty
//
// This routine reports on whether the ring buffer is empty.
//
// RETURNS:
// TRUE if empty, FALSE if not.

    BOOL isEmpty () const
	{
	return rngIsEmpty (rid_);
	}
///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::isFull - test whether ring buffer is full (no more room)
//
// This routine reports on whether the ring buffer is completely full.
//
// RETURNS:
// TRUE if full, FALSE if not.

    BOOL isFull () const
	{
	return rngIsFull (rid_);
	}
///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::moveAhead - advance ring pointer by <n> bytes
// 
// This routine advances the ring buffer input pointer by <n> bytes.
// This makes <n> bytes available in the ring buffer, after having
// been written ahead in the ring buffer with VXWRingBuf::putAhead().
//
// RETURNS: N/A

    void moveAhead (int n)
	{
	rngMoveAhead (rid_, n);
	}
///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::nBytes - determine the number of bytes in ring buffer
//
// This routine determines the number of bytes currently in the
// ring buffer.
//
// RETURNS: The number of bytes filled in the ring buffer.

    int nBytes () const
	{
	return rngNBytes (rid_);
	}
///////////////////////////////////////////////////////////////////////////////
//
// VXWRingBuf::putAhead - put a byte ahead in a ring buffer without moving ring pointers
// 
// This routine writes a byte into the ring, but does not move the
// ring buffer pointers.  Thus the byte is not yet be available to
// VXWRingBuf::get() calls.  The byte is written <offset> bytes ahead
// of the next input location in the ring.  Thus, an offset of 0 puts
// the byte in the same position as VXWRingBuf::put() would put a
// byte, except that the input pointer is not updated.
// 
// Bytes written ahead in the ring buffer with this routine can be
// made available all at once by subsequently moving the ring buffer
// pointers with the routine VXWRingBuf::moveAhead().
//
// Before calling VXWRingBuf::putAhead(), the caller must verify that at least
// <offset> + 1 bytes are available in the ring buffer.
//
// RETURNS: N/A

    void putAhead (char byte, int offset)
	{
	rngPutAhead (rid_, byte, offset);
	}
  protected:
    VXWRingBuf ()
	{
	}
    VXWRingBuf (const VXWRingBuf &)
	{
	}
    VXWRingBuf & operator = (const VXWRingBuf &)
	{
	return *this;
	}

    // Added to fix SPR # 7821.
    virtual void * myValue ();

    RING_ID rid_;
    };

#endif /* ifndef vxwRngLib_h */
