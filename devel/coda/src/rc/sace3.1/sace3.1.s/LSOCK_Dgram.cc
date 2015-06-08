/* Defines the member functions for the Local 
   SOCK datagram abstraction. */

#include "LSOCK_Dgram.h"
#include "Log_Msg.h"

/* Here's the general-purpose open routine. */

int
LSOCK_Dgram::open (const Addr &local, int protocol_family, int protocol)
{
  if (SOCK_Dgram::open (local, protocol_family, protocol) == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;
  LSOCK::set_handle (this->SOCK_Dgram::get_handle ());
  return this->SOCK_Dgram::get_handle ();
}

/* Create a local SOCK datagram. */

LSOCK_Dgram::LSOCK_Dgram (const Addr &local, int protocol_family, int protocol)
{
  if (this->open (local, protocol_family, protocol) == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "LSOCK_Dgram"));
}

