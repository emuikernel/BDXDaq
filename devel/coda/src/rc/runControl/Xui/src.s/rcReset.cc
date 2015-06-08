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
//      Implementation of Reset Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcReset.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <rcNetStatus.h>
#include <rcResetDialog.h>
#include <rcButtonPanel.h>
#include <rcAudioOutput.h>
#include "rcReset.h"

#define RC_RESET_NAME "   Reset  "
#define RC_RESET_MSG  "Reset all components"

rcReset::rcReset (Widget parent, rcButtonPanel* panel, 
		  rcClientHandler& handler)
:rcComdButton (parent, RC_RESET_NAME, RC_RESET_MSG, panel, handler),
 dialog_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcReset Class Object\n");
#endif
  // empty
}

rcReset::~rcReset (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcReset Class Object\n");
#endif
  // empty
  // dialog_ will be destroyed by Xt destroy mechanism
}

void
rcReset::reset (void)
{
  assert (stWin_);

  // get network handler first
  rcClient& client = netHandler_.clientHandler ();
  daqData data ("RCS", "command", (int)DARESET);
  if (client.sendCmdCallback (DARESET, data,
			      (rcCallback)&(rcReset::resetCallback),
			      (void *)this) != CODA_SUCCESS)
  {
    reportErrorMsg ("Cannot send end command to the server.");
  }
  else
  { 
    stWin_->start ();
  }
}

void
rcReset::doit (void)
{
  rcAudio ("Reset everything, are you sure ?");
  if (!dialog_) {
    dialog_ = new rcResetDialog (this, "resetDialog", "Reset Dialog",
				 netHandler_);
    dialog_->init ();
    dialog_->alignment (XmALIGNMENT_CENTER);
    // set message
    char temp[256];
    sprintf (temp, "This resets RunControl to the booted state\n   and terminates all remote processes.\n \n    Only do this as a last resort!!!!");
    dialog_->setMessage (temp);
  }
  if (dialog_->isMapped ())
    dialog_->popdown ();
  dialog_->popup ();
}

void
rcReset::undoit (void)
{
  // empty
}

void
rcReset::resetCallback (int status, void* arg, daqNetData* data)
{
  rcReset* obj = (rcReset *)arg;

  // stop netStatus updater
  obj->stWin_->stop ();

  if (status != CODA_SUCCESS)
    obj->reportErrorMsg ("Resetting a run failed !!!\n");
}

