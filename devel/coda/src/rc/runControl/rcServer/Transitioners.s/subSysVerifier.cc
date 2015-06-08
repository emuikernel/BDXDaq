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
//      subSysVerifier Class Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysVerifier.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#include "subSysVerifier.h"

subSysVerifier::subSysVerifier (daqSubSystem* subsys)
:subSysTransitioner (subsys)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create subSysVerifier Class Object\n");
#endif
  // empty
}

subSysVerifier::~subSysVerifier (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete subSysVerifier Class Object\n");
#endif
  // empty
}

int
subSysVerifier::action (void) const
{
  return CODA_VERIFY_ACTION;
}

void
subSysVerifier::executeItem (daqComponent* comp)
{
  comp->verify ();
}

int
subSysVerifier::failureState (void)
{
  return CODA_BOOTED;
}

int
subSysVerifier::successState (void)
{
  return CODA_VERIFIED;
}
