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
//      Resetter Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: resetter.cc,v $
//   Revision 1.2  1996/12/04 18:32:53  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include <daqRun.h>
#include <daqSubSystem.h>
#include "resetter.h"

resetter::resetter (daqSystem* system)
:transitioner (system)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create resetter Class Object\n");
#endif
  // empty
}

resetter::~resetter (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete resetter Class Object\n");
#endif
  // empty
}

int
resetter::action (void)
{
  return CODA_RESET_ACTION;
}

void
resetter::executeItem (daqSubSystem* subsys)
{
  subsys->reset ();
}

void
resetter::extraRunParmSetup (void)
{
  system_->run()->removeDynamicVars ();
  system_->run()->eraseStartTime ();
  system_->run()->eraseEndTime ();
  system_->run()->endMonitoringComp ();
}

int
resetter::failureState (void)
{
  return CODA_BOOTED;
}

int
resetter::successState (void)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  return CODA_CONFIGURED;
#else
  return CODA_DORMANT;
#endif
}

int
resetter::transitionState (void)
{
  return CODA_RESETTING;
}

int
resetter::transitionFinished (int fstate, int successState)
{
  // reset stage is special
  return (fstate <= successState);
}
