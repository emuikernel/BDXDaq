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
//      DAQ SubSystem Transitioner Class (Implementation)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysTransitioner.cc,v $
//   Revision 1.2  1996/10/14 20:02:54  heyes
//   changed message system
//
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#include <daqActions.h>
#include <daqState.h>
#include <rcMsgReporter.h>
#include "subSysTransitioner.h"

subSysTransitioner::subSysTransitioner (daqSubSystem* subsys)
:subsystem_ (subsys)
{
#ifdef _TRACE_OBJECTS
  printf ("Create subSysTransitioner Class Object\n");
#endif
  // empty
}

subSysTransitioner::~subSysTransitioner (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete subSysTransitioner Class Object\n");
#endif
  // empty
}

int
subSysTransitioner::execute (void)
{
  int res = CODA_SUCCESS;
  if (subsystem_->enabled ()) {
    codaSlistIterator ite (subsystem_->compList_);
    daqComponent* comp = 0;

    for (ite.init (); !ite; ++ite) {
      comp = (daqComponent *)ite ();
      if (compEnabled (comp)) {
	executeItem (comp);
#if !defined (_CODA_USE_THREADS)
	if (comp->status () != CODA_SUCCESS) {
	  res = CODA_ERROR;
	  break;
	}
#endif  // in thread situation, try to boot all processes up
      }
    }
    if (res == CODA_ERROR) {
      reporter->cmsglog (CMSGLOG_ERROR,"%s subsystem %s failed\n", subsystem_->title(),
			       codaDaqActions->actionString (action ()));
      subsystem_->setState (failureState ());
    }
  }
  if (res == CODA_SUCCESS)
    subsystem_->setState (successState ());
  return res;
}

void
subSysTransitioner::cancel (void)
{
  if (subsystem_->enabled ()) {
    codaSlistIterator ite (subsystem_->compList_);
    daqComponent* comp = 0;

    for (ite.init (); !ite; ++ite) {
      comp = (daqComponent *)ite ();
      if (compEnabled (comp)) 
	comp->cancelTransition ();
    }
  }
}

int
subSysTransitioner::compEnabled (daqComponent* comp)
{
  return comp->enabled ();
}

