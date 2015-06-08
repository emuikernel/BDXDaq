/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    SOCK_CODgram.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_SOCK_CODGRAM_H)
#define ACE_SOCK_CODGRAM_H

#include "SOCK_IO.h"
#include "Addr.h"

class SOCK_CODgram : public SOCK_IO
  // = TITLE
  //     Defines the member functions for the SOCK connected
  // datagram abstraction. 
  //
  // = DESCRIPTION
  // 
{
public:
  SOCK_CODgram (void);
  SOCK_CODgram (const Addr &remote_sap, 
		const Addr &local_sap = sap_any, 
		int protocol_family = PF_INET, 
		int protocol = 0);

  HANDLE open (const Addr &remote_sap, 
	       const Addr &local_sap = sap_any, 
	       int protocol_family = PF_INET, 
	       int protocol = 0);
};

#include "SOCK_CODgram.i"

#endif /* _SOCK_CODGRAM_H */




