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
//      Data Acquisition Network Component Configuration Class
//      This Class describes the network connection of one components
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: netConfig.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:18  chen
//   run control source
//
//
#ifndef _CODA_NET_CONFIG_H
#define _CODA_NET_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

class netComponent;

//===========================================================================
//                ioComponent associated with one network component
//===========================================================================
class netConfig;

class ioComp
{
public:
  // constructor and destructor
  ioComp  (char* compname, char* portname);
  ~ioComp (void);
  ioComp  (const ioComp& comp);
  ioComp& operator = (const ioComp& comp);

  char* compname (void) const;
  char* portname (void) const;

private:
  char* compname_;
  char* portname_;
  friend class netConfig;
  friend class netComponent;
};

//============================================================================
//          Network Configuration for the netComponent
//============================================================================
class netConfig
{
public:
  // constructors and destructor
  netConfig  (void);
  netConfig  (char* code, char* user_string, char* boot_string);
  ~netConfig (void);
  netConfig  (const netConfig& config);
  netConfig& operator = (const netConfig& config);

  // operations
  void netInputs  (ioComp** comps, int numInputs);
  void netOutputs (ioComp** comps, int numOutputs);
  void infos      (char* code, char* user_string, char* boot_string);
  // cleanup all config information
  void cleanup (void);

private:
  ioComp** inputs_;
  int      numInputs_;
  ioComp** outputs_;
  int      numOutputs_;
  char*    code_;
  char*    userString_;
  char*    bootString_;
  friend class netComponent;
};
#endif
