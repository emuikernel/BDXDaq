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
//      RunControl's Parameters for monitoring all components
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMonitorParmsButton.cc,v $
//   Revision 1.2  1998/05/28 17:46:59  heyes
//   new GUI look
//
//   Revision 1.1  1996/11/04 16:16:36  chen
//   monitoring components dialog etc
//
//
#include <rcInfoPanel.h>
#include <rcMonitorParmsDialog.h>
#include "rcMonitorParmsButton.h"

rcMonitorParmsButton::rcMonitorParmsButton (char* name, int active,
					    char* acc, char* acc_text,
					    rcClientHandler& handler)
:rcMenuComd (name, active, acc, acc_text, handler), netHandler_ (handler),
 dialog_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("         Create rcMonitorParmsButton Class Object\n");
#endif
}

rcMonitorParmsButton::~rcMonitorParmsButton (void)
{
#ifdef _TRACE_OBJECTS
  printf ("         Delete rcMonitorParmsButton Class Object\n");
#endif
}

void
rcMonitorParmsButton::doit (void)
{
}

void
rcMonitorParmsButton::undoit (void)
{
  // empty
}

void
rcMonitorParmsButton::setMonitorParms (daqMonitorStruct* info)
{
  // get network handler
  rcClient& client = netHandler_.clientHandler (); 

  daqData data (client.exptname (), "command", (daqArbStruct *)info);

  if (client.sendCmdCallback (DAMONITOR_PARM, data,
		      (rcCallback)&(rcMonitorParmsButton::monitorParmsCbk),
		      (void *)this) != CODA_SUCCESS) 
    reportErrorMsg ("Cannot send monitor options to the server");
}

void
rcMonitorParmsButton::monitorParmsCbk (int status, void* arg, daqNetData *)
{
  rcMonitorParmsButton* obj = (rcMonitorParmsButton *)arg;

  if (status != CODA_SUCCESS)
    obj->reportErrorMsg ("Setting monitor options failed");
}

