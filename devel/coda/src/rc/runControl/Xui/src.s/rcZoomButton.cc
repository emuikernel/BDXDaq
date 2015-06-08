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
//      RunControl Zoom Option Menu
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcZoomButton.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#include <rcInfoPanel.h>
#include "rcZoomButton.h"

rcZoomButton::rcZoomButton (char* name, int active,
			    char* acc, char* acc_text,
			    rcClientHandler& handler)
:rcMenuComd (name, active, acc, acc_text, handler)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcZoomButton Class Object\n");
#endif
}

rcZoomButton::~rcZoomButton (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcZoomButton Class Object\n");
#endif
}

void
rcZoomButton::doit (void)
{
  rcipanel_->zoomOnEventInfo ();
}

void
rcZoomButton::undoit (void)
{
  // empty
}
