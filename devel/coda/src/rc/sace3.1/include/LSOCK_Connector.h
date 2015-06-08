/* -*- C++ -*- */
// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    LSOCK_Connector.h
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_LOCAL_SOCK_CONNECTOR_H)
#define ACE_LOCAL_SOCK_CONNECTOR_H

#include "SOCK_Connector.h"
#include "LSOCK_Stream.h"
#include "UNIX_Addr.h"

class LSOCK_Connector : public SOCK_Connector
  // = TITLE
  //     Defines the format and interface for the connector side of the 
  // local SOCK Stream. 
  //
  // = DESCRIPTION
  //
{
public:
  LSOCK_Connector (void);

  LSOCK_Connector (LSOCK_Stream &new_stream, 
		   const UNIX_Addr &remote_sap, 
		   int blocking_semantics = 0, 
		   int protcol_family = PF_UNIX,
		   int protocol = 0);
  // Actively connect and produce a new LSOCK_Stream if things go well...

  int connect (LSOCK_Stream &new_stream, 
	       const UNIX_Addr &remote_sap, 
	       int blocking_semantics = 0, 
	       int protcol_family = PF_UNIX,
	       int protocol = 0);
  // Actively connect and produce a new LSOCK_Stream if things go well...
};

#include "LSOCK_Connector.i"

#endif /* _LOCAL_SOCK_CONNECTOR_H */
