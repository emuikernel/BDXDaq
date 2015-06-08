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
//      Implementation of rcMonitorParmsDialog Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMonitorParmsDialog.cc,v $
//   Revision 1.4  1998/09/01 18:48:40  heyes
//   satisfy Randy's lust for command line options
//
//   Revision 1.3  1998/05/28 17:46:59  heyes
//   new GUI look
//
//   Revision 1.2  1997/07/22 19:39:03  heyes
//   cleaned up lots of things
//
//   Revision 1.1  1996/11/04 16:16:38  chen
//   monitoring components dialog etc
//
//
//

#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <rcClient.h>
#include <rcMonitorParmsButton.h>
#include <daqMonitorStruct.h>
#include "rcMonitorParmsDialog.h"

extern Widget monFr;

rcMonitorParmsDialog::rcMonitorParmsDialog (char* name, char* title,
					    rcClientHandler& handler)
:XcodaForm(monFr, name, title),
 netHandler_ (handler), num_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcMonitorParmsDialog Class Object\n");
#endif
  // empty
}

rcMonitorParmsDialog::~rcMonitorParmsDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcMonitorParmsDialog Class Object\n");
#endif
  if (num_ > 0) {
    for (int i = 0; i < num_; i++)
      delete []names_[i];
  }
}

void
rcMonitorParmsDialog::createFormChildren (void)
{
  Arg arg[20];
  int ac = 0;

  XmString t;
  ac = 0;
  // create action panel
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  Widget actform = XtCreateWidget ("actForm", xmFormWidgetClass,
				   _w, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("  Ok   ");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 35); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  Widget ok = XtCreateManagedWidget ("ok", xmPushButtonGadgetClass,
				     actform, arg, ac);
  ac = 0;
  XmStringFree (t);

  // create a toggle button to signal auto end or not when something is wrong
  t = XmStringCreateSimple ("End a run automatically when errors are found");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 35); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 5); ac++;
  autoend_ = XtCreateManagedWidget ("autoend", xmToggleButtonGadgetClass,
				    actform, arg, ac);
  ac = 0;
  XmStringFree (t);

  // Create as scale to change monitoring timer interval
  ac = 0;
  
  // t = XmStringCreateSimple ("Monitor interval (Sec)");
  // XtSetArg (arg[ac], XmNtitleString, t); ac++;
  XtSetArg (arg[ac], XmNminimum, 10); ac++;
  XtSetArg (arg[ac], XmNmaximum, 600); ac++;
  XtSetArg (arg[ac], XmNorientation, XmHORIZONTAL); ac++;
  XtSetArg (arg[ac], XmNscaleMultiple, 10); ac++;
  XtSetArg (arg[ac], XmNshowValue, True); ac++;


  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, actform); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;

  timer_ = XtCreateManagedWidget ("scale", xmScaleWidgetClass,
				  _w, arg, ac);
  ac = 0;
  XmStringFree (t);
  
  // create a row column widget with two row in it
  XtSetArg (arg[ac], XmNpacking, XmPACK_COLUMN); ac++;
  XtSetArg (arg[ac], XmNnumColumns, 5); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, timer_); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNwidth, 300); ac++;
  XtSetArg (arg[ac], XmNheight, 200); ac++;
  row_ = XtCreateWidget ("rowcolumn", xmRowColumnWidgetClass,
			 _w, arg, ac);
  ac = 0;

  // add callback for action button
  XtAddCallback (ok, XmNactivateCallback,
		 (XtCallbackProc)&(rcMonitorParmsDialog::okCallback), 
		 (void *)this);

  XtManageChild (actform);
  XtManageChild (row_);

  // set default button
  defaultButton (ok);


}

void
rcMonitorParmsDialog::popdown (void)
{
  if (num_ > 0) {
    for (int i = 0; i < num_; i++) {
      delete []names_[i];
      XtDestroyWidget (toggles_[i]);
    }
    num_ = 0;
  }
}

void
rcMonitorParmsDialog::popup (void)
{
  char** components = 0;
  long* monitored = 0;
  long autoend = 0;
  long  interval = 0;
  long  numComp = netHandler_.monitorParms (components, monitored,
					    autoend, interval);


  assert (numComp < RCXUI_MAX_COMPONENTS);

  Arg arg[10];
  int ac = 0;
  XmString t;



  for (int i = 0; i < numComp; i++)
  {
    names_[i] = new char[::strlen (components[i]) + 1];
    ::strcpy (names_[i], components[i]);

    t = XmStringCreateSimple (names_[i]);
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetArg (arg[ac], XmNset, monitored[i]); ac++;
    toggles_[i] = XtCreateWidget ("toggles", xmToggleButtonGadgetClass,
				  row_, arg, ac);
    ac = 0;
    XmStringFree (t);

    // manage child
    XtManageChild (toggles_[i]);
  }
  num_ = numComp;
  
  // set scale value
  XmScaleSetValue (timer_, interval);
  // set autoend toggle button

/*help:
#include <Xm/ToggleBG.h>
void XmToggleButtonGadgetSetState (widget, state, notify);
Widget widget;
Boolean state;
Boolean notify; 
*/

  /* when crashes, autoend is used as 2nd argument; if 0 or 1 used, does not crash !!! */
  printf("rcMonitorParmsDialog::popup: 1 0x%08x 0x%08x\n",autoend_, autoend);fflush(stdout);

  /* guess: crashes if first time called with '1', and second time with '0' ??? */
  /* if both times the same, it does not crash .. */
  /* CHECK 'autoend_' !!! */

  /*sergey XmToggleButtonGadgetSetState (autoend_, autoend, 1);*/
  XmToggleButtonGadgetSetState (autoend_, 0, 1);

  printf("rcMonitorParmsDialog::popup: 2\n");fflush(stdout);


}


void
rcMonitorParmsDialog::cancelCallback (Widget, XtPointer data, 
				  XmAnyCallbackStruct *)
{
  rcMonitorParmsDialog* obj = (rcMonitorParmsDialog *)data;

  obj->popdown ();
}

void
rcMonitorParmsDialog::okCallback (Widget, XtPointer data,
				  XmAnyCallbackStruct *)
{
  rcMonitorParmsDialog* obj = (rcMonitorParmsDialog *)data;

  daqMonitorStruct monInfo;
  int state;
  int interval = 0;

  for (int i = 0; i < obj->num_; i++) {
    state = XmToggleButtonGadgetGetState (obj->toggles_[i]);
    if (state) 
      monInfo.insertInfo (obj->names_[i], 1);
    else
      monInfo.insertInfo (obj->names_[i], 0);
  }

  if (XmToggleButtonGadgetGetState (obj->autoend_))
    monInfo.enableAutoEnd ();
  else
    monInfo.disableAutoEnd ();

  XmScaleGetValue (obj->timer_, &interval);

  monInfo.timerInterval (interval);

  // get network handler
  rcClient& client = obj->netHandler_.clientHandler (); 
  daqData data2 (client.exptname (), "command", (daqArbStruct *)&monInfo);

  if (client.sendCmdCallback (DAMONITOR_PARM, data2,
		      (rcCallback)&(rcMonitorParmsDialog::monitorParmsCbk),
		      (void *)obj) != CODA_SUCCESS) 
    printf ("Cannot send monitor options to the server\n");

}
    
void
rcMonitorParmsDialog::monitorParmsCbk (int status, void* arg, daqNetData *)
{
  rcMonitorParmsButton* obj = (rcMonitorParmsButton *)arg;
  
  if (status != CODA_SUCCESS)
    printf("Setting monitor options failed\n");
}



  
  
  

							 
  
