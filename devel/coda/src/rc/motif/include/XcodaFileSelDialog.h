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
//	 Motif File selection Dialog
//	
// Author:  Jie Chen
//       CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: XcodaFileSelDialog.h,v $
//   Revision 1.2  1998/04/08 17:28:55  heyes
//   get in there
//
//   Revision 1.1.1.1  1996/10/10 19:24:57  chen
//   coda motif C++ library
//
//
#ifndef _XCODA_FILESEL_DIALOG_H
#define _XCODA_FILESEL_DIALOG_H

#include <XcodaUi.h>
#include <Xm/FileSB.h>

class XcodaErrorDialog;

class XcodaFileSelDialog: public XcodaUi
{
 public:
  //constructor and destrutors
  XcodaFileSelDialog(Widget, char *, char *);
  virtual ~XcodaFileSelDialog();
  // init routine
  void init();
  // popup and popdown
  void popup();
  void popdown();
  int  isMapped();
  // file status
  enum FILESTATUS {NOTFOUND, OPEN_ERR, DIREC, READABLE, WRITABLE};
  FILESTATUS fileStatus()  { return file_status;}
  char *selectedFileName(); // user free it
  // class name
  virtual const char *className() const {return "XcodaFileSelDialog";}
  
 protected:
  // ok callback function, subclass can override this
  void ok();
  // derived class redefine check file. eg. open for read and open for write
  virtual int  checkFile();
  // derived class can add what ever here
  virtual void execute();
  virtual void cancel();
  // need error dialog here to signal something is wrong
  XcodaErrorDialog *_errorDialog;

 private:
  char             *_filename;
  char             *_old_filename;
  // callbacks
  static           void okCallback(Widget, XtPointer, 
				   XmFileSelectionBoxCallbackStruct *);
  static           void cancelCallback(Widget, XtPointer,
				       XmFileSelectionBoxCallbackStruct *);
  int              status(char *fname);
  // pass callback struct
  void             setCbs(XmFileSelectionBoxCallbackStruct *);
  // data memeber
  Widget           _parent;
  char             *_title;
  int              alreadyManaged;
  XmFileSelectionBoxCallbackStruct *_cbs;
  FILESTATUS       file_status;
};
#endif

