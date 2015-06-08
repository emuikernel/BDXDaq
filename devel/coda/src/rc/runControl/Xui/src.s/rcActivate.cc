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
//      Implementation of Go Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcActivate.cc,v $
//   Revision 1.2  1998/06/18 12:20:28  heyes
//   new GUI ready I think
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <rcNetStatus.h>
#include <rcButtonPanel.h>
#include <rcAudioOutput.h>
#include "rcActivate.h"

#define RC_ACTIVATE_NAME "    Go    "
#define RC_ACTIVATE_MSG  "Activate a run"

rcActivate::rcActivate (Widget parent, rcButtonPanel* panel, 
			rcClientHandler& handler)
:rcComdButton (parent, RC_ACTIVATE_NAME, RC_ACTIVATE_MSG, panel, handler, "B")
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcActivate Class Object\n");
#endif
  // empty
}

rcActivate::~rcActivate (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcActivate Class Object\n");
#endif
  // empty
}

void
rcActivate::doit (void)
{
  rcAudio ("activate a run");

  assert (stWin_);

  // get network handler first
  rcClient& client = netHandler_.clientHandler ();
  daqData data ("RCS", "command", (int)DAGO);
  if (client.sendCmdCallback (DAGO, data,
			      (rcCallback)&(rcActivate::activateCallback),
			      (void *)this) != CODA_SUCCESS) {
    reportErrorMsg ("Cannot send activate command to the server.");
    rcAudio ("can not send go command");
  }
  else {
    stWin_->start ();
    bpanel_->deactivateTransitionPanel ();
  }
}

void
rcActivate::undoit (void)
{
  // empty
}

void
rcActivate::activateCallback (int status, void* arg, daqNetData* data)
{
  rcActivate* obj = (rcActivate *)arg;

  // stop netStatus updater
  obj->stWin_->stop ();
  obj->bpanel_->activateTransitionPanel ();

  if (status != CODA_SUCCESS) {
    obj->reportErrorMsg ("Activating a run failed !!!\n");
    rcAudio ("activating a run failed");
  }
}

