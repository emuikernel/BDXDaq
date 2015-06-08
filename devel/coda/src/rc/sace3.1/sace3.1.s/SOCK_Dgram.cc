/* Contains the definitions for the SOCK datagram abstraction. */

#include "SOCK_Dgram.h"
#include "Log_Msg.h"
#include "misc.h"

/* Here's the shared open function.  Note that if we are using the
   PF_INET protocol family and the address of LOCAL == the address of 
   the special variable SAP_ANY then we are going to arbitrarily 
   bind to a portnumber. */ 

HANDLE
SOCK_Dgram::shared_open (const Addr &local, int protocol_family)
{
  int error = 0;

  if (&local == &sap_any)
    {
      if (protocol_family == PF_INET 
	  && ::ace_bind_port (this->get_handle ()) == IPC_SAP::INVALID_HANDLE)
	error = 1;
    }
  else if (::bind (this->get_handle (), (sockaddr *) local.get_addr (), 
		   local.get_size ()) == IPC_SAP::INVALID_HANDLE)
    error = 1;
  if (error)
    {
      this->close ();
      this->set_handle (IPC_SAP::INVALID_HANDLE);
    }
  return this->get_handle ();
}

/* Here's the general-purpose constructor used by a connectionless 
   datagram ``server''... */

INLINE
SOCK_Dgram::SOCK_Dgram (const Addr &local, int protocol_family, int protocol)
		       : SOCK (SOCK_DGRAM, protocol_family, protocol)
{
  if (this->shared_open (local, protocol_family) == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "SOCK_Dgram"));
}

/* Here's the general-purpose open routine. */

HANDLE
SOCK_Dgram::open (const Addr &local, int protocol_family, int protocol)
{
  if (SOCK::open (SOCK_DGRAM, protocol_family, 
		  protocol) == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;
  else
    return this->shared_open (local, protocol_family);
}

/* Send an iovec of size N to ADDR as a datagram (connectionless version). */

ssize_t
SOCK_Dgram::send (const iovec iov[], size_t n, const Addr &addr, int flags) const
{
  msghdr send_msg;  

  send_msg.msg_iov          = (iovec *) iov;
  send_msg.msg_iovlen       = n;
  send_msg.msg_name	       = (char *) addr.get_addr ();
  send_msg.msg_namelen      = addr.get_size ();
  return ::sendmsg (this->get_handle (), &send_msg, flags);
}

/* Recv an iovec of size N to ADDR as a datagram (connectionless version). */

ssize_t
SOCK_Dgram::recv (iovec iov[], size_t n, Addr &addr, int flags) const
{
  msghdr recv_msg;  

printf("SOCK_Dgram::recv: reached\n");

  recv_msg.msg_iov          = (iovec *) iov;
  recv_msg.msg_iovlen       = n;
  recv_msg.msg_name	       = (char *) addr.get_addr ();
  recv_msg.msg_namelen      = addr.get_size ();

  ssize_t status = ::recvmsg (this->get_handle (), &recv_msg, flags);
  addr.set_size (recv_msg.msg_namelen);
  return status;
}
