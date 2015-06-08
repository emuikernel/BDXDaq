/* -*- C++ -*- */
/* Create a Local SOCK datagram. */

/* The "do nothing" constructor. */

inline
LSOCK_Dgram::LSOCK_Dgram (void)
{
}

inline void
LSOCK_Dgram::set_handle (HANDLE h)
{
  this->SOCK_Dgram::set_handle (h);
  this->LSOCK::set_handle (h);
}

inline HANDLE
LSOCK_Dgram::get_handle (void) const
{
  return this->SOCK_Dgram::get_handle ();
}

