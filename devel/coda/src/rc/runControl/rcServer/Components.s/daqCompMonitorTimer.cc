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
//      Implementation of components' monitor timer
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqCompMonitorTimer.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#include <daqCompMonitor.h>
#include "daqCompMonitorTimer.h"

daqCompMonitorTimer::daqCompMonitorTimer (int interval)
:reactorTimer (*glbReactor), monitor_ (0), interval_ (interval)
{
#ifdef _TRACE_OBJECTS
  printf ("    Create daqCompMonitorTimer Class Object\n");
#endif
}

daqCompMonitorTimer::~daqCompMonitorTimer (void)
{
#ifdef _TRACE_OBJECTS
  printf ("    Delete daqCompMonitorTimer Class Object\n");
#endif
  monitor_ = 0;
}

void
daqCompMonitorTimer::timer_callback (void)
{
  assert (monitor_);
  monitor_->timerCallback ();
}

void
daqCompMonitorTimer::monitor (daqCompMonitor* mon)
{
  monitor_ = mon;
}

void
daqCompMonitorTimer::timerInterval (int sec)
{
  interval_ = sec;
  interval (sec*1000);
}

int
daqCompMonitorTimer::timerInterval (void) const
{
  return interval_;
}

