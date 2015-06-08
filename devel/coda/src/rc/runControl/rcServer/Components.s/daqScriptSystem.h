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
//      Data Acquisition Script System
//
//      This is a system that holds all scripts that are executed
//      upon success of state transitions
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqScriptSystem.h,v $
//   Revision 1.1  1997/05/16 16:42:48  chen
//   add global scripting capabilities
//
//
//
#ifndef _CODA_SCRIPT_SYSTEM_H
#define _CODA_SCRIPT_SYSTEM_H

#include <stdio.h>
#include <string.h>
#include <daqConst.h>

class daqScriptSystem
{
public:
  // constructor and destructor
  daqScriptSystem  (void);
  ~daqScriptSystem (void);

  // add a script associated with an action
  int addScript    (int action, char *script);
  // get a script for an action
  char* script     (int action);

  // clean up all scripts
  void  cleanup    (void);

private:
  // all scripts
  char* scripts_[CODA_NUM_ACTIONS];

  // deny access to copy and assignment operations
  daqScriptSystem (const daqScriptSystem& sys);
  daqScriptSystem& operator = (const daqScriptSystem& sys);
};
#endif
