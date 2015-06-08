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
//      RunControl Run Status Panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunStatusPanel.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#ifndef _RC_RUN_STATUS_PANEL_H
#define _RC_RUN_STATUS_PANEL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcClientHandler.h>

#include <cgDispDev.h>
#include <cgVmeCrate.h>
#include <cgComputer.h>
#include <cgTapeDrive.h>
#include <cgTrashCan.h>
#include <cgLine.h>

class rcRunStatusPanel
{
public:
  // constructor and destructor
  rcRunStatusPanel  (Widget parent, char* name, rcClientHandler& handler,
		     Dimension width = 200,
		     Dimension height = 200);
  ~rcRunStatusPanel (void);

  // return widget assocaited with this
  const Widget baseWidget (void);

  // change under different state
  void config (int status);
  // change for diffrent ana log files
  void anaLogChanged (daqNetData* info, int added);

  // starting and ending data taking
  void startDataTaking (void);
  void endDataTaking   (void);
  
  // class name
  const char* className (void) const {return "rcRunStatusPanel";}

protected:

  // Timer callback
  static void timerCallback (XtPointer, XtIntervalId *);

  // internal functions for different state
  void disconnectComponents (void);
  void connectComponents    (void);
  void readyToGo            (void);

private:
  // network handler
  rcClientHandler& netHandler_;
  // graphical part
  cgDispDev*   disp_;
  cgScene*     scene_;
  cgVmeCrate** crates_;
  cgComputer*  computer_;
  cgTapeDrive* drive_;
  cgTrashCan*  can_;
  cgLine**     lines_;
  Pixel        lineFg_;
  Pixel        bg_;

  // flag that notifies a started timer
  int armed_;
  XtIntervalId autoTimerId_;
  XtAppContext appContext_;
  // timer counter
  int counter_;
  // data logged flag
  int dataLogged_;
};
#endif
