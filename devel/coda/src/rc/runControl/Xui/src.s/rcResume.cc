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
//      Implementation of Resume Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcResume.cc,v $
//   Revision 1.1  1999/07/29 14:35:13  rwm
//   Pause and Resume button.
//
//
//
#include <rcNetStatus.h>
#include <rcButtonPanel.h>
#include <rcAudioOutput.h>
#include "rcResume.h"

#define RC_RESUME_NAME "   Resume  "
#define RC_RESUME_MSG  "Resume the run"

rcResume::rcResume (Widget parent, rcButtonPanel* panel, 
		  rcClientHandler& handler)
:rcComdButton (parent, RC_RESUME_NAME, RC_RESUME_MSG, panel, handler, "B")
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcResume Class Object\n");
#endif
  // empty
}

rcResume::~rcResume (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcResume Class Object\n");
#endif
  // empty
}

void
rcResume::doit (void)
{
  rcAudio ("resume a run");

  assert (stWin_);

  // get network handler first
  rcClient& client = netHandler_.clientHandler ();
  daqData data ("RCS", "command", (int)DARESUME);

  if (client.sendCmdCallback (DAGO, data,
			      (rcCallback) &(rcResume::resumeCallback),
			      (void *)this) != CODA_SUCCESS) {

    reportErrorMsg ("Cannot send resume command to the server.");
    rcAudio ("cannot send resume command");
  } else {
    stWin_->start ();
    bpanel_->deactivateTransitionPanel ();
  }
}

void
rcResume::undoit (void)
{
  // empty
}

void
rcResume::resumeCallback (int status, void* arg, daqNetData* data)
{
  rcResume* obj = (rcResume *)arg;

  // stop netStatus updater
  obj->stWin_->stop ();
  obj->bpanel_->activateTransitionPanel ();

  if (status != CODA_SUCCESS) {
    obj->reportErrorMsg ("Resuming a run failed !!!\n");
    rcAudio ("resuming a run failed");
  }
}

