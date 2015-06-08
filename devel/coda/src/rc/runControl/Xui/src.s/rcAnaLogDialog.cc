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
//      Implementation of ANA Log File Dialog Box
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcAnaLogDialog.cc,v $
//   Revision 1.4  1998/06/18 12:20:29  heyes
//   new GUI ready I think
//
//   Revision 1.3  1998/05/28 17:46:50  heyes
//   new GUI look
//
//   Revision 1.2  1998/04/08 18:31:10  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <rcMenuWindow.h>

#include <daqNetData.h>
#include "rcAnaLogDialog.h"

extern Widget fileFr;

#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
rcAnaLogDialog::rcAnaLogDialog (rcMenuWindow* button,
				char* name, char* title,
				rcClientHandler& handler)
:XcodaForm(fileFr, name, title),
 button_ (button), netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcAnaLogDialog Class Object\n");
#endif
}

void
rcAnaLogDialog::createFormChildren (void)
{
  Arg arg[20];
  int ac = 0;

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  Widget actionForm = XtCreateWidget ("anaLogActFrom", xmFormWidgetClass,
				      _w, arg, ac);

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, actionForm); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  Widget entryForm = XtCreateWidget ("anaLogEntFrom", xmFormWidgetClass,
				     _w, arg, ac);
  ac = 0;
  /*
  XmString t = XmStringCreateSimple ("Apply");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 0); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_OPPOSITE_FORM); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 20); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;    
  Widget apply = XtCreateManagedWidget ("anaLogApply", xmPushButtonGadgetClass,
					actionForm, arg, ac);
  ac = 0;
  XmStringFree (t);  
  */

  // create a toggle button
  XmString t = XmStringCreateSimple ("Log events to a file");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 20); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  toggle_ = XtCreateManagedWidget ("anaLogToggle", xmToggleButtonGadgetClass,
				    actionForm, arg, ac);
  ac = 0;
  XmStringFree (t);


  // create text field entry
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, actionForm); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 30); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNblinkRate, 0); ac++;
  XtSetArg (arg[ac], XmNvalue, ""); ac++;
  file_ = XtCreateManagedWidget ("anaLogFile", xmTextFieldWidgetClass,
				 entryForm, arg, ac);
  ac = 0;

  // create a label associated with text entry
  t = XmStringCreateSimple ("File name:");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_OPPOSITE_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 30); ac++;  
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
  Widget fileLabel = XtCreateManagedWidget ("anaLogFileLabel",
					    xmLabelGadgetClass,
					    entryForm, arg, ac);
  ac = 0;
  XmStringFree (t);
    
  XtAddCallback (file_, XmNactivateCallback,
		 (XtCallbackProc)&(rcAnaLogDialog::applyCallback),
		 (XtPointer)this);

  XtManageChild (actionForm);
  XtManageChild (entryForm);
}

void
rcAnaLogDialog::popup (void)
{
  char* logdes = netHandler_.logFileDescriptor ();
  if (logdes && ::strcmp (logdes, "unknown") != 0) {
    XmToggleButtonGadgetSetState (toggle_, 1, 0);
    XmTextFieldSetString   (file_, netHandler_.logFileDescriptor ());
  }
  else 
    XmToggleButtonGadgetSetState (toggle_, 0, 0);

  XcodaForm::popup (); 
}

void
rcAnaLogDialog::changeAnaLogInfo (void)
{
  // get network handler
  rcClient& client = netHandler_.clientHandler ();

  // get toggle button state: if it is not set, do not care the file entry
  if (XmToggleButtonGadgetGetState (toggle_)) {
    char *log = XmTextFieldGetString (file_);
    if (!log || !*log) {
      button_->reportErrorMsg ("Need file name to write events");
      return;
    }

    daqData sdata (client.exptname (), "logFileDescriptor", log);
    if (client.setValueCallback (sdata, 
				 (rcCallback)&(rcAnaLogDialog::anaLogCallback),
				 (void *)this) != CODA_SUCCESS) {
      button_->reportErrorMsg ("Cannot send set value command"); 
      XtFree (log);
      return;
    }    
    XtFree (log);
  }
  else {
    daqData sdata (client.exptname (), "logFileDescriptor", "unknown");
    if (client.setValueCallback (sdata, 
				 (rcCallback)&(rcAnaLogDialog::anaLogCallback),
				 (void *)this) != CODA_SUCCESS) {
      button_->reportErrorMsg ("Cannot send set value command"); 
      return;
    }    
  }
}

void
rcAnaLogDialog::anaLogCallback (int status, void* arg, daqNetData* data)
{
  rcAnaLogDialog* obj = (rcAnaLogDialog *)arg;

  if (status != CODA_SUCCESS) 
    obj->button_->reportErrorMsg ("Setting log file name failed");
}
#else

rcAnaLogDialog::rcAnaLogDialog (rcMenuWindow* button,
				char* name, char* title,
				rcClientHandler& handler)
