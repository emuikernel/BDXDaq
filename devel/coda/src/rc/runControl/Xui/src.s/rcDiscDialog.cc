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
//      Implmentation of runControl disconnect dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcDiscDialog.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#include <rcDisc.h>
#include "rcDiscDialog.h"

rcDiscDialog::rcDiscDialog (rcDisc* disc, char* name,
			    char *title, rcClientHandler& handler)
:XcodaWarningDialog (disc->dialogBaseWidget(), name, title), disc_ (disc),
 netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Create rcDiscDialog Class Object\n");
#endif
  // empty
}

rcDiscDialog::~rcDiscDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Delete rcDiscDialog Class Object\n");
#endif
  // empty
}

void
rcDiscDialog::ok (void)
{
  netHandler_.disconnect ();
}
