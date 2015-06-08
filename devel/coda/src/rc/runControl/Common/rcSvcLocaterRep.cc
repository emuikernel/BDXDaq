//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
//
// Description:
//       CODA RunControl Server Client Side Svc locater
//
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcSvcLocaterRep.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//

#include <stdio.h>
#include <string.h>

#include "rcSvcLocaterRep.h"
#include "rcServerLocater.h"

rcSvcLocaterRep::rcSvcLocaterRep (Reactor& r, 
				  unsigned short port,
				  int req,
				  char *hostname,
				  char *exptname)
:reactor_ (r), brdcaster_ (sap_any),
 foundServer_ (0), port_ (port), req_ (req)
{
#ifdef _TRACE_OBJECTS 
  printf ("Create rcSvcLocaterRep Class Objects\n");
#endif
  ::strncpy (exptname_, exptname, sizeof (exptname_));
  ::strncpy (serverHost_, hostname, sizeof (serverHost_));
}

int
rcSvcLocaterRep::handle_close (int, Reactor_Mask)
{
  return brdcaster_.close ();
}

rcSvcLocaterRep::~rcSvcLocaterRep (void)
{
  // do not delete reqBuf_ it is just a pointer to a memory 
  // allocated by Server_Locater
  handle_close (-1, Event_Handler::READ_MASK);
}

int
rcSvcLocaterRep::foundServer (void) const
{
printf("rcSvcLocaterRep::foundServer returns %d\n",foundServer_);
  return foundServer_;
}

int
rcSvcLocaterRep::get_handle (void) const
{
  return brdcaster_.get_handle ();
}

int
rcSvcLocaterRep::handle_input (int)
{
  INET_Addr sa;
  unsigned  short port;
  int       n;

  if ( (n = brdcaster_.recv (&port, sizeof (unsigned short), sa)) == -1){
    return -1;
  }
  else {
    serverPort_ = ntohs (port);

    printf("rcSvcLocaterRep::handle_input: server is on %s and at TCP port %d\n",
      serverHost_, serverPort_);

    if (serverPort_ != 0)
    {
      foundServer_ = 1;
      reactor_.remove_handler (this, Event_Handler::READ_MASK);
    }
    return 0;
  }
}


int
rcSvcLocaterRep::sendRequest (void)
{
  char   buffer[128];
  
  int   req = htonl (req_);
  memcpy (buffer, &req, sizeof (int));
  int i = sizeof (int);
  memcpy (&(buffer[i]), exptname_, MAX_STRING_LEN);
  i += MAX_STRING_LEN;
  INET_Addr addr (port_, serverHost_);
printf("rcSvcLocaterRep::sendRequest: port=%d buffer>%s<\n",port_,&(buffer[sizeof (int)]));

  return brdcaster_.send (buffer, i, addr);
}




