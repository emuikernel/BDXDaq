/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    LSOCK_Dgram.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_LOCAL_SOCK_DGRAM_H)
#define ACE_LOCAL_SOCK_DGRAM_H

#include "SOCK_Dgram.h"
#include "LSOCK.h"

class LSOCK_Dgram : public SOCK_Dgram, public LSOCK
  // = TITLE
  //     Create a Local SOCK datagram. 
  //
  // = DESCRIPTION
  //
{
public:
  LSOCK_Dgram (void);
  LSOCK_Dgram (const Addr &local, int protocol_family = PF_UNIX, int protocol = 0);

  HANDLE open (const Addr &local, int protocol_family = PF_UNIX, int protocol = 0);

  HANDLE get_handle (void) const;
  void set_handle (HANDLE);
};

#include "LSOCK_Dgram.i"

#endif /* _LOCAL_SOCK_DGRAM_H */
