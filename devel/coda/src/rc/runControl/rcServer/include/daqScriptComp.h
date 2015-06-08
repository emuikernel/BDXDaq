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
//      CODA user script component class
//           This class will invoke a user provide script at a right
//           transition stage with right transition priority
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: daqScriptComp.h,v $
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#ifndef _CODA_DAQ_SCRIPT_COMP_H
#define _CODA_DAQ_SCRIPT_COMP_H

#ifdef _CODA_USE_THREADS
#include <pthread.h>
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <daqSubSystem.h>
#include <daqComponent.h>

class daqScriptComp: public daqComponent
{
public:
  // constructor and destructor
  // create a script component that is attached to a component
  // comp and is used in the transition action 'action' with
  // script 'script' and priority 'pri'
  daqScriptComp (char* title, int action, int pri, char* script,
		 daqSubSystem& subsys, daqComponent& comp);
  virtual ~daqScriptComp (void);

  // inherited operatiions
  virtual int attach (void);
  virtual int detach (void);

  virtual int  boot      (void);
  virtual int  terminate (void);
  virtual int  status    (void) const;

  virtual int  configure (void);
  virtual int  download  (void);
  virtual int  go        (void);
  virtual int  pause     (void);
  virtual int  prestart  (void);
  virtual int  end       (void);
  virtual int  verify    (void);
  virtual int  reset     (void);

  virtual int  state     (void);

  // redefine cancel transition
  virtual void cancelTransition (void);

  // turn on/off the current transitioner wait for this script to finish flag
  void         askTransitionerToWait  (void);
  void         tellTransitionerToMove (void);

  // return success/failure state
  int          successState (void);

  // return daqSubsystem reference
  daqSubSystem& subSystem (void) const;

  virtual const char* className (void) const {return "daqScriptComp";}

protected:
  // deny access to copy and assignment operations
  daqScriptComp (const daqScriptComp&);
  daqScriptComp& operator = (const daqScriptComp& );
  // blocked function to start a script and wait for it to finish
  static int doScript (daqSystem& system, char* script, daqScriptComp* comp);

  // setup success and failure state
  void      setupStateInfo (int action);

private:
  // data areas

  // subsystem
  daqSubSystem& subsys_;
  // associated real component
  daqComponent& comp_;
  // state at which this script will be invoked
  int action_;
  // success and failure state associated with an action 'action'
  int successState_;
  int failureState_;
  // user script
  char* script_;

#ifdef _CODA_USE_THREADS
  static void*           scriptThread (void *);
  static void            cancelCleaner (void *);
  pthread_t              thr_;
  pthread_mutex_t        lock_;
  int                    thrCreated_;
#endif
};
#endif
