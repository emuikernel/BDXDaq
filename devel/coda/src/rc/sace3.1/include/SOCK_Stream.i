/* -*- C++ -*- */
#include "SOCK_Stream.h"

/* Shut down just the reading end of a SOCK. */

inline int
SOCK_Stream::close_reader (void)
{
  int result = ::shutdown (this->get_handle (), 0);
  return result;
}

/* Shut down just the writing end of a SOCK. */

inline int
SOCK_Stream::close_writer (void)
{
  int result = ::shutdown (this->get_handle (), 1);
  return result;
}

/* Receive exactly BUF_SIZE bytes from file descriptor THIS->SOK_FD 
   into BUF.  Keep trying until this many bytes are received. */


#ifdef Darwin
inline ssize_t
#else
inline int
#endif
SOCK_Stream::recv_n (void *buf, int buf_size, int flags) const
{
  return ::ace_recv_n (this->get_handle (), buf, buf_size, flags);
}

/* Send exactly N bytes from BUF to THIS->SOK_FD.  Keeping trying 
   until this many bytes are sent. */

inline ssize_t
SOCK_Stream::send_n (const void *buf, int buf_size, int flags) const
{
  return ace_send_n (this->get_handle (), buf, buf_size, flags);
}

/* Receive exactly BUF_SIZE bytes from file descriptor THIS->SOK_FD 
   into BUF.  Keep trying until this many bytes are received. */

#ifdef Darwin
inline ssize_t
#else
inline int
#endif
SOCK_Stream::recv_n (void *buf, int buf_size) const
{
  return ::ace_recv_n (this->get_handle (), buf, buf_size);
}

/* Send exactly N bytes from BUF to THIS->SOK_FD.  Keeping trying 
   until this many bytes are sent. */

inline ssize_t
SOCK_Stream::send_n (const void *buf, int buf_size) const
{
  return ace_send_n (this->get_handle (), buf, buf_size);
}

inline ssize_t  
SOCK_Stream::send_urg (void *ptr, int len)
{
  return ::send (this->get_handle (), (char *) ptr, len, MSG_OOB);
}

inline ssize_t  
SOCK_Stream::recv_urg (void *ptr, int len)
{
  return ::recv (this->get_handle (), (char *) ptr, len, MSG_OOB);
}

