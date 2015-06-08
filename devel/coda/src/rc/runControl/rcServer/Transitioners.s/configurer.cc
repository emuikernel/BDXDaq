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
//      Configurer Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: configurer.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#include <daqSubSystem.h>
#include "configurer.h"

configurer::configurer (daqSystem* system)
:transitioner (system)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create configurer Class Object\n");
#endif
  // empty
}

configurer::~configurer (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete configurer Class Object\n");
#endif
  // empty
}

int
configurer::transitionFinished (int fstate, int successState)
{
  if (fstate != CODA_DISCONNECTED)
    return 1;
  return 0;
}

int
configurer::action (void)
{
  return CODA_CONFIGURE_ACTION;
}

void
configurer::executeItem (daqSubSystem* subsys)
{
  subsys->configure ();
}

int
configurer::failureState (void)
{
  return CODA_BOOTED;
}

int
configurer::successState (void)
{
  return CODA_CONFIGURED;
}

int
configurer::transitionState (void)
{
  return CODA_BOOTED;
}
