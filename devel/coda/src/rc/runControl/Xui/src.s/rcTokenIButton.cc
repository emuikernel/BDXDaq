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
//      Implementation of rcTokenIButton Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcTokenIButton.cc,v $
//   Revision 1.2  1996/12/04 18:32:32  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#if defined (_CODA_2_0_T) || defined (_CODA_2_0)

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <XcodaApp.h>

#include <codaComdXInterface.h>

#include <rcTokenIDialog.h>
#include <rcNetStatus.h>
#include <rcInfoPanel.h>
#include "rcTokenIButton.h"

rcTokenIButton::rcTokenIButton (char* name, int active,
				char* acc, char* acc_text,
				rcClientHandler& handler)
:rcMenuComd (name, active, acc, acc_text, handler), dialog_ (0), interval_ (1)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcTokenIButton Class Object\n");
#endif
}

rcTokenIButton::~rcTokenIButton (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcTokenIButton Class Object\n");
#endif
  // dialog_ will be destroyed by Xt Mechanism
}

void
rcTokenIButton::doit (void)
{
  if (!dialog_) {
    dialog_ = new rcTokenIDialog (this, "Token Interval Dialog", 
				  "Token Interval",
				  netHandler_);
    dialog_->init ();
    dialog_->setMessage ("Enter token interval value");
    dialog_->setTokenInterval (netHandler_.tokenInterval() );
  }
  if (dialog_->isMapped ())
    dialog_->popdown ();
  dialog_->popup ();
}

void
rcTokenIButton::undoit (void)
{
  // empty
}

void
rcTokenIButton::setTokenInterval (int interval)
{
  if (dialog_)
    dialog_->setTokenInterval (interval);
}

int
rcTokenIButton::tokenInterval (void) const
{
  return netHandler_.tokenInterval ();
}

void
rcTokenIButton::sendTokenInterval (int newval)
{
  // get client handler
  rcClient& client = netHandler_.clientHandler ();

  daqData data (client.exptname (), "tokenInterval", newval);
  if (client.setValueCallback (data, 
			       (rcCallback)&(rcTokenIButton::setTICallback),
			       (void *)this) != CODA_SUCCESS) {
    reportErrorMsg ("Cannot send new token interval value to the server !");
    return;
  }
}

void
rcTokenIButton::setTICallback (int status, void* arg, daqNetData* )
{
  rcTokenIButton* obj = (rcTokenIButton *)arg;
  
  if (status != CODA_SUCCESS) {
    obj->reportErrorMsg ("Setting new token interval to the server failed !");
    return;
  }
}
#endif
