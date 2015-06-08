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
//      Implementation of RunControl Top Level Window
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcTopW.cc,v $
//   Revision 1.4  1998/06/18 12:20:43  heyes
//   new GUI ready I think
//
//   Revision 1.3  1997/10/20 12:45:53  heyes
//   first tag for B
//
//   Revision 1.2  1997/07/08 14:59:17  heyes
//   deep trouble
//
//   Revision 1.1.1.1  1996/10/11 13:39:24  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <rcMenuWindow.h>
#include "rcTopW.h"
#include <XmHTML.h>

//Sergey extern "C" _XmHTMLRaiseFormWidgets(Widget html);;

rcTopW::rcTopW (char* name, rcClientHandler& handler)
:XcodaTopLevel (name), window_ (0), netHandler_ (handler)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcTopW Class Object\n");
#endif
  // empty
}

rcTopW::~rcTopW (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcTopW Class Object\n");
#endif
  // menu window will be destroyed by Xt destroy callback
  if (netHandler_.connected ())
    netHandler_.clientHandler().disconnect ();
}

Widget
rcTopW::CreateBaseWidget (Widget parent)
{
  Arg arg[20];
  int ac = 0;
  char	  help_file[1000];
  
  ac = 0;
  XtSetArg (arg[ac], XtNheight, HeightOfScreen(XtScreen(parent))); ac++;
  XtSetArg (arg[ac], XtNwidth, WidthOfScreen(XtScreen(parent))); ac++;
  XtSetValues (parent, arg, ac);

  window_ = new rcMenuWindow (parent, "rcTopWindow", netHandler_);
  window_->init ();

  return window_->baseWidget ();
}

Widget
rcTopW::RightWindow ()
{
  return window_->rframe_;
}
