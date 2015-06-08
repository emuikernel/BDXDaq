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
//      Booter Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: booter.cc,v $
//   Revision 1.7  1999/02/25 14:38:17  rwm
//   Limits defined in daqRunLimits.h
//
//   Revision 1.6  1996/12/04 18:32:52  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.5  1996/10/31 15:56:14  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.4  1996/10/22 17:17:20  chen
//   fix bugs of boot stage different state among components
//
//   Revision 1.3  1996/10/14 20:02:53  heyes
//   changed message system
//
//   Revision 1.2  1996/10/14 13:29:40  heyes
//   fix timeouts
//
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#include <daqRun.h>
#include <daqSubSystem.h>
#include <daqState.h>
#include <daqActions.h>
#include <rcMsgReporter.h>
#include "booter.h"

booter::booter (daqSystem* system)
:transitioner (system), timeoutCache_(0)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create booter Class Object\n");
#endif
  // empty
}

booter::~booter (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete booter Class Object\n");
#endif
  // empty
}

int
booter::action (void)
{
  return CODA_BOOT_ACTION;
}

int
booter::transitionFinished (int fstate, int successState)
{
  // boot stage is special
  return (fstate < CODA_VERIFYING && fstate >= successState);
}


int
booter::transitionTimeout (void)
{
  if (!timeoutCache_) {
    char *cs[MAX_NUM_COMPONENTS];

    int counter = system_->allEnabledComponents(cs,MAX_NUM_COMPONENTS);
    for(int i = 0; i < counter; i++)
      delete []cs[i];

    timeoutCache_ = (transitioner::transitionTimeout ()) * counter;
  }
  return timeoutCache_;
}


void
booter::executeItem (daqSubSystem* subsys)
{
  subsys->boot ();
}

int
booter::failureState (void)
{
  return CODA_CONFIGURED;
}

int
booter::successState (void)
{
  return CODA_BOOTED;
}

int
booter::transitionState (void)
{
  return CODA_DORMANT;
}

void
booter::setupSuccess (void)
{
  transitioner::setupSuccess ();
  system_->run ()->createDynamicVars ();
}

int
booter::subSystemState (daqSubSystem* subsys)
{
  return subsys->bootState ();
}
  
