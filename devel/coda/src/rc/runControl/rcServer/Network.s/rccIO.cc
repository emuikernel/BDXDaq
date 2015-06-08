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
//      CODA RunControl Server/Client IO Handler
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rccIO.cc,v $
//   Revision 1.5  1997/02/25 14:32:17  chen
//   Remove non block option from socket IO
//
//   Revision 1.4  1997/02/18 17:07:25  chen
//   fix a minor memory leaks
//
//   Revision 1.3  1997/02/03 13:47:33  heyes
//   add ask command
//
//   Revision 1.2  1996/11/04 16:13:50  chen
//   add options for monitoring components
//
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <daqRun.h>
#include <rcSvcProtocol.h>
#include <daqNetData.h>
#include "rccIO.h"

/*#define  _CODA_DEBUG 1*/

rccIO::rccIO (Reactor& r)
:clientStream_ (), master_ (0), acceptor_ (0), reactor_ (r)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rccIO class object\n");
#endif
  clientHost[0] = '\0';
  clientInfo_ = 0;
  clientDisp_ = 0;
}

rccIO::~rccIO (void)
{
  clientStream_.close ();
  // do not delete daqrun pointer run_, since it is statically created
  run_ = 0;
  if (clientInfo_)
    delete []clientInfo_;
  if (clientDisp_)
    delete []clientDisp_;
  clientInfo_ = 0;
  clientDisp_ = 0;
}

rccIO::operator SOCK_Stream & (void)
{
  return clientStream_;
}

int
rccIO::clientAddr (const INET_Addr& addr)
{
  ::strncpy (clientHost, addr.get_host_name (), MAX_STRING_LEN);
  
  return 0;
}

int
rccIO::handle_close (int, Reactor_Mask)
{
  // deallocate all the resources since this one is created dynamically.
  // Reactor will only remove it from its array and calls this handle_close
  // check all callback lists and remove all related callbacks
  run_->removeCallbacks (this);
  // remove this client from client list
  acceptor_->remove (this);
  // update client information
  run_->updateClientInfo (this);
  if (master_) {
    master_ = 0;
    run_->updateMasterInfo ("unknown");
    run_->updateControlDispInfo (0);
  }
  delete this;
  return 0;
}

int
rccIO::get_handle (void) const
{
  return clientStream_.get_handle ();
}

void
rccIO::manager (rccAcceptor* mana)
{
  acceptor_ = mana;
}

int
rccIO::handle_input (int)
{
  int n, status = 0;
  int funcst = 0;
  long type, size;
  daqNetData ndata;
  rcMsg *recver = new rcMsg (DAUNKNOWN, ndata);

  n = clientStream_ >> *recver;
#ifdef _CODA_DEBUG
  printf ("%d bytes is received from client to rcServer\n", n);
  printf ("recver type is %d\n", recver->type ());
#endif
  switch (n){
  case -1:
  case 0:
    printf("Client connection is broken.\n");
    delete recver;
    status = -1;
    break;
  default:
    switch (recver->type ()) {
    case DAGET_VAL:
      status = run_->getValue (this, recver);
      break;
    case DASET_VAL:
      if (master_ || !acceptor_->hasMaster ())
	status = run_->setValue (this, recver);
      else
	status = accessViolation (recver->type (), recver->reqId ());
      break;
    case DAMONITOR_VAL_ON:
      status = run_->monitorOnValue (this, recver);
      break;
    case DAMONITOR_VAL_OFF:
      status = run_->monitorOffValue (this, recver);
      break;
    case DAREG_CLIENT_INFO:
      {
	long cbkid = recver->reqId ();
	registerClientInfo (recver);
	status = run_->updateClientInfo (this, cbkid);
      }
      break;
    case DABECOMEMASTER:
      {
	funcst = requestMastership ();
	daqNetData data (run_->exptname(), "command", funcst);
	sendResult (recver->type(), data, recver->reqId ());
	delete recver;
      }
      break;
    case DACANCELMASTER:
      {
	funcst = giveupMastership ();
	daqNetData data (run_->exptname(), "command", funcst);
	sendResult (recver->type(), data, recver->reqId ());
	delete recver;
      }
      break;
    case DAISMASTER:
      {
	if (isMaster ())
	  funcst = CODA_SUCCESS;
	else
	  funcst = CODA_ERROR;
	daqNetData data (run_->exptname (), "command", funcst);
	sendResult (recver->type (), data, recver->reqId ());
	delete recver;
      }
      break;
    case DAONLINE:
      {
	run_->online ();
	run_->startUpdatingDynamicVars ();
	daqNetData data (run_->exptname (), "command", CODA_SUCCESS);
	sendResult (recver->type (), data, recver->reqId ());
	delete recver;
      }
      break;
    case DAOFFLINE:
      {
	run_->stopUpdatingDynamicVars ();
	run_->offline ();
	daqNetData data (run_->exptname (), "command", CODA_SUCCESS);
	sendResult (recver->type (), data, recver->reqId ());
	delete recver;
      }
      break;
    case DACANCELTRAN:
      {
	run_->cancelTransition ();
	daqNetData data (run_->exptname (), "command", CODA_SUCCESS);
	sendResult (recver->type (), data, recver->reqId ());
	delete recver;
      }
      break;
    case DAAUTOBOOT_INFO:
      run_->autoBootInfo (this, recver);
      break;
    case DAMONITOR_PARM:
      run_->monitorParms (this, recver);
      break;
    case DAASK: 
      {
	char *result;
	int status = run_->ask_component (this, recver, &result);
	if (status != CODA_SUCCESS) {
	  daqNetData data (run_->exptname (), "command", (char *) "ERROR");
	  sendResult (recver->type (), data, recver->reqId ());
	} else {
	  if (result) {
	    daqNetData data (run_->exptname (), "command", result);
	    sendResult (recver->type () , data, recver->reqId ());

	    // result is allocated inside the tcl part, has to free it
	    free (result);
	    
	  } else {
	    daqNetData data (run_->exptname (), "command", "OK");
	    sendResult (recver->type () , data, recver->reqId ());
	  }
	}
      }
      break;
    default:
      // command callback
      if (master_ || !acceptor_->hasMaster ())
	status = run_->processCommand (this, recver->type(), recver); 
      else
	status = accessViolation (recver->type(), recver->reqId () );
      break;
    }
    break;
  }

  return status;
}

