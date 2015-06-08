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
//      subSysAborter Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysAborter.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#include "subSysAborter.h"

subSysAborter::subSysAborter (daqSubSystem* subsys)
:subSysTransitioner (subsys), wanted_ (CODA_BOOTED)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create subSysAborter Class Object\n");
#endif
  // empty
}

subSysAborter::~subSysAborter (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete subSysAborter Class Object\n");
#endif
  // empty
}

void
subSysAborter::abortedState (int wanted)
{
  wanted_ = wanted;
}

int
subSysAborter::action (void) const
{
  return CODA_ABORT_ACTION;
}

void
subSysAborter::executeItem (daqComponent* comp)
{
  comp->abort (wanted_);
}

int
subSysAborter::failureState (void)
{
  return CODA_BOOTED;
}

int
subSysAborter::successState (void)
{
  return wanted_;
}
