/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    SOCK_Stream.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_SOCK_STREAM_H)
#define ACE_SOCK_STREAM_H

#include "SOCK_IO.h"
#include "Addr.h"

class SOCK_Stream : public SOCK_IO
  // = TITLE
  //     Defines the member functions for the SOCK Stream abstraction. 
  //
  // = DESCRIPTION
  //
{
public:
  // = The following two methods use <write> and <read> system calls, which are
  // faster than the <send> and <recv> library functions used by the
  // following two methods.
  ssize_t send_n (const void *buf, int n) const;
  // Send n bytes, keep trying until n are sent.
  ssize_t recv_n (void *buf, int n) const;	      
  // Recv n bytes, keep trying until n are received.

  // = The following two methods use <send> and <recv> system calls.
  ssize_t send_n (const void *buf, int n, int flags) const;
  // Send n bytes, keep trying until n are sent.
  ssize_t recv_n (void *buf, int n, int flags) const;	      
  // Recv n bytes, keep trying until n are received.

  // = Send/receive an "urgent" character (see TCP specs...).
  ssize_t send_urg (void *ptr, int len = sizeof (char));
  ssize_t recv_urg (void *ptr, int len = sizeof (char));

  // = Selectively close down either the reader or the writer.
  int  close_reader (void);
  int  close_writer (void);
};

#include "SOCK_Stream.i"

#endif /* _SOCK_STREAM_H */
