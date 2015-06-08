//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
// 
// Description:
//	 Implementation of reactor timer internal representaion class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: reactorTimerRep.C,v $
//   Revision 1.1.1.1  1996/10/10 20:04:48  chen
//   simple ACE version 3.1
//
//
#include <assert.h>
#include "reactorTimer.h"
#include "reactorTimerRep.h"

reactorTimerRep::reactorTimerRep (void)
:Event_Handler (), counter_(0), interval_ (0)
{
  realTimer_ = 0;
}

reactorTimerRep::~reactorTimerRep (void)
{
  // empty
  // no need to delete pointer *realTimer_ since this one
  // calls rep
}

int
reactorTimerRep::handle_timeout (const Time_Value &tv, const void *arg)
{
  counter_++;
  assert (realTimer_);
  realTimer_->timer_callback();
  return 0;
}

int
reactorTimerRep::get_handle (void) const
{
  return 0;
}

void
reactorTimerRep::timerSelf (reactorTimer *timer)
{
  realTimer_ = timer;
}

void
reactorTimerRep::counter (int cnt) 
{
  counter_ = cnt;
}

int
reactorTimerRep::counter (void) const
{
  return counter_;
}

void 
reactorTimerRep::interval (int val)
{
  interval_ = val;
}

int
reactorTimerRep::interval (void) const
{
  return interval_;
}
