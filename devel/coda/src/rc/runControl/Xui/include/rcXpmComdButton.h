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
//      Generic RunControl Command Button (Abstract Class)
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcXpmComdButton.h,v $
//   Revision 1.1  1997/07/30 14:33:40  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#ifndef _RC_XPM_COMD_BUTTON_H
#define _RC_XPM_COMD_BUTTON_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <codaXpmpbtnComd.h>
#include <rcClientHandler.h>

class rcHelpMsgWin;
class rcNetStatus;
class rcInfoPanel;
class XcodaErrorDialog;

class rcButtonPanel;

class rcXpmComdButton: public codaXpmpbtnComd
{
public:
  // destructor
  virtual ~rcXpmComdButton (void);

  // class create real widget
  void           init       (void);
  const   Widget baseWidget (void);

  // manage and unmanage
  void           unmanage   (void);
  void           manage     (void);

  // set up help message window
  void           helpMsgWin   (rcHelpMsgWin* win);
  // set up network communication status pointer
  void           netStatusWin (rcNetStatus* stWin);
  // setup  runInfoPanel Window
  void           infoPanel    (rcInfoPanel* ip);

  // return network client handler
  rcClientHandler& clientHandler (void) const;

  // return dialog base widget
  const    Widget dialogBaseWidget (void);

  // class name
  virtual const char* className (void) const {return "rcXpmComdButton";}

  // constructor
  rcXpmComdButton (Widget parent, char* name, char **xpm, char* msg,
		rcButtonPanel* panel,
		rcClientHandler& handler);

protected:
  // enter and leaving window event handler
  static void crossEventHandler (Widget w, XtPointer, XEvent*, Boolean);

  // error reporting mechanism
  virtual void reportErrorMsg (char* msg);

  // net work handler
  rcClientHandler& netHandler_;
  // network status window
  rcNetStatus* stWin_;
  // run information panel
  rcInfoPanel*     infoPanel_;
  // button panel
  rcButtonPanel*   bpanel_;

private:
  Widget parent_;
  Widget w_;
  // error dialog for all buttons
  static XcodaErrorDialog* errDialog_;
  // message for help message
  char*         helpMsg_;
  // help msg window
  rcHelpMsgWin* msgWin_;
};
#endif

  

  
  
  