:XcodaForm (fileFr, name, title),
 button_ (button), netHandler_ (handler), numAnas_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcAnaLogDialog Class Object\n");
#endif
  anaList_ = &(netHandler_.allAnas ());
}

void
rcAnaLogDialog::createFormChildren (void)
{
  Arg arg[20];
  int ac = 0;
  char temp[128];

  sprintf (temp, "Select one of the following ANA components\nto log events to a file");

  // create label first
  XmString t = XmStringCreateLtoR (temp, XmSTRING_DEFAULT_CHARSET);
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  Widget label0 = XtCreateManagedWidget ("analogDialogLabel",
					 xmLabelGadgetClass,
					 _w, arg, ac);
  ac = 0;
  XmStringFree (t);

  // create a sepearator
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, label0); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 10); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  Widget subf = XtCreateWidget ("anaLogSubf", xmFormWidgetClass,
				_w, arg, ac);
  ac = 0;

  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 20); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNtopPosition, 49); ac++;
  Widget sep0 = XtCreateManagedWidget ("sep", xmSeparatorGadgetClass,
				       subf, arg, ac);
  ac = 0;
  
  t = XmStringCreateSimple ("ANA components");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, sep0); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  Widget anacomp = XtCreateManagedWidget ("anacomp", xmLabelGadgetClass,
					  subf, arg, ac);
  ac = 0;
  XmStringFree (t);


  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, anacomp); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNtopPosition, 49); ac++;
  Widget sep1 = XtCreateManagedWidget ("sep", xmSeparatorGadgetClass,
				      subf, arg, ac);
  ac = 0;

  // create radio box
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, subf); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 3); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNpacking, XmPACK_COLUMN); ac++;
  XtSetArg (arg[ac], XmNorientation, XmHORIZONTAL); ac++;
  XtSetArg (arg[ac], XmNradioBehavior, TRUE); ac++;
  XtSetArg (arg[ac], XmNradioAlwaysOne, TRUE); ac++;
  XtSetArg (arg[ac], XmNnumColumns, 10); ac++;
  Widget rowcol = XtCreateWidget ("anaLogRow", xmRowColumnWidgetClass,
				  _w, arg, ac);
  ac = 0;
  
  // create all toggle button gadget
  for (int i = 0; i < RCXUI_MAX_ANAS; i++) {
    t = XmStringCreateSimple ("        ");
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    anas_[i] = XtCreateWidget ("toggles", xmToggleButtonGadgetClass,
			       rowcol, arg, ac);
    ac = 0;
    XmStringFree (t);
  }
  
  // create bottom action form
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  Widget actionForm = XtCreateWidget ("anaLogActFrom", xmFormWidgetClass,
				      _w, arg, ac);
  ac = 0;

  t = XmStringCreateSimple ("Ok");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 70); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 90); ac++;
  XtSetArg (arg[ac], XmNshowAsDefault, 1); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  Widget ok = XtCreateManagedWidget ("anaLogOk", xmPushButtonGadgetClass,
				     actionForm, arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("Apply");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 40); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 60); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;    
  Widget apply = XtCreateManagedWidget ("anaLogApply", xmPushButtonGadgetClass,
					actionForm, arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("Dismiss");
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 10); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNrightPosition, 30); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;    
  Widget cancel = XtCreateManagedWidget ("anaLogCancel", 
					 xmPushButtonGadgetClass,
					 actionForm, arg, ac);
  ac = 0;
  XmStringFree (t);  

  // create text field entry
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, actionForm); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  XtSetArg (arg[ac], XmNblinkRate, 0); ac++;
  XtSetArg (arg[ac], XmNvalue, ""); ac++;
  file_ = XtCreateManagedWidget ("anaLogFile", xmTextFieldWidgetClass,
				 _w, arg, ac);
  ac = 0;

  // create a label associated with text entry
  t = XmStringCreateSimple ("File name:");
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, file_); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;  
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
  Widget fileLabel = XtCreateManagedWidget ("anaLogFileLabel",
					    xmLabelGadgetClass,
					    _w, arg, ac);
  ac = 0;
  XmStringFree (t);
    
  // create a toggle button
  t = XmStringCreateSimple ("Log events to a file");
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, fileLabel); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_POSITION); ac++;
  XtSetArg (arg[ac], XmNleftPosition, 10); ac++;
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  toggle_ = XtCreateManagedWidget ("anaLogToggle", xmToggleButtonGadgetClass,
				    _w, arg, ac);
  ac = 0;
  XmStringFree (t);

  // create a sperator
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, toggle_); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 2); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 2); ac++;
  Widget sep = XtCreateManagedWidget ("sep", xmSeparatorGadgetClass,
				      _w, arg, ac);
  ac = 0; 

  // finally attach radio box to separator
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNbottomWidget, sep); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 3); ac++;
  XtSetValues (rowcol, arg, ac);
  ac = 0;

  // set default button to ok
  defaultButton (ok);

  // add all callbacks to push buttons
  XtAddCallback (ok, XmNactivateCallback,
		 (XtCallbackProc)&(rcAnaLogDialog::okCallback),
		 (XtPointer)this);
  XtAddCallback (apply, XmNactivateCallback,
		 (XtCallbackProc)&(rcAnaLogDialog::applyCallback),
		 (XtPointer)this);
  XtAddCallback (cancel, XmNactivateCallback,
		 (XtCallbackProc)&(rcAnaLogDialog::cancelCallback),
		 (XtPointer)this);

  XtManageChild (subf);
  XtManageChild (rowcol);
  XtManageChild (actionForm);
}

