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
//      Data Acquisition Network Component Configuration Implementation
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: netConfig.cc,v $
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#include "netConfig.h"

//============================================================================
//        Implementation for ioComp Class
//============================================================================
ioComp::ioComp (char* compname, char* portname)
{
#ifdef _TRACE_OBJECTS
  printf ("Create ioComp Class Object\n");
#endif
  compname_ = new char[::strlen (compname) + 1];
  ::strcpy (compname_, compname);

  portname_ = new char[::strlen (portname) + 1];
  ::strcpy (portname_, portname);
}

ioComp::~ioComp (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete ioComp Class Object\n");
#endif
  delete []compname_;
  delete []portname_;
}

ioComp::ioComp (const ioComp& comp)
{
#ifdef _TRACE_OBJECTS
  printf ("Create ioComp Class Object\n");
#endif
  compname_ = new char[::strlen (comp.compname_) + 1];
  ::strcpy (compname_, comp.compname_);

  portname_ = new char[::strlen (comp.portname_) + 1];
  ::strcpy (portname_, comp.portname_);
}

ioComp&
ioComp::operator = (const ioComp& comp)
{
  if (this != &comp) {
    delete []compname_;
    delete []portname_;
    
    compname_ = new char[::strlen (comp.compname_) + 1];
    ::strcpy (compname_, comp.compname_);

    portname_ = new char[::strlen (comp.portname_) + 1];
    ::strcpy (portname_, comp.portname_);
  }
  return *this;
}

char*
ioComp::compname (void) const
{
  return compname_;
}

char*
ioComp::portname (void) const
{
  return portname_;
}

//===========================================================================
//         Implementation of netConfig Class
//===========================================================================
netConfig::netConfig (void)
:inputs_ (0), numInputs_ (0), outputs_ (0), numOutputs_ (0),
 code_ (0), userString_ (0), bootString_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create netConfig Class Object\n");
#endif
  // empty
}

netConfig::netConfig (char* code, char* userString, char* bootString)
:inputs_ (0), numInputs_ (0), outputs_ (0), numOutputs_ (0)
{
#ifdef _TRACE_OBJECTS
  printf ("Create netConfig Class Object\n");
#endif
  if (code) {
    code_ = new char[::strlen (code) + 1];
    ::strcpy (code_, code);
  }
  else
    code_ = 0;
  
  if (userString) {
    userString_ = new char[::strlen (userString) + 1];
    ::strcpy (userString_, userString);
  }
  else
    userString_ = 0;

  bootString_ = new char[::strlen (bootString) + 1];
  ::strcpy (bootString_, bootString);
}

netConfig::~netConfig (void)
{
#ifdef _TRACE_OBJECTS
  printf ("Delete netConfig Class Object\n");
#endif
  cleanup ();
}

netConfig::netConfig (const netConfig& config)
{
#ifdef _TRACE_OBJECTS
  printf ("Create netConfig Class Object\n");
#endif
  int i = 0;
  numInputs_ = config.numInputs_;
  if (numInputs_ != 0) {
    inputs_ = new ioComp* [config.numInputs_];
    for (i = 0; i < numInputs_; i++) 
      inputs_[i] = new ioComp (*(config.inputs_[i]));
  }
  numOutputs_ = config.numOutputs_;
  if (numOutputs_ != 0) {
    outputs_ = new ioComp* [config.numOutputs_];
    for (i = 0; i < numOutputs_; i++) 
      outputs_[i] = new ioComp (*(config.outputs_[i]));
  }
  if (config.code_ != 0) {
    code_ = new char[::strlen (config.code_) + 1];
    ::strcpy (code_, config.code_);
  }
  else
    code_ = 0;
  
  if (config.userString_ != 0) {
    userString_ = new char[::strlen (config.userString_) + 1];
    ::strcpy (userString_, config.userString_);
  }
  else
    userString_ = 0;

  if (config.bootString_ != 0) {
    bootString_ = new char[::strlen (config.bootString_) + 1];
    ::strcpy (bootString_, config.bootString_);
  }
  else
    bootString_ = 0;
}

netConfig&
netConfig::operator = (const netConfig& config)
{
  if (this != &config) {
    cleanup ();

    int i = 0;
    numInputs_ = config.numInputs_;
    if (numInputs_ != 0) {
      inputs_ = new ioComp* [config.numInputs_];
    for (i = 0; i < numInputs_; i++) 
      inputs_[i] = new ioComp (*(config.inputs_[i]));
    }
    numOutputs_ = config.numOutputs_;
    if (numOutputs_ != 0) {
      outputs_ = new ioComp* [config.numOutputs_];
      for (i = 0; i < numOutputs_; i++) 
	outputs_[i] = new ioComp (*(config.outputs_[i]));
    }
    if (config.code_ != 0) {
      code_ = new char[::strlen (config.code_) + 1];
      ::strcpy (code_, config.code_);
    }
    else
      code_ = 0;
  
    if (config.userString_ != 0) {
      userString_ = new char[::strlen (config.userString_) + 1];
      ::strcpy (userString_, config.userString_);
    }
    else
      userString_ = 0;

    if (config.bootString_ != 0) {
      bootString_ = new char[::strlen (config.bootString_) + 1];
      ::strcpy (bootString_, config.bootString_);
    }
    else
      bootString_ = 0;
  }
  return *this;
}

void
netConfig::cleanup (void)
{
  int i = 0;
  if (numInputs_ != 0) {
    for (i = 0; i < numInputs_; i++)
      delete inputs_[i];
    delete []inputs_;
    inputs_ = 0;
    numInputs_ = 0;
  }
  if (numOutputs_ != 0) {
    for (i = 0; i < numOutputs_; i++)
      delete outputs_[i];
    delete []outputs_;
    outputs_ = 0;
    numOutputs_ = 0;
  }
  if (code_ != 0) {
    delete []code_;
    code_ = 0;
  }
  if (userString_ != 0) {
    delete []userString_;
    userString_ = 0;
  }
  if (bootString_ != 0) {
    delete []bootString_;
    bootString_ = 0;
  }
}  

void
netConfig::netInputs (ioComp** comps, int numInputs)
{
  assert (numInputs_ == 0);
  if (numInputs > 0) {
    numInputs_ = numInputs;
    inputs_ = new ioComp* [numInputs_];
    for (int i = 0; i < numInputs_; i++)
      inputs_[i] = new ioComp (*comps[i]);
  }
}

void
netConfig::netOutputs (ioComp** comps, int numOutputs)
{
  assert (numOutputs_ == 0);
  if (numOutputs > 0) {
    numOutputs_ = numOutputs;
    outputs_ = new ioComp* [numOutputs_];
    for (int i = 0; i < numOutputs_; i++)
      outputs_[i] = new ioComp (*comps[i]);
  }
}

void
netConfig::infos (char* code, char* userString, char* bootString)
{
  if (code_ != 0)
    delete []code_;
  if (userString_ != 0)
    delete []userString_;
  if (bootString_ != 0)
    delete []bootString_;

  if (code != 0){
    code_ = new char[::strlen (code) + 1];
    ::strcpy (code_, code);
  }
  else
    code_ = 0;
  
  if (userString != 0) {
    userString_ = new char[::strlen (userString) + 1];
    ::strcpy (userString_, userString);
  }
  else
    userString_ = 0;

  if (bootString != 0) {
    bootString_ = new char[::strlen (bootString) + 1];
    ::strcpy (bootString_, bootString);
  }
  else
    bootString_ = 0;
}

