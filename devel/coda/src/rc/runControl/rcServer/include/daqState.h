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
//      Data Acquisition State And Conrresponding String
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqState.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#ifndef _CODA_DAQ_STATE_H
#define _CODA_DAQ_STATE_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <daqConst.h>

typedef struct _daq_state
{
  char *string_;
  int  state_;
}_daq_state;

class daqState
{
public:
  // constructor and destructor
  daqState (void);
  ~daqState (void);

  // return string value associated with state
  const char* stateString (int state) const;
};

extern daqState* codaDaqState;

#endif
