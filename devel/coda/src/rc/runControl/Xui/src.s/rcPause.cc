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
//      Implementation of Pause Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcPause.cc,v $
//   Revision 1.3  1999/07/28 19:56:54  rwm
//   Added pause button (script) for Hall-B
//
//   Revision 1.2  1998/06/18 12:20:40  heyes
//   new GUI ready I think
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <rcNetStatus.h>
#include <rcButtonPanel.h>
#include <rcAudioOutput.h>
#include "rcPause.h"

#define RC_PAUSE_NAME "   Pause  "
#define RC_PAUSE_MSG  "Pause a run"

rcPause::rcPause (Widget parent, rcButtonPanel* panel, 
		  rcClientHandler& handler)
:rcComdButton (parent, RC_PAUSE_NAME, RC_PAUSE_MSG, panel, handler, "B")
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcPause Class Object\n");
#endif
  // empty
}

rcPause::~rcPause (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcPause Class Object\n");
#endif
  // empty
}

void
rcPause::doit (void)
{
  rcAudio ("pause a run");

  assert (stWin_);

  // get network handler first
  rcClient& client = netHandler_.clientHandler ();
  daqData data ("RCS", "command", (int)DAPAUSE);

  if (client.sendCmdCallback ( DAPAUSE, data, (rcCallback)&(rcPause::pauseCallback), 
			       (void *)this) != CODA_SUCCESS) {
    reportErrorMsg ("Cannot send pause command to the server.");
    rcAudio ("cannot send pause command");
  }
  else {
    stWin_->start ();
    bpanel_->deactivateTransitionPanel ();
  }
}

void
rcPause::undoit (void)
{
  // empty
}

void
rcPause::pauseCallback (int status, void* arg, daqNetData* data)
{
  rcPause* obj = (rcPause *)arg;

  // stop netStatus updater
  obj->stWin_->stop ();
  obj->bpanel_->activateTransitionPanel ();

  if (status != CODA_SUCCESS) {
    obj->reportErrorMsg ("Pausing a run failed !!!\n");
    rcAudio ("pausing a run failed");
  }
}

