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
//      Data Acquisition Component Class (Abstract)
//      Enforcement of State Transition
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqComponent.h,v $
//   Revision 1.3  1996/11/05 17:37:47  chen
//   bootable flag is added to daqComponent
//
//   Revision 1.2  1996/11/04 16:13:47  chen
//   add options for monitoring components
//
//   Revision 1.1.1.1  1996/10/11 13:39:17  chen
//   run control source
//
//
#ifndef _CODA_DAQ_COMPONENT_H
#define _CODA_DAQ_COMPONENT_H

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "daqTarget.h"

class daqComponent: public daqTarget
{
public:
  // operations

  // abort a transition to a diffrent state
  virtual void abort  (int wanted);
  // register this component to the system
  virtual int  attach (void) = 0;
  // remove this component from the system
  virtual int  detach (void) = 0;
  // enable a component
  virtual void enable (void);
  // disable a component
  virtual void disable (void);
  // enabled status
  int          enabled (void) const;
  // cancel a transition
  virtual void  cancelTransition (void);
  // priority
  virtual void priority (int pri);
  int          priority (void) const;
  // automatic boot flag
  void         enableAutoBoot  (void);
  void         disableAutoBoot (void);
  int          autoBoot        (void) const;

  // bootable flag
  int          bootable        (void) const;

  // monitored by run control
  void         monitorOff      (void);
  void         monitorOn       (void);
  int          monitored       (void) const;

  // comparision 
  // return 0: priority is the same, < 0: this smaller than comp
  int          compare  (const daqComponent& comp) const;

  // class name
  virtual const char* className (void) const {return "daqComponet";}

  // destructor
  virtual ~daqComponent (void);
  
  // overload relational operators
  friend int operator <  (const daqComponent& left, const daqComponent& right);
  friend int operator <= (const daqComponent& left, const daqComponent& right);
  friend int operator >  (const daqComponent& left, const daqComponent& right);
  friend int operator >= (const daqComponent& left, const daqComponent& right);
  friend int operator != (const daqComponent& left, const daqComponent& right);
  friend int operator == (const daqComponent& left, const daqComponent& right);

protected:
  // constructor
  daqComponent (char *title);

  // priority
  int priority_;
  // enabled flag
  int enabled_;
  // automatic booting flag
  int autoBoot_;
  // monitored by run control flag
  int monitored_;
  // check wether one can boot this component or not
  int bootable_;
};

#endif
