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
//      Ender Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: ender.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#include <daqRun.h>
#include <daqSubSystem.h>
#include "ender.h"

ender::ender (daqSystem* system)
:transitioner (system)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create ender Class Object\n");
#endif
  // empty
}

ender::~ender (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete ender Class Object\n");
#endif
  // empty
}

int
ender::action (void)
{
  return CODA_END_ACTION;
}

void
ender::executeItem (daqSubSystem* subsys)
{
  subsys->end ();
}

int
ender::failureState (void)
{
  return CODA_DOWNLOADED;
}

int
ender::successState (void)
{
  return CODA_DOWNLOADED;
}

int
ender::transitionState (void)
{
  return CODA_PAUSED;
}

void
ender::setupSuccess (void)
{
  transitioner::setupSuccess ();
  // go out to get last dynamic variable values before I stop updating
  system_->run()->updateDynamicVars ();
  system_->run()->stopUpdatingDynamicVars ();
  system_->run()->setEndTime ();
  system_->run()->endMonitoringComp ();
}

void
ender::setupFailure (void)
{
  transitioner::setupFailure ();
  system_->run()->stopUpdatingDynamicVars ();
  system_->run()->setEndTime ();
  system_->run()->endMonitoringComp ();
}

