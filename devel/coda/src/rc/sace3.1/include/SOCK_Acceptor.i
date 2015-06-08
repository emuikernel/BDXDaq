/* -*- C++ -*- */
/* Defines the member functions for the listener
   of the Stream SOCK abstraction. */

#include "SOCK_Stream.h"

/* Do nothing routine for constructor. */

inline
SOCK_Acceptor::SOCK_Acceptor (void): micro_second_delay_ (-1)
{
}

/* General purpose routine for accepting new connections. */

inline HANDLE
SOCK_Acceptor::accept (SOCK_Stream &new_stream, Addr *remote_addr, int restart) const
{
  HANDLE new_handle = this->shared_accept (remote_addr, restart);
  new_stream.set_handle (new_handle);
  return new_handle;
}
