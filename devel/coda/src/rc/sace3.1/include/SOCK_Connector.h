/* -*- C++ -*- */

// ============================================================================
//
// = LIBRARY
//    ACE
// 
// = FILENAME
//    SOCK_Connector.h 
//
// = AUTHOR
//    Doug Schmidt 
// 
// ============================================================================

#if !defined (ACE_SOCK_CONNECTOR_H)
#define ACE_SOCK_CONNECTOR_H

#include "SOCK_Stream.h"

class SOCK_Connector : public SOCK
  // = TITLE
  //     Defines an active connection factory for the socket wrappers. 
  // 
  // = DESCRIPTION
  // 
{
public:
  SOCK_Connector (void);

  SOCK_Connector (SOCK_Stream &new_stream, const Addr &remote_sap,
		  int blocking_semantics = 0, int protcol_family = PF_INET, 
		  int protocol = 0);
  // Actively connect and produce a new SOCK_Stream if things go well...

  int connect (SOCK_Stream &new_stream, const Addr &remote_sap,
	       int blocking_semantics = 0, int protcol_family = PF_INET, 
	       int protocol = 0); 
  // Actively connect and produce a new SOCK_Stream if things go well...

  int complete (SOCK_Stream &new_stream, Addr &remote_sap);
  // Try to complete a non-blocking connection.
};

#include "SOCK_Connector.i"

#endif /* _SOCK_CONNECTOR_H */
