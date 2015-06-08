/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    FIFO_Send_Msg.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_FIFO_SEND_MSG_H)
#define ACE_FIFO_SEND_MSG_H

#include "FIFO_Send.h"

class FIFO_Send_Msg : public FIFO_Send
  // = TITLE
  //     Sender-side for the Record-oriented C++ wrapper for UNIX FIFOs 
  //
  // = DESCRIPTION
  // 
{
public:
  FIFO_Send_Msg (void);
  FIFO_Send_Msg (const char *rendezvous, int flags = O_WRONLY, int perms = 0666);

  HANDLE open (const char *rendezvous, int flags = O_WRONLY, int perms = 0666);

  ssize_t send (const Str_Buf &msg);
  ssize_t send (const void *buf, size_t len);

#if defined (ACE_HAS_STREAM_PIPES)
  ssize_t send (const Str_Buf *data, const Str_Buf *cntl = 0, int flags = 0);
  ssize_t send (int band, const Str_Buf *data, const Str_Buf *cntl = 0, int flags = MSG_BAND);
#endif /* ACE_HAS_STREAM_PIPES */
};

#include "FIFO_Send_Msg.i"
#endif /* __FIFO_SEND_MSG_H */

