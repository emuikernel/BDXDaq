/* Contains the definitions for the SOCK datagram abstraction. */

#include "SOCK_Dgram_Brdcast.h"
#include "Log_Msg.h"

/* Here's the general-purpose constructor used by a connectionless 
   datagram ``server''... */

SOCK_Dgram_Brdcast::SOCK_Dgram_Brdcast (const Addr &local, int protocol_family, int protocol)
				       : SOCK_Dgram (local, protocol_family, protocol), ifptr_ (0)
{
  if (this->mk_broadcast () == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "SOCK_Dgram_Brdcast"));
}

/* Here's the general-purpose open routine. */

HANDLE
SOCK_Dgram_Brdcast::open (const Addr &local, int protocol_family, int protocol)
{
  if (this->SOCK_Dgram::open (local, protocol_family, 
			      protocol) == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;

  if (this->mk_broadcast () == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;

  return this->get_handle ();
}

/* Make broadcast available for Datagram socket */

int
SOCK_Dgram_Brdcast::mk_broadcast (void)
{
  int one = 1;
  if (::setsockopt(this->get_handle (), SOL_SOCKET, SO_BROADCAST, 
		   (char *) &one, sizeof one) == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;

  /* Get interface structure */
  
  char buf[BUFSIZ];
  struct ifconf ifc;
  struct ifreq *ifr;

  struct ifreq  flags;
  struct ifreq  if_req;

  int s = this->get_handle ();

  ifc.ifc_len = sizeof buf;
  ifc.ifc_buf = buf;

  if (ioctl (s, SIOCGIFCONF, (char *)&ifc) < 0) 
    LM_ERROR_RETURN ((LOG_ERROR, "%p\n", 
		      "SOCK_Dgram_Brdcast::mk_broadcast: ioctl (get interface configuration)"),
		     IPC_SAP::INVALID_HANDLE);

  ifr = ifc.ifc_req;

  for (int n = ifc.ifc_len / sizeof (struct ifreq) ; n > 0; n--, ifr++) 
    {
    
      if (ifr->ifr_addr.sa_family != AF_INET) 
	{
#ifdef _CDEV_DEBUG
	  LM_ERROR ((LOG_ERROR, "%p\n", "SOCK_Dgram_Brdcast::mk_broadcast: Not AF_INET"));
#endif
	  continue;
	}

      flags = if_req = *ifr;
    
      if (ioctl(s, SIOCGIFFLAGS, (char *)&flags) < 0) 
	{
	  LM_ERROR ((LOG_ERROR, "%p\n", 
		     "SOCK_Dgram_Brdcast::mk_broadcast: ioctl (get interface flags)"));
	  continue;
	}   

      if ((flags.ifr_flags & IFF_UP) == 0) 
	{
	  LM_ERROR ((LOG_ERROR, "%p\n", 
		     "SOCK_Dgram_Brdcast::mk_broadcast: Network interface is not up"));
	  continue;
	}

      if (flags.ifr_flags & IFF_LOOPBACK) 
	continue;

      if (flags.ifr_flags & IFF_BROADCAST) 
	{
	  if (ioctl(s, SIOCGIFBRDADDR, (char *)&if_req) < 0) 
	    LM_ERROR ((LOG_ERROR, "%p\n", 
		       "SOCK_Dgram_Brdcast::mk_broadcast: ioctl (get broadaddr)"));
	  else 
	    {
	      struct ifnode *ptr = new ifnode;
	      ptr->brdAddr       = *(struct sockaddr_in *) &if_req.ifr_broadaddr;
	      ptr->next		 = this->ifptr_;
	      this->ifptr_	 = ptr;
	    }
	}
      else 
	LM_ERROR ((LOG_ERROR, "%p\n", 
		   "SOCK_Dgram_Brdcast::mk_broadcast: Broadcast is not enable for this interface."));
    }

  return this->ifptr_ == 0 ? IPC_SAP::INVALID_HANDLE : 0;
}

/* Broadcast the datagram to every interface.  Returns the average number 
   of bytes sent. */

ssize_t
SOCK_Dgram_Brdcast::send (const void *buf, size_t n,
			  unsigned short port_number, int flags) const
{
  size_t  iterations = 0;
  ssize_t bytes	     = 0;

  if (this->ifptr_ == 0) 
    return IPC_SAP::INVALID_HANDLE;

  for (struct ifnode *tptr = this->ifptr_;
       tptr != 0;
       tptr = tptr->next)
    {
      sockaddr_in to_addr = tptr->brdAddr;
      to_addr.sin_port	  = htons (port_number);

      bytes += ::sendto (this->get_handle(), (const char *) buf, n, flags, 
			 (struct sockaddr *) &to_addr, sizeof to_addr);
      iterations++;
    }

  return iterations == 0 ? 0 : bytes / iterations;
}

/* Broadcast datagram to every interfaces */

ssize_t
SOCK_Dgram_Brdcast::send (const iovec iov[], size_t n, 
			  unsigned short port_number, int flags) const
{
  if (this->ifptr_ == 0)
    return IPC_SAP::INVALID_HANDLE;
  
  for (struct ifnode *tptr = this->ifptr_; tptr != 0; tptr++) 
    {
      struct sockaddr_in to_addr = tptr->brdAddr;
      msghdr send_msg;

      to_addr.sin_port	         = htons (port_number);
      send_msg.msg_iov		 = (iovec *) iov;
      send_msg.msg_iovlen	 = n;
      send_msg.msg_name		 = (char *) &to_addr;
      send_msg.msg_namelen	 = sizeof to_addr;
      ::sendmsg (this->get_handle (), &send_msg, flags);
    }

  return 0;
}

/* Broadcast an IO_Vector of size N to ADDR as a datagram (note that addr must
   be preassigned to the broadcast address of the subnet...) */

ssize_t
SOCK_Dgram_Brdcast::send (const iovec iov[], size_t n, const Addr &addr, int flags) const
{
  msghdr   msg;  

  msg.msg_iov          = (iovec *) iov;
  msg.msg_iovlen       = n;
  msg.msg_name	       = (char *) addr.get_addr ();
  msg.msg_namelen      = addr.get_size ();
  return ::sendmsg (this->get_handle (), &msg, flags);
}
