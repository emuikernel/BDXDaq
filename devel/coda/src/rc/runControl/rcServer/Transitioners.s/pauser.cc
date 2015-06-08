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
//      Pauser Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: pauser.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include <daqSubSystem.h>
#include "pauser.h"

pauser::pauser (daqSystem* system)
:transitioner (system)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create pauser Class Object\n");
#endif
  // empty
}

pauser::~pauser (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete pauser Class Object\n");
#endif
  // empty
}

int
pauser::action (void)
{
  return CODA_PAUSE_ACTION;
}

void
pauser::executeItem (daqSubSystem* subsys)
{
  subsys->pause ();
}

int
pauser::failureState (void)
{
  return CODA_ACTIVE;
}

int
pauser::successState (void)
{
  return CODA_PAUSED;
}

int
pauser::transitionState (void)
{
  return CODA_ACTIVE;
}
