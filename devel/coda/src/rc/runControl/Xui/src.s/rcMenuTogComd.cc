
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
//      Implementation of RunControl Menu Toggle Command Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMenuTogComd.cc,v $
//   Revision 1.2  1997/10/23 13:00:55  heyes
//   fix Alt/W
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaApp.h>
#include <XcodaErrorDialog.h>

#include "codaComdXInterface.h"

#include "rcNetStatus.h"
#include "rcInfoPanel.h"
#include "rcMenuTogComd.h"

// initialize error dialog
XcodaErrorDialog* rcMenuTogComd::errDialog_ = 0;

rcMenuTogComd::rcMenuTogComd (char* name, int active,
			      char* acc, char* acc_text, int state,
			      rcClientHandler& handler)
:codaTbtnComd (name, active, acc, acc_text, state), netSt_ (0), rcipanel_ (0),
 netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcMenuTogComd Class Object\n");
#endif
}

rcMenuTogComd::~rcMenuTogComd (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcMenuTogComd Class Object\n");
#endif
  // netSt_ and rcipanel_ will be deleted by Xt mechanism
  // and the same is true for dialog_
}

void
rcMenuTogComd::netStatusPanel (rcNetStatus* st)
{
  netSt_ = st;
}

void
rcMenuTogComd::infoPanel (rcInfoPanel* ipanel)
{
  rcipanel_ = ipanel;
}

const Widget
rcMenuTogComd::dialogBaseWidget (void)
{
  assert (rcipanel_);
  return rcipanel_->baseWidget ();
}

void
rcMenuTogComd::reportErrorMsg (char* msg)
{
  if (rcMenuTogComd::errDialog_ == 0) {
    rcMenuTogComd::errDialog_ = new XcodaErrorDialog (dialogBaseWidget(),
						      "comdError",
						      "Error Dialog");
    rcMenuTogComd::errDialog_->init ();
  }
  rcMenuTogComd::errDialog_->setMessage (msg);
  rcMenuTogComd::errDialog_->popup ();
}

