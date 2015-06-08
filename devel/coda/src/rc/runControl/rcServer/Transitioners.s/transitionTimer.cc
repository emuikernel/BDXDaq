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
//      State Transition Timer
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: transitionTimer.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#include <transitioner.h>
#include "transitionTimer.h"

transitionTimer::transitionTimer (void)
:reactorTimer (*glbReactor), trans_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create transitionTimer Class Object\n");
#endif
}

transitionTimer::~transitionTimer (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete transitionTimer Class Object\n");
#endif
  // don't free pointer of transitioner
  trans_ = 0;
}

void
transitionTimer::timer_callback (void)
{
  if (trans_) 
    trans_->timerCallback ();
}

void
transitionTimer::transitionerPtr (transitioner* tran)
{
  trans_ = tran;
}
