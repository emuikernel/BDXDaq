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
//      RunControl Run Time Information panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcInfoRunPanel.h,v $
//   Revision 1.2  1998/04/08 18:31:22  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#ifndef _RC_INFO_RUN_PANEL_H
#define _RC_INFO_RUN_PANEL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <XcodaUi.h>
#include <rcClientHandler.h>
#include <rcMenuWindow.h>

class rcRunCInfoPanel;
class rcRunSInfoPanel;
class rcRunDInfoPanel;
class rcRunStatusPanel;
class rcRunTypeDialog;
class daqNetData;

class rcInfoRunPanel: public XcodaUi
{
public:
  // constructor and destructor
  rcInfoRunPanel (Widget parent, char* name, rcClientHandler& handler);
  virtual ~rcInfoRunPanel (void);

  // Initialize all widgets
  void         init (void);

  // redefine manage + unmanage
  virtual void manage    (void);
  virtual void unmanage  (void);
  // config function
  void         config (int status);
  // handle ana log changed information
  void         anaLogChanged (daqNetData* info, int added);

  // stop any dynamic feature of the panel
  void         stop   (void);

  // return run type dialog
  rcRunTypeDialog* runTypeDialog (void);

  // popup zoomed display of event information
  void zoomOnEventInfo (void);

  // popup graphical display of event rate
  void popupRateDisplay (rcMenuWindow *menW);       

  // class name
  virtual const char* className (void) const {return "rcInfoRunPanel";}

private:
  // network handler to run control server
  rcClientHandler& netHandler_;
  // parent widget
  Widget parent_;  

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // widget to hold data log file
  Widget datafile_;
  // update data file label to this widget
  void updateDataFileLabel (void);
#endif
       
  Widget statusPanel_;
  rcRunCInfoPanel*  cinfoPanel_;
  rcRunSInfoPanel*  sinfoPanel_;
  rcRunDInfoPanel*  dinfoPanel_;
  rcRunTypeDialog*  runTypeDialog_;
};
#endif
