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
//      RunControl RunType Dialog Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunTypeDialog.h,v $
//   Revision 1.2  1997/06/16 12:26:51  heyes
//   add dbedit and so on
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#ifndef _RC_RUN_TYPE_DIALOG_H
#define _RC_RUN_TYPE_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcClientHandler.h>
#include <XcodaFormDialog.h>

class rcRunTypeOption;
class XcodaErrorDialog;

class rcRunTypeDialog: public XcodaFormDialog
{
public:
  // constructor and destructor
  rcRunTypeDialog (Widget parent,
		   char* name,
		   char* title,
		   rcClientHandler& handler);
  virtual ~rcRunTypeDialog (void);

  // start monitoring runType of runControl server
  // this will be called by runPanel manage routine
  void startMonitoringRunTypes (void);
  void endMonitoringRunTypes   (void);

  // dialog send out configure command
  void configure (void);

  // redefine popup function
  void popup (void);

  // class name
  virtual const char* className (void) const {return "rcRunTypeDialog";}

protected:
  // implementation of this class required by super class of XcodaFormDialog
  void createFormChildren (void);

  // report error message
  void reportErrorMsg (char *msg);

  // push button callbacks
  static void okCallback (Widget, XtPointer data, XmAnyCallbackStruct *);
  static void cancelCallback (Widget, XtPointer data, XmAnyCallbackStruct *);

  // send configure command to the server callback
  static void configureCallback (int status, void* arg, daqNetData* data);

private:
  // network handler
  rcClientHandler& netHandler_;
  // option menu
  rcRunTypeOption* option_;
  // widget of pushbuttons
  // error dialog
  XcodaErrorDialog* errDialog_;
  // push button widgets
  Widget ok_;
  Widget cancel_;
};
#endif
