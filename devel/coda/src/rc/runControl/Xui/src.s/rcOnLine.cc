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
//      Implementation of Button Feed back option button
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcOnLine.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:25  chen
//   run control source
//
//
#include "rcOnLine.h"

rcOnLine::rcOnLine (char* name, int active,
		    char* acc, char* acc_text,
		    int state,
		    rcClientHandler& handler)
:rcMenuTogComd (name, active, acc, acc_text, state, handler)
{
#ifdef _TRACE_OBJECTS
  printf ("               Create rcOnLine class object\n");
#endif
}

rcOnLine::~rcOnLine (void)
{
#ifdef _TRACE_OBJECTS
  printf ("               Delete rcOnLine class object\n");
#endif
}

void
rcOnLine::doit (void)
{
  // get network handler
  rcClient& client = netHandler_.clientHandler ();
  
  if (state () > 0) {  // after button pressed
    daqData data (client.exptname (), "command", (int)DAONLINE);
    if (client.sendCmdCallback (DAONLINE, data,
				(rcCallback)&(rcOnLine::onlineCallback),
				(void *)this) != CODA_SUCCESS) {
      reportErrorMsg ("Cannot send command of setting online to the server");
    }
  }
  else {
    daqData data (client.exptname (), "command", (int)DAOFFLINE);
    if (client.sendCmdCallback (DAOFFLINE, data,
				(rcCallback)&(rcOnLine::offlineCallback),
				(void *)this) != CODA_SUCCESS) {
      reportErrorMsg ("Cannot send command of setting offline to the server");
    }
  }
}

void
rcOnLine::undoit (void)
{
  // empty
}

void
rcOnLine::onlineCallback (int status, void* arg, daqNetData* )
{
  rcOnLine* obj = (rcOnLine *)arg;

  if (status != CODA_SUCCESS) 
    obj->reportErrorMsg ("Setting Online flag to the server failed");
}

void
rcOnLine::offlineCallback (int status, void* arg, daqNetData* )
{
  rcOnLine* obj = (rcOnLine *)arg;

  if (status != CODA_SUCCESS) 
    obj->reportErrorMsg ("Setting Offline flag to the server failed");
}
