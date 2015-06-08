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
//	 Timer Interface to Reactor Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: reactorTimer.h,v $
//   Revision 1.1.1.1  1996/10/10 20:04:48  chen
//   simple ACE version 3.1
//
//
#ifndef _REACTOR_TIMER_H
#define _REACTOR_TIMER_H

#include <Reactor.h>
#include <Event_Handler.h>
#include "reactorTimerRep.h"

class reactorTimer
{
 public:
  // single alarm firing in milliseconds
  void arm (unsigned int msec);
  // Auto timer for automatic firing event msec
  void auto_arm (unsigned int msec);
  // Auto timer after tick interval has been set
  void auto_arm (void);
  // is timer auto armed
  int  auto_armed () const;
  // disable an alarm
  void dis_arm ();
  // return time since timer started for auto_timer in seconds
  float elapsedTime();
  // return counter and interval
  void counter (int cnt);
  int  counter (void) const;
  void interval (int val);
  int  interval (void) const;
  // real task will be performed by this routine
  virtual void timer_callback () = 0;
  // Destructor
  virtual ~reactorTimer (void);

 protected:
  // constructor
  reactorTimer (Reactor &reactor);

 private:
  Reactor         &reactor_;
  reactorTimerRep timerRep_;
  int             autoArmed_;
};
#endif
