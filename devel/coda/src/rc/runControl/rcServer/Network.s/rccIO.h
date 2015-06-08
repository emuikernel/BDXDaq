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
//      CODA RunControl Server to Client IO Handler
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rccIO.h,v $
//   Revision 1.3  1997/02/25 14:32:18  chen
//   Remove non block option from socket IO
//
//   Revision 1.2  1997/02/03 13:47:34  heyes
//   add ask command
//
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#ifndef _CODA_RCC_IO_H
#define _CODA_RCC_IO_H

#include <daqCommon.h>
#include <SOCK_Stream.h>
#include <INET_Addr.h>
#include <Event_Handler.h>
#include <Reactor.h>
#include <rcMsg.h>
#include "rccAcceptor.h"

class daqRun;

class rccIO: public Event_Handler
{
public:
  // constructor
  rccIO (Reactor& r);

  // remember client address
  int   clientAddr (const INET_Addr &addr);
  // set flag of sending message to client
  void  messageToClient (int toc);
  // retrieve flag of sending message to client
  int   messageToClient (void) const;
  // conversion operator for SOCK_Stream
  operator SOCK_Stream& (void);
  // set socket options
  int setSockOption (int level, int option, void* optval, int optlen) const;
  // attach this to manager that is an acceptor
  void manager (rccAcceptor* manager);

  // enable and disable signal associated with this socket stream
  int enable   (int signum);
  int disable  (int signum);

  // client information in the form user_pid@hostname
  char* clientInfo (void) const;
  // client x window display information
  char* clientXDisplay (void) const;
  int   registerClientInfo (rcMsg* cmsg);

  // return mastership information
  int   isMaster           (void) const;
  int   requestMastership  (void);
  int   giveupMastership   (void);

  // setup pointer to data acquisition run object
  void setupRun (daqRun* run);

  // send result back to client
  void sendResult (int command, daqNetData& data, long id);
  // send access violation information back to client
  int  accessViolation (int command, long id);

protected:
  virtual int get_handle   (void) const;
  virtual int handle_close (int, Reactor_Mask);
  virtual int handle_input (int);

  // destructor, all rccIOs must be deleted dynamically
  ~rccIO (void);

private:
  // connection to remote client
  SOCK_Stream clientStream_;
  // flag of mastership, if this connection is a master, it has
  // sole control of everything
  int master_;
  // TCP connection manager
  rccAcceptor* acceptor_;
  // Reactor reference
  Reactor& reactor_;
  // client host address
  char clientHost[MAX_STRING_LEN];

  // client information string, in the form of user_pid@hostname
  char* clientInfo_;

  // client X window display information
  char* clientDisp_;

  // data acquisition  run pointer
  daqRun*      run_;
  // friend clsss
  friend class rccAcceptor;
};
#endif
