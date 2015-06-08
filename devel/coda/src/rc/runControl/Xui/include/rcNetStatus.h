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
//      RunControl Network Communication Status
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcNetStatus.h,v $
//   Revision 1.2  1998/04/08 18:31:26  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#ifndef _RC_NET_STATUS_H
#define _RC_NET_STATUS_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <Xm/Xm.h>
class rcNetStatus
{
public:
  // constructor and destructor
  rcNetStatus  (Widget parent, char* name,
		Dimension width = 200,
		Dimension height = 20);
  ~rcNetStatus (void);

  // return widget associated with this
  const Widget baseWidget (void);

  // start showing status
  void  start (void);
  void  stop  (void);

  // interface to non-timer based calling routines
  void  startShowingStatus  (void);
  void  stopShowingStatus (void);
  void  showingInProgress   (void);

  // class name
  const char* className (void) const {return "rcNetStatus";}

protected:
  static void autoTimerCallback (XtPointer, XtIntervalId *);

private:
  // frame widget
  Widget frame_;
  // flag to tell already started
  int armed_;
  XtIntervalId autoTimerId_;
  XtAppContext appContext_;
  // counter
  int counter_;
};
#endif
