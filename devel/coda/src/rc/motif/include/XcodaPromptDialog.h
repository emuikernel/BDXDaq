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
//	 Motif Prompt selection Dialog (Abstract Class )
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaPromptDialog.h,v $
//   Revision 1.2  1998/04/08 17:29:21  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:58  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_PROMPT_DIALOG_H
#define _XCODA_PROMPT_DIALOG_H

#include <XcodaUi.h>
#include <Xm/SelectioB.h>

class XcodaErrorDialog;

class XcodaPromptDialog: public XcodaUi
{
public:
  //destructor
  virtual ~XcodaPromptDialog (void);
  // init routine
  void init (void);
  void setMessage (char *msg);
  void setOkLabel (char *label);
  void setCancelLabel (char *label);
  // setup model dialog
  void setModal (void);
  void unsetModal (void);
  // popup and popdown
  void popup (void);
  void popdown (void);
  int  isMapped (void);
  // set text to the dialog input field
  void setText (char *str);
  virtual const char *className() const { return "XcodaPromptDialog";}

protected:
  //constructor
  XcodaPromptDialog(Widget parent,
		    char   *name,
		    char   *title);
  // real callback function for Ok and cancel
  virtual void ok() = 0;
  virtual void cancel() = 0;
  // check output syntax
  virtual int  checkSyntax() = 0;
  // text output
  char   *_result;

private:
  //callbacks
  static void okCallback(Widget, XtPointer, 
			 XmSelectionBoxCallbackStruct *);
  static void cancelCallback(Widget, XtPointer, 
			     XmSelectionBoxCallbackStruct *);
  void localOk();
  void localCancel();

  // pass callback structure
  void        setCbs(XmSelectionBoxCallbackStruct *);
  // data memebrs
  Widget _parent;
  char   *_title;
  XcodaErrorDialog *_errorDialog;
  int    alreadyManaged;
  int    modalDialog;
  XmSelectionBoxCallbackStruct *_cbs;
};
#endif

  
