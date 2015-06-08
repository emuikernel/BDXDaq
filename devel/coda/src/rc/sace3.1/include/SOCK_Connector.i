/* -*- C++ -*- */
/* Defines an active connection factory for the socket wrappers. */

#include "Log_Msg.h"

// This constructor is used by a client when it wants to connect to the 
// specified REMOTE_SAP address using a blocking open. 

inline
SOCK_Connector::SOCK_Connector (SOCK_Stream &new_stream, const Addr &remote_sap, 
			       int blocking_semantics, int protocol_family,
			       int protocol)
{
  if (this->connect (new_stream, remote_sap, blocking_semantics,
		     protocol_family, protocol) == IPC_SAP::INVALID_HANDLE
      && blocking_semantics == ACE_NONBLOCK && errno != EWOULDBLOCK)
    LM_ERROR ((LOG_ERROR, "%p\n", "SOCK_Connector::SOCK_Connector"));
}

// Do-nothing constructor...

inline
SOCK_Connector::SOCK_Connector (void)
{
}
