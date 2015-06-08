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
//      RunControl Connect Dialog Box
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcConnectDialog.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#ifndef _RC_CONNECT_DIALOG_H
#define _RC_CONNECT_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcClientHandler.h>
#include <XcodaFormDialog.h>
#include <XcodaErrorDialog.h>
#include <ComboBox.h>

class rcConnect;

class rcConnectDialog: public XcodaFormDialog
{
public:
  // constructor and destructor
  rcConnectDialog (rcConnect* connect, char* name, char* title, 
		   rcClientHandler& handler);
  virtual ~rcConnectDialog (void);

  // redefine popup
  void     popup (void);

  // operations to handle network connection
  void     connect (void);
  
protected:
  // implementation of the pure virtual function
  void     createFormChildren (void);
  // callbacks for push buttons
  static void cancelCallback (Widget, XtPointer, XmAnyCallbackStruct*);
  static void openCallback   (Widget, XtPointer, XmAnyCallbackStruct*);  
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  // store all information of runcontrol server
  int         storeRcServerInfo (void);

  // callback for database combo box
  static void dbaseDropDownCbk (Widget, XtPointer, 
				XmComboBoxDropDownCallbackStruct *);
  // callback for session combo box
  static void sessionDropDownCbk (Widget, XtPointer,
				  XmComboBoxDropDownCallbackStruct *);

  // spawn callback
  static void spawnCallback (Widget, XtPointer,
			     XmAnyCallbackStruct* );
#endif

  // callbacks for connection timer
  static int  updateCallback (void* data);

  // check all expose and button event for X display
  static void updateDisplay  (Display* display);

  // report error
  void reportError (char* msg);

private:
  // connect command button
  rcConnect* connect_;
  // network handler
  rcClientHandler& netHandler_;
  // All widgets
  Widget hostname_;
  Widget exptname_;  // session name
  Widget exptid_;    // database name
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  Widget codadb_; // local database directory
  Widget spawn_;  // spawn a server button
#endif
  // comnmand buttons
  Widget open_;
  Widget cancel_;
  // int first time popup
  int    firstTime_;
  // error dialog
  XcodaErrorDialog* errDialog_;
  // application context for Xt
  XtAppContext appContext_;
};
#endif
