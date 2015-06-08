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
//      Verifier Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: verifier.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include <daqSubSystem.h>
#include "verifier.h"

verifier::verifier (daqSystem* system)
:transitioner (system)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create verifier Class Object\n");
#endif
  // empty
}

verifier::~verifier (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete verifier Class Object\n");
#endif
  // empty
}

int
verifier::action (void)
{
  return CODA_VERIFY_ACTION;
}

void
verifier::executeItem (daqSubSystem* subsys)
{
  subsys->verify ();
}

int
verifier::failureState (void)
{
  return CODA_DOWNLOADED;
}

int
verifier::successState (void)
{
  return CODA_VERIFIED;
}

int
verifier::transitionState (void)
{
  return CODA_VERIFYING;
}
