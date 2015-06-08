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
//      RunControl Exit Menu Button Dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcExitDialog.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#include <rcExit.h>
#include "rcExitDialog.h"

rcExitDialog::rcExitDialog (rcExit* exit, char* name,
			    char *title, rcClientHandler& handler)
:XcodaWarningDialog (exit->dialogBaseWidget(), name, title), exit_ (exit),
 netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Create rcExitDialog Class Object\n");
#endif
  // empty
}

rcExitDialog::~rcExitDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Delete rcExitDialog Class Object\n");
#endif
  // empty
}

void
rcExitDialog::ok (void)
{
  exit_->exitRunControl ();
}
