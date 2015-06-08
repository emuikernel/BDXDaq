//-----------------------------------------------------------------------------
// copyright (c) 1994,1995 Southeastern Universities Research Association,
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
//      Implementation of RunControl Message Window
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMsgWindow.cc,v $
//   Revision 1.2  1996/10/14 20:13:40  chen
//   add display server messages preference
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <rcComdOption.h>
#include <rcClient.h>
#include "rcMsgWindow.h"

const char* RC_MSG_ATTR = "runMessage";

rcMsgWindow::rcMsgWindow (Widget parent, 
			  char* name,
			  rcClientHandler& handler)
:XcodaUi (name), rcPanel (), parent_ (parent), textw_ (0),
 netHandler_ (handler), wprPosition_ (0), connectionHist_ (0), showText_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcMsgWindow Class Object\n");
#endif
  // first register this panel to net handler
  netHandler_.addPanel (this);
}

rcMsgWindow::~rcMsgWindow (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcMsgWindow Class Object\n");
#endif
  // remove this panel from netHandler
  netHandler_.removePanel (this);
}



/* sergey: defines log messaging window !!! */
void
rcMsgWindow::init (void)
{
  rcComdOption* option = rcComdOption::option ();

  Arg arg[20];
  int ac = 0;

  XtSetArg (arg[ac], XmNrows, 6); ac++;
  XtSetArg (arg[ac], XmNcolumns, 50); ac++;
  XtSetArg (arg[ac], XmNeditable, FALSE); ac++;
  XtSetArg (arg[ac], XmNeditMode, XmMULTI_LINE_EDIT); ac++;
  XtSetArg (arg[ac], XmNwordWrap, TRUE); ac++;
  XtSetArg (arg[ac], XmNscrollHorizontal, FALSE); ac++;
  XtSetArg (arg[ac], XmNblinkRate, 0); ac++;
  XtSetArg (arg[ac], XmNautoShowCursorPosition, TRUE); ac++;
  XtSetArg (arg[ac], XmNcursorPositionVisible, FALSE); ac++;


  textw_ = XmCreateScrolledText (parent_, "rcMsgWindow", arg, ac); 
  //textw_ = XmCreateText (parent_, "rcMsgWindow", arg, ac); 


  ac = 0;
  _w = XtParent (textw_);


  if (option->reportMsg ()) {
    XtManageChild (textw_);
    showText_ = 1;
  }
  else {
    XtUnmanageChild (_w);
    XtUnmanageChild (textw_);
    showText_ = 1;
  }
  // install destroy callback
  installDestroyHandler ();
}

void
rcMsgWindow::monitorRunMsg (void)
{
  if (connectionHist_ == 0 && netHandler_.connected ()) {
    // first time to connect to remote server
    // expriment name must be non null
    assert (netHandler_.clientHandler ().exptname());
	
    int st;
    st = netHandler_.clientHandler ().monitorOnCallback (
		netHandler_.clientHandler().exptname(),
		(char *)RC_MSG_ATTR,
		&(rcMsgWindow::runMsgCallback),
		(void *)this);
    if (st == CODA_SUCCESS) 
      connectionHist_ = 1;
  }
}

void
rcMsgWindow::config (int status)
{
  if (status != DA_NOT_CONNECTED) 
    monitorRunMsg ();
  else 
    connectionHist_ = 0;
}

void
rcMsgWindow::runMsgCallback (int status, void* arg, daqNetData* data)
{
  rcMsgWindow* obj = (rcMsgWindow *)arg;
  if (status == CODA_SUCCESS) {
    if (obj->showText_) {
      Arg arg[10];
      int ac = 0;

      char *msg = (char *)(*data);
      XmTextInsert (obj->textw_, obj->wprPosition_, msg);
      (obj->wprPosition_) += ::strlen (msg);
      // set cursor position
      XtSetArg (arg[ac], XmNcursorPosition, obj->wprPosition_); ac++;
      XtSetValues (obj->textw_, arg, ac); 
      ac = 0;
      // show new cursor position
      XmTextShowPosition (obj->textw_, obj->wprPosition_);
    }
  }
  else 
    obj->connectionHist_ = 0;
}

void
rcMsgWindow::manage (void)
{
  XtManageChild (textw_);
  XcodaUi::manage ();
}

void
rcMsgWindow::unmanage (void)
{
  XtUnmanageChild (textw_);
  XcodaUi::unmanage ();
}

void
rcMsgWindow::displayMsg (void)
{
  showText_ = 1;
  manage ();
}

void
rcMsgWindow::displayNoMsg (void)
{
  showText_ = 0;
  unmanage ();
}



