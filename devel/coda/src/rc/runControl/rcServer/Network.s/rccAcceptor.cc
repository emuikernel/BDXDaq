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
//   $Log: rccAcceptor.cc,v $
//   Revision 1.2  1997/02/25 14:32:16  chen
//   Remove non block option from socket IO
//
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#include <daqRun.h>
#include "rccAcceptor.h"
#include "rccIO.h"

rccAcceptor::rccAcceptor (Reactor& r)
:reactor_ (r), rcClientList_ (), run_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rccAcceptor Class Object\n");
#endif
}

rccAcceptor::~rccAcceptor (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete rccAcceptor Class Object\n");
#endif
  // delete all clients
  deleteAllClients ();
  // close socket
  handle_close (-1, Event_Handler::READ_MASK);
  run_ = 0;
}

int
rccAcceptor::handle_close (int, Reactor_Mask)
{
  return listener_.close ();
}

int
rccAcceptor::get_handle (void) const
{
  return listener_.get_handle ();
}

int
rccAcceptor::open (void)
{
  INET_Addr addr ((unsigned short)0);

  if (listener_.open (addr) == -1)
    return -1;
  else if (listener_.enable (O_NONBLOCK) == -1)
    return -1;
  else
    return 0;
}

unsigned short
rccAcceptor::port_number (void)
{
  INET_Addr addr;
  listener_.get_local_addr (addr);
  return ntohs (addr.get_port_number());
}

int
rccAcceptor::numberClients (void)
{
  return rcClientList_.count ();
}

int
rccAcceptor::remove (rccIO* client)
{
  return rcClientList_.remove ((void *)client);
}

void
rccAcceptor::setupRun (daqRun* run)
{
  run_ = run;
  run_->networkManager (this);
}

void
rccAcceptor::sendToAllClients (int command, daqNetData& data, long id)
{
  codaSlistIterator ite (rcClientList_);
  rccIO* cio = 0;

  for (ite.init (); !ite; ++ite) {
    cio = (rccIO *) ite ();
    cio->sendResult (command, data, id);
  }
}

void
rccAcceptor::deleteAllClients (void)
{
  codaSlistIterator ite (rcClientList_);
  rccIO* cio = 0;

  for (ite.init (); !ite; ++ite) {
    cio = (rccIO *) ite ();
    delete cio;
  }
}

int
rccAcceptor::handle_input (int)
{
  INET_Addr rem_addr;
  rccIO     *client = new rccIO (reactor_);

  // accept the connection from a client
  if (listener_.accept (*client, &rem_addr) != -1){
#ifdef _CODA_DEBUG
    printf ("Received client connection from host %s at port %d\n",
	    rem_addr.get_host_name (), client->get_handle ());
#endif
    // remeber client address
    client->clientAddr (rem_addr);
    // set socket options to KEEP_ALIVE
    int alive = 1;
    client->setSockOption (SOL_SOCKET, SO_KEEPALIVE,
			   (void *)&alive, sizeof (alive));
    if (reactor_.register_handler (client,
				   Event_Handler::READ_MASK) == -1) {
#ifdef _CODA_DEBUG
      printf ("Unable to register rccIO handler\n");
#endif
      return -1;
    }

    // first turn asynchronous IO off (problem between different machines)
    client->disable (ACE_NONBLOCK);

    // register this acceptor to rccIO
    client->manager (this);
    // add this client TCP connection to the list
    rcClientList_.add ((void *)client);
    // pass data acquisition run pointer
    client->setupRun (run_);
    // send dynamic vars information to the client
    run_->sendDynamicVarsInfo (client);
    // send ana log vars information to the client
    run_->sendAnaLogVarsInfo (client);
  }
  else {
#ifdef _CODA_DEBUG
    printf ("Accept failed\n");
#endif
    return -1;
  }
  return 0;
}

int
rccAcceptor::hasMaster (void)
{
  codaSlistIterator ite (rcClientList_);
  rccIO* io = 0;

  for (ite.init(); !ite; ++ite) {
    io = (rccIO *) ite ();
    if (io->isMaster ())
      return 1;
  }
  return 0;
}

codaSlist&
rccAcceptor::clientList (void)
{
  return rcClientList_;
}
