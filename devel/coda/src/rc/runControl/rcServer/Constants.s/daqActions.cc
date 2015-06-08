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
//      Data Acquistion Actions and their associated string values
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqActions.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#include "daqActions.h"

daqActions* codaDaqActions = 0;

static _daq_action actions_[]=
{
  {"Connect", CODA_CONNECT_ACTION},
  {"Boot", CODA_BOOT_ACTION},
  {"Configure", CODA_CONFIGURE_ACTION},
  {"Download", CODA_DOWNLOAD_ACTION},
  {"Prestart", CODA_PRESTART_ACTION},
  {"Reset", CODA_RESET_ACTION},
  {"End", CODA_END_ACTION},
  {"Pause", CODA_PAUSE_ACTION},
  {"Go", CODA_GO_ACTION},
  {"Verify", CODA_VERIFY_ACTION},
  {"Abort", CODA_ABORT_ACTION},
  {"Terminate", CODA_TERMINATE_ACTION},
};

daqActions::daqActions (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqActions Class Objects\n");
#endif
  codaDaqActions = this;
}

daqActions::~daqActions (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqActions Class Objects\n");
#endif
  codaDaqActions = 0;
}

const char *
daqActions::actionString (int action) const
{
  if (action < CODA_CONNECT_ACTION || action > CODA_TERMINATE_ACTION)
    return 0;
  for (int i = 0; i < CODA_NUM_ACTIONS; i++) {
    if (action == actions_[i].action_)
      return actions_[i].string_;
  }
}

const int
daqActions::action (char *string) const
{
  for (int i = 0; i < CODA_NUM_ACTIONS; i++) {
    if (::strcasecmp (actions_[i].string_, string) == 0)
      return actions_[i].action_;
  }
  return CODA_ERROR;
}
