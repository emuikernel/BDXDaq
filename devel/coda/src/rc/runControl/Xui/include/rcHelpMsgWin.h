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
//      RunControl Help Message Window
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcHelpMsgWin.h,v $
//   Revision 1.2  1998/04/08 18:31:17  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#ifndef _RC_HELP_MSG_WIN_H
#define _RC_HELP_MSG_WIN_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <XcodaUi.h>
#include <Balloon.h>

class rcHelpMsgWin: public XcodaUi
{
public:
  // constructor and destructor
  rcHelpMsgWin (Widget parent, char* name);
  virtual ~rcHelpMsgWin (void);
  
  // Initiate all widgets
  void init (void);
  
  // set and erase message
  void setMessage    (char* msg);
  void eraseMessage  (void);
  
  // change display message flag
  void displayMsg   (void);
  void displayNoMsg (void);
  
  // class name
  virtual const char* className (void) const {return "rcHelpMsgWin";}
  
private:
  // Widget
  Widget  parent_;
  // flag of displaying message
  int     displayMsg_;
  Widget  balloon_;
};
#endif
