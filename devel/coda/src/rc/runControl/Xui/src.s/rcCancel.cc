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
//      Implementation of Cancel Command Button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcCancel.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:28  chen
//   run control source
//
//
#include <rcNetStatus.h>
#include <rcButtonPanel.h>
#include <rcAudioOutput.h>
#include "rcCancel.h"

#define RC_CANCEL_NAME "  Cancel  "
#define RC_CANCEL_MSG  "Cancel Transition"

rcCancel::rcCancel (Widget parent, rcButtonPanel* panel,
		    rcClientHandler& handler)
:rcComdButton (parent, RC_CANCEL_NAME, RC_CANCEL_MSG, panel, handler)
{
#ifdef _TRACE_OBJECTS
  printf ("              Create rcCancel Class Object\n");
#endif
  // empty
}

rcCancel::~rcCancel (void)
{
#ifdef _TRACE_OBJECTS
  printf ("              Delete rcCancel Class Object\n");
#endif
  // empty
}

void
rcCancel::doit (void)
{
  rcAudio ("cancel this transition");

  assert (stWin_);

  // get network handler first
  rcClient& client = netHandler_.clientHandler ();
  daqData data ("RCS", "command", (int)DACANCELTRAN);
  if (client.sendCmdCallback (DACANCELTRAN, data,
			      (rcCallback)&(rcCancel::cancelCallback),
			      (void *)this) != CODA_SUCCESS)
    reportErrorMsg ("Cannot send download command to the server.");
}

void
rcCancel::undoit (void)
{
  // empty
}

void
rcCancel::cancelCallback (int status, void* arg, daqNetData* data)
{
  rcCancel* obj = (rcCancel *)arg;

  if (status != CODA_SUCCESS)
    obj->reportErrorMsg ("Cancel a transaction failed !!!\n");
}

