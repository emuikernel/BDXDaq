/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    LSOCK_CODgram.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_LOCAL_SOCK_CODGRAM_H)
#define ACE_LOCAL_SOCK_CODGRAM_H

#include "LSOCK.h"
#include "SOCK_CODgram.h"
#include "Addr.h"

class LSOCK_CODgram : public SOCK_CODgram, public LSOCK
  // = TITLE
  //     Defines the member functions for the LSOCK 
  //  connected datagram abstraction. 
  //
  // = DESCRIPTION
  //
{
public:
  LSOCK_CODgram (void);
  LSOCK_CODgram (const Addr &remote_sap, 
		 const Addr &local_sap = sap_any, 
		 int protocol_family = PF_UNIX, 
		 int protocol = 0);

  int open (const Addr &remote_sap, 
	    const Addr &local_sap = sap_any, 
	    int protocol_family = PF_UNIX, 
	    int protocol = 0);

  HANDLE get_handle (void) const;
  void set_handle (HANDLE);
};

#include "LSOCK_CODgram.i"

#endif /* _LOCAL_SOCK_CODGRAM_H */




