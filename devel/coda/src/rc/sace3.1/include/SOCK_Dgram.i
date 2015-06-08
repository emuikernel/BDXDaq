/* -*- C++ -*- */
/* Contains the definitions for the SOCK datagram abstraction. */

/* Here's the simple-minded constructor. */

inline
SOCK_Dgram::SOCK_Dgram (void)
{
}

/* Send an N byte datagram to ADDR (connectionless version). */

inline ssize_t
SOCK_Dgram::send (const void *buf, size_t n, const Addr &addr, int flags) const
{
  sockaddr *saddr = (sockaddr *) addr.get_addr ();
  size_t   len	  = addr.get_size ();
  return ::sendto (this->get_handle (), (const char *) buf, n, flags, 
		   (struct sockaddr *) saddr, len);
}

/* Recv an n byte datagram to ADDR (connectionless version). */

inline ssize_t
SOCK_Dgram::recv (void *buf, size_t n, Addr &addr, int flags) const
{
  sockaddr *saddr   = (sockaddr *) addr.get_addr ();
  int	   addr_len = addr.get_size ();

  ssize_t status = ::recvfrom (this->get_handle (), (char *) buf, n, flags,
                               (sockaddr *) saddr, (socklen_t *)&addr_len);
			       /*Sergey (sockaddr *) saddr, &addr_len);*/
  addr.set_size (addr_len);
  return status;
}
