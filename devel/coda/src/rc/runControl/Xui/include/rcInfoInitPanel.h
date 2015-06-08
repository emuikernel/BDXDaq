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
//      runControl X window Interface initial panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcInfoInitPanel.h,v $
//   Revision 1.3  1998/04/08 18:31:19  heyes
//   new look and feel GUI
//
//   Revision 1.2  1997/05/26 12:27:45  heyes
//   embed tk in RC GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#ifndef _RC_INFO_INIT_PANEL_H
#define _RC_INFO_INIT_PANEL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <Xm/Xm.h>

#define RC_INFO_NUM_IMAGES 8

class rcInfoInitPanel
{
public:
  // constructor and destructor
  rcInfoInitPanel   (Widget parent, char* name,
		     Dimension width = 300,
		     Dimension height = 350);
  ~rcInfoInitPanel  (void);

  // initiate widget
  void init     (void);

  // return widget soociated with this
  const Widget baseWidget (void);

  // manage and unmanage
  void  unmanage (void);
  void  manage   (void);

  // start and end showing rotating log
  void  start              (void);
  void  stop               (void);
  void  disableRotatingLog (void);

  // return class name
  virtual const char* className (void) const {return "rcInfoInitPanel";}

protected:
  // deny access to copy and assignment
  rcInfoInitPanel (const rcInfoInitPanel&);
  rcInfoInitPanel& operator = (const rcInfoInitPanel);

  // some window and viewport constants
  static double xmin, xmax, ymin, ymax;
  static double vxmin, vxmax, vymin, vymax;

  // timer callback
  static void timerCallback (XtPointer, XtIntervalId *);

  // create all images
  double createRotatingImages (double yprev);
  
private:
  // frame Widget
  Widget     frame_;
  // flag to tell already started
  int        armed_;
  XtIntervalId autoTimerId_;
  XtAppContext appContext_;
  // counter
  int          counter_;
  // flag to denote whether to rotate these images
  int          rotate_;
};
#endif
