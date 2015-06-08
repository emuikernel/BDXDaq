/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    FIFO_Send.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_FIFO_SEND_H)
#define ACE_FIFO_SEND_H

#include "FIFO.h"

class FIFO_Send : public FIFO
  // = TITLE
  //     Sender-side for the bytestream-oriented C++ wrapper for UNIX FIFOs 
  // 
  // = DESCRIPTION
  // 
{
public:
  FIFO_Send (void);
  FIFO_Send (const char *rendezvous, int flags = O_WRONLY, int perms = 0666);

  int open (const char *rendezvous, int flags = O_WRONLY, int perms = 0666);

  ssize_t send (const void *buf, size_t len);
  ssize_t send_n (const void *buf, size_t len);
};

#include "FIFO_Send.i"
#endif /* ACE_FIFO_SEND_H */

