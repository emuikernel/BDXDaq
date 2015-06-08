/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    FIFO.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_FIFO_H)
#define ACE_FIFO_H

#include "IPC_SAP.h"
#include "Str_Buf.h"

class FIFO : public IPC_SAP
  // = TITLE
  //    Abstract base class for UNIX FIFOs (a.k.a. "named-pipes"). 
  // 
  // = DESCRIPTION
  // 
{
public:
  HANDLE open (const char *rendezvous, int flags, int perms);

  int  close (void);
  // Close down the FIFO without removing the rendezvous point.

  int  remove (void);
  // Close down the FIFO and remove the rendezvous point from the file system.

  int get_local_addr (const char *&rendezvous) const;
  // Return the local address of this endpoint.

protected:
  FIFO (void);
  FIFO (const char *rendezvous, int flags, int perms);

private:
  char rendezvous_[MAXPATHLEN + 1];
};

#if defined (__INLINE__)
#define INLINE inline
#include "FIFO.i"
#else
#define INLINE 
#endif /* __INLINE__ */
#endif /* _FIFO_H */
