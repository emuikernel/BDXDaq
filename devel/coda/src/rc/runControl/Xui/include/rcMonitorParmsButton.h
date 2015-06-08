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
//      RunControl Option Menu For Monitoring Components parameters
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: rcMonitorParmsButton.h,v $
//   Revision 1.1  1996/11/04 16:16:37  chen
//   monitoring components dialog etc
//
//
//
#ifndef _RC_MONITOR_PARMS_BUTTON_H
#define _RC_MONITOR_PARMS_BUTTON_H

#include <rcMenuComd.h>
#include <rcClientHandler.h>

class rcMonitorParmsDialog;
class daqMonitorStruct;

class rcMonitorParmsButton: public rcMenuComd
{
public:
  rcMonitorParmsButton (char* name, int active,
			char* acc, char* acc_text,
			rcClientHandler& handler);
  virtual ~rcMonitorParmsButton (void);

  // operation
  void    setMonitorParms (daqMonitorStruct* info);

  // class name
  virtual const char* className (void) const {return "rcMonitorParmsButton";}

protected:
  virtual void doit (void);
  virtual void undoit (void);

  // setup boot info callbacks
  static void monitorParmsCbk (int status, void* arg, daqNetData* data);

private:
  rcClientHandler& netHandler_;
  rcMonitorParmsDialog* dialog_;
};

#endif
