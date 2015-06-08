/* Contains the definitions for the Local SOCK 
   connection-oriented datagram abstraction. */

#include "LSOCK_CODgram.h"
#include "Log_Msg.h"

/* Here's the general-purpose open routine. */

int
LSOCK_CODgram::open (const Addr &remote, const Addr &local, 
		     int protocol_family, int protocol)
{
  if (SOCK_CODgram::open (remote, local, protocol_family, protocol) == IPC_SAP::INVALID_HANDLE)
    return IPC_SAP::INVALID_HANDLE;
  LSOCK::set_handle (this->get_handle ());
  return 0;
}

/* Create a local SOCK datagram. */

LSOCK_CODgram::LSOCK_CODgram (const Addr &remote, const Addr &local, int protocol_family, int protocol)
{
  if (this->open (remote, local, protocol_family, protocol) == IPC_SAP::INVALID_HANDLE)
    LM_ERROR ((LOG_ERROR, "%p\n", "LSOCK_CODgram"));
}
