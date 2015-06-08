/* -*- C++ -*- */
/* Create a Local SOCK, which is used for passing file descriptors. */

/* Simple-minded constructor. */

inline
LSOCK::LSOCK (void)
{
}

/* Sets the underlying file descriptor. */

inline void
LSOCK::set_handle (HANDLE handle)
{
  this->aux_handle_ = handle;
}

/* Gets the underlying file descriptor. */

inline HANDLE
LSOCK::get_handle (void) const
{
  return this->aux_handle_;
}

/* Sets the underlying file descriptor. */

inline
LSOCK::LSOCK (int handle)
  : aux_handle_ (handle)
{
}
