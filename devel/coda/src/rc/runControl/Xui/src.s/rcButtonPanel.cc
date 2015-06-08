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
//      Implementation of RunControl Button Panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcButtonPanel.cc,v $
//   Revision 1.17  2000/01/20 16:06:52  rwm
//   Pause button only appears if LIMIT_BUTTON[2] env vars not set.
//
//   Revision 1.16  1999/07/28 19:54:44  rwm
//   Added pause button (script) for Hall-B
//
//   Revision 1.15  1998/09/01 18:48:36  heyes
//   satisfy Randy's lust for command line options
//
//   Revision 1.14  1998/06/18 12:20:31  heyes
//   new GUI ready I think
//
//   Revision 1.13  1998/05/28 17:46:51  heyes
//   new GUI look
//
//   Revision 1.12  1998/04/08 18:31:12  heyes
//   new look and feel GUI
//
//   Revision 1.11  1997/12/04 14:05:49  heyes
//   stuff for Dieter, eye candy!
//
//   Revision 1.10  1997/10/23 13:00:54  heyes
//   fix Alt/W
//
//   Revision 1.9  1997/10/15 16:08:23  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.8  1997/09/11 16:07:29  heyes
//   fix connect button
//
//   Revision 1.7  1997/09/05 12:03:49  heyes
//   almost final
//
//   Revision 1.6  1997/08/01 18:38:05  heyes
//   nobody will believe this!
//
//   Revision 1.5  1997/07/30 15:32:26  heyes
//   clean for Solaris
//
//   Revision 1.4  1997/07/30 14:32:44  heyes
//   add more xpm support
//
//   Revision 1.3  1997/06/16 12:26:43  heyes
//   add dbedit and so on
//
//   Revision 1.2  1997/06/06 18:51:21  heyes
//   new RC
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>

#include <rcHelpMsgWin.h>
#include <rcNetStatus.h>
#include <rcConnect.h>
#include <rcDisc.h>
#include <rcLoad.h>
#include <rcConfigure.h>
#include <rcDownload.h>
#include <rcPrestart.h>
#include <rcActivate.h>
#include <rcEnd.h>
#include <rcPause.h>
#include <rcResume.h>
#include <rcReset.h>
#include <rcAbort.h>
#include <rcAuto.h>
#include <rcCancel.h>

#include "rcButtonPanel.h"

rcButtonPanel::rcButtonPanel (Widget parent,
			      char* name,
			      rcClientHandler& handler)
:XcodaUi (name), rcPanel (), parent_ (parent), netHandler_ (handler),
 connect_ (0), disc_ (0), load_ (0), config_ (0), download_ (0),
 prestart_ (0), go_ (0), end_ (0), pause_ (0), resume_(0), reset_ (0), 
 abort_ (0), auto_ (0), cancel_ (0), limitButtons_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Create rcButtonPanel Class Object\n");
#endif
  netHandler_.addPanel (this);
}

rcButtonPanel::~rcButtonPanel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Delete rcButtonPanel Class Object\n");
#endif
  // remove all commands
  delete disc_;
  delete reset_;
  delete cancel_;

  delete connect_;
  delete config_;
  delete load_;
  delete download_;
  delete prestart_;
  delete go_;
  delete end_;
  delete pause_;
  delete resume_;
  delete abort_;
  delete auto_;


  // remove this panel from net handler
  netHandler_.removePanel (this);
}
extern "C" Pixmap XcodaCreatePixmapFromXpm(Widget,char**,int);
//extern "C" void *Editor_save_callback;
//extern "C" void *Editor_rc_object;

