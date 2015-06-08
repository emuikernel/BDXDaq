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
//      Go State Transition Class. Performs Go Transition on the
//      DAQ system
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: activater.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#ifndef _CODA_ACTIVATER_H
#define _CODA_ACTIVATER_H

#include <transitioner.h>

class activater: public transitioner
{
public:
  // constructor and destructors
  activater (daqSystem* system);
  ~activater (void);

  virtual const char* className (void) const {return "activater";}

protected:
  // return right action value
  virtual int  action (void);
  // ask subsystem to execute
  virtual void executeItem (daqSubSystem* subsys);
  // extra system wide run set up associated with active command
  virtual void extraRunParmSetup ();
  // return successful state value
  virtual int  successState (void);
  // return failure state value
  virtual int  failureState (void);
  // return transition state value
  virtual int  transitionState (void);
  // set up success 
  virtual void setupSuccess (void);

private:
  // deny access to copy and assignment
  activater (const activater&);
  activater& operator = (const activater&);
};
#endif

  
