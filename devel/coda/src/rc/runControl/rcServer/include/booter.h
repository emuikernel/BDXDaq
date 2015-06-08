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
//      Boot State Transition Class. Performs boot Transition on the
//      DAQ system
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: booter.h,v $
//   Revision 1.3  1996/12/04 18:32:53  chen
//   port to 1.4 on hp and ultrix
//
//   Revision 1.2  1996/10/22 17:17:20  chen
//   fix bugs of boot stage different state among components
//
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#ifndef _CODA_BOOTER_H
#define _CODA_BOOTER_H

#include <transitioner.h>

class booter: public transitioner
{
public:
  // constructor and destructors
  booter (daqSystem* system);
  ~booter (void);

  // redefine transitionTimeout according to number of components
  virtual int transitionTimeout (void);

  virtual const char* className (void) const {return "booter";}

protected:
  // return right action value
  virtual int  action (void);
  // ask subsystem to execute
  virtual void executeItem (daqSubSystem* subsys);
  // return successful state value
  virtual int  successState (void);
  // return failure state value
  virtual int  failureState (void);
  // return transition state value
  virtual int  transitionState (void);
  // check a transition finished or not
  virtual int  transitionFinished (int fstate, int successState);
  // setup success and create dynamic variables
  virtual void setupSuccess (void);
  // check subsystem state
  virtual int  subSystemState (daqSubSystem* subsys);

private:
  // deny access to copy and assignment
  booter (const booter&);
  booter& operator = (const booter&);

  // timeout cache value
  int timeoutCache_;
};
#endif

  
