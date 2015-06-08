/* -*- C++ -*- */

inline ssize_t
FIFO_Send_Msg::send (const void *buf, size_t len)
{
  Str_Buf send_msg ((char *) buf, len);

  return this->send (send_msg);
}

#if defined (ACE_HAS_STREAM_PIPES)
inline ssize_t
FIFO_Send_Msg::send (const Str_Buf *data, const Str_Buf *cntl, int flags)
{
  return ::putmsg (this->get_handle (), (strbuf *) cntl, (strbuf *) data, flags);
}

inline ssize_t
FIFO_Send_Msg::send (int band, const Str_Buf *data, const Str_Buf *cntl, int flags)
{
  return ::putpmsg (this->get_handle (), (strbuf *) cntl, (strbuf *) data, band, flags);
}
#endif /* ACE_HAS_STREAM_PIPES */
