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
//	 Coda Generic Message Dialog Box
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaMsgDialog.cc,v $
//   Revision 1.2  1998/04/08 17:29:13  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <XcodaMsgDialog.h>

XcodaMsgDialog::XcodaMsgDialog(Widget parent,
			       char   *name,
			       char   *title)
:XcodaUi (name)
{
  alreadyManaged = 0;
  _parent = parent;

  _title = new char[strlen(title)+1];
  strcpy (_title, title);

  _okLabel = (char *)0;
  _cancelLabel = (char *)0;
  _helpLabel = (char *)0;
}

XcodaMsgDialog::~XcodaMsgDialog()
{
  delete []_title;
  if (_okLabel != 0)
    delete []_okLabel;
  if (_cancelLabel != 0)
    delete []_cancelLabel;
  if (_helpLabel != 0)
    delete []_helpLabel;
}

void XcodaMsgDialog::init()
{
  Arg arg[10];
  int ac = 0;

  if (_w == 0){
    _w = createDialog(_parent, _name);
    if (_title != 0){
      XtSetArg (arg[ac], XmNtitle, _title); 
      ac++;
    }
    XtSetArg (arg[ac], XmNautoUnmanage, False); 
    ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;

    XtAddCallback(_w, XmNokCallback,
		  (XtCallbackProc)&XcodaMsgDialog::okCallback,
		  (XtPointer)this);
    XtAddCallback(_w, XmNcancelCallback,
		  (XtCallbackProc)&XcodaMsgDialog::cancelCallback,
		  (XtPointer)this);
    XtAddCallback (_w, XmNhelpCallback,
		   (XtCallbackProc)&XcodaMsgDialog::helpCallback,
		   (XtPointer)this);
    // install destroy handler
    installDestroyHandler();
  }
}

void XcodaMsgDialog::popup()
{
  if (!alreadyManaged){
    manage();
    alreadyManaged = 1;
  }
}

void XcodaMsgDialog::popdown()
{
  if (alreadyManaged){
    unmanage();
    alreadyManaged = 0;
  }
}

void XcodaMsgDialog::okCallback(Widget w,
				XtPointer client_data,
				XtPointer)
{
  XcodaMsgDialog *obj = (XcodaMsgDialog *)client_data;
  obj->localOk ();
}

void XcodaMsgDialog::cancelCallback(Widget w,
				    XtPointer client_data,
				    XtPointer)
{
  XcodaMsgDialog *obj = (XcodaMsgDialog *)client_data;
  obj->localCancel ();
}

void XcodaMsgDialog::helpCallback(Widget w,
				  XtPointer client_data,
				  XtPointer)
{
  XcodaMsgDialog *obj = (XcodaMsgDialog *)client_data;
  obj->localHelp();
}

void XcodaMsgDialog::ok()
{
  // empty, children class must redefine this
}

void XcodaMsgDialog::cancel()
{
  //empty, children class must redefine this 
}

void XcodaMsgDialog::help()
{
  //empty children class must redefine this
}

void XcodaMsgDialog::localOk()
{
  ok();
  alreadyManaged = 0;
  popdown();
}

void XcodaMsgDialog::localCancel()
{
  cancel();
  alreadyManaged = 0;
  popdown();
}

void XcodaMsgDialog::localHelp()
{
  help();
}

int XcodaMsgDialog::isMapped()
{
  return alreadyManaged;
}

void XcodaMsgDialog::setMessage (char *newlabel)
{
  Arg arg[5];
  int ac = 0;
  XmString t;

  if(_w){
    t = XmStringCreateLtoR (newlabel, XmSTRING_DEFAULT_CHARSET);
    XtSetArg (arg[ac], XmNmessageString, t); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

void XcodaMsgDialog::setOkLabel (char *newlabel)
{
  Arg arg[5];
  int ac = 0;
  XmString t;

  if(_w){
    t = XmStringCreateSimple (newlabel);
    XtSetArg (arg[ac], XmNokLabelString, t); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}   

void XcodaMsgDialog::setCancelLabel (char *newlabel)
{
  Arg arg[5];
  int ac = 0;
  XmString t;

  if(_w){
    t = XmStringCreateSimple (newlabel);
    XtSetArg (arg[ac], XmNcancelLabelString, t); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}   

void XcodaMsgDialog::setHelpLabel (char *newlabel)
{
  Arg arg[5];
  int ac = 0;
  XmString t;

  if(_w){
    t = XmStringCreateSimple (newlabel);
    XtSetArg (arg[ac], XmNhelpLabelString, t); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}   

void
XcodaMsgDialog::alignment (unsigned char type)
{
  Arg arg[5];
  int ac = 0;

  if (_w) {
    XtSetArg (arg[ac], XmNmessageAlignment, type); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
  }
}

void
XcodaMsgDialog::defaultButton (unsigned char button)
{
  Arg arg[5];
  int ac = 0;

  if (_w) {
    XtSetArg (arg[ac], XmNdefaultButtonType, button); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
  }
}

    

