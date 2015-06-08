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
//      subSysEnder Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysEnder.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include "subSysEnder.h"

subSysEnder::subSysEnder (daqSubSystem* subsys)
:subSysTransitioner (subsys)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create subSysEnder Class Object\n");
#endif
  // empty
}

subSysEnder::~subSysEnder (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete subSysEnder Class Object\n");
#endif
  // empty
}

int
subSysEnder::action (void) const
{
  return CODA_END_ACTION;
}

void
subSysEnder::executeItem (daqComponent* comp)
{
  comp->end ();
}

int
subSysEnder::failureState (void)
{
  return CODA_DOWNLOADED;
}

int
subSysEnder::successState (void)
{
  return CODA_DOWNLOADED;
}
