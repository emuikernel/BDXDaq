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
//      RunControl Mastership Display + Action
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMastership.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#ifndef _RC_MASTERSHIP_H
#define _RC_MASTERSHIP_H

#include <XcodaUi.h>
#include <rcPanel.h>
#include <rcClientHandler.h>

class rcMastershipButton;
class XcodaXpmLabel;

class rcMastership: public XcodaUi, public rcPanel
{
public:
  // constructor and destructor
  rcMastership (Widget parent, char* name, rcClientHandler& handler);
  virtual ~rcMastership (void);

  // init all widgets
  void     init (void);

  // inherited operations
  virtual void config (int status);
  virtual void configMastership (int type);

private:
  // network handler
  rcClientHandler& netHandler_;
  // label pixmap
  XcodaXpmLabel* label_;
  // toggle button
  rcMastershipButton* button_;
  // Widget
  Widget parent_;
};
#endif
