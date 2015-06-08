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
//      Implementation of Reset Dialog Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcResetDialog.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <rcReset.h>
#include "rcResetDialog.h"

rcResetDialog::rcResetDialog (rcReset* reset, char* name,
			      char *title, rcClientHandler& handler)
:XcodaWarningDialog (reset->dialogBaseWidget(), name, title), reset_ (reset),
 netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Create rcResetDialog Class Object\n");
#endif
  // empty
}

rcResetDialog::~rcResetDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Delete rcResetDialog Class Object\n");
#endif
  // empty
}

void
rcResetDialog::ok (void)
{
  reset_->reset ();
}
