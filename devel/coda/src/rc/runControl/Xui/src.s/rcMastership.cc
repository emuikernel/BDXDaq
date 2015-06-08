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
//      RunControl Mastership Display
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMastership.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#include <pixmaps/lock_no_key.xpm>
#include <rcMastershipButton.h>
#include <XcodaXpmLabel.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include "rcMastership.h"

rcMastership::rcMastership (Widget parent, char* name, 
			    rcClientHandler& handler)
:XcodaUi (name), rcPanel(), parent_ (parent), netHandler_ (handler),
 label_ (0), button_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Create rcMastership Class Object\n");
#endif
  netHandler_.addPanel (this);
}

rcMastership::~rcMastership (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                   Delete rcMastership Class Object\n");
#endif
  netHandler_.removePanel (this);
  // button_ has to be deleted since it is not derived from XcodaUi
  delete button_;
}

void
rcMastership::init (void)
{
  Arg arg[20];
  int ac = 0;
  
  XtSetArg (arg[ac], XmNwidth, 45); ac++;
  XtSetArg (arg[ac], XmNheight, 25); ac++;
  _w = XtCreateWidget (_name, xmFormWidgetClass, parent_, arg, ac);
  ac = 0;
  
  button_ = new rcMastershipButton (_w, "mastershipButton", netHandler_);
  button_->init ();
  label_ = new XcodaXpmLabel (_w, "mastershipLabel", lock_no_key_xpm,
			      "background");
  label_->init ();

  XtSetArg (arg[ac], XmNshadowThickness, 2); ac++;
  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetValues (label_->baseWidget(), arg, ac);
  ac = 0;

  XtSetArg (arg[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
  XtSetArg (arg[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
  XtSetValues (button_->baseWidget(), arg, ac);
  ac = 0;

  // install destroy handler
  installDestroyHandler ();
}

void
rcMastership::config (int st)
{
  if (st == DA_NOT_CONNECTED)
    unmanage ();
  else
    manage ();
}

void
rcMastership::configMastership (int type)
{
  if (type == CODA_ISMASTER || type == CODA_NOMASTER) {
    button_->manage ();
    label_->unmanage ();
  }
  else {
    label_->manage ();
    button_->unmanage ();
  }
}

    
    
  
