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
//      Implementation of Auto Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcAuto.cc,v $
//   Revision 1.3  1998/06/18 12:20:30  heyes
//   new GUI ready I think
//
//   Revision 1.2  1997/10/15 16:08:23  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#include <rcNetStatus.h>
#include <rcButtonPanel.h>
#include "rcAuto.h"

#define RC_AUTO_NAME "Start Run"
#define RC_AUTO_MSG  "Automatic start a run"

rcAuto::rcAuto (Widget parent, rcButtonPanel* panel,
		rcClientHandler& handler)
:rcComdButton (parent, RC_AUTO_NAME, RC_AUTO_MSG, panel, handler, "B")
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcAuto Class Object\n");
#endif
  // empty
}

rcAuto::~rcAuto (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcAuto Class Object\n");
#endif
  // empty
}

void
rcAuto::doit (void)
{
  assert (stWin_);

  // get network handler first
  rcClient& client = netHandler_.clientHandler ();
  int states[2];
  if (netHandler_.status () == DA_CONFIGURED)
    states[0] = DA_DORMANT;
  else
    states[0] = netHandler_.status ();
  
  states[1] = DA_ACTIVE;
  daqData data ("RCS", "command", states, 2);
  if (client.sendCmdCallback (DACHANGE_STATE, data,
			      (rcCallback)&(rcAuto::autoStartCallback),
			      (void *)this) != CODA_SUCCESS)
    reportErrorMsg ("Cannot send change state command to the server.");
  else {
    stWin_->start ();
    bpanel_->deactivateTransitionPanel ();
  }
}

void
rcAuto::undoit (void)
{
  // empty
}

void
rcAuto::autoStartCallback (int status, void* arg, daqNetData* data)
{
  rcAuto* obj = (rcAuto *)arg;

  // stop netStatus updater
  obj->stWin_->stop ();
  obj->bpanel_->activateTransitionPanel ();

  if (status != CODA_SUCCESS)
    obj->reportErrorMsg ("Automatic activating a run failed !!!\n");
}

