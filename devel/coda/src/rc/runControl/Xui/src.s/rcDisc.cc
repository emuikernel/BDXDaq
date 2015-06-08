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
//      Implementation of rcConnect Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcDisc.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <rcDiscDialog.h>
#include <rcButtonPanel.h>
#include <rcAudioOutput.h>
#include "rcDisc.h"

#define RC_DISC_NAME "Disconnect"
#define RC_DISC_MSG  "Disconnect from the Server"

rcDisc::rcDisc (Widget parent, rcButtonPanel* panel, rcClientHandler& handler)
:rcComdButton (parent, RC_DISC_NAME, RC_DISC_MSG, panel, handler), dialog_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("               Create rcDisc Class Object\n");
#endif
  // empty
}

rcDisc::~rcDisc (void)
{
#ifdef _TRACE_OBJECTS
  printf ("               Delete rcDisc Class Object\n");
#endif
  // empty
  // dialog_ will be derstroyed by Xt destroy mechanism
}

void
rcDisc::doit (void)
{
  rcAudio ("dis connect from the server");

  if (!dialog_) {
    char temp[80];
    sprintf (temp, "Disconnect from the RunControl server,\n");
    strcat  (temp, "Are you sure?");
    dialog_ = new rcDiscDialog (this, "disconnectDialog", "Disconnect Dialog",
				netHandler_);
    dialog_->init ();
    dialog_->alignment (XmALIGNMENT_CENTER);
    dialog_->setMessage (temp);
  }
  dialog_->popup ();
}

void
rcDisc::undoit (void)
{
  // empty
}
