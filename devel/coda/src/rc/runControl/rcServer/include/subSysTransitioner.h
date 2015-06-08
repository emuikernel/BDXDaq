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
//      Data Acquisition Sub System Transitioner Class
//      Abstract Class
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: subSysTransitioner.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#ifndef _CODA_SUBSYS_TRANSITIONER_H
#define _CODA_SUBSYS_TRANSITIONER_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <daqCompSortedList.h>
#include <daqSubSystem.h>

class subSysTransitioner
{
public:
  // destructor
  virtual ~subSysTransitioner (void);

  // cancel transition
  virtual void cancel (void);
  // execute a transition
  virtual int  execute (void);
  
  virtual const char* className (void) const {return "subSysTransitioner";}

protected:
  // constructor
  subSysTransitioner (daqSubSystem* subSys);
  
  // subclass provide a particular action
  virtual int  action (void) const = 0;

  // execute an item inside daqSubSystem according to action
  virtual void executeItem (daqComponent *item) = 0;

  // check a component enabled flag
  virtual int  compEnabled (daqComponent *item);

  // subclass return success state
  virtual int successState (void) = 0;
  virtual int failureState (void) = 0;

  // data area
  daqSubSystem* subsystem_;
};
#endif
