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
//      Implementation of Simple RunControl Information
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcRunSInfoPanel.cc,v $
//   Revision 1.8  2000/08/21 18:13:27  abbottd
//   Sun 5.0 C++ compiler fix
//
//   Revision 1.7  1998/09/17 19:18:37  rwm
//   Only print if _CODA_DEBUG is set.
//
//   Revision 1.6  1998/06/18 12:20:42  heyes
//   new GUI ready I think
//
//   Revision 1.5  1998/04/08 18:31:32  heyes
//   new look and feel GUI
//
//   Revision 1.4  1997/10/15 16:08:31  heyes
//   embed dbedit, ddmon and codaedit
//
//   Revision 1.3  1997/06/06 18:51:32  heyes
//   new RC
//
//   Revision 1.2  1997/05/26 12:27:50  heyes
//   embed tk in RC GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <Xm/LabelG.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/TextF.h>

#include "rcRunSInfoPanel.h"

stateString rcRunSInfoPanel::states[] =
{
  {"dormant", DA_DORMANT},
  {"booting", DA_BOOTING},
  {"booted", DA_BOOTED},
  {"configuring", DA_CONFIGURING},
  {"configured", DA_CONFIGURED},
  {"downloading", DA_DOWNLOADING},
  {"downloaded", DA_DOWNLOADED},
  {"prestarting", DA_PRESTARTING},
  {"paused", DA_PAUSED},
  {"pausing", DA_PAUSING},
  {"activating", DA_ACTIVATING},
  {"active", DA_ACTIVE},
  {"ending", DA_ENDING},
  {"verifying", DA_VERIFYING},
  {"verified", DA_VERIFIED},
  {"prestarted", DA_PRESTARTED},
  {"resetting", DA_TERMINATING},
};

int rcRunSInfoPanel::numStates = 17;


rcRunSInfoPanel::rcRunSInfoPanel (Widget parent, char* name,
				  rcClientHandler& handler)
:XcodaUi (name), parent_ (parent), runNumValue_ (0), 
 eventLimitValue_ (0), dataLimitValue_ (0), netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("               Create rcRunSInfoPanel Class Object\n");
#endif
  // empty
}

rcRunSInfoPanel::~rcRunSInfoPanel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("               Delete rcRunSInfoPanel Class Object\n");
#endif
  // empty
}

