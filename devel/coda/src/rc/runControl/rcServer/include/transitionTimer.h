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
//      State Transition Timer
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: transitionTimer.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#ifndef _CODA_TRANSITION_TIMER_H
#define _CODA_TRANSITION_TIMER_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <daqGlobal.h>
#include <reactorTimer.h>

class transitioner;

class transitionTimer : public reactorTimer
{
public:
  // constructor and destructor
  transitionTimer  (void);
  ~transitionTimer (void);
  
  void transitionerPtr (transitioner* traner);

  virtual void timer_callback (void);

private:
  transitioner* trans_;
};
#endif


