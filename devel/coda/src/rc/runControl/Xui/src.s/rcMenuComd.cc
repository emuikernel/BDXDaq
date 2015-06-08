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
//      Implementation of RunControl MenuCommand Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMenuComd.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaApp.h>
#include <XcodaErrorDialog.h>
#include <codaComdXInterface.h>

#include "rcNetStatus.h"
#include "rcInfoPanel.h"
#include "rcMenuComd.h"

// initialize error dialog
XcodaErrorDialog* rcMenuComd::errDialog_ = 0;

rcMenuComd::rcMenuComd (char* name, int active,
			char* acc, char* acc_text,
			rcClientHandler& handler)
:codaPbtnComd (name, active, acc, acc_text), netSt_ (0), rcipanel_ (0),
 netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcMenuComd Class Object\n");
#endif
}

rcMenuComd::~rcMenuComd (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcMenuComd Class Object\n");
#endif
  // netSt_ and rcipanel_ will be deleted by Xt mechanism
  // and the same is true for dialog_
}

void
rcMenuComd::netStatusPanel (rcNetStatus* st)
{
  netSt_ = st;
}

void
rcMenuComd::infoPanel (rcInfoPanel* ipanel)
{
  rcipanel_ = ipanel;
}

const Widget
rcMenuComd::dialogBaseWidget (void)
{
  assert (rcipanel_);
  return rcipanel_->baseWidget ();
}

void
rcMenuComd::reportErrorMsg (char* msg)
{
  if (rcMenuComd::errDialog_ == 0) {
    rcMenuComd::errDialog_ = new XcodaErrorDialog (dialogBaseWidget(),
						   "comdError",
						   "Error Dialog");
    rcMenuComd::errDialog_->init ();
  }
  rcMenuComd::errDialog_->setMessage (msg);
  rcMenuComd::errDialog_->popup ();
}

