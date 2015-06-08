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
//      Implementation of rcHelpAboutDialog Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcHelpAboutDialog.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <XcodaXpmLabel.h>
#include <rcHelpAbout.h>
#include <codaConst.h>
#include <rcSvcProtocol.h>

#include "rcHelpAboutDialog.h"

#include <RCLogo.xpm>

rcHelpAboutDialog::rcHelpAboutDialog (rcHelpAbout* base,
				      char* name,
				      char* title)
:XcodaFormDialog (base->dialogBaseWidget (), name, title),
base_ (base), log_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcHelpAboutDialog Class Object\n");
#endif
  // empty
}

rcHelpAboutDialog::~rcHelpAboutDialog (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcHelpAboutDialog Class Object\n");
#endif
  // empty
}

void
rcHelpAboutDialog::createFormChildren (void)
{
  Arg arg[20];
  int ac = 0;

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 10); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 10); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  Widget leftform = XtCreateWidget ("left_form", xmFormWidgetClass,
				    _w, arg, ac);
  ac = 0;

  log_ = new XcodaXpmLabel (leftform, "       ", RCLogo_xpm);
  log_->init ();

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 10); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetValues (log_->baseWidget (), arg, ac);
  ac = 0;

  XmString t = XmStringCreateSimple ("RunControl");

  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNtopWidget, log_->baseWidget ()); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 10); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  Widget label = XtCreateManagedWidget ("label", xmLabelGadgetClass,
					leftform, arg, ac);
  ac = 0;
  XmStringFree (t);

  // create a separator
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 2); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 2); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, leftform); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 10); ac++;
  XtSetArg (arg[ac], XmNorientation, XmVERTICAL); ac++;
  Widget sep = XtCreateManagedWidget ("sep", xmSeparatorGadgetClass,
				      _w, arg, ac);
  ac = 0;


  char msg[256], vstr[128];

  ::sprintf (msg, "Motif-based RunControl for Physics Experiments\n\n");
  ::sprintf (vstr, "RunControl Version %3.1f (CODA %s)\n\n",
	     (float)(CODA_RC_VERSION)/1000.0, CODA_VERSION);
  ::strcat  (msg, vstr);
  ::sprintf (vstr, "Developed at TJNAF, by Data Acquisition Group");
  ::strcat  (msg, vstr);

  t = XmStringCreateLtoR (msg, XmSTRING_DEFAULT_CHARSET);
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, sep) ;ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  Widget label1 = XtCreateManagedWidget ("label", xmLabelGadgetClass,
					 _w, arg, ac);
  ac = 0;
  XmStringFree (t);

  t = XmStringCreateSimple ("ok");
  XtSetArg (arg[ac], XmNlabelString, t); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNtopOffset, 5); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
  XtSetArg (arg[ac], XmNleftWidget, label1); ac++;
  XtSetArg (arg[ac], XmNleftOffset, 5); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightOffset, 5); ac++;
  Widget ok = XtCreateManagedWidget ("ok", xmPushButtonGadgetClass,
				     _w, arg, ac);
  ac = 0;
  XmStringFree (t);

  XtAddCallback (ok, XmNactivateCallback,
		 (XtCallbackProc)&(rcHelpAboutDialog::okCallback),
		 (XtPointer)this);

  XtManageChild (leftform);
}

void
rcHelpAboutDialog::okCallback (Widget w, 
			       XtPointer data,
			       XmAnyCallbackStruct* cbs)
{
  rcHelpAboutDialog* dialog = (rcHelpAboutDialog *)data;

  dialog->popdown ();
}


  
