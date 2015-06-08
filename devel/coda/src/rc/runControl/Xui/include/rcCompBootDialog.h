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
//      Run Control Components' auto boot option dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcCompBootDialog.h,v $
//   Revision 1.3  1998/05/28 17:46:54  heyes
//   new GUI look
//
//   Revision 1.2  1998/04/08 18:31:15  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#ifndef _RC_COMP_BOOT_DIALOG_H
#define _RC_COMP_BOOT_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcXuiConsts.h>
#include <rcClientHandler.h>
#include <XcodaForm.h>
#include <XcodaErrorDialog.h>

class rcCompBootDialog: public XcodaForm
{
public:
  // constructor and destructor
  rcCompBootDialog (char* name, char* title,
		    rcClientHandler& handler);
  virtual ~rcCompBootDialog (void);

  // redefine popup/popdown
  void     popup (void);
  void     popdown (void);

protected:
  // implementation of pure virtual function of XcodaFormDialog
  void     createFormChildren (void);

  // callbacks
  static void okCallback (Widget w, XtPointer data, XmAnyCallbackStruct* cbs);
  static void cancelCallback (Widget, XtPointer, XmAnyCallbackStruct* );
  static void compBootInfoCbk (int status, void* arg, daqNetData *);

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
};
#endif


       
