/* Defines the member functions for the base class of the SOCK
   abstraction. */ 

#include "SOCK.h"
#include "Log_Msg.h"

/* Returns information about the remote peer endpoint (if there is one). */

ssize_t
SOCK::get_remote_addr (Addr &sa) const
{
  int len = sa.get_size ();

  if (::getpeername (this->get_handle (), (sockaddr *) sa.get_addr (),
                     (socklen_t *)&len) == IPC_SAP::INVALID_HANDLE)
		     /*Sergey &len) == IPC_SAP::INVALID_HANDLE)*/
    return IPC_SAP::INVALID_HANDLE;
  else
    {
      sa.set_size (len);
      return 0;
    }
}

HANDLE
SOCK::open (int type, int protocol_family, int protocol)
{
  this->set_handle (::socket (protocol_family, type, protocol));
  return this->get_handle ();
}

/* Close down a SOCK. */

int
SOCK::close (void)
{
  int result = ::close (this->get_handle ());
  this->set_handle (IPC_SAP::INVALID_HANDLE);
  return result;
}

/* General purpose constructor for performing server SOCK creation. */

SOCK::SOCK (int type, int protocol_family, int protocol)
{
  this->set_handle (this->open (type, protocol_family, protocol));

  if (this->get_handle () == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "SOCK::SOCK"));
}
