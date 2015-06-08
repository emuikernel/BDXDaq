/* Defines the member functions for the listener
   portion of the local SOCK stream abstraction. */

#include "Log_Msg.h"
#include "LSOCK_Acceptor.h"

#if defined (__INLINE__)
#include "LSOCK_Acceptor.i"
#endif /* __INLINE__ */

/* Do nothing routine for constructor. */

LSOCK_Acceptor::LSOCK_Acceptor (void)
{
}

HANDLE
LSOCK_Acceptor::open (const Addr &remote_sap, int reuse_addr, 
		      int micro_sec_delay, int protocol_family, 
		      int backlog, int protocol)
{
  this->local_addr_ = *((UNIX_Addr *) &remote_sap); // This is a gross hack...
  return SOCK_Acceptor::open (remote_sap, reuse_addr, micro_sec_delay, 
			      protocol_family, backlog, protocol);
}

/* General purpose routine for performing server SOCK creation. */

LSOCK_Acceptor::LSOCK_Acceptor (const Addr &remote_sap, int reuse_addr, 
				int micro_sec_delay, int protocol_family, 
				int backlog, int protocol)
{
  if (this->open (remote_sap, reuse_addr, micro_sec_delay, 
		  protocol_family, backlog, protocol) == -1)
    LM_ERROR ((LOG_ERROR, "LSOCK_Acceptor::LSOCK_Acceptor"));
}

/* General purpose routine for accepting new connections. 
   Note that if THIS->MICRO_SECOND_DELAY == -1 we block on ::accept.
   Otherwise, we use set a timeval struct to wait for the desired
   number of micro seconds using ::select. */

HANDLE
LSOCK_Acceptor::accept (LSOCK_Stream &new_local_ipc_sap, 
			Addr *remote_addr, int restart) const
{
  HANDLE new_handle = SOCK_Acceptor::shared_accept (remote_addr, restart);
  new_local_ipc_sap.set_handle (new_handle);
  return new_handle;
}

// Close down the UNIX domain stream and remove the rendezvous point from the file system.

int
LSOCK_Acceptor::remove (void)
{
  int result = this->close ();
  return ::unlink (this->local_addr_.get_path_name ()) == -1 || result == -1 ? -1 : 0;
}

// Return the local endpoint address.

int 
LSOCK_Acceptor::get_local_addr (Addr &a) const
{
  a = this->local_addr_;
  return 0;
}

