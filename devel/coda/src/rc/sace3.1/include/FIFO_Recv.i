/* -*- C++ -*- */

inline ssize_t
FIFO_Recv::recv (void *buf, size_t len)
{
  return ::read (this->get_handle (), (char *) buf, len);
}

inline ssize_t
FIFO_Recv::recv_n (void *buf, size_t n)
{
  return ace_recv_n (this->get_handle (), buf, n);
}
