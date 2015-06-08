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
//      CODA RunControl Top Level Menu Window
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMenuWindow.cc,v $
//   Revision 1.24  1998/11/06 20:07:04  timmer
//   Linux port wchar_t stuff
//
//   Revision 1.23  1998/09/15 14:14:20  heyes
//   fix bus error on exit problem
//
//   Revision 1.22  1998/08/25 18:24:00  rwm
//   Appease the great compiler.
//
//   Revision 1.21  1998/06/18 12:20:38  heyes
//   new GUI ready I think
//
//   Revision 1.20  1998/05/28 17:46:58  heyes
//   new GUI look
//
//   Revision 1.19  1998/04/08 18:31:23  heyes
//   new look and feel GUI
//
//   Revision 1.18  1997/10/23 13:00:56  heyes
//   fix Alt/W
//
//   Revision 1.17  1997/10/15 16:08:29  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.16  1997/09/05 12:03:53  heyes
//   almost final
//
//   Revision 1.15  1997/08/01 18:38:15  heyes
//   nobody will believe this!
//
//   Revision 1.14  1997/07/30 15:32:28  heyes
//   clean for Solaris
//
//   Revision 1.13  1997/07/30 14:32:52  heyes
//   add more xpm support
//
//   Revision 1.12  1997/07/22 19:39:00  heyes
//   cleaned up lots of things
//
//   Revision 1.11  1997/07/18 16:54:46  heyes
//   new GUI
//
//   Revision 1.10  1997/07/08 15:22:57  heyes
//   deep trouble
//
//   Revision 1.9  1997/07/08 14:59:12  heyes
//   deep trouble
//
//   Revision 1.8  1997/06/16 12:26:47  heyes
//   add dbedit and so on
//
//   Revision 1.7  1997/06/06 18:51:27  heyes
//   new RC
//
//   Revision 1.6  1997/05/26 12:27:47  heyes
//   embed tk in RC GUI
//
//   Revision 1.5  1996/12/04 18:32:30  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.4  1996/11/04 16:14:55  chen
//   add options for monitoring components
//
//   Revision 1.3  1996/10/31 15:57:29  chen
//   Add server message to database option
//
//   Revision 1.2  1996/10/14 20:13:38  chen
//   add display server messages preference
//
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/PanedW.h>
#include <Xm/RowColumn.h>

#include <codaComd.h>
#include <codaComdXInterface.h>
#include <codaComdList.h>
#include <codaSepComd.h>
#include <XcodaMenuBar.h>
#include <XcodaXpmLabel.h>

#include <rcButtonPanel.h>
#include <rcInfoPanel.h>
#include <rcMsgWindow.h>
#include <rcHelpMsgWin.h>
#include <rcNetStatus.h>
#include <rcMastership.h>
#include <rcComdOption.h>
#include <XmTabs.h>
#include <RCLogo.xpm>
#ifdef USE_TK
#include <tcl.h>
#include <tk.h>
#endif
#ifdef USE_CREG
#include <codaRegistry.h>
#endif
#include "rcExit.h"
#include "rcClose.h"
#include "rcButtonFeedBack.h"
#include "rcDisplayMsg.h"
#include "rcOnLine.h"
#include "rcAnaLogDialog.h"
#include "rcUpdateIntervalDialog.h"

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
#include "rcAudioOption.h"
#include "rcWidthOption.h"
#include "rcRcsMsgToDbase.h"
#include "rcTokenIButton.h"
#endif

#include "rcCompBootDialog.h"
#include "rcMonitorParmsDialog.h"
#include "rcZoomButton.h"
//#include "rcRateGraphButton.h"
#include "rcHelpOverview.h"
#include "rcHelpAbout.h"
#include <XcodaErrorDialog.h>

#include "rcMenuWindow.h"

#ifdef NEED_BZERO
extern "C" {
  void bzero(void *,int);
}

#endif

static int pids_[200]; // store process ID
rcHelpMsgWin*  helpWindow;

rcMenuWindow::rcMenuWindow (Widget parent, 
			    char* name,
			    rcClientHandler& handler)
:XcodaMenuWindow (parent, name), netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcMenuWindow Class Object\n");
#endif

  // register this panel to net handler
  netHandler_.addPanel (this);
  exit_ = 0;
  close_ = 0;
  feedBack_ = 0;
  dispMsg_ = 0;
  online_ = 0;
  anaLogButton_ = 0;
  updateInterval_ = 0;
  bootButton_ = 0;
  monParmsButton_ = 0;
  zoomButton_ = 0;
  netStatus_ = 0;
  rcipanel_ = 0;
  helpMsgWin_ = 0;
  msgw_ = 0;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  audio_ = 0;
  Owidth_ = 0;
  serverMsgToDbase_ = 0;
  tokenIButton_ = 0;
