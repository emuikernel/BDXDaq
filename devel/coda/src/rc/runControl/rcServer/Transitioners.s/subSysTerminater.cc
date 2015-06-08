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
//      subSysTerminater Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysTerminater.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#include "subSysTerminater.h"

subSysTerminater::subSysTerminater (daqSubSystem* subsys)
:subSysTransitioner (subsys)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create subSysTerminater Class Object\n");
#endif
  // empty
}

subSysTerminater::~subSysTerminater (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete subSysTerminater Class Object\n");
#endif
  // empty
}

int
subSysTerminater::action (void) const
{
  return CODA_TERMINATE_ACTION;
}

void
subSysTerminater::executeItem (daqComponent* comp)
{
  comp->terminate ();
}

int
subSysTerminater::failureState (void)
{
  return CODA_BOOTED;
}

int
subSysTerminater::successState (void)
{
  return CODA_DORMANT;
}

int
subSysTerminater::compEnabled (daqComponent* comp)
{
  // we should terminate all components
  return 1;
}
