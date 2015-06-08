/* -*- C++ -*- */
/* Sender-side for the bytestream-oriented C++ wrapper for UNIX FIFOs */

inline ssize_t
FIFO_Send::send (const void *buf, size_t len)
{
  return ::write (this->get_handle (), (const char *) buf, len);	
}

inline ssize_t
FIFO_Send::send_n (const void *buf, size_t n)
{
  return ace_send_n (this->get_handle (), buf, n);
}




