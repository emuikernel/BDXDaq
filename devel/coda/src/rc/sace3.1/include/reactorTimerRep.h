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
//	 Reactor Timer Internal Representation
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: reactorTimerRep.h,v $
//   Revision 1.1.1.1  1996/10/10 20:04:48  chen
//   simple ACE version 3.1
//
//
#ifndef _REACTOR_TIMER_REP_H
#define _REACTOR_TIMER_REP_H

#include <sysincludes.h>

#include <Reactor.h>
#include <Event_Handler.h>

class reactorTimer;

class reactorTimerRep: public Event_Handler
{
  friend class reactorTimer;
 public:
  // Destructor
  ~reactorTimerRep (void);
  // inherited from event handler
  virtual int handle_timeout (const Time_Value &tv, const void *arg = 0);
  virtual int get_handle (void) const;

 private:
  // constructor 
  reactorTimerRep (void);
  // register real timer 
  void timerSelf (reactorTimer *timer);
  // some communication between timer and timeRep
  void counter (int cnter);
  int  counter (void) const;
  void interval (int val);
  int  interval (void) const;
  // data area
  reactorTimer *realTimer_;
  int     counter_;
  int     interval_;
};
#endif
