/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    SOCK_Dgram_Brdcast.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_SOCK_DGRAM_BRDCAST_H)
#define ACE_SOCK_DGRAM_BRDCAST_H

#include "SOCK.h"
#include "Addr.h"
#include "INET_Addr.h"
#include "SOCK_Dgram.h"

class SOCK_Dgram_Brdcast : public SOCK_Dgram
  // = TITLE
  //     Defines the member functions for the SOCK datagram
  // abstraction. 
  //
  // = DESCRIPTION
  // 
{
public:
  SOCK_Dgram_Brdcast (void);
  SOCK_Dgram_Brdcast (const Addr &local, int protocol_family = PF_INET, int protocol = 0);

  HANDLE open (const Addr &local, int protocol_family = PF_INET, int protocol = 0);
  
  ssize_t send (const void *buf, size_t n, unsigned short portnum, int flags = 0) const;
  ssize_t send (const iovec iov[], size_t n, unsigned short portnum, int flags = 0) const;

  ssize_t send (const void *buf, size_t n, const Addr &addr, int flags = 0) const;
  ssize_t send (const iovec iov[], size_t n, const Addr &addr, int flags = 0) const;

private:
  int mk_broadcast (void);

private:

  struct ifnode 
  {
    struct sockaddr_in brdAddr;
    struct ifnode *next;
  };

  struct ifnode *ifptr_;

  int  get_remote_addr (Addr &) const; 
  // Do not allow this function to percolate up to this interface... 
};

#include "SOCK_Dgram_Brdcast.i"

#endif /* _SOCK_DGRAM_BRDCAST_H */



