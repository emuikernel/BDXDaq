/* Defines an active connection factory for the socket wrappers. */

#include "SOCK_Connector.h"

// Actively connect and produce a new SOCK_Stream if things go well...

int
SOCK_Connector::connect (SOCK_Stream &new_stream, const Addr &remote_sap, 
			 int blocking_semantics, int protocol_family, int protocol)
{
  int result = 0;

  // Only open a new socket if we don't already have a valid descriptor.
  if (new_stream.get_handle () == IPC_SAP::INVALID_HANDLE)
    {
      if (SOCK::open (SOCK_STREAM, protocol_family, protocol) == IPC_SAP::INVALID_HANDLE)
	return -1;
    }
  else // Borrow the handle from the NEW_STREAM. 
    this->set_handle (new_stream.get_handle ());

  sockaddr *addr = (sockaddr *) remote_sap.get_addr ();
  size_t   size	 = remote_sap.get_size ();

  // Enable non-blocking, if required.
  if (blocking_semantics == ACE_NONBLOCK && this->enable (ACE_NONBLOCK) == -1)
    result = -1;
  else if (::connect (this->get_handle (), addr, size) == IPC_SAP::INVALID_HANDLE)
    {
      // Check to see if we simply haven't finished connecting yet...
      if (blocking_semantics == ACE_NONBLOCK && errno == EINPROGRESS)
	errno = EWOULDBLOCK; // EINPROGRESS is too weird, let's use EWOULDBLOCK instead...
      result = -1;
    }

  // EISCONN is treated specially since this routine may be used to check 
  // if we are already connected.
  if (result != -1 || errno == EISCONN)
    {
      new_stream.set_handle (this->get_handle ());
      this->set_handle (IPC_SAP::INVALID_HANDLE);
    }
  else if (errno != EWOULDBLOCK)
    {
      // If things have gone wrong, close down shop and return an error.
      this->close ();
      this->set_handle (IPC_SAP::INVALID_HANDLE);
    }
  return result;
}

// Try to complete a non-blocking connection.

int
SOCK_Connector::complete (SOCK_Stream &new_stream, Addr &remote_sap)
{
  // Note, this code is not fully tested...
  int fd = this->get_handle ();
  fd_set rd_fds;
  fd_set wr_fds;
#ifndef HP_UX
  struct timeval tv = {0, 0};
#else
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
#endif

  FD_ZERO (&rd_fds);
  FD_ZERO (&wr_fds);
  FD_SET (fd, &wr_fds);
  FD_SET (fd, &rd_fds);

#if defined (ACE_SELECT_USES_INT)
   if (::select (fd + 1, (int *) &rd_fds, (int *) &wr_fds, 0, &tv) == 0)
#else
   if (::select (fd + 1, &rd_fds, &wr_fds, 0, &tv) == 0)
#endif /* ACE_SELECT_USES_INT */
    {
      errno = EWOULDBLOCK;
      return -1;
    }
  else if (FD_ISSET (fd, &wr_fds) || FD_ISSET (fd, &rd_fds))
    {
      sockaddr_in addr;
      int len = sizeof addr;

      /*Sergey if (::getpeername (fd, (sockaddr *) &addr, &len) == -1)*/
      if (::getpeername (fd, (sockaddr *) &addr, (socklen_t *)&len) == -1)
	{
	  int err = errno;
	  this->close ();
	  new_stream.set_handle (IPC_SAP::INVALID_HANDLE);
	  errno = err;
	  return -1;
	}
      else
	{
	  // We've successfully connected!
	  new_stream.set_handle (this->get_handle ());
	  new_stream.get_remote_addr (remote_sap);
	  this->set_handle (IPC_SAP::INVALID_HANDLE);
	  return 0;
	}
    }
  else
    return -1;
}

