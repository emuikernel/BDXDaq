/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    FIFO_Recv_Msg.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_FIFO_RECV_MSG_H)
#define ACE_FIFO_RECV_MSG_H

#include "FIFO_Recv.h"

class FIFO_Recv_Msg : public FIFO_Recv
  // = TITLE
  //     Receiver-side for the record-oriented C++ wrapper for UNIX FIFOs
  // 
  // = DESCRIPTION
  // 
{
public:
  FIFO_Recv_Msg (void);
  FIFO_Recv_Msg (const char *rendezvous, int flags = O_CREAT | O_RDONLY, int perms = 0666, int persistent = 1);
  HANDLE open (const char *rendezvous, int flags = O_CREAT | O_RDONLY, int perms = 0666, int persistent = 1);
  ssize_t recv (Str_Buf &msg);
  ssize_t recv (void *buf, size_t len);

#if defined (ACE_HAS_STREAM_PIPES)
  ssize_t recv (Str_Buf *data, Str_Buf *cntl, int *flags);
  ssize_t recv (int *band, Str_Buf *data, Str_Buf *cntl, int *flags);
#endif /* ACE_HAS_STREAM_PIPES */
};

#include "FIFO_Recv_Msg.i"
#endif /* ACE_FIFO_RECV_MSG_H */
