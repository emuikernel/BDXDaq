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
//      Implementation of rcUpdateInterval Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcUpdateInterval.cc,v $
//   Revision 1.2  1998/05/28 17:47:05  heyes
//   new GUI look
//
//   Revision 1.1.1.1  1996/10/11 13:39:27  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaApp.h>

#include <codaComdXInterface.h>

#include <rcUpdateIntervalDialog.h>
#include <rcNetStatus.h>
#include <rcInfoPanel.h>
#include "rcUpdateInterval.h"

rcUpdateInterval::rcUpdateInterval (char* name, int active,
				    char* acc, char* acc_text,
				    rcClientHandler& handler)
:rcMenuComd (name, active, acc, acc_text, handler), dialog_ (0), interval_ (1)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcUpdateInterval Class Object\n");
#endif
}

rcUpdateInterval::~rcUpdateInterval (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcUpdateInterval Class Object\n");
#endif
  // dialog_ will be destroyed by Xt Mechanism
}

void
rcUpdateInterval::doit (void)
{
  
  if (dialog_->isMapped ())
    dialog_->popdown ();
  dialog_->popup ();
}

void
rcUpdateInterval::undoit (void)
{
  // empty
}

void
rcUpdateInterval::setUpdateInterval (int interval)
{
  interval_ = interval;
  if (dialog_)
    dialog_->setUpdateInterval (interval);
}

int
rcUpdateInterval::updateInterval (void) const
{
  return interval_;
}


void
rcUpdateInterval::sendUpdateInterval (int newval)
{
  // get client handler
  rcClient& client = netHandler_.clientHandler ();

  daqData data (client.exptname (), "updateInterval", newval);
  if (client.setValueCallback (data, 
			       (rcCallback)&(rcUpdateInterval::simpleCallback),
			       (void *)this) != CODA_SUCCESS) {
    reportErrorMsg ("Cannot send new update interval value to the server !");
    return;
  }
}

void
rcUpdateInterval::simpleCallback (int status, void* arg, daqNetData* )
{
  rcUpdateInterval* obj = (rcUpdateInterval *)arg;
  
  if (status != CODA_SUCCESS) {
    obj->reportErrorMsg ("Setting new update interval to the server failed !");
    return;
  }
}

