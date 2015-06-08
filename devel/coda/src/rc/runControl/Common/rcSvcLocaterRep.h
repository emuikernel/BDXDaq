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
//       CODA RunControl Service Client Side Svc locater
//
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcSvcLocaterRep.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
#ifndef _CODA_SVC_LOCATER_REP
#define _CODA_SVC_LOCATER_REP

#include <daqCommon.h>
#include <INET_Addr.h>
#include <Reactor.h>
#include <SOCK_Dgram.h>
#include <rcSvcProtocol.h>

class rcServerLocater;

class rcSvcLocaterRep: public Event_Handler
{
public:
  // constructor and destructor
  rcSvcLocaterRep  (Reactor &r, 
		    unsigned short port, 
		    int req, 
		    char* hostname,
		    char* exptname);

  ~rcSvcLocaterRep (void);

  // send out braodcast request
  int sendRequest (void);
  // status of findind server
  int foundServer (void) const;

  virtual int get_handle () const;
  virtual int handle_input (int fd);
  virtual int handle_close (int fd, Reactor_Mask);

private:
  friend class       rcServerLocater;
  // data area
  Reactor&           reactor_;
  SOCK_Dgram         brdcaster_;
  int                foundServer_;
  unsigned           serverPort_;
  // port number into which send udp packet
  unsigned short     port_;
  // request protocol
  int               req_;
  // request experiment name
  char               exptname_[MAX_STRING_LEN];
  // try udp packet to host
  char               serverHost_[MAX_HOSTNAME_LEN];
};
#endif
  
