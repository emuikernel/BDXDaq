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
//	 Coda Motif Generic Message Dialog Box
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaMsgDialog.h,v $
//   Revision 1.2  1998/04/08 17:29:15  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:58  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_MSG_DIALOG_H
#define _XCODA_MSG_DIALOG_H

#include <XcodaUi.h>
#include <Xm/MessageB.h>

class XcodaMsgDialog: public XcodaUi
{
public:
  void    init            (void);
  void    popup           (void);
  void    popdown         (void);
  int     isMapped        (void);
  // set message string aligment method
  // type = XmALIGNMENT_BEGINNING, XmALIGNMENT_END, XmALIGNMENT_CENTER
  void    alignment       (unsigned char type);
  void    setMessage      (char *);
  // default button
  // button = XmDIALOG_CANCEL_BUTTON
  // XmDIALOG_OK_BUTTON, XmDIALOG_HELP_BUTTON
  void    defaultButton   (unsigned char button);

  void    setOkLabel      (char *);
  void    setCancelLabel  (char *);
  void    setHelpLabel    (char *);
  virtual ~XcodaMsgDialog (void);

  virtual const char *className() const {return "XcodaMsgDialog";}
  
 protected:
  // constructor
  XcodaMsgDialog (Widget parent,
		  char   *name,
		  char   *title);

  // create different instantance
  virtual Widget createDialog (Widget, char *) = 0;

  virtual void ok();
  virtual void cancel();
  virtual void help();

 private:
  static void okCallback (Widget,
			  XtPointer,
			  XtPointer);
  static void cancelCallback (Widget,
			      XtPointer,
			      XtPointer);
  static void helpCallback (Widget,
			    XtPointer,
			    XtPointer);
  // all those callbacks
  void         localOk();
  void         localCancel();
  void         localHelp();

  int    alreadyManaged;
  // X window info
  Widget _parent;
  char   *_title;
  char   *_okLabel;
  char   *_cancelLabel;
  char   *_helpLabel;
};
#endif
