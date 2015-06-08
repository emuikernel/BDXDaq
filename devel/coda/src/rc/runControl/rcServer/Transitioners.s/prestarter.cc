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
//      Prestarter Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: prestarter.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include <daqSubSystem.h>
#include <daqRun.h>
#include "prestarter.h"

prestarter::prestarter (daqSystem* system)
:transitioner (system)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create prestarter Class Object\n");
#endif
  // empty
}

prestarter::~prestarter (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete prestarter Class Object\n");
#endif
  // empty
}

int
prestarter::action (void)
{
  return CODA_PRESTART_ACTION;
}

void
prestarter::executeItem (daqSubSystem* subsys)
{
  subsys->prestart ();
}

void
prestarter::extraRunParmSetup (void)
{
  // increment data acquisition run number by one
  if (system_->run()->autoIncrement ())
    system_->run()->increaseRunNumber ();
}

int
prestarter::failureState (void)
{
  return CODA_DOWNLOADED;
}

int
prestarter::successState (void)
{
  return CODA_PAUSED;
}

int
prestarter::transitionState (void)
{
  return CODA_DOWNLOADED;
}

void
prestarter::setupSuccess (void)
{
  transitioner::setupSuccess ();
  system_->run()->startUpdatingDynamicVars ();
}
