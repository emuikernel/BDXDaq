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
//      Implementation of Prestart Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcPrestart.cc,v $
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
#include "rcPrestart.h"

#define RC_PRESTART_NAME " Prestart "
#define RC_PRESTART_MSG  "Prestart a run"

rcPrestart::rcPrestart (Widget parent, rcButtonPanel* panel,
			rcClientHandler& handler)
:rcComdButton (parent, RC_PRESTART_NAME, RC_PRESTART_MSG, panel, handler, "B")
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcPrestart Class Object\n");
#endif
  // empty
}

rcPrestart::~rcPrestart (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcPrestart Class Object\n");
#endif
  // empty
}

void
rcPrestart::doit (void)
{
  rcAudio ("prestart a run");

  assert (stWin_);

  // get network handler first
  rcClient& client = netHandler_.clientHandler ();
  daqData data ("RCS", "command", (int)DAPRESTART);
  if (client.sendCmdCallback (DAPRESTART, data,
			      (rcCallback)&(rcPrestart::prestartCallback),
			      (void *)this) != CODA_SUCCESS){
    reportErrorMsg ("Cannot send prestart command to the server.");
    rcAudio ("can not send prestart");
  }
  else {
    stWin_->start ();
    bpanel_->deactivateTransitionPanel ();
  }
}

void
rcPrestart::undoit (void)
{
  // empty
}

void
rcPrestart::prestartCallback (int status, void* arg, daqNetData* data)
{
  rcPrestart* obj = (rcPrestart *)arg;

  // stop netStatus updater
  obj->stWin_->stop ();
  obj->bpanel_->activateTransitionPanel ();

  if (status != CODA_SUCCESS) {
    obj->reportErrorMsg ("Prestarting a run failed !!!\n");
    rcAudio ("prestarting failed");
  }
}

