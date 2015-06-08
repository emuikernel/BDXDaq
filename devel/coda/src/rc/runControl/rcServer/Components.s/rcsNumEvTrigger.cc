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
//      Implementation of trigger for number of events data variable
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcsNumEvTrigger.cc,v $
//   Revision 1.2  1998/11/05 20:11:55  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#include <daqRun.h>
#include "rcsNumEvTrigger.h"

rcsNumEvTrigger::rcsNumEvTrigger (daqRun* r)
:daqDataTrigger (), run_ (r)
{
#ifdef _TRACE_OBJECTS
	printf ("     Create rcsNumEvTrigger Class Object\n");
#endif
	// empty
}

rcsNumEvTrigger::~rcsNumEvTrigger (void)
{
#ifdef _TRACE_OBJECTS
	printf ("     Delete rcsNumEvTrigger Class Object\n");
#endif
	// empty
}

void
rcsNumEvTrigger::trigger (daqData* data)
{
	if (!run_->locked ()) {
		if (run_->eventLimit () > 0 && (int)(*data) >= run_->eventLimit () ) {
			// disable further triggering
			data->disableTrigger ();
			// use network inteerface with first argument 0 to denote local
			// call
			run_->processCommand (0, DAEND, 0);
		}
		else	{
			run_->eventNumber ((int)(*data));
		}
	}
}

