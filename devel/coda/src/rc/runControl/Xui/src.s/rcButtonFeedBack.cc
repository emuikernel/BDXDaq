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
//      Implementation of Button Feed back option button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcButtonFeedBack.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#include <rcHelpMsgWin.h>
#include "rcButtonFeedBack.h"

rcButtonFeedBack::rcButtonFeedBack (char* name, int active,
				    char* acc, char* acc_text,
				    int state,
				    rcClientHandler& handler)
:rcMenuTogComd (name, active, acc, acc_text, state, handler),
 msgWin_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("               Create rcButtonFeedBack class object\n");
#endif
}

rcButtonFeedBack::~rcButtonFeedBack (void)
{
#ifdef _TRACE_OBJECTS
  printf ("               Delete rcButtonFeedBack class object\n");
#endif
  // mshWin_ is just a pointer
}

void
rcButtonFeedBack::doit (void)
{
  assert (msgWin_);
  if (state () > 0)  // after button pressed
    msgWin_->displayMsg ();
  else
    msgWin_->displayNoMsg ();
}

void
rcButtonFeedBack::undoit (void)
{
  // empty
}

void
rcButtonFeedBack::helpMsgWindow (rcHelpMsgWin* win)
{
  msgWin_ = win;
}
