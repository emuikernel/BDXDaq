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
//      Implementation of rcXpmComdButton Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcXpmComdButton.cc,v $
//   Revision 1.1  1997/07/30 14:33:40  heyes
//   add more xpm support
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <rcHelpMsgWin.h>
#include <rcNetStatus.h>
#include <rcInfoPanel.h>
#include <codaComdXInterface.h>
#include <XcodaErrorDialog.h>
#include <rcButtonPanel.h>
#include "rcXpmComdButton.h"

// initialize error dialog
XcodaErrorDialog* rcXpmComdButton::errDialog_ = 0;

rcXpmComdButton::rcXpmComdButton (Widget parent, char* name, char **xpm, char* msg,
			    rcButtonPanel* panel,
			    rcClientHandler& handler)
:codaXpmpbtnComd (name, xpm, 1), parent_ (parent), bpanel_ (panel), 
 netHandler_ (handler),
 msgWin_ (0), infoPanel_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcXpmComdButton Class Object\n");
#endif
  helpMsg_ = new char[::strlen (msg) + 1];
  ::strcpy (helpMsg_, msg);
}

rcXpmComdButton::~rcXpmComdButton (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcXpmComdButton Class Object\n");
#endif
  delete []helpMsg_;
}

void
rcXpmComdButton::init (void)
{
  Arg arg[10];
  int ac = 0;

  codaComdXInterface *btn;
  
  createXInterface (parent_);
  btn = getXInterface ();
  w_ = btn->baseWidget ();
  XtAddEventHandler (w_, EnterWindowMask | LeaveWindowMask, FALSE, 
		     (XtEventHandler)&(rcXpmComdButton::crossEventHandler),
		     (XtPointer)this);

  // setup X window resources
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetValues (w_, arg, ac); 
  ac = 0;
}

const Widget
rcXpmComdButton::baseWidget (void)
{
  return w_;
}

rcClientHandler&
rcXpmComdButton::clientHandler (void) const
{
  return netHandler_;
}

void
rcXpmComdButton::manage (void)
{
  codaComdXInterface *btn;
  btn = getXInterface ();
  btn->manage ();  
}

void
rcXpmComdButton::unmanage (void)
{
  codaComdXInterface *btn;
  btn = getXInterface ();
  btn->unmanage ();
}

void
rcXpmComdButton::helpMsgWin (rcHelpMsgWin* win)
{
  msgWin_ = win;
}

void
rcXpmComdButton::netStatusWin (rcNetStatus* win)
{
  stWin_ = win;
}

void
rcXpmComdButton::infoPanel (rcInfoPanel* p)
{
  infoPanel_ = p;
}

void
rcXpmComdButton::crossEventHandler (Widget, XtPointer clientData,
				 XEvent* event, Boolean)
{
  rcXpmComdButton* obj = (rcXpmComdButton *)clientData;
  XCrossingEvent* cev = (XCrossingEvent *)event;

  if (obj->msgWin_) {
    if (cev->type == EnterNotify) 
      obj->msgWin_->setMessage (obj->helpMsg_);
    else
      obj->msgWin_->eraseMessage ();
  }
}

void
rcXpmComdButton::reportErrorMsg (char* msg)
{
  assert (infoPanel_);
  if (rcXpmComdButton::errDialog_ == 0) {
    rcXpmComdButton::errDialog_ = new XcodaErrorDialog (infoPanel_->baseWidget(),
						     "comdError",
						     "Error Dialog");
    rcXpmComdButton::errDialog_->init ();
  }
  rcXpmComdButton::errDialog_->setMessage (msg);
  rcXpmComdButton::errDialog_->popup ();
}

const Widget
rcXpmComdButton::dialogBaseWidget (void)
{
  assert (infoPanel_);
  return infoPanel_->baseWidget ();
}


