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
//      RunControl Data Log Options Dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcAnaLogDialog.h,v $
//   Revision 1.2  1998/04/08 18:31:11  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_ANA_LOG_DIALOG_H
#define _RC_ANA_LOG_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcClientHandler.h>
#include <XcodaForm.h>
#include <rcXuiConsts.h>

class rcMenuWindow;

class rcAnaLogDialog: public XcodaForm
{
public:
  // constructor and destructor
  rcAnaLogDialog (rcMenuWindow* logOption, char* name, char* title,
		  rcClientHandler& handler);
  virtual ~rcAnaLogDialog (void);

  // redefine popup
  void        popup (void);
  
  // real action
  void changeAnaLogInfo (void);
  
  // class name
  virtual const char* className (void) const {return "rcAnaLogDialog";}

protected:
  // inherited operations
  void        createFormChildren (void);
  // callbacks for all pushbuttons
  static void okCallback    (Widget, XtPointer, XmAnyCallbackStruct* );
  static void applyCallback (Widget, XtPointer, XmAnyCallbackStruct* );
  static void cancelCallback (Widget, XtPointer, XmAnyCallbackStruct* );
  // callback from setvalue of the ana log
  static void anaLogCallback (int status, void* arg, daqNetData* data);

private:
  rcMenuWindow* button_;
  // network handler
  rcClientHandler& netHandler_;
  // all widgets
  Widget toggle_;   // toggle button to override whether to log in or not
  Widget file_;
#if !defined (_CODA_2_0_T) && !defined (_CODA_2_0)
  // assume not more than 32 anas in the system
  Widget anas_[RCXUI_MAX_ANAS];
  int    numAnas_;
  // Pointer to a list of ANAS
  codaSlist* anaList_;
#endif
};
#endif
