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
//      CODA RunControl Server Client Connection Acceptor
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rccAcceptor.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#ifndef _CODA_RCC_ACCEPTOR_H
#define _CODA_RCC_ACCEPTOR_H

#include <daqCommon.h>
#include <INET_Addr.h>
#include <SOCK_Acceptor.h>
#include <Reactor.h>
#include <rcSvcProtocol.h>
#include <daqNetData.h>
#include <codaSlist.h>

class rccIO;
class daqRun;

class rccAcceptor: public Event_Handler
{
public:
  // constructor and destructor
  rccAcceptor  (Reactor& r);
  ~rccAcceptor (void);

  // open local address
  int open (void);
  // return number of rc client IO connection
  int numberClients (void);
  // remove a client from the list
  int remove (rccIO* client);
  
  // setup daq run object
  void setupRun (daqRun* run);

  // send a result to all clients
  void sendToAllClients (int command, daqNetData& data, long id = 0);

  // return port number
  unsigned short port_number (void);

  // return client list
  codaSlist& clientList (void);

  // return whether there is a master 
  int hasMaster (void);

protected:
  virtual int get_handle (void) const;
  virtual int handle_input (int fd);
  virtual int handle_close (int fd, Reactor_Mask);

  // delete all clients : only used in the destructor
  void deleteAllClients (void);

private:
  // data areas
  SOCK_Acceptor listener_;
  Reactor&      reactor_;
  codaSlist     rcClientList_;
  // pointer to daqrun object
  daqRun*       run_;
};

#endif
