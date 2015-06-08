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
//      Run Control Components' monitoring options
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMonitorParmsDialog.h,v $
//   Revision 1.3  1998/09/01 18:48:41  heyes
//   satisfy Randy's lust for command line options
//
//   Revision 1.2  1998/05/28 17:47:00  heyes
//   new GUI look
//
//   Revision 1.1  1996/11/04 16:16:39  chen
//   monitoring components dialog etc
//
//
//
#ifndef _RC_MONITOR_PARMS_DIALOG_H
#define _RC_MONITOR_PARMS_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcXuiConsts.h>
#include <rcClientHandler.h>
#include <XcodaForm.h>
#include <XcodaErrorDialog.h>

class rcMonitorParmsDialog: public XcodaForm
{
public:
  // constructor and destructor
  rcMonitorParmsDialog (char* name, char* title,
			rcClientHandler& handler);
  virtual ~rcMonitorParmsDialog (void);

  // redefine popup/popdown
  void     popup   (void);
  void     popdown (void);

protected:
  // implementation of pure virtual function of XcodaFormDialog
  void     createFormChildren (void);

  // callbacks
  static void okCallback (Widget w, XtPointer data, XmAnyCallbackStruct* cbs);
  static void cancelCallback (Widget, XtPointer, XmAnyCallbackStruct* );
  static void infoCbk (int status, void* arg, daqNetData *);
  static void monitorParmsCbk (int status, void* arg, daqNetData *);

private:
  // network handler
  rcClientHandler& netHandler_;
  // toggle widgets for all components
  Widget   toggles_[RCXUI_MAX_COMPONENTS];
  // row column widget holding all toggles
  Widget   row_;
  // number of widgets to map
  int      num_;
  // name of components
  char*    names_[RCXUI_MAX_COMPONENTS];

  // scale widget for timer interval
  Widget   timer_;
  // Toggle widget for autoend
  Widget   autoend_;
};
#endif


       
