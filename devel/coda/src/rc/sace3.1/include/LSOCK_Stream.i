/* -*- C++ -*- */
/* Create a Local SOCK stream. */

/* Sets both the file descriptors... Overrides handle from the base classes. */

inline void
LSOCK_Stream::set_handle (HANDLE fd)
{
  this->SOCK_Stream::set_handle (fd);
  this->LSOCK::set_handle (fd);
}

inline HANDLE
LSOCK_Stream::get_handle (void) const
{
  return this->SOCK_Stream::get_handle ();
}



