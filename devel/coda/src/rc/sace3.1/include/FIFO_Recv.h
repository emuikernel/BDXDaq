/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    FIFO_Recv.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_FIFO_RECV_H)
#define ACE_FIFO_RECV_H

#include "FIFO.h"

class FIFO_Recv : public FIFO
  // = TITLE
  //    Receiver-side of the bytestream-oriented C++ wrapper for UNIX FIFOs.
  //
  // = DESCRIPTION
  // 
{
public:
  FIFO_Recv (void);
  FIFO_Recv (const char *rendezvous, int flags = O_CREAT | O_RDONLY, int perms = 0666, int persistent = 1);
  HANDLE open (const char *rendezvous, int flags = O_CREAT | O_RDONLY, int perms = 0666, int persistent = 1);
  int close (void);

  ssize_t recv (void *buf, size_t len);
  ssize_t recv_n (void *buf, size_t len);
  
private:
  HANDLE aux_handle_;
  // Auxiliary handle that is used to implement persistent FIFOs. 
};

#include "FIFO_Recv.i"
#endif /* __FIFO_RECV_H */
