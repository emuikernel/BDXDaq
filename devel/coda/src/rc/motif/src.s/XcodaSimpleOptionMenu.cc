//-----------------------------------------------------------------------------
// Copyright (c) 1991,1992 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
// Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
//-----------------------------------------------------------------------------
// 
// Description:
//	Implementation of option menu
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaSimpleOptionMenu.cc,v $
//   Revision 1.4  2000/08/21 18:11:55  abbottd
//   Sun 5.0 C++ compiler fix
//
//   Revision 1.3  1998/04/08 17:29:32  heyes
//   get in there
//
//   Revision 1.2  1997/08/01 18:39:00  heyes
//   add background pixmap support
//
//   Revision 1.1.1.1  1996/10/10 19:25:00  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>

#include "XcodaSimpleOptionMenu.h"

XcodaSimpleOptionMenu::XcodaSimpleOptionMenu (Widget parent,
					      char* name,
					      char* title,
					      int   maxentries)
: XcodaUi (name), currentSel_ (0), prevSel_ (-1), numWidgets_ (maxentries),
  parent_ (parent), pbmapped_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create XcodaSimpleOptionMenu Class Object\n");
#endif
  title_ = new char[::strlen (title) + 1];
  ::strcpy (title_, title);

  widgets_ = new Widget[numWidgets_];
  menu_ = 0;
}

XcodaSimpleOptionMenu::~XcodaSimpleOptionMenu (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete XcodaSimpleOptionMenu Class Object\n");
#endif
  delete []title_;
  delete []widgets_;
  // all widgets will be destroyed by Xt destroy mechanism
}

extern "C" Pixel get_pixel(Display*, Colormap,int,int,int);

void
XcodaSimpleOptionMenu::init (void)
{
  Arg arg[20];
  int ac = 0;
  int i;
  menu_ = XmCreatePulldownMenu (parent_, "optionPulldown", arg, ac);
  
  ac = 0;
  XmString t = XmStringCreateSimple (title_);
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNsubMenuId, menu_); ac++;
  _w = XmCreateOptionMenu (parent_, _name, arg, ac);

  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ((char *)"              ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  for (i = 0; i < numWidgets_; i++) {
    widgets_[i] = XtCreateWidget ("optionButton", xmPushButtonGadgetClass,
				  menu_, arg, ac);
    XtAddCallback (widgets_[i], XmNactivateCallback,
		   (XtCallbackProc)&(XcodaSimpleOptionMenu::comdCallback),
		   (XtPointer)this);
    if (i == 0)
      XtManageChild (widgets_[i]);
  }
  ac = 0;
  XmStringFree (t);
  // install destroy handler
  installDestroyHandler ();
}

void
XcodaSimpleOptionMenu::addEntries (char** names, int num, int selection)
{
  Arg arg[10];
  int ac = 0;
  int i;
  XmString t;

  int realnum = num < numWidgets_ ? num : numWidgets_;
  for (i = 0; i < realnum; i++) {
    t = XmStringCreateSimple (names[i]);
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (widgets_[i], arg, ac);
    ac = 0;
    XmStringFree (t);
    XtManageChild (widgets_[i]);
  }
  pbmapped_ = 1;

  if (selection != 0 && selection < realnum) {
    currentSel_ = selection;
    prevSel_ = selection;

    XtSetArg (arg[ac], XmNmenuHistory, widgets_[selection]); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
  }
  else {
    currentSel_ = 0;
    prevSel_ = 0;
  }

  // real action hook here
  entriesChangedAction ();
}


void
XcodaSimpleOptionMenu::addEntries (char** names, int* active, 
				   int num, int selection)
{
  Arg arg[10];
  int ac = 0;
  int i;
  XmString t;

  int realnum = num < numWidgets_ ? num : numWidgets_;
  for (i = 0; i < realnum; i++) {
    t = XmStringCreateSimple (names[i]);
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (widgets_[i], arg, ac);
    ac = 0;
    XmStringFree (t);
    if (active[i] == 0)
      XtSetSensitive (widgets_[i], FALSE);
    else
      XtSetSensitive (widgets_[i], TRUE);
    XtManageChild (widgets_[i]);
  }
  
  pbmapped_ = 1;
  if (selection != 0 && selection < realnum) {
    currentSel_ = selection;
    prevSel_ = selection;

    XtSetArg (arg[ac], XmNmenuHistory, widgets_[selection]); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
  }
  else {
    currentSel_ = 0;
    prevSel_ = 0;
  }
  // real action hook here
  entriesChangedAction ();
}

void
XcodaSimpleOptionMenu::removeAll (void)
{
  int i; 
  
  for (i = 1; i < numWidgets_; i++) {
    XtUnmanageChild (widgets_[i]);
    XtSetSensitive (widgets_[i], TRUE);
  }

  Arg arg[20];
  int ac = 0;
  XmString t = XmStringCreateSimple ("         ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetValues (widgets_[0], arg, ac);
  ac = 0;
  XmStringFree (t);
  XtSetSensitive (widgets_[0], TRUE);

  // reset option menu selection to 0
  currentSel_ = 0;
  XtSetArg (arg[ac], XmNmenuHistory, widgets_[0]); ac++;
  XtSetValues (_w, arg, ac);
  ac = 0;
  // reset mapping flag
  pbmapped_ = 0;
  currentSel_ = 0;
  prevSel_ = -1;
}

void
XcodaSimpleOptionMenu::comdCallback (Widget w,
				     XtPointer clientData,
				     XmAnyCallbackStruct *)
{
  int i;
  XcodaSimpleOptionMenu* obj = (XcodaSimpleOptionMenu *)clientData;
 
  if (obj->pbmapped_) {
    for (i = 0; i < obj->numWidgets_; i++) 
      if (w == obj->widgets_[i])
	break;

    obj->prevSel_ = obj->currentSel_;
    obj->currentSel_ = i;
    // doit will use prev and current selection
    obj->doit ();
  }
}

void
XcodaSimpleOptionMenu::doit (void)
{

}

void
XcodaSimpleOptionMenu::entriesChangedAction (void)
{
  // empty
}
