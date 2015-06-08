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
//      Implementation of trigger for number of long words data variable
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcsNumLongTrigger.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#include <daqRun.h>
#include "rcsNumLongTrigger.h"

rcsNumLongTrigger::rcsNumLongTrigger (daqRun* r)
:daqDataTrigger (), run_ (r)
{
#ifdef _TRACE_OBJECTS
  printf ("     Create rcsNumLongTrigger Class Object\n");
#endif
  // empty
}

rcsNumLongTrigger::~rcsNumLongTrigger (void)
{
#ifdef _TRACE_OBJECTS
  printf ("     Delete rcsNumLongTrigger Class Object\n");
#endif
  // empty
}

void
rcsNumLongTrigger::trigger (daqData* data)
{
  if (!run_->locked ()) {
    if (run_->dataLimit () > 0 && ((int)(*data))/256 >= run_->dataLimit () ) {
      // disable further triggering
      data->disableTrigger ();
      // use network inteerface with first argument 0 to denote local
      // call
      run_->processCommand (0, DAEND, 0);
    }
    else 
      run_->longWords ((int)(*data));
  }
}
