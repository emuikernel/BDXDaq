/* Send an n byte message to the connected socket. */

inline ssize_t  
SOCK_IO::send (const void *buf, size_t n, int flags) const
{
  return ::send (this->get_handle (), (const char *) buf, n, flags);
}

/* Recv an n byte message from the connected socket. */

inline ssize_t  
SOCK_IO::recv (void *buf, size_t n, int flags) const
{
  return ::recv (this->get_handle (), (char *) buf, n, flags);
}

/* Send an n byte message to the connected socket. */

inline ssize_t  
SOCK_IO::send (const void *buf, size_t n) const
{
  return ::write (this->get_handle (), (const char *) buf, n);
}

/* Recv an n byte message from the connected socket. */

inline ssize_t  
SOCK_IO::recv (void *buf, size_t n) const
{
  return ::read (this->get_handle (), (char *) buf, n);
}

/* Send a vector of n byte messages to the connected socket. */

inline ssize_t  
SOCK_IO::send (const iovec iov[], size_t n) const
{
  return ::writev (this->get_handle (), (iovec *) iov, n);
}

/* Recv an n byte message from the connected socket. */

inline ssize_t  
SOCK_IO::recv (iovec iov[], size_t n) const
{
  return ::readv (this->get_handle (), (iovec *) iov, n);
}

