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
//      Terminater Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: terminater.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include <daqRun.h>
#include <daqSubSystem.h>
#include "terminater.h"

terminater::terminater (daqSystem* system)
:transitioner (system)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create terminater Class Object\n");
#endif
  // empty
}

terminater::~terminater (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete terminater Class Object\n");
#endif
  // empty
}

int
terminater::action (void)
{
  return CODA_TERMINATE_ACTION;
}

void
terminater::executeItem (daqSubSystem* subsys)
{
  subsys->terminate ();
}

void
terminater::extraRunParmSetup (void)
{
  system_->run ()->removeDynamicVars ();
}

int
terminater::failureState (void)
{
  return CODA_BOOTED;
}

int
terminater::successState (void)
{
  return CODA_DORMANT;
}

int
terminater::transitionState (void)
{
  return CODA_TERMINATING;
}
