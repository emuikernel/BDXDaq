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
//      Download State Transition Class. Performs download Transition on the
//      DAQ system
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: downloader.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:21  chen
//   run control source
//
//
#ifndef _CODA_DOWNLOADER_H
#define _CODA_DOWNLOADER_H

#include <transitioner.h>

class downloader: public transitioner
{
public:
  // constructor and destructors
  downloader (daqSystem* system);
  ~downloader (void);

  // redefine transitionTimeout according to number of components
  virtual int transitionTimeout (void);

  virtual const char* className (void) const {return "downloader";}

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
  // setup success state + create dynamic variables if necessary
  virtual void setupSuccess (void);

private:
  // deny access to copy and assignment
  downloader (const downloader&);
  downloader& operator = (const downloader&);
  // timeout cache value
  int timeoutCache_;
};
#endif

  