void
rcButtonPanel::init (void)
{
  Arg arg[20];
  int ac = 0;

  ac = 0; 

  ac = 0;
  // create outer frame to hold two row column widgets
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XtNwidth, 512); ac++;
  
  _w = XtCreateManagedWidget ("buttonFrame", xmFrameWidgetClass, parent_,
			      arg, ac);

  // create frame title
  
  ac = 0;
  XmString t = XmStringCreateSimple ("Run control Buttons");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget rnplabel = XtCreateManagedWidget ("rcButtonFrameLabel",
					 xmLabelWidgetClass,
					 _w, arg, ac);
					 
  XmStringFree (t);

  ac = 0;
  XtSetArg (arg[ac], XmNshadowThickness, 0); ac++;
  XtSetArg (arg[ac], XmNfractionBase, 100); ac++;
  Widget buttonForm = XtCreateManagedWidget ("rcButtonForm", xmFormWidgetClass, _w, arg, ac);

  // left button frame to hold left 
  
    ac = 0;
  // create outer frame for left
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 10); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  
  Widget leftFrame = XtCreateManagedWidget ("RbuttonFrame", xmFrameWidgetClass, buttonForm,
			      arg, ac);

  // create frame title
  
  ac = 0;
  t = XmStringCreateSimple ("Control");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget leftlabel = XtCreateManagedWidget ("rcLeftFrameLabel",
					 xmLabelWidgetClass,
					 leftFrame, arg, ac);
					 
  XmStringFree (t);

  // create RC widget the left one holds one column of buttons
  ac = 0;
  XtSetArg (arg[ac], XmNorientation, XmVERTICAL); ac++;
  XtSetArg (arg[ac], XmNnumColumns, 1); ac++;
  XtSetArg (arg[ac], XmNentryBorder, 2); ac++;
  XtSetArg (arg[ac], XmNpacking, XmPACK_TIGHT); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_CENTER); ac++;
  XtSetArg (arg[ac], XmNresizable, FALSE); ac++;

  lw_ = XtCreateManagedWidget ("buttonLRowCol", xmRowColumnWidgetClass,
			leftFrame, arg, ac );

  // handle destroy callback

  installDestroyHandler ();

  // create outer frame for right buttons 
  ac = 0;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, leftFrame); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 20); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  //  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  
  Widget rightFrame = XtCreateManagedWidget ("RbuttonFrame", xmFrameWidgetClass, buttonForm,
			      arg, ac);

  // create frame title
  
  ac = 0;
  t = XmStringCreateSimple ("Transition");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget rightlabel = XtCreateManagedWidget ("rcRightFrameLabel",
					 xmLabelWidgetClass,
					 rightFrame, arg, ac);
					 
  XmStringFree (t);

  // Right button widget this is a horizontal row of up to three buttons
  
  ac = 0;

  XtSetArg (arg[ac], XmNorientation, XmHORIZONTAL); ac++;
  XtSetArg (arg[ac], XmNnumColumns, 3); ac++;
  XtSetArg (arg[ac], XmNpacking, XmPACK_TIGHT); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_CENTER); ac++;
  XtSetArg (arg[ac], XmNentryBorder, 5); ac++;
  XtSetArg (arg[ac], XtNheight, 82); ac++;
  XtSetArg (arg[ac], XtNwidth, 321); ac++;

  rw_ = XtCreateManagedWidget ("buttonRRowCol", xmRowColumnWidgetClass,
  			rightFrame, arg, ac );
  ac = 0;

  XtManageChild (rw_);

  // buttons inside left (vertical) row column widget
  connect_ = new rcConnect (lw_, this, netHandler_);
  disc_ = new rcDisc (lw_, this, netHandler_);
  reset_ = new rcReset (lw_, this, netHandler_);
  cancel_ = new rcCancel (lw_, this, netHandler_);

  // button inside right (horizontal) row column widget
  load_ = new rcLoad (rw_, this, netHandler_);
  config_ = new rcConfigure (rw_, this, netHandler_);
  abort_ = new rcAbort (rw_, this, netHandler_);

  download_ = new rcDownload (rw_, this, netHandler_);
  prestart_ = new rcPrestart (rw_, this, netHandler_);
  go_ = new rcActivate (rw_, this, netHandler_);
  end_ = new rcEnd (rw_, this, netHandler_);

  pause_ = new rcPause (rw_, this, netHandler_);
  resume_ = new rcResume (rw_, this, netHandler_);

  auto_ = new rcAuto (rw_, this, netHandler_);

  cancel_->init ();
  reset_->init ();
  disc_->init ();
  connect_->init ();

  ac = 0;
  XtSetArg (arg[ac], XmNaccelerator,"Alt<Key>n");
  XtSetValues(connect_->baseWidget(), arg, ac);
  load_->init ();
  config_->init ();
  download_->init ();
  prestart_->init ();
  go_->init ();
  end_->init ();
  pause_->init ();
  resume_->init ();
  abort_->init ();
  auto_->init ();
  
  manage ();
  
  //Editor_rc_object = (void *) config_;
  //Editor_save_callback = (void *)doitCbk;

  config (netHandler_.status ());
  if (getenv("LIMIT_BUTTONS")) {
    limitButtons_ = 1;
  }
  if (getenv("LIMIT_BUTTONS2")) {
    limitButtons_ = 2;
  }
}

