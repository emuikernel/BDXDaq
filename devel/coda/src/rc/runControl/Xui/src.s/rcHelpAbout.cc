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
//      Implementation of rcHelpAbout Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcHelpAbout.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaApp.h>
#include <rcHelpAboutDialog.h>

#include "rcNetStatus.h"
#include "rcInfoPanel.h"


#include "rcHelpAbout.h"

rcHelpAbout::rcHelpAbout (char* name, int active,
			  char* acc, char* acc_text, 
			  rcClientHandler& handler)
:rcMenuComd (name, active, acc, acc_text, handler), dialog_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcHelpAbout Class Object\n");
#endif
}

rcHelpAbout::~rcHelpAbout (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcHelpAbout Class Object\n");
#endif
}

void
rcHelpAbout::doit (void)
{
  if (!dialog_) {
    dialog_ = new rcHelpAboutDialog (this, "helpAboutDialog", "Version");
    dialog_->init ();
  }
  dialog_->popup ();
}

void
rcHelpAbout::undoit (void)
{
  // empty
}

