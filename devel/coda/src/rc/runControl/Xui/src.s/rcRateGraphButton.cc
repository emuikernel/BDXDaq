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
//      RunControl Show Event Rate Graphical Display Option Menu
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRateGraphButton.cc,v $
//   Revision 1.2  1998/04/08 18:31:28  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#include <rcInfoPanel.h>
#include "rcRateGraphButton.h"

rcRateGraphButton::rcRateGraphButton (char* name, int active,
				      char* acc, char* acc_text,
				      rcClientHandler& handler)
:rcMenuComd (name, active, acc, acc_text, handler)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcRateGraphButton Class Object\n");
#endif
}

rcRateGraphButton::~rcRateGraphButton (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcRateGraphButton Class Object\n");
#endif
}

void
rcRateGraphButton::doit (void)
{
  //  rcipanel_->popupRateDisplay ();
}

void
rcRateGraphButton::undoit (void)
{
  // empty
}