void
rcButtonPanel::config (int status)
{
  static int hackIsActive = 0;

  if (status == DA_NOT_CONNECTED) {
    notConnected ();
  }
  else {
    connected ();
    switch (status) {
    case DA_DORMANT:
      // printf("    case DA_DORMANT:\n");
      loadAndConfigure ();
      break;
    case DA_BOOTED:
      // printf("    case DA_BOOTED:\n");
    case DA_CONFIGURED:
      // printf("    case DA_CONFIGURED:\n");
      download ();
      break;
    case DA_DOWNLOADED:
      // printf("    case DA_DOWNLOADED:\n");
      hackIsActive = 0;
      prestart ();
      break;
    case DA_PAUSED:
      // printf("    case DA_PAUSED:\n");
      if (hackIsActive) {
	resumeAndEnd ();
	// For some reason, the code called twice per transition
	// so set hackIsActive to 2 below and decrement twice.
	hackIsActive--;
	//hackIsActive = 0;
      } else {
	// printf("RWM:called when: IsActive = %d.", hackIsActive);
	hackIsActive = 0;
	goAndEnd ();
      }
      break;
    case DA_ACTIVE:
      // printf("    case DA_ACTIVE:\n");
      hackIsActive = 2;
      endAndPause ();
      break;
    default:
      // printf("    default.\n");
      break;
    }
  }
}

void
rcButtonPanel::configMastership (int type)
{
  if (type == CODA_LOCKED) {
    deactivateTransitionPanel ();
    reset_->deactivate ();
    cancel_->unmanage ();
  }
  else {
    activateTransitionPanel ();
    reset_->activate ();
    cancel_->unmanage ();
  }
}

void
rcButtonPanel::helpMsgWin (rcHelpMsgWin* win)
{
  disc_->helpMsgWin (win);
  reset_->helpMsgWin (win);
  cancel_->helpMsgWin (win);

  connect_->helpMsgWin (win);
  load_->helpMsgWin (win);
  config_->helpMsgWin (win);
  download_->helpMsgWin (win);
  prestart_->helpMsgWin (win);
  go_->helpMsgWin (win);
  end_->helpMsgWin (win);
  pause_->helpMsgWin (win);
  resume_->helpMsgWin (win);
  abort_->helpMsgWin (win);
  auto_->helpMsgWin (win);
}

void
rcButtonPanel::netStatusWin (rcNetStatus* win)
{
  disc_->netStatusWin (win);
  reset_->netStatusWin (win);
  cancel_->netStatusWin (win);

  connect_->netStatusWin (win);
  load_->netStatusWin (win);
  config_->netStatusWin (win);
  download_->netStatusWin (win);
  prestart_->netStatusWin (win);
  go_->netStatusWin (win);
  end_->netStatusWin (win);
  pause_->netStatusWin (win);
  resume_->netStatusWin (win);
  abort_->netStatusWin (win);
  auto_->netStatusWin (win);
}

void
rcButtonPanel::infoPanel (rcInfoPanel* panel)
{
  disc_->infoPanel (panel);
  reset_->infoPanel (panel);
  cancel_->infoPanel (panel);

  connect_->infoPanel (panel);
  load_->infoPanel (panel);
  config_->infoPanel (panel);
  download_->infoPanel (panel);
  prestart_->infoPanel (panel);
  go_->infoPanel (panel);
  end_->infoPanel (panel);
  pause_->infoPanel (panel);
  resume_->infoPanel (panel);
  abort_->infoPanel (panel);
  auto_->infoPanel (panel);
}

