/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    LSOCK_Aceeptor.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_LOCAL_SOCK_ACCEPTOR_H)
#define ACE_LOCAL_SOCK_ACCEPTOR_H

#include "SOCK_Acceptor.h"
#include "UNIX_Addr.h"
#include "LSOCK_Stream.h"

class LSOCK_Acceptor : public SOCK_Acceptor
  // = TITLE
  //     Defines the format and interface for the acceptor side of the 
  // local SOCK Stream. 
  //
  // = DESCRIPTION
  //
{
public:
  LSOCK_Acceptor (void);

  LSOCK_Acceptor (const Addr &local_sap, int reuse_addr = 0, 
		  int micro_sec_delay = -1, int protocol_family = PF_UNIX, 
		  int backlog = 5, int protocol = 0);

  HANDLE  open (const Addr &local_sap, int reuse_addr = 0, 
		int micro_sec_delay= -1, int protocol_family = PF_UNIX, 
		int backlog = 5, int protocol = 0);
  // Initiate a passive mode socket.

  HANDLE  accept (LSOCK_Stream &new_ipc_sap, Addr * = 0, int restart = 1) const;
  // Accept a new data transfer connection.

  int remove (void);
  // Close down the LSOCK and remove the rendezvous point from the file system.

  int get_local_addr (Addr &) const;
  // Return the local endpoint address.

private:
  UNIX_Addr local_addr_;
};

#if defined (__INLINE__)
#define INLINE inline
#include "LSOCK_Acceptor.i"
#else
#define INLINE
#endif /* __INLINE__ */

#endif /* _LOCAL_SOCK_ACCEPTOR_H */
