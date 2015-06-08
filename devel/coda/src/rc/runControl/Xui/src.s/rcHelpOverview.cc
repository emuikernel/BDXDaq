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
//      Implementation of rcHelpOverview Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcHelpOverview.cc,v $
//   Revision 1.2  1996/12/04 18:32:27  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaApp.h>

#include <rcHelpContext.h>
#include <rcInfoPanel.h>

#include "rcHelpOverview.h"

rcHelpOverview::rcHelpOverview (char* name, int active)
:codaCbtnComd (name, active), infoPanel_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcHelpOverview Class Object\n");
#endif
}

rcHelpOverview::~rcHelpOverview (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcHelpOverview Class Object\n");
#endif
  delete connect_;
  delete disconnect_;
  delete close_;
  delete exit_;
  delete master_;
  delete config_;
  delete download_;
  delete reset_;
  delete evlimit_;
  delete runnum_;  
  delete repbug_;
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#else
  delete load_;
  delete script_;
#endif
}

void
rcHelpOverview::infoPanel (rcInfoPanel* panel)
{
  infoPanel_ = panel;
}

const Widget
rcHelpOverview::dialogBaseWidget (void)
{
  assert (infoPanel_);
  return infoPanel_->baseWidget ();
}  

void
rcHelpOverview::config (int state)
{
  if (state != DA_NOT_CONNECTED) {
    connect_->deactivate ();
    disconnect_->activate ();
    close_->activate ();
    exit_->activate ();
    master_->activate ();
    config_->activate ();
    download_->activate ();
    reset_->activate ();
    evlimit_->activate ();
    runnum_->activate ();    
    repbug_->activate ();
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#else
    load_->activate ();
    script_->activate ();
#endif
  }
  else {
    connect_->activate ();
    disconnect_->deactivate ();
    close_->deactivate ();
    exit_->deactivate ();
    master_->deactivate ();
    config_->deactivate ();
    download_->deactivate ();
    reset_->deactivate ();
    evlimit_->deactivate ();
    runnum_->deactivate ();    
    repbug_->activate ();
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#else
    load_->deactivate ();
    script_->deactivate ();
#endif
  }
}

void
rcHelpOverview::setHelpContext (void)
{
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)
  connect_ = new rcHelpContext ("To connect to a server", 1, 0, this);
  disconnect_ = new rcHelpContext ("To disconnect from a server", 0, 1, this);
  close_ = new rcHelpContext ("To close the run control GUI", 0, 2, this);
  exit_ = new rcHelpContext ("To exit from a server", 0, 3, this);
  master_ = new rcHelpContext ("To change mastership", 0, 4, this);
  config_ = new rcHelpContext ("To configure a run", 0, 5, this);
  download_ = new rcHelpContext ("To download a run", 0, 6, this);
  reset_ = new rcHelpContext ("To reset a run", 0, 7, this);
  evlimit_ = new rcHelpContext ("To change event/data limit", 0, 8, this);
  runnum_ = new rcHelpContext ("To change run number", 0, 9, this);
  repbug_ = new rcHelpContext ("To report bugs", 1, 10, this);

  registerCommand (connect_);
  registerCommand (disconnect_);
  registerCommand (close_);
  registerCommand (exit_);
  registerCommand (master_);
  registerCommand (config_);
  registerCommand (download_);
  registerCommand (reset_);
  registerCommand (evlimit_);
  registerCommand (runnum_);
  registerCommand (repbug_);

#else
  connect_ = new rcHelpContext ("To connect to a server", 1, 0, this);
  disconnect_ = new rcHelpContext ("To disconnect from a server", 0, 1, this);
  close_ = new rcHelpContext ("To close the run control GUI", 0, 2, this);
  exit_ = new rcHelpContext ("To exit from a server", 0, 3, this);
  load_ = new rcHelpContext ("To load database to the server", 0, 4, this);
  master_ = new rcHelpContext ("To change mastership", 0, 5, this);
  config_ = new rcHelpContext ("To configure a run", 0, 6, this);
  download_ = new rcHelpContext ("To download a run", 0, 7, this);
  reset_ = new rcHelpContext ("To reset a run", 0, 8, this);
  evlimit_ = new rcHelpContext ("To change event/data limit", 0, 9, this);
  runnum_ = new rcHelpContext ("To change run number", 0, 10, this);
  script_ = new rcHelpContext ("To run a script", 0, 11, this);
  repbug_ = new rcHelpContext ("To report bugs", 1, 12, this);

  registerCommand (connect_);
  registerCommand (disconnect_);
  registerCommand (close_);
  registerCommand (exit_);
  registerCommand (load_);
  registerCommand (master_);
  registerCommand (config_);
  registerCommand (download_);
  registerCommand (reset_);
  registerCommand (evlimit_);
  registerCommand (runnum_);
  registerCommand (script_);
  registerCommand (repbug_);
#endif
}

