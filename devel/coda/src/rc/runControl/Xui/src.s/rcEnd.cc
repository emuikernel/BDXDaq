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
//      Implementation of End Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcEnd.cc,v $
//   Revision 1.3  1998/06/18 12:20:37  heyes
//   new GUI ready I think
//
//   Revision 1.2  1997/10/15 16:08:28  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <rcNetStatus.h>
#include <rcButtonPanel.h>
#include "rcAudioOutput.h"
#include "rcEnd.h"

#define RC_END_NAME "End Run"
#define RC_END_MSG  "End a run"

rcEnd::rcEnd (Widget parent, rcButtonPanel* panel, rcClientHandler& handler)
:rcComdButton (parent, RC_END_NAME, RC_END_MSG, panel, handler, "B")
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcEnd Class Object\n");
#endif
  // empty
}

rcEnd::~rcEnd (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcEnd Class Object\n");
#endif
  // empty
}

void
rcEnd::doit (void)
{
  rcAudio ("end this run");
  assert (stWin_);

  // get network handler first
  rcClient& client = netHandler_.clientHandler ();
  daqData data ("RCS", "command", (int)DAEND);
  if (client.sendCmdCallback (DAEND, data,
			      (rcCallback)&(rcEnd::endCallback),
			      (void *)this) != CODA_SUCCESS) {
    reportErrorMsg ("Cannot send end command to the server.");
    rcAudio ("can not send end command");
  }
  else {
    stWin_->start ();
    bpanel_->deactivateTransitionPanel ();
  }
}

void
rcEnd::undoit (void)
{
  // empty
}

void
rcEnd::endCallback (int status, void* arg, daqNetData* data)
{
  rcEnd* obj = (rcEnd *)arg;

  // stop netStatus updater
  obj->stWin_->stop ();
  obj->bpanel_->activateTransitionPanel ();

  if (status != CODA_SUCCESS) {
    obj->reportErrorMsg ("Ending a run failed !!!\n");
    rcAudio ("ending this run failed");
  }
}

