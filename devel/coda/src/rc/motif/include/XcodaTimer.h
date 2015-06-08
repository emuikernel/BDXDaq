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
//	X Window Timer Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaTimer.h,v $
//   Revision 1.3  1998/04/08 17:29:39  heyes
//   get in there
//
//   Revision 1.2  1997/04/16 18:12:21  chen
//   add multilist and extensions
//
//   Revision 1.1.1.1  1996/10/10 19:24:59  chen
//   coda motif C++ library
//
//

#ifndef _XCODATIMER_H
#define _XCODATIMER_H

#include <stdio.h>
#include <assert.h>
#include <Xm/Xm.h>

class XcodaTimer{
public:
  // Single alarm firing in milliseconds
  void arm (unsigned int);
  // Is timer single armed?
  // Auto timer for automatic firing every msec
  void auto_arm (unsigned int);
  // Is timer auto_armed?
  Boolean auto_armed();
  // automatic armed flag
  Boolean is_armed();
  // disarm an alarm
  void dis_arm();
  // returns time since timer started for auto_timer
  float elapsedTime();
  // Destructor
  virtual ~XcodaTimer();


protected:
  // abstract class
  XcodaTimer (Widget);
  XcodaTimer (XtAppContext appcontext);
  // real timer callback function, derived class must provide
  // real callback function
  virtual void timer_callback () = 0;
  // data area
  unsigned int  _timer_interval;
  unsigned int  _counter;

private:
  XtAppContext  _app_context;
  XtIntervalId  _single_timer_id;
  XtIntervalId  _auto_timer_id;
  static        void autoTimerCallback(XtPointer, XtIntervalId *);
  static        void singleTimerCallback(XtPointer, XtIntervalId *);
  Boolean       _armed;
  void          local_timer_callback();
};
#endif  
    
