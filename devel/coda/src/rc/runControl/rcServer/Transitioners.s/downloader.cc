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
//      Downloader Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: downloader.cc,v $
//   Revision 1.3  1999/02/25 14:38:19  rwm
//   Limits defined in daqRunLimits.h
//
//   Revision 1.2  1996/10/31 15:56:15  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#include <daqRun.h>
#include <daqSubSystem.h>
#include "downloader.h"

downloader::downloader (daqSystem* system)
:transitioner (system), timeoutCache_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create downloader Class Object\n");
#endif
  // empty
}

downloader::~downloader (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete downloader Class Object\n");
#endif
  // empty
}

int
downloader::action (void)
{
  return CODA_DOWNLOAD_ACTION;
}

void
downloader::executeItem (daqSubSystem* subsys)
{
  subsys->download ();
}

int
downloader::failureState (void)
{
  return CODA_CONFIGURED;
}

int
downloader::successState (void)
{
  return CODA_DOWNLOADED;
}

int
downloader::transitionState (void)
{
  return CODA_CONFIGURED;
}

int
downloader::transitionTimeout (void)
{
  if (!timeoutCache_) {
    char *cs[MAX_NUM_COMPONENTS];

    int counter = system_->allEnabledComponents(cs,MAX_NUM_COMPONENTS);
    for(int i = 0; i < counter; i++)
      delete []cs[i];

    timeoutCache_ = (transitioner::transitionTimeout ()) * counter;
  }
  return timeoutCache_;
}

void
downloader::setupSuccess (void)
{
  transitioner::setupSuccess ();
  // check dynamic variable numbers, if it is 0, create them
  if (system_->run()->numDynamicVars () == 0)
    system_->run ()->createDynamicVars ();
}
