/* -*- C++ -*- */
/* Contains the definitions for the SOCK datagram abstraction. */

#include "SOCK_Dgram_Brdcast.h"

/* Here's the simple-minded constructor. */

inline
SOCK_Dgram_Brdcast::SOCK_Dgram_Brdcast (void): ifptr_ (0)
{
}

/* Broadcast an N byte datagram to ADDR (note that addr must
   be preassigned to the broadcast address of the subnet...) */

inline ssize_t
SOCK_Dgram_Brdcast::send (const void *buf, size_t n, const Addr &addr, int flags) const
{
  sockaddr *saddr = (sockaddr *) addr.get_addr ();
  size_t   len	  = addr.get_size ();
  return ::sendto (this->get_handle (), (const char *) buf, n, flags, 
		   (struct sockaddr *) saddr, len);
}


