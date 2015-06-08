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
//      Reset State Transition Class. Performs Reset Transition on the
//      DAQ system
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: resetter.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:22  chen
//   run control source
//
//
#ifndef _CODA_RESETTER_H
#define _CODA_RESETTER_H

#include <transitioner.h>

class resetter: public transitioner
{
public:
  // constructor and destructors
  resetter (daqSystem* system);
  ~resetter (void);

  virtual const char* className (void) const {return "resetter";}

protected:
  // return right action value
  virtual int  action (void);
  // ask subsystem to execute
  virtual void executeItem (daqSubSystem* subsys);
  // extra run parameter setup
  virtual void extraRunParmSetup (void);
  // return successful state value
  virtual int  successState (void);
  // return failure state value
  virtual int  failureState (void);
  // return transition state value
  virtual int  transitionState (void);
  // check a transition finished or not
  virtual int  transitionFinished (int fstate, int successState);

private:
  // deny access to copy and assignment
  resetter (const resetter&);
  resetter& operator = (const resetter&);
};
#endif

  