#endif
  ::bzero(pids_,sizeof(pids_));
  setpgid(getpid(),getpid());
}

rcMenuWindow::~rcMenuWindow (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcMenuWindow Class Object\n");
#endif
  // remove this panel from nethandler
  netHandler_.removePanel (this);

//   delete exit_;
//   delete close_;
//   delete feedBack_;
//   delete dispMsg_;
//	 delete online_;
//   delete anaLogButton_;
//   delete updateInterval_;
//   delete bootButton_;
//   delete monParmsButton_;
//   delete zoomButton_;
//   delete helpOverview_;
//   delete helpAbout_;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  delete audio_;
  delete Owidth_;
  delete serverMsgToDbase_;
  //delete tokenIButton_;
#endif

  // delete network status updater
  if (netStatus_ != 0)
    delete netStatus_;
}

void
rcMenuWindow::createMenuPanes (void)
{
  codaComdList* list;
  rcComdOption* option = rcComdOption::option ();

  // file menu
  close_ = new rcClose ("Close", 1, "Alt<Key>c", "Alt/C", netHandler_);
  exit_ = new rcExit ("Exit", 1, "Alt<Key>q", "Alt/Q", netHandler_);

  list = new codaComdList ();
  list->add (close_);
  list->add (exit_);

  MenuBar->addRegMenuEntry (list, "File", 'F');
  delete list;

  // preference menu
  feedBack_ = new rcButtonFeedBack ("Button feedback", 1, "Alt<Key>b",
				    "Alt/B", 1, netHandler_);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  if (option->audio ())
    audio_ = new rcAudioOption ("Audio messages", 1, "Alt<Key>a",
				"Alt/A", 1, netHandler_);
  else
    audio_ = new rcAudioOption ("Audio messages", 1, "Alt<Key>a",
				"Alt/A", 0, netHandler_);
#endif
  Owidth_ = new rcWidthOption ("Toggle width", 1, "Alt<Key>w",
			      "Alt/W", 0, netHandler_);
  if (option->reportMsg ())
    dispMsg_ = new rcDisplayMsg ("Show server messages", 1, "Alt<Key>s",
				 "Alt/S", 1, netHandler_);
  else
    dispMsg_ = new rcDisplayMsg ("Show server messages", 1, "Alt<Key>s",
				 "Alt/S", 0, netHandler_);

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  serverMsgToDbase_ = new rcRcsMsgToDbase ("Log Server Message", 0,
					   "Alt<Key>m", "Alt/M", 0,
					   netHandler_);
#endif

  online_ = new rcOnLine ("Online", 0, "Alt<Key>l",
			  "Alt/B", 1, netHandler_);
  list = new codaComdList ();
  list->add (feedBack_);
  list->add (dispMsg_);
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  list->add (audio_);
  list->add (serverMsgToDbase_);
#endif
  list->add (Owidth_);
  list->add (online_);
  MenuBar->addRegMenuEntry (list, "Preference", 'P');
  delete list;

  // before return setup pointers of dynamic panels to related command
  // buttons
  exit_->netStatusPanel (netStatus_);
  exit_->infoPanel      (rcipanel_);
  // apply the same technique to close command
  close_->netStatusPanel (netStatus_);
  close_->infoPanel      (rcipanel_);

  // attach help message window to feedback command
  feedBack_->helpMsgWindow (helpMsgWin_);

  // attach message window to display message command
  dispMsg_->msgWindow (msgw_);
}

extern int root_height;

void
rcMenuWindow::handle_tab (Widget w, XtPointer data, XtPointer calldata)
{
  Arg arg[20];
  int ac = 0;
  int state, i, j, tab;
  char* tmp;
  rcMenuWindow *self = (rcMenuWindow *) data;
  /* get tab data, start on tab 0 */
  static int curr = 0;

  if ((int) calldata == 0)
    return;

  if (self->tabChildren_[curr]) {
    XtUnmanageChild(self->tabChildren_[curr]);
  }
  curr += (int)calldata;
  tab = curr;

  if (self->tabChildren_[curr]) {
    XtManageChild(self->tabChildren_[curr]);
  }
  ac = 0;
  XtSetArg (arg[ac], XtNlefttabs, tab); ac++;
  XtSetArg (arg[ac], XtNrighttabs, self->numTabs_ - tab); 
  ac++;
  XtSetValues (self->rtab_, arg, ac);
}

