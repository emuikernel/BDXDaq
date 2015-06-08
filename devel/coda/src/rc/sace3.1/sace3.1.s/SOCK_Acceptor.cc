/* Defines the member functions for the listener
   portion of the SOCK Stream abstraction. */

#include "SOCK_Acceptor.h"
#include "Log_Msg.h"

/* Code shared by constructor and open. */

HANDLE
SOCK_Acceptor::shared_open (const Addr &local_sap, int reuse_addr, int backlog)
{
  int one = 1;

  if (reuse_addr 
      && this->set_option (SOL_SOCKET, SO_REUSEADDR, 
			   &one, sizeof one) == IPC_SAP::INVALID_HANDLE)
    this->close ();      
  else if (::bind (this->get_handle (), (sockaddr *) local_sap.get_addr (), 
		   local_sap.get_size ()) == IPC_SAP::INVALID_HANDLE
	   || ::listen (this->get_handle (), backlog) == IPC_SAP::INVALID_HANDLE)
    this->close ();
  return this->get_handle ();
}

/* General purpose routine for performing server SOCK creation. */

SOCK_Acceptor::SOCK_Acceptor (const Addr &local_sap, int reuse_addr, 
			      int micro_sec_delay, int protocol_family,
			      int backlog, int protocol)
			      : SOCK (SOCK_STREAM, protocol_family, protocol), 
			        micro_second_delay_ (micro_sec_delay)
{

  if (this->shared_open (local_sap, reuse_addr, backlog) == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "SOCK_Acceptor"));
}

/* General purpose routine for performing server SOCK creation. */

HANDLE
SOCK_Acceptor::open (const Addr &local_sap, int reuse_addr, int micro_sec_delay,
		     int protocol_family, int backlog, int protocol)
{
  this->micro_second_delay_ = micro_sec_delay;

  (void) SOCK::open (SOCK_STREAM, protocol_family, protocol);

  if (this->get_handle () == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;
  else
    return this->shared_open (local_sap, reuse_addr, backlog);
}
    
int
SOCK_Acceptor::handle_timed_wait (int restart) const
{
  timeval tv;
  int	  n;

  tv.tv_sec  = this->micro_second_delay_ / 1000000;
  tv.tv_usec = this->micro_second_delay_ % 1000000;

  fd_set  read_handles;
  FD_ZERO (&read_handles);
  FD_SET (this->get_handle (), &read_handles);

  do
    {
#if defined (ACE_SELECT_USES_INT)
      if ((n = ::select (this->get_handle () + 1, (int *) &read_handles, 0, 0, &tv)) == -1)
#else
      if ((n = ::select (this->get_handle () + 1, &read_handles, 0, 0, &tv)) == -1)
#endif /* ACE_SELECT_USES_INT */
	{
	  if (errno == EINTR && restart)
	    continue;
	  else
	    return -1;
	}
      else if (n == 0)
	{
	  errno = this->micro_second_delay_ == 0 ? EAGAIN : ETIMEDOUT;
	  return -1;
	}
    }
  while (n != 1);

  return 0;
}


/* Performs the timed accept operation. */

HANDLE
SOCK_Acceptor::shared_accept (Addr *remote_addr, int restart) const
{
  sockaddr *addr    = 0;
  int	   *len_ptr = 0;
  int	   len;
  HANDLE   new_handle;

  if (remote_addr != 0)
    {
      len     = remote_addr->get_size ();
      len_ptr = &len;
      addr    = (sockaddr *) remote_addr->get_addr ();
    }

  /* Handle the timeout case */

  if (this->micro_second_delay_ > -1 
      && this->handle_timed_wait (restart) == -1)
    return IPC_SAP::INVALID_HANDLE;
  else
    {
      /* Accept the connection! */
  
      do
		/*Sergey new_handle = ::accept (this->get_handle (), addr, len_ptr);*/
	new_handle = ::accept (this->get_handle (), addr, (socklen_t *)len_ptr);
      while (new_handle == IPC_SAP::INVALID_HANDLE && restart && errno == EINTR);

      /* Reset the size of the addr (really only necessary for the UNIX domain sockets)! */
      if (remote_addr != 0)
	remote_addr->set_size (*len_ptr);
    }

  return new_handle;
}

