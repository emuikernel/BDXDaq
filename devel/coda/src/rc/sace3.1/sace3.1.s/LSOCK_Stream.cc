/* Defines the member functions for the local
   SOCK stream abstraction. */

#include "LSOCK_Stream.h"

/* Send a readv-style vector of buffers, along with an open I/O handle. */

ssize_t
LSOCK_Stream::send_msg (const iovec iov[], size_t n, int fd)
{
  msghdr send_msg;

  send_msg.msg_iov	    = (iovec *) iov;
  send_msg.msg_iovlen	    = n;
  send_msg.msg_name	    = (char *) 0;
  send_msg.msg_namelen	    = 0;

  return ::sendmsg (this->SOCK_Stream::get_handle (), &send_msg, 0);
}

/* Read a readv-style vector of buffers, along with an open I/O handle. */

ssize_t
LSOCK_Stream::recv_msg (iovec iov[], size_t n, int &fd)
{
  msghdr recv_msg;
  
  recv_msg.msg_iov	    = (iovec *) iov;
  recv_msg.msg_iovlen	    = n;
  recv_msg.msg_name	    = (char *) 0;
  recv_msg.msg_namelen	    = 0;

  return ::recvmsg (this->SOCK_Stream::get_handle (), &recv_msg, 0);
}