void
rcAnaLogDialog::popup (void)
{
  Arg arg[10];
  int ac = 0;

  /*Sergey: define 'i' here, was defined in 'for' */
  int i;
  
  // first unmanage all old toggle buttons
  for (/*int*/ i = 0; i < numAnas_; i++)
    XtUnmanageChild (anas_[i]);

  // get latest all ana components
  codaSlistIterator ite (*anaList_);
  daqNetData* ndata = 0;

  i = 0;
  XmString t;
  for (ite.init(); !ite; ++ite) {
    ndata = (daqNetData *) ite ();
    t = XmStringCreateSimple (ndata->name ());
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetValues (anas_[i], arg, ac);
    XtManageChild (anas_[i]);
    XmStringFree (t); 
    ac = 0;
    if (i == 0) { // set first one as the selected
      XmToggleButtonGadgetSetState (anas_[i], 1, FALSE);
      XmTextFieldSetString (file_, (char *)(*ndata));
      if (::strcmp ((char *)(*ndata), "NOLOG") != 0)
       XmToggleButtonGadgetSetState (toggle_, 0, FALSE);
    }
    i++;
  }
  numAnas_ = i;

  XcodaForm::popup (); 
}

void
rcAnaLogDialog::changeAnaLogInfo (void)
{
  // get network handler
  rcClient& client = netHandler_.clientHandler ();

  /*Sergey: define 'i' here, was defined in 'for' */
  int i;

  // get current selection of ana components
  for (/*int*/ i = 0; i < numAnas_; i++) {
    if (XmToggleButtonGadgetGetState (anas_[i]))
      break;
  }

  // check list to find the ana
  int j = 0;
  codaSlistIterator ite (*anaList_);
  daqNetData* ndata = 0;
  for (ite.init (); !ite; ++ite) {
    ndata = (daqNetData *)ite ();
    if (j == i)
      break;
    j++;
  }
  char *compname = ndata->name ();
  
  // get toggle button state: if it is not set, do not care the file entry
  if (XmToggleButtonGadgetGetState (toggle_)) {
    char *log = XmTextFieldGetString (file_);
    if (!log || !*log) {
      button_->reportErrorMsg ("Need file name to write events");
      return;
    }
    daqData sdata (compname, DYN_ANA_LOG, log);
    if (client.setValueCallback (sdata, 
				 (rcCallback)&(rcAnaLogDialog::anaLogCallback),
				 (void *)this) != CODA_SUCCESS) {
      button_->reportErrorMsg ("Cannot send set value command"); 
      return;
    }
    XtFree (log);
  }
  else {
    char log[40];
    ::strcpy (log, "NOLOG");
    daqData sdata (compname, DYN_ANA_LOG, log);
    if (client.setValueCallback (sdata, 
				 (rcCallback)&(rcAnaLogDialog::anaLogCallback),
				 (void *)this) != CODA_SUCCESS) {
      button_->reportErrorMsg ("Cannot send set value command"); 
      return;
    }
  }
}

void
rcAnaLogDialog::anaLogCallback (int status, void* arg, daqNetData* data)
{
  if (status != CODA_SUCCESS) {
    rcAnaLogDialog* obj = (rcAnaLogDialog *)arg;
    obj->button_->reportErrorMsg ("Changing name of an ANA log file failed !");
  }
}
#endif

void
rcAnaLogDialog::okCallback (Widget w, XtPointer data, XmAnyCallbackStruct* )
{
  rcAnaLogDialog* obj = (rcAnaLogDialog *)data;

  obj->changeAnaLogInfo ();
  obj->popdown ();
}

void
rcAnaLogDialog::applyCallback (Widget w, XtPointer data, XmAnyCallbackStruct* )
{
  rcAnaLogDialog* obj = (rcAnaLogDialog *)data;

  obj->changeAnaLogInfo ();
}

void
rcAnaLogDialog::cancelCallback (Widget w, XtPointer data, 
				XmAnyCallbackStruct* )
{
  rcAnaLogDialog* obj = (rcAnaLogDialog *)data;

  obj->popdown ();
}

rcAnaLogDialog::~rcAnaLogDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcAnaLogDialog Class Object\n");
#endif
  // anaList_ is just a pointer
}

