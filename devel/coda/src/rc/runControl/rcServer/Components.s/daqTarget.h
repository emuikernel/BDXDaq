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
//      Data Acquisition Target Class. This Class will force all components
//      to go through state transition properly
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqTarget.h,v $
//   Revision 1.4  1998/11/05 20:11:48  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.3  1996/10/31 15:56:10  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.2  1996/10/28 14:23:04  heyes
//   communication with ROCs changed
//
//   Revision 1.1.1.1  1996/10/11 13:39:17  chen
//   run control source
//
//
#ifndef _CODA_DAQ_TARGET_H
#define _CODA_DAQ_TARGET_H

#include <daqConst.h>
#include <target.h>

class daqTarget: public target
{
public:
  // operations
  virtual int configure (void) = 0;
  virtual int download  (void) = 0;
  virtual int go        (void) = 0;
  virtual int pause     (void) = 0;
  virtual int prestart  (void) = 0;
  virtual int end       (void) = 0;
  virtual int verify    (void) = 0;
  virtual int reset     (void) = 0;

  // set state
  void  setState (int newst);
  // get state information
  virtual int   state (void);
  // state2 may return real network call 
  // while state1 may return internal information
  virtual int   state2 (void);
  // get previous state information
  virtual int   previousState (void);
  

  // set override state which signals whether one has to find state
  // remotely instead of using state information locally
  virtual void  setOverrideState (int wanted);
  virtual int   overrideState    (void) const;

  // validate action
  int testAction (int action);
  // validate state
  int testState  (int newst);

  virtual const char* className (void) const {return "daqTarget";}
  // destructor
  virtual ~daqTarget (void);
  
protected:
  // constructor
  daqTarget (char *title);

  // data area
  // current state
  int state_;
  // previous stage state
  int prevState_;
  // usually it is ok to check state information locally. The following
  // is the flag to tell whether one has to go out to find real
  // state information from remote (or not) component. This usually happens
  // when one goes through a state transition
  int prevtime_;
  int overrideState_;
  // target status from transition
  int status_;
};
#endif
