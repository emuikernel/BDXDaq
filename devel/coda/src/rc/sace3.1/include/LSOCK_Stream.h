/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    LSOCK_Stream.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_LOCAL_SOCK_STREAM_H)
#define ACE_LOCAL_SOCK_STREAM_H

#include "SOCK_Stream.h"
#include "LSOCK.h"

class LSOCK_Stream : public SOCK_Stream, public LSOCK
  // = TITLE
  //     Create a Local SOCK stream. 
  // 
  // = DESCRIPTION
  //
{
public:
  ssize_t send_msg (const iovec iov[], size_t n, int fd);
  ssize_t recv_msg (iovec iov[], size_t n, int &fd);

  HANDLE get_handle (void) const;
  void set_handle (HANDLE fd); /* Overrides set_handle from the base classes. */

private:
  int get_remote_addr (Addr &) const; 
  // Do not allow this function to percolate up to this interface... 
};

#include "LSOCK_Stream.i"

#endif /* _LOCAL_SOCK_STREAM_H */
