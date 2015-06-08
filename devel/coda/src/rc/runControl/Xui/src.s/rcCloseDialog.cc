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
//      RunControl Close Menu Button Dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcCloseDialog.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#include <rcClose.h>
#include "rcCloseDialog.h"

rcCloseDialog::rcCloseDialog (rcClose* close, char* name,
			      char *title, rcClientHandler& handler)
:XcodaWarningDialog (close->dialogBaseWidget(), name, title), close_ (close),
 netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Create rcCloseDialog Class Object\n");
#endif
  // empty
}

rcCloseDialog::~rcCloseDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Delete rcCloseDialog Class Object\n");
#endif
  // empty
}

void
rcCloseDialog::ok (void)
{
  close_->close ();
}
