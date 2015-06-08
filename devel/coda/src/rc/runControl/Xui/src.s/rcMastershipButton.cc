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
//      Implementation of RunControl Mastership Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMastershipButton.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:26  chen
//   run control source
//
//
#include <pixmaps/lock_and_key.xpm>
#include <pixmaps/unlock.xpm>
#include <XcodaXpmtbtnInterface.h>
#include <XcodaXpm.h>

#include "rcMastershipButton.h"

rcMastershipButton::rcMastershipButton (Widget parent, char* name,
					rcClientHandler& handler)
:codaXpmtbtnComd (name, unlock_xpm, lock_and_key_xpm, 1, "background"),
 parent_ (parent), netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("                  Create rcMastershipButton Class Object\n");
#endif
  w_ = 0;
}

rcMastershipButton::~rcMastershipButton (void)
{
#ifdef _TRACE_OBJECTS
  printf ("                  Delete rcMastershipButton Class Object\n");
#endif
}

void
rcMastershipButton::init (void)
{
  Arg arg[10];
  int ac = 0;

  XcodaXpmtbtnInterface *btn;

  createXInterface (parent_);
  btn = (XcodaXpmtbtnInterface *)getXInterface ();
  w_ = btn->baseWidget ();
  // set some X resources
  XtSetArg    (arg[ac], XmNshadowThickness, 0); ac++;
  XtSetValues (w_, arg, ac);
  ac = 0;
}

const Widget
rcMastershipButton::baseWidget (void)
{
  return w_;
}

void
rcMastershipButton::doit (void)
{
  if (state () > 0) { // clicked to set ie. become master
    netHandler_.requestMastership ();
  }
  else {
    netHandler_.giveupMastership ();
  }
}

void
rcMastershipButton::undoit (void)
{
  // empty
}

void
rcMastershipButton::manage (void)
{
  XtManageChild (w_);
}

void
rcMastershipButton::unmanage (void)
{
  XtUnmanageChild (w_);
}

