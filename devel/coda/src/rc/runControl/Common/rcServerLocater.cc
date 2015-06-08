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
//       CODA RunControl Service Client Svc locater
//
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcServerLocater.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:31  chen
//   run control source
//
//
#include "rcServerLocater.h"

rcServerLocater::rcServerLocater (int req, char *hostname, 
				  char *exptname, int exptid, int numRetries)
:req_ (req), numRetries_ (numRetries), ufunc_ (0), uarg_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create rcServerLocater Class Object\n");
#endif
  char *tmp = ::getenv ("CODA_RC_PORT");
  int  tmport;
  if (tmp)
    sscanf (tmp, "%d", &tmport);
  else 
    tmport = RC_PORT + exptid;

  brdPort_ = (unsigned short)tmport;
  ::strncpy (exptname_, exptname, MAX_STRING_LEN);
  ::strncpy (serverHost_, hostname, MAX_HOSTNAME_LEN);
}

rcServerLocater::rcServerLocater (int req, char *hostname, char* exptname,
				  unsigned short port, int numRetries)
:req_ (req), brdPort_ (port), numRetries_ (numRetries), ufunc_ (0), uarg_ (0)
 
{
#ifdef _TRACE_OBJECTS
  printf ("Create rcServerLocater Class Object\n");
#endif

  ::strncpy (exptname_, exptname, MAX_STRING_LEN);
  ::strncpy (serverHost_, hostname, MAX_HOSTNAME_LEN);
}

rcServerLocater::~rcServerLocater (void)
{
  // empty
  // ufunc and uarg_ are managed by caller
}

void
rcServerLocater::registerUpdateFunc (rcUpdateFunc func, void* arg)
{
  ufunc_ = func;
  uarg_ = arg;
}

int
rcServerLocater::locatingServer (unsigned short& port)
{
  Reactor r_;
  rcSvcLocaterRep svcl(r_, brdPort_, req_, serverHost_, exptname_);

  if (r_.register_handler (&svcl, Event_Handler::READ_MASK) == -1){
    fprintf (stderr, "Cannot register svc locater \n");
    return -1;
  }
  
  int status = 0;
  int i = 0;
  while(!svcl.foundServer() && i < numRetries_)
  {
    status = svcl.sendRequest ();
    if (status == -1)
      break;
    Time_Value tv (0, 250000); // 0.25 seconds
    r_.handle_events (tv);

    // call the external function 
    if (ufunc_) {
      if((*ufunc_)(uarg_) == -1)
	return -1;
    }
    i++;
  }
  if (i >= numRetries_ || status == -1){
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot find server\n");
#endif
    return -1;
  }
  port = svcl.serverPort_;
  return 0;
} 