static void childHandler(int sig)
{
  int status,pid;
  pid = wait(&status);
  printf("process %d exit with status %d (core dump %d)\n",pid,WEXITSTATUS(status),WCOREDUMP(status));
}

void 
rcMenuWindow::destroyHandler(Widget w,void *data,XEvent *eventPtr,Boolean *b)
{
  rcMenuWindow *self = (rcMenuWindow *) data;
  char temp2[100];

  if (eventPtr->type == DestroyNotify) {
    int ix;
    /*    for (ix=0;ix<30;ix++) {
      if (self->tabChildren_[ix] == w) {
	printf("program \"%s\" has unexpectedly quit\n", self->tabLabels_[ix]);
	
	if (strcmp(self->tabLabels_[ix],"cedit") == 0) {
	  sprintf (temp2,"(echo \"start cedit\"; sleep 1; %s/codaedit )&",getenv("CODA_BIN"));
	  system(temp2);
    	}

	if (strcmp(self->tabLabels_[ix],"dbedit") == 0) {
	  sprintf (temp2,"(echo \"start dbedit\";sleep 1; %s/common/scripts/dbedit )&",getenv("CODA"));
	  system(temp2);
    	}

	if (strcmp(self->tabLabels_[ix],"ddmon") == 0) {
	  sprintf (temp2,"(echo \"start ddmon\";sleep 1;  export DD_NAME; DD_NAME=%s;$CODA_BIN/ddmon )&",self->netHandler_.exptname());
	  system(temp2);
	}
      }
      }
    */
  }
}

void
rcMenuWindow::crossEventHandler (Widget, XtPointer clientData,
				 XEvent* event, Boolean)
{
  rcMenuWindow* obj = (rcMenuWindow *)clientData;
  XCrossingEvent* cev = (XCrossingEvent *)event;

  if (obj->helpMsgWin_) {
    if (cev->type == EnterNotify) 
      obj->helpMsgWin_->setMessage ("press a button to change page");
    else
      obj->helpMsgWin_->eraseMessage ();
  }
}

Widget 
rcMenuWindow::createTabFrame (char *name,int pid)
{
  Widget widget;
  Arg arg[20];
  int ltabs,rtabs,ac = 0;

  numTabs_++;

  tabLabels_[numTabs_] = strdup(name);
  
  pids_ [numTabs_] = pid;
  
  ac = 0;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  widget = tabChildren_[numTabs_] = XtCreateManagedWidget ("tabChild", xmFrameWidgetClass, rframe_,
			      arg, ac);

  XtAddEventHandler(widget,SubstructureNotifyMask, False, destroyHandler, (XtPointer) this);
  char tmp[200];
  sprintf(tmp,"%s_WINDOW",name);

#ifdef USE_CREG
  CODASetAppName (XtDisplay(tabChildren_[numTabs_]),XtWindow(tabChildren_[numTabs_]),tmp);
#endif
  XStoreName(XtDisplay(tabChildren_[numTabs_]),XtWindow(tabChildren_[numTabs_]),tmp);

  ac = 0;
  XtSetArg (arg[ac], XtNlefttabs, &ltabs);ac++;
  XtSetArg (arg[ac], XtNrighttabs, &rtabs);ac++;
  XtGetValues (rtab_, arg, ac);
  ac = 0;
  XtSetArg (arg[ac], XtNrighttabs, numTabs_ - ltabs);ac++;
  XtSetArg (arg[ac], XtNlabels, tabLabels_); ac++;
  XtSetValues (rtab_, arg, ac);
  XtUnmanageChild (tabChildren_[numTabs_]);
  XtUnmapWidget(rtab_);
  XSync(XtDisplay(rtab_),False);
  XtMapWidget(rtab_);
  return widget;
}

