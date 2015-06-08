/* -*- C++ -*- */

#include "FIFO_Send_Msg.h"
#include "Log_Msg.h"

ssize_t
FIFO_Send_Msg::send (const Str_Buf &send_msg)
{
#if defined (ACE_HAS_STREAM_PIPES)
  return ::putmsg (this->get_handle (), (strbuf *) 0, (strbuf *) &send_msg, 0);
#else
  struct iovec iov[2];

  iov[0].iov_base = (char *) &send_msg.len;
  iov[0].iov_len  = sizeof send_msg.len;

  iov[1].iov_base = (char *) send_msg.buf;
  iov[1].iov_len  =  int (send_msg.len);

  return ::writev (this->get_handle (), iov, 2);
#endif /* ACE_HAS_STREAM_PIPES */
}

FIFO_Send_Msg::FIFO_Send_Msg (void)
{
}

HANDLE
FIFO_Send_Msg::open (const char *fifo_name, int flags, int perms)
{
  return FIFO_Send::open (fifo_name, flags | O_WRONLY, perms);	
}

FIFO_Send_Msg::FIFO_Send_Msg (const char *fifo_name, int flags, int perms)
{
  if (this->FIFO_Send_Msg::open (fifo_name, flags, perms) == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "FIFO_Send_Msg"));
}
