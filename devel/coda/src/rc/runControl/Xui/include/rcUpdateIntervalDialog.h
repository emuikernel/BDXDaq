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
//      RunControl Update Interval Dialog
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcUpdateIntervalDialog.h,v $
//   Revision 1.2  1998/05/28 17:47:07  heyes
//   new GUI look
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#ifndef _RC_UPDATE_INTERVAL_DIALOG_H
#define _RC_UPDATE_INTERVAL_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcClientHandler.h>
#include <XcodaForm.h>

class rcUpdateInterval;

class rcUpdateIntervalDialog: public XcodaForm
{
public:
  // constructor and destructor
  rcUpdateIntervalDialog (char* name, char* title,
			  rcClientHandler& handler);
  virtual ~rcUpdateIntervalDialog (void);

  // set update interval
  void    setUpdateInterval (int interval);
  
  // class name
  virtual const char* className (void) const {return "rcUpdateIntervalDialog";}

protected:
  // inherited operations
  void        createFormChildren (void);
  // callbacks for all pushbuttons
  static void okCallback    (Widget, XtPointer, XmAnyCallbackStruct* );
  static void applyCallback (Widget, XtPointer, XmAnyCallbackStruct* );
  static void cancelCallback (Widget, XtPointer, XmAnyCallbackStruct* );
  // set update interval value
  int  updateInterval    (void) const;
  void sendUpdateInterval (int interval);
  // callbacks for setting new update interval
  static void simpleCallback (int status, void* arg, daqNetData* data);

private:
  rcUpdateInterval* button_;
  // network handler
  rcClientHandler& netHandler_;
  // all widgets
  Widget scale_;
  // update interval cached value
  int interval_;
};
#endif
