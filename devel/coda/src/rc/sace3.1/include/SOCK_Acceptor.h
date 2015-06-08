/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    SOCK_Acceptor.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_SOCK_ACCEPTOR_H)
#define ACE_SOCK_ACCEPTOR_H

#include "SOCK_Stream.h"

class SOCK_Acceptor : public SOCK
  // = TITLE
  //     Defines the format and interface for an SOCK Stream acceptor. 
  // 
  // = DESCRIPTION
  //
{
public:
  SOCK_Acceptor (void);
  SOCK_Acceptor (const Addr &local_sap, int reuse_addr = 0, 
		 int micro_sec_delay = -1, int protocol_family = PF_INET, 
		 int backlog = 5, int protocol = 0);
  
  HANDLE open (const Addr &local_sap, int reuse_addr = 0, 
	       int micro_sec_delay = -1, int protocol_family = PF_INET, 
	       int backlog = 5, int protocol = 0);
  // Initiate a passive mode socket.
  
  HANDLE accept (SOCK_Stream &new_stream, Addr *remote_addr = 0, 
		 int restart = 1) const;
  // Accept a new data transfer connection.

protected:
  int micro_second_delay_;	
// Number of micro seconds to wait for arriving connection
// requests. -1 means block forever, 0 means poll. 
  
  HANDLE shared_open (const Addr &local_sap, int reuse_addr, int backlog);
  HANDLE shared_accept (Addr *remote_addr, int restart) const;
  int handle_timed_wait (int restart) const;

private:
  int get_remote_addr (Addr &) const; 
  // Do not allow this function to percolate up to this interface... 
};

#include "SOCK_Acceptor.i"

#endif /* _SOCK_ACCEPTOR_H */
