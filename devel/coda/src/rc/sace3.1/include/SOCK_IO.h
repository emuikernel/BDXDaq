/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    SOCK_IO.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_SOCK_IO_H)
#define ACE_SOCK_IO_H

#include "SOCK.h"

class SOCK_IO : public SOCK
  // = TITLE
  //     Defines the methods for the SOCK I/O routines (i.e., send/recv). 
  //
  // = DESCRIPTION
  //
{
public:
  ssize_t send (const void *buf, size_t n, int flags) const;
  // Send an n byte buffer to the connected socket (uses send(3)).

  ssize_t recv (void *buf, size_t n, int flags) const;
  // Recv an n byte buffer from the connected socket (uses recv(3)).

  ssize_t send (const void *buf, size_t n) const;
  // Send an n byte buffer to the connected socket (uses write(2)).  

  ssize_t recv (void *buf, size_t n) const;
  // Recv an n byte buffer from the connected socket (uses read(2)).

  ssize_t send (const iovec iov[], size_t n) const;
  // Send a vector of n byte messages to the connected socket. 

  ssize_t recv (iovec iov[], size_t n) const;
  // Recv a vector of n byte messages to the connected socket. 

  ssize_t send (size_t n, ...) const;
  // Send varargs messages to the connected socket. 

  ssize_t recv (size_t n, ...) const;
  // Recv varargs messages to the connected socket. 
};

#include "SOCK_IO.i"

#endif /* _SOCK_IO_H */
