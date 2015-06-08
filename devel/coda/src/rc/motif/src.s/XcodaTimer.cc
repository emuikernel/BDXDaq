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
//   $Log: XcodaTimer.cc,v $
//   Revision 1.3  1998/04/08 17:29:38  heyes
//   get in there
//
//   Revision 1.2  1997/04/16 18:12:20  chen
//   add multilist and extensions
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <XcodaTimer.h>

XcodaTimer::XcodaTimer (Widget ref)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaTimer Object \n");
#endif
  _app_context = XtWidgetToApplicationContext(ref);
  _armed = False;
  _timer_interval = 0;
  _single_timer_id = (XtIntervalId)NULL;
  _auto_timer_id = (XtIntervalId)NULL;
  _counter = 0;
}

XcodaTimer::XcodaTimer (XtAppContext context)
:_app_context (context)
{
#ifdef _TRACE_OBJECTS
  printf("Create XcodaTimer Object \n");
#endif
  _armed = False;
  _timer_interval = 0;
  _single_timer_id = (XtIntervalId)NULL;
  _auto_timer_id = (XtIntervalId)NULL;
  _counter = 0;
}

void XcodaTimer::auto_arm (unsigned int msec)
{
  _armed = True;
  _timer_interval = msec;
  _auto_timer_id = XtAppAddTimeOut (_app_context,  msec,
				    &XcodaTimer::autoTimerCallback,
				    (XtPointer) this);
}

void XcodaTimer::autoTimerCallback(XtPointer client_data, XtIntervalId *)
{
  XcodaTimer *obj = (XcodaTimer *)client_data;
  obj->_counter++;
  if(obj->_armed)
    obj->local_timer_callback();
}

void XcodaTimer::local_timer_callback()
{
  timer_callback();
  _auto_timer_id = XtAppAddTimeOut (_app_context, _timer_interval,
				    &XcodaTimer::autoTimerCallback,
				    (XtPointer)this);  
}

void XcodaTimer::arm (unsigned int msec)
{
  _single_timer_id = XtAppAddTimeOut (_app_context, msec,
				     &XcodaTimer::singleTimerCallback,
				     (XtPointer)this);
}

void XcodaTimer::singleTimerCallback(XtPointer client_data, XtIntervalId *)
{
  XcodaTimer *obj = (XcodaTimer *)client_data;

  obj->timer_callback();
}

Boolean XcodaTimer::is_armed()
{
  return _armed;
}

void XcodaTimer::dis_arm()
{
  if(_armed && _auto_timer_id){
    XtRemoveTimeOut (_auto_timer_id);
  }
  _auto_timer_id = 0;
  _armed = False;
}

float XcodaTimer::elapsedTime()
{
  if (_armed){
    return ((float)_counter*_timer_interval/1000.0);
  }
  else
    return 0.0;
}

XcodaTimer::~XcodaTimer()
{
#ifdef _TRACE_OBJECTS
  printf("Destroy XcodaTimer Object \n");
#endif
  if(_armed && _auto_timer_id){
    XtRemoveTimeOut (_auto_timer_id);
  }
  _auto_timer_id = (XtIntervalId)NULL;
}
