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
//      RunControl Information and status panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcInfoPanel.h,v $
//   Revision 1.2  1998/04/08 18:31:20  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#ifndef _RC_INFO_PANEL_H
#define _RC_INFO_PANEL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <XcodaUi.h>
#include <rcPanel.h>
#include <rcClientHandler.h>
#include <rcMenuWindow.h>

#include <Xm/Form.h>

class rcInfoInitPanel;
class rcInfoRunPanel;
class rcRunTypeDialog;

class rcInfoPanel: public XcodaUi, public rcPanel
{
public:
  // constructor and destructor
  rcInfoPanel  (Widget parent, char* name, rcClientHandler& handler);
  ~rcInfoPanel (void);

  // initialize all widgets
  void         init (void);

  // redefine config
  virtual void config (int status);
  virtual void anaLogChanged (daqNetData* info, int added);

  // stop all dynamic features
  virtual void stop   (void);
  // get ready to exit the runcontrol
  virtual void readyToExit (void);

  // return runType dialog
  rcRunTypeDialog* runTypeDialog (void);

  // popup zoomed dialog box for event display
  void zoomOnEventInfo (void);

  // popup a event/data rate graphical display
  void popupRateDisplay (rcMenuWindow *menW);

  // class name
  virtual const char* className (void) const {return "rcInfoPanel";}

private:
  // parent widget
  Widget parent_;
  // internal panel corresponding to different state
  rcInfoInitPanel* initPanel_;
  // run time information panel
  rcInfoRunPanel*  runPanel_;
  // network handler to remote runcontrol server
  rcClientHandler& netHandler_;
};
#endif

