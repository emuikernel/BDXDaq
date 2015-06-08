//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      CODA RunControl Port Handler (UDP Service)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: portHandler.h,v $
//   Revision 1.2  1998/11/05 20:11:58  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#ifndef _CODA_PORT_HANDLER_H
#define _CODA_PORT_HANDLER_H

#include <daqCommon.h>
#include <rcSvcProtocol.h>
#include <INET_Addr.h>
#include <SOCK_Dgram.h>
#include <Reactor.h>
#include <daqRun.h>

class portHandler: public Event_Handler
{
public:
  // constructor and destructor
  portHandler  (Reactor& r, char *exptname, int exptid);

  ~portHandler (void);

  void acceptorPort (unsigned short portnum);
  int register_system(daqRun *daqrun);

  // open broadcast listener port
  int open (int async = 0);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // return port number 
  unsigned short port_number (void);
#endif
  daqRun         *daqrun_;
  
protected:
  // send back reply ro the client who ask the server location
  int findRcSvcReply (INET_Addr& reqAddr);

private:
  virtual int get_handle (void) const;
  virtual int handle_input (int fd);
  virtual int handle_close (int fd, Reactor_Mask);


  // data area
  Reactor&       reactor_;
  SOCK_Dgram     brdcastListener_;
  // port number for TCP port for client to connect
  unsigned short acceptorPort_;
  // experiment name 
  char           exptname_[MAX_STRING_LEN];
  // experiment id
  int            exptid_;
};
#endif
