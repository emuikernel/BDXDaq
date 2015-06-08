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
//      RunControl Simple Information Panel
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunSInfoPanel.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#ifndef _RC_RUN_SIMPLE_INFO_PANEL_H
#define _RC_RUN_SIMPLE_INFO_PANEL_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <rcSvcProtocol.h>
#include <XcodaUi.h>
#include <rcClientHandler.h>

typedef struct _state_string
{
  char *string;
  int  state;
}stateString;

class rcRunSInfoPanel: public XcodaUi
{
public:
  // constructor and destructor
  rcRunSInfoPanel (Widget parent, char* name, rcClientHandler& handler);
  virtual ~rcRunSInfoPanel (void);

  // initialize all widgets
  void     init (void);
  // redefine manage and unmange
  virtual void manage   (void);
  virtual void unmanage (void);
  // configuration depending on status
  void     config (int status);
  
  // class name
  virtual const char* className (void) const {return "rcRunSInfoPanel";}

protected:
  static stateString  states[];
  static int          numStates;

  // all callbacks
  static void runNumberCallback (int status, void* arg, daqNetData* data);
  static void startTimeCallback (int status, void* arg, daqNetData* data);
  static void endTimeCallback   (int status, void* arg, daqNetData* data);
  static void eventLimitCallback (int status, void* arg, daqNetData* data);
  static void dataLimitCallback (int status, void* arg, daqNetData* data);
  static void setValueCallback  (int status, void* arg, daqNetData* data);
  // monitor off callbacks
  static void offCallback       (int status, void* arg, daqNetData* data);
  // convert whole date string (like Mon Feb 12 xx:xx:xx EST 1996) into
  // the following Feb 12 xx:xx:xx, 
  static void stripDateString  (char out[], char* in);
  
  // X window callback
  static void changeRunNumCallback (Widget w, XtPointer data,
				    XmAnyCallbackStruct *cbs);
  static void changeEventLimitCallback (Widget w, XtPointer data,
					XmAnyCallbackStruct *cbs);
  static void changeDataLimitCallback (Widget w, XtPointer data,
				       XmAnyCallbackStruct *cbs);

private:
  // parent widget
  Widget parent_;
  // Widget display runNumber
  Widget runNumber_;
  // Widget display start time
  Widget startTime_;
  // Widget display end time
  Widget endTime_;
  // Widget display status
  Widget status_;
  // Widget display event limit
  Widget eventLimit_;
  // Widget display data limit
  Widget dataLimit_;
  // cached value for runnumber
  int    runNumValue_;
  // cached value for event limit
  int    eventLimitValue_;
  // cached value for data limit
  int    dataLimitValue_;
  // network handler
  rcClientHandler& netHandler_;
};
#endif
