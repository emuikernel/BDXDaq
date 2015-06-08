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
//      Graham Heyes
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcBackButton.cc,v $
//   Revision 1.1  1997/07/30 14:33:35  heyes
//   add more xpm support
//
//
//
#include <rcNetStatus.h>
#include <rcButtonPanel.h>
#include <rcComdOption.h>
#include "rcBackButton.h"
#include "pixmaps/back.xpm"

rcBackButton::rcBackButton (Widget parent, rcButtonPanel* panel, 
		      rcClientHandler& handler)
  :rcXpmComdButton (parent, "Back", back_xpm, "Go back one html page", panel, handler)
{
  appContext_ = XtWidgetToApplicationContext (parent);
}

rcBackButton::~rcBackButton (void)
{
#ifdef _TRACE_OBJECTS
  printf ("               Delete rcBackButton Class Object\n");
#endif
  // empty
}

extern int HTMLbackCallback();

void
rcBackButton::doit (void)
{
  HTMLbackCallback();
}

void
rcBackButton::undoit (void)
{
  // empty
}
