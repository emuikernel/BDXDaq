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
//      subSysActivater Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysActivater.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include "subSysActivater.h"

subSysActivater::subSysActivater (daqSubSystem* subsys)
:subSysTransitioner (subsys)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create subSysActivater Class Object\n");
#endif
  // empty
}

subSysActivater::~subSysActivater (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete subSysActivater Class Object\n");
#endif
  // empty
}

int
subSysActivater::action (void) const
{
  return CODA_GO_ACTION;
}

void
subSysActivater::executeItem (daqComponent* comp)
{
  comp->go ();
}

int
subSysActivater::failureState (void)
{
  return CODA_PAUSED;
}

int
subSysActivater::successState (void)
{
  return CODA_ACTIVE;
}
