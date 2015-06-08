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
//      Implementation of rcComdButton Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcComdButton.cc,v $
//   Revision 1.3  1998/06/18 12:20:32  heyes
//   new GUI ready I think
//
//   Revision 1.2  1997/08/01 18:38:07  heyes
//   nobody will believe this!
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
XcodaErrorDialog* rcComdButton::errDialog_ = 0;

rcComdButton::rcComdButton (Widget parent, char* name, char* msg,
			    rcButtonPanel* panel,
			    rcClientHandler& handler)
:codaXpmpbtnComd (name, NULL, 1), parent_ (parent), bpanel_ (panel), 
 netHandler_ (handler),
 msgWin_ (0), infoPanel_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcComdButton Class Object\n");
#endif
  helpMsg_ = new char[::strlen (msg) + 1];
  ::strcpy (helpMsg_, msg);
}

rcComdButton::rcComdButton (Widget parent, char* name, char* msg,
			    rcButtonPanel* panel,
			    rcClientHandler& handler,char *symbol)
:codaXpmpbtnComd (name, NULL, 1,symbol), parent_ (parent), bpanel_ (panel), 
 netHandler_ (handler),
 msgWin_ (0), infoPanel_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcComdButton Class Object\n");
#endif
  helpMsg_ = new char[::strlen (msg) + 1];
  ::strcpy (helpMsg_, msg);
}

rcComdButton::~rcComdButton (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcComdButton Class Object\n");
#endif
  delete []helpMsg_;
}

void
rcComdButton::init (void)
{
  Arg arg[10];
  int ac = 0;

  codaComdXInterface *btn;
  
  createXInterface (parent_);
  btn = getXInterface ();
  w_ = btn->baseWidget ();
  XtAddEventHandler (w_, EnterWindowMask | LeaveWindowMask, FALSE, 
		     (XtEventHandler)&(rcComdButton::crossEventHandler),
		     (XtPointer)this);

  // setup X window resources
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetValues (w_, arg, ac); 
  ac = 0;
}

const Widget
rcComdButton::baseWidget (void)
{
  return w_;
}

rcClientHandler&
rcComdButton::clientHandler (void) const
{
  return netHandler_;
}

void
rcComdButton::manage (void)
{
  codaComdXInterface *btn;
  btn = getXInterface ();
  btn->manage ();  
}

void
rcComdButton::unmanage (void)
{
  codaComdXInterface *btn;
  btn = getXInterface ();
  btn->unmanage ();
}

void
rcComdButton::helpMsgWin (rcHelpMsgWin* win)
{
  msgWin_ = win;
}

void
rcComdButton::netStatusWin (rcNetStatus* win)
{
  stWin_ = win;
}

void
rcComdButton::infoPanel (rcInfoPanel* p)
{
  infoPanel_ = p;
}

void
rcComdButton::crossEventHandler (Widget, XtPointer clientData,
				 XEvent* event, Boolean)
{
  rcComdButton* obj = (rcComdButton *)clientData;
  XCrossingEvent* cev = (XCrossingEvent *)event;

  if (obj->msgWin_) {
    if (cev->type == EnterNotify) 
      obj->msgWin_->setMessage (obj->helpMsg_);
    else
      obj->msgWin_->eraseMessage ();
  }
}

void
rcComdButton::reportErrorMsg (char* msg)
{
  assert (infoPanel_);
  if (rcComdButton::errDialog_ == 0) {
    rcComdButton::errDialog_ = new XcodaErrorDialog (infoPanel_->baseWidget(),
						     "comdError",
						     "Error Dialog");
    rcComdButton::errDialog_->init ();
  }
  rcComdButton::errDialog_->setMessage (msg);
  rcComdButton::errDialog_->popup ();
}

const Widget
rcComdButton::dialogBaseWidget (void)
{
  assert (infoPanel_);
  return infoPanel_->baseWidget ();
}


