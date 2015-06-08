/* -*- C++ -*- */
/* Create a Local SOCK datagram. */

/* Do nothing constructor. */

inline
LSOCK_CODgram::LSOCK_CODgram (void)
{
}

inline void
LSOCK_CODgram::set_handle (HANDLE h)
{
  this->SOCK_CODgram::set_handle (h);
  this->LSOCK::set_handle (h);
}

inline HANDLE
LSOCK_CODgram::get_handle (void) const
{
  return this->SOCK_CODgram::get_handle ();
}