int
rccIO::setSockOption (int level, int option, void *optval, int optlen) const
{
  return clientStream_.set_option (level, option, optval, optlen);
}

int
rccIO::enable (int signum)
{
  return clientStream_.enable (signum);
}

int
rccIO::disable (int signum)
{
  return clientStream_.disable (signum);
}

void
rccIO::setupRun (daqRun* run)
{
  run_ = run;
}

void
rccIO::sendResult (int command, daqNetData& data, long id)
{
  rcMsg msg (command, data, id);
  int n = clientStream_ << msg;
  if (n <= 0) {
    fprintf (stderr, "Fatal error: cannot send result back to client\n");
  }
}

int
rccIO::accessViolation (int command, long id)
{
  assert (run_);

  daqNetData res (run_->exptname(), "command", (int)CODA_ACCESS_VIOLATION);
  rcMsg msg (command, res, id);
  int n = clientStream_ << msg;
  if (n <= 0) {
    fprintf (stderr, "Fatal Error: cannot send access violation message to client\n");
    return -1;
  }
  else
    return 0;
}

int
rccIO::registerClientInfo (rcMsg* cmsg)
{
  daqNetData ndata = (daqNetData)(*cmsg);
  char *temp[3];
  int  count = 3;
  char buffer[256];

  if (ndata.getData (temp, count) == CODA_SUCCESS) {
    ::sprintf (buffer,"%s_%s@%s", temp[0], temp[1], clientHost);
    delete []temp[0];
    delete []temp[1];

    // get client X window display information
    clientDisp_ = new char[::strlen (temp[2]) + 1];
    ::strcpy (clientDisp_, temp[2]);
    delete []temp[2];
  }
  else {
    ::sprintf (buffer, "Unknown@%s", clientHost);
    clientDisp_ = new char[::strlen ("unknown:0.0") + 1];
    ::strcpy (clientDisp_, "unknown:0.0");
  }
  
  clientInfo_ = new char[::strlen (buffer) + 1];
  ::strcpy (clientInfo_, buffer);

  // update client display information if this is the master
  // or this is the first one to connect
  if (master_ || acceptor_->numberClients () == 1)
    run_->updateControlDispInfo (clientDisp_);
  
  // free memory of cmsg
  delete cmsg;
  return 0;
}

char*
rccIO::clientInfo (void) const
{
  return clientInfo_;
}

char*
rccIO::clientXDisplay (void) const
{
  return clientDisp_;
}

int
rccIO::isMaster (void) const
{
  return master_;
}

int
rccIO::requestMastership (void)
{
  if (master_)
    return CODA_SUCCESS;
  assert (acceptor_);

  codaSlistIterator ite (acceptor_->clientList ());
  rccIO *io = 0;

  for (ite.init (); !ite; ++ite) {
    io = (rccIO *)ite ();
    if (io->isMaster())
      return CODA_ERROR;
  }
  master_ = 1;
  
  // update run master variable
  assert (run_);
  assert (clientInfo_);
  run_->updateMasterInfo (clientInfo_);
  run_->updateControlDispInfo (clientDisp_);
  return CODA_SUCCESS;
}

int
rccIO::giveupMastership (void)
{
  if (master_) {
    master_ = 0;

    assert (run_);
    run_->updateMasterInfo ("unknown");
    run_->updateControlDispInfo (0);

    return CODA_SUCCESS;
  }
  return CODA_ERROR;
}
