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
//      subSysPrestarter Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysPrestarter.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#include "subSysPrestarter.h"

subSysPrestarter::subSysPrestarter (daqSubSystem* subsys)
:subSysTransitioner (subsys)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create subSysPrestarter Class Object\n");
#endif
  // empty
}

subSysPrestarter::~subSysPrestarter (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete subSysPrestarter Class Object\n");
#endif
  // empty
}

int
subSysPrestarter::action (void) const
{
  return CODA_PRESTART_ACTION;
}

void
subSysPrestarter::executeItem (daqComponent* comp)
{
  comp->prestart ();
}

int
subSysPrestarter::failureState (void)
{
  return CODA_DOWNLOADED;
}

int
subSysPrestarter::successState (void)
{
  return CODA_PAUSED;
}
