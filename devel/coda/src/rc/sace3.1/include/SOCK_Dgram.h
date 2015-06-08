/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    SOCK_Dgram.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_SOCK_DGRAM_H)
#define ACE_SOCK_DGRAM_H

#include "SOCK.h"
#include "Addr.h"

class SOCK_Dgram : public SOCK
  // = TITLE
  //     Defines the member functions for the SOCK datagram
  // abstraction. 
  //
  // = DESCRIPTION
  // 
{
public:
  SOCK_Dgram (void);
  SOCK_Dgram (const Addr &local, int protocol_family = PF_INET, int protocol = 0);

  HANDLE open (const Addr &local, int protocol_family = PF_INET, int protocol = 0);
  ssize_t send (const void *buf, size_t n, const Addr &addr, int flags = 0) const;
  ssize_t send (const iovec iov[], size_t n, const Addr &addr, int flags = 0) const;
  ssize_t recv (void *buf, size_t n, Addr &addr, int flags = 0) const;
  ssize_t recv (iovec iov[], size_t n, Addr &addr, int flags = 0) const;
    
private:
  int shared_open (const Addr &local, int protocol_family);

  int  get_remote_addr (Addr &) const; 
  // Do not allow this function to percolate up to this interface... 
};

#include "SOCK_Dgram.i"

#endif /* ACE_SOCK_DGRAM_H */
