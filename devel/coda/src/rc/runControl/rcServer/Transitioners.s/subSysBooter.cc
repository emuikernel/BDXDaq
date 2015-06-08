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
//      subSysBooter Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysBooter.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include "subSysBooter.h"

subSysBooter::subSysBooter (daqSubSystem* subsys)
:subSysTransitioner (subsys)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create subSysBooter Class Object\n");
#endif
  // empty
}

subSysBooter::~subSysBooter (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete subSysBooter Class Object\n");
#endif
  // empty
}

int
subSysBooter::action (void) const
{
  return CODA_BOOT_ACTION;
}

void
subSysBooter::executeItem (daqComponent* comp)
{
  comp->boot ();
}

int
subSysBooter::failureState (void)
{
  return CODA_DORMANT;
}

int
subSysBooter::successState (void)
{
  return CODA_BOOTED;
}
