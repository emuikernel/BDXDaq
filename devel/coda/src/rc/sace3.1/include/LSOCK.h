/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    LSOCK.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_LOCAL_SOCK_H)
#define ACE_LOCAL_SOCK_H

#include "SOCK.h"

class LSOCK
  // = TITLE
  //     Create a Local SOCK, which is used for passing file descriptors. 
  //
  // = DESCRIPTION
  //
{
public:
  int send_handle (const HANDLE handle) const;	
  // Send an open FD to another process. 

  int recv_handle (HANDLE &handles, char *pbuf = 0, int *len = 0) const; 
  // Recv an open FD from another process. 

protected:
  LSOCK (void);
  // Ensure that LSOCK is an abstract base class 
  LSOCK (HANDLE handle);

  HANDLE get_handle (void) const;
  void set_handle (HANDLE handle);

private:
  HANDLE aux_handle_;
  // An auxiliary handle used to avoid virtual base classes... 
};

#include "LSOCK.i"

#endif /* _LOCAL_SOCK_H */
