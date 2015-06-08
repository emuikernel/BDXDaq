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
//      Activater Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: activater.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include <daqSubSystem.h>
#include <daqRun.h>
#include "activater.h"

activater::activater (daqSystem* system)
:transitioner (system)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create activater Class Object\n");
#endif
  // empty
}

activater::~activater (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete activater Class Object\n");
#endif
  // empty
}

int
activater::action (void)
{
  return CODA_GO_ACTION;
}

void
activater::executeItem (daqSubSystem* subsys)
{
  subsys->go ();
}

void
activater::extraRunParmSetup (void)
{
  // update starting time of this run
  if (!system_->pauseActiveLoop ()) {
    system_->run()->setStartTime ();
    system_->run()->eraseEndTime ();
  }
}

int
activater::failureState (void)
{
  return CODA_PAUSED;
}

int
activater::successState (void)
{
  return CODA_ACTIVE;
}

int
activater::transitionState (void)
{
  return CODA_PAUSED;
}

void
activater::setupSuccess (void)
{
  transitioner::setupSuccess ();
  system_->run()->startMonitoringComp ();
}


