/* -*- C++ -*- */
/* Defines an active connection factory for the socket wrappers. */

inline
LSOCK_Connector::LSOCK_Connector (void)
{
}

// Establish a connection (blocking).
inline
LSOCK_Connector::LSOCK_Connector (LSOCK_Stream &new_stream, 
				  const UNIX_Addr &remote_sap, 
				  int blocking_semantics, 
				  int protocol_family, 
				  int protocol)
  : SOCK_Connector (new_stream, remote_sap, blocking_semantics, protocol_family, protocol)
{
  // This is necessary due to the weird inheritance relationships of LSOCK_Stream.
  new_stream.set_handle (new_stream.get_handle ());
}

// Establish a connection (blocking).
inline int 
LSOCK_Connector::connect (LSOCK_Stream &new_stream, 
			  const UNIX_Addr &remote_sap, 
			  int blocking_semantics, 
			  int protocol_family, 
			  int protocol)
{
  int result = SOCK_Connector::connect (new_stream, remote_sap, blocking_semantics, 
					protocol_family, protocol);
  if (result != -1)
    // This is necessary due to the weird inheritance relationships of LSOCK_Stream.
    new_stream.set_handle (new_stream.get_handle ());
  return result;
}

