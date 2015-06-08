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
//      Implementation of rcExit Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcExit.cc,v $
//   Revision 1.2  1999/12/09 21:29:01  rwm
//   Do not do clean memory on linux - bug in Xlibs?
//
//   Revision 1.1.1.1  1996/10/11 13:39:23  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaApp.h>

#include <rcNetStatus.h>
#include <rcInfoPanel.h>
#include <rcExitDialog.h>

#include "rcExit.h"

rcExit::rcExit (char* name, int active,
		char* acc, char* acc_text, 
		rcClientHandler& handler)
:rcMenuComd (name, active, acc, acc_text, handler), dialog_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcExit Class Object\n");
#endif
}

rcExit::~rcExit (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcExit Class Object\n");
#endif
  // netSt_ and rcipanel_ will be deleted by Xt mechanism
}

void
rcExit::doit (void)
{
  if (!netHandler_.connected ()) 
    exitRunControl ();
  
  if (!dialog_) {
    dialog_ = new rcExitDialog (this, "exitdialog", "Exit RunControl",
				netHandler_);
    dialog_->init ();
    dialog_->alignment (XmALIGNMENT_CENTER);
  }
  // set message
  char temp[256];
  if (netHandler_.numConnectedClients () == 1) {
    sprintf (temp, "This kills RunControl server and all remote processes,\n");
    strcat  (temp, "Are you sure?");
    dialog_->setMessage (temp);
  }
  else {
    sprintf (temp, "This leaves RunControl server still running,\n");
    strcat  (temp, "Are you sure?");
    dialog_->setMessage (temp);
  }    
  if (dialog_->isMapped ())
    dialog_->popdown ();
  dialog_->popup ();
}

void
rcExit::undoit (void)
{
  // empty
}

void
rcExit::exitRunControl (void)
{
  if (netSt_)
    netSt_->stop ();
  if (rcipanel_) 
    rcipanel_->readyToExit ();
  if (netHandler_.connected () && netHandler_.numConnectedClients () == 1) {
    netHandler_.killServer ();
    XtAppAddTimeOut (XtWidgetToApplicationContext (dialogBaseWidget()),
		     3000, &(rcExit::timerCallback),
		     (XtPointer)this);
  } else {
#ifndef linux
    // clean up memory
    theApplication->cleanMemory ();
#else
    exit(EXIT_SUCCESS);
#endif
  }
}

void
rcExit::timerCallback (XtPointer data, XtIntervalId *)
{
#ifndef linux
  // clean up memory
  theApplication->cleanMemory ();
#else
  exit(EXIT_SUCCESS);
#endif
}
