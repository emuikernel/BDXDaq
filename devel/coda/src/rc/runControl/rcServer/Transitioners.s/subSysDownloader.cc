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
//      subSysDownloader Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysDownloader.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include "subSysDownloader.h"

subSysDownloader::subSysDownloader (daqSubSystem* subsys)
:subSysTransitioner (subsys)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create subSysDownloader Class Object\n");
#endif
  // empty
}

subSysDownloader::~subSysDownloader (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete subSysDownloader Class Object\n");
#endif
  // empty
}

int
subSysDownloader::action (void) const
{
  return CODA_DOWNLOAD_ACTION;
}

void
subSysDownloader::executeItem (daqComponent* comp)
{
  comp->download ();
}

int
subSysDownloader::failureState (void)
{
  return CODA_CONFIGURED;
}

int
subSysDownloader::successState (void)
{
  return CODA_DOWNLOADED;
}
