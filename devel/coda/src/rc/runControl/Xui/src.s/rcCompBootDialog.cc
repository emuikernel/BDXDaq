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
//      Implementation of rcCompBootDialog Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcCompBootDialog.cc,v $
//   Revision 1.3  1998/05/28 17:46:53  heyes
//   new GUI look
//
//   Revision 1.2  1998/04/08 18:31:14  heyes
//   new look and feel GUI
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <rcCompBootButton.h>
#include <daqCompBootStruct.h>
#include "rcCompBootDialog.h"

extern Widget bootFr;
rcCompBootDialog::rcCompBootDialog (char* name, char* title,
				    rcClientHandler& handler)
:XcodaForm (bootFr, name, title),
 netHandler_ (handler), num_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcCompBootDialog Class Object\n");
#endif
  // empty
}

rcCompBootDialog::~rcCompBootDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcCompBootDialog Class Object\n");
#endif
  if (num_ > 0) {
    for (int i = 0; i < num_; i++)
      delete []names_[i];
  }
}

void
rcCompBootDialog::createFormChildren (void)
{
  Arg arg[20];
  int ac = 0;


  // create a row column widget with two row in it
  XtSetArg (arg[ac], XmNpacking, XmPACK_COLUMN); ac++;
  XtSetArg (arg[ac], XmNnumColumns, 5); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
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

  XtManageChild (row_);

}

void
rcCompBootDialog::popdown (void)
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
rcCompBootDialog::popup (void)
{
  char** components = 0;
  long* autoboot = 0;
  long  numComp = netHandler_.compBootInfo (components, autoboot);

  assert (numComp < RCXUI_MAX_COMPONENTS);

  Arg arg[10];
  int ac = 0;
  XmString t;

  for (int i = 0; i < numComp; i++) {
    names_[i] = new char[::strlen (components[i]) + 1];
    ::strcpy (names_[i], components[i]);

    t = XmStringCreateSimple (names_[i]);
    XtSetArg (arg[ac], XmNlabelString, t); ac++;
    XtSetArg (arg[ac], XmNset, autoboot[i]); ac++;
    toggles_[i] = XtCreateWidget ("toggles", xmToggleButtonGadgetClass,
				  row_, arg, ac);

    XtAddCallback (toggles_[i], XmNvalueChangedCallback,
		   (XtCallbackProc)&(rcCompBootDialog::okCallback), 
		   (void *)this);
    ac = 0;
    XmStringFree (t);

    // manage child
    XtManageChild (toggles_[i]);
  }
  num_ = numComp;
}


void
rcCompBootDialog::cancelCallback (Widget, XtPointer data, 
				  XmAnyCallbackStruct *)
{
  rcCompBootDialog* obj = (rcCompBootDialog *)data;

  obj->popdown ();
}

void
rcCompBootDialog::okCallback (Widget, XtPointer data,
			      XmAnyCallbackStruct *)
{
  rcCompBootDialog* obj = (rcCompBootDialog *)data;

  daqCompBootStruct bootInfo;
  int state;

  for (int i = 0; i < obj->num_; i++) {
    state = XmToggleButtonGadgetGetState (obj->toggles_[i]);
    if (state) 
      bootInfo.insertInfo (obj->names_[i], 1);
    else
      bootInfo.insertInfo (obj->names_[i], 0);
  }

  // get network handler
  rcClient& client = obj->netHandler_.clientHandler (); 

  daqData data2 (client.exptname (), "command", (daqArbStruct *)&bootInfo);

  client.sendCmdCallback (DAAUTOBOOT_INFO, data2,
			  (rcCallback)&(rcCompBootDialog::compBootInfoCbk),
			  (void *)obj);

}

void
rcCompBootDialog::compBootInfoCbk (int status, void* arg, daqNetData *)
{
  
}

    

  
  
  

							 
  