void
rcButtonPanel::unmanageButton (rcComdButton* button)
{
  button->unmanage ();
}

void
rcButtonPanel::notConnected (void)
{
  unmanageButton (disc_);
  unmanageButton (reset_);
  unmanageButton (cancel_);
  unmanageButton (load_);
  unmanageButton (config_);
  unmanageButton (download_);
  unmanageButton (prestart_);
  unmanageButton (go_);
  unmanageButton (end_);
  unmanageButton (pause_);
  unmanageButton (resume_);
  unmanageButton (abort_);
  unmanageButton (auto_);

  connect_->manage ();
  connect_->activate ();
}

void
rcButtonPanel::connected (void)
{
  unmanageButton ((class rcComdButton *)connect_);

  disc_->manage ();
  reset_->manage ();
  cancel_->manage ();
}

void
rcButtonPanel::loadAndConfigure (void)
{
  download_->unmanage ();
  prestart_->unmanage ();
  go_->unmanage ();
  end_->unmanage ();
  pause_->unmanage ();
  resume_->unmanage ();
  abort_->unmanage ();
  auto_->unmanage ();
  config_->manage (); 
}

void
rcButtonPanel::download (void)
{
  load_->unmanage ();
  prestart_->unmanage ();
  go_->unmanage ();
  end_->unmanage ();
  pause_->unmanage ();
  resume_->unmanage ();
  abort_->unmanage ();

  if (!(limitButtons_ & 1)) {
    download_->manage ();
  }
  config_->manage ();
  if (!(limitButtons_ & 2))
    auto_->manage ();
}

void
rcButtonPanel::prestart (void)
{
  load_->unmanage ();
  config_->unmanage ();
  download_->unmanage ();
  go_->unmanage ();
  end_->unmanage ();
  pause_->unmanage ();
  resume_->unmanage ();

  if (!(limitButtons_ & 1)) {
    prestart_->manage ();
    abort_->manage ();
  } else {
    config_->manage ();
  }
  if (!(limitButtons_ & 2)) {
    auto_->manage ();
  }
}

void
rcButtonPanel::goAndEnd (void)
{
  // puts("RWM: goAndEnd called.");
  load_->unmanage ();
  config_->unmanage ();
  download_->unmanage ();
  prestart_->unmanage ();
  pause_->unmanage ();
  resume_->unmanage ();
  abort_->unmanage ();

  if (!(limitButtons_ & 1)) {
    go_->manage ();
  }
  end_->manage ();
  if (!(limitButtons_ & 2))
    auto_->manage ();
}  


void
rcButtonPanel::endAndPause (void)
{
  load_->unmanage ();
  config_->unmanage ();
  download_->unmanage ();
  prestart_->unmanage ();
  go_->unmanage ();
  abort_->unmanage ();
  auto_->unmanage ();

  end_->manage ();
  if ((limitButtons_ != 1) &&
      (limitButtons_ != 2) ) {
    resume_->unmanage ();
    pause_->manage ();
  }
}  

void
rcButtonPanel::resumeAndEnd (void)
{
  load_->unmanage ();
  config_->unmanage ();
  download_->unmanage ();
  prestart_->unmanage ();
  go_->unmanage ();
  abort_->unmanage ();
  auto_->unmanage ();

  if ((limitButtons_ != 1) &&
      (limitButtons_ != 2) ) {
    pause_->unmanage ();
    resume_->manage ();
  }
  end_->manage ();
}  


void
rcButtonPanel::deactivateTransitionPanel (void)
{
  reset_->unmanage ();
  disc_->unmanage ();  
  XtSetSensitive (rw_, FALSE);
  disc_->manage ();
  reset_->manage ();
  cancel_->manage ();

}

void
rcButtonPanel::activateTransitionPanel (void)
{
  XtSetSensitive (rw_, TRUE);
  cancel_->manage ();
  reset_->manage ();
  disc_->manage ();
}