void
rcRunSInfoPanel::init (void)
{
  Arg arg[20];
  Widget children[20];
  int ac = 0;

  // Run Status Frame
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_IN); ac++;
  _w = XtCreateWidget (_name, xmFrameWidgetClass, parent_, arg, ac);

  // create frame title
  ac = 0;
  XmString t = XmStringCreateSimple ("Run status");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget rilabel = XtCreateWidget ("runstatusLabel",
					 xmLabelWidgetClass,
					 _w, arg, ac);
  XmStringFree (t);

  ac = 0;
  XtSetArg (arg[ac], XmNshadowThickness, 0); ac++;
  XtSetArg (arg[ac], XmNfractionBase, 2); ac++;
  Widget form = XtCreateWidget (_name, xmFormWidgetClass, _w, arg, ac);

  XtManageChild (form);  
  XtManageChild (rilabel);
  // create first row contains run number
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 1); ac++; 
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;

  Widget subf1 = XtCreateWidget ("subform", xmFrameWidgetClass,
				form, arg, ac);
  ac = 0;
  t = XmStringCreateSimple ("Run number");
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;

  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  Widget rnlabel = XtCreateManagedWidget ("runNumberLabel", 
					  xmLabelWidgetClass, subf1, arg, ac);
  XmStringFree (t);

  ac = 0;
  XtSetArg (arg[ac], XmNheight, 20); ac++;
  XtSetArg (arg[ac], XmNmarginHeight, 0); ac++;
  XtSetArg (arg[ac], XmNmarginWidth, 0); ac++;
  XtSetArg (arg[ac], XmNblinkRate, 0); ac++;
  XtSetArg (arg[ac], XmNvalue, "0"); ac++;
  if (getenv("UNEDIT_RUN_NUMBER"))
    XtSetArg (arg[ac], XmNeditable, False); ac++;
  runNumber_ = XtCreateManagedWidget ("runNumber",
				      xmTextFieldWidgetClass, 
				      subf1, arg, ac);
  ac = 0;
  XtAddCallback (runNumber_, XmNactivateCallback,
		 (XtCallbackProc)&(rcRunSInfoPanel::changeRunNumCallback),
		 (XtPointer)this);

  XtManageChild (subf1);

  // row containing status information in char string form
  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, subf1); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, subf1); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNleftWidget, subf1); ac++; 
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  Widget subf6 = XtCreateWidget ("runstatusFrame", xmFrameWidgetClass,
			 form, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("Run status");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget statuslabel = XtCreateManagedWidget ("statusLabel", 
					      xmLabelWidgetClass, subf6,
					      arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("Dormant");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNborderWidth, 0); ac++;
  status_ = XtCreateManagedWidget ("status",
				   xmLabelWidgetClass, subf6, arg, ac);

  // create second row containing experiment starting time
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, subf1); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 1); ac++; 
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  Widget subf2 = XtCreateWidget ("subform", xmFrameWidgetClass,
			 form, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("Start time");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget stlabel = XtCreateManagedWidget ("startTimeLabel", 
					  xmLabelWidgetClass, subf2, arg, ac);
  XmStringFree (t);
  ac = 0;

  t = XmStringCreateSimple ("      ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNborderWidth, 0); ac++;
  startTime_ = XtCreateManagedWidget ("startTimeG",
				      xmLabelWidgetClass, subf2, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtManageChild (subf2);  

  // create the 3rd row containing end time of experiment
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, subf2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, subf2); ac++; 
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  Widget subf3 = XtCreateWidget ("subform", xmFrameWidgetClass,
			 form, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("End time");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget etlabel = XtCreateManagedWidget ("endTimeLabel", 
					  xmLabelWidgetClass, subf3, arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("      ");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNrecomputeSize, FALSE); ac++;
  XtSetArg (arg[ac], XmNborderWidth, 0); ac++;
  endTime_ = XtCreateManagedWidget ("endTimeG",
				    xmLabelWidgetClass, subf3, arg, ac);
  ac = 0;
  XmStringFree (t);
  
  XtManageChild (subf3);

  // create the 4th row containing event limit
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, subf2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 2); ac++; 
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  
  Widget limitFr = XtCreateWidget ("limitframe", xmFrameWidgetClass,
			 form, arg, ac);

  ac = 0;
  t = XmStringCreateSimple ("Limits");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget limlabel = XtCreateManagedWidget ("LimitsLabel", 
					  xmLabelWidgetClass, limitFr, arg, ac);
  
  XtManageChild(limlabel);
  
  XmStringFree (t);

  ac = 0;			 
  XtSetArg (arg[ac], XmNshadowThickness, 0); ac++;
  XtSetArg (arg[ac], XmNfractionBase, 2); ac++;
  Widget limitFo = XtCreateWidget ("limitform", xmFormWidgetClass,
			 limitFr, arg, ac);

  XtManageChild(limitFr);
  		 
  ac = 0;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 1); ac++; 
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  //XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  Widget subf4 = XtCreateWidget ("eventLimitFrame", xmFrameWidgetClass,
			 limitFo, arg, ac);
  ac = 0;
  XtManageChild(limitFo);
  
  t = XmStringCreateSimple ("Events");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget ellabel = XtCreateManagedWidget ("EventLimitLabel", 
					  xmLabelWidgetClass, subf4, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtSetArg (arg[ac], XmNheight, 20); ac++;
  XtSetArg (arg[ac], XmNmarginHeight, 0); ac++;
  XtSetArg (arg[ac], XmNmarginWidth, 0); ac++;
  XtSetArg (arg[ac], XmNblinkRate, 0); ac++;
  XtSetArg (arg[ac], XmNvalue, "0"); ac++;
  eventLimit_ = XtCreateManagedWidget ("eventLimit",
				       xmTextFieldWidgetClass, subf4, arg, ac);
  ac = 0;

  XtManageChild (subf4);

  // create 5th row containing data limit

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 1); ac++; 
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  //XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNshadowType, XmSHADOW_ETCHED_OUT); ac++;
  Widget subf5 = XtCreateWidget ("dataLimitFrame", xmFrameWidgetClass,
			 limitFo, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("KBytes");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNchildType, XmFRAME_TITLE_CHILD); ac++;
  Widget dllabel = XtCreateManagedWidget ("DataLimitLabel", 
					  xmLabelWidgetClass, subf5, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtSetArg (arg[ac], XmNheight, 20); ac++;
  XtSetArg (arg[ac], XmNmarginHeight, 0); ac++;
  XtSetArg (arg[ac], XmNmarginWidth, 0); ac++;
  XtSetArg (arg[ac], XmNblinkRate, 0); ac++;
  XtSetArg (arg[ac], XmNvalue, "0"); ac++;
  dataLimit_ = XtCreateManagedWidget ("dataLimit",
				      xmTextFieldWidgetClass, subf5, arg, ac);
  
  XtManageChild (subf5);

  ac = 0;
  
  XtManageChild (subf6);

  // add callback for datalimit and event limit to change those values
  XtAddCallback (eventLimit_, XmNactivateCallback,
		 (XtCallbackProc)&(rcRunSInfoPanel::changeEventLimitCallback),
		 (XtPointer)this);  
  XtAddCallback (dataLimit_, XmNactivateCallback,
		 (XtCallbackProc)&(rcRunSInfoPanel::changeDataLimitCallback),
		 (XtPointer)this);  

  // finally manage top form widget and install destroy handler
  installDestroyHandler ();
  XtManageChild (_w);
}

