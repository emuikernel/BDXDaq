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
//      RunControl Help Message Window
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcHelpMsgWin.cc,v $
//   Revision 1.2  1998/04/08 18:31:16  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <Xm/LabelG.h>
#include <Balloon.h>
#include "rcHelpMsgWin.h"

Widget helpBalloon;
extern Widget toplevel;

rcHelpMsgWin::rcHelpMsgWin (Widget parent, char* name)
:XcodaUi (name), parent_ (parent), displayMsg_ (1)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcHelpMsgWin Class Object\n");
#endif
}

rcHelpMsgWin::~rcHelpMsgWin (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcHelpMsgWin Class Object\n");
#endif
}

void
rcHelpMsgWin::init (void){
  Arg arg[10];
  int ac = 0;

  Arg args[5];

  XmString t = XmStringCreateSimple ("            ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 3); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_IN); ac++;
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
  _w = XtCreateManagedWidget (_name, xmLabelGadgetClass,
			      parent_, arg, ac);

  ac = 0;
  XmStringFree (t);

  installDestroyHandler ();

}

void
rcHelpMsgWin::setMessage (char* msg)
{
  if (displayMsg_) {
    Arg arg[10];
    int ac = 0;

    XmString t = XmStringCreateSimple (msg);
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
    XmStringFree (t);

    XmBalloonPopup(helpBalloon, 0, 0, msg);

  }
}

void
rcHelpMsgWin::eraseMessage (void)
{
  Arg arg[10];
  int ac = 0;

  XmString t = XmStringCreateSimple ("            ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetValues (_w, arg, ac);
  ac = 0;
  XmStringFree (t);
  //XmBalloonPopdown(helpBalloon);
}

void
rcHelpMsgWin::displayMsg (void)
{
  displayMsg_ = 1;
}

void
rcHelpMsgWin::displayNoMsg (void)
{
  displayMsg_ = 0;
  eraseMessage ();
}

  
