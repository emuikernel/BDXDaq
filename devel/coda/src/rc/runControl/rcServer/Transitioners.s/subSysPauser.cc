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
//      subSysPauser Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysPauser.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#include "subSysPauser.h"

subSysPauser::subSysPauser (daqSubSystem* subsys)
:subSysTransitioner (subsys)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create subSysPauser Class Object\n");
#endif
  // empty
}

subSysPauser::~subSysPauser (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete subSysPauser Class Object\n");
#endif
  // empty
}

int
subSysPauser::action (void) const
{
  return CODA_PAUSE_ACTION;
}

void
subSysPauser::executeItem (daqComponent* comp)
{
  comp->pause ();
}

int
subSysPauser::failureState (void)
{
  return CODA_ACTIVE;
}

int
subSysPauser::successState (void)
{
  return CODA_PAUSED;
}
