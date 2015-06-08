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
//      CODA RunControl Server Components' Monitor Timer
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqCompMonitorTimer.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:20  chen
//   run control source
//
//
#ifndef _CODA_DAQCOMP_MONITOR_TIMER_H
#define _CODA_DAQCOMP_MONITOR_TIMER_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <daqGlobal.h>
#include <reactorTimer.h>

class daqCompMonitor;

class daqCompMonitorTimer: public reactorTimer
{
public:
  // constructor and destructor

  // timer interval default to 10 seconds
  daqCompMonitorTimer  (int timerInterval = 10);
  ~daqCompMonitorTimer (void);

  // setup pointer to daqCompMonitor
  void     monitor     (daqCompMonitor* mon);
  // set/change timer interval
  void     timerInterval (int interval);
  int      timerInterval (void) const;
  // timer callback function
  virtual void  timer_callback (void);

private:
  daqCompMonitor* monitor_;
  int             interval_;
};

#endif



  
