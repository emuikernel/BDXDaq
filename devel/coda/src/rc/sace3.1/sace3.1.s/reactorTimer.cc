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
//	 Implementation of Reactor Timer Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: reactorTimer.C,v $
//   Revision 1.1.1.1  1996/10/10 20:04:48  chen
//   simple ACE version 3.1
//
//
#include "reactorTimer.h"

reactorTimer::reactorTimer (Reactor &reactor)
:reactor_ (reactor), timerRep_(), autoArmed_ (0)
{
  timerRep_.timerSelf (this);
}

reactorTimer::~reactorTimer (void)
{
  // empty
}

void
reactorTimer::arm (unsigned int msec)
{
  Time_Value tv (0, (long)(msec*1000));
  timerRep_.interval (msec);
  timerRep_.counter (0);
  reactor_.schedule_timer (&timerRep_, 0, tv);
  autoArmed_ = 0;
}

void 
reactorTimer::auto_arm (unsigned int msec)
{
  if (autoArmed_) 
    dis_arm ();
  Time_Value tv (0, (long)(msec*1000));
  timerRep_.interval (msec);
  timerRep_.counter (0);
  reactor_.schedule_timer (&timerRep_, 0, tv, tv);
  autoArmed_ = 1;
}

void 
reactorTimer::auto_arm (void)
{
  if (timerRep_.interval () == 0)
    return;
  if (autoArmed_)
    dis_arm ();
  Time_Value tv (0, (long)(timerRep_.interval()*1000));
  timerRep_.counter (0);
  reactor_.schedule_timer (&timerRep_, 0, tv, tv);
  autoArmed_ = 1;
}

int
reactorTimer::auto_armed() const
{
  return autoArmed_;
}

void
reactorTimer::dis_arm ()
{
  if (autoArmed_){
    reactor_.cancel_timer (&timerRep_);
    autoArmed_ = 0;
    timerRep_.interval (0);
    timerRep_.counter (0);
  }
}

float 
reactorTimer::elapsedTime ()
{
  if (autoArmed_){
    return ((float)timerRep_.counter()*timerRep_.interval()/1000.0);
  }
  else
    return 0.0;
}

void 
reactorTimer::counter (int cnt)
{
  timerRep_.counter (cnt);
}

int 
reactorTimer::counter (void) const
{
  return timerRep_.counter ();
}

void 
reactorTimer::interval (int val)
{
  timerRep_.interval (val);
}

int
reactorTimer::interval (void) const
{
  return timerRep_.interval ();
}
