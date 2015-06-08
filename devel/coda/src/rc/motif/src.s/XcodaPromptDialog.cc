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
//	 CODA Motif Prompt Dialog Class
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaPromptDialog.cc,v $
//   Revision 1.2  1998/04/08 17:29:19  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:56  chen
//   coda motif C++ library
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaErrorDialog.h>
#include <XcodaPromptDialog.h>

XcodaPromptDialog::XcodaPromptDialog(Widget parent,
				     char   *name,
				     char   *title)
:XcodaUi(name)
{
#ifdef _TRACE_OBJECTS
  printf("         Create XcodaPromptDialog Object\n");
#endif
  _parent = parent;
  _title = new char[strlen(title)+1];
  ::strcpy(_title, title);
  alreadyManaged = 0;
  modalDialog = 0;
  _result = 0;
  _errorDialog = 0;
}

XcodaPromptDialog::~XcodaPromptDialog()
{
#ifdef _TARCE_OBJECTS
  printf("         Delete XcodaPormptDialog Object");
#endif
  if(_title)
    delete []_title;
  if(_result)
    delete []_result;
}

void XcodaPromptDialog::init()
{
  Arg arg[10];
  int ac = 0;

  if(!_w){
    XtSetArg (arg[ac], XmNtitle, _title); ac++;
    XtSetArg (arg[ac], XmNautoUnmanage, False); ac++;
    _w = XmCreatePromptDialog(_parent, _name, arg, ac);
    ac = 0;
    
    XtSetSensitive(XmSelectionBoxGetChild(_w, XmDIALOG_HELP_BUTTON),False);
    XtAddCallback (_w, XmNokCallback, 
		   (XtCallbackProc)&XcodaPromptDialog::okCallback,
		   (XtPointer)this);
    XtAddCallback (_w, XmNcancelCallback,
		   (XtCallbackProc)&XcodaPromptDialog::cancelCallback,
		   (XtPointer)this);
    installDestroyHandler();
  }
}

void XcodaPromptDialog::setMessage(char *msg)
{
  Arg arg[5];
  int ac = 0;
  XmString t;

  if(_w){
    t = XmStringCreateLtoR (msg, XmSTRING_DEFAULT_CHARSET);
    XtSetArg (arg[ac], XmNselectionLabelString, t); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

void XcodaPromptDialog::setOkLabel(char *label)
{
  Arg arg[5];
  int ac = 0;
  XmString t;

  if(_w){
    t = XmStringCreateLtoR (label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg (arg[ac], XmNokLabelString, t); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

void XcodaPromptDialog::setCancelLabel(char *label)
{
  Arg arg[5];
  int ac = 0;
  XmString t;

  if(_w){
    t = XmStringCreateLtoR (label, XmSTRING_DEFAULT_CHARSET);
    XtSetArg (arg[ac], XmNcancelLabelString, t); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

void
XcodaPromptDialog::setText (char* str)
{
  Arg arg[5];
  int ac = 0;

  if (_w) {
    XmString t = XmStringCreateSimple (str);
    XtSetArg (arg[ac], XmNtextString, t); ac++;
    XtSetValues (_w, arg, ac);
    ac = 0;
  }
}

void XcodaPromptDialog::popup()
{
  assert(_w);
  if(!alreadyManaged){
    manage();
    alreadyManaged = 1;
    if(modalDialog)
      XtAddGrab(XtParent(_w), True, False);
  }
}

void XcodaPromptDialog::popdown()
{
  assert(_w);
  if(alreadyManaged){
    unmanage();
    alreadyManaged = 0;
    if(modalDialog)
      XtRemoveGrab(XtParent(_w));
  }
}

int XcodaPromptDialog::isMapped()
{
  return alreadyManaged;
}
 
void XcodaPromptDialog::okCallback(Widget w, XtPointer client_data,
				   XmSelectionBoxCallbackStruct *cbs)
{
  XcodaPromptDialog *obj = (XcodaPromptDialog *)client_data;
  obj->setCbs(cbs);
  obj->localOk();
}

void XcodaPromptDialog::cancelCallback(Widget w, XtPointer client_data,
				       XmSelectionBoxCallbackStruct *cbs)
{
  XcodaPromptDialog *obj = (XcodaPromptDialog *)client_data;
  obj->setCbs(cbs);
  obj->localCancel();
}

void XcodaPromptDialog::localOk()
{
  char *output = 0;

  if(!XmStringGetLtoR(_cbs->value, XmSTRING_DEFAULT_CHARSET, &output)){
    if(!_errorDialog){
      _errorDialog = new XcodaErrorDialog(_w,"Input Error","Input Error");
      _errorDialog->init();
    }
    _errorDialog->popdown();
    _errorDialog->setMessage("Need Input, Try again!");
    _errorDialog->popup();
    output = 0;
    return;
  }
  if(!output || !*output){
    if(!_errorDialog){
      _errorDialog = new XcodaErrorDialog(_w,"Input Error","Input Error");
      _errorDialog->init();
    }
    _errorDialog->popdown();
    _errorDialog->setMessage("Need Input, Try again!");
    _errorDialog->popup();
    output = 0;
    return;
  }
  if(_result)
    delete []_result;
  _result = new char[::strlen(output) + 1];
  ::strcpy(_result, output);
  XtFree(output);
  if(!checkSyntax()){
    if(!_errorDialog){
      _errorDialog = new XcodaErrorDialog(_w, "Input Error", "Input Error");
      _errorDialog->init();
    }
    _errorDialog->popdown();
    _errorDialog->setMessage("Syntax Error, Try Again");
    _errorDialog->popup();
    return;
  }
  // call derived class ok function
  ok();
}

void XcodaPromptDialog::localCancel()
{
  // call derived class's cancel()
  cancel();
  popdown();
}

void XcodaPromptDialog::setCbs(XmSelectionBoxCallbackStruct *cbs)
{
  _cbs = cbs;
}

void XcodaPromptDialog::setModal()
{
  modalDialog = 1;
}

void XcodaPromptDialog::unsetModal()
{
  modalDialog = 0;
}
