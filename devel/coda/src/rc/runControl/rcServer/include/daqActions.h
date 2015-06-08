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
//      Data Acquisition Action and Corresponding String Values
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqActions.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:14  chen
//   run control source
//
//
#ifndef _CODA_DAQ_ACTIONS_H
#define _CODA_DAQ_ACTIONS_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <daqConst.h>

typedef struct _daq_action
{
  char *string_;
  int  action_;
}_daq_action;

class daqActions
{
public:
  // constructor
  daqActions  (void);
  ~daqActions (void);
  
  // return string values associated with action type
  const char *actionString (int action) const;
  const int  action        (char* string) const;
};

extern daqActions* codaDaqActions;
#endif
