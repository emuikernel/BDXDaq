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
//      CODA daqState Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqState.cc,v $
//   Revision 1.2  1997/09/16 12:28:43  heyes
//   make comms async
//
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#include "daqState.h"

daqState* codaDaqState = 0;

static _daq_state states_[]=
{
  {"dormant", CODA_DORMANT},
  {"booting", CODA_BOOTING},
  {"booted", CODA_BOOTED},
  {"configuring", CODA_CONFIGURING},
  {"configured", CODA_CONFIGURED},
  {"downloading", CODA_DOWNLOADING},
  {"downloaded", CODA_DOWNLOADED},
  {"prestarting", CODA_PRESTARTING},
  {"paused", CODA_PAUSED},
  {"pausing", CODA_PAUSING},
  {"activating", CODA_ACTIVATING},
  {"active", CODA_ACTIVE},
  {"ending", CODA_ENDING},
  {"verifying", CODA_VERIFYING},
  {"verified", CODA_VERIFIED},
  {"prestarted", CODA_PRESTARTED},
  {"resetting", CODA_RESETTING},
  {"disconnected", CODA_DISCONNECTED},
  {"connecting", CODA_CONNECTING},
  {"busy", CODA_BUSY},
};

daqState::daqState (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Create daqState Class Object\n");
#endif
  codaDaqState = this;
}

daqState::~daqState (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete daqState Class Object\n");
#endif
  codaDaqState = 0;
}

const char*
daqState::stateString (int state) const
{
  if (state < CODA_LOW_STATE || state > CODA_HIGH_STATE)
    return "UNKNOWN";
  for (int i = 0; i < CODA_HIGH_STATE - CODA_LOW_STATE + 1; i++) {
    if (state == states_[i].state_)
      return states_[i].string_;
  }
}