Widget
rcMenuWindow::createMenuWindow (Widget parent)
{
  Arg arg[20];
  int ac = 0;

  rcComdOption* option = rcComdOption::option ();
  bootall_ = 0;
  // create all widgets

  XtSetValues (MenuBar->baseWidget (), arg, ac);
  ac = 0;
  XtSetArg (arg[ac], XtNheight, HeightOfScreen(XtScreen(parent))); ac++;
  XtSetArg (arg[ac], XtNwidth, WidthOfScreen(XtScreen(parent))); ac++;
  XtSetValues (parent, arg, ac);
  ac = 0;
  Widget formouter = XtCreateManagedWidget ("rcFormOuter", xmFormWidgetClass, parent, 
  				     arg, ac);
  ac = 0;
  XtSetArg (arg[ac], XtNwidth, 512); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNresizable, FALSE); ac++;
  Widget lform = XtCreateManagedWidget ("rcLform", xmFormWidgetClass, formouter, 
  				     arg, ac);

  int   rwid = WidthOfScreen(XtScreen(parent)) - 480;

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, lform); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  //XtSetArg (arg[ac], XtNwidth, rwid); ac++;
  rform_ = XtCreateManagedWidget ("rcform", xmFormWidgetClass, formouter,
				 arg, ac);

  numTabs_ = 0;

  tabLabels_[0] = strdup("help");

  ac = 0;
  XtSetArg (arg[ac], XtNlabels, tabLabels_); ac++;
  XtSetArg (arg[ac], XtNtabWidthPercentage, 0); ac++;
  XtSetArg (arg[ac], XtNlefttabs, 0); ac++;
  XtSetArg (arg[ac], XtNrighttabs, 0); ac++;
  XtSetArg (arg[ac], XtNorientation, XfwfUpTabs); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 5); ac++;
  XtSetArg (arg[ac], XmNheight, 30); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  //XtSetArg (arg[ac], XtNwidth, rwid); ac++;
  rtab_ = XtCreateManagedWidget ("scriptTab", xmTabsWidgetClass,
					rform_, arg, ac);
  /* add all callbacks */
  XtAddCallback (rtab_, XtNactivateCallback, handle_tab, 
  		 (XtPointer)this);
  XtAddEventHandler (rtab_, EnterWindowMask | LeaveWindowMask, FALSE, 
		     (XtEventHandler)&(crossEventHandler),
		     (XtPointer)this);

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, rtab_); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  rframe_ = XtCreateWidget ("rcframe", xmFormWidgetClass, rform_,
			      arg, ac);

  // create button panel first  
  rcButtonPanel* bpanel = new rcButtonPanel (lform, "buttonPanel", netHandler_);

  bpanel->init ();

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  //XtSetArg (arg[ac], XtNresize, FALSE); ac++;
  XtSetValues (bpanel->baseWidget(), arg, ac);

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, bpanel->baseWidget()); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XtNwidth, 480); ac++;
  Widget pw = XtCreateWidget ("rcpane", xmPanedWindowWidgetClass, lform,
			      arg, ac);
  ac = 0;




  /* sergey: seems ERROR here: was
  XtSetArg (arg[ac], XmNpaneMinimum, 480); ac++;
  XtSetArg (arg[ac], XmNpaneMaximum, 480); ac++;
   */
  XtSetArg (arg[ac], XmNpaneMinimum, 100); ac++;
  XtSetArg (arg[ac], XmNpaneMaximum, 900); ac++;






  Widget form = XtCreateWidget ("rcForm", xmFormWidgetClass, pw, arg, ac);
  ac = 0;

  Widget form1 = XtCreateWidget ("rcForm1", xmFormWidgetClass, pw, 
				 arg, ac);
  ac = 0;

  // create bottom form
  Widget bform = XtCreateWidget ("bform", xmFormWidgetClass, form1, NULL, 0);

  // information panel
  rcipanel_ = new rcInfoPanel (form, "infoPanel", netHandler_);
  rcipanel_->init ();

  // message window panel
  msgw_ = new rcMsgWindow (form1, "msgWindow", netHandler_);
  msgw_->init ();

  // help msg window
  helpMsgWin_ = new rcHelpMsgWin (bform, "helpMsgWindow");
  helpMsgWin_->init ();
  
  // net status
  netStatus_ = new rcNetStatus (bform, "netstatus", 150, 10);
  // mastership button
  rcMastership* master = new rcMastership (bform, 
					   "mastership",
					   netHandler_);
  master->init ();
  // create rcLog Image
  XcodaXpmLabel *log = new XcodaXpmLabel (bform, "      ", RCLogo_xpm);
  log->init ();

  // set help msg window pointer to button panel
  bpanel->helpMsgWin (helpMsgWin_);
  // set network status window to button panel
  bpanel->netStatusWin (netStatus_);
  // set run information panel to button panel
  bpanel->infoPanel (rcipanel_);

  // set x resouces for bottom form first
  ac = 0;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetValues (master->baseWidget (), arg, ac);
  ac = 0;

  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;

  // I know the size of pixmap inside master widget

  ac = 0;

  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetValues (log->baseWidget (), arg, ac);

  ac = 0;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 3); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;  
  XtSetArg (arg[ac], XmNrightOffset, 3); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetValues (bform, arg, ac);
  
  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 3); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, log->baseWidget () ); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 60); ac++;
  XtSetValues (helpMsgWin_->baseWidget (), arg, ac);
  
  ac = 0;
  // set all X resources
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 3); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 3); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  //XtSetArg (arg[ac], XmNrightOffset, 3); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;

  XtSetValues (rcipanel_->baseWidget(), arg, ac);

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 3); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 3); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, bform); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  XtSetValues (msgw_->baseWidget(), arg, ac);
  ac = 0;

  XtManageChild (formouter);
  XtManageChild (lform);
  XtManageChild (rform_);
  XtManageChild (rtab_);
  XtManageChild (bform);
  XtManageChild (form);
  XtManageChild (form1);
  XtManageChild (pw);
  XtManageChild (rframe_);

  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  tabChildren_[0] = XtCreateManagedWidget ("HelpWidget", xmFormWidgetClass, rframe_,
			      arg, ac);

  
  XtManageChild (tabChildren_[0]);

  rcipanel_->popupRateDisplay (this);

  anaLogButton_ = new rcAnaLogDialog (this, "Ana Log Dialog", 
				      "Logging File Dialog",
				      netHandler_);
  
  updateInterval_ = new rcUpdateIntervalDialog ("updateI","Update Interval", netHandler_); 
  
  bootButton_ = new rcCompBootDialog ("bootC","boot components", netHandler_);

  
  monParmsButton_ = new rcMonitorParmsDialog ("compM","Monitor components", netHandler_);
					      
  bootButton_->init();
  bootButton_->popup();

  updateInterval_->init();
  updateInterval_->popup();

  monParmsButton_->init();
  monParmsButton_->popup();

  zoomButton_ = new rcZoomButton ("Zoom on event information", 0, 
				  "Alt<Key>z",
				  "Alt/Z", netHandler_);
  
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  tokenIButton_ = new rcTokenIButton ("Token interval value", 0,
				      "Alt<Key>t", "Alt/T", netHandler_);
