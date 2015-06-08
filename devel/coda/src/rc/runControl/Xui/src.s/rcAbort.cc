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
//      Implementation of Abort Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcAbort.cc,v $
//   Revision 1.2  1998/06/18 12:20:28  heyes
//   new GUI ready I think
//
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#include <rcNetStatus.h>
#include <rcButtonPanel.h>
#include <rcAudioOutput.h>
#include "rcAbort.h"

#define RC_ABORT_NAME "   Abort  "
#define RC_ABORT_MSG  "Abort a run"

rcAbort::rcAbort (Widget parent, rcButtonPanel* panel, 
		  rcClientHandler& handler)
:rcComdButton (parent, RC_ABORT_NAME, RC_ABORT_MSG, panel, handler, "B")
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcAbort Class Object\n");
#endif
  // empty
}

rcAbort::~rcAbort (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcAbort Class Object\n");
#endif
  // empty
}

void
rcAbort::doit (void)
{
  rcAudio ("abort this run");

  assert (stWin_);

  // get network handler first
  rcClient& client = netHandler_.clientHandler ();
  daqData data ("RCS", "command", (int)DAABORT);
  if (client.sendCmdCallback (DAABORT, data,
			      (rcCallback)&(rcAbort::abortCallback),
			      (void *)this) != CODA_SUCCESS) {
    reportErrorMsg ("Cannot send abort command to the server.");
    rcAudio ("can not send abort command");
  }
  else 
    stWin_->start ();
}

void
rcAbort::undoit (void)
{
  // empty
}

void
rcAbort::abortCallback (int status, void* arg, daqNetData* data)
{
  rcAbort* obj = (rcAbort *)arg;

  // stop netStatus updater
  obj->stWin_->stop ();

  if (status != CODA_SUCCESS) {
    obj->reportErrorMsg ("Aborting a run failed !!!\n");
    rcAudio ("aborting failed");
  }
}