void
rcRunSInfoPanel::config (int st)
{
  int i;
  int found = 0;
  Arg arg[10];
  int ac = 0;
  XmString t;

  for (i = 0; i < rcRunSInfoPanel::numStates; i++) {
    if (st == rcRunSInfoPanel::states[i].state) {
      found = 1;
      break;
    }
  }
  if (found) {
    t = XmStringCreateSimple (rcRunSInfoPanel::states[i].string);
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (status_, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

void
rcRunSInfoPanel::manage (void)
{
  XcodaUi::manage ();

  rcClient& client = netHandler_.clientHandler ();
  if (client.monitorOnCallback (client.exptname(), "runNumber",
				(rcCallback)&(rcRunSInfoPanel::runNumberCallback),
				(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot monitor on runNumber\n");
#endif
  }
  if (client.monitorOnCallback (client.exptname(), "startTime",
		(rcCallback)&(rcRunSInfoPanel::startTimeCallback),
		(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot monitor on startTime\n");
#endif
  }
  if (client.monitorOnCallback (client.exptname(), "endTime",
		(rcCallback)&(rcRunSInfoPanel::endTimeCallback),
		(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot monitor on endTime\n");
#endif
  }
  if (client.monitorOnCallback (client.exptname(), "eventLimit",
		(rcCallback)&(rcRunSInfoPanel::eventLimitCallback),
		(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot monitor on event limit\n");
#endif
  }
  if (client.monitorOnCallback (client.exptname(), "dataLimit",
		(rcCallback)&(rcRunSInfoPanel::dataLimitCallback),
		(void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot monitor on data limit\n");
#endif
  }
}

void
rcRunSInfoPanel::unmanage (void)
{
  rcClient& client = netHandler_.clientHandler ();
  if (client.monitorOffCallback (client.exptname(), "runNumber",
		 (rcCallback)&(rcRunSInfoPanel::runNumberCallback),
		 (void *)this,
		 (rcCallback)&(rcRunSInfoPanel::offCallback),
		 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot monitor off runNumber\n");
#endif
  }
  if (client.monitorOffCallback (client.exptname(), "startTime",
		 (rcCallback)&(rcRunSInfoPanel::startTimeCallback),
		 (void *)this,
		 (rcCallback)&(rcRunSInfoPanel::offCallback),
		 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot monitor off startTime\n");
#endif
  }
  if (client.monitorOffCallback (client.exptname(), "endTime",
		 (rcCallback)&(rcRunSInfoPanel::endTimeCallback),
		 (void *)this,
		 (rcCallback)&(rcRunSInfoPanel::offCallback),
		 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot monitor off endTime\n");
#endif
  }
  if (client.monitorOffCallback (client.exptname(), "eventLimit",
		 (rcCallback)&(rcRunSInfoPanel::eventLimitCallback),
		 (void *)this,
		 (rcCallback)&(rcRunSInfoPanel::offCallback),
		 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot monitor off eventLimit\n");
#endif
  }
  if (client.monitorOffCallback (client.exptname(), "dataLimit",
		 (rcCallback)&(rcRunSInfoPanel::dataLimitCallback),
		 (void *)this,
		 (rcCallback)&(rcRunSInfoPanel::offCallback),
		 (void *)this) != CODA_SUCCESS) {
#ifdef _CODA_DEBUG
    fprintf (stderr, "Cannot monitor off dataLimit\n");
#endif
  }
  XcodaUi::unmanage ();
}

void
rcRunSInfoPanel::runNumberCallback (int status, void* arg, daqNetData* data)
{
  rcRunSInfoPanel* obj = (rcRunSInfoPanel *)arg;

  if (status == CODA_SUCCESS) {
    Arg arg[10];
    int ac = 0;

    XtSetArg (arg[ac], XmNvalue, (char *)(*data)); ac++;
    XtSetValues (obj->runNumber_, arg, ac);
    ac = 0;

    // update cached value
    obj->runNumValue_ = (int)(*data);
  }
}

void
rcRunSInfoPanel::startTimeCallback (int status, void* arg, daqNetData* data)
{
  rcRunSInfoPanel* obj = (rcRunSInfoPanel *)arg;

  if (status == CODA_SUCCESS) {
    Arg arg[10];
    int ac = 0;

    char temp[80];
    rcRunSInfoPanel::stripDateString (temp, (char *)(*data));

    XmString t = XmStringCreateSimple (temp);
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (obj->startTime_, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

void
rcRunSInfoPanel::endTimeCallback (int status, void* arg, daqNetData* data)
{
  rcRunSInfoPanel* obj = (rcRunSInfoPanel *)arg;

  if (status == CODA_SUCCESS) {
    Arg arg[10];
    int ac = 0;

    char temp[80];
    rcRunSInfoPanel::stripDateString (temp, (char *)(*data));

    XmString t = XmStringCreateSimple (temp);
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (obj->endTime_, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
}

void
rcRunSInfoPanel::eventLimitCallback (int status, void* arg, daqNetData* data)
{
  rcRunSInfoPanel* obj = (rcRunSInfoPanel *)arg;

  if (status == CODA_SUCCESS) {
    XmTextFieldSetString (obj->eventLimit_, (char *)(*data));
    obj->eventLimitValue_ = (int)(*data);
  }
}

void
rcRunSInfoPanel::dataLimitCallback (int status, void* arg, daqNetData* data)
{
  rcRunSInfoPanel* obj = (rcRunSInfoPanel *)arg;

  if (status == CODA_SUCCESS) {
    XmTextFieldSetString (obj->dataLimit_, (char *)(*data));
    obj->dataLimitValue_ = (int)(*data);
  }
}

void
rcRunSInfoPanel::offCallback (int status, void* arg, daqNetData* data)
{
  rcRunSInfoPanel *obj = (rcRunSInfoPanel *)arg;
  if (status != CODA_SUCCESS) 
    printf ("monitor off failed\n");
}

void
rcRunSInfoPanel::stripDateString (char out[], char *in)
{
  char temp[80];

  // some gcc will only work for allocated memory of string
  ::strcpy (temp, in);
  char token[6][30]; // 6 field inside date string
  
  if (::sscanf (temp, "%s %s %s %s %s %s", token[0], token[1], token[2],
		token[3], token[4], token[5]) >= 5)
    sprintf (out, "%s %s %s",token[1], token[2], token[3]);
  else
    sprintf (out, "             ");
}

void
rcRunSInfoPanel::changeRunNumCallback (Widget w, XtPointer data,
				       XmAnyCallbackStruct *)
{
  rcRunSInfoPanel* obj = (rcRunSInfoPanel *)data;
  char *value = XmTextFieldGetString (w);
  int  err = 0;

  if (!value || !*value) 
    err = 1;
  else {
    char temp[256];
    int  newvalue;
    strncpy (temp, value, 256);
    if (::sscanf (temp, "%d", &newvalue) >= 1) {
      rcClient& client = obj->netHandler_.clientHandler ();
      daqData da (client.exptname (), "runNumber", newvalue);
      if (client.setValueCallback (da, 
		   (rcCallback)&(rcRunSInfoPanel::setValueCallback),
		   (void *)0) != CODA_SUCCESS) { 
	err = 1;
      }
    }
    else 
      err = 1;
  }
  if (err) {
    char tmp[80];
    sprintf (tmp, "%d", obj->runNumValue_);
    XmTextFieldSetString (w, tmp);
    XBell (XtDisplay (w), 50);
  }
  XtFree (value);
}

void
rcRunSInfoPanel::setValueCallback (int status, void* arg, daqNetData* data)
{
  if (status != CODA_SUCCESS) { 
    fprintf (stderr, "Communication to server error\n");
  }
}

void
rcRunSInfoPanel::changeEventLimitCallback (Widget w, XtPointer data,
					   XmAnyCallbackStruct *)
{
  rcRunSInfoPanel* obj = (rcRunSInfoPanel *)data;
  char *value = XmTextFieldGetString (w);
  int  err = 0;

  if (!value || !*value) 
    err = 1;
  else {
    char temp[256];
    int  newvalue;
    strncpy (temp, value, 256);
    if (::sscanf (temp, "%d", &newvalue) >= 1) {
      rcClient& client = obj->netHandler_.clientHandler ();
      daqData da (client.exptname (), "eventLimit", newvalue);
      if (client.setValueCallback (da, 
		   (rcCallback)&(rcRunSInfoPanel::setValueCallback),
		   (void *)0) != CODA_SUCCESS) { 
	err = 1;
      }
    }
    else 
      err = 1;
  }
  if (err) {
    char tmp[80];
    sprintf (tmp, "%d", obj->eventLimitValue_);
    XmTextFieldSetString (w, tmp);
    XBell (XtDisplay (w), 50);
  }
  XtFree (value);
}


void
rcRunSInfoPanel::changeDataLimitCallback (Widget w, XtPointer data,
					  XmAnyCallbackStruct *)
{
  rcRunSInfoPanel* obj = (rcRunSInfoPanel *)data;
  char *value = XmTextFieldGetString (w);
  int  err = 0;

  if (!value || !*value) 
    err = 1;
  else {
    char temp[256];
    int  newvalue;
    strncpy (temp, value, 256);
    if (::sscanf (temp, "%d", &newvalue) >= 1) {
      rcClient& client = obj->netHandler_.clientHandler ();
      daqData da (client.exptname (), "dataLimit", newvalue);
      if (client.setValueCallback (da, 
		   (rcCallback)&(rcRunSInfoPanel::setValueCallback),
		   (void *)0) != CODA_SUCCESS) { 
	err = 1;
      }
    }
    else 
      err = 1;
  }
  if (err) {
    char tmp[80];
    sprintf (tmp, "%d", obj->dataLimitValue_);
    XmTextFieldSetString (w, tmp);
    XBell (XtDisplay (w), 50);
  }
  XtFree (value);
}

