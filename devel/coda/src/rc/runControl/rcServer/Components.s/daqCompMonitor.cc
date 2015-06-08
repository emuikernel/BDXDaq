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
//      Implementation of daqCompMonitor Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqCompMonitor.cc,v $
//   Revision 1.10  1999/07/28 19:17:38  rwm
//   Bunch of hacks to support Pause script button for Hall B.
//
//   Revision 1.9  1999/02/25 14:36:56  rwm
//   Limits defined in daqRunLimits.h
//
//   Revision 1.8  1999/02/02 19:02:09  heyes
//   AUTOEND feature
//
//   Revision 1.7  1998/11/05 20:11:41  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.6  1997/09/05 12:03:59  heyes
//   almost final
//
//   Revision 1.5  1997/07/22 19:38:52  heyes
//   cleaned up lots of things
//
//   Revision 1.4  1997/05/16 16:04:08  chen
//   add global script capability
//
//   Revision 1.3  1996/11/04 16:13:44  chen
//   add options for monitoring components
//
//   Revision 1.2  1996/10/28 14:23:02  heyes
//   communication with ROCs changed
//
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#include <rcMsgReporter.h>
#include <daqComponent.h>
#include "daqCompMonitor.h"
#include <daqState.h>

daqCompMonitor::daqCompMonitor (daqRun& run, int interval)
:run_ (run), junkList_ (), timer_ (interval), autoend_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqCompMonitor Class Object\n");
#endif
  if (::getenv("AUTOEND") != NULL) {
     autoend_ = 1;
  }
  timer_.monitor (this);
}

daqCompMonitor::~daqCompMonitor (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqCompMonitor Class Object\n");
#endif
  // disable timer
  timer_.dis_arm ();
}

void
daqCompMonitor::startMonitoringComp (void)
{
  // remove old information

printf("daqCompMonitor::startMonitoringComp reached\n");
  junkList_.deleteAllValues ();

  int mint = timer_.timerInterval () * 1000;
  timer_.auto_arm (mint);
}

void
daqCompMonitor::endMonitoringComp (void)
{
printf("daqCompMonitor::endMonitoringComp reached\n");
  timer_.dis_arm ();
}

void
daqCompMonitor::timerInterval (int sec)
{
  if (timer_.auto_armed ()) {
    timer_.dis_arm ();
    timer_.timerInterval (sec);
    startMonitoringComp ();
  }
  else
    timer_.timerInterval (sec);    
}

int
daqCompMonitor::timerInterval (void) const
{
  return timer_.timerInterval ();
}

void
daqCompMonitor::timerCallback (void)
{
  daqSystem& system = run_.system ();
  // assume no more than MAX_NUM_COMPONENTS components
  daqComponent *comps[MAX_NUM_COMPONENTS]; 
  int          bufsize = MAX_NUM_COMPONENTS;
  int          numComps = 0;
  int          state = 0, found = 0;

printf("daqCompMonitor::timerCallback reached\n");

  numComps = system.allEnabledComponents (comps, bufsize);

  if (numComps > 0) 
    for (int i = 0; i < numComps; i++) {
      if (comps[i]->monitored ()) {
	reporter->cmsglog (CMSGLOG_INFO1, "Checking status of %s\n", 
			   comps[i]->title ());
	state = comps[i]->state2 ();
	if (state == CODA_DISCONNECTED || 
	    // These are all the valid states.
	    ((state != CODA_ACTIVE) && 
	     (state != CODA_DOWNLOADED) && 
	     (state != CODA_PAUSED) && 
	     (state != CODA_ENDING))) {

	  // disable this component so that it will not particpate
	  // state transition

	  reporter->cmsglog (CMSGLOG_ERROR, "%s is in state %s should be %s\n",
			     comps[i]->title (), 
			     codaDaqState->stateString (state),
			     codaDaqState->stateString (CODA_ACTIVE));

	  // BUG BUG Take this out for now...
	  //reporter->cmsglog (CMSGLOG_ERROR, "%s now disabled (re-enable via configure)\n",
	  //	comps[i]->title ());
	  // comps[i]->disable ();  

	  if (autoend_) {
	    junkList_.add ((void *)comps[i]);
	    found = 1;
	  }
	}
      }
    }
  if (found) {
    if (!run_.locked () && autoend_) {
				// use network interface with first argument 0 
				// to denote this is a local call
      run_.processCommand (0, DAEND, 0);
      timer_.dis_arm ();
    }
  }

}

void
daqCompMonitor::enableDisconnectedComp (void)
{
  codaSlistIterator ite (junkList_);
  daqComponent* comp = 0;

  for (ite.init (); !ite; ++ite) {
    comp = (daqComponent *) ite ();
    comp->enable ();
  }
}

void
daqCompMonitor::enableAutoEnd (void)
{
  autoend_ = 1;
}

void
daqCompMonitor::disableAutoEnd (void)
{
  autoend_ = 0;
}

int
daqCompMonitor::autoend (void) const
{
  return autoend_;
}



  
    
