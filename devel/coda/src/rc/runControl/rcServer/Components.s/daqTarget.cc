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
//      Data Acquisition Target Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqTarget.cc,v $
//   Revision 1.4  1999/07/28 19:17:40  rwm
//   Bunch of hacks to support Pause script button for Hall B.
//
//   Revision 1.3  1998/11/05 20:11:47  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.2  1996/10/28 14:23:03  heyes
//   communication with ROCs changed
//
//   Revision 1.1.1.1  1996/10/11 13:39:17  chen
//   run control source
//
//
#include <stdio.h>
#include <time.h>
#include "daqTarget.h"

daqTarget::daqTarget (char *title)
:target (title), state_ (CODA_DORMANT),
 prevState_ (CODA_DORMANT), status_ (CODA_SUCCESS), 
 overrideState_ (CODA_DISCONNECTED)
{
#ifdef _TRACE_OBEJCTS
  printf ("    Create daqTarget class object\n");
#endif
  // empty
  prevtime_ = time(0);
}

daqTarget::~daqTarget (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete daqTarget class object\n");
#endif
  // empty
}

void
daqTarget::setState (int newst)
{
  if (newst != state_){
    // Warning this equals is overridden.
    prevState_ = state_;
    state_ = newst;
  }

  prevtime_ = time(0);
  //printf("set state of %s to %d\n",title_, newst);
  overrideState_ = CODA_DISCONNECTED;
}

int
daqTarget::state (void)
{
    if (time(0)	> prevtime_ + 30) {
	   printf("timeout checking state for %s\n",title_);
	  state_ = CODA_DISCONNECTED;
	}
  return state_;
}

int
daqTarget::state2 (void)
{
    if (time(0)	> prevtime_ + 30) {
	  state_ = CODA_DISCONNECTED;
	}
  return state_;
}

int
daqTarget::previousState (void)
{
  return prevState_;
}

void
daqTarget::setOverrideState (int wanted)
{
  overrideState_ = wanted;
}

int
daqTarget::overrideState (void) const
{
  return overrideState_;
}

int
daqTarget::testAction (int action)
{
  return (action >= CODA_BOOT_ACTION && action <= CODA_TERMINATE_ACTION);
}

int
daqTarget::testState (int newst)
{
  return (newst == state());
}