#endif
  
  
  anaLogButton_->init();
  anaLogButton_->popup();
  // return widget
  return formouter;
}

void
rcMenuWindow::config (int status)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  //  if (status >= DA_CONFIGURED) {
  //    anaLogButton_->activate ();
    
  //}
  //else
  //  anaLogButton_->deactivate ();
#else
  if (status == DA_DOWNLOADED) {
    anaLogButton_->activate ();
  }
  else
    anaLogButton_->deactivate ();
#endif


  if (status != DA_NOT_CONNECTED) {
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
    serverMsgToDbase_->activate ();
#endif
  }
  else {
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
    serverMsgToDbase_->deactivate ();
#endif
  }

  if (status >= DA_CONFIGURED) {
    //    zoomButton_->activate ();
  }
  else {
    //    zoomButton_->deactivate ();
  }

  if (status >= DA_DOWNLOADED)
    online_->activate ();
  else
    online_->deactivate ();  
}

void
rcMenuWindow::configOnlineOption (int online)
{
  if (online)
    online_->setState (1);
  else
    online_->setState (0);
}

void
rcMenuWindow::configUpdateInterval (int interval)
{
  updateInterval_->setUpdateInterval (interval);
}

void
rcMenuWindow::configBoot ()
{
  bootButton_->popdown();
  bootButton_->popup();
}

void
rcMenuWindow::configMonParms ()
{
  monParmsButton_->popdown();
  monParmsButton_->popup();
}

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
void
rcMenuWindow::configTokenInterval (int interval)
{
  tokenIButton_->setTokenInterval (interval);
}

void
rcMenuWindow::configRcsMsgToDbase (int state)
{
  serverMsgToDbase_->setState (state);
}
#endif

const Widget
rcMenuWindow::dialogBaseWidget (void)
{
  assert (rcipanel_);
  return rcipanel_->baseWidget ();
}

void
rcMenuWindow::reportErrorMsg (char* msg)
{
  if (rcMenuWindow::errDialog_ == 0) {
    rcMenuWindow::errDialog_ = new XcodaErrorDialog (dialogBaseWidget(),
                                                   "comdError",
                                                   "Error Dialog");
    rcMenuWindow::errDialog_->init ();
  }
  rcMenuWindow::errDialog_->setMessage (msg);
  rcMenuWindow::errDialog_->popup ();
}
