/* -*- C++ -*- */
/* Receiver-side for the record-oriented C++ wrapper for UNIX FIFOs */

/* Note that the return values mean different things if
   ACE_HAS_STREAM_PIPES vs. if it doesn't...  See the manual page on
   getmsg(2) and read(2) for more details. */

inline ssize_t
FIFO_Recv_Msg::recv (Str_Buf &recv_msg)
{
#if defined (ACE_HAS_STREAM_PIPES)
  int i = 0;
  return ::getmsg (this->get_handle (), (strbuf *) 0, (strbuf *) &recv_msg, &i);
#else /* Do the ol' 2-read trick... */
  if (::read (this->get_handle (), (char *) &recv_msg.len, sizeof recv_msg.len) != sizeof recv_msg.len)
    return -1;
  else
    return ::read (this->get_handle (), (char *) recv_msg.buf, (int) recv_msg.len);	
#endif /* ACE_HAS_STREAM_PIPES */
}

inline ssize_t
FIFO_Recv_Msg::recv (void *buf, size_t max_len)
{
  Str_Buf recv_msg ((char *) buf, 0, max_len);

  return this->recv (recv_msg);
}

#if defined (ACE_HAS_STREAM_PIPES)
inline ssize_t
FIFO_Recv_Msg::recv (Str_Buf *data, Str_Buf *cntl, int *flags)
{
  return ::getmsg (this->get_handle (), (strbuf *) cntl, (strbuf *) data, flags);
}

inline ssize_t
FIFO_Recv_Msg::recv (int *band, Str_Buf *data, Str_Buf *cntl, int *flags)
{
  return ::getpmsg (this->get_handle (), (strbuf *) cntl, (strbuf *) data, band, flags);
}

#endif /* ACE_HAS_STREAM_PIPES */
