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
//      CODA runControl Menu Bar + Menu Window
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMenuWindow.h,v $
//   Revision 1.14  1998/11/06 20:07:04  timmer
//   Linux port wchar_t stuff
//
//   Revision 1.13  1998/06/18 12:20:39  heyes
//   new GUI ready I think
//
//   Revision 1.12  1998/05/28 17:46:58  heyes
//   new GUI look
//
//   Revision 1.11  1998/04/08 18:31:24  heyes
//   new look and feel GUI
//
//   Revision 1.10  1997/10/15 16:08:30  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.9  1997/09/05 12:03:53  heyes
//   almost final
//
//   Revision 1.8  1997/07/22 19:39:02  heyes
//   cleaned up lots of things
//
//   Revision 1.7  1997/07/18 16:54:48  heyes
//   new GUI
//
//   Revision 1.6  1997/07/08 14:59:13  heyes
//   deep trouble
//
//   Revision 1.5  1996/12/04 18:32:31  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.4  1996/11/04 16:14:56  chen
//   add options for monitoring components
//
//   Revision 1.3  1996/10/31 15:57:30  chen
//   Add server message to database option
//
//   Revision 1.2  1996/10/14 20:13:39  chen
//   add display server messages preference
//
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#ifndef _RC_MENU_WINDOW_H
#define _RC_MENU_WINDOW_H

#include <XcodaMenuWindow.h>
#include <rcClientHandler.h>
#include <rcPanel.h>
#include <XcodaErrorDialog.h>

class rcMenuComd;
class rcExit;
class rcClose;
class rcButtonFeedBack;
class rcDisplayMsg;
class rcOnLine;
class rcAnaLogDialog;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
class rcAudioOption;
class rcTokenIButton;
class rcRcsMsgToDbase;
#endif
class rcWidthOption;
class rcUpdateIntervalDialog;
class rcCompBootDialog;
class rcMonitorParmsDialog;
class rcZoomButton;
class rcRateGraphButton;

class rcNetStatus;
class rcInfoPanel;
class rcHelpMsgWin;
class rcMsgWindow;

class rcHelpOverview;

extern rcHelpMsgWin*  helpWindow;

/* extern */
class rcMenuWindow: public XcodaMenuWindow, public rcPanel
{
public:
  rcMenuWindow          (Widget parent, 
			 char* name, rcClientHandler& handler);
  virtual ~rcMenuWindow (void);

  Widget createTabFrame (char *name,int pid);

  // inherited config function
  void    config (int status);
  
  void    configBoot();
  void    configMonParms();
  // inherited config online option
  void    configOnlineOption (int online);
  
  // inherited config update interval
  void    configUpdateInterval (int interval);
  
  const Widget dialogBaseWidget (void);
  void  reportErrorMsg (char* msg);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  void    configRcsMsgToDbase (int yes);
  void    configTokenInterval (int interval);
#endif
  Widget            rframe_;
  Widget            rform_;
  Widget            rtab_;
  Widget tabChildren_[200];
  XcodaErrorDialog  *errDialog_;
  static void handle_tab (Widget w, XtPointer data, XtPointer calldata);
  static void destroyHandler(Widget w,void *data,XEvent *eventPtr,Boolean *b);
  rcCompBootDialog* bootButton_;
  int bootall_;
  
protected:
  // enter and leaving window event handler
  static void crossEventHandler (Widget w, XtPointer, XEvent*, Boolean);
  virtual Widget createMenuWindow (Widget );
  virtual void   createMenuPanes  (void);
  
private:
  // tab widgets
  char* tabLabels_[200];
  int numTabs_;

  // network handler
  rcClientHandler& netHandler_;
  // file menu
  rcExit*        exit_;
  rcClose*       close_;
  // preference menu
  rcButtonFeedBack* feedBack_;
  rcDisplayMsg*     dispMsg_;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  rcAudioOption*    audio_;
  rcWidthOption*    Owidth_;
  rcRcsMsgToDbase*  serverMsgToDbase_;
#endif
  rcOnLine*         online_;
  // option menu
  rcAnaLogDialog*   anaLogButton_;
  rcUpdateIntervalDialog* updateInterval_;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  rcTokenIButton*   tokenIButton_;
#endif
  rcMonitorParmsDialog* monParmsButton_;
  rcZoomButton*     zoomButton_;
  rcRateGraphButton* rateGraphButton_;
  // help menu
  rcHelpOverview*  helpOverview_;
  rcMenuComd*      helpAbout_;
  // network communication dynamic status
  rcNetStatus*   netStatus_;
  // initial information panel
  rcInfoPanel*   rcipanel_;
  // help message window (button feedback message window)
  rcHelpMsgWin*  helpMsgWin_;
  // server message window
  rcMsgWindow*   msgw_;
};
#endif
