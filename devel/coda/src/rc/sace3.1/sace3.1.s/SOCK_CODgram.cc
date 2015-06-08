/* Contains the definitions for the SOCK connection-oriented 
   datagram abstraction. */

#include "SOCK_CODgram.h"
#include "Log_Msg.h"
#include "misc.h"

/* Here's the general-purpose constructor. */

SOCK_CODgram::SOCK_CODgram (const Addr &remote, const Addr &local, 
			    int protocol_family, int protocol)
{
  if (this->open (remote, local, protocol_family, protocol) == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "SOCK_CODgram"));
}

/* This is the general-purpose open routine.  Note that it performs
   a different set of functions depending on the LOCAL and REMOTE
   addresses passed to it.  Here's the basic logic:
   
   1. remote == sap_any && local == sap_any
         if protocol_family == PF_INET then 
	     bind the local address to a randomly generated port number... 

   2. remote == sap_any && local != sap_any
         we are just binding the local address
	 (used primarily by servers)

   3. remote != sap_any && local == sap_any
         we are connecting to the remote address
	 (used primarily by clients)

   4. remote != sap_any && local != sap_any
         we are binding to the local address 
	 and connecting to the remote address 
*/

HANDLE
SOCK_CODgram::open (const Addr &remote, const Addr &local,
		    int protocol_family, int protocol)
{
  if (SOCK::open (SOCK_DGRAM, protocol_family, protocol) == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;
  else
    {
      int error = 0;

      if (&local == &sap_any && &remote == &sap_any)
	{
	  /* Assign an arbitrary port number from the transient range!! */

	  if (protocol_family == PF_INET 
	      && ::ace_bind_port (this->get_handle ()) == IPC_SAP::INVALID_HANDLE)
	    error = 1;
	}
      /* We are binding just the local address. */
      else if (&local != &sap_any && &remote == &sap_any)
	{
	  if (::bind (this->get_handle (), (sockaddr *) local.get_addr (), 
		      local.get_size ()) == IPC_SAP::INVALID_HANDLE)
	    error = 1;
	}
      /* We are connecting to the remote address. */
      else if (&local == &sap_any && &remote != &sap_any)
	{
	  if (::connect (this->get_handle (), (sockaddr *) remote.get_addr (), 
			 remote.get_size ()) == IPC_SAP::INVALID_HANDLE)
	    error = 1;
	}
      /* We are binding to the local address and connecting to the
	 remote addresses. */ 
      else
	{
	  if (::bind (this->get_handle (), (sockaddr *) local.get_addr (), 
		      local.get_size ()) == IPC_SAP::INVALID_HANDLE
	      || ::connect (this->get_handle (), (sockaddr *) remote.get_addr (), 
			    remote.get_size ()) == IPC_SAP::INVALID_HANDLE)
	    error = 1;
	}
      if (error)
	{
	  this->close ();
	  this->set_handle (IPC_SAP::INVALID_HANDLE);
	}
      return this->get_handle ();
    }
}
