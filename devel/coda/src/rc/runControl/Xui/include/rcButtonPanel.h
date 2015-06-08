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
//      RunControl Command Button Panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcButtonPanel.h,v $
//   Revision 1.3  1999/07/28 19:54:44  rwm
//   Added pause button (script) for Hall-B
//
//   Revision 1.2  1997/10/15 16:08:24  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#ifndef _RC_BUTTON_PANEL_H
#define _RC_BUTTON_PANEL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <XcodaUi.h>
#include <rcPanel.h>
#include <rcClientHandler.h>
#include <rcComdButton.h>

class rcConnect;
class rcDisc;
class rcLoad;
class rcConfigure;
class rcDownload;
class rcPrestart;
class rcActivate;
class rcEnd;
class rcPause;
class rcResume;
class rcReset;
class rcAbort;
class rcAuto;
class rcCancel;

class rcHelpMsgWin;
class rcNetStatus;
class rcInfoPanel;

class rcButtonPanel: public XcodaUi, public rcPanel
{
public:
  // constructor and destructor
  rcButtonPanel  (Widget parent, char* name, rcClientHandler& handler);
  ~rcButtonPanel (void);

  // initialize all widgets
  void         init   (void);
  // redefine config inherited from rcPanel
  virtual void config (int status);
  virtual void configMastership (int type);

  // setup help message window
  void         helpMsgWin    (rcHelpMsgWin* win);
  // setup network status window
  void         netStatusWin  (rcNetStatus* win);
  // setup runinfo panel
  void         infoPanel     (rcInfoPanel* panel);

  // deactivate transition panel
  void         deactivateTransitionPanel (void);
  void         activateTransitionPanel   (void);

  // class name
  virtual const char* className (void) const {return "rcButtonPanel";}

protected:
  // different configuration for different states
  void         notConnected     (void);
  void         connected        (void);
  void         loadAndConfigure (void);
  void         download         (void);
  void         prestart         (void);
  void         goAndEnd         (void);
  void         endAndPause      (void);
  void         resumeAndEnd     (void);

  // unmanage a particular button
  void         unmanageButton (rcComdButton* button);

private:
  // parent widget
  Widget parent_;
  // network connection handler
  rcClientHandler& netHandler_;
  // two row column widgets
  Widget lw_;
  Widget rw_;
  // all internal buttons
  rcConnect*   connect_;
  rcDisc*      disc_;
  rcLoad*      load_;
  rcConfigure* config_;
  rcDownload*  download_;
  rcPrestart*  prestart_;
  rcActivate*  go_;
  rcEnd*       end_;
  rcPause*     pause_;
  rcResume*    resume_;
  rcReset*     reset_;
  rcAbort*     abort_;
  rcAuto*      auto_;
  rcCancel*    cancel_;
  int limitButtons_;
};
#endif

