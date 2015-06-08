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
//      RunControl Message Window
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMsgWindow.h,v $
//   Revision 1.2  1996/10/14 20:13:41  chen
//   add display server messages preference
//
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#ifndef _RC_MSG_WINDOW_H
#define _RC_MSG_WINDOW_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <Xm/Xm.h>
#include <Xm/Text.h>

#include <XcodaUi.h>
#include <rcPanel.h>
#include <rcClientHandler.h>

class rcMsgWindow: public XcodaUi, public rcPanel
{
public:
  // constructor and destructor
  rcMsgWindow (Widget parent,
	       char* name, rcClientHandler& handler);
  virtual ~rcMsgWindow (void);

  // initialize xwindow widgets
  void    init         (void);

  // redefine manage and unmanage
  void    manage       (void);
  void    unmanage     (void);

  // display/undisplay server messages
  void    displayMsg   (void);
  void    displayNoMsg (void);

  // redefine config
  virtual void config  (int status);

  // class name
  virtual const char* className (void) const {return "rcMsgWindow";}

protected:
  // monitor on callback function
  static void runMsgCallback (int status, void* arg, daqNetData* data);
  void        monitorRunMsg  (void);

private:
  // parent widget
  Widget parent_;
  // text window widget, _w will be scrolled window
  Widget textw_;
  
  // network client handler
  rcClientHandler& netHandler_;
  // current text cursor position
  XmTextPosition wprPosition_;
  // connection history flag
  int connectionHist_;
  // show text flag
  int showText_;
};
#endif

  
