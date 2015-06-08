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
//      Implementation of Display server message option
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcDisplayMsg.cc,v $
//   Revision 1.1  1996/10/14 20:12:52  chen
//   init inport
//
//
//
#include <rcMsgWindow.h>
#include "rcDisplayMsg.h"

rcDisplayMsg::rcDisplayMsg (char* name, int active,
			    char* acc, char* acc_text,
			    int state,
			    rcClientHandler& handler)
:rcMenuTogComd (name, active, acc, acc_text, state, handler),
 msgWin_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("               Create rcDisplayMsg class object\n");
#endif
}

rcDisplayMsg::~rcDisplayMsg (void)
{
#ifdef _TRACE_OBJECTS
  printf ("               Delete rcDisplayMsg class object\n");
#endif
  // mshWin_ is just a pointer
}

void
rcDisplayMsg::doit (void)
{
  assert (msgWin_);
  if (state () > 0)  // after button pressed
    msgWin_->displayMsg ();
  else
    msgWin_->displayNoMsg ();
}

void
rcDisplayMsg::undoit (void)
{
  // empty
}

void
rcDisplayMsg::msgWindow (rcMsgWindow* win)
{
  msgWin_ = win;
}
