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
//   $Log: rcUpdateIntervalDialog.cc,v $
//   Revision 1.4  1998/11/24 13:57:58  heyes
//   check in for Carl...
//
//   Revision 1.3  1998/06/02 19:51:37  heyes
//   fixed rcServer
//
//   Revision 1.2  1998/05/28 17:47:07  heyes
//   new GUI look
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#include <Xm/Scale.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/SeparatoG.h>
#include <Xm/Form.h>

#include "rcUpdateInterval.h"
#include "rcUpdateIntervalDialog.h"

const int RC_UPDATEINTERVAL_MIN = 1;
const int RC_UPDATEINTERVAL_MAX = 30;

extern Widget updFr;

rcUpdateIntervalDialog::rcUpdateIntervalDialog (char* name,
						char* title,
						rcClientHandler& handler)
:XcodaForm(updFr, name, title), 
 netHandler_ (handler), scale_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcUpdateIntervalDialog Class Object\n");
#endif
  // empty
}

rcUpdateIntervalDialog::~rcUpdateIntervalDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcUpdateIntervalDialog Class Object\n");
#endif
}

void
rcUpdateIntervalDialog::createFormChildren (void)
{
  Arg arg[20];
  int ac = 0;

  // create label first
  char temp[128];
  ac = 0;
   XmString t = XmStringCreateSimple ("Apply");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 20); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  Widget ok = XtCreateManagedWidget ("anaLogOk", xmPushButtonWidgetClass,
				     _w, arg, ac);
  ac = 0;
  XmStringFree (t);
  // create a scale widget
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 20); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNorientation, XmHORIZONTAL); ac++;
  XtSetArg (arg[ac], XmNminimum, RC_UPDATEINTERVAL_MIN); ac++;
  XtSetArg (arg[ac], XmNmaximum, RC_UPDATEINTERVAL_MAX); ac++;
  XtSetArg (arg[ac], XmNshowValue, TRUE); ac++;
  scale_ = XtCreateManagedWidget ("rcUpIntDialogScale",
				  xmScaleWidgetClass,
				  _w, arg, ac);

  // add callbacks
  XtAddCallback (ok, XmNactivateCallback,
		 (XtCallbackProc)&(rcUpdateIntervalDialog::okCallback),
		 (XtPointer)this);

}

void
rcUpdateIntervalDialog::okCallback (Widget w, XtPointer data,
				    XmAnyCallbackStruct* )
{
  rcUpdateIntervalDialog* obj = (rcUpdateIntervalDialog *)data;

  // get value from scale widget
  int value;
  XmScaleGetValue (obj->scale_, &value);
  
  obj->sendUpdateInterval (value);
}

void
rcUpdateIntervalDialog::applyCallback (Widget w, XtPointer data,
				    XmAnyCallbackStruct* )
{
  rcUpdateIntervalDialog* obj = (rcUpdateIntervalDialog *)data;

  // get value from scale widget
  int value;
  XmScaleGetValue (obj->scale_, &value);
  obj->button_->sendUpdateInterval (value);
  
}


void
rcUpdateIntervalDialog::cancelCallback (Widget w, XtPointer data,
				    XmAnyCallbackStruct* )
{
  rcUpdateIntervalDialog* obj = (rcUpdateIntervalDialog *)data;


  //obj->popdown ();
  //reset value
  obj->setUpdateInterval (obj->updateInterval ());
}


void
rcUpdateIntervalDialog::setUpdateInterval (int sec)
{
  interval_ = sec;
  if (sec < RC_UPDATEINTERVAL_MIN)
    sec = RC_UPDATEINTERVAL_MIN;
  if (sec > RC_UPDATEINTERVAL_MAX)
    sec = RC_UPDATEINTERVAL_MAX;

  XmScaleSetValue (scale_, sec);

  //sendUpdateInterval (sec);

}

int
rcUpdateIntervalDialog::updateInterval (void) const
{
  return interval_;
}


void
rcUpdateIntervalDialog::sendUpdateInterval (int newval)
{
  // get client handler
  rcClient& client = netHandler_.clientHandler ();

  daqData data (client.exptname (), "updateInterval", newval);
  if (client.setValueCallback (data, 
			       (rcCallback)&(rcUpdateIntervalDialog::simpleCallback),
			       (void *)this) != CODA_SUCCESS) {
    return;
  }
}

void
rcUpdateIntervalDialog::simpleCallback (int status, void* arg, daqNetData* )
{
  rcUpdateInterval* obj = (rcUpdateInterval *)arg;
  
  if (status != CODA_SUCCESS) {
    obj->reportErrorMsg ("Setting new update interval to the server failed !");
    return;
  }
}







  
