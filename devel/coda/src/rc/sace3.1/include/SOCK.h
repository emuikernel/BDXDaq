/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    SOCK.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_SOCK_H)
#define ACE_SOCK_H

#include "Addr.h"
#include "IPC_SAP.h"

class SOCK : public IPC_SAP
  // = TITLE
  //     Defines the member functions for the base class of the SOCK
  // abstraction. 
  // 
  // = DESCRIPTION
  //
{
public:
  int open (int type, int protocol_family, int protocol);
  // Wrapper around the socket() system call.

  int  set_option (int level, int option, void *optval, int optlen) const;
  // Wrapper around the setsockopt() system call.

  int  get_option (int level, int option, void *optval, int *optlen) const;
  // Wrapper around the getsockopt() system call.

  int close (void);
  // Close down the socket.

  int get_local_addr (Addr &) const;
  // Return the local endpoint address.

#ifdef Darwin
ssize_t
#else
int
#endif
get_remote_addr (Addr &) const;
  // Return the address of the remotely connected peer (if there is one).

protected:
  SOCK (void);
  SOCK (int type, int protocol_family, int protocol = 0);
};

#include "SOCK.i"

#endif /* _SOCK_H */
