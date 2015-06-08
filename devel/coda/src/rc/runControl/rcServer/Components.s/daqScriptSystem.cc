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
//      Implementation of daqScriptSystem Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqScriptSystem.cc,v $
//   Revision 1.1  1997/05/16 16:42:48  chen
//   add global scripting capabilities
//
//
//
#include <codaConst.h>
#include "daqScriptSystem.h"

daqScriptSystem::daqScriptSystem (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqScriptSystem Class Object\n");
#endif
  for (int i = 0; i < CODA_NUM_ACTIONS; i++)
    scripts_[i] = 0;
}

daqScriptSystem::~daqScriptSystem (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqScriptSystem Class Object\n");
#endif
  for (int i = 0; i < CODA_NUM_ACTIONS; i++)
    if (scripts_[i])
      delete []scripts_[i];
}

void
daqScriptSystem::cleanup (void)
{
  for (int i = 0; i < CODA_NUM_ACTIONS; i++)
    if (scripts_[i]) {
      delete []scripts_[i];
      scripts_[i] = 0;
    }

}

int
daqScriptSystem::addScript (int action, char* script)
{
  if (action < CODA_CONNECT_ACTION || action > CODA_TERMINATE_ACTION)  
    return CODA_ERROR;

  // this script will override existing script
  if (scripts_[action])
    delete []scripts_[action];

  scripts_[action] = new char[::strlen (script) + 1];
  ::strcpy (scripts_[action], script);

  return CODA_SUCCESS;
}

char*
daqScriptSystem::script (int action)
{
  if (action < CODA_CONNECT_ACTION || action > CODA_TERMINATE_ACTION)  
    return 0;

  return scripts_[action];
}
